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

#include "ArenaTeam.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "Chat.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Log.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Config.h"

namespace Trinity
{
    class BattlegroundQueueLocalizedAnnounceBuilder
    {
        public:
            explicit BattlegroundQueueLocalizedAnnounceBuilder(int32 textId, Battleground* bg, uint32 q_min_level, uint32 q_max_level, uint32 qAlliance, uint32 qAllianceLeft, uint32 qHorde, uint32 qHordeLeft, bool qBoth)
                : i_textId(textId), i_bg(bg), i_q_min_level(q_min_level), i_q_max_level(q_max_level), i_qAlliance(qAlliance), i_qAllianceLeft(qAllianceLeft), i_qHorde(qHorde), i_qHordeLeft(qHordeLeft), iqBoth(qBoth) { }
            void operator()(std::vector<WorldPacket*>& data_list, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);
                char str[2048];
                if (iqBoth)
                    sprintf(str, text, i_bg->GetName(loc_idx), i_q_min_level, i_q_max_level, i_qAlliance + i_qHorde, i_qAllianceLeft + i_qHordeLeft);
                else
                    sprintf(str, text, i_bg->GetName(loc_idx), i_q_min_level, i_q_max_level, i_qAlliance, i_qAllianceLeft, i_qHorde, i_qHordeLeft);
                do_helper(data_list, &str[0]);
            }
        private:
            char* lineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = NULL; return start; }
            void do_helper(std::vector<WorldPacket*>& data_list, char* text)
            {
                char* pos = text;
                while (char* line = lineFromMessage(pos))
                {
                    WorldPacket* data = new WorldPacket();
                    ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, 0, 0, std::string(line));
                    data_list.push_back(data);
                }
            }

            int32 i_textId;
            Battleground* i_bg;
            uint32 i_q_min_level, i_q_max_level, i_qAlliance, i_qAllianceLeft, i_qHorde, i_qHordeLeft;
            bool iqBoth;
    };
}

/*********************************************************/
/***            BATTLEGROUND QUEUE SYSTEM              ***/
/*********************************************************/

BattlegroundQueue::BattlegroundQueue()
{
    for (uint32 i = 0; i < BG_TEAMS_COUNT; ++i)
    {
        for (uint32 j = 0; j < MAX_BATTLEGROUND_BRACKETS; ++j)
        {
            m_SumOfWaitTimes[i][j] = 0;
            m_WaitTimeLastPlayer[i][j] = 0;
            for (uint32 k = 0; k < COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME; ++k)
                m_WaitTimes[i][j][k] = 0;
        }
    }
}

BattlegroundQueue::~BattlegroundQueue()
{
    m_events.KillAllEvents(false);

    for (int i = 0; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (uint32 j = 0; j < BG_QUEUE_GROUP_TYPES_COUNT; ++j)
        {
            for (GroupsQueueType::iterator itr = m_QueuedGroups[i][j].begin(); itr!= m_QueuedGroups[i][j].end(); ++itr)
                delete (*itr);
        }
    }
}

/*********************************************************/
/***      BATTLEGROUND QUEUE SELECTION POOLS           ***/
/*********************************************************/

// selection pool initialization, used to clean up from prev selection
void BattlegroundQueue::SelectionPool::Init()
{
    SelectedGroups.clear();
    PlayerCount = 0;
}

// remove group info from selection pool
// returns true when we need to try to add new group to selection pool
// returns false when selection pool is ok or when we kicked smaller group than we need to kick
// sometimes it can be called on empty selection pool
bool BattlegroundQueue::SelectionPool::KickGroup(uint32 size)
{
    //find maxgroup or LAST group with size == size and kick it
    bool found = false;
    GroupsQueueType::iterator groupToKick = SelectedGroups.begin();
    for (GroupsQueueType::iterator itr = groupToKick; itr != SelectedGroups.end(); ++itr)
    {
        if (abs((int32)((*itr)->Players.size() - size)) <= 1)
        {
            groupToKick = itr;
            found = true;
        }
        else if (!found && (*itr)->Players.size() >= (*groupToKick)->Players.size())
            groupToKick = itr;
    }
    //if pool is empty, do nothing
    if (GetPlayerCount())
    {
        //update player count
        GroupQueueInfo* ginfo = (*groupToKick);
        SelectedGroups.erase(groupToKick);
        PlayerCount -= ginfo->Players.size();
        //return false if we kicked smaller group or there are enough players in selection pool
        if (ginfo->Players.size() <= size + 1)
            return false;
    }
    return true;
}

// add group to selection pool
// used when building selection pools
// returns true if we can invite more players, or when we added group to selection pool
// returns false when selection pool is full
bool BattlegroundQueue::SelectionPool::AddGroup(GroupQueueInfo* ginfo, uint32 desiredCount)
{
    //if group is larger than desired count - don't allow to add it to pool
    if (!ginfo->IsInvitedToBGInstanceGUID && desiredCount >= PlayerCount + ginfo->Players.size())
    {
        SelectedGroups.push_back(ginfo);
        // increase selected players count
        PlayerCount += ginfo->Players.size();
        return true;
    }
    if (PlayerCount < desiredCount)
        return true;
    return false;
}

/*********************************************************/
/***               BATTLEGROUND QUEUES                 ***/
/*********************************************************/

// add group or player (grp == NULL) to bg queue with the given leader and bg specifications
GroupQueueInfo* BattlegroundQueue::AddGroup(Player* leader, Group* grp, BattlegroundTypeId bgTypeId, PvPDifficultyEntry const*  bracketEntry, uint8 arenaType, bool isRated, bool isPremade, uint32 arenaRating, uint32 matchmakerRating)
{
    BattlegroundBracketId bracketId = bracketEntry->GetBracketId();

    // create new ginfo
    GroupQueueInfo* ginfo            = new GroupQueueInfo;
    ginfo->BgTypeId                  = bgTypeId;
    ginfo->ArenaType                 = arenaType;
    ginfo->IsRated                   = isRated;
    ginfo->IsRatedBG                 = bgTypeId == BATTLEGROUND_RATED_10_VS_10;
    ginfo->IsInvitedToBGInstanceGUID = 0;
    ginfo->JoinTime                  = getMSTime();
    ginfo->RemoveInviteTime          = 0;
    ginfo->Team                      = leader->GetTeam();
    ginfo->ArenaTeamRating           = arenaRating;
    ginfo->ArenaMatchmakerRating     = matchmakerRating;
    ginfo->OpponentsTeamRating       = 0;
    ginfo->OpponentsMatchmakerRating = 0;

    ginfo->Players.clear();

    if (sWorld->getBoolConfig(CONFIG_BATTLEGROUND_IGNORE_FACTION) && !arenaType)
    {
        bool found = false;
        BGFreeSlotQueueContainer& bgQueues = sBattlegroundMgr->GetBGFreeSlotQueueStore(bgTypeId);
        for (BGFreeSlotQueueContainer::iterator itr = bgQueues.begin(); itr != bgQueues.end();)
        {
            Battleground* bg = *itr; ++itr;
            if (!bg->IsRated() && bg->GetTypeID() == bgTypeId && bg->GetBracketId() == bracketId &&
                bg->GetStatus() > STATUS_WAIT_QUEUE && bg->GetStatus() < STATUS_WAIT_LEAVE)
            {
                int32 hordeFree = bg->GetFreeSlotsForTeam(HORDE);
                int32 aliFree   = bg->GetFreeSlotsForTeam(ALLIANCE);
                if (hordeFree == aliFree)
                {
                    if (urand(0,100) < 50)
                        ginfo->Team = HORDE;
                    else
                        ginfo->Team = ALLIANCE;
                }
                else if (hordeFree > aliFree)
                    ginfo->Team = HORDE;
                else
                    ginfo->Team = ALLIANCE;
                found = true;
                break;
            }
        }
        if (!found)
        {
            uint32 qHorde = 0;
            uint32 qAlliance = 0;
            GroupsQueueType::const_iterator itr;
            for (itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].end(); ++itr)
                if (!(*itr)->IsInvitedToBGInstanceGUID)
                    qAlliance += (*itr)->Players.size();
            for (itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].end(); ++itr)
                if (!(*itr)->IsInvitedToBGInstanceGUID)
                    qHorde += (*itr)->Players.size();
            if (qAlliance == qHorde)
            {
                if (qAlliance == 0)
                    ginfo->Team = leader->GetTeam();
                else
                    if (urand(0, 100) < 50)
                        ginfo->Team = HORDE;
                    else
                        ginfo->Team = ALLIANCE;
            }
            else if (qAlliance < qHorde)
                ginfo->Team = ALLIANCE;
            else
                ginfo->Team = HORDE;
        }
    }

    //compute index (if group is premade or joined a rated match) to queues
    uint32 index = 0;
    if (!isRated && !isPremade)
        index += BG_TEAMS_COUNT;
    if (ginfo->Team == HORDE)
        index++;
    TC_LOG_DEBUG("bg.battleground", "Adding Group to BattlegroundQueue bgTypeId : %u, bracket_id : %u, index : %u", bgTypeId, bracketId, index);

    uint32 lastOnlineTime = getMSTime();

    // announce world (this don't need mutex)
    if (isRated && sWorld->getBoolConfig(CONFIG_ARENA_QUEUE_ANNOUNCER_ENABLE))
    {
        if (ginfo->ArenaType == 3)
            sWorld->SendWorldText(LANG_ARENA_QUEUE_ANNOUNCE_WORLD_SOLO, ginfo->ArenaType, ginfo->ArenaType, ginfo->ArenaTeamRating, ginfo->ArenaMatchmakerRating);
        else
            sWorld->SendWorldText(LANG_ARENA_QUEUE_ANNOUNCE_WORLD_JOIN, ginfo->ArenaType, ginfo->ArenaType, ginfo->ArenaTeamRating, ginfo->ArenaMatchmakerRating);
    }

    //add players from group to ginfo
    if (grp)
    {
        for (auto&& member : *grp)
        {
            PlayerQueueInfo& playerInfo = m_QueuedPlayers[member->GetGUID()];
            playerInfo.LastOnlineTime   = lastOnlineTime;
            playerInfo.GroupInfo        = ginfo;
            playerInfo.Role             = SoloQueue::GetRole(member->GetSpecialization());
            // add the pinfo to ginfo's list
            ginfo->Players[member->GetGUID()]  = &playerInfo;
            if (m_isSolo)
            {
                if (playerInfo.Role == SoloQueueRole::Healer)
                    ++m_healersCount;
                else
                    ++m_damagersCount;
            }
        }
    }
    else
    {
        PlayerQueueInfo& playerInfo = m_QueuedPlayers[leader->GetGUID()];
        playerInfo.LastOnlineTime   = lastOnlineTime;
        playerInfo.GroupInfo        = ginfo;
        playerInfo.Role             = SoloQueue::GetRole(leader->GetSpecialization());
        ginfo->Players[leader->GetGUID()]  = &playerInfo;

        if (m_isSolo)
        {
            if (playerInfo.Role == SoloQueueRole::Healer)
                ++m_healersCount;
            else
                ++m_damagersCount;
        }
    }

    //add GroupInfo to m_QueuedGroups
    {
        m_QueuedGroups[bracketId][index].push_back(ginfo);

        //announce to world, this code needs mutex
        if (!isRated && !isPremade && sWorld->getBoolConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE))
        {
            if (Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(ginfo->BgTypeId))
            {
                uint32 MinPlayers = bg->GetMinPlayersPerTeam();
                uint32 qHorde = 0;
                uint32 qAlliance = 0;
                uint32 q_min_level = bracketEntry->minLevel;
                uint32 q_max_level = bracketEntry->maxLevel;
                GroupsQueueType::const_iterator itr;
                for (itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].end(); ++itr)
                    if (!(*itr)->IsInvitedToBGInstanceGUID)
                        qAlliance += (*itr)->Players.size();
                for (itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].end(); ++itr)
                    if (!(*itr)->IsInvitedToBGInstanceGUID)
                        qHorde += (*itr)->Players.size();

                bool playerOnly = sWorld->getBoolConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY);
                Trinity::BattlegroundQueueLocalizedAnnounceBuilder builder(playerOnly ? LANG_BG_QUEUE_ANNOUNCE_SELF : LANG_BG_QUEUE_ANNOUNCE_WORLD, bg, q_min_level, q_max_level,
                        qAlliance, (MinPlayers > qAlliance) ? MinPlayers - qAlliance : (uint32)0, qHorde, (MinPlayers > qHorde) ? MinPlayers - qHorde : (uint32)0, playerOnly);
                Trinity::LocalizedPacketListDo<Trinity::BattlegroundQueueLocalizedAnnounceBuilder> sender(builder);

                // Show queue status to player only (when joining queue)
                if (playerOnly)
                    sender(leader);
                else // System message
                {
                    SessionMap const& sessions = sWorld->GetAllSessions();
                    for (SessionMap::const_iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
                        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
                            sender(itr->second->GetPlayer());
                }
            }
        }
        //release mutex
    }

    return ginfo;
}

