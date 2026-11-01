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

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include <list>
#include <vector>
#include <utility>

void WorldSession::SendVoidStorageTransferResult(VoidTransferError result)
{
    WorldPacket data(SMSG_VOID_TRANSFER_RESULT, 4);
    data << uint32(result);
    SendPacket(&data);
}

void WorldSession::HandleVoidStorageUnlock(WorldPackets::VoidStorage::VoidStorageUnlock& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_UNLOCK_VOID_STORAGE");
    Player* player = GetPlayer();

    Creature* unit = player->GetNPCIfCanInteractWith(packet.NpcGuid, UNIT_NPC_FLAG_VAULTKEEPER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageUnlock - Unit (GUID: {}) not found or player can't interact with it.", packet.NpcGuid.GetCounter());
        return;
    }

    if (player->IsVoidStorageUnlocked())
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageUnlock - Player (GUID: {}, name: {}) tried to unlock void storage a 2nd time.", player->GetGUID().GetCounter(), player->GetName().c_str());
        return;
    }

    player->ModifyMoney(-int64(VOID_STORAGE_UNLOCK));
    player->UnlockVoidStorage();
}

void WorldSession::HandleVoidStorageQuery(WorldPackets::VoidStorage::VoidStorageQuery& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_QUERY_VOID_STORAGE");
    Player* player = GetPlayer();

    Creature* unit = player->GetNPCIfCanInteractWith(packet.NpcGuid, UNIT_NPC_FLAG_TRANSMOGRIFIER | UNIT_NPC_FLAG_VAULTKEEPER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageQuery - Unit (GUID: {}) not found or player can't interact with it.", packet.NpcGuid.GetCounter());
        return;
    }

    if (!player->IsVoidStorageUnlocked())
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageQuery - Player (GUID: {}, name: {}) queried void storage without unlocking it.", player->GetGUID().GetCounter(), player->GetName().c_str());
        return;
    }

    uint8 count = 0;
    for (uint8 i = 0; i < VOID_STORAGE_MAX_SLOT; ++i)
        if (player->GetVoidStorageItem(i))
            ++count;

    WorldPacket data(SMSG_VOID_STORAGE_CONTENTS);
    ByteBuffer itemData;

    data.WriteBits(count, 7);

    for (uint8 i = 0; i < VOID_STORAGE_MAX_SLOT; ++i)
    {
        VoidStorageItem* item = player->GetVoidStorageItem(i);
        if (!item)
            continue;

        ObjectGuid itemId(item->ItemId);
        ObjectGuid creatorGuid = item->CreatorGuid;

        data.WriteBit(creatorGuid[1]);
        data.WriteBit(creatorGuid[3]);
        data.WriteBit(itemId[1]);
        data.WriteBit(creatorGuid[2]);
        data.WriteBit(itemId[2]);
        data.WriteBit(creatorGuid[5]);
        data.WriteBit(creatorGuid[0]);
        data.WriteBit(itemId[6]);
        data.WriteBit(itemId[5]);
        data.WriteBit(creatorGuid[4]);
        data.WriteBit(itemId[7]);
        data.WriteBit(itemId[3]);
        data.WriteBit(itemId[4]);
        data.WriteBit(itemId[0]);
        data.WriteBit(creatorGuid[6]);
        data.WriteBit(creatorGuid[7]);

        itemData.WriteByteSeq(creatorGuid[4]);
        itemData.WriteByteSeq(creatorGuid[7]);
        itemData.WriteByteSeq(itemId[6]);
        itemData.WriteByteSeq(creatorGuid[6]);
        itemData.WriteByteSeq(itemId[2]);

        itemData << uint32(item->ItemUpgradeId);

        itemData.WriteByteSeq(itemId[7]);
        itemData.WriteByteSeq(itemId[3]);
        itemData.WriteByteSeq(creatorGuid[0]);

        itemData << uint32(item->ItemEntry);

        itemData.WriteByteSeq(itemId[0]);

        itemData << uint32(item->ItemRandomPropertyId);

        itemData.WriteByteSeq(creatorGuid[2]);
        itemData.WriteByteSeq(creatorGuid[5]);
        itemData.WriteByteSeq(creatorGuid[3]);

        itemData << uint32(i);

        itemData.WriteByteSeq(itemId[5]);
        itemData.WriteByteSeq(itemId[1]);

        itemData << uint32(item->ItemSuffixFactor);

        itemData.WriteByteSeq(itemId[4]);
        itemData.WriteByteSeq(creatorGuid[1]);
    }

    data.FlushBits();
    data.append(itemData);

    SendPacket(&data);
}

