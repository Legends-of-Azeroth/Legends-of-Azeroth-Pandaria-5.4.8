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

#ifndef AuctionHousePackets_h__
#define AuctionHousePackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "ItemPacketsCommon.h"

namespace WorldPackets
{
    namespace AuctionHouse
    {
        struct AuctionItem
        {
            struct AuctionItemEnchant
            {
                AuctionItemEnchant(int32 id, uint32 expiration, int32 charges, uint8 slot) : ID(id), Expiration(expiration), Charges(charges), Slot(slot) { }
                int32 ID = 0;
                uint32 Expiration = 0;
                int32 Charges = 0;
                uint8 Slot = 0;
            };

            Item::ItemInstance Item;
            int32 Count = 0;
            int32 Charges = 0;
            std::vector<AuctionItemEnchant> Enchantments;
            int32 Flags = 0;
            int32 AuctionItemID = 0;
            ObjectGuid Owner;
            uint64 MinBid = 0;
            uint64 MinIncrement = 0;
            uint64 BuyoutPrice = 0;
            int32 DurationLeft = 0;
            uint8 DeleteReason = 0;
            bool CensorServerSideInfo = false;
            bool CensorBidInfo = false;
            ObjectGuid ItemGuid;
            ObjectGuid OwnerAccountID;
            uint32 EndTime = 0;
            ObjectGuid Bidder;
            uint64 BidAmount = 0;
        };

        class AuctionReplicateResponse;
    }
}

#include "AuctionHouseMgr.h"

namespace WorldPackets
{
    namespace AuctionHouse
    {
        class AuctionReplicateItems final : public ClientPacket
        {
        public:
            AuctionReplicateItems(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_REPLICATE_ITEMS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Auctioneer;
            uint32 ChangeNumberGlobal = 0;
            uint32 ChangeNumberCursor = 0;
            uint32 ChangeNumberTombstone = 0;
            uint32 Count = 0;
        };

        class AuctionReplicateResponse final : public ServerPacket
        {
        public:
            AuctionReplicateResponse() : ServerPacket(SMSG_AUCTION_REPLICATE_RESPONSE, 165) { }

            WorldPacket const* Write() override;

            uint32 ChangeNumberCursor = 0;
            uint32 ChangeNumberGlobal = 0;
            uint32 DesiredDelay = 0;
            uint32 ChangeNumberTombstone = 0;
            uint32 Result = 0;
            std::vector<AuctionItem> Items;
        };

        class AuctionHello final : public ClientPacket
        {
        public:
            AuctionHello(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_HELLO, std::move(packet)) { }

            void Read() override;

            ObjectGuid Auctioneer;
        };

        class AuctionSellItem final : public ClientPacket
        {
        public:
            AuctionSellItem(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_SELL_ITEM, std::move(packet)) { }

            void Read() override;

            uint64 Buyout = 0;
            uint64 Bid = 0;
            uint32 Etime = 0;
            ObjectGuid Auctioneer;
            uint32 ItemCount = 0;
            ObjectGuid ItemGuids[MAX_AUCTION_ITEMS];
            uint32 Count[MAX_AUCTION_ITEMS];
        };

        class AuctionPlaceBid final : public ClientPacket
        {
        public:
            AuctionPlaceBid(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_PLACE_BID, std::move(packet)) { }

            void Read() override;

            uint32 AuctionId = 0;
            uint64 Price = 0;
            ObjectGuid Auctioneer;
        };

        class AuctionRemoveItem final : public ClientPacket
        {
        public:
            AuctionRemoveItem(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_REMOVE_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 AuctionId = 0;
            ObjectGuid Auctioneer;
        };

        class AuctionListBidderItems final : public ClientPacket
        {
        public:
            AuctionListBidderItems(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_LIST_BIDDER_ITEMS, std::move(packet)) { }

            void Read() override;

            uint32 ListFrom = 0;
            ObjectGuid Auctioneer;
            uint8 OutbiddedCount = 0;
            uint32 OutbiddedAuctionIds[128];
        };

        class AuctionListOwnerItems final : public ClientPacket
        {
        public:
            AuctionListOwnerItems(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_LIST_OWNER_ITEMS, std::move(packet)) { }

            void Read() override;

            uint32 ListFrom = 0;
            ObjectGuid Auctioneer;
        };

        class AuctionListItems final : public ClientPacket
        {
        public:
            AuctionListItems(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_LIST_ITEMS, std::move(packet)) { }

            void Read() override;

            uint32 AuctionSlotId = 0;
            uint32 ListFrom = 0;
            uint32 AuctionMainCategory = 0;
            uint8 Unk = 0;
            uint8 LevelMax = 0;
            uint8 LevelMin = 0;
            uint32 Quality = 0;
            uint32 AuctionSubCategory = 0;
            uint32 SortCount = 0;
            std::vector<int8> SortOrder;
            ObjectGuid Auctioneer;
            bool GetAll = false;
            bool UsableItems = false;
            uint8 SearchStringLen = 0;
            std::string SearchedName;
        };

        class AuctionListPendingSales final : public ClientPacket
        {
        public:
            AuctionListPendingSales(WorldPacket&& packet) : ClientPacket(CMSG_AUCTION_LIST_PENDING_SALES, std::move(packet)) { }

            void Read() override;
        };
    }
}

#endif