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

#ifndef BattlePayPackets_h__
#define BattlePayPackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace BattlePay
    {
        class BattlePayGetProductList final : public ClientPacket
        {
        public:
            BattlePayGetProductList(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_GET_PRODUCT_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class BattlePayGetPurchaseList final : public ClientPacket
        {
        public:
            BattlePayGetPurchaseList(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_GET_PURCHASE_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class BattlePayStartPurchase final : public ClientPacket
        {
        public:
            BattlePayStartPurchase(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_START_PURCHASE, std::move(packet)) { }

            void Read() override;

            uint32 ProductId;
            uint32 ClientToken;
            ObjectGuid Guid;
        };

        class BattlePayConfirmPurchaseResponse final : public ClientPacket
        {
        public:
            BattlePayConfirmPurchaseResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint64 CurrentPrice;
            uint32 ServerToken;
            bool Confirm;
        };

        class BattlePayCharBoost final : public ClientPacket
        {
        public:
            BattlePayCharBoost(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_CHAR_BOOST, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            ObjectGuid Guid2;
            bool HasCharInfo;
            uint32 CharInfo;
        };
    }
}

#endif // BattlePayPackets_h__
