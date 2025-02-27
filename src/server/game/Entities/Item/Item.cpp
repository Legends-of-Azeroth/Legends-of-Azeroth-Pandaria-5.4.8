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
#include "Item.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "DatabaseEnv.h"
#include "ItemEnchantmentMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "ScriptMgr.h"
#include "ConditionMgr.h"
#include "Player.h"
#include "Opcodes.h"

void AddItemsSetItem(Player* player, Item* item)
{
    ItemTemplate const* proto = item->GetTemplate();
    uint32 setid = proto->ItemSet;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);

    if (!set)
    {
        TC_LOG_ERROR("sql.sql", "Item set %u for item (id %u) not found, mods not applied.", setid, proto->ItemId);
        return;
    }

    if (set->required_skill_id && player->GetSkillValue(set->required_skill_id) < set->required_skill_value)
        return;

    ItemSetEffect* eff = NULL;

    for (size_t x = 0; x < player->ItemSetEff.size(); ++x)
    {
        if (player->ItemSetEff[x] && player->ItemSetEff[x]->setid == setid)
        {
            eff = player->ItemSetEff[x];
            break;
        }
    }

    if (!eff)
    {
        eff = new ItemSetEffect();
        eff->setid = setid;

        size_t x = 0;
        for (; x < player->ItemSetEff.size(); ++x)
            if (!player->ItemSetEff[x])
                break;

        if (x < player->ItemSetEff.size())
            player->ItemSetEff[x]=eff;
        else
            player->ItemSetEff.push_back(eff);
    }

    ++eff->item_count;

    for (uint32 x = 0; x < MAX_ITEM_SET_SPELLS; ++x)
    {
        if (!set->spells [x])
            continue;
        //not enough for  spell
        if (set->items_to_triggerspell[x] > eff->item_count)
            continue;

        uint32 z = 0;
        for (; z < MAX_ITEM_SET_SPELLS; ++z)
            if (eff->spells[z] && eff->spells[z]->Id == set->spells[x])
                break;

        if (z < MAX_ITEM_SET_SPELLS)
            continue;

        //new spell
        for (uint32 y = 0; y < MAX_ITEM_SET_SPELLS; ++y)
        {
            if (!eff->spells[y])                             // free slot
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(set->spells[x]);
                if (!spellInfo)
                {
                    TC_LOG_ERROR("entities.player.items", "WORLD: unknown spell id %u in items set %u effects", set->spells[x], setid);
                    break;
                }

                // spell casted only if fit form requirement, in other case will casted at form change
                player->ApplyEquipSpell(spellInfo, nullptr, true, false, true);
                eff->spells[y] = spellInfo;
                break;
            }
        }
    }
}

void RemoveItemsSetItem(Player*player, ItemTemplate const* proto)
{
    uint32 setid = proto->ItemSet;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);

    if (!set)
    {
        TC_LOG_ERROR("sql.sql", "Item set #%u for item #%u not found, mods not removed.", setid, proto->ItemId);
        return;
    }

    ItemSetEffect* eff = NULL;
    size_t setindex = 0;
    for (; setindex < player->ItemSetEff.size(); setindex++)
    {
        if (player->ItemSetEff[setindex] && player->ItemSetEff[setindex]->setid == setid)
        {
            eff = player->ItemSetEff[setindex];
            break;
        }
    }

    // can be in case now enough skill requirement for set appling but set has been appliend when skill requirement not enough
    if (!eff)
        return;

    --eff->item_count;

    for (uint32 x = 0; x < MAX_ITEM_SET_SPELLS; x++)
    {
        if (!set->spells[x])
            continue;

        // enough for spell
        if (set->items_to_triggerspell[x] <= eff->item_count)
            continue;

        for (uint32 z = 0; z < MAX_ITEM_SET_SPELLS; z++)
        {
            if (eff->spells[z] && eff->spells[z]->Id == set->spells[x])
            {
                // spell can be not active if not fit form requirement
                player->ApplyEquipSpell(eff->spells[z], nullptr, false, false, true);
                eff->spells[z] = nullptr;
                break;
            }
        }
    }

    if (!eff->item_count)                                    //all items of a set were removed
    {
        ASSERT(eff == player->ItemSetEff[setindex]);
        delete eff;
        player->ItemSetEff[setindex] = NULL;
    }
}

bool ItemCanGoIntoBag(ItemTemplate const* pProto, ItemTemplate const* pBagProto)
{
    if (!pProto || !pBagProto)
        return false;

    switch (pBagProto->Class)
    {
        case ITEM_CLASS_CONTAINER:
            switch (pBagProto->SubClass)
            {
                case ITEM_SUBCLASS_CONTAINER:
                    return true;
                case ITEM_SUBCLASS_SOUL_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_SOUL_SHARDS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_HERB_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_HERBS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_ENCHANTING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENCHANTING_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_MINING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_MINING_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_ENGINEERING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENGINEERING_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_GEM_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_GEMS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_LEATHERWORKING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_LEATHERWORKING_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_INSCRIPTION_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_INSCRIPTION_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_TACKLE_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_FISHING_SUPP))
                        return false;
                    return true;
                case ITEM_SUBCLASS_COOKING_CONTAINER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_COOKING_SUPP))
                        return false;
                    return true;
                default:
                    return false;
            }
        case ITEM_CLASS_QUIVER:
            switch (pBagProto->SubClass)
            {
                case ITEM_SUBCLASS_QUIVER:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_ARROWS))
                        return false;
                    return true;
                case ITEM_SUBCLASS_AMMO_POUCH:
                    if (!(pProto->BagFamily & BAG_FAMILY_MASK_BULLETS))
                        return false;
                    return true;
                default:
                    return false;
            }
    }
    return false;
}

Item::Item()
{
    m_objectType |= TYPEMASK_ITEM;
    m_objectTypeId = TYPEID_ITEM;

    m_updateFlag = 0;

    m_valuesCount = ITEM_END;
    m_slot = 0;
    uState = ITEM_NEW;
    uQueuePos = -1;
    m_container = NULL;
    m_lootGenerated = false;
    mb_in_trade = false;

    m_refundRecipient = 0;
    m_paidMoney = 0;
    m_paidExtendedCost = 0;

    m_dynamicValuesCount = ITEM_DYNAMIC_END;
}

bool Item::Create(ObjectGuid::LowType guidlow, uint32 itemid, Player const* owner)
{
    Object::_Create(guidlow, 0, HighGuid::Item);

    SetEntry(itemid);
    SetObjectScale(1.0f);

    SetUInt64Value(ITEM_FIELD_OWNER, owner ? owner->GetGUID() : 0);
    SetUInt64Value(ITEM_FIELD_CONTAINED_IN, owner ? owner->GetGUID() : 0);

    ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(itemid);
    if (!itemProto)
        return false;

    SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
    SetUInt32Value(ITEM_FIELD_MAX_DURABILITY, itemProto->MaxDurability);
    SetUInt32Value(ITEM_FIELD_DURABILITY, itemProto->MaxDurability);

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        SetSpellCharges(i, itemProto->Spells[i].SpellCharges);

    SetUInt32Value(ITEM_FIELD_EXPIRATION, itemProto->Duration);
    SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, 0);

    switch (itemid)     // Facets of Research
    {
        case 90395:
        case 90397:
        case 90398:
        case 90399:
        case 90400:
        case 90401:
        case 90406:
            loot.maxDuplicates = 2;
            break;
        default:
            break;
    }

    return true;
}

// Returns true if Item is a bag AND it is not empty.
// Returns false if Item is not a bag OR it is an empty bag.
bool Item::IsNotEmptyBag() const
{
    if (Bag const* bag = ToBag())
        return !bag->IsEmpty();
    return false;
}

void Item::UpdateDuration(Player* owner, uint32 diff)
{
    if (!GetUInt32Value(ITEM_FIELD_EXPIRATION))
        return;

    TC_LOG_DEBUG("entities.player.items", "Item::UpdateDuration Item (Entry: %u Duration %u Diff %u)", GetEntry(), GetUInt32Value(ITEM_FIELD_EXPIRATION), diff);

    if (GetUInt32Value(ITEM_FIELD_EXPIRATION) <= diff)
    {
        sScriptMgr->OnItemExpire(owner, GetTemplate());
        owner->DestroyItem(GetBagSlot(), GetSlot(), true);
        return;
    }

    SetUInt32Value(ITEM_FIELD_EXPIRATION, GetUInt32Value(ITEM_FIELD_EXPIRATION) - diff);
    SetState(ITEM_CHANGED, owner);                          // save new time in database
}

