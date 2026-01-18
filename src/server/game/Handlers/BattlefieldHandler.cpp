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
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Object.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Opcodes.h"
#include "Player.h"

//This send to player windows for invite player to join the war
//Param1:(guid) the guid of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(time) Time in second that the player have for accept
void WorldSession::SendBfInvitePlayerToWar(ObjectGuid guid, uint32 zoneId, uint32 pTime)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 16);

    data.WriteBit(guidBytes[0]);
    data.WriteBit(guidBytes[2]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[3]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[5]);

    data << uint32(zoneId);             // Zone Id
    data << uint32(time(NULL) + pTime); // Invite lasts until

    // Sending the packet to player
    SendPacket(&data);
}

//This send invitation to player to join the queue
void WorldSession::SendBfInvitePlayerToQueue(ObjectGuid guid)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

    data.WriteBit(0);               // unk
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[3]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[2]);
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[0]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(1);               // unk
    data.WriteBit(0);               // Has Warmup
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[1]);
    data.WriteBit(1);               // unk

    data.FlushBits();

    data.WriteByteSeq(guidBytes[5]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[3]);
    data << uint8(1);               // Warmup
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[1]);

    // Sending packet to player
    SendPacket(&data);
}

//This send packet for inform player that he join queue
//Param1:(guid) the guid of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(CanQueue) if able to queue
//Param4:(Full) on log in is full
void WorldSession::SendBfQueueInviteResponse(ObjectGuid guid, uint32 ZoneId, bool CanQueue, bool Full)
{
    const bool hasSecondGuid = false;
    const bool warmup = true;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 16);

    data.WriteBit(!hasSecondGuid);
    data.WriteBit(guidBytes[6]);

    // if (hasSecondGuid) 5 0 4 3 7 6 1 2

    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[0]);
    data.WriteBit(Full);  // Logging In: 0 wg full - 1 queue for upcoming - May be inversed - CHECK!
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[2]);

    data.FlushBits();

    // if (hasSecondGuid)  1 3 7 6 5 0 2 4

    data.WriteByteSeq(guidBytes[5]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[4]);

    data << uint32(ZoneId);

    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[0]);

    data << uint8(warmup);
    data << uint8(CanQueue);  // Accepted

    SendPacket(&data);
}

//This is call when player accept to join war
void WorldSession::SendBfEntered(ObjectGuid guid)
{
    uint8 isAFK = _player->isAFK() ? 1 : 0;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 11);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[0]);
    data.WriteBit(0);               // unk
    data.WriteBit(isAFK);           // Clear AFK
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[2]);
    data.WriteBit(0);               // unk
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(guidBytes[6]);

    data.FlushBits();

    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[5]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[1]);
    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(ObjectGuid guid, BFLeaveReason reason)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 11);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(0);               // Relocated
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[0]);
    data.WriteBit(guidBytes[2]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[3]);

    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[5]);
    data << uint8(2);               // BattleStatus
    data << uint8(reason);          // Reason
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[1]);
    SendPacket(&data);
}

// Send by client when he click on accept for queue
void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    accepted = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);

    TC_LOG_DEBUG("misc", "HandleQueueInviteResponse: GUID:" UI64FMTD " Accepted:%u", (uint64)guid, accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (accepted)
        bf->PlayerAcceptInviteToQueue(_player);
}

// Send by client on clicking in accept or refuse of invitation windows for join game
void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    accepted = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[0]);

    TC_LOG_DEBUG("misc", "HandleBattlefieldInviteResponse: GUID:" UI64FMTD " Accepted:%u", (uint64) guid, accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (accepted)
        bf->PlayerAcceptInviteToWar(_player);
    else
        if (_player->GetZoneId() == bf->GetZoneId())
            bf->KickPlayerFromBattlefield(_player->GetGUID());
}

void WorldSession::HandleBfQueueRequest(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);

    TC_LOG_DEBUG("misc", "HandleBfQueueRequest: GUID:" UI64FMTD " ", (uint64) guid);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (bf->IsWarTime())
        bf->InvitePlayerToWar(_player);
    else
    {
        uint32 timer = bf->GetTimer() / 1000;
        if (timer < 15 * MINUTE)
            bf->InvitePlayerToQueue(_player);
    }
}

void WorldSession::HandleBfExitRequest(WorldPacket& recvData)
{
    TC_LOG_DEBUG("misc", "HandleBfExitRequest");

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId());
    if (!bf)
        return;

    bf->KickPlayerFromBattlefield(_player->GetGUID());
}
