/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_FUNCTIONS_H
#define LUA_FUNCTIONS_H

struct lua_State;

void RegisterGlobals(lua_State* L);
void RegisterFunctions(lua_State* L);

#endif