void Item::SaveToDB(CharacterDatabaseTransaction trans)
{
    bool isInTransaction = trans != nullptr;
    if (!isInTransaction)
        trans = CharacterDatabase.BeginTransaction();

    uint32 guid = GetGUID().GetCounter();
    switch (uState)
    {
        case ITEM_NEW:
        case ITEM_CHANGED:
        {
            uint8 index = 0;
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(uState == ITEM_NEW ? CHAR_REP_ITEM_INSTANCE : CHAR_UPD_ITEM_INSTANCE);
            stmt->setUInt32(  index, GetEntry());
            stmt->setUInt32(++index, GetOwnerGUID().GetCounter());
            stmt->setUInt32(++index, GetGuidValue(ITEM_FIELD_CREATOR).GetCounter());
            stmt->setUInt32(++index, GetGuidValue(ITEM_FIELD_GIFT_CREATOR).GetCounter());
            stmt->setUInt32(++index, GetCount());
            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_EXPIRATION));

            std::ostringstream ssSpells;
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                ssSpells << GetSpellCharges(i) << ' ';
            stmt->setString(++index, ssSpells.str());

            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS));

            std::ostringstream ssEnchants;
            for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
            {
                ssEnchants << GetEnchantmentId(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentDuration(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentCharges(EnchantmentSlot(i)) << ' ';
            }
            stmt->setString(++index, ssEnchants.str());

            stmt->setInt16 (++index, GetItemRandomPropertyId());
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_REFORGE));
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_TRANSMOGRIFICATION));
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_UPGRADE));
            stmt->setUInt16(++index, GetUInt32Value(ITEM_FIELD_DURABILITY));
            stmt->setUInt32(++index, GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME));
            stmt->setString(++index, m_text);
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_SPECIES));
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_BREED) & 0xFFFFFF);
            stmt->setUInt32(++index, (GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_BREED) >> 24) & 0xFF);
            stmt->setUInt32(++index, GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_LEVEL));

            stmt->setUInt32(++index, guid);

            trans->Append(stmt);

            if ((uState == ITEM_CHANGED) && HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_WRAPPED))
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GIFT_OWNER);
                stmt->setUInt32(0, GetOwnerGUID().GetCounter());
                stmt->setUInt32(1, guid);
                trans->Append(stmt);
            }
            break;
        }
        case ITEM_REMOVED:
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
            stmt->setUInt32(0, guid);
            trans->Append(stmt);

            if (HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_WRAPPED))
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GIFT);
                stmt->setUInt32(0, guid);
                trans->Append(stmt);
            }

            // DeleteRefundDataFromDB(trans);

            if (!isInTransaction)
                CharacterDatabase.CommitTransaction(trans);

            // Delete the items if this is a container
            if (!loot.isLooted())
                ItemContainerDeleteLootMoneyAndLootItemsFromDB();

            delete this;
            return;
        }
        case ITEM_UNCHANGED:
            break;
    }

    SetState(ITEM_UNCHANGED);

    if (!isInTransaction)
        CharacterDatabase.CommitTransaction(trans);
}

bool Item::LoadFromDB(uint32 guid, ObjectGuid ownerGuid, Field* fields, uint32 entry, Player* owner)
{
    //                                                    0                1      2         3        4      5             6                 7  8          9                 10          11          12        13     14       15     16       17 
    //result = CharacterDatabase.PQuery("SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, reforgeId, transmogrifyId, upgradeID, durability, playedTime, text, species, breed, quality, level FROM item_instance WHERE guid = '%u'", guid);

    // create item before any checks for store correct guid
    // and allow use "FSetState(ITEM_REMOVED); SaveToDB();" for deleting item from DB
    Object::_Create(guid, 0, HighGuid::Item);

    // Set entry, MUST be before proto check
    SetEntry(entry);
    SetObjectScale(1.0f);

    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    // set owner (not if item is only loaded for gbank/auction/mail
    if (!ownerGuid.IsEmpty())
        SetOwnerGUID(ownerGuid);

    bool need_save = false;                                 // need explicit save data at load fixes
    SetGuidValue(ITEM_FIELD_CREATOR, ObjectGuid(HighGuid::Player, fields[0].GetUInt32()));
    SetGuidValue(ITEM_FIELD_GIFT_CREATOR, ObjectGuid(HighGuid::Player, fields[1].GetUInt32()));
    SetCount(fields[2].GetUInt32());

    uint32 duration = fields[3].GetUInt32();
    SetUInt32Value(ITEM_FIELD_EXPIRATION, duration);
    // update duration if need, and remove if not need
    if ((proto->Duration == 0) != (duration == 0))
    {
        SetUInt32Value(ITEM_FIELD_EXPIRATION, proto->Duration);
        need_save = true;
    }

    Tokenizer tokens(fields[4].GetString(), ' ', MAX_ITEM_PROTO_SPELLS);
    if (tokens.size() == MAX_ITEM_PROTO_SPELLS)
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
            SetSpellCharges(i, atoi(tokens[i]));

    SetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS, fields[5].GetUInt32());
    // Remove bind flag for items vs BIND_NONE set
    if (IsSoulBound() && proto->Bonding == BIND_NONE)
    {
        ApplyModFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_SOULBOUND, false);
        need_save = true;
    }

    std::string enchants = fields[6].GetString();
    _LoadIntoDataField(enchants.c_str(), ITEM_FIELD_ENCHANTMENT, MAX_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET);

    int32 firstPrismatic = 0;
    while (firstPrismatic < MAX_GEM_SOCKETS && proto->Socket[firstPrismatic].Color)
        ++firstPrismatic;

    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
    {
        auto slot = EnchantmentSlot(SOCK_ENCHANTMENT_SLOT + i);
        if (uint32 enchantId = GetEnchantmentId(slot))
        {
            if (!proto->Socket[i].Color)            // Gem in prismatic socket 
            {
                if (!GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT) || i != firstPrismatic) // Oy vey, gem isn't allowed in this socket!
                {
                    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET, 0);
                    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, 0);
                    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, 0);
                    need_save = true;
                }
            }
        }
    }

    if (GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT) == 5015 && !IsEyeOfTheBlackPrinceAllowed())
    {
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + PRISMATIC_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET, 0);
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + PRISMATIC_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, 0);
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + PRISMATIC_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, 0);
        need_save = true;
    }

    int16 randomPropertyId = fields[7].GetInt16();
    if (owner && owner->HasAtLoginFlag(AT_LOGIN_POST_AUTOTRANSFER) && randomPropertyId)
    {
        SetItemRandomProperties(randomPropertyId);
        need_save = true;
    }
    SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, randomPropertyId);
    // recalculate suffix factor
    if (GetItemRandomPropertyId() < 0)
        UpdateItemSuffixFactor();

    if (uint32 reforgeEntry = fields[8].GetInt32())
        SetDynamicModifier(ITEM_MODIFIER_INDEX_REFORGE, reforgeEntry, nullptr, true);

    if (uint32 transmogId = fields[9].GetInt32())
        if (sObjectMgr->GetItemTemplate(transmogId))
            SetDynamicModifier(ITEM_MODIFIER_INDEX_TRANSMOGRIFICATION, transmogId, nullptr, true);

    if (uint32 upgradeId = fields[10].GetUInt32())
    {
        if (GetUpgradeId(GetEntry())) // Check first level upgrade
            SetDynamicModifier(ITEM_MODIFIER_INDEX_UPGRADE, upgradeId, nullptr, true);
    }
    else if (uint32 upgradeId = GetUpgradeId(GetEntry()))
        SetDynamicModifier(ITEM_MODIFIER_INDEX_UPGRADE, upgradeId, nullptr, true);

    uint32 durability = fields[11].GetUInt16();
    SetUInt32Value(ITEM_FIELD_DURABILITY, durability);
    // update max durability (and durability) if need
    SetUInt32Value(ITEM_FIELD_MAX_DURABILITY, proto->MaxDurability);
    if (durability > proto->MaxDurability)
    {
        SetUInt32Value(ITEM_FIELD_DURABILITY, proto->MaxDurability);
        need_save = true;
    }

    SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, fields[12].GetUInt32());

    // TEMPORARY, TO BE REMOVED
    if (HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_UNK10) && owner)
    {
        uint32 realTime = owner->GetTotalPlayedTime() >= fields[12].GetUInt32() ? owner->GetTotalPlayedTime() - fields[12].GetUInt32() : owner->GetTotalPlayedTime();
        SetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME, realTime);
        RemoveFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_UNK10);
        need_save = true;
    }

    SetText(fields[13].GetString());

    if (uint32 speciesId = fields[14].GetInt32())
        SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_SPECIES, speciesId, nullptr, true);
    uint32 breed = fields[15].GetInt32();
    uint32 quality = fields[16].GetInt32();
    if (breed && quality)
        SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_BREED, breed | (quality << 24), nullptr, true);
    if (uint32 level = fields[17].GetInt32())
        SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_LEVEL, level, nullptr, true);

    if (need_save)                                           // normal item changed state set not work at loading
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ITEM_INSTANCE_ON_LOAD);
        stmt->setUInt32(0, GetUInt32Value(ITEM_FIELD_EXPIRATION));
        stmt->setUInt32(1, GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS));
        stmt->setUInt32(2, GetUInt32Value(ITEM_FIELD_DURABILITY));
        stmt->setUInt32(3, GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME));
        std::ostringstream ssEnchants;
        for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
        {
            ssEnchants << GetEnchantmentId(EnchantmentSlot(i)) << ' ';
            ssEnchants << GetEnchantmentDuration(EnchantmentSlot(i)) << ' ';
            ssEnchants << GetEnchantmentCharges(EnchantmentSlot(i)) << ' ';
        }
        stmt->setString(4, ssEnchants.str());
        stmt->setUInt32(5, guid);
        CharacterDatabase.Execute(stmt);
    }

    return true;
}

