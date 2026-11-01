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

#include "GuildFinderPackets.h"

void WorldPackets::GuildFinder::LFGuildAddApplication::Read()
{
    _worldPacket >> ClassRoles >> Availability >> GuildInterests;

    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    CommentLength = _worldPacket.ReadBits(10);
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    Comment = _worldPacket.ReadString(CommentLength);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
}

void WorldPackets::GuildFinder::LFGuildBrowse::Read()
{
    _worldPacket >> PlayerLevel >> Availability >> ClassRoles >> GuildInterests;
}

void WorldPackets::GuildFinder::LFGuildDeclineRecruit::Read()
{
    PlayerGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();
    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
}

void WorldPackets::GuildFinder::LFGuildGetApplications::Read()
{
}

void WorldPackets::GuildFinder::LFGuildGetRecruits::Read()
{
    _worldPacket >> UnkTime;
}

void WorldPackets::GuildFinder::LFGuildPostRequest::Read()
{
}

void WorldPackets::GuildFinder::LFGuildRemoveApplication::Read()
{
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
}

void WorldPackets::GuildFinder::LFGuildSetGuildPost::Read()
{
    _worldPacket >> Level >> Availability >> ClassRoles >> GuildInterests;

    Length = _worldPacket.ReadBits(10);
    Listed = _worldPacket.ReadBit();
    Comment = _worldPacket.ReadString(Length);
}