void BattlegroundQueue::PlayerInvitedToBGUpdateAverageWaitTime(GroupQueueInfo* ginfo, BattlegroundBracketId bracket_id)
{
    uint32 timeInQueue = getMSTimeDiff(ginfo->JoinTime, getMSTime());
    uint8 team_index = TEAM_ALLIANCE;                    //default set to TEAM_ALLIANCE - or non rated arenas!
    if (!ginfo->ArenaType)
    {
        if (ginfo->Team == HORDE)
            team_index = TEAM_HORDE;
    }
    else
    {
        if (ginfo->IsRated)
            team_index = TEAM_HORDE;                     //for rated arenas use TEAM_HORDE
    }

    //store pointer to arrayindex of player that was added first
    uint32* lastPlayerAddedPointer = &(m_WaitTimeLastPlayer[team_index][bracket_id]);
    //remove his time from sum
    m_SumOfWaitTimes[team_index][bracket_id] -= m_WaitTimes[team_index][bracket_id][(*lastPlayerAddedPointer)];
    //set average time to new
    m_WaitTimes[team_index][bracket_id][(*lastPlayerAddedPointer)] = timeInQueue;
    //add new time to sum
    m_SumOfWaitTimes[team_index][bracket_id] += timeInQueue;
    //set index of last player added to next one
    (*lastPlayerAddedPointer)++;
    (*lastPlayerAddedPointer) %= COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME;
}

uint32 BattlegroundQueue::GetAverageQueueWaitTime(GroupQueueInfo* ginfo, BattlegroundBracketId bracket_id) const
{
    uint8 team_index = TEAM_ALLIANCE;                    //default set to TEAM_ALLIANCE - or non rated arenas!
    if (!ginfo->ArenaType)
    {
        if (ginfo->Team == HORDE)
            team_index = TEAM_HORDE;
    }
    else
    {
        if (ginfo->IsRated)
            team_index = TEAM_HORDE;                     //for rated arenas use TEAM_HORDE
    }
    //check if there is enought values(we always add values > 0)
    if (m_WaitTimes[team_index][bracket_id][COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME - 1])
        return (m_SumOfWaitTimes[team_index][bracket_id] / COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME);
    else
        //if there aren't enough values return 0 - not available
        return 0;
}

