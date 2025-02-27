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

#include "Common.h"
#include "SharedDefines.h"
#include "DBCStores.h"
#include "DisableMgr.h"
#include "ObjectMgr.h"
#include "SocialMgr.h"
#include "LFGMgr.h"
#include "LFGScripts.h"
#include "LFGGroupData.h"
#include "LFGPlayerData.h"
#include "LFGQueue.h"
#include "Group.h"
#include "Player.h"
#include "GroupMgr.h"
#include "GameEventMgr.h"
#include "ReputationMgr.h"
#include "Config.h"
#include "Guild.h"
#include "ScenarioMgr.h"
#include "Chat.h"
#include "GridNotifiers.h"
#include "BattlegroundMgr.h"
#include "Field.h"

template <typename... Args>
static void AlmostAssert(char const* filter, Args... args)
{
    auto str = Format(filter, args...);
}

namespace lfg
{


LFGMgr::LFGMgr(): m_QueueTimer(0), m_ShortageCheckTimer(0), m_lfgProposalId(1),
    m_options(sWorld->getIntConfig(CONFIG_LFG_OPTIONSMASK))
{
    new LFGPlayerScript();
    new LFGGroupScript();
}

LFGMgr::~LFGMgr()
{
    for (LfgRewardContainer::iterator itr = RewardMapStore.begin(); itr != RewardMapStore.end(); ++itr)
        delete itr->second;
}

void LFGMgr::LoadFromDB(Field* fields, ObjectGuid guid)
{
    if (!fields)
        return;

    if (!guid.IsGroup())
        return;

    uint32 dungeon = fields[16].GetUInt32();
    LfgState state = LfgState(fields[17].GetUInt8());

    if (!dungeon)
        return;

    switch (state)
    {
        case LFG_STATE_DUNGEON:
        case LFG_STATE_FINISHED_DUNGEON:
        {
            uint32 queueId = GenerateNewQueueId();
            AddQueue(guid, queueId);
            SetState(guid, queueId, state);
            SetDungeon(guid, dungeon);
            SetLeader(guid, ObjectGuid(HighGuid::Player, fields[0].GetUInt32()));
            break;
        }
        default:
            break;
    }
}

void LFGMgr::SaveToDB(ObjectGuid guid, uint32 dbGuid)
{
    if (!guid.IsGroup())
        return;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_LFG_DATA);

    stmt->setUInt32(0, dbGuid);

    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LFG_DATA);
    stmt->setUInt32(0, dbGuid);

    stmt->setUInt32(1, GetDungeon(guid));
    stmt->setUInt32(2, GetActiveState(guid));

    CharacterDatabase.Execute(stmt);
}

/// Load rewards for completing dungeons
void LFGMgr::LoadRewards()
{
    uint32 oldMSTime = getMSTime();

    for (LfgRewardContainer::iterator itr = RewardMapStore.begin(); itr != RewardMapStore.end(); ++itr)
        delete itr->second;
    RewardMapStore.clear();

    // ORDER BY is very important for GetRandomDungeonReward!
    QueryResult result = WorldDatabase.Query("SELECT dungeonId, maxLevel, firstQuestId, otherQuestId FROM lfg_dungeon_rewards ORDER BY dungeonId, maxLevel ASC");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 lfg dungeon rewards. DB table `lfg_dungeon_rewards` is empty!");
        return;
    }

    uint32 count = 0;

    Field* fields = NULL;
    do
    {
        fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        uint32 maxLevel = fields[1].GetUInt8();
        uint32 firstQuestId = fields[2].GetUInt32();
        uint32 otherQuestId = fields[3].GetUInt32();

        if (!GetLFGDungeonEntry(dungeonId))
        {
            TC_LOG_ERROR("sql.sql", "Dungeon %u specified in table `lfg_dungeon_rewards` does not exist!", dungeonId);
            continue;
        }

        if (!maxLevel || maxLevel > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        {
            TC_LOG_ERROR("sql.sql", "Level %u specified for dungeon %u in table `lfg_dungeon_rewards` can never be reached!", maxLevel, dungeonId);
            maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        }

        if (!firstQuestId || !sObjectMgr->GetQuestTemplate(firstQuestId))
        {
            TC_LOG_ERROR("sql.sql", "First quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", firstQuestId, dungeonId);
            continue;
        }

        if (otherQuestId && !sObjectMgr->GetQuestTemplate(otherQuestId))
        {
            TC_LOG_ERROR("sql.sql", "Other quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", otherQuestId, dungeonId);
            otherQuestId = 0;
        }

        RewardMapStore.insert(LfgRewardContainer::value_type(dungeonId, new LfgReward(maxLevel, firstQuestId, otherQuestId)));
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u lfg dungeon rewards in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

LFGDungeonData const* LFGMgr::GetLFGDungeon(uint32 id)
{
    LFGDungeonContainer::const_iterator itr = LfgDungeonStore.find(id);
    if (itr != LfgDungeonStore.end())
        return &(itr->second);

    return NULL;
}

void LFGMgr::LoadLFGDungeons(bool reload /* = false */)
{
    uint32 oldMSTime = getMSTime();

    LfgDungeonStore.clear();

    // Initialize Dungeon map with data from dbcs
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon)
            continue;

        switch (dungeon->type)
        {
            case LFG_TYPE_DUNGEON:
            case LFG_TYPE_RAID:
            case LFG_TYPE_RANDOM:
                LfgDungeonStore[dungeon->ID] = LFGDungeonData(dungeon);
                break;
        }
    }

    // Fill teleport locations from DB
    QueryResult result = WorldDatabase.Query("SELECT dungeonId, position_x, position_y, position_z, orientation, requiredItemLevel FROM lfg_dungeon_template");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 lfg dungeon templates. DB table `lfg_dungeon_template` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        LFGDungeonContainer::iterator dungeonItr = LfgDungeonStore.find(dungeonId);
        if (dungeonItr == LfgDungeonStore.end())
        {
            TC_LOG_ERROR("sql.sql", "table `lfg_dungeon_template` contains coordinates for wrong dungeon %u", dungeonId);
            continue;
        }

        LFGDungeonData& data   = dungeonItr->second;
        data.x                 = fields[1].GetFloat();
        data.y                 = fields[2].GetFloat();
        data.z                 = fields[3].GetFloat();
        data.o                 = fields[4].GetFloat();
        data.requiredItemLevel = fields[5].GetUInt16();

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u lfg entrance positions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

    // Fill all other teleport coords from areatriggers
    for (LFGDungeonContainer::iterator itr = LfgDungeonStore.begin(); itr != LfgDungeonStore.end(); ++itr)
    {
        LFGDungeonData& dungeon = itr->second;

        // No teleport coords in database, load from areatriggers
        if (dungeon.type != LFG_TYPE_RANDOM && dungeon.x == 0.0f && dungeon.y == 0.0f && dungeon.z == 0.0f &&
            dungeon.group != 38 && dungeon.group != 43) // scenarios should not have entrance trigger
        {
            AreaTriggerStruct const* at = sObjectMgr->GetMapEntranceTrigger(dungeon.map);
            if (!at)
            {
                TC_LOG_ERROR("lfg", "LFGMgr::LoadLFGDungeons: Failed to load dungeon %s (Id: %u), cant find areatrigger for map %u", dungeon.name.c_str(), dungeon.id, dungeon.map);
                continue;
            }

            dungeon.map = at->target_mapId;
            dungeon.x = at->target_X;
            dungeon.y = at->target_Y;
            dungeon.z = at->target_Z;
            dungeon.o = at->target_Orientation;
        }

        if (dungeon.type != LFG_TYPE_RANDOM)
            CachedDungeonMapStore[dungeon.group].insert(dungeon.id);
        CachedDungeonMapStore[0].insert(dungeon.id);
    }

    if (reload)
    {
        CachedDungeonMapStore.clear();
        // Recalculate locked dungeons
        for (LfgPlayerDataContainer::const_iterator it = PlayersStore.begin(); it != PlayersStore.end(); ++it)
            if (Player* player = ObjectAccessor::FindPlayer(it->first))
                InitializeLockedDungeons(player);
    }
}

void LFGMgr::Update(uint32 diff)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    time_t currTime = time(NULL);

    if (sConfigMgr->GetBoolDefault("DEBUG_LFG_QUEUE_SAVE_PREV_STATE", false))
        for (auto&& manager : QueueManagers)
            manager.second.SavePrevState();

    // Remove obsolete role checks
    for (LfgRoleCheckContainer::iterator it = RoleChecksStore.begin(); it != RoleChecksStore.end();)
    {
        LfgRoleCheckContainer::iterator itRoleCheck = it++;
        LfgRoleCheck& roleCheck = itRoleCheck->second;
        if (currTime < roleCheck.cancelTime)
            continue;
        roleCheck.state = LFG_ROLECHECK_MISSING_ROLE;

        ObjectGuid gguid = itRoleCheck->first;
        uint32 queueId = GetActiveQueueId(gguid);
        LfgState oldState = GetOldState(gguid, queueId);

        for (LfgRolesMap::const_iterator itRoles = roleCheck.roles.begin(); itRoles != roleCheck.roles.end(); ++itRoles)
        {
            ObjectGuid guid = itRoles->first;

            for (LfgRolesMap::const_iterator itRoles2 = roleCheck.roles.begin(); itRoles2 != roleCheck.roles.end(); ++itRoles2)
                if (itRoles2->second < PLAYER_ROLE_TANK)
                    SendLfgRoleChosen(guid, itRoles2->first, itRoles2->second);

            SendLfgRoleCheckUpdate(guid, roleCheck);
            if (guid == roleCheck.leader)
                SendLfgJoinResult(guid, queueId, LfgJoinResultData(LFG_JOIN_ROLE_CHECK_FAILED, LFG_ROLECHECK_MISSING_ROLE));
            SendLfgUpdateStatus(LFG_UPDATETYPE_ROLECHECK_FAILED, guid, queueId);
            RestoreState(guid, queueId, "Remove Obsolete RoleCheck");

            if (oldState == LFG_STATE_NONE)
                RemoveQueue(guid, queueId);
        }

        RestoreState(gguid, queueId, "Remove Obsolete RoleCheck");
        if (oldState == LFG_STATE_NONE)
            RemoveQueue(gguid, queueId);
        RoleChecksStore.erase(itRoleCheck);
    }

    // Remove obsolete proposals
    for (LfgProposalContainer::iterator it = ProposalsStore.begin(); it != ProposalsStore.end();)
    {
        LfgProposalContainer::iterator itRemove = it++;
        if (itRemove->second.cancelTime < currTime)
            RemoveProposal(itRemove, LFG_UPDATETYPE_PROPOSAL_FAILED);
    }

    // Remove obsolete kicks
    for (LfgPlayerBootContainer::iterator it = BootsStore.begin(); it != BootsStore.end();)
    {
        LfgPlayerBootContainer::iterator itBoot = it++;
        LfgPlayerBoot& boot = itBoot->second;

        if (boot.cancelTime < currTime)
        {
            boot.inProgress = false;
            for (LfgAnswerContainer::const_iterator itVotes = boot.votes.begin(); itVotes != boot.votes.end(); ++itVotes)
            {
                ObjectGuid pguid = itVotes->first;
                if (pguid != boot.victim)
                    SendLfgBootProposalUpdate(pguid, boot);
                if (GetActiveQueueId(pguid))
                RestoreState(pguid, GetActiveQueueId(pguid), "Remove Obsolete Boot");
            }
            if (GetActiveQueueId(itBoot->first))
            RestoreState(itBoot->first, GetActiveQueueId(itBoot->first), "Remove Obsolete Boot");
            BootsStore.erase(itBoot);
        }
    }

    // Check if a proposal can be formed with the new groups being added
    for (auto&& manager : QueueManagers)
        manager.second.MakeProposals();

    // Force update proposals which has all players accepted without anyone
    // actually accepting the proposal by hand (in the latter case the proposal
    // would've been removed in UpdateProposal called from opcode handler). This
    // happens if a full LFG group queues again after finishing a dungeon.
    for (auto itr = ProposalsStore.begin(); itr != ProposalsStore.end();)
    {
        LfgProposal& proposal = (itr++)->second; // Preincrement the iterator, as it will be removed in UpdateProposal
        if (std::all_of(proposal.players.begin(), proposal.players.end(), [](LfgProposalPlayerContainer::const_reference player) { return player.second.accept == LFG_ANSWER_AGREE; }))
            UpdateProposal(proposal.id, proposal.players.begin()->first, true);
    }

    // Update all players status queue info
    if (m_QueueTimer > LFG_QUEUEUPDATE_INTERVAL)
    {
        m_QueueTimer = 0;
        time_t currTime = time(nullptr);
        for (auto&& manager : QueueManagers)
            manager.second.UpdateQueueTimers(currTime);
    }
    else
        m_QueueTimer += diff;

    // Update all queues shortage data
    if (m_ShortageCheckTimer > sWorld->getIntConfig(CONFIG_LFG_SHORTAGE_CHECK_INTERVAL) * IN_MILLISECONDS)
    {
        m_ShortageCheckTimer = 0;
        for (auto&& it : QueueManagers)
            it.second.UpdateShortageData();
    }
    else
        m_ShortageCheckTimer += diff;
}

