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
#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "ServiceBoost.h"
#include "BattlePetMgr.h"

#pragma execution_character_set("UTF-8")


BattlePayMgr::BattlePayMgr() : m_currency(BATTLE_PAY_CURRENCY_BETA), m_enabled(false)
{
    m_purchase = new PurchaseInfo();
}

BattlePayMgr::~BattlePayMgr()
{
    for (auto&& product : m_productStore)
        delete product;

    for (auto&& group : m_groupStore)
        delete group;

    for (auto&& entry : m_shopEntryStore)
        delete entry;

    m_productStore.clear();
    m_groupStore.clear();
    m_shopEntryStore.clear();

    delete m_purchase;
}

BattlePayMgr* BattlePayMgr::instance()
{
    static BattlePayMgr instance;
    return &instance;
}

void BattlePayMgr::LoadFromDb()
{
    LoadProductsFromDb();
    LoadProductLocalesFromDb();
    LoadProductItemsFromDb();
    LoadGroupsFromDb();
    LoadGroupLocalesFromDb();
    LoadEntriesFromDb();
    LoadEntryLocalesFromDb();
}

void BattlePayMgr::LoadProductsFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_productStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, title, description, icon, price, discount, displayId, type, choiceType, flags, flagsInfo FROM battle_pay_product ORDER BY id ASC");
    if (!result)
    {
        TC_LOG_INFO("sql.sql", ">> Loaded 0 Battle Pay store products, table `battle_pay_product` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string title       = fields[1].GetString();
        std::string description = fields[2].GetString();
        uint32 icon             = fields[3].GetUInt32();
        uint32 price            = fields[4].GetUInt32();
        uint32 discount         = fields[5].GetUInt32();
        uint32 displayId        = fields[6].GetUInt32();
        uint8 type              = fields[7].GetUInt8();
        uint8 choiceType        = fields[8].GetUInt8();
        uint32 flags            = fields[9].GetUInt32();
        uint32 flagsInfo        = fields[10].GetUInt32();

        if (GetProductId(id))
        {
            TC_LOG_ERROR("sql.sql", "Product id %u defined in `battle_pay_product` already exists, skipped!", id);
            continue;
        }

        if (title.size() > MAX_BATTLE_PAY_PRODUCT_TITLE_SIZE)
        {
            TC_LOG_ERROR("sql.sql", "Title for product id %u defined in `battle_pay_product` is too large (max %d), skipped!", id, MAX_BATTLE_PAY_PRODUCT_TITLE_SIZE);
            continue;
        }

        if (description.size() > MAX_BATTLE_PAY_PRODUCT_DESCRIPTION_SIZE)
        {
            TC_LOG_ERROR("sql.sql", "Description for product id %u defined in `battle_pay_product` is too large (max %d), skipped!", id, MAX_BATTLE_PAY_PRODUCT_DESCRIPTION_SIZE);
            continue;
        }

        if (discount > 99)
        {
            TC_LOG_ERROR("sql.sql", "Product id %u defined in `battle_pay_product` has to large discount %u (max 99), skipped!", id, discount);
            discount = 0;
            continue;
        }

        m_productStore.push_back(new BattlePayProduct(id, title, description, icon, price, discount, displayId, type, choiceType, flags, flagsInfo));
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pay store products in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadProductLocalesFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_productLocalesMap.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, title_loc1, description_loc1, title_loc2, description_loc2, title_loc3, description_loc3, title_loc4, description_loc4, title_loc5, description_loc5,"
        " title_loc6, description_loc6, title_loc7, description_loc7, title_loc8, description_loc8, title_loc9, description_loc9, title_loc10, description_loc10, title_loc11, description_loc11"
        " FROM locales_battle_pay_product");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Battle Pay store products locale strings. DB table `locales_battle_pay_product` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        if (!GetProductId(id))
        {
            TC_LOG_ERROR("sql.sql", "Table `locales_battle_pay_product` (Entry: %u) has locale strings for non-existing Battle Pay product.", id);
            continue;
        }

        BattlePayProductLocale& data = m_productLocalesMap[id];

        for (int i = 1; i < TOTAL_LOCALES; ++i)
        {
            LocaleConstant locale = (LocaleConstant)i;
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1)].GetString(), locale, data.Title);
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1) + 1].GetString(), locale, data.Description);
        }
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battle Pay store products locale strings in %u ms", (unsigned long)m_productLocalesMap.size(), GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadProductItemsFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_productItemsMap.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, itemId, count, productId FROM battle_pay_product_items ORDER BY id ASC");
    if (!result)
    {
        TC_LOG_INFO("sql.sql", ">> Loaded 0 Battle Pay store product items, table `battle_pay_product_items` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        uint32 itemId = fields[1].GetUInt32();
        uint32 itemCount = fields[2].GetUInt32();
        uint32 productId = fields[3].GetUInt32();

        if (!GetProductId(productId))
        {
            TC_LOG_ERROR("sql.sql", "Product id %u defined in `battle_pay_product_items` not exists, skipped!", productId);
            continue;
        }

        if (itemId && !sObjectMgr->GetItemTemplate(itemId))
        {
            TC_LOG_ERROR("sql.sql", "Item id %u for product id %u defined in `battle_pay_product_items` doesn't exist, skipped!", itemId, productId);
            continue;
        }

        m_productItemsMap[productId].push_back(BattlePayProductItem(id, itemId, itemCount));
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pay store product items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadGroupsFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_groupStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, idx, name, icon, type FROM battle_pay_group ORDER BY type ASC");
    if (!result)
    {
        TC_LOG_INFO("sql.sql", ">> Loaded 0 Battle Pay store groups, table `battle_pay_group` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 id        = fields[0].GetUInt32();
        uint32 index     = fields[1].GetUInt32();
        std::string name = fields[2].GetString();
        uint32 icon      = fields[3].GetUInt32();
        uint8 type       = fields[4].GetUInt8();

        if (HasGroupId(id))
        {
            TC_LOG_ERROR("sql.sql", "Group id %u defined in `battle_pay_group` already exists, skipped!", id);
            continue;
        }

        if (HasGroupName(name))
        {
            TC_LOG_ERROR("sql.sql", "Group name %s defined in `battle_pay_group` is invalid because a group of the same name already exists, skipped!", name.c_str());
            continue;
        }

        if (name.size() > MAX_BATTLE_PAY_GROUP_NAME_SIZE)
        {
            TC_LOG_ERROR("sql.sql", "Name for group id %u defined in `battle_pay_group` is too large (max %d), skipped!",
                id, MAX_BATTLE_PAY_GROUP_NAME_SIZE);

            continue;
        }

        if (type >= BATTLE_PAY_GROUP_TYPE_END)
        {
            TC_LOG_ERROR("sql.sql", "Group id %u defined in `battle_pay_group` has invalid group type %u, skipped!", id, type);
            continue;
        }

        m_groupStore.push_back(new BattlePayGroup(id, index, name, icon, type));
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pay store groups in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadGroupLocalesFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_groupLocalesMap.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, name_loc1, name_loc2, name_loc3, name_loc4, name_loc5, name_loc6, name_loc7, name_loc8, name_loc9, name_loc10, name_loc11"
        " FROM locales_battle_pay_group");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Battle Pay store groups locale strings. DB table `locales_battle_pay_group` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        if (!HasGroupId(id))
        {
            TC_LOG_ERROR("sql.sql", "Table `locales_battle_pay_group` (Entry: %u) has locale strings for non-existing Battle Pay group.", id);
            continue;
        }

        BattlePayGroupLocale& data = m_groupLocalesMap[id];

        for (int i = 1; i < TOTAL_LOCALES; ++i)
            ObjectMgr::AddLocaleString(fields[i].GetString(), LocaleConstant(i), data.Name);

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battle Pay store groups locale strings in %u ms", (unsigned long)m_groupLocalesMap.size(), GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadEntriesFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_shopEntryStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, productId, groupId, idx, title, description, icon, displayId, banner, `flags` FROM battle_pay_entry ORDER BY id ASC");
    if (!result)
    {
        TC_LOG_INFO("sql.sql", ">> Loaded 0 Battle Pay store entries, table `battle_pay_entry` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        uint32 productId        = fields[1].GetUInt32();
        uint32 groupId          = fields[2].GetUInt32();
        int32 index             = fields[3].GetInt32();
        std::string title       = fields[4].GetString();
        std::string description = fields[5].GetString();
        uint32 icon             = fields[6].GetUInt32();
        uint32 displayId        = fields[7].GetUInt32();
        uint8 banner           = fields[8].GetUInt8();
        uint32 flags            = fields[9].GetUInt32();

        if (HasEntryId(id))
        {
            TC_LOG_ERROR("sql.sql", "Entry id %u defined in `battle_pay_entry` already exists, skipped!", id);
            continue;
        }

        if (!HasGroupId(groupId))
        {
            TC_LOG_ERROR("sql.sql", "Group id %u for entry id %u defined in `battle_pay_entry` is invalid because the group doesn't exists, skipped!", groupId, id);
            continue;
        }

        if (!GetProductId(productId))
        {
            TC_LOG_ERROR("sql.sql", "Product id %u for entry id %u defined in `battle_pay_entry` is invalid because the group doesn't exists, skipped!", productId, id);
            continue;
        }

        if (banner >= BATTLE_PAY_BANNER_TYPE_END)
        {
            TC_LOG_ERROR("sql.sql", "Entry id %u defined in `battle_pay_entry` has invalid banner type %u, skipped!", id, banner);
            continue;
        }

        m_shopEntryStore.push_back(new BattlePayShopEntry(id, productId, groupId, index, title, description, icon, displayId, banner, flags));
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pay store entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlePayMgr::LoadEntryLocalesFromDb()
{
    uint32 oldMSTime = getMSTime();

    m_shopEntryLocalesMap.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, title_loc1, description_loc1, title_loc2, description_loc2, title_loc3, description_loc3, title_loc4, description_loc4, title_loc5, description_loc5,"
        " title_loc6, description_loc6, title_loc7, description_loc7, title_loc8, description_loc8, title_loc9, description_loc9, title_loc10, description_loc10, title_loc11, description_loc11"
        " FROM locales_battle_pay_entry");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Battle Pay store entries locale strings. DB table `locales_battle_pay_entry` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        if (!HasEntryId(id))
        {
            TC_LOG_ERROR("sql.sql", "Table `locales_battle_pay_entry` (Entry: %u) has locale strings for non-existing Battle Pay entry.", id);
            continue;
        }

        BattlePayShopEntryLocale& data = m_shopEntryLocalesMap[id];

        for (int i = 1; i < TOTAL_LOCALES; ++i)
        {
            LocaleConstant locale = (LocaleConstant)i;
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1)].GetString(), locale, data.Title);
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1) + 1].GetString(), locale, data.Description);
        }
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battle Pay store entries locale strings in %u ms", (unsigned long)m_shopEntryLocalesMap.size(), GetMSTimeDiffToNow(oldMSTime));
}

