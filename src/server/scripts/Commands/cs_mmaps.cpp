/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file cs_mmaps.cpp
* @brief .mmap related commands
*
* This file contains the CommandScripts for all
* mmap sub-commands
*/

#include "ScriptMgr.h"
#include "Chat.h"
#include "DisableMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PointMovementGenerator.h"
#include "PathGenerator.h"
#include "MMapFactory.h"
#include "Map.h"
#include "TargetedMovementGenerator.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "World.h"

class mmaps_commandscript : public CommandScript
{
public:
    mmaps_commandscript() : CommandScript("mmaps_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> mmapEnableCommandTable =
        {
            { "map",            SEC_GAMEMASTER,     false, &HandleMmapEnableMapCommand,         },
            { "zone",           SEC_GAMEMASTER,     false, &HandleMmapEnableZoneCommand,        },
            { "area",           SEC_GAMEMASTER,     false, &HandleMmapEnableAreaCommand,        },
            { "creature",       SEC_GAMEMASTER,     false, &HandleMmapEnableCreatureCommand,    },
            { "",               SEC_GAMEMASTER,     false, &HandleMmapEnableCommand,            },
        };

        static std::vector<ChatCommand> mmapDisableCommandTable =
        {
            { "map",            SEC_GAMEMASTER,     false, &HandleMmapDisableMapCommand,        },
            { "zone",           SEC_GAMEMASTER,     false, &HandleMmapDisableZoneCommand,       },
            { "area",           SEC_GAMEMASTER,     false, &HandleMmapDisableAreaCommand,       },
            { "creature",       SEC_GAMEMASTER,     false, &HandleMmapDisableCreatureCommand,   },
            { "",               SEC_GAMEMASTER,     false, &HandleMmapDisableCommand,           },
        };

        static std::vector<ChatCommand> mmapCommandTable =
        {
            { "loadedtiles",    SEC_GAMEMASTER,     false, &HandleMmapLoadedTilesCommand,   },
            { "loc",            SEC_GAMEMASTER,     false, &HandleMmapLocCommand,           },
            { "path",           SEC_GAMEMASTER,     false, &HandleMmapPathCommand,          },
            { "stats",          SEC_GAMEMASTER,     false, &HandleMmapStatsCommand,         },
            { "testarea",       SEC_GAMEMASTER,     false, &HandleMmapTestArea,             },
            { "enable",         SEC_GAMEMASTER,     false, mmapEnableCommandTable           },
            { "disable",        SEC_GAMEMASTER,     false, mmapDisableCommandTable          },
            { "visualize",      SEC_GAMEMASTER,     false, &HandleMmapVisualizeCommand,     },
            
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "mmap",           SEC_GAMEMASTER,     true,  mmapCommandTable                 },
        };
        return commandTable;
    }

    static bool HandleMmapPathCommand(ChatHandler* handler, char const* args)
    {
        if (!MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId()))
        {
            handler->PSendSysMessage("NavMesh not loaded for current map.");
            return true;
        }

        handler->PSendSysMessage("mmap path:");

        // units
        Player* player = handler->GetSession()->GetPlayer();
        Unit* target = handler->getSelectedUnit();
        if (!player || !target)
        {
            handler->PSendSysMessage("Invalid target/source selection.");
            return true;
        }

        char* para = strtok((char*)args, " ");
        bool back = false;
        if (para && strcmp(para, "back") == 0)
        {
            back = true;
            para = strtok(NULL, " ");
        }
        char* para2 = strtok(NULL, " ");
        char* para3 = strtok(NULL, " ");

        bool useStraightPath = false;
        if (para && strcmp(para, "true") == 0)
            useStraightPath = true;

        bool forceDest = false;
        if (para2 && strcmp(para2, "true") == 0)
            forceDest = true;

        bool straightLine = false;
        if (para3 && strcmp(para3, "true") == 0)
            straightLine = true;

        // unit locations
        float x, y, z;
        (back ? player : target)->GetPosition(x, y, z);

        // path
        PathGenerator path(back ? target : player);
        path.SetUseStraightPath(useStraightPath);
        bool result = path.CalculatePath(x, y, z, forceDest, straightLine);

        Movement::PointsArray const& pointPath = path.GetPath();
        handler->PSendSysMessage("%s's path to %s:", (back ? target : player)->GetName().c_str(), (back ? player : target)->GetName().c_str());
        handler->PSendSysMessage("Building: %s", useStraightPath ? "StraightPath" : "SmoothPath");
        handler->PSendSysMessage("Result: %s - Length: %lu - Type: %u", (result ? "true" : "false"), pointPath.size(), path.GetPathType());

        G3D::Vector3 const &start = path.GetStartPosition();
        G3D::Vector3 const &end = path.GetEndPosition();
        G3D::Vector3 const &actualEnd = path.GetActualEndPosition();

        handler->PSendSysMessage("StartPosition     (%.3f, %.3f, %.3f)", start.x, start.y, start.z);
        handler->PSendSysMessage("EndPosition       (%.3f, %.3f, %.3f)", end.x, end.y, end.z);
        handler->PSendSysMessage("ActualEndPosition (%.3f, %.3f, %.3f)", actualEnd.x, actualEnd.y, actualEnd.z);

        if (!player->IsGameMaster())
            handler->PSendSysMessage("Enable GM mode to see the path points.");

        for (uint32 i = 0; i < pointPath.size(); ++i)
            player->SummonCreature(VISUAL_WAYPOINT, pointPath[i].x, pointPath[i].y, pointPath[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 9000);

        return true;
    }

    static bool HandleMmapLocCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage("mmap tileloc:");

        // grid tile location
        Player* player = handler->GetSession()->GetPlayer();

        int32 gx = 32 - player->GetPositionX() / SIZE_OF_GRIDS;
        int32 gy = 32 - player->GetPositionY() / SIZE_OF_GRIDS;

        handler->PSendSysMessage("%04u_%02i_%02i.mmtile", player->GetMapId(), gx, gy);
        handler->PSendSysMessage("gridloc [%i, %i]", gy, gx);

        // calculate navmesh tile location
        dtNavMesh const* navmesh = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId());
        dtNavMeshQuery const* navmeshquery = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMeshQuery(handler->GetSession()->GetPlayer()->GetMapId(), player->GetInstanceId());
        if (!navmesh || !navmeshquery)
        {
            handler->PSendSysMessage("NavMesh not loaded for current map.");
            return true;
        }

        float const* min = navmesh->getParams()->orig;
        float x, y, z;
        player->GetPosition(x, y, z);
        float location[VERTEX_SIZE] = {y, z, x};
        float extents[VERTEX_SIZE] = {3.0f, 5.0f, 3.0f};

        int32 tilex = int32((y - min[0]) / SIZE_OF_GRIDS);
        int32 tiley = int32((x - min[2]) / SIZE_OF_GRIDS);

        handler->PSendSysMessage("Calc   [%02i, %02i]", tilex, tiley);

        // navmesh poly -> navmesh tile location
        dtQueryFilter filter = dtQueryFilter();
        dtPolyRef polyRef = INVALID_POLYREF;
        if (dtStatusFailed(navmeshquery->findNearestPoly(location, extents, &filter, &polyRef, NULL)))
        {
            handler->PSendSysMessage("Dt     [??,??] (invalid poly, probably no tile loaded)");
            return true;
        }

        if (polyRef == INVALID_POLYREF)
            handler->PSendSysMessage("Dt     [??, ??] (invalid poly, probably no tile loaded)");
        else
        {
            dtMeshTile const* tile;
            dtPoly const* poly;
            if (dtStatusSucceed(navmesh->getTileAndPolyByRef(polyRef, &tile, &poly)))
            {
                if (tile)
                {
                    handler->PSendSysMessage("Dt     [%02i,%02i]", tile->header->x, tile->header->y);
                    return false;
                }
            }

            handler->PSendSysMessage("Dt     [??,??] (no tile loaded)");
        }

        return true;
    }

    static bool HandleMmapLoadedTilesCommand(ChatHandler* handler, char const* /*args*/)
    {
        uint32 mapid = handler->GetSession()->GetPlayer()->GetMapId();
        dtNavMesh const* navmesh = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(mapid);
        dtNavMeshQuery const* navmeshquery = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMeshQuery(mapid, handler->GetSession()->GetPlayer()->GetInstanceId());
        if (!navmesh || !navmeshquery)
        {
            handler->PSendSysMessage("NavMesh not loaded for current map.");
            return true;
        }

        handler->PSendSysMessage("mmap loadedtiles:");

        for (int32 i = 0; i < navmesh->getMaxTiles(); ++i)
        {
            dtMeshTile const* tile = navmesh->getTile(i);
            if (!tile || !tile->header)
                continue;

            handler->PSendSysMessage("[%02i, %02i]", tile->header->x, tile->header->y);
        }

        return true;
    }

    static bool HandleMmapStatsCommand(ChatHandler* handler, char const* /*args*/)
    {
        const char* textEnabled = "|cFF00FF00en";
        const char* textDisabled = "|cFFFF0000dis";

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        uint32 mapId = target->GetMapId();
        uint32 zoneId = target->GetZoneId();
        uint32 areaId = target->GetAreaId();
        uint32 entry = target->GetEntry();
        handler->PSendSysMessage("mmap stats:");
        handler->PSendSysMessage("  global mmap pathfinding is %sabled|r", sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) ? textEnabled : textDisabled);
        handler->PSendSysMessage("  current target pathfinding is %sabled|r", sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS)
            && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_MAP, mapId, nullptr)
            && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_ZONE, zoneId, nullptr)
            && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_AREA, areaId, nullptr)
            && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_CREATURE, entry, nullptr) ? textEnabled : textDisabled);
        handler->PSendSysMessage("    in map %u is %sabled|r", mapId, sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_MAP, mapId, nullptr) ? textEnabled : textDisabled);
        handler->PSendSysMessage("    in zone %u is %sabled|r", zoneId, sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_ZONE, zoneId, nullptr) ? textEnabled : textDisabled);
        handler->PSendSysMessage("    in area %u is %sabled|r", areaId, sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_AREA, areaId, nullptr) ? textEnabled : textDisabled);
        handler->PSendSysMessage("    for creature %u is %sabled|r", entry, sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_CREATURE, entry, nullptr) ? textEnabled : textDisabled);

        MMAP::MMapManager* manager = MMAP::MMapFactory::createOrGetMMapManager();
        handler->PSendSysMessage(" %u maps loaded with %u tiles overall", manager->getLoadedMapsCount(), manager->getLoadedTilesCount());

        dtNavMesh const* navmesh = manager->GetNavMesh(mapId);
        if (!navmesh)
        {
            handler->PSendSysMessage("NavMesh not loaded for current map.");
            return true;
        }

        uint32 tileCount = 0;
        uint32 nodeCount = 0;
        uint32 polyCount = 0;
        uint32 vertCount = 0;
        uint32 triCount = 0;
        uint32 triVertCount = 0;
        uint32 dataSize = 0;
        for (int32 i = 0; i < navmesh->getMaxTiles(); ++i)
        {
            dtMeshTile const* tile = navmesh->getTile(i);
            if (!tile || !tile->header)
                continue;

            tileCount++;
            nodeCount += tile->header->bvNodeCount;
            polyCount += tile->header->polyCount;
            vertCount += tile->header->vertCount;
            triCount += tile->header->detailTriCount;
            triVertCount += tile->header->detailVertCount;
            dataSize += tile->dataSize;
        }

        handler->PSendSysMessage("Navmesh stats:");
        handler->PSendSysMessage(" %u tiles loaded", tileCount);
        handler->PSendSysMessage(" %u BVTree nodes", nodeCount);
        handler->PSendSysMessage(" %u polygons (%u vertices)", polyCount, vertCount);
        handler->PSendSysMessage(" %u triangles (%u vertices)", triCount, triVertCount);
        handler->PSendSysMessage(" %.2f MB of data (not including pointers)", ((float)dataSize / sizeof(unsigned char)) / 1048576);

        return true;
    }

    static bool HandleMmapTestArea(ChatHandler* handler, char const* /*args*/)
    {
        float radius = 40.0f;
        WorldObject* object = handler->GetSession()->GetPlayer();

        CellCoord pair(Trinity::ComputeCellCoord(object->GetPositionX(), object->GetPositionY()));
        Cell cell(pair);
        cell.SetNoCreate();

        std::list<Creature*> creatureList;

        Trinity::AnyUnitInObjectRangeCheck go_check(object, radius);
        Trinity::CreatureListSearcher<Trinity::AnyUnitInObjectRangeCheck> go_search(object, creatureList, go_check);
        TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AnyUnitInObjectRangeCheck>, GridTypeMapContainer> go_visit(go_search);

        // Get Creatures
        cell.Visit(pair, go_visit, *(object->GetMap()), *object, radius);

        if (!creatureList.empty())
        {
            handler->PSendSysMessage("Found %lu Creatures.", creatureList.size());

            uint32 paths = 0;
            uint32 uStartTime = getMSTime();

            float gx, gy, gz;
            object->GetPosition(gx, gy, gz);
            for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
            {
                PathGenerator path(*itr);
                path.CalculatePath(gx, gy, gz);
                ++paths;
            }

            uint32 uPathLoadTime = getMSTimeDiff(uStartTime, getMSTime());
            handler->PSendSysMessage("Generated %i paths in %i ms", paths, uPathLoadTime);
        }
        else
            handler->PSendSysMessage("No creatures in %f yard range.", radius);

        return true;
    }

    struct MMAPDisableName
    {
        DisableType type;
        const char* name;
    };
    static bool DisableHelper(ChatHandler* handler, char const* args, bool disable, DisableType type)
    {
        const MMAPDisableName table[] =
        {
            { DISABLE_TYPE_MMAP_MAP, "map" },
            { DISABLE_TYPE_MMAP_ZONE, "zone" },
            { DISABLE_TYPE_MMAP_AREA, "area" },
            { DISABLE_TYPE_MMAP_CREATURE, "creature" },
        };
        const uint8 TABLE_ENTRIES_COUNT = 4;

        if (!sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS))
        {
            handler->PSendSysMessage("Unable to execute command: pathfinding is globally disabled.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 entry = args && strlen(args) ? std::atoi(args) : type == DISABLE_TYPE_MMAP_MAP ? -1 : 0;
        Unit* target = handler->GetSession()->GetPlayer()->GetSelectedUnit() ? handler->GetSession()->GetPlayer()->GetSelectedUnit() : handler->GetSession()->GetPlayer();
        std::stringstream ss;
        ss << "MMap Disabled - ";

        bool skip = true;
        switch (type)
        {
            case DISABLE_TYPE_MMAP_MAP:
                if (entry == -1)
                    entry = target->GetMapId();
                if (entry != -1)
                    skip = (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_MAP, entry, nullptr)) != disable;
                if (MapEntry const* map = sMapStore.LookupEntry(entry))
                    ss << "Map - " << map->name[DEFAULT_LOCALE];
                break;
            case DISABLE_TYPE_MMAP_ZONE:
                if (!entry)
                    entry = target->GetZoneId();
                if (entry)
                    skip = (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_ZONE, entry, nullptr)) != disable;
                if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(entry))
                    ss << "Area - " << area->area_name[DEFAULT_LOCALE];
                break;
            case DISABLE_TYPE_MMAP_AREA:
                if (!entry)
                    entry = target->GetAreaId();
                if (entry)
                    skip = (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_AREA, entry, nullptr)) != disable;
                if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(entry))
                    ss << "Area - " << zone->area_name[DEFAULT_LOCALE];
                break;
            case DISABLE_TYPE_MMAP_CREATURE:
                if (!entry)
                    entry = target->GetEntry();
                if (entry)
                    skip = (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_CREATURE, entry, nullptr)) != disable;
                ss << "Creature - " << target->GetNameForLocaleIdx(DEFAULT_LOCALE);
                break;
            default: break;
        }

        const char* name;
        for (uint8 i = 0; i < TABLE_ENTRIES_COUNT; ++i)
        {
            if (table[i].type == type)
            {
                name = table[i].name;
                break;
            }
        }

        if (type != DISABLE_TYPE_MMAP_MAP && !entry)
        {
            handler->PSendSysMessage("Wrong %s: 0", name);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (skip)
        {
            handler->PSendSysMessage("Pathfinding is already %sabled for %s %u", disable ? "dis" : "en", name, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ss << " (" << TimeToTimestampStr(time(nullptr)) << " by " << handler->GetSession()->GetPlayerName() << ")";

        std::string comment = ss.str();
        size_t index = 0;
        while ((index = comment.find("\"", index)) != std::string::npos)
        {
            comment.replace(index, 1, "\"\"");
            index += 2;
        }

        WorldDatabase.PExecute(disable ? "INSERT INTO disables (sourceType, entry, `comment`) VALUES (%u, %u, \"%s\")" : "DELETE FROM disables WHERE sourceType=%u AND entry=%u", uint32(type), entry, comment.c_str());
        handler->PSendSysMessage("%sabled pathfinding for %s %u", disable ? "Dis" : "En", name, entry);
        DisableMgr::LoadDisables();
        return true;
    }
    static bool HandleMmapEnableMapCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, false, DISABLE_TYPE_MMAP_MAP);
    }
    static bool HandleMmapEnableZoneCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, false, DISABLE_TYPE_MMAP_ZONE);
    }
    static bool HandleMmapEnableAreaCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, false, DISABLE_TYPE_MMAP_AREA);
    }
    static bool HandleMmapEnableCreatureCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, false, DISABLE_TYPE_MMAP_CREATURE);
    }
    static bool HandleMmapEnableCommand(ChatHandler* handler, char const* /*args*/)
    {
        sWorld->setBoolConfig(CONFIG_ENABLE_MMAPS, true);
        handler->PSendSysMessage("Enabled pathfinding globally (not saved in config)");
        return true;
    }
    static bool HandleMmapDisableMapCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, true, DISABLE_TYPE_MMAP_MAP);
    }
    static bool HandleMmapDisableZoneCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, true, DISABLE_TYPE_MMAP_ZONE);
    }
    static bool HandleMmapDisableAreaCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, true, DISABLE_TYPE_MMAP_AREA);
    }
    static bool HandleMmapDisableCreatureCommand(ChatHandler* handler, char const* args)
    {
        return DisableHelper(handler, args, true, DISABLE_TYPE_MMAP_CREATURE);
    }
    static bool HandleMmapDisableCommand(ChatHandler* handler, char const* /*args*/)
    {
        sWorld->setBoolConfig(CONFIG_ENABLE_MMAPS, false);
        handler->PSendSysMessage("Disabled pathfinding globally (not saved in config)");
        return true;
    }
    
    static bool HandleMmapVisualizeCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
            return false;

        target->VisualizePathfinding ^= true;
        return true;
    }
};

void AddSC_mmaps_commandscript()
{
    new mmaps_commandscript();
}
