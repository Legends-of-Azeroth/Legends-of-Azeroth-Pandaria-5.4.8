/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ConditionMgr.h"
#include "AchievementMgr.h"
#include "GameEventMgr.h"
#include "InstanceScript.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SmartAI.h"
#include "Guild.h"

char const* const ConditionMgr::StaticSourceTypeData[CONDITION_SOURCE_TYPE_MAX] =
{
    "None",
    "Creature Loot",
    "Disenchant Loot",
    "Fishing Loot",
    "GameObject Loot",
    "Item Loot",
    "Mail Loot",
    "Milling Loot",
    "Pickpocketing Loot",
    "Prospecting Loot",
    "Reference Loot",
    "Skinning Loot",
    "Spell Loot",
    "Spell Impl. Target",
    "Gossip Menu",
    "Gossip Menu Option",
    "Creature Vehicle",
    "Spell Expl. Target",
    "Spell Click Event",
    "Quest Accept",
    "Quest Show Mark",
    "Vehicle Spell",
    "SmartScript",
    "Npc Vendor",
    "Spell Proc",
    "Terrain Swap",
    "Phase"
};

ConditionMgr::ConditionTypeInfo const ConditionMgr::StaticConditionTypeData[CONDITION_MAX] =
{
    { "None",                     false, false, false },
    { "Aura",                      true, true,  true  },
    { "Item Stored",               true, true,  true  },
    { "Item Equipped",             true, false, false },
    { "Zone",                      true, false, false },
    { "Reputation",                true, true,  false },
    { "Team",                      true, false, false },
    { "Skill",                     true, true,  false },
    { "Quest Rewarded",            true, false, false },
    { "Quest Taken",               true, false, false },
    { "Drunken",                   true, false, false },
    { "WorldState",                true, true,  false },
    { "Active Event",              true, false, false },
    { "Instance Info",             true, true,  true  },
    { "Quest None",                true, false, false },
    { "Class",                     true, false, false },
    { "Race",                      true, false, false },
    { "Achievement",               true, false, false },
    { "Title",                     true, false, false },
    { "SpawnMask",                 true, false, false },
    { "Gender",                    true, false, false },
    { "Unit State",                true, false, false },
    { "Map",                       true, false, false },
    { "Area",                      true, false, false },
    { "CreatureType",              true, false, false },
    { "Spell Known",               true, false, false },
    { "PhaseMask",                 true, false, false },
    { "Level",                     true, true,  false },
    { "Quest Completed",           true, false, false },
    { "Near Creature",             true, true,  true  },
    { "Near GameObject",           true, true,  false },
    { "Object Entry or Guid",      true, true,  true  },
    { "Object TypeMask",           true, false, false },
    { "Relation",                  true, true,  false },
    { "Reaction",                  true, true,  false },
    { "Distance",                  true, true,  true  },
    { "Alive",                    false, false, false },
    { "Health Value",              true, true,  false },
    { "Health Pct",                true, true,  false },
    { "Realm Achievement",         true, false, false },
    { "In Water",                 false, false, false },
    { "Terrain Swap",             false, false, false },
    { "Sit/stand state",           true, true,  false },
    { "Daily Quest Completed",     true, false, false },
    { "Charmed",                  false, false, false },
    { "Pet type",                  true, false, false },
    { "On Taxi",                  false, false, false },
    { "Quest state mask",          true, true,  false },
    { "Quest objective progress",  true, true,   true },
    { "Map difficulty",            true, false, false },
    { "Is Gamemaster",             true, false, false },
    { "Object Entry or Guid",      true, true,  true  },
    { "Object TypeMask",           true, false, false }
};

