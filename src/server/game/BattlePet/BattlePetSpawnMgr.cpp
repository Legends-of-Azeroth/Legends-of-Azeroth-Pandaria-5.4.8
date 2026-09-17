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

#include "BattlePetSpawnMgr.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "DB2Stores.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"

void BattlePetSpawnMgr::Initialise()
{
    m_updateTimer = 0;

    m_battlePetMapPools.clear();

    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT zone, species, entry, max, minLevel, maxLevel FROM battle_pet_wild_pool");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 wild battle pet spawn definitions. DB table `battle_pet_wild_pool` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 zoneId   = fields[0].GetUInt32();
        uint16 species  = fields[1].GetUInt16();
        uint32 npcEntry = fields[2].GetUInt32();
        uint8 max       = fields[3].GetUInt8();
        uint8 minLevel  = fields[4].GetUInt8();
        uint8 maxLevel  = fields[5].GetUInt8();

        // check if battle pet species is valid
        auto speciesEntry = sBattlePetSpeciesStore.LookupEntry(species);
        if (!speciesEntry)
        {
            TC_LOG_INFO("server.loading", "Invalid battle pet species {} in `wild_battlepet_zone_pool` table, skipping!", species);
            continue;
        }

        // check if zone is valid
        int32 mapId = -1;
        for (uint32 i = 0; i < sAreaTableStore.GetNumRows(); ++i)
        {
            if (auto areaInfo = sAreaTableStore.LookupEntry(i))
            {
                if (areaInfo->ID != zoneId)
                    continue;

                mapId = areaInfo->mapid;
                break;
            }
        }

        if (mapId == -1)
        {
            TC_LOG_INFO("server.loading", "Invalid zone id {} in `wild_battlepet_zone_pool` table, skipping!", zoneId);
            continue;
        }

        bool duplicate = false;

        for (uint32 i = 0; i < m_battlePetMapPools[mapId][zoneId].m_spawnTemplates.size(); i++)
        {
            // make sure there are no duplicate entries for the zone
            if (m_battlePetMapPools[mapId][zoneId].m_spawnTemplates[i].Entry == npcEntry)
            {
                duplicate = true;
                break;;
            }
        }

        if (duplicate)
        {
            TC_LOG_INFO("server.loading", "`wild_battlepet_zone_pool` duplicate entry ({}) for map {} and zone {}", npcEntry, mapId, zoneId);
            continue;
        }

        BattlePetSpawnTemplate spawnTemplate;
        spawnTemplate.Entry    = npcEntry;
        spawnTemplate.Species  = species;
        spawnTemplate.Max      = max;
        spawnTemplate.MinLevel = minLevel;
        spawnTemplate.MaxLevel = maxLevel;

        m_battlePetMapPools[mapId][zoneId].AddTemplate(spawnTemplate);

        count++;

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} battle pet spawns in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlePetSpawnMgr::OnAddToMap(Creature* creature)
{
    auto it = m_battlePetMapPools.find(creature->GetMapId());
    if (it == m_battlePetMapPools.end())
        return;

    auto& pool = it->second;
    auto it2 = pool.find(creature->GetZoneId());
    if (it2 != pool.end())
        it2->second.OnAddToZone(creature);
}

void BattlePetSpawnMgr::OnRemoveFromMap(Creature* creature)
{
    if (!creature->IsInGrid() || creature->IsSummon())
        return;

    auto it = m_battlePetMapPools.find(creature->GetMapId());
    if (it == m_battlePetMapPools.end())
        return;

    auto& pool = it->second;
    auto it2 = pool.find(creature->GetZoneId());
    if (it2 != pool.end())
        it2->second.OnRemoveFromZone(creature);
}

void BattlePetSpawnMgr::OnRespawn(Creature* creature)
{
    if (!creature)
        return;

    uint32 zoneId = creature->GetZoneId();
    uint32 mapId = creature->GetMapId();

    // check if there are any wild battle pets for the map
    if (m_battlePetMapPools.find(mapId) == m_battlePetMapPools.end())
        return;

    // check if there are any wild battle pets for the zone
    if (m_battlePetMapPools[mapId].find(zoneId) == m_battlePetMapPools[mapId].end())
        return;

    auto battlePet = GetWildBattlePet(creature);
    if (!battlePet)
        return;

    m_battlePetMapPools[mapId][zoneId].OnRespawn(creature, battlePet);
}

void BattlePetSpawnMgr::PopulateWorld()
{
    // populate wild battle pets on all maps
    for (auto &battlePetPool : m_battlePetMapPools)
        for (auto &zonePool : battlePetPool.second)
        {
            auto map = sMapMgr->FindBaseMap(battlePetPool.first);
            if (map)
                zonePool.second.PopulateZone(map);
        }
}

