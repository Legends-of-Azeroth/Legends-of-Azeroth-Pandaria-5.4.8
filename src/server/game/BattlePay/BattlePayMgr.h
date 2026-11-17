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

#ifndef BATTLE_PAY_MGR_H
#define BATTLE_PAY_MGR_H

#include "Common.h"
#include "Opcodes.h"
#include "WorldSession.h"

#define MAX_BATTLE_PAY_PRODUCT_TITLE_SIZE 55
#define MAX_BATTLE_PAY_PRODUCT_DESCRIPTION_SIZE 500
#define MAX_BATTLE_PAY_GROUP_NAME_SIZE 16
#define BATTLE_PAY_CURRENCY_PRECISION 10000

enum BattlePayCurrency
{
    BATTLE_PAY_CURRENCY_UNKNOWN = 0,
    BATTLE_PAY_CURRENCY_USD     = 1,
    BATTLE_PAY_CURRENCY_GBP     = 2,
    BATTLE_PAY_CURRENCY_KRW     = 3,
    BATTLE_PAY_CURRENCY_EUR     = 4,
    BATTLE_PAY_CURRENCY_RUB     = 5,
    BATTLE_PAY_CURRENCY_ARS     = 8,
    BATTLE_PAY_CURRENCY_CLP     = 9,
    BATTLE_PAY_CURRENCY_MXN     = 10,
    BATTLE_PAY_CURRENCY_BRL     = 11,
    BATTLE_PAY_CURRENCY_AUD     = 12,
    BATTLE_PAY_CURRENCY_CPT     = 14,
    BATTLE_PAY_CURRENCY_TPT     = 15,
    BATTLE_PAY_CURRENCY_BETA    = 16,
};

enum BattlePayGroupType
{
    BATTLE_PAY_GROUP_TYPE_GRID              = 0,
    BATTLE_PAY_GROUP_TYPE_SINGLE            = 1,
    BATTLE_PAY_GROUP_TYPE_END
};

enum BattlePayBannerType
{
    BATTLE_PAY_BANNER_TYPE_FEATURED         = 0,
    BATTLE_PAY_BANNER_TYPE_DISCOUNT         = 1, // doing nothing and looks like BATTLE_PAY_BANNER_TYPE_FEATURED, name from client
    BATTLE_PAY_BANNER_TYPE_NEW              = 2,
    BATTLE_PAY_BANNER_TYPE_CUSTOM           = 3,
    BATTLE_PAY_BANNER_TYPE_END
};

enum BattlePayProductType
{
    BATTLE_PAY_PRODUCT_TYPE_ITEM            = 0,
    BATTLE_PAY_PRODUCT_TYPE_SERVICE         = 2,
    BATTLE_PAY_PRODUCT_TYPE_END
};

enum BattlePayProductFlags
{
    BATTLE_PAY_PRODUCT_FLAG_SHOW_IN_CHARACTERS_SCREEN = 0x001,
    BATTLE_PAY_PRODUCT_FLAG_UNK2                      = 0x002,
    BATTLE_PAY_PRODUCT_FLAG_UNK3                      = 0x004, // gray buy button with 1 flag
    BATTLE_PAY_PRODUCT_FLAG_UNK4                      = 0x008,
    BATTLE_PAY_PRODUCT_FLAG_UNK5                      = 0x010,
    BATTLE_PAY_PRODUCT_FLAG_UNK6                      = 0x020,
    BATTLE_PAY_PRODUCT_FLAG_UNK7                      = 0x040,
    BATTLE_PAY_PRODUCT_FLAG_UNK8                      = 0x080,
    BATTLE_PAY_PRODUCT_FLAG_UNK9                      = 0x100,
    BATTLE_PAY_PRODUCT_FLAG_UNK10                     = 0x200,