// Checks if object meets the condition
// Can have CONDITION_SOURCE_TYPE_NONE && !mReferenceId if called from a special event (ie: SmartAI)
bool Condition::Meets(ConditionSourceInfo& sourceInfo) const
{
    ASSERT(ConditionTarget < MAX_CONDITION_TARGETS);
    WorldObject* object = sourceInfo.mConditionTargets[ConditionTarget];
    // object not present, return false
    if (!object)
    {
        TC_LOG_DEBUG("condition", "Condition object not found for condition (Entry: %u Type: %u Group: %u)", SourceEntry, SourceType, SourceGroup);
        return false;
    }
    bool condMeets = false;
    switch (ConditionType)
    {
        case CONDITION_NONE:
            condMeets = true;                                    // empty condition, always met
            break;
        case CONDITION_AURA:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->HasAuraEffect(ConditionValue1, ConditionValue2);
            break;
        }
        case CONDITION_ITEM:
        {
            if (Player* player = object->ToPlayer())
            {
                // don't allow 0 items (it's checked during table load)
                ASSERT(ConditionValue2);
                bool checkBank = ConditionValue3 ? true : false;
                condMeets = player->HasItemCount(ConditionValue1, ConditionValue2, checkBank);
            }
            break;
        }
        case CONDITION_ITEM_EQUIPPED:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasItemOrGemWithIdEquipped(ConditionValue1, 1);
            break;
        }
        case CONDITION_ZONEID:
            condMeets = object->GetZoneId() == ConditionValue1;
            break;
        case CONDITION_REPUTATION_RANK:
        {
            if (Player* player = object->ToPlayer())
            {
                if (FactionEntry const* faction = sFactionStore.LookupEntry(ConditionValue1))
                    condMeets = (ConditionValue2 & (1 << player->GetReputationMgr().GetRank(faction)));
            }
            break;
        }
        case CONDITION_ACHIEVEMENT:
        {
            AchievementType type = AchievementType(ConditionValue2);
            switch (type)
            {
                case AchievementType::Player:
                    if (Player* player = object->ToPlayer())
                        condMeets = player->GetAchievementMgr().HasAchieved(ConditionValue1);
                    break;
                case AchievementType::Account:
                    if (Player* player = object->ToPlayer())
                        condMeets = player->HasAchieved(ConditionValue1);
                    break;
                case AchievementType::Guild:
                    if (Player* player = object->ToPlayer())
                        if (Guild* guild = player->GetGuild())
                            condMeets = guild->HasAchieved(ConditionValue1);
                    break;
            }
            break;
        }
        case CONDITION_TEAM:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetTeam() == ConditionValue1;
            break;
        }
        case CONDITION_CLASS:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->GetClassMask() & ConditionValue1;
            break;
        }
        case CONDITION_RACE:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->GetRaceMask() & ConditionValue1;
            break;
        }
        case CONDITION_GENDER:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetGender() == ConditionValue1;
            break;
        }
        case CONDITION_SKILL:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasSkill(ConditionValue1) && player->GetBaseSkillValue(ConditionValue1) >= ConditionValue2;
            break;
        }
        case CONDITION_QUESTREWARDED:
        {
            if (Player* player = object->ToPlayer())
            {
                if (!ConditionValue3)
                {
                    // Fucking hacky. Need to use two fractional quests in one phase definition.
                    // One condition (which is not suitable for race) must be failed all the time.
                    if (auto quest = sObjectMgr->GetQuestTemplate(ConditionValue1))
                    {
                        if (!quest->GetAllowableRaces() || (quest->GetAllowableRaces() & player->GetRaceMask()))
                            condMeets = player->GetQuestRewardStatus(ConditionValue1);
                        else
                            condMeets = NegativeCondition;
                    }
                }
                else
                    condMeets = player->GetQuestRewardStatus(ConditionValue1);
            }
            break;
        }
        case CONDITION_QUESTTAKEN:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
                condMeets = (status == QUEST_STATUS_INCOMPLETE);
            }
            break;
        }
        case CONDITION_QUEST_COMPLETE:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
                condMeets = (status == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(ConditionValue1));
            }
            break;
        }
        case CONDITION_QUEST_NONE:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
                condMeets = (status == QUEST_STATUS_NONE);
            }
            break;
        }
        case CONDITION_ACTIVE_EVENT:
            condMeets = sGameEventMgr->IsActiveEvent(ConditionValue1);
            break;
        case CONDITION_INSTANCE_INFO:
        {
            Map* map = object->GetMap();
            if (map && map->IsDungeon())
            {
                if (InstanceScript const* instance = ((InstanceMap*)map)->GetInstanceScript())
                {
                    switch (ConditionValue3)
                    {
                        case INSTANCE_INFO_DATA:
                            condMeets = instance->GetData(ConditionValue1) == ConditionValue2;
                            break;
                        case INSTANCE_INFO_DATA64:
                            condMeets = instance->GetData64(ConditionValue1) == ConditionValue2;
                            break;
                        case INSTANCE_INFO_BOSS_STATE:
                            condMeets = instance->GetBossState(ConditionValue1) == EncounterState(ConditionValue2);
                            break;
                    }
                }
            }
            break;
        }
        case CONDITION_MAPID:
            condMeets = object->GetMapId() == ConditionValue1;
            break;
        case CONDITION_AREAID:
            condMeets = object->GetAreaId() == ConditionValue1;
            break;
        case CONDITION_SPELL:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasSpell(ConditionValue1);
            break;
        }
        case CONDITION_LEVEL:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), static_cast<uint32>(unit->GetLevel()), ConditionValue1);
            break;
        }
        case CONDITION_DRUNKENSTATE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = (uint32)Player::GetDrunkenstateByValue(player->GetDrunkValue()) >= ConditionValue1;
            break;
        }
        case CONDITION_NEAR_CREATURE:
        {
            condMeets = GetClosestCreatureWithEntry(object, ConditionValue1, (float)ConditionValue2, !ConditionValue3) ? true : false;
            break;
        }
        case CONDITION_NEAR_GAMEOBJECT:
        {
            condMeets = GetClosestGameObjectWithEntry(object, ConditionValue1, (float)ConditionValue2) ? true : false;
            break;
        }
        case CONDITION_OBJECT_ENTRY_GUID:
        {
            if (uint32(object->GetTypeId()) == ConditionValue1)
            {
                condMeets = !ConditionValue2 || (object->GetEntry() == ConditionValue2);

                if (ConditionValue3)
                {
                    switch (object->GetTypeId())
                    {
                        case TYPEID_UNIT:
                            condMeets &= object->ToCreature()->GetDBTableGUIDLow() == ConditionValue3;
                            break;
                        case TYPEID_GAMEOBJECT:
                            condMeets &= object->ToGameObject()->GetDBTableGUIDLow() == ConditionValue3;
                            break;
                    }
                }
            }
            break;
        }
        case CONDITION_TYPE_MASK:
        {
            condMeets = object->isType(ConditionValue1);
            break;
        }
        case CONDITION_RELATION_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
            {
                Unit* toUnit = toObject->ToUnit();
                Unit* unit = object->ToUnit();
                if (toUnit && unit)
                {
                    switch (ConditionValue2)
                    {
                        case RELATION_SELF:
                            condMeets = unit == toUnit;
                            break;
                        case RELATION_IN_PARTY:
                            condMeets = unit->IsInPartyWith(toUnit);
                            break;
                        case RELATION_IN_RAID_OR_PARTY:
                            condMeets = unit->IsInRaidWith(toUnit);
                            break;
                        case RELATION_OWNED_BY:
                            condMeets = unit->GetOwnerGUID() == toUnit->GetGUID();
                            break;
                        case RELATION_PASSENGER_OF:
                            condMeets = unit->IsOnVehicle(toUnit);
                            break;
                        case RELATION_CREATED_BY:
                            condMeets = unit->GetCreatorGUID() == toUnit->GetGUID();
                            break;
                    }
                }
            }
            break;
        }
        case CONDITION_REACTION_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
            {
                Unit* toUnit = toObject->ToUnit();
                Unit* unit = object->ToUnit();
                if (toUnit && unit)
                    condMeets = (1 << unit->GetReactionTo(toUnit)) & ConditionValue2;
            }
            break;
        }
        case CONDITION_DISTANCE_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue3), object->GetDistance(toObject), static_cast<float>(ConditionValue2));
            break;
        }
        case CONDITION_ALIVE:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->IsAlive();
            break;
        }
        case CONDITION_HP_VAL:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), unit->GetHealth(), static_cast<uint32>(ConditionValue1));
            break;
        }
        case CONDITION_HP_PCT:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), unit->GetHealthPct(), static_cast<float>(ConditionValue1));
            break;
        }
        case CONDITION_WORLD_STATE:
        {
            condMeets = ConditionValue2 == sWorld->getWorldState(ConditionValue1);
            break;
        }
        case CONDITION_PHASEMASK:
        {
            condMeets = object->GetPhaseMask() & ConditionValue1;
            break;
        }
        case CONDITION_TITLE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasTitle(ConditionValue1);
            break;
        }
        case CONDITION_SPAWNMASK:
        {
            condMeets = ((1 << object->GetMap()->GetSpawnMode()) & ConditionValue1);
            break;
        }
        case CONDITION_UNIT_STATE:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->HasUnitState(ConditionValue1);
            break;
        }
        case CONDITION_CREATURE_TYPE:
        {
            if (Creature* creature = object->ToCreature())
                condMeets = creature->GetCreatureTemplate()->type == ConditionValue1;
            break;
        }
        case CONDITION_REALM_ACHIEVEMENT:
        {
            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(ConditionValue1);
            if (achievement && sAchievementMgr->IsRealmCompleted(achievement))
                condMeets = true;
            break;
        }
        case CONDITION_IN_WATER:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->IsInWater();
            break;
        }
        case CONDITION_STAND_STATE:
        {
            if (Unit* unit = object->ToUnit())
            {
                if (ConditionValue1 == 0)
                    condMeets = (unit->GetStandState() == ConditionValue2);
                else if (ConditionValue2 == 0)
                    condMeets = unit->IsStandState();
                else if (ConditionValue2 == 1)
                    condMeets = unit->IsSitState();
            }
            break;
        }
        case CONDITION_DAILY_QUEST_DONE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->IsDailyQuestDone(ConditionValue1);
            break;
        }
        case CONDITION_CHARMED:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->IsCharmed();
            break;
        }
        case CONDITION_PET_TYPE:
        {
            if (Player* player = object->ToPlayer())
                if (Pet* pet = player->GetPet())
                    condMeets = (((1 << pet->getPetType()) & ConditionValue1) != 0);
            break;
        }
        case CONDITION_TAXI:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->IsInFlight();
            break;
        }
        case CONDITION_QUESTSTATE:
        {
            if (Player* player = object->ToPlayer())
            {
                if (
                    ((ConditionValue2 & (1 << QUEST_STATUS_NONE)) && (player->GetQuestStatus(ConditionValue1) == QUEST_STATUS_NONE)) ||
                    ((ConditionValue2 & (1 << QUEST_STATUS_COMPLETE)) && (player->GetQuestStatus(ConditionValue1) == QUEST_STATUS_COMPLETE)) ||
                    ((ConditionValue2 & (1 << QUEST_STATUS_INCOMPLETE)) && (player->GetQuestStatus(ConditionValue1) == QUEST_STATUS_INCOMPLETE)) ||
                    ((ConditionValue2 & (1 << QUEST_STATUS_FAILED)) && (player->GetQuestStatus(ConditionValue1) == QUEST_STATUS_FAILED)) ||
                    ((ConditionValue2 & (1 << QUEST_STATUS_REWARDED)) && player->GetQuestRewardStatus(ConditionValue1))
                )
                    condMeets = true;
            }
            break;
        }
        case CONDITION_QUEST_OBJECTIVE_COMPLETE:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestObjective const* obj = sObjectMgr->GetQuestObjective(ConditionValue1);
                if (!obj)
                    break;
                Quest const* qInfo = sObjectMgr->GetQuestTemplate(obj->QuestID);
                ASSERT(qInfo);

                condMeets = (!player->GetQuestRewardStatus(obj->QuestID) && player->IsQuestObjectiveComplete(qInfo, *obj));
            }
            break;
        }
        case CONDITION_DIFFICULTY_ID:
        {
            condMeets = object->GetMap()->GetDifficulty() == ConditionValue1;
            break;
        }
        case CONDITION_SAI_PHASE:
        {
            if (Creature* creature = object->ToCreature())
                if (creature->GetAIName() == "SmartAI" && !creature->IsPet())
                    condMeets = CAST_AI(SmartAI, creature->AI())->GetPhase() == ConditionValue1;
            if (GameObject* go = object->ToGameObject())
                if (go->GetAIName() == "SmartAI")
                    condMeets = CAST_AI(SmartAI, go->AI())->GetPhase() == ConditionValue1;
            break;
        }
        case CONDITION_PLAYER_SPEC:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetTalentSpecialization(player->GetActiveSpec()) == ConditionValue1;
            break;
        }
        case CONDITION_CURRENCY_SEASON:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetCurrencyOnSeason(ConditionValue1, true) >= ConditionValue2;
            break;
        }
        case CONDITION_HAS_GROUP:
        {
            if (Player* player = object->ToPlayer())
            {
                Group* group = player->GetGroup();
                condMeets = group ? true : false;
            }
            break;
        }
        case CONDITION_WEEKLY_QUEST_DONE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->IsWeeklyQuestDone(ConditionValue1);
            break;
        }
        case CONDITION_MONTHLY_QUEST_DONE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->IsMonthlyQuestDone(ConditionValue1);
            break;
        }
        case CONDITION_REPUTATION_VALUE:
        {
            if (Player* player = object->ToPlayer())
            {
                if (FactionEntry const* faction = sFactionStore.LookupEntry(ConditionValue1))
                    condMeets = (ConditionValue2 && player->GetReputationMgr().GetReputation(faction) >= int32(ConditionValue2));
            }
            break;
        }
        case CONDITION_PHASEID:
        {
            condMeets = object->IsPhased(ConditionValue1);
            break;
        }
        case CONDITION_TERRAIN_SWAP:
        {
            condMeets = object->IsTerrainSwaped(ConditionValue1);
            break;
        }
        case CONDITION_WORLD_MAP_SWAP:
        {
            condMeets = object->IsWorldMapSwaped(ConditionValue1);
            break;
        }
        default:
            condMeets = false;
            break;
    }

    if (NegativeCondition)
        condMeets = !condMeets;

    if (!condMeets)
        sourceInfo.mLastFailedCondition = this;

    bool script = sScriptMgr->OnConditionCheck(this, sourceInfo); // Returns true by default.
    return condMeets && script;
}

