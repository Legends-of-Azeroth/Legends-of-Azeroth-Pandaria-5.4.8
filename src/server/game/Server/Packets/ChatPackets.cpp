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

#include "ChatPackets.h"

void WorldPackets::Chat::EmoteClient::Read()
{
    _worldPacket >> EmoteID;
}

void WorldPackets::Chat::CTextEmote::Read()
{
    _worldPacket >> EmoteID;
    _worldPacket >> SoundIndex;

    Target[6] = _worldPacket.ReadBit();
    Target[7] = _worldPacket.ReadBit();
    Target[3] = _worldPacket.ReadBit();
    Target[2] = _worldPacket.ReadBit();
    Target[0] = _worldPacket.ReadBit();
    Target[5] = _worldPacket.ReadBit();
    Target[1] = _worldPacket.ReadBit();
    Target[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Target[0]);
    _worldPacket.ReadByteSeq(Target[5]);
    _worldPacket.ReadByteSeq(Target[1]);
    _worldPacket.ReadByteSeq(Target[4]);
    _worldPacket.ReadByteSeq(Target[2]);
    _worldPacket.ReadByteSeq(Target[3]);
    _worldPacket.ReadByteSeq(Target[7]);
    _worldPacket.ReadByteSeq(Target[6]);
}

void WorldPackets::Chat::ChatReportIgnored::Read()
{
    _worldPacket >> Reason;

    IgnoredGUID[5] = _worldPacket.ReadBit();
    IgnoredGUID[0] = _worldPacket.ReadBit();
    IgnoredGUID[1] = _worldPacket.ReadBit();
    IgnoredGUID[3] = _worldPacket.ReadBit();
    IgnoredGUID[6] = _worldPacket.ReadBit();
    IgnoredGUID[7] = _worldPacket.ReadBit();
    IgnoredGUID[4] = _worldPacket.ReadBit();
    IgnoredGUID[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(IgnoredGUID[2]);
    _worldPacket.ReadByteSeq(IgnoredGUID[0]);
    _worldPacket.ReadByteSeq(IgnoredGUID[3]);
    _worldPacket.ReadByteSeq(IgnoredGUID[4]);
    _worldPacket.ReadByteSeq(IgnoredGUID[7]);
    _worldPacket.ReadByteSeq(IgnoredGUID[6]);
    _worldPacket.ReadByteSeq(IgnoredGUID[1]);
    _worldPacket.ReadByteSeq(IgnoredGUID[5]);
}