void BattlePetSpawnMgr::DepopulateMap(uint32 mapId)
{
    // check if there are any wild battle pets for the map
    if (m_battlePetMapPools.find(mapId) == m_battlePetMapPools.end())
        return;

    // depopulate all wild battle pets in a map
    for (auto &mapPool : m_battlePetMapPools[mapId])
    {
        auto map = sMapMgr->FindBaseMap(mapId);
        mapPool.second.DepopulateZone(map);
    }
}

void BattlePetSpawnMgr::Update(uint32 diff)
{
    m_updateTimer += diff;

    // update wild battle pets every 2 seconds
    if (m_updateTimer >= BATTLE_PET_SPAWN_MGR_UPDATE)
    {
        PopulateWorld();
        m_updateTimer = 0;
    }
}

BattlePet* BattlePetSpawnMgr::GetWildBattlePet(Creature* creature)
{
    if (!creature)
        return nullptr;

    uint32 zoneId = creature->GetZoneId();
    uint32 mapId = creature->GetMapId();

    // find wild battle pet in map and zone
    for (auto &spawnTemplate : m_battlePetMapPools[mapId][zoneId].m_spawnTemplates)
        if (spawnTemplate.WildBattlePetInfo.find(creature->GetGUID()) != spawnTemplate.WildBattlePetInfo.end())
            return spawnTemplate.WildBattlePetInfo[creature->GetGUID()];

    return nullptr;
}

void BattlePetSpawnMgr::EnteredBattle(Creature* creature)
{
    // remove creature from world
    creature->ForcedDespawn();
    creature->SetRespawnTime(MONTH);
    creature->RemoveCorpse(false);
}

void BattlePetSpawnMgr::LeftBattle(Creature* creature, bool killed)
{
    // wild pet battle defeated player or pet battle was abandoned
    if (!killed)
    {
        auto battlePet = GetWildBattlePet(creature);
        if (!battlePet)
            return;

        battlePet->SetCurrentHealth(battlePet->GetMaxHealth());

        // respawn wild battle pet in the world
        creature->Respawn(true);
    }
    else
    {
        uint32 zoneId = creature->GetZoneId();
        uint32 mapId = creature->GetMapId();

        // remove battle pet and allow spawning of orginal creature
        for (auto &spawnTemplate : m_battlePetMapPools[mapId][zoneId].m_spawnTemplates)
            for (auto &creatureRelation : spawnTemplate.CreaturesRelation)
                if (creatureRelation.second == creature->GetGUID())
                    m_battlePetMapPools[mapId][zoneId].RemoveCreature(creature->GetMap(), creatureRelation.first, &spawnTemplate);
    }
}

// -------------------------------------------------------------------------------

void BattlePetSpawnZoneMgr::PopulateZone(Map* map)
{
    for (auto &spawnTemplate : m_spawnTemplates)
    {
        // check if too many of one type of battle pet has spawned in the zone
        if (spawnTemplate.Max <= spawnTemplate.CreaturesRelation.size())
            continue;

        // check if there are creatures to replace
        if (!spawnTemplate.CreaturesReadyForReplace.size())
            continue;

        // replace all creatures
        for (auto &replacement : spawnTemplate.CreaturesReadyForReplace)
            SpawnCreature(map, replacement, &spawnTemplate);

        spawnTemplate.CreaturesReadyForReplace.clear();
    }
}

void BattlePetSpawnZoneMgr::DepopulateZone(Map* map)
{
    // remove all wild battle pets from the zone
    for (auto &spawnTemplate : m_spawnTemplates)
        for (auto &relationTemplate : spawnTemplate.CreaturesRelation)
            RemoveCreature(map, relationTemplate.first, &spawnTemplate);
}

// called when a creature is added to the zone
void BattlePetSpawnZoneMgr::OnAddToZone(Creature* creature)
{
    for (auto&& spawnTemplate : m_spawnTemplates)
    {
        // if creature isn't in the replacement queue, add it
        if (spawnTemplate.Entry == creature->GetEntry() &&
            spawnTemplate.CreaturesReadyForReplace.find(creature->GetGUID()) == spawnTemplate.CreaturesReadyForReplace.end())
            spawnTemplate.CreaturesReadyForReplace.insert(creature->GetGUID());
    }
}