BattlePayProduct* BattlePayMgr::GetProductId(uint32 id)
{
    for (auto&& itr : m_productStore)
        if (itr->Id == id)
            return itr;

    return nullptr;
}

bool BattlePayMgr::HasGroupId(uint32 id)
{
    for (auto&& group : m_groupStore)
        if (group->Id == id)
            return true;

    return false;
}

bool BattlePayMgr::HasGroupName(std::string name)
{
    for (auto&& group : m_groupStore)
        if (group->Name == name)
            return true;

    return false;
}

bool BattlePayMgr::HasEntryId(uint32 id)
{
    for (auto&& itemEntry : m_shopEntryStore)
        if (itemEntry->Id == id)
            return true;

    return false;
}

void BattlePayMgr::SendPointsBalance(WorldSession* session)
{
    if (Player* player = session->GetPlayer())
    {
        std::ostringstream data;
        data << float(player->GetDonateTokens()) / BATTLE_PAY_CURRENCY_PRECISION;
        player->SendBattlePayMessage(sObjectMgr->GetTrinityString(15005, session->GetSessionDbLocaleIndex()),data);
    }
}

void BattlePayMgr::UpdatePointsBalance(WorldSession* session, uint64 points)
{
    if (Player* player = session->GetPlayer())
    {
        player->DestroyDonateTokenCount(points);

        std::ostringstream data1;
        data1 << float(player->GetDonateTokens()) / BATTLE_PAY_CURRENCY_PRECISION;
        player->SendBattlePayMessage(sObjectMgr->GetTrinityString(15006, session->GetSessionDbLocaleIndex()),data1);
    }
    else
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BATTLEPAY_DECREMENT_COINS);
        stmt->setUInt32(0, points);
        stmt->setUInt32(1, session->GetAccountId());    
        LoginDatabase.Query(stmt);
    }
}