/*static*/
void Item::DeleteFromDB(CharacterDatabaseTransaction trans, uint32 itemGuid)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
    stmt->setUInt32(0, itemGuid);
    trans->Append(stmt);
}

void Item::DeleteFromDB(CharacterDatabaseTransaction trans)
{
    DeleteFromDB(trans, GetGUID().GetCounter());

    // Delete the items if this is a container
    if (!loot.isLooted())
        ItemContainerDeleteLootMoneyAndLootItemsFromDB();
}

/*static*/
void Item::DeleteFromInventoryDB(CharacterDatabaseTransaction trans, uint32 itemGuid)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_INVENTORY_BY_ITEM);
    stmt->setUInt32(0, itemGuid);
    trans->Append(stmt);
}

void Item::DeleteFromInventoryDB(CharacterDatabaseTransaction trans)
{
    DeleteFromInventoryDB(trans, GetGUID().GetCounter());
}

ItemTemplate const* Item::GetTemplate() const
{
    return sObjectMgr->GetItemTemplate(GetEntry());
}

Player* Item::GetOwner()const
{
    return ObjectAccessor::FindPlayer(GetOwnerGUID());
}

uint32 Item::GetSkill(uint32 id)
{
    const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
        SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
        SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
        SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
        SKILL_FISHING
    };

    const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
    };

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(id);

    switch (proto->Class)
    {
        case ITEM_CLASS_WEAPON:
            if (proto->SubClass >= MAX_ITEM_SUBCLASS_WEAPON)
                return 0;
            else
                return item_weapon_skills[proto->SubClass];

        case ITEM_CLASS_ARMOR:
            if (proto->SubClass >= MAX_ITEM_SUBCLASS_ARMOR)
                return 0;
            else
                return item_armor_skills[proto->SubClass];

        default:
            return 0;
    }
}

int32 Item::GenerateItemRandomPropertyId(uint32 item_id)
{
    ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(item_id);

    if (!itemProto)
        return 0;

    // item must have one from this field values not null if it can have random enchantments
    if ((!itemProto->RandomProperty) && (!itemProto->RandomSuffix))
        return 0;

    // item can have not null only one from field values
    if ((itemProto->RandomProperty) && (itemProto->RandomSuffix))
    {
        TC_LOG_ERROR("sql.sql", "Item template %u have RandomProperty == %u and RandomSuffix == %u, but must have one from field =0", itemProto->ItemId, itemProto->RandomProperty, itemProto->RandomSuffix);
        return 0;
    }

    // RandomProperty case
    if (itemProto->RandomProperty)
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomProperty, ItemRandomEnchantmentType::Property);
        ItemRandomPropertiesEntry const* random_id = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            TC_LOG_ERROR("sql.sql", "Enchantment id #%u used but it doesn't have records in 'ItemRandomProperties.dbc'", randomPropId);
            return 0;
        }

        return random_id->ID;
    }
    // RandomSuffix case
    else
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomSuffix, ItemRandomEnchantmentType::Suffix);
        ItemRandomSuffixEntry const* random_id = sItemRandomSuffixStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            TC_LOG_ERROR("sql.sql", "Enchantment id #%u used but it doesn't have records in sItemRandomSuffixStore.", randomPropId);
            return 0;
        }

        return -int32(random_id->ID);
    }
}

void Item::SetItemRandomProperties(int32 randomPropId)
{
    if (!randomPropId)
        return;

    if (randomPropId > 0)
    {
        ItemRandomPropertiesEntry const* item_rand = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != int32(item_rand->ID))
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, item_rand->ID);
                if (Player* owner = GetOwner())
                    SetState(ITEM_CHANGED, owner);
            }
            for (uint32 i = PROP_ENCHANTMENT_SLOT_1; i < PROP_ENCHANTMENT_SLOT_1 + 3; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->enchant_id[i - PROP_ENCHANTMENT_SLOT_1], 0, 0);
        }
    }
    else
    {
        ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(-randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != -int32(item_rand->ID) ||
                !GetItemSuffixFactor())
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, -int32(item_rand->ID));
                UpdateItemSuffixFactor();
                if (Player* owner = GetOwner())
                    SetState(ITEM_CHANGED, owner);
            }

            for (uint32 i = PROP_ENCHANTMENT_SLOT_0; i <= PROP_ENCHANTMENT_SLOT_4; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->enchant_id[i - PROP_ENCHANTMENT_SLOT_0], 0, 0);
        }
    }
}

void Item::UpdateItemSuffixFactor()
{
    uint32 suffixFactor = GenerateEnchSuffixFactor(GetEntry());
    if (GetItemSuffixFactor() == suffixFactor)
        return;
    SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, suffixFactor);
}

void Item::SetState(ItemUpdateState state, Player* forplayer)
{
    if (uState == ITEM_NEW && state == ITEM_REMOVED)
    {
        // pretend the item never existed
        RemoveFromUpdateQueueOf(forplayer);
        forplayer->DeleteRefundReference(GetGUID());
        delete this;
        return;
    }
    if (state != ITEM_UNCHANGED)
    {
        // new items must stay in new state until saved
        if (uState != ITEM_NEW)
            uState = state;

        AddToUpdateQueueOf(forplayer);
    }
    else
    {
        // unset in queue
        // the item must be removed from the queue manually
        uQueuePos = -1;
        uState = ITEM_UNCHANGED;
    }
}

void Item::AddToUpdateQueueOf(Player* player)
{
    if (IsInUpdateQueue())
        return;

    ASSERT(player != NULL);

    if (player->GetGUID() != GetOwnerGUID())
    {
        TC_LOG_DEBUG("entities.player.items", "Item::AddToUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GetOwnerGUID().GetCounter(), player->GetGUID().GetCounter());
        return;
    }

    if (player->m_itemUpdateQueueBlocked)
        return;

    player->m_itemUpdateQueue.push_back(this);
    uQueuePos = player->m_itemUpdateQueue.size()-1;
}

void Item::RemoveFromUpdateQueueOf(Player* player)
{
    if (!IsInUpdateQueue())
        return;

    ASSERT(player != NULL);

    if (player->GetGUID() != GetOwnerGUID())
    {
        TC_LOG_DEBUG("entities.player.items", "Item::RemoveFromUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GetOwnerGUID().GetCounter(), player->GetGUID().GetCounter());
        return;
    }

    if (player->m_itemUpdateQueueBlocked)
        return;

    player->m_itemUpdateQueue[uQueuePos] = NULL;
    uQueuePos = -1;
}

uint8 Item::GetBagSlot() const
{
    return m_container ? m_container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);
}

bool Item::IsEquipped() const
{
    return !IsInBag() && m_slot < EQUIPMENT_SLOT_END;
}

bool Item::CanBeTraded(bool mail, bool trade) const
{
    if (m_lootGenerated)
        return false;

    if ((!mail || !IsBoundAccountWide()) && (IsSoulBound() && (!HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_BOP_TRADEABLE) || !trade)))
        return false;

    if (IsBag() && (Player::IsBagPos(GetPos()) || !((Bag const*)this)->IsEmpty()))
        return false;

    if (Player* owner = GetOwner())
    {
        if (owner->CanUnequipItem(GetPos(), false) != EQUIP_ERR_OK)
            return false;
        if (owner->GetLootGUID() == GetGUID())
            return false;
    }

    if ((!mail || !IsBoundAccountWide()) && IsBoundByEnchant())
        return false;

    return true;
}