// called when a creature is removed from the zone
void BattlePetSpawnZoneMgr::OnRemoveFromZone(Creature* creature)
{
    for (auto&& spawnTemplate : m_spawnTemplates)
    {
        // if creature is in the replacement queue, remove it
        if (spawnTemplate.Entry == creature->GetEntry() &&
            spawnTemplate.CreaturesReadyForReplace.find(creature->GetGUID()) != spawnTemplate.CreaturesReadyForReplace.end())
            spawnTemplate.CreaturesReadyForReplace.erase(creature->GetGUID());
    }
}

void BattlePetSpawnZoneMgr::OnRespawn(Creature* creature, BattlePet* battlePet)
{
    ASSERT(battlePet);

    // update wild battle pet flags
    creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_WILDPET_CAPTURABLE);
    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
}

void BattlePetSpawnZoneMgr::SpawnCreature(Map* map, ObjectGuid guid, BattlePetSpawnTemplate* spawnTemplate)
{
    if (!spawnTemplate)
        return;

    // check if too many of this type of battle pet has spawned in the zone
    if (spawnTemplate->Max <= spawnTemplate->CreaturesRelation.size())
        return;

    // make sure creature that is being replaced exists in the world
    auto creature = map->GetCreature(guid);
    if (!creature)
        return;

    // this also checked when loading spawn templates from database
    auto speciesEntry = sBattlePetSpeciesStore.LookupEntry(spawnTemplate->Species);
    if (!speciesEntry)
        return;

    // initialise replacement creature
    Creature* replacementCreature = new Creature();
    replacementCreature->m_isTempWorldObject = true;

    if (!replacementCreature->Create(map->GenerateLowGuid<HighGuid::Unit>(), creature->GetMap(), creature->GetPhaseMask(),
        speciesEntry->NpcId, 0, 0, creature->m_positionX, creature->m_positionY, creature->m_positionZ, creature->GetOrientation()))
    {
        // something went wrong, delete newly created creature
        delete replacementCreature;
        return;
    }

    // apply random movement to replacement creature
    replacementCreature->SetWanderDistance(20.0f);
    replacementCreature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
    replacementCreature->Respawn(true);

    // generate battle pet information for replacement creature
    uint8 breed   = sObjectMgr->BattlePetGetRandomBreed(spawnTemplate->Species);
    uint8 quality = sObjectMgr->BattlePetGetRandomQuality(spawnTemplate->Species);
    uint8 level   = urand(spawnTemplate->MinLevel, spawnTemplate->MaxLevel);

    auto battlePet = new BattlePet(0, spawnTemplate->Species, speciesEntry->FamilyId, level, quality, breed);
    battlePet->InitialiseAbilities(true);

    spawnTemplate->WildBattlePetInfo[replacementCreature->GetGUID()] = battlePet;

    // update replacement creature
    replacementCreature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_WILDPET_CAPTURABLE);
    replacementCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    replacementCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
    replacementCreature->SetUInt32Value(UNIT_FIELD_WILD_BATTLE_PET_LEVEL, level);

    creature->GetMap()->AddToMap(replacementCreature);

    // despawn replaced creature
    creature->ForcedDespawn();
    creature->SetRespawnTime(MONTH);
    creature->RemoveCorpse(false);

    spawnTemplate->CreaturesRelation[creature->GetGUID()] = replacementCreature->GetGUID();
}

void BattlePetSpawnZoneMgr::RemoveCreature(Map* map, ObjectGuid guid, BattlePetSpawnTemplate* spawnTemplate)
{
    if (!spawnTemplate)
        return;

    // check if the creature was replaced by a wild battle pet
    if (spawnTemplate->CreaturesRelation.find(guid) == spawnTemplate->CreaturesRelation.end())
        return;

    // make sure the creature that was replaced still exists in the world
    auto creature = map->GetCreature(guid);
    if (!creature)
        return;

    // make sure the creature that was the replacement still exists in the world
    auto replacementCreature = map->GetCreature(spawnTemplate->CreaturesRelation[guid]);
    if (!replacementCreature)
        return;

    {
        // remove battle pet information
        auto itr = spawnTemplate->WildBattlePetInfo.find(replacementCreature->GetGUID());
        if (itr != spawnTemplate->WildBattlePetInfo.end())
        {
            delete itr->second;
            spawnTemplate->WildBattlePetInfo.erase(itr);
        }
    }

    // remove replacement creature
    replacementCreature->RemoveFromWorld();
    replacementCreature->AddObjectToRemoveList();

    spawnTemplate->CreaturesRelation.erase(spawnTemplate->CreaturesRelation[guid]);

    // allow original creature to spawn again
    creature->SetRespawnTime(creature->GetCreatureData()->spawntimesecs);
}
