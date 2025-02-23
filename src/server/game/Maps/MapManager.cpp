/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "CellImpl.h"
#include "Config.h"
#include "Corpse.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "GridDefines.h"
#include "Group.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "Language.h"
#include "Log.h"
#include "MapInstanced.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "Transport.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

extern GridState* si_GridStates[];                          // debugging code, should be deleted some day

MapManager::MapManager()
    : _freeInstanceIds(std::make_unique<InstanceIds>()), _nextInstanceId(0)
{
    i_gridCleanUpDelay = sWorld->getIntConfig(CONFIG_INTERVAL_GRIDCLEAN);
    i_timer.SetInterval(sWorld->getIntConfig(CONFIG_INTERVAL_MAPUPDATE));
}

MapManager::~MapManager() { }

MapManager* MapManager::instance()
{
    static MapManager instance;
    return &instance;
}

void MapManager::Initialize()
{
    Map::InitStateMachine();

    // debugging code, should be deleted some day
    {
        for (uint8 i = 0; i < MAX_GRID_STATE; ++i)
             i_GridStates[i] = si_GridStates[i];

        i_GridStateErrorCount = 0;
    }
    int num_threads(sWorld->getIntConfig(CONFIG_NUMTHREADS));

    // Start mtmaps if needed.
    if (num_threads > 0)
        m_updater.activate(num_threads);
    
}

void MapManager::InitializeVisibilityDistanceInfo()
{
    for (MapMapType::iterator iter=i_maps.begin(); iter != i_maps.end(); ++iter)
        (*iter).second->InitVisibilityDistance();
}

// debugging code, should be deleted some day
void MapManager::checkAndCorrectGridStatesArray()
{
    bool ok = true;
    for (int i=0; i<MAX_GRID_STATE; i++)
    {
        if (i_GridStates[i] != si_GridStates[i])
        {
            TC_LOG_ERROR("maps", "MapManager::checkGridStates(), GridState: si_GridStates is currupt !!!");
            ok = false;
            si_GridStates[i] = i_GridStates[i];
        }
        #ifdef TRINITY_DEBUG
        // inner class checking only when compiled with debug
        if (!si_GridStates[i]->checkMagic())
        {
            ok = false;
            si_GridStates[i]->setMagic();
        }
        #endif
    }
    if (!ok)
        ++i_GridStateErrorCount;
}

Map* MapManager::CreateBaseMap(uint32 id)
{
    Map* map = FindBaseMap(id);

    if (map == NULL)
    {
        std::shared_lock<std::shared_mutex> lock(_mapsLock);

        MapEntry const* entry = sMapStore.LookupEntry(id);
        ASSERT(entry);

        if (entry->Instanceable())
            map = new MapInstanced(id, i_gridCleanUpDelay);
        else
        {
            map = new Map(id, i_gridCleanUpDelay, 0, REGULAR_DIFFICULTY);
            map->LoadRespawnTimes();
            map->LoadCorpseData();
        }

        i_maps[id] = map;

        if (!entry->Instanceable())
            sTransportMgr->SpawnLocalTransports(map);
    }

    ASSERT(map);
    return map;
}

Map* MapManager::FindBaseNonInstanceMap(uint32 mapId) const
{
    Map* map = FindBaseMap(mapId);
    if (map && map->Instanceable())
        return NULL;
    return map;
}

Map* MapManager::CreateMap(uint32 id, Player* player)
{
    Map* m = CreateBaseMap(id);

    if (m && m->Instanceable())
        m = ((MapInstanced*)m)->CreateInstanceForPlayer(id, player);

    return m;
}

Map* MapManager::FindMap(uint32 mapid, uint32 instanceId) const
{
    Map* map = FindBaseMap(mapid);
    if (!map)
        return NULL;

    if (!map->Instanceable())
        return instanceId == 0 ? map : NULL;

    return ((MapInstanced*)map)->FindInstanceMap(instanceId);
}