bool Item::HasEnchantRequiredSkill(const Player* player) const
{
    // Check all enchants for required skill
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > ENGINEERING_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->RequiredSkill && player->GetSkillValue(enchantEntry->RequiredSkill) < enchantEntry->RequiredSkillValue)
                    return false;
    }

  return true;
}

uint32 Item::GetEnchantRequiredLevel() const
{
    uint32 level = 0;

    // Check all enchants for required level
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > PRISMATIC_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->RequiredLevel > level)
                    level = enchantEntry->RequiredLevel;
    }

    return level;
}

bool Item::IsBoundByEnchant() const
{
    // Check all enchants for soulbound
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
    {
        if (enchant_slot > PRISMATIC_ENCHANTMENT_SLOT && enchant_slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
            continue;

        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->Flags & ENCHANTMENT_FLAG_CAN_SOULBOUND)
                    return true;
    }

    return false;
}

InventoryResult Item::CanBeMergedPartlyWith(ItemTemplate const* proto) const
{
    // not allow merge looting currently items
    if (m_lootGenerated)
        return EQUIP_ERR_LOOT_GONE;

    // check item type
    if (GetEntry() != proto->ItemId)
        return EQUIP_ERR_CANT_STACK;

    // check free space (full stacks can't be target of merge
    if (GetCount() >= proto->GetMaxStackSize())
        return EQUIP_ERR_CANT_STACK;

    return EQUIP_ERR_OK;
}

bool Item::IsFitToSpellRequirements(SpellInfo const* spellInfo) const
{
    ItemTemplate const* proto = GetTemplate();

    if (spellInfo->EquippedItemClass != -1)                 // -1 == any item class
    {
        // Special case - accept vellum for armor/weapon requirements
        if ((spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR ||
            spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON) && proto->IsVellum())
            if (spellInfo->IsAbilityOfSkillType(SKILL_ENCHANTING)) // only for enchanting spells
                return true;

        if (spellInfo->EquippedItemClass != int32(proto->Class))
            return false;                                   //  wrong item class

        if (spellInfo->EquippedItemSubClassMask != 0)        // 0 == any subclass
        {
            if ((spellInfo->EquippedItemSubClassMask & (1 << proto->SubClass)) == 0)
                return false;                               // subclass not present in mask
        }
    }

    if (spellInfo->EquippedItemInventoryTypeMask != 0)       // 0 == any inventory type
    {
        // Special case - accept weapon type for main and offhand requirements
        if (proto->InventoryType == INVTYPE_WEAPON &&
            (spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONMAINHAND) ||
             spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONOFFHAND)))
            return true;
        else if (spellInfo->EquippedItemInventoryTypeMask == (1 << INVTYPE_WEAPONOFFHAND))
            return GetSlot() == EQUIPMENT_SLOT_OFFHAND;
        else if ((spellInfo->EquippedItemInventoryTypeMask & (1 << proto->InventoryType)) == 0)
            return false;                                   // inventory type not present in mask
    }

    return true;
}

void Item::SetEnchantment(EnchantmentSlot slot, uint32 id, uint32 duration, uint32 charges, ObjectGuid caster /*= 0*/)
{
    // Better lost small time at check in comparison lost time at item save to DB.
    if ((GetEnchantmentId(slot) == id) && (GetEnchantmentDuration(slot) == duration) && (GetEnchantmentCharges(slot) == charges))
        return;

    Player* owner = GetOwner();
    if (owner && slot < MAX_INSPECTED_ENCHANTMENT_SLOT)
    {
        if (uint32 oldEnchant = GetEnchantmentId(slot))
            owner->GetSession()->SendEnchantmentLog(GetOwnerGUID(), ObjectGuid::Empty, GetGUID(), GetEntry(), oldEnchant, slot);

        if (id)
            owner->GetSession()->SendEnchantmentLog(GetOwnerGUID(), caster, GetGUID(), GetEntry(), id, slot);

        SetState(ITEM_CHANGED, owner);
    }

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET, id);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
}

void Item::SetEnchantmentDuration(EnchantmentSlot slot, uint32 duration, Player* owner)
{
    if (GetEnchantmentDuration(slot) == duration)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetState(ITEM_CHANGED, owner);
    // Cannot use GetOwner() here, has to be passed as an argument to avoid freeze due to hashtable locking
}

void Item::SetEnchantmentCharges(EnchantmentSlot slot, uint32 charges)
{
    if (GetEnchantmentCharges(slot) == charges)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
    SetState(ITEM_CHANGED, GetOwner());
}

void Item::ClearEnchantment(EnchantmentSlot slot)
{
    if (!GetEnchantmentId(slot))
        return;

    for (uint8 x = 0; x < MAX_ITEM_ENCHANTMENT_EFFECTS; ++x)
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot*MAX_ENCHANTMENT_OFFSET + x, 0);
    SetState(ITEM_CHANGED, GetOwner());
}

bool Item::GemsFitSockets() const
{
    for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT+MAX_GEM_SOCKETS; ++enchant_slot)
    {
        uint8 SocketColor = GetTemplate()->Socket[enchant_slot-SOCK_ENCHANTMENT_SLOT].Color;

        if (!SocketColor) // no socket slot
            continue;

        uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot));
        if (!enchant_id) // no gems on this socket
            return false;

        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchantEntry) // invalid gem id on this socket
            return false;

        uint8 GemColor = 0;

        uint32 gemid = enchantEntry->GemID;
        if (gemid)
        {
            ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(gemid);
            if (gemProto)
            {
                GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(gemProto->GemProperties);
                if (gemProperty)
                    GemColor = gemProperty->color;
            }
        }

        if (!(GemColor & SocketColor)) // bad gem color on this socket
            return false;
    }
    return true;
}

uint8 Item::GetGemCountWithID(uint32 GemID) const
{
    uint8 count = 0;
    for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT+MAX_GEM_SOCKETS; ++enchant_slot)
    {
        uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot));
        if (!enchant_id)
            continue;

        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchantEntry)
            continue;

        if (GemID == enchantEntry->GemID)
            ++count;
    }
    return count;
}

uint8 Item::GetGemCountWithLimitCategory(uint32 limitCategory) const
{
    uint8 count = 0;
    for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT+MAX_GEM_SOCKETS; ++enchant_slot)
    {
        uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot));
        if (!enchant_id)
            continue;

        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchantEntry)
            continue;

        ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(enchantEntry->GemID);
        if (!gemProto)
            continue;

        if (gemProto->ItemLimitCategory == limitCategory)
            ++count;
    }
    return count;
}

bool Item::IsLimitedToAnotherMapOrZone(uint32 cur_mapId, uint32 cur_zoneId) const
{
    ItemTemplate const* proto = GetTemplate();
    return proto && ((proto->Map && proto->Map != cur_mapId) || (proto->Area && proto->Area != cur_zoneId));
}

void Item::SendUpdateSockets()
{
    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_SOCKET_GEMS_RESULT, 8+4+4+4+4);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[4]);

    data.FlushBits();

    data.WriteByteSeq(guid[2]);
    data << uint32(GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) ? GetTemplate()->socketBonus : 0);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    for (uint32 i = SOCK_ENCHANTMENT_SLOT; i <= SOCK_ENCHANTMENT_SLOT_3; ++i)
        data << uint32(GetEnchantmentId(EnchantmentSlot(i)));
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);

    GetOwner()->GetSession()->SendPacket(&data);
}

// Though the client has the information in the item's data field,
// we have to send SMSG_ITEM_TIME_UPDATE to display the remaining
// time.
void Item::SendTimeUpdate(Player* owner)
{
    uint32 duration = GetUInt32Value(ITEM_FIELD_EXPIRATION);

    if (!duration)
        return;

    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_ITEM_TIME_UPDATE, (8+4));
    data.WriteBit(guid[5]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[1]);
    data << uint32(duration);

    owner->GetSession()->SendPacket(&data);
}

Item* Item::CreateItem(uint32 itemEntry, uint32 count, Player const* player)
{
    if (count < 1)
        return NULL;                                        //don't create item at zero count

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry);
    if (proto)
    {
        if (count > proto->GetMaxStackSize())
            count = proto->GetMaxStackSize();

        ASSERT(count != 0 && "pProto->Stackable == 0 but checked at loading already");

        Item* item = NewItemOrBag(proto);
        if (item->Create(sObjectMgr->GetGenerator<HighGuid::Item>().Generate(), itemEntry, player))
        {
            item->SetCount(count);
            return item;
        }
        else
            delete item;
    }
    else
        ASSERT(false);
    return NULL;
}

