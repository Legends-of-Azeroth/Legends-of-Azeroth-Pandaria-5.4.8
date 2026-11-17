/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_QUERYMETHODS_H
#define LUA_QUERYMETHODS_H

struct lua_State;
class ResultSet;

namespace LuaQuery
{
    using QueryResult = std::shared_ptr<ResultSet>;

    int NextRow(lua_State* L, QueryResult* result);
    int GetColumnCount(lua_State* L, QueryResult* result);
    int GetRowCount(lua_State* L, QueryResult* result);
    int IsNull(lua_State* L, QueryResult* result);
    int GetBool(lua_State* L, QueryResult* result);
    int GetUInt8(lua_State* L, QueryResult* result);
    int GetUInt16(lua_State* L, QueryResult* result);
    int GetUInt32(lua_State* L, QueryResult* result);
    int GetUInt64(lua_State* L, QueryResult* result);
    int GetInt8(lua_State* L, QueryResult* result);
    int GetInt16(lua_State* L, QueryResult* result);
    int GetInt32(lua_State* L, QueryResult* result);
    int GetInt64(lua_State* L, QueryResult* result);
    int GetFloat(lua_State* L, QueryResult* result);
    int GetDouble(lua_State* L, QueryResult* result);
    int GetString(lua_State* L, QueryResult* result);
};
#endif
