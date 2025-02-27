/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "BlackMarketMgr.h"
#include "Item.h"
#include "Log.h"
#include "Random.h"
#include <vector>

BlackMarketMgr::~BlackMarketMgr()
{
    for (auto&& itr : _auctions)
        delete itr.second;

    for (auto&& itr : _templates)
        delete itr.second;
}

BlackMarketMgr::BlackMarketMgr() { }

BlackMarketMgr* BlackMarketMgr::instance()
{
    static BlackMarketMgr instance;
    return &instance;
}

BlackMarketAuctionTemplate* BlackMarketAuction::GetTemplate() const
{
    if (BlackMarketAuctionTemplate* bmTemplate = sBlackMarketMgr->GetTemplate(GetTemplateId()))
        return bmTemplate;

    return nullptr;
}

void BlackMarketAuction::SaveToDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BLACKMARKET_AUCTION);
    stmt->setUInt32(0, GetAuctionId());
    stmt->setUInt32(1, GetTemplateId());
    stmt->setUInt32(2, GetStartTime());
    stmt->setUInt32(3, GetEndTime());
    stmt->setUInt32(4, GetCurrentBidder());
    stmt->setUInt64(5, GetCurrentBid());
    stmt->setUInt64(6, GetMinIncrement());
    stmt->setUInt32(7, GetNumBids());
    trans->Append(stmt);
}

void BlackMarketAuction::DeleteFromDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BLACKMARKET_AUCTION);
    stmt->setUInt32(0, GetAuctionId());
    trans->Append(stmt);
}

void BlackMarketAuction::UpdateToDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_BLACKMARKET_AUCTION);
    stmt->setUInt32(0, GetCurrentBidder());
    stmt->setUInt64(1, GetCurrentBid());
    stmt->setUInt64(2, GetMinIncrement());
    stmt->setUInt32(3, GetNumBids());
    stmt->setUInt32(4, GetAuctionId());
    trans->Append(stmt);
}

uint32 BlackMarketAuction::TimeLeft()
{
    uint32 endTime = GetEndTime();
    uint32 curTime = time(nullptr);
    return (endTime >= curTime) ? endTime - curTime : 0;
}

