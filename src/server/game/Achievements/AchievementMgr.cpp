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

#include "AchievementMgr.h"
#include "Battleground.h"
#include "CellImpl.h"
#include "Chat.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "DBCStructure.h"
#include "DisableMgr.h"
#include "GameEventMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "InstanceScript.h"
#include "Language.h"
#include "Map.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "BattlePetMgr.h"
#include "PetBattle.h"
#include "BattlegroundSA.h"

namespace Trinity
{
    class AchievementChatBuilder
    {
        public:
            AchievementChatBuilder(Player const& player, ChatMsg msgtype, int32 textId, uint32 ach_id)
                : i_player(player), i_msgtype(msgtype), i_textId(textId), i_achievementId(ach_id) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                std::string text = sObjectMgr->GetTrinityString(i_textId, loc_idx);
                ChatHandler::BuildChatPacket(data, i_msgtype, LANG_UNIVERSAL, &i_player, &i_player, text, i_achievementId);
            }

        private:
            Player const& i_player;
            ChatMsg i_msgtype;
            int32 i_textId;
            uint32 i_achievementId;
    };
} // namespace Trinity

bool AchievementCriteriaData::IsValid(CriteriaEntry const* criteria)
{
    if (dataType >= MAX_ACHIEVEMENT_CRITERIA_DATA_TYPE)
    {
        TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` for criteria (Entry: %u) has wrong data type (%u), ignored.", criteria->ID, dataType);
        return false;
    }

    switch (criteria->Type)
    {
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
        case ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST:          // only hardcoded list
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
        case ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE:
        case ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL:
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE:
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2:
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        case ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST:    // only Children's Week achievements
        case ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM:                // only Children's Week achievements
        case ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN:
            break;
        default:
            if (dataType != ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` has data for non-supported criteria type (Entry: %u Type: %u), ignored.", criteria->ID, criteria->Type);
                return false;
            }
            break;
    }

    switch (dataType)
    {
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE:
        case ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT:
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_NTH_BIRTHDAY:
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_CREATURE:
            if (!creature.id || !sObjectMgr->GetCreatureTemplate(creature.id))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_CREATURE (%u) has non-existing creature id in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, creature.id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE:
            if (!classRace.class_id && !classRace.race_id)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) must not have 0 in either value field, ignored.",
                    criteria->ID, criteria->Type, dataType);
                return false;
            }
            if (classRace.class_id && ((1 << (classRace.class_id-1)) & CLASSMASK_ALL_PLAYABLE) == 0)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) has non-existing class in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, classRace.class_id);
                return false;
            }
            if (classRace.race_id && ((1 << (classRace.race_id-1)) & RACEMASK_ALL_PLAYABLE) == 0)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) has non-existing race in value2 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, classRace.race_id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH:
            if (health.percent < 1 || health.percent > 100)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_PLAYER_LESS_HEALTH (%u) has wrong percent value in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, health.percent);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA:
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA:
        {
            SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(aura.spell_id);
            if (!spellEntry)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type %s (%u) has wrong spell id in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, (dataType == ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA?"ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA":"ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA"), dataType, aura.spell_id);
                return false;
            }
            if (aura.effect_idx >= 3)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type %s (%u) has wrong spell effect index in value2 (%u), ignored.",
                    criteria->ID, criteria->Type, (dataType == ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA?"ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA":"ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA"), dataType, aura.effect_idx);
                return false;
            }
            if (!spellEntry->Effects[aura.effect_idx].ApplyAuraName)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type %s (%u) has non-aura spell effect (ID: %u Effect: %u), ignores.",
                    criteria->ID, criteria->Type, (dataType == ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA?"ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA":"ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA"), dataType, aura.spell_id, aura.effect_idx);
                return false;
            }
            return true;
        }
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AREA:
            if (!sAreaTableStore.LookupEntry(area.id))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AREA (%u) has wrong area id in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, area.id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_VALUE:
            if (value.compType >= COMP_TYPE_MAX)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_VALUE (%u) has wrong ComparisionType in value2 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, value.compType);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_LEVEL:
            if (level.minlevel > STRONG_MAX_LEVEL)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_LEVEL (%u) has wrong minlevel in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, level.minlevel);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_GENDER:
            if (gender.gender > GENDER_NONE)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_GENDER (%u) has wrong gender in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, gender.gender);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT:
            if (!ScriptId)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT (%u) does not have ScriptName set, ignored.",
                    criteria->ID, criteria->Type, dataType);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT:
            if (map_players.maxcount <= 0)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT (%u) has wrong max players count in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, map_players.maxcount);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_TEAM:
            if (team.team != ALLIANCE && team.team != HORDE)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_T_TEAM (%u) has unknown team in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, team.team);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_DRUNK:
            if (drunk.state >= MAX_DRUNKEN)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_DRUNK (%u) has unknown drunken state in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, drunk.state);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_HOLIDAY:
            if (!sHolidaysStore.LookupEntry(holiday.id))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_HOLIDAY (%u) has unknown holiday in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, holiday.id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE:
            return true;                                    // not check correctness node indexes
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_EQUIPED_ITEM:
            if (equipped_item.item_quality >= MAX_ITEM_QUALITY)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_requirement` (Entry: %u Type: %u) for requirement ACHIEVEMENT_CRITERIA_REQUIRE_S_EQUIPED_ITEM (%u) has unknown quality state in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, equipped_item.item_quality);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE:
            if (!classRace.class_id && !classRace.race_id)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) must not have 0 in either value field, ignored.",
                    criteria->ID, criteria->Type, dataType);
                return false;
            }
            if (classRace.class_id && ((1 << (classRace.class_id-1)) & CLASSMASK_ALL_PLAYABLE) == 0)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) has non-existing class in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, classRace.class_id);
                return false;
            }
            if (classRace.race_id && ((1 << (classRace.race_id-1)) & RACEMASK_ALL_PLAYABLE) == 0)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) has non-existing race in value2 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, classRace.race_id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_KNOWN_TITLE:
            if (!sCharTitlesStore.LookupEntry(known_title.title_id))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_S_KNOWN_TITLE (%u) have unknown title_id in value1 (%u), ignore.",
                    criteria->ID, criteria->Type, dataType, known_title.title_id);
                return false;
            }
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_GAME_EVENT:
        {
            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (game_event.id < 1 || game_event.id >= events.size())
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) for data type ACHIEVEMENT_CRITERIA_DATA_TYPE_GAME_EVENT (%u) has unknown game_event in value1 (%u), ignored.",
                    criteria->ID, criteria->Type, dataType, game_event.id);
                return false;
            }
            return true;
        }
        default:
            TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` (Entry: %u Type: %u) has data for non-supported data type (%u), ignored.", criteria->ID, criteria->Type, dataType);
            return false;
    }
}

bool AchievementCriteriaData::Meets(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscValue1 /*= 0*/) const
{
    switch (dataType)
    {
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE:
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_CREATURE:
            if (!target || target->GetTypeId() != TYPEID_UNIT)
                return false;
            return target->GetEntry() == creature.id;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            if (classRace.class_id && classRace.class_id != target->ToPlayer()->GetClass())
                return false;
            if (classRace.race_id && classRace.race_id != target->ToPlayer()->GetRace())
                return false;
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE:
            if (!source || source->GetTypeId() != TYPEID_PLAYER)
                return false;
            if (classRace.class_id && classRace.class_id != source->ToPlayer()->GetClass())
                return false;
            if (classRace.race_id && classRace.race_id != source->ToPlayer()->GetRace())
                return false;
            return true;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            return !target->HealthAbovePct(health.percent);
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA:
            return source->HasAuraEffect(aura.spell_id, aura.effect_idx);
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AREA:
        {
            uint32 zone_id, area_id;
            source->GetZoneAndAreaId(zone_id, area_id);
            return area.id == zone_id || area.id == area_id;
        }
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA:
            return target && target->HasAuraEffect(aura.spell_id, aura.effect_idx);
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_VALUE:
            return CompareValues(ComparisionType(value.compType), miscValue1, value.value);
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_LEVEL:
            if (!target)
                return false;
            return target->GetLevel() >= level.minlevel;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_GENDER:
            if (!target)
                return false;
            return target->GetGender() == gender.gender;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT:
            return sScriptMgr->OnCriteriaCheck(ScriptId, const_cast<Player*>(source), const_cast<Unit*>(target));
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT:
            return source->GetMap()->GetPlayersCountExceptGMs() <= map_players.maxcount;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_T_TEAM:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            return target->ToPlayer()->GetTeam() == team.team;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_DRUNK:
            return Player::GetDrunkenstateByValue(source->GetDrunkValue()) >= DrunkenState(drunk.state);
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_HOLIDAY:
            return IsHolidayActive(HolidayIds(holiday.id));
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE:
        {
            Battleground* bg = source->GetBattleground();
            if (!bg)
                return false;

            uint32 score = bg->GetTeamScore(source->GetBGTeam() == ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE);
            return score >= bg_loss_team_score.min_score && score <= bg_loss_team_score.max_score;
        }
        case ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT:
        {
            if (!source->IsInWorld())
                return false;
            Map* map = source->GetMap();
            if (!map->IsDungeon())
            {
                TC_LOG_ERROR("achievement", "Achievement system call ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT (%u) for achievement criteria %u for non-dungeon/non-raid map %u",
                    ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT, criteria_id, map->GetId());
                    return false;
            }
            InstanceScript* instance = ((InstanceMap*)map)->GetInstanceScript();
            if (!instance)
            {
                TC_LOG_ERROR("achievement", "Achievement system call ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT (%u) for achievement criteria %u for map %u but map does not have a instance script",
                    ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT, criteria_id, map->GetId());
                return false;
            }
            return instance->CheckAchievementCriteriaMeet(criteria_id, source, target, miscValue1);
        }
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_EQUIPED_ITEM:
        {
            ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(miscValue1);
            if (!pProto)
                return false;
            return pProto->ItemLevel >= equipped_item.item_level && pProto->Quality >= equipped_item.item_quality;
        }
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_ID:
            return source->GetMapId() == map_id.mapId;
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_NTH_BIRTHDAY:
            return (IsHolidayActive(HOLIDAY_ANNIVERSARY_7_YEARS) ||
                    IsHolidayActive(HOLIDAY_ANNIVERSARY_8_YEARS) ||
                    IsEventActive(67 /*HOLIDAY_ANNIVERSARY_9_YEARS = 509*/) ||
                    IsEventActive(68 /*HOLIDAY_ANNIVERSARY_10_YEARS = 514*/));
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_S_KNOWN_TITLE:
        {
            if (CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(known_title.title_id))
                return source && source->HasTitle(titleInfo->ID);

            return false;
        }
        case ACHIEVEMENT_CRITERIA_DATA_TYPE_GAME_EVENT:
            return IsEventActive(game_event.id);
        default:
            break;
    }
    return false;
}

bool AchievementCriteriaDataSet::Meets(Player const* source, Unit const* target, uint32 miscValue /*= 0*/) const
{
    for (Storage::const_iterator itr = storage.begin(); itr != storage.end(); ++itr)
        if (!itr->Meets(criteria_id, source, target, miscValue))
            return false;

    return true;
}

void GuildAchievementMgr::SendPacket(WorldPacket* data) const
{
    GetOwner()->BroadcastPacket(data);
}

void PlayerAchievementMgrBase::SendPacket(WorldPacket* data) const
{
    GetOwner()->GetSession()->SendPacket(data);
}

ObjectGuid PlayerAchievementMgrBase::GetGUID() const
{
    return GetOwner()->GetGUID();
}

ObjectGuid GuildAchievementMgr::GetGUID() const
{
    return GetOwner()->GetGUID();
}

// Btw, there is no one ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS criteria that is not account wide
uint32 PlayerAchievementMgr::GetAchievementPoints() const
{
    return m_achievementPoints + GetOwner()->GetSession()->GetAchievementMgr().m_achievementPoints;
}

uint32 AccountAchievementMgr::GetAchievementPoints() const
{
    return m_achievementPoints + GetOwner()->GetAchievementMgr().m_achievementPoints;
}

void PlayerAchievementMgrBase::RemoveCriteriaProgress(CriteriaEntry const* entry)
{
    if (!entry)
        return;

    CriteriaProgressMap::iterator criteriaProgress = m_criteriaProgress.find(entry->ID);
    if (criteriaProgress == m_criteriaProgress.end())
        return;

    WorldPacket data(SMSG_CRITERIA_DELETED, 4);
    data << uint32(entry->ID);
    SendPacket(&data);

    m_criteriaProgress.erase(criteriaProgress);
}

void GuildAchievementMgr::RemoveCriteriaProgress(CriteriaEntry const* entry)
{
    if (!entry)
        return;

    CriteriaProgressMap::iterator criteriaProgress = m_criteriaProgress.find(entry->ID);
    if (criteriaProgress == m_criteriaProgress.end())
        return;

    ObjectGuid guid = GetOwner()->GetGUID();

    WorldPacket data(SMSG_GUILD_CRITERIA_DELETED, 4 + 8);

    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);

    data << uint32(entry->ID);
    SendPacket(&data);

    m_criteriaProgress.erase(criteriaProgress);
}

void PlayerAchievementMgrBase::ResetAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1, uint64 miscValue2, bool evenIfCriteriaComplete)
{
    TC_LOG_DEBUG("achievement", "ResetAchievementCriteria(%u, " UI64FMTD ", " UI64FMTD ")", type, miscValue1, miscValue2);

    // disable for gamemasters with GM-mode enabled
    if (GetOwner()->IsGameMaster())
        return;

    for (auto&& criteria : sAchievementMgr->GetAchievementCriteriaByType(type, m_type))
    {
        if (criteria->Entry->FailEvent != miscValue1 || (criteria->Entry->FailAsset && criteria->Entry->FailAsset != miscValue2))
            continue;

        std::vector<CriteriaTreeNode const*> const* trees = sAchievementMgr->GetCriteriaTreesByCriteria(criteria->Entry->ID, m_type);
        if (!trees)
            continue;

        bool allComplete = true;
        for (CriteriaTreeNode const* tree : *trees)
        {
            // don't update already completed criteria if not forced or achievement already complete
            if (!(IsCompletedCriteriaTree(tree, GetOwner()) && !evenIfCriteriaComplete) || !HasAchieved(tree->Achievement->ID))
            {
                allComplete = false;
                break;
            }
        }

        if (allComplete)
            continue;

        RemoveCriteriaProgress(criteria->Entry);
    }
}

void PlayerAchievementMgrBase::ResetCriterias(CriteriaResetTypes type, uint32 asset)
{
    for (auto&& criteria : sAchievementMgr->GetCriteriaByResetType(type))
    {
        if (!criteria->Entry->FailAsset || asset == criteria->Entry->FailAsset)
        {
            std::vector<CriteriaTreeNode const*> const* trees = sAchievementMgr->GetCriteriaTreesByCriteria(criteria->Entry->ID, m_type);
            if (!trees)
                continue;

            // Where are only one case of this and client shows progress correctly for completed achievement somehow
            size_t achieved = 0;
            for (CriteriaTreeNode const* tree : *trees)
                if (HasAchieved(tree->Achievement->ID))
                    ++achieved;

            if (achieved == trees->size())
                continue;

            m_additionalCriteriaProgress.erase(criteria->Entry->ID);
            SetCriteriaProgress(criteria, 0, GetOwner(), PROGRESS_SET);
        }
    }
}

void PlayerAchievementMgrBase::ResetCriterias(CriteriaStartTypes type, uint32 asset)
{
    for (auto&& criteria : sAchievementMgr->GetCriteriaByStartType(type))
    {
        if (asset == criteria->Entry->StartAsset)
        {
            std::vector<CriteriaTreeNode const*> const* trees = sAchievementMgr->GetCriteriaTreesByCriteria(criteria->Entry->ID, m_type);
            if (!trees)
                continue;

            bool hasAchieved = false;
            for (CriteriaTreeNode const* tree : *trees)
            {
                if (HasAchieved(tree->Achievement->ID))
                {
                    hasAchieved = true;
                    break;
                }
            }

            if (hasAchieved)
                continue;

            if (criteria->Entry->TimeLimit)
                RemoveCriteriaTimer(CriteriaStartTypes(criteria->Entry->StartEvent), criteria->Entry->StartAsset);
            else
                SetCriteriaProgress(criteria, 0, GetOwner(), PROGRESS_SET);
        }
    }
}

void PlayerAchievementMgr::DeleteFromDB(ObjectGuid::LowType lowguid)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void GuildAchievementMgr::DeleteFromDB(uint32 lowguid)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ALL_GUILD_ACHIEVEMENTS);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ALL_GUILD_ACHIEVEMENT_CRITERIA);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void PlayerAchievementMgr::SaveToDB(CharacterDatabaseTransaction trans)
{
    for (auto&& it : m_completedAchievements)
    {
        if (!it.second.Changed)
            continue;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_BY_ACHIEVEMENT);
        stmt->setUInt16(0, it.first);
        stmt->setUInt32(1, GetOwner()->GetGUID());
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ACHIEVEMENT);
        stmt->setUInt32(0, GetOwner()->GetGUID());
        stmt->setUInt16(1, it.first);
        stmt->setUInt32(2, uint32(it.second.Date));
        trans->Append(stmt);

        it.second.Changed = false;
    }

    for (auto&& it : m_criteriaProgress)
    {
        if (!it.second.Changed)
            continue;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS_BY_CRITERIA);
        stmt->setUInt32(0, GetOwner()->GetGUID());
        stmt->setUInt16(1, it.first);
        trans->Append(stmt);

        if (it.second.Counter)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ACHIEVEMENT_PROGRESS);
            stmt->setUInt32(0, GetOwner()->GetGUID());
            stmt->setUInt16(1, it.first);
            stmt->setUInt32(2, it.second.Counter);
            stmt->setUInt32(3, uint32(it.second.Date));
            trans->Append(stmt);
        }
        it.second.Changed = false;
    }
}

