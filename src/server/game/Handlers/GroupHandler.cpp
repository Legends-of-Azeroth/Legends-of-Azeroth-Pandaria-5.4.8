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

#include "Common.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "PartyPackets.h"
#include "Pet.h"
#include "Player.h"
#include "SocialMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "InstanceScript.h"
#include "LFGMgr.h"

class Aura;

/* differeces from off:
    -you can uninvite yourself - is is useful
    -you can accept invitation even if leader went offline
*/
/* todo:
    -group_destroyed msg is sent but not shown
    -reduce xp gaining when in raid group
    -quest sharing has to be corrected
    -FIX sending PartyMemberStats
*/

void WorldSession::SendPartyResult(PartyOperation operation, const std::string& member, PartyResult res, uint32 val /* = 0 */)
{
    WorldPacket data(SMSG_PARTY_COMMAND_RESULT, 4 + member.size() + 1 + 4 + 4 + 8);
    data << uint32(operation);
    data << member;
    data << uint32(res);
    data << uint32(val);                                // LFD cooldown related (used with ERR_PARTY_LFG_BOOT_COOLDOWN_S and ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S)
    data << uint64(0);                                  // player who caused error (in some cases).

    SendPacket(&data);
}

void WorldSession::SendGroupInviteNotification(const std::string& inviterName, bool inGroup)
{
    TC_LOG_DEBUG("network", "WORLD: sending SMSG_GROUP_INVITE");
    
    ObjectGuid invitedGuid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_GROUP_INVITE, 6 + 1 + 8 + 8 + 4 + 4 + 4 + inviterName.size());
    data.WriteBits(0, 8);
    data.WriteBits(0, 8);
    data.WriteBit(invitedGuid[2]);
    data.WriteBit(0);
    data.WriteBits(inviterName.size(), 6);              // inviter name length
    data.WriteBit(invitedGuid[7]);
    data.WriteBit(invitedGuid[5]);
    data.WriteBit(!inGroup);                            // inverse already in group
    data.WriteBit(0);                                   // auto decline
    data.WriteBit(invitedGuid[1]);
    data.WriteBit(0);                                   // cross realm invite (includes hyphen between inviter and server name)
    data.WriteBit(0);                                   // realm transfer warning("Accepting this invitation may transfer you to another realm")
    data.WriteBits(0, 22);                              // counter
    data.WriteBit(invitedGuid[3]);
    data.WriteBit(invitedGuid[0]);
    data.WriteBit(invitedGuid[4]);
    data.WriteBit(invitedGuid[6]);
    data.FlushBits();

    data.WriteByteSeq(invitedGuid[6]);
    data.WriteByteSeq(invitedGuid[7]);
    data.WriteByteSeq(invitedGuid[2]);
    data.WriteByteSeq(invitedGuid[0]);
    data << uint64(0);
    data << uint32(0);
    data << uint32(0);
    data.WriteByteSeq(invitedGuid[1]);
    data.WriteByteSeq(invitedGuid[5]);
    data.WriteByteSeq(invitedGuid[4]);
    data << int32(0);
    data.WriteString(inviterName);
    data.WriteByteSeq(invitedGuid[3]);
    data << uint32(0);

    /*for (int i = 0; i < counter; i++)
        data << int32(0);*/

    SendPacket(&data);
}

