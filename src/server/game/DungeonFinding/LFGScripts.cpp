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

/*
 * Interaction between core and LFGScripts
 */

#include "Common.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Group.h"
#include "LFGScripts.h"
#include "LFGMgr.h"
#include "ScriptMgr.h"
#include "ObjectAccessor.h"
#include "WorldSession.h"

namespace lfg
{

LFGPlayerScript::LFGPlayerScript() : PlayerScript("LFGPlayerScript") { }

void LFGPlayerScript::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    sLFGMgr->InitializeLockedDungeons(player);
    if (player->GetSession())
        player->GetSession()->SendLfgPlayerLockInfo();
}

void LFGPlayerScript::OnEquipChanged(Player* player, uint32 /*item*/)
{
    sLFGMgr->InitializeLockedDungeons(player);
    if (player->GetSession())
        player->GetSession()->SendLfgPlayerLockInfo();
}

void LFGPlayerScript::OnQuestRewarded(Player* player, const Quest* /*quest*/)
{
    sLFGMgr->InitializeLockedDungeons(player);
    if (player->GetSession())
        player->GetSession()->SendLfgPlayerLockInfo();
}

void LFGPlayerScript::OnLogout(Player* player)
{

    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (!player->GetGroup())
    {
        sLFGMgr->RemovePlayerQueues(player->GetGUID());
    }

    Group* group = player->GetGroup();
    if (group && !group->isBGGroup())
        sLFGMgr->RemoveGroupQueues(group->GetGUID());
}

void LFGPlayerScript::OnLogin(Player* player)
{

    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    // Temporal: Trying to determine when group data and LFG data gets desynched
    ObjectGuid guid = player->GetGUID();

    sLFGMgr->InitializeLockedDungeons(player);
    sLFGMgr->SetTeam(player->GetGUID(), player->GetTeam());
    /// @todo - Restore LfgPlayerData and send proper status to player if it was in a group
}

void LFGPlayerScript::OnBindToInstance(Player* player, Difficulty difficulty, uint32 mapId, bool /*permanent*/)
{
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
    if (mapEntry->IsDungeon() && difficulty > DUNGEON_DIFFICULTY_NORMAL)
        sLFGMgr->InitializeLockedDungeons(player);
}

void LFGPlayerScript::OnMapChanged(Player* player)
{
    Map const* map = player->GetMap();

    if (sLFGMgr->InLfgDungeonMap(player->GetGUID(), map->GetId(), map->GetDifficulty()))
    {
        Group* group = player->GetGroup();
        // This function is also called when players log in
        // if for some reason the LFG system recognises the player as being in a LFG dungeon,
        // but the player was loaded without a valid group, we'll teleport to homebind to prevent
        // crashes or other undefined behaviour
        if (!group)
        {
            //sLFGMgr->LeaveLfg(player->GetGUID());
            player->RemoveAurasDueToSpell(LFG_SPELL_LUCK_OF_THE_DRAW);
            player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, 0.0f);
            TC_LOG_ERROR("lfg", "LFGPlayerScript::OnMapChanged, Player {} ({}) is in LFG dungeon map but does not have a valid group! "
                "Teleporting to homebind.", player->GetName().c_str(), player->GetGUID().GetCounter());
            return;
        }

        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            if (Player* member = itr->GetSource())
                player->GetSession()->SendNameQueryOpcode(member->GetGUID());

        if (sLFGMgr->IsSelectedRandomLfgDungeon(player->GetGUID()))
        {
            player->RemoveAurasDueToSpell(LFG_SPELL_LUCK_OF_THE_DRAW);
            uint8 stacks = sLFGMgr->GetSoloJoinedPlayersCount(group->GetGUID());
            for (uint8 i = 0; i < stacks; ++i)
                player->CastSpell(player, LFG_SPELL_LUCK_OF_THE_DRAW, true);
        }
    }
    else
        player->RemoveAurasDueToSpell(LFG_SPELL_LUCK_OF_THE_DRAW);
}

