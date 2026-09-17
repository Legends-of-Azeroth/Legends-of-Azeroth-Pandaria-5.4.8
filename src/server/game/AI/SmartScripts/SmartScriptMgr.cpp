/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectDefines.h"
#include "GridDefines.h"
#include "GridNotifiers.h"
#include "SpellMgr.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "GameEventMgr.h"
#include "CreatureTextMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"

#include "SmartScriptMgr.h"

#define TC_SAI_IS_BOOLEAN_VALID(e, value) \
{ \
    if (value > 1) \
    { \
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses param {} of type Boolean with value {}, valid values are 0 or 1, skipped.", \
            e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), STRINGIZE(value), value); \
        return false; \
    } \
}

SmartWaypointMgr* SmartWaypointMgr::instance()
{
    static SmartWaypointMgr instance;
    return &instance;
}

void SmartWaypointMgr::LoadFromDB()
{
    uint32 oldMSTime = getMSTime();

    for (std::unordered_map<uint32, WPPath*>::iterator itr = waypoint_map.begin(); itr != waypoint_map.end(); ++itr)
    {
        for (WPPath::iterator pathItr = itr->second->begin(); pathItr != itr->second->end(); ++pathItr)
            delete pathItr->second;

        delete itr->second;
    }

    waypoint_map.clear();

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_SMARTAI_WP);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 SmartAI Waypoint Paths. DB table `waypoints` is empty.");

        return;
    }

    uint32 count = 0;
    uint32 total = 0;
    uint32 last_entry = 0;
    uint32 last_id = 1;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        uint32 id = fields[1].GetUInt32();
        float x, y, z;
        x = fields[2].GetFloat();
        y = fields[3].GetFloat();
        z = fields[4].GetFloat();
        Optional<float> o;
        if (!fields[5].IsNull())
            o = fields[5].GetFloat();
        uint32 delay = fields[6].GetUInt32();

        if (last_entry != entry)
        {
            waypoint_map[entry] = new WPPath();
            last_id = 1;
            count++;
        }

        if (last_id != id)
            TC_LOG_ERROR("sql.sql", "SmartWaypointMgr::LoadFromDB: Path entry {}, unexpected point id {}, expected {}.", entry, id, last_id);

        last_id++;
        (*waypoint_map[entry])[id] = new WayPoint(id, x, y, z, o, delay);

        last_entry = entry;
        total++;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} SmartAI waypoint paths (total {} waypoints) in {} ms", count, total, GetMSTimeDiffToNow(oldMSTime));

}

WPPath* SmartWaypointMgr::GetPath(uint32 id)
{
    if (waypoint_map.find(id) != waypoint_map.end())
        return waypoint_map[id];
    else return 0;
}

// SmartWaypointMgr::~SmartWaypointMgr()
// {
//     for (std::unordered_map<uint32, WPPath*>::iterator itr = waypoint_map.begin(); itr != waypoint_map.end(); ++itr)
//     {
//         for (WPPath::iterator pathItr = itr->second->begin(); pathItr != itr->second->end(); ++pathItr)
//             delete pathItr->second;

//         delete itr->second;
//     }
// }

SmartAIMgr* SmartAIMgr::instance()
{
    static SmartAIMgr instance;
    return &instance;
}

