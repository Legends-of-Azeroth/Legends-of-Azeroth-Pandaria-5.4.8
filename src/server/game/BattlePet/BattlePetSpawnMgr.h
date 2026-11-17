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

#ifndef BATTLE_PET_SPAWN_MGR
#define BATTLE_PET_SPAWN_MGR

#include "BattlePet.h"
#include "Common.h"

#define BATTLE_PET_SPAWN_MGR_UPDATE 2000

//          replacement  
typedef std::map<ObjectGuid, BattlePet*> BattlePetInfoStore;
//             replaced  replacement
typedef std::map<ObjectGuid, ObjectGuid> CreatureReplacedRelationStore;
//         toBeReplaced
typedef std::set<ObjectGuid> ReadyReplacementStore;

struct BattlePetSpawnTemplate
{
    uint16 Species;
    uint32 Entry;
    uint8 Max;
    uint8 MinLevel;
    uint8 MaxLevel;

    BattlePetInfoStore WildBattlePetInfo;                       // contains BattlePet info for replacement creatures
    CreatureReplacedRelationStore CreaturesRelation;            // contains replaced guids and their replacements
    ReadyReplacementStore CreaturesReadyForReplace;             // contains creatures currently in the world ready for replacement
};

typedef std::vector<BattlePetSpawnTemplate> WildBattlePetSpawnVec;

// handles spawning of battle pets in a particular zone
class BattlePetSpawnZoneMgr
{
public:
    void AddTemplate(BattlePetSpawnTemplate spawnTemplate) { m_spawnTemplates.push_back(spawnTemplate); }

    void PopulateZone(Map* map);
    void DepopulateZone(Map* map);

    void OnAddToZone(Creature* creature);
    void OnRemoveFromZone(Creature* creature);
    void OnRespawn(Creature* creature, BattlePet* battlePet);

    void SpawnCreature(Map* map, ObjectGuid guid, BattlePetSpawnTemplate* spawnTemplate);
    void RemoveCreature(Map* map, ObjectGuid guid, BattlePetSpawnTemplate* spawnTemplate);

    WildBattlePetSpawnVec m_spawnTemplates;
};

typedef std::map<uint32, BattlePetSpawnZoneMgr> WildBattlePetZonePool;
typedef std::map<uint32, WildBattlePetZonePool> WildBattlePetMapPool;

// handles global spawning of battle pets
class BattlePetSpawnMgr
{
public:

    static BattlePetSpawnMgr* instance()
    {
        static BattlePetSpawnMgr _instance;
        return &_instance;
    }

    void Initialise();
    void Update(uint32 diff);

    BattlePet* GetWildBattlePet(Creature* creature);

    void OnAddToMap(Creature* creature);
    void OnRemoveFromMap(Creature* creature);
    void OnRespawn(Creature* creature);

    void EnteredBattle(Creature* creature);
    void LeftBattle(Creature* creature, bool killed);

    void PopulateWorld();
    void DepopulateMap(uint32 map);

private:
    WildBattlePetMapPool m_battlePetMapPools;
    uint32 m_updateTimer;
};

#define sBattlePetSpawnMgr BattlePetSpawnMgr::instance()

#endif
