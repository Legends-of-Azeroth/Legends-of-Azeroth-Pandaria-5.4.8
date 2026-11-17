/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_OBJECTMETHODS_H
#define LUA_OBJECTMETHODS_H

struct lua_State;
class Object;

namespace LuaObject
{
    int RemoveFlag(lua_State* L, Object* obj);
    int SetFlag(lua_State* L, Object* obj);
    int HasFlag(lua_State* L, Object* obj);
    int GetInt32Value(lua_State* L, Object* obj);
    int GetUInt32Value(lua_State* L, Object* obj);
    int GetFloatValue(lua_State* L, Object* obj);
    int GetByteValue(lua_State* L, Object* obj);
    int GetUInt16Value(lua_State* L, Object* obj);
    int SetInt32Value(lua_State* L, Object* obj);
    int SetUInt32Value(lua_State* L, Object* obj);
    int UpdateUInt32Value(lua_State* L, Object* obj);
    int SetFloatValue(lua_State* L, Object* obj);
    int SetByteValue(lua_State* L, Object* obj);
    int SetUInt16Value(lua_State* L, Object* obj);
    int SetInt16Value(lua_State* L, Object* obj);
    int ToCorpse(lua_State* L, Object* obj);
    int ToGameObject(lua_State* L, Object* obj);
    int ToUnit(lua_State* L, Object* obj);
    int ToCreature(lua_State* L, Object* obj);
    int ToPlayer(lua_State* L, Object* obj);
    int SetScale(lua_State* L, Object* obj);
    int GetScale(lua_State* L, Object* obj);
    int GetEntry(lua_State* L, Object* obj);
    int GetGUID(lua_State* L, Object* obj);
    int GetGUIDLow(lua_State* L, Object* obj);
    int IsInWorld(lua_State* L, Object* obj);
};
#endif