void GuildAchievementMgr::SaveToDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt;
    std::ostringstream guidstr;
    for (auto&& it : m_completedAchievements)
    {
        if (!it.second.Changed)
            continue;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_ACHIEVEMENT);
        stmt->setUInt32(0, GetOwner()->GetId());
        stmt->setUInt16(1, it.first);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ACHIEVEMENT);
        stmt->setUInt32(0, GetOwner()->GetId());
        stmt->setUInt16(1, it.first);
        stmt->setUInt32(2, it.second.Date);
        for (auto&& guid : it.second.Guids)
            guidstr << guid.GetCounter() << ',';

        stmt->setString(3, guidstr.str());
        trans->Append(stmt);

        guidstr.str("");
    }

    for (auto&& it : m_criteriaProgress)
    {
        if (!it.second.Changed)
            continue;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_ACHIEVEMENT_CRITERIA);
        stmt->setUInt32(0, GetOwner()->GetId());
        stmt->setUInt16(1, it.first);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ACHIEVEMENT_CRITERIA);
        stmt->setUInt32(0, GetOwner()->GetId());
        stmt->setUInt16(1, it.first);
        stmt->setUInt64(2, it.second.Counter);
        stmt->setUInt32(3, it.second.Date);
        stmt->setUInt32(4, it.second.CompletedGUID.GetCounter());
        trans->Append(stmt);
    }
}

void AccountAchievementMgr::SaveToDB(CharacterDatabaseTransaction trans)
{
    for (auto&& it : m_completedAchievements)
    {
        if (!it.second.Changed)
            continue;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ACCOUNT_ACHIEVEMENT);
        stmt->setUInt32(0, GetOwner()->GetSession()->GetAccountId());
        stmt->setUInt16(1, it.first);
        stmt->setUInt32(2, uint32(it.second.Date));
        stmt->setUInt32(3, it.second.CompletedBy);
        trans->Append(stmt);

        it.second.Changed = false;
    }

    for (auto&& it : m_criteriaProgress)
    {
        if (!it.second.Changed)
            continue;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ACCOUNT_ACHIEVEMENT_PROGRESS);
        stmt->setUInt32(0, GetOwner()->GetSession()->GetAccountId());
        stmt->setUInt16(1, it.first);
        stmt->setUInt32(2, it.second.Counter);
        stmt->setUInt32(3, uint32(it.second.Date));
        trans->Append(stmt);

        it.second.Changed = false;
    }
}

void PlayerAchievementMgr::LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult)
{
    auto& achievements = GetOwner()->GetSession()->GetAchievementMgr().m_completedAchievements;

    auto rewardIfNeed = [&](AchievementEntry const* achievement, bool account)
    {
        if (AchievementReward const* reward = sAchievementMgr->GetAchievementReward(achievement))
        {
            uint32 index;
            if (achievement->ID == 1793 || achievement->ID == 8397)
                index = GetOwner()->GetGender() == GENDER_MALE ? 0 : 1;
            else
                index = GetOwner()->GetTeam() == ALLIANCE ? 0 : 1;
            if (uint32 titleId = reward->TitleId[index])
                if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(titleId))
                    if (!account || titleEntry->Unk1)   // 0 for gladiators and it must be for single player
                        GetOwner()->SetTitle(titleEntry);
            if (reward->SpellId < 0)
            {
                // Because spells are not loaded yet
                uint32 spellId = uint32(-reward->SpellId);
                Player* player = GetOwner();
                player->Schedule(Milliseconds(1), [=]()
                {
                    if (!player->HasSpell(spellId))
                        player->AddSpell(spellId, true, true, false, false, true);
                });
            }
        }
    };

    if (achievementResult)
    {
        do
        {
            Field* fields = achievementResult->Fetch();
            uint32 achievementid = fields[0].GetUInt16();

            // must not happen: cleanup at server startup in sAchievementMgr->LoadCompletedAchievements()
            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(achievementid);
            if (!achievement)
                continue;

            CompletedAchievementData& ca = m_completedAchievements[achievementid];
            ca.Date = time_t(fields[1].GetUInt32());
            ca.Changed = false;
            ca.CompletedBy = 0;

            m_achievementPoints += achievement->Points;

            rewardIfNeed(achievement, false);

            if (achievements.find(achievementid) == achievements.end())
            {
                auto& data = achievements[achievementid];
                data.Date = ca.Date;
                data.CompletedBy = GetOwner()->GetGUID().GetCounter();
                data.Changed = true;
            }
        }
        while (achievementResult->NextRow());
    }

    if (criteriaResult)
    {
        time_t now = time(NULL);
        do
        {
            Field* fields = criteriaResult->Fetch();
            uint32 id      = fields[0].GetUInt16();
            uint64 counter = fields[1].GetUInt64();
            time_t date    = time_t(fields[2].GetUInt32());

            CriteriaEntry const* criteria = sAchievementMgr->GetAchievementCriteria(id);
            if (!criteria)
            {
                // we will remove not existed criteria for all characters
                TC_LOG_ERROR("achievement", "Non-existing achievement criteria %u data removed from table `character_achievement_progress`.", id);

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA);
                stmt->setUInt16(0, uint16(id));
                CharacterDatabase.Execute(stmt);

                continue;
            }

            if (criteria->TimeLimit && time_t(date + criteria->TimeLimit) < now)
                continue;

            CriteriaProgress& progress = m_criteriaProgress[id];
            progress.Counter = counter;
            progress.Date    = date;
            progress.Changed = false;
        }
        while (criteriaResult->NextRow());
    }

    for (auto&& it : achievements)
        if (AchievementEntry const* achievement = sAchievementMgr->GetAchievement(it.first))
            rewardIfNeed(achievement, true);

    // After loading

    for (auto&& achievementId : sAchievementMgr->GetAchievementsWithTitle(Player::TeamForRace(GetOwner()->GetRace())))
        if (AchievementEntry const* achievement = sAchievementMgr->GetAchievement(achievementId))
            if (AchievementReward const* reward = sAchievementMgr->GetAchievementReward(achievement))
                if (uint32 titleId = reward->TitleId[Player::TeamForRace(GetOwner()->GetRace()) == ALLIANCE ? 0 : 1])
                    if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(titleId))
                        if (GetOwner()->HasTitle(titleEntry) && !GetOwner()->HasAchieved(achievementId, !titleEntry->Unk1))
                            GetOwner()->SetTitle(titleEntry, true);
}

void AccountAchievementMgr::LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult)
{
    if (achievementResult)
    {
        do
        {
            Field* fields = achievementResult->Fetch();
            uint32 achievementid = fields[0].GetUInt16();

            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(achievementid);
            if (!achievement)
                continue;

            CompletedAchievementData& ca = m_completedAchievements[achievementid];
            ca.Date = time_t(fields[1].GetUInt32());
            ca.CompletedBy = fields[2].GetUInt32();
            ca.Changed = false;

            m_achievementPoints += achievement->Points;
        } while (achievementResult->NextRow());
    }

    if (criteriaResult)
    {
        time_t now = time(NULL);
        do
        {
            Field* fields = criteriaResult->Fetch();
            uint32 id = fields[0].GetUInt16();
            uint64 counter = fields[1].GetUInt64();
            time_t date = time_t(fields[2].GetUInt32());

            CriteriaEntry const* criteria = sAchievementMgr->GetAchievementCriteria(id);
            if (!criteria)
            {
                // we will remove not existed criteria for all characters
                TC_LOG_ERROR("achievement", "Non-existing achievement criteria %u data removed from table `account_achievement_progress`.", id);

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA);
                stmt->setUInt16(0, uint16(id));
                CharacterDatabase.Execute(stmt);
                continue;
            }

            if (criteria->TimeLimit && time_t(date + criteria->TimeLimit) < now)
                continue;

            CriteriaProgress& progress = m_criteriaProgress[id];
            progress.Counter = counter;
            progress.Date = date;
            progress.Changed = false;
        } while (criteriaResult->NextRow());
    }
}

void GuildAchievementMgr::LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult)
{
    if (achievementResult)
    {
        do
        {
            Field* fields = achievementResult->Fetch();
            uint32 achievementid = fields[0].GetUInt16();

            // must not happen: cleanup at server startup in sAchievementMgr->LoadCompletedAchievements()
            AchievementEntry const* achievement = sAchievementMgr->GetAchievement(achievementid);
            if (!achievement)
                continue;

            CompletedAchievementData& ca = m_completedAchievements[achievementid];
            ca.Date = time_t(fields[1].GetUInt32());
            Tokenizer guids(fields[2].GetString(), ' ');
            for (uint32 i = 0; i < guids.size(); ++i)
                ca.Guids.insert(ObjectGuid::Create<HighGuid::Player>(uint32(atol(guids[i]))));

            ca.Changed = false;

            m_achievementPoints  += achievement->Points;
        }
        while (achievementResult->NextRow());
    }

    if (criteriaResult)
    {
        time_t now = time(NULL);
        do
        {
            Field* fields = criteriaResult->Fetch();
            uint32 id      = fields[0].GetUInt16();
            uint32 counter = fields[1].GetUInt32();
            time_t date    = time_t(fields[2].GetUInt32());
            uint32 guid    = fields[3].GetUInt32();

            CriteriaEntry const* criteria = sAchievementMgr->GetAchievementCriteria(id);
            if (!criteria)
            {
                // we will remove not existed criteria for all guilds
                TC_LOG_ERROR("achievement", "Non-existing achievement criteria %u data removed from table `guild_achievement_progress`.", id);

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA_GUILD);
                stmt->setUInt16(0, uint16(id));
                CharacterDatabase.Execute(stmt);
                continue;
            }

            if (criteria->TimeLimit && time_t(date + criteria->TimeLimit) < now)
                continue;

            CriteriaProgress& progress = m_criteriaProgress[id];
            progress.Counter = counter;
            progress.Date    = date;
            progress.CompletedGUID = ObjectGuid(HighGuid::Player, guid);
            progress.Changed = false;
        } while (criteriaResult->NextRow());
    }
}

void PlayerAchievementMgr::Reset()
{
    for (CompletedAchievementMap::const_iterator iter = m_completedAchievements.begin(); iter != m_completedAchievements.end(); ++iter)
    {
        WorldPacket data(SMSG_ACHIEVEMENT_DELETED, 4);
        data << uint32(iter->first);
        data << uint32(0); // Unk
        SendPacket(&data);
    }

    for (CriteriaProgressMap::const_iterator iter = m_criteriaProgress.begin(); iter != m_criteriaProgress.end(); ++iter)
    {
        WorldPacket data(SMSG_CRITERIA_DELETED, 4);
        data << uint32(iter->first);
        SendPacket(&data);
    }

    m_completedAchievements.clear();
    m_achievementPoints = 0;
    m_criteriaProgress.clear();
    DeleteFromDB(GetOwner()->GetGUID().GetCounter());

    // re-fill data
    CheckAllAchievementCriteria(GetOwner());
}

void GuildAchievementMgr::Reset()
{
    ObjectGuid guid = GetOwner()->GetGUID();
    for (CompletedAchievementMap::const_iterator iter = m_completedAchievements.begin(); iter != m_completedAchievements.end(); ++iter)
    {
        WorldPacket data(SMSG_GUILD_ACHIEVEMENT_DELETED, 4 + 8);
        data << uint32(iter->first);
        data.AppendPackedTime(iter->second.Date);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[3]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[3]);
        SendPacket(&data);
    }

    while (!m_criteriaProgress.empty())
        if (CriteriaEntry const* criteria = sAchievementMgr->GetAchievementCriteria(m_criteriaProgress.begin()->first))
            RemoveCriteriaProgress(criteria);

    m_achievementPoints = 0;
    m_completedAchievements.clear();
    DeleteFromDB(GetOwner()->GetId());
}

uint32 AchievementMgr::GetCriteriaProgress(uint32 id)
{
    auto itr = m_criteriaProgress.find(id);
    if (itr == m_criteriaProgress.end())
        return 0;
    return itr->second.Counter;
}

bool AchievementMgr::TryToCompleteAchievement(uint32 achievementId, Player* player)
{
    if (auto const* achievement = sAchievementStore.LookupEntry(achievementId))
    {
        if (IsCompletedAchievement(achievement, player))
        {
            CompletedAchievement(achievement, player);
            return true;
        }
    }
    return false;
}

void AchievementMgr::SendAchievementEarned(AchievementEntry const* achievement, Player* referencePlayer) const
{
    // Don't send for achievements with ACHIEVEMENT_FLAG_HIDDEN
    if (achievement->Flags & ACHIEVEMENT_FLAG_HIDDEN)
        return;

    TC_LOG_DEBUG("achievement", "AchievementMgr::SendAchievementEarned(%u)", achievement->ID);

    if (Guild* guild = sGuildMgr->GetGuildById(referencePlayer->GetGuildId()))
    {
        Trinity::AchievementChatBuilder say_builder(*referencePlayer, CHAT_MSG_GUILD_ACHIEVEMENT, LANG_ACHIEVEMENT_EARNED, achievement->ID);
        Trinity::LocalizedPacketDo<Trinity::AchievementChatBuilder> say_do(say_builder);
        guild->BroadcastWorker(say_do);
    }

    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_KILL | ACHIEVEMENT_FLAG_REALM_FIRST_REACH))
    {
        // broadcast realm first reached
        ObjectGuid guid = referencePlayer->GetGUID();
        WorldPacket data(SMSG_SERVER_FIRST_ACHIEVEMENT, referencePlayer->GetName().size() + 1 + 8 + 4 + 4);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[4]);
        data.WriteBits(referencePlayer->GetName().size(), 7);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[1]);
        data.WriteBit(false); // 1=link supplied string as player name, 0=display plain string
        data.WriteByteSeq(guid[1]);
        data.WriteString(referencePlayer->GetName());
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[2]);
        data << uint32(achievement->ID);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[7]);
        sWorld->SendGlobalMessage(&data);
    }
    // if player is in world he can tell his friends about new achievement
    else if (referencePlayer->IsInWorld())
    {
        Trinity::AchievementChatBuilder say_builder(*referencePlayer, CHAT_MSG_ACHIEVEMENT, LANG_ACHIEVEMENT_EARNED, achievement->ID);

        CellCoord p = Trinity::ComputeCellCoord(referencePlayer->GetPositionX(), referencePlayer->GetPositionY());

        Cell cell(p);
        cell.SetNoCreate();

        Trinity::LocalizedPacketDo<Trinity::AchievementChatBuilder> say_do(say_builder);
        Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::AchievementChatBuilder> > say_worker(referencePlayer, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), say_do);
        TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::AchievementChatBuilder> >, WorldTypeMapContainer > message(say_worker);
        cell.Visit(p, message, *referencePlayer->GetMap(), *referencePlayer, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY));
    }

    ObjectGuid guid = referencePlayer->GetGUID();
    ObjectGuid guid2 = referencePlayer->GetGUID();

    WorldPacket data(SMSG_ACHIEVEMENT_EARNED, 8+4+8);
    data.WriteBit(guid2[6]);
    data.WriteBit(guid2[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(0);  // does not notify player ingame
    data.WriteBit(guid2[7]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid2[0]);
    data.WriteBit(guid2[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid2[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid2[5]);

    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid2[6]);
    data.WriteByteSeq(guid[6]);
    data.AppendPackedTime(time(NULL));
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid2[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid2[7]);
    data << uint32(achievement->ID);
    data.WriteByteSeq(guid2[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid2[0]);
    data.WriteByteSeq(guid[5]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(guid[5]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(guid2[2]);
    referencePlayer->SendMessageToSetInRange(&data, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY), true);

    referencePlayer->SetSaveTimer(1);
}

void GuildAchievementMgr::SendAchievementEarned(AchievementEntry const* achievement) const
{
    ObjectGuid guid = GetOwner()->GetGUID();

    WorldPacket data(SMSG_GUILD_ACHIEVEMENT_EARNED, 8+4+8);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[7]);
    data.AppendPackedTime(time(NULL));
    data << uint32(achievement->ID);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);

    SendPacket(&data);
}

void PlayerAchievementMgr::SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const
{
    ObjectGuid guid = GetOwner()->GetGUID();

    WorldPacket data(SMSG_CRITERIA_UPDATE, 8 + 4 + 8);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[2]);

    data << uint32(entry->ID);

    if (!entry->TimeLimit)
        data << uint32(0);
    else
        data << uint32(timedCompleted ? 0 : 1); // this are some flags, 1 is for keeping the counter at 0 in client

    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[1]);

    data.AppendPackedTime(progress->Date);

    data.WriteByteSeq(guid[4]);

    data << uint32(timeElapsed);    // time elapsed in seconds
    data << uint32(0);              // unk

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);

    data << uint64(progress->Counter);
    SendPacket(&data);
}

