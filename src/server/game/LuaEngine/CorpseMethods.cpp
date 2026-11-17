/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#include "CorpseMethods.h"
#include "Includes.h"

namespace LuaCorpse
{
    int GetOwnerGUID(lua_State* L, Corpse* corpse)
    {
        sEluna->Push(L, corpse->GetOwnerGUID());
        return 1;
    }

    int GetGhostTime(lua_State* L, Corpse* corpse)
    {
        sEluna->Push(L, corpse->GetGhostTime());
        return 1;
    }

    int GetType(lua_State* L, Corpse* corpse)
    {
        sEluna->Push(L, corpse->GetType());
        return 1;
    }

    int ResetGhostTime(lua_State* L, Corpse* corpse)
    {
        corpse->ResetGhostTime();
        return 0;
    }

    int SaveToDB(lua_State* L, Corpse* corpse)
    {
        corpse->SaveToDB();
        return 0;
    }
};