uint32 Condition::GetSearcherTypeMaskForCondition() const
{
    // build mask of types for which condition can return true
    // this is used for speeding up gridsearches
    if (NegativeCondition)
        return (GRID_MAP_TYPE_MASK_ALL);
    uint32 mask = 0;
    switch (ConditionType)
    {
        case CONDITION_NONE:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_AURA:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_ITEM:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_ITEM_EQUIPPED:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_ZONEID:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_REPUTATION_RANK:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_ACHIEVEMENT:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_TEAM:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_CLASS:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_RACE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_SKILL:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUESTREWARDED:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUESTTAKEN:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUEST_COMPLETE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUEST_NONE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_ACTIVE_EVENT:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_INSTANCE_INFO:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_MAPID:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_AREAID:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_SPELL:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_LEVEL:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_DRUNKENSTATE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_NEAR_CREATURE:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_NEAR_GAMEOBJECT:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_OBJECT_ENTRY_GUID:
            switch (ConditionValue1)
            {
                case TYPEID_UNIT:
                    mask |= GRID_MAP_TYPE_MASK_CREATURE;
                    break;
                case TYPEID_PLAYER:
                    mask |= GRID_MAP_TYPE_MASK_PLAYER;
                    break;
                case TYPEID_GAMEOBJECT:
                    mask |= GRID_MAP_TYPE_MASK_GAMEOBJECT;
                    break;
                case TYPEID_CORPSE:
                    mask |= GRID_MAP_TYPE_MASK_CORPSE;
                    break;
                case TYPEID_AREATRIGGER:
                    mask |= GRID_MAP_TYPE_MASK_AREATRIGGER;
                    break;
                default:
                    break;
            }
            break;
        case CONDITION_TYPE_MASK:
            if (ConditionValue1 & TYPEMASK_UNIT)
                mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            if (ConditionValue1 & TYPEMASK_PLAYER)
                mask |= GRID_MAP_TYPE_MASK_PLAYER;
            if (ConditionValue1 & TYPEMASK_GAMEOBJECT)
                mask |= GRID_MAP_TYPE_MASK_GAMEOBJECT;
            if (ConditionValue1 & TYPEMASK_CORPSE)
                mask |= GRID_MAP_TYPE_MASK_CORPSE;
            if (ConditionValue1 & TYPEMASK_AREATRIGGER)
                mask |= GRID_MAP_TYPE_MASK_AREATRIGGER;
            break;
        case CONDITION_RELATION_TO:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_REACTION_TO:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_DISTANCE_TO:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_ALIVE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_HP_VAL:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_HP_PCT:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_WORLD_STATE:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_PHASEMASK:
        case CONDITION_PHASEID:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_TITLE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_SPAWNMASK:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_GENDER:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_UNIT_STATE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_CREATURE_TYPE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE;
            break;
        case CONDITION_REALM_ACHIEVEMENT:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_IN_WATER:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_TERRAIN_SWAP:
        case CONDITION_WORLD_MAP_SWAP:
            mask |= GRID_MAP_TYPE_MASK_ALL;
            break;
        case CONDITION_STAND_STATE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_DAILY_QUEST_DONE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_CHARMED:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_PET_TYPE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_TAXI:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUESTSTATE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_QUEST_OBJECTIVE_COMPLETE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_SAI_PHASE:
            mask |= GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_GAMEOBJECT;
        case CONDITION_PLAYER_SPEC:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_CURRENCY_SEASON:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_HAS_GROUP:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_WEEKLY_QUEST_DONE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_MONTHLY_QUEST_DONE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        case CONDITION_REPUTATION_VALUE:
            mask |= GRID_MAP_TYPE_MASK_PLAYER;
            break;
        default:
            ASSERT(false && "Condition::GetSearcherTypeMaskForCondition - missing condition handling!");
            break;
    }
    return mask;
}

uint32 Condition::GetMaxAvailableConditionTargets() const
{
    // returns number of targets which are available for given source type
    switch (SourceType)
    {
        case CONDITION_SOURCE_TYPE_SPELL:
        case CONDITION_SOURCE_TYPE_SPELL_IMPLICIT_TARGET:
        case CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE:
        case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
        case CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
        case CONDITION_SOURCE_TYPE_SMART_EVENT:
        case CONDITION_SOURCE_TYPE_NPC_VENDOR:
        case CONDITION_SOURCE_TYPE_SPELL_PROC:
            return 2;
        default:
            return 1;
    }
}

std::string Condition::ToString(bool ext /*= false*/) const
{
    std::ostringstream ss;
    ss << "[Condition ";
    ss << "SourceType: " << SourceType;
    if (SourceType < CONDITION_SOURCE_TYPE_MAX)
        ss << " (" << ConditionMgr::StaticSourceTypeData[SourceType] << ")";
    else
        ss << " (Unknown)";
    if (ConditionMgr::CanHaveSourceGroupSet(SourceType))
        ss << ", SourceGroup: " << SourceGroup;
    ss << ", SourceEntry: " << SourceEntry;
    if (ConditionMgr::CanHaveSourceIdSet(SourceType))
        ss << ", SourceId: " << SourceId;

    if (ext)
    {
        ss << ", ConditionType: " << ConditionType;
        if (ConditionType < CONDITION_MAX)
            ss << " (" << ConditionMgr::StaticConditionTypeData[ConditionType].Name << ")";
        else
            ss << " (Unknown)";
    }

    ss << "]";
    return ss.str();
}

ConditionMgr::ConditionMgr() { }

ConditionMgr::~ConditionMgr()
{
    Clean();
}

uint32 ConditionMgr::GetSearcherTypeMaskForConditionList(ConditionContainer const& conditions) const
{
    if (conditions.empty())
        return GRID_MAP_TYPE_MASK_ALL;
    //     groupId, typeMask
    std::map<uint32, uint32> elseGroupSearcherTypeMasks;
    for (ConditionContainer::const_iterator i = conditions.begin(); i != conditions.end(); ++i)
    {
        // no point of having not loaded conditions in list
        ASSERT((*i)->isLoaded() && "ConditionMgr::GetSearcherTypeMaskForConditionList - not yet loaded condition found in list");
        std::map<uint32, uint32>::const_iterator itr = elseGroupSearcherTypeMasks.find((*i)->ElseGroup);
        // group not filled yet, fill with widest mask possible
        if (itr == elseGroupSearcherTypeMasks.end())
            elseGroupSearcherTypeMasks[(*i)->ElseGroup] = GRID_MAP_TYPE_MASK_ALL;
        // no point of checking anymore, empty mask
        else if (!(*itr).second)
            continue;

        if ((*i)->ReferenceId) // handle reference
        {
            ConditionReferenceContainer::const_iterator ref = ConditionReferenceStore.find((*i)->ReferenceId);
            ASSERT(ref != ConditionReferenceStore.end() && "ConditionMgr::GetSearcherTypeMaskForConditionList - incorrect reference");
            elseGroupSearcherTypeMasks[(*i)->ElseGroup] &= GetSearcherTypeMaskForConditionList((*ref).second);
        }
        else // handle normal condition
        {
            // object will match conditions in one ElseGroupStore only when it matches all of them
            // so, let's find a smallest possible mask which satisfies all conditions
            elseGroupSearcherTypeMasks[(*i)->ElseGroup] &= (*i)->GetSearcherTypeMaskForCondition();
        }
    }
    // object will match condition when one of the checks in ElseGroupStore is matching
    // so, let's include all possible masks
    uint32 mask = 0;
    for (std::map<uint32, uint32>::const_iterator i = elseGroupSearcherTypeMasks.begin(); i != elseGroupSearcherTypeMasks.end(); ++i)
        mask |= i->second;

    return mask;
}

bool ConditionMgr::IsObjectMeetToConditionList(ConditionSourceInfo& sourceInfo, ConditionContainer const& conditions) const
{
    //     groupId, groupCheckPassed
    std::map<uint32, bool> elseGroupStore;
    for (Condition const* condition : conditions)
    {
        TC_LOG_DEBUG("condition", "ConditionMgr::IsPlayerMeetToConditionList %s val1: %u", condition->ToString().c_str(), condition->ConditionValue1);
        if (condition->isLoaded())
        {
            //! Find ElseGroup in ElseGroupStore
            std::map<uint32, bool>::const_iterator itr = elseGroupStore.find(condition->ElseGroup);
            //! If not found, add an entry in the store and set to true (placeholder)
            if (itr == elseGroupStore.end())
                elseGroupStore[condition->ElseGroup] = true;
            else if (!(*itr).second)
                continue;

            if (condition->ReferenceId)//handle reference
            {
                ConditionReferenceContainer::const_iterator ref = ConditionReferenceStore.find(condition->ReferenceId);
                if (ref != ConditionReferenceStore.end())
                {
                    if (!IsObjectMeetToConditionList(sourceInfo, ref->second))
                        elseGroupStore[condition->ElseGroup] = false;
                }
                else
                {
                    TC_LOG_DEBUG("condition", "IsPlayerMeetToConditionList: Reference template -%u not found",
                        condition->ToString().c_str(), condition->ReferenceId);//checked at loading, should never happen
                }

            }
            else //handle normal condition
            {
                if (!condition->Meets(sourceInfo))
                    elseGroupStore[condition->ElseGroup] = false;
            }
        }
    }
    for (std::map<uint32, bool>::const_iterator i = elseGroupStore.begin(); i != elseGroupStore.end(); ++i)
        if (i->second)
            return true;

    return false;
}