void SmartAIMgr::LoadSmartAIFromDB()
{
    uint32 oldMSTime = getMSTime();

    for (uint8 i = 0; i < SMART_SCRIPT_TYPE_MAX; i++)
        mEventMap[i].clear();  //Drop Existing SmartAI List

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_SMART_SCRIPTS);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 SmartAI scripts. DB table `smartai_scripts` is empty.");

        return;
    }

    _creatureSpellSummonCache.clear();
    _gameObjectSpellSummonCache.clear();
    _killCreditsSpellCache.clear();

    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i);
        if (!spellInfo)
            continue;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            switch (spellInfo->Effects[j].Effect)
            {
                case SPELL_EFFECT_SUMMON:
                    _creatureSpellSummonCache.insert({ uint32(spellInfo->Effects[j].MiscValue), { uint32(spellInfo->Id), j } });
                    break;
                case SPELL_EFFECT_KILL_CREDIT:
                case SPELL_EFFECT_KILL_CREDIT2:
                    _killCreditsSpellCache.insert({ uint32(spellInfo->Effects[j].MiscValue), { uint32(spellInfo->Id), j } });
                    break;
                case SPELL_EFFECT_SUMMON_OBJECT_WILD:
                    _gameObjectSpellSummonCache.insert({ uint32(spellInfo->Effects[j].MiscValue), { uint32(spellInfo->Id), j } });
                    break;
                default:
                    break;
            }
        }
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        SmartScriptHolder temp;

        temp.entryOrGuid = fields[0].GetInt32();
        SmartScriptType source_type = (SmartScriptType)fields[1].GetUInt8();
        if (source_type >= SMART_SCRIPT_TYPE_MAX)
        {
            TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: invalid source_type ({}), skipped loading.", uint32(source_type));
            continue;
        }
        if (temp.entryOrGuid >= 0)
        {
            switch (source_type)
            {
                case SMART_SCRIPT_TYPE_CREATURE:
                {
                    CreatureTemplate const* t = sObjectMgr->GetCreatureTemplate((uint32)temp.entryOrGuid);
                    if (!t)
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: Creature entry ({}) does not exist, skipped loading.", uint32(temp.entryOrGuid));
                        continue;
                    }
                    if (t->AIName != "SmartAI" && t->AIName != "SmartVehicleAI")
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: Creature entry {} has SmartAI scripts, but its AIName is not 'SmartAI'", uint32(temp.entryOrGuid));
                    break;
                }
                case SMART_SCRIPT_TYPE_GAMEOBJECT:
                {
                    GameObjectTemplate const* t = sObjectMgr->GetGameObjectTemplate((uint32)temp.entryOrGuid);
                    if (!t)
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: GameObject entry ({}) does not exist, skipped loading.", uint32(temp.entryOrGuid));
                        continue;
                    }
                    if (t->AIName != "SmartGameObjectAI")
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: GameObject entry {} has SmartAI scripts, but its AIName is not 'SmartGameObjectAI'", uint32(temp.entryOrGuid));
                    break;
                }
                case SMART_SCRIPT_TYPE_AREATRIGGER:
                {
                    if (!sAreaTriggerStore.LookupEntry((uint32)temp.entryOrGuid))
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: AreaTrigger entry ({}) does not exist, skipped loading.", uint32(temp.entryOrGuid));
                        continue;
                    }
                    break;
                }
                case SMART_SCRIPT_TYPE_TIMED_ACTIONLIST:
                    break;//nothing to check, really
                default:
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: not yet implemented source_type {}", (uint32)source_type);
                    continue;
            }
        }
        else
        {
            if (!sObjectMgr->GetCreatureData(uint32(abs(temp.entryOrGuid))))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr::LoadSmartAIFromDB: Creature guid ({}) does not exist, skipped loading.", uint32(abs(temp.entryOrGuid)));
                continue;
            }
        }

        temp.source_type = source_type;
        temp.event_id = fields[2].GetUInt16();
        temp.link = fields[3].GetUInt16();
        temp.event.type = (SMART_EVENT)fields[4].GetUInt8();
        temp.event.event_phase_mask = fields[5].GetUInt8();
        temp.event.event_chance = fields[6].GetUInt8();
        temp.event.event_flags = fields[7].GetUInt32();

        temp.event.raw.param1 = fields[8].GetUInt32();
        temp.event.raw.param2 = fields[9].GetUInt32();
        temp.event.raw.param3 = fields[10].GetUInt32();
        temp.event.raw.param4 = fields[11].GetUInt32();
        temp.event.raw.param5 = fields[12].GetUInt32();

        temp.action.type = (SMART_ACTION)fields[13].GetUInt8();
        temp.action.raw.param1 = fields[14].GetUInt32();
        temp.action.raw.param2 = fields[15].GetUInt32();
        temp.action.raw.param3 = fields[16].GetUInt32();
        temp.action.raw.param4 = fields[17].GetUInt32();
        temp.action.raw.param5 = fields[18].GetUInt32();
        temp.action.raw.param6 = fields[19].GetUInt32();

        temp.target.type = (SMARTAI_TARGETS)fields[20].GetUInt8();
        temp.target.raw.param1 = fields[21].GetUInt32();
        temp.target.raw.param2 = fields[22].GetUInt32();
        temp.target.raw.param3 = fields[23].GetUInt32();
        temp.target.raw.param4 = fields[24].GetUInt32();
        temp.target.x = fields[25].GetFloat();
        temp.target.y = fields[26].GetFloat();
        temp.target.z = fields[27].GetFloat();
        temp.target.o = fields[28].GetFloat();

        //check target
        if (!IsTargetValid(temp))
            continue;

        // check all event and action params
        if (!IsEventValid(temp))
            continue;

        // creature entry / guid not found in storage, create empty event list for it and increase counters
        auto& eventmap = mEventMap[source_type];
        if (eventmap.find(temp.entryOrGuid) == eventmap.end())
        {
            ++count;
            SmartAIEventList eventList;
            eventmap[temp.entryOrGuid] = eventList;
        }
        // store the new event
        eventmap[temp.entryOrGuid].push_back(temp);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} SmartAI scripts in {} ms", count, GetMSTimeDiffToNow(oldMSTime));

}

bool SmartAIMgr::IsTargetValid(SmartScriptHolder const& e)
{
    if (e.GetActionType() == SMART_ACTION_INSTALL_AI_TEMPLATE)
        return true; // AI template has special handling
    switch (e.GetTargetType())
    {
        case SMART_TARGET_CREATURE_DISTANCE:
        case SMART_TARGET_CREATURE_RANGE:
        {
            if (e.target.unitDistance.creature && !sObjectMgr->GetCreatureTemplate(e.target.unitDistance.creature))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Creature entry {} as target_param1, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.target.unitDistance.creature);
                return false;
            }
            break;
        }
        case SMART_TARGET_GAMEOBJECT_DISTANCE:
        case SMART_TARGET_GAMEOBJECT_RANGE:
        {
            if (e.target.goDistance.entry && !sObjectMgr->GetGameObjectTemplate(e.target.goDistance.entry))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent GameObject entry {} as target_param1, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.target.goDistance.entry);
                return false;
            }
            break;
        }
        case SMART_TARGET_CREATURE_GUID:
        {
            if (e.target.unitGUID.entry && !IsCreatureValid(e, e.target.unitGUID.entry))
                return false;
            break;
        }
        case SMART_TARGET_GAMEOBJECT_GUID:
        {
            if (e.target.goGUID.entry && !IsGameObjectValid(e, e.target.goGUID.entry))
                return false;
            break;
        }
        case SMART_TARGET_PLAYER_DISTANCE:
        case SMART_TARGET_CLOSEST_PLAYER:
        {
            if (e.target.playerDistance.dist == 0)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} has maxDist 0 as target_param1, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                return false;
            }
            break;
        }
        case SMART_TARGET_PLAYER_RANGE:
        case SMART_TARGET_SELF:
        case SMART_TARGET_VICTIM:
        case SMART_TARGET_HOSTILE_SECOND_AGGRO:
        case SMART_TARGET_HOSTILE_LAST_AGGRO:
        case SMART_TARGET_HOSTILE_RANDOM:
        case SMART_TARGET_HOSTILE_RANDOM_NOT_TOP:
        case SMART_TARGET_ACTION_INVOKER:
        case SMART_TARGET_INVOKER_PARTY:
        case SMART_TARGET_POSITION:
        case SMART_TARGET_NONE:
        case SMART_TARGET_ACTION_INVOKER_VEHICLE:
        case SMART_TARGET_OWNER_OR_SUMMONER:
        case SMART_TARGET_THREAT_LIST:
        case SMART_TARGET_CLOSEST_GAMEOBJECT:
        case SMART_TARGET_CLOSEST_CREATURE:
        case SMART_TARGET_CLOSEST_ENEMY:
        case SMART_TARGET_CLOSEST_FRIENDLY:
        case SMART_TARGET_STORED:
        case SMART_TARGET_LOOT_RECIPIENTS:
            break;
        default:
            TC_LOG_ERROR("sql.sql", "SmartAIMgr: Not handled target_type({}), Entry {} SourceType {} Event {} Action {}, skipped.", e.GetTargetType(), e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
            return false;
    }
    return true;
}

