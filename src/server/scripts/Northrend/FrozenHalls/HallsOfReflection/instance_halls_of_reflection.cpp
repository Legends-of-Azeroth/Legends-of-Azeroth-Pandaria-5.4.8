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

#include "ScriptPCH.h"
#include "halls_of_reflection.h"
#include "Transport.h"

/* Halls of Reflection encounters:
 0 - Falric
 1 - Marwyn
 2 - Frostworn General
 3 - The Lich King
*/

enum Events
{
    EVENT_NONE,
    EVENT_NEXT_WAVE,
    EVENT_START_LICH_KING,
};

struct TrashSpawnPosition
{
    ::Position Position;
    std::vector<Creatures> Entries;
};

uint32 const WaveTrashCount[] = { 0, 3, 4, 4, 5, 0, 4, 4, 5, 5, 0 };

TrashSpawnPosition const TrashSpawnPositions[] =
{
    { { 5275.076f, 2008.724f, 707.7781f, 6.21337200f }, { NPC_WAVE_MAGE,                   NPC_WAVE_PRIEST                                        }, },
    { { 5275.479f, 2001.135f, 707.7781f, 0.17453290f }, { NPC_WAVE_MAGE                                                                           }, },
    { { 5277.365f, 1993.229f, 707.7781f, 0.40142570f }, {                                                                      NPC_WAVE_MERCENARY }, },
    { { 5277.739f, 2016.882f, 707.7781f, 5.96902600f }, { NPC_WAVE_MAGE,                   NPC_WAVE_PRIEST                                        }, },
    { { 5279.649f, 2004.656f, 707.7781f, 0.06981317f }, { NPC_WAVE_MAGE,                   NPC_WAVE_PRIEST                                        }, },
    { { 5280.513f, 1997.842f, 707.7781f, 0.29670600f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5280.632f, 2012.156f, 707.7781f, 6.05629300f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN, NPC_WAVE_MERCENARY }, },
    { { 5282.897f, 2019.597f, 707.7781f, 5.88176000f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST,                    NPC_WAVE_MERCENARY }, },
    { { 5295.635f, 1973.757f, 707.7781f, 1.18682400f }, {                NPC_WAVE_FOOTMAN,                  NPC_WAVE_RIFLEMAN                     }, },
    { { 5295.885f, 2040.342f, 707.7781f, 5.07890800f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST                                        }, },
    { { 5299.250f, 2035.998f, 707.7781f, 5.02654800f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5299.434f, 1979.009f, 707.7781f, 1.23918400f }, {                NPC_WAVE_FOOTMAN,                                     NPC_WAVE_MERCENARY }, },
    { { 5302.247f, 1972.415f, 707.7781f, 1.37881000f }, {                                                                      NPC_WAVE_MERCENARY }, },
    { { 5302.448f, 2042.222f, 707.7781f, 4.90437500f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST                                        }, },
    { { 5306.057f, 2037.002f, 707.7781f, 4.81710900f }, {                NPC_WAVE_FOOTMAN                                                         }, },
    { { 5306.572f, 1977.474f, 707.7781f, 1.50098300f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN, NPC_WAVE_MERCENARY }, },
    { { 5309.577f, 2042.668f, 707.7781f, 4.69493600f }, { NPC_WAVE_MAGE,                   NPC_WAVE_PRIEST                                        }, },
    { { 5311.031f, 1972.229f, 707.7781f, 1.64061000f }, {                                                                      NPC_WAVE_MERCENARY }, },
    { { 5312.752f, 2037.122f, 707.7781f, 4.59021600f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST,                    NPC_WAVE_MERCENARY }, },
    { { 5313.820f, 1978.146f, 707.7781f, 1.74532900f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN,                                     NPC_WAVE_MERCENARY }, },
    { { 5316.701f, 2041.550f, 707.7781f, 4.50294900f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST                                        }, },
    { { 5318.704f, 2036.108f, 707.7781f, 4.22369700f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN, NPC_WAVE_MERCENARY }, },
    { { 5319.158f, 1973.998f, 707.7781f, 1.91986200f }, {                NPC_WAVE_FOOTMAN,                  NPC_WAVE_RIFLEMAN                     }, },
    { { 5320.369f, 1980.125f, 707.7781f, 2.00712900f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN,                  NPC_WAVE_RIFLEMAN                     }, },
    { { 5322.964f, 2040.288f, 707.7781f, 4.34587000f }, {                NPC_WAVE_FOOTMAN                                                         }, },
    { { 5325.072f, 1977.597f, 707.7781f, 2.07694200f }, {                NPC_WAVE_FOOTMAN,                  NPC_WAVE_RIFLEMAN                     }, },
    { { 5335.724f, 1996.859f, 707.7781f, 2.74016700f }, {                NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5336.599f, 2017.278f, 707.7781f, 3.47320500f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN, NPC_WAVE_MERCENARY }, },
    { { 5337.833f, 2010.057f, 707.7781f, 3.22885900f }, { NPC_WAVE_MAGE, NPC_WAVE_FOOTMAN, NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5337.865f, 2003.403f, 707.7781f, 2.98451300f }, {                                  NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5340.836f, 1992.458f, 707.7781f, 2.75762000f }, {                NPC_WAVE_FOOTMAN                                                         }, },
    { { 5343.293f, 1999.384f, 707.7781f, 2.91470000f }, {                NPC_WAVE_FOOTMAN                                                         }, },
    { { 5343.467f, 2015.951f, 707.7781f, 3.49065900f }, {                                  NPC_WAVE_PRIEST, NPC_WAVE_RIFLEMAN                     }, },
    { { 5344.150f, 2007.168f, 707.7781f, 3.15904600f }, {                NPC_WAVE_FOOTMAN                                                         }, },
};