bool ConditionMgr::IsObjectMeetToConditions(WorldObject* object, ConditionContainer const& conditions) const
{
    ConditionSourceInfo srcInfo = ConditionSourceInfo(object);
    return IsObjectMeetToConditions(srcInfo, conditions);
}

bool ConditionMgr::IsObjectMeetToConditions(WorldObject* object1, WorldObject* object2, ConditionContainer const& conditions) const
{
    ConditionSourceInfo srcInfo = ConditionSourceInfo(object1, object2);
    return IsObjectMeetToConditions(srcInfo, conditions);
}

bool ConditionMgr::IsObjectMeetToConditions(ConditionSourceInfo& sourceInfo, ConditionContainer const& conditions) const
{
    if (conditions.empty())
        return true;

    TC_LOG_DEBUG("condition", "ConditionMgr::IsObjectMeetToConditions");
    return IsObjectMeetToConditionList(sourceInfo, conditions);
}

bool ConditionMgr::CanHaveSourceGroupSet(ConditionSourceType sourceType)
{
    return (sourceType == CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE ||
            sourceType == CONDITION_SOURCE_TYPE_GOSSIP_MENU ||
            sourceType == CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION ||
            sourceType == CONDITION_SOURCE_TYPE_VEHICLE_SPELL ||
            sourceType == CONDITION_SOURCE_TYPE_SPELL_IMPLICIT_TARGET ||
            sourceType == CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT ||
            sourceType == CONDITION_SOURCE_TYPE_SMART_EVENT ||
            sourceType == CONDITION_SOURCE_TYPE_PHASE_DEFINITION ||
            sourceType == CONDITION_SOURCE_TYPE_NPC_VENDOR ||
		    sourceType == CONDITION_SOURCE_TYPE_PHASE);
}

bool ConditionMgr::CanHaveSourceIdSet(ConditionSourceType sourceType)
{
    return (sourceType == CONDITION_SOURCE_TYPE_SMART_EVENT);
}

bool ConditionMgr::IsObjectMeetingNotGroupedConditions(ConditionSourceType sourceType, uint32 entry, ConditionSourceInfo& sourceInfo) const
{
    if (sourceType > CONDITION_SOURCE_TYPE_NONE && sourceType < CONDITION_SOURCE_TYPE_MAX)
    {
        ConditionsByEntryMap::const_iterator i = ConditionStore[sourceType].find(entry);
        if (i != ConditionStore[sourceType].end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForNotGroupedEntry: found conditions for type %u and entry %u", uint32(sourceType), entry);
            return IsObjectMeetToConditions(sourceInfo, i->second);
        }
    }
    return true;
}

bool ConditionMgr::IsObjectMeetingNotGroupedConditions(ConditionSourceType sourceType, uint32 entry, WorldObject* target0, WorldObject* target1 /*= nullptr*/, WorldObject* target2 /*= nullptr*/) const
{
    ConditionSourceInfo conditionSource(target0, target1, target2);
    return IsObjectMeetingNotGroupedConditions(sourceType, entry, conditionSource);
}

bool ConditionMgr::HasConditionsForNotGroupedEntry(ConditionSourceType sourceType, uint32 entry) const
{
    if (sourceType > CONDITION_SOURCE_TYPE_NONE && sourceType < CONDITION_SOURCE_TYPE_MAX)
    {
        if (ConditionStore[sourceType].find(entry) != ConditionStore[sourceType].end())
            return true;
    }
    return false;
}

bool ConditionMgr::IsObjectMeetingSpellClickConditions(uint32 creatureId, uint32 spellId, WorldObject* clicker, WorldObject* target) const
{
    ConditionEntriesByCreatureIdMap::const_iterator itr = SpellClickEventConditionStore.find(creatureId);
    if (itr != SpellClickEventConditionStore.end())
    {
        ConditionsByEntryMap::const_iterator i = itr->second.find(spellId);
        if (i != itr->second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForSpellClickEvent: found conditions for SpellClickEvent entry %u spell %u", creatureId, spellId);
            ConditionSourceInfo sourceInfo(clicker, target);
            return IsObjectMeetToConditions(sourceInfo, i->second);
        }
    }
    return true;
}

ConditionContainer const* ConditionMgr::GetConditionsForSpellClickEvent(uint32 creatureId, uint32 spellId) const
{
    ConditionEntriesByCreatureIdMap::const_iterator itr = SpellClickEventConditionStore.find(creatureId);
    if (itr != SpellClickEventConditionStore.end())
    {
        ConditionsByEntryMap::const_iterator i = itr->second.find(spellId);
        if (i != itr->second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForSpellClickEvent: found conditions for SpellClickEvent entry %u spell %u", creatureId, spellId);
            return &i->second;
        }
    }
    return nullptr;
}

bool ConditionMgr::IsObjectMeetingVehicleSpellConditions(uint32 creatureId, uint32 spellId, Player* player, Unit* vehicle) const
{
    ConditionEntriesByCreatureIdMap::const_iterator itr = VehicleSpellConditionStore.find(creatureId);
    if (itr != VehicleSpellConditionStore.end())
    {
        ConditionsByEntryMap::const_iterator i = itr->second.find(spellId);
        if (i != itr->second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForVehicleSpell: found conditions for Vehicle entry %u spell %u", creatureId, spellId);
            ConditionSourceInfo sourceInfo(player, vehicle);
            return IsObjectMeetToConditions(sourceInfo, i->second);
        }
    }
    return true;
}

bool ConditionMgr::IsObjectMeetingSmartEventConditions(int32 entryOrGuid, uint32 eventId, uint32 sourceType, Unit* unit, WorldObject* baseObject) const
{
    SmartEventConditionContainer::const_iterator itr = SmartEventConditionStore.find(std::make_pair(entryOrGuid, sourceType));
    if (itr != SmartEventConditionStore.end())
    {
        ConditionsByEntryMap::const_iterator i = itr->second.find(eventId + 1);
        if (i != itr->second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForSmartEvent: found conditions for Smart Event entry or guid %d event_id %u", entryOrGuid, eventId);
            ConditionSourceInfo sourceInfo(unit, baseObject);
            return IsObjectMeetToConditions(sourceInfo, i->second);
        }
    }
    return true;
}

ConditionContainer const* ConditionMgr::GetConditionsForPhaseDefinition(uint32 zone, uint32 entry) const
{
    PhaseDefinitionConditionContainer::const_iterator itr = PhaseDefinitionsConditionStore.find(zone);
    if (itr != PhaseDefinitionsConditionStore.end())
    {
        ConditionsByEntryMap::const_iterator i = itr->second.find(entry);
        if (i != itr->second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForPhaseDefinition: found conditions for zone %u entry %u", zone, entry);
            return &i->second;
        }
    }

    return NULL;
}

bool ConditionMgr::IsObjectMeetingVendorItemConditions(uint32 creatureId, uint32 itemId, Player* player, Creature* vendor) const
{
    ConditionContainer cond;
    ConditionEntriesByCreatureIdMap::const_iterator itr = NpcVendorConditionContainerStore.find(creatureId);
    if (itr != NpcVendorConditionContainerStore.end())
    {
        ConditionsByEntryMap::const_iterator i = (*itr).second.find(itemId);
        if (i != (*itr).second.end())
        {
            TC_LOG_DEBUG("condition", "GetConditionsForNpcVendorEvent: found conditions for creature entry %u item %u", creatureId, itemId);
            ConditionSourceInfo sourceInfo(player, vendor);
            return IsObjectMeetToConditions(sourceInfo, i->second);            
        }
    }
    return true;
}

ConditionMgr* ConditionMgr::instance()
{
    static ConditionMgr instance;
    return &instance;
}