void WorldSession::HandleGroupInviteOpcode(WorldPackets::Party::PartyInviteClient& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_INVITE");

    // attempt add selected player

    // cheating
    if (!normalizePlayerName(packet.MemberName))
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    Player* player = ObjectAccessor::FindPlayerByName(packet.MemberName);

    // no player
    if (!player)
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // restrict invite to GMs
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_GM_GROUP) && !GetPlayer()->IsGameMaster() && player->IsGameMaster())
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // can't group with
    if (!GetPlayer()->IsGameMaster() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && GetPlayer()->GetTeam() != player->GetTeam())
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_PLAYER_WRONG_FACTION);
        return;
    }

    if (GetPlayer()->GetInstanceId() != 0 && player->GetInstanceId() != 0 && GetPlayer()->GetInstanceId() != player->GetInstanceId() && GetPlayer()->GetMapId() == player->GetMapId())
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_TARGET_NOT_IN_INSTANCE_S);
        return;
    }

    // just ignore us
    if (player->GetInstanceId() != 0 && player->GetDungeonDifficulty() != GetPlayer()->GetDungeonDifficulty())
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_IGNORING_YOU_S);
        return;
    }

    if (player->GetSocial()->HasIgnore(GetPlayer()->GetGUID()))
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_IGNORING_YOU_S);
        return;
    }

    // Player must not invite any player in challenge mode
    if (InstanceScript* instance = GetPlayer()->GetInstanceScript())
    {
        if (instance->instance->IsChallengeDungeon() && instance->IsChallengeModeStarted())
        {
            SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_IGNORING_YOU_S);
            return;
        }
    }

    // Player must not invite if invited player in challenge mode
    if (InstanceScript* instance = player->GetInstanceScript())
    {
        if (instance->instance->IsChallengeDungeon() && instance->IsChallengeModeStarted())
        {
            SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_IGNORING_YOU_S);
            return;
        }
    }

    Group* group = GetPlayer()->GetGroup();
    if (group)
    {
        // If any player in group in challenge dungeon leader must not allowed to invite any players
        for (auto itr = group->GetMemberSlots().begin(); itr != group->GetMemberSlots().end(); itr++)
        {
            if (Player* plr = ObjectAccessor::FindConnectedPlayer(itr->guid))
                if (InstanceScript* instance = plr->GetInstanceScript())
                    if (instance->instance->IsChallengeDungeon() && instance->IsChallengeModeStarted())
                    {
                        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_IGNORING_YOU_S);
                        return;
                    }
        }
    }

    if (group && group->isBGGroup())
        group = GetPlayer()->GetOriginalGroup();

    Group* group2 = player->GetGroup();
    if (group2 && group2->isBGGroup())
        group2 = player->GetOriginalGroup();
    // player already in another group or invited
    if (group2 || player->GetGroupInvite() || player == GetPlayer()) // Sometimes client allows to invite yourself
    {
        SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_ALREADY_IN_GROUP_S);

        if (group2)
        {
            // tell the player that they were invited but it failed as they were already in a group
            player->GetSession()->SendGroupInviteNotification(GetPlayer()->GetName(), true);
        }

        return;
    }

    if (group)
    {
        // not have permissions for invite
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_NOT_LEADER);
            return;
        }
        // not have place
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }

        if (group->isLFGGroup() && _player->GetMap()->GetDifficulty() != RAID_DIFFICULTY_1025MAN_FLEX)
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_LFG_PENDING);
            return;
        }
    }

    // ok, but group not exist, start a new group
    // but don't create and save the group to the DB until
    // at least one person joins
    if (!group)
    {
        group = new Group;
        // new group: if can't add then delete
        if (!group->AddLeaderInvite(GetPlayer()) || !group->AddInvite(player))
        {
            delete group;
            return;
        }
    }
    else
    {
        // already existed group: if can't add then just leave
        if (!group->AddInvite(player))
        {
            return;
        }
    }

    // ok, we do it
    player->GetSession()->SendGroupInviteNotification(GetPlayer()->GetName(), false);

    SendPartyResult(PARTY_OP_INVITE, packet.MemberName, ERR_PARTY_RESULT_OK);
}

void WorldSession::HandleGroupInviteResponseOpcode(WorldPackets::Party::GroupInviteResponse& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_INVITE_RESPONSE");

    Group* group = GetPlayer()->GetGroupInvite();

    if (!group)
        return;

    if (packet.Accept)
    {
        // Remove player from invitees in any case
        group->RemoveInvite(GetPlayer());

        if (group->GetLeaderGUID() == GetPlayer()->GetGUID())
        {
            TC_LOG_ERROR("network", "HandleGroupAcceptOpcode: player {}({}) tried to accept an invite to his own group", GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
            return;
        }

        // Group is full
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }

        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // Forming a new group, create it
        if (!group->IsCreated())
        {
            // This can happen if the leader is zoning. To be removed once delayed actions for zoning are implemented
            if (!leader)
            {
                group->RemoveAllInvites();
                return;
            }

            // If we're about to create a group there really should be a leader present
            ASSERT(leader);
            group->RemoveInvite(leader);
            group->Create(leader);
        }

        // Everything is fine, do it, PLAYER'S GROUP IS SET IN ADDMEMBER!!!
        if (!group->AddMember(GetPlayer()))
            return;

        group->BroadcastGroupUpdate();

    }
    else
    {
        // Remember leader if online (group pointer will be invalid if group gets disbanded)
        Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

        // uninvite, group can be deleted
        GetPlayer()->UninviteFromGroup();

        if (!leader || !leader->GetSession())
            return;

        // report
        WorldPacket data(SMSG_GROUP_DECLINE, GetPlayer()->GetName().size());
        data << GetPlayer()->GetName();
        leader->GetSession()->SendPacket(&data);
    }
}