class instance_halls_of_reflection : public InstanceMapScript
{
    public:
        instance_halls_of_reflection() : InstanceMapScript("instance_halls_of_reflection", 668) { }

        struct instance_halls_of_reflection_InstanceMapScript : public InstanceScript
        {
            instance_halls_of_reflection_InstanceMapScript(Map* map) : InstanceScript(map) { };

            ObjectGuid uiFalric;
            ObjectGuid uiMarwyn;
            ObjectGuid uiLichKing;
            ObjectGuid outroJainaSylvanasGuid;
            ObjectGuid frostmourneAltarBunnyGuid = ObjectGuid::Empty;
            std::list<ObjectGuid> trashGuids;
            std::list<ObjectGuid> activeTrashGuids;

            ObjectGuid CaveInGUID = ObjectGuid::Empty;

            ObjectGuid GunshipGUID = ObjectGuid::Empty;
            ObjectGuid CaptainsChestGUID = ObjectGuid::Empty;
            ObjectGuid CaptainGUID = ObjectGuid::Empty;
            std::set<ObjectGuid> GunshipCannonGUIDs;
            std::set<ObjectGuid> GunshipStairGUIDs;

            ObjectGuid uiFrostmourne;
            ObjectGuid uiFrontDoor;
            ObjectGuid uiFrostwornDoor;
            ObjectGuid uiArthasDoor;
            ObjectGuid uiRunDoor;
            uint32 uiWall[4];
            std::vector<ObjectGuid> wallGuids;
            std::vector<ObjectGuid> wallTargetGuids;

            uint32 uiEncounter[MAX_ENCOUNTER];
            uint32 uiTeamInInstance;
            uint32 uiWaveCount;
            uint32 uiIntroDone;
            uint32 uiSummons;
            uint32 uiDataPhase;

            EventMap events;

            void Initialize() override
            {
                events.Reset();

                uiFalric = ObjectGuid::Empty;
                uiMarwyn = ObjectGuid::Empty;
                uiLichKing = ObjectGuid::Empty;
                outroJainaSylvanasGuid = ObjectGuid::Empty;

                uiFrostmourne = ObjectGuid::Empty;
                uiArthasDoor = ObjectGuid::Empty;
                uiFrostwornDoor = ObjectGuid::Empty;
                uiFrontDoor = ObjectGuid::Empty;
                uiTeamInInstance = 0;
                uiWaveCount = 0;
                uiIntroDone = 0;

                for (uint8 i = 0; i < 4; ++i)
                    uiWall[i] = 0;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    uiEncounter[i] = NOT_STARTED;
            }

