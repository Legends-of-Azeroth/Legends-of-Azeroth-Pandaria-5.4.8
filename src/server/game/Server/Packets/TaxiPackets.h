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

#ifndef TaxiPackets_h__
#define TaxiPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include <vector>

namespace WorldPackets
{
    namespace Taxi
    {
        class ActivateTaxi final : public ClientPacket
        {
        public:
            ActivateTaxi(WorldPacket&& packet) : ClientPacket(CMSG_ACTIVATE_TAXI, std::move(packet)) { }

            void Read() override;

            std::vector<uint32> Nodes;
            ObjectGuid Guid;
        };

        class ActivateTaxiExpress final : public ClientPacket
        {
        public:
            ActivateTaxiExpress(WorldPacket&& packet) : ClientPacket(CMSG_ACTIVATE_TAXI_EXPRESS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            std::vector<uint32> Nodes;
        };

        class EnableTaxiNode final : public ClientPacket
        {
        public:
            EnableTaxiNode(WorldPacket&& packet) : ClientPacket(CMSG_ENABLE_TAXI, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class TaxiNodeStatusQuery final : public ClientPacket
        {
        public:
            TaxiNodeStatusQuery(WorldPacket&& packet) : ClientPacket(CMSG_TAXI_NODE_STATUS_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class TaxiQueryAvailableNodes final : public ClientPacket
        {
        public:
            TaxiQueryAvailableNodes(WorldPacket&& packet) : ClientPacket(CMSG_TAXI_QUERY_AVAILABLE_NODES, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };
    }
}

#endif // TaxiPackets_h__