bool BattlePayMgr::HasPointsBalance(WorldSession* session, uint64 points)
{
    if(Player* player = session->GetPlayer())
    {
        uint64 balance = player->GetDonateTokens();

        if(balance >= points)
        {
            return true;
        }
        else
        {
            player->SendBattlePayMessage(sObjectMgr->GetTrinityString(15007, session->GetSessionDbLocaleIndex()));
            return false;
        }
    }
    else
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BATTLEPAY_COINS);
        stmt->setUInt32(0, session->GetAccountId());
        PreparedQueryResult result_don = LoginDatabase.Query(stmt);

        if (!result_don)
            return false;

        Field* fields = result_don->Fetch();
        uint64 balans = fields[0].GetUInt32();

        if(balans >= points)
            return true;

        return false;
    }
    return false;
}

void BattlePayMgr::RegisterPurchase(PurchaseInfo* purchase, uint32 item, uint64 price)
{
  // Register Purchase
    LoginDatabase.PExecute("INSERT INTO battlepay_log (accountId, characterGuid, realm, item, price) VALUES (%u, %u, %u, %u, %u);", purchase->GetSession()->GetAccountId(), purchase->SelectedPlayer.GetCounter(), realm.Id.Realm, item, price);
}

void BattlePayMgr::Update(uint32 diff)
{
    if (m_sendPacket)
    {
        if (m_timer <= diff)
        {
            m_sendPacket = false;
            if (GetPurchaseInfo()->GetSession())
                SendBattlePayPurchaseUpdate(GetPurchaseInfo());
        }
        else
            m_timer -= diff;
    }
}

