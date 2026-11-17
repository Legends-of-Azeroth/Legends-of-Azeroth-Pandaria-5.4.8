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

#ifndef TradePackets_h__
#define TradePackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace Trade
    {
        class BeginTrade final : public ClientPacket
        {
        public:
            BeginTrade(WorldPacket&& packet) : ClientPacket(CMSG_BEGIN_TRADE, std::move(packet)) { }

            void Read() override;
        };

        class AcceptTrade final : public ClientPacket
        {
        public:
            AcceptTrade(WorldPacket&& packet) : ClientPacket(CMSG_ACCEPT_TRADE, std::move(packet)) { }

            void Read() override;
        };

        class CancelTrade final : public ClientPacket
        {
        public:
            CancelTrade(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_TRADE, std::move(packet)) { }

            void Read() override;
        };

        class UnacceptTrade final : public ClientPacket
        {
        public:
            UnacceptTrade(WorldPacket&& packet) : ClientPacket(CMSG_UNACCEPT_TRADE, std::move(packet)) { }

            void Read() override;
        };

        class IgnoreTrade final : public ClientPacket
        {
        public:
            IgnoreTrade(WorldPacket&& packet) : ClientPacket(CMSG_IGNORE_TRADE, std::move(packet)) { }

            void Read() override;
        };

        class ClearTradeItem final : public ClientPacket
        {
        public:
            ClearTradeItem(WorldPacket&& packet) : ClientPacket(CMSG_CLEAR_TRADE_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 TradeSlot = 0;
        };

        class SetTradeItem final : public ClientPacket
        {
        public:
            SetTradeItem(WorldPacket&& packet) : ClientPacket(CMSG_SET_TRADE_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 TradeSlot = 0;
            uint8 Slot = 0;
            uint8 Bag = 0;
        };

        class SetTradeGold final : public ClientPacket
        {
        public:
            SetTradeGold(WorldPacket&& packet) : ClientPacket(CMSG_SET_TRADE_GOLD, std::move(packet)) { }

            void Read() override;

            uint64 Gold = 0;
        };

        class InitiateTrade final : public ClientPacket
        {
        public:
            InitiateTrade(WorldPacket&& packet) : ClientPacket(CMSG_INITIATE_TRADE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };
    }
}

#endif // TradePackets_h__
