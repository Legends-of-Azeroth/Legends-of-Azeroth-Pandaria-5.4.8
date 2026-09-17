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

#include "Common.h"
#include "Log.h"
#include "Corpse.h"
#include "Creature.h"
#include "GameObject.h"
#include "Group.h"
#include "GuildMgr.h"
#include "LootMgr.h"
#include "ObjectAccessor.h"
#include "Object.h"
#include "Opcodes.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ScriptMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#ifdef ELUNA
#include "HookMgr.h"
#endif

void WorldSession::HandleAutostoreLootItemOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_AUTOSTORE_LOOT_ITEM");
    Player* player = GetPlayer();

    Object* src = nullptr;
    AELootResult result;
    auto& lootView = GetPlayer()->GetLootView();
    AELootResult* aoeLoot = lootView.size() > 1 ? &result : nullptr;

    int32 lootCount = recvData.ReadBits(23);
    std::vector<ObjectGuid> guids;
    guids.resize(lootCount);

    for (int i = 0; i < lootCount; i++)
    {
        (guids[i])[2] = recvData.ReadBit();
        (guids[i])[7] = recvData.ReadBit();
        (guids[i])[0] = recvData.ReadBit();
        (guids[i])[6] = recvData.ReadBit();
        (guids[i])[5] = recvData.ReadBit();
        (guids[i])[3] = recvData.ReadBit();
        (guids[i])[1] = recvData.ReadBit();
        (guids[i])[4] = recvData.ReadBit();
    }

    for (int i = 0; i < lootCount; i++)
    {
        recvData.ReadByteSeq((guids[i])[0]);
        recvData.ReadByteSeq((guids[i])[4]);
        recvData.ReadByteSeq((guids[i])[1]);
        recvData.ReadByteSeq((guids[i])[7]);
        recvData.ReadByteSeq((guids[i])[6]);
        recvData.ReadByteSeq((guids[i])[5]);
        recvData.ReadByteSeq((guids[i])[3]);
        recvData.ReadByteSeq((guids[i])[2]);
        uint8 lootSlot;
        recvData >> lootSlot;

        Loot* loot = nullptr;
        auto it = lootView.find(guids[i]);
        if (it == lootView.end())
            continue;

        ObjectGuid guid = it->second;

        if (guid.IsGameObject())
        {
            GameObject* go = player->GetMap()->GetGameObject(guid);

            // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
            if (!go || ((go->GetOwnerGUID() != _player->GetGUID() && go->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !go->IsAtInteractDistance(_player)))
            {
                player->SendLootRelease(guid);
                break;
            }

            loot = &go->loot;
            src = go;
        }
        else if (guid.IsItem())
        {
            Item* pItem = player->GetItemByGuid(guid);

            if (!pItem)
            {
                player->SendLootRelease(guid);
                break;
            }

            loot = &pItem->loot;
            src = pItem;
        }
        else if (guid.IsCorpse())
        {
            Corpse* bones = ObjectAccessor::GetCorpse(*player, guid);
            if (!bones)
            {
                player->SendLootRelease(guid);
                break;
            }

            loot = &bones->loot;
        }
        else
        {
            Creature* creature = GetPlayer()->GetMap()->GetCreature(guid);

            bool lootAllowed = creature && creature->IsAlive() == ((player->GetClass() == CLASS_ROGUE || creature->GetEntry() == 56233) && creature->lootForPickPocketed);  // hack, but I dont know how it must work (pickpocket loot should be available for npc 56233)
            bool isInDistance = creature && creature->IsWithinDistInMap(player, INTERACTION_DISTANCE) || aoeLoot || player->HasAura(126746); // Glyph of Fetch
            if (!lootAllowed || !isInDistance)
            {
                player->SendLootRelease(guid);
                break;
            }

            loot = &creature->loot;
            src = creature;
        }

        if (!loot->HasLooter(_player->GetGUID()))
            break;

        player->StoreLootItem(lootSlot, loot, src, aoeLoot);

        // If player is removing the last LootItem, delete the empty container.
        if (loot->isLooted() && guid.IsItem())
            player->GetSession()->DoLootRelease(guid);
    }

    if (aoeLoot)
    {
        for (auto&& it : *aoeLoot)
        {
            player->SendNewItem(it.Item, it.Count, false, false, true);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, it.Item->GetEntry(), it.Count);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE, it.Item->GetEntry(), it.Count, it.Type);
        }
    }
}