Item* Item::CloneItem(uint32 count, Player const* player) const
{
    Item* newItem = CreateItem(GetEntry(), count, player);
    if (!newItem)
        return NULL;

    newItem->SetGuidValue(ITEM_FIELD_CREATOR,         GetGuidValue(ITEM_FIELD_CREATOR));
    newItem->SetGuidValue(ITEM_FIELD_GIFT_CREATOR,    GetGuidValue(ITEM_FIELD_GIFT_CREATOR));
    newItem->SetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS, GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS) & ~(ITEM_FLAG_REFUNDABLE | ITEM_FLAG_BOP_TRADEABLE));
    newItem->SetUInt32Value(ITEM_FIELD_EXPIRATION,    GetUInt32Value(ITEM_FIELD_EXPIRATION));
    // player CAN be NULL in which case we must not update random properties because that accesses player's item update queue
    if (player)
        newItem->SetItemRandomProperties(GetItemRandomPropertyId());
    return newItem;
}

bool Item::IsBindedNotWith(Player const* player) const
{
    // not binded item
    if (!IsSoulBound())
        return false;

    // own item
    if (GetOwnerGUID() == player->GetGUID())
        return false;

    if (HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_BOP_TRADEABLE))
        if (allowedGUIDs.find(player->GetGUID()) != allowedGUIDs.end())
            return false;

    // BOA item case
    if (IsBoundAccountWide())
        return false;

    return true;
}

void Item::BuildUpdate(UpdateDataMapType& data_map)
{
    if (Player* owner = GetOwner())
        BuildFieldsUpdate(owner, data_map);
    ClearUpdateMask(false);
}

void Item::SaveRefundDataToDB()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_REFUND_INSTANCE);
    stmt->setUInt32(0, GetGUID());
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ITEM_REFUND_INSTANCE);
    stmt->setUInt32(0, GetGUID());
    stmt->setUInt32(1, GetRefundRecipient());
    stmt->setUInt32(2, GetPaidMoney());
    stmt->setUInt16(3, uint16(GetPaidExtendedCost()));
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void Item::DeleteRefundDataFromDB(CharacterDatabaseTransaction* trans)
{
    if (trans)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_REFUND_INSTANCE);
        stmt->setUInt32(0, GetGUID());
        (*trans)->Append(stmt);

    }
}

void Item::SetNotRefundable(Player* owner, bool changestate /*=true*/, CharacterDatabaseTransaction* trans /*=nullptr*/)
{
    if (!HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_REFUNDABLE))
        return;

    RemoveFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_REFUNDABLE);
    // Following is not applicable in the trading procedure
    if (changestate)
        SetState(ITEM_CHANGED, owner);

    SetRefundRecipient(0);
    SetPaidMoney(0);
    SetPaidExtendedCost(0);
    DeleteRefundDataFromDB(trans);

    owner->DeleteRefundReference(GetGUID());
}

void Item::UpdatePlayedTime(Player* owner)
{
    if (GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME) + 2 * HOUR < owner->GetTotalPlayedTime())
        SetNotRefundable(owner);
}

void Item::SetSoulboundTradeable(AllowedLooterSet const& allowedLooters)
{
    SetFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_BOP_TRADEABLE);
    allowedGUIDs = allowedLooters;
}

void Item::ClearSoulboundTradeable(Player* currentOwner)
{
    RemoveFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_BOP_TRADEABLE);
    if (allowedGUIDs.empty())
        return;

    allowedGUIDs.clear();
    SetState(ITEM_CHANGED, currentOwner);
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_BOP_TRADE);
    stmt->setUInt32(0, GetGUID().GetCounter());
    CharacterDatabase.Execute(stmt);
}

bool Item::CheckSoulboundTradeExpire()
{
    // called from owner's update - GetOwner() MUST be valid
    if (GetUInt32Value(ITEM_FIELD_CREATE_PLAYED_TIME) + 2 * HOUR < GetOwner()->GetTotalPlayedTime())
    {
        ClearSoulboundTradeable(GetOwner());
        return true; // remove from tradeable list
    }

    return false;
}

bool Item::HasStats() const
{
    if (GetItemRandomPropertyId() != 0)
        return true;

    ItemTemplate const* proto = GetTemplate();
    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        if (proto->ItemStat[i].ItemStatValue != 0)
            return true;

    return false;
}

// used by mail items, transmog cost, stationeryinfo and others
uint32 Item::GetSellPrice(ItemTemplate const* proto, bool& normalSellPrice)
{
    normalSellPrice = true;

    if (proto->Flags2 & ITEM_FLAGS_EXTRA_HAS_NORMAL_PRICE)
    {
        return proto->BuyPrice;
    }
    else
    {
        ImportPriceQualityEntry const* qualityPrice = sImportPriceQualityStore.LookupEntry(proto->Quality + 1);
        ItemPriceBaseEntry const* basePrice = sItemPriceBaseStore.LookupEntry(proto->ItemLevel);

        if (!qualityPrice || !basePrice)
            return 0;

        float qualityFactor = qualityPrice->Factor;
        float baseFactor = 0.0f;

        uint32 inventoryType = proto->InventoryType;

        if (inventoryType == INVTYPE_WEAPON ||
            inventoryType == INVTYPE_2HWEAPON ||
            inventoryType == INVTYPE_WEAPONMAINHAND ||
            inventoryType == INVTYPE_WEAPONOFFHAND ||
            inventoryType == INVTYPE_RANGED ||
            inventoryType == INVTYPE_THROWN ||
            inventoryType == INVTYPE_RANGEDRIGHT)
            baseFactor = basePrice->WeaponFactor;
        else
            baseFactor = basePrice->ArmorFactor;

        if (inventoryType == INVTYPE_ROBE)
            inventoryType = INVTYPE_CHEST;

        float typeFactor = 0.0f;
        int8 weapType = -1;

        switch (inventoryType)
        {
            case INVTYPE_HEAD:
            case INVTYPE_SHOULDERS:
            case INVTYPE_CHEST:
            case INVTYPE_WAIST:
            case INVTYPE_LEGS:
            case INVTYPE_FEET:
            case INVTYPE_WRISTS:
            case INVTYPE_HANDS:
            case INVTYPE_CLOAK:
            {
                ImportPriceArmorEntry const* armorPrice = sImportPriceArmorStore.LookupEntry(inventoryType);
                if (!armorPrice)
                    return 0;

                switch (proto->SubClass)
                {
                    case ITEM_SUBCLASS_ARMOR_MISCELLANEOUS:
                    case ITEM_SUBCLASS_ARMOR_CLOTH:
                        typeFactor = armorPrice->ClothFactor;
                        break;
                    case ITEM_SUBCLASS_ARMOR_LEATHER:
                        typeFactor = armorPrice->LeatherFactor;
                        break;
                    case ITEM_SUBCLASS_ARMOR_MAIL:
                        typeFactor = armorPrice->MailFactor;
                        break;
                    case ITEM_SUBCLASS_ARMOR_PLATE:
                        typeFactor = armorPrice->PlateFactor;
                        break;
                    default:
                        return 0;
                }

                break;
            }
            case INVTYPE_SHIELD:
            {
                ImportPriceShieldEntry const* shieldPrice = sImportPriceShieldStore.LookupEntry(1); // it only has two rows, it's unclear which is the one used
                if (!shieldPrice)
                    return 0;

                typeFactor = shieldPrice->Factor;
                break;
            }
            case INVTYPE_WEAPONMAINHAND:
                weapType = 0;
                break;
            case INVTYPE_WEAPONOFFHAND:
                weapType = 1;
                break;
            case INVTYPE_WEAPON:
                weapType = 2;
                break;
            case INVTYPE_2HWEAPON:
                weapType = 3;
                break;
            case INVTYPE_RANGED:
            case INVTYPE_RANGEDRIGHT:
            case INVTYPE_RELIC:
                weapType = 4;
                break;
            default:
                return proto->BuyPrice;
        }

        if (weapType != -1)
        {
            ImportPriceWeaponEntry const* weaponPrice = sImportPriceWeaponStore.LookupEntry(weapType + 1);
            if (!weaponPrice)
                return 0;

            typeFactor = weaponPrice->Factor;
        }

        normalSellPrice = false;
        return uint32(qualityFactor * proto->Unk430_2 * proto->Unk430_1 * typeFactor * baseFactor);
    }
}

