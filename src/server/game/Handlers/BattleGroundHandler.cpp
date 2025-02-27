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
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "Chat.h"
#include "Language.h"
#include "Log.h"
#include "Player.h"
#include "Object.h"
#include "Opcodes.h"
#include "DisableMgr.h"
#include "Group.h"
#include "LFGMgr.h"

void WorldSession::HandleBattlemasterHelloOpcode(WorldPacket& recvData)
{
    uint64 i;
    recvData >> i;
    ObjectGuid guid(i);
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEMASTER_HELLO Message from (GUID: %u TypeId:%u)", guid.GetCounter(), guid.GetTypeId());

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->IsBattleMaster())                             // it's not battlemaster
        return;

    // Stop the npc if moving
    unit->StopMoving();

    BattlegroundTypeId bgTypeId = sBattlegroundMgr->GetBattleMasterBG(unit->GetEntry());

    if (!_player->GetBGAccessByLevel(bgTypeId))
    {
                                                            // temp, must be gossip message...
        SendNotification(LANG_YOUR_BG_LEVEL_REQ_ERROR);
        return;
    }

    SendBattleGroundList(guid, bgTypeId);
}

void WorldSession::SendBattleGroundList(ObjectGuid guid, BattlegroundTypeId bgTypeId)
{
    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundListPacket(&data, guid, _player, bgTypeId);
    SendPacket(&data);
}

void WorldSession::HandleBattlemasterJoinOpcode(WorldPacket& recvData)
{
    uint32 bgTypeId_;
    uint8 asGroup;
    bool isPremade = false;
    Group* grp = NULL;
    ObjectGuid guid;
    bool hasRoleMask;
    uint8 roleMask = 0;

    for (int i = 0; i < 2; i++) // blacklistedMapIds
        recvData.read_skip<uint32>();

    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    asGroup = recvData.ReadBit();           // As Group
    guid[4] = recvData.ReadBit();
    hasRoleMask = !recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[1]);

    if (hasRoleMask)
        recvData >> roleMask; // Need to set this as group role later

    //extract from guid
    bgTypeId_ = guid.GetCounter();

    if (!sBattlemasterListStore.LookupEntry(bgTypeId_))
    {
        TC_LOG_ERROR("network", "Battleground: invalid bgtype (%u) received. possible cheater? player guid %u", bgTypeId_, _player->GetGUID().GetCounter());
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeId_, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_BG_DISABLED);
        return;
    }
    BattlegroundTypeId bgTypeId = BattlegroundTypeId(bgTypeId_);

    //TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEMASTER_JOIN Message from (GUID:"UI64FMTD" TypeId:%u)", guid, bgTypeId_);

    // can do this, since it's battleground, not arena
    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, 0);
    BattlegroundQueueTypeId bgQueueTypeIdRandom = BattlegroundMgr::BGQueueTypeId(BATTLEGROUND_RB, 0);

    // ignore if player is already in BG
    if (_player->InBattleground())
        return;

    // get bg instance or bg template if instance not found
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return;

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err = ERR_BATTLEGROUND_NONE;

    // check queue conditions
    if (!asGroup)
    {
        if (GetPlayer()->IsUsingLfg(true))
        {
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, ERR_LFG_CANT_USE_BATTLEGROUND);
            GetPlayer()->GetSession()->SendPacket(&data);
            return;
        }

        // check Deserter debuff
        if (!_player->CanJoinToBattleground(bg))
        {
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        if (_player->GetBattlegroundQueueIndex(bgQueueTypeIdRandom) < PLAYER_MAX_BATTLEGROUND_QUEUES)
        {
            // player is already in random queue
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, ERR_IN_RANDOM_BG);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        if (_player->InBattlegroundQueue() && bgTypeId == BATTLEGROUND_RB)
        {
            // player is already in queue, can't start random queue
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, ERR_IN_NON_RANDOM_BG);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        // check if already in queue
        if (_player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            // player is already in this queue
            return;

        // check if has free queue slots
        if (!_player->HasFreeBattlegroundQueueId())
        {
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        GroupQueueInfo* ginfo = bgQueue.AddGroup(_player, NULL, bgTypeId, bracketEntry, 0, false, isPremade, 0, 0);

        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        uint32 queueSlot = _player->AddBattlegroundQueueId(bgQueueTypeId);

        // add joined time data
        _player->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

        WorldPacket data; // send status packet (in queue)
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, _player, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, ginfo->ArenaType);
        SendPacket(&data);

        TC_LOG_DEBUG("bg.battleground", "Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",
                       bgQueueTypeId, bgTypeId, _player->GetGUID().GetCounter(), _player->GetName().c_str());

    }
    else
    {
        grp = _player->GetGroup();

        if (!grp)
            return;

        if (grp->GetLeaderGUID() != _player->GetGUID())
            return;

        err = grp->CanJoinBattlegroundQueue(bg, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), false, 0);
        isPremade = (grp->GetMembersCount() >= 4);

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        GroupQueueInfo* ginfo = NULL;
        uint32 avgTime = 0;

        if (!err)
        {
            TC_LOG_DEBUG("bg.battleground", "Battleground: the following players are joining as group:");
            ginfo = bgQueue.AddGroup(_player, grp, bgTypeId, bracketEntry, 0, false, isPremade, 0, 0);
            avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member)
                continue;   // this should never happen

            if (err)
            {
                WorldPacket data;
                sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, err);
                member->GetSession()->SendPacket(&data);
                continue;
            }

            // add to queue
            uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);

            // add joined time data
            member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

            WorldPacket data; // send status packet (in queue)
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, ginfo->ArenaType);
            member->GetSession()->SendPacket(&data);
            TC_LOG_DEBUG("bg.battleground", "Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",
                bgQueueTypeId, bgTypeId, member->GetGUID().GetCounter(), member->GetName().c_str());

        }
        TC_LOG_DEBUG("bg.battleground", "Battleground: group end");
    }

    sBattlegroundMgr->ScheduleQueueUpdate(0, 0, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

void WorldSession::HandlePVPLogDataOpcode(WorldPacket & /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_PVP_LOG_DATA Message");

    Battleground* bg = _player->GetBattleground();
    if (!bg)
        return;

    // Prevent players from sending BuildPvpLogDataPacket in an arena except for when sent in BattleGround::EndBattleGround.
    if (bg->IsArena())
        return;

    WorldPacket data;
    sBattlegroundMgr->BuildPvpLogDataPacket(&data, bg);
    SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_PVP_LOG_DATA Message");
}

void WorldSession::HandleBattlefieldListOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEFIELD_LIST Message");

    uint32 bgTypeId;
    recvData >> bgTypeId;                                  // id from DBC

    BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);
    if (!bl)
    {
        TC_LOG_DEBUG("bg.battleground", "BattlegroundHandler: invalid bgtype (%u) with player (Name: %s, GUID: %u) received.", bgTypeId, _player->GetName().c_str(), _player->GetGUID().GetCounter());
        return;
    }

    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundListPacket(&data, ObjectGuid::Empty, _player, BattlegroundTypeId(bgTypeId));
    SendPacket(&data);
}