void WorldSession::HandleLootMoneyOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_LOOT_MONEY");

    uint32 param = 0;

    Player* player = GetPlayer();

    bool shareMoney = true;
    bool isAoE = player->GetLootView().size() > 1;
    ObjectGuid containerGuid = ObjectGuid::Empty;
    for (auto&& it : player->GetLootView())
    {
        Loot* loot = nullptr;
        ObjectGuid guid = it.second;

        switch (guid.GetHigh())
        {
            case HighGuid::GameObject:
            {
                GameObject* go = GetPlayer()->GetMap()->GetGameObject(guid);

                // do not check distance for GO if player is the owner of it (ex. fishing bobber)
                if (go && ((go->GetOwnerGUID() == player->GetGUID() || go->IsAtInteractDistance(player))))
                    loot = &go->loot;

                break;
            }
            case HighGuid::Corpse:                               // remove insignia ONLY in BG
            {
                Corpse* bones = ObjectAccessor::GetCorpse(*player, guid);

                if (bones && bones->IsWithinDistInMap(player, INTERACTION_DISTANCE))
                {
                    loot = &bones->loot;
                    shareMoney = false;
                }

                break;
            }
            case HighGuid::Item:
            {
                if (Item* item = player->GetItemByGuid(guid))
                {
                    loot = &item->loot;
                    shareMoney = false;

                    param = item->GetEntry();
                }
                break;
            }
            case HighGuid::Unit:
            case HighGuid::Vehicle:
            {
                Creature* creature = player->GetMap()->GetCreature(guid);
                bool lootAllowed = creature && creature->IsAlive() == (player->GetClass() == CLASS_ROGUE && creature->lootForPickPocketed);
                bool isInDistance = creature && creature->IsWithinDistInMap(player, INTERACTION_DISTANCE) || isAoE || player->HasAura(126746); // Glyph of Fetch
                if (lootAllowed && isInDistance)
                {
                    loot = &creature->loot;
                    if (creature->IsAlive())
                        shareMoney = false;
                    param = creature->GetEntry();
                }
                break;
            }
            default:
                continue;                                         // unlootable type
        }

        if (!loot)
            continue;

        loot->NotifyMoneyRemoved();
        if (shareMoney && player->GetGroup())      //item, pickpocket and players can be looted only single player
        {
            Group* group = player->GetGroup();

            std::vector<Player*> playersNear;
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (!member)
                    continue;

                if (player->IsAtGroupRewardDistance(member))
                    playersNear.push_back(member);
            }

            uint32 goldPerPlayer = uint32((loot->gold) / (playersNear.size()));

            std::stringstream playerNames;
            bool playerNamesStarted = false;

            for (auto&& groupMember : playersNear)
            {
                groupMember->ModifyMoney(goldPerPlayer);

                groupMember->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, goldPerPlayer);

                if (Guild* guild = sGuildMgr->GetGuildById(groupMember->GetGuildId()))
                    if (uint32 guildGold = CalculatePct(goldPerPlayer, groupMember->GetTotalAuraModifier(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT)))
                        guild->HandleMemberDepositMoney(this, guildGold, true);

                WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
                data.WriteBit(playersNear.size() <= 1); // Controls the text displayed in chat. 0 is "Your share is..." and 1 is "You loot..."
                data.FlushBits();
                data << uint32(goldPerPlayer);
                groupMember->GetSession()->SendPacket(&data);

            }
        }
        else
        {
            player->ModifyMoney(loot->gold);

            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, loot->gold);

            if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
                if (uint32 guildGold = CalculatePct(loot->gold, player->GetTotalAuraModifier(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT)))
                    guild->HandleMemberDepositMoney(this, guildGold, true);

            WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
            data.WriteBit(1);   // "You loot..."
            data.FlushBits();
            data << uint32(loot->gold);
            SendPacket(&data);

        }

#ifdef ELUNA
        sHookMgr->OnLootMoney(player, loot->gold);
#endif
        loot->gold = 0;

        // Delete the money loot record from the DB
        if (loot->containerID > 0)
            loot->DeleteLootMoneyFromContainerItemDB();

        // Delete container if empty
        if (loot->isLooted() && guid.IsItem())
            containerGuid = guid;
    }

    if (containerGuid)
        player->GetSession()->DoLootRelease(containerGuid);
}

struct AELootCreatureCheck
{
    static float LootDistance;