// Terrace of Endless Spring achievements requirement
enum ToES_achi_check
{
    // Heart of Fear (6 bosses)
    // For Normal
    ACH_THE_DREAD_APPROACH                  = 6718,
    ACH_NIGHTMARE_OF_SHEKZEER               = 6845,
    // For Heroic
    ACH_ZORLOK_H                            = 6725,
    ACH_TAYAK_H                             = 6726,
    ACH_GARALON_H                           = 6727,
    ACH_MELJARAK_H                          = 6728,
    ACH_UNSOK_H                             = 6729,
    ACH_SHEKZEER_H                          = 6730,
    // Mogushan Vaults
    // For Normal
    ACH_GUARDIANS_OF_MOGUSHAN               = 6458,
    ACH_THE_VAULT_OF_MYSTERIES              = 6844,
    // For Heroic
    ACH_STONE_GUARD_H                       = 6719,
    ACH_FENG_H                              = 6720,
    ACH_GARAJAL_H                           = 6721,
    ACH_FOUR_KINGS_H                        = 6722,
    ACH_ELEGON_H                            = 6723,
    ACH_WILL_OF_THE_EMPEROR_H               = 6724,
    // Terrace of Endless Spring
    ACH_TOES_NORMAL                         = 6689,
    /// Strings
    STRING_ID_00                            = 15000,
    STRING_ID_01                            = 15001,
    STRING_ID_02                            = 15002,
    STRING_ID_03                            = 15003,
    STRING_ID_04                            = 15004
};

bool HasCompletedMogushan(Player* p, Difficulty mapDiff)
{
    if (mapDiff == RAID_DIFFICULTY_10MAN_HEROIC || mapDiff == RAID_DIFFICULTY_25MAN_HEROIC)
    {
        if (p->HasAchieved(ACH_STONE_GUARD_H) && p->HasAchieved(ACH_FENG_H) && p->HasAchieved(ACH_GARAJAL_H) && p->HasAchieved(ACH_FOUR_KINGS_H) && p->HasAchieved(ACH_ELEGON_H) && p->HasAchieved(ACH_WILL_OF_THE_EMPEROR_H))
            return true;
        else
        {
            if (p->GetSession())
                p->GetSession()->SendNotification(STRING_ID_04);
            return false;
        }
    }
    else
    {
        if (p->HasAchieved(ACH_GUARDIANS_OF_MOGUSHAN) && p->HasAchieved(ACH_THE_VAULT_OF_MYSTERIES))
            return true;
        else
        {
            if (p->GetSession())
                p->GetSession()->SendNotification(STRING_ID_03);
            return false;
        }
    }
}

bool HasCompletedHoF(Player* p, Difficulty mapDiff)
{
    if (mapDiff == RAID_DIFFICULTY_10MAN_HEROIC || mapDiff == RAID_DIFFICULTY_25MAN_HEROIC)
    {
        if (p->HasAchieved(ACH_ZORLOK_H) && p->HasAchieved(ACH_TAYAK_H) && p->HasAchieved(ACH_GARALON_H) && p->HasAchieved(ACH_MELJARAK_H) && p->HasAchieved(ACH_UNSOK_H) && p->HasAchieved(ACH_SHEKZEER_H))
            return true;
        else
        {
            if (p->GetSession())
                p->GetSession()->SendNotification(STRING_ID_02);
            return false;
        }

    }
    else
    {
        if (p->HasAchieved(ACH_THE_DREAD_APPROACH) && p->HasAchieved(ACH_NIGHTMARE_OF_SHEKZEER))
            return true;
        else
        {
            if (p->GetSession())
                p->GetSession()->SendNotification(STRING_ID_01);
            return false;
        }
    }
}

