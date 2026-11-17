/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef WORLDOBJECTMETHODS_H
#define WORLDOBJECTMETHODS_H

struct lua_State;
class WorldObject;

namespace LuaWorldObject
{
    int GetName(lua_State* L, WorldObject* obj);

    int GetMap(lua_State* L, WorldObject* obj);

    int GetPhaseMask(lua_State* L, WorldObject* obj);

    int GetInstanceId(lua_State* L, WorldObject* obj);

    int GetAreaId(lua_State* L, WorldObject* obj);

    int GetZoneId(lua_State* L, WorldObject* obj);

    int GetMapId(lua_State* L, WorldObject* obj);

    int GetX(lua_State* L, WorldObject* obj);

    int GetY(lua_State* L, WorldObject* obj);

    int GetZ(lua_State* L, WorldObject* obj);

    int GetO(lua_State* L, WorldObject* obj);

    int GetLocation(lua_State* L, WorldObject* obj);

    int GetNearestPlayer(lua_State* L, WorldObject* obj);

    int GetNearestGameObject(lua_State* L, WorldObject* obj);

    int GetNearestCreature(lua_State* L, WorldObject* obj);

    int GetPlayersInRange(lua_State* L, WorldObject* obj);

    int GetCreaturesInRange(lua_State* L, WorldObject* obj);

    int GetGameObjectsInRange(lua_State* L, WorldObject* obj);

    int GetWorldObject(lua_State* L, WorldObject* obj);
};
#endif
