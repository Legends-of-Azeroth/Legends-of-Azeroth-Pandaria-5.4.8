/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef ELUNA_WEATHERMETHODS_H
#define ELUNA_WEATHERMETHODS_H

struct lua_State;
class Weather;

namespace LuaWeather
{
    int GetScriptId(lua_State* L, Weather* weather);
    int GetZoneId(lua_State* L, Weather* weather);
    int SetWeather(lua_State* L, Weather* weather);
    int SendWeatherUpdateToPlayer(lua_State* L, Weather* weather);
    int Regenerate(lua_State* L, Weather* weather);
    int UpdateWeather(lua_State* L, Weather* weather);
};

#endif