void BattlePayMgr::SendBattlePayDistributionList(WorldSession* session)
{
    // TODO: finish this
    WorldPacket data(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE, 4 + 4);
    data << uint32(0);
    data.WriteBits(0, 19);
    data.FlushBits();
    session->SendPacket(&data);
}

void BattlePayMgr::SendBattlePayPurchaseList(WorldSession* session)
{
    // TODO: finish this
    WorldPacket data(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE, 4 + 4);
    data.WriteBits(0, 19);
    data.FlushBits();
    data << uint32(0);
    session->SendPacket(&data);
}

void BattlePayMgr::SendBattlePayProductList(WorldSession* session)
{
    bool hasItemInfo = false, unkBit1 = false, unkBit2 = false, unkBit3 = false, unkBit4 = false, hasBattlePetResult = false, unkBit5 = false, unkBit6 = false, unkBit7 = false;

    WorldPacket data(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE);
    data.WriteBits(m_shopEntryStore.size(), 19);
    data.WriteBits(m_productStore.size(), 19);

    for (auto&& product : m_productStore)
    {
        BattlePayProductItemsVector const* items = GetItemsByProductId(product->Id);

        std::string productTitle = product->Title;
        std::string productDescription = product->Description;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayProductLocale const* locProd = GetProductLocale(product->Id))
            {
                ObjectMgr::GetLocaleString(locProd->Title, localeConstant, productTitle);
                ObjectMgr::GetLocaleString(locProd->Description, localeConstant, productDescription);
            }

        data.WriteBits(product->ChoiceType, 2);
        data.WriteBits(items->size(), 20);

        for (auto&& item : *items)
        {
            bool hasProduct = false;
            if (product->Type == BATTLE_PAY_PRODUCT_TYPE_SERVICE)
            {
                if (product->Id == BATTLE_PAY_SERVICE_BOOST)
                    if (session->HasBoost())
                        hasProduct = true;
            }
            else if (product->Type == BATTLE_PAY_PRODUCT_TYPE_ITEM)
            {
                ItemTemplate const* itemTemp = sObjectMgr->GetItemTemplate(item.ItemId);
                uint32 productSpell = itemTemp ? (itemTemp->Spells ? (itemTemp->Spells[0].SpellTrigger == 6 ? itemTemp->Spells[0].SpellId : itemTemp->Spells[1].SpellId) : 0) : 0;
                if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(productSpell))
                {
                    if (spell->IsAbilityOfSkillType(SKILL_MOUNTS))
                    {
                        if (Player* player = session->GetPlayer())
                        {
                            if (player->HasSpell(productSpell))
                                hasProduct = true;
                        }
                        else
                        {
                            if (CharacterDatabase.PQuery("SELECT spell FROM account_spell WHERE account = '%u' AND spell = '%u'", session->GetAccountId(), productSpell))
                                hasProduct = true;
                        }
                    }
                    else if (spell->IsAbilityOfSkillType(SKILL_COMPANIONS))
                    {
                        uint16 speciesId = sObjectMgr->BattlePetGetSpeciesFromItem(itemTemp->ItemId);
                        if (Player* player = session->GetPlayer())
                        {
                            if (player->GetBattlePetMgr().GetBattlePetCount(speciesId))
                                hasProduct = true;
                        }
                        else
                        {
                            if (CharacterDatabase.PQuery("SELECT species FROM account_battle_pet WHERE accountId = '%u' AND species = '%u'", session->GetAccountId(), speciesId))
                                hasProduct = true;
                        }
                    }
                }
            }

            data.WriteBit(false);
            data.WriteBit(hasProduct);
            data.WriteBit(hasItemInfo);
            if (hasItemInfo)
            {
                data.WriteBits(0, 10);
                data.WriteBits(0, 13);
                data.WriteBit(unkBit1);
                data.WriteBit(unkBit2);
                data.WriteBit(unkBit3);
                data.WriteBit(unkBit4);
                data.WriteBits(0, 10);
            }
            data.WriteBit(hasBattlePetResult);
            if (hasBattlePetResult)
                data.WriteBits(0, 4);
        }
        data.WriteBit(!productTitle.empty() ? true : false);
        if (!productTitle.empty())
        {
            data.WriteBit(product->DisplayId ? true : false);
            data.WriteBits(productTitle.size(), 10);
            data.WriteBits(productDescription.size(), 13);
            data.WriteBits(0, 10);
            data.WriteBit(product->FlagsInfo ? true : false);
            data.WriteBit(unkBit5);
            data.WriteBit(product->Icon ? true : false);
        }
    }

    data.WriteBits(m_groupStore.size(), 20);

    for (auto&& entry : m_shopEntryStore)
    {
        std::string entryTitle = entry->Title;
        std::string entryDescription = entry->Description;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayShopEntryLocale const* locEntry = GetShopEntryLocale(entry->Id))
            {
                ObjectMgr::GetLocaleString(locEntry->Title, localeConstant, entryTitle);
                ObjectMgr::GetLocaleString(locEntry->Description, localeConstant, entryDescription);
            }

        data.WriteBit(!entryTitle.empty() ? true : false);
        if (!entryTitle.empty())
        {
            data.WriteBits(entryTitle.size(), 10);
            data.WriteBit(unkBit6);
            data.WriteBits(0, 10);
            data.WriteBits(entryDescription.size(), 13);
            data.WriteBit(entry->DisplayId);
            data.WriteBit(entry->Icon);
            data.WriteBit(unkBit7);
        }
    }

    for (auto&& group : m_groupStore)
    {
        std::string groupName = group->Name;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayGroupLocale const* locGroup = GetGroupLocale(group->Id))
                ObjectMgr::GetLocaleString(locGroup->Name, localeConstant, groupName);

        data.WriteBits(groupName.size(), 8);
    }

    data.FlushBits();

    for (auto&& group : m_groupStore)
    {
        std::string groupName = group->Name;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayGroupLocale const* locGroup = GetGroupLocale(group->Id))
                ObjectMgr::GetLocaleString(locGroup->Name, localeConstant, groupName);

        data << uint32(group->Index);
        data.WriteString(groupName);
        data << uint8(group->Type);
        data << uint32(group->Icon);
        data << uint32(group->Id);
    }

    for (auto&& product : m_productStore)
    {
        BattlePayProductItemsVector const* items = GetItemsByProductId(product->Id);

        std::string productTitle = product->Title;
        std::string productDescription = product->Description;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayProductLocale const* locProd = GetProductLocale(product->Id))
            {
                ObjectMgr::GetLocaleString(locProd->Title, localeConstant, productTitle);
                ObjectMgr::GetLocaleString(locProd->Description, localeConstant, productDescription);
            }

        data << uint8(product->Type);
        for (auto&& item : *items)
        {
            if (hasItemInfo)
            {
                if (unkBit2)
                    data << uint32(0);
                if (unkBit3)
                    data << uint32(0);
                if (unkBit4)
                    data << uint32(0);
                data.WriteString(std::string());
                if (unkBit1)
                    data << uint32(0);
                data.WriteString(std::string());
                data.WriteString(std::string());
            }
            data << uint32(item.ItemId);
            data << uint32(item.Count);
            data << uint32(item.Id);
        }
        if (!productTitle.empty())
        {
            if (product->DisplayId)
                data << uint32(product->DisplayId);
            if (product->Icon)
                data << uint32(product->Icon);
            data.WriteString(std::string());
            data.WriteString(productTitle);
            if (unkBit5)
                data << uint32(0);
            if (product->FlagsInfo)
                data << uint32(product->FlagsInfo);
            data.WriteString(productDescription);
        }
        data << uint32(product->Id);
        data << uint32(product->Flags);

        uint64 price = product->Price * BATTLE_PAY_CURRENCY_PRECISION;
        float discount = float(product->Discount) / 100;
        uint64 currentPrice = price - (price * discount);

        data << uint64(price);
        data << uint64(currentPrice);
    }

    for (auto&& entry : m_shopEntryStore)
    {
        std::string entryTitle = entry->Title;
        std::string entryDescription = entry->Description;
        LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (BattlePayShopEntryLocale const* locEntry = GetShopEntryLocale(entry->Id))
            {
                ObjectMgr::GetLocaleString(locEntry->Title, localeConstant, entryTitle);
                ObjectMgr::GetLocaleString(locEntry->Description, localeConstant, entryDescription);
            }

        if (!entryTitle.empty())
        {
            if (unkBit6)
                data << uint32(0);
            data.WriteString(std::string());
            if (unkBit7)
                data << uint32(unkBit7);
            if (entry->Icon)
                data << uint32(entry->Icon);
            if (entry->DisplayId)
                data << uint32(entry->DisplayId);
            data.WriteString(entryDescription);
            data.WriteString(entryTitle);
        }

        data << uint32(entry->Flags);
        data << uint8(entry->Banner);
        data << uint32(entry->Id);
        data << int32(entry->Index);
        data << uint32(entry->GroupId);
        data << uint32(entry->ProductId);
    }
    
    data << uint32(GetStoreCurrency());
    data << uint32(BATTLE_PAY_STATUS_OK);

    session->SendPacket(&data);
}