void WorldSession::HandleGroupUninviteGuidOpcode(WorldPackets::Party::GroupUninviteGuid& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_UNINVITE_GUID");

    //can't uninvite yourself
    if (packet.Guid == GetPlayer()->GetGUID())
    {
        TC_LOG_ERROR("network", "WorldSession::HandleGroupUninviteGuidOpcode: leader {}({}) tried to uninvite himself from the group.",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    uint32 val = 0;
    PartyResult res = GetPlayer()->CanUninviteFromGroup(NULL, packet.Guid, val);
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res, val);
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (grp->IsLeader(packet.Guid) && !grp->isLFGGroup())
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", ERR_NOT_LEADER);
        return;
    }

    if (grp->IsMember(packet.Guid))
    {
        Player::RemoveFromGroup(grp, packet.Guid, grp->IsFlex() ? GROUP_REMOVEMETHOD_KICK_LFG : GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID(), packet.Reason.c_str());
        return;
    }

    if (Player* player = grp->GetInvited(packet.Guid))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, "", ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupSetLeaderOpcode(WorldPackets::Party::GroupSetLeader& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_SET_LEADER");

    Player* player = ObjectAccessor::FindPlayer(packet.Guid);
    Group* group = GetPlayer()->GetGroup();

    if (!group || !player)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) || player->GetGroup(group->GetGroupSlot()) != group)
        return;

    // Everything's fine, accepted.
    group->ChangeLeader(packet.Guid);
    group->SendUpdate();

}

void WorldSession::HandleGroupSetRolesOpcode(WorldPackets::Party::GroupSetRoles& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_SET_ROLES");

    Player* tPlayer = ObjectAccessor::FindPlayer(packet.TargetGuid);
    Group* group = GetPlayer()->GetGroup();

    if (!tPlayer || !group)
        return;

    if (group != tPlayer->GetGroup())
        return;

    ObjectGuid assignerGuid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_GROUP_SET_ROLE, 1 + 8 + 1 + 8 + 4 + 1 + 4);
    data.WriteBit(assignerGuid[1]);
    data.WriteBit(packet.TargetGuid[7]);
    data.WriteBit(packet.TargetGuid[6]);
    data.WriteBit(packet.TargetGuid[4]);
    data.WriteBit(packet.TargetGuid[1]);
    data.WriteBit(packet.TargetGuid[0]);
    data.WriteBit(assignerGuid[0]);
    data.WriteBit(assignerGuid[7]);
    data.WriteBit(packet.TargetGuid[3]);
    data.WriteBit(assignerGuid[6]);
    data.WriteBit(packet.TargetGuid[2]);
    data.WriteBit(assignerGuid[4]);
    data.WriteBit(assignerGuid[5]);
    data.WriteBit(assignerGuid[2]);
    data.WriteBit(packet.TargetGuid[5]);
    data.WriteBit(assignerGuid[3]);

    data.WriteByteSeq(assignerGuid[1]);
    data.WriteByteSeq(assignerGuid[6]);
    data.WriteByteSeq(assignerGuid[2]);
    data.WriteByteSeq(packet.TargetGuid[3]);
    data << uint32(group->GetMemberRole(packet.TargetGuid));
    data.WriteByteSeq(assignerGuid[7]);
    data.WriteByteSeq(packet.TargetGuid[5]);
    data.WriteByteSeq(assignerGuid[3]);
    data.WriteByteSeq(packet.TargetGuid[4]);
    data.WriteByteSeq(packet.TargetGuid[7]);
    data.WriteByteSeq(assignerGuid[5]);
    data.WriteByteSeq(packet.TargetGuid[6]);
    data.WriteByteSeq(packet.TargetGuid[2]);
    data.WriteByteSeq(packet.TargetGuid[1]);
    data.WriteByteSeq(packet.TargetGuid[0]);
    data.WriteByteSeq(assignerGuid[4]);
    data << uint8(0);                           // unknown
    data.WriteByteSeq(assignerGuid[0]);
    data << uint32(packet.NewRole);

    group->BroadcastPacket(&data, false);
    group->SetMemberRole(packet.TargetGuid, packet.NewRole);
    group->SendUpdate();
}

void WorldSession::HandleGroupDisbandOpcode(WorldPackets::Party::GroupDisband& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_DISBAND");

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    if (_player->InBattleground())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
        return;
    }

    /** error handling **/
    /********************/

    // everything's fine, do it
    SendPartyResult(PARTY_OP_LEAVE, GetPlayer()->GetName(), ERR_PARTY_RESULT_OK);

    GetPlayer()->RemoveFromGroup(GROUP_REMOVEMETHOD_LEAVE);
}

void WorldSession::HandleLootMethodOpcode(WorldPackets::Party::LootMethod& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LOOT_METHOD");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    /** error handling **/
    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;
    /********************/

    // everything's fine, do it
    group->SetLootMethod((LootMethod)packet.Method);
    group->SetMasterLooterGuid(packet.LootMaster);
    group->SetLootThreshold((ItemQualities)packet.LootThreshold);
    group->SendUpdate();
}

void WorldSession::HandleLootRoll(WorldPackets::Loot::LootRoll& packet)
{
    ObjectGuid const lootGuid = packet.LootObj;
    uint8 const itemSlot = packet.LootListID;
    uint8 const rollType = packet.RollType;              // 0: pass, 1: need, 2: greed

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    group->CountRollVote(lootGuid, GetPlayer()->GetGUID(), itemSlot, rollType);

    switch (rollType)
    {
        case ROLL_NEED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
            break;
        case ROLL_GREED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
    }
}