/**
    Generate the dungeon lock map for a given player

   @param[in]     player Player we need to initialize the lock status map
*/
void LFGMgr::InitializeLockedDungeons(Player* player, uint8 level /* = 0 */)
{
    ObjectGuid guid = player->GetGUID();
    if (!level)
        level = player->GetLevel();
    uint8 expansion = player->GetSession()->Expansion();
    LfgDungeonSet const& dungeons = GetDungeonsByRandom(0);
    LfgLockMap lock;

    float itemLevel = player->GetAverageItemLevel();

    for (LfgDungeonSet::const_iterator it = dungeons.begin(); it != dungeons.end(); ++it)
    {
        LFGDungeonData const* dungeon = GetLFGDungeon(*it);
        if (!dungeon) // should never happen - We provide a list from sLFGDungeonStore
            continue;

        uint32 lockStatus = 0;
        if (dungeon->faction >= 0 && player->GetTeamId() != !dungeon->faction)
            lockStatus = LFG_LOCKSTATUS_INCOMPATIBLE_FACTION;
        else if (DisableMgr::IsDisabledFor(DISABLE_TYPE_MAP, dungeon->map, player, dungeon->difficulty))
            lockStatus = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (DisableMgr::IsDisabledFor(DISABLE_TYPE_LFG_MAP, dungeon->map, player))
            lockStatus = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (DisableMgr::IsDisabledFor(DISABLE_TYPE_LFG_DUNGEON, dungeon->id, player))
            lockStatus = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (dungeon->expansion > expansion)
            lockStatus = LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        else if (dungeon->minlevel > level)
            lockStatus = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (dungeon->maxlevel && dungeon->maxlevel < level)
            lockStatus = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
        else if (dungeon->seasonal && !IsSeasonActive(dungeon->id))
            lockStatus = LFG_LOCKSTATUS_NOT_IN_SEASON;
        else if (dungeon->requiredItemLevel > itemLevel)
            lockStatus = LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE;
        else if (AccessRequirement const* ar = sObjectMgr->GetAccessRequirement(dungeon->map, Difficulty(dungeon->difficulty)))
        {
            if (ar->achievement && !player->HasAchieved(ar->achievement))
                lockStatus = LFG_LOCKSTATUS_MISSING_ACHIEVEMENT;
            else if (player->GetTeam() == ALLIANCE && ar->quest_A && !player->GetQuestRewardStatus(ar->quest_A))
                lockStatus = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else if (player->GetTeam() == HORDE && ar->quest_H && !player->GetQuestRewardStatus(ar->quest_H))
                lockStatus = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else
                if (ar->item)
                {
                    if (!player->HasItemCount(ar->item) && (!ar->item2 || !player->HasItemCount(ar->item2)))
                        lockStatus = LFG_LOCKSTATUS_MISSING_ITEM;
                }
                else if (ar->item2 && !player->HasItemCount(ar->item2))
                    lockStatus = LFG_LOCKSTATUS_MISSING_ITEM;
        }
        if (dungeon->difficulty != RAID_DIFFICULTY_25MAN_LFR && dungeon->difficulty != SCENARIO_DIFFICULTY_NORMAL && dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL && player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
            lockStatus = LFG_LOCKSTATUS_RAID_LOCKED;

        /* @todo VoA closed if WG is not under team control (LFG_LOCKSTATUS_RAID_LOCKED)
            lockData = LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE;
            lockData = LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL;
            lockData = LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL;
        */

        if ((dungeon->id == 655 || dungeon->id == 588) && player->GetTeam() == ALLIANCE) // Battle on the High Seas (Alliance only)
            lockStatus = LFG_LOCKSTATUS_NOT_IN_SEASON;

        if (lockStatus)
            lock[dungeon->Entry()] = LfgLockInfoData(lockStatus, dungeon->requiredItemLevel, itemLevel);
    }
    SetLockedDungeons(guid, lock);
}

/**
    Adds the player/group to lfg queue. If player is in a group then it is the leader
    of the group tying to join the group. Join conditions are checked before adding
    to the new queue.

   @param[in]     player Player trying to join (or leader of group trying to join)
   @param[in]     roles Player selected roles
   @param[in]     dungeons Dungeons the player/group is applying for
   @param[in]     comment Player selected comment
*/
void LFGMgr::JoinLfg(Player* player, LfgRoles roles, LfgDungeonSet& dungeons, const std::string& comment)
{
    if (!player || !player->GetSession() || dungeons.empty())
        return;

    if (!roles || roles > (lfg::PLAYER_ROLE_LEADER | lfg::PLAYER_ROLE_TANK | lfg::PLAYER_ROLE_HEALER | lfg::PLAYER_ROLE_DAMAGE)) // All roles
        return;

    Group* group = player->GetGroup();
    ObjectGuid guid = player->GetGUID();
    ObjectGuid gguid = group ? group->GetGUID() : guid;
    LfgJoinResultData joinData;
    LfgCategory category = LFG_CATEGORY_NONE;
    LfgGuidSet players;
    uint32 rDungeonId = 0;
    uint32 queueId = GetActiveQueueId(gguid);
    bool isContinue = false;

    // Check if all dungeons are valid
    if (joinData.result == LFG_JOIN_OK)
    {
        for (LfgDungeonSet::const_iterator it = dungeons.begin(); it != dungeons.end() && joinData.result == LFG_JOIN_OK; ++it)
        {
            auto dungeon = GetLFGDungeon(*it);
            if (!dungeon)
            {
                joinData.result = LFG_JOIN_DUNGEON_INVALID;
                continue;
            }

            switch (dungeon->type)
            {
                case LFG_TYPE_RANDOM:
                    if (dungeons.size() > 1)               // Only allow 1 random dungeon
                        joinData.result = LFG_JOIN_DUNGEON_INVALID;
                    else
                        rDungeonId = (*dungeons.begin());
                    break;
                case LFG_TYPE_HEROIC:
                case LFG_TYPE_DUNGEON:
                    break;
                default:
                    TC_LOG_ERROR("lfg", "Wrong dungeon type %u for dungeon %u", dungeon->type, *it);
                    joinData.result = LFG_JOIN_DUNGEON_INVALID;
                    break;
            }
            if (category == LFG_CATEGORY_NONE)
                category = dungeon->category;
            else if (category != dungeon->category)
                joinData.result = LFG_JOIN_MIXED_RAID_DUNGEON;
        }
    }

    uint32 lfrQueues = 0;
    auto queues = GetPlayerQueues(guid);
    if (joinData.result == LFG_JOIN_OK && queues)
    {
        for (auto&& itr : *queues)
        {
            switch (itr.second.State)
            {
                case LFG_STATE_DUNGEON:
                    ASSERT(group && group->isLFGGroup());
                    isContinue = true;
                    queueId = itr.first;
                    break;
                case LFG_STATE_FINISHED_DUNGEON:
                    queueId = itr.first;
                    break;
                case LFG_STATE_QUEUED:
                {
                    if (!sWorld->getBoolConfig(CONFIG_LFG_MULTIQUEUE_ENABLED))
                    {
                        joinData.result = LFG_JOIN_TOO_MANY_QUEUES;
                        break;
                    }
                    if (itr.second.Category == LFG_CATEGORY_LFR)
                    {
                        if (*itr.second.Dungeons.begin() == *dungeons.begin())
                            return;
                        ++lfrQueues;
                    }
                    else if (category == itr.second.Category)
                        return;
                    break;
                }
                default:
                    return;
            }
        }
    }

    if (category == LFG_CATEGORY_LFR && lfrQueues + 1> sWorld->getIntConfig(CONFIG_LFG_MAX_LFR_QUEUES))
        joinData.result = LFG_JOIN_TOO_MANY_QUEUES;

    // Do not allow to change dungeon in the middle of a current dungeon
    if (isContinue)
    {
        dungeons.clear();
        dungeons.insert(GetDungeon(gguid));
    }

    // Check player or group member restrictions
    bool playerOnCooldown = false, partyOnCooldown = false;
    // At first check that dungeons are not empty
    if (dungeons.empty())
        joinData.result = LFG_JOIN_NOT_MEET_REQS;
    // If not always check group, all checks below included for ALL group member
    else if (group)
    {
        if (group->GetMembersCount() > GetGroupSize(group->isRaidGroup()))
            joinData.result = LFG_JOIN_TOO_MUCH_MEMBERS;
        else
        {
            uint8 memberCount = 0;
            for (GroupReference* itr = group->GetFirstMember(); itr && joinData.result == LFG_JOIN_OK; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (member->HasAura(LFG_SPELL_DUNGEON_DESERTER))
                    joinData.result = LFG_JOIN_PARTY_DESERTER;
                else if (member->HasAura(LFG_SPELL_DUNGEON_COOLDOWN) && !isContinue)
                    partyOnCooldown = true; // Cooldown case is handled later, should not apply to LFR and specific dungeons
                else if (member->InBattleground() || member->InArena())
                    joinData.result = LFG_JOIN_USING_BG_SYSTEM;
                else 
                {
                    if (auto queues = GetPlayerQueues(member->GetGUID()))
                    {
                        for (auto&& itr : *queues)
                        {
                            switch (itr.second.State)
                            {
                                case LFG_STATE_DUNGEON:
                                case LFG_STATE_FINISHED_DUNGEON:
                                    if (queueId != itr.first)               // Dude in instance group. Original group's leader do join
                                        joinData.result = LFG_JOIN_FAILED;
                                    break;
                                case LFG_STATE_QUEUED:
                                    if (!sWorld->getBoolConfig(CONFIG_LFG_MULTIQUEUE_ENABLED))
                                        joinData.result = LFG_JOIN_TOO_MANY_QUEUES;
                                    // FIXME but idk
                                    break;
                                default:
                                    joinData.result = LFG_JOIN_FAILED;
                                    break;
                            }
                        }
                    }
                    ++memberCount;
                    players.insert(member->GetGUID());
                }
            }

            if (joinData.result == LFG_JOIN_OK && memberCount != group->GetMembersCount())
                joinData.result = LFG_JOIN_DISCONNECTED;
        }
    }
    // Now single player case
    else if (player->InBattleground() || player->InArena())
        joinData.result = LFG_JOIN_USING_BG_SYSTEM;
    else if (player->HasAura(LFG_SPELL_DUNGEON_DESERTER))
        joinData.result = LFG_JOIN_DESERTER;
    else if (player->HasAura(LFG_SPELL_DUNGEON_COOLDOWN) && !isContinue)
        playerOnCooldown = true; // Cooldown case is handled later, should not apply to LFR and specific dungeons
    else
        players.insert(player->GetGUID());

    bool isRaid = category == LFG_CATEGORY_LFR || LFG_CATEGORY_FLEX;
    if ((playerOnCooldown || partyOnCooldown) && rDungeonId && !isRaid)
        joinData.result = playerOnCooldown ? LFG_JOIN_RANDOM_COOLDOWN : LFG_JOIN_PARTY_RANDOM_COOLDOWN;

    if (joinData.result == LFG_JOIN_OK)
    {
        // Expand random dungeons and check restrictions
        if (rDungeonId)
            dungeons = GetDungeonsByRandom(rDungeonId);

        // if we have lockmap then there are no compatible dungeons
        GetCompatibleDungeons(dungeons, players, joinData.lockmap, isContinue || rDungeonId != 0);
        if (dungeons.empty())
            joinData.result = group ? LFG_JOIN_INTERNAL_ERROR : LFG_JOIN_NOT_MEET_REQS;
    }

    // Can't join. Send result
    if (joinData.result != LFG_JOIN_OK)
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::Join: [" UI64FMTD "] joining with %u members. result: %u", GetGuidForLog(guid), group ? group->GetMembersCount() : 1, joinData.result);
        if (!dungeons.empty())                             // Only should show lockmap when have no dungeons available
            joinData.lockmap.clear();
        player->GetSession()->SendLfgJoinResult(0, joinData);
        return;
    }

    time_t now = time(nullptr);
    // This is queue ID for client and for server
    if (!isContinue)
        queueId = GenerateNewQueueId();

    std::string debugNames = "";
    if (group)                                               // Begin rolecheck
    {
        // Create new rolecheck
        LfgRoleCheck& roleCheck = RoleChecksStore[gguid];
        roleCheck.cancelTime = now + LFG_TIME_ROLECHECK;
        roleCheck.state = LFG_ROLECHECK_INITIALITING;
        roleCheck.leader = guid;
        roleCheck.raid = isRaid;
        roleCheck.dungeons = dungeons;
        roleCheck.rDungeonId = rDungeonId;
        roleCheck.isContinue = isContinue;

        DungeonQueue const& queue = GetQueueManager(gguid).GetQueue(*dungeons.begin());
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*dungeons.begin());
        bool flex = dungeon && dungeon->difficulty == RAID_DIFFICULTY_1025MAN_FLEX;

        roleCheck.neededTanks   = flex ? 0 : queue.GetRoleSlots(PLAYER_ROLE_TANK);
        roleCheck.neededHealers = flex ? 0 : queue.GetRoleSlots(PLAYER_ROLE_HEALER);
        roleCheck.neededDamage  = flex ? group->GetMembersCount() : queue.GetRoleSlots(PLAYER_ROLE_DAMAGE);

        if (rDungeonId)
        {
            dungeons.clear();
            dungeons.insert(rDungeonId);
        }

        if (!isContinue)
            AddQueue(gguid, queueId);
        SetState(gguid, queueId, LFG_STATE_ROLECHECK);
        for (auto&& member : *group)
        {
            ObjectGuid pguid = member->GetGUID();
            if (!isContinue)
            {
                AddQueue(pguid, queueId, gguid);
                SetSelectedDungeons(pguid, queueId, dungeons);
                if (rDungeonId)
                    SetRandomDungeon(pguid, queueId, rDungeonId);
            }
            SetState(pguid, queueId, LFG_STATE_ROLECHECK);
            // Is this still needed?...
            ObjectGuid oldGroup = GetGroup(pguid, queueId);
            if (oldGroup)
            {
                if (oldGroup != gguid)
                {
                    AlmostAssert("LFGMgr::JoinLfg oldGroup != gguid, player: %u, oldGroup: " UI64FMTD ", gguid: " UI64FMTD, member->GetGUID().GetCounter(), oldGroup.GetRawValue(), gguid.GetRawValue());
                    RemovePlayerFromGroup(oldGroup, queueId, pguid);
                }
            }
            AddPlayerToGroup(gguid, queueId, pguid);
            if (pguid == group->GetLeaderGUID())
                SetLeader(gguid, pguid);

            roleCheck.roles[pguid] = 0;
            if (!debugNames.empty())
                debugNames.append(", ");
            debugNames.append(member->GetName());
        }
        // Update leader role
        UpdateRoleCheck(gguid, guid, roles);
    }
    else                                                   // Add player to queue
    {
        AddQueue(guid, queueId);

        LfgRolesMap rolesMap;
        rolesMap[guid] = roles;
        GetQueueManager(guid).AddQueueData({ guid, queueId }, now, dungeons, rolesMap, isContinue);

        if (isContinue)
            AlmostAssert("LFGMgr::JoinLfg - !group && isContinue, player: %u", guid);

        if (rDungeonId)
        {
            dungeons.clear();
            dungeons.insert(rDungeonId);
            SetRandomDungeon(guid, queueId, rDungeonId);
        }

        SetSelectedDungeons(guid, queueId, dungeons);

        // Send update to player
        SetRoles(guid, queueId, roles);
        SetJoinTime(guid, queueId, now);
        auto& queueData = GetPlayerQueueData(guid, queueId);
        player->GetSession()->SendLfgUpdateStatus(LFG_UPDATETYPE_JOIN_QUEUE_INITIAL, queueData);
        player->GetSession()->SendLfgUpdateStatus(LFG_UPDATETYPE_ADDED_TO_QUEUE, queueData);
        player->GetSession()->SendLfgJoinResult(queueId, joinData);
        player->GetSession()->SendLfgUpdateStatus(LFG_UPDATETYPE_ADDED_TO_QUEUE, queueData);
        SetState(gguid, queueId, LFG_STATE_QUEUED);

        if (!isContinue && category != LFG_CATEGORY_SCENARIO)
        {
            uint32 dungeon = isRaid ? *dungeons.begin() : rDungeonId;
            if (dungeon)
                SetRolesForCTAReward(guid, queueId, GetQueueManager(guid).GetRolesForCTAReward(dungeon, LfgRoles(roles & ~PLAYER_ROLE_LEADER)));
        }

        debugNames.append(player->GetName());

        // announce personal
        uint8 role;
        for (auto i : { PLAYER_ROLE_TANK, PLAYER_ROLE_HEALER, PLAYER_ROLE_DAMAGE })
        {
            if (roles & i)
            {
                role = roles & i;
                break;
            }
        }
        if (!roles)
            role = PLAYER_ROLE_DAMAGE;

    }

    if (sLog->ShouldLog("lfg", LOG_LEVEL_DEBUG))
    {
        std::ostringstream o;
        o << "LFGMgr::Join: [" << GetGuidForLog(guid) << "] joined (" << (group ? "group" : "player") << ") Members: " << debugNames.c_str()
          << ". Dungeons (" << uint32(dungeons.size()) << "): " << ConcatenateDungeons(dungeons);
        TC_LOG_DEBUG("lfg", "%s", o.str().c_str());
    }
}