    BATTLE_PAY_PRODUCT_FLAG_SEND_MAIL       = BATTLE_PAY_PRODUCT_FLAG_SHOW_IN_CHARACTERS_SCREEN | BATTLE_PAY_PRODUCT_FLAG_UNK2  | BATTLE_PAY_PRODUCT_FLAG_UNK3 |
                                              BATTLE_PAY_PRODUCT_FLAG_UNK4                      | BATTLE_PAY_PRODUCT_FLAG_UNK6,
    BATTLE_PAY_PRODUCT_FLAG_EXECUTE_SERVICE = BATTLE_PAY_PRODUCT_FLAG_SHOW_IN_CHARACTERS_SCREEN | BATTLE_PAY_PRODUCT_FLAG_UNK2  | BATTLE_PAY_PRODUCT_FLAG_UNK3 |
                                              BATTLE_PAY_PRODUCT_FLAG_UNK4                      | BATTLE_PAY_PRODUCT_FLAG_UNK7  | BATTLE_PAY_PRODUCT_FLAG_UNK8 |
                                              BATTLE_PAY_PRODUCT_FLAG_UNK9                      | BATTLE_PAY_PRODUCT_FLAG_UNK10,
    BATTLE_PAY_PRODUCT_DISABLES             = BATTLE_PAY_PRODUCT_FLAG_SHOW_IN_CHARACTERS_SCREEN |BATTLE_PAY_PRODUCT_FLAG_UNK3,
};

enum BattlePayProductInfoFlags
{
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK1       = 0x00001,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK2       = 0x00002,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK3       = 0x00004,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK4       = 0x00008,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK5       = 0x00010,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK6       = 0x00020,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK7       = 0x00040,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK8       = 0x00080,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK9       = 0x00100,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK10      = 0x00200,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK11      = 0x00400,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK12      = 0x00800,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK13      = 0x01000,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK14      = 0x02000,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK15      = 0x04000,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK16      = 0x08000,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK17      = 0x10000,
    BATTLE_PAY_PRODUCT_INFO_FLAG_UNK18      = 0x20000,

    // unk values for check
    BATTLE_PAY_FLAG_ALTERAC_BREW_PUP        = BATTLE_PAY_PRODUCT_INFO_FLAG_UNK2 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK5 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK6 |
                                              BATTLE_PAY_PRODUCT_INFO_FLAG_UNK7 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK8 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK10 |
                                              BATTLE_PAY_PRODUCT_INFO_FLAG_UNK11 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK12 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK13,
    BATTLE_PAY_FLAG_ENCH_FEY_DRAGON         = BATTLE_PAY_PRODUCT_INFO_FLAG_UNK3 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK4 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK10 |
                                              BATTLE_PAY_PRODUCT_INFO_FLAG_UNK11 | BATTLE_PAY_PRODUCT_INFO_FLAG_UNK18,
};

enum BattlePayStatus
{
    BATTLE_PAY_STATUS_OK                    = 0,
    BATTLE_PAY_STATUS_UNAVAILABLE           = 1,
};

enum BattlePayBoostStatus
{
    CHARACTER_BOOST_ALLOW                     = 1,
    CHARACTER_BOOST_CHOOSED                   = 2,
    CHARACTER_BOOST_ITEMS                     = 3,
    CHARACTER_BOOST_APPLIED                   = 4,
};

enum BattlePayBoostCharInfoMask
{
    CHARACTER_BOOST_SPEC_MASK                 = 0x0000FFF,
    CHARACTER_BOOST_FACTION_ALLIANCE          = 0x1000000
};

enum BattlePayPurchaseStatus
{
    BATTLE_PAY_PURCHASE_STATUS_READY          = 2,
    BATTLE_PAY_PURCHASE_STATUS_ALLOWED_TO_BUY = 3,
    BATTLE_PAY_PURCHASE_STATUS_BUYING         = 5,
    BATTLE_PAY_PURCHASE_STATUS_BUYED          = 6,
};

enum BattlePayResultCode
{
    BATTLE_PAY_RESULT_OK                      = 0,
    BATTLE_PAY_RESULT_SHOP_ERROR              = 3,
    BATTLE_PAY_RESULT_DECLINED                = 4,
    BATTLE_PAY_RESULT_UNAVAILABLE             = 25,
};

enum BattlePayServices
{
    BATTLE_PAY_SERVICE_BOOST                  = 83,
    BATTLE_PAY_SERVICE_PREMIUM                = 84,
};