bool MapManager::CanPlayerEnter(uint32 mapid, Player* player, bool loginCheck)
{
    MapEntry const* entry = sMapStore.LookupEntry(mapid);
    if (!entry)
       return false;

    if (!entry->IsDungeon())
        return true;

    InstanceTemplate const* instance = sObjectMgr->GetInstanceTemplate(mapid);
    if (!instance)
        return false;

    Difficulty targetDifficulty = player->GetDifficulty(entry->IsRaid());

    //The player has a heroic mode and tries to enter into instance which has no a heroic mode
    MapDifficulty const* mapDiff = sDBCManager.GetDownscaledMapDifficultyData(entry->MapID, targetDifficulty);
    if (!mapDiff && mapid != 1112) // something wrong with this solo scenario
    {
        player->SendTransferAborted(mapid, TRANSFER_ABORT_DIFFICULTY_NOT_FOUND);
        return false;
    }
    // FIXME: mapDiff is never used

    //Bypass checks for GMs
    if (player->IsGameMaster())
        return true;

    char const* mapName = entry->name;

    Group* group = player->GetGroup();
    if (entry->IsRaid() && entry->Expansion() >= EXPANSION_MISTS_OF_PANDARIA)
    {
        // can only enter in a raid group
        if ((!group || !group->isRaidGroup()) && !sWorld->getBoolConfig(CONFIG_INSTANCE_IGNORE_RAID))
        {
            /// @todo this is not a good place to send the message
            player->SendGameError(GameError::ERR_RAID_GROUP_ONLY);
            TC_LOG_DEBUG("maps", "MAP: Player '%s' must be in a raid group to enter instance '%s'", player->GetName().c_str(), mapName);
            return false;
        }
    }

    if (!player->IsAlive())
    {
        if (player->HasCorpse())
        {
            // let enter in ghost mode in instance that connected to inner instance with corpse
            uint32 corpseMap = player->GetCorpseLocation().GetMapId();
            do
            {
                if (corpseMap == mapid)
                    break;

                InstanceTemplate const* corpseInstance = sObjectMgr->GetInstanceTemplate(corpseMap);
                corpseMap = corpseInstance ? corpseInstance->Parent : 0;
            } while (corpseMap);

            if (!corpseMap)
            {
                WorldPacket data(SMSG_CORPSE_NOT_IN_INSTANCE);
                player->GetSession()->SendPacket(&data);
                TC_LOG_DEBUG("maps", "MAP: Player '%s' does not have a corpse in instance '%s' and cannot enter.", player->GetName().c_str(), mapName);
                return false;
            }
            TC_LOG_DEBUG("maps", "MAP: Player '%s' has corpse in instance '%s' and can enter.", player->GetName().c_str(), mapName);
            player->ResurrectPlayer(0.5f, false);
            player->SpawnCorpseBones();
        }
        else
            TC_LOG_DEBUG("maps", "Map::CanPlayerEnter - player '%s' is dead but does not have a corpse!", player->GetName().c_str());
    }

    //Get instance where player's group is bound & its map
    if (group)
    {
        InstanceGroupBind* boundInstance = group->GetBoundInstance(entry);
        if (boundInstance && boundInstance->save)
            if (Map* boundMap = sMapMgr->FindMap(mapid, boundInstance->save->GetInstanceId()))
                if (!loginCheck && !boundMap->CanEnter(player))
                    return false;
            /*
                This check has to be moved to InstanceMap::CanEnter()
                // Player permanently bounded to different instance than groups one
                InstancePlayerBind* playerBoundedInstance = player->GetBoundInstance(mapid, player->GetDifficulty(entry->IsRaid()));
                if (playerBoundedInstance && playerBoundedInstance->perm && playerBoundedInstance->save &&
                    boundedInstance->save->GetInstanceId() != playerBoundedInstance->save->GetInstanceId())
                {
                    /// @todo send some kind of error message to the player
                    return false;
                }*/
    }

    // players are only allowed to enter 5 instances per hour
    if (entry->IsDungeon() && (!player->GetGroup() || (player->GetGroup() && !player->GetGroup()->isLFGGroup())))
    {
        uint32 instaceIdToCheck = 0;
        if (InstanceSave* save = player->GetInstanceSave(mapid, entry->IsRaid()))
            instaceIdToCheck = save->GetInstanceId();

        // instanceId can never be 0 - will not be found
        if (!player->CheckInstanceCount(instaceIdToCheck) && !player->isDead() && targetDifficulty != DUNGEON_DIFFICULTY_CHALLENGE)
        {
            player->SendTransferAborted(mapid, TRANSFER_ABORT_TOO_MANY_INSTANCES);
            return false;
        }
    }

    // Terrace of Endless Spring additional checks
    if (mapid == 996)
    {
        if (!HasCompletedHoF(player, targetDifficulty) || !HasCompletedMogushan(player, targetDifficulty))
            return false;
        if (targetDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || targetDifficulty == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            if (!player->HasAchieved(ACH_TOES_NORMAL))
            {
                if (player->GetSession())
                    player->GetSession()->SendNotification(STRING_ID_00);
                return false;
            }
        }
    }

    //Other requirements
    return player->Satisfy(sObjectMgr->GetAccessRequirement(mapid, targetDifficulty), mapid, true);
}

