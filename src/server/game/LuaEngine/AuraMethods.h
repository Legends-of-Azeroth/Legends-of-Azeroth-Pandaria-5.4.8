/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_AURAMETHODS_H
#define LUA_AURAMETHODS_H

struct lua_State;
class Aura;

namespace LuaAura
{
    int GetCaster(lua_State* L, Aura* aura);
    int GetCasterGUID(lua_State* L, Aura* aura);
    int GetCasterLevel(lua_State* L, Aura* aura);
    int GetDuration(lua_State* L, Aura* aura);
    int GetCharges(lua_State* L, Aura* aura);
    int GetAuraId(lua_State* L, Aura* aura);
    int GetMaxDuration(lua_State* L, Aura* aura);
    int GetStackAmount(lua_State* L, Aura* aura);
    int SetDuration(lua_State* L, Aura* aura);
    int SetMaxDuration(lua_State* L, Aura* aura);
    int SetStackAmount(lua_State* L, Aura* aura);
    int Remove(lua_State* L, Aura* aura);
    int GetOwner(lua_State* L, Aura* aura);
};
#endif