//remove player from queue and from group info, if group info is empty then remove it too
void BattlegroundQueue::RemovePlayer(ObjectGuid guid, bool decreaseInvitedCount, bool leave /*= false*/)
{
    int32 bracket_id = -1;                                     // signed for proper for-loop finish
    QueuedPlayersMap::iterator itr;

    //remove player from map, if he's there
    itr = m_QueuedPlayers.find(guid);
    if (itr == m_QueuedPlayers.end())
    {
        std::string playerName = "Unknown";
        if (Player* player = ObjectAccessor::FindPlayer(guid))
            playerName = player->GetName();
        TC_LOG_ERROR("bg.battleground", "BattlegroundQueue: couldn't find player %s (%s)", playerName.c_str(), guid.ToString().c_str());
        return;
    }

    if (m_isSolo)
    {
        if (itr->second.Role == SoloQueueRole::Healer)
            --m_healersCount;
        else
            --m_damagersCount;
    }

    GroupQueueInfo* group = itr->second.GroupInfo;
    GroupsQueueType::iterator group_itr;
    // mostly people with the highest levels are in battlegrounds, thats why
    // we count from MAX_BATTLEGROUND_QUEUES - 1 to 0

    // https://us.battle.net/forums/en/wow/topic/13242714145
    if (leave && group->IsInvitedToBGInstanceGUID && (group->IsRated || group->IsRatedBG))
        if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
            player->ApplyDeserter();

    uint32 index = (group->Team == HORDE) ? BG_QUEUE_PREMADE_HORDE : BG_QUEUE_PREMADE_ALLIANCE;

    for (int32 bracket_id_tmp = MAX_BATTLEGROUND_BRACKETS - 1; bracket_id_tmp >= 0 && bracket_id == -1; --bracket_id_tmp)
    {
        //we must check premade and normal team's queue - because when players from premade are joining bg,
        //they leave groupinfo so we can't use its players size to find out index
        for (uint32 j = index; j < BG_QUEUE_GROUP_TYPES_COUNT; j += BG_TEAMS_COUNT)
        {
            GroupsQueueType::iterator k = m_QueuedGroups[bracket_id_tmp][j].begin();
            for (; k != m_QueuedGroups[bracket_id_tmp][j].end(); ++k)
            {
                if ((*k) == group)
                {
                    bracket_id = bracket_id_tmp;
                    group_itr = k;
                    //we must store index to be able to erase iterator
                    index = j;
                    break;
                }
            }
        }
    }

    //player can't be in queue without group, but just in case
    if (bracket_id == -1)
    {
        TC_LOG_ERROR("bg.battleground", "BattlegroundQueue: ERROR Cannot find groupinfo for %s", guid.ToString().c_str());
        return;
    }
    TC_LOG_DEBUG("bg.battleground", "BattlegroundQueue: Removing player GUID %s, from bracket_id %u", guid.ToString().c_str(), (uint32)bracket_id);

    // ALL variables are correctly set
    // We can ignore leveling up in queue - it should not cause crash
    // remove player from group
    // if only one player there, remove group

    // remove player queue info from group queue info
    std::map<ObjectGuid, PlayerQueueInfo*>::iterator pitr = group->Players.find(guid);
    if (pitr != group->Players.end())
        group->Players.erase(pitr);

    // if invited to bg, and should decrease invited count, then do it
    if (decreaseInvitedCount && group->IsInvitedToBGInstanceGUID)
        if (Battleground* bg = sBattlegroundMgr->GetBattleground(group->IsInvitedToBGInstanceGUID, group->BgTypeId))
            bg->DecreaseInvitedCount(group->Team);

    // remove player queue info
    m_QueuedPlayers.erase(itr);

    // if player leaves queue and he is invited to rated arena match, then he have to lose
    if (group->IsInvitedToBGInstanceGUID && (group->IsRated || group->IsRatedBG) && decreaseInvitedCount)
    {
        TC_LOG_DEBUG("bg.battleground", "UPDATING memberLost's personal arena rating for %s by opponents rating: %u", guid.ToString().c_str(), group->OpponentsTeamRating);

        if (Battleground* bg = sBattlegroundMgr->GetBattleground(group->IsInvitedToBGInstanceGUID, group->BgTypeId))
        {
            if (ArenaTeam* team = bg->GetArenaTeam(group->Team))
            {
                if (m_isSolo)
                {
                    sBattlegroundMgr->ApplyDeserter(guid, sWorld->getIntConfig(CONFIG_SOLO_QUEUE_DESERTER_TIMER_NOT_ACCEPT));
                    int32 change = -int32(sWorld->getIntConfig(CONFIG_SOLO_QUEUE_NOT_ACCEPTED_RATING_PENALTY));
                    auto member = team->GetMember(guid);
                    if (change + int32(member->Rating) < 0)
                        change = -int32(member->Rating);
                    member->Rating += change;
                    team->SetPlayerHandled(guid);
                }
                else if (!bg->GetInvitedCount(group->Team))
                    bg->EndBattleground(bg->GetOtherTeam(group->Team));
            }
        }
    }

    // remove group queue info if needed
    if (group->Players.empty())
    {
        m_QueuedGroups[bracket_id][index].erase(group_itr);
        delete group;
        return;
    }

    // if group wasn't empty, so it wasn't deleted, and player have left a rated
    // queue -> everyone from the group should leave too
    // don't remove recursively if already invited to bg!
    if (!group->IsInvitedToBGInstanceGUID && (group->IsRated || group->IsRatedBG))
    {
        // remove next player, this is recursive
        // first send removal information
        if (Player* plr2 = ObjectAccessor::FindPlayer(group->Players.begin()->first))
        {
            Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(group->BgTypeId);
            BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(group->BgTypeId, group->ArenaType);
            uint32 queueSlot = plr2->GetBattlegroundQueueIndex(bgQueueTypeId);

            plr2->RemoveBattlegroundQueueId(bgQueueTypeId); // must be called this way, because if you move this call to
                                                            // queue->removeplayer, it causes bugs
            WorldPacket data;
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, plr2, queueSlot, STATUS_NONE, plr2->GetBattlegroundQueueJoinTime(group->BgTypeId), 0, 0);
            plr2->GetSession()->SendPacket(&data);
        }
        // then actually delete, this may delete the group as well!
        RemovePlayer(group->Players.begin()->first, decreaseInvitedCount, leave);
    }
}

//returns true when player pl_guid is in queue and is invited to bgInstanceGuid
bool BattlegroundQueue::IsPlayerInvited(uint64 pl_guid, const uint32 bgInstanceGuid, const uint32 removeTime)
{
    QueuedPlayersMap::const_iterator qItr = m_QueuedPlayers.find(pl_guid);
    return (qItr != m_QueuedPlayers.end()
        && qItr->second.GroupInfo->IsInvitedToBGInstanceGUID == bgInstanceGuid
        && qItr->second.GroupInfo->RemoveInviteTime == removeTime);
}

bool BattlegroundQueue::GetPlayerGroupInfoData(uint64 guid, GroupQueueInfo* ginfo)
{
    QueuedPlayersMap::const_iterator qItr = m_QueuedPlayers.find(guid);
    if (qItr == m_QueuedPlayers.end())
        return false;
    *ginfo = *(qItr->second.GroupInfo);
    return true;
}

uint32 BattlegroundQueue::GetPlayersInQueue(TeamId id)
{
    return m_SelectionPools[id].GetPlayerCount();
}

bool BattlegroundQueue::InviteGroupToBG(GroupQueueInfo* ginfo, Battleground* bg, uint32 side)
{
    // set side if needed
    if (side)
        ginfo->Team = side;

    if (!ginfo->IsInvitedToBGInstanceGUID)
    {
        // not yet invited
        // set invitation
        ginfo->IsInvitedToBGInstanceGUID = bg->GetInstanceID();
        BattlegroundTypeId bgTypeId = bg->GetTypeID();
        BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, bg->GetArenaType());
        BattlegroundBracketId bracket_id = bg->GetBracketId();

        ginfo->RemoveInviteTime = getMSTime() + INVITE_ACCEPT_WAIT_TIME;

        // loop through the players
        for (std::map<ObjectGuid, PlayerQueueInfo*>::iterator itr = ginfo->Players.begin(); itr != ginfo->Players.end(); ++itr)
        {
            // get the player
            Player* player = ObjectAccessor::FindPlayer(itr->first);
            // if offline, skip him, this should not happen - player is removed from queue when he logs out
            if (!player)
                continue;

            bg->OnInvite(player, ginfo);

            // invite the player
            PlayerInvitedToBGUpdateAverageWaitTime(ginfo, bracket_id);
            //sBattlegroundMgr->InvitePlayer(player, bg, ginfo->Team);

            // set invited player counters
            bg->IncreaseInvitedCount(ginfo->Team);

            if (bgQueueTypeId == BATTLEGROUND_QUEUE_3v3 && player->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_SOLO) != PLAYER_MAX_BATTLEGROUND_QUEUES)
                bgQueueTypeId = BATTLEGROUND_QUEUE_SOLO;

            player->SetInviteForBattlegroundQueueType(bgQueueTypeId, ginfo->IsInvitedToBGInstanceGUID);

            // create remind invite events
            BGQueueInviteEvent* inviteEvent = new BGQueueInviteEvent(player->GetGUID(), ginfo->IsInvitedToBGInstanceGUID, bgTypeId, ginfo->ArenaType, ginfo->RemoveInviteTime);
            m_events.AddEvent(inviteEvent, m_events.CalculateTime(INVITATION_REMIND_TIME));
            // create automatic remove events
            BGQueueRemoveEvent* removeEvent = new BGQueueRemoveEvent(player->GetGUID(), ginfo->IsInvitedToBGInstanceGUID, bgTypeId, bgQueueTypeId, ginfo->RemoveInviteTime);
            m_events.AddEvent(removeEvent, m_events.CalculateTime(INVITE_ACCEPT_WAIT_TIME));

            WorldPacket data;

            uint32 queueSlot = player->GetBattlegroundQueueIndex(bgQueueTypeId);

            TC_LOG_DEBUG("bg.battleground", "Battleground: invited player %s (%u) to BG instance %u queueindex %u bgtype %u",
                 player->GetName().c_str(), player->GetGUID().GetCounter(), bg->GetInstanceID(), queueSlot, bg->GetTypeID());

            // send status packet
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME, player->GetBattlegroundQueueJoinTime(bgTypeId), ginfo->ArenaType);
            player->GetSession()->SendPacket(&data);
        }
        return true;
    }

    return false;
}

