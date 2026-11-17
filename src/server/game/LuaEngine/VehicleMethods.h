/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef VEHICLEMETHODS_H
#define VEHICLEMETHODS_H

struct lua_State;
class Vehicle;

namespace LuaVehicle
{
    int GetBase(lua_State* L, Vehicle* vehicle);

    int GetAvailableSeatCount(lua_State* L, Vehicle* vehicle);

    int GetCreatureEntry(lua_State* L, Vehicle* vehicle);

    int GetPassenger(lua_State* L, Vehicle* vehicle);

    int HasEmptySeat(lua_State* L, Vehicle* vehicle);

    int IsVehicleInUse(lua_State* L, Vehicle* vehicle);

    int InstallAccessory(lua_State* L, Vehicle* vehicle);

    int ApplyAllImmunities(lua_State* L, Vehicle* vehicle);

    int AddPassenger(lua_State* L, Vehicle* vehicle);

    int EjectPassenger(lua_State* L, Vehicle* vehicle);

    int RelocatePassengers(lua_State* L, Vehicle* vehicle);

    int RemoveAllPassengers(lua_State* L, Vehicle* vehicle);

    int RemovePassenger(lua_State* L, Vehicle* vehicle);

    int RemovePendingEventsForPassenger(lua_State* L, Vehicle* vehicle);

    int Reset(lua_State* L, Vehicle* vehicle);
}

#endif