LFGGroupScript::LFGGroupScript() : GroupScript("LFGGroupScript") { }

void LFGGroupScript::OnAddMember(Group* group, ObjectGuid guid)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (group->isBGGroup())
        return;

    ObjectGuid gguid = group->GetGUID();
    ObjectGuid leader = group->GetLeaderGUID();

    LfgState gstate = sLFGMgr->GetActiveState(gguid);
    LfgState state  = sLFGMgr->GetActiveState(guid);

    auto gstateStr = GetStateString(gstate);
    auto stateStr = GetStateString(state);

    TC_LOG_DEBUG("lfg", "LFGScripts::OnAddMember [" "{}" "]: added [{}] leader [{}] gstate: {}, state: {}", gguid.GetRawValue(), guid.GetCounter(), leader.GetCounter(), gstateStr.c_str(), stateStr.c_str());

    // When is a member added?
    // 1) Normal group. We must remove all player standalon queues and all group's queues
    // 2) Lfg group made. Do nothing. Group doesn't have queues yet and player must keep all standalone/original group's queues
    if (!group->isLFGGroup())
    {
        sLFGMgr->RemovePlayerQueues(guid);
        sLFGMgr->RemoveGroupQueues(gguid);
    }

    // Fucking hack.
    // While adding leader group doesn't have queue yet. And this whole code needed only for newcomers and not for initial group memebers.
    if (group->IsFlex() && guid != leader)
    {
        uint32 groupQeueuId = sLFGMgr->GetActiveQueueId(gguid);
        ASSERT(groupQeueuId);
        if (!sLFGMgr->HasQueueId(guid, groupQeueuId))
        {
            // FIXME ^ 2
            sLFGMgr->RemovePlayerQueuesOnPartyFound(guid, 0);
            sLFGMgr->AddQueue(guid, groupQeueuId);
            sLFGMgr->SetState(guid, groupQeueuId, LFG_STATE_DUNGEON);
            sLFGMgr->AddPlayerToGroup(gguid, groupQeueuId, guid);
        }
    }
}