void GuildAchievementMgr::SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32, bool) const
{
    // will send response to criteria progress request
    WorldPacket data(SMSG_GUILD_CRITERIA_DATA, 3 + 1 + 1 + 8 + 8 + 4 + 4 + 4 + 4 + 4);

    ObjectGuid counter(progress->Counter); // for accessing every byte individually
    ObjectGuid guid = progress->CompletedGUID;

    data.WriteBits(1, 19);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(counter[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(counter[2]);
    data.WriteBit(counter[5]);
    data.WriteBit(counter[1]);
    data.WriteBit(counter[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(counter[7]);
    data.WriteBit(counter[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(counter[3]);
    data.WriteBit(guid[5]);

    data.FlushBits();

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(counter[5]);
    data.WriteByteSeq(guid[6]);
    data << uint32(entry->ID);
    data.WriteByteSeq(counter[6]);
    data << uint32(progress->Date);      // unknown date
    data << uint32(progress->Date);      // last update time (not packed!)
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(counter[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(counter[4]);
    data.WriteByteSeq(counter[2]);
    data.WriteByteSeq(counter[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(counter[1]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data << uint32(0);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(counter[7]);
    data << uint32(progress->Date);      // unknown date

    GetOwner()->BroadcastPacketIfTrackingAchievement(&data, entry->ID);
}

void AccountAchievementMgr::SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const
{
    WorldPacket data(SMSG_ACCOUNT_CRITERIA_UPDATE, 4 + 4 + 4 + 4 + 8 + 4);
    ObjectGuid guid = GetOwner()->GetGUID();
    ObjectGuid counter(progress->Counter);

    data.WriteBit(counter[4]);
    data.WriteBit(guid[2]);
    data.WriteBit(counter[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(counter[0]);
    data.WriteBit(counter[5]);
    data.WriteBit(guid[3]);
    data.WriteBit(counter[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(counter[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[7]);
    data.WriteBit(counter[1]);

    data.WriteBits(0, 4);

    data.WriteBit(guid[5]);
    data.WriteBit(counter[7]);
    data.WriteBit(guid[0]);

    data.WriteByteSeq(guid[7]);

    data << uint32(timeElapsed);
    data << uint32(entry->ID);

    data.WriteByteSeq(counter[7]);

    data << uint32(timeElapsed);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);

    data.AppendPackedTime(progress->Date);

    data.WriteByteSeq(counter[0]);
    data.WriteByteSeq(counter[1]);
    data.WriteByteSeq(counter[2]);
    data.WriteByteSeq(counter[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(counter[4]);
    data.WriteByteSeq(counter[5]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(counter[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);
    SendPacket(&data);
}

void GuildAchievementMgr::SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const
{
    ObjectGuid guid;
    uint32 trackedCriteriasCount = 0;

    for (std::set<uint32>::iterator itr = trackedCriterias.begin(); itr != trackedCriterias.end(); ++itr)
    {
        CriteriaEntry const* entry = sAchievementMgr->GetAchievementCriteria(*itr);

        CriteriaProgressMap::const_iterator progress = m_criteriaProgress.find(entry->ID);
        if (progress == m_criteriaProgress.end())
            continue;

        ++trackedCriteriasCount;
    }

    ByteBuffer criteriaBits, criteriaData;

    for (std::set<uint32>::iterator itr = trackedCriterias.begin(); itr != trackedCriterias.end(); ++itr)
    {
        CriteriaEntry const* entry = sAchievementMgr->GetAchievementCriteria(*itr);

        CriteriaProgressMap::const_iterator progress = m_criteriaProgress.find(entry->ID);
        if (progress == m_criteriaProgress.end())
            continue;

        ObjectGuid counter(progress->second.Counter);
        guid = progress->second.CompletedGUID;

        criteriaBits.WriteBit(guid[2]);
        criteriaBits.WriteBit(guid[4]);
        criteriaBits.WriteBit(guid[6]);
        criteriaBits.WriteBit(counter[0]);
        criteriaBits.WriteBit(guid[7]);
        criteriaBits.WriteBit(counter[2]);
        criteriaBits.WriteBit(counter[5]);
        criteriaBits.WriteBit(counter[1]);
        criteriaBits.WriteBit(counter[4]);
        criteriaBits.WriteBit(guid[0]);
        criteriaBits.WriteBit(guid[3]);
        criteriaBits.WriteBit(counter[7]);
        criteriaBits.WriteBit(counter[6]);
        criteriaBits.WriteBit(guid[1]);
        criteriaBits.WriteBit(counter[3]);
        criteriaBits.WriteBit(guid[5]);

        criteriaData.WriteByteSeq(guid[1]);
        criteriaData.WriteByteSeq(counter[5]);
        criteriaData.WriteByteSeq(guid[6]);
        criteriaData << uint32(entry->ID);
        criteriaData.WriteByteSeq(counter[6]);
        criteriaData << uint32(progress->second.Date);      // unknown date
        criteriaData << uint32(progress->second.Date);      // last update time (not packed!)
        criteriaData.WriteByteSeq(guid[3]);
        criteriaData.WriteByteSeq(counter[0]);
        criteriaData.WriteByteSeq(guid[7]);
        criteriaData.WriteByteSeq(counter[4]);
        criteriaData.WriteByteSeq(counter[2]);
        criteriaData.WriteByteSeq(counter[3]);
        criteriaData.WriteByteSeq(guid[4]);
        criteriaData.WriteByteSeq(counter[1]);
        criteriaData.WriteByteSeq(guid[5]);
        criteriaData.WriteByteSeq(guid[0]);
        criteriaData << uint32(0);
        criteriaData.WriteByteSeq(guid[2]);
        criteriaData.WriteByteSeq(counter[7]);
        criteriaData << uint32(progress->second.Date);      // unknown date
    }

    WorldPacket data(SMSG_GUILD_CRITERIA_DATA, criteriaBits.size() + criteriaData.size());
    data.WriteBits(trackedCriteriasCount, 19);
    if (trackedCriteriasCount)
        data.append(criteriaBits);

    data.FlushBits();

    if (trackedCriteriasCount)
        data.append(criteriaData);

    receiver->GetSession()->SendPacket(&data);
}

/**
 * called at player login. The player might have fulfilled some achievements when the achievement system wasn't working yet
 */
void AchievementMgr::CheckAllAchievementCriteria(Player* referencePlayer)
{
    // suppress sending packets
    for (uint32 i=0; i<ACHIEVEMENT_CRITERIA_TYPE_TOTAL; ++i)
        UpdateAchievementCriteria(AchievementCriteriaTypes(i), 0, 0, 0, nullptr, referencePlayer);
}

static const uint32 achievIdByArenaSlot[3] = {1057, 1107, 1108};
static const uint32 achievIdForDungeon[][4] =
{
    // ach_cr_id, is_dungeon, is_raid, is_heroic_dungeon
    { 321,       true,      true,   true  },
    { 916,       false,     true,   false },
    { 917,       false,     true,   false },
    { 918,       true,      false,  false },
    { 2219,      false,     false,  true  },
    { 0,         false,     false,  false }
};

/**
 * this function will be called whenever the user might have done a criteria relevant action
 */
void AchievementMgr::UpdateAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer)
{

    if (type >= ACHIEVEMENT_CRITERIA_TYPE_TOTAL)
    {
        TC_LOG_ERROR("achievement", "UpdateAchievementCriteria: Wrong criteria type %u", type);
        return;
    }

    if (!referencePlayer)
    {
        TC_LOG_DEBUG("achievement", "UpdateAchievementCriteria: Player is NULL! Cant update criteria");
        return;
    }

    // disable for gamemasters with GM-mode enabled
    if (referencePlayer->IsGameMaster())
    {
        TC_LOG_DEBUG("achievement", "UpdateAchievementCriteria: [Player %s GM mode on] %s, %s (%u), " UI64FMTD ", " UI64FMTD ", " UI64FMTD,
            referencePlayer->GetName().c_str(), GetGUID().GetTypeName(), AchievementGlobalMgr::GetCriteriaTypeString(type), type, miscValue1, miscValue2, miscValue3);
        return;
    }

    TC_LOG_DEBUG("achievement", "UpdateAchievementCriteria: %s, %s (%u), " UI64FMTD ", " UI64FMTD ", " UI64FMTD,
        referencePlayer->GetGUID().GetTypeName(), AchievementGlobalMgr::GetCriteriaTypeString(type), type, miscValue1, miscValue2, miscValue3);

    // Lua_GetGuildLevelEnabled() is checked in achievement UI to display guild tab
    if (m_type == AchievementType::Guild && !sWorld->getBoolConfig(CONFIG_GUILD_LEVELING_ENABLED))
        return;

    auto criterias = sAchievementMgr->GetAchievementCriteriaByType(type, m_type);
    for (auto&& achievementCriteria : criterias)
    {
        CriteriaTreeList const* trees = sAchievementMgr->GetCriteriaTreesByCriteria(achievementCriteria->Entry->ID, m_type);
        if (!trees)
            continue;

        if (!CanUpdateCriteria(achievementCriteria, trees, miscValue1, miscValue2, miscValue3, unit, referencePlayer))
            continue;

        // requirements not found in the dbc
        if (AchievementCriteriaDataSet const* data = sAchievementMgr->GetCriteriaDataSet(achievementCriteria->Entry))
            if (!data->Meets(referencePlayer, unit, miscValue1))
                continue;

        switch (type)
        {
            // std. case: increment at 1
            case ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
            case ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL:
            case ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION:
            case ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS:    /* FIXME: for online player only currently */
            case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED:
            case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED:
            case ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED:
            case ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
            case ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
            case ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_DEATH:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
            case ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP:
            case ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON:
            case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE:
            case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER:
            case ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM:
            case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
            case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
            case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL:
            case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
            case ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
            case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
            case ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE:
            case ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT:
            case ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL:
            case ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS:
            case ACHIEVEMENT_CRITERIA_TYPE_HK_RACE:
            case ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL:
            case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL:
            case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
            case ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL:
            case ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS:
            case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA: // This also behaves like ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA
            case ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE_GUILD:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
            case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST:
            case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_PET_BATTLE:
            case ACHIEVEMENT_CRITERIA_TYPE_REACH_BATTLE_PET_LEVEL:
            case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET2:
                SetCriteriaProgress(achievementCriteria, 1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            // std case: increment at miscValue1
            case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_VENDORS:
            case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
            case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
            case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
            case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
            case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
            case ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY:
            case ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:/* FIXME: for online player only currently */
            case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
            case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
            case ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
            case ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE:
            case ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE:
            case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
            case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_TABARD:
            case ACHIEVEMENT_CRITERIA_TYPE_CATCH_FROM_POOL:
                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_SET);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
            case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE:
            case ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE:
            case ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_GATHER_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_CURRENCY:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            case ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
                SetCriteriaProgress(achievementCriteria, miscValue2, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            // std case: high value at miscValue1
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID:
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD: /* FIXME: for online player only currently */
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT:
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED:
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
            case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
            case ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
            case ACHIEVEMENT_CRITERIA_TYPE_REACH_BG_RATING:
                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_HIGHEST);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetLevel(), referencePlayer, PROGRESS_HIGHEST);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL:
                if (uint32 skillvalue = referencePlayer->GetBaseSkillValue(achievementCriteria->Entry->Asset.SkillID))
                    SetCriteriaProgress(achievementCriteria, skillvalue, referencePlayer, PROGRESS_HIGHEST);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL:
                if (uint32 maxSkillvalue = referencePlayer->GetPureMaxSkillValue(achievementCriteria->Entry->Asset.SkillID))
                    SetCriteriaProgress(achievementCriteria, maxSkillvalue, referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetRewardedQuestCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
            {
                time_t nextDailyResetTime = sWorld->GetNextDailyQuestsResetTime();
                CriteriaProgress *progress = GetCriteriaProgress(achievementCriteria->Entry);

                if (!miscValue1) // Login case.
                {
                    // reset if player missed one day.
                    if (progress && progress->Date < (nextDailyResetTime - 2 * DAY))
                        SetCriteriaProgress(achievementCriteria, 0, referencePlayer, PROGRESS_SET);
                    continue;
                }

                ProgressType progressType;
                if (!progress)
                    // 1st time. Start count.
                    progressType = PROGRESS_SET;
                else if (progress->Date < (nextDailyResetTime - 2 * DAY))
                    // last progress is older than 2 days. Player missed 1 day => Restart count.
                    progressType = PROGRESS_SET;
                else if (progress->Date < (nextDailyResetTime - DAY))
                    // last progress is between 1 and 2 days. => 1st time of the day.
                    progressType = PROGRESS_ACCUMULATE;
                else
                    // last progress is within the day before the reset => Already counted today.
                    continue;

                SetCriteriaProgress(achievementCriteria, 1, referencePlayer, progressType);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            {
                uint32 counter = 0;

                const RewardedQuestSet &rewQuests = referencePlayer->getRewardedQuests();
                for (RewardedQuestSet::const_iterator itr = rewQuests.begin(); itr != rewQuests.end(); ++itr)
                {
                    Quest const* quest = sObjectMgr->GetQuestTemplate(*itr);
                    if (quest && quest->GetZoneOrSort() >= 0 && uint32(quest->GetZoneOrSort()) == achievementCriteria->Entry->Asset.ZoneID)
                        ++counter;
                }
                SetCriteriaProgress(achievementCriteria, counter, referencePlayer);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING:
                // miscValue1 is the ingame fallheight*100 as stored in dbc
                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL:
            case ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA:
            case ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP:
            case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM:
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET:
            case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET_SPECIES:
                SetCriteriaProgress(achievementCriteria, 1, referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetBankBagSlotCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION:
            {
                int32 reputation = referencePlayer->GetReputationMgr().GetReputation(achievementCriteria->Entry->Asset.FactionID);
                if (reputation > 0)
                    SetCriteriaProgress(achievementCriteria, reputation, referencePlayer, PROGRESS_HIGHEST); // Idk, 114 achievements but seems legit
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetReputationMgr().GetExaltedFactionCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
            {
                uint32 spellCount = 0;
                for (PlayerSpellMap::const_iterator spellIter = referencePlayer->GetSpellMap().begin();
                    spellIter != referencePlayer->GetSpellMap().end();
                    ++spellIter)
                {
                    SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellIter->first);
                    for (SkillLineAbilityMap::const_iterator skillIter = bounds.first; skillIter != bounds.second; ++skillIter)
                    {
                        if (skillIter->second->skillId == achievementCriteria->Entry->Asset.SkillLine)
                            spellCount++;
                    }
                }
                SetCriteriaProgress(achievementCriteria, spellCount, referencePlayer);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetReputationMgr().GetReveredFactionCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetReputationMgr().GetHonoredFactionCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetReputationMgr().GetVisibleFactionCount(), referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE:
            {
                uint32 spellCount = 0;
                for (PlayerSpellMap::const_iterator spellIter = referencePlayer->GetSpellMap().begin();
                    spellIter != referencePlayer->GetSpellMap().end();
                    ++spellIter)
                {
                    SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellIter->first);
                    for (SkillLineAbilityMap::const_iterator skillIter = bounds.first; skillIter != bounds.second; ++skillIter)
                        if (skillIter->second->skillId == achievementCriteria->Entry->Asset.SkillLine)
                            spellCount++;
                }
                SetCriteriaProgress(achievementCriteria, spellCount, referencePlayer);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
                SetCriteriaProgress(achievementCriteria, referencePlayer->GetMoney(), referencePlayer, PROGRESS_HIGHEST);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
                SetCriteriaProgress(achievementCriteria, GetAchievementPoints(), referencePlayer, PROGRESS_SET);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING:
            {
                uint32 reqTeamType = achievementCriteria->Entry->Asset.TeamType;

                if (miscValue1)
                {
                    if (miscValue2 != reqTeamType)
                        continue;

                    SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_HIGHEST);
                }
                else // login case
                {
                    /*for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
                    {
                        uint32 teamId = referencePlayer->GetArenaTeamId(arena_slot);
                        if (!teamId)
                            continue;

                        ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(teamId);
                        if (!team || team->GetType() != reqTeamType)
                            continue;

                        SetCriteriaProgress(achievementCriteria, team->GetStats().Rating, referencePlayer, PROGRESS_HIGHEST);
                        break;
                    }*/
                }

                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
            {
                uint32 reqTeamType = achievementCriteria->Entry->Asset.TeamType;

                if (miscValue1)
                {
                    if (miscValue2 != reqTeamType)
                        continue;

                    SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_HIGHEST);
                }
                else // login case
                {
                    /*for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
                    {
                        uint32 teamId = referencePlayer->GetArenaTeamId(arena_slot);
                        if (!teamId)
                            continue;

                        ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(teamId);
                        if (!team || team->GetType() != reqTeamType)
                            continue;

                        if (ArenaTeamMember const* member = team->GetMember(referencePlayer->GetGUID()))
                        {
                            SetCriteriaProgress(achievementCriteria, member->PersonalRating, referencePlayer, PROGRESS_HIGHEST);
                            break;
                        }
                    }*/
                }
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL:
                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS:
            {
                if (!miscValue1)
                    continue;

                SetCriteriaProgress(achievementCriteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_COOK_RECIPES_GUILD:
            case ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD:
            {
                if (!miscValue1 || !miscValue2)
                    continue;

                SetCriteriaProgress(achievementCriteria, miscValue2, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            }
            case ACHIEVEMENT_CRITERIA_TYPE_PET_BATTLE_ACHIEVEMENT_POINTS:
            {
                uint32 points = 0;
                for (auto&& itr : m_completedAchievements)
                    if (auto achievement = sAchievementStore.LookupEntry(itr.first))
                        if (achievement->CategoryId == 15118 || achievement->CategoryId == 15119 || achievement->CategoryId == 15120)
                            points += achievement->Points;

                SetCriteriaProgress(achievementCriteria, points, referencePlayer, PROGRESS_SET);
                break;
            }
            // FIXME: not triggered in code as result, need to implement
            case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID:
            case ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA:
            case ACHIEVEMENT_CRITERIA_TYPE_OWN_RANK:
                break;                                   // Not implemented yet :(
        }

        for (CriteriaTreeNode const* tree : *trees)
        {
            if (IsCompletedCriteriaTree(tree, referencePlayer))
                CompletedCriteriaTree(tree, referencePlayer);

            AchievementEntry const* achievement = tree->Achievement;
            if (!achievement)
                continue;

            // check again the completeness for SUMM and REQ COUNT achievements,
            // as they don't depend on the completed criteria but on the sum of the progress of each individual criteria
            if (achievement->Flags & ACHIEVEMENT_FLAG_SUMM)
                if (IsCompletedAchievement(achievement, referencePlayer))
                    CompletedAchievement(achievement, referencePlayer);

            if (std::vector<AchievementEntry const*> const* achRefList = sAchievementMgr->GetAchievementByReferencedId(achievement->ID))
                for (AchievementEntry const* refAchievement : *achRefList)
                    if (IsCompletedAchievement(refAchievement, referencePlayer))
                        CompletedAchievement(refAchievement, referencePlayer);
        }
    }

    if (m_type == AchievementType::Player)
        referencePlayer->GetSession()->GetAchievementMgr().UpdateAchievementCriteria(type, miscValue1, miscValue2, miscValue3, unit, referencePlayer);
}

bool AchievementMgr::IsCompletedCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer)
{
    if (!CanCompleteCriteriaTree(tree, referencePlayer))
        return false;

    uint64 requiredCount = tree->Entry->RequieredCount;
    switch (tree->Entry->Operator)
    {
        case CRITERIA_TREE_OPERATOR_SINGLE:
            return tree->Criteria && IsCompletedCriteria(tree->Criteria, requiredCount);
        case CRITERIA_TREE_OPERATOR_SINGLE_NOT_COMPLETED:
            return !tree->Criteria || !IsCompletedCriteria(tree->Criteria, requiredCount);
        case CRITERIA_TREE_OPERATOR_ALL:
            for (CriteriaTreeNode const* node : tree->Nodes)
                if (!IsCompletedCriteriaTree(node, referencePlayer))
                    return false;
            return true;
        case CRITERIA_TREE_OPERAROR_SUM_CHILDREN:
        {
            uint64 progress = 0;
            WalkCriteriaTree(tree, [this, &progress](CriteriaTreeNode const* criteriaTree)
            {
                if (criteriaTree->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria->Entry))
                        progress += criteriaProgress->Counter;
            });
            return progress >= requiredCount;
        }
        case CRITERIA_TREE_OPERATOR_MAX_CHILD:
        {
            uint64 progress = 0;
            WalkCriteriaTree(tree, [this, &progress](CriteriaTreeNode const* criteriaTree)
            {
                if (criteriaTree->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria->Entry))
                        if (criteriaProgress->Counter > progress)
                            progress = criteriaProgress->Counter;
            });
            return progress >= requiredCount;
        }
        case CRITERIA_TREE_OPERATOR_COUNT_DIRECT_CHILDREN:
        {
            uint64 progress = 0;
            for (CriteriaTreeNode const* node : tree->Nodes)
                if (node->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(node->Criteria->Entry))
                        if (criteriaProgress->Counter >= 1)
                            if (++progress >= requiredCount)
                                return true;

            return false;
        }
        case CRITERIA_TREE_OPERATOR_ANY:
        {
            uint64 progress = 0;
            for (CriteriaTreeNode const* node : tree->Nodes)
                if (IsCompletedCriteriaTree(node, referencePlayer))
                    if (++progress >= requiredCount)
                        return true;

            return false;
        }
        default:
            break;
    }

    return false;
}

bool AchievementMgr::CanCompleteCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return false;

    // counter can never complete
    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return false;

    ASSERT(IsFitTypeCondition(achievement));

    if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
    {
        // someone on this realm has already completed that achievement
        if (sAchievementMgr->IsRealmCompleted(achievement))
            return false;
    }

    return CanUpdateCriteriaTree(tree, referencePlayer);
}

bool AchievementMgr::IsCompletedCriteria(Criteria const* criteria, uint64 requiredAmount)
{
    CriteriaProgress const* progress = GetCriteriaProgress(criteria->Entry);
    if (!progress)
        return false;

    switch (AchievementCriteriaTypes(criteria->Entry->Type))
    {
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
        case ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE:
        case ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
        case ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL:
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2:
        case ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
        case ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP:
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        case ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS:
        case ACHIEVEMENT_CRITERIA_TYPE_HK_RACE:
        case ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE:
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY:
        case ACHIEVEMENT_CRITERIA_TYPE_GATHER_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT:
        case ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL:
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE:
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE:
        case ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
        case ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS:
        case ACHIEVEMENT_CRITERIA_TYPE_CURRENCY:
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_BG_RATING:
        case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET:
        case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET_SPECIES:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
        case ACHIEVEMENT_CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS:
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
        case ACHIEVEMENT_CRITERIA_TYPE_COOK_RECIPES_GUILD:
        case ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD:
        case ACHIEVEMENT_CRITERIA_TYPE_CATCH_FROM_POOL:
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST:
        case ACHIEVEMENT_CRITERIA_TYPE_PET_BATTLE_ACHIEVEMENT_POINTS:
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_PET_BATTLE:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_BATTLE_PET_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET2:
            return progress->Counter >= requiredAmount;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL:
        case ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON:
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_TABARD:
            return progress->Counter >= 1;
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            return progress->Counter >= (requiredAmount * 75);
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA:
            return requiredAmount && progress->Counter >= requiredAmount;
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:     // Ruled by the ModifierEntry
        case ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN:
            return true;
        // handle all statistic-only criteria here
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP:
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH:
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON:
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE:
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER:
        case ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING:
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_VENDORS:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
        case ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
        case ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL:
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
        case ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED:
        case ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED:
        case ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
        case ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
        default:
            return false;
    }
    return false;
}

void AchievementMgr::CompletedCriteriaFor(AchievementEntry const* achievement, Player* referencePlayer)
{
    // counter can never complete
    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return;

    // already completed and stored
    if (HasAchieved(achievement->ID))
        return;

    if (IsCompletedAchievement(achievement, referencePlayer))
        CompletedAchievement(achievement, referencePlayer);
}

bool AchievementMgr::IsCompletedAchievement(AchievementEntry const* entry, Player const* referencePlayer)
{
    // counter can never complete
    if (entry->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return false;

    // Player only achievements can have reference to the account-wide achievement, so we get crash when updates criteria.
    // Assert moved to AchievementMgr::CompletedAchievement
    if (!IsFitTypeCondition(entry))
        return false;

    CriteriaTreeNode const* tree = sAchievementMgr->GetCriteriaTree(entry->CriteriaTree);
    if (!tree)
        return false;

    // For SUMM achievements, we have to count the progress of each criteria of the achievement.
    // Oddly, the target count is NOT contained in the achievement, but in each individual criteria
    if (entry->Flags & ACHIEVEMENT_FLAG_SUMM)
    {
        uint32 progress = 0;
        WalkCriteriaTree(tree, [this, &progress](CriteriaTreeNode const* criteriaTree)
        {
            if (criteriaTree->Criteria)
                if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria->Entry))
                    progress += criteriaProgress->Counter;
        });
        return progress >= tree->Entry->RequieredCount;
    }

    return IsCompletedCriteriaTree(tree, referencePlayer);
}

bool AchievementMgr::IsFitTypeCondition(AchievementEntry const* entry)
{
    switch (m_type)
    {
        case AchievementType::Player:
            return !(entry->Flags & (ACHIEVEMENT_FLAG_GUILD | ACHIEVEMENT_FLAG_ACCOUNT));
        case AchievementType::Account:
            return entry->Flags & ACHIEVEMENT_FLAG_ACCOUNT;
        case AchievementType::Guild:
            return entry->Flags & ACHIEVEMENT_FLAG_GUILD;
        default:
            std::abort();
    }
}

bool AchievementMgr::CanUpdateCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return false;

    ASSERT(IsFitTypeCondition(achievement));

    if (HasAchieved(achievement->ID))
        return false;

    if (achievement->MapID != -1 && referencePlayer->GetMapId() != uint32(achievement->MapID))
        return false;

    if ((achievement->RequiredFaction == ACHIEVEMENT_FACTION_HORDE    && referencePlayer->GetTeam() != HORDE) ||
        (achievement->RequiredFaction == ACHIEVEMENT_FACTION_ALLIANCE && referencePlayer->GetTeam() != ALLIANCE))
        return false;

    if ((tree->Entry->Flags & CRITERIA_TREE_FLAG_HORDE_ONLY && referencePlayer->GetTeam() != HORDE) ||
        (tree->Entry->Flags & CRITERIA_TREE_FLAG_ALLIANCE_ONLY && referencePlayer->GetTeam() != ALLIANCE))
        return false;

    return true;
}

CriteriaProgress* AchievementMgr::GetCriteriaProgress(CriteriaEntry const* entry)
{
    CriteriaProgressMap::iterator iter = m_criteriaProgress.find(entry->ID);

    if (iter == m_criteriaProgress.end())
        return NULL;

    return &(iter->second);
}

void AchievementMgr::SetCriteriaProgress(Criteria const* criteria, uint64 changeValue, Player* referencePlayer, ProgressType ptype)
{
    TC_LOG_DEBUG("achievement", "SetCriteriaProgress(%u, " UI64FMTD ") for (%s GUID: %u)",
                   criteria->Entry->ID, changeValue, GetGUID().GetTypeName(), GetGUID().GetCounter());

    CriteriaProgress* progress = GetCriteriaProgress(criteria->Entry);
    if (!progress)
    {
        // not create record for 0 counter but allow it for timed achievements
        // we will need to send 0 progress to client to start the timer
        if (changeValue == 0 && !criteria->Entry->TimeLimit)
            return;

        progress = &m_criteriaProgress[criteria->Entry->ID];
        progress->Counter = changeValue;
    }
    else
    {
        uint64 newValue = 0;
        switch (ptype)
        {
            case PROGRESS_SET:
                newValue = changeValue;
                break;
            case PROGRESS_ACCUMULATE:
            {
                // avoid overflow
                uint64 max_value = std::numeric_limits<uint64>::max();
                newValue = max_value - progress->Counter > changeValue ? progress->Counter + changeValue : max_value;
                break;
            }
            case PROGRESS_HIGHEST:
                newValue = progress->Counter < changeValue ? changeValue : progress->Counter;
                break;
        }

        // not update (not mark as changed) if counter will have same value
        if (progress->Counter == newValue && !criteria->Entry->TimeLimit)
            return;

        progress->Counter = newValue;
    }

    progress->Changed = true;
    progress->Date = time(NULL); // set the date to the latest update.
    progress->CompletedGUID = referencePlayer ? referencePlayer->GetGUID() : ObjectGuid::Empty;
    uint32 timeElapsed = 0; // @todo : Fix me

    if (criteria->Entry->TimeLimit)
    {
        // Client expects this in packet
        auto itr = m_additionalCriteriaProgress.find(criteria->Entry->ID);
        if (itr != m_additionalCriteriaProgress.end())
            timeElapsed = criteria->Entry->TimeLimit - (itr->second.Timer / IN_MILLISECONDS);
    }

    SendCriteriaUpdate(criteria->Entry, progress, timeElapsed, false);
}

void AchievementMgr::CompletedCriteriaTree(CriteriaTreeNode const* tree, Player* referencePlayer)
{
    AchievementEntry const* achievement = tree->Achievement;
    if (!achievement)
        return;

    // counter can never complete
    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
        return;

    // already completed and stored
    if (HasAchieved(achievement->ID))
        return;

    for (auto itr = m_additionalCriteriaProgress.begin(); itr != m_additionalCriteriaProgress.end();)
    {
        if (itr->second.Root == tree)
            m_additionalCriteriaProgress.erase(itr++);
        else
            ++itr;
    }

    if (IsCompletedAchievement(achievement, referencePlayer))
        CompletedAchievement(achievement, referencePlayer);
}

void PlayerAchievementMgrBase::UpdateTimedCriteria(uint32 timeDiff)
{
    for (auto it = m_additionalCriteriaProgress.begin(); it != m_additionalCriteriaProgress.end();)
    {
        if (it->second.Criteria->TimeLimit)
        {
            // Time is up, remove timer and reset progress
            if (it->second.Timer <= timeDiff)
            {
                if (Criteria const* criteria = sAchievementMgr->GetCriteria(it->first))
                    SetCriteriaProgress(criteria, 0, GetOwner(), PROGRESS_SET);

                it = m_additionalCriteriaProgress.erase(it);

            }
            else
            {
                it->second.Timer -= timeDiff;
                ++it;
            }
        }
        else
            ++it;
    }
}

// FIXME
static bool NeedToStartNotTimedCriteria(CriteriaEntry const* criteria)
{
    if (criteria->StartEvent == CRITERIA_START_TYPE_EVENT)
    {
        if (criteria->StartAsset == 28223)  // It's Not Easy Being Green
        {
            return true;
        }
    }
    return false;
}

void PlayerAchievementMgrBase::StartCriteria(CriteriaStartTypes type, uint32 entry, uint32 timeLost)
{
    for (auto&& criteria : sAchievementMgr->GetCriteriaByStartType(type))
    {
        if (criteria->Entry->StartAsset != entry)
            continue;

        CriteriaTreeList const* trees = sAchievementMgr->GetCriteriaTreesByCriteria(criteria->Entry->ID, m_type);
        if (!trees)
            continue;

        // Check existence only for timers. Reset non timed anyway, because it's pointless to do it manully
        if (criteria->Entry->TimeLimit && m_additionalCriteriaProgress.find(criteria->Entry->ID) != m_additionalCriteriaProgress.end())
            continue;

        for (CriteriaTreeNode const* tree : *trees)
        {
            // Yes, this is needed. Criterias with timer may be in different trees.
            if (!IsCompletedCriteriaTree(tree, GetOwner()))
            {
                // Start the timer
                if ((criteria->Entry->TimeLimit && criteria->Entry->TimeLimit * uint32(IN_MILLISECONDS) > timeLost) ||
                    (!criteria->Entry->TimeLimit && NeedToStartNotTimedCriteria(criteria->Entry)))
                {
                    auto& progress = m_additionalCriteriaProgress[criteria->Entry->ID];
                    progress.Criteria = criteria->Entry;
                    progress.Root = tree;
                    progress.Timer = criteria->Entry->TimeLimit ? criteria->Entry->TimeLimit * IN_MILLISECONDS - timeLost : 0;
                    SetCriteriaProgress(criteria, 0, GetOwner(), PROGRESS_SET);
                    break;
                }
            }
        }
    }
}

void PlayerAchievementMgrBase::RemoveCriteriaTimer(CriteriaStartTypes type, uint32 entry)
{
    CriteriaList const& criteriaList = sAchievementMgr->GetTimedCriteriaByType(type);
    for (Criteria const* criteria : criteriaList)
    {
        if (criteria->Entry->StartAsset != entry)
            continue;

        m_additionalCriteriaProgress.erase(criteria->Entry->ID);

        // remove progress
        SetCriteriaProgress(criteria, 0, GetOwner(), PROGRESS_SET);
    }
}

void AchievementMgr::CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer)
{
    ASSERT(IsFitTypeCondition(achievement));

    switch (achievement->ID)
    {
        case 6954: // Ahead of the Curve: Will of the Emperor
        case 7485: // Cutting Edge: Will of the Emperor
        case 8246: // Ahead of the Curve: Grand Empress Shek'zeer
        case 7486: // Cutting Edge: Grand Empress Shek'zeer
        case 8248: // Ahead of the Curve: Sha of Fear
        case 7487: // Cutting Edge: Sha of Fear
        case 8249: // Ahead of the Curve: Lei Shen
        case 8238: // Cutting Edge: Lei Shen
        case 8260: // Cutting Edge: Ra-den
            return;
    }

    // disable for gamemasters with GM-mode enabled
    if (referencePlayer->IsGameMaster())
        return;

    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER || HasAchieved(achievement->ID))
        return;

    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_NEWS)
        if (Guild* guild = referencePlayer->GetGuild())
            guild->AddGuildNews(GUILD_NEWS_PLAYER_ACHIEVEMENT, referencePlayer->GetGUID(), achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_HEADER, achievement->ID);

    if (!referencePlayer->GetSession()->PlayerLoading())
        SendAchievementEarned(achievement, referencePlayer);

    TC_LOG_INFO("achievement", "AchievementMgr::CompletedAchievement(%u). Player: %s (%u)",
        achievement->ID, referencePlayer->GetName().c_str(), referencePlayer->GetGUID().GetCounter());

    CompletedAchievementData& ca = m_completedAchievements[achievement->ID];
    ca.Date = time(nullptr);
    ca.Changed = true;
    ca.CompletedBy = referencePlayer->GetGUID().GetCounter();

    // Okay, for kill we must do it after all the group members get achievement, so just delay it (and don't care for multiple triggering)
    if (achievement->Flags & ACHIEVEMENT_FLAG_REALM_FIRST_KILL)
        referencePlayer->Schedule(Seconds(1), [=]() { sAchievementMgr->SetRealmCompleted(achievement); });
    else
        sAchievementMgr->SetRealmCompleted(achievement);

    m_achievementPoints += achievement->Points;

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT, 0, 0, 0, NULL, referencePlayer);
    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS, 0, 0, 0, NULL, referencePlayer);
    if (achievement->CategoryId == 15118 || achievement->CategoryId == 15119 || achievement->CategoryId == 15120)
        UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_PET_BATTLE_ACHIEVEMENT_POINTS, 0, 0, 0, nullptr, referencePlayer);

    if (m_type == AchievementType::Player)
    {
        auto& accMgr = referencePlayer->GetSession()->GetAchievementMgr();
        if (accMgr.m_completedAchievements.find(achievement->ID) == accMgr.m_completedAchievements.end())
        {
            accMgr.m_completedAchievements.emplace(achievement->ID, ca);
            accMgr.m_achievementPoints += achievement->Points;
        }
    }

    // FIXME: A Test of Valor and similar quest (32474, 32476)
    if (achievement->ID == 8030 || achievement->ID == 8031)
        referencePlayer->KilledMonsterCredit(69145);

    // reward items and titles if any
    AchievementReward const* reward = sAchievementMgr->GetAchievementReward(achievement);

    // no rewards
    if (!reward)
        return;

    // titles
    //! Currently there's only one achievement that deals with gender-specific titles.
    //! Since no common attributes were found, (not even in titleRewardFlags field)
    //! we explicitly check by ID. Maybe in the future we could move the achievement_reward
    //! condition fields to the condition system.
    uint32 index;
    if (achievement->ID == 1793 || achievement->ID == 8397)
        index = referencePlayer->GetGender() == GENDER_MALE ? 0 : 1;
    else
        index = referencePlayer->GetTeam() == ALLIANCE ? 0 : 1;
    if (uint32 titleId = reward->TitleId[index])
        if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(titleId))
            referencePlayer->SetTitle(titleEntry);

    if (reward->SpellId > 0)
        referencePlayer->CastSpell(referencePlayer, reward->SpellId);
    else if (reward->SpellId < 0)
        referencePlayer->LearnSpell(uint32(-reward->SpellId), false);

    // mail
    if (reward->Sender)
    {
        Item* item = reward->ItemId ? Item::CreateItem(reward->ItemId, 1, referencePlayer) : NULL;

        // subject and text
        std::string subject = reward->Subject;
        std::string text = reward->Text;
        LocaleConstant localeConstant = referencePlayer->GetSession()->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
        {
            if (AchievementRewardLocale const* loc = sAchievementMgr->GetAchievementRewardLocale(achievement))
            {
                ObjectMgr::GetLocaleString(loc->Subject, localeConstant, subject);
                ObjectMgr::GetLocaleString(loc->Text, localeConstant, text);
            }
        }

        MailDraft draft(subject, text);

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        if (item)
        {
            // save new item before send
            item->SaveToDB(trans);                               // save for prevent lost at next mail load, if send fail then item will deleted

            // item
            draft.AddItem(item);
        }

        draft.SendMailTo(trans, referencePlayer, MailSender(MAIL_CREATURE, reward->Sender));
        CharacterDatabase.CommitTransaction(trans);
    }
}

void GuildAchievementMgr::CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer)
{
    TC_LOG_DEBUG("achievement", "GuildAchievementMgr::CompletedAchievement(%u)", achievement->ID);

    switch (achievement->ID)
    {
        case 5409: // Realm First! Nefarian
        case 5410: // Realm First! Al'Akir
        case 5411: // Realm First! Sinestra
        case 5985: // Realm First! Ragnaros
        case 6126: // Realm First! Deathwing
            return;
    }

    if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER || HasAchieved(achievement->ID))
        return;

    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_NEWS)
        if (Guild* guild = referencePlayer->GetGuild())
            guild->AddGuildNews(GUILD_NEWS_GUILD_ACHIEVEMENT, ObjectGuid::Empty, achievement->Flags & ACHIEVEMENT_FLAG_SHOW_IN_GUILD_HEADER, achievement->ID);

    SendAchievementEarned(achievement);
    CompletedAchievementData& ca = m_completedAchievements[achievement->ID];
    ca.Date = time(NULL);
    ca.Changed = true;

    if (achievement->Flags & ACHIEVEMENT_FLAG_SHOW_GUILD_MEMBERS)
    {
        if (referencePlayer->GetGuildId() == GetOwner()->GetId())
            ca.Guids.insert(referencePlayer->GetGUID());

        if (Group const* group = referencePlayer->GetGroup())
            for (GroupReference const* ref = group->GetFirstMember(); ref != NULL; ref = ref->next())
                if (Player const* groupMember = ref->GetSource())
                    if (groupMember->GetGuildId() == GetOwner()->GetId())
                        ca.Guids.insert(groupMember->GetGUID());
    }

    sAchievementMgr->SetRealmCompleted(achievement);

    m_achievementPoints += achievement->Points;

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT, 0, 0, 0, nullptr, referencePlayer);
    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS, 0, 0, 0, nullptr, referencePlayer);
    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS, m_achievementPoints, 0, 0, nullptr, referencePlayer);
}

void PlayerAchievementMgr::RemoveAchievement(AchievementEntry const* entry)
{
    Player* player = GetOwner();

    WorldPacket data(SMSG_ACHIEVEMENT_DELETED, 4);
    data << uint32(entry->ID);
    data << uint32(0);
    SendPacket(&data);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    trans->PAppend("DELETE FROM character_achievement WHERE achievement = %u AND guid = %u", entry->ID, player->GetGUID().GetCounter());
    m_completedAchievements.erase(entry->ID);

    if (auto tree = sAchievementMgr->GetCriteriaTree(entry->CriteriaTree))
    {
        WalkCriteriaTree(tree, [&](CriteriaTreeNode const* node)
        {
            if (node->Criteria)
            {
                data.Initialize(SMSG_CRITERIA_DELETED, 4);
                data << uint32(node->Criteria->Entry->ID);
                SendPacket(&data);

                trans->PAppend("DELETE FROM character_achievement_progress WHERE criteria = %u AND guid = %u", node->Criteria->Entry->ID, player->GetGUID().GetCounter());
                m_criteriaProgress.erase(node->Criteria->Entry->ID);
            }
        });
    }

    CharacterDatabase.CommitTransaction(trans);

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT, 0, 0, 0, nullptr, player);
    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS, 0, 0, 0, nullptr, player);
}

void AccountAchievementMgr::RemoveAchievement(AchievementEntry const* entry)
{
    //CALL_NOT_IMPLEMENTED();
}

void GuildAchievementMgr::RemoveAchievement(AchievementEntry const* entry)
{
    //CALL_NOT_IMPLEMENTED();
}

struct VisibleAchievementPred
{
    bool operator()(CompletedAchievementMap::value_type const& val)
    {
        AchievementEntry const* achievement = sAchievementMgr->GetAchievement(val.first);
        return achievement && !(achievement->Flags & ACHIEVEMENT_FLAG_HIDDEN);
    }
};

void AccountAchievementMgr::SendAllAchievementData() const
{
    WorldPacket data{ SMSG_ACCOUNT_CRITERIA_UPDATE_ALL, m_criteriaProgress.size() * 40 };
    ByteBuffer buff;
    ObjectGuid counter;
    ObjectGuid acc(HighGuid::WowAccount, GetOwner()->GetSession()->GetAccountId());
    data.WriteBits(m_criteriaProgress.size(), 19);

    for (auto&& it : m_criteriaProgress)
    {
        counter.Set(it.second.Counter);

        data.WriteBit(counter[0]);
        data.WriteBit(acc[7]);

        data.WriteBits(0, 4);

        data.WriteBit(acc[4]);
        data.WriteBit(counter[6]);
        data.WriteBit(acc[5]);
        data.WriteBit(acc[2]);
        data.WriteBit(acc[0]);
        data.WriteBit(acc[6]);
        data.WriteBit(counter[5]);
        data.WriteBit(counter[7]);
        data.WriteBit(acc[1]);
        data.WriteBit(acc[3]);
        data.WriteBit(counter[4]);
        data.WriteBit(counter[3]);
        data.WriteBit(counter[2]);
        data.WriteBit(counter[1]);

        buff.WriteByteSeq(counter[2]);
        buff.WriteByteSeq(acc[2]);
        buff.WriteByteSeq(acc[4]);
        buff.WriteByteSeq(counter[4]);
        buff.WriteByteSeq(acc[6]);
        buff.WriteByteSeq(acc[0]);
        buff << uint32(0);
        buff.WriteByteSeq(acc[1]);
        buff.WriteByteSeq(acc[7]);
        buff.AppendPackedTime(it.second.Date);
        buff.WriteByteSeq(counter[7]);
        buff.WriteByteSeq(counter[6]);
        buff.WriteByteSeq(counter[5]);
        buff.WriteByteSeq(counter[1]);
        buff.WriteByteSeq(acc[3]);
        buff << uint32(0);
        buff.WriteByteSeq(acc[5]);
        buff.WriteByteSeq(counter[3]);
        buff.WriteByteSeq(counter[0]);
        buff << uint32(it.first);
    }

    data.FlushBits();
    data.append(buff);

    SendPacket(&data);
}

void PlayerAchievementMgr::SendAllAchievementData() const
{
    Player* player = GetOwner();
    auto& accountAchievements = player->GetSession()->GetAchievementMgr().m_completedAchievements;

    VisibleAchievementPred isVisible;
    size_t numCriteria = m_criteriaProgress.size();
    size_t numAchievements = 0;
    ByteBuffer criteriaData;
    ByteBuffer completedData;
    ObjectGuid guid = player->GetGUID();
    ObjectGuid counter;

    WorldPacket data(SMSG_ALL_ACHIEVEMENT_DATA, 5 + numAchievements * (1 + 4 + 4 + 4 + 4 + 8) + numCriteria * (1 + 4 + 4 + 4 + 4 + 8 + 8));
    data.WriteBits(numCriteria, 19);

    {
        for (auto&& it : m_criteriaProgress)
        {
            counter.Set(it.second.Counter);

            data.WriteBit(counter[3]);
            data.WriteBit(guid[3]);
            data.WriteBit(guid[6]);
            data.WriteBit(counter[0]);
            data.WriteBit(guid[7]);
            data.WriteBit(counter[1]);
            data.WriteBit(counter[5]);
            data.WriteBit(guid[2]);
            data.WriteBit(guid[1]);
            data.WriteBit(counter[7]);
            data.WriteBit(guid[4]);
            data.WriteBit(guid[0]);
            data.WriteBit(counter[2]);
            data.WriteBit(guid[5]);
            data.WriteBit(counter[4]);
            data.WriteBits(0, 4);
            data.WriteBit(counter[6]);

            criteriaData.WriteByteSeq(counter[7]);
            criteriaData << uint32(0);                              // timer 1
            criteriaData.WriteByteSeq(counter[6]);
            criteriaData.WriteByteSeq(guid[1]);
            criteriaData << uint32(it.first);                       // criteria id
            criteriaData.WriteByteSeq(counter[4]);
            criteriaData.WriteByteSeq(guid[0]);
            criteriaData.WriteByteSeq(guid[4]);
            criteriaData.WriteByteSeq(guid[6]);
            criteriaData.WriteByteSeq(counter[1]);
            criteriaData.WriteByteSeq(counter[5]);
            criteriaData.WriteByteSeq(guid[7]);
            criteriaData.WriteByteSeq(guid[2]);
            criteriaData.WriteByteSeq(counter[2]);
            criteriaData.WriteByteSeq(counter[0]);
            criteriaData.WriteByteSeq(guid[3]);
            criteriaData.WriteByteSeq(counter[3]);
            criteriaData << uint32(0);                              // timer 2
            criteriaData.WriteByteSeq(guid[5]);
            criteriaData.AppendPackedTime(it.second.Date);          // criteria date
        }
    }

    size_t pos = data.bitwpos();
    data.WriteBits(numAchievements, 20);

    for (auto&& map : { accountAchievements })
    {
        for (auto&& it : map)
        {
            if (!isVisible(it))
                continue;

            AchievementEntry const* achievement = sAchievementStore.LookupEntry(it.first);
            if (!achievement)
                continue;

            if ((achievement->RequiredFaction == ACHIEVEMENT_FACTION_HORDE    && player->GetTeam() != HORDE) ||
                (achievement->RequiredFaction == ACHIEVEMENT_FACTION_ALLIANCE && player->GetTeam() != ALLIANCE))
                continue;

            if (achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT)
                guid.Clear();
            else if (m_completedAchievements.find(it.first) == m_completedAchievements.end())
                guid = ObjectGuid(HighGuid::Player, it.second.CompletedBy);
            else
                guid = GetOwner()->GetGUID();

            ++numAchievements;

            data.WriteBit(guid[0]);
            data.WriteBit(guid[7]);
            data.WriteBit(guid[1]);
            data.WriteBit(guid[5]);
            data.WriteBit(guid[2]);
            data.WriteBit(guid[4]);
            data.WriteBit(guid[6]);
            data.WriteBit(guid[3]);

            completedData << uint32(it.first);                      // achievement Id
            completedData << uint32(achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT ? 0 : realm.Id.Realm);
            completedData.WriteByteSeq(guid[5]);
            completedData.WriteByteSeq(guid[7]);
            completedData << uint32(achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT ? 0 : realm.Id.Realm);
            completedData.AppendPackedTime(it.second.Date);         // achievement date
            completedData.WriteByteSeq(guid[0]);
            completedData.WriteByteSeq(guid[4]);
            completedData.WriteByteSeq(guid[1]);
            completedData.WriteByteSeq(guid[6]);
            completedData.WriteByteSeq(guid[2]);
            completedData.WriteByteSeq(guid[3]);
        }
    }

    data.FlushBits();
    data.PutBits(pos, numAchievements, 20);

    data.append(completedData);
    data.append(criteriaData);

    SendPacket(&data);

    player->GetSession()->GetAchievementMgr().SendAllAchievementData();
}

void GuildAchievementMgr::SendAllAchievementData(Player* receiver) const
{
    VisibleAchievementPred isVisible;
    WorldPacket data(SMSG_GUILD_ACHIEVEMENT_DATA, m_completedAchievements.size() * (4 + 4) + 3);
    data.WriteBits(std::count_if(m_completedAchievements.begin(), m_completedAchievements.end(), isVisible), 20);
    for (CompletedAchievementMap::const_iterator itr = m_completedAchievements.begin(); itr != m_completedAchievements.end(); ++itr)
    {
        if (!isVisible(*itr))
            continue;

        data.WriteBit(0); // 0
        data.WriteBit(0); // 6
        data.WriteBit(0); // 2
        data.WriteBit(0); // 1
        data.WriteBit(0); // 7
        data.WriteBit(0); // 4
        data.WriteBit(0); // 5
        data.WriteBit(0); // 3
    }
    data.FlushBits();
    for (CompletedAchievementMap::const_iterator itr = m_completedAchievements.begin(); itr != m_completedAchievements.end(); ++itr)
    {
        if (!isVisible(*itr))
            continue;

        data << uint32(itr->first);
        // data.WriteByteSeq 5
        // data.WriteByteSeq 0
        data.AppendPackedTime(itr->second.Date);
        // data.WriteByteSeq 4
        // data.WriteByteSeq 3
        // data.WriteByteSeq 1
        // data.WriteByteSeq 6
        // data.WriteByteSeq 2
        data << uint32(0);
        data << uint32(0);
        // data.WriteByteSeq 7
    }

    receiver->GetSession()->SendPacket(&data);
}

void PlayerAchievementMgr::SendAchievementInfo(Player* receiver) const
{
    Player* player = GetOwner();
    ObjectGuid guid = player->GetGUID();
    ObjectGuid counter;

    auto& accountAchievements = player->GetSession()->GetAchievementMgr().m_completedAchievements;
    auto& accountProgress = player->GetSession()->GetAchievementMgr().m_criteriaProgress;

    VisibleAchievementPred isVisible;
    size_t numCriteria = m_criteriaProgress.size() + accountProgress.size();
    size_t numAchievements = 0;

    ByteBuffer criteriaData;
    ByteBuffer achievementsData;

    WorldPacket data(SMSG_RESPOND_INSPECT_ACHIEVEMENTS, 1 + 8 + 3 + 3 + numAchievements * (4 + 4) + numCriteria * (0));

    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[2]);

    size_t pos = data.bitwpos();
    data.WriteBits(numAchievements, 20);
    data.WriteBits(numCriteria, 19);

    for (auto&& map : { m_criteriaProgress, accountProgress })
    {
        for (auto&& it : map)
        {
            counter.Set(it.second.Counter);
            ObjectGuid guid2 = it.second.CompletedGUID;

            data.WriteBit(guid2[1]);
            data.WriteBit(guid2[4]);
            data.WriteBit(guid2[5]);
            data.WriteBit(counter[7]);
            data.WriteBit(counter[4]);
            data.WriteBit(counter[3]);
            data.WriteBit(guid2[7]);
            data.WriteBit(guid2[0]);
            data.WriteBit(guid2[6]);
            data.WriteBits(0, 4);           // criteria progress flags
            data.WriteBit(guid2[2]);
            data.WriteBit(counter[5]);
            data.WriteBit(counter[6]);
            data.WriteBit(counter[0]);
            data.WriteBit(counter[2]);
            data.WriteBit(counter[1]);
            data.WriteBit(guid2[3]);

            criteriaData.WriteByteSeq(counter[4]);
            criteriaData << uint32(0);             // timer 1
            criteriaData.WriteByteSeq(counter[1]);
            criteriaData.WriteByteSeq(guid2[1]);
            criteriaData.WriteByteSeq(counter[7]);
            criteriaData << uint32(it.first);
            criteriaData.WriteByteSeq(guid2[3]);
            criteriaData.WriteByteSeq(counter[3]);
            criteriaData.WriteByteSeq(counter[5]);
            criteriaData.WriteByteSeq(counter[2]);
            criteriaData.WriteByteSeq(guid2[4]);
            criteriaData.WriteByteSeq(counter[0]);
            criteriaData.WriteByteSeq(guid2[0]);
            criteriaData << uint32(0);             // timer 2
            criteriaData.WriteByteSeq(guid2[7]);
            criteriaData.AppendPackedTime(it.second.Date);
            criteriaData.WriteByteSeq(counter[6]);
            criteriaData.WriteByteSeq(guid2[2]);
            criteriaData.WriteByteSeq(guid2[6]);
            criteriaData.WriteByteSeq(guid2[5]);
        }
    }

    data.WriteBit(guid[5]);

    for (auto&& map : { m_completedAchievements, accountAchievements })
    {
        for (auto&& it : map)
        {
            if (!isVisible(it))
                continue;

            ++numAchievements;

            ObjectGuid guid3 = *(it.second.Guids.begin());

            data.WriteBit(guid3[0]);
            data.WriteBit(guid3[2]);
            data.WriteBit(guid3[5]);
            data.WriteBit(guid3[4]);
            data.WriteBit(guid3[3]);
            data.WriteBit(guid3[6]);
            data.WriteBit(guid3[1]);
            data.WriteBit(guid3[7]);

            achievementsData.WriteByteSeq(guid3[1]);
            achievementsData.WriteByteSeq(guid3[0]);
            achievementsData.AppendPackedTime(it.second.Date);
            achievementsData << uint32(0);           //RealmID
            achievementsData << uint32(it.first);
            achievementsData.WriteByteSeq(guid3[7]);
            achievementsData.WriteByteSeq(guid3[4]);
            achievementsData.WriteByteSeq(guid3[6]);
            achievementsData.WriteByteSeq(guid3[2]);
            achievementsData.WriteByteSeq(guid3[3]);
            achievementsData.WriteByteSeq(guid3[5]);
            achievementsData << uint32(0);           //RealmID
        }
    }

    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);

    data.FlushBits();

    data.WriteByteSeq(guid[5]);

    data.append(achievementsData);
    data.append(criteriaData);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);

    data.PutBits(pos, numAchievements, 20);

    receiver->GetSession()->SendPacket(&data);
}

void GuildAchievementMgr::SendAchievementInfo(Player* receiver, uint32 achievementId) const
{
    // will send response to criteria progress request
    AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementId);
    if (!achievement)
        return;

    CriteriaTreeNode const* tree = sAchievementMgr->GetCriteriaTree(achievement->CriteriaTree);
    if (!tree)
        return;

    ObjectGuid counter;
    ObjectGuid guid;
    uint32 numCriteria = 0;
    ByteBuffer criteriaData;

    WorldPacket data(SMSG_GUILD_CRITERIA_DATA, criteriaData.size() * 20);
    data.WriteBits(numCriteria, 19);

    WalkCriteriaTree(tree, [&](CriteriaTreeNode const* node)
    {
        if (!node->Criteria)
            return;

        uint32 criteriaId = node->Criteria->Entry->ID;
        CriteriaProgressMap::const_iterator progress = m_criteriaProgress.find(criteriaId);
        if (progress == m_criteriaProgress.end())
            return;

        ++numCriteria;
        counter.Set(progress->second.Counter);
        guid = progress->second.CompletedGUID;

        data.WriteBit(guid[2]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[6]);
        data.WriteBit(counter[0]);
        data.WriteBit(guid[7]);
        data.WriteBit(counter[2]);
        data.WriteBit(counter[5]);
        data.WriteBit(counter[1]);
        data.WriteBit(counter[4]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[3]);
        data.WriteBit(counter[7]);
        data.WriteBit(counter[6]);
        data.WriteBit(guid[1]);
        data.WriteBit(counter[3]);
        data.WriteBit(guid[5]);

        criteriaData.WriteByteSeq(guid[1]);
        criteriaData.WriteByteSeq(counter[5]);
        criteriaData.WriteByteSeq(guid[6]);
        criteriaData << uint32(criteriaId);
        criteriaData.WriteByteSeq(counter[6]);
        criteriaData << uint32(progress->second.Date);      // unknown date
        criteriaData << uint32(progress->second.Date);      // last update time (not packed!)
        criteriaData.WriteByteSeq(guid[3]);
        criteriaData.WriteByteSeq(counter[0]);
        criteriaData.WriteByteSeq(guid[7]);
        criteriaData.WriteByteSeq(counter[4]);
        criteriaData.WriteByteSeq(counter[2]);
        criteriaData.WriteByteSeq(counter[3]);
        criteriaData.WriteByteSeq(guid[4]);
        criteriaData.WriteByteSeq(counter[1]);
        criteriaData.WriteByteSeq(guid[5]);
        criteriaData.WriteByteSeq(guid[0]);
        criteriaData << uint32(0);
        criteriaData.WriteByteSeq(guid[2]);
        criteriaData.WriteByteSeq(counter[7]);
        criteriaData << uint32(progress->second.Date);      // unknown date
    });

    data.FlushBits();
    data.PutBits(0, numCriteria, 19);

    if (numCriteria)
        data.append(criteriaData);

    receiver->GetSession()->SendPacket(&data);
}