void WorldSession::HandleVoidStorageTransfer(WorldPackets::VoidStorage::VoidStorageTransfer& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_VOID_STORAGE_TRANSFER");
    Player* player = GetPlayer();

    if (packet.CountDeposit > 9)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) wants to deposit more than 9 items ({}).", player->GetGUID().GetCounter(), player->GetName().c_str(), packet.CountDeposit);
        return;
    }

    if (packet.CountWithdraw > 9)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) wants to withdraw more than 9 items ({}).", player->GetGUID().GetCounter(), player->GetName().c_str(), packet.CountWithdraw);
        return;
    }

    Creature* unit = player->GetNPCIfCanInteractWith(packet.NpcGuid, UNIT_NPC_FLAG_VAULTKEEPER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Unit (GUID: {}) not found or player can't interact with it.", packet.NpcGuid.GetCounter());
        return;
    }

    if (!player->IsVoidStorageUnlocked())
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) queried void storage without unlocking it.", player->GetGUID().GetCounter(), player->GetName().c_str());
        return;
    }

    if (packet.ItemGuids.size() > player->GetNumOfVoidStorageFreeSlots())
    {
        SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_FULL);
        return;
    }

    uint32 freeBagSlots = 0;
    if (packet.ItemIds.size() != 0)
    {
        // make this a Player function
        for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
            if (Bag* bag = player->GetBagByPos(i))
                freeBagSlots += bag->GetFreeSlots();
        for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
            if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                ++freeBagSlots;
    }

    if (packet.ItemIds.size() > freeBagSlots)
    {
        SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_INVENTORY_FULL);
        return;
    }

    if (!player->HasEnoughMoney(uint64(packet.ItemGuids.size() * VOID_STORAGE_STORE_ITEM)))
    {
        SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_NOT_ENOUGH_MONEY);
        return;
    }

    std::pair<VoidStorageItem, uint8> depositItems[VOID_STORAGE_MAX_DEPOSIT];
    uint8 depositCount = 0;
    for (std::vector<ObjectGuid>::iterator itr = packet.ItemGuids.begin(); itr != packet.ItemGuids.end(); ++itr)
    {
        Item* item = player->GetItemByGuid(*itr);
        if (!item)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) wants to deposit an invalid item (item guid: " "{}" ").", player->GetGUID().GetCounter(), player->GetName().c_str(), uint64(*itr));
            continue;
        }

        VoidStorageItem itemVS(sObjectMgr->GenerateVoidStorageItemId(), item->GetEntry(), item->GetGuidValue(ITEM_FIELD_CREATOR), item->GetItemRandomPropertyId(), item->GetItemSuffixFactor(), item->GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_UPGRADE));

        uint8 slot = player->AddVoidStorageItem(itemVS);

        depositItems[depositCount++] = std::make_pair(itemVS, slot);

        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
    }

    int64 cost = depositCount * VOID_STORAGE_STORE_ITEM;

    player->ModifyMoney(-cost);

    VoidStorageItem withdrawItems[VOID_STORAGE_MAX_WITHDRAW];
    uint8 withdrawCount = 0;
    for (std::vector<ObjectGuid>::iterator itr = packet.ItemIds.begin(); itr != packet.ItemIds.end(); ++itr)
    {
        uint8 slot;
        VoidStorageItem* itemVS = player->GetVoidStorageItem(*itr, slot);
        if (!itemVS)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) tried to withdraw an invalid item (id: " "{}" ")", player->GetGUID().GetCounter(), player->GetName().c_str(), uint64(*itr));
            continue;
        }

        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemVS->ItemEntry, 1);
        if (msg != EQUIP_ERR_OK)
        {
            SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_INVENTORY_FULL);
            TC_LOG_DEBUG("network", "WORLD: HandleVoidStorageTransfer - Player (GUID: {}, name: {}) couldn't withdraw item id " "{}" " because inventory was full.", player->GetGUID().GetCounter(), player->GetName().c_str(), uint64(*itr));
            return;
        }

        Item* item = player->StoreNewItem(dest, itemVS->ItemEntry, true, itemVS->ItemRandomPropertyId);
        item->SetGuidValue(ITEM_FIELD_CREATOR, itemVS->CreatorGuid);
        item->SetDynamicModifier(ITEM_MODIFIER_INDEX_UPGRADE, itemVS->ItemUpgradeId, nullptr, true);
        item->SetBinding(true);
        player->SendNewItem(item, 1, false, false, false);

        withdrawItems[withdrawCount++] = *itemVS;

        player->DeleteVoidStorageItem(slot);
    }

    WorldPacket data(SMSG_VOID_STORAGE_TRANSFER_CHANGES, ((5 + 5 + (7 + 7) * depositCount +
        7 * withdrawCount) / 8) + 7 * withdrawCount + (7 + 7 + 4 * 4) * depositCount);

    data.WriteBits(withdrawCount, 4);

    for (uint8 i = 0; i < withdrawCount; ++i)
    {
        ObjectGuid itemId(withdrawItems[i].ItemId);

        data.WriteBit(itemId[1]);
        data.WriteBit(itemId[6]);
        data.WriteBit(itemId[7]);
        data.WriteBit(itemId[3]);
        data.WriteBit(itemId[2]);
        data.WriteBit(itemId[0]);
        data.WriteBit(itemId[4]);
        data.WriteBit(itemId[5]);
    }

    data.WriteBits(depositCount, 4);

    for (uint8 i = 0; i < depositCount; ++i)
    {
        ObjectGuid itemId(depositItems[i].first.ItemId);
        ObjectGuid creatorGuid = depositItems[i].first.CreatorGuid;

        data.WriteBit(itemId[0]);
        data.WriteBit(creatorGuid[6]);
        data.WriteBit(creatorGuid[4]);
        data.WriteBit(itemId[3]);
        data.WriteBit(creatorGuid[3]);
        data.WriteBit(itemId[5]);
        data.WriteBit(itemId[7]);
        data.WriteBit(creatorGuid[0]);
        data.WriteBit(creatorGuid[5]);
        data.WriteBit(creatorGuid[7]);
        data.WriteBit(itemId[6]);
        data.WriteBit(itemId[4]);
        data.WriteBit(creatorGuid[1]);
        data.WriteBit(itemId[1]);
        data.WriteBit(creatorGuid[2]);
        data.WriteBit(itemId[2]);
    }

    data.FlushBits();

    for (uint8 i = 0; i < depositCount; ++i)
    {
        ObjectGuid itemId(depositItems[i].first.ItemId);
        ObjectGuid creatorGuid = depositItems[i].first.CreatorGuid;

        data << uint32(depositItems[i].second); // slot

        data.WriteByteSeq(creatorGuid[5]);

        data << uint32(depositItems[i].first.ItemEntry);

        data.WriteByteSeq(creatorGuid[6]);
        data.WriteByteSeq(creatorGuid[3]);

        data << uint32(depositItems[i].first.ItemSuffixFactor);

        data.WriteByteSeq(creatorGuid[2]);
        data.WriteByteSeq(itemId[5]);

        data << uint32(depositItems[i].first.ItemRandomPropertyId);

        data.WriteByteSeq(itemId[3]);
        data.WriteByteSeq(creatorGuid[7]);
        data.WriteByteSeq(creatorGuid[4]);
        data.WriteByteSeq(creatorGuid[1]);
        data.WriteByteSeq(itemId[0]);
        data.WriteByteSeq(itemId[4]);
        data.WriteByteSeq(itemId[6]);

        data << uint32(depositItems[i].first.ItemUpgradeId);

        data.WriteByteSeq(itemId[1]);
        data.WriteByteSeq(itemId[2]);
        data.WriteByteSeq(creatorGuid[0]);
        data.WriteByteSeq(itemId[7]);
    }

    for (uint8 i = 0; i < withdrawCount; ++i)
    {
        ObjectGuid itemId(withdrawItems[i].ItemId);

        data.WriteByteSeq(itemId[7]);
        data.WriteByteSeq(itemId[3]);
        data.WriteByteSeq(itemId[1]);
        data.WriteByteSeq(itemId[5]);
        data.WriteByteSeq(itemId[4]);
        data.WriteByteSeq(itemId[0]);
        data.WriteByteSeq(itemId[6]);
        data.WriteByteSeq(itemId[2]);
    }

    SendPacket(&data);

    SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_NO_ERROR);
}