void BattlePayMgr::SendBattlePayDistributionUpdate(WorldSession* session, uint32 productId, uint8 status)
{
    ObjectGuid guid = session->GetPlayer() ? session->GetPlayer()->GetGUID() : ObjectGuid(HighGuid::WowAccount, session->GetAccountId());
    ObjectGuid guid2 = ObjectGuid::Empty;

    BattlePayProduct* product = GetProductId(productId);
    bool HasBattlePayProduct = product;

    std::string title = product->Title;
    std::string description = product->Description;
    LocaleConstant localeConstant = session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
        if (BattlePayShopEntryLocale const* locEntry = GetShopEntryLocale(product->Id))
        {
            ObjectMgr::GetLocaleString(locEntry->Title, localeConstant, title);
            ObjectMgr::GetLocaleString(locEntry->Description, localeConstant, description);
        }

    WorldPacket data(SMSG_BATTLE_PAY_DISTRIBUTION_UPDATE);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(HasBattlePayProduct);
    data.WriteBit(guid[1]);
    data.WriteBit(guid2[4]);
    data.WriteBit(guid2[7]);
    data.WriteBit(guid2[0]);
    data.WriteBit(0); // Revoked
    data.WriteBit(guid2[1]);
    data.WriteBit(guid2[2]);
    if (HasBattlePayProduct)
    {
        data.WriteBits(1, 2);
        data.WriteBits(0, 20);
        data.WriteBit(!title.empty());
        if (!title.empty())
        {
            data.WriteBits(0, 10);
            data.WriteBit(0);
            data.WriteBits(title.length(), 10);
            data.WriteBit(0);
            data.WriteBit(0);
            data.WriteBits(description.length(), 13);
            data.WriteBit(0); // bool11
        }
    }

    data.WriteBit(guid[7]);
    data.WriteBit(guid2[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid2[5]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid[4]);

    data.FlushBits();

    if (HasBattlePayProduct)
    {
        data << int32(0);
        data << int64(0);
        data.WriteString(description);
        data.WriteString(title);
        // if (bool11)
            //data << int32(0);
        data << int64(0);
        data << int8(product->Type);
        data << int32(0);
    }

    data << int32(product->Id);
    data.WriteByteSeq(guid2[4]);
    data << int64(0);
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);
    data << int32(0);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid2[0]);
    data.WriteByteSeq(guid2[7]);
    data << int32(0);
    data << int32(status);
    data.WriteByteSeq(guid2[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid2[3]);
    data.WriteByteSeq(guid2[2]);


    session->SendPacket(&data);
}