bool SmartAIMgr::IsEventValid(SmartScriptHolder& e)
{
    if (e.event.type >= SMART_EVENT_END)
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: EntryOrGuid {} using event({}) has invalid event type ({}), skipped.", e.entryOrGuid, e.event_id, e.GetEventType());
        return false;
    }
    // in SMART_SCRIPT_TYPE_TIMED_ACTIONLIST all event types are overriden by core
    if (e.GetScriptType() != SMART_SCRIPT_TYPE_TIMED_ACTIONLIST && !(SmartAIEventMask[e.event.type][1] & SmartAITypeMask[e.GetScriptType()][1]))
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: EntryOrGuid {}, event type {} can not be used for Script type {}", e.entryOrGuid, e.GetEventType(), e.GetScriptType());
        return false;
    }
    if (e.action.type <= 0 || (e.action.type >= SMART_ACTION_END && e.action.type <= SMART_ACTION_project) || e.action.type >= SMART_ACTION_END_project)
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: EntryOrGuid {} using event({}) has invalid action type ({}), skipped.", e.entryOrGuid, e.event_id, e.GetActionType());
        return false;
    }
    if (e.event.event_phase_mask > SMART_EVENT_PHASE_ALL)
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: EntryOrGuid {} using event({}) has invalid phase mask ({}), skipped.", e.entryOrGuid, e.event_id, e.event.event_phase_mask);
        return false;
    }
    if (e.event.event_flags > SMART_EVENT_FLAGS_ALL)
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: EntryOrGuid {} using event({}) has invalid event flags ({}), skipped.", e.entryOrGuid, e.event_id, e.event.event_flags);
        return false;
    }
    if (e.GetScriptType() == SMART_SCRIPT_TYPE_TIMED_ACTIONLIST)
    {
        e.event.type = SMART_EVENT_UPDATE_OOC;//force default OOC, can change when calling the script!
        if (!IsMinMaxValid(e, e.event.minMaxRepeat.min, e.event.minMaxRepeat.max))
            return false;

        if (!IsMinMaxValid(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax))
            return false;
    }
    else
    {
        uint32 type = e.event.type;
        switch (type)
        {
            case SMART_EVENT_UPDATE:
            case SMART_EVENT_UPDATE_IC:
            case SMART_EVENT_UPDATE_OOC:
            case SMART_EVENT_HEALTH_PCT:
            case SMART_EVENT_MANA_PCT:
            case SMART_EVENT_TARGET_HEALTH_PCT:
            case SMART_EVENT_TARGET_MANA_PCT:
            case SMART_EVENT_RANGE:
            case SMART_EVENT_DAMAGED:
            case SMART_EVENT_DAMAGED_TARGET:
            case SMART_EVENT_RECEIVE_HEAL:
                if (!IsMinMaxValid(e, e.event.minMaxRepeat.min, e.event.minMaxRepeat.max))
                    return false;

                if (!IsMinMaxValid(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax))
                    return false;
                break;
            case SMART_EVENT_SPELLHIT:
            case SMART_EVENT_SPELLHIT_TARGET:
                if (e.event.spellHit.spell)
                {
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(e.event.spellHit.spell);
                    if (!spellInfo)
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Spell entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.spellHit.spell);
                        return false;
                    }
                    if (e.event.spellHit.school && (e.event.spellHit.school & spellInfo->SchoolMask) != spellInfo->SchoolMask)
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses Spell entry {} with invalid school mask, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.spellHit.spell);
                        return false;
                    }
                }
                if (!IsMinMaxValid(e, e.event.spellHit.cooldownMin, e.event.spellHit.cooldownMax))
                    return false;
                break;
            case SMART_EVENT_OOC_LOS:
            case SMART_EVENT_IC_LOS:
                if (!IsMinMaxValid(e, e.event.los.cooldownMin, e.event.los.cooldownMax))
                    return false;
                break;
            case SMART_EVENT_RESPAWN:
                if (e.event.respawn.type == SMART_SCRIPT_RESPAWN_CONDITION_MAP && !sMapStore.LookupEntry(e.event.respawn.map))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Map entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.respawn.map);
                    return false;
                }
                if (e.event.respawn.type == SMART_SCRIPT_RESPAWN_CONDITION_AREA && !sAreaTableStore.LookupEntry(e.event.respawn.area))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Area entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.respawn.area);
                    return false;
                }
                break;
            case SMART_EVENT_FRIENDLY_HEALTH:
                if (!NotNULL(e, e.event.friendlyHealth.radius))
                    return false;

                if (!IsMinMaxValid(e, e.event.friendlyHealth.repeatMin, e.event.friendlyHealth.repeatMax))
                    return false;
                break;
            case SMART_EVENT_FRIENDLY_IS_CC:
                if (!IsMinMaxValid(e, e.event.friendlyCC.repeatMin, e.event.friendlyCC.repeatMax))
                    return false;
                break;
            case SMART_EVENT_FRIENDLY_MISSING_BUFF:
            {
                if (!IsSpellValid(e, e.event.missingBuff.spell))
                    return false;

                if (!NotNULL(e, e.event.missingBuff.radius))
                    return false;

                if (!IsMinMaxValid(e, e.event.missingBuff.repeatMin, e.event.missingBuff.repeatMax))
                    return false;
                break;
            }
            case SMART_EVENT_KILL:
                if (!IsMinMaxValid(e, e.event.kill.cooldownMin, e.event.kill.cooldownMax))
                    return false;

                if (e.event.kill.creature && !IsCreatureValid(e, e.event.kill.creature))
                    return false;
                break;
            case SMART_EVENT_VICTIM_CASTING:
                if (e.event.targetCasting.spellId > 0 && !sSpellMgr->GetSpellInfo(e.event.targetCasting.spellId))
                {
                    TC_LOG_DEBUG("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Spell entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.spellHit.spell);
                    return false;
                }

                if (!IsMinMaxValid(e, e.event.targetCasting.repeatMin, e.event.targetCasting.repeatMax))
                    return false;
                break;
            case SMART_EVENT_PASSENGER_BOARDED:
            case SMART_EVENT_PASSENGER_REMOVED:
                if (!IsMinMaxValid(e, e.event.minMax.repeatMin, e.event.minMax.repeatMax))
                    return false;
                break;
            case SMART_EVENT_SUMMON_DESPAWNED:
            case SMART_EVENT_SUMMONED_UNIT:
                if (e.event.summoned.creature && !IsCreatureValid(e, e.event.summoned.creature))
                    return false;

                if (!IsMinMaxValid(e, e.event.summoned.cooldownMin, e.event.summoned.cooldownMax))
                    return false;
                break;
            case SMART_EVENT_ACCEPTED_QUEST:
            case SMART_EVENT_REWARD_QUEST:
                if (e.event.quest.quest && !IsQuestValid(e, e.event.quest.quest))
                    return false;

                if (!IsMinMaxValid(e, e.event.quest.cooldownMin, e.event.quest.cooldownMax))
                    return false;
                break;
            case SMART_EVENT_RECEIVE_EMOTE:
            {
                if (e.event.emote.emote && !IsTextEmoteValid(e, e.event.emote.emote))
                    return false;

                if (!IsMinMaxValid(e, e.event.emote.cooldownMin, e.event.emote.cooldownMax))
                    return false;
                break;
            }
            case SMART_EVENT_HAS_AURA:
            case SMART_EVENT_TARGET_BUFFED:
            {
                if (!IsSpellValid(e, e.event.aura.spell))
                    return false;

                if (!IsMinMaxValid(e, e.event.aura.repeatMin, e.event.aura.repeatMax))
                    return false;
                break;
            }
            case SMART_EVENT_TRANSPORT_ADDCREATURE:
            {
                if (e.event.transportAddCreature.creature && !IsCreatureValid(e, e.event.transportAddCreature.creature))
                    return false;
                break;
            }
            case SMART_EVENT_MOVEMENTINFORM:
            {
                if (e.event.movementInform.type > NULL_MOTION_TYPE)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses invalid Motion type {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.movementInform.type);
                    return false;
                }
                break;
            }
            case SMART_EVENT_DATA_SET:
            {
                if (!IsMinMaxValid(e, e.event.dataSet.cooldownMin, e.event.dataSet.cooldownMax))
                    return false;
                break;
            }
            case SMART_EVENT_AREATRIGGER_ONTRIGGER:
            {
                if (e.event.areatrigger.id && !IsAreaTriggerValid(e, e.event.areatrigger.id))
                    return false;
                break;
            }
            case SMART_EVENT_TEXT_OVER:
                //if (e.event.textOver.textGroupID && !IsTextValid(e, e.event.textOver.textGroupID)) return false;// 0 is a valid text group!
                break;
            case SMART_EVENT_LINK:
            {
                if (e.link && e.link == e.event_id)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {}, Event {}, Link Event is linking self (infinite loop), skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id);
                    return false;
                }
                break;
            }
            case SMART_EVENT_DUMMY_EFFECT:
            {
                if (!IsSpellValid(e, e.event.dummy.spell))
                    return false;

                if (e.event.dummy.effIndex > EFFECT_2)
                    return false;
                break;
            }
            case SMART_EVENT_IS_BEHIND_TARGET:
            {
                if (!IsMinMaxValid(e, e.event.behindTarget.cooldownMin, e.event.behindTarget.cooldownMax))
                    return false;
                break;
            }
            case SMART_EVENT_GAME_EVENT_START:
            case SMART_EVENT_GAME_EVENT_END:
            {
                GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
                if (e.event.gameEvent.gameEventId >= events.size() || !events[e.event.gameEvent.gameEventId].isValid())
                    return false;
                break;
            }
            case SMART_EVENT_ACTION_DONE:
            {
                if (e.event.doAction.eventId > EVENT_CHARGE)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses invalid event id {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.event.doAction.eventId);
                    return false;
                }
                break;
            }
            case SMART_EVENT_FRIENDLY_HEALTH_PCT:
                if (!IsMinMaxValid(e, e.event.friendlyHealthPct.repeatMin, e.event.friendlyHealthPct.repeatMax))
                    return false;

                if (e.event.friendlyHealthPct.maxHpPct > 100 || e.event.friendlyHealthPct.minHpPct > 100)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} has pct value above 100, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                    return false;
                }

                switch (e.GetTargetType())
                {
                    case SMART_TARGET_CREATURE_RANGE:
                    case SMART_TARGET_CREATURE_GUID:
                    case SMART_TARGET_CREATURE_DISTANCE:
                    case SMART_TARGET_CLOSEST_CREATURE:
                    case SMART_TARGET_CLOSEST_PLAYER:
                    case SMART_TARGET_PLAYER_RANGE:
                    case SMART_TARGET_PLAYER_DISTANCE:
                        break;
                    default:
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses invalid target_type {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.GetTargetType());
                        return false;
                }
                break;
            case SMART_EVENT_DISTANCE_CREATURE:
                if (e.event.distance.guid == 0 && e.event.distance.entry == 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_CREATURE did not provide creature guid or entry, skipped.");
                    return false;
                }

                if (e.event.distance.guid != 0 && e.event.distance.entry != 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_CREATURE provided both an entry and guid, skipped.");
                    return false;
                }

                if (e.event.distance.guid != 0 && !sObjectMgr->GetCreatureData(e.event.distance.guid))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_CREATURE using invalid creature guid {}, skipped.", e.event.distance.guid);
                    return false;
                }

                if (e.event.distance.entry != 0 && !sObjectMgr->GetCreatureTemplate(e.event.distance.entry))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_CREATURE using invalid creature entry {}, skipped.", e.event.distance.entry);
                    return false;
                }
                break;
            case SMART_EVENT_DISTANCE_GAMEOBJECT:
                if (e.event.distance.guid == 0 && e.event.distance.entry == 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_GAMEOBJECT did not provide gameobject guid or entry, skipped.");
                    return false;
                }

                if (e.event.distance.guid != 0 && e.event.distance.entry != 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_GAMEOBJECT provided both an entry and guid, skipped.");
                    return false;
                }

                if (e.event.distance.guid != 0 && !sObjectMgr->GetGOData(e.event.distance.guid))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_GAMEOBJECT using invalid gameobject guid {}, skipped.", e.event.distance.guid);
                    return false;
                }

                if (e.event.distance.entry != 0 && !sObjectMgr->GetGameObjectTemplate(e.event.distance.entry))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_DISTANCE_GAMEOBJECT using invalid gameobject entry {}, skipped.", e.event.distance.entry);
                    return false;
                }
                break;
            case SMART_EVENT_COUNTER_SET:
                if (!IsMinMaxValid(e, e.event.counter.cooldownMin, e.event.counter.cooldownMax))
                    return false;

                if (e.event.counter.id == 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_COUNTER_SET using invalid counter id {}, skipped.", e.event.counter.id);
                    return false;
                }

                if (e.event.counter.value == 0)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Event SMART_EVENT_COUNTER_SET using invalid value {}, skipped.", e.event.counter.value);
                    return false;
                }
                break;
            case SMART_EVENT_GO_STATE_CHANGED:
            case SMART_EVENT_GO_EVENT_INFORM:
            case SMART_EVENT_TIMED_EVENT_TRIGGERED:
            case SMART_EVENT_INSTANCE_PLAYER_ENTER:
            case SMART_EVENT_TRANSPORT_RELOCATE:
            case SMART_EVENT_CHARMED:
            case SMART_EVENT_CHARMED_TARGET:
            case SMART_EVENT_CORPSE_REMOVED:
            case SMART_EVENT_AI_INIT:
            case SMART_EVENT_TRANSPORT_ADDPLAYER:
            case SMART_EVENT_TRANSPORT_REMOVE_PLAYER:
            case SMART_EVENT_AGGRO:
            case SMART_EVENT_DEATH:
            case SMART_EVENT_EVADE:
            case SMART_EVENT_REACHED_HOME:
            case SMART_EVENT_RESET:
            case SMART_EVENT_QUEST_ACCEPTED:
            case SMART_EVENT_QUEST_OBJ_COMPLETION:
            case SMART_EVENT_QUEST_COMPLETION:
            case SMART_EVENT_QUEST_REWARDED:
            case SMART_EVENT_QUEST_FAIL:
            case SMART_EVENT_JUST_SUMMONED:
            case SMART_EVENT_WAYPOINT_START:
            case SMART_EVENT_WAYPOINT_REACHED:
            case SMART_EVENT_WAYPOINT_PAUSED:
            case SMART_EVENT_WAYPOINT_RESUMED:
            case SMART_EVENT_WAYPOINT_STOPPED:
            case SMART_EVENT_WAYPOINT_ENDED:
            case SMART_EVENT_GOSSIP_SELECT:
            case SMART_EVENT_GOSSIP_HELLO:
            case SMART_EVENT_JUST_CREATED:
            case SMART_EVENT_FOLLOW_COMPLETED:
            case SMART_EVENT_ON_SPELLCLICK:
            case SMART_EVENT_ON_GO_REPORT_USE:
                break;
            default:
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Not handled event_type({}), Entry {} SourceType {} Event {} Action {}, skipped.", e.GetEventType(), e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                return false;
        }
    }

    switch (e.GetActionType())
    {
        case SMART_ACTION_TALK:
            TC_SAI_IS_BOOLEAN_VALID(e, e.action.talk.useTalkTarget);
            if (!IsTextValid(e, e.action.talk.textGroupID))
                return false;
            break;
        case SMART_ACTION_SIMPLE_TALK:
            if (!IsTextValid(e, e.action.simpleTalk.textGroupID))
                return false;
            break;
        case SMART_ACTION_SET_FACTION:
            if (e.action.faction.factionID && !sFactionTemplateStore.LookupEntry(e.action.faction.factionID))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Faction {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.faction.factionID);
                return false;
            }
            break;
        case SMART_ACTION_MORPH_TO_ENTRY_OR_MODEL:
        case SMART_ACTION_MOUNT_TO_ENTRY_OR_MODEL:
            if (e.action.morphOrMount.creature || e.action.morphOrMount.model)
            {
                if (e.action.morphOrMount.creature > 0 && !sObjectMgr->GetCreatureTemplate(e.action.morphOrMount.creature))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Creature entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.morphOrMount.creature);
                    return false;
                }

                if (e.action.morphOrMount.model)
                {
                    if (e.action.morphOrMount.creature)
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} has ModelID set with also set CreatureId, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                        return false;
                    }
                    else if (!sCreatureDisplayInfoStore.LookupEntry(e.action.morphOrMount.model))
                    {
                        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Model id {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.morphOrMount.model);
                        return false;
                    }
                }
            }
            break;
        case SMART_ACTION_SOUND:
            if (!IsSoundValid(e, e.action.sound.sound))
                return false;
            TC_SAI_IS_BOOLEAN_VALID(e, e.action.sound.onlySelf);
            break;
        case SMART_ACTION_SET_EMOTE_STATE:
        case SMART_ACTION_PLAY_EMOTE:
            if (!IsEmoteValid(e, e.action.emote.emote))
                return false;
            break;
        case SMART_ACTION_PLAY_ANIMKIT:
            if (e.action.animKit.animKit && !IsAnimKitValid(e, e.action.animKit.animKit))
                return false;

            if (e.action.animKit.type > 3)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses invalid AnimKit type {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.animKit.type);
                return false;
            }
            break;
        case SMART_ACTION_FAIL_QUEST:
        case SMART_ACTION_ADD_QUEST:
            if (!e.action.quest.quest || !IsQuestValid(e, e.action.quest.quest))
                return false;
            break;
        case SMART_ACTION_ACTIVATE_TAXI:
            {
                if (!sTaxiPathStore.LookupEntry(e.action.taxi.id))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses invalid Taxi path ID {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.taxi.id);
                    return false;
                }
                break;
            }
        case SMART_ACTION_RANDOM_EMOTE:
            if (e.action.randomEmote.emote1 && !IsEmoteValid(e, e.action.randomEmote.emote1))
                return false;

            if (e.action.randomEmote.emote2 && !IsEmoteValid(e, e.action.randomEmote.emote2))
                return false;

            if (e.action.randomEmote.emote3 && !IsEmoteValid(e, e.action.randomEmote.emote3))
                return false;

            if (e.action.randomEmote.emote4 && !IsEmoteValid(e, e.action.randomEmote.emote4))
                return false;

            if (e.action.randomEmote.emote5 && !IsEmoteValid(e, e.action.randomEmote.emote5))
                return false;

            if (e.action.randomEmote.emote6 && !IsEmoteValid(e, e.action.randomEmote.emote6))
                return false;
            break;
        case SMART_ACTION_RANDOM_SOUND:
        {
            if (std::all_of(e.action.randomSound.sounds.begin(), e.action.randomSound.sounds.end(), [](uint32 sound) { return sound == 0; }))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} does not have any non-zero sound",
                    e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                return false;
            }

            for (uint32 sound : e.action.randomSound.sounds)
                if (sound && !IsSoundValid(e, sound))
                    return false;

            break;
        }
        case SMART_ACTION_ADD_AURA:
        case SMART_ACTION_CAST:
        case SMART_ACTION_INVOKER_CAST:
            if (!IsSpellValid(e, e.action.cast.spell))
                return false;
            break;
        case SMART_ACTION_CAST_RANDOM_SPELL:
            if (e.action.castRandomSpell.spell1 && !IsSpellValid(e, e.action.castRandomSpell.spell1))
                return false;
            if (e.action.castRandomSpell.spell2 && !IsSpellValid(e, e.action.castRandomSpell.spell2))
                return false;
            if (e.action.castRandomSpell.spell3 && !IsSpellValid(e, e.action.castRandomSpell.spell3))
                return false;
            if (e.action.castRandomSpell.spell4 && !IsSpellValid(e, e.action.castRandomSpell.spell4))
                return false;
            if (e.action.castRandomSpell.spell5 && !IsSpellValid(e, e.action.castRandomSpell.spell5))
                return false;
            break;
        case SMART_ACTION_CALL_AREAEXPLOREDOREVENTHAPPENS:
        case SMART_ACTION_CALL_GROUPEVENTHAPPENS:
            if (Quest const* qid = sObjectMgr->GetQuestTemplate(e.action.quest.quest))
            {
                if (!qid->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} SpecialFlags for Quest entry {} does not include FLAGS_EXPLORATION_OR_EVENT(2), skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.quest.quest);
                    return false;
                }
            }
            else
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Quest entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.quest.quest);
                return false;
            }
            break;
        case SMART_ACTION_SET_EVENT_PHASE:
            if (e.action.setEventPhase.phase >= SMART_EVENT_PHASE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} attempts to set phase {}. Phase mask cannot be used past phase {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.setEventPhase.phase, SMART_EVENT_PHASE_MAX-1);
                return false;
            }
            break;
        case SMART_ACTION_INC_EVENT_PHASE:
            if (!e.action.incEventPhase.inc && !e.action.incEventPhase.dec)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} is incrementing phase by 0, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                return false;
            }
            else if (e.action.incEventPhase.inc > SMART_EVENT_PHASE_MAX || e.action.incEventPhase.dec > SMART_EVENT_PHASE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} attempts to increment phase by too large value, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                return false;
            }
            break;
        case SMART_ACTION_REMOVEAURASFROMSPELL:
            if (e.action.removeAura.spell != 0 && !IsSpellValid(e, e.action.removeAura.spell))
                return false;
            break;
        case SMART_ACTION_RANDOM_PHASE:
            {
                if (e.action.randomPhase.phase1 >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhase.phase2 >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhase.phase3 >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhase.phase4 >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhase.phase5 >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhase.phase6 >= SMART_EVENT_PHASE_MAX)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} attempts to set invalid phase, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                    return false;
                }
            }
            break;
        case SMART_ACTION_RANDOM_PHASE_RANGE:       //PhaseMin, PhaseMax
            {
                if (e.action.randomPhaseRange.phaseMin >= SMART_EVENT_PHASE_MAX ||
                    e.action.randomPhaseRange.phaseMax >= SMART_EVENT_PHASE_MAX)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} attempts to set invalid phase, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
                    return false;
                }
                if (!IsMinMaxValid(e, e.action.randomPhaseRange.phaseMin, e.action.randomPhaseRange.phaseMax))
                    return false;
                break;
            }
        case SMART_ACTION_SUMMON_CREATURE:
            if (!IsCreatureValid(e, e.action.summonCreature.creature))
                return false;

            {
                auto it = _creatureSpellSummonCache.find(e.action.summonCreature.creature);
                if (it != _creatureSpellSummonCache.end())
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} creature summon: There is a summon spell for creature entry {} (SpellId: {}, effect: {})",
                        e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.summonCreature.creature, it->second.SpellId, it->second.EffectIndex);
            }

            if (e.action.summonCreature.type < TEMPSUMMON_TIMED_OR_DEAD_DESPAWN || e.action.summonCreature.type > TEMPSUMMON_MANUAL_DESPAWN)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses incorrect TempSummonType {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.summonCreature.type);
                return false;
            }
            break;
        case SMART_ACTION_CALL_KILLEDMONSTER:
            if (!IsCreatureValid(e, e.action.killedMonster.creature))
                return false;

            {
                auto it = _killCreditsSpellCache.find(e.action.killedMonster.creature);
                if (it != _killCreditsSpellCache.end())
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} Kill Credit: {} has already spell kill credit (SpellId: {} effect: {})", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.killedMonster.creature, it->second.SpellId, it->second.EffectIndex);
            }

            if (e.GetTargetType() == SMART_TARGET_POSITION)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses incorrect TargetType {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.GetTargetType());
                return false;
            }
            break;
        case SMART_ACTION_UPDATE_TEMPLATE:
            if (e.action.updateTemplate.creature && !IsCreatureValid(e, e.action.updateTemplate.creature))
                return false;
            break;
        case SMART_ACTION_SET_SHEATH:
            if (e.action.setSheath.sheath && e.action.setSheath.sheath >= MAX_SHEATH_STATE)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses incorrect Sheath state {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.setSheath.sheath);
                return false;
            }
            break;
        case SMART_ACTION_SET_REACT_STATE:
            {
                if (e.action.react.state > REACT_AGGRESSIVE)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Creature {} Event {} Action {} uses invalid React State {}, skipped.", e.entryOrGuid, e.event_id, e.GetActionType(), e.action.react.state);
                    return false;
                }
                break;
            }
        case SMART_ACTION_SUMMON_GO:
            if (!IsGameObjectValid(e, e.action.summonGO.entry))
                return false;

            {
                auto it = _gameObjectSpellSummonCache.find(e.action.summonGO.entry);
                if (it != _gameObjectSpellSummonCache.end())
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} gameobject summon: There is a summon spell for gameobject entry {} (SpellId: {}, effect: {})",
                        e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.summonGO.entry, it->second.SpellId, it->second.EffectIndex);
            }
            break;
        case SMART_ACTION_ADD_ITEM:
        case SMART_ACTION_REMOVE_ITEM:
            if (!IsItemValid(e, e.action.item.entry))
                return false;

            if (!NotNULL(e, e.action.item.count))
                return false;
            break;
        case SMART_ACTION_TELEPORT:
            if (!sMapStore.LookupEntry(e.action.teleport.mapID))
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Map entry {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.teleport.mapID);
                return false;
            }
            break;
        case SMART_ACTION_SET_COUNTER:
            if (e.action.setCounter.counterId == 0)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses wrong counterId {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.setCounter.counterId);
                return false;
            }

            if (e.action.setCounter.value == 0)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses wrong value {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.setCounter.value);
                return false;
            }

            break;
        case SMART_ACTION_INSTALL_AI_TEMPLATE:
            if (e.action.installTtemplate.id >= SMARTAI_TEMPLATE_END)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Creature {} Event {} Action {} uses non-existent AI template id {}, skipped.", e.entryOrGuid, e.event_id, e.GetActionType(), e.action.installTtemplate.id);
                return false;
            }
            break;
        case SMART_ACTION_WP_STOP:
            if (e.action.wpStop.quest && !IsQuestValid(e, e.action.wpStop.quest))
                return false;
            break;
        case SMART_ACTION_WP_START:
            {
                if (!sSmartWaypointMgr->GetPath(e.action.wpStart.pathID))
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Creature {} Event {} Action {} uses non-existent WaypointPath id {}, skipped.", e.entryOrGuid, e.event_id, e.GetActionType(), e.action.wpStart.pathID);
                    return false;
                }
                if (e.action.wpStart.quest && !IsQuestValid(e, e.action.wpStart.quest))
                    return false;
                if (e.action.wpStart.reactState > REACT_AGGRESSIVE)
                {
                    TC_LOG_ERROR("sql.sql", "SmartAIMgr: Creature {} Event {} Action {} uses invalid React State {}, skipped.", e.entryOrGuid, e.event_id, e.GetActionType(), e.action.wpStart.reactState);
                    return false;
                }
                break;
            }
        case SMART_ACTION_CREATE_TIMED_EVENT:
        {
            if (!IsMinMaxValid(e, e.action.timeEvent.min, e.action.timeEvent.max))
                return false;

            if (!IsMinMaxValid(e, e.action.timeEvent.repeatMin, e.action.timeEvent.repeatMax))
                return false;
            break;
        }
        case SMART_ACTION_SET_POWER:
        case SMART_ACTION_ADD_POWER:
        case SMART_ACTION_REMOVE_POWER:
            if (e.action.power.powerType > MAX_POWERS)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent Power {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.power.powerType);
                return false;
            }
            break;
        case SMART_ACTION_GAME_EVENT_STOP:
        {
            uint32 eventId = e.action.gameEventStop.id;

            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (eventId < 1 || eventId >= events.size())
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent event, eventId {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.gameEventStop.id);
                return false;
            }

            GameEventData const& eventData = events[eventId];
            if (!eventData.isValid())
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent event, eventId {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.gameEventStop.id);
                return false;
            }
            break;
        }
        case SMART_ACTION_GAME_EVENT_START:
        {
            uint32 eventId = e.action.gameEventStart.id;

            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (eventId < 1 || eventId >= events.size())
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent event, eventId {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.gameEventStart.id);
                return false;
            }

            GameEventData const& eventData = events[eventId];
            if (!eventData.isValid())
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent event, eventId {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), e.action.gameEventStart.id);
                return false;
            }
            break;
        }
        case SMART_ACTION_OVERRIDE_INHABIT_TYPE:
        {
            uint32 inhabbitType = e.action.inhabbitType.type;
            if (inhabbitType > 7)
            {
                TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} uses non-existent inhabit type {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), inhabbitType);
                return false;
            }
            break;
        }
        case SMART_ACTION_START_CLOSEST_WAYPOINT:
        case SMART_ACTION_FOLLOW:
        case SMART_ACTION_SET_ORIENTATION:
        case SMART_ACTION_STORE_TARGET_LIST:
        case SMART_ACTION_EVADE:
        case SMART_ACTION_FLEE_FOR_ASSIST:
        case SMART_ACTION_DIE:
        case SMART_ACTION_SET_IN_COMBAT_WITH_ZONE:
        case SMART_ACTION_SET_ACTIVE:
        case SMART_ACTION_WP_RESUME:
        case SMART_ACTION_KILL_UNIT:
        case SMART_ACTION_SET_INVINCIBILITY_HP_LEVEL:
        case SMART_ACTION_RESET_GOBJECT:
        case SMART_ACTION_ATTACK_START:
        case SMART_ACTION_THREAT_ALL_PCT:
        case SMART_ACTION_THREAT_SINGLE_PCT:
        case SMART_ACTION_SET_INST_DATA:
        case SMART_ACTION_SET_INST_DATA64:
        case SMART_ACTION_AUTO_ATTACK:
        case SMART_ACTION_ALLOW_COMBAT_MOVEMENT:
        case SMART_ACTION_CALL_FOR_HELP:
        case SMART_ACTION_SET_DATA:
        case SMART_ACTION_SET_VISIBILITY:
        case SMART_ACTION_WP_PAUSE:
        case SMART_ACTION_SET_FLY:
        case SMART_ACTION_SET_RUN:
        case SMART_ACTION_SET_SWIM:
        case SMART_ACTION_FORCE_DESPAWN:
        case SMART_ACTION_SET_INGAME_PHASE_MASK:
        case SMART_ACTION_SET_UNIT_FLAG:
        case SMART_ACTION_REMOVE_UNIT_FLAG:
        case SMART_ACTION_PLAYMOVIE:
        case SMART_ACTION_MOVE_TO_POS:
        case SMART_ACTION_RESPAWN_TARGET:
        case SMART_ACTION_CLOSE_GOSSIP:
        case SMART_ACTION_EQUIP:
        case SMART_ACTION_TRIGGER_TIMED_EVENT:
        case SMART_ACTION_REMOVE_TIMED_EVENT:
        case SMART_ACTION_OVERRIDE_SCRIPT_BASE_OBJECT:
        case SMART_ACTION_RESET_SCRIPT_BASE_OBJECT:
        case SMART_ACTION_ACTIVATE_GOBJECT:
        case SMART_ACTION_CALL_SCRIPT_RESET:
        case SMART_ACTION_SET_RANGED_MOVEMENT:
        case SMART_ACTION_CALL_TIMED_ACTIONLIST:
        case SMART_ACTION_SET_NPC_FLAG:
        case SMART_ACTION_ADD_NPC_FLAG:
        case SMART_ACTION_REMOVE_NPC_FLAG:
        // case SMART_ACTION_TALK:
        // case SMART_ACTION_SIMPLE_TALK:
        case SMART_ACTION_CROSS_CAST:
        case SMART_ACTION_CALL_RANDOM_TIMED_ACTIONLIST:
        case SMART_ACTION_CALL_RANDOM_RANGE_TIMED_ACTIONLIST:
        case SMART_ACTION_RANDOM_MOVE:
        case SMART_ACTION_SET_UNIT_FIELD_BYTES_1:
        case SMART_ACTION_REMOVE_UNIT_FIELD_BYTES_1:
        case SMART_ACTION_INTERRUPT_SPELL:
        case SMART_ACTION_SEND_GO_CUSTOM_ANIM:
        case SMART_ACTION_SET_DYNAMIC_FLAG:
        case SMART_ACTION_ADD_DYNAMIC_FLAG:
        case SMART_ACTION_REMOVE_DYNAMIC_FLAG:
        case SMART_ACTION_JUMP_TO_POS:
        case SMART_ACTION_SEND_GOSSIP_MENU:
        case SMART_ACTION_GO_SET_LOOT_STATE:
        case SMART_ACTION_SEND_TARGET_TO_TARGET:
        case SMART_ACTION_SET_HOME_POS:
        case SMART_ACTION_SET_HEALTH_REGEN:
        case SMART_ACTION_SET_ROOT:
        case SMART_ACTION_SET_GO_FLAG:
        case SMART_ACTION_ADD_GO_FLAG:
        case SMART_ACTION_REMOVE_GO_FLAG:
        case SMART_ACTION_SUMMON_CREATURE_GROUP:
        case SMART_ACTION_MOVE_OFFSET:
        case SMART_ACTION_SET_CORPSE_DELAY:
        case SMART_ACTION_DISABLE_EVADE:
        case SMART_ACTION_PLAY_SCENE_ID:
        case SMART_ACTION_PLAY_SCENE_PACKAGE:
        case SMART_ACTION_COMBAT_STOP:
        case SMART_ACTION_STOP_FOLLOW:
        case SMART_ACTION_SPELL_VISUAL_KIT:
        case SMART_ACTION_SET_INGAME_PHASE_ID:
            break;
        default:
            TC_LOG_ERROR("sql.sql", "SmartAIMgr: Not handled action_type({}), event_type({}), Entry {} SourceType {} Event {}, skipped.", e.GetActionType(), e.GetEventType(), e.entryOrGuid, e.GetScriptType(), e.event_id);
            return false;
    }

    return true;
}