            void OpenDoor(ObjectGuid guid)
            {
                if (!guid)
                    return;

                if (GameObject* go = instance->GetGameObject(guid))
                    go->SetGoState(GO_STATE_ACTIVE);
            }

            void CloseDoor(ObjectGuid guid)
            {
                if (!guid)
                    return;

                if (GameObject* go = instance->GetGameObject(guid))
                    go->SetGoState(GO_STATE_READY);
            }

            void OnPlayerLeave(Player* player) override
            {
                player->RemoveTimedAchievement(CRITERIA_START_TYPE_EVENT, 22615);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!uiTeamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            uiTeamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_FALRIC:
                        uiFalric = creature->GetGUID();
                        break;
                    case NPC_MARWYN:
                        uiMarwyn = creature->GetGUID();
                        break;
                    case NPC_FROSTWORN_GENERAL:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    case NPC_JAINA_OUTRO:
                    case NPC_SYLVANAS_OUTRO:
                        creature->SetHealth(252000);
                        outroJainaSylvanasGuid = creature->GetGUID();
                        break;
                    case NPC_LICH_KING:
                        creature->SetHealth(20917000);
                        uiLichKing = creature->GetGUID();
                        break;
                    case NPC_ICE_WALL_TARGET:
                        wallTargetGuids.push_back(creature->GetGUID());
                        break;
                    case NPC_FROSTMOURNE_ALTAR_BUNNY:
                        frostmourneAltarBunnyGuid = creature->GetGUID();
                        break;
                    case NPC_JUSTIN_BARTLETT:
                    case NPC_KORM_BLACKSCAR:
                        CaptainGUID = creature->GetGUID();
                        // no break
                    case NPC_CHIEF_ENGINEER_COPPERCLAW:
                        creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        break;
                    case NPC_WORLD_TRIGGER:
                        if (!creature->GetTransport())
                            break;
                        // no break
                    case NPC_GUNSHIP_CANNON_HORDE:
                        GunshipCannonGUIDs.insert(creature->GetGUID());
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_WORLD_TRIGGER:
                    case NPC_GUNSHIP_CANNON_HORDE:
                        GunshipCannonGUIDs.erase(creature->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            uint32 GetGameObjectEntry(uint32 /*guidLow*/, uint32 entry) override
            {
                if (!uiTeamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            uiTeamInInstance = player->GetTeam();
                }

                switch (entry)
                {
                    case GO_THE_CAPTAIN_CHEST_ALLIANCE_NORMAL:
                    case GO_THE_CAPTAIN_CHEST_ALLIANCE_HEROIC:
                    case GO_THE_SKYBREAKER_STAIRS:
                        if (uiTeamInInstance == HORDE)
                            return 0;
                        break;
                    case GO_THE_CAPTAIN_CHEST_HORDE_NORMAL:
                    case GO_THE_CAPTAIN_CHEST_HORDE_HEROIC:
                    case GO_ORGRIMS_HAMMER_STAIRS:
                        if (uiTeamInInstance == ALLIANCE)
                            return 0;
                        break;
                    default:
                        break;
                }

                return entry;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                // TODO: init state depending on encounters
                switch (go->GetEntry())
                {
                    case GO_FROSTMOURNE:
                        uiFrostmourne = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        HandleGameObject(ObjectGuid::Empty, false, go);
                        break;
                    case GO_FROSTMOURNE_ALTAR:
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_FRONT_DOOR:
                        uiFrontDoor = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        OpenDoor(uiFrontDoor);
                        break;
                    case GO_FROSTWORN_DOOR:
                        uiFrostwornDoor = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                        if (uiEncounter[1] == DONE)
                            OpenDoor(uiFrostwornDoor);
                        else
                            CloseDoor(uiFrostwornDoor);
                        break;
                    case GO_RUN_DOOR:
                        uiRunDoor = go->GetGUID();
                        go->SetGoState(GO_STATE_READY);
                        break;
                    case GO_ARTHAS_DOOR:
                        uiArthasDoor = go->GetGUID();
                        break;
                    case GO_ICE_WALL:
                        wallGuids.push_back(go->GetGUID());
                        break;
                    case GO_CAVE:
                        CaveInGUID = go->GetGUID();
                        break;
                    case GO_THE_SKYBREAKER:
                    case GO_ORGRIMS_HAMMER:
                        GunshipGUID = go->GetGUID();
                        break;
                    case GO_THE_SKYBREAKER_STAIRS:
                    case GO_ORGRIMS_HAMMER_STAIRS:
                        if (GetData(DATA_LICHKING_EVENT) == DONE)
                            go->SetRespawnTime(DAY);
                        GunshipStairGUIDs.insert(go->GetGUID());
                        break;
                    case GO_THE_CAPTAIN_CHEST_ALLIANCE_NORMAL:
                    case GO_THE_CAPTAIN_CHEST_HORDE_NORMAL:
                    case GO_THE_CAPTAIN_CHEST_ALLIANCE_HEROIC:
                    case GO_THE_CAPTAIN_CHEST_HORDE_HEROIC:
                        CaptainsChestGUID = go->GetGUID();
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_THE_SKYBREAKER_STAIRS:
                    case GO_ORGRIMS_HAMMER_STAIRS:
                        GunshipStairGUIDs.erase(go->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_WAVE_COUNT && data == SPECIAL)
                {
                    if (!uiWaveCount)
                    {
                        if (GetData(DATA_FALRIC_EVENT) == DONE)
                        {
                            uiWaveCount = 5;
                            SpawnTrash(true);
                        }
                        else
                            SpawnTrash(false);
                    }

                    CloseDoor(uiFrontDoor);
                    uiWaveCount++;
                    events.ScheduleEvent(EVENT_NEXT_WAVE, uiWaveCount == 1 ? 7000 : 3000);
                    return;
                }

                if (type == DATA_WAVE_COUNT && uiWaveCount && data == NOT_STARTED)
                    DoWipe();

                switch (type)
                {
                    case DATA_INTRO_EVENT:
                        uiIntroDone = data;
                        if (data == IN_PROGRESS)
                        {
                            Creature* npc;
                            if (uiTeamInInstance == ALLIANCE)
                                npc = instance->SummonCreature(NPC_JAINA_PART1, Position { 5236.659f, 1929.894f, 707.7781f, 0.8726646f });
                            else
                                npc = instance->SummonCreature(NPC_SYLVANAS_PART1, Position { 5236.667f, 1929.906f, 707.7781f, 0.8377581f });
                        }
                        break;
                    case DATA_FALRIC_EVENT:
                        uiEncounter[0] = data;
                        if (data == DONE)
                        {
                            uiWaveCount++;
                            events.ScheduleEvent(EVENT_NEXT_WAVE, 60000);
                        }
                        break;
                    case DATA_MARWYN_EVENT:
                        uiEncounter[1] = data;
                        if (data == DONE)
                        {
                            OpenDoor(uiFrostwornDoor);
                            OpenDoor(uiFrontDoor);

                            DoUpdateWorldState(WORLD_STATE_HOR, 0);
                            DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 0);

                            if (Creature* bunny = instance->GetCreature(frostmourneAltarBunnyGuid))
                                bunny->CastSpell(bunny, SPELL_START_HALLS_OF_REFLECTION_QUEST_AE, true);
                        }
                        break;
                    case DATA_FROSWORN_EVENT:
                        uiEncounter[2] = data;
                        if (data == DONE)
                        {
                            OpenDoor(uiArthasDoor);
                            SetData(DATA_PHASE, 3);
                        }
                        break;
                    case DATA_LICHKING_EVENT:
                        uiEncounter[3] = data;
                        if (data == IN_PROGRESS)
                        {
                            DoUpdateWorldState(WORLD_STATE_HOR_APPROACHING_LICH_KING, 0);
                            DoUpdateWorldState(WORLD_STATE_HOR_LICH_KING_CHASE, 1);

                            OpenDoor(uiRunDoor);

                            if (instance->IsHeroic())
                                DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_NOT_RETREATING_EVENT);
                        }
                        if (data == NOT_STARTED)
                        {
                            for (auto&& guid : wallGuids)
                                if (GameObject* go = instance->GetGameObject(guid))
                                    go->Delete();
                            wallGuids.clear();

                            for (auto&& guid : wallTargetGuids)
                                if (Creature* creature = instance->GetCreature(guid))
                                    creature->DespawnOrUnsummon();
                            wallTargetGuids.clear();

                            DoUpdateWorldState(WORLD_STATE_HOR_LICH_KING_CHASE, 0);

                            CloseDoor(uiRunDoor);

                            if (Creature* lichKing = instance->GetCreature(uiLichKing))
                                lichKing->DespawnOrUnsummon(10000);
                            if (Creature* outroJainaSylvanas = instance->GetCreature(outroJainaSylvanasGuid))
                                outroJainaSylvanas->DespawnOrUnsummon(10000);

                            DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_NOT_RETREATING_EVENT);
                            DoCastSpellOnPlayers(5); // Kill all players

                            SetData(DATA_PHASE, 3);
                        }
                        if (data == DONE)
                        {
                            DoUpdateWorldState(WORLD_STATE_HOR_LICH_KING_CHASE, 0);

                            if (GameObject* chest = instance->GetGameObject(CaptainsChestGUID))
                                chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);

                            DoUseDoorOrButton(CaveInGUID, 15);

                            if (Creature* outroJainaSylvanas = instance->GetCreature(outroJainaSylvanasGuid))
                                outroJainaSylvanas->CastSpell(outroJainaSylvanas, SPELL_ACHIEV_CHECK, true);

                            DoCompleteAchievement(ACHIEV_HALLS_OF_REFLECTION_N);
                            if (instance->IsHeroic())
                            {
                                DoCompleteAchievement(ACHIEV_HALLS_OF_REFLECTION_H);
                                DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_NOT_RETREATING_EVENT);
                            }
                        }
                        break;
                    case DATA_SUMMONS:
                        if (data == 0)
                            --uiSummons;
                        else 
                            uiSummons = data;
                        data = NOT_STARTED;
                        break;
                    case DATA_ICE_WALL_1:
                        uiWall[0] = data;
                        break;
                    case DATA_ICE_WALL_2:
                        uiWall[1] = data;
                        break;
                    case DATA_ICE_WALL_3:
                        uiWall[2] = data;
                        break;
                    case DATA_ICE_WALL_4:
                        uiWall[3] = data;
                        break;
                    case DATA_PHASE:
                        uiDataPhase = data;
                        if (data == 3)
                        {
                            DoUpdateWorldState(WORLD_STATE_HOR_APPROACHING_LICH_KING, 1);

                            SpawnGunship();

                            instance->SummonCreature(NPC_LICH_KING, OutroSpawns[0]);
                            instance->SummonCreature(uiTeamInInstance == ALLIANCE ? NPC_JAINA_OUTRO : NPC_SYLVANAS_OUTRO, OutroSpawns[1]);
                        }
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_INTRO_EVENT:          return uiIntroDone;
                    case DATA_TEAM_IN_INSTANCE:     return uiTeamInInstance;