void LFGMgr::RemovePlayerQueues(ObjectGuid guid)
{
    if (auto queues = GetPlayerQueues(guid))
        for (auto&& itr : PlayerQueueDataMap{ *queues })
            if (itr.second.State == LFG_STATE_ROLECHECK || itr.second.State == LFG_STATE_QUEUED || itr.second.State == LFG_STATE_PROPOSAL)
                LeaveLfg(guid, itr.first);
}

void LFGMgr::RemovePlayerQueuesOnPartyFound(ObjectGuid guid, uint32 except)
{
    if (auto queues = GetPlayerQueues(guid))
        for (auto&& itr : PlayerQueueDataMap{ *queues })
            if (itr.first != except)
                LeaveLfg(guid, itr.first);
}

void LFGMgr::RemoveGroupQueues(ObjectGuid guid)
{
    if (auto queues = GetGroupQueues(guid))
        for (auto&& itr : GroupQueueDataMap{ *queues })
            if (itr.second.State == LFG_STATE_ROLECHECK || itr.second.State == LFG_STATE_QUEUED || itr.second.State == LFG_STATE_PROPOSAL)
                LeaveLfg(guid, itr.first);
}

/**
    Leaves Dungeon System. Player/Group is removed from queue, rolechecks, proposals
    or votekicks. Player or group needs to be not NULL and using Dungeon System

   @param[in]     guid Player or group guid
*/
void LFGMgr::LeaveLfg(ObjectGuid guid, uint32 queueId)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::LeaveLfg: [" UI64FMTD "] queue: %u", GetGuidForLog(guid), queueId);

    if (!HasQueueId(guid, queueId))
    {
        TC_LOG_ERROR("lfg", "LFGMgr::LeaveLfg: [" UI64FMTD "] doesn't have queue: %u", GetGuidForLog(guid), queueId);
        return;
    }

    ObjectGuid gguid = guid.IsGroup() ? guid : GetGroup(guid, queueId);
    LfgDungeonSet dungeons;
    if (gguid != guid)
        dungeons = GetSelectedDungeons(guid, queueId);

    LfgState state = GetState(guid, queueId);
    if (state == LFG_STATE_ROLECHECK || state == LFG_STATE_BOOT)
    {
        LfgState oldState = GetOldState(guid, queueId);  // Get real state when in rolecheck/boot overlay state
        if (state == LFG_STATE_ROLECHECK)
            UpdateRoleCheck(gguid);    // No player to update role = LFG_ROLECHECK_ABORTED
        if (gguid && state == LFG_STATE_BOOT)
        {
            // Abort boot now, otherwise when it ends or expires all votes will have their state reverted to OldState,
            // and a player might have already left the party by then and queued again
            auto itr = BootsStore.find(gguid);
            if (itr != BootsStore.end())
            {
                LfgPlayerBoot& boot = itr->second;
                boot.inProgress = false;
                for (auto&& vote : boot.votes)
                {
                    if (vote.first != boot.victim)
                        SendLfgBootProposalUpdate(vote.first, boot);
                    RestoreState(vote.first, queueId, "Boot Cancelled (a player left the group)");
                }
                RestoreState(gguid, queueId, "Boot Cancelled (a player left the group)");
                BootsStore.erase(itr);
            }
        }
        state = oldState;
    }

    switch (state)
    {
        case LFG_STATE_QUEUED:
            if (gguid)
            {
                LfgState state = GetOldState(gguid, queueId);

                GetQueueManager(gguid).RemoveFromQueue(Queuer{ gguid, queueId });
                SetState(gguid, queueId, state);
                const LfgGuidSet& players = GetPlayers(gguid);
                for (auto&& guid : players)
                {
                    SendLfgUpdateStatus(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, guid, queueId);
                    SetState(guid, queueId, state);
                    if (state == LFG_STATE_NONE)
                        RemoveQueue(guid, queueId);
                }
                if (state == LFG_STATE_NONE)
                    RemoveQueue(gguid, queueId);
            }
            else
            {
                SendLfgUpdateStatus(LFG_UPDATETYPE_PROPOSAL_BEGIN, guid, queueId);
                SendLfgUpdateStatus(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, guid, queueId);
                GetQueueManager(guid).RemoveFromQueue(Queuer{ guid, queueId });
                SetState(guid, queueId, LFG_STATE_NONE);
                RemoveQueue(guid, queueId);
            }
            break;
        case LFG_STATE_PROPOSAL:
        {
            // Remove from Proposals
            LfgProposalContainer::iterator it = ProposalsStore.begin();
            ObjectGuid pguid = gguid == guid ? GetLeader(gguid) : guid;
            while (it != ProposalsStore.end())
            {
                LfgProposalPlayerContainer::iterator itPlayer = it->second.players.find(pguid);
                if (itPlayer != it->second.players.end())
                {
                    // Mark the player/leader of group who left as didn't accept the proposal
                    itPlayer->second.accept = LFG_ANSWER_DENY;
                    break;
                }
                ++it;
            }

            // Remove from queue - if proposal is found, RemoveProposal will call RemoveFromQueue
            if (it != ProposalsStore.end())
                RemoveProposal(it, LFG_UPDATETYPE_PROPOSAL_DECLINED);
            break;
        }
        case LFG_STATE_NONE:
            break;
        case LFG_STATE_DUNGEON:
        case LFG_STATE_FINISHED_DUNGEON:
        case LFG_STATE_BOOT:
        case LFG_STATE_ROLECHECK:
            break;
    }

}

/**
   Update the Role check info with the player selected role.

   @param[in]     grp Group guid to update rolecheck
   @param[in]     guid Player guid (0 = rolecheck failed)
   @param[in]     roles Player selected roles
*/
void LFGMgr::UpdateRoleCheck(ObjectGuid gguid, ObjectGuid guid /* = 0 */, uint8 roles /* = PLAYER_ROLE_NONE */)
{
    ASSERT(gguid);

    // One rolecheck per time
    LfgRolesMap check_roles;
    LfgRoleCheckContainer::iterator itRoleCheck = RoleChecksStore.find(gguid);
    if (itRoleCheck == RoleChecksStore.end())
        return;

    Group const* group = sGroupMgr->GetGroupByGUID(gguid);
    ASSERT(group);

    LfgRoleCheck& roleCheck = itRoleCheck->second;
    uint32 queueId = GetActiveQueueId(gguid);
    bool sendRoleChosen = roleCheck.state != LFG_ROLECHECK_DEFAULT && guid;

    LfgDungeonSet dungeons;
    if (roleCheck.rDungeonId)
        dungeons.insert(roleCheck.rDungeonId);
    else
        dungeons = roleCheck.dungeons;

    Difficulty difficulty = GetLFGDungeon(*dungeons.begin())->difficulty;

    if (!guid)
        roleCheck.state = LFG_ROLECHECK_ABORTED;
    else if (roles < PLAYER_ROLE_TANK)                            // Player selected no role.
        roleCheck.state = LFG_ROLECHECK_NO_ROLE;
    else
    {
        roleCheck.roles[guid] = roles;

        // Check if all players have selected a role
        LfgRolesMap::const_iterator itRoles = roleCheck.roles.begin();
        while (itRoles != roleCheck.roles.end() && itRoles->second != PLAYER_ROLE_NONE)
            ++itRoles;

        if (itRoles == roleCheck.roles.end())
        {
            // use temporal var to check roles, CheckGroupRoles modifies the roles
            check_roles = roleCheck.roles;

            auto check = roleCheck.raid ? CheckRaidRoles : CheckGroupRoles;
            if (difficulty == RAID_DIFFICULTY_1025MAN_FLEX || difficulty == SCENARIO_DIFFICULTY_NORMAL || difficulty == SCENARIO_DIFFICULTY_HEROIC)
                check = CheckDpsOnly;
            roleCheck.state = check(check_roles, roleCheck.neededTanks, roleCheck.neededHealers, roleCheck.neededDamage) ? LFG_ROLECHECK_FINISHED : LFG_ROLECHECK_WRONG_ROLES;
        }
    }

    LfgJoinResult joinResult = LFG_JOIN_FAILED;
    if (roleCheck.state == LFG_ROLECHECK_MISSING_ROLE || roleCheck.state == LFG_ROLECHECK_WRONG_ROLES)
        joinResult = LFG_JOIN_ROLE_CHECK_FAILED;

    LfgJoinResultData joinData = LfgJoinResultData(joinResult, roleCheck.state);
    for (LfgRolesMap::const_iterator it = roleCheck.roles.begin(); it != roleCheck.roles.end(); ++it)
    {
        ObjectGuid pguid = it->first;

        if (sendRoleChosen)
            SendLfgRoleChosen(pguid, guid, roles);

        SendLfgRoleCheckUpdate(pguid, roleCheck);
        switch (roleCheck.state)
        {
            case LFG_ROLECHECK_INITIALITING:
                continue;
            case LFG_ROLECHECK_FINISHED:
                break;  // Code below
            default:
                if (roleCheck.leader == pguid)
                    SendLfgJoinResult(pguid, queueId, joinData);
                SendLfgUpdateStatus(LFG_UPDATETYPE_ROLECHECK_FAILED, pguid, queueId);
                RestoreState(pguid, queueId, "Rolecheck Failed");
                if (GetState(pguid, queueId) == LFG_STATE_NONE)
                    RemoveQueue(pguid, queueId);
                break;
        }
    }

    if (roleCheck.state == LFG_ROLECHECK_FINISHED)
    {
        time_t now = time(nullptr);

        if (difficulty == RAID_DIFFICULTY_1025MAN_FLEX) // Flex doesn't use queue
        {
            LfgProposal proposal{};
            proposal.isNew = true;
            proposal.dungeonId = *dungeons.begin();
            proposal.state = LFG_PROPOSAL_INITIATING;
            proposal.cancelTime = now + LFG_TIME_PROPOSAL;
            proposal.raid = true;

            if (group->isLFGGroup())
            {
                proposal.isNew = sLFGMgr->GetOldState(gguid, queueId) != LFG_STATE_DUNGEON;
                proposal.group = gguid;
            }
            proposal.leader = roleCheck.leader;

            for (auto&& itr : roleCheck.roles)
            {
                auto& player = proposal.players[itr.first];
                player.role = itr.second;
                player.group = group->GetGUID();
                player.queueId = queueId;
                if (group->isLFGGroup())
                    player.accept = LFG_ANSWER_AGREE;
            }
            AddProposal(proposal);
        }
        else // Normal LFG
        {
            SetState(gguid, queueId, LFG_STATE_QUEUED);
            GetQueueManager(gguid).AddQueueData({ gguid, queueId }, now, roleCheck.dungeons, roleCheck.roles, roleCheck.isContinue);

            for (auto&& it : roleCheck.roles)
            {
                ObjectGuid pguid = it.first;
                if (!roleCheck.isContinue)
                    SetJoinTime(pguid, queueId, now);
                SetState(pguid, queueId, LFG_STATE_QUEUED);
                SetRoles(pguid, queueId, LfgRoles(it.second));
                SendLfgUpdateStatus(LFG_UPDATETYPE_JOIN_QUEUE_INITIAL, pguid, queueId);
                SendLfgUpdateStatus(LFG_UPDATETYPE_ADDED_TO_QUEUE, pguid, queueId);
                if (Player* player = ObjectAccessor::FindConnectedPlayer(pguid))
                    if (group && group->IsLeader(pguid))
                        player->GetSession()->SendLfgJoinResult(queueId, joinData);
                SendLfgUpdateStatus(LFG_UPDATETYPE_ADDED_TO_QUEUE, pguid, queueId);
            }
        }

        RoleChecksStore.erase(itRoleCheck);
    }
    else if (roleCheck.state != LFG_ROLECHECK_INITIALITING)
    {
        // If initial rolecheck failed - remove group data
        RestoreState(gguid, queueId, "Rolecheck Failed");
        if (GetState(gguid, queueId) == LFG_STATE_NONE)
            RemoveQueue(gguid, queueId);
        RoleChecksStore.erase(itRoleCheck);
    }
}

/**
   Given a list of dungeons remove the dungeons players have restrictions.

   @param[in, out] dungeons Dungeons to check restrictions
   @param[in]     players Set of players to check their dungeon restrictions
   @param[out]    lockMap Map of players Lock status info of given dungeons (Empty if dungeons is not empty)
*/
void LFGMgr::GetCompatibleDungeons(LfgDungeonSet& dungeons, LfgGuidSet const& players, LfgLockPartyMap& lockMap, bool randomDungeon)
{
    lockMap.clear();
    for (LfgGuidSet::const_iterator it = players.begin(); it != players.end() && !dungeons.empty(); ++it)
    {
        ObjectGuid guid = (*it);
        LfgLockMap const& cachedLockMap = GetLockedDungeons(guid);
        for (LfgLockMap::const_iterator it2 = cachedLockMap.begin(); it2 != cachedLockMap.end() && !dungeons.empty(); ++it2)
        {
            uint32 dungeonId = (it2->first & 0x00FFFFFF); // Compare dungeon ids
            LfgDungeonSet::iterator itDungeon = dungeons.find(dungeonId);
            if (itDungeon != dungeons.end())
            {
                LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId);
                if (randomDungeon && it2->second.lockStatus == LFG_LOCKSTATUS_RAID_LOCKED)
                    if (Player* player = ObjectAccessor::FindPlayer(guid))
                        if (dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL && player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
                            continue;
                dungeons.erase(itDungeon);
                lockMap[guid][dungeonId] = it2->second;
            }
        }
    }

    if (randomDungeon)
    {
        for (auto it = dungeons.begin(); it != dungeons.end();)
        {
            uint32 dungeonId = *it;
            ++it;
            if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(dungeonId))
                if (dungeon->dpsNeeded == 1)
                    dungeons.erase(dungeonId);
        }
    }

    if (!dungeons.empty())
        lockMap.clear();
}

