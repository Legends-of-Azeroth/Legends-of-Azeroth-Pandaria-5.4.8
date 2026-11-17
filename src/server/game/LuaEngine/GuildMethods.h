/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_GUILDMETHODS_H
#define LUA_GUILDMETHODS_H

struct lua_State;
class Guild;

namespace LuaGuild
{
    int GetMembers(lua_State* L, Guild* guild);
    int GetMemberCount(lua_State* L, Guild* guild);
    int GetLeader(lua_State* L, Guild* guild);
    int SetLeader(lua_State* L, Guild* guild);
    int GetLeaderGUID(lua_State* L, Guild* guild);
    int SendPacket(lua_State* L, Guild* guild);
    int SendPacketToRanked(lua_State* L, Guild* guild);
    int Disband(lua_State* L, Guild* guild);
    int GetId(lua_State* L, Guild* guild);
    int GetName(lua_State* L, Guild* guild);
    int GetMOTD(lua_State* L, Guild* guild);
    int GetInfo(lua_State* L, Guild* guild);
    int AddMember(lua_State* L, Guild* guild);
    int DeleteMember(lua_State* L, Guild* guild);
    int ChangeMemberRank(lua_State* L, Guild* guild);
    int SetBankTabText(lua_State* L, Guild* guild);
};
#endif
