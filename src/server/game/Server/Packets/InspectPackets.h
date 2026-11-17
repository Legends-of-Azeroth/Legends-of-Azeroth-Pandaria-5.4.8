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

#ifndef InspectPackets_h__
#define InspectPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Inspect
    {
        class Inspect final : public ClientPacket
        {
        public:
            explicit Inspect(WorldPacket&& packet) : ClientPacket(CMSG_INSPECT, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
        };

        class InspectHonorStats final : public ClientPacket
        {
        public:
            explicit InspectHonorStats(WorldPacket&& packet) : ClientPacket(CMSG_INSPECT_HONOR_STATS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
        };

        class QueryInspectAchievements final : public ClientPacket
        {
        public:
            explicit QueryInspectAchievements(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_INSPECT_ACHIEVEMENTS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
        };

        class InspectRatedBGStats final : public ClientPacket
        {
        public:
            explicit InspectRatedBGStats(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_INSPECT_RATED_BG_STATS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
        };
    }
}

#endif // InspectPackets_h__
