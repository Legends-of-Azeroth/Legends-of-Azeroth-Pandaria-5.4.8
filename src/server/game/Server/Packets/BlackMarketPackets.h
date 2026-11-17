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

#ifndef BlackMarketPackets_h__
#define BlackMarketPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace BlackMarket
    {
        class BlackMarketOpen final : public ClientPacket
        {
        public:
            BlackMarketOpen(WorldPacket&& packet) : ClientPacket(CMSG_BLACK_MARKET_OPEN, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class BlackMarketRequestItems final : public ClientPacket
        {
        public:
            BlackMarketRequestItems(WorldPacket&& packet) : ClientPacket(CMSG_BLACK_MARKET_REQUEST_ITEMS, std::move(packet)) { }

            void Read() override;

            uint32 Timestamp = 0;
            ObjectGuid Guid;
        };

        class BlackMarketBidOnItem final : public ClientPacket
        {
        public:
            BlackMarketBidOnItem(WorldPacket&& packet) : ClientPacket(CMSG_BLACK_MARKET_BID_ON_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 ItemId = 0;
            uint32 AuctionId = 0;
            uint64 BidAmount = 0;
            ObjectGuid Guid;
        };
    }
}

#endif // BlackMarketPackets_h__
