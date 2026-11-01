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

#include "LFGPackets.h"

void WorldPackets::LFG::LFGJoin::Read()
{
    _worldPacket.read_skip<uint8>();
    for (int32 i = 0; i < 3; ++i)
        _worldPacket.read_skip<uint32>();
    Roles = _worldPacket.read<uint32>();

    uint32 numDungeons = _worldPacket.ReadBits(22);
    uint32 commentLen = _worldPacket.ReadBits(8);
    _worldPacket.ReadBit();

    for (uint32 i = 0; i < numDungeons; ++i)
        Dungeons.push_back(_worldPacket.read<uint32>());

    Comment = _worldPacket.ReadString(commentLen);
}

void WorldPackets::LFG::LFGLeave::Read()
{
    _worldPacket.read_skip<uint32>();
    _worldPacket.read_skip<uint32>();
    _worldPacket.read_skip<uint32>();
    QueueId = _worldPacket.read<uint32>();

    LeaveGuid[1] = _worldPacket.ReadBit();
    LeaveGuid[6] = _worldPacket.ReadBit();
    LeaveGuid[0] = _worldPacket.ReadBit();
    LeaveGuid[7] = _worldPacket.ReadBit();
    LeaveGuid[2] = _worldPacket.ReadBit();
    LeaveGuid[4] = _worldPacket.ReadBit();
    LeaveGuid[3] = _worldPacket.ReadBit();
    LeaveGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(LeaveGuid[4]);
    _worldPacket.ReadByteSeq(LeaveGuid[5]);
    _worldPacket.ReadByteSeq(LeaveGuid[2]);
    _worldPacket.ReadByteSeq(LeaveGuid[6]);
    _worldPacket.ReadByteSeq(LeaveGuid[1]);
    _worldPacket.ReadByteSeq(LeaveGuid[3]);
    _worldPacket.ReadByteSeq(LeaveGuid[7]);
    _worldPacket.ReadByteSeq(LeaveGuid[0]);
}

void WorldPackets::LFG::DFProposalResponse::Read()
{
    ProposalID = _worldPacket.read<uint32>();
    _worldPacket.read_skip<uint32>(); // queueId
    _worldPacket.read_skip<uint32>(); // unk flags (always 3)
    _worldPacket.read_skip<uint32>(); // join time

    Accepted = _worldPacket.ReadBit();

    ObjectGuid guid1;
    ObjectGuid guid2;
    guid1[6] = _worldPacket.ReadBit();
    guid1[0] = _worldPacket.ReadBit();
    guid1[2] = _worldPacket.ReadBit();
    guid1[4] = _worldPacket.ReadBit();
    guid2[6] = _worldPacket.ReadBit();
    guid2[7] = _worldPacket.ReadBit();
    guid1[3] = _worldPacket.ReadBit();
    guid2[4] = _worldPacket.ReadBit();
    guid1[7] = _worldPacket.ReadBit();
    guid2[1] = _worldPacket.ReadBit();
    guid1[5] = _worldPacket.ReadBit();
    guid2[0] = _worldPacket.ReadBit();
    guid1[1] = _worldPacket.ReadBit();
    guid2[2] = _worldPacket.ReadBit();
    guid2[3] = _worldPacket.ReadBit();
    guid2[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid1[3]);
    _worldPacket.ReadByteSeq(guid1[6]);
    _worldPacket.ReadByteSeq(guid1[4]);
    _worldPacket.ReadByteSeq(guid1[1]);
    _worldPacket.ReadByteSeq(guid2[7]);
    _worldPacket.ReadByteSeq(guid2[0]);
    _worldPacket.ReadByteSeq(guid1[7]);
    _worldPacket.ReadByteSeq(guid2[6]);
    _worldPacket.ReadByteSeq(guid1[5]);
    _worldPacket.ReadByteSeq(guid2[3]);
    _worldPacket.ReadByteSeq(guid2[1]);
    _worldPacket.ReadByteSeq(guid2[5]);
    _worldPacket.ReadByteSeq(guid2[4]);
    _worldPacket.ReadByteSeq(guid1[0]);
    _worldPacket.ReadByteSeq(guid1[2]);
    _worldPacket.ReadByteSeq(guid2[2]);
}

void WorldPackets::LFG::DFSetRoles::Read()
{
    RolesDesired = _worldPacket.read<uint32>();
    _worldPacket.read_skip<uint8>();
}

void WorldPackets::LFG::DFBootPlayerVote::Read()
{
    Vote = _worldPacket.ReadBit();
}

void WorldPackets::LFG::DFTeleport::Read()
{
    TeleportOut = _worldPacket.ReadBit();
}

void WorldPackets::LFG::DFGetSystemInfo::Read()
{
    _worldPacket.read_skip<uint8>();
    Player = _worldPacket.ReadBit();
}

void WorldPackets::LFG::SetLfgBonusFactionId::Read()
{
    BonusFactionId = _worldPacket.read<uint32>();
}