/**
   Check if a group can be formed with the given group roles

   @param[in]     groles Map of roles to check
   @return True if roles are compatible
*/
bool LFGMgr::CheckGroupRoles(LfgRolesMap& groles, uint8 neededTanks, uint8 neededHealers, uint8 neededDamage)
{
    if (groles.empty())
        return false;

    uint8 damage = 0;
    uint8 tank = 0;
    uint8 healer = 0;

    for (LfgRolesMap::iterator it = groles.begin(); it != groles.end(); ++it)
    {
        uint8 role = it->second & ~PLAYER_ROLE_LEADER;
        if (role == PLAYER_ROLE_NONE)
            return false;

        if (role & PLAYER_ROLE_DAMAGE)
        {
            if (role != PLAYER_ROLE_DAMAGE)
            {
                it->second -= PLAYER_ROLE_DAMAGE;
                if (CheckGroupRoles(groles, neededTanks, neededHealers, neededDamage))
                    return true;
                it->second += PLAYER_ROLE_DAMAGE;
            }
            else if (damage == neededDamage)
                return false;
            else
                damage++;
        }

        if (role & PLAYER_ROLE_HEALER)
        {
            if (role != PLAYER_ROLE_HEALER)
            {
                it->second -= PLAYER_ROLE_HEALER;
                if (CheckGroupRoles(groles, neededTanks, neededHealers, neededDamage))
                    return true;
                it->second += PLAYER_ROLE_HEALER;
            }
            else if (healer == neededHealers)
                return false;
            else
                healer++;
        }

        if (role & PLAYER_ROLE_TANK)
        {
            if (role != PLAYER_ROLE_TANK)
            {
                it->second -= PLAYER_ROLE_TANK;
                if (CheckGroupRoles(groles, neededTanks, neededHealers, neededDamage))
                    return true;
                it->second += PLAYER_ROLE_TANK;
            }
            else if (tank == neededTanks)
                return false;
            else
                tank++;
        }
    }
    return (tank + healer + damage) == uint8(groles.size());
}