void LFGGroupScript::OnRemoveMember(Group* group, ObjectGuid guid, RemoveMethod method, ObjectGuid kicker, char const* reason)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (group->isBGGroup())
        return;

    ObjectGuid gguid = group->GetGUID();
    LfgState state = sLFGMgr->GetActiveState(gguid);
    uint32 groupQueueId = sLFGMgr->GetActiveQueueId(gguid);

    auto stateStr = GetStateString(state);

    TC_LOG_DEBUG("lfg", "LFGScripts::OnRemoveMember [" "{}" "]: remove [{}] Method: {} Kicker: [{}] Reason: {}, group state: {}", gguid.GetRawValue(), guid.GetCounter(), method, kicker.GetCounter(), (reason ? reason : ""), stateStr.c_str());

    if (state == LFG_STATE_NONE)
        return;

    bool isLFG = group->isLFGGroup();

    if (isLFG && method == GROUP_REMOVEMETHOD_KICK)        // Player have been kicked
    {
        /// @todo - Update internal kick cooldown of kicker
        std::string str_reason = "";
        if (reason)
            str_reason = std::string(reason);
        sLFGMgr->InitBoot(gguid, kicker, guid, str_reason);
        return;
    }

    // If group is being formed after proposal success do nothing more
    if (state == LFG_STATE_PROPOSAL && method == GROUP_REMOVEMETHOD_DEFAULT)
    {
        // LfgData: Remove player from group
        sLFGMgr->RemovePlayerFromGroup(gguid, groupQueueId, guid);
        return;
    }

    if (auto* queues = sLFGMgr->GetPlayerQueues(guid))
    {
        for (auto&& itr : PlayerQueueDataMap{ *queues })
        {
            if (!sLFGMgr->HasQueueId(gguid, itr.first))
                continue;

            uint32 queueId = itr.first;
            uint8 players = sLFGMgr->GetPlayerCount(gguid) - 1;

            if (Player* player = ObjectAccessor::FindPlayer(guid))
            {
                if (method == GROUP_REMOVEMETHOD_LEAVE && state == LFG_STATE_DUNGEON &&
                    players >= sLFGMgr->GetBootVotesNeeded(gguid) && sWorld->getBoolConfig(CONFIG_LFG_CASTDESERTER) && !group->IsFlex())
                    player->CastSpell(player, LFG_SPELL_DUNGEON_DESERTER, true);

                if (method == GROUP_REMOVEMETHOD_KICK_LFG || method == GROUP_REMOVEMETHOD_LEAVE && state == LFG_STATE_DUNGEON && players < sLFGMgr->GetBootVotesNeeded(gguid))
                    player->RemoveAurasDueToSpell(LFG_SPELL_DUNGEON_COOLDOWN);

                player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);
                player->GetSession()->SendLfgUpdateStatus(LFG_UPDATETYPE_LEADER_UNK1, sLFGMgr->GetPlayerQueueData(player->GetGUID(), queueId));

                if (isLFG && player->GetMap()->IsDungeon())            // Teleport player out the dungeon
                    sLFGMgr->TeleportPlayer(player, true);
            }

            // Remove from LFG only after sending update
            sLFGMgr->LeaveLfg(guid, queueId);
            // While "rolecheck" or "queued" states player can be already removed in LFGMgr::LeaveLfg. Fuck it
            if (sLFGMgr->HasQueueId(guid, queueId))
            {
                sLFGMgr->SetState(guid, queueId, LFG_STATE_NONE);
                sLFGMgr->RemoveQueue(guid, queueId);
            }
        }
    }

    sLFGMgr->RemovePlayerFromGroup(gguid, guid);

    if (group->GetMembersCount() == 0)
        return;

    // Need to assign a new leader here, otherwise continue offer will be sent to the player who just left the group
    if (group->IsLeader(guid))
        group->FindNewLeader(guid);

    if (isLFG && state != LFG_STATE_FINISHED_DUNGEON) // Need more players to finish the dungeon
        if (Player* leader = ObjectAccessor::FindPlayer(sLFGMgr->GetLeader(gguid)))
            leader->GetSession()->SendLfgOfferContinue(sLFGMgr->GetDungeon(gguid, false));
}

void LFGGroupScript::OnDisband(Group* group)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetGUID();

    if (sLFGMgr->GetActiveState(gguid) == LFG_STATE_NONE)
        return;

    TC_LOG_DEBUG("lfg", "LFGScripts::OnDisband [" "{}" "]", gguid.GetRawValue());

    sLFGMgr->RemoveGroupData(gguid);
}

void LFGGroupScript::OnChangeLeader(Group* group, ObjectGuid newLeaderGuid, ObjectGuid oldLeaderGuid)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (group->isBGGroup())
        return;

    ObjectGuid gguid = group->GetGUID();

    // FIXME
    if (sLFGMgr->GetActiveState(gguid) == LFG_STATE_NONE)
        return;

    TC_LOG_DEBUG("lfg", "LFGScripts::OnChangeLeader [" "{}" "]: old [{}] new [{}]", gguid.GetRawValue(), oldLeaderGuid.GetCounter(), newLeaderGuid.GetCounter());
    sLFGMgr->SetLeader(gguid, newLeaderGuid);
}

void LFGGroupScript::OnInviteMember(Group* group, ObjectGuid guid)
{
    if (!sLFGMgr->isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (group->isBGGroup())
        return;

    ObjectGuid gguid = group->GetGUID();

    ObjectGuid leader = group->GetLeaderGUID();
    TC_LOG_DEBUG("lfg", "LFGScripts::OnInviteMember [" "{}" "]: invite [{}] leader [{}]", gguid.GetRawValue(), guid.GetCounter(), leader.GetCounter());
    // No gguid ==  new group being formed
    // No leader == after group creation first invite is new leader
    // leader and no gguid == first invite after leader is added to new group (this is the real invite)
    if (leader && gguid.IsEmpty())
        sLFGMgr->RemovePlayerQueues(leader);
}

} // namespace lfg