void BattlePayMgr::SendBattlePayPurchaseUpdate(PurchaseInfo* purchase)
{
    if (!purchase->GetSession())
        return;

    if (purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_READY && purchase->ResultCode == BATTLE_PAY_RESULT_OK)
    {
        WorldPacket data(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE);
        data << uint64(purchase->PurchaseId);
        data << uint32(1); // PurchaseResult
        data << uint32(purchase->ClientToken);
        purchase->GetSession()->SendPacket(&data);
    }

    std::string wallet = purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY || purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_BUYING || purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_BUYED ? "Account" : "";

    WorldPacket data(SMSG_BATTLE_PAY_PURCHASE_UPDATE);
    data.WriteBits(1, 19); // PurchaseCount
    for (uint8 i = 0; i < 1; i++)
        data.WriteBits(wallet.size(), 8);
    data.FlushBits();
    for (uint8 i = 0; i < 1; i++)
    {
        data << uint32(purchase->ResultCode);
        data << uint64(purchase->PurchaseId);
        data << uint32(purchase->ProductId);
        data << uint32(purchase->PurchaseStatus);
        data.WriteString(wallet);
    }
    purchase->GetSession()->SendPacket(&data);

    if (purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_READY && purchase->ResultCode == BATTLE_PAY_RESULT_OK)
    {
        if (sBattlePayMgr->IsStoreEnabled())
            sBattlePayMgr->AddQuenedPurchaseResponse(new PurchaseInfo(purchase->GetSession(), purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY, BATTLE_PAY_RESULT_OK, purchase->ClientToken, 0, false), 500, true);
        else
            sBattlePayMgr->AddQuenedPurchaseResponse(new PurchaseInfo(purchase->GetSession(), purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_READY, BATTLE_PAY_RESULT_UNAVAILABLE, purchase->ClientToken, 0, false), 500, true);
    }

    if (purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY && purchase->ResultCode == BATTLE_PAY_RESULT_OK)
    {
        bool validPurchase = true;
        BattlePayProduct* product = GetProductId(purchase->ProductId);
        if (product->Id == BATTLE_PAY_SERVICE_BOOST && purchase->GetSession()->HasBoost())
            validPurchase = false;

        uint32 serverToken = irand(1, 999999); // temp solution
        
        uint64 price = product->Price * BATTLE_PAY_CURRENCY_PRECISION;
        float discount = float(product->Discount) / 100;
        uint64 currentPrice = price - (price * discount);

        data.Initialize(SMSG_BATTLE_PAY_CONFIRM_PURCHASE);
        data << uint32(serverToken);
        data << uint64(currentPrice);
        data << uint64(purchase->PurchaseId);
        purchase->GetSession()->SendPacket(&data);

        GetPurchaseInfo()->ServerToken = serverToken;

        if(product->Flags == BATTLE_PAY_PRODUCT_DISABLES)
            validPurchase = false;
        else
        {
            bool confirmBalance = HasPointsBalance(purchase->GetSession(), (currentPrice));
            if(!confirmBalance)
                validPurchase = false;
        }        

        if (!validPurchase)
            sBattlePayMgr->AddQuenedPurchaseResponse(new PurchaseInfo(purchase->GetSession(), purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY, BATTLE_PAY_RESULT_SHOP_ERROR, purchase->ClientToken, 0, false), 200, true);
    }

    if (purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_BUYING && !purchase->Buyed)
    {
        data.Initialize(SMSG_BATTLE_PAY_DELIVERY_ENDED);
        data.WriteBits(1, 22); // count
        for (uint8 i = 0; i < 1; i++)
        {
            BattlePayProductItemsVector const* items = GetItemsByProductId(purchase->ProductId);
            data << uint32(items->front().ItemId);
        }
        data << uint64(0); // DistributionID
        purchase->GetSession()->SendPacket(&data);

        AddQuenedPurchaseResponse(new PurchaseInfo(purchase->GetSession(), purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_BUYING, BATTLE_PAY_RESULT_OK, purchase->ClientToken, purchase->ServerToken, true), 1000, true);
    }

    if (purchase->PurchaseStatus == BATTLE_PAY_PURCHASE_STATUS_BUYING && purchase->Buyed)
    {
        BattlePayProduct* product = GetProductId(purchase->ProductId);
        uint32 itemid = GetItemsByProductId(purchase->ProductId)->front().ItemId;

        if (product->Type == BATTLE_PAY_PRODUCT_TYPE_SERVICE)
        {
            if (product->Id == BATTLE_PAY_SERVICE_BOOST)
                if (!purchase->GetSession()->HasBoost())
                {
                    SetBoosting(purchase->GetSession(), purchase->GetSession()->GetAccountId(), true);
                    SendBattlePayDistributionUpdate(purchase->GetSession(), BATTLE_PAY_SERVICE_BOOST, CHARACTER_BOOST_ALLOW);
                }
        }
        else if (product->Type == BATTLE_PAY_PRODUCT_TYPE_ITEM)
        {
            if (!purchase->GetSession()->GetPlayer())
            {
                uint32 mailId = sObjectMgr->GenerateMailID();

                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

                // not blizzlike, but who cares (temp solution)
                std::string productTitle = product->Title;
                std::string productDescription = product->Description;
                LocaleConstant localeConstant = purchase->GetSession()->GetSessionDbLocaleIndex();
                if (localeConstant != LOCALE_enUS)
                    if (BattlePayProductLocale const* locProd = GetProductLocale(product->Id))
                    {
                        ObjectMgr::GetLocaleString(locProd->Title, localeConstant, productTitle);
                        ObjectMgr::GetLocaleString(locProd->Description, localeConstant, productDescription);
                    }

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL);
                stmt->setUInt32(0, mailId);
                stmt->setUInt8(1, MAIL_NORMAL);
                stmt->setInt8(2, MAIL_STATIONERY_DEFAULT);
                stmt->setUInt16(3, 0);
                stmt->setUInt32(4, purchase->SelectedPlayer.GetCounter());
                stmt->setUInt32(5, purchase->SelectedPlayer.GetCounter());
                stmt->setString(6, productTitle);
                stmt->setString(7, productDescription);
                stmt->setBool(8, true);
                stmt->setUInt64(9, time(NULL) + 180 * DAY);
                stmt->setUInt64(10, time(NULL));
                stmt->setUInt32(11, 0);
                stmt->setUInt32(12, 0);
                stmt->setUInt8(13, 0);
                trans->Append(stmt);

                if (Item* item = Item::CreateItem(itemid, 1, 0))
                {
                    item->SaveToDB(trans);

                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);
                    stmt->setUInt32(0, mailId);
                    stmt->setUInt32(1, item->GetGUID().GetCounter());
                    stmt->setUInt32(2, purchase->SelectedPlayer);
                    trans->Append(stmt);
                }

                CharacterDatabase.CommitTransaction(trans);
            }
            else
            {
                purchase->GetSession()->GetPlayer()->AddItem(itemid, 1);
                // save PJ
                purchase->GetSession()->GetPlayer()->SaveToDB();
            }
        }

        uint64 price = product->Price * BATTLE_PAY_CURRENCY_PRECISION;
        float discount = float(product->Discount) / 100;
        uint64 currentPrice = price - (price * discount);

        UpdatePointsBalance(purchase->GetSession(), (currentPrice));

        RegisterPurchase(purchase, itemid, currentPrice);
        
        SendBattlePayPurchaseUpdate(new PurchaseInfo(purchase->GetSession(), purchase->SelectedPlayer, purchase->PurchaseId, purchase->ProductId, BATTLE_PAY_PURCHASE_STATUS_BUYED, BATTLE_PAY_RESULT_OK, purchase->ClientToken, purchase->ServerToken, true));
    }
}
