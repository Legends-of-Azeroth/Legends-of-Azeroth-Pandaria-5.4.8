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

#include "BattlePayMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "ServiceBoost.h"

void WorldSession::HandleBattlePayGetProductList(WorldPackets::BattlePay::BattlePayGetProductList& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PAY_GET_PRODUCT_LIST");

    if (!sBattlePayMgr->IsStoreEnabled())
    {
        WorldPacket data(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE);

        data.WriteBits(0, 19);
        data.WriteBits(0, 19);
        data.WriteBits(0, 20);
        data.FlushBits();

        data << uint32(BATTLE_PAY_CURRENCY_UNKNOWN);
        data << uint32(BATTLE_PAY_STATUS_UNAVAILABLE);
        SendPacket(&data);
        return;
    }

    sBattlePayMgr->SendBattlePayProductList(this);
    sBattlePayMgr->SendPointsBalance(this);
}

void WorldSession::HandleBattlePayStartPurchase(WorldPackets::BattlePay::BattlePayStartPurchase& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PAY_START_PURCHASE");

    uint32 productID = packet.ProductId;
    uint32 clientToken = packet.ClientToken;
    ObjectGuid guid = packet.Guid;

    uint32 purchaseId = irand(1, 999999); // temp solution

    sBattlePayMgr->AddQuenedPurchaseResponse(new PurchaseInfo(this, guid, purchaseId, productID, BATTLE_PAY_PURCHASE_STATUS_READY, BATTLE_PAY_RESULT_OK, clientToken, 0, false), 500, true);
}

void WorldSession::HandleBattlePayGetPurchaseList(WorldPackets::BattlePay::BattlePayGetPurchaseList& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PAY_GET_PURCHASE_LIST");

    sBattlePayMgr->SendBattlePayPurchaseList(this);
}

void WorldSession::HandleBattlePayConfirmPurchaseResponse(WorldPackets::BattlePay::BattlePayConfirmPurchaseResponse& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE");

    uint64 currentPrice = packet.CurrentPrice; // TODO: use it!
    uint32 serverToken = packet.ServerToken;
    bool confirm = packet.Confirm;

    PurchaseInfo* purchase = sBattlePayMgr->GetPurchaseInfo();
    if (confirm && serverToken == purchase->ServerToken)
        sBattlePayMgr->AddQuenedPurchaseResponse(new PurchaseInfo(this, purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_BUYING, BATTLE_PAY_RESULT_OK, purchase->ClientToken, serverToken, false), 8000, true); // do not touch timers! everything here should be as it is! decreasing timer will cause errors, for example: window with purchase close automatically and causing random client crashes
    else
        sBattlePayMgr->SendBattlePayPurchaseUpdate(new PurchaseInfo(this, purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY, BATTLE_PAY_RESULT_DECLINED, purchase->ClientToken, 0, false));
}

void WorldSession::HandleBattlePayCharBoost(WorldPackets::BattlePay::BattlePayCharBoost& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PAY_CHAR_BOOST");

    if (!HasBoost())
    {
        TC_LOG_ERROR("network", "WorldSession::HandleBattleCharBoost called, but player has not allowed boost.");
        return;
    }

    ObjectGuid guid = packet.Guid;

    if (packet.HasCharInfo)
    {
        uint32 charInfo = packet.CharInfo;

        sBattlePayMgr->SendBattlePayDistributionUpdate(this, BATTLE_PAY_SERVICE_BOOST, CHARACTER_BOOST_CHOOSED);
        GetBoost()->SetBoostedCharInfo(guid, CHARACTER_BOOST_ITEMS, (charInfo & CHARACTER_BOOST_SPEC_MASK), (charInfo & CHARACTER_BOOST_FACTION_ALLIANCE));

        WorldPacket data(SMSG_CHARACTER_UPGRADE_STARTED, 8);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[1]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[3]);
        SendPacket(&data);
    }
}
