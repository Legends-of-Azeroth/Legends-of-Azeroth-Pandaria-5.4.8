/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_SPELLMETHODS_H
#define LUA_SPELLMETHODS_H

struct lua_State;
class Spell;

namespace LuaSpell
{
    int GetCaster(lua_State* L, Spell* spell);
    int GetCastTime(lua_State* L, Spell* spell);
    int GetId(lua_State* L, Spell* spell);
    int GetPowerCost(lua_State* L, Spell* spell);
    int GetDuration(lua_State* L, Spell* spell);
    int Cast(lua_State* L, Spell* spell);
    int IsAutoRepeat(lua_State* L, Spell* spell);
    int SetAutoRepeat(lua_State* L, Spell* spell);
    int cancel(lua_State* L, Spell* spell);
    int Finish(lua_State* L, Spell* spell);
    int GetTargetDest(lua_State* L, Spell* spell);
};
#endif