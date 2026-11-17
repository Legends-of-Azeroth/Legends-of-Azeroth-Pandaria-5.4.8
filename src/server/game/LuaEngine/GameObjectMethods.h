/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_GAMEOBJECTMETHODS_H
#define LUA_GAMEOBJECTMETHODS_H

struct lua_State;
class GameObject;

namespace LuaGameObject
{
    int GetRelativePoint(lua_State* L, GameObject* go);
    int SummonCreature(lua_State* L, GameObject* go);
    int SummonGameObject(lua_State* L, GameObject* go);
    int GetDisplayId(lua_State* L, GameObject* go);
    int HasQuest(lua_State* L, GameObject* go);
    int IsSpawned(lua_State* L, GameObject* go);
    int IsTransport(lua_State* L, GameObject* go);
    int IsDestructible(lua_State* L, GameObject* go);
    int IsActive(lua_State* L, GameObject* go);
    int Move(lua_State* L, GameObject* go);
    int SaveToDB(lua_State* L, GameObject* go);
    int RemoveFromWorld(lua_State* L, GameObject* go);
    int RegisterEvent(lua_State* L, GameObject* go);
    int RemoveEventById(lua_State* L, GameObject* go);
    int RemoveEvents(lua_State* L, GameObject* go);
    int UseDoorOrButton(lua_State* L, GameObject* go);
    int SetGoState(lua_State* L, GameObject* go);
    int GetGoState(lua_State* L, GameObject* go);
    int SetLootState(lua_State* L, GameObject* go);
    int GetLootState(lua_State* L, GameObject* go);
    int Despawn(lua_State* L, GameObject* go);
    int Respawn(lua_State* L, GameObject* go);
};
#endif