/*
This function is inviting players to already running battlegrounds
Invitation type is based on config file
large groups are disadvantageous, because they will be kicked first if invitation type = 1
*/
void BattlegroundQueue::FillPlayersToBG(Battleground* bg, BattlegroundBracketId bracket_id)
{
    int32 hordeFree = bg->GetFreeSlotsForTeam(HORDE);
    int32 aliFree   = bg->GetFreeSlotsForTeam(ALLIANCE);

    //iterator for iterating through bg queue
    GroupsQueueType::const_iterator Ali_itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE].begin();
    //count of groups in queue - used to stop cycles
    uint32 aliCount = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE].size();
    //index to queue which group is current
    uint32 aliIndex = 0;
    for (; aliIndex < aliCount && m_SelectionPools[TEAM_ALLIANCE].AddGroup((*Ali_itr), aliFree); aliIndex++)
        ++Ali_itr;
    //the same thing for horde
    GroupsQueueType::const_iterator Horde_itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_HORDE].begin();
    uint32 hordeCount = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_HORDE].size();
    uint32 hordeIndex = 0;
    for (; hordeIndex < hordeCount && m_SelectionPools[TEAM_HORDE].AddGroup((*Horde_itr), hordeFree); hordeIndex++)
        ++Horde_itr;

    //if ofc like BG queue invitation is set in config, then we are happy
    if (sWorld->getIntConfig(CONFIG_BATTLEGROUND_INVITATION_TYPE) == 0)
        return;

    /*
    if we reached this code, then we have to solve NP - complete problem called Subset sum problem
    So one solution is to check all possible invitation subgroups, or we can use these conditions:
    1. Last time when BattlegroundQueue::Update was executed we invited all possible players - so there is only small possibility
        that we will invite now whole queue, because only 1 change has been made to queues from the last BattlegroundQueue::Update call
    2. Other thing we should consider is group order in queue
    */

    // At first we need to compare free space in bg and our selection pool
    int32 diffAli   = aliFree   - int32(m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount());
    int32 diffHorde = hordeFree - int32(m_SelectionPools[TEAM_HORDE].GetPlayerCount());
    while (abs(diffAli - diffHorde) > 1 && (m_SelectionPools[TEAM_HORDE].GetPlayerCount() > 0 || m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() > 0))
    {
        //each cycle execution we need to kick at least 1 group
        if (diffAli < diffHorde)
        {
            //kick alliance group, add to pool new group if needed
            if (m_SelectionPools[TEAM_ALLIANCE].KickGroup(diffHorde - diffAli))
            {
                for (; aliIndex < aliCount && m_SelectionPools[TEAM_ALLIANCE].AddGroup((*Ali_itr), (aliFree >= diffHorde) ? aliFree - diffHorde : 0); aliIndex++)
                    ++Ali_itr;
            }
            //if ali selection is already empty, then kick horde group, but if there are less horde than ali in bg - break;
            if (!m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount())
            {
                if (aliFree <= diffHorde + 1)
                    break;
                m_SelectionPools[TEAM_HORDE].KickGroup(diffHorde - diffAli);
            }
        }
        else
        {
            //kick horde group, add to pool new group if needed
            if (m_SelectionPools[TEAM_HORDE].KickGroup(diffAli - diffHorde))
            {
                for (; hordeIndex < hordeCount && m_SelectionPools[TEAM_HORDE].AddGroup((*Horde_itr), (hordeFree >= diffAli) ? hordeFree - diffAli : 0); hordeIndex++)
                    ++Horde_itr;
            }
            if (!m_SelectionPools[TEAM_HORDE].GetPlayerCount())
            {
                if (hordeFree <= diffAli + 1)
                    break;
                m_SelectionPools[TEAM_ALLIANCE].KickGroup(diffAli - diffHorde);
            }
        }
        //count diffs after small update
        diffAli   = aliFree   - int32(m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount());
        diffHorde = hordeFree - int32(m_SelectionPools[TEAM_HORDE].GetPlayerCount());
    }
}

// this method checks if premade versus premade battleground is possible
// then after 30 mins (default) in queue it moves premade group to normal queue
// it tries to invite as much players as it can - to MaxPlayersPerTeam, because premade groups have more than MinPlayersPerTeam players
bool BattlegroundQueue::CheckPremadeMatch(BattlegroundBracketId bracket_id, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam)
{
    //check match
    if (!m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].empty() && !m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].empty())
    {
        //start premade match
        //if groups aren't invited
        GroupsQueueType::const_iterator ali_group, horde_group;
        for (ali_group = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].begin(); ali_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].end(); ++ali_group)
            if (!(*ali_group)->IsInvitedToBGInstanceGUID)
                break;
        for (horde_group = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].begin(); horde_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].end(); ++horde_group)
            if (!(*horde_group)->IsInvitedToBGInstanceGUID)
                break;

        if (ali_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].end() && horde_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].end())
        {
            m_SelectionPools[TEAM_ALLIANCE].AddGroup((*ali_group), MaxPlayersPerTeam);
            m_SelectionPools[TEAM_HORDE].AddGroup((*horde_group), MaxPlayersPerTeam);
            //add groups/players from normal queue to size of bigger group
            uint32 maxPlayers = std::min(m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount(), m_SelectionPools[TEAM_HORDE].GetPlayerCount());
            GroupsQueueType::const_iterator itr;
            for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
            {
                for (itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].end(); ++itr)
                {
                    //if itr can join BG and player count is less that maxPlayers, then add group to selectionpool
                    if (!(*itr)->IsInvitedToBGInstanceGUID && !m_SelectionPools[i].AddGroup((*itr), maxPlayers))
                        break;
                }
            }
            //premade selection pools are set
            return true;
        }
    }
    // now check if we can move group from Premade queue to normal queue (timer has expired) or group size lowered!!
    // this could be 2 cycles but i'm checking only first team in queue - it can cause problem -
    // if first is invited to BG and seconds timer expired, but we can ignore it, because players have only 80 seconds to click to enter bg
    // and when they click or after 80 seconds the queue info is removed from queue
    uint32 time_before = getMSTime() - sWorld->getIntConfig(CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH);
    for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        if (!m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].empty())
        {
            GroupsQueueType::iterator itr = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].begin();
            if (!(*itr)->IsInvitedToBGInstanceGUID && ((*itr)->JoinTime < time_before || (*itr)->Players.size() < MinPlayersPerTeam))
            {
                //we must insert group to normal queue and erase pointer from premade queue
                m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].push_front((*itr));
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].erase(itr);
            }
        }
    }
    //selection pools are not set
    return false;
}

// this method tries to create battleground or arena with MinPlayersPerTeam against MinPlayersPerTeam
bool BattlegroundQueue::CheckNormalMatch(Battleground* /*bg_template*/, BattlegroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers)
{
    GroupsQueueType::const_iterator itr_team[BG_TEAMS_COUNT];
    for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        itr_team[i] = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].begin();
        for (; itr_team[i] != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].end(); ++(itr_team[i]))
        {
            if (!(*(itr_team[i]))->IsInvitedToBGInstanceGUID)
            {
                m_SelectionPools[i].AddGroup(*(itr_team[i]), maxPlayers);
                if (m_SelectionPools[i].GetPlayerCount() >= minPlayers)
                    break;
            }
        }
    }
    //try to invite same number of players - this cycle may cause longer wait time even if there are enough players in queue, but we want ballanced bg
    uint32 j = TEAM_ALLIANCE;
    if (m_SelectionPools[TEAM_HORDE].GetPlayerCount() < m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount())
        j = TEAM_HORDE;
    if (sWorld->getIntConfig(CONFIG_BATTLEGROUND_INVITATION_TYPE) != 0
        && m_SelectionPools[TEAM_HORDE].GetPlayerCount() >= minPlayers && m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() >= minPlayers)
    {
        //we will try to invite more groups to team with less players indexed by j
        ++(itr_team[j]);                                         //this will not cause a crash, because for cycle above reached break;
        for (; itr_team[j] != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + j].end(); ++(itr_team[j]))
        {
            if (!(*(itr_team[j]))->IsInvitedToBGInstanceGUID)
                if (!m_SelectionPools[j].AddGroup(*(itr_team[j]), m_SelectionPools[(j + 1) % BG_TEAMS_COUNT].GetPlayerCount()))
                    break;
        }
        // do not allow to start bg with more than 2 players more on 1 faction
        if (abs((int32)(m_SelectionPools[TEAM_HORDE].GetPlayerCount() - m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount())) > 2)
            return false;
    }
    //allow 1v0 if debug bg
    if (sBattlegroundMgr->IsTesting() && (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() || m_SelectionPools[TEAM_HORDE].GetPlayerCount()))
        return true;
    //return true if there are enough players in selection pools - enable to work .debug bg command correctly
    return m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() >= minPlayers && m_SelectionPools[TEAM_HORDE].GetPlayerCount() >= minPlayers;
}

// this method will check if we can invite players to same faction skirmish match
bool BattlegroundQueue::CheckSkirmishForSameFaction(BattlegroundBracketId bracket_id, uint32 minPlayersPerTeam)
{
    if (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() < minPlayersPerTeam && m_SelectionPools[TEAM_HORDE].GetPlayerCount() < minPlayersPerTeam)
        return false;
    uint32 teamIndex = TEAM_ALLIANCE;
    uint32 otherTeam = TEAM_HORDE;
    uint32 otherTeamId = HORDE;
    if (m_SelectionPools[TEAM_HORDE].GetPlayerCount() == minPlayersPerTeam)
    {
        teamIndex = TEAM_HORDE;
        otherTeam = TEAM_ALLIANCE;
        otherTeamId = ALLIANCE;
    }
    //clear other team's selection
    m_SelectionPools[otherTeam].Init();
    //store last ginfo pointer
    GroupQueueInfo* ginfo = m_SelectionPools[teamIndex].SelectedGroups.back();
    //set itr_team to group that was added to selection pool latest
    GroupsQueueType::iterator itr_team = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].begin();
    for (; itr_team != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr_team)
        if (ginfo == *itr_team)
            break;
    if (itr_team == m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end())
        return false;
    GroupsQueueType::iterator itr_team2 = itr_team;
    ++itr_team2;
    //invite players to other selection pool
    for (; itr_team2 != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr_team2)
    {
        //if selection pool is full then break;
        if (!(*itr_team2)->IsInvitedToBGInstanceGUID && !m_SelectionPools[otherTeam].AddGroup(*itr_team2, minPlayersPerTeam))
            break;
    }
    if (m_SelectionPools[otherTeam].GetPlayerCount() != minPlayersPerTeam)
        return false;

    //here we have correct 2 selections and we need to change one teams team and move selection pool teams to other team's queue
    for (GroupsQueueType::iterator itr = m_SelectionPools[otherTeam].SelectedGroups.begin(); itr != m_SelectionPools[otherTeam].SelectedGroups.end(); ++itr)
    {
        //set correct team
        (*itr)->Team = otherTeamId;
        //add team to other queue
        m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + otherTeam].push_front(*itr);
        //remove team from old queue
        GroupsQueueType::iterator itr2 = itr_team;
        ++itr2;
        for (; itr2 != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr2)
        {
            if (*itr2 == *itr)
            {
                m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].erase(itr2);
                break;
            }
        }
    }
    return true;
}

