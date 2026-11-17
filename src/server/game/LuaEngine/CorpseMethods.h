/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_CORPSEMETHODS_H
#define LUA_CORPSEMETHODS_H

struct lua_State;
class Corpse;

namespace LuaCorpse
{
    int GetOwnerGUID(lua_State* L, Corpse* corpse);
    int GetGhostTime(lua_State* L, Corpse* corpse);
    int GetType(lua_State* L, Corpse* corpse);
    int ResetGhostTime(lua_State* L, Corpse* corpse);
    int SaveToDB(lua_State* L, Corpse* corpse);
    int DeleteBonesFromWorld(lua_State* L, Corpse* corpse);
};
#endif