    AELootCreatureCheck(Player* looter, uint64 mainLootTarget) : _looter(looter), _mainLootTarget(mainLootTarget) { }

    bool operator()(Creature* creature) const
    {
        if (creature->IsAlive())
            return false;

        if (creature->GetGUID() == _mainLootTarget)
            return false;

        if (!_looter->IsWithinDist(creature, LootDistance))
            return false;

        return _looter->IsAllowedToLoot(creature);
    }

    Player* _looter;
    uint64 _mainLootTarget;
};

float AELootCreatureCheck::LootDistance = 30.0f;

void WorldSession::HandleLootOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_LOOT");

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

    // Check possible cheat
    if (!_player->IsAlive())
        return;

    std::list<Creature*> corpses;
    AELootCreatureCheck check(_player, guid);
    Trinity::CreatureListSearcher<AELootCreatureCheck> searcher(_player, corpses, check);
    _player->VisitNearbyGridObject(AELootCreatureCheck::LootDistance, searcher);

    WorldPacket data{ SMSG_AE_LOOT_TARGETS, 4 };
    data << uint32(corpses.size());
    SendPacket(&data);

    GetPlayer()->SendLoot(guid, LOOT_CORPSE);

    for (auto creature : corpses)
        GetPlayer()->SendLoot(creature->GetGUID(), LOOT_CORPSE, true);

    // interrupt cast
    if (GetPlayer()->IsNonMeleeSpellCasted(false))
        GetPlayer()->InterruptNonMeleeSpells(false);
}

void WorldSession::HandleLootReleaseOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_LOOT_RELEASE");

    // cheaters can modify lguid to prevent correct apply loot release code and re-loot
    // use internal stored guid

    ObjectGuid guid;
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);

    for (auto&& it : GetPlayer()->GetLootView())
    {
        if (it.second == guid)
        {
            DoLootRelease(guid);
            break;
        }
    }
}

void WorldSession::DoLootRelease(ObjectGuid guid)
{
    Player  *player = GetPlayer();
    Loot    *loot;

    if (player->GetLootGUID() == guid)
        player->SetLootGUID(ObjectGuid::Empty);
    player->SendLootRelease(guid);

    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);

    player->RemoveLootedObject(guid);

    if (!player->IsInWorld())
        return;

    if (guid.IsGameObject())
    {
        GameObject* go = GetPlayer()->GetMap()->GetGameObject(guid);

        // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
        if (!go || ((go->GetOwnerGUID() != _player->GetGUID() && go->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !go->IsAtInteractDistance(_player)))
            return;

        loot = &go->loot;

        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR)
        {
            // locked doors are opened with spelleffect openlock, prevent remove its as looted
            go->UseDoorOrButton();
        }
        else if (loot->isLooted() || go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
        {
            if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGHOLE)
            {                                               // The fishing hole used once more
                go->AddUse();                               // if the max usage is reached, will be despawned in next tick
                if (go->GetUseCount() >= go->GetGOValue()->FishingHole.MaxOpens)
                    go->SetLootState(GO_JUST_DEACTIVATED);
                else
                    go->SetLootState(GO_READY);
            }
            else
                go->SetLootState(GO_JUST_DEACTIVATED, player);

            loot->clear();
        }
        else
        {
            // not fully looted object
            // If GO contains just quest item 
            if (loot->quest_items.size() && !loot->items.size())
                go->SetLootState(GO_READY); // Reset for next looter
            else
                go->SetLootState(GO_ACTIVATED, player);

            // if the round robin player release, reset it.
            if (player->GetGUID() == loot->roundRobinPlayer)
            {
                if (Group* group = player->GetGroup())
                {
                    if (group->GetLootMethod() != MASTER_LOOT)
                    {
                        loot->roundRobinPlayer = 0;
                    }
                }
                else
                    loot->roundRobinPlayer = 0;
            }
        }
    }
    else if (guid.IsCorpse())        // ONLY remove insignia at BG
    {
        Corpse* corpse = ObjectAccessor::GetCorpse(*player, guid);
        if (!corpse || !corpse->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        loot = &corpse->loot;

        if (loot->isLooted())
        {
            loot->clear();
            corpse->RemoveFlag(CORPSE_FIELD_DYNAMIC_FLAGS, CORPSE_DYNFLAG_LOOTABLE);
        }
    }
    else if (guid.IsItem())
    {
        Item* pItem = player->GetItemByGuid(guid);
        if (!pItem)
            return;

        ItemTemplate const* proto = pItem->GetTemplate();

        // destroy only 5 items from stack in case prospecting and milling
        if (proto->Flags & (ITEM_PROTO_FLAG_PROSPECTABLE | ITEM_PROTO_FLAG_MILLABLE))
        {
            pItem->m_lootGenerated = false;
            pItem->loot.clear();

            uint32 count = pItem->GetCount();

            // >=5 checked in spell code, but will work for cheating cases also with removing from another stacks.
            if (count > 5)
                count = 5;

            player->DestroyItemCount(pItem, count, true);
        }
        else
        {
            if (pItem->loot.isLooted() || !(proto->Flags & ITEM_FLAG_HAS_LOOT)) // Only delete item if no loot or money (unlooted loot is saved to db) + Fix Exploit 
                player->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        }
        return;                                             // item can be looted only single player
    }
    else
    {
        Creature* creature = GetPlayer()->GetMap()->GetCreature(guid);

        bool lootAllowed = creature && creature->IsAlive() == (player->GetClass() == CLASS_ROGUE && creature->lootForPickPocketed);
        if (!lootAllowed || (player->GetLootGUID() == guid && !creature->IsWithinDistInMap(_player, INTERACTION_DISTANCE)))
            return;

        loot = &creature->loot;
        if (loot->isLooted())
        {
            // skip pickpocketing loot for speed, skinning timer reduction is no-op in fact
            if (!creature->IsAlive())
                creature->AllLootRemovedFromCorpse();

            creature->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            loot->clear();
        }
        else
        {
            // if the round robin player release, reset it.
            if (player->GetGUID() == loot->roundRobinPlayer)
            {
                if (Group* group = player->GetGroup())
                {
                    if (group->GetLootMethod() != MASTER_LOOT)
                    {
                        loot->roundRobinPlayer = 0;
                        group->SendLooter(creature, NULL);

                        // force update of dynamic flags, otherwise other group's players still not able to loot.
                        creature->ForceValuesUpdateAtIndex(OBJECT_FIELD_DYNAMIC_FLAGS);
                    }
                }
                else
                    loot->roundRobinPlayer = 0;
            }
        }
    }

    //Player is not looking at loot list, he doesn't need to see updates on the loot list
    loot->RemoveLooter(player->GetGUID());
}

