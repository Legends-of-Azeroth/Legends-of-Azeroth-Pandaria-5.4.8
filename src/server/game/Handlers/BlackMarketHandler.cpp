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

#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "Unit.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "BlackMarketMgr.h"

void WorldSession::HandleBlackMarketHelloOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACK_MARKET_OPEN");

    ObjectGuid guid;

    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);

    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendBlackMarketHello(guid, sBlackMarketMgr->isBlackMarketOpen());
}

void WorldSession::SendBlackMarketHello(ObjectGuid guid, bool open)
{
    WorldPacket data(SMSG_BLACK_MARKET_OPEN_RESULT, 9);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(open);
    data.FlushBits();
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    SendPacket(&data);
}

void WorldSession::HandleBlackMarketRequestItemOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACK_MARKET_REQUEST_ITEMS");

    ObjectGuid guid;
    uint32 timestamp = recvData.read<uint32>();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[0]);

    SendBlackMarketRequestItemsResult();
}

void WorldSession::SendBlackMarketRequestItemsResult()
{
    WorldPacket data(SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT);
    sBlackMarketMgr->BuildBlackMarketRequestItemsResult(data, GetPlayer()->GetGUID().GetCounter());
    SendPacket(&data);
}

void WorldSession::HandleBlackMarketBidOnItem(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACK_MARKET_BID_ON_ITEM");

    ObjectGuid guid;
    uint32 itemId = recvData.read<uint32>();
    uint32 auctionId = recvData.read<uint32>();
    uint64 bidAmount = recvData.read<uint64>();
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);

    TC_LOG_DEBUG("blackMarket", ">> HandleBlackMarketBid >> MarketID : {}, BidAmount : " "{}" ", ItemID : {}", auctionId, bidAmount, itemId);

    if (!bidAmount)
        return;

    BlackMarketAuction* auction = sBlackMarketMgr->GetAuction(auctionId);
    if (!auction)
    {
        TC_LOG_DEBUG("blackMarket", "HandleBlackMarketBid - Auction (MarketID: {}) not found.", auctionId);
        SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_ITEM_NOT_FOUND);
        return;
    }

    if (auction->GetCurrentBidder() == GetPlayer()->GetGUID().GetCounter())
    {
        TC_LOG_DEBUG("blackMarket", "HandleBlackMarketBid - Player (GUID: {}) is already the highest bidder.", GetPlayer()->GetGUID().GetCounter());
        SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_ALREADY_BID);
        return;
    }

    if (auction->GetCurrentBid() > bidAmount && bidAmount != auction->GetTemplate()->MinBid)
    {
        TC_LOG_DEBUG("blackMarket", "HandleBlackMarketBid - Player (GUID: {}) could not bid. The current bid (" "{}" ") is higher than the given amount (" "{}" ").", GetPlayer()->GetGUID().GetCounter(), auction->GetCurrentBid(), bidAmount);
        SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_HIGHER_BID);
        return;
    }

    uint64 currentRequiredIncrement = !auction->GetNumBids() ? auction->GetCurrentBid() : (auction->GetCurrentBid() + auction->GetMinIncrement());
    if (currentRequiredIncrement > bidAmount)
    {
        TC_LOG_DEBUG("blackMarket", "HandleBlackMarketBid - Player (GUID: {}) could not bid. The BidAmount (" "{}" ") is lower than the current requiredIncrement (" "{}" ").", GetPlayer()->GetGUID().GetCounter(), bidAmount, currentRequiredIncrement);
        SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_DATABASE_ERROR);
        return;
    }

    uint64 newIncrement = bidAmount - currentRequiredIncrement;
    if (!GetPlayer()->HasEnoughMoney(bidAmount))
    {
        TC_LOG_DEBUG("blackMarket", "HandleBlackMarketBid - Player (GUID: {}) has not enough money to bid.", GetPlayer()->GetGUID().GetCounter());
        SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_NOT_ENOUGH_MONEY);
        return;
    }
    else
    {
        GetPlayer()->ModifyMoney(-int64(bidAmount));
    }

    sBlackMarketMgr->UpdateAuction(auction, currentRequiredIncrement, newIncrement, GetPlayer());

    SendBlackMarketBidOnItemResult(auctionId, itemId, ERR_BMAH_OK);
    SendBlackMarketHello(guid, sBlackMarketMgr->isBlackMarketOpen());
}

void WorldSession::SendBlackMarketBidOnItemResult(uint32 auctionId, uint32 itemID, BlackMarketError result)
{
    WorldPacket data(SMSG_BLACK_MARKET_BID_ON_ITEM_RESULT, 12);
    data << uint32(itemID);
    data << uint32(result);
    data << uint32(auctionId);
    SendPacket(&data);
}
