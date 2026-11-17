/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_ITEMMETHODS_H
#define LUA_ITEMMETHODS_H

struct lua_State;
class Item;

namespace LuaItem
{
    int GetItemLink(lua_State* L, Item* item);
    int GetGUID(lua_State* L, Item* item);
    int GetOwnerGUID(lua_State* L, Item* item);
    int GetOwner(lua_State* L, Item* item);
    int SetOwner(lua_State* L, Item* item);
    int SetBinding(lua_State* L, Item* item);
    int IsSoulBound(lua_State* L, Item* item);
    int IsBoundAccountWide(lua_State* L, Item* item);
    int IsBoundByEnchant(lua_State* L, Item* item);
    int IsNotBoundToPlayer(lua_State* L, Item* item);
    int IsLocked(lua_State* L, Item* item);
    int IsBag(lua_State* L, Item* item);
    int IsCurrencyToken(lua_State* L, Item* item);
    int IsNotEmptyBag(lua_State* L, Item* item);
    int IsBroken(lua_State* L, Item* item);
    int CanBeTraded(lua_State* L, Item* item);
    int IsInTrade(lua_State* L, Item* item);
    int GetCount(lua_State* L, Item* item);
    int SetCount(lua_State* L, Item* item);
    int GetMaxStackCount(lua_State* L, Item* item);
    int GetSlot(lua_State* L, Item* item);
    int GetBagSlot(lua_State* L, Item* item);
    int IsInBag(lua_State* L, Item* item);
    int IsEquipped(lua_State* L, Item* item);
    int HasQuest(lua_State* L, Item* item);
    int IsPotion(lua_State* L, Item* item);
    int IsWeaponVellum(lua_State* L, Item* item);
    int IsArmorVellum(lua_State* L, Item* item);
    int IsConjuredConsumable(lua_State* L, Item* item);
    int IsRefundExpired(lua_State* L, Item* item);
    int SetEnchantment(lua_State* L, Item* item);
    int ClearEnchantment(lua_State* L, Item* item);
    int GetGUIDLow(lua_State* L, Item* item);
    int GetEnchantmentId(lua_State* L, Item* item);
    int GetSpellId(lua_State* L, Item* item);
    int GetSpellTrigger(lua_State* L, Item* item);
    int GetClass(lua_State* L, Item* item);
    int GetSubClass(lua_State* L, Item* item);
    int GetName(lua_State* L, Item* item);
    int GetDisplayId(lua_State* L, Item* item);
    int GetQuality(lua_State* L, Item* item);
    int GetBuyCount(lua_State* L, Item* item);
    int GetBuyPrice(lua_State* L, Item* item);
    int GetSellPrice(lua_State* L, Item* item);
    int GetInventoryType(lua_State* L, Item* item);
    int GetAllowableClass(lua_State* L, Item* item);
    int GetAllowableRace(lua_State* L, Item* item);
    int GetItemLevel(lua_State* L, Item* item);
    int GetRequiredLevel(lua_State* L, Item* item);
    int GetStatsCount(lua_State* L, Item* item);
    int GetRandomProperty(lua_State* L, Item* item);
    int GetRandomSuffix(lua_State* L, Item* item);
    int GetItemSet(lua_State* L, Item* item);
    int GetBagSize(lua_State* L, Item* item);
    int SaveToDB(lua_State* L, Item* item);
};
#endif