uint32 Item::GetSpecialPrice(ItemTemplate const* proto, uint32 minimumPrice /*= 10000*/)
{
    uint32 cost = 0;

    if (proto->Flags2 & ITEM_FLAGS_EXTRA_HAS_NORMAL_PRICE)
        cost = proto->SellPrice;
    else
    {
        bool normalPrice;
        cost = Item::GetSellPrice(proto, normalPrice);

        if (!normalPrice)
        {
            if (proto->BuyCount <= 1)
            {
                ItemClassEntry const* classEntry = sItemClassStore.LookupEntry(proto->Class);
                if (classEntry)
                    cost *= classEntry->PriceFactor;
                else
                    cost = 0;
            }
            else
                cost /= 4 * proto->BuyCount;
        }
        else
            cost = proto->SellPrice;
    }

    if (cost < minimumPrice)
        cost = minimumPrice;

    return cost;
}

int32 Item::GetStatValue(uint32 statIndex) const
{
    uint32 itemLevel = GetItemLevel();
    ItemTemplate const* proto = GetTemplate();

    // Expertise and hit must not be scalled down
    uint32 statType = proto->ItemStat[statIndex].ItemStatType;
    if (statType == ITEM_MOD_HIT_RATING || statType == ITEM_MOD_EXPERTISE_RATING)
        itemLevel = GetBaseItemLevel();

    int32 val = proto->ItemStat[statIndex].ItemStatValue;
    if (proto->ItemLevel != itemLevel)
    {
        if (int32 randomPoints = proto->CalculateScalingStatValue(itemLevel))
        {
            float stat = (proto->ItemStat[statIndex].ItemStatAllocation * randomPoints) * 0.0001f;
            if (GtItemSocketCostPerLevelEntry const* entry = sGtItemSocketCostPerLevelStore.LookupEntry(itemLevel))
                stat -= int32(proto->ItemStat[statIndex].ItemStatSocketCostMultiplier * entry->SocketCost);
            val = int32(std::floor(stat + 0.5f)); // From client
        }
    }
    return val;
}

int32 Item::GetReforgableStat(ItemModType statType) const
{
    ItemTemplate const* proto = GetTemplate();
    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        if (ItemModType(proto->ItemStat[i].ItemStatType) == statType)
            return GetStatValue(i);

    int32 randomPropId = GetItemRandomPropertyId();
    if (!randomPropId)
        return 0;

    if (randomPropId < 0)
    {
        ItemRandomSuffixEntry const* randomSuffix = sItemRandomSuffixStore.LookupEntry(-randomPropId);
        if (!randomSuffix)
            return 0;

        for (uint32 e = PROP_ENCHANTMENT_SLOT_0; e <= PROP_ENCHANTMENT_SLOT_4; ++e)
            if (SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(GetEnchantmentId(EnchantmentSlot(e))))
                for (uint32 f = 0; f < MAX_ITEM_ENCHANTMENT_EFFECTS; ++f)
                    if (enchant->Type[f] == ITEM_ENCHANTMENT_TYPE_STAT && ItemModType(enchant->SpellID[f]) == statType)
                        for (int k = 0; k < 5; ++k)
                            if (randomSuffix->enchant_id[k] == enchant->ID)
                                return int32((randomSuffix->prefix[k] * GetItemSuffixFactor()) / 10000);
    }
    else
    {
        ItemRandomPropertiesEntry const* randomProp = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (!randomProp)
            return 0;

        for (uint32 e = PROP_ENCHANTMENT_SLOT_0; e <= PROP_ENCHANTMENT_SLOT_4; ++e)
            if (SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(GetEnchantmentId(EnchantmentSlot(e))))
                for (uint32 f = 0; f < MAX_ITEM_ENCHANTMENT_EFFECTS; ++f)
                    if (enchant->Type[f] == ITEM_ENCHANTMENT_TYPE_STAT && ItemModType(enchant->SpellID[f]) == statType)
                        for (int k = 0; k < MAX_ITEM_ENCHANTMENT_EFFECTS; ++k)
                            if (randomProp->enchant_id[k] == enchant->ID)
                                return int32(enchant->Amount[k]);
    }

    return 0;
}

void Item::ItemContainerSaveLootToDB()
{
    // Saves the money and item loot associated with an openable item to the DB
    if (loot.isLooted()) // no money and no loot
        return;

    uint32 container_id = GetGUID().GetCounter();
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    loot.containerID = container_id; // Save this for when a LootItem is removed

    // Save money
    if (loot.gold > 0)
    {
        CharacterDatabasePreparedStatement* stmt_money = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_MONEY);
        stmt_money->setUInt32(0, container_id);
        trans->Append(stmt_money);

        stmt_money = CharacterDatabase.GetPreparedStatement(CHAR_INS_ITEMCONTAINER_MONEY);
        stmt_money->setUInt32(0, container_id);
        stmt_money->setUInt32(1, loot.gold);
        trans->Append(stmt_money);
    }

    // Save items
    if (!loot.isLooted())
    {
        CharacterDatabasePreparedStatement* stmt_items = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_ITEMS);
        stmt_items->setUInt32(0, container_id);
        trans->Append(stmt_items);

        // Now insert the items
        for (LootItemList::const_iterator _li = loot.items.begin(); _li != loot.items.end(); ++_li)
        {
            // When an item is looted, it doesn't get removed from the items collection
            //  but we don't want to resave it.
            if (!_li->canSave)
                continue;

            stmt_items = CharacterDatabase.GetPreparedStatement(CHAR_INS_ITEMCONTAINER_ITEMS);

            // container_id, item_id, item_count, follow_rules, ffa, blocked, counted, under_threshold, needs_quest, rnd_prop, rnd_suffix
            stmt_items->setUInt32(0, container_id);
            stmt_items->setUInt32(1, _li->itemid);
            stmt_items->setUInt32(2, _li->count);
            stmt_items->setBool(3, _li->follow_loot_rules);
            stmt_items->setBool(4, _li->freeforall);
            stmt_items->setBool(5, _li->is_blocked);
            stmt_items->setBool(6, _li->is_counted);
            stmt_items->setBool(7, _li->is_underthreshold);
            stmt_items->setBool(8, _li->needs_quest);
            stmt_items->setInt32(9, _li->randomPropertyId);
            stmt_items->setUInt32(10, _li->randomSuffix);
            trans->Append(stmt_items);
        }
    }

    CharacterDatabase.CommitTransaction(trans);
}

bool Item::ItemContainerLoadLootFromDB()
{
    // Loads the money and item loot associated with an openable item from the DB
    // Default. If there are no records for this item then it will be rolled for in Player::SendLoot()
    m_lootGenerated = false;

    uint32 container_id = GetGUID().GetCounter();

    // Save this for later use
    loot.containerID = container_id;

    // First, see if there was any money loot. This gets added directly to the container.
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ITEMCONTAINER_MONEY);
    stmt->setUInt32(0, container_id);
    PreparedQueryResult money_result = CharacterDatabase.Query(stmt);

    if (money_result)
    {
        Field* fields = money_result->Fetch();
        loot.gold = fields[0].GetUInt32();
    }

    // Next, load any items that were saved
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ITEMCONTAINER_ITEMS);
    stmt->setUInt32(0, container_id);
    PreparedQueryResult item_result = CharacterDatabase.Query(stmt);

    if (item_result)
    {
        // Get a LootTemplate for the container item. This is where
        //  the saved loot was originally rolled from, we will copy conditions from it
        LootTemplate const* lt = LootTemplates_Item.GetLootFor(GetEntry());
        if (lt)
        {
            do
            {
                // Create an empty LootItem
                LootItem loot_item = LootItem();

                // Fill in the rest of the LootItem from the DB
                Field* fields = item_result->Fetch();

                // item_id, itm_count, follow_rules, ffa, blocked, counted, under_threshold, needs_quest, rnd_prop, rnd_suffix
                loot_item.itemid = fields[0].GetUInt32();
                loot_item.count = fields[1].GetUInt32();
                loot_item.follow_loot_rules = fields[2].GetBool();
                loot_item.freeforall = fields[3].GetBool();
                loot_item.is_blocked = fields[4].GetBool();
                loot_item.is_counted = fields[5].GetBool();
                loot_item.canSave = true;
                loot_item.is_underthreshold = fields[6].GetBool();
                loot_item.needs_quest = fields[7].GetBool();
                loot_item.randomPropertyId = fields[8].GetInt32();
                loot_item.randomSuffix = fields[9].GetUInt32();

                // Copy the extra loot conditions from the item in the loot template
                lt->CopyConditions(&loot_item);

                // If container item is in a bag, add that player as an allowed looter
                if (GetBagSlot())
                    loot_item.allowedGUIDs.insert(GetOwner()->GetGUID());

                // Finally add the LootItem to the container
                loot.items.push_back(loot_item);

                // Increment unlooted count
                loot.unlootedCount++;

            }
            while (item_result->NextRow());
        }
    }

    // Mark the item if it has loot so it won't be generated again on open
    m_lootGenerated = !loot.isLooted();

    return m_lootGenerated;
}