void BattlegroundQueue::UpdateEvents(uint32 diff)
{
    m_events.Update(diff);
}

/*
this method is called when group is inserted, or player / group is removed from BG Queue - there is only one player's status changed, so we don't use while (true) cycles to invite whole queue
it must be called after fully adding the members of a group to ensure group joining
should be called from Battleground::RemovePlayer function in some cases
*/
void BattlegroundQueue::BattlegroundQueueUpdate(uint32 /*diff*/, BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id, uint8 arenaType, bool isRated, uint32 arenaRating)
{
    //if no players in queue - do nothing
    if (m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].empty() &&
        m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].empty() &&
        m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE].empty() &&
        m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_HORDE].empty())
        return;

    // battleground with free slot for player should be always in the beggining of the queue
    // maybe it would be better to create bgfreeslotqueue for each bracket_id
    BGFreeSlotQueueContainer& bgQueues = sBattlegroundMgr->GetBGFreeSlotQueueStore(bgTypeId);
    for (BGFreeSlotQueueContainer::iterator itr = bgQueues.begin(); itr != bgQueues.end();)
    {
        Battleground* bg = *itr; ++itr;
        // DO NOT allow queue manager to invite new player to rated games
        if (!bg->IsRated() && bg->GetTypeID() == bgTypeId && bg->GetBracketId() == bracket_id &&
            bg->GetStatus() > STATUS_WAIT_QUEUE && bg->GetStatus() < STATUS_WAIT_LEAVE)
        {
            // clear selection pools
            m_SelectionPools[TEAM_ALLIANCE].Init();
            m_SelectionPools[TEAM_HORDE].Init();

            // call a function that does the job for us
            FillPlayersToBG(bg, bracket_id);

            // now everything is set, invite players
            for (GroupsQueueType::const_iterator citr = m_SelectionPools[TEAM_ALLIANCE].SelectedGroups.begin(); citr != m_SelectionPools[TEAM_ALLIANCE].SelectedGroups.end(); ++citr)
                InviteGroupToBG((*citr), bg, (*citr)->Team);

            for (GroupsQueueType::const_iterator citr = m_SelectionPools[TEAM_HORDE].SelectedGroups.begin(); citr != m_SelectionPools[TEAM_HORDE].SelectedGroups.end(); ++citr)
                InviteGroupToBG((*citr), bg, (*citr)->Team);

            if (!bg->HasFreeSlots())
                bg->RemoveFromBGFreeSlotQueue();
        }
    }

    // finished iterating through the bgs with free slots, maybe we need to create a new bg

    Battleground* bgTemplate = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bgTemplate)
    {
        TC_LOG_ERROR("bg.battleground", "Battleground: Update: bg template not found for %u", bgTypeId);
        return;
    }

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketById(bgTemplate->GetMapId(), bracket_id);
    if (!bracketEntry)
    {
        TC_LOG_ERROR("bg.battleground", "Battleground: Update: bg bracket entry not found for map %u bracket id %u", bgTemplate->GetMapId(), bracket_id);
        return;
    }

    // get the min. players per team, properly for larger arenas as well. (must have full teams for arena matches!)
    uint32 MinPlayersPerTeam = bgTemplate->GetMinPlayersPerTeam();
    uint32 MaxPlayersPerTeam = bgTemplate->GetMaxPlayersPerTeam();

    if (bgTemplate->IsArena())
    {
        MaxPlayersPerTeam = arenaType;
        MinPlayersPerTeam = sBattlegroundMgr->IsArenaTesting() ? 1 : arenaType;
    }
    else if (bgTemplate->IsRatedBG())
    {
        MinPlayersPerTeam = 2;
        MaxPlayersPerTeam = 2;
    }
    else if (sBattlegroundMgr->IsTesting())
        MinPlayersPerTeam = 1;

    m_SelectionPools[TEAM_ALLIANCE].Init();
    m_SelectionPools[TEAM_HORDE].Init();

    if (bgTemplate->IsBattleground() && !bgTemplate->IsRatedBG())
    {
        if (CheckPremadeMatch(bracket_id, MinPlayersPerTeam, MaxPlayersPerTeam))
        {
            // create new battleground
            Battleground* bg2 = sBattlegroundMgr->CreateNewBattleground(bgTypeId, bracketEntry, 0, false);
            if (!bg2)
            {
                TC_LOG_ERROR("bg.battleground", "BattlegroundQueue::Update - Cannot create battleground: %u", bgTypeId);
                return;
            }
            // invite those selection pools
            for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
                for (GroupsQueueType::const_iterator citr = m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups.begin(); citr != m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups.end(); ++citr)
                    InviteGroupToBG((*citr), bg2, (*citr)->Team);

            bg2->StartBattleground();
            //clear structures
            m_SelectionPools[TEAM_ALLIANCE].Init();
            m_SelectionPools[TEAM_HORDE].Init();
        }
    }

    // now check if there are in queues enough players to start new game of (normal battleground, or non-rated arena)
    if (!isRated)
    {
        // if there are enough players in pools, start new battleground or non rated arena
        if (CheckNormalMatch(bgTemplate, bracket_id, MinPlayersPerTeam, MaxPlayersPerTeam)
            || (bgTemplate->IsArena() && CheckSkirmishForSameFaction(bracket_id, MinPlayersPerTeam)))
        {
            // we successfully created a pool
            Battleground* bg2 = sBattlegroundMgr->CreateNewBattleground(bgTypeId, bracketEntry, arenaType, false);
            if (!bg2)
            {
                TC_LOG_ERROR("bg.battleground", "BattlegroundQueue::Update - Cannot create battleground: %u", bgTypeId);
                return;
            }

            // invite those selection pools
            for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
                for (GroupsQueueType::const_iterator citr = m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups.begin(); citr != m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups.end(); ++citr)
                    InviteGroupToBG((*citr), bg2, (*citr)->Team);
            // start bg
            bg2->StartBattleground();
        }
    }
    else if (bgTemplate->IsArena() || bgTemplate->IsRatedBG())
    {
        // found out the minimum and maximum ratings the newly added team should battle against
        // arenaRating is the rating of the latest joined team, or 0
        // 0 is on (automatic update call) and we must set it to team's with longest wait time
        uint32 mmrMaxDiff = 0;
        if (!arenaRating)
        {
            GroupQueueInfo* front1 = nullptr;
            GroupQueueInfo* front2 = nullptr;
            for (auto&& iter : m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE])
            {
                if (!iter->IsInvitedToBGInstanceGUID)
                {
                    front1 = iter;
                    arenaRating = front1->ArenaMatchmakerRating;
                    // progressive mmr
                    float timeinq = getMSTime() - front1->JoinTime;
                    float mmrSteps = floor(timeinq / sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_TIMER));
                    mmrMaxDiff = mmrSteps * sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_STEPSIZE);
                }
            }

            for (auto&& iter : m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE])
            {
                if (!iter->IsInvitedToBGInstanceGUID)
                {
                    front2 = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].front();
                    arenaRating = front2->ArenaMatchmakerRating;
                    // progressive mmr
                    float mmrSteps = floor((getMSTime() - front2->JoinTime) / sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_TIMER));
                    mmrMaxDiff = mmrSteps * sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_STEPSIZE);
                }
            }

            if (front1 && front2)
            {
                if (front1->JoinTime < front2->JoinTime)
                {
                    arenaRating = front1->ArenaMatchmakerRating;
                    // progressive mmr
                    float mmrSteps = floor((getMSTime() - front1->JoinTime) / sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_TIMER));
                    mmrMaxDiff = mmrSteps * sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_STEPSIZE);
                }
            }
            else if (!front1 && !front2)
                return; //queues are empty
        }

        //set rating range
        uint32 arenaMinRating = (arenaRating <= sBattlegroundMgr->GetMaxRatingDifference()) ? 0 : arenaRating - sBattlegroundMgr->GetMaxRatingDifference();
        uint32 arenaMaxRating = arenaRating + sBattlegroundMgr->GetMaxRatingDifference();
        // if max rating difference is set and the time past since server startup is greater than the rating discard time
        // (after what time the ratings aren't taken into account when making teams) then
        // the discard time is current_time - time_to_discard, teams that joined after that, will have their ratings taken into account
        // else leave the discard time on 0, this way all ratings will be discarded
        uint32 discardTime = getMSTime();
        discardTime -= std::min(discardTime, sBattlegroundMgr->GetRatingDiscardTimer());
        // progressive mmr
        if (mmrMaxDiff > 0)
        {
            arenaMinRating = (mmrMaxDiff >= arenaMinRating) ? 0 : arenaMinRating - mmrMaxDiff;
            arenaMaxRating = mmrMaxDiff + arenaMaxRating;
        }

        // we need to find 2 teams which will play next game
        GroupsQueueType::iterator groups[BG_TEAMS_COUNT];
        uint8 found = 0;
        uint8 team = 0;

        for (uint8 i = BG_QUEUE_PREMADE_ALLIANCE; i < BG_QUEUE_NORMAL_ALLIANCE; ++i)
        {
            // take the group that joined first
            for (auto it = m_QueuedGroups[bracket_id][i].begin(); it != m_QueuedGroups[bracket_id][i].end(); ++it)
            {
                GroupQueueInfo* info = *it;
                // if group match conditions, then add it to pool
                if (!info->IsInvitedToBGInstanceGUID
                    && ((info->ArenaMatchmakerRating >= arenaMinRating && info->ArenaMatchmakerRating <= arenaMaxRating)
                        || info->JoinTime < discardTime))
                {
                    groups[found++] = it;
                    team = i;
                    break;
                }
            }
        }

        if (!found)
            return;

        if (found == 1)
        {
            auto it = groups[0];
            ++it;
            for (; it != m_QueuedGroups[bracket_id][team].end(); ++it)
            {
                GroupQueueInfo* info = *it;
                if (!info->IsInvitedToBGInstanceGUID &&
                    ((info->ArenaMatchmakerRating >= arenaMinRating && info->ArenaMatchmakerRating <= arenaMaxRating)
                        || info->JoinTime < discardTime))
                {
                    groups[found++] = it;
                    break;
                }
            }
        }

        //if we have 2 teams, then start new arena and invite players!
        if (found == 2)
        {
            GroupQueueInfo* aTeam = *groups[TEAM_ALLIANCE];
            GroupQueueInfo* hTeam = *groups[TEAM_HORDE];
            bool ratedBG = bgTemplate->IsRatedBG();
            Battleground* arena = sBattlegroundMgr->CreateNewBattleground(bgTypeId, bracketEntry, arenaType, !ratedBG);
            if (!arena)
            {
                TC_LOG_ERROR("bg.battleground", "BattlegroundQueue::Update couldn't create arena instance for rated arena match!");
                return;
            }

            aTeam->OpponentsTeamRating = hTeam->ArenaTeamRating;
            hTeam->OpponentsTeamRating = aTeam->ArenaTeamRating;
            aTeam->OpponentsMatchmakerRating = hTeam->ArenaMatchmakerRating;
            hTeam->OpponentsMatchmakerRating = aTeam->ArenaMatchmakerRating;

            // now we must move team if we changed its faction to another faction queue, because then we will spam log by errors in Queue::RemovePlayer
            if (aTeam->Team != ALLIANCE)
            {
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].push_front(aTeam);
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].erase(groups[TEAM_ALLIANCE]);
            }
            if (hTeam->Team != HORDE)
            {
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].push_front(hTeam);
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].erase(groups[TEAM_HORDE]);
            }

            arena->SetArenaTeam(ALLIANCE, std::make_shared<ArenaTeam>(aTeam));
            arena->SetArenaTeam(HORDE, std::make_shared<ArenaTeam>(hTeam));
            InviteGroupToBG(aTeam, arena, ALLIANCE);
            InviteGroupToBG(hTeam, arena, HORDE);

            TC_LOG_DEBUG("bg.battleground", "Starting rated arena match!");
            arena->StartBattleground();
        }
    }
}

