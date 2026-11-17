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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "CreatureTextMgr.h"
#include "shadopan_monastery.h"

Position snowdriftCenterPos = {3659.08f, 3015.38f, 804.74f};

static std::vector<DoorData> const doorData =
{
    {GO_CLOUDSTRIKE_ENTRANCE, DATA_GU_CLOUDSTRIKE,   DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_CLOUDSTRIKE_EXIT,     DATA_GU_CLOUDSTRIKE,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_SNOWDRIFT_DOJO_DOOR,  DATA_MASTER_SNOWDRIFT, DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_SNOWDRIFT_EXIT,       DATA_MASTER_SNOWDRIFT, DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_SHA_ENTRANCE,         DATA_SHA_VIOLENCE,     DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_SHA_EXIT,             DATA_SHA_VIOLENCE,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { DATA_GU_CLOUDSTRIKE,   CRITERIA_GU_CLOUDSTRIKE   },
    { DATA_MASTER_SNOWDRIFT, CRITERIA_MASTER_SNOWDRIFT },
    { DATA_SHA_VIOLENCE,     CRITERIA_SHA_VIOLENCE     },
    { DATA_TARAN_ZHU,        CRITERIA_TARAN_ZHU        },
};


class instance_shadopan_monastery : public InstanceMapScript
{
    public: instance_shadopan_monastery() : InstanceMapScript("instance_shadopan_monastery", 959) { }

    struct instance_shadopan_monastery_InstanceMapScript : public InstanceScript
    {
        instance_shadopan_monastery_InstanceMapScript(Map* map) : InstanceScript(map) { }

        std::list<ObjectGuid> firstArcherySet;
        std::list<ObjectGuid> secondArcherySet;
        std::list<ObjectGuid> archeryTargetGuids;
        std::list<ObjectGuid> minibossPositionsGuid;
        std::list<ObjectGuid> minibossPositionsGuidSave;
        std::list<ObjectGuid> firstDefeatedNovicePositionsGuid;
        std::list<ObjectGuid> secondDefeatedNovicePositionsGuid;
        std::list<ObjectGuid> firstDefeatedNovicePositionsGuidSave;
        std::list<ObjectGuid> secondDefeatedNovicePositionsGuidSave;

        uint8 aliveNoviceCount;
        uint8 aliveMinibossCount;
        uint32 dataStorage[MAX_DATA];
        uint32 iceArchersState;
        uint32 fireArchersState;
        uint32 snowdriftState;
        ObjectGuid guCloudstikeGUID;
        ObjectGuid masterSnowdriftGUID;
        ObjectGuid shaViolenceGuid;
        ObjectGuid taranZhuGuid;
        ObjectGuid azureSerpentGUID;
        ObjectGuid snowdriftRefereeGUID;
        ObjectGuid taranZhuCache;
        ObjectGuid snowdriftPossesions;
        ObjectGuid taranZhuCacheH;
        ObjectGuid snowdriftPossesionsH;
        ObjectGuid masterSnowdriftOutroGUID;
        ObjectGuid shadowriftEntance;
        ObjectGuid shadowriftFireWall;
        ObjectGuid shaPrison1GUID;
        ObjectGuid shaPrison2GUID;
        ObjectGuid shaPrison3GUID;
        ObjectGuid containOverrideGUID;

        void Initialize() override
        {
            SetBossNumber(EncounterCount);
            LoadDoorData(doorData);
            memset(dataStorage, 0, MAX_DATA * sizeof(uint32));
            iceArchersState          = NOT_STARTED;
            fireArchersState         = NOT_STARTED;
            snowdriftState           = NOT_STARTED;
            aliveNoviceCount         = MAX_NOVICE;
            aliveMinibossCount       = 2;
            guCloudstikeGUID = ObjectGuid::Empty;
            masterSnowdriftGUID = ObjectGuid::Empty;
            shaViolenceGuid = ObjectGuid::Empty;
            taranZhuGuid = ObjectGuid::Empty;
            azureSerpentGUID = ObjectGuid::Empty;
            snowdriftRefereeGUID = ObjectGuid::Empty;
            taranZhuCache            = ObjectGuid::Empty;
            snowdriftPossesions      = ObjectGuid::Empty;
            taranZhuCacheH           = ObjectGuid::Empty;
            snowdriftPossesionsH     = ObjectGuid::Empty;
            masterSnowdriftOutroGUID = ObjectGuid::Empty;
            shadowriftEntance        = ObjectGuid::Empty;
            shadowriftFireWall       = ObjectGuid::Empty;
            shaPrison1GUID           = ObjectGuid::Empty;
            shaPrison2GUID           = ObjectGuid::Empty;
            shaPrison3GUID           = ObjectGuid::Empty;
            containOverrideGUID = ObjectGuid::Empty;
            firstArcherySet.clear();
            secondArcherySet.clear();

            if (instance->IsChallengeDungeon())
                LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);

            instance->SetWorldState(WORLDSTATE_RESPECT, 1); // by default
        }

        void OnPlayerEnter(Player* player) override
        {
            if (instance->IsChallengeDungeon())
                SendChallengeInfo(player, SCENARIO_ID);
        }

        void OnCreatureCreate(Creature* creature) override
        {
            if (instance->IsChallengeDungeon() && creature->isDead())
                creature->Respawn();

            switch (creature->GetEntry())
            {
                case NPC_GU_CLOUDSTRIKE:
                    guCloudstikeGUID = creature->GetGUID();
                    break;
                case NPC_MASTER_SNOWDRIFT:
                    masterSnowdriftGUID = creature->GetGUID();
                    break;
                case NPC_SNOWDRIFT_QUESTENDER:
                    masterSnowdriftOutroGUID = creature->GetGUID();
                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    creature->SetVisible(false);
                    break;
                case NPC_SHA_VIOLENCE:
                    shaViolenceGuid = creature->GetGUID();
                    break;
                case NPC_TARAN_ZHU:
                    taranZhuGuid = creature->GetGUID();
                    break;
                case NPC_AZURE_SERPENT:
                    azureSerpentGUID = creature->GetGUID();
                    break;
                case NPC_PANDAREEN_REFEREE:
                    snowdriftRefereeGUID = creature->GetGUID();
                    break;
                case NPC_ARCHERY_TARGET:
                    archeryTargetGuids.push_back(creature->GetGUID()); 
                    break;
                case NPC_HATEFUL_ESSENCE:
                    creature->SetDisplayId(41106);
                    break;
                case NPC_SNOWDRIFT_POSITION:
                {
                    if (creature->GetDistance(snowdriftCenterPos) > 5.0f && creature->GetDistance(snowdriftCenterPos) < 15.0f)
                    {
                        minibossPositionsGuid.push_back(creature->GetGUID());
                        minibossPositionsGuidSave.push_back(creature->GetGUID());
                    }
                    else if (creature->GetDistance(snowdriftCenterPos) > 15.0f  && creature->GetDistance(snowdriftCenterPos) < 18.5f)
                    {
                        firstDefeatedNovicePositionsGuid.push_back(creature->GetGUID());
                        firstDefeatedNovicePositionsGuidSave.push_back(creature->GetGUID());
                    }
                    else if (creature->GetDistance(snowdriftCenterPos) > 15.0f && creature->GetDistance(snowdriftCenterPos) < 25.0f)
                    {
                        secondDefeatedNovicePositionsGuid.push_back(creature->GetGUID());
                        secondDefeatedNovicePositionsGuidSave.push_back(creature->GetGUID());
                    }

                    break;
                }
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_TARAN_ZHU_CACHE:
                    taranZhuCache = go->GetGUID();
                    break;
                case GO_SNOWDRIFT_POSSESSIONS:
                    snowdriftPossesions = go->GetGUID();
                    break;
                case GO_SNOWDRIFT_POSSISIONS_H:
                    snowdriftPossesionsH = go->GetGUID();
                    break;
                case GO_TARAN_ZHU_CACHE_H:
                    taranZhuCacheH = go->GetGUID();
                    break;
                case GO_SNOWDRIFT_ENTRANCE:
                    shadowriftEntance = go->GetGUID();
                    break;
                case GO_SNOWDRIFT_FIRE_WALL:
                    shadowriftFireWall = go->GetGUID();
                    break;
                case GO_CLOUDSTRIKE_ENTRANCE:
                case GO_CLOUDSTRIKE_EXIT:
                case GO_SNOWDRIFT_DOJO_DOOR:
                case GO_SNOWDRIFT_EXIT:
                case GO_SHA_ENTRANCE:
                case GO_SHA_EXIT:
                    AddDoor(go, true);
                    break;
                case GO_CHALLENGE_DOOR:
                    SetChallengeDoorGuid(go->GetGUID());
                    break;
                case GO_SHA_PRISON_1:
                    shaPrison1GUID = go->GetGUID();
                    break;
                case GO_SHA_PRISON_2:
                    shaPrison2GUID = go->GetGUID();
                    break;
                case GO_SHA_PRISON_3:
                    shaPrison3GUID = go->GetGUID();
                    break;
                case GO_CONTAINMENT_OVERRIDE:
                    containOverrideGUID = go->GetGUID();
                    break;
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                if (Creature* creature = unit->ToCreature())
                    UpdateConditionInfo(creature, ENEMIES_COUNT);
        }

        void Update(uint32 diff) override
        {
            ScheduleBeginningTimeUpdate(diff);
            ScheduleChallengeStartup(diff);
            ScheduleChallengeTimeUpdate(diff);
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case DATA_MASTER_SNOWDRIFT:
                {
                    if (state == DONE)
                        DoRespawnGameObject(instance->IsHeroic() ? snowdriftPossesionsH : snowdriftPossesions, 1 * DAY);
                    break;
                }
                case DATA_TARAN_ZHU:
                {
                    switch (state)
                    {
                        case IN_PROGRESS:
                        {
                            Map::PlayerList const &PlayerList = instance->GetPlayers();

                            if (!PlayerList.isEmpty())
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            if (Player* player = i->GetSource())
                                player->AddAura(SPELL_HATE, player);
                            break;
                        }
                        case FAIL:
                        case NOT_STARTED:
                        case SPECIAL:
                            {
                                  Map::PlayerList const &PlayerList = instance->GetPlayers();
                                  if (!PlayerList.isEmpty())
                                      for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                          if (Player* player = i->GetSource())
                                          {
                                               player->RemoveAurasDueToSpell(SPELL_HATE);
                                               player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE);
                                               player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE_VISUAL);
                                          }
                            }
                            break;
                        case DONE:
                        {
                            Map::PlayerList const &PlayerList = instance->GetPlayers();
                            if (!PlayerList.isEmpty())
                               for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                               if (Player* player = i->GetSource())
                               {
                                  player->RemoveAurasDueToSpell(SPELL_HATE);
                                  player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE);
                                  player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE_VISUAL);
                               }

                            DoRespawnGameObject(instance->IsHeroic() ? taranZhuCacheH : taranZhuCache, 1 * DAY);
                        }
                        case TO_BE_DECIDED:
                        break;
                    }
                    break;
                }
            }
            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_DEFEATED_NOVICE:
                {
                    if (!--aliveNoviceCount)
                       if (Creature* referee = instance->GetCreature(snowdriftRefereeGUID))
                          if (referee->IsAIEnabled)
                              referee->AI()->DoAction(1); // ACTION_NOVICE_DONE
                    break;
                }
                case DATA_DEFEATED_MINIBOSS:
                {
                    if (!--aliveMinibossCount)
                    {
                        if (Creature* referee = instance->GetCreature(snowdriftRefereeGUID))
                           if (referee->IsAIEnabled)
                               referee->AI()->DoAction(2); // ACTION_MINIBOSS_DONE
                    }
                    break;
                }
                case DATA_ARCHERY:
                {
                    iceArchersState = data;
                    switch (data)
                    {
                        case IN_PROGRESS:
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                if (Creature* archer = instance->SummonCreature(NPC_ARCHERY_FIRST, IceArchersPosition[i]))
                                {
                                    firstArcherySet.push_back(archer->GetGUID());

                                    if (i == 2)
                                        sCreatureTextMgr->SendChat(archer, 0, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                }
                            }
                            break;
                        }
                        case DONE:
                        {
                            if (Creature* archer = instance->GetCreature(firstArcherySet.back()))
                                sCreatureTextMgr->SendChat(archer, 1, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);

                            for (auto &&itr : firstArcherySet)
                            {
                                if (auto const archer = instance->GetCreature(itr))
                                    archer->DespawnOrUnsummon();
                            }
                            break;
                        }
                    }
                    break;
                }
                case DATA_FIRE_ARCHERY:
                {
                    fireArchersState = data;
                    switch (data)
                    {
                        case IN_PROGRESS:
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                if (Creature* archer = instance->SummonCreature(NPC_ARCHERY_SECOND, FireArchersPosition[i]))
                                {
                                    secondArcherySet.push_back(archer->GetGUID());

                                    if (i == 2)
                                        sCreatureTextMgr->SendChat(archer, 0, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
                                }
                            }
                            break;
                        }
                        case DONE:
                        {
                            if (Creature* archer = instance->GetCreature(secondArcherySet.back()))
                                sCreatureTextMgr->SendChat(archer, 1, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);

                            for (auto &&itr : secondArcherySet)
                            {
                                if (auto const archer = instance->GetCreature(itr))
                                    archer->DespawnOrUnsummon();
                            }
                            break;
                        }
                    }
                    break;
                }
                case DATA_SNOWDRIFT_STATE:
                    snowdriftState = data;
                    if (data == IN_PROGRESS || data == FAIL)
                    {
                        aliveNoviceCount = MAX_NOVICE;
                        aliveMinibossCount = 2;
                        minibossPositionsGuid = minibossPositionsGuidSave;
                        firstDefeatedNovicePositionsGuid = firstDefeatedNovicePositionsGuidSave;
                        secondDefeatedNovicePositionsGuid = secondDefeatedNovicePositionsGuidSave;
                    }
                    switch (data)
                    {
                        case IN_PROGRESS:
                            HandleGameObject(shadowriftEntance, false);
                            break;
                        case DONE:
                            HandleGameObject(shadowriftFireWall, true);
                        case FAIL:
                        case NOT_STARTED:
                            HandleGameObject(shadowriftEntance, true);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    if (type < MAX_DATA)
                        dataStorage[type] = data;
                    break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_ARCHERY:
                    return iceArchersState;
                case DATA_SNOWDRIFT_STATE:
                    return snowdriftState;
                case DATA_FIRE_ARCHERY:
                    return fireArchersState;
                default:
                {
                    if (type < MAX_DATA)
                        return dataStorage[type];
                }
                break;
            }

            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_GU_CLOUDSTRIKE:
                    return guCloudstikeGUID;
                case DATA_MASTER_SNOWDRIFT:
                    return masterSnowdriftGUID;
                case NPC_SNOWDRIFT_QUESTENDER:
                    return masterSnowdriftOutroGUID;
                case NPC_SHA_VIOLENCE:
                    return shaViolenceGuid;
                case NPC_TARAN_ZHU:
                    return taranZhuGuid;
                case DATA_AZURE_SERPENT:
                    return azureSerpentGUID;
                case DATA_POSSESSIONS:
                    return instance->IsHeroic() ? snowdriftPossesionsH : snowdriftPossesions;
                case DATA_TARAN_ZHU_CACHE:
                    return instance->IsHeroic() ? taranZhuCacheH : taranZhuCache;
                case DATA_PANDAREN_REFEREE:
                    return snowdriftRefereeGUID;
                case DATA_ARCHERY_TARGET:
                    return Trinity::Containers::SelectRandomContainerElement(archeryTargetGuids);
                case DATA_RANDOM_FIRST_POS:
                {
                    if (firstDefeatedNovicePositionsGuid.empty())
                        return ObjectGuid::Empty;

                    if (ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(firstDefeatedNovicePositionsGuid))
                    {
                        const_cast<instance_shadopan_monastery_InstanceMapScript*>(this)->firstDefeatedNovicePositionsGuid.remove(guid);
                        return guid;
                    }
                    break;
                }
                case DATA_RANDOM_SECOND_POS:
                {
                     if (secondDefeatedNovicePositionsGuid.empty())
                        return ObjectGuid::Empty;

                     if (ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(secondDefeatedNovicePositionsGuid))
                     {
                         const_cast<instance_shadopan_monastery_InstanceMapScript*>(this)->secondDefeatedNovicePositionsGuid.remove(guid);
                         return guid;
                     }
                     break;
                }
                case DATA_RANDOM_MINIBOSS_POS:
                {
                    if (minibossPositionsGuid.empty())
                       return ObjectGuid::Empty;

                    if (ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(minibossPositionsGuid))
                    {
                        const_cast<instance_shadopan_monastery_InstanceMapScript*>(this)->minibossPositionsGuid.remove(guid);
                        return guid;
                    }
                    break;
                }
                case DATA_SHA_PRISON_1:
                    return shaPrison1GUID;
                case DATA_SHA_PRISON_2:
                    return shaPrison2GUID;
                case DATA_SHA_PRISON_3:
                    return shaPrison3GUID;
                case DATA_CONTAINMENT_OVERRIDE:
                    return containOverrideGUID;
            }

            return ObjectGuid::Empty;
        }

        std::string GetSaveData() override
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "S P M " << GetBossSaveData() << snowdriftState << " " << iceArchersState << " " << fireArchersState;

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

            char dataHead1, dataHead2, dataHead3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> dataHead3;

            if (dataHead1 == 'S' && dataHead2 == 'P' && dataHead3 == 'M')
            {
                for (uint8 i = 0; i < EncounterCount; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;

                    SetBossState(i, EncounterState(tmpState));
                }

                uint32 temp = 0;
                loadStream >> temp;
                snowdriftState = temp ? DONE : NOT_STARTED;

                loadStream >> temp;
                iceArchersState = temp ? DONE : NOT_STARTED;

                loadStream >> temp;
                fireArchersState = temp ? DONE : NOT_STARTED;
            }
            else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_shadopan_monastery_InstanceMapScript(map);
    }
};

void AddSC_instance_shadopan_monastery()
{
    new instance_shadopan_monastery();
}