void MapManager::Update(uint32 diff)
{
    i_timer.Update(diff);
    if (!i_timer.Passed())
        return;
    sWorld->RecordTimeDiff(nullptr);
    MapMapType::iterator iter = i_maps.begin();
    for (; iter != i_maps.end(); ++iter)
    {
        if (m_updater.activated())
            m_updater.schedule_update(*iter->second, uint32(i_timer.GetCurrent()));
        else
            iter->second->Update(uint32(i_timer.GetCurrent()));
    }
    if (m_updater.activated())
        m_updater.wait();

    for (iter = i_maps.begin(); iter != i_maps.end(); ++iter)
        iter->second->DelayedUpdate(uint32(i_timer.GetCurrent()));
    sWorld->RecordTimeDiff("MapUpdate");

    i_timer.SetCurrent(0);
}

void MapManager::DoDelayedMovesAndRemoves() { }

bool MapManager::ExistMapAndVMap(uint32 mapid, float x, float y)
{
    GridCoord p = Trinity::ComputeGridCoord(x, y);

    int gx=63-p.x_coord;
    int gy=63-p.y_coord;

    return Map::ExistMap(mapid, gx, gy) && Map::ExistVMap(mapid, gx, gy);
}

bool MapManager::IsValidMAP(uint32 mapid, bool startUp)
{
    MapEntry const* mEntry = sMapStore.LookupEntry(mapid);

    if (startUp)
        return mEntry ? true : false;
    else
        return mEntry && (!mEntry->IsDungeon() || sObjectMgr->GetInstanceTemplate(mapid));

    /// @todo add check for battleground template
}

void MapManager::UnloadAll()
{
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end();)
    {
        iter->second->UnloadAll();
        delete iter->second;
        i_maps.erase(iter++);
    }

    if (m_updater.activated())
        m_updater.deactivate();

    Map::DeleteStateMachine();
}

uint32 MapManager::GetNumInstances()
{
    std::shared_lock<std::shared_mutex> lock(_mapsLock);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;
        MapInstanced::InstancedMaps &maps = ((MapInstanced*)map)->GetInstancedMaps();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon()) ret++;
    }
    return ret;
}

uint32 MapManager::GetNumPlayersInInstances()
{
    std::shared_lock<std::shared_mutex> lock(_mapsLock);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;
        MapInstanced::InstancedMaps &maps = ((MapInstanced*)map)->GetInstancedMaps();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon())
                ret += ((InstanceMap*)mitr->second)->GetPlayers().getSize();
    }
    return ret;
}

void MapManager::InitInstanceIds()
{
    _nextInstanceId = 1;

    if (QueryResult result = CharacterDatabase.Query("SELECT IFNULL(MAX(id), 0) FROM instance"))
        _freeInstanceIds->resize((*result)[0].GetUInt64() + 2, true); // make space for one extra to be able to access [_nextInstanceId] index in case all slots are taken
    else
        _freeInstanceIds->resize(_nextInstanceId + 1, true);

    // never allow 0 id
    _freeInstanceIds->set(0, false);
}

void MapManager::RegisterInstanceId(uint32 instanceId)
{
    // Allocation and sizing was done in InitInstanceIds()
    _freeInstanceIds->set(instanceId, false);

    // Instances are pulled in ascending order from db and nextInstanceId is initialized with 1,
    // so if the instance id is used, increment until we find the first unused one for a potential new instance
    if (_nextInstanceId == instanceId)
        ++_nextInstanceId;
}

uint32 MapManager::GenerateInstanceId()
{
    if (_nextInstanceId == 0xFFFFFFFF)
    {
        TC_LOG_ERROR("maps", "Instance ID overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
        return _nextInstanceId;
    }

    uint32 newInstanceId = _nextInstanceId;
    ASSERT(newInstanceId < _freeInstanceIds->size());
    _freeInstanceIds->set(newInstanceId, false);

    // Find the lowest available id starting from the current NextInstanceId (which should be the lowest according to the logic in FreeInstanceId()
    size_t nextFreedId = _freeInstanceIds->find_next(_nextInstanceId++);
    if (nextFreedId == InstanceIds::npos)
    {
        _nextInstanceId = uint32(_freeInstanceIds->size());
        _freeInstanceIds->push_back(true);
    }
    else
        _nextInstanceId = uint32(nextFreedId);

    return newInstanceId;
}

void MapManager::FreeInstanceId(uint32 instanceId)
{
    // If freed instance id is lower than the next id available for new instances, use the freed one instead
    _nextInstanceId = std::min(instanceId, _nextInstanceId);
    _freeInstanceIds->set(instanceId, true);
}
