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

#include "ScriptPCH.h"
#include "zulgurub.h"

static std::vector<DoorData> const doordata = 
{
    {GO_VENOXIS_EXIT,              DATA_VENOXIS,                DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_MANDOKIR_EXIT1,            DATA_MANDOKIR,               DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_MANDOKIR_EXIT2,            DATA_MANDOKIR,               DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_MANDOKIR_EXIT3,            DATA_MANDOKIR,               DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_MANDOKIR_EXIT4,            DATA_MANDOKIR,               DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_MANDOKIR_EXIT5,            DATA_MANDOKIR,               DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_ZANZIL_EXIT,               DATA_ZANZIL,                 DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_KILNARA_EXIT,              DATA_KILNARA,                DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_THE_CACHE_OF_MADNESS_DOOR, DATA_CACHE_OF_MADNESS_BOSS,  DOOR_TYPE_ROOM,     BOUNDARY_NONE},
};

typedef std::list<WorldObject*> ObjectList;

class instance_zulgurub : public InstanceMapScript
{
    public:
        instance_zulgurub() : InstanceMapScript("instance_zulgurub", 859) { }

        struct instance_zulgurub_InstanceMapScript : public InstanceScript
        {
            instance_zulgurub_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doordata);
                venoxisGUID = ObjectGuid::Empty;
                mandokirGUID = ObjectGuid::Empty;
                kilnaraGUID = ObjectGuid::Empty;
                zanzilGUID = ObjectGuid::Empty;
                jindoGUID = ObjectGuid::Empty;
                cacheOfMadnessBossGUID = ObjectGuid::Empty;
                uiBosses               = 0;

                cacheOfMadnessSummonerGUID = ObjectGuid::Empty;

                for (uint32 type = 0; type < ARTIFACT_TYPE_COUNT; ++type)
                {
                    uint32 count = ARTIFACT_GUIDS_BY_TYPE[type][0];
                    int selected = rand() % count;
                    randomArtifactGUID[type] = ObjectGuid(uint64(ARTIFACT_GUIDS_BY_TYPE[type][selected + 1]));
                }
                artifactsActivated = 0;