SoloPlayer::SoloPlayer(Player* player)
    : Target(player)
{
    Class = player->GetClass();
    Specialization = Specializations(player->GetTalentSpecialization());
    Role = SoloQueue::GetRole(Specialization);
}

bool SoloGroup::Create(GroupQueueInfo* ginfo)
{
    for (auto&& it : ginfo->Players)
    {
        Player* player = ObjectAccessor::FindConnectedPlayer(it.first);
        if (!player)
            return false;

        SoloPlayer p = { player };
        p.Info = ginfo;
        Members.push_back(p);
        Team = ginfo->Team;
        Rating = ginfo->ArenaMatchmakerRating;
    }
    return true;
}

void SoloGroup::Reset()
{
    if (!InTeam)
        return;

    InTeam = false;
    Melee = 0;
    Ranged = 0;
    Healer = 0;
    Tank = 0;
    Rating = Members.front().Info->ArenaMatchmakerRating;

    for (auto&& it : Current)
        if (it.OriginalGroup != this)
            it.OriginalGroup->Reset();

    Current.clear();
    for (auto&& it : Members)
        AddPlayer(it);
}

void SoloGroup::AddPlayer(SoloPlayer const& p)
{
    Current.push_back(p);

    uint8 dpsCount = sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ALLOW_WITHOUT_HEALERS) ? 3 : 2;

    switch (p.Role)
    {
        case SoloQueueRole::Melee:     ASSERT(Melee < dpsCount);    ++Melee;    break;
        case SoloQueueRole::Ranged:    ASSERT(Ranged < dpsCount);   ++Ranged;   break;
        case SoloQueueRole::Healer:    ASSERT(!Healer);             ++Healer;   break;
        case SoloQueueRole::Tank:      ASSERT(!Tank);               ++Tank;     break;
    }
}

SoloQueue::SoloQueue()
{
    m_isSolo = true;
}