// tc version
// bool SmartAIMgr::IsTextValid(SmartScriptHolder const& e, uint32 id)
// {
//     if (e.GetScriptType() != SMART_SCRIPT_TYPE_CREATURE)
//         return true;

//     uint32 entry = 0;

//     if (e.GetEventType() == SMART_EVENT_TEXT_OVER)
//     {
//         entry = e.event.textOver.creatureEntry;
//     }
//     else
//     {
//         switch (e.GetTargetType())
//         {
//             case SMART_TARGET_CREATURE_DISTANCE:
//             case SMART_TARGET_CREATURE_RANGE:
//             case SMART_TARGET_CLOSEST_CREATURE:
//                 return true; // ignore
//             default:
//                 if (e.entryOrGuid < 0)
//                 {
//                     ObjectGuid::LowType guid = ObjectGuid::LowType(-e.entryOrGuid);
//                     CreatureData const* data = sObjectMgr->GetCreatureData(guid);
//                     if (!data)
//                     {
//                         TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} using non-existent Creature guid {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), guid);
//                         return false;
//                     }
//                     else
//                         entry = data->id;
//                 }
//                 else
//                     entry = uint32(e.entryOrGuid);
//                 break;
//         }
//     }

//     if (!entry || !sCreatureTextMgr->TextExist(entry, uint8(id)))
//     {
//         TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} using non-existent Text id {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), id);
//         return false;
//     }

//     return true;
// }

bool SmartAIMgr::IsTextValid(SmartScriptHolder const& e, uint32 id) 
{
    if (e.GetScriptType() != SMART_SCRIPT_TYPE_CREATURE)
        return true;

    uint32 entry = 0;
    if (e.entryOrGuid >= 0)
        entry = uint32(e.entryOrGuid);
    else {
        entry = uint32(abs(e.entryOrGuid));
        CreatureData const* data = sObjectMgr->GetCreatureData(entry);
        if (!data)
        {
            TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} using non-existent Creature guid {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType(), entry);
            return false;
        }
        else
            entry = data->id;
    }

    if (!entry || !sCreatureTextMgr->TextExist(entry, uint8(id)))
    {
        TC_LOG_ERROR("sql.sql", "SmartAIMgr: Entry {} SourceType {} Event {} Action {} using non-existent Text id {}, skipped.", e.entryOrGuid, e.GetScriptType(), e.source_type, e.GetActionType(), id);
        return false;
    }

    return true;
}