void ConditionMgr::LoadConditions(bool isReload)
{
    uint32 oldMSTime = getMSTime();

    Clean();

    //must clear all custom handled cases (groupped types) before reload
    if (isReload)
    {
        TC_LOG_INFO("misc", "Reseting Loot Conditions...");
        LootTemplates_Creature.ResetConditions();
        LootTemplates_Fishing.ResetConditions();
        LootTemplates_Gameobject.ResetConditions();
        LootTemplates_Item.ResetConditions();
        LootTemplates_Mail.ResetConditions();
        LootTemplates_Milling.ResetConditions();
        LootTemplates_Pickpocketing.ResetConditions();
        LootTemplates_Reference.ResetConditions();
        LootTemplates_Skinning.ResetConditions();
        LootTemplates_Disenchant.ResetConditions();
        LootTemplates_Prospecting.ResetConditions();
        LootTemplates_Spell.ResetConditions();

        TC_LOG_INFO("misc", "Re-Loading `gossip_menu` Table for Conditions!");
        sObjectMgr->LoadGossipMenu();

        TC_LOG_INFO("misc", "Re-Loading `gossip_menu_option` Table for Conditions!");
        sObjectMgr->LoadGossipMenuItems();
        sSpellMgr->UnloadSpellInfoImplicitTargetConditionLists();

        TC_LOG_INFO("misc", "Re-Loading `terrain_phase_info` Table for Conditions!");
        sObjectMgr->LoadTerrainPhaseInfo();

        TC_LOG_INFO("misc", "Re-Loading `terrain_swap_defaults` Table for Conditions!");
        sObjectMgr->LoadTerrainSwapDefaults();

        TC_LOG_INFO("misc", "Re-Loading `terrain_worldmap` Table for Conditions!");
        sObjectMgr->LoadTerrainWorldMaps();

        TC_LOG_INFO("misc", "Re-Loading `phase_area` Table for Conditions!");
        sObjectMgr->LoadAreaPhases();
    }

    QueryResult result = WorldDatabase.Query("SELECT SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, "
                                             " ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName FROM conditions");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 conditions. DB table `conditions` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        Condition* cond = new Condition();
        int32 iSourceTypeOrReferenceId  = fields[0].GetInt32();
        cond->SourceGroup               = fields[1].GetUInt32();
        cond->SourceEntry               = fields[2].GetInt32();
        cond->SourceId                  = fields[3].GetInt32();
        cond->ElseGroup                 = fields[4].GetUInt32();
        int32 iConditionTypeOrReference = fields[5].GetInt32();
        cond->ConditionTarget           = fields[6].GetUInt8();
        cond->ConditionValue1           = fields[7].GetUInt32();
        cond->ConditionValue2           = fields[8].GetUInt32();
        cond->ConditionValue3           = fields[9].GetUInt32();
        cond->NegativeCondition         = fields[10].GetUInt8();
        cond->ErrorType                 = fields[11].GetUInt32();
        cond->ErrorTextId               = fields[12].GetUInt32();
        cond->ScriptId                  = sObjectMgr->GetScriptId(fields[13].GetCString());

        if (iConditionTypeOrReference >= 0)
            cond->ConditionType = ConditionTypes(iConditionTypeOrReference);

        if (iSourceTypeOrReferenceId >= 0)
            cond->SourceType = ConditionSourceType(iSourceTypeOrReferenceId);

        if (iConditionTypeOrReference < 0)//it has a reference
        {
            if (iConditionTypeOrReference == iSourceTypeOrReferenceId)//self referencing, skip
            {
                TC_LOG_ERROR("sql.sql", "Condition reference %i is referencing self, skipped", iSourceTypeOrReferenceId);
                delete cond;
                continue;
            }
            cond->ReferenceId = uint32(abs(iConditionTypeOrReference));

            const char* rowType = "reference template";
            if (iSourceTypeOrReferenceId >= 0)
                rowType = "reference";
            //check for useless data
            if (cond->ConditionTarget)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in ConditionTarget (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionTarget);
            if (cond->ConditionValue1)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in value1 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue1);
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in value2 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in value3 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue3);
            if (cond->NegativeCondition)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in NegativeCondition (%u)!", rowType, iSourceTypeOrReferenceId, cond->NegativeCondition);
            if (cond->SourceGroup && iSourceTypeOrReferenceId < 0)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in SourceGroup (%u)!", rowType, iSourceTypeOrReferenceId, cond->SourceGroup);
            if (cond->SourceEntry && iSourceTypeOrReferenceId < 0)
                TC_LOG_ERROR("sql.sql", "Condition %s %i has useless data in SourceEntry (%u)!", rowType, iSourceTypeOrReferenceId, cond->SourceEntry);
        }
        else if (!isConditionTypeValid(cond))//doesn't have reference, validate ConditionType
        {
            delete cond;
            continue;
        }

        if (iSourceTypeOrReferenceId < 0)//it is a reference template
        {
            ConditionReferenceStore[std::abs(iSourceTypeOrReferenceId)].push_back(cond);//add to reference storage
                        ++count;
            continue;
        }//end of reference templates

        //if not a reference and SourceType is invalid, skip
        if (iConditionTypeOrReference >= 0 && !isSourceTypeValid(cond))
        {
            delete cond;
            continue;
        }

        //Grouping is only allowed for some types (loot templates, gossip menus, gossip items)
        if (cond->SourceGroup && !CanHaveSourceGroupSet(cond->SourceType))
        {
            TC_LOG_ERROR("sql.sql", "Condition type %u has not allowed value of SourceGroup = %u!", uint32(cond->SourceType), cond->SourceGroup);
            delete cond;
            continue;
        }
        if (cond->SourceId && !CanHaveSourceIdSet(cond->SourceType))
        {
            TC_LOG_ERROR("sql.sql", "Condition type %u has not allowed value of SourceId = %u!", uint32(cond->SourceType), cond->SourceId);
            delete cond;
            continue;
        }

        if (cond->ErrorType && cond->SourceType != CONDITION_SOURCE_TYPE_SPELL)
        {
            TC_LOG_ERROR("sql.sql", "Condition type %u entry %i can't have ErrorType (%u), set to 0!", uint32(cond->SourceType), cond->SourceEntry, cond->ErrorType);
            cond->ErrorType = 0;
        }

        if (cond->ErrorTextId && !cond->ErrorType)
        {
            TC_LOG_ERROR("sql.sql", "Condition type %u entry %i has any ErrorType, ErrorTextId (%u) is set, set to 0!", uint32(cond->SourceType), cond->SourceEntry, cond->ErrorTextId);
            cond->ErrorTextId = 0;
        }

        if (cond->SourceGroup)
        {
            bool valid = false;
            // handle grouped conditions
            switch (cond->SourceType)
            {
                case CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Creature.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Disenchant.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Fishing.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Gameobject.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Item.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Mail.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Milling.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Pickpocketing.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Prospecting.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Reference.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Skinning.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Spell.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
                    valid = addToGossipMenus(cond);
                    break;
                case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
                    valid = addToGossipMenuItems(cond);
                    break;
                case CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT:
                {
                    SpellClickEventConditionStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid = true;
                    ++count;
                    continue;   // do not add to m_AllocatedMemory to avoid double deleting
                }
                case CONDITION_SOURCE_TYPE_SPELL_IMPLICIT_TARGET:
                    valid = AddToSpellImplicitTargetConditions(cond);
                    break;
                case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
                {
                    VehicleSpellConditionStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid = true;
                    ++count;
                    continue;   // do not add to m_AllocatedMemory to avoid double deleting
                }
                case CONDITION_SOURCE_TYPE_SMART_EVENT:
                {
                    //! TODO: PAIR_32 ?
                    std::pair<int32, uint32> key = std::make_pair(cond->SourceEntry, cond->SourceId);
                    SmartEventConditionStore[key][cond->SourceGroup].push_back(cond);
                    valid = true;
                    ++count;
                    continue;
                }
                case CONDITION_SOURCE_TYPE_NPC_VENDOR:
                {
                    NpcVendorConditionContainerStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid = true;
                    ++count;
                    continue;
                }
                case CONDITION_SOURCE_TYPE_PHASE_DEFINITION:
                {
                    PhaseDefinitionsConditionStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid = true;
                    ++count;
                    continue;
                }
                case CONDITION_SOURCE_TYPE_PHASE:
                {
                    valid = addToPhases(cond);
                    break;
                }
                default:
                    break;
            }

            if (!valid)
            {
                TC_LOG_ERROR("sql.sql", "Not handled grouped condition, SourceGroup %u", cond->SourceGroup);
                delete cond;
            }
            else
            {
                AllocatedMemoryStore.push_back(cond);
                ++count;
            }
            continue;
        }
        //handle not grouped conditions

        //add new Condition to storage based on Type/Entry
        ConditionStore[cond->SourceType][cond->SourceEntry].push_back(cond);
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u conditions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

    if (isReload)
    {
        oldMSTime = getMSTime();

        for (auto&& ai : m_vehicleAIs)
            ai->LoadConditions();

        TC_LOG_INFO("server.loading", ">> Reloaded conditions for %lu vehicles in %u ms", m_vehicleAIs.size(), GetMSTimeDiffToNow(oldMSTime));
    }
}

bool ConditionMgr::addToLootTemplate(Condition* cond, LootTemplate* loot) const
{
    if (!loot)
    {
        TC_LOG_ERROR("sql.sql", "ConditionMgr: LootTemplate %u not found", cond->SourceGroup);
        return false;
    }

    if (loot->addConditionItem(cond))
        return true;

    TC_LOG_ERROR("sql.sql", "ConditionMgr: Item %u not found in LootTemplate %u", cond->SourceEntry, cond->SourceGroup);
    return false;
}