bool SoloQueue::IsCompatible(SoloGroup const* lhs, SoloGroup const* rhs, bool strong)
{
    if (lhs == rhs)
        return false;

    if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_INTERFACTIONAL_TEAMS) && lhs->Team != rhs->Team)
        return false;

    if (lhs->Current.size() == 2 && rhs->Members.size() == 2)
        return false;

    uint32 melee  = lhs->Melee;
    uint32 ranged = lhs->Ranged;
    uint32 healer = lhs->Healer;
    uint32 tank   = lhs->Tank;

    for (auto&& it : rhs->Members)
    {
        uint32 oldteammates = 0;
        for (auto&& it2 : lhs->Current)
        {
            if (it2.Class == it.Class)
                return false;

            auto& x = m_rules[it2.Target->GetGUID()];
            if (x.has(it.Target->GetGUID()))
                ++oldteammates;
        }

        if (oldteammates == 2)
            return false;

        uint8 dpsCount = sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ALLOW_WITHOUT_HEALERS) ? 3 : 2;

        switch (it.Role)
        {
            case SoloQueueRole::Melee:
                if (strong && melee)
                    return false;
                if (melee + ranged + tank == dpsCount)
                    return false;
                ++melee;
                break;
            case SoloQueueRole::Ranged:
                if (strong && ranged)
                    return false;
                if (melee + ranged + tank == dpsCount)
                    return false;
                ++ranged;
                break;
            case SoloQueueRole::Healer:
                if (healer)
                    return false;
                ++healer;
                break;
            case SoloQueueRole::Tank:
                if (tank || melee + ranged + tank == dpsCount)
                    return false;
                ++tank;
                break;
        }
    }

    if (!m_disabled.empty())
    {
        TeamSetup ts;
        size_t i = 0;
        ts[i].Class = lhs->Current[0].Class;
        ts[i].Spec = lhs->Current[0].Specialization;
        ++i;

        if (lhs->Current.size() == 2)
        {
            ts[i].Class = lhs->Current[1].Class;
            ts[i].Spec = lhs->Current[1].Specialization;
            ++i;
        }
        ts[i].Class = rhs->Members[0].Class;
        ts[i].Spec = rhs->Members[0].Specialization;
        ++i;

        if (rhs->Members.size() == 2)
        {
            ts[i].Class = rhs->Members[1].Class;
            ts[i].Spec = rhs->Members[1].Specialization;
            ++i;
        }

        ts.sort(i);

        for (auto&& it : m_disabled)
        {
            if (it[2].Class)    // Full rule.
            {
                if (it[0] == ts[0] && it[1] == ts[1] && it[2] == ts[2])
                    return false;
            }
            else                // Rule for 2 of 3 players.
            {
                for (size_t j = 0; j < i; ++j)
                {
                    if (ts[j] == it[0])
                    {
                        for (size_t j2 = 0; j2 < i; ++j2)
                        {
                            if (ts[j2] == it[1])
                                return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool SoloQueue::MakeTeams(SoloGroup* group, SoloGroup* opponent, bool strong)
{
    SoloGroup* first = group->Rating < opponent->Rating ? group : opponent;
    SoloGroup* second = group->Rating < opponent->Rating ? opponent : group;

    if (!Search(first, second->Rating, strong))
        return false;
    if (!Search(second, first->Rating, strong))
        return false;

    first = group->Rating < opponent->Rating ? group : opponent;
    second = group->Rating < opponent->Rating ? opponent : group;

    if (!Search(first, second->Rating, strong))
        return false;
    if (!Search(second, first->Rating, strong))
        return false;

    first = group->Rating < opponent->Rating ? group : opponent;
    second = group->Rating < opponent->Rating ? opponent : group;

    if (!Search(first, second->Rating, strong))
        return false;
    if (!Search(second, first->Rating, strong))
        return false;

    return true;
}

bool SoloQueue::Search(SoloGroup* group, uint32 desired, bool strong)
{
    if (group->Current.size() == SoloQueue::TeamSize())
        return true;

    uint32 upper;
    uint32 lower;
    if (group->Current.size() == 2)
    {
        lower = group->Rating > m_teamDiff * 0.66 ? group->Rating - m_teamDiff * 0.66 : 0;
        upper = group->Rating + m_teamDiff * 0.66;
    }
    else
    {
        lower = group->Rating > m_teamDiff ? group->Rating - m_teamDiff : 0;
        upper = group->Rating + m_teamDiff;
    }

    int32 min = desired;
    size_t size = group->Current.size();
    uint32 x = (desired * (size + 1)) - (group->Rating * size);

    if (x > upper || x < lower)
        return false;

    SoloGroup* teammate = nullptr;

    for (auto itr = m_selection.begin(); itr != m_selection.end(); ++itr)
    {
        SoloGroup* right = *itr;
        if (right->InTeam)
            continue;

        if (!IsCompatible(group, right, strong))
            continue;

        int32 rating = 0;
        for (auto&& it : group->Current)
            rating += it.Info->ArenaMatchmakerRating;
        for (auto&& it : right->Members)
            rating += it.Info->ArenaMatchmakerRating;

        rating /= (size + right->Members.size());
        int32 diff = std::abs(rating - int32(desired));
        if (diff > min)
            break;

        min = diff;
        teammate = right;
    }

    if (!teammate)
        return false;

    group->Rating = (group->Rating * size + teammate->Rating) / (size + 1);

    for (auto&& it : teammate->Members)
        group->AddPlayer(it);

    group->InTeam = true;
    teammate->InTeam = true;
    return true;
}

struct TimeCounter
{
    TimeCounter(char const* message)
        : Message(message), StartTime(getMSTime()) { }

    ~TimeCounter()
    {
        uint32 diff = getMSTimeDiff(StartTime, getMSTime());
        if (diff > sWorld->getIntConfig(CONFIG_MIN_LOG_UPDATE))
            TC_LOG_INFO("diff", "Difftime %s: %u.", Message, diff);
    }

    char const* Message;
    uint32 StartTime;
};

void SoloQueue::DoMatchmaking(GroupQueueInfo* ginfo, uint32 extraDiff, bool strong)
{
    if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_BALANCE_PRIORITY_ENABLED))
        strong = false;

    BattlegroundBracketId bracketId = GetSoloQueueBracketId();
    auto& alliance = m_QueuedGroups[bracketId][BG_QUEUE_PREMADE_ALLIANCE];
    auto& horde = m_QueuedGroups[bracketId][BG_QUEUE_PREMADE_HORDE];
    if (alliance.empty() && horde.empty())
        return;

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketById(bg->GetMapId(), bracketId);
    if (!bracketEntry)
    {
        TC_LOG_ERROR("server", "SoloQueue::DoMatchmaking: bg bracket entry not found for map %u bracket id %u", bg->GetMapId(), bracketId);
        return;
    }

    TimeCounter t{ "SoloQueue::DoMatchmaking" };

    uint32 arenaRating = ginfo->ArenaMatchmakerRating;
    uint32 baseRange = sWorld->getIntConfig(CONFIG_SOLO_QUEUE_TEAMMATE_BASE_RATING_DIFFERENCE);
    uint32 range = std::min(baseRange + extraDiff, sWorld->getIntConfig(CONFIG_SOLO_QUEUE_PROGRESSIVE_MMR_THRESHOLD));
    m_teamDiff = range;
    uint32 lower = arenaRating > range ? arenaRating - range : 0;
    uint32 upper = arenaRating + range;

    m_all.clear();
    m_selection.clear();

    SoloGroup* aTeam = nullptr;
    SoloGroup* hTeam = nullptr;

    for (auto&& team : { alliance, horde })
    {
        for (auto&& it : team)
        {
            if (it->ArenaMatchmakerRating >= lower && it->ArenaMatchmakerRating <= upper && !it->IsInvitedToBGInstanceGUID)
            {
                m_all.emplace_back();
                SoloGroup& g = m_all.back();
                if (!g.Create(it))
                    m_all.erase(m_all.end() - 1);
            }
        }
    }

    if (m_all.empty())
        return;

    SoloGroup* opponent = &m_all.front();
    int32 min = std::abs(int32(ginfo->ArenaMatchmakerRating) - int32(opponent->Rating));
    for (auto&& it : m_all)
    {
        for (auto&& p : it.Members)
        {
            p.OriginalGroup = &it;      // Now the pointer won't be changed, we can use it.
            it.AddPlayer(p);
        }
        m_selection.push_back(&it);
        int32 diff = std::abs(int32(ginfo->ArenaMatchmakerRating) - int32(it.Rating));
        if (diff < min && it.Members.front().Info != ginfo)
        {
            min = diff;
            opponent = &it;
        }
    }

    std::sort(m_selection.begin(), m_selection.end(),
        [](SoloGroup const* lhs, SoloGroup const* rhs)
    {
        return lhs->Rating < rhs->Rating;
    });

    uint32 now = getMSTime();

    if (m_selection.size() < sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_QUEUE_SIZE))
        return;

    auto groupit = std::find_if(m_selection.begin(), m_selection.end(), [=](SoloGroup const* g) { return g->Members.front().Info == ginfo; });
    ASSERT(groupit != m_selection.end());
    SoloGroup* group = *groupit;

    auto opponentit = std::find(m_selection.begin(), m_selection.end(), opponent);
    ASSERT(opponentit != m_selection.end());
    size_t index = std::distance(m_selection.begin(), opponentit);

    m_opponents.clear();
    m_opponents.push_back(index);

    size_t i = 0;
    bool plus = true;
    bool minus = true;

    while (plus || minus)
    {
        ++i;
        plus = index + i < m_selection.size();
        minus = i <= index;
        if (plus)
            m_opponents.push_back(index + i);
        if (minus)
            m_opponents.push_back(index - i);
    }

    for (auto&& opponentIndex : m_opponents)
    {
        ASSERT(opponentIndex < m_opponents.size());
        opponent = m_selection[opponentIndex];
        if (opponent == group)
            continue;

        group->InTeam = true;
        opponent->InTeam = true;

        if (MakeTeams(group, opponent, strong))
        {
            uint32 diff = std::abs(int32(group->Rating) - int32(opponent->Rating));
            if (diff < sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MATCH_MAKER_RATING_DIFFERENCE))
            {
                aTeam = group;
                hTeam = opponent;
                break;
            }
        }

        group->Reset();
        opponent->Reset();
    }

    if (!aTeam || !hTeam)
    {
        if (!strong)
            return;
        // TaskMgr::Default()->ScheduleInvocation([=]
        // {
        //     auto& a = m_QueuedGroups[bracketId][BG_QUEUE_PREMADE_ALLIANCE];
        //     auto& h = m_QueuedGroups[bracketId][BG_QUEUE_PREMADE_HORDE];
        //     auto it = std::find(a.begin(), a.end(), ginfo);
        //     if (it == a.end())
        //     {
        //         it = std::find(h.begin(), h.end(), ginfo);
        //         if (it == h.end())
        //             return;
        //     }
        //     DoMatchmaking(*it, extraDiff, false);
        // });
        return;
    }

    m_buff << "Groups are picked: ";
    for (auto&& it : aTeam->Current)
        m_buff << it.Target->GetGUID().GetCounter() << ' ';
    m_buff << "vs ";
    for (auto&& it : hTeam->Current)
        m_buff << it.Target->GetGUID().GetCounter() << ' ';

    m_buff.str(std::string());

    // Invite to arena

    uint32 mmr = 0;
    uint32 rating = 0;
    for (auto&& it : aTeam->Current)
    {
        mmr += it.Info->ArenaMatchmakerRating;
        rating += it.Info->ArenaTeamRating;
    }

    mmr /= aTeam->Current.size();
    rating /= aTeam->Current.size();
    for (auto&& it : aTeam->Current)
    {
        it.Info->ArenaMatchmakerRating = mmr;
        rating += it.Info->ArenaTeamRating;
    }

    mmr = 0;
    rating = 0;
    for (auto&& it : hTeam->Current)
    {
        mmr += it.Info->ArenaMatchmakerRating;
        rating += it.Info->ArenaTeamRating;
    }

    mmr /= hTeam->Current.size();
    rating /= hTeam->Current.size();
    for (auto&& it : hTeam->Current)
    {
        it.Info->ArenaMatchmakerRating = mmr;
        it.Info->ArenaTeamRating = rating;
        it.Info->OpponentsMatchmakerRating = aTeam->Current.front().Info->ArenaMatchmakerRating;
        it.Info->OpponentsTeamRating = aTeam->Current.front().Info->ArenaMatchmakerRating;
    }

    for (auto&& it : aTeam->Current)
    {
        it.Info->OpponentsMatchmakerRating = mmr;
        it.Info->OpponentsTeamRating = rating;
    }

    for (auto&& it : aTeam->Current)
    {
        if (it.Info->Team != ALLIANCE)
        {
            if (alliance.empty() || it.Info != alliance.back())
            {
                horde.remove(it.Info);
                alliance.push_back(it.Info);
            }
        }
    }
    for (auto&& it : hTeam->Current)
    {
        if (it.Info->Team != HORDE)
        {
            if (horde.empty() || it.Info != horde.back())
            {
                alliance.remove(it.Info);
                horde.push_back(it.Info);
            }
        }
    }

    Battleground* arena = sBattlegroundMgr->CreateNewBattleground(BATTLEGROUND_AA, bracketEntry, ARENA_TYPE_3v3, true);
    if (!arena)
    {
        TC_LOG_ERROR("arena", "BattlegroundQueue::Update couldn't create arena instance for rated arena match!");
        return;
    }

    arena->EnableSoloMode();

    arena->SetArenaTeam(ALLIANCE, std::make_shared<ArenaTeam>(aTeam));
    arena->SetArenaTeam(HORDE, std::make_shared<ArenaTeam>(hTeam));

    for (auto&& team : { aTeam, hTeam })
        for (auto&& it : team->Current)
            InviteGroupToBG(it.Info, arena, team == aTeam ? ALLIANCE : HORDE);

    arena->StartBattleground();
}

void SoloQueue::DynamicUpdate()
{
    uint32 threshold = sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_TIMER) / (15 * IN_MILLISECONDS);
    if (m_dynamicUpdateCount == threshold)
    {
        m_dynamicUpdateCount = 0;
        BattlegroundBracketId bracket_id = GetSoloQueueBracketId();
        auto* alliance = &m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE];
        auto* horde = &m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE];
        if (alliance->size() + horde->size() < sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_QUEUE_SIZE))
            return;

        uint32 now = getMSTime();
        for (auto&& team : { alliance, horde })
        {
            size_t i = 0;
            for (auto it = team->rbegin(); it != team->rend(); ++it)
            {
                bool heal = false;
                for (auto&& it2 : (*it)->Players)
                {
                    if (Player* player = ObjectAccessor::FindConnectedPlayer(it2.first))
                    {
                        if (GetRole(player->GetSpecialization()) == SoloQueueRole::Healer)
                        {
                            heal = true;
                            break;
                        }
                    }
                }

                uint32 timeinq = now - (*it)->JoinTime;
                if (timeinq < 30 * IN_MILLISECONDS)
                    continue;

                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_PROGRESSIVE_MMR_ONLY_HEALERS))
                {
                    if (!heal)
                        continue;
                }

                ++i;
                // TaskMgr::Default()->ScheduleInvocation([=]
                // {
                //     if (team->size() <= i)
                //         return;

                //     auto itr = team->begin();
                //     std::advance(itr, i);

                //     if ((*itr)->IsInvitedToBGInstanceGUID)
                //         return;

                //     uint32 extradiff = (timeinq / sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_TIMER)) * sWorld->getIntConfig(CONFIG_ARENA_PROGRESSIVE_MMR_STEPSIZE);
                //     if (!heal)
                //         extradiff /= 2;
                //     DoMatchmaking(*itr, extradiff, false);
                // });
            }
        }
    }
    else
        ++m_dynamicUpdateCount;
}

