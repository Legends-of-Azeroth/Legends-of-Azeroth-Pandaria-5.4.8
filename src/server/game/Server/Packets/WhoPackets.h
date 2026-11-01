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

#ifndef WhoPackets_h__
#define WhoPackets_h__

#include "Packet.h"

#include <string>
#include <vector>

namespace WorldPackets
{
    namespace Who
    {
        class WhoRequestPkt final : public ClientPacket
        {
        public:
            WhoRequestPkt(WorldPacket&& packet) : ClientPacket(CMSG_WHO, std::move(packet)) { }

            void Read() override;

            int32 ClassMask = 0;
            int32 RaceMask = 0;
            uint32 LevelMax = 0;
            uint32 LevelMin = 0;
            bool ShowEnemies = false;
            bool ExactName = false;
            bool RequestServerInfo = false;
            bool ShowArenaPlayers = false;
            uint32 ZonesCount = 0;
            uint32 WordCount = 0;
            std::vector<std::string> Words;
            std::string GuildRealmName;
            std::vector<uint32> ZoneIds;
            std::string PlayerName;
            std::string RealmName;
            std::string GuildName;
            int32 Locale = 0;
            uint32 VirtualRealmAddress = 0;
            int32 Faction = 0;
        };

        class WhoIsRequest final : public ClientPacket
        {
        public:
            WhoIsRequest(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            std::string CharName;
        };
    }
}

#endif // WhoPackets_h__
