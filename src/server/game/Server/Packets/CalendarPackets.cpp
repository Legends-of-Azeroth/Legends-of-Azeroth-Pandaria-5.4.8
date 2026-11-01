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

#include "CalendarPackets.h"

void WorldPackets::Calendar::CalendarGetEvent::Read()
{
    EventId = _worldPacket.read<uint64>();
}

void WorldPackets::Calendar::CalendarRemoveEvent::Read()
{
    _worldPacket.read_skip<uint64>(); // Skip InviteId
    EventId = _worldPacket.read<uint64>();
    _worldPacket.read_skip<uint32>(); // Skip flags
}

void WorldPackets::Calendar::CalendarCopyEvent::Read()
{
    EventId = _worldPacket.read<uint64>();
    InviteId = _worldPacket.read<uint64>();
    Time = _worldPacket.ReadPackedTime();
}

void WorldPackets::Calendar::CalendarInvite::Read()
{
    EventId = _worldPacket.read<uint64>();
    InviteId = _worldPacket.read<uint64>();
    IsPreInvite = _worldPacket.ReadBit();
    uint16 length = _worldPacket.ReadBits(9);
    IsGuildEvent = _worldPacket.ReadBit();
    Name = _worldPacket.ReadString(length);
}

void WorldPackets::Calendar::CalendarRemoveInvite::Read()
{
    InviteId = _worldPacket.read<uint64>();
    SenderId = _worldPacket.read<uint64>();
    EventId = _worldPacket.read<uint64>();
    Invitee[6] = _worldPacket.ReadBit();
    Invitee[3] = _worldPacket.ReadBit();
    Invitee[2] = _worldPacket.ReadBit();
    Invitee[4] = _worldPacket.ReadBit();
    Invitee[5] = _worldPacket.ReadBit();
    Invitee[7] = _worldPacket.ReadBit();
    Invitee[0] = _worldPacket.ReadBit();
    Invitee[1] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Invitee[0]);
    _worldPacket.ReadByteSeq(Invitee[4]);
    _worldPacket.ReadByteSeq(Invitee[7]);
    _worldPacket.ReadByteSeq(Invitee[3]);
    _worldPacket.ReadByteSeq(Invitee[5]);
    _worldPacket.ReadByteSeq(Invitee[1]);
    _worldPacket.ReadByteSeq(Invitee[2]);
    _worldPacket.ReadByteSeq(Invitee[6]);
}

void WorldPackets::Calendar::CalendarRSVP::Read()
{
    EventId = _worldPacket.read<uint64>();
    InviteId = _worldPacket.read<uint64>();
    Status = _worldPacket.read<uint8>();
}

void WorldPackets::Calendar::CalendarEventSignUp::Read()
{
    EventId = _worldPacket.read<uint64>();
    Tentative = _worldPacket.ReadBit();
}

void WorldPackets::Calendar::CalendarStatus::Read()
{
    SenderId = _worldPacket.read<uint64>();
    EventId = _worldPacket.read<uint64>();
    InviteId = _worldPacket.read<uint64>();
    Status = _worldPacket.read<uint8>();
    Invitee[4] = _worldPacket.ReadBit();
    Invitee[3] = _worldPacket.ReadBit();
    Invitee[7] = _worldPacket.ReadBit();
    Invitee[6] = _worldPacket.ReadBit();
    Invitee[2] = _worldPacket.ReadBit();
    Invitee[0] = _worldPacket.ReadBit();
    Invitee[5] = _worldPacket.ReadBit();
    Invitee[1] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Invitee[7]);
    _worldPacket.ReadByteSeq(Invitee[5]);
    _worldPacket.ReadByteSeq(Invitee[2]);
    _worldPacket.ReadByteSeq(Invitee[1]);
    _worldPacket.ReadByteSeq(Invitee[4]);
    _worldPacket.ReadByteSeq(Invitee[6]);
    _worldPacket.ReadByteSeq(Invitee[0]);
    _worldPacket.ReadByteSeq(Invitee[3]);
}

void WorldPackets::Calendar::CalendarModeratorStatusQuery::Read()
{
    Rank = _worldPacket.read<uint8>();
    EventId = _worldPacket.read<uint64>();
    SenderId = _worldPacket.read<uint64>();
    InviteId = _worldPacket.read<uint64>();
    Invitee[6] = _worldPacket.ReadBit();
    Invitee[5] = _worldPacket.ReadBit();
    Invitee[1] = _worldPacket.ReadBit();
    Invitee[3] = _worldPacket.ReadBit();
    Invitee[4] = _worldPacket.ReadBit();
    Invitee[7] = _worldPacket.ReadBit();
    Invitee[0] = _worldPacket.ReadBit();
    Invitee[2] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Invitee[7]);
    _worldPacket.ReadByteSeq(Invitee[5]);
    _worldPacket.ReadByteSeq(Invitee[0]);
    _worldPacket.ReadByteSeq(Invitee[4]);
    _worldPacket.ReadByteSeq(Invitee[1]);
    _worldPacket.ReadByteSeq(Invitee[3]);
    _worldPacket.ReadByteSeq(Invitee[2]);
    _worldPacket.ReadByteSeq(Invitee[6]);
}