void SoloQueue::GetQueuedPlayersCount(uint32& dd, uint32& healers)
{
    dd = m_damagersCount;
    healers = m_healersCount;
}

void SoloQueue::SetQueuedPlayersCount(uint32 dd, uint32 healers)
{
    m_damagersCount = dd;
    m_healersCount = healers;
}

void SoloQueue::SetBlacklist(std::vector<TeamSetup> const& blist)
{
    m_disabled.clear();
    m_disabled = blist;
}

uint32 SoloQueue::TeamSize()
{
    return sBattlegroundMgr->IsArenaTesting() ? 2 : 3;   // For debug.
}

SoloQueueRole SoloQueue::GetRole(Specializations spec)
{
    switch (spec)
    {
        case SPEC_PALADIN_RETRIBUTION:
        case SPEC_WARRIOR_ARMS:
        case SPEC_WARRIOR_FURY:
        case SPEC_DRUID_FERAL:
        case SPEC_DEATH_KNIGHT_FROST:
        case SPEC_DEATH_KNIGHT_UNHOLY:
        case SPEC_ROGUE_ASSASSINATION:
        case SPEC_ROGUE_COMBAT:
        case SPEC_ROGUE_SUBTLETY:
        case SPEC_SHAMAN_ENHANCEMENT:
        case SPEC_MONK_WINDWALKER:
            return SoloQueueRole::Melee;
        case SPEC_MAGE_ARCANE:
        case SPEC_MAGE_FIRE:
        case SPEC_MAGE_FROST:
        case SPEC_DRUID_BALANCE:
        case SPEC_HUNTER_BEAST_MASTERY:
        case SPEC_HUNTER_MARKSMANSHIP:
        case SPEC_HUNTER_SURVIVAL:
        case SPEC_PRIEST_SHADOW:
        case SPEC_SHAMAN_ELEMENTAL:
        case SPEC_WARLOCK_AFFLICTION:
        case SPEC_WARLOCK_DEMONOLOGY:
        case SPEC_WARLOCK_DESTRUCTION:
            return SoloQueueRole::Ranged;
        case SPEC_PALADIN_PROTECTION:
        case SPEC_WARRIOR_PROTECTION:
        case SPEC_DRUID_GUARDIAN:
        case SPEC_DEATH_KNIGHT_BLOOD:
            return SoloQueueRole::Tank;
        case SPEC_PALADIN_HOLY:
        case SPEC_DRUID_RESTORATION:
        case SPEC_PRIEST_DISCIPLINE:
        case SPEC_PRIEST_HOLY:
        case SPEC_SHAMAN_RESTORATION:
        case SPEC_MONK_BREWMASTER:
        case SPEC_MONK_MISTWEAVER:
            return SoloQueueRole::Healer;
        default:
            break;
    }

    return SoloQueueRole::None;
}

BattlegroundBracketId SoloQueue::GetSoloQueueBracketId()
{
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bg)
        return BG_BRACKET_ID_FIRST;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), DEFAULT_MAX_LEVEL);
    if (!bracketEntry)
        return BG_BRACKET_ID_FIRST;

    return BattlegroundBracketId(bracketEntry->bracketId);
}

/*********************************************************/
/***            BATTLEGROUND QUEUE EVENTS              ***/
/*********************************************************/

bool BGQueueInviteEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* player = ObjectAccessor::FindPlayer(m_PlayerGuid);
    // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
    if (!player)
        return true;

    Battleground* bg = sBattlegroundMgr->GetBattleground(m_BgInstanceGUID, m_BgTypeId);
    //if battleground ended and its instance deleted - do nothing
    if (!bg)
        return true;

    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
    uint32 queueSlot = player->GetBattlegroundQueueIndex(bgQueueTypeId);
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES)         // player is in queue or in battleground
    {
        // check if player is invited to this bg
        BattlegroundQueue &bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        if (bgQueue.IsPlayerInvited(m_PlayerGuid, m_BgInstanceGUID, m_RemoveTime))
        {
            WorldPacket data;
            //we must send remaining time in queue
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME - INVITATION_REMIND_TIME, player->GetBattlegroundQueueJoinTime(m_BgTypeId), m_ArenaType);
            player->GetSession()->SendPacket(&data);
        }
    }
    return true;                                            //event will be deleted
}

void BGQueueInviteEvent::Abort(uint64 /*e_time*/)
{
    //do nothing
}

/*
    this event has many possibilities when it is executed:
    1. player is in battleground (he clicked enter on invitation window)
    2. player left battleground queue and he isn't there any more
    3. player left battleground queue and he joined it again and IsInvitedToBGInstanceGUID = 0
    4. player left queue and he joined again and he has been invited to same battleground again -> we should not remove him from queue yet
    5. player is invited to bg and he didn't choose what to do and timer expired - only in this condition we should call queue::RemovePlayer
    we must remove player in the 5. case even if battleground object doesn't exist!
*/
bool BGQueueRemoveEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* player = ObjectAccessor::FindPlayer(m_PlayerGuid);
    if (!player)
        // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
        return true;

    Battleground* bg = sBattlegroundMgr->GetBattleground(m_BgInstanceGUID, m_BgTypeId);
    //battleground can be deleted already when we are removing queue info
    //bg pointer can be NULL! so use it carefully!

    uint32 queueSlot = player->GetBattlegroundQueueIndex(m_BgQueueTypeId);
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES)         // player is in queue, or in Battleground
    {
        // check if player is in queue for this BG and if we are removing his invite event
        BattlegroundQueue &bgQueue = sBattlegroundMgr->GetBattlegroundQueue(m_BgQueueTypeId);
        if (bgQueue.IsPlayerInvited(m_PlayerGuid, m_BgInstanceGUID, m_RemoveTime))
        {
            TC_LOG_DEBUG("bg.battleground", "Battleground: removing player %u from bg queue for instance %u because of not pressing enter battle in time.", player->GetGUID().GetCounter(), m_BgInstanceGUID);

            player->RemoveBattlegroundQueueId(m_BgQueueTypeId);
            bgQueue.RemovePlayer(m_PlayerGuid, true);
            //update queues if battleground isn't ended
            if (bg && bg->IsBattleground() && bg->GetStatus() != STATUS_WAIT_LEAVE)
                sBattlegroundMgr->ScheduleQueueUpdate(0, 0, m_BgQueueTypeId, m_BgTypeId, bg->GetBracketId());

            WorldPacket data;
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, queueSlot, STATUS_NONE, player->GetBattlegroundQueueJoinTime(m_BgTypeId), 0, 0);
            player->GetSession()->SendPacket(&data);
        }
    }

    //event will be deleted
    return true;
}

void BGQueueRemoveEvent::Abort(uint64 /*e_time*/)
{
    //do nothing
}