                    case DATA_FALRIC_EVENT:         return uiEncounter[0];
                    case DATA_MARWYN_EVENT:         return uiEncounter[1];
                    case DATA_WAVE_COUNT:           return uiWaveCount;

                    case DATA_FROSWORN_EVENT:       return uiEncounter[2];

                    case DATA_LICHKING_EVENT:       return uiEncounter[3];
                    case DATA_ICE_WALL_1:           return uiWall[0];
                    case DATA_ICE_WALL_2:           return uiWall[1];
                    case DATA_ICE_WALL_3:           return uiWall[2];
                    case DATA_ICE_WALL_4:           return uiWall[3];
                    case DATA_SUMMONS:              return uiSummons;

                    case DATA_PHASE:                return uiDataPhase;
                }

                return 0;
            }

            void SetGuidData(uint32 type, ObjectGuid guid) override
            {
                if (type == DATA_WAVE_COUNT)
                {
                    activeTrashGuids.remove(guid);

                    if (activeTrashGuids.empty())
                        SetData(DATA_WAVE_COUNT, SPECIAL);
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_FALRIC:               return uiFalric;
                    case DATA_MARWYN:               return uiMarwyn;
                    case DATA_LICHKING:             return uiLichKing;
                    case DATA_ESCAPE_LIDER:         return outroJainaSylvanasGuid;
                    case DATA_FROSTMOURNE:          return uiFrostmourne;
                    case DATA_FRONT_DOOR:           return uiFrontDoor;
                    case DATA_FROSTWORN_DOOR:       return uiFrostwornDoor;
                    case DATA_ARTHAS_DOOR:          return uiArthasDoor;
                    case DATA_GUNSHIP:              return GunshipGUID;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "H R 1 " << uiEncounter[0] << " " << uiEncounter[1] << " " << uiEncounter[2] << " " << uiEncounter[3] << " " << uiIntroDone;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;
                uint16 version;
                uint16 data0, data1, data2, data3, data4;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> version >> data0 >> data1 >> data2 >> data3 >> data4;

                if (dataHead1 == 'H' && dataHead2 == 'R')
                {
                    uiEncounter[0] = data0;
                    uiEncounter[1] = data1;
                    uiEncounter[2] = data2;
                    uiEncounter[3] = data3;
                    uiIntroDone = data4;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (uiEncounter[i] == IN_PROGRESS)
                            uiEncounter[i] = NOT_STARTED;

                    OpenDoor(uiFrontDoor);
                    if (uiEncounter[1] == DONE)
                        OpenDoor(uiFrostwornDoor);
                    if (uiEncounter[2] == DONE)
                    {
                        uiDataPhase = 2;
                        OpenDoor(uiArthasDoor);
                    }
                    if (uiEncounter[3] == FAIL)
                        uiEncounter[3] = NOT_STARTED;

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void AddWave()
            {
                DoUpdateWorldState(WORLD_STATE_HOR, 1);
                DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, uiWaveCount);

                switch (uiWaveCount)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        SpawnWave();
                        break;
                    case 5:
                        if (GetData(DATA_FALRIC_EVENT) == DONE)
                        {
                            uiWaveCount++;
                            events.ScheduleEvent(EVENT_NEXT_WAVE, 10000);
                        }
                        else if (Creature* pFalric = instance->GetCreature(uiFalric))
                            if (pFalric->AI())
                                pFalric->AI()->DoAction(ACTION_ENTER_COMBAT);
                        break;
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        SpawnWave();
                        break;
                    case 10:
                        if (GetData(DATA_MARWYN_EVENT) != DONE) // wave should not have been started if DONE. Check anyway to avoid bug exploit!
                            if (Creature* pMarwyn = instance->GetCreature(uiMarwyn))
                                if (pMarwyn->AI())
                                    pMarwyn->AI()->DoAction(ACTION_ENTER_COMBAT);
                        break;
                }
            }

            // Wipe has been detected. Perform cleanup and reset.
            void DoWipe()
            {
                if (GetData(DATA_MARWYN_EVENT) != DONE)
                {
                    for (auto&& guid : trashGuids)
                        if (Creature* trash = instance->GetCreature(guid))
                            trash->DespawnOrUnsummon();

                    for (auto&& guid : activeTrashGuids)
                        if (Creature* trash = instance->GetCreature(guid))
                            trash->DespawnOrUnsummon();

                    trashGuids.clear();
                    activeTrashGuids.clear();

                    SetData(DATA_WAVE_COUNT, FAIL);
                    uiWaveCount = 0;
                    events.Reset();
                    DoUpdateWorldState(WORLD_STATE_HOR, 0);
                    DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, uiWaveCount);
                    OpenDoor(uiFrontDoor);

                    if (Creature* falric = instance->GetCreature(uiFalric))
                        falric->SetVisible(false);
                    if (Creature* marwyn = instance->GetCreature(uiMarwyn))
                    {
                        marwyn->SetVisible(false);
                        marwyn->AI()->DoAction(ACTION_SAY_GAUNTLET_WIPE);
                    }
                }
            }

            void SpawnTrash(bool half)
            {
                trashGuids.clear();

                std::list<TrashSpawnPosition> spawns = std::list<TrashSpawnPosition>(std::begin(TrashSpawnPositions), std::end(TrashSpawnPositions));

                if (half)
                {
                    uint32 count = 0;
                    for (uint32 wave = 6; wave <= 9; ++wave)
                        count += WaveTrashCount[wave];

                    Trinity::Containers::RandomResizeList(spawns, count);
                }

                for (auto&& spawn : spawns)
                {
                    if (Creature* trash = instance->SummonCreature(Trinity::Containers::SelectRandomContainerElement(spawn.Entries), spawn.Position))
                    {
                        trashGuids.push_back(trash->GetGUID());
                        trash->CastSpell(trash, SPELL_WELL_OF_SOULS_SMALL, true);
                        trash->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
            }

            void SpawnWave()
            {
                uint32 count = WaveTrashCount[uiWaveCount];
                for (uint32 i = 0; i < count; ++i)
                {
                    if (trashGuids.empty())
                        break;

                    ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(trashGuids);
                    trashGuids.remove(guid);

                    if (Creature* trash = instance->GetCreature(guid))
                    {
                        activeTrashGuids.push_back(trash->GetGUID());
                        trash->CastSpell(trash, SPELL_SPIRIT_ACTIVATE_VISUAL);
                        trash->m_Events.Schedule(5000, [trash]()
                        {
                            trash->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                            trash->AI()->DoZoneInCombat(trash, 100.0f);

                            if (!trash->IsInCombat())
                                trash->AI()->EnterEvadeMode();
                        });
                    }
                }
            }

            void SpawnGunship()
            {
                // don't spawn gunship twice
                if (GunshipGUID)
                    return;

                if (!uiTeamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            uiTeamInInstance = player->GetTeam();
                }

                sTransportMgr->CreateTransport(uiTeamInInstance == HORDE ? GO_ORGRIMS_HAMMER : GO_THE_SKYBREAKER, 0, instance);
            }

            void ProcessEvent(WorldObject* /*obj*/, uint32 eventId) override
            {
                switch (eventId)
                {
                    // Gunship Outro
                    case EVENT_GUNSHIP_ARRIVAL:
                        if (GetData(DATA_LICHKING_EVENT) == DONE)
                            break;

                        if (Transport* gunship = instance->GetTransport(GunshipGUID))
                            gunship->EnableMovement(false);

                        if (Creature* jainaOrSylvanas = instance->GetCreature(outroJainaSylvanasGuid))
                            jainaOrSylvanas->AI()->DoAction(ACTION_GUNSHIP_ARRIVAL);

                        if (Creature* captain = instance->GetCreature(CaptainGUID))
                            captain->AI()->Talk(SAY_BARTLETT_KORM_FIRE);

                        for (ObjectGuid guid : GunshipCannonGUIDs)
                        {
                            uint32 entry = guid.GetEntry();
                            if ((entry == NPC_WORLD_TRIGGER && uiTeamInInstance == ALLIANCE) || (entry == NPC_GUNSHIP_CANNON_HORDE && uiTeamInInstance == HORDE))
                                if (Creature* cannon = instance->GetCreature(guid))
                                    cannon->CastSpell(cannon, SPELL_GUNSHIP_CANNON_FIRE, true);
                        }
                        break;
                    case EVENT_GUNSHIP_ARRIVAL_2:
                        if (Transport* gunship = instance->GetTransport(GunshipGUID))
                            gunship->EnableMovement(false);

                        for (ObjectGuid guid : GunshipStairGUIDs)
                        {
                            if (GameObject* stairs = instance->GetGameObject(guid))
                            {
                                stairs->SetRespawnTime(DAY);
                                stairs->UpdateObjectVisibility();
                            }
                        }

                        if (Creature* jainaOrSylvanas = instance->GetCreature(outroJainaSylvanasGuid))
                            jainaOrSylvanas->AI()->DoAction(ACTION_GUNSHIP_ARRIVAL_2);

                        if (Creature* captain = instance->GetCreature(CaptainGUID))
                            captain->AI()->Talk(SAY_BARTLETT_KORM_FINAL_1);

                        break;
                    default:
                        break;
                }
            }

            void Update(uint32 diff) override
            {
                if (!instance->HavePlayers())
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_NEXT_WAVE:
                        AddWave();
                        break;
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_halls_of_reflection_InstanceMapScript(map);
        }
};

void AddSC_instance_halls_of_reflection()
{
    new instance_halls_of_reflection();
}