// Version of the above for LFR without recursion which would be deadly to the server when requiring to check combinations of 25 players
bool CheckRaidRolesMethod(LfgRolesMap& groles, uint8 neededTanks, uint8 neededHealers, uint8 neededDamage, uint8 method)
{
    uint8 damage = 0;
    uint8 tank = 0;
    uint8 healer = 0;

    // Copy map because we'll be modifying it and we don't want to leave the real data in an intermediate state
    LfgRolesMap members = groles;

    // Helper lambda for assigning a role to a member, if they fit, and incrementing the role counter
    auto const assignRole = [](LfgRolesMap::reference& member, LfgRoles role, uint8& current, uint8 const& needed)
    {
        if (current < needed)
        {
            ++current;
            member.second = role;
            return true;
        }
        return false;
    };

    // Prepare some containers which are explained later
    uint8 totalQueuedPlayers = groles.size();
    using RolePointers = std::vector<LfgRolesMap::pointer>;
    RolePointers thd, th_, t_d, _hd;
    thd.reserve(totalQueuedPlayers);
    th_.reserve(totalQueuedPlayers);
    t_d.reserve(totalQueuedPlayers);
    _hd.reserve(totalQueuedPlayers);

    // First fill slots with players who opted only for a single role
    for (auto&& member : members)
    {
        member.second &= ~PLAYER_ROLE_LEADER; // Clear this flag to ease calculations, we will restore it at the end
        switch (member.second)
        {
            // These members we have no other choice but to assign to the only role they picked or to fail the check if they don't fit
            case PLAYER_ROLE_TANK:   if (!assignRole(member, PLAYER_ROLE_TANK, tank, neededTanks)) return false; break;
            case PLAYER_ROLE_HEALER: if (!assignRole(member, PLAYER_ROLE_HEALER, healer, neededHealers)) return false; break;
            case PLAYER_ROLE_DAMAGE: if (!assignRole(member, PLAYER_ROLE_DAMAGE, damage, neededDamage)) return false; break;
            // These members we can slot in multiple ways, let's save them to handy subsets so that we don't have to iterate through all 25 members on every fill pass
            case PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE: thd.push_back(&member); break;
            case PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER:                      th_.push_back(&member); break;
            case PLAYER_ROLE_TANK | PLAYER_ROLE_DAMAGE:                      t_d.push_back(&member); break;
            case PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE:                    _hd.push_back(&member); break;
            // And this should just not be possible
            case PLAYER_ROLE_NONE: return false;
        }
    }

    // Helper lambda for performing a single fill pass for a given role
    auto const pass = [&assignRole](RolePointers& subset, LfgRoles role, uint8& current, uint8 const& needed)
    {
        // Try to assign players to the role and remove them from the subset in case we succeed
        for (auto itr = subset.begin(); current < needed && itr != subset.end(); ++itr)
            if (*itr && assignRole(**itr, role, current, needed))
                *itr = nullptr; // Faster than erasing from a vector
    };

    // Three different ways of filling tank/healer spots. The only thing that changes is the order.
    switch (method)
    {
        case 0:
            pass(t_d, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots, choosing among least-valuable players (who picked tank|damage)
            pass(_hd, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots, choosing among least-valuable players (who picked healer|damage)
            pass(th_, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots again, now choosing among players who picked tank|healer
            pass(th_, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots again, now choosing among players who picked tank|healer
            break;
        // Because players can pick tank|healer roles, which are the most important to fill, but smallest in required quantity,
        // in case tank or healer roles are clogged with players that picked tank|damage or healer|damage roles (method #0), tank|healer players wouldn't have a spot left.
        // Methods #1 and #2 solve this by trying to fit tank|healer players first, but tank|damage or healer|damage players might not find a spot for them. Hence 2 different ways with either TD or TH first.
        case 1:
            pass(th_, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots, choosing among players who picked tank|healer
            pass(th_, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots, choosing among players who picked tank|healer
            pass(t_d, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots again, now choosing among players who picked tank|damage
            pass(_hd, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots again, now choosing among players who picked healer|damage
            break;
        case 2:
            pass(th_, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots, choosing among players who picked tank|healer
            pass(th_, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots, choosing among players who picked tank|healer
            pass(t_d, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots again, now choosing among players who picked tank|damage
            pass(_hd, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots again, now choosing among players who picked healer|damage
            break;
    }
    pass(thd, PLAYER_ROLE_TANK, tank, neededTanks); // Fill tank slots again, now choosing among players who picked tank|healer|damage
    pass(thd, PLAYER_ROLE_HEALER, healer, neededHealers); // Fill healer slots again, now choosing among players who picked tank|healer|damage
    // Ideally this should follow, but it's an unnecessary effort: we can assume that all players who lack a singular role at this point should be damage. If there are still players, who didn't pick damage, this combination is incompatible.
    //pass(t_d, PLAYER_ROLE_DAMAGE, damage, neededDamage ); // Fill damage slots
    //pass(_hd, PLAYER_ROLE_DAMAGE, damage, neededDamage ); // Fill damage slots
    //pass(thd, PLAYER_ROLE_DAMAGE, damage, neededDamage ); // Fill damage slots

    // Restore leader flag and set remaining roles to damage
    for (auto&& member : members)
    {
        switch (member.second)
        {
            // Members that now have either of these roles have already been assigned and counted towards that role
            case PLAYER_ROLE_TANK:
            case PLAYER_ROLE_HEALER:
            case PLAYER_ROLE_DAMAGE:
                break;
            // The rest should all be eligible for damage role. If we still have any unassigned tank|healer players - they failed to be slotted
            case PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER:
                return false;
            // Assign damage role to the rest
            default:
                if (++damage > neededDamage)
                    return false; // Too many players queued for damage role
                member.second = PLAYER_ROLE_DAMAGE;
                break;
        }
        if (groles[member.first] & PLAYER_ROLE_LEADER)
            member.second |= PLAYER_ROLE_LEADER;
    }

    if (tank + healer + damage != totalQueuedPlayers)
        return false; // Not all players were slotted

    // Success
    groles = members;
    return true;
}

bool LFGMgr::CheckRaidRoles(LfgRolesMap& groles, uint8 neededTanks, uint8 neededHealers, uint8 neededDamage)
{
    for (uint8 method = 0; method <= 2; ++method)
        if (CheckRaidRolesMethod(groles, neededTanks, neededHealers, neededDamage, method))
            return true;

    return false;
}

bool LFGMgr::CheckDpsOnly(LfgRolesMap& groles, uint8 neededTanks, uint8 neededHealers, uint8 neededDamage)
{
    uint32 damagers = 0;
    for (auto&& role : groles)
    {
        if (damagers < neededDamage)
        {
            role.second = PLAYER_ROLE_DAMAGE;
            ++damagers;
        }
    }
    return damagers == groles.size();
}

/**
   Makes a new group given a proposal
   @param[in]     proposal Proposal to get info from
*/
void LFGMgr::MakeNewGroup(LfgProposal const& proposal)
{
    struct ProposalPlayer
    {
        ObjectGuid guid;
        uint32 queueId;
    };

    std::list<ProposalPlayer> players;
    LfgGuidList playersToTeleport;
    uint8 soloJoinedPlayersCount = 0;

    for (auto&& itr : proposal.players)
    {
        ObjectGuid guid = itr.first;
        if (guid == proposal.leader)
            players.push_front({ guid, itr.second.queueId });
        else
            players.push_back({ guid, itr.second.queueId });

        if (proposal.isNew || GetGroup(guid, itr.second.queueId) != proposal.group)
            playersToTeleport.push_back(guid);

        if (!itr.second.group)
            ++soloJoinedPlayersCount;
    }

    // Set the dungeon difficulty
    LFGDungeonData const* dungeon = GetLFGDungeon(proposal.dungeonId);
    ASSERT(dungeon);

    Group* lfgGroup = proposal.group ? sGroupMgr->GetGroupByGUID(proposal.group.GetCounter()) : nullptr;
    bool groupAlreadyExisted = lfgGroup != nullptr;
    bool isContinue = false;
    bool reuseGroup = false;
    uint32 newQueueId = 0;
    if (lfgGroup && lfgGroup->isLFGGroup())
    {
        if (uint32 queueId = GetActiveQueueId(proposal.group))
        {
            auto oldState = GetOldState(proposal.group, queueId);
            isContinue = oldState == LFG_STATE_DUNGEON;
            reuseGroup = isContinue;
            if (!reuseGroup)
                if (auto queues = GetGroupQueues(proposal.group))
                    for (auto&& itr : *queues)
                        if (itr.second.OldState == LFG_STATE_FINISHED_DUNGEON)
                            reuseGroup = true;
            if (reuseGroup)
                newQueueId = queueId;
            else
                AlmostAssert("LFGMgr::MakeNewGroup lfgGroup: [" UI64FMTD "] not reused", lfgGroup->GetGUID());
        }
    }

    if (reuseGroup)
    {
        SetState(proposal.group, newQueueId, LFG_STATE_DUNGEON);
        if (!isContinue)
            RemoveFinishedDungeons(proposal.group);
    }
    else
    {
        newQueueId = GenerateNewQueueId();
        lfgGroup = nullptr;
    }

    TC_LOG_DEBUG("lfg", "LFGMgr::MakeNewGroup: isContinue: %u, newQueueId: %u", uint32(isContinue), newQueueId);

    std::vector<std::pair<ObjectGuid, uint32>> groupsToRemove;

    for (auto&& itr : players)
    {
        ObjectGuid pguid = itr.guid;
        uint32 oldQueueId = itr.queueId;

        Player* player = ObjectAccessor::FindConnectedPlayer(pguid);
        if (!player)
        {
            TC_LOG_DEBUG("lfg", "LFGMgr::MakeNewGroup: Player [%u] not found", pguid.GetCounter());
            continue;
        }

        SetState(pguid, oldQueueId, LFG_STATE_DUNGEON);

        bool isNew = true;
        Group* instanceGroup = player->GetGroup(GroupSlot::Instance);
        Group* originalGroup = player->GetGroup(GroupSlot::Original);
        if (instanceGroup && instanceGroup->isLFGGroup())
        {
            if (lfgGroup)
            {
                if (lfgGroup != instanceGroup)
                    AlmostAssert("LFGMgr::MakeNewGroup grp != group, player %u, lfgGroup: [" UI64FMTD "], instanceGroup: [" UI64FMTD "]", player->GetGUID().GetCounter(), lfgGroup->GetGUID().GetRawValue(), instanceGroup->GetGUID().GetRawValue());
            }
        }

        // Before group creation.
        // This queue moved to new ID, same as ID for lfg group. But client queue ID still be the same
        ReformQueue(itr.guid, oldQueueId, newQueueId);
        RemoveFinishedDungeons(itr.guid);

        RemovePlayerQueuesOnPartyFound(itr.guid, newQueueId);

        for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            if (BattlegroundQueueTypeId queueTypeId = player->GetBattlegroundQueueTypeId(i))
                sBattlegroundMgr->RemovePlayerFromQueue(player, queueTypeId);

        if (!lfgGroup)
        {
            lfgGroup = new Group();
            lfgGroup->ConvertToLFG(dungeon->difficulty == RAID_DIFFICULTY_1025MAN_FLEX);
            lfgGroup->Create(player);
            ObjectGuid gguid = lfgGroup->GetGUID();
            TC_LOG_DEBUG("lfg", "LFGMgr::MakeNewGroup: new group created: [" UI64FMTD "]", gguid.GetRawValue());
            AddQueue(gguid, newQueueId);
            SetState(gguid, newQueueId, LFG_STATE_DUNGEON);
        }

        // Still fucked up. We need to store group in queue data to be sure
        if (instanceGroup && instanceGroup != lfgGroup && HasQueueId(instanceGroup->GetGUID(), oldQueueId))
            RemoveQueue(instanceGroup->GetGUID(), oldQueueId);
        if (originalGroup && originalGroup != lfgGroup && HasQueueId(originalGroup->GetGUID(), oldQueueId))
            RemoveQueue(originalGroup->GetGUID(), oldQueueId);

        if (instanceGroup != lfgGroup)
        {
            if (instanceGroup && instanceGroup != lfgGroup)
                instanceGroup->RemoveMember(player->GetGUID());

            if (lfgGroup->GetLeaderGUID() == pguid)  // Already in group due Group::Create
                SetLeader(lfgGroup->GetGUID(), player->GetGUID());
            else                                // Nope, add new player to group
                lfgGroup->AddMember(player);

            AddPlayerToGroup(lfgGroup->GetGUID(), newQueueId, player->GetGUID());
            if (proposal.raid && !lfgGroup->isRaidGroup() && lfgGroup->GetMembersCount() > 1)
                lfgGroup->ConvertToRaid();
        }

        lfgGroup->SetLfgRoles(pguid, proposal.players.find(pguid)->second.role);

        if (!proposal.raid)
        {
            // Add the cooldown spell if queued for a random dungeon
            LfgDungeonSet const& dungeons = GetSelectedDungeons(pguid, newQueueId);
            if (isNew && !dungeons.empty())
                if (LFGDungeonData const* selectedDungeon = GetLFGDungeon(*dungeons.begin()))
                    if (selectedDungeon->type == LFG_TYPE_RANDOM)
                        player->CastSpell(player, LFG_SPELL_DUNGEON_COOLDOWN, true);

            // Reapply Luck of the Draw to players inside the dungeon
            if (player->GetMapId() == dungeon->map)
            {
                player->RemoveAurasDueToSpell(lfg::LFG_SPELL_LUCK_OF_THE_DRAW);
                for (uint8 i = 0; i < soloJoinedPlayersCount; ++i)
                    player->CastSpell(player, lfg::LFG_SPELL_LUCK_OF_THE_DRAW, true);
            }
        }
    }

    lfgGroup->SaveRolesToDB();

    if (proposal.raid)
    {
        if (!isContinue)
            lfgGroup->UnbindInstance(dungeon->map, 0);
        lfgGroup->SetDungeonDifficulty(Difficulty(REGULAR_DIFFICULTY));
        lfgGroup->SetRaidDifficulty(Difficulty(dungeon->difficulty));
    }
    else
    {
        if (!isContinue)
            lfgGroup->UnbindInstance(dungeon->map, Difficulty(dungeon->difficulty));
        lfgGroup->SetDungeonDifficulty(Difficulty(dungeon->difficulty));
        lfgGroup->SetRaidDifficulty(Difficulty(REGULAR_DIFFICULTY));
    }

    ObjectGuid gguid = lfgGroup->GetGUID();
    SetDungeon(gguid, dungeon->Entry());
    SetSoloJoinedPlayersCount(gguid, soloJoinedPlayersCount);
    if (!isContinue)
        SetKicksLeft(gguid, proposal.raid ? LFG_RAID_MAX_KICKS : LFG_GROUP_MAX_KICKS);

    SaveToDB(gguid, lfgGroup->GetDbStoreId());

    // Teleport Player
    bool forceChangeInstance = isContinue ? false : groupAlreadyExisted;
    for (LfgGuidList::const_iterator it = playersToTeleport.begin(); it != playersToTeleport.end(); ++it)
        if (Player* player = ObjectAccessor::FindPlayer(*it))
            if (player->GetMapId() != dungeon->map || forceChangeInstance)
                TeleportPlayer(player, false, false, forceChangeInstance);

    // Update group info
    lfgGroup->SendUpdate();
}

bool LFGMgr::AddProposal(LfgProposal& proposal)
{
    for (auto&& itr : proposal.queuers)
        if (GetActiveState(itr.GetGUID()) == LFG_STATE_PROPOSAL)
            return false;

    proposal.id = ++m_lfgProposalId;
    ProposalsStore[m_lfgProposalId] = proposal;

    // Announce proposal to queuers
    for (auto&& it : proposal.players)
    {
        ObjectGuid guid = it.first;
        uint32 queueId = it.second.queueId;

        // LFG_STATE_PROPOSAL sets active queueId. Queue id for each player in group is equal to group's queue id.
        SetState(guid, queueId, LFG_STATE_PROPOSAL);
        ObjectGuid gguid = GetGroup(guid, queueId);
        // GetGroup returns active group i.e. lfg if we are in dungeon or group in which player was registered.
        // And if lfg group doesn't have this queue it means that this proposal for another queue of original group...
        // At least if another queues should exist during dungeon in progress...
        if (gguid && !HasQueueId(gguid, queueId))
            gguid = GetPlayerQueueData(guid, queueId).OriginalGroup;

        if (gguid && GetState(gguid, queueId) != LFG_STATE_PROPOSAL) // Prevent spam to log
            SetState(gguid, queueId, LFG_STATE_PROPOSAL);

        SendLfgUpdateStatus(LFG_UPDATETYPE_PROPOSAL_BEGIN, guid, queueId);
        SendLfgUpdateProposal(guid, proposal);
    }
    return true;
}

/**
   Update Proposal info with player answer

   @param[in]     proposalId Proposal id to be updated
   @param[in]     guid Player guid to update answer
   @param[in]     accept Player answer
*/
void LFGMgr::UpdateProposal(uint32 proposalId, ObjectGuid guid, bool accept)
{
    // Check if the proposal exists
    LfgProposalContainer::iterator itProposal = ProposalsStore.find(proposalId);
    if (itProposal == ProposalsStore.end())
        return;

    LfgProposal& proposal = itProposal->second;

    // Check if proposal have the current player
    LfgProposalPlayerContainer::iterator itProposalPlayer = proposal.players.find(guid);
    if (itProposalPlayer == proposal.players.end())
        return;

    LfgProposalPlayer& player = itProposalPlayer->second;
    player.accept = LfgAnswer(accept);

    TC_LOG_DEBUG("lfg", "LFGMgr::UpdateProposal: Player [%u] of proposal %u selected: %u", guid.GetCounter(), proposalId, accept);
    if (!accept)
    {
        RemoveProposal(itProposal, LFG_UPDATETYPE_PROPOSAL_DECLINED);
        return;
    }

    // check if all have answered and reorder players (leader first)
    bool allAnswered = true;
    for (LfgProposalPlayerContainer::const_iterator itPlayers = proposal.players.begin(); itPlayers != proposal.players.end(); ++itPlayers)
        if (itPlayers->second.accept != LFG_ANSWER_AGREE)   // No answer (-1) or not accepted (0)
            allAnswered = false;

    for (auto&& it : proposal.players)
        SendLfgUpdateProposal(it.first, proposal);

    if (!allAnswered)
        return;

    bool flex = GetLFGDungeon(proposal.dungeonId)->difficulty == RAID_DIFFICULTY_1025MAN_FLEX;
    bool sendUpdate = proposal.state != LFG_PROPOSAL_SUCCESS;
    proposal.state = LFG_PROPOSAL_SUCCESS;
    time_t joinTime = time(NULL);

    QueueManager& manager = GetQueueManager(guid);
    for (LfgProposalPlayerContainer::const_iterator it = proposal.players.begin(); it != proposal.players.end(); ++it)
    {
        ObjectGuid pguid = it->first;
        ObjectGuid gguid = it->second.group;
        uint32 queueId = it->second.queueId;
        auto& queueData = GetPlayerQueueData(pguid, queueId);
        //ASSERT(!updateData.dungeons.empty());
        uint32 dungeonId = queueData.Dungeons.empty() ? 0 : *queueData.Dungeons.begin();

        int32 waitTime = -1;
        if (sendUpdate)
            SendLfgUpdateProposal(pguid, proposal);

        // Flex doesn't use queue, don't update timers
        if (!flex)
            waitTime = int32((joinTime - queueData.JoinTime) / IN_MILLISECONDS);

        SendLfgUpdateStatus(LFG_UPDATETYPE_GROUP_FOUND, pguid, queueId);

        if (!dungeonId)
        {
            AlmostAssert("LFGMgr::UpdateProposal !dungeonId. player: %u, proposal dungeonId: %u", pguid.GetCounter(), proposal.dungeonId);
            continue;
        }

        // Update timers
        if (!flex)
            manager.UpdateWaitTime((LfgRoles)GetRoles(pguid, queueId), waitTime, dungeonId);
    }

    // Remove players/groups from Queue
    manager.RemoveFromQueue(proposal.queuers);

    // Pass by copy, just in case for some reason proposal will be removed from store
    LfgProposal copy{ proposal };
    ProposalsStore.erase(itProposal);

    MakeNewGroup(copy);
}

/**
   Remove a proposal from the pool, remove the group that didn't accept (if needed) and readd the other members to the queue

   @param[in]     itProposal Iterator to the proposal to remove
   @param[in]     type Type of removal (LFG_UPDATETYPE_PROPOSAL_FAILED, LFG_UPDATETYPE_PROPOSAL_DECLINED)
*/
void LFGMgr::RemoveProposal(LfgProposalContainer::iterator itProposal, LfgUpdateType type)
{
    LfgProposal& proposal = itProposal->second;
    proposal.state = LFG_PROPOSAL_FAILED;

    TC_LOG_DEBUG("lfg", "LFGMgr::RemoveProposal: Proposal %u, state FAILED, UpdateType %u", itProposal->first, type);
    // Mark all people that didn't answered as no accept
    if (type == LFG_UPDATETYPE_PROPOSAL_FAILED)
        for (LfgProposalPlayerContainer::iterator it = proposal.players.begin(); it != proposal.players.end(); ++it)
            if (it->second.accept == LFG_ANSWER_PENDING)
                it->second.accept = LFG_ANSWER_DENY;

    // Mark players/groups to be removed
    std::list<Queuer> toRemove;
    for (auto&& itr : proposal.players)
    {
        if (itr.second.accept == LFG_ANSWER_AGREE)
            continue;

        ObjectGuid guid = itr.second.group ? itr.second.group : itr.first;
        // Player didn't accept or still pending when no secs left
        if (itr.second.accept == LFG_ANSWER_DENY || type == LFG_UPDATETYPE_PROPOSAL_FAILED)
        {
            itr.second.accept = LFG_ANSWER_DENY;
            // Multiple players from the same group rejected the proposal
            Queuer queuer{ guid, itr.second.queueId };
            if (std::find(toRemove.begin(), toRemove.end(), queuer) != toRemove.end())
                continue;
            toRemove.emplace_back(queuer);
        }
    }

    // Notify players
    for (auto&& itr : proposal.players)
    {
        ObjectGuid guid = itr.first;
        ObjectGuid gguid = itr.second.group ? itr.second.group : guid;
        uint32 queueId = itr.second.queueId;    // QueueId will be equal for both player and they group

        SendLfgUpdateProposal(guid, proposal);

        bool removeFromQueue = false;
        for (auto&& itr : toRemove)
        {
            if (itr.GetGUID() == gguid && itr.GetQeueueId() == queueId)
            {
                removeFromQueue = true;
                break;
            }
        }

        if (removeFromQueue)         // Didn't accept or in same group that someone that didn't accept
        {
            auto updateType = type;
            if (itr.second.accept == LFG_ANSWER_DENY)
            {
                TC_LOG_DEBUG("lfg", "LFGMgr::RemoveProposal: [" UI64FMTD "] didn't accept. Removing from queue and compatible cache", GetGuidForLog(guid));
            }
            else
            {
                updateType = LFG_UPDATETYPE_REMOVED_FROM_QUEUE;
                TC_LOG_DEBUG("lfg", "LFGMgr::RemoveProposal: [" UI64FMTD "] in same group that someone that didn't accept. Removing from queueData and compatible cache", GetGuidForLog(guid));
            }

            // Send update before RestoreState call, bacause queue will be removed if old state == LFG_STATE_NONE
            SendLfgUpdateStatus(updateType, guid, queueId);

            LfgState oldState = GetOldState(guid, queueId);
            RestoreState(guid, queueId, "Proposal Fail (didn't accepted or in group with someone that didn't accept");
            if (gguid != guid && HasQueueId(gguid, queueId)) // Because for group queue may be already removed. Do it only once
                RestoreState(gguid, queueId, "Proposal Fail (someone in group didn't accepted)");
            if (oldState == LFG_STATE_NONE)
                RemoveQueue(guid, queueId);
        }
        else
        {
            TC_LOG_DEBUG("lfg", "LFGMgr::RemoveProposal: Readding [" UI64FMTD "] to queue (%u).", GetGuidForLog(guid), queueId);
            SetState(guid, queueId, LFG_STATE_QUEUED);
            if (gguid != guid && GetState(gguid, queueId) != LFG_STATE_QUEUED)
                SetState(gguid, queueId, LFG_STATE_QUEUED);
            SendLfgUpdateStatus(LFG_UPDATETYPE_ADDED_TO_QUEUE, guid, queueId);
        }
    }

    QueueManager& manager = GetQueueManager(proposal.players.begin()->first);
    // Remove players/groups from queue
    // Flex doesn't use queue, do nothing
    if (GetLFGDungeon(proposal.dungeonId)->difficulty != RAID_DIFFICULTY_1025MAN_FLEX)
        manager.RemoveFromQueue(toRemove);

    for (auto&& queuer : toRemove)
    {
        proposal.queuers.remove(queuer);
        // Players already removed from queue. Process groups
        if (queuer.IsGroup() && GetState(queuer.GetGUID(), queuer.GetQeueueId()) == LFG_STATE_NONE)
            RemoveQueue(queuer.GetGUID(), queuer.GetQeueueId());
    }

    // Readd to queue
    if (!proposal.queuers.empty())
        manager.AddToQueue(proposal.queuers, true);

    ProposalsStore.erase(itProposal);
}

/**
   Initialize a boot kick vote

   @param[in]     gguid Group the vote kicks belongs to
   @param[in]     kicker Kicker guid
   @param[in]     victim Victim guid
   @param[in]     reason Kick reason
*/
void LFGMgr::InitBoot(ObjectGuid gguid, ObjectGuid kicker, ObjectGuid victim, std::string const& reason)
{
    PlayersStore[kicker].IncrementBootCounter();
    PlayersStore[victim].MarkLastBootTime();

    LfgPlayerBoot& boot = BootsStore[gguid];

    SetState(gguid, GetActiveQueueId(gguid), LFG_STATE_BOOT);

    boot.inProgress = true;
    boot.cancelTime = time_t(time(NULL)) + LFG_TIME_BOOT;
    boot.reason = reason;
    boot.victim = victim;

    LfgGuidSet const& players = GetPlayers(gguid);

    // Set votes
    for (LfgGuidSet::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        ObjectGuid guid = (*itr);
        SetState(guid, GetActiveQueueId(guid), LFG_STATE_BOOT);
        boot.votes[guid] = LFG_ANSWER_PENDING;
    }

    boot.votes[victim] = LFG_ANSWER_DENY;                  // Victim auto vote NO
    boot.votes[kicker] = LFG_ANSWER_AGREE;                 // Kicker auto vote YES

    // Notify players
    for (LfgGuidSet::const_iterator it = players.begin(); it != players.end(); ++it)
        SendLfgBootProposalUpdate(*it, boot);
}

/**
   Update Boot info with player answer

   @param[in]     guid Player who has answered
   @param[in]     player answer
*/
void LFGMgr::UpdateBoot(ObjectGuid guid, bool accept)
{
    uint32 queueId = GetActiveQueueId(guid);
    if (!queueId)
    {
        TC_LOG_ERROR("lfg", "LFGMgr::UpdateBoot Active queue not found [" UI64FMTD "]", GetGuidForLog(guid));
        return;
    }

    ObjectGuid gguid = GetGroup(guid, queueId);
    if (!gguid)
    {
        TC_LOG_ERROR("lfg", "LFGMgr::UpdateBoot Group not found [" UI64FMTD "], queue id: %u", GetGuidForLog(guid), queueId);
        return;
    }

    LfgPlayerBootContainer::iterator itBoot = BootsStore.find(gguid);
    if (itBoot == BootsStore.end())
        return;

    LfgPlayerBoot& boot = itBoot->second;

    if (boot.votes[guid] != LFG_ANSWER_PENDING)    // Cheat check: Player can't vote twice
        return;

    boot.votes[guid] = LfgAnswer(accept);

    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    for (LfgAnswerContainer::const_iterator itVotes = boot.votes.begin(); itVotes != boot.votes.end(); ++itVotes)
    {
        if (itVotes->second != LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (itVotes->second == LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }

    // if we don't have enough votes (agree or deny) do nothing
    if (agreeNum < sLFGMgr->GetBootVotesNeeded(gguid) && (votesNum - agreeNum) < sLFGMgr->GetBootVotesNeeded(gguid))
        return;

    // Send update info to all players
    boot.inProgress = false;
    for (LfgAnswerContainer::const_iterator itVotes = boot.votes.begin(); itVotes != boot.votes.end(); ++itVotes)
    {
        ObjectGuid pguid = itVotes->first;
        if (pguid != boot.victim || agreeNum != sLFGMgr->GetBootVotesNeeded(gguid))
            SendLfgBootProposalUpdate(pguid, boot);
        RestoreState(pguid, GetActiveQueueId(pguid), "Boot Finished");
    }

    RestoreState(gguid, GetActiveQueueId(gguid), "Boot Finished");
    if (agreeNum == sLFGMgr->GetBootVotesNeeded(gguid))           // Vote passed - Kick player
    {
        if (Group* group = sGroupMgr->GetGroupByGUID(gguid.GetCounter()))
            Player::RemoveFromGroup(group, boot.victim, GROUP_REMOVEMETHOD_KICK_LFG);
        DecreaseKicksLeft(gguid);
    }
    BootsStore.erase(itBoot);
}

/**
   Teleports the player in or out the dungeon

   @param[in]     player Player to teleport
   @param[in]     out Teleport out (true) or in (false)
   @param[in]     fromOpcode Function called from opcode handlers? (Default false)
*/
void LFGMgr::TeleportPlayer(Player* player, bool out, bool fromOpcode /*= false*/, bool forceChangeInstance /*= false*/)
{
    LFGDungeonData const* dungeon = NULL;
    Group* group = player->GetGroup();
    if (fromOpcode && group && group->GetMembersCount() == 1)
    {
        group->RemoveMember(player->GetGUID(), GROUP_REMOVEMETHOD_LEAVE);
        return;
    }

    if (group && group->isLFGGroup())
        dungeon = GetLFGDungeon(GetDungeon(group->GetGUID()));

    if (!dungeon)
    {
        TC_LOG_ERROR("lfg", "TeleportPlayer: Player %s (%u) not in group/lfggroup or dungeon not found! (group: " UI64FMTD ", dungeon: %u)",
            player->GetName().c_str(), player->GetGUID().GetCounter(), group ? GetGuidForLog(group->GetGUID()) : 0, group ? GetDungeon(group->GetGUID()) : 0);
        player->GetSession()->SendLfgTeleportError(uint8(LFG_TELEPORTERROR_INVALID_LOCATION));
        return;
    }

    if (out)
    {
        TC_LOG_DEBUG("lfg", "TeleportPlayer: Player %s (%u) is being teleported out. Current Map %u - Expected Map %u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), player->GetMapId(), uint32(dungeon->map));
        if (player->GetMapId() == uint32(dungeon->map))
            player->TeleportToBGEntryPoint();

        return;
    }

    LfgTeleportError error = LFG_TELEPORTERROR_OK;

    if (!player->IsAlive())
        error = LFG_TELEPORTERROR_PLAYER_DEAD;
    else if (player->IsFalling() || player->HasUnitState(UNIT_STATE_JUMPING))
        error = LFG_TELEPORTERROR_FALLING;
    else if (player->IsMirrorTimerActive(FATIGUE_TIMER))
        error = LFG_TELEPORTERROR_FATIGUE;
    else if (player->GetVehicle())
        error = LFG_TELEPORTERROR_IN_VEHICLE;
    else if (player->GetCharmGUID())
        error = LFG_TELEPORTERROR_CHARMING;
    else if (player->GetMapId() != uint32(dungeon->map) || forceChangeInstance)  // Do not teleport players in dungeon to the entrance
    {
        uint32 mapid = dungeon->map;
        float x = dungeon->x;
        float y = dungeon->y;
        float z = dungeon->z;
        float orientation = dungeon->o;

        if (!fromOpcode && !forceChangeInstance)
        {
            // Select a player inside to be teleported to
            bool found = false;
            if (Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID()))
                if (leader->IsInWorld() && leader != player && leader->GetMapId() == mapid)
                {
                    found = true;
                    x = leader->GetPositionX();
                    y = leader->GetPositionY();
                    z = leader->GetPositionZ();
                    orientation = leader->GetOrientation();
                }
            if (!found)
                for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* plrg = itr->GetSource();
                    if (plrg && plrg != player && plrg->IsInWorld() && plrg->GetMapId() == mapid)
                    {
                        x = plrg->GetPositionX();
                        y = plrg->GetPositionY();
                        z = plrg->GetPositionZ();
                        orientation = plrg->GetOrientation();
                        break;
                    }
                }
        }

        if (!player->GetMap()->IsDungeon())
            player->SetBattlegroundEntryPoint();

        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }

        player->SetForcedTeleportFar(forceChangeInstance);
        if (!player->TeleportTo(mapid, x, y, z, orientation))
            error = LFG_TELEPORTERROR_INVALID_LOCATION;
        player->SetForcedTeleportFar(false);
    }
    else
        error = LFG_TELEPORTERROR_INVALID_LOCATION;

    if (error != LFG_TELEPORTERROR_OK)
        player->GetSession()->SendLfgTeleportError(uint8(error));

    TC_LOG_DEBUG("lfg", "TeleportPlayer: Player %s is being teleported in to map %u "
        "(x: %f, y: %f, z: %f) Result: %u", player->GetName().c_str(), dungeon->map,
        dungeon->x, dungeon->y, dungeon->z, error);
}

/**
   Finish a dungeon and give reward, if any.

   @param[in]     guid Group guid
   @param[in]     dungeonId Dungeonid
*/
void LFGMgr::FinishDungeon(ObjectGuid gguid, uint32 dungeonId, Map* map)
{
    uint32 queueId = GetActiveQueueId(gguid);
    if (!queueId)
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [" UI64FMTD "] Finished dungeon %u but group has no active queue", gguid.GetRawValue(), dungeonId);
        return;
    }

    LfgState oldState = GetState(gguid, queueId);
    bool needsRemoveFromQueue = GetDungeon(gguid) && oldState != LFG_STATE_DUNGEON;

    uint32 gDungeonId = GetDungeon(gguid);
    if (gDungeonId != dungeonId)
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [" UI64FMTD "] Finished dungeon %u but group queued for %u. Ignoring", gguid.GetRawValue(), dungeonId, gDungeonId);
        return;
    }

    if (GetState(gguid, queueId) == LFG_STATE_FINISHED_DUNGEON) // Shouldn't happen. Do not reward multiple times
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [" UI64FMTD "] Already rewarded group. Ignoring", gguid.GetRawValue());
        return;
    }

    SetState(gguid, queueId, LFG_STATE_FINISHED_DUNGEON);

    LFGDungeonData const* dungeonDone = GetLFGDungeon(dungeonId);
    uint32 mapId = dungeonDone ? uint32(dungeonDone->map) : 0;
    uint32 scenarioId = sScenarioMgr->GetScenarioIdForMap(mapId);

    bool guildRewarded = true;

    const LfgGuidSet& players = GetPlayers(gguid);
    for (LfgGuidSet::const_iterator it = players.begin(); it != players.end(); ++it)
    {
        ObjectGuid guid = (*it);
        uint32 queueId = GetActiveQueueId(guid);
        auto& queueData = GetPlayerQueueData(guid, queueId);

        if (queueData.State == LFG_STATE_FINISHED_DUNGEON)
        {
            TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [" UI64FMTD "] Already rewarded player. Ignoring", guid.GetRawValue());
            continue;
        }

        uint32 rDungeonId = GetRandomDungeon(guid, queueId);
        if (!rDungeonId)
        {
            // LFR and not random stuff
            LfgDungeonSet const& dungeons = GetSelectedDungeons(guid, queueId);
            if (!dungeons.empty())
                rDungeonId = (*dungeons.begin());
        }

        SetState(guid, queueId, LFG_STATE_FINISHED_DUNGEON);

        // Give rewards only if its a random dungeon
        LFGDungeonData const* dungeon = GetLFGDungeon(rDungeonId);

        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player || !player->IsInWorld())
        {
            TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [%u] not found in world", guid.GetCounter());
            continue;
        }

        if (player->GetMap() != map)
        {
            TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [%u] is in map %u and should be in %u to get reward", guid.GetCounter(), player->GetMapId(), mapId);
            continue;
        }

        // We have no rDungeonId if registered to concrete scenario. Do it before !dungeon check
        if (scenarioId)
        {
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO, 1);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO, scenarioId);
        }

        if (!dungeon || (dungeon->type != LFG_TYPE_RANDOM && !dungeon->seasonal && dungeon->difficulty != RAID_DIFFICULTY_25MAN_LFR))
        {
            if (dungeon)
            {
                LfgPlayerRewardData data = LfgPlayerRewardData(dungeon->Entry(), GetDungeon(gguid, false), false, NULL, NULL);
                player->GetSession()->SendLfgPlayerReward(data);
            }
            TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [%u] dungeon %u is not random or seasonal", guid.GetCounter(), rDungeonId);
            continue;
        }

        // Update achievements
        if (dungeon->difficulty == DUNGEON_DIFFICULTY_HEROIC)
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, 1);

        LfgReward const* reward = GetRandomDungeonReward(rDungeonId, player->GetLevel());
        if (!reward)
            continue;

        bool done = false;
        Quest const* quest = sObjectMgr->GetQuestTemplate(reward->firstQuest);
        Quest const* ctaQuest = NULL;
        if (quest)
        {
            // if we can take the quest, means that we haven't done this kind of "run", IE: First Heroic Random of Day.
            if (player->CanRewardQuest(quest, false))
            {
                player->RewardQuest(quest, 0, NULL, false);
                if (quest->GetQuestId() == 31614)   // Daily Heroic Random (1st) (Mists)
                {
                    if (uint32 bonusRepFaction = player->GetUInt32Value(PLAYER_FIELD_LFG_BONUS_FACTION_ID))
                    {
                        if (auto factionEntry = sFactionStore.LookupEntry(bonusRepFaction))
                        {
                            float donerep = player->CalculateReputationGain(REPUTATION_SOURCE_DAILY_QUEST, quest->GetQuestLevel(), 300, bonusRepFaction);
                            player->GetReputationMgr().ModifyReputation(factionEntry, donerep);
                        }
                    }
                }
            }
            else
            {
                done = true;
                quest = sObjectMgr->GetQuestTemplate(reward->otherQuest);
                // we give reward without informing client (retail does this)
                if (quest)
                    player->RewardQuest(quest, 0, NULL, false);
            }
        }


        if (queueData.CTARoles != PLAYER_ROLE_NONE && player->GetGroup() && (player->GetGroup()->GetLfgRoles(guid) & ~PLAYER_ROLE_LEADER) == queueData.CTARoles)
        {
            ctaQuest = sObjectMgr->GetQuestTemplate(QUEST_LFG_SHORTAGE);
            if (ctaQuest)
                player->RewardQuest(ctaQuest, 0, nullptr, false);
        }

        // Give rewards
        TC_LOG_DEBUG("lfg", "LFGMgr::FinishDungeon: [%u] done dungeon %u, %s previously done.", player->GetGUID().GetCounter(), GetDungeon(gguid), done? " " : " not");
        LfgPlayerRewardData data = LfgPlayerRewardData(dungeon->Entry(), GetDungeon(gguid, false), done, quest, ctaQuest);
        player->GetSession()->SendLfgPlayerReward(data);
        player->RemoveAurasDueToSpell(LFG_SPELL_DUNGEON_COOLDOWN);
    }

    if (Group* group = sGroupMgr->GetGroupByGUID(gguid))
    {
        group->SendUpdate();
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LFG_DATA);
        stmt->setUInt32(0, LFG_STATE_FINISHED_DUNGEON);
        stmt->setUInt32(1, group->GetDbStoreId());
        CharacterDatabase.Execute(stmt);

        if (scenarioId)
        {
            Player* participant;
            if (Guild* guild = group->GetGroupGuild(map->GetFirstPlayerInInstance(), &participant))
            {
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO, 1, 0, 0, participant, participant);
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO, scenarioId, 0, 0, participant, participant);
                guild->CompleteGuildChallenge(CHALLENGE_SCENARIO, participant);
            }
        }
    }

    if (needsRemoveFromQueue)
    {
        switch (oldState)
        {
            case LFG_STATE_ROLECHECK:
            case LFG_STATE_QUEUED:
            case LFG_STATE_PROPOSAL:
            case LFG_STATE_BOOT:
                SetState(gguid, queueId, oldState);
                LeaveLfg(gguid, queueId);
                break;
            case LFG_STATE_NONE:
            case LFG_STATE_DUNGEON:
            case LFG_STATE_FINISHED_DUNGEON:
            default:
                TC_LOG_ERROR("lfg", "LFGMgr::FinishDungeon: Prior to finishing dungeon group %u state was %s", gguid.GetCounter(), GetStateString(oldState).c_str());
                break;
        }
    }
}

// --------------------------------------------------------------------------//
// Auxiliar Functions
// --------------------------------------------------------------------------//

/**
   Get the dungeon list that can be done given a random dungeon entry.

   @param[in]     randomdungeon Random dungeon id (if value = 0 will return all dungeons)
   @returns Set of dungeons that can be done.
*/
LfgDungeonSet const& LFGMgr::GetDungeonsByRandom(uint32 randomdungeon)
{
    LFGDungeonData const* dungeon = GetLFGDungeon(randomdungeon);
    uint32 group = dungeon ? dungeon->group : 0;
    return CachedDungeonMapStore[group];
}

/**
   Get the reward of a given random dungeon at a certain level

   @param[in]     dungeon dungeon id
   @param[in]     level Player level
   @returns Reward
*/
LfgReward const* LFGMgr::GetRandomDungeonReward(uint32 dungeon, uint8 level)
{
    LfgReward const* rew = NULL;
    LfgRewardContainerBounds bounds = RewardMapStore.equal_range(dungeon & 0x00FFFFFF);
    for (LfgRewardContainer::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        rew = itr->second;
        // ordered properly at loading
        if (itr->second->maxLevel >= level)
            break;
    }

    return rew;
}

/**
   Given a Dungeon id returns the dungeon Type

   @param[in]     dungeon dungeon id
   @returns Dungeon type
*/
LfgType LFGMgr::GetDungeonType(uint32 dungeonId)
{
    LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId);
    if (!dungeon)
        return LFG_TYPE_NONE;

    return LfgType(dungeon->type);
}

uint32 LFGMgr::ConvertToServerQueueId(ObjectGuid guid, uint32 clientQueueId) const
{
    if (auto queues = GetPlayerQueues(guid))
        for (auto&& itr : *queues)
            if (itr.second.ClientQueueId == clientQueueId)
                return itr.first;
    return 0;
}

void LFGMgr::AddQueue(ObjectGuid guid, uint32 queueId, ObjectGuid originalGroup)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::AddQueue %s [" UI64FMTD "] queue: %u", guid.IsGroup() ? "Group" : "Player", GetGuidForLog(guid), queueId);

    if (guid.IsGroup())
    {
        ASSERT(originalGroup.IsEmpty());
        GroupsStore[guid].AddQueue(queueId);
    }
    else
        PlayersStore[guid].AddQueue(queueId, originalGroup);
}

