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

#ifndef LootPackets_h__
#define LootPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Loot
    {
        class LootMoney final : public ClientPacket
        {
        public:
            LootMoney(WorldPacket&& packet) : ClientPacket(CMSG_LOOT_MONEY, std::move(packet)) { }

            void Read() override { }
        };

        class LootUnit final : public ClientPacket
        {
        public:
            LootUnit(WorldPacket&& packet) : ClientPacket(CMSG_LOOT, std::move(packet)) { }

            void Read() override;

            ObjectGuid Unit;
        };

        class LootRelease final : public ClientPacket
        {
        public:
            LootRelease(WorldPacket&& packet) : ClientPacket(CMSG_LOOT_RELEASE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Unit;
        };

        class LootRoll final : public ClientPacket
        {
        public:
            LootRoll(WorldPacket&& packet) : ClientPacket(CMSG_LOOT_ROLL, std::move(packet)) { }

            void Read() override;

            ObjectGuid LootObj;
            uint8 LootListID = 0;
            uint8 RollType = 0;
        };

        class LootItem final : public ClientPacket
        {
        public:
            LootItem(WorldPacket&& packet) : ClientPacket(CMSG_AUTOSTORE_LOOT_ITEM, std::move(packet)) { }

            void Read() override;

            std::vector<ObjectGuid> guids;
            std::vector<uint8> lootSlots;
        };

        class MasterLootItem final : public ClientPacket
        {
        public:
            MasterLootItem(WorldPacket&& packet) : ClientPacket(CMSG_LOOT_MASTER_GIVE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
            std::vector<ObjectGuid> lootViews;
            std::vector<uint8> slots;
        };

        class SetCurrencyFlags final : public ClientPacket
        {
        public:
            SetCurrencyFlags(WorldPacket&& packet) : ClientPacket(CMSG_SET_CURRENCY_FLAGS, std::move(packet)) { }

            void Read() override;

            uint32 Flags = 0;
            uint32 CurrencyId = 0;
        };
    }
}

#endif // LootPackets_h__