void WorldSession::HandleBattleFieldPortOpcode(WorldPacket &recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEFIELD_PORT Message");

    uint32 time;
    uint32 queueSlot;
    uint32 id;
    uint8 action;                       // enter battle 0x1, leave queue 0x0
    ObjectGuid guid;

    recvData >> action >> queueSlot >> id >> time;

    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[1]);

    if (!_player->InBattlegroundQueue())
    {
        TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u. Player not in queue!",
            GetPlayerInfo().c_str(), queueSlot, id, time, action);
        return;
    }

    BattlegroundQueueTypeId bgQueueTypeId = _player->GetBattlegroundQueueTypeId(queueSlot);
    if (bgQueueTypeId == BATTLEGROUND_QUEUE_NONE)
    {
        TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u. Invalid queueSlot!",
            GetPlayerInfo().c_str(), queueSlot, id, time, action);
        return;
    }

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);

    //we must use temporary variable, because GroupQueueInfo pointer can be deleted in BattlegroundQueue::RemovePlayer() function
    GroupQueueInfo ginfo;
    if (!bgQueue.GetPlayerGroupInfoData(_player->GetGUID(), &ginfo))
    {
        TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u. Player not in queue (No player Group Info)!",
            GetPlayerInfo().c_str(), queueSlot, id, time, action);
        return;
    }
    // if action == 1, then instanceId is required
    if (!ginfo.IsInvitedToBGInstanceGUID && action)
    {
        TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u. Player is not invited to any bg!",
            GetPlayerInfo().c_str(), queueSlot, id, time, action);
        return;
    }

    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(bgQueueTypeId);
    // BGTemplateId returns BATTLEGROUND_AA when it is arena queue.
    // Do instance id search as there is no AA bg instances.
    Battleground* bg = sBattlegroundMgr->GetBattleground(ginfo.IsInvitedToBGInstanceGUID, bgTypeId == BATTLEGROUND_AA ? BATTLEGROUND_TYPE_NONE : bgTypeId);
    if (!bg)
    {
        if (action)
        {
            TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u. Cant find BG with id %u!",
                GetPlayerInfo().c_str(), queueSlot, id, time, action, ginfo.IsInvitedToBGInstanceGUID);
            return;
        }

        bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
        if (!bg)
        {
            TC_LOG_ERROR("network", "BattlegroundHandler: bg_template not found for type id %u.", bgTypeId);
            return;
        }
    }

    TC_LOG_DEBUG("bg.battleground", "CMSG_BATTLEFIELD_PORT %s Slot: %u, Unk: %u, Time: %u, Action: %u.",
        GetPlayerInfo().c_str(), queueSlot, id, time, action);

    // get real bg type
    bgTypeId = bg->GetTypeID();

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    //some checks if player isn't cheating - it is not exactly cheating, but we cannot allow it
    if (action && ginfo.ArenaType == 0)
    {
        //if player is trying to enter battleground (not arena!) and he has deserter debuff, we must just remove him from queue
        if (!_player->CanJoinToBattleground(bg))
        {
            //send bg command result to show nice message
            WorldPacket data2;
            sBattlegroundMgr->BuildStatusFailedPacket(&data2, bg, _player, 0, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(&data2);
            action = 0;
            TC_LOG_DEBUG("bg.battleground", "Player %s (%u) has a deserter debuff, do not port him to battleground!", _player->GetName().c_str(), _player->GetGUID().GetCounter());
        }
        //if player don't match battleground max level, then do not allow him to enter! (this might happen when player leveled up during his waiting in queue
        if (_player->GetLevel() > bg->GetMaxLevel())
        {
            TC_LOG_DEBUG("network", "Player %s (%u) has level (%u) higher than maxlevel (%u) of battleground (%u)! Do not port him to battleground!",
                _player->GetName().c_str(), _player->GetGUID().GetCounter(), _player->GetLevel(), bg->GetMaxLevel(), bg->GetTypeID());
            action = 0;
        }
    }

    WorldPacket data;
    if (action)
    {
        if (!_player->IsInvitedForBattlegroundQueueType(bgQueueTypeId))
            return;                                 // cheating?

        if (!_player->InBattleground())
            _player->SetBattlegroundEntryPoint();

        // resurrect the player
        if (!_player->IsAlive())
        {
            _player->ResurrectPlayer(1.0f);
            _player->SpawnCorpseBones();
        }
        // stop taxi flight at port
        if (_player->IsInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }

        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, _player, queueSlot, STATUS_IN_PROGRESS, _player->GetBattlegroundQueueJoinTime(bgTypeId), bg->GetElapsedTime(), bg->GetArenaType());
        _player->GetSession()->SendPacket(&data);

        // remove battleground queue status from BGmgr
        bgQueue.RemovePlayer(_player->GetGUID(), false);
        // this is still needed here if battleground "jumping" shouldn't add deserter debuff
        // also this is required to prevent stuck at old battleground after SetBattlegroundId set to new
        if (Battleground* currentBg = _player->GetBattleground())
            currentBg->RemovePlayerAtLeave(_player->GetGUID(), false, true);

        // set the destination instance id
        _player->SetBattlegroundId(bg->GetInstanceID(), bgTypeId);
        // set the destination team
        _player->SetBGTeam(ginfo.Team);

        sLFGMgr->RemovePlayerQueuesOnPartyFound(_player->GetGUID());

        // bg->HandleBeforeTeleportToBattleground(_player);
        sBattlegroundMgr->SendToBattleground(_player, ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
        // add only in HandleMoveWorldPortAck()
        // bg->AddPlayer(_player, team);
        TC_LOG_DEBUG("bg.battleground", "Battleground: player %s (%u) joined battle for bg %u, bgtype %u, queue type %u.", _player->GetName().c_str(), _player->GetGUID().GetCounter(), bg->GetInstanceID(), bg->GetTypeID(), bgQueueTypeId);
    }
    else // leave queue
    {
        // if player leaves rated arena match before match start, it is counted as he played but he lost
        if (ginfo.IsRated && ginfo.IsInvitedToBGInstanceGUID)
        {
            TC_LOG_DEBUG("bg.battleground", "UPDATING memberLost's personal arena rating for %u by opponents rating: %u, because he has left queue!", _player->GetGUID().GetCounter(), ginfo.OpponentsTeamRating);
        }
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, _player, queueSlot, STATUS_NONE, _player->GetBattlegroundQueueJoinTime(bgTypeId), 0, 0);
        SendPacket(&data);

        _player->RemoveBattlegroundQueueId(bgQueueTypeId);  // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
        bgQueue.RemovePlayer(_player->GetGUID(), true, true);
        // player left queue, we should update it - do not update Arena Queue
        if (!ginfo.ArenaType)
            sBattlegroundMgr->ScheduleQueueUpdate(ginfo.ArenaMatchmakerRating, ginfo.ArenaType, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());

        TC_LOG_DEBUG("bg.battleground", "Battleground: player %s (%u) left queue for bgtype %u, queue type %u.", _player->GetName().c_str(), _player->GetGUID().GetCounter(), bg->GetTypeID(), bgQueueTypeId);
    }
}

void WorldSession::HandleBattlefieldLeaveOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEFIELD_LEAVE Message");

    // not allow leave battleground in combat
    if (_player->IsInCombat())
        if (Battleground* bg = _player->GetBattleground())
            if (bg->GetStatus() != STATUS_WAIT_LEAVE)
                return;

    _player->LeaveBattleground();
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket & /*recvData*/)
{
    // empty opcode
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_BATTLEFIELD_STATUS Message");

    WorldPacket data;
    // we must update all queues here
    Battleground* bg = NULL;
    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId bgQueueTypeId = _player->GetBattlegroundQueueTypeId(i);
        if (!bgQueueTypeId)
            continue;
        BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(bgQueueTypeId);
        uint8 arenaType = BattlegroundMgr::BGArenaType(bgQueueTypeId);
        if (bgTypeId == _player->GetBattlegroundTypeId())
        {
            bg = _player->GetBattleground();
            //i cannot check any variable from player class because player class doesn't know if player is in 2v2 / 3v3 or 5v5 arena
            //so i must use bg pointer to get that information
            if (bg && bg->GetArenaType() == arenaType)
            {
                // this line is checked, i only don't know if GetElapsedTime() is changing itself after bg end!
                // send status in Battleground
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, GetPlayer(), i, STATUS_IN_PROGRESS, _player->GetBattlegroundQueueJoinTime(bgTypeId), bg->GetElapsedTime(), arenaType);
                SendPacket(&data);
                continue;
            }
        }

        //we are sending update to player about queue - he can be invited there!
        //get GroupQueueInfo for queue status
        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        GroupQueueInfo ginfo;
        if (!bgQueue.GetPlayerGroupInfoData(_player->GetGUID(), &ginfo))
            continue;
        if (ginfo.IsInvitedToBGInstanceGUID)
        {
            bg = sBattlegroundMgr->GetBattleground(ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
            if (!bg)
                continue;

            // send status invited to Battleground
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, GetPlayer(), i, STATUS_WAIT_JOIN, getMSTimeDiff(getMSTime(), ginfo.RemoveInviteTime), _player->GetBattlegroundQueueJoinTime(bgTypeId), arenaType);
            SendPacket(&data);
        }
        else
        {
            bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
            if (!bg)
                continue;

            // expected bracket entry
            PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
            if (!bracketEntry)
                continue;

            uint32 avgTime = bgQueue.GetAverageQueueWaitTime(&ginfo, bracketEntry->GetBracketId());
            // send status in Battleground Queue
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, GetPlayer(), i, STATUS_WAIT_QUEUE, avgTime, _player->GetBattlegroundQueueJoinTime(bgTypeId), arenaType);
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleBattlemasterJoinArena(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_BATTLEMASTER_JOIN_ARENA");

    uint8 arenaslot;                                        // 2v2, 3v3 or 5v5

    recvData >> arenaslot;

    // ignore if we already in BG or BG queue
    if (_player->InBattleground())
        return;

    if (arenaslot == PVP_SLOT_ARENA_5v5 && sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLED))
    {
        ChatHandler(GetPlayer()).PSendSysMessage(LANG_SOLO_QUEUE_5V5_DISALED);
        return;
    }

    uint8 arenatype = ArenaTeam::GetTypeBySlot(arenaslot);

    //check existance
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bg)
    {
        TC_LOG_ERROR("network", "Battleground: template bg (all arenas) not found");
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_ARENA_DISABLED);
        return;
    }

    BattlegroundTypeId bgTypeId = bg->GetTypeID();
    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, arenatype);
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err = ERR_BATTLEGROUND_NONE;

    Group* grp = _player->GetGroup();
    // no group found, error
    if (!grp)
        return;
    if (grp->GetLeaderGUID() != _player->GetGUID())
        return;

    RatedPvpSlot slot = RatedPvpSlot(arenaslot);

    ArenaTeam team{ slot, grp };
    uint32 arenaRating = team.GetRating();
    uint32 matchmakerRating = team.GetMatchmakerRating();

    // the arenateam id must match for everyone in the group
    if (arenaRating <= 0)
        arenaRating = 1;

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);

    uint32 avgTime = 0;
    GroupQueueInfo* ginfo = nullptr;

    err = grp->CanJoinBattlegroundQueue(bg, bgQueueTypeId, arenatype, arenatype, true, arenaslot);
    if (!err)
    {
        TC_LOG_DEBUG("bg.battleground", "Battleground: leader %s queued with matchmaker rating %u for type %u", _player->GetName().c_str(), matchmakerRating, arenatype);

        ginfo = bgQueue.AddGroup(_player, grp, bgTypeId, bracketEntry, arenatype, true, false, arenaRating, matchmakerRating);
        avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
    }

    for (auto&& member : *grp)
    {
        if (err)
        {
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, err);
            member->GetSession()->SendPacket(&data);
            continue;
        }

        // add to queue
        uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);

        // add joined time data
        member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

        WorldPacket data; // send status packet (in queue)
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, arenatype);
        member->GetSession()->SendPacket(&data);

        TC_LOG_DEBUG("bg.battleground", "Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUID().GetCounter(), member->GetName().c_str());

    }

    sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, arenatype, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