                archaeologyQuestAura = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (archaeologyQuestAura)
                    if (!player->HasAura(archaeologyQuestAura))
                        player->CastSpell(player, archaeologyQuestAura, true);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_VENOXIS:
                       venoxisGUID = creature->GetGUID();
                       break;
                    case NPC_MANDOKIR:
                       mandokirGUID = creature->GetGUID();
                       break;
                    case NPC_KILNARA:
                       kilnaraGUID = creature->GetGUID();
                       break;
                    case NPC_ZANZIL:
                       zanzilGUID = creature->GetGUID();
                       break;
                    case NPC_HAZZARAH:
                    case NPC_RENATAKI:
                    case NPC_WUSHOOLAY:
                    case NPC_GRILEK:
                       cacheOfMadnessBossGUID = creature->GetGUID();
                       break;
                    case NPC_JINDO:
                       jindoGUID = creature->GetGUID();
                       break;
                    case NPC_CACHE_OF_MADNESS_SUMMONER:
                        if (creature->GetDBTableGUIDLow() == CACHE_OF_MADNESS_SUMMONER_GUID)
                            cacheOfMadnessSummonerGUID = creature->GetGUID();
                    case NPC_ARTIFACT_DWARVEN:
                    case NPC_ARTIFACT_ELVEN:
                    case NPC_ARTIFACT_TROLL:
                    case NPC_ARTIFACT_FOSSIL:
                        {
                            uint32 entry = creature->GetEntry();
                            uint32 type;
                            for (type = 0; type < ARTIFACT_TYPE_COUNT; ++type)
                                if (entry == ARTIFACT_ENTRIES[type])
                                    break;
                            if (randomArtifactGUID[type] == creature->GetDBTableGUIDLow())
                                randomArtifactGUID[type] = creature->GetGUID();
                            break;
                        }
                    default:
                        break;
                }
            }
            
            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_VENOXIS_EXIT:
                    case GO_MANDOKIR_EXIT1:
                    case GO_MANDOKIR_EXIT2:
                    case GO_MANDOKIR_EXIT3:
                    case GO_MANDOKIR_EXIT4:
                    case GO_MANDOKIR_EXIT5:
                    case GO_ZANZIL_EXIT:
                    case GO_KILNARA_EXIT:
                        AddDoor(go, true);
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state == DONE)
                {
                    uiBosses++;
                    SaveToDB();
                }

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_ARTIFACTS_ACTIVATED:
                        artifactsActivated = data;
                        if (artifactsActivated == 4)
                        {
                            EncounterState state = GetBossState(DATA_CACHE_OF_MADNESS_BOSS);
                            if (state == NOT_STARTED || state == TO_BE_DECIDED)
                                events.ScheduleEvent(EVENT_CACHE_OF_MADNESS_START_SUMMON, 5 * IN_MILLISECONDS);
                        }
                        break;
                    case uint32(-1):
                        archaeologyQuestAura = data;
                        SaveToDB();
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_BOSSES:
                        return uiBosses;
                    case DATA_ARTIFACTS_ACTIVATED:
                        return artifactsActivated;
                    default:
                        break;
                }
                return 0;
            }

            void Update(uint32 diff) override
            {
                if (!instance->HavePlayers())
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_CACHE_OF_MADNESS_START_SUMMON:
                        if (Creature* summoner = instance->GetCreature(cacheOfMadnessSummonerGUID))
                        {
                            summoner->CastSpell(summoner, CACHE_OF_MADNESS_SUMMON_VISUAL, false);
                            events.ScheduleEvent(EVENT_CACHE_OF_MADNESS_SHATTER_ARTIFACTS, 10 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_CACHE_OF_MADNESS_SUMMON, 17 * IN_MILLISECONDS);
                        }
                        break;
                    case EVENT_CACHE_OF_MADNESS_SHATTER_ARTIFACTS:
                        if (Creature* summoner = instance->GetCreature(cacheOfMadnessSummonerGUID))
                        {
                            ObjectList* units = GetWorldObjectsInDist(summoner, 50);
                            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                            {
                                if (!(*itr) || (*itr)->GetTypeId() != TYPEID_UNIT)
                                    continue;

                                Creature* nearby = (*itr)->ToCreature();
                                uint32 nearbyId = nearby->GetEntry();

                                if (nearbyId == NPC_ARTIFACT_ACTIVE_DWARVEN ||
                                    nearbyId == NPC_ARTIFACT_ACTIVE_ELVEN ||
                                    nearbyId == NPC_ARTIFACT_ACTIVE_TROLL ||
                                    nearbyId == NPC_ARTIFACT_ACTIVE_FOSSIL)
                                {
                                    nearby->CastSpell(nearby, CACHE_OF_MADNESS_SHATTER_ARTIFACT);
                                    nearby->DespawnOrUnsummon(1.5f * IN_MILLISECONDS);
                                }
                            }
                        }
                        break;
                    case EVENT_CACHE_OF_MADNESS_SUMMON:
                        if (Creature* summoner = instance->GetCreature(cacheOfMadnessSummonerGUID))
                        {
                            uint32 bossIndex = rand() % CACHE_OF_MADNESS_BOSSES_COUNT;
                            uint32 bossId = CACHE_OF_MADNESS_BOSSES[bossIndex];
                            Position pos = summoner->GetPosition();
                            summoner->SummonCreature(bossId, pos, TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_CACHE_OF_MADNESS_HIDE_PORTAL, 5 * IN_MILLISECONDS);
                        }
                        break;
                    case EVENT_CACHE_OF_MADNESS_HIDE_PORTAL:
                        if (Creature* summoner = instance->GetCreature(cacheOfMadnessSummonerGUID))
                        {
                            summoner->RemoveAurasDueToSpell(97081);
                        }
                    default:
                        break;
                }
            }

            ObjectList* GetWorldObjectsInDist(WorldObject* obj, float dist)
            {
                ObjectList* targets = new ObjectList();
                if (obj)
                {
                    Trinity::AllWorldObjectsInRange u_check(obj, dist);
                    Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(obj, *targets, u_check);
                    obj->VisitNearbyObject(dist, searcher);
                }
                return targets;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_VENOXIS:
                        return venoxisGUID;
                    case DATA_MANDOKIR:
                        return mandokirGUID;
                    case DATA_KILNARA:
                        return kilnaraGUID;
                    case DATA_ZANZIL:
                        return zanzilGUID;
                    case DATA_JINDO:
                        return jindoGUID;
                    case DATA_CACHE_OF_MADNESS_BOSS:
                        return cacheOfMadnessBossGUID;
                    case DATA_ARTIFACT_DWARVEN:
                        return randomArtifactGUID[0];
                    case DATA_ARTIFACT_ELVEN:
                        return randomArtifactGUID[1];
                    case DATA_ARTIFACT_TROLL:
                        return randomArtifactGUID[2];
                    case DATA_ARTIFACT_FOSSIL:
                        return randomArtifactGUID[3];
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "Z G " << uiBosses << " " << GetBossSaveData() << archaeologyQuestAura;

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

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'Z' && dataHead2 == 'G')
                {
                    loadStream >> uiBosses;
                    if (uiBosses > 5)
                        uiBosses = 0;
                    for (uint8 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }
                    loadStream >> archaeologyQuestAura;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        protected:
             ObjectGuid venoxisGUID;
             ObjectGuid mandokirGUID;
             ObjectGuid kilnaraGUID;
             ObjectGuid zanzilGUID;
             ObjectGuid jindoGUID;
             ObjectGuid cacheOfMadnessBossGUID;
             ObjectGuid cacheOfMadnessSummonerGUID;
             ObjectGuid randomArtifactGUID[ARTIFACT_TYPE_COUNT];
             uint32 artifactsActivated;
             uint32 uiBosses;
             uint32 archaeologyQuestAura;
             EventMap events;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_zulgurub_InstanceMapScript(map);
        }
};

void AddSC_instance_zulgurub()
{
    new instance_zulgurub();
}