void WorldSession::HandleMinimapPingOpcode(WorldPackets::Party::MinimapPingClient& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_MINIMAP_PING");

    if (!GetPlayer()->GetGroup())
        return;

    //TC_LOG_DEBUG("misc", "Received opcode MSG_MINIMAP_PING X: {}, Y: {}", packet.X, packet.Y);

    /** error handling **/
    /********************/

    ObjectGuid guid = GetPlayer()->GetGUID();

    // everything's fine, do it
    WorldPacket data(SMSG_MINIMAP_PING, 1 + 8 + 4 + 4);
    data << float(packet.Y);
    data << float(packet.X);

    data.WriteBit(guid[0]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[4]);

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[4]);

    GetPlayer()->GetGroup()->BroadcastPacket(&data, true, -1, GetPlayer()->GetGUID());
}

void WorldSession::HandleRandomRollOpcode(WorldPackets::Party::RandomRollClient& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RANDOM_ROLL");

    /** error handling **/
    if (packet.Minimum > packet.Maximum || packet.Maximum > 10000)                // < 32768 for urand call
        return;
    /********************/

    // everything's fine, do it
    uint32 roll = urand(packet.Minimum, packet.Maximum);

    //TC_LOG_DEBUG("misc", "ROLL: MIN: {}, MAX: {}, ROLL: {}", packet.Minimum, packet.Maximum, roll);

    ObjectGuid guid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_RANDOM_ROLL, 4 + 4 + 4 + 1 + 8);
    data << uint32(roll);
    data << uint32(packet.Minimum);
    data << uint32(packet.Maximum);

    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[3]);

    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);

    if (GetPlayer()->GetGroup())
        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    else
        SendPacket(&data);

}

void WorldSession::HandleRaidTargetUpdateOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received MSG_RAID_TARGET_UPDATE");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint8 PartyIndex, Symbol;
    recvData >> PartyIndex >> Symbol;

    /** error handling **/
    /********************/

    // everything's fine, do it
    if (Symbol == 0xFF)                                     // target icon request
        group->SendTargetIconList(this);
    else                                                    // target icon update
    {
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;

        ObjectGuid targetGuid;

        targetGuid[3] = recvData.ReadBit();
        targetGuid[2] = recvData.ReadBit();
        targetGuid[1] = recvData.ReadBit();
        targetGuid[5] = recvData.ReadBit();
        targetGuid[0] = recvData.ReadBit();
        targetGuid[6] = recvData.ReadBit();
        targetGuid[7] = recvData.ReadBit();
        targetGuid[4] = recvData.ReadBit();

        recvData.ReadByteSeq(targetGuid[2]);
        recvData.ReadByteSeq(targetGuid[3]);
        recvData.ReadByteSeq(targetGuid[0]);
        recvData.ReadByteSeq(targetGuid[7]);
        recvData.ReadByteSeq(targetGuid[5]);
        recvData.ReadByteSeq(targetGuid[1]);
        recvData.ReadByteSeq(targetGuid[6]);
        recvData.ReadByteSeq(targetGuid[4]);

        group->SetTargetIcon(Symbol, _player->GetGUID(), targetGuid, PartyIndex);
    }
}

void WorldSession::HandleGroupRaidConvertOpcode(WorldPackets::Party::GroupRaidConvert& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_RAID_CONVERT");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (_player->InBattleground())
        return;

    if (group->isLFGGroup())
        return;

    // error handling
    if (!group->IsLeader(GetPlayer()->GetGUID()) || group->GetMembersCount() < 2)
        return;

    // everything's fine, do it (is it 0 (PARTY_OP_INVITE) correct code)
    SendPartyResult(PARTY_OP_INVITE, "", ERR_PARTY_RESULT_OK);

    // New 4.x: it is now possible to convert a raid to a group if member count is 5 or less
    if (packet.ToRaid)
        group->ConvertToRaid();
    else
        group->ConvertToGroup();
}

void WorldSession::HandleGroupChangeSubGroupOpcode(WorldPackets::Party::GroupChangeSubGroup& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_CHANGE_SUB_GROUP");

    // we will get correct pointer for group here, so we don't have to check if group is BG raid
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (packet.GroupNr >= MAX_RAID_SUBGROUPS)
        return;

    ObjectGuid senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    if (!group->HasFreeSlotSubGroup(packet.GroupNr))
        return;

    std::string name;
    if (sObjectMgr->GetPlayerNameByGUID(packet.Guid, name))
        group->ChangeMembersGroup(packet.Guid, packet.GroupNr);
}

void WorldSession::HandleGroupSwapSubGroupOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_SWAP_SUB_GROUP");
    std::string unk1;
    std::string unk2;

    recvData >> unk1;
    recvData >> unk2;
}