void Item::ItemContainerDeleteLootItemsFromDB()
{
    // Deletes items associated with an openable item from the DB
    uint32 containerId = GetGUID().GetCounter();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_ITEMS);
    stmt->setUInt32(0, containerId);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
}

void Item::ItemContainerDeleteLootItemFromDB(uint32 itemID)
{
    // Deletes a single item associated with an openable item from the DB
    uint32 containerId = GetGUID().GetCounter();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_ITEM);
    stmt->setUInt32(0, containerId);
    stmt->setUInt32(1, itemID);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
}

void Item::ItemContainerDeleteLootMoneyFromDB()
{
    // Deletes the money loot associated with an openable item from the DB
    uint32 containerId = GetGUID().GetCounter();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_MONEY);
    stmt->setUInt32(0, containerId);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
}

void Item::ItemContainerDeleteLootMoneyAndLootItemsFromDB()
{
    // Deletes money and items associated with an openable item from the DB
    ItemContainerDeleteLootMoneyFromDB();
    ItemContainerDeleteLootItemsFromDB();
}

bool ItemTemplate::IsPvPItem() const
{
    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
    {
        uint32 stat = ItemStat[i].ItemStatType;
        if ((stat == ITEM_MOD_PVP_POWER || stat == ITEM_MOD_RESILIENCE_RATING) &&
            (ItemStat[i].ItemStatValue || ItemStat[i].ItemStatAllocation))
            return true;
    }

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        int32 spell = Spells[i].SpellId;
        if (spell == 132586 || spell == 139891)
            return true;
    }

    return false;
}

bool Item::IsStuffItem() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    uint32 invType = proto->InventoryType;

    switch (invType)
    {
        case INVTYPE_NON_EQUIP:
        case INVTYPE_BODY:
        case INVTYPE_BAG:
        case INVTYPE_TABARD:
        case INVTYPE_HOLDABLE:
        case INVTYPE_AMMO:
        case INVTYPE_THROWN:
        case INVTYPE_QUIVER:
        case INVTYPE_RELIC:
        case INVTYPE_RANGEDRIGHT:
            return false;
        default:
            return true;
    }

    return false;
}

bool Item::IsLegendaryCloak() const
{
    ItemTemplate const* proto = GetTemplate();
    if (!proto)
        return false;

    switch (proto->ItemId)
    {
        case 102245: // Qian-Le, Courage of Niuzao
        case 102246: // Xing-Ho, Breath of Yu'lon
        case 102247: // Jina-Kang, Kindness of Chi-Ji
        case 102248: // Fen-Yu, Fury of Xuen
        case 102249: // Gong-Lu, Strength of Xuen
        case 102250: // Qian-Ying, Fortitude of Niuzao
            return true;
        default:
            break;
    }

    return false;
}

void Item::SetDynamicModifier(ItemModifierIndex dynIdx, uint32 value, Player* player, bool load)
{
    SetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, dynIdx, value);
    SetFlag(ITEM_FIELD_MODIFIERS_MASK, 1 << dynIdx);

    if (!load && player)
        SetState(ITEM_CHANGED, player);
}

void Item::RemoveDynamicModifier(ItemModifierIndex dynIdx, Player* player, bool load)
{
    RemoveDynamicValue(ITEM_DYNAMIC_MODIFIERS, dynIdx);
    RemoveFlag(ITEM_FIELD_MODIFIERS_MASK, 1 << dynIdx);

    if (!load && player)
        SetState(ITEM_CHANGED, player);
}

uint32 Item::GetBaseItemLevel() const
{
    uint32 itemLevel = GetTemplate()->ItemLevel;
    if (ItemUpgradeEntry const* upgrade = sItemUpgradeStore.LookupEntry(GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_UPGRADE)))
        itemLevel += upgrade->ItemLevelBonus;
    return itemLevel;
}

uint32 Item::GetItemLevel() const
{
    if (m_overrideItemLevel)
        return m_overrideItemLevel;
    return GetBaseItemLevel();
}

bool Item::IsEyeOfTheBlackPrinceAllowed() const
{
    std::set<uint32> const shaTouched = { 86321,86226,86386,86879,86328,86227,86387,86886,86331,86864,86391,86889,86335,86865,98043,
        86893,86988,98070,86905,86990,86906,86910,87156,87164,87166,87168,87170,87173,87176,97296 };

    if (GetTemplate()->Class != ITEM_CLASS_WEAPON)
        return false;
    // Throne of Thuner
    // Base item level, not with upgrades.
    switch (GetTemplate()->ItemLevel)
    {
        case 503: // LFR
        case 522: // Normal
        case 535: // Heroic
        case 541: // Heroic Thunderforged
            return !IsPvPItem();
        case 528: // Normal Thunderforged
            if (IsPvPItem())
                return false;
            if (auto entry = sItemNameDescriptionStore.LookupEntry(GetEntry()))
                if (entry && strcmp(entry->Description[LOCALE_enUS], "Raid Finder") == 0)   // SoO LFR
                    return false;
            return true;
    }
    // Sha-Touched
    if (shaTouched.find(GetEntry()) != shaTouched.end())
        return true;
    return false;
}

void Item::AddToUpdate()
{
    Player* owner = GetOwner();
    if (!owner)
    {
        if (GetOwnerGUID() != 0) // TODO: this is guild bank...no complaints...so, no need to update ?
        {
            TC_LOG_ERROR("shitlog", "Item::AddToUpdate - owner not found, guid %u, entry %u, owner %u\n",
                GetGUID().GetCounter(), GetEntry(), GetOwnerGUID().GetCounter());
        }
        return;
    }

    if (!owner->FindMap())
    {
        TC_LOG_ERROR("shitlog", "Item::AddToUpdate - owner hasn't map, guid %u, entry %u, owner %u\n",
            GetGUID().GetCounter(), GetEntry(), GetOwnerGUID().GetCounter());
        return;
    }

    // if (owner->FindMap() != CurrentMap && CurrentMap)
    // {
    //     TC_LOG_ERROR("shitlog", "Item::AddToUpdate - invalid map, m_currMap ID %u, CurrentMap ID: %u. Object type: %u, entry: %u, GUID: %u, owner: %u (InWorld: %u).\nStack trace:\n%s",
    //         owner->FindMap()->GetId(), CurrentMap->GetId(), uint32(GetTypeId()), GetEntry(), GetGUIDLow(), GUID_LOPART(GetOwnerGUID()), owner->IsInWorld());
    //     return;
    // }

    owner->FindMap()->AddUpdateObject(this);
    m_objectUpdated = true;
}

void Item::RemoveFromUpdate()
{
    Player* owner = ObjectAccessor::FindPlayer(GetOwnerGUID());   // player can be out of world - logout, teleport to cross-server
    if (!owner)
    {
        TC_LOG_ERROR("shitlog", "Item::RemoveFromUpdate - owner not found, guid %u, entry %u, owner %u\n",
            GetGUID().GetCounter(), GetEntry(), GetOwnerGUID().GetCounter());
        return;
    }

    if (!owner->FindMap())
    {
        TC_LOG_ERROR("shitlog", "Item::RemoveFromUpdate - owner hasn't map, guid %u, entry %u, owner %u\n",
            GetGUID().GetCounter(), GetEntry(), GetOwnerGUID().GetCounter());
        return;
    }

    // if (owner->FindMap() != CurrentMap && CurrentMap)
    // {
    //     TC_LOG_ERROR("shitlog", "Item::RemoveFromUpdate - invalid map, m_currMap ID %u, CurrentMap ID: %u. Object type: %u, entry: %u, GUID: %u, owner: %u (InWorld: %u).\nStack trace:\n",
    //         owner->FindMap()->GetId(), CurrentMap->GetId(), uint32(GetTypeId()), GetEntry(), GetGUIDLow(), GUID_LOPART(GetOwnerGUID()), owner->IsInWorld());
    //     return;
    // }

    owner->FindMap()->RemoveUpdateObject(this);
    m_objectUpdated = false;
}