void WorldSession::HandleBattlemasterJoinRated(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_BATTLEMASTER_JOIN_RATED");

    // ignore if we already in BG or BG queue
    if (_player->InBattleground())
        return;

    //check existance
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_RATED_10_VS_10);
    if (!bg)
    {
        TC_LOG_ERROR("network", "Battleground: template bg (10 vs 10) not found");
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_RATED_10_VS_10, NULL))
        return;

    BattlegroundTypeId bgTypeId = bg->GetTypeID();
    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, 0);

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), GetPlayer()->GetLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err = ERR_BATTLEGROUND_NONE;

    Group* group = GetPlayer()->GetGroup();

    // no group found, error
    if (!group)
        return;

    if (group->GetLeaderGUID() != GetPlayer()->GetGUID())
        return;

    ArenaTeam at{ PVP_SLOT_RATED_BG, group };
    uint32 arenaRating = at.GetRating();
    uint32 matchmakerRating = at.GetMatchmakerRating();

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);

    uint32 avgTime = 0;
    GroupQueueInfo* ginfo = nullptr;

    uint32 memberCount = sBattlegroundMgr->IsRatedBgTesting() ? 2 : bg->GetMaxPlayersPerTeam();

    err = group->CanJoinBattlegroundQueue(bg, bgQueueTypeId, memberCount, memberCount, true, 0);
    if (!err)
    {
        ginfo = bgQueue.AddGroup(_player, group, bgTypeId, bracketEntry, 0, false, true, arenaRating, matchmakerRating);
        avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
    }

    for (auto&& member : *group)
    {
        if (err)
        {
            WorldPacket data;
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, _player, 0, err);
            member->GetSession()->SendPacket(&data);
            continue;
        }

        // add to queue
        uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);

        // add joined time data
        member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

        WorldPacket data; // send status packet (in queue)
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, 0);
        member->GetSession()->SendPacket(&data);

        TC_LOG_DEBUG("bg.battleground", "Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUID().GetCounter(), member->GetName().c_str());
    }

    sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, 0, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