void LFGMgr::RemoveQueue(ObjectGuid guid, uint32 queueId)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::RemoveQueue: %s [" UI64FMTD "] queueId: %u. Queue removed", guid.IsGroup() ? "Group" : "Player", GetGuidForLog(guid), queueId);

    if (guid.IsGroup())
    {
        auto itr = GroupsStore.find(guid);
        ASSERT(itr != GroupsStore.end());
        itr->second.RemoveQueue(queueId);
        if (itr->second.GetQueues().empty())
            RemoveGroupData(guid);
    }
    else
    {
        auto itr = PlayersStore.find(guid);
        ASSERT(itr != PlayersStore.end());
        itr->second.RemoveQueue(queueId);
    }
}

uint32 LFGMgr::GetActiveQueueId(ObjectGuid guid) const
{
    if (guid.IsGroup())
    {
        auto itr = GroupsStore.find(guid);
        if (itr == GroupsStore.end())
            return 0;
        return itr->second.GetActiveQueueId();
    }
    else
    {
        auto itr = PlayersStore.find(guid);
        if (itr == PlayersStore.end())
            return 0;
        return itr->second.GetActiveQueueId();
    }
}

LfgState LFGMgr::GetActiveState(ObjectGuid guid) const
{
    if (guid.IsGroup())
    {
        auto itr = GroupsStore.find(guid);
        if (itr == GroupsStore.end())
            return LFG_STATE_NONE;
        if (!itr->second.GetActiveQueueId())
            return LFG_STATE_NONE;
        return itr->second.GetActiveQueue().State;
    }
    else
    {
        auto itr = PlayersStore.find(guid);
        if (itr == PlayersStore.end())
            return LFG_STATE_NONE;
        if (!itr->second.GetActiveQueueId())
            return LFG_STATE_NONE;
        return itr->second.GetActiveQueue().State;
    }
}