void WorldSession::HandleVoidSwapItem(WorldPackets::VoidStorage::VoidSwapItem& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_SWAP_VOID_ITEM");

    Player* player = GetPlayer();

    Creature* unit = player->GetNPCIfCanInteractWith(packet.NpcGuid, UNIT_NPC_FLAG_VAULTKEEPER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidSwapItem - Unit (GUID: {}) not found or player can't interact with it.", packet.NpcGuid.GetCounter());
        return;
    }

    if (!player->IsVoidStorageUnlocked())
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidSwapItem - Player (GUID: {}, name: {}) queried void storage without unlocking it.", player->GetGUID().GetCounter(), player->GetName().c_str());
        return;
    }

    uint8 oldSlot;
    if (!player->GetVoidStorageItem(packet.ItemId, oldSlot))
    {
        TC_LOG_DEBUG("network", "WORLD: HandleVoidSwapItem - Player (GUID: {}, name: {}) requested swapping an invalid item (slot: {}, itemid: " "{}" ").", player->GetGUID().GetCounter(), player->GetName().c_str(), packet.NewSlot, uint64(packet.ItemId));
        return;
    }

    bool usedSrcSlot = player->GetVoidStorageItem(oldSlot) != NULL; // should be always true
    bool usedDestSlot = player->GetVoidStorageItem(packet.NewSlot) != NULL;
    ObjectGuid itemIdDest;
    if (usedDestSlot)
        itemIdDest.Set(player->GetVoidStorageItem(packet.NewSlot)->ItemId);

    if (!player->SwapVoidStorageItem(oldSlot, packet.NewSlot))
    {
        SendVoidStorageTransferResult(VOID_TRANSFER_ERROR_INTERNAL_ERROR_1);
        return;
    }

    WorldPacket data(SMSG_VOID_ITEM_SWAP_RESPONSE, 1 + (usedSrcSlot + usedDestSlot) * (1 + 7 + 4));

    data.WriteBit(!usedSrcSlot);

    data.WriteBit(packet.ItemId[4]);
    data.WriteBit(packet.ItemId[1]);
    data.WriteBit(packet.ItemId[6]);
    data.WriteBit(packet.ItemId[0]);
    data.WriteBit(packet.ItemId[3]);
    data.WriteBit(packet.ItemId[7]);
    data.WriteBit(packet.ItemId[2]);
    data.WriteBit(packet.ItemId[5]);

    data.WriteBit(!usedDestSlot);

    data.WriteBit(itemIdDest[6]);
    data.WriteBit(itemIdDest[0]);
    data.WriteBit(itemIdDest[3]);
    data.WriteBit(itemIdDest[2]);
    data.WriteBit(itemIdDest[1]);
    data.WriteBit(itemIdDest[5]);
    data.WriteBit(itemIdDest[7]);
    data.WriteBit(itemIdDest[4]);

    data.WriteBit(!usedDestSlot);
    data.WriteBit(!usedSrcSlot);

    data.FlushBits();

    data.WriteByteSeq(itemIdDest[3]);
    data.WriteByteSeq(itemIdDest[7]);
    data.WriteByteSeq(itemIdDest[2]);
    data.WriteByteSeq(itemIdDest[5]);
    data.WriteByteSeq(itemIdDest[0]);
    data.WriteByteSeq(itemIdDest[1]);
    data.WriteByteSeq(itemIdDest[4]);
    data.WriteByteSeq(itemIdDest[6]);
    data.WriteByteSeq(packet.ItemId[0]);
    data.WriteByteSeq(packet.ItemId[2]);
    data.WriteByteSeq(packet.ItemId[7]);
    data.WriteByteSeq(packet.ItemId[5]);
    data.WriteByteSeq(packet.ItemId[6]);
    data.WriteByteSeq(packet.ItemId[4]);
    data.WriteByteSeq(packet.ItemId[3]);
    data.WriteByteSeq(packet.ItemId[1]);

    if (usedDestSlot)
        data << uint32(oldSlot);

    if (usedSrcSlot)
        data << uint32(packet.NewSlot);

    SendPacket(&data);
}
