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

#ifndef ItemPackets_h__
#define ItemPackets_h__

#include "ItemPacketsCommon.h"

namespace WorldPackets
{
    namespace Item
    {
        class SplitItem final : public ClientPacket
        {
        public:
            SplitItem(WorldPacket&& packet) : ClientPacket(CMSG_SPLIT_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 SrcBag = 0;
            uint8 SrcSlot = 0;
            uint8 DstBag = 0;
            uint8 DstSlot = 0;
            uint32 Count = 0;
        };

        class SwapInvItem final : public ClientPacket
        {
        public:
            SwapInvItem(WorldPacket&& packet) : ClientPacket(CMSG_SWAP_INV_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 SrcSlot = 0;
            uint8 DstSlot = 0;
            uint8 SrcBag = 0;
            uint8 DstBag = 0;
            uint8 CheckSrcSlot = 0;
            uint8 CheckDstSlot = 0;
            uint8 Size = 0;
            bool HasDstSlot = false;
            bool HasDstBag = false;
            bool HasSrcSlot = false;
            bool HasSrcBag = false;
        };

        class AutoEquipItemSlot final : public ClientPacket
        {
        public:
            AutoEquipItemSlot(WorldPacket&& packet) : ClientPacket(CMSG_AUTOEQUIP_ITEM_SLOT, std::move(packet)) { }

            void Read() override;

            uint8 Slot = 0;
            ObjectGuid ItemGuid;
        };

        class SwapItem final : public ClientPacket
        {
        public:
            SwapItem(WorldPacket&& packet) : ClientPacket(CMSG_SWAP_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 SrcSlotAlt = 0;
            uint8 SrcBagAlt = 0;
            uint8 DstBagAlt = 0;
            uint8 DstSlotAlt = 0;
            uint32 Count = 0;
            bool HasSlot[2] = { false, false };
            bool HasBag[2] = { false, false };
            uint8 DstBag = 0;
            uint8 DstSlot = 0;
            uint8 SrcBag = 0;
            uint8 SrcSlot = 0;
        };

        class AutoEquipItem final : public ClientPacket
        {
        public:
            AutoEquipItem(WorldPacket&& packet) : ClientPacket(CMSG_AUTOEQUIP_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 SrcSlot = 0;
            uint8 SrcBag = 0;
        };

        class DestroyItem final : public ClientPacket
        {
        public:
            DestroyItem(WorldPacket&& packet) : ClientPacket(CMSG_DESTROY_ITEM, std::move(packet)) { }

            void Read() override;

            int32 Count = 0;
            int8 Bag = 0;
            int8 Slot = 0;
        };

        class ReadItem final : public ClientPacket
        {
        public:
            ReadItem(WorldPacket&& packet) : ClientPacket(CMSG_READ_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 Bag = 0;
            uint8 Slot = 0;
        };

        class SellItem final : public ClientPacket
        {
        public:
            SellItem(WorldPacket&& packet) : ClientPacket(CMSG_SELL_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 Count = 0;
            ObjectGuid VendorGuid;
            ObjectGuid ItemGuid;
        };

        class BuyBackItem final : public ClientPacket
        {
        public:
            BuyBackItem(WorldPacket&& packet) : ClientPacket(CMSG_BUYBACK_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 Slot = 0;
            ObjectGuid VendorGuid;
        };

        class BuyItem final : public ClientPacket
        {
        public:
            BuyItem(WorldPacket&& packet) : ClientPacket(CMSG_BUY_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 BagSlot = 0;
            uint32 Count = 0;
            uint32 Item = 0;
            uint32 Slot = 0;
            uint8 ItemType = 0;
            ObjectGuid VendorGuid;
            ObjectGuid BagGuid;
        };

        class AutoStoreBagItem final : public ClientPacket
        {
        public:
            AutoStoreBagItem(WorldPacket&& packet) : ClientPacket(CMSG_AUTOSTORE_BAG_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 SrcSlot = 0;
            uint8 SrcBag = 0;
            uint8 DstBag = 0;
        };

        class WrapItem final : public ClientPacket
        {
        public:
            WrapItem(WorldPacket&& packet) : ClientPacket(CMSG_WRAP_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 Size = 0;
            bool HasGiftSlot = false;
            bool HasGiftBag = false;
            bool HasItemSlot = false;
            bool HasItemBag = false;
            uint8 GiftBag = 0;
            uint8 GiftSlot = 0;
            uint8 ItemBag = 0;
            uint8 ItemSlot = 0;
        };

        class SocketGems final : public ClientPacket
        {
        public:
            SocketGems(WorldPacket&& packet) : ClientPacket(CMSG_SOCKET_GEMS, std::move(packet)) { }

            void Read() override;

            ObjectGuid ItemGuid;
            ObjectGuid GemGuids[MAX_GEM_SOCKETS];
        };

        class GetItemPurchaseData final : public ClientPacket
        {
        public:
            GetItemPurchaseData(WorldPacket&& packet) : ClientPacket(CMSG_ITEM_REFUND_INFO, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class ItemPurchaseRefund final : public ClientPacket
        {
        public:
            ItemPurchaseRefund(WorldPacket&& packet) : ClientPacket(CMSG_ITEM_REFUND, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class ReforgeItem final : public ClientPacket
        {
        public:
            ReforgeItem(WorldPacket&& packet) : ClientPacket(CMSG_REFORGE_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 ReforgeEntry = 0;
            uint32 Bag = 0;
            uint32 Slot = 0;
            ObjectGuid Guid;
        };

        class UpgradeItem final : public ClientPacket
        {
        public:
            UpgradeItem(WorldPacket&& packet) : ClientPacket(CMSG_UPGRADE_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 ItemSlot = 0;
            uint32 ItemBag = 0;
            uint32 UpgradeEntry = 0;
            ObjectGuid NpcGuid;
            ObjectGuid ItemGuid;
        };

        class OpenItem final : public ClientPacket
        {
        public:
            OpenItem(WorldPacket&& packet) : ClientPacket(CMSG_OPEN_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 BagIndex = 0;
            uint8 Slot = 0;
        };

        class RepairItem final : public ClientPacket
        {
        public:
            RepairItem(WorldPacket&& packet) : ClientPacket(CMSG_REPAIR_ITEM, std::move(packet)) { }

            void Read() override;

            ObjectGuid ItemGuid;
            ObjectGuid NpcGuid;
            bool GuildBank = false;
        };

        class ItemTextQuery final : public ClientPacket
        {
        public:
            ItemTextQuery(WorldPacket&& packet) : ClientPacket(CMSG_ITEM_TEXT_QUERY, std::move(packet)) { }

            void Read() override;

            uint64 ItemGuid = 0;
        };

        class SetLootSpecialization final : public ClientPacket
        {
        public:
            SetLootSpecialization(WorldPacket&& packet) : ClientPacket(CMSG_SET_LOOT_SPECIALIZATION, std::move(packet)) { }

            void Read() override;

            uint32 LootSpecialization = 0;
        };
    }
}

#endif // ItemPackets_h__