LfgState LFGMgr::GetState(ObjectGuid guid, uint32 queueId) const
{
    LfgState state;
    if (guid.IsGroup())
    {
        auto it = GroupsStore.find(guid);
        state = it != GroupsStore.end() ? it->second.GetState(queueId) : LFG_STATE_NONE;
    }
    else
    {
        auto it = PlayersStore.find(guid);
        state = it != PlayersStore.end() ? it->second.GetState(queueId) : LFG_STATE_NONE;
    }

    TC_LOG_TRACE("lfg", "LFGMgr::GetState: [" UI64FMTD "] = %u", GetGuidForLog(guid), state);
    return state;
}

LfgState LFGMgr::GetOldState(ObjectGuid guid, uint32 queueId)
{
    LfgState state;
    if (guid.IsGroup())
        state = GroupsStore[guid].GetOldState(queueId);
    else
        state = PlayersStore[guid].GetOldState(queueId);

    TC_LOG_TRACE("lfg", "LFGMgr::GetOldState: [" UI64FMTD "] = %u, queueId: %u", GetGuidForLog(guid), state, queueId);
    return state;
}

uint32 LFGMgr::GetDungeon(ObjectGuid guid, bool asId /*= true */)
{
    uint32 dungeon = 0;

    auto itr = GroupsStore.find(guid);
    if (itr != GroupsStore.end())
        dungeon = itr->second.GetDungeon(asId);

    TC_LOG_TRACE("lfg", "LFGMgr::GetDungeon: [" UI64FMTD "] asId: %u = %u", GetGuidForLog(guid), asId, dungeon);
    return dungeon;
}

uint32 LFGMgr::GetDungeonMapId(ObjectGuid guid)
{
    uint32 dungeonId = GroupsStore[guid].GetDungeon(true);
    uint32 mapId = 0;
    if (dungeonId)
        if (LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId))
            mapId = dungeon->map;

    TC_LOG_TRACE("lfg", "LFGMgr::GetDungeonMapId: [" UI64FMTD "] = %u (DungeonId = %u)", GetGuidForLog(guid), mapId, dungeonId);
    return mapId;
}

uint8 LFGMgr::GetRoles(ObjectGuid guid, uint32 queueId)
{
    uint8 roles = PlayersStore[guid].GetRoles(queueId);
    TC_LOG_TRACE("lfg", "LFGMgr::GetRoles: [" UI64FMTD "] = %u, queueId: %u", GetGuidForLog(guid), roles, queueId);
    return roles;
}

LfgDungeonSet const& LFGMgr::GetSelectedDungeons(ObjectGuid guid, uint32 queueId)
{
    TC_LOG_TRACE("lfg", "LFGMgr::GetSelectedDungeons: [" UI64FMTD "]", GetGuidForLog(guid));
    return PlayersStore[guid].GetSelectedDungeons(queueId);
}

LfgLockMap const& LFGMgr::GetLockedDungeons(ObjectGuid guid)
{
    TC_LOG_TRACE("lfg", "LFGMgr::GetLockedDungeons: [" UI64FMTD "]", GetGuidForLog(guid));
    return PlayersStore[guid].GetLockedDungeons();
}

uint8 LFGMgr::GetKicksLeft(ObjectGuid guid)
{
    uint8 kicks = GroupsStore[guid].GetKicksLeft();
    TC_LOG_TRACE("lfg", "LFGMgr::GetKicksLeft: [" UI64FMTD "] = %u", GetGuidForLog(guid), kicks);
    return kicks;
}

void LFGMgr::RestoreState(ObjectGuid guid, uint32 queueId, char const* debugMsg)
{
    if (guid.IsGroup())
    {
        LfgGroupData& data = GroupsStore[guid];
        if (sLog->ShouldLog("lfg", LOG_LEVEL_DEBUG))
        {
            std::string const& ps = GetStateString(data.GetState(queueId));
            std::string const& os = GetStateString(data.GetOldState(queueId));
            TC_LOG_DEBUG("lfg", "LFGMgr::RestoreState: Group: [" UI64FMTD "] (%s) queueId: %u state: %s, oldState: %s",
                guid.GetRawValue(), debugMsg, queueId, ps.c_str(), os.c_str());
        }

        data.RestoreState(queueId);
    }
    else
    {
        LfgPlayerData& data = PlayersStore[guid];
        if (sLog->ShouldLog("lfg", LOG_LEVEL_DEBUG))
        {
            std::string const& ps = GetStateString(data.GetState(queueId));
            std::string const& os = GetStateString(data.GetOldState(queueId));
            TC_LOG_DEBUG("lfg", "LFGMgr::RestoreState: Player: [" UI64FMTD "] (%s) queueId: %u, state: %s, oldState: %s",
                GetGuidForLog(guid), debugMsg, queueId, ps.c_str(), os.c_str());
        }
        data.RestoreState(queueId);
    }
}

void LFGMgr::SetState(ObjectGuid guid, uint32 queueId, LfgState state)
{
    if (state != LFG_STATE_QUEUED)
    {
        bool validCase = state == LFG_STATE_FINISHED_DUNGEON && GetOldState(guid, queueId) == LFG_STATE_DUNGEON;
        if (!validCase && GetQueueManager(guid).Contains(Queuer{ guid, queueId }))
        {
            std::ostringstream ss;
            Queuer(guid, queueId).OutDebug(ss, nullptr);
            TC_LOG_ERROR("lfg", "LFGMgr::SetState(" UI64FMTD " (%s), state: %u, queueId: %u) while queuer still in queues, stack trace:\n", GetGuidForLog(guid), ss.str().c_str(), (uint32)state, queueId);
        }
    }

    if (guid.IsGroup())
    {
        LfgGroupData& data = GroupsStore[guid];
        if (sLog->ShouldLog("lfg", LOG_LEVEL_DEBUG))
        {
            std::string const& ns = GetStateString(state);
            std::string const& ps = GetStateString(data.GetState(queueId));
            std::string const& os = GetStateString(data.GetOldState(queueId));
            TC_LOG_DEBUG("lfg", "LFGMgr::SetState: Group: [" UI64FMTD "] queueId: %u, newState: %s, previous: %s, oldState: %s",
                guid.GetRawValue(), queueId, ns.c_str(), ps.c_str(), os.c_str());
        }
        data.SetState(queueId, state);
    }
    else
    {
        LfgPlayerData& data = PlayersStore[guid];
        if (sLog->ShouldLog("lfg", LOG_LEVEL_DEBUG))
        {
            std::string const& ns = GetStateString(state);
            std::string const& ps = GetStateString(data.GetState(queueId));
            std::string const& os = GetStateString(data.GetOldState(queueId));
            TC_LOG_DEBUG("lfg", "LFGMgr::SetState: Player: [" UI64FMTD "] queueId: %u, newState: %s, previous: %s, oldState: %s",
                GetGuidForLog(guid), queueId, ns.c_str(), ps.c_str(), os.c_str());
        }
        data.SetState(queueId, state);
    }
}

void LFGMgr::SetDungeon(ObjectGuid guid, uint32 dungeon)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetDungeon: [" UI64FMTD "] dungeon %u", GetGuidForLog(guid), dungeon);
    GroupsStore[guid].SetDungeon(dungeon);
}

void LFGMgr::SetRoles(ObjectGuid guid, uint32 queueId, LfgRoles roles)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetRoles: [" UI64FMTD "] queueId: %u, roles: %u", GetGuidForLog(guid), queueId, roles);
    PlayersStore[guid].SetRoles(queueId, roles);
}

void LFGMgr::SetJoinTime(ObjectGuid guid, uint32 queueId, time_t time)
{
    PlayersStore[guid].SetJoinTime(queueId, time);
}

void LFGMgr::SetSelectedDungeons(ObjectGuid guid, uint32 queueId, LfgDungeonSet const& dungeons)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetSelectedDungeons: [" UI64FMTD "] Queue Id: %u, Dungeons: %s", GetGuidForLog(guid), queueId, ConcatenateDungeons(dungeons).c_str());
    PlayersStore[guid].SetSelectedDungeons(queueId, dungeons);
}

uint32 LFGMgr::GetRandomDungeon(ObjectGuid guid, uint32 queueId) const
{
    auto itr = PlayersStore.find(guid);
    ASSERT(itr != PlayersStore.end());
    return itr->second.GetRandomDungeon(queueId);
}

void LFGMgr::SetRandomDungeon(ObjectGuid guid, uint32 queueId, uint32 dungeon)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetRandomDungeon: [" UI64FMTD "] Queue Id: %u, Dungeon: %u", GetGuidForLog(guid), queueId, dungeon);

    auto itr = PlayersStore.find(guid);
    ASSERT(itr != PlayersStore.end());
    itr->second.SetRandomDungeon(queueId, dungeon);
}

void LFGMgr::SetLockedDungeons(ObjectGuid guid, LfgLockMap const& lock)
{
    TC_LOG_TRACE("lfg", "LFGMgr::SetLockedDungeons: [" UI64FMTD "]", GetGuidForLog(guid));
    PlayersStore[guid].SetLockedDungeons(lock);
}

void LFGMgr::SetKicksLeft(ObjectGuid guid, uint8 kicksLeft)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetKicksLeft: [%u] KicksLeft: %u", guid.GetCounter(), kicksLeft);
    GroupsStore[guid].SetKicksLeft(kicksLeft);
}

void LFGMgr::DecreaseKicksLeft(ObjectGuid guid)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::DecreaseKicksLeft: [%u]", guid.GetCounter());
    GroupsStore[guid].DecreaseKicksLeft();
}

void LFGMgr::RemovePlayerData(ObjectGuid guid)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::RemovePlayerData: [%u]", guid.GetCounter());
    LfgPlayerDataContainer::iterator it = PlayersStore.find(guid);
    if (it != PlayersStore.end())
        PlayersStore.erase(it);
}

void LFGMgr::ReformQueue(ObjectGuid guid, uint32 oldQueueId, uint32 newQueueId)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::ReformQueue: [%u] old queue: %u, new queue: %u", guid.GetCounter(), oldQueueId, newQueueId);

    auto itr = PlayersStore.find(guid);
    ASSERT(itr != PlayersStore.end());
    return itr->second.ReformQueue(oldQueueId, newQueueId);
}

void LFGMgr::RemoveFinishedDungeons(ObjectGuid guid)
{
    if (guid.IsGroup())
    {
        if (auto queues = GetGroupQueues(guid))
        {
            for (auto&& itr : GroupQueueDataMap{ *queues })
            {
                if (itr.second.State == LFG_STATE_FINISHED_DUNGEON)
                {
                    TC_LOG_DEBUG("lfg", "LFGMgr::RemoveFinishedDungeons: Group [" UI64FMTD "] Queue %u removed", GetGuidForLog(guid), itr.first);
                    RemoveQueue(guid, itr.first);
                }
            }
        }
    }
    else
    {
        if (auto queues = GetPlayerQueues(guid))
        {
            for (auto&& itr : PlayerQueueDataMap{ *queues })
            {
                if (itr.second.State == LFG_STATE_FINISHED_DUNGEON)
                {
                    TC_LOG_DEBUG("lfg", "LFGMgr::RemoveFinishedDungeons: Player [" UI64FMTD "] Queue %u removed", GetGuidForLog(guid), itr.first);
                    RemoveQueue(guid, itr.first);
                }
            }
        }
    }
}

void LFGMgr::RemoveGroupData(ObjectGuid group)
{
    auto it = GroupsStore.find(group);
    if (it == GroupsStore.end())
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::RemoveGroupData: group not found [" UI64FMTD "]", group.GetRawValue());
        return;
    }

    // Make copy because I want RemovePlayerFromGroup for logging
    LfgGuidSet players = it->second.GetPlayers();
    for (auto&& player : players)
    {
        if (auto queues = GetGroupQueues(group))
        {
            for (auto&& itr : *queues)
            {
                if (HasQueueId(player, itr.first))
                {
                    SendLfgUpdateStatus(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, player, itr.first);
                    SetState(player, itr.first, LFG_STATE_NONE);
                    RemoveQueue(player, itr.first);
                }
            }
        }
        RemovePlayerFromGroup(group, player);
    }

    GroupsStore.erase(it);
    TC_LOG_DEBUG("lfg", "LFGMgr::RemoveGroupData: Group removed [" UI64FMTD "]", group.GetRawValue());
}

uint8 LFGMgr::GetTeam(ObjectGuid guid)
{
    return PlayersStore[guid].GetTeam();
}

