/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_QUESTMETHODS_H
#define LUA_QUESTMETHODS_H

struct lua_State;
class Quest;

namespace LuaQuest
{
    int GetId(lua_State* L, Quest* quest);
    int GetLevel(lua_State* L, Quest* quest);
    int GetMaxLevel(lua_State* L, Quest* quest);
    int GetMinLevel(lua_State* L, Quest* quest);
    int GetNextQuestId(lua_State* L, Quest* quest);
    int GetPrevQuestId(lua_State* L, Quest* quest);
    int GetNextQuestInChain(lua_State* L, Quest* quest);
    int GetFlags(lua_State* L, Quest* quest);
    int GetQuestInfoID(lua_State* L, Quest* quest);
    int HasFlag(lua_State* L, Quest* quest);
    int IsDaily(lua_State* L, Quest* quest);
    int IsRepeatable(lua_State* L, Quest* quest);
    int SetFlag(lua_State* L, Quest* quest);
};
#endif