bool ConditionMgr::addToGossipMenus(Condition* cond) const
{
    GossipMenusMapBoundsNonConst pMenuBounds = sObjectMgr->GetGossipMenusMapBoundsNonConst(cond->SourceGroup);

    if (pMenuBounds.first != pMenuBounds.second)
    {
        for (GossipMenusContainer::iterator itr = pMenuBounds.first; itr != pMenuBounds.second; ++itr)
        {
            if ((*itr).second.MenuID == cond->SourceGroup && (*itr).second.TextID == uint32(cond->SourceEntry))
            {
                (*itr).second.Conditions.push_back(cond);
                return true;
            }
        }
    }

    TC_LOG_ERROR("sql.sql", "addToGossipMenus: GossipMenu %u not found", cond->SourceGroup);
    return false;
}

bool ConditionMgr::addToGossipMenuItems(Condition* cond) const
{
    GossipMenuItemsMapBoundsNonConst pMenuItemBounds = sObjectMgr->GetGossipMenuItemsMapBoundsNonConst(cond->SourceGroup);
    if (pMenuItemBounds.first != pMenuItemBounds.second)
    {
        for (GossipMenuItemsContainer::iterator itr = pMenuItemBounds.first; itr != pMenuItemBounds.second; ++itr)
        {
            if ((*itr).second.MenuID == cond->SourceGroup && (*itr).second.OptionID == uint32(cond->SourceEntry))
            {
                (*itr).second.Conditions.push_back(cond);
                return true;
            }
        }
    }

    TC_LOG_ERROR("sql.sql", "addToGossipMenuItems: GossipMenuId %u Item %u not found", cond->SourceGroup, cond->SourceEntry);
    return false;
}

bool ConditionMgr::AddToSpellImplicitTargetConditions(Condition* cond) const
{
    for (uint32 i = REGULAR_DIFFICULTY; i < MAX_DIFFICULTY; ++i)
        if (SpellInfo* spellInfo = sSpellMgr->mSpellInfoMap[i][cond->SourceEntry])
            if (!AddToSpellImplicitTargetConditions(cond, spellInfo))
                return false;
    return true;
}

bool ConditionMgr::AddToSpellImplicitTargetConditions(Condition* cond, SpellInfo* spellInfo) const
{
    uint32 conditionEffMask = cond->SourceGroup;

    std::list<uint32> sharedMasks;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // check if effect is already a part of some shared mask
        bool found = false;
        for (std::list<uint32>::iterator itr = sharedMasks.begin(); itr != sharedMasks.end(); ++itr)
        {
            if ((1<<i) & *itr)
            {
                found = true;
                break;
            }
        }
        if (found)
            continue;

        // build new shared mask with found effect
        uint32 sharedMask = 1<<i;
        ConditionContainer* cmp = spellInfo->Effects[i].ImplicitTargetConditions;
        for (uint8 effIndex = i + 1; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        {
            if (spellInfo->Effects[effIndex].ImplicitTargetConditions == cmp)
                sharedMask |= 1<<effIndex;
        }
        sharedMasks.push_back(sharedMask);
    }

    for (std::list<uint32>::iterator itr = sharedMasks.begin(); itr != sharedMasks.end(); ++itr)
    {
        // some effect indexes should have same data
        if (uint32 commonMask = *itr & conditionEffMask)
        {
            uint8 firstEffIndex = 0;
            for (; firstEffIndex < MAX_SPELL_EFFECTS; ++firstEffIndex)
                if ((1<<firstEffIndex) & *itr)
                    break;

            if (firstEffIndex >= MAX_SPELL_EFFECTS)
                return false;

            // get shared data
            ConditionContainer* sharedList = spellInfo->Effects[firstEffIndex].ImplicitTargetConditions;

            // there's already data entry for that sharedMask
            if (sharedList)
            {
                // we have overlapping masks in db
                if (conditionEffMask != *itr)
                {
                    TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, has incorrect SourceGroup %u (spell effectMask) set - "
                        "effect masks are overlapping (all SourceGroup values having given bit set must be equal) - ignoring.", cond->SourceEntry, cond->SourceGroup);
                    return false;
                }
            }
            // no data for shared mask, we can create new submask
            else
            {
                // add new list, create new shared mask
                sharedList = new ConditionContainer();
                bool assigned = false;
                for (uint8 i = firstEffIndex; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if ((1<<i) & commonMask)
                    {
                        spellInfo->Effects[i].ImplicitTargetConditions = sharedList;
                        assigned = true;
                    }
                }

                if (!assigned)
                    delete sharedList;
            }
            sharedList->push_back(cond);
            break;
        }
    }
    return true;
}

bool ConditionMgr::addToPhases(Condition* cond)
{
    if (!cond->SourceEntry)
    {
        bool found = false;
        PhaseAreaInfo& p = sObjectMgr->GetAreaAndZonePhasesForLoading();
        for (auto phaseItr = p.begin(); phaseItr != p.end(); ++phaseItr)
        {
            for (PhaseInfoStruct& phase : phaseItr->second)
            {
                if (phase.id == cond->SourceGroup)
                {
                    phase.Conditions.push_back(cond);
                    found = true;
                }
            }
        }

        if (found)
            return true;
    }
    else if (std::vector<PhaseInfoStruct>* phases = sObjectMgr->GetPhasesForAreaOrZoneForLoading(cond->SourceEntry))
    {
        for (PhaseInfoStruct& phase : *phases)
        {
            if (phase.id == cond->SourceGroup)
            {
                phase.Conditions.push_back(cond);
                return true;
            }
        }
    }

    TC_LOG_ERROR("sql.sql", "%u phase %u does not have Area %u.", fmt::ptr(&cond), cond->SourceGroup, cond->SourceEntry);
    return false;
}