int32 LFGMgr::RemovePlayerFromGroup(ObjectGuid gguid, uint32 queueId, ObjectGuid guid)
{
    auto itr = GroupsStore.find(gguid);
    if (itr == GroupsStore.end())
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::RemovePlayerFromGroup: group not found [" UI64FMTD "]", gguid.GetRawValue());
        return -1;
    }

    bool hasAnotherQueues = false;
    TC_LOG_DEBUG("lfg", "LFGMgr::RemovePlayerFromGroup: player [%u] group [" UI64FMTD "]", guid.GetCounter(), gguid.GetRawValue());
    // FIXME: Stupid logic right now. We don't remove player in LeaveLfg or whatever
    if (auto queues = GetPlayerQueues(guid))
    {
        for (auto&& itr : *GetPlayerQueues(guid))
        {
            if (itr.first == queueId)
            {
                SetGroup(guid, queueId, ObjectGuid::Empty);

            }
            else if (itr.second.LfgGroup == gguid)
                hasAnotherQueues = true;
        }
    }

    // Currently should not occur
    if (hasAnotherQueues)
    {
        TC_LOG_ERROR("lfg", "LFGMgr::RemovePlayerFromGroup hasAnotherQueues guid [%u] group [" UI64FMTD "]", guid.GetCounter(), gguid.GetRawValue());
        return itr->second.GetPlayerCount();
    }

    return int32(itr->second.RemovePlayer(guid));
}

void LFGMgr::RemovePlayerFromGroup(ObjectGuid gguid, ObjectGuid guid)
{
    auto itr = GroupsStore.find(gguid);
    if (itr == GroupsStore.end())
    {
        TC_LOG_DEBUG("lfg", "LFGMgr::RemovePlayerFromGroup: group not found [" UI64FMTD "]", gguid.GetRawValue());
        return;
    }

    TC_LOG_DEBUG("lfg", "LFGMgr::RemovePlayerFromGroup: player [%u] group [" UI64FMTD "]", guid.GetCounter(), gguid.GetRawValue());

    if (auto queues = GetPlayerQueues(guid))
    {
        for (auto&& itr : *GetPlayerQueues(guid))
        {
            if (itr.second.LfgGroup == gguid)
            {
                SetGroup(guid, itr.first, ObjectGuid::Empty);
            }
        }
    }
    itr->second.RemovePlayer(guid);
}

void LFGMgr::AddPlayerToGroup(ObjectGuid gguid, uint32 queueId, ObjectGuid guid)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::AddPlayerToGroup: player [%u] group [" UI64FMTD "] queue: %u", guid.GetCounter(), gguid.GetRawValue(), queueId);
    SetGroup(guid, queueId, gguid);
    GroupsStore[gguid].AddPlayer(guid);
}

void LFGMgr::SetLeader(ObjectGuid gguid, ObjectGuid leader)
{
    auto itr = GroupsStore.find(gguid);
    if (itr == GroupsStore.end())
    {
        AlmostAssert("LFGMgr::SetLeader group [" UI64FMTD "] not found", gguid);
        return;
    }
    itr->second.SetLeader(leader);
}

void LFGMgr::SetTeam(ObjectGuid guid, uint8 team)
{
    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_LFG))
        team = 0;

    PlayersStore[guid].SetTeam(team);
}

ObjectGuid LFGMgr::GetGroup(ObjectGuid guid, uint32 queueId)
{
    return PlayersStore[guid].GetGroup(queueId);
}

void LFGMgr::SetGroup(ObjectGuid guid, uint32 queueId, ObjectGuid group)
{
    PlayersStore[guid].SetGroup(queueId, group);
}

LfgGuidSet const& LFGMgr::GetPlayers(ObjectGuid guid)
{
    return GroupsStore[guid].GetPlayers();
}

uint8 LFGMgr::GetPlayerCount(ObjectGuid guid)
{
    return GroupsStore[guid].GetPlayerCount();
}

ObjectGuid LFGMgr::GetLeader(ObjectGuid guid)
{
    return GroupsStore[guid].GetLeader();
}

void LFGMgr::SendLfgRoleChosen(ObjectGuid guid, ObjectGuid pguid, uint8 roles)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgRoleChosen(pguid, roles);
}

void LFGMgr::SendLfgRoleCheckUpdate(ObjectGuid guid, LfgRoleCheck const& roleCheck)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgRoleCheckUpdate(roleCheck);
}

void LFGMgr::SendLfgUpdateStatus(LfgUpdateType updateType, ObjectGuid guid, uint32 queueId)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgUpdateStatus(updateType, GetPlayerQueueData(guid, queueId));
}

void LFGMgr::SendLfgJoinResult(ObjectGuid guid, uint32 queueId, LfgJoinResultData const& data)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgJoinResult(queueId, data);
}

void LFGMgr::SendLfgBootProposalUpdate(ObjectGuid guid, LfgPlayerBoot const& boot)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgBootProposalUpdate(boot);
}

void LFGMgr::SendLfgUpdateProposal(ObjectGuid guid, LfgProposal const& proposal)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgUpdateProposal(proposal);
}

uint64 LFGMgr::GetGuidForLog(ObjectGuid guid) const
{
    return guid.IsGroup() ? guid : uint64(guid.GetCounter());
}

void LFGMgr::SendLfgQueueStatus(ObjectGuid guid, LfgQueueStatusData const& data)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->GetSession()->SendLfgQueueStatus(data);
}

bool LFGMgr::IsLfgGroup(ObjectGuid guid)
{
    if (!guid.IsGroup())
        return false;
    Group* group = sGroupMgr->GetGroupByGUID(guid.GetCounter());
    return group && group->isLFGGroup();
}

QueueManager& LFGMgr::GetQueueManager(ObjectGuid guid)
{
    uint8 queueId = 0;
    if (guid.IsGroup())
    {
        LfgGuidSet const& players = GetPlayers(guid);
        ObjectGuid pguid = players.empty() ? ObjectGuid::Empty : (*players.begin());
        if (pguid)
            queueId = GetTeam(pguid);
    }
    else
        queueId = GetTeam(guid);
    return QueueManagers[queueId];
}

// Only for debugging purposes
void LFGMgr::Clean()
{
    QueueManagers.clear();
}

bool LFGMgr::isOptionEnabled(uint32 option)
{
    return m_options & option;
}

uint32 LFGMgr::GetOptions()
{
    return m_options;
}

void LFGMgr::SetOptions(uint32 options)
{
    m_options = options;
}

bool LFGMgr::IsSeasonActive(uint32 dungeonId)
{
    switch (dungeonId)
    {
        case 285: // The Headless Horseman
            return IsHolidayActive(HOLIDAY_HALLOWS_END);
        case 286: // The Frost Lord Ahune
            return IsHolidayActive(HOLIDAY_FIRE_FESTIVAL);
        case 287: // Coren Direbrew
            return IsHolidayActive(HOLIDAY_BREWFEST);
        case 288: // The Crown Chemical Co.
            return IsHolidayActive(HOLIDAY_LOVE_IS_IN_THE_AIR);
    }
    return false;
}

std::string LFGMgr::DumpQueueInfo(uint32 dungeonID, bool client)
{
    std::ostringstream o;

    o << "Number of Managers: " << QueueManagers.size() << "\n";
    for (auto&& manager : QueueManagers)
        manager.second.OutDebug(o, dungeonID, client);

    return o.str();
}

void LFGMgr::SetupGroupMember(ObjectGuid guid, ObjectGuid gguid)
{
    if (GroupsStore.find(gguid) == GroupsStore.end())
    {
        TC_LOG_ERROR("lfg", "LFGMgr::SetupGroupMember: group [" UI64FMTD "] not found (player " UI64FMTD ")", gguid.GetRawValue(), guid.GetRawValue());
        return;
    }
    LfgDungeonSet dungeons;
    dungeons.insert(GetDungeon(gguid));
    uint32 queueId = GetActiveQueueId(gguid);
    AddQueue(guid, queueId);
    SetState(guid, queueId, GetState(gguid, queueId));
    SetSelectedDungeons(guid, queueId, dungeons);
    AddPlayerToGroup(gguid, queueId, guid);
}

bool LFGMgr::IsSelectedRandomLfgDungeon(ObjectGuid guid)
{
    if (auto queues = GetPlayerQueues(guid))
    {
        for (auto&& itr : *queues)
        {
            if (itr.second.State == LFG_STATE_DUNGEON || itr.second.State == LFG_STATE_FINISHED_DUNGEON || itr.second.State == LFG_STATE_BOOT)
            {
                LfgDungeonSet const& dungeons = itr.second.Dungeons;
                if (!dungeons.empty())
                {
                    LFGDungeonData const* dungeon = GetLFGDungeon(*dungeons.begin());
                    if (dungeon && (dungeon->type == LFG_TYPE_RANDOM || dungeon->seasonal))
                        return true;
                }
            }
        }
    }
    return false;
}

bool LFGMgr::InLfgDungeonMap(ObjectGuid guid, uint32 map, Difficulty difficulty)
{
    uint32 queueId = GetActiveQueueId(guid);
    if (!queueId)
        return false;

    if (!guid.IsGroup())
        guid = GetGroup(guid, queueId);

    if (uint32 dungeonId = GetDungeon(guid, true))
        if (LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId))
            if (uint32(dungeon->map) == map && dungeon->difficulty == difficulty)
                return true;

    return false;
}

uint32 LFGMgr::GetLFGDungeonEntry(uint32 id)
{
    if (id)
        if (LFGDungeonData const* dungeon = GetLFGDungeon(id))
            return dungeon->Entry();

    return 0;
}

LfgDungeonSet LFGMgr::GetRandomAndSeasonalDungeons(uint8 level, uint8 expansion)
{
    LfgDungeonSet randomDungeons;
    for (lfg::LFGDungeonContainer::const_iterator itr = LfgDungeonStore.begin(); itr != LfgDungeonStore.end(); ++itr)
    {
        lfg::LFGDungeonData const& dungeon = itr->second;
        if (((dungeon.type == lfg::LFG_TYPE_RANDOM || dungeon.difficulty == RAID_DIFFICULTY_25MAN_LFR) || (dungeon.seasonal && sLFGMgr->IsSeasonActive(dungeon.id)))
            && dungeon.expansion <= expansion && dungeon.minlevel <= level && level <= dungeon.maxlevel)
            randomDungeons.insert(dungeon.Entry());
    }
    return randomDungeons;
}

void LFGMgr::SetRolesForCTAReward(ObjectGuid guid, uint32 queueId, LfgRoles roles)
{
    TC_LOG_DEBUG("lfg", "LFGMgr::SetRolesForCTAReward [%u] queue %u roles %u", guid.GetCounter(), queueId, uint32(roles));

    PlayersStore[guid].SetCTARoles(queueId, roles);
}

LfgRoles LFGMgr::GetShortageRolesForQueue(ObjectGuid guid, uint32 dungeonId)
{
    return GetQueueManager(guid).GetShortageRoles(dungeonId);
}

LfgRoles LFGMgr::GetEligibleRolesForCTA(ObjectGuid guid, uint32 dungeonId)
{
    auto itr = PlayersStore.find(guid);
    if (itr != PlayersStore.end())
    {
        for (auto&& pItr : itr->second.GetQueues())
        {
            if ((pItr.second.RandomDungeon && pItr.second.RandomDungeon == dungeonId) ||
                (pItr.second.Dungeons.size() == 1 && *pItr.second.Dungeons.begin() == dungeonId))
            {
                if (pItr.second.RandomDungeon == dungeonId)
                {
                    auto roles = pItr.second.CTARoles;
                    if (roles != PLAYER_ROLE_NONE)
                    {
                        if (pItr.second.State != LFG_STATE_QUEUED && pItr.second.State != LFG_STATE_PROPOSAL)
                            return roles;
                        return LfgRoles(roles | sLFGMgr->GetShortageRolesForQueue(guid, dungeonId));
                    }
                }
            }
        }
    }

    // Player doesn't have this queue, send him theoretical
    return sLFGMgr->GetShortageRolesForQueue(guid, dungeonId);
}

PartyResult LFGMgr::CanBoot(ObjectGuid kguid, ObjectGuid vguid, uint32& timeLeft)
{
     LfgPlayerDataContainer::const_iterator kitr = PlayersStore.find(kguid);
     if (kitr == PlayersStore.end())
         return ERR_PARTY_RESULT_OK;

     LfgPlayerDataContainer::const_iterator vitr = PlayersStore.find(vguid);
     if (vitr == PlayersStore.end())
         return ERR_PARTY_RESULT_OK;

     LfgPlayerData const& kdata = kitr->second;
     LfgPlayerData const& vdata = vitr->second;

     if (!kdata.CanInitBoot(timeLeft))
         return ERR_PARTY_LFG_BOOT_COOLDOWN_S;

     if (!vdata.CanBeBooted(timeLeft))
         return ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S;

     return ERR_PARTY_RESULT_OK;
}

uint8 LFGMgr::GetBootVotesNeeded(ObjectGuid gguid)
{
    if (uint32 dungeonId = GetDungeon(gguid))
        if (LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId))
            if (MapEntry const* map = sMapStore.LookupEntry(dungeon->map))
                if (map->IsRaid())
            return LFG_RAID_KICK_VOTES_NEEDED;
    return LFG_GROUP_KICK_VOTES_NEEDED;
}

void LFGMgr::SetSoloJoinedPlayersCount(ObjectGuid gguid, uint8 count)
{
    GroupsStore[gguid].SetSoloJoinedPlayersCount(count);
}

uint8 LFGMgr::GetSoloJoinedPlayersCount(ObjectGuid gguid)
{
    return GroupsStore[gguid].GetSoloJoinedPlayersCount();
}

bool LFGMgr::HasQueueId(ObjectGuid guid, uint32 queueId)
{
    if (guid.IsGroup())
    {
        if (auto queues = GetGroupQueues(guid))
            return queues->find(queueId) != queues->end();
    }
    else
    {
        if (auto queues = GetPlayerQueues(guid))
            return queues->find(queueId) != queues->end();
    }
    return false;
}

PlayerQueueData const& LFGMgr::GetPlayerQueueData(ObjectGuid guid, uint32 queueId) const
{
    auto itr = PlayersStore.find(guid);
    ASSERT(itr != PlayersStore.end());
    return itr->second.GetQueueData(queueId);
}

GroupQueueData const& LFGMgr::GetGroupQueueData(ObjectGuid guid, uint32 queueId) const
{
    auto itr = GroupsStore.find(guid);
    ASSERT(itr != GroupsStore.end());
    return itr->second.GetQueueData(queueId);
}

PlayerQueueDataMap const* LFGMgr::GetPlayerQueues(ObjectGuid guid) const
{
    auto itr = PlayersStore.find(guid);
    if (itr == PlayersStore.end())
        return nullptr;
    auto& queues = itr->second.GetQueues();
    return queues.empty() ? nullptr : &queues;
}

GroupQueueDataMap const* LFGMgr::GetGroupQueues(ObjectGuid guid) const
{
    auto itr = GroupsStore.find(guid);
    if (itr == GroupsStore.end())
        return nullptr;
    auto& queues = itr->second.GetQueues();
    return queues.empty() ? nullptr : &queues;
}

} // namespace lfg
