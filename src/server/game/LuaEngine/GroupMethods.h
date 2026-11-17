/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_GROUPMETHODS_H
#define LUA_GROUPMETHODS_H

struct lua_State;
class Group;

namespace LuaGroup
{
    int GetMembers(lua_State* L, Group* group);
    int GetLeaderGUID(lua_State* L, Group* group);
    int GetLeader(lua_State* L, Group* group);
    int GetGUID(lua_State* L, Group* group);
    int ChangeLeader(lua_State* L, Group* group);
    int IsLeader(lua_State* L, Group* group);
    int SendPacket(lua_State* L, Group* group);
    int AddInvite(lua_State* L, Group* group);
    int RemoveMember(lua_State* L, Group* group);
    int Disband(lua_State* L, Group* group);
    int IsFull(lua_State* L, Group* group);
    int isLFGGroup(lua_State* L, Group* group);
    int isRaidGroup(lua_State* L, Group* group);
    int isBGGroup(lua_State* L, Group* group);
    int isBFGroup(lua_State* L, Group* group);
    int IsMember(lua_State* L, Group* group);
    int IsAssistant(lua_State* L, Group* group);
    int SameSubGroup(lua_State* L, Group* group);
    int HasFreeSlotSubGroup(lua_State* L, Group* group);
    int GetMemberGUID(lua_State* L, Group* group);
    int GetMembersCount(lua_State* L, Group* group);
    int ConvertToLFG(lua_State* L, Group* group);
    int ConvertToRaid(lua_State* L, Group* group);
    int ChangeMembersGroup(lua_State* L, Group* group);
    int GetMemberGroup(lua_State* L, Group* group);
};
#endif
