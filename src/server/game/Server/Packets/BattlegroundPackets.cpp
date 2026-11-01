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

#include "BattlegroundPackets.h"
#include "World.h"

void WorldPackets::Battleground::BattlefieldListRequest::Read()
{
    _worldPacket >> BgTypeId;
}

void WorldPackets::Battleground::BattlefieldPort::Read()
{
    _worldPacket >> Action >> QueueSlot >> Id >> Time;

    Guid[6] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[1]);
}

void WorldPackets::Battleground::BattlemasterJoinArena::Read()
{
    _worldPacket >> ArenaSlot;
}

void WorldPackets::Battleground::ReportPvPPlayerAFK::Read()
{
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();
    PlayerGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
}

void WorldPackets::Battleground::BattlefieldMgrQueueInviteResponse::Read()
{
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Accepted = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Battleground::BattlefieldMgrEntryInviteResponse::Read()
{
    Guid[0] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Accepted = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

void WorldPackets::Battleground::BattlefieldMgrQueueRequest::Read()
{
    Guid[5] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Battleground::AreaSpiritHealerQuery::Read()
{
    Guid[5] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Battleground::AreaSpiritHealerQueue::Read()
{
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[5]);
}

void WorldPackets::Battleground::BattlemasterJoin::Read()
{
    for (int i = 0; i < 2; i++) // blacklistedMapIds
        _worldPacket.read_skip<uint32>();

    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    AsGroup = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    HasRoleMask = !_worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[1]);

    if (HasRoleMask)
        _worldPacket >> RoleMask;
}