void WorldSession::HandleGroupAssistantLeaderOpcode(WorldPackets::Party::GroupAssistantLeader& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_ASSISTANT_LEADER");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    group->SetGroupMemberFlag(packet.Guid, packet.Apply, MEMBER_FLAG_ASSISTANT);

    group->SendUpdate();

}

void WorldSession::HandleGroupEveryoneIsAssistantOpcode(WorldPackets::Party::SetEveryoneIsAssistant& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_SET_EVERYONE_IS_ASSISTANT");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    group->ChangeFlagEveryoneAssistant(packet.Apply);
}

void WorldSession::HandlePartyAssignmentOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received MSG_PARTY_ASSIGNMENT");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    ObjectGuid senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    uint8 assignment;
    bool apply;
    uint64 guid;
    recvData >> assignment >> apply;
    recvData >> guid;

    switch (assignment)
    {
        case GROUP_ASSIGN_MAINASSIST:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINASSIST);
            group->SetGroupMemberFlag(ObjectGuid(guid), apply, MEMBER_FLAG_MAINASSIST);
            break;
        case GROUP_ASSIGN_MAINTANK:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINTANK);           // Remove main assist flag from current if any.
            group->SetGroupMemberFlag(ObjectGuid(guid), apply, MEMBER_FLAG_MAINTANK);
        default:
            break;
    }

    group->SendUpdate();
}