void BlackMarketMgr::LoadTemplates()
{
    uint32 count = 0;
    uint32 oldMSTime = getMSTime();

    if (PreparedQueryResult result = WorldDatabase.Query(WorldDatabase.GetPreparedStatement(WORLD_SEL_BLACKMARKET_TEMPLATE)))
    {
        do
        {
            Field* fields = result->Fetch();

            BlackMarketAuctionTemplate* blackmarket_template = new BlackMarketAuctionTemplate();

            blackmarket_template->Id             = fields[0].GetUInt32();
            blackmarket_template->MarketId       = fields[1].GetUInt32();
            blackmarket_template->SellerNPCEntry = fields[2].GetUInt32();
            blackmarket_template->ItemEntry      = fields[3].GetUInt32();
            blackmarket_template->Quantity       = fields[4].GetUInt32();
            blackmarket_template->MinBid         = fields[5].GetUInt32();
            blackmarket_template->Chance         = fields[6].GetFloat();

            blackmarket_template->Duration       = urand(7200, 172800);
            blackmarket_template->RandomStats    = Item::GenerateItemRandomPropertyId(blackmarket_template->ItemEntry);

            _templates[blackmarket_template->Id] = blackmarket_template;

            ++count;
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u BlackMarket templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BlackMarketMgr::LoadAuctions()
{
    uint32 count = 0;
    uint32 oldMSTime = getMSTime();

    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_BLACKMARKET_AUCTIONS)))
    {
        _lastUpdate = time(nullptr); // Set update time before loading

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        do
        {
            Field* fields = result->Fetch();

            uint32 auctionId     = fields[0].GetUInt32();
            uint32 templateId    = fields[1].GetUInt32();
            uint32 startTime     = fields[2].GetUInt32();
            uint32 endTime       = fields[3].GetUInt32();
            uint32 currentBidder = fields[4].GetUInt32();
            uint64 currentBid    = fields[5].GetUInt64();
            uint64 minIncrement  = fields[6].GetUInt64();
            uint32 numBids       = fields[7].GetUInt32();

            BlackMarketAuction* auction = new BlackMarketAuction(auctionId, templateId, startTime, endTime, currentBidder, currentBid, minIncrement, numBids);

            if (!auction->GetTemplate())
            {
                auction->DeleteFromDB(trans);
                delete auction;
                continue;
            }

            _auctions[auction->GetAuctionId()] = auction;

            ++count;

        } while (result->NextRow());

        CharacterDatabase.CommitTransaction(trans);
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u BlackMarket Auctions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BlackMarketMgr::Update()
{
    uint32 curTime = time(nullptr);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    // Delete expired auctions
    for (auto&& itr : _auctions)
    {
        BlackMarketAuction* auction = itr.second;
        if (auction->IsExpired())
        {
            if (auction->GetCurrentBidder())
                SendAuctionWon(auction, trans);

            auction->DeleteFromDB(trans);
            _auctions.erase(itr.first);
        }
    }

    // Add New Auctions
    int32 add = sWorld->getIntConfig(CONFIG_BLACK_MARKET_MAX_AUCTIONS) - _auctions.size();
    if (add > 0)
        CreateAuctions(add, trans);

    _lastUpdate = curTime;

    CharacterDatabase.CommitTransaction(trans);
}

uint32 BlackMarketMgr::GetFreeAuctionId()
{
    uint32 newId = 1;
    while (GetAuction(newId))
        ++newId;
    return newId;
}

void BlackMarketMgr::CreateAuctions(uint32 number, CharacterDatabaseTransaction trans)
{
    if (_templates.empty())
        return;

    for (uint32 i = 0; i < number; ++i)
    {
        // Select a template
        std::vector<uint32> templateList;

        for (auto&& itr : _templates)
            if (roll_chance_i(itr.second->Chance))
                templateList.push_back(itr.first);

        for (auto&& itr : _auctions)
            templateList.erase(std::remove(templateList.begin(), templateList.end(), itr.second->GetTemplateId()), templateList.end());

        if (templateList.empty())
            continue;

        BlackMarketAuctionTemplate* selTemplate = GetTemplate(templateList[urand(0, templateList.size() - 1)]);
        if (!selTemplate)
            continue;

        uint32 startTime = time(nullptr) + sWorld->getIntConfig(CONFIG_BLACK_MARKET_AUCTION_DELAY) + urand(0, sWorld->getIntConfig(CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD) * 2) - sWorld->getIntConfig(CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD) / 2;

        uint32 minIncrement = CalculatePct((selTemplate->MinBid / 10000), 5) * 10000;
        if (!minIncrement)
            minIncrement = 10000;

        BlackMarketAuction* auction = new BlackMarketAuction;
        auction->SetAuctionId(GetFreeAuctionId());
        auction->SetCurrentBid(selTemplate->MinBid);
        auction->SetCurrentBidder(0);
        auction->SetNumBids(0);
        auction->SetMinIncrement(minIncrement);
        auction->SetStartTime(startTime);
        auction->SetEndTime(startTime + selTemplate->Duration);
        auction->SetTemplateId(selTemplate->Id);

        _auctions[auction->GetAuctionId()] = auction;

        auction->SaveToDB(trans);
    }
}

void BlackMarketMgr::BuildBlackMarketRequestItemsResult(WorldPacket& data, uint32 guidLow)
{
    uint32 count = 0;
    ByteBuffer auctionData;

    data << uint32(GetLastUpdate());                        // LastUpdateID
    size_t count_pos = data.bitwpos();
    data.WriteBits(count, 18);                              // ItemCount Placeholder

    for (auto&& itr : _auctions)
    {
        BlackMarketAuction* auction = itr.second;
        if (!auction->IsActive())
            continue;

        data.WriteBit(guidLow == auction->GetCurrentBidder());

        auctionData << int32(auction->GetTemplate()->RandomStats);     // RandomProperty or RandomSuffix
        auctionData << uint32(auction->GetTemplate()->Quantity);       // Quantity
        auctionData << uint32(auction->GetAuctionId());                // AuctionId
        auctionData << uint32(auction->GetTemplate()->MarketId);       // MarketId
        auctionData << uint32(auction->GetTemplate()->ItemEntry);      // ItemEntry

        uint64 currentBid = 0, minBid = 0, minIncrement = 0;
        if (!auction->GetNumBids())
        {
            currentBid = 0;
            minBid = auction->GetTemplate()->MinBid;
            minIncrement = 1;
        }
        else
        {
            currentBid = auction->GetCurrentBid();
            minIncrement = auction->GetMinIncrement(); // 5% increment minimum
            minBid = auction->GetCurrentBid() + auction->GetMinIncrement();
        }

        auctionData << uint64(minBid);                                 // MinBid
        auctionData << uint64(currentBid);                             // CurrentBid
        auctionData << uint64(minIncrement);                           // MinIncrement
        auctionData << uint32(auction->GetTemplate()->SellerNPCEntry); // SellerNPC
        auctionData << uint32(auction->TimeLeft());                    // SecondsRemaining

        ++count;
    }

    data.FlushBits();
    data.append(auctionData);
    data.PutBits(count_pos, count, 18);

    TC_LOG_DEBUG("network", ">> Sent %u Black Market Auctions", count);
}

void BlackMarketMgr::UpdateAuction(BlackMarketAuction* auction, uint64 newPrice, uint64 requiredIncrement, Player* newBidder)
{
    uint64 currentBid = newPrice + requiredIncrement;
    uint64 minIncrement = CalculatePct((currentBid / 10000), 5) * 10000;
    if (!minIncrement)
        minIncrement = 10000;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    if (auction->GetCurrentBidder())
        SendAuctionOutbidded(auction, newBidder, trans);

    auction->SetCurrentBid(currentBid);
    auction->SetCurrentBidder(newBidder->GetGUID().GetCounter());
    auction->SetMinIncrement(minIncrement);
    auction->SetNumBids(auction->GetNumBids() + 1);

    auction->UpdateToDB(trans);

    CharacterDatabase.CommitTransaction(trans);
}

BlackMarketAuctionTemplate* BlackMarketMgr::GetTemplate(uint32 templateId) const
{
    for (auto&& itr : _templates)
        if (itr.second->Id == templateId)
            return itr.second;

    TC_LOG_DEBUG("blackMarket", "BlackMarketMgr::GetTemplate: [%u] not found!", templateId);
    return nullptr;
}

BlackMarketAuction* BlackMarketMgr::GetAuction(uint32 auctionId) const
{
    for (auto&& itr : _auctions)
        if (itr.second->GetAuctionId() == auctionId)
            return itr.second;

    TC_LOG_DEBUG("blackMarket", "BlackMarketMgr::GetAuction: [%u] not found!", auctionId);
    return nullptr;
}

std::string BlackMarketAuction::BuildAuctionMailSubject(BMMailAuctionAnswers response)
{
    std::ostringstream strm;
    strm << GetTemplate()->ItemEntry << ":0:" << response << ':' << GetAuctionId() << ':' << GetTemplate()->Quantity;
    return strm.str();
}

std::string BlackMarketAuction::BuildAuctionMailBody(ObjectGuid::LowType lowGuid)
{
    std::ostringstream strm;
    strm.width(16);
    strm << std::right << std::hex << ObjectGuid(HighGuid::Player, lowGuid); // HighGuid::Player always present, even for empty guids
    strm << std::dec << ':' << GetCurrentBid() << ':' << 0;
    strm << ':' << 0 << ':' << 0;
    return strm.str();
}

void BlackMarketMgr::SendAuctionOutbidded(BlackMarketAuction* auction, Player* newBidder, CharacterDatabaseTransaction trans)
{
    ObjectGuid bidder_guid(HighGuid::Player, auction->GetCurrentBidder());
    Player* bidder = ObjectAccessor::FindConnectedPlayer(bidder_guid);
    uint32 oldBidder_accId = sObjectMgr->GetPlayerAccountIdByGUID(bidder_guid);

    // old bidder exist
    if (!bidder && !oldBidder_accId)
        return;

    if (bidder)
    {
        WorldPacket data(SMSG_BLACK_MARKET_OUTBID, 12);
        data << uint32(auction->GetTemplate()->ItemEntry);
        data << uint32(1);
        data << uint32(1);
        bidder->GetSession()->SendPacket(&data);
    }

    MailDraft(auction->BuildAuctionMailSubject(BM_AUCTION_OUTBIDDED), auction->BuildAuctionMailBody(auction->GetTemplate()->SellerNPCEntry))
        .AddMoney(auction->GetCurrentBid())
        .SendMailTo(trans, MailReceiver(bidder, auction->GetCurrentBidder()), auction, MAIL_CHECK_MASK_COPIED);
}

void BlackMarketMgr::SendAuctionWon(BlackMarketAuction* auction, CharacterDatabaseTransaction trans)
{
    ObjectGuid bidder_guid(HighGuid::Player, auction->GetCurrentBidder());
    Player* bidder = ObjectAccessor::FindConnectedPlayer(bidder_guid);

    if (bidder)
    {
        WorldPacket data(SMSG_BLACK_MARKET_WON, 12);
        data << uint32(1);                                  // 6 - might be OK - 6 (win msg recvd)
        data << uint32(auction->GetTemplate()->ItemEntry);  // 4 - ItemEntry
        data << uint32(1);                                  // 5 - might be OK - 5 (win msg recvd)
        bidder->GetSession()->SendPacket(&data);
    }

    ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(auction->GetTemplate()->ItemEntry);
    if (!itemTemplate)
        return;

    Item* item = Item::CreateItem(auction->GetTemplate()->ItemEntry, auction->GetTemplate()->Quantity);
    if (!item)
        return;

    if (int32 randomPropertyId = auction->GetTemplate()->RandomStats)
        item->SetItemRandomProperties(randomPropertyId);

    item->SaveToDB(trans);

    MailDraft(auction->BuildAuctionMailSubject(BM_AUCTION_WON), auction->BuildAuctionMailBody(auction->GetCurrentBidder()))
        .AddItem(item)
        .SendMailTo(trans, MailReceiver(bidder, auction->GetCurrentBidder()), MailSender(auction), MAIL_CHECK_MASK_COPIED);
}