bool AchievementMgr::HasAchieved(uint32 achievementId) const
{
    return m_completedAchievements.find(achievementId) != m_completedAchievements.end();
}

bool AchievementMgr::CanUpdateCriteria(Criteria const* criteria, CriteriaTreeList const* trees, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer)
{
    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_ACHIEVEMENT_CRITERIA, criteria->Entry->ID, NULL))
    {
        TC_LOG_TRACE("achievement", "CanUpdateCriteria: (Id: %u Type %s) Disabled",
            criteria->Entry->ID, AchievementGlobalMgr::GetCriteriaTypeString(criteria->Entry->Type));
        return false;
    }

    bool treeRequirementPassed = false;
    for (auto&& tree : *trees)
    {
        if (!CanUpdateCriteriaTree(tree, referencePlayer))
            continue;

        treeRequirementPassed = true;
        break;
    }

    if (!treeRequirementPassed)
        return false;

    if (!RequirementsSatisfied(criteria->Entry, miscValue1, miscValue2, miscValue3, unit, referencePlayer))
    {
        TC_LOG_TRACE("achievement", "CanUpdateCriteria: (Id: %u Type %s) Requirements not satisfied",
            criteria->Entry->ID, AchievementGlobalMgr::GetCriteriaTypeString(criteria->Entry->Type));
        return false;
    }

    if (criteria->Modifier && !ModifierTreeRequirementsSatisfied(criteria->Modifier, miscValue1, miscValue2, unit, referencePlayer))
    {
        TC_LOG_TRACE("achievement", "CanUpdateCriteria: (Id: %u Type %s) Additional requirements not satisfied",
            criteria->Entry->ID, AchievementGlobalMgr::GetCriteriaTypeString(criteria->Entry->Type));
        return false;
    }

    if (!ConditionsSatisfied(criteria->Entry, referencePlayer))
    {
        TC_LOG_TRACE("achievement", "CanUpdateCriteria: (Id: %u Type %s) Conditions not satisfied",
            criteria->Entry->ID, AchievementGlobalMgr::GetCriteriaTypeString(criteria->Entry->Type));
        return false;
    }

    return true;
}