void WorldSession::HandleRaidReadyCheckOpcode(WorldPackets::Party::RaidReadyCheck& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RAID_READY_CHECK");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    ObjectGuid playerGuid = GetPlayer()->GetGUID();

    /** error handling **/
    if (!group->IsLeader(playerGuid) && !group->IsAssistant(playerGuid))
        return;

    // check is also done client side
    if (group->ReadyCheckInProgress())
        return;
    /********************/

    uint32 readyCheckDuration = 35000;
    ObjectGuid groupGuid = group->GetGUID();

    // everything's fine, do it
    WorldPacket data(SMSG_RAID_READY_CHECK, 1 + 8 + 1 + 8 + 1 + 4);
    data.WriteBit(groupGuid[4]);
    data.WriteBit(groupGuid[2]);
    data.WriteBit(playerGuid[4]);
    data.WriteBit(groupGuid[3]);
    data.WriteBit(groupGuid[7]);
    data.WriteBit(groupGuid[1]);
    data.WriteBit(groupGuid[0]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(playerGuid[5]);
    data.WriteBit(groupGuid[6]);
    data.WriteBit(groupGuid[5]);
    data.WriteBit(playerGuid[0]);
    data.WriteBit(playerGuid[1]);
    data.WriteBit(playerGuid[2]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[3]);

    data << uint32(readyCheckDuration);
    data.WriteByteSeq(groupGuid[2]);
    data.WriteByteSeq(groupGuid[7]);
    data.WriteByteSeq(groupGuid[3]);
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(groupGuid[1]);
    data.WriteByteSeq(groupGuid[0]);
    data.WriteByteSeq(playerGuid[1]);
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(groupGuid[6]);
    data.WriteByteSeq(playerGuid[0]);
    data << uint8(0);                       // unknown
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(groupGuid[4]);
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(groupGuid[5]);

    group->BroadcastPacket(&data, false);

    group->ReadyCheck(playerGuid);
    group->ReadyCheckMemberHasResponded(playerGuid);
    group->OfflineReadyCheck();

    // leader keeps track of ready check timer
    GetPlayer()->SetReadyCheckTimer(readyCheckDuration);
}

void WorldSession::HandleRaidReadyCheckConfirmOpcode(WorldPackets::Party::RaidReadyCheckConfirm& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RAID_READY_CHECK_CONFIRM");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->ReadyCheckInProgress())
        return;

    ObjectGuid groupGuid = group->GetGUID();
    ObjectGuid playerGuid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_RAID_READY_CHECK_CONFIRM, 1 + 1 + 8 + 1 + 8);
    data.WriteBit(groupGuid[4]);
    data.WriteBit(playerGuid[5]);
    data.WriteBit(playerGuid[3]);
    data.WriteBit(packet.Status);
    data.WriteBit(groupGuid[2]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(groupGuid[3]);
    data.WriteBit(playerGuid[0]);
    data.WriteBit(playerGuid[1]);
    data.WriteBit(groupGuid[1]);
    data.WriteBit(groupGuid[5]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[4]);
    data.WriteBit(groupGuid[6]);
    data.WriteBit(playerGuid[2]);
    data.WriteBit(groupGuid[0]);
    data.WriteBit(groupGuid[7]);
    data.FlushBits();

    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[1]);
    data.WriteByteSeq(groupGuid[4]);
    data.WriteByteSeq(groupGuid[2]);
    data.WriteByteSeq(playerGuid[0]);
    data.WriteByteSeq(groupGuid[5]);
    data.WriteByteSeq(groupGuid[3]);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(groupGuid[6]);
    data.WriteByteSeq(groupGuid[1]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(groupGuid[0]);
    data.WriteByteSeq(groupGuid[7]);

    group->BroadcastPacket(&data, false);
    group->ReadyCheckMemberHasResponded(playerGuid);

    if (group->ReadyCheckAllResponded())
    {
        Player* initiator = ObjectAccessor::FindPlayer(group->ReadyCheckInitiator());
        if (initiator)
            initiator->SetReadyCheckTimer(0);

        group->ReadyCheck(ObjectGuid::Empty);
        group->ReadyCheckResetResponded();
        group->SendReadyCheckCompleted();
    }
}

void WorldSession::HandleRaidReadyCheckFinishedOpcode(WorldPacket& /*recvData*/)
{
    //Group* group = GetPlayer()->GetGroup();
    //if (!group)
    //    return;

    //if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
    //    return;

    // Is any reaction need?
}

void WorldSession::BuildPartyMemberStatsChangedPacket(Player* player, WorldPacket* data, uint32 mask, ObjectGuid guid, bool full /*= false*/)
{
    if (mask == GROUP_UPDATE_FLAG_NONE)
        return;

    std::set<uint32> phases;
    if (player)
    {
        player->GetPhaseMgr().GetActivePhases(phases);

        for (auto itr : player->GetPhases())
            phases.insert(itr);
    }

    ByteBuffer dataBuffer;

    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)                // if update power type, update current/max power also
        mask |= (GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER);

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)            // same for pets
        mask |= (GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER);

    Pet* pet = NULL;
    if (!player)
        mask &= ~GROUP_UPDATE_FULL;
    else if (!(pet = player->GetPet()))
        mask &= ~GROUP_UPDATE_PET;

    mask |= GROUP_UPDATE_FLAG_STATUS;

    //if (player && (mask & GROUP_UPDATE_FLAG_STATUS))
    //    mask |= GROUP_UPDATE_PLAYER_BASE;

    data->Initialize(SMSG_PARTY_MEMBER_STATS, 200);         // average value

    data->WriteBit(guid[0]);
    data->WriteBit(guid[5]);
    data->WriteBit(false);
    data->WriteBit(guid[1]);
    data->WriteBit(guid[4]);
    data->WriteBit(false);
    data->WriteBit(guid[6]);
    data->WriteBit(guid[2]);
    data->WriteBit(guid[7]);
    data->WriteBit(guid[3]);

    data->FlushBits();

    data->WriteByteSeq(guid[3]);
    data->WriteByteSeq(guid[2]);
    data->WriteByteSeq(guid[6]);
    data->WriteByteSeq(guid[7]);
    data->WriteByteSeq(guid[5]);

    *data << uint32(mask);

    data->WriteByteSeq(guid[1]);
    data->WriteByteSeq(guid[4]);
    data->WriteByteSeq(guid[0]);

    if (mask & GROUP_UPDATE_FLAG_STATUS)
    {
        uint16 status = MEMBER_STATUS_OFFLINE;

        if (player)
        {
            status |= MEMBER_STATUS_ONLINE;

            if (player->IsPvP())
                status |= MEMBER_STATUS_PVP;

            if (player->isDead())
                status |= MEMBER_STATUS_DEAD;

            if (player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                status |= MEMBER_STATUS_GHOST;

            if (player->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP))
                status |= MEMBER_STATUS_PVP_FFA;

            if (player->isAFK())
                status |= MEMBER_STATUS_AFK;

            if (player->isDND())
                status |= MEMBER_STATUS_DND;
        }

        dataBuffer << uint16(status);
    }

    if (mask & GROUP_UPDATE_FLAG_MOP_UNK)
    {
        dataBuffer << uint8(1); // Same realms ?
        dataBuffer << uint8(0); // in lfg
    }

    if (mask & GROUP_UPDATE_FLAG_CUR_HP)
        dataBuffer << uint32(player->GetHealth());

    if (mask & GROUP_UPDATE_FLAG_MAX_HP)
        dataBuffer << uint32(player->GetMaxHealth());

    Powers powerType = POWER_MANA;
    if (player)
        powerType = player->GetPowerType();

    if (mask & GROUP_UPDATE_FLAG_POWER_TYPE)
        dataBuffer << uint8(powerType);

    if (mask & GROUP_UPDATE_FLAG_UNK20)
        dataBuffer << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_CUR_POWER)
        dataBuffer << uint16(player->GetPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_MAX_POWER)
        dataBuffer << uint16(player->GetMaxPower(powerType));

    if (mask & GROUP_UPDATE_FLAG_LEVEL)
        dataBuffer << uint16(player->GetLevel());

    if (mask & GROUP_UPDATE_FLAG_AREA)
        dataBuffer << uint16(player->GetAreaId());

    if (mask & GROUP_UPDATE_FLAG_UNK400)
        dataBuffer << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_POSITION)
        dataBuffer << uint16(player->GetPositionX()) << uint16(player->GetPositionY()) << uint16(player->GetPositionZ());

    if (mask & GROUP_UPDATE_FLAG_AURAS)
    {
        uint64 auramask = player->GetAuraUpdateMaskForRaid();

        dataBuffer << uint8(1);
        dataBuffer << uint64(auramask);
        dataBuffer << uint32(MAX_AURAS_GROUP_UPDATE);
        for (uint32 i = 0; i < MAX_AURAS_GROUP_UPDATE; ++i)
        {
            if (!(auramask & (UI64LIT(1) << i)))
                continue;

            AuraApplication const* aurApp = player->GetVisibleAura(i);
            if (!aurApp)
            {
                dataBuffer << uint32(0);
                dataBuffer << uint8(0);
                dataBuffer << uint32(0);
                continue;
            }

            dataBuffer << uint32(aurApp->GetBase()->GetId());
            dataBuffer << uint8(aurApp->GetFlags());
            dataBuffer << uint32(aurApp->GetBase()->GetEffectMask());

            if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
            {
                uint32 auraEffectMask = aurApp->GetBase()->GetEffectMask();

                size_t pos = dataBuffer.wpos();
                uint8 amount_count = 0;

                dataBuffer << uint8(0);
                for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                {
                    if (!(auraEffectMask & (1 << j)))
                        continue;

                    if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(j)) // NULL if effect flag not set
                        dataBuffer << float(eff->GetAmount());
                    else
                        dataBuffer << float(0.0f);

                    ++amount_count;
                }
                dataBuffer.put(pos, amount_count);
            }
        }
    }

    if (mask & GROUP_UPDATE_FLAG_PET_GUID)
    {
        if (pet)
            dataBuffer << uint64(pet->GetGUID());
        else
            dataBuffer << uint64(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_NAME)
    {
        if (pet)
            dataBuffer << pet->GetName();
        else
            dataBuffer << uint8(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MODEL_ID)
    {
        if (pet)
            dataBuffer << uint16(pet->GetDisplayId());
        else
            dataBuffer << uint16(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_HP)
    {
        if (pet)
            dataBuffer << uint32(pet->GetHealth());
        else
            dataBuffer << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_HP)
    {
        if (pet)
            dataBuffer << uint32(pet->GetMaxHealth());
        else
            dataBuffer << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
    {
        if (pet)
            dataBuffer << uint8(pet->GetPowerType());
        else
            dataBuffer << uint8(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_CUR_POWER)
    {
        if (pet)
            dataBuffer << uint16(pet->GetPower(pet->GetPowerType()));
        else
            dataBuffer << uint16(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PET_MAX_POWER)
    {
        if (pet)
            dataBuffer << uint16(pet->GetMaxPower(pet->GetPowerType()));
        else
            dataBuffer << uint16(0);
    }

    if (mask & GROUP_UPDATE_FLAG_MOP_UNK_2)
        dataBuffer << uint16(0); // Unk

    if (mask & GROUP_UPDATE_FLAG_PET_AURAS)
    {
        if (pet)
        {
            uint64 auramask = pet->GetAuraUpdateMaskForRaid();

            dataBuffer << uint8(1);
            dataBuffer << uint64(auramask);
            dataBuffer << uint32(MAX_AURAS_GROUP_UPDATE);
            for (uint32 i = 0; i < MAX_AURAS_GROUP_UPDATE; ++i)
            {
                if (!(auramask & (UI64LIT(1) << i)))
                    continue;

                AuraApplication const* aurApp = pet->GetVisibleAura(i);
                if (!aurApp)
                {
                    dataBuffer << uint32(0);
                    dataBuffer << uint8(0);
                    dataBuffer << uint32(0);
                    continue;
                }

                dataBuffer << uint32(aurApp->GetBase()->GetId());
                dataBuffer << uint8(aurApp->GetFlags());
                dataBuffer << uint32(aurApp->GetBase()->GetEffectMask());

                if (aurApp->GetFlags() & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                {
                    uint32 auraEffectMask = aurApp->GetBase()->GetEffectMask();

                    size_t pos = dataBuffer.wpos();
                    uint8 count = 0;
                    dataBuffer << uint8(0);
                    for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                    {
                        if (!(auraEffectMask & (1 << j)))
                            continue;

                        if (AuraEffect const* eff = aurApp->GetBase()->GetEffect(j)) // NULL if effect flag not set
                            dataBuffer << float(eff->GetAmount());
                        else
                            dataBuffer << float(0.0f);

                        ++count;
                    }
                    dataBuffer.put(pos, count);
                }
            }
        }
        else
        {
            dataBuffer << uint8(0);
            dataBuffer << uint64(0);
            dataBuffer << uint32(0);
        }
    }

    if (mask & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
    {
        if (Vehicle* veh = player->GetVehicle())
            dataBuffer << uint32(veh->GetVehicleInfo()->m_seatID[player->m_movementInfo.transport.seat]);
        else
            dataBuffer << uint32(0);
    }

    if (mask & GROUP_UPDATE_FLAG_PHASE)
    {
        dataBuffer << uint32(phases.empty() ? 0x8 : 0x10); // Same as found in SMSG_SET_PHASE_SHIFT.
        dataBuffer.WriteBits(phases.size(), 23);           // Phases Count.

        dataBuffer.FlushBits();

        for (std::set<uint32>::const_iterator itr = phases.begin(); itr != phases.end(); ++itr)
            dataBuffer << uint16(*itr);
    }

    if (mask & GROUP_UPDATE_FLAG_UNK_2000000)
        dataBuffer << uint16(0);

    if (mask & GROUP_UPDATE_FLAG_UNK_4000000)
        dataBuffer << uint32(0);

    /* this flags are not handled by client */
    /*if (mask & GROUP_UPDATE_FLAG_UNK_8000000)
    dataBuffer << uint8(0);
    if (mask & GROUP_UPDATE_FLAG_UNK_10000000)
    dataBuffer << uint8(0);
    if (mask & GROUP_UPDATE_FLAG_UNK_20000000)
    dataBuffer << uint8(253);
    if (mask & GROUP_UPDATE_FLAG_UNK_40000000)
    dataBuffer << uint8(0);*/

    *data << uint32(dataBuffer.size());
    data->append(dataBuffer);
}

/*this procedure handles clients CMSG_REQUEST_PARTY_MEMBER_STATS request*/
void WorldSession::HandleRequestPartyMemberStatsOpcode(WorldPackets::Party::RequestPartyMemberStats& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_REQUEST_PARTY_MEMBER_STATS");

    Player* player = HashMapHolder<Player>::Find(packet.Guid);
    if (player && player->GetGroup() != GetPlayer()->GetGroup())
        return;

    uint32 mask = GROUP_UPDATE_FLAG_STATUS;
    if (player)
    {
        mask |= GROUP_UPDATE_PLAYER;

        if (player->GetPet())
            mask |= GROUP_UPDATE_PET;
    }

    WorldPacket data;
    BuildPartyMemberStatsChangedPacket(player, &data, mask, packet.Guid, true);
    SendPacket(&data);
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPackets::Party::RequestRaidInfo& packet)
{
    // every time the player checks the character screen
    _player->SendRaidInfo();
}

void WorldSession::HandleOptOutOfLootOpcode(WorldPackets::Party::OptOutOfLoot& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_OPT_OUT_OF_LOOT");

    // ignore if player not loaded
    if (!GetPlayer())                                        // needed because STATUS_AUTHED
    {
        if (packet.PassOnLoot)
            TC_LOG_ERROR("network", "CMSG_OPT_OUT_OF_LOOT value<>0 for not-loaded character!");
        return;
    }

    GetPlayer()->SetPassOnGroupLoot(packet.PassOnLoot);
}

void WorldSession::HandleGroupInitiatePollRole(WorldPackets::Party::RolePollBegin& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_ROLE_POLL_BEGIN");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    SendRolePollInform(packet.Index);
}

void WorldSession::SendRolePollInform(uint8 Index)
{
    ObjectGuid guid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_GROUP_ROLE_POLL_INFORM, 8 + 1);

    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[7]);
    data << uint8(Index);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);

    GetPlayer()->GetGroup()->BroadcastPacket(&data, false, -1);
}

void WorldSession::HandleGroupRequestJoinUpdates(WorldPackets::Party::GroupRequestJoinUpdates& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GROUP_REQUEST_JOIN_UPDATES");

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    group->SendUpdate();
    group->SendTargetIconList(this, packet.PartyIndex);
}

void WorldSession::HandleClearRaidMarkerOpcode(WorldPackets::Party::ClearRaidMarker& packet)
{
    Player* plr = GetPlayer();
    if (!plr)
        return;

    Group* group = plr->GetGroup();
    if (!group)
        return;

    if (!group->isRaidGroup() || (group->isRaidGroup() && (group->IsAssistant(plr->GetGUID()) || group->IsLeader(plr->GetGUID()) || (group->GetGroupType() & GROUPTYPE_EVERYONE_IS_ASSISTANT))))
    {
        if (packet.MarkerId >= RAID_MARKER_NONE && packet.MarkerId < MAX_RAID_MARKERS)
            group->RemoveRaidMarker(packet.MarkerId);
        else
            group->RemoveAllRaidMarkers();
    }
}