void WorldSession::HandleReportPvPAFK(WorldPacket& recvData)
{
    ObjectGuid playerGuid;

    playerGuid[3] = recvData.ReadBit();
    playerGuid[6] = recvData.ReadBit();
    playerGuid[1] = recvData.ReadBit();
    playerGuid[4] = recvData.ReadBit();
    playerGuid[5] = recvData.ReadBit();
    playerGuid[0] = recvData.ReadBit();
    playerGuid[7] = recvData.ReadBit();
    playerGuid[2] = recvData.ReadBit();

    recvData.ReadByteSeq(playerGuid[3]);
    recvData.ReadByteSeq(playerGuid[0]);
    recvData.ReadByteSeq(playerGuid[4]);
    recvData.ReadByteSeq(playerGuid[1]);
    recvData.ReadByteSeq(playerGuid[7]);
    recvData.ReadByteSeq(playerGuid[6]);
    recvData.ReadByteSeq(playerGuid[2]);
    recvData.ReadByteSeq(playerGuid[5]);

    if (Battleground* bg = GetPlayer()->GetBattleground())
        bg->ReportAfk(playerGuid, GetPlayer());
}

// not in 5.4.8
// void WorldSession::HandleRequestRatedBgInfo(WorldPacket & recvData)
// {
//     TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_RATED_BG_INFO");