bool AchievementMgr::ConditionsSatisfied(CriteriaEntry const* criteria, Player* referencePlayer) const
{
    if (criteria->TimeLimit)
    {
        // Do not update if timer not running
        if (criteria->StartEvent)
            if (m_additionalCriteriaProgress.find(criteria->ID) == m_additionalCriteriaProgress.end())
                return false;
    }
    else
    {
        switch (criteria->StartEvent)
        {
            case CRITERIA_START_TYPE_QUEST:
                if (referencePlayer->GetQuestStatus(criteria->StartAsset) != QUEST_STATUS_INCOMPLETE)
                    return false;
                break;
            case CRITERIA_START_TYPE_BATTLEGROUND:
                if (referencePlayer->GetMapId() != criteria->StartAsset)
                    return false;
                break;
            default:
                break;
        }

        if (NeedToStartNotTimedCriteria(criteria))
            return m_additionalCriteriaProgress.find(criteria->ID) != m_additionalCriteriaProgress.end();
    }

    // TODO: move more shit to here

    return true;
}

bool AchievementMgr::RequirementsSatisfied(CriteriaEntry const* achievementCriteria, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer) const
{
    switch (AchievementCriteriaTypes(achievementCriteria->Type))
    {
        case ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
        case ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION:
        case ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
        case ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY:
        case ACHIEVEMENT_CRITERIA_TYPE_GATHER_ITEM:
        case ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL:
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_VENDORS:
        case ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
        case ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED:
        case ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
        case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
        case ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
        case ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
        case ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET:
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_PET_BATTLE:
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET2:
            if (!miscValue1)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING:
        case ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS:
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL:
        case ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN:
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
        case ACHIEVEMENT_CRITERIA_TYPE_PET_BATTLE_ACHIEVEMENT_POINTS:
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            return HasAchieved(achievementCriteria->Asset.LinkedAchievement);
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
            if (!miscValue1 || achievementCriteria->Asset.BgMapID != referencePlayer->GetMapId())
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
            if (!miscValue1 || achievementCriteria->Asset.CreatureID != miscValue1)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL:
            // update at loading or specific skill update
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.SkillID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            // update at loading or specific skill update
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.SkillID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.ZoneID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
            if (!miscValue1 || referencePlayer->GetMapId() != achievementCriteria->Asset.MapID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP:
            if (!miscValue1 || referencePlayer->GetMapId() != achievementCriteria->Asset.MapID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH:
        {
            if (!miscValue1)
                return false;
            // skip wrong arena achievements, if not achievIdByArenaSlot then normal total death counter
            /*bool notfit = false;
            for (int j = 0; j < MAX_ARENA_SLOT; ++j)
            {
                if (achievIdByArenaSlot[j] == achievementCriteria->achievement)
                {
                    Battleground* bg = referencePlayer->GetBattleground();
                    if (!bg || !bg->isArena() || ArenaTeam::GetSlotByType(bg->GetArenaType()) != j)
                        notfit = true;
                    break;
                }
            }
            if (notfit)
                return false;*/
            break;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON:
        {
            if (!miscValue1)
                return false;
            /*
            Map const* map = referencePlayer->IsInWorld() ? referencePlayer->GetMap() : sMapMgr->FindMap(referencePlayer->GetMapId(), referencePlayer->GetInstanceId());
            if (!map || !map->IsDungeon())
                return false;

            // search case
            bool found = false;
            for (int j = 0; achievIdForDungeon[j][0]; ++j)
            {
                if (achievIdForDungeon[j][0] == achievementCriteria->achievement)
                {
                    if (map->IsRaid())
                    {
                        // if raid accepted (ignore difficulty)
                        if (!achievIdForDungeon[j][2])
                            break;                      // for
                    }
                    else if (referencePlayer->GetDungeonDifficulty() == DUNGEON_DIFFICULTY_NORMAL)
                    {
                        // dungeon in normal mode accepted
                        if (!achievIdForDungeon[j][1])
                            break;                      // for
                    }
                    else
                    {
                        // dungeon in heroic mode accepted
                        if (!achievIdForDungeon[j][3])
                            break;                      // for
                    }

                    found = true;
                    break;                              // for
                }
            }
            if (!found)
                return false;

            //FIXME: work only for instances where max == min for players
            if (((InstanceMap*)map)->GetMaxPlayers() != achievementCriteria->death_in_dungeon.manLimit)
                return false;
            */
            return false;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.CreatureEntry)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER:
            if (!miscValue1 || !unit || unit->GetTypeId() != TYPEID_PLAYER)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM:
            if (!miscValue1 || miscValue2 != achievementCriteria->Asset.Type)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST:
        {
            // if miscValues != 0, it contains the questID.
            if (miscValue1)
            {
                if (miscValue1 != achievementCriteria->Asset.QuestID)
                    return false;
            }
            else
            {
                // login case.
                if (!referencePlayer->GetQuestRewardStatus(achievementCriteria->Asset.QuestID))
                    return false;
            }

            if (AchievementCriteriaDataSet const* data = sAchievementMgr->GetCriteriaDataSet(achievementCriteria))
                if (!data->Meets(referencePlayer, unit))
                    return false;
            break;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.SpellID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL:
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.SpellID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL:
            return miscValue1 == achievementCriteria->Asset.SpellID;
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE:
            // miscValue1 = itemId - miscValue2 = count of item loot
            // miscValue3 = loot_type (note: 0 = LOOT_CORPSE and then it ignored)
            if (!miscValue1 || !miscValue2 || !miscValue3 || miscValue3 != achievementCriteria->Asset.LootType)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM:
            if (miscValue1 && achievementCriteria->Asset.ItemID != miscValue1)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM:
            if (!miscValue1 || achievementCriteria->Asset.ItemID != miscValue1)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM:
            if (!miscValue1)
                return false;
            // For statistic
            if (!achievementCriteria->Asset.ItemID)
                return true;
            // For real achievements
            return miscValue1 == achievementCriteria->Asset.ItemID;
        case ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA:
        {
            WorldMapOverlayEntry const* worldOverlayEntry = sWorldMapOverlayStore.LookupEntry(achievementCriteria->Asset.AreaReference);
            if (!worldOverlayEntry)
                break;

            bool matchFound = false;
            for (int j = 0; j < MAX_WORLD_MAP_OVERLAY_AREA_IDX; ++j)
            {
                AreaTableEntry const* area = sAreaTableStore.LookupEntry(worldOverlayEntry->areatableID[j]);
                if (!area)                            // array have 0 only in empty tail
                    break;

                if (area->exploreFlag < 0)
                    continue;

                uint32 playerIndexOffset = uint32(area->exploreFlag) / 32;
                if (playerIndexOffset >= PLAYER_EXPLORED_ZONES_SIZE)
                    continue;

                uint32 mask = 1 << (uint32(area->exploreFlag) % 32);

                if (referencePlayer->GetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + playerIndexOffset) & mask)
                {
                    matchFound = true;
                    break;
                }
            }

            if (!matchFound)
                return false;
            break;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION:
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.FactionID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            // miscValue1 = itemid miscValue2 = itemSlot
            if (!miscValue1 || miscValue2 != achievementCriteria->Asset.ItemSlot)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        {
            // miscValue1 = itemid miscValue2 = diced value
            if (!miscValue1 || miscValue2 != achievementCriteria->Asset.RollValue)
                return false;

            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!proto)
                return false;
            break;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.EmoteID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE:
        case ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE:
            if (!miscValue1)
                return false;
            // map specific case (BG in fact) expected player targeted damage/heal
            if (!unit || unit->GetTypeId() != TYPEID_PLAYER)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM:
            // miscValue1 = item_id
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.ItemID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.GoEntry)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.GoEntry)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.SkillLine)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
        {
            if (!miscValue1)
                return false;
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!proto || proto->Quality < ITEM_QUALITY_EPIC)
                return false;
            break;
        }
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE:
            if (miscValue1 && miscValue1 != achievementCriteria->Asset.SkillLine)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.ClassID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_HK_RACE:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.RaceID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.ObjectiveId)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.AreaID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_CURRENCY:
            if (!miscValue1 || !miscValue2 || int64(miscValue2) < 0
                || miscValue1 != achievementCriteria->Asset.Currency)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA:
            if (miscValue1 != achievementCriteria->Asset.MapID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON:
            if (!miscValue1 || miscValue1 != achievementCriteria->Asset.DungeonMapID)
                return false;
            break;
        case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET:
            return miscValue1 == achievementCriteria->Asset.PetEntry;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            return miscValue1 == achievementCriteria->Asset.ChallengeType;
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO:
            return miscValue1 == achievementCriteria->Asset.ScenarioId;
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_BATTLE_PET_LEVEL:
            return miscValue1 == achievementCriteria->Asset.Level;
        default:
            break;
    }
    return true;
}