void Item::BuildItemDynamicModifiersData(ByteBuffer* buffer) const
{
    uint32 modifiersMask = GetUInt32Value(ITEM_FIELD_MODIFIERS_MASK);
    uint32 modifiersCount = 0;
    for (uint32 i = 0; i < ITEM_MODIFIER_INDEX_MAX; ++i)
        if (modifiersMask & (1 << i))
            ++modifiersCount;

    *buffer << uint32((modifiersCount * 4) + 4);
    *buffer << uint32(modifiersMask);

    for (uint32 i = 0; i < ITEM_MODIFIER_INDEX_MAX; ++i)
        if (modifiersMask & (1 << i))
            *buffer << GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, i);
}

float ItemTemplate::GetScalingDamageValue(uint32 ilvl) const
{
    if (Quality > ITEM_QUALITY_HEIRLOOM)
        return 0.f;

    ItemDamageEntry const* damageEntry = NULL;

    switch (InventoryType)
    {
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            if (Flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
            {
                damageEntry =  sItemDamageOneHandCasterStore.LookupEntry(ilvl);
                break;
            }
            damageEntry = sItemDamageOneHandStore.LookupEntry(ilvl);
            break;
        case INVTYPE_RANGED:
        case INVTYPE_THROWN:
        case INVTYPE_RANGEDRIGHT:
            if (SubClass < 4)
            {
                if (Flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
                {
                    damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
                    break;
                }
                damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
                break;
            }
            else if (SubClass == 19)
            {
                damageEntry = sItemDamageOneHandCasterStore.LookupEntry(ilvl);
                break;
            }
            else
            {
                if (Flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
                {
                    damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
                    break;
                }
                damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
                break;
            }
        case INVTYPE_AMMO:
            damageEntry = sItemDamageAmmoStore.LookupEntry(ilvl);
            break;
        case INVTYPE_2HWEAPON:
            if (Flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
            {
                damageEntry = sItemDamageTwoHandCasterStore.LookupEntry(ilvl);
                break;
            }
            damageEntry = sItemDamageTwoHandStore.LookupEntry(ilvl);
            break;
        default:
            break;
    }
    return damageEntry ? damageEntry->DPS[Quality == ITEM_QUALITY_HEIRLOOM ? ITEM_QUALITY_RARE : Quality] : 0.f;
}

int32 ItemTemplate::GetRandomPointsOffset() const
{
    switch (InventoryType)
    {
        case INVTYPE_HEAD:
        case INVTYPE_BODY:
        case INVTYPE_CHEST:
        case INVTYPE_LEGS:
        case INVTYPE_RANGED:
        case INVTYPE_2HWEAPON:
        case INVTYPE_ROBE:
        case INVTYPE_THROWN:
            return 0;
        case INVTYPE_RANGEDRIGHT:
            if (SubClass == ITEM_SUBCLASS_WEAPON_WAND)
                return 3;
            return 0;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            return 3;
        case INVTYPE_SHOULDERS:
        case INVTYPE_WAIST:
        case INVTYPE_FEET:
        case INVTYPE_HANDS:
        case INVTYPE_TRINKET:
            return 1;
        case INVTYPE_NECK:
        case INVTYPE_WRISTS:
        case INVTYPE_FINGER:
        case INVTYPE_SHIELD:
        case INVTYPE_CLOAK:
        case INVTYPE_HOLDABLE:
            return 2;
        case INVTYPE_RELIC:
            return 4;
        default:
            return -1;
    }
}

uint32 ItemTemplate::CalculateScalingStatValue(uint32 ilvl) const
{
    int32 offset = GetRandomPointsOffset();
    if (offset == -1)
        return 0;

    RandomPropertiesPointsEntry const* randomProperty = sRandomPropertiesPointsStore.LookupEntry(ilvl);
    if (!randomProperty)
        return 0;

    switch (Quality)
    {
        case ITEM_QUALITY_UNCOMMON:
            return randomProperty->UncommonPropertiesPoints[offset];
        case ITEM_QUALITY_RARE:
            return randomProperty->RarePropertiesPoints[offset];
        case ITEM_QUALITY_EPIC:
        case ITEM_QUALITY_LEGENDARY:
        case ITEM_QUALITY_ARTIFACT:
            return randomProperty->EpicPropertiesPoints[offset];
        default:
            break;
    }
    return 0;
}

uint32 ItemTemplate::CalculateArmor(uint32 ilvl) const
{
    if (ilvl == ItemLevel)
        return Armor;

    uint32 quality = Quality == ITEM_QUALITY_HEIRLOOM ? ITEM_QUALITY_RARE : Quality;

    if (Class != ITEM_CLASS_ARMOR || SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        if (InventoryType == 1 || InventoryType == 5 || InventoryType == 3 || InventoryType == 7 || InventoryType == 8 || InventoryType == 9 || InventoryType == 10 || InventoryType == 6 || InventoryType == 16 || InventoryType == 20)
        {
            ItemArmorQualityEntry const* armorQuality = sItemArmorQualityStore.LookupEntry(ilvl);
            ItemArmorTotalEntry const* armorTotal = sItemArmorTotalStore.LookupEntry(ilvl);
            ArmorLocationEntry const* armorLoc = sArmorLocationStore.LookupEntry(InventoryType == 20 ? 5 : InventoryType);

            if (SubClass == 0 || SubClass > 4)
                return 0.0f;

            return (int)floor(armorQuality->Value[quality] * armorTotal->Value[SubClass - 1] * armorLoc->Value[SubClass - 1] + 0.5f);
        }
        return 0;
    }
    else
    {
        ItemArmorShieldEntry const* shieldEntry = sItemArmorShieldStore.LookupEntry(ilvl);
        return shieldEntry->Value[quality];
    }
}

void ItemTemplate::CalculateMinMaxDamageScaling(uint32 ilvl, uint32& minDamage, uint32& maxDamage) const
{
    minDamage = 0;
    maxDamage = 0;

    if (ilvl == ItemLevel)
    {
        minDamage = DamageMin;
        maxDamage = DamageMax;
        return;
    }

    if (Class != ITEM_CLASS_WEAPON)
        return;

    float weaponMinDamageCalc = (float)Delay * GetScalingDamageValue(ilvl) * 0.001f;
    float weaponMaxDamageCalc = (((StatScalingFactor * 0.5f) + 1.f) * weaponMinDamageCalc) + 0.5f;

    if (Delay != 0)
    {
        float delayModifier = 1000.0f / (float)Delay;
        float midCalc = (delayModifier * ((1.f - (StatScalingFactor * 0.5f)) * weaponMinDamageCalc)) + ArmorDamageModifier;
        midCalc = midCalc > 1.f ? midCalc : 1.f;
        float delayCoeff = 1.f / delayModifier;
        minDamage = floor((delayCoeff * midCalc) + 0.5f);
        if (ItemId == 19019)
            maxDamage = abs(floor((delayCoeff * ((delayModifier * weaponMaxDamageCalc) + ArmorDamageModifier)) + 0.5f));
        else
            maxDamage = floor((delayCoeff * ((delayModifier * weaponMaxDamageCalc) + ArmorDamageModifier)) + 0.5f);
    }
    else
    {
        maxDamage = floor(weaponMaxDamageCalc + 0.5f);
        minDamage = floor(((1.f - (StatScalingFactor * 0.5f)) * weaponMinDamageCalc) + 0.5f);
    }
}

bool ItemTemplate::IsUsableByLootSpecialization(Player const* player, bool alwaysAllowBoundToAccount) const
{
    if (Flags & ITEM_PROTO_FLAG_BIND_TO_ACCOUNT && alwaysAllowBoundToAccount)
        return true;

    // TODO: fall back to GetDefaultSpecId
    uint32 spec = player->GetLootSpecOrClassSpec();

    ChrSpecializationEntry const* chrSpecialization = sChrSpecializationStore.LookupEntry(spec);
    if (!chrSpecialization)
        return false;

    std::size_t levelIndex = 0;
    if (player->GetLevel() >= 110)
        levelIndex = 2;
    else if (player->GetLevel() > 40)
        levelIndex = 1;

    return Specializations[levelIndex].test(CalculateItemSpecBit(chrSpecialization));
}

std::size_t ItemTemplate::CalculateItemSpecBit(ChrSpecializationEntry const* spec)
{
    return (spec->classId - 1) * MAX_SPECIALIZATIONS + spec->TabPage;
}