void WorldPackets::Calendar::CalendarComplain::Read()
{
    InviteId = _worldPacket.read<uint64>();
    EventId = _worldPacket.read<uint64>();
    ComplainGuid[4] = _worldPacket.ReadBit();
    ComplainGuid[6] = _worldPacket.ReadBit();
    ComplainGuid[2] = _worldPacket.ReadBit();
    ComplainGuid[7] = _worldPacket.ReadBit();
    ComplainGuid[1] = _worldPacket.ReadBit();
    ComplainGuid[5] = _worldPacket.ReadBit();
    ComplainGuid[3] = _worldPacket.ReadBit();
    ComplainGuid[0] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(ComplainGuid[6]);
    _worldPacket.ReadByteSeq(ComplainGuid[7]);
    _worldPacket.ReadByteSeq(ComplainGuid[1]);
    _worldPacket.ReadByteSeq(ComplainGuid[0]);
    _worldPacket.ReadByteSeq(ComplainGuid[4]);
    _worldPacket.ReadByteSeq(ComplainGuid[2]);
    _worldPacket.ReadByteSeq(ComplainGuid[3]);
    _worldPacket.ReadByteSeq(ComplainGuid[5]);
}

void WorldPackets::Calendar::CalendarGuildFilter::Read()
{
    MaxLevel = _worldPacket.read<uint8>();
    MinRank = _worldPacket.read<uint8>();
    MinLevel = _worldPacket.read<uint8>();
}

void WorldPackets::Calendar::CalendarAddEvent::Read()
{
    MaxInvites = _worldPacket.read<uint32>();
    Flags = _worldPacket.read<uint32>();
    DungeonId = _worldPacket.read<int32>();
    EventPackedTime = _worldPacket.ReadPackedTime();
    Type = _worldPacket.read<uint8>();

    uint32 inviteeCount = _worldPacket.ReadBits(22);
    uint16 descriptionLength = _worldPacket.ReadBits(11);

    for (uint32 i = 0; i < inviteeCount; i++)
    {
        ObjectGuid guid;
        guid[7] = _worldPacket.ReadBit();
        guid[2] = _worldPacket.ReadBit();
        guid[6] = _worldPacket.ReadBit();
        guid[3] = _worldPacket.ReadBit();
        guid[5] = _worldPacket.ReadBit();
        guid[1] = _worldPacket.ReadBit();
        guid[0] = _worldPacket.ReadBit();
        guid[4] = _worldPacket.ReadBit();
    }

    uint16 titleLength = _worldPacket.ReadBits(8);

    Invites.resize(inviteeCount);
    for (uint32 i = 0; i < inviteeCount; i++)
    {
        ObjectGuid guid;
        _worldPacket.ReadByteSeq(guid[4]);
        _worldPacket.ReadByteSeq(guid[2]);
        _worldPacket.ReadByteSeq(guid[3]);
        _worldPacket.ReadByteSeq(guid[1]);
        _worldPacket.ReadByteSeq(guid[0]);
        _worldPacket.ReadByteSeq(guid[6]);
        _worldPacket.ReadByteSeq(guid[7]);
        Invites[i].Status = _worldPacket.read<uint8>();
        _worldPacket.ReadByteSeq(guid[5]);
        Invites[i].ModerationRank = _worldPacket.read<uint8>();
    }

    Title = _worldPacket.ReadString(titleLength);
    Description = _worldPacket.ReadString(descriptionLength);
}

void WorldPackets::Calendar::CalendarUpdateEvent::Read()
{
    MaxInvites = _worldPacket.read<uint32>();
    DungeonId = _worldPacket.read<int32>();
    EventPackedTime = _worldPacket.ReadPackedTime();
    Flags = _worldPacket.read<uint32>();
    Type = _worldPacket.read<uint8>();

    EventId[4] = _worldPacket.ReadBit();
    EventId[5] = _worldPacket.ReadBit();
    EventId[2] = _worldPacket.ReadBit();
    InviteId[4] = _worldPacket.ReadBit();
    EventId[7] = _worldPacket.ReadBit();
    EventId[0] = _worldPacket.ReadBit();
    InviteId[5] = _worldPacket.ReadBit();
    InviteId[3] = _worldPacket.ReadBit();
    EventId[6] = _worldPacket.ReadBit();
    EventId[1] = _worldPacket.ReadBit();
    InviteId[6] = _worldPacket.ReadBit();
    InviteId[2] = _worldPacket.ReadBit();
    InviteId[7] = _worldPacket.ReadBit();
    InviteId[1] = _worldPacket.ReadBit();
    InviteId[0] = _worldPacket.ReadBit();

    uint16 descriptionLength = _worldPacket.ReadBits(11);
    uint16 titleLength = _worldPacket.ReadBits(8);
    EventId[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(InviteId[6]);
    _worldPacket.ReadByteSeq(EventId[0]);
    _worldPacket.ReadByteSeq(InviteId[7]);
    _worldPacket.ReadByteSeq(InviteId[3]);
    _worldPacket.ReadByteSeq(EventId[6]);
    _worldPacket.ReadByteSeq(InviteId[1]);
    _worldPacket.ReadByteSeq(EventId[2]);
    Title = _worldPacket.ReadString(titleLength);
    _worldPacket.ReadByteSeq(InviteId[5]);
    _worldPacket.ReadByteSeq(InviteId[4]);
    _worldPacket.ReadByteSeq(EventId[5]);
    _worldPacket.ReadByteSeq(EventId[3]);
    _worldPacket.ReadByteSeq(InviteId[0]);
    _worldPacket.ReadByteSeq(EventId[4]);
    Description = _worldPacket.ReadString(descriptionLength);
    _worldPacket.ReadByteSeq(EventId[1]);
    _worldPacket.ReadByteSeq(InviteId[2]);
    _worldPacket.ReadByteSeq(EventId[7]);
}