bool ConditionMgr::isSourceTypeValid(Condition* cond) const
{
    if (cond->SourceType == CONDITION_SOURCE_TYPE_NONE || cond->SourceType >= CONDITION_SOURCE_TYPE_MAX)
    {
        TC_LOG_ERROR("sql.sql", "Invalid ConditionSourceType %u in `condition` table, ignoring.", uint32(cond->SourceType));
        return false;
    }

    switch (cond->SourceType)
    {
        case CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Creature.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `creature_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Creature.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Disenchant.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `disenchant_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Disenchant.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Fishing.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `fishing_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Fishing.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Gameobject.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `gameobject_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Gameobject.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Item.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `item_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Item.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Mail.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `mail_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Mail.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Milling.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `milling_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Milling.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Pickpocketing.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `pickpocketing_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Pickpocketing.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Prospecting.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `prospecting_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Prospecting.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Reference.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `reference_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Reference.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Skinning.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `skinning_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Skinning.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Spell.HaveLootFor(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceGroup %u in `condition` table, does not exist in `spell_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Spell.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL_IMPLICIT_TARGET:
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(cond->SourceEntry);
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }

            if ((cond->SourceGroup > MAX_EFFECT_MASK) || !cond->SourceGroup)
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, has incorrect SourceGroup %u (spell effectMask) set, ignoring.", cond->SourceEntry, cond->SourceGroup);
                return false;
            }

            uint32 origGroup = cond->SourceGroup;

            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (!((1<<i) & cond->SourceGroup))
                    continue;

                if (spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA_ENTRY)
                    continue;

                switch (spellInfo->Effects[i].TargetA.GetSelectionCategory())
                {
                    case TARGET_SELECT_CATEGORY_NEARBY:
                    case TARGET_SELECT_CATEGORY_CONE:
                    case TARGET_SELECT_CATEGORY_AREA:
                        continue;
                    default:
                        break;
                }

                switch (spellInfo->Effects[i].TargetB.GetSelectionCategory())
                {
                    case TARGET_SELECT_CATEGORY_NEARBY:
                    case TARGET_SELECT_CATEGORY_CONE:
                    case TARGET_SELECT_CATEGORY_AREA:
                        continue;
                    default:
                        break;
                }

                TC_LOG_ERROR("sql.sql", "SourceEntry %u SourceGroup %u in `condition` table - spell %u does not have implicit targets of types: _AREA_, _CONE_, _NEARBY_ for effect %u, SourceGroup needs correction, ignoring.", cond->SourceEntry, origGroup, cond->SourceEntry, uint32(i));
                cond->SourceGroup &= ~(1<<i);
            }
            // all effects were removed, no need to add the condition at all
            if (!cond->SourceGroup)
                return false;
            break;
        }
        case CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL:
        case CONDITION_SOURCE_TYPE_SPELL_PROC:
        {
            SpellInfo const* spellProto = sSpellMgr->GetSpellInfo(cond->SourceEntry);
            if (!spellProto)
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_QUEST_ACCEPT:
            if (!sObjectMgr->GetQuestTemplate(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "CONDITION_SOURCE_TYPE_QUEST_ACCEPT specifies non-existing quest (%u), skipped", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            if (!sSpellMgr->GetSpellInfo(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT:
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            if (!sSpellMgr->GetSpellInfo(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_PHASE_DEFINITION:
            if (!PhaseMgr::IsConditionTypeSupported(cond->ConditionType))
            {
                TC_LOG_ERROR("sql.sql", "Condition source type `CONDITION_SOURCE_TYPE_PHASE_DEFINITION` does not support condition type %u, ignoring.", cond->ConditionType);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_PHASE:
        {
            if (cond->SourceEntry && !sAreaTableStore.LookupEntry(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "%u SourceEntry in `condition` table, does not exist in AreaTable.dbc, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_TERRAIN_SWAP:
        {
            if (!sMapStore.LookupEntry(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "%u SourceEntry in `condition` table, does not exist in Map.dbc, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_NPC_VENDOR:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceGroup))
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceGroup);
                return false;
            }
            ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!itemTemplate)
            {
                TC_LOG_ERROR("sql.sql", "SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_AREATRIGGER_CLIENT_TRIGGERED:
        {
            if (!sAreaTriggerStore.LookupEntry(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "%s SourceEntry in `condition` table, does not exists in AreaTrigger.dbc, ignoring.", cond->ToString().c_str());
                return false;
            }
            break;
        }        
        case CONDITION_SOURCE_TYPE_GRAVEYARD:
            if (!sWorldSafeLocsStore.LookupEntry(cond->SourceEntry))
            {
                TC_LOG_ERROR("sql.sql", "%u SourceEntry in `condition` table, does not exist in WorldSafeLocs.db2, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
        case CONDITION_SOURCE_TYPE_SMART_EVENT:
        case CONDITION_SOURCE_TYPE_NONE:
        default:
            break;
    }

    return true;
}

bool ConditionMgr::isConditionTypeValid(Condition* cond) const
{
    if (cond->ConditionType == CONDITION_NONE || cond->ConditionType >= CONDITION_MAX)
    {
        TC_LOG_ERROR("sql.sql", "Invalid ConditionType %u at SourceEntry %u in `condition` table, ignoring.", uint32(cond->ConditionType), cond->SourceEntry);
        return false;
    }

    if (cond->ConditionTarget >= cond->GetMaxAvailableConditionTargets())
    {
        TC_LOG_ERROR("sql.sql", "SourceType %u, SourceEntry %u, SourceGroup %u in `condition` table, has incorrect ConditionTarget set, ignoring.", cond->SourceType, cond->SourceEntry, cond->SourceGroup);
        return false;
    }

    switch (cond->ConditionType)
    {
        case CONDITION_AURA:
        {
            if (!sSpellMgr->GetSpellInfo(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "Aura condition has non existing spell (Id: %d), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 > EFFECT_2)
            {
                TC_LOG_ERROR("sql.sql", "Aura condition has non existing effect index (%u) (must be 0..2), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Aura condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ITEM:
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(cond->ConditionValue1);
            if (!proto)
            {
                TC_LOG_ERROR("sql.sql", "Item condition has non existing item (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (!cond->ConditionValue2)
            {
                TC_LOG_ERROR("sql.sql", "Item condition has 0 set for item count in value2 (%u), skipped", cond->ConditionValue2);
                return false;
            }
            break;
        }
        case CONDITION_ITEM_EQUIPPED:
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(cond->ConditionValue1);
            if (!proto)
            {
                TC_LOG_ERROR("sql.sql", "ItemEquipped condition has non existing item (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "ItemEquipped condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "ItemEquipped condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ZONEID:
        {
            AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(cond->ConditionValue1);
            if (!areaEntry)
            {
                TC_LOG_ERROR("sql.sql", "ZoneID condition has non existing area (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (areaEntry->ParentAreaID != 0)
            {
                TC_LOG_ERROR("sql.sql", "ZoneID condition requires to be in area (%u) which is a subzone but zone expected, skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "ZoneID condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "ZoneID condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_REPUTATION_RANK:
        case CONDITION_REPUTATION_VALUE:
        {
            FactionEntry const* factionEntry = sFactionStore.LookupEntry(cond->ConditionValue1);
            if (!factionEntry)
            {
                TC_LOG_ERROR("sql.sql", "Reputation condition has non existing faction (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Reputation condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_TEAM:
        {
            if (cond->ConditionValue1 != ALLIANCE && cond->ConditionValue1 != HORDE)
            {
                TC_LOG_ERROR("sql.sql", "Team condition specifies unknown team (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Team condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Team condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_SKILL:
        {
            SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(cond->ConditionValue1);
            if (!pSkill)
            {
                TC_LOG_ERROR("sql.sql", "Skill condition specifies non-existing skill (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 < 1 || cond->ConditionValue2 > sWorld->GetConfigMaxSkillValue())
            {
                TC_LOG_ERROR("sql.sql", "Skill condition specifies skill (%u) with invalid value (%u), skipped", cond->ConditionValue1, cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Skill condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_QUESTSTATE:
            if (cond->ConditionValue2 >= (1 << MAX_QUEST_STATUS))
            {
                TC_LOG_ERROR("sql.sql", "QuestState condition has invalid state mask (%u), skipped.", cond->ConditionValue2);
                return false;
            }
            // intentional missing break
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_NONE:
        case CONDITION_QUEST_COMPLETE:
        case CONDITION_DAILY_QUEST_DONE:
        case CONDITION_WEEKLY_QUEST_DONE:
        case CONDITION_MONTHLY_QUEST_DONE:
        {
            if (!sObjectMgr->GetQuestTemplate(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "Quest condition (Type: %u) points to non-existing quest (%u) for Source Entry %u. SourceGroup: %u, SourceTypeOrReferenceId: %u",
                    cond->ConditionType, cond->ConditionValue1, cond->SourceEntry, cond->SourceGroup, cond->SourceType);
                return false;
            }

            if (cond->ConditionValue2 > 1)
                TC_LOG_ERROR("sql.sql", "Quest condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Quest condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_DRUNKENSTATE:
        {
            if (cond->ConditionValue1 > DRUNKEN_SMASHED)
            {
                TC_LOG_ERROR("sql.sql", "DrunkState condition has invalid state (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2)
            {
                TC_LOG_ERROR("sql.sql", "DrunkState condition has useless data in value2 (%u)!", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "DrunkState condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_WORLD_STATE:
        {
            if (!sWorld->getWorldState(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "World state condition has non existing world state in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "World state condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ACTIVE_EVENT:
        {
            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (cond->ConditionValue1 >=events.size() || !events[cond->ConditionValue1].isValid())
            {
                TC_LOG_ERROR("sql.sql", "ActiveEvent condition has non existing event id (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "ActiveEvent condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "ActiveEvent condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_INSTANCE_INFO:
            break;
        case CONDITION_CLASS:
        {
            if (!(cond->ConditionValue1 & CLASSMASK_ALL_PLAYABLE))
            {
                TC_LOG_ERROR("sql.sql", "Class condition has non existing classmask (%u), skipped", cond->ConditionValue1 & ~CLASSMASK_ALL_PLAYABLE);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Class condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Class condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_RACE:
        {
            if (!(cond->ConditionValue1 & RACEMASK_ALL_PLAYABLE))
            {
                TC_LOG_ERROR("sql.sql", "Race condition has non existing racemask (%u), skipped", cond->ConditionValue1 & ~RACEMASK_ALL_PLAYABLE);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Race condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Race condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ACHIEVEMENT:
        {
            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(cond->ConditionValue1);
            if (!achievement)
            {
                TC_LOG_ERROR("sql.sql", "Achivement condition has non existing achivement id (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 > uint32(AchievementType::Guild))
                TC_LOG_ERROR("sql.sql", "Achivement condition has invaild achievement type in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Achivement condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_TITLE:
        {
            CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(cond->ConditionValue1);
            if (!titleEntry)
            {
                TC_LOG_ERROR("sql.sql", "Title condition has non existing title in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_SPAWNMASK:
        {
            if (cond->ConditionValue1 > SPAWNMASK_RAID_ALL)
            {
                TC_LOG_ERROR("sql.sql", "SpawnMask condition has non existing SpawnMask in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_GENDER:
        {
            if (!Player::IsValidGender(uint8(cond->ConditionValue1)))
            {
                TC_LOG_ERROR("sql.sql", "Gender condition has invalid gender (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Gender condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Gender condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_UNIT_STATE:
        {
            if (cond->ConditionValue1 > uint32(UNIT_STATE_ALL_STATE))
            {
                TC_LOG_ERROR("sql.sql", "UnitState condition has non existing UnitState in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_MAPID:
        {
            MapEntry const* me = sMapStore.LookupEntry(cond->ConditionValue1);
            if (!me)
            {
                TC_LOG_ERROR("sql.sql", "Map condition has non existing map (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Map condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Map condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_AREAID:
            break;
        case CONDITION_CREATURE_TYPE:
        {
            if (!cond->ConditionValue1 || cond->ConditionValue1 > CREATURE_TYPE_GAS_CLOUD)
            {
                TC_LOG_ERROR("sql.sql", "CreatureType condition has non existing CreatureType in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_SPELL:
        {
            if (!sSpellMgr->GetSpellInfo(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "Spell condition has non existing spell (Id: %d), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Spell condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Spell condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_PHASEMASK:
        {
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Phasemask condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Phasemask condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_LEVEL:
        {
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "Level condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Level condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_NEAR_CREATURE:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "NearCreature condition has non existing creature template entry (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "NearCreature condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_NEAR_GAMEOBJECT:
        {
            if (!sObjectMgr->GetGameObjectTemplate(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "NearGameObject condition has non existing gameobject template entry (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "NearGameObject condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_OBJECT_ENTRY_GUID:
        {
            switch (cond->ConditionValue1)
            {
                case TYPEID_UNIT:
                    if (cond->ConditionValue2 && !sObjectMgr->GetCreatureTemplate(cond->ConditionValue2))
                    {
                        TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has non existing creature template entry (%u), skipped", cond->ConditionValue2);
                        return false;
                    }
                    if (cond->ConditionValue3)
                    {
                        if (CreatureData const* creatureData = sObjectMgr->GetCreatureData(cond->ConditionValue3))
                        {
                            if (cond->ConditionValue2 && creatureData->id != cond->ConditionValue2)
                            {
                                TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has guid %u set but does not match creature entry (%u), skipped", cond->ConditionValue3, cond->ConditionValue2);
                                return false;
                            }
                        }
                        else
                        {
                            TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has non existing creature guid (%u), skipped", cond->ConditionValue3);
                            return false;
                        }
                    }
                    break;
                case TYPEID_GAMEOBJECT:
                    if (cond->ConditionValue2 && !sObjectMgr->GetGameObjectTemplate(cond->ConditionValue2))
                    {
                        TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has non existing gameobject template entry (%u), skipped", cond->ConditionValue2);
                        return false;
                    }
                    if (cond->ConditionValue3)
                    {
                        if (GameObjectData const* goData = sObjectMgr->GetGOData(cond->ConditionValue3))
                        {
                            if (cond->ConditionValue2 && goData->id != cond->ConditionValue2)
                            {
                                TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has guid %u set but does not match gameobject entry (%u), skipped", cond->ConditionValue3, cond->ConditionValue2);
                                return false;
                            }
                        }
                        else
                        {
                            TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has non existing gameobject guid (%u), skipped", cond->ConditionValue3);
                            return false;
                        }
                    }
                    break;
                case TYPEID_PLAYER:
                case TYPEID_CORPSE:
                    if (cond->ConditionValue2)
                        TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has useless data in value2 (%u)!", cond->ConditionValue2);
                    if (cond->ConditionValue3)
                        TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has useless data in value3 (%u)!", cond->ConditionValue3);
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "ObjectEntryGuid condition has wrong typeid set (%u), skipped", cond->ConditionValue1);
                    return false;
            }
            break;
        }
        case CONDITION_TYPE_MASK:
        {
            if (!cond->ConditionValue1 || (cond->ConditionValue1 & ~(TYPEMASK_UNIT | TYPEMASK_PLAYER | TYPEMASK_GAMEOBJECT | TYPEMASK_CORPSE)))
            {
                TC_LOG_ERROR("sql.sql", "TypeMask condition has invalid typemask set (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "TypeMask condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "TypeMask condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_RELATION_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                TC_LOG_ERROR("sql.sql", "RelationTo condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                TC_LOG_ERROR("sql.sql", "RelationTo condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2 >= RELATION_MAX)
            {
                TC_LOG_ERROR("sql.sql", "RelationTo condition has invalid ConditionValue2(RelationType) (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "RelationTo condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_REACTION_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                TC_LOG_ERROR("sql.sql", "ReactionTo condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                TC_LOG_ERROR("sql.sql", "ReactionTo condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (!cond->ConditionValue2)
            {
                TC_LOG_ERROR("sql.sql", "mConditionValue2 condition has invalid ConditionValue2(rankMask) (%u), skipped", cond->ConditionValue2);
                return false;
            }
            break;
        }
        case CONDITION_DISTANCE_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                TC_LOG_ERROR("sql.sql", "DistanceTo condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                TC_LOG_ERROR("sql.sql", "DistanceTo condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3 >= COMP_TYPE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "DistanceTo condition has invalid ComparisionType (%u), skipped", cond->ConditionValue3);
                return false;
            }
            break;
        }
        case CONDITION_ALIVE:
        {
            if (cond->ConditionValue1)
                TC_LOG_ERROR("sql.sql", "Alive condition has useless data in value1 (%u)!", cond->ConditionValue1);
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Alive condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Alive condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_HP_VAL:
        {
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "HpVal condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "HpVal condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_HP_PCT:
        {
            if (cond->ConditionValue1 > 100)
            {
                TC_LOG_ERROR("sql.sql", "HpPct condition has too big percent value (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "HpPct condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "HpPct condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_REALM_ACHIEVEMENT:
        {
            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(cond->ConditionValue1);
            if (!achievement)
            {
                TC_LOG_ERROR("sql.sql", "RealmAchievement condition has non existing realm first achivement id (%u), skipped.", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_IN_WATER:
            break;
        case CONDITION_STAND_STATE:
        {
            bool valid = false;
            switch (cond->ConditionValue1)
            {
                case 0:
                    valid = cond->ConditionValue2 <= UNIT_STAND_STATE_SUBMERGED;
                    break;
                case 1:
                    valid = cond->ConditionValue2 <= 1;
                    break;
                default:
                    valid = false;
                    break;
            }
            if (!valid)
            {
                TC_LOG_ERROR("sql.sql", "StandState condition has non-existing stand state (%u,%u), skipped.", cond->ConditionValue1, cond->ConditionValue2);
                return false;
            }
            break;
        }
        case CONDITION_CHARMED:
            break;
        case CONDITION_PET_TYPE:
            if (cond->ConditionValue1 >= (1 << MAX_PET_TYPE))
            {
                TC_LOG_ERROR("sql.sql", "PetType condition has non-existing pet type %u, skipped.", cond->ConditionValue1);
                return false;
            }
            break;
        case CONDITION_TAXI:
            break;
        case CONDITION_QUEST_OBJECTIVE_COMPLETE:
        {
            QuestObjective const* obj = sObjectMgr->GetQuestObjective(cond->ConditionValue1);
            if (!obj)
            {
                TC_LOG_ERROR("sql.sql", "QuestObjectiveComplete condition points to non-existing quest objective (%u), skipped.", cond->ConditionValue1);
                return false;
            }
            break;
        }
        case CONDITION_PHASEID:
        {
            if (!sPhaseStore.LookupEntry(cond->ConditionValue1))
            {
                TC_LOG_ERROR("sql.sql", "Phase condition has nonexistent phaseid in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Phase condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Phase condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_TERRAIN_SWAP:
        {
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Terrain swap condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Terrain swap condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_WORLD_MAP_SWAP:
        {
            if (cond->ConditionValue2)
                TC_LOG_ERROR("sql.sql", "Terrain swap condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                TC_LOG_ERROR("sql.sql", "Terrain swap condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_SAI_PHASE:
        case CONDITION_PLAYER_SPEC:
        case CONDITION_CURRENCY_SEASON:
        case CONDITION_HAS_GROUP:
            break;
        default:
            break;
    }
    return true;
}

void ConditionMgr::Clean()
{
    for (ConditionReferenceContainer::iterator itr = ConditionReferenceStore.begin(); itr != ConditionReferenceStore.end(); ++itr)
        for (ConditionContainer::const_iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            delete *it;

    ConditionReferenceStore.clear();

    for (uint32 i = 0; i < CONDITION_SOURCE_TYPE_MAX; ++i)
    {
        for (ConditionsByEntryMap::iterator it = ConditionStore[i].begin(); it != ConditionStore[i].end(); ++it)
            for (ConditionContainer::const_iterator itr = it->second.begin(); itr != it->second.end(); ++itr)
                delete *itr;

        ConditionStore[i].clear();
    }

    for (ConditionEntriesByCreatureIdMap::iterator itr = VehicleSpellConditionStore.begin(); itr != VehicleSpellConditionStore.end(); ++itr)
        for (ConditionsByEntryMap::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            for (ConditionContainer::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;

    VehicleSpellConditionStore.clear();

    for (SmartEventConditionContainer::iterator itr = SmartEventConditionStore.begin(); itr != SmartEventConditionStore.end(); ++itr)
        for (ConditionsByEntryMap::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            for (ConditionContainer::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;

    SmartEventConditionStore.clear();

    for (ConditionEntriesByCreatureIdMap::iterator itr = SpellClickEventConditionStore.begin(); itr != SpellClickEventConditionStore.end(); ++itr)
        for (ConditionsByEntryMap::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            for (ConditionContainer::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;

    SpellClickEventConditionStore.clear();

    for (PhaseDefinitionConditionContainer::iterator itr = PhaseDefinitionsConditionStore.begin(); itr != PhaseDefinitionsConditionStore.end(); ++itr)
        for (ConditionsByEntryMap::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            for (ConditionContainer::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;

    PhaseDefinitionsConditionStore.clear();

    for (ConditionEntriesByCreatureIdMap::iterator itr = NpcVendorConditionContainerStore.begin(); itr != NpcVendorConditionContainerStore.end(); ++itr)
        for (ConditionsByEntryMap::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            for (ConditionContainer::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;

    NpcVendorConditionContainerStore.clear();

    // this is a BIG hack, feel free to fix it if you can figure out the ConditionMgr ;)
    for (std::vector<Condition*>::const_iterator itr = AllocatedMemoryStore.begin(); itr != AllocatedMemoryStore.end(); ++itr)
        delete *itr;

    AllocatedMemoryStore.clear();
}