enum BattlePayChoiceType
{
    BATTLE_PAY_CHOICE_TYPE_DEFAULT            = 0,
    BATTLE_PAY_CHOICE_GREEN_POINTER           = 1,
    BATTLE_PAY_CHOICE_UNK_2                   = 2,
    BATTLE_PAY_CHOICE_UNK3                    = 3,
};

struct BattlePayProduct
{
    BattlePayProduct(uint32 id, std::string title, std::string description, uint32 icon, uint64 price, uint32 discount,
                     uint32 displayId, uint8 type, uint8 choiceType, uint32 flags, uint32 flagsInfo)
        : Id(id), Title(title), Description(description), Icon(icon), Price(price), Discount(discount),
          DisplayId(displayId), Type(type), ChoiceType(choiceType), Flags(flags), FlagsInfo(flagsInfo) { }

    uint32 Id;
    std::string Title;
    std::string Description;
    uint32 Icon;
    uint64 Price;
    uint32 Discount;
    uint32 DisplayId;
    uint8 Type;
    uint8 ChoiceType;
    uint32 Flags;
    uint32 FlagsInfo;
};

struct BattlePayProductLocale
{
    std::vector<std::string> Title;
    std::vector<std::string> Description;
};

struct BattlePayProductItem
{
    BattlePayProductItem(uint32 id, uint32 itemId, uint32 count)
        : Id(id), ItemId(itemId), Count(count) { }

    uint32 Id;
    uint32 ItemId;
    uint32 Count;
};

struct BattlePayGroup
{
    BattlePayGroup(uint32 id, uint32 index, std::string name, uint32 icon, uint8 type)
        : Id(id), Index(index), Name(name), Icon(icon), Type(type) { }

    public:
        uint32 Id;
        uint32 Index;
        std::string Name;
        uint32 Icon;
        uint8 Type;
};

struct BattlePayGroupLocale
{
    std::vector<std::string> Name;
};

struct BattlePayShopEntry
{
    BattlePayShopEntry(uint32 id, uint32 productId, uint32 groupId, uint32 index, std::string title, std::string description, uint32 icon, uint32 displayId, uint8 banner, uint32 flags)
        : Id(id), ProductId(productId), GroupId(groupId), Index(index), Title(title), Description(description), Icon(icon), DisplayId(displayId), Banner(banner), Flags(flags) { }

    public:
        uint32 Id;
        uint32 ProductId;
        uint32 GroupId;
        int32 Index;
        std::string Title;
        std::string Description;
        uint32 Icon;
        uint32 DisplayId;
        uint8 Banner;
        uint32 Flags;
};

struct BattlePayShopEntryLocale
{
    std::vector<std::string> Title;
    std::vector<std::string> Description;
};

struct PurchaseInfo
{
    PurchaseInfo() : Session(nullptr), SelectedPlayer(ObjectGuid::Empty), PurchaseId(0), ProductId(0), PurchaseStatus(0), ResultCode(0), ClientToken(0), ServerToken(0), Buyed(false) { }

    PurchaseInfo(WorldSession* session, ObjectGuid selectedPlayer, uint64 purchaseId, uint32 productId, uint32 purchaseStatus, uint32 resultCode, uint32 clientToken, uint32 serverToken, bool buyed)
        : Session(session), SelectedPlayer(selectedPlayer), PurchaseId(purchaseId), ProductId(productId), PurchaseStatus(purchaseStatus), ResultCode(resultCode), ClientToken(clientToken), ServerToken(serverToken), Buyed(buyed) { }

    WorldSession* Session;
    ObjectGuid SelectedPlayer;
    uint64 PurchaseId;
    uint32 ProductId;
    uint32 PurchaseStatus;
    uint32 ResultCode;
    uint32 ClientToken;
    uint32 ServerToken;
    bool Buyed;

    WorldSession* GetSession() { return Session; }
};

