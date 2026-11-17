/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_WORLDPACKETMETHODS_H
#define LUA_WORLDPACKETMETHODS_H

struct lua_State;
class WorldPacket;

namespace LuaPacket
{
    int GetOpcode(lua_State* L, WorldPacket* packet);
    int GetSize(lua_State* L, WorldPacket* packet);
    int SetOpcode(lua_State* L, WorldPacket* packet);
    int ReadByte(lua_State* L, WorldPacket* packet);
    int ReadUByte(lua_State* L, WorldPacket* packet);
    int ReadShort(lua_State* L, WorldPacket* packet);
    int ReadUShort(lua_State* L, WorldPacket* packet);
    int ReadLong(lua_State* L, WorldPacket* packet);
    int ReadULong(lua_State* L, WorldPacket* packet);
    int ReadFloat(lua_State* L, WorldPacket* packet);
    int ReadDouble(lua_State* L, WorldPacket* packet);
    int ReadGUID(lua_State* L, WorldPacket* packet);
    int ReadString(lua_State* L, WorldPacket* packet);
    int WriteGUID(lua_State* L, WorldPacket* packet);
    int WriteString(lua_State* L, WorldPacket* packet);
    int WriteByte(lua_State* L, WorldPacket* packet);
    int WriteUByte(lua_State* L, WorldPacket* packet);
    int WriteUShort(lua_State* L, WorldPacket* packet);
    int WriteShort(lua_State* L, WorldPacket* packet);
    int WriteLong(lua_State* L, WorldPacket* packet);
    int WriteULong(lua_State* L, WorldPacket* packet);
    int WriteFloat(lua_State* L, WorldPacket* packet);
    int WriteDouble(lua_State* L, WorldPacket* packet);
};

#endif