bool AchievementMgr::ModifierTreeRequirementsSatisfied(ModifierTreeNode const* tree, uint64 miscValue1, uint64 miscValue2, Unit const* unit, Player* referencePlayer) const
{
    switch (tree->Entry->Operator)
    {
        case MODIFIER_TREE_OPERATOR_SINGLE:
            return AdditionalRequirementsSatisfied(tree, miscValue1, miscValue2, unit, referencePlayer);
        case MODIFIER_TREE_OPERATOR_NOT:
            return !AdditionalRequirementsSatisfied(tree, miscValue1, miscValue2, unit, referencePlayer);
        case MODIFIER_TREE_OPERATOR_ALL:
            for (auto&& node : tree->Nodes)
                if (!ModifierTreeRequirementsSatisfied(node, miscValue1, miscValue2, unit, referencePlayer))
                    return false;
            return true;
        case MODIFIER_TREE_OPERATOR_ANY:
            for (auto&& node : tree->Nodes)
                if (ModifierTreeRequirementsSatisfied(node, miscValue1, miscValue2, unit, referencePlayer))
                    return true;
            return false;
        default:
            break;
    }
    return false;
}

bool AchievementMgr::AdditionalRequirementsSatisfied(ModifierTreeNode const* tree, uint64 miscValue1, uint64 miscValue2, Unit const* unit, Player* referencePlayer) const
{
    if (uint32 reqType = tree->Entry->ConditionType)
    {
        uint32 reqValue = tree->Entry->ConditionValue[0];
        switch (AchievementCriteriaAdditionalCondition(reqType))
        {
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_DRUNK_VALUE:          // 1
                return referencePlayer->GetDrunkValue() >= reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_UNK2:                        // 2
                if (reqValue == 923)    // Horde
                    return referencePlayer->GetTeam() == HORDE;
                if (reqValue == 924)    // Alliance
                    return referencePlayer->GetTeam() == ALLIANCE;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_ITEM_LEVEL:                  // 3
            {
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(miscValue1);
                if (!proto)
                    return false;
                return proto->ItemLevel >= reqValue;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_ENTRY: // 4
                if (!unit || unit->GetEntry() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_PLAYER: // 5
                if (!unit || unit->GetTypeId() != TYPEID_PLAYER)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_DEAD: // 6
                if (!unit || unit->IsAlive())
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_ENEMY: // 7
                if (!unit || !referencePlayer->IsHostileTo(unit))
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_HAS_AURA: // 8
                if (referencePlayer->GetVehicleBase() && referencePlayer->GetVehicleBase()->HasAura(reqValue))
                    return true;
                if (!referencePlayer->HasAura(reqValue))
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA: // 10
                if (!unit)
                    return false;
                if (!unit->HasAura(reqValue))
                {
                    // Cross-faction hacks. Horde/alliacne flags
                    if (reqValue == 23335 && referencePlayer->GetTeam() == ALLIANCE)
                        return unit->HasAura(23333);
                    if (reqValue == 23333 && referencePlayer->GetTeam() == HORDE)
                        return unit->HasAura(23335);
                    return false;
                }
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA_TYPE: // 11
                if (!unit || !unit->HasAuraType(AuraType(reqValue)))
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_MIN: // 14
            {
                // miscValue1 is itemid
                ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
                if (!item || item->Quality < reqValue)
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_EQUALS: // 15
            {
                // miscValue1 is itemid
                ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
                if (!item || item->Quality != reqValue)
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_AREA_OR_ZONE: // 17
            {
                uint32 zoneId, areaId;
                referencePlayer->GetZoneAndAreaId(zoneId, areaId);
                if (zoneId != reqValue && areaId != reqValue)
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_AREA_OR_ZONE: // 18
            {
                if (!unit)
                    return false;
                uint32 zoneId, areaId;
                unit->GetZoneAndAreaId(zoneId, areaId);
                if (zoneId != reqValue && areaId != reqValue)
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_INSTANCE_DIFFICULTY: // 20
            {
                Difficulty difficulty = referencePlayer->GetMap()->GetDifficulty();
                switch (reqValue)
                {
                    case 0:
                        return difficulty == DUNGEON_DIFFICULTY_NORMAL || difficulty == RAID_DIFFICULTY_10MAN_NORMAL;
                    case 1:
                        return difficulty == DUNGEON_DIFFICULTY_HEROIC || difficulty == RAID_DIFFICULTY_25MAN_NORMAL;
                    case 2:
                        return difficulty == RAID_DIFFICULTY_10MAN_HEROIC;
                    case 3:
                        return difficulty == RAID_DIFFICULTY_25MAN_HEROIC;
                }
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_ARENA_TYPE:
                return reqValue == miscValue2;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_RACE: // 25
                if (referencePlayer->GetRace() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_CLASS: // 26
                if (referencePlayer->GetClass() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_RACE: // 27
                if (!unit || unit->GetTypeId() != TYPEID_PLAYER || unit->GetRace() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_CLASS: // 28
                if (!unit || unit->GetTypeId() != TYPEID_PLAYER || unit->GetClass() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_MAX_GROUP_MEMBERS: // 29
                if (referencePlayer->GetGroup() && referencePlayer->GetGroup()->GetMembersCount() >= reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_TYPE: // 30
            {
                if (!unit)
                    return false;
                Creature const* const creature = unit->ToCreature();
                if (!creature || creature->GetCreatureType() != reqValue)
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_MAP: // 32
                if (referencePlayer->GetMapId() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_PET_BATTLE_FULL_TEAM_25:
            {
                PetBattle* battle = sPetBattleSystem->GetPlayerPetBattle(referencePlayer->GetGUID());
                if (!battle)
                    return false;
                PetBattleTeam* team = battle->GetTeam(referencePlayer->GetGUID());
                if (!team)
                    return false;
                if (team->BattlePets.size() != PET_BATTLE_MAX_TEAM_PETS)
                    return false;
                for (auto&& pet : team->BattlePets)
                    if (pet->GetLevel() < reqValue)
                        return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_COMPLETE_QUEST_NOT_IN_GROUP: // 35
                if (referencePlayer->GetGroup())
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TITLE_BIT_INDEX: // 38
                // miscValue1 is title's bit index
                if (miscValue1 != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SOURCE_LEVEL: // 39
                if (referencePlayer->GetLevel() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_LEVEL: // 40
                if (!unit || unit->GetLevel() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_ZONE: // 41
                if (!unit || unit->GetZoneId() != reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_HEALTH_PERCENT_BELOW: // 46
                if (!unit || unit->GetHealthPct() >= reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_MIN_ACHIEVEMENT_POINTS:      // 56
                return GetAchievementPoints() >= reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_REQUIRES_LFG_GROUP: // 58
                if (!referencePlayer || !referencePlayer->GetGroup() || !referencePlayer->GetGroup()->isLFGGroup())
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_REQUIRES_GUILD_GROUP:
                if (referencePlayer && referencePlayer->GetGroup())
                    return referencePlayer->GetGroup()->IsGuildGroup(referencePlayer->GetGuildId(), referencePlayer);
                return false;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_GUILD_REPUTATION: // 62
                return referencePlayer->GetReputation(GUILD_REPUTATION_ID) >= reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_RATED_BATTLEGROUND:
                if (referencePlayer)
                    if (Battleground* bg = referencePlayer->GetBattleground())
                        return bg->IsRatedBG();
                return false;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_PROJECT_RARITY:
                if (ResearchProjectEntry const* currentProjectEntry = sResearchProjectStore.LookupEntry(miscValue1))
                    return currentProjectEntry->Rarity == reqValue;
                return false;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_PROJECT_RACE:
                if (ResearchProjectEntry const* currentProjectEntry = sResearchProjectStore.LookupEntry(miscValue1))
                    return currentProjectEntry->ResearchBranchId == reqValue;
                return false;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_WORLD_STATE:
            {
                if (reqValue == 3785) // FIXME
                    if (!referencePlayer || (referencePlayer->GetMap()->GetDifficulty() != RAID_DIFFICULTY_10MAN_HEROIC && referencePlayer->GetMap()->GetDifficulty() != RAID_DIFFICULTY_25MAN_HEROIC))
                        return false;
                // hacks for crossfaction
                if (Battleground* bg = referencePlayer->GetBattleground())
                {
                    if (bg->GetMapId() == 607)
                    {
                        if (referencePlayer->GetMap()->GetWorldState(6450) != 0 && referencePlayer->GetTeam() == HORDE && referencePlayer->GetBGTeamId() == TEAM_ALLIANCE && dynamic_cast<BattlegroundSA*>(bg)->GetDefenerTeam() == TEAM_ALLIANCE)
                            return true;
                        if (referencePlayer->GetMap()->GetWorldState(6451) != 0 && referencePlayer->GetTeam() == ALLIANCE && referencePlayer->GetBGTeamId() == TEAM_HORDE && dynamic_cast<BattlegroundSA*>(bg)->GetDefenerTeam() == TEAM_HORDE)
                            return true;
                        if (referencePlayer->GetMap()->GetWorldState(6449) != 0 && referencePlayer->GetTeam() == HORDE && referencePlayer->GetBGTeamId() == TEAM_ALLIANCE && dynamic_cast<BattlegroundSA*>(bg)->Attackers == TEAM_ALLIANCE)
                            return true;
                        if (referencePlayer->GetMap()->GetWorldState(6448) != 0 && referencePlayer->GetTeam() == ALLIANCE && referencePlayer->GetBGTeamId() == TEAM_HORDE && dynamic_cast<BattlegroundSA*>(bg)->Attackers == TEAM_HORDE)
                            return true;
                        if (referencePlayer->GetMap()->GetWorldState(5668) != 0 && referencePlayer->GetTeam() == HORDE && referencePlayer->GetBGTeamId() == TEAM_ALLIANCE && dynamic_cast<BattlegroundSA*>(bg)->Attackers == TEAM_ALLIANCE)
                            return true;
                        if (referencePlayer->GetMap()->GetWorldState(5669) != 0 && referencePlayer->GetTeam() == ALLIANCE && referencePlayer->GetBGTeamId() == TEAM_HORDE && dynamic_cast<BattlegroundSA*>(bg)->Attackers == TEAM_HORDE)
                            return true;
                    }
                }

                if (sWorld->getWorldState(reqValue) != 0)
                    return true;
                return referencePlayer->GetMap()->GetWorldState(reqValue) != 0;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_MAP_DIFFICULTY:
                return referencePlayer && referencePlayer->GetMap()->GetDifficulty() == reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_MODIFIER_TREE:
                if (ModifierTreeNode const* tree = sAchievementMgr->GetCriteriaModifierTree(reqValue))
                    return ModifierTreeRequirementsSatisfied(tree, miscValue1, miscValue2, unit, referencePlayer);
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_TARGET_MIN_LEVEL:
                if (!unit || unit->GetLevel() < reqValue)
                    return false;
                break;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_FAMILY:
                return miscValue2 & uint32(1 << reqValue);
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_HEALTH_PCT:
            {
                // Used only for ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET so just check caged pet
                PetBattle* battle = sPetBattleSystem->GetPlayerPetBattle(referencePlayer->GetGUID());
                if (!battle)
                    return false;
                BattlePet* cagedPet = battle->GetCagedPet();
                if (!cagedPet)
                    return false;
                return cagedPet->GetCurrentHealth() * 100.0f / cagedPet->GetMaxHealth() < float(reqValue);
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_AT_LEAST_N_GUILD_MEMBERS:
            {
                uint32 guildId = referencePlayer->GetGuildId();
                Group* group = referencePlayer->GetGroup();
                if (!guildId || !group)
                    return false;

                uint32 count = 0;
                for (auto&& member : *group)
                    if (member->GetGuildId() == guildId)
                        ++count;

                return count >= reqValue;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_PET_BATTLE_AGAINST_TAMER:
                // Not Impemented
                return false;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_NEED_CHALLENGE_MEDAL:
                return miscValue2 >= reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_QUALITY:
            {
                // Used only for ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET so just check caged pet
                PetBattle* battle = sPetBattleSystem->GetPlayerPetBattle(referencePlayer->GetGUID());
                if (!battle)
                    return false;
                BattlePet* cagedPet = battle->GetCagedPet();
                if (!cagedPet)
                    return false;
                auto qualityEntry = sBattlePetBreedQualityStore.LookupEntry(reqValue);
                if (!qualityEntry)
                    return false;
                return cagedPet->GetQuality() == qualityEntry->Quality;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_PET_BATTLE_PVP:
            {
                // Not implemented
                return false;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_SPECIES:
                return miscValue1 == reqValue;
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_ITEM_CLASS_SUBCLASS:
            {
                // miscValue1 is itemid
                ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
                if (!item || item->Class != tree->Entry->ConditionValue[0] || item->SubClass != tree->Entry->ConditionValue[1])
                    return false;
                break;
            }
            case ACHIEVEMENT_CRITERIA_ADDITIONAL_CONDITION_SEASON_CURRENCY:
                return referencePlayer->GetCurrencyOnSeason(tree->Entry->ConditionValue[0], false) >= tree->Entry->ConditionValue[1];
            default:
                break;
        }
    }
    return true;
}

char const* AchievementGlobalMgr::GetCriteriaTypeString(uint32 type)
{
    return GetCriteriaTypeString(AchievementCriteriaTypes(type));
}

char const* AchievementGlobalMgr::GetCriteriaTypeString(AchievementCriteriaTypes type)
{
    switch (type)
    {
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
            return "KILL_CREATURE";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
            return "TYPE_WIN_BG";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
            return "COMPLETE_RESEARCH";
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL:
            return "REACH_LEVEL";
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL:
            return "REACH_SKILL_LEVEL";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            return "COMPLETE_ACHIEVEMENT";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
            return "COMPLETE_QUEST_COUNT";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
            return "COMPLETE_DAILY_QUEST_DAILY";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            return "COMPLETE_QUESTS_IN_ZONE";
        case ACHIEVEMENT_CRITERIA_TYPE_CURRENCY:
            return "CURRENCY";
        case ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE:
            return "DAMAGE_DONE";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
            return "COMPLETE_DAILY_QUEST";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
            return "COMPLETE_BATTLEGROUND";
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP:
            return "DEATH_AT_MAP";
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH:
            return "DEATH";
        case ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON:
            return "DEATH_IN_DUNGEON";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID:
            return "COMPLETE_RAID";
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE:
            return "KILLED_BY_CREATURE";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE_GUILD:
            return "COMPLETE_CHALLENGE_MODE_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER:
            return "KILLED_BY_PLAYER";
        case ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING:
            return "FALL_WITHOUT_DYING";
        case ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM:
            return "DEATHS_FROM";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST:
            return "COMPLETE_QUEST";
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
            return "BE_SPELL_TARGET";
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL:
            return "CAST_SPELL";
        case ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
            return "BG_OBJECTIVE_CAPTURE";
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            return "HONORABLE_KILL_AT_AREA";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA:
            return "WIN_ARENA";
        case ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA:
            return "PLAY_ARENA";
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL:
            return "LEARN_SPELL";
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL:
            return "HONORABLE_KILL";
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM:
            return "OWN_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
            return "WIN_RATED_ARENA";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING:
            return "HIGHEST_TEAM_RATING";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
            return "HIGHEST_PERSONAL_RATING";
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            return "LEARN_SKILL_LEVEL";
        case ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM:
            return "USE_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM:
            return "LOOT_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA:
            return "EXPLORE_AREA";
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_RANK:
            return "OWN_RANK";
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT:
            return "BUY_BANK_SLOT";
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION:
            return "GAIN_REPUTATION";
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
            return "GAIN_EXALTED_REPUTATION";
        case ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP:
            return "VISIT_BARBER_SHOP";
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            return "EQUIP_EPIC_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
            return "ROLL_NEED_ON_LOOT";
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
            return "GREED_ON_LOOT";
        case ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS:
            return "HK_CLASS";
        case ACHIEVEMENT_CRITERIA_TYPE_HK_RACE:
            return "HK_RACE";
        case ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE:
            return "DO_EMOTE";
        case ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE:
            return "HEALING_DONE";
        case ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS:
            return "GET_KILLING_BLOWS";
        case ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM:
            return "EQUIP_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_VENDORS:
            return "MONEY_FROM_VENDORS";
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
            return "GOLD_SPENT_FOR_TALENTS";
        case ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
            return "NUMBER_OF_TALENT_RESETS";
        case ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
            return "MONEY_FROM_QUEST_REWARD";
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
            return "GOLD_SPENT_FOR_TRAVELLING";
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
            return "GOLD_SPENT_AT_BARBER";
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
            return "GOLD_SPENT_FOR_MAIL";
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY:
            return "LOOT_MONEY";
        case ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT:
            return "USE_GAMEOBJECT";
        case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
            return "BE_SPELL_TARGET2";
        case ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL:
            return "SPECIAL_PVP_KILL";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON:
            return "WIN_CHALLENGE_DUNGEON";
        case ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            return "FISH_IN_GAMEOBJECT";
        case ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN:
            return "ON_LOGIN";
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
            return "LEARN_SKILLLINE_SPELLS";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL:
            return "WIN_DUEL";
        case ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL:
            return "LOSE_DUEL";
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE:
            return "KILL_CREATURE_TYPE";
        case ACHIEVEMENT_CRITERIA_TYPE_COOK_RECIPES_GUILD:
            return "COOK_RECIPES_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
            return "GOLD_EARNED_BY_AUCTIONS";
        case ACHIEVEMENT_CRITERIA_TYPE_PET_BATTLE_ACHIEVEMENT_POINTS:
            return "PET_BATTLE_ACHIEVEMENT_POINTS";
        case ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION:
            return "CREATE_AUCTION";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID:
            return "HIGHEST_AUCTION_BID";
        case ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS:
            return "WON_AUCTIONS";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
            return "HIGHEST_AUCTION_SOLD";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
            return "HIGHEST_GOLD_VALUE_OWNED";
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
            return "GAIN_REVERED_REPUTATION";
        case ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
            return "GAIN_HONORED_REPUTATION";
        case ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS:
            return "KNOWN_FACTIONS";
        case ACHIEVEMENT_CRITERIA_TYPE_GATHER_ITEM:
            return "GATHER_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
            return "RECEIVE_EPIC_ITEM";
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED:
            return "ROLL_NEED";
        case ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED:
            return "ROLL_GREED";
        case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET:
            return "COLLECT_BATTLE_PET";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT:
            return "HIT_DEALT";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
            return "HIT_RECEIVED";
        case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
            return "TOTAL_DAMAGE_RECEIVED";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED:
            return "HIGHEST_HEAL_CASTED";
        case ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
            return "TOTAL_HEALING_RECEIVED";
        case ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
            return "HIGHEST_HEALING_RECEIVED";
        case ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED:
            return "QUEST_ABANDONED";
        case ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
            return "FLIGHT_PATHS_TAKEN";
        case ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE:
            return "LOOT_TYPE";
        case ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2:
            return "CAST_SPELL2";
        case ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE:
            return "LEARN_SKILL_LINE";
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL:
            return "EARN_HONORABLE_KILL";
        case ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
            return "ACCEPTED_SUMMONINGS";
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
            return "EARN_ACHIEVEMENT_POINTS";
        case ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
            return "USE_LFD_TO_GROUP_WITH_PLAYERS";
        case ACHIEVEMENT_CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS:
            return "SPENT_GOLD_GUILD_REPAIRS";
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL:
            return "REACH_GUILD_LEVEL";
        case ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD:
            return "CRAFT_ITEMS_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_CATCH_FROM_POOL:
            return "CATCH_FROM_POOL";
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
            return "BUY_GUILD_BANK_SLOTS";
        case ACHIEVEMENT_CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
            return "EARN_GUILD_ACHIEVEMENT_POINTS";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
            return "WIN_RATED_BATTLEGROUND";
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_BG_RATING:
            return "REACH_BG_RATING";
        case ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_TABARD:
            return "BUY_GUILD_TABARD";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
            return "COMPLETE_QUESTS_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
            return "HONORABLE_KILLS_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
            return "KILL_CREATURE_TYPE_GUILD";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            return "GUILD_CHALLENGE_TYPE";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
            return "GUILD_CHALLENGE";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO:
            return "COMPLETE_SCENARIO";
        case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO:
            return "COMPLETE_SPECIFIC_SCENARIO";
        case ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET_SPECIES:
            return "COLLECT_BATTLE_PET_SPECIES";
        case ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
            return "BATTLE_PET_COUNT";
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET:
            return "CAPTURE_BATTLE_PET";
        case ACHIEVEMENT_CRITERIA_TYPE_WIN_PET_BATTLE:
            return "WIN_PET_BATTLE";
        case ACHIEVEMENT_CRITERIA_TYPE_REACH_BATTLE_PET_LEVEL:
            return "REACH_BATTLE_PET_LEVEL";
        case ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET2:
            return "CAPTURE_BATTLE_PET2";
    }
    return "MISSING_TYPE";
}

//==========================================================
void AchievementGlobalMgr::LoadAchievementCriteriaList()
{
    uint32 oldMSTime = getMSTime();

    if (sCriteriaStore.GetNumRows() == 0)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 achievement criteria.");
        return;
    }

    for (uint32 entryId = 0; entryId < sCriteriaTreeStore.GetNumRows(); entryId++)
    {
        CriteriaTreeEntry const* entry = sCriteriaTreeStore.LookupEntry(entryId);
        if (!entry)
            continue;

        std::unique_ptr<CriteriaTreeNode> c{ new CriteriaTreeNode };
        c->Entry = entry;
        c->Achievement = GetAchievementEntryByCriteriaTree(entry);
        m_criteriaTrees.emplace(entryId, std::move(c));
    }

    for (auto&& it : m_criteriaTrees)
    {
        if (!it.second->Entry->Parent)
            continue;

        auto p = m_criteriaTrees.find(it.second->Entry->Parent);
        if (p!= m_criteriaTrees.end())
        {
            p->second->Nodes.push_back(it.second.get());
            while (p != m_criteriaTrees.end())
            {
                auto cur = p;
                p = m_criteriaTrees.find(p->second->Entry->Parent);
                if (p == m_criteriaTrees.end())
                {
                    if (sCriteriaStore.LookupEntry(it.second->Entry->CriteriaID))
                    {
                        if (cur->second.get()->Achievement)
                        {
                            AchievementType type = AchievementType::Player;
                            if (cur->second->Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT)
                                type = AchievementType::Account;
                            else if (cur->second->Achievement->Flags & ACHIEVEMENT_FLAG_GUILD)
                                type = AchievementType::Guild;
                            m_criteriaTreeByCriteriaTyped[size_t(type)][it.second->Entry->CriteriaID].push_back(cur->second.get());
                        }
                        m_criteriaTreeByCriteria[it.second->Entry->CriteriaID].push_back(cur->second.get());
                    }
                }
            }
        }
    }

    for (uint32 entryId = 0; entryId < sModifierTreeStore.GetNumRows(); entryId++)
    {
        ModifierTreeEntry const* entry = sModifierTreeStore.LookupEntry(entryId);
        if (!entry)
            continue;

        std::unique_ptr<ModifierTreeNode> m{ new ModifierTreeNode };
        m->Entry = entry;
        m_criteriaModifiers.emplace(entryId, std::move(m));
    }

    for (auto&& it : m_criteriaModifiers)
    {
        if (!it.second->Entry->Parent)
            continue;

        auto parent = m_criteriaModifiers.find(it.second->Entry->Parent);
        if (parent != m_criteriaModifiers.end())
            parent->second->Nodes.push_back(it.second.get());
    }

    uint32 criterias = 0;
    for (uint32 entryId = 0; entryId < sCriteriaStore.GetNumRows(); entryId++)
    {
        CriteriaEntry const* criteriaEntry = sCriteriaStore.LookupEntry(entryId);
        if (!criteriaEntry)
            continue;

        auto treeItr = m_criteriaTreeByCriteria.find(criteriaEntry->ID);
        if (treeItr == m_criteriaTreeByCriteria.end())
            continue;

        std::unique_ptr<Criteria> criteria{ new Criteria };
        criteria->Entry = criteriaEntry;
        auto it = m_criteriaModifiers.find(criteriaEntry->ModifierTree);
        if (it != m_criteriaModifiers.end())
        {
            criteria->Modifier = it->second.get();
            m_criteriasByModifier[criteriaEntry->ModifierTree] = criteria.get();
        }

        if (criteriaEntry->TimeLimit)
            m_criteriasByTimedType[criteriaEntry->StartEvent].push_back(criteria.get());

        if (criteriaEntry->StartEvent)
            m_criteriasByStartType[criteriaEntry->StartEvent].push_back(criteria.get());

        if (criteriaEntry->FailEvent)
            m_criteriasByResetType[criteriaEntry->FailEvent].push_back(criteria.get());

        bool player = false, account = false, guild = false;
        for (CriteriaTreeNode const* tree : treeItr->second)
        {
            if (AchievementEntry const* achievement = tree->Achievement)
            {
                if (achievement->Flags & ACHIEVEMENT_FLAG_GUILD)
                    guild = true;
                else if (achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT)
                    account = true;
                else
                    player = true;
            }
        }

        if (player)
            m_criteriasByType[size_t(AchievementType::Player)][criteriaEntry->Type].push_back(criteria.get());
        if (account)
            m_criteriasByType[size_t(AchievementType::Account)][criteriaEntry->Type].push_back(criteria.get());
        if (guild)
            m_criteriasByType[size_t(AchievementType::Guild)][criteriaEntry->Type].push_back(criteria.get());

        m_criterias.emplace(entryId, std::move(criteria));
    }

    for (auto& p : m_criteriaTrees)
        p.second->Criteria = GetCriteria(p.second->Entry->CriteriaID);

    // "Praise the Sun!" has MapID 870 (pandaria) but Nalak is in Isle of Thunder =/ just hack it
    // And same with Nalak Guild Run
    for (auto&& id : { 8028, 8137 })
        if (AchievementEntry* achievement = const_cast<AchievementEntry*>(GetAchievement(id)))
            achievement->MapID = 1064;

    // Flame Keeper/Warden of Pandaria, wrong map (530)
    for (auto&& id : { 8044, 8045 })
        if (AchievementEntry* achievement = const_cast<AchievementEntry*>(GetAchievement(id)))
            achievement->MapID = 870;

    // Scarlet Monastery Guild Run - seems to be old map
    if (AchievementEntry* achievement = const_cast<AchievementEntry*>(GetAchievement(5046)))
        achievement->MapID = 1004;

    // 8355 - Weighed Down. Why the fuck it is MODIFIER_TREE_OPERATOR_ANY?
    if (ModifierTreeEntry* modifier = const_cast<ModifierTreeEntry*>(sModifierTreeStore.LookupEntry(11947)))
        modifier->Operator = MODIFIER_TREE_OPERATOR_ALL;
    if (ModifierTreeEntry* modifier = const_cast<ModifierTreeEntry*>(sModifierTreeStore.LookupEntry(11999)))
        modifier->Operator = MODIFIER_TREE_OPERATOR_ALL;

    TC_LOG_INFO("server.loading", ">> Loaded %u achievement criteria in %u ms", ++criterias, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadAchievementReferenceList()
{
    uint32 oldMSTime = getMSTime();

    if (sAchievementStore.GetNumRows() == 0)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 achievement references.");
        return;
    }

    uint32 count = 0;

    for (uint32 entryId = 0; entryId < sAchievementStore.GetNumRows(); ++entryId)
    {
        AchievementEntry const* achievement = sAchievementMgr->GetAchievement(entryId);
        if (!achievement)
            continue;

        m_achievementEntryByCriteriaTree[achievement->CriteriaTree] = achievement;

        if (!achievement->RefAchievement)
            continue;

        m_achievementListByReferencedId[achievement->RefAchievement].push_back(achievement);
        ++count;
    }

    // Once Bitten, Twice Shy (10 player) - Icecrown Citadel
    if (AchievementEntry const* achievement = sAchievementMgr->GetAchievement(4539))
        const_cast<AchievementEntry*>(achievement)->MapID = 631;    // Correct map requirement (currently has Ulduar)

    TC_LOG_INFO("server.loading", ">> Loaded %u achievement references in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadAchievementCriteriaData()
{
    uint32 oldMSTime = getMSTime();

    m_criteriaDataMap.clear();                              // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT criteria_id, type, value1, value2, ScriptName FROM achievement_criteria_data");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 additional achievement criteria data. DB table `achievement_criteria_data` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 criteria_id = fields[0].GetUInt32();

        CriteriaEntry const* criteria = sAchievementMgr->GetAchievementCriteria(criteria_id);

        if (!criteria)
        {
            TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` has data for non-existing criteria (Entry: %u), ignore.", criteria_id);
            continue;
        }

        uint32 dataType = fields[1].GetUInt8();
        std::string scriptName = fields[4].GetString();
        uint32 scriptId = 0;
        if (scriptName.length()) // not empty
        {
            if (dataType != ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT)
                TC_LOG_ERROR("sql.sql", "Table `achievement_criteria_data` has ScriptName set for non-scripted data type (Entry: %u, type %u), useless data.", criteria_id, dataType);
            else
                scriptId = sObjectMgr->GetScriptId(scriptName.c_str());
        }

        AchievementCriteriaData data(dataType, fields[2].GetUInt32(), fields[3].GetUInt32(), scriptId);

        if (!data.IsValid(criteria))
            continue;

        // this will allocate empty data set storage
        AchievementCriteriaDataSet& dataSet = m_criteriaDataMap[criteria_id];
        dataSet.SetCriteriaId(criteria_id);

        // add real data only for not NONE data types
        if (data.dataType != ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE)
            dataSet.Add(data);

        // counting data by and data types
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u additional achievement criteria data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadCompletedAchievements()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT achievement FROM character_achievement GROUP BY achievement UNION SELECT achievement FROM guild_achievement");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 realm first completed achievements. DB table `character_achievement` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint16 achievementId = fields[0].GetUInt16();
        const AchievementEntry* achievement = sAchievementMgr->GetAchievement(achievementId);
        if (!achievement)
        {
            // Remove non existent achievements from all characters
            TC_LOG_ERROR("achievement", "Non-existing achievement %u data removed from table `character_achievement`.", achievementId);

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INVALID_ACHIEVMENT);
            stmt->setUInt16(0, uint16(achievementId));
            CharacterDatabase.Execute(stmt);

            continue;
        }
        else if (achievement->Flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL))
            m_allCompletedAchievements.insert(achievementId);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu realm first completed achievements in %u ms", (unsigned long)m_allCompletedAchievements.size(), GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadRewards()
{
    uint32 oldMSTime = getMSTime();

    m_achievementRewards.clear();                           // need for reload case

    //                                               0      1        2         3      4       5        6     7
    QueryResult result = WorldDatabase.Query("SELECT entry, title_A, title_H, item, spell, sender, subject, text FROM achievement_reward");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 achievement rewards. DB table `achievement_reward` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        const AchievementEntry* achievement = GetAchievement(entry);
        if (!achievement)
        {
            TC_LOG_ERROR("sql.sql", "Table `achievement_reward` has wrong achievement (Entry: %u), ignored.", entry);
            continue;
        }

        AchievementReward reward;
        reward.TitleId[0] = fields[1].GetUInt32();
        reward.TitleId[1] = fields[2].GetUInt32();
        reward.ItemId     = fields[3].GetUInt32();
        reward.SpellId    = fields[4].GetInt32();
        reward.Sender     = fields[5].GetUInt32();
        reward.Subject    = fields[6].GetString();
        reward.Text       = fields[7].GetString();

        // must be title or mail at least
        if (!reward.TitleId[0] && !reward.TitleId[1] && !reward.Sender && !reward.SpellId)
        {
            TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) does not have title or item or spell reward data, ignored.", entry);
            continue;
        }

        if (achievement->RequiredFaction == ACHIEVEMENT_FACTION_ANY && ((reward.TitleId[0] == 0) != (reward.TitleId[1] == 0)))
            TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has title (A: %u H: %u) for only one team.", entry, reward.TitleId[0], reward.TitleId[1]);

        if (reward.TitleId[0])
        {
            CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(reward.TitleId[0]);
            if (!titleEntry)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has invalid title id (%u) in `title_A`, set to 0", entry, reward.TitleId[0]);
                reward.TitleId[0] = 0;
            }
        }

        if (reward.TitleId[1])
        {
            CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(reward.TitleId[1]);
            if (!titleEntry)
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has invalid title id (%u) in `title_H`, set to 0", entry, reward.TitleId[1]);
                reward.TitleId[1] = 0;
            }
        }

        //check mail data before item for report including wrong item case
        if (reward.Sender)
        {
            if (!sObjectMgr->GetCreatureTemplate(reward.Sender))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has invalid creature entry %u as sender, mail reward skipped.", entry, reward.Sender);
                reward.Sender = 0;
            }
        }
        else
        {
            if (reward.ItemId)
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) does not have sender data but has item reward, item will not be rewarded.", entry);

            if (!reward.Subject.empty())
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) does not have sender data but has mail subject.", entry);

            if (!reward.Text.empty())
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) does not have sender data but has mail text.", entry);
        }

        if (reward.ItemId)
        {
            if (!sObjectMgr->GetItemTemplate(reward.ItemId))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has invalid item id %u, reward mail will not contain item.", entry, reward.ItemId);
                reward.ItemId = 0;
            }
        }

        if (reward.SpellId)
        {
            if (!sSpellMgr->GetSpellInfo(std::abs(reward.SpellId)))
            {
                TC_LOG_ERROR("sql.sql", "Table `achievement_reward` (Entry: %u) has invalid spell id %u, skipped.", entry, reward.ItemId);
                reward.SpellId = 0;
            }
        }

        m_achievementRewards[entry] = reward;
        ++count;

        if (reward.TitleId[0] && (achievement->RequiredFaction == -1 || achievement->RequiredFaction == ACHIEVEMENT_FACTION_ALLIANCE))
            m_achievementsWithTitle[0].insert(entry);
        if (reward.TitleId[1] && (achievement->RequiredFaction == -1|| achievement->RequiredFaction == ACHIEVEMENT_FACTION_HORDE))
            m_achievementsWithTitle[1].insert(entry);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u achievement rewards in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void AchievementGlobalMgr::LoadRewardLocales()
{
    uint32 oldMSTime = getMSTime();

    m_achievementRewardLocales.clear();                       // need for reload case

    //                                               0   1       2        3
    QueryResult result = WorldDatabase.Query("SELECT ID, Locale, Subject, Body FROM achievement_reward_locale");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 achievement reward locale strings.  DB table `achievement_reward_locale` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        if (m_achievementRewards.find(id) == m_achievementRewards.end())
        {
            TC_LOG_ERROR("sql.sql", "Table `achievement_reward_locale` (ID: %u) contains locale strings for a non-existing achievement reward.", id);
            continue;
        }

        AchievementRewardLocale& data = m_achievementRewardLocales[id];
        ObjectMgr::AddLocaleString(fields[2].GetString(), locale, data.Subject);
        ObjectMgr::AddLocaleString(fields[3].GetString(), locale, data.Text);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u achievement reward locale strings in %u ms.", uint32(m_achievementRewardLocales.size()), GetMSTimeDiffToNow(oldMSTime));
}

AchievementEntry const* AchievementGlobalMgr::GetAchievement(uint32 achievementId) const
{
    return sAchievementStore.LookupEntry(achievementId);
}

CriteriaEntry const* AchievementGlobalMgr::GetAchievementCriteria(uint32 criteriaId) const
{
    return sCriteriaStore.LookupEntry(criteriaId);
}
