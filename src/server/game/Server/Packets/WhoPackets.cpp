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

#include "WhoPackets.h"

void WorldPackets::Who::WhoRequestPkt::Read()
{
    _worldPacket >> ClassMask;
    _worldPacket >> RaceMask;
    _worldPacket >> LevelMax;
    _worldPacket >> LevelMin;

    ShowEnemies = _worldPacket.ReadBit();
    ExactName = _worldPacket.ReadBit();
    RequestServerInfo = _worldPacket.ReadBit();
    uint8 guildRealmNameLen = _worldPacket.ReadBits(9);
    ShowArenaPlayers = _worldPacket.ReadBit();
    uint8 playerLen = _worldPacket.ReadBits(6);

    ZonesCount = _worldPacket.ReadBits(4);

    uint8 realmNameLen = _worldPacket.ReadBits(9);
    uint8 guildLen = _worldPacket.ReadBits(7);

    WordCount = _worldPacket.ReadBits(3);

    std::vector<uint8> wordLens(WordCount);
    for (uint8 i = 0; i < WordCount; i++)
        wordLens[i] = _worldPacket.ReadBits(7);

    Words.resize(WordCount);
    for (uint32 i = 0; i < WordCount; ++i)
        Words[i] = _worldPacket.ReadString(wordLens[i]);

    GuildRealmName = _worldPacket.ReadString(guildRealmNameLen);

    ZoneIds.resize(ZonesCount);
    for (uint32 i = 0; i < ZonesCount; ++i)
        _worldPacket >> ZoneIds[i];

    PlayerName = _worldPacket.ReadString(playerLen);
    RealmName = _worldPacket.ReadString(realmNameLen);
    GuildName = _worldPacket.ReadString(guildLen);

    if (RequestServerInfo)
    {
        _worldPacket >> Locale;
        _worldPacket >> VirtualRealmAddress;
        _worldPacket >> Faction;
    }
}

void WorldPackets::Who::WhoIsRequest::Read()
{
    _worldPacket >> CharName;
}