typedef std::list<BattlePayProduct*> BattlePayProductList;
typedef std::vector<BattlePayProductItem> BattlePayProductItemsVector;
typedef std::unordered_map<uint32, BattlePayProductItemsVector> BattlePayProductItemsMap;
typedef std::list<BattlePayGroup*> BattlePayGroupList;
typedef std::list<BattlePayShopEntry*> BattlePayShopEntryList;
typedef std::unordered_map<uint32, BattlePayProductLocale> BattlePayProductLocales;
typedef std::unordered_map<uint32, BattlePayGroupLocale> BattlePayGroupLocales;
typedef std::unordered_map<uint32, BattlePayShopEntryLocale> BattlePayShopEntryLocales;

class BattlePayMgr
{

    private:
         BattlePayMgr();
        ~BattlePayMgr();       

    public:

        static BattlePayMgr* instance();

        void SendPointsBalance(WorldSession* session);
        void UpdatePointsBalance(WorldSession* session, uint64 points);
        bool HasPointsBalance(WorldSession* session, uint64 points);
        void Update(uint32 diff);
        void RegisterPurchase(PurchaseInfo* purchase, uint32 item, uint64 price);
        bool IsStoreEnabled() { return m_enabled; }
        void SetEnableState(bool enabled) { m_enabled = enabled; }
        uint32 GetStoreCurrency() { return m_currency; }
        void SetStoreCurrency(uint32 currency) { m_currency = currency; }

        BattlePayProductItemsVector const* GetItemsByProductId(uint32 productId) const
        {
            BattlePayProductItemsMap::const_iterator iter = m_productItemsMap.find(productId);
                return iter != m_productItemsMap.end() ? &iter->second : NULL;
        }
        BattlePayProductLocale const* GetProductLocale(uint32 productId) const
        {
            BattlePayProductLocales::const_iterator iter = m_productLocalesMap.find(productId);
            return iter != m_productLocalesMap.end() ? &iter->second : NULL;
        }
        BattlePayGroupLocale const* GetGroupLocale(uint32 groupId) const
        {
            BattlePayGroupLocales::const_iterator iter = m_groupLocalesMap.find(groupId);
            return iter != m_groupLocalesMap.end() ? &iter->second : NULL;
        }
        BattlePayShopEntryLocale const* GetShopEntryLocale(uint32 entryId) const
        {
            BattlePayShopEntryLocales::const_iterator iter = m_shopEntryLocalesMap.find(entryId);
            return iter != m_shopEntryLocalesMap.end() ? &iter->second : NULL;
        }

        void LoadFromDb();

        void SendBattlePayDistributionList(WorldSession* session);
        void SendBattlePayPurchaseList(WorldSession* session);
        void SendBattlePayProductList(WorldSession* session);
        void SendBattlePayDistributionUpdate(WorldSession* session, uint32 productId, uint8 status);
        void SendBattlePayPurchaseUpdate(PurchaseInfo* purchase);
        void AddQuenedPurchaseResponse(PurchaseInfo* purchase, uint32 timer, bool sendPacket) { m_purchase = purchase; m_timer = timer; m_sendPacket = sendPacket; };

        PurchaseInfo* GetPurchaseInfo() { return m_purchase; }

    private:
        BattlePayProductList m_productStore;
        BattlePayProductItemsMap m_productItemsMap;
        BattlePayGroupList m_groupStore;
        BattlePayShopEntryList m_shopEntryStore;
        BattlePayProductLocales m_productLocalesMap;
        BattlePayGroupLocales m_groupLocalesMap;
        BattlePayShopEntryLocales m_shopEntryLocalesMap;
        BattlePayProduct* GetProductId(uint32 productId);

        bool HasGroupId(uint32 groupId);
        bool HasGroupName(std::string name);
        bool HasEntryId(uint32 entryId);

        PurchaseInfo* m_purchase;
        uint32 m_currency;
        uint32 m_timer;
        bool m_enabled;
        bool m_sendPacket;

        void LoadProductsFromDb();
        void LoadProductLocalesFromDb();
        void LoadProductItemsFromDb();
        void LoadGroupsFromDb();
        void LoadGroupLocalesFromDb();
        void LoadEntriesFromDb();
        void LoadEntryLocalesFromDb();
};

#define sBattlePayMgr BattlePayMgr::instance()

#endif
