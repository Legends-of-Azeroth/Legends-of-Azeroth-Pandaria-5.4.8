/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_MAPMETHODS_H
#define LUA_MAPMETHODS_H

struct lua_State;
class Map;

namespace LuaMap
{
    int GetName(lua_State* L, Map* map);
    int GetHeight(lua_State* L, Map* map);
    int GetDifficulty(lua_State* L, Map* map);
    int GetInstanceId(lua_State* L, Map* map);
    int GetPlayerCount(lua_State* L, Map* map);
    int GetMapId(lua_State* L, Map* map);
    int GetAreaId(lua_State* L, Map* map);
    int IsArena(lua_State* L, Map* map);
    int IsBattleground(lua_State* L, Map* map);
    int IsDungeon(lua_State* L, Map* map);
    int IsEmpty(lua_State* L, Map* map);
    int IsHeroic(lua_State* L, Map* map);
    int IsRaid(lua_State* L, Map* map);
};
#endif