//     uint8 unk;
//     recvData >> unk;

//     TC_LOG_DEBUG("bg.battleground", "WorldSession::HandleRequestRatedBgInfo: unk = %u", unk);

//     /// @Todo: perfome research in this case
//     /// The unk fields are related to arenas
//     WorldPacket data(SMSG_RATED_BG_STATS, 72);
//     data << uint32(0);      // BgWeeklyWins20vs20
//     data << uint32(0);      // BgWeeklyPlayed20vs20
//     data << uint32(0);      // BgWeeklyPlayed15vs15
//     data << uint32(0);
//     data << uint32(0);      // BgWeeklyWins10vs10
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);      // BgWeeklyWins15vs15
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);
//     data << uint32(0);      // BgWeeklyPlayed10vs10
//     data << uint32(0);
//     data << uint32(0);

//     SendPacket(&data);
// }

void WorldSession::HandleRequestPvpOptions(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_PVP_OPTIONS_ENABLED");

    // triggers PVP_TYPES_ENABLED lua event
    WorldPacket data(SMSG_PVP_OPTIONS_ENABLED, 1);
    data.WriteBit(1);
    data.WriteBit(1);
    data.WriteBit(1);
    data.WriteBit(1);
    data.WriteBit(1);

    data.FlushBits();

    SendPacket(&data);
}

void WorldSession::HandleRequestPvpReward(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_PVP_REWARDS");

    _player->SendPvpRewards();
}

void WorldSession::HandleRequestRatedBgStats(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_RATED_BG_STATS");

    WorldPacket data(SMSG_BATTLEFIELD_RATED_INFO, 128);

    for (uint32 i = PVP_SLOT_ARENA_2v2; i < PVP_SLOT_MAX; ++i)
    {
        data << uint32(0);  // unk always 0, maybe this should be projected cap
        if (auto info = RatedPvpMgr::Instance()->GetInfo(RatedPvpSlot(i), GetPlayer()->GetGUID()))
        {
            data << uint32(info->SeasonWins);
            data << uint32(info->SeasonGames);
            data << uint32(info->Rating);
            data << uint32(info->WeekWins);
            data << uint32(info->WeekBest);
            data << uint32(info->WeekGames);
            data << uint32(info->SeasonBest);
        }
        else
        {
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
        }
    }

    SendPacket(&data);
}

void WorldSession::HandleRequestConquestFormulaConstants(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS");

    WorldPacket data(SMSG_CONQUEST_FORMULA_CONSTANTS);

    data << uint32(2000);    // PvpMinCPPerWeek
    data << float(1639.28);  // PvpCPExpCoefficient
    data << float(0.00412);  // PvpCPNumerator
    data << uint32(3500);    // PvpMaxCPPerWeek
    data << float(1511.26);  // PvpCPBaseCoefficient

    SendPacket(&data);
}

void WorldSession::EnterSoloQueue(bool asGroup)
{

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bg)
        return;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), GetPlayer()->GetLevel());
    if (!bracketEntry)
        return;

    BattlegroundQueueTypeId bgQueueTypeId = BATTLEGROUND_QUEUE_SOLO;
    GroupJoinBattlegroundResult err = ERR_BATTLEGROUND_JOIN_FAILED;
    Group* group = nullptr;
    std::vector<uint64> g;
    WorldPacket data;

    uint32 arenaRating = 0;
    uint32 matchmakerRating = 0;
    std::array<SoloQueueRole, 2> arr;
    SoloQueue& queue = sBattlegroundMgr->GetSoloQueue();

    if (asGroup)
    {
        group = GetPlayer()->GetGroup();
        if (!group)
        {
            SendNotification(LANG_NOT_IN_GROUP);
            return;
        }

        if (group->GetLeaderGUID() != GetPlayer()->GetGUID())
            return;

        err = group->CanJoinBattlegroundQueue(bg, bgQueueTypeId, 2, 2, false, PVP_SLOT_ARENA_5v5);
        if (err > 0)
        {
            Player* player1 = nullptr;
            Player* player2 = nullptr;
            uint32 rating1 = 0, rating2 = 0;
            for (auto&& player : *group)
            {
                if (player->HasAura(SPELL_DESERTER))
                {
                    err = ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS;
                    break;
                }
                if (!player1)
                {
                    player1 = player;
                    rating1 = sRatedPvpMgr->GetOrCreateInfo(PVP_SLOT_ARENA_5v5, player->GetGUID())->MatchmakerRating;
                }
                else
                {
                    player2 = player;
                    rating2 = sRatedPvpMgr->GetOrCreateInfo(PVP_SLOT_ARENA_5v5, player->GetGUID())->MatchmakerRating;
                }
                arenaRating += sRatedPvpMgr->GetOrCreateInfo(PVP_SLOT_ARENA_5v5, player->GetGUID())->Rating;
                if (player == group->GetFirstMember()->GetSource())
                {
                    matchmakerRating += rating1;
                    arr[0] = queue.GetRole(player->GetSpecialization());
                }
                else
                {
                    matchmakerRating += rating2;
                    arr[1] = queue.GetRole(player->GetSpecialization());
                }

                if (sWorld->getIntConfig(CONFIG_SOLO_QUEUE_AVERAGE_ITEM_LEVEL) > 0)
                {
                    uint32 ilvl = uint32(std::floor(player->GetAverageItemLevel()));
                    uint32 threshold = sWorld->getIntConfig(CONFIG_SOLO_QUEUE_AVERAGE_ITEM_LEVEL);
                    if (ilvl < threshold)
                    {
                        SendNotification(LANG_SOLO_QUEUE_ITEM_LEVEL_ERROR, ilvl, threshold);
                        return;
                    }
                }
                if (sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_PVP_POWER) > 0)
                {
                    auto field = queue.GetRole(player->GetSpecialization()) == SoloQueueRole::Healer ? PLAYER_FIELD_PVP_POWER_HEALING : PLAYER_FIELD_PVP_POWER_DAMAGE;
                    uint32 pvpPower = uint32(std::floor(player->GetFloatValue(field)));
                    uint32 threshold = sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_PVP_POWER);
                    if (pvpPower < threshold)
                    {
                        SendNotification(LANG_SOLO_QUEUE_PVP_POWER_ERROR, pvpPower, threshold);
                        return;
                    }
                }
            }

            if (!err)
            {
                SoloGroup g1, g2;
                g1.Team = player1->GetTeam();
                g2.Team = player2->GetTeam();
                g1.Members.push_back(player1);
                g1.AddPlayer(player1);
                g2.Members.push_back(player2);
                g2.AddPlayer(player2);

                uint32 rdiff = rating1 > rating2 ? rating1 - rating2 : rating2 - rating1;
                if (!queue.IsCompatible(&g1, &g2, false) ||
                    (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_GROUP_ONLY_HEALER_AND_DAMAGER) &&
                        !((arr[0] != SoloQueueRole::Healer && arr[1] == SoloQueueRole::Healer) || (arr[0] == SoloQueueRole::Healer && arr[1] != SoloQueueRole::Healer))) ||
                    rdiff > sWorld->getIntConfig(CONFIG_SOLO_QUEUE_GROUP_RATING_DIFFERENCE))
                {
                    SendNotification(LANG_SOLO_QUEUE_INVALID_GROUP);
                    return;
                }

                if (arr[0] == SoloQueueRole::None || arr[1] == SoloQueueRole::None)
                {
                    SendNotification(LANG_SOLO_QUEUE_SPEC_ERROR);
                    return;
                }
                arenaRating /= group->GetMembersCount();
                matchmakerRating /= group->GetMembersCount();
            }
        }
    }
    else
    {
        if (sWorld->getIntConfig(CONFIG_SOLO_QUEUE_AVERAGE_ITEM_LEVEL) > 0)
        {
            uint32 ilvl = uint32(std::floor(GetPlayer()->GetAverageItemLevel()));
            uint32 threshold = sWorld->getIntConfig(CONFIG_SOLO_QUEUE_AVERAGE_ITEM_LEVEL);
            if (ilvl < threshold)
            {
                SendNotification(LANG_SOLO_QUEUE_ITEM_LEVEL_ERROR, ilvl, threshold);
                return;
            }
        }

        if (sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_PVP_POWER) > 0)
        {
            auto field = queue.GetRole(GetPlayer()->GetSpecialization()) == SoloQueueRole::Healer ? PLAYER_FIELD_PVP_POWER_HEALING : PLAYER_FIELD_PVP_POWER_DAMAGE;
            uint32 pvpPower = uint32(std::floor(GetPlayer()->GetFloatValue(field)));
            uint32 threshold = sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MIN_PVP_POWER);
            if (pvpPower < threshold)
            {
                SendNotification(LANG_SOLO_QUEUE_PVP_POWER_ERROR, pvpPower, threshold);
                return;
            }
        }

        if (GetPlayer()->InBattlegroundQueue() || GetPlayer()->IsUsingLfg())
            err = ERR_BATTLEGROUND_CANNOT_QUEUE_FOR_RATED;
        else if (GetPlayer()->HasAura(SPELL_DESERTER))
            err = ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS;
        else
        {
            auto stat = sRatedPvpMgr->GetOrCreateInfo(PVP_SLOT_ARENA_5v5, GetPlayer()->GetGUID());
            g.push_back(GetPlayer()->GetGUID());
            arenaRating = stat->Rating;
            matchmakerRating = stat->MatchmakerRating;;

            err = ERR_BATTLEGROUND_NONE;
            arr[0] = queue.GetRole(GetPlayer()->GetSpecialization());
            if (arr[0] == SoloQueueRole::None)
            {
                SendNotification(LANG_SOLO_QUEUE_SPEC_ERROR);
                return;
            }
        }
        if (err)
        {
            sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, GetPlayer(), 0, err);
            SendPacket(&data);
            return;
        }
    }

    bg->SetRated(true);

    GroupQueueInfo* ginfo = nullptr;

    if (asGroup)
    {
        uint32 avgTime = 0;

        if (!err)
        {
            ginfo = queue.AddGroup(_player, group, BATTLEGROUND_AA, bracketEntry, ARENA_TYPE_3v3, true, false, arenaRating, matchmakerRating);
            avgTime = queue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for (auto&& member : *group)
        {
            if (err)
            {
                sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, member, 0, err);
                member->GetSession()->SendPacket(&data);
                continue;
            }

            uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);
            member->AddBattlegroundQueueJoinTime(BATTLEGROUND_AA, ginfo->JoinTime);

            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, ARENA_TYPE_3v3);
            member->GetSession()->SendPacket(&data);

        }

    }
    else
    {
        ginfo = queue.AddGroup(_player, nullptr, BATTLEGROUND_AA, bracketEntry, ARENA_TYPE_3v3, true, false, arenaRating, matchmakerRating);
        uint32 avgTime = queue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        uint32 queueSlot = _player->AddBattlegroundQueueId(bgQueueTypeId);

        GetPlayer()->AddBattlegroundQueueJoinTime(BATTLEGROUND_AA, ginfo->JoinTime);

        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, GetPlayer(), queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, ARENA_TYPE_3v3);
        SendPacket(&data);
    }

    if (ginfo)
        queue.DoMatchmaking(ginfo, 0, true);
}