void WorldSession::DoLootReleaseAll()
{
    auto& lootView = GetPlayer()->GetLootView();
    while (!lootView.empty())
        DoLootRelease(lootView.begin()->second);
}

void WorldSession::HandleLootMasterGiveOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->GetGroup() || GetPlayer()->GetGroup()->GetLooterGuid() != GetPlayer()->GetGUID())
    {
        GetPlayer()->SendLootRelease(GetPlayer()->GetLootGUID());
        return;
    }

    ObjectGuid targetGuid;
    targetGuid[0] = recvData.ReadBit();
    targetGuid[2] = recvData.ReadBit();
    targetGuid[4] = recvData.ReadBit();
    targetGuid[1] = recvData.ReadBit();
    targetGuid[7] = recvData.ReadBit();
    targetGuid[6] = recvData.ReadBit();
    uint32 size = recvData.ReadBits(23);

    std::vector<ObjectGuid> lootViews;
    std::vector<uint8> slots;
    for (uint32 i = 0; i < size; ++i)
    {
        lootViews.emplace_back();
        auto& g = lootViews.back();
        g[6] = recvData.ReadBit();
        g[1] = recvData.ReadBit();
        g[7] = recvData.ReadBit();
        g[0] = recvData.ReadBit();
        g[3] = recvData.ReadBit();
        g[4] = recvData.ReadBit();
        g[5] = recvData.ReadBit();
        g[2] = recvData.ReadBit();
    }
    targetGuid[3] = recvData.ReadBit();
    targetGuid[5] = recvData.ReadBit();
    for (auto&& g : lootViews)
    {
        uint8 slot;
        recvData >> slot;
        slots.push_back(slot);
        recvData.ReadByteSeq(g[3]);
        recvData.ReadByteSeq(g[0]);
        recvData.ReadByteSeq(g[6]);
        recvData.ReadByteSeq(g[7]);
        recvData.ReadByteSeq(g[1]);
        recvData.ReadByteSeq(g[4]);
        recvData.ReadByteSeq(g[2]);
        recvData.ReadByteSeq(g[5]);
    }
    recvData.ReadByteSeq(targetGuid[4]);
    recvData.ReadByteSeq(targetGuid[5]);
    recvData.ReadByteSeq(targetGuid[2]);
    recvData.ReadByteSeq(targetGuid[3]);
    recvData.ReadByteSeq(targetGuid[6]);
    recvData.ReadByteSeq(targetGuid[0]);
    recvData.ReadByteSeq(targetGuid[7]);
    recvData.ReadByteSeq(targetGuid[1]);

    Player* target = ObjectAccessor::FindPlayer(targetGuid);
    if (!target)
        return;

    TC_LOG_DEBUG("network", "WorldSession::HandleLootMasterGiveOpcode (CMSG_LOOT_MASTER_GIVE, 0x1DE1) Target = [{}].", target->GetName().c_str());

    auto& lootView = GetPlayer()->GetLootView();

    for (size_t i = 0; i < lootViews.size(); ++i)
    {
        auto it = lootView.find(lootViews[i]);
        if (it == lootView.end())
            continue;

        Loot* loot = nullptr;

        ObjectGuid lootguid = it->second;
        WorldObject* lootedObject = nullptr;
        if (lootguid.IsCreatureOrVehicle())
        {
            Creature* creature = GetPlayer()->GetMap()->GetCreature(lootguid);
            if (!creature)
                return;

            lootedObject = creature;
            loot = &creature->loot;
        }
        else if (lootguid.IsGameObject())
        {
            GameObject* pGO = GetPlayer()->GetMap()->GetGameObject(lootguid);
            if (!pGO)
                return;

            lootedObject = pGO;
            loot = &pGO->loot;
        }

        if (!loot)
            return;

        uint8 slotId = slots[i];
        if (slots[i] >= loot->items.size() + loot->quest_items.size())
        {
            TC_LOG_DEBUG("loot", "MasterLootItem: Player {} might be using a hack! (slot {}, size {})",
                GetPlayer()->GetName().c_str(), slotId, (unsigned long)loot->items.size());
            return;
        }

        LootItem& item = slotId >= loot->items.size() ? loot->quest_items[slotId - loot->items.size()] : loot->items[slotId];

        ItemPosCountVec dest;
        InventoryResult msg = target->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item.itemid, item.count);
        if (item.follow_loot_rules && !item.AllowedForPlayer(target, lootedObject))
            msg = EQUIP_ERR_CANT_EQUIP_EVER;
        if (msg != EQUIP_ERR_OK)
        {
            target->SendEquipError(msg, NULL, NULL, item.itemid);
            // send duplicate of error massage to master looter
            _player->SendEquipError(msg, NULL, NULL, item.itemid);
            return;
        }

        // list of players allowed to receive this item in trade
        AllowedLooterSet const& looters = item.GetAllowedLooters();

        if (!looters.empty() && looters.find(target->GetGUID()) == looters.end())
        {
            // Definitely wrong error
            _player->SendEquipError(EQUIP_ERR_NOT_OWNER, NULL, NULL, item.itemid);
            return;
        }

        // not move item from loot to target inventory
        Item* newitem = target->StoreNewItem(dest, item.itemid, true, item.randomPropertyId, looters);
        if (!newitem)
        {
            target->SendEquipError(EQUIP_ERR_INTERNAL_BAG_ERROR, NULL, NULL, item.itemid);
            // send duplicate of error massage to master looter
            _player->SendEquipError(EQUIP_ERR_INTERNAL_BAG_ERROR, NULL, NULL, item.itemid);
            return;
        }
        target->SendNewItem(newitem, uint32(item.count), false, false, true);
        target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item.itemid, item.count);
        target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE, item.itemid, item.count, loot->loot_type);

        if (target->HasPendingBind())
            target->BindToInstance();

        sScriptMgr->OnItemPickup(target, newitem, loot->GetItemPickupSourceType(), loot->sourceEntry);
#ifdef ELUNA
        sHookMgr->OnLootItem(target, newitem, item.count, lootguid);
#endif
        // mark as looted
        item.count = 0;
        item.is_looted = true;

        loot->NotifyItemRemoved(slotId);
        --loot->unlootedCount;
    }
}

void WorldSession::HandleChangeCurrencyFlags(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CHARACTER: CMSG_SET_CURRENCY_FLAGS");

    uint32 currencyId, flags;

    recvPacket >> flags >> currencyId;

    if (GetPlayer())
        GetPlayer()->ModifyCurrencyFlag(currencyId, uint8(flags));
}
