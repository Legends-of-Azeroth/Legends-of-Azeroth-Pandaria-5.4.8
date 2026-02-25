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
#include "VMapFactory.h"
#include "crypt_of_forgotten_kings.h"
#include "ScenarioMgr.h"

static std::vector<DoorData> const doorData =
{
    { GO_GOLDEN_DOORS_2, DATA_POOL_OF_LIFE, DOOR_TYPE_PASSAGE, BOUNDARY_E },
};

class instance_crypt_of_forgotten_kings : public InstanceMapScript
{
    public:
        instance_crypt_of_forgotten_kings() : InstanceMapScript("instance_crypt_of_forgotten_kings", 1030) { }

        struct instance_crypt_of_forgotten_kings_InstanceMapScript : public InstanceScript
        {
            instance_crypt_of_forgotten_kings_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;

            uint32 chapterOne, chapterTwo, chapterThree, m_essense;
            ObjectGuid jinGUID, AbominationGUID, LifePoolGUID, CryptGuardianHallGUID, CryptGuardianVaultGUID;
            std::list<ObjectGuid> CryptGuardians;
            std::list<Player*> PlayersIn;
            uint32 m_auiEncounter[CHAPTERS];
            std::unordered_map<uint32, ObjectGuid> m_mGoEntryGuidMap;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                LoadDoorData(doorData);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                jinGUID = ObjectGuid::Empty;
                AbominationGUID = ObjectGuid::Empty;
                chapterOne      = 0;
                chapterTwo      = 0;
                chapterThree    = 0;
                m_essense       = 0;

                CryptGuardians.clear();
            }

            void OnPlayerEnter(Player* player) override 
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1030, DATA_JIN_IRONFIST, 0);

                if (GetData(DATA_POOL_OF_LIFE) != IN_PROGRESS)
                    return;

                if (!player->HasAura(SPELL_POOL_OF_LIFE_PROGRESS_BAR))
                    player->CastSpell(player, SPELL_POOL_OF_LIFE_PROGRESS_BAR, false);
                else // if we had server crash, then need remove old bar
                {
                    player->RemoveAurasDueToSpell(SPELL_POOL_OF_LIFE_PROGRESS_BAR);
                    player->CastSpell(player, SPELL_POOL_OF_LIFE_PROGRESS_BAR, false);
                }

                if (m_essense > 0)
                    player->SetPower(POWER_ALTERNATE_POWER, m_essense);
            }

            GameObject* GetGameObjectFromStorage(uint32 uiEntry)
            {
                GameObject* go = NULL;

                std::unordered_map<uint32, ObjectGuid>::iterator find = m_mGoEntryGuidMap.find(uiEntry);

                if (find != m_mGoEntryGuidMap.cend())
                    go = instance->GetGameObject(find->second);

                return go;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_JIN_IRONFIST:
                        jinGUID = creature->GetGUID();
                        break;
                    case NPC_ABOMINATION_OF_ANGER:
                        AbominationGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_CRYPT_GUARDIAN:
                        creature->SetVisible(false);
                        CryptGuardians.push_back(creature->GetGUID());
                        break;
                    case NPC_ESSENCE_OF_HATE:
                        if (GetData(DATA_JIN_IRONFIST) >= DONE)
                            break;

                        creature->SetVisible(false);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_POOL_OF_LIFE:
                        LifePoolGUID = creature->GetGUID();
                        break;
                    case NPC_CRYPT_GUARDIAN_2:
                        CryptGuardianHallGUID = creature->GetGUID();
                        break;
                    case NPC_SHADOWS_OF_ANGER:
                    case NPC_CLOUD_OF_ANGER:
                        creature->SetDisplayId(42332);
                        break;
                    case NPC_WOUNDED_SHADO_PAN:
                        creature->setRegeneratingHealth(false);
                        creature->SetHealth((uint32)creature->GetMaxHealth() * 0.1f);
                        creature->SetStandState(UNIT_STAND_STATE_DEAD);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                switch (unit->GetEntry())
                {
                    case NPC_ESSENCE_OF_HATE:
                        if (m_essense >= 40)
                            break;
                        ++m_essense;
                        SetData(DATA_ESSENCE, m_essense);
                        PlayersIn.clear();
                        GetPlayerListInGrid(PlayersIn, unit, 200.0f);

                        for (auto&& itr : PlayersIn)
                            if (itr->HasAura(SPELL_POOL_OF_LIFE_PROGRESS_BAR) && itr->GetPower(POWER_ALTERNATE_POWER) + 1 < 40)
                                itr->SetPower(POWER_ALTERNATE_POWER, itr->GetPower(POWER_ALTERNATE_POWER) + 1);

                        if (m_essense == 40)
                        {
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_POOL_OF_LIFE_PROGRESS_BAR);

                            if (Creature* LifePool = instance->GetCreature(LifePoolGUID))
                                LifePool->AI()->DoAction(ACTION_CHAPTER_TWO_RESET);
                        }
                        break;
                    case NPC_CRYPT_GUARDIAN_3:
                        if (GameObject* go_vault = GetGameObjectFromStorage(GO_GOLDEN_DOORS_3))
                            go_vault->UseDoorOrButton();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_GOLDEN_DOORS:
                    case GO_GOLDEN_DOORS_2:
                    case GO_GOLDEN_DOORS_3:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_SHA_CORRUPTION:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_JIN_IRONFIST:
                        chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1030, DATA_POOL_OF_LIFE, 0);
                        break;
                    case DATA_POOL_OF_LIFE:
                        chapterTwo = data;

                        if (chapterTwo == DONE)
                        {
                            events.ScheduleEvent(1, 1000); // require delay

                            if (GameObject* go = GetGameObjectFromStorage(GO_GOLDEN_DOORS_2))
                                go->UseDoorOrButton();

                            if (GameObject* goShaCorruption = GetGameObjectFromStorage(GO_SHA_CORRUPTION))
                                goShaCorruption->Delete();

                            if (Creature* HallGuardian = instance->GetCreature(CryptGuardianHallGUID))
                                HallGuardian->AI()->DoAction(ACTION_GUARDIAN_INIT);

                            for (auto&& itr : CryptGuardians)
                            {
                                if (Creature* guardian = instance->GetCreature(itr))
                                {
                                    if (guardian->IsAlive() && guardian->AI())
                                        guardian->AI()->DoAction(ACTION_GUARDIAN_INIT);

                                    guardian->SetVisible(true);
                                }
                            }
                        }
                        break;
                    case DATA_ABOMINATION_OF_ANGER:
                        chapterThree = data;
                        if (chapterThree == DONE)
                        {
                            DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_HEROIC ? 648 : 504);
                            DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_NORMAL ? 648 : 504);
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_ABOMINATION_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        }
                        break;
                    case DATA_ESSENCE:
                        m_essense = data;
                        SaveToDB();
                        break;
                }

                // Synch with data
                SetBossState(type, EncounterState(data));

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_JIN_IRONFIST:
                        return chapterOne;
                    case DATA_POOL_OF_LIFE:
                        return chapterTwo;
                    case DATA_ABOMINATION_OF_ANGER:
                        return chapterThree;
                    case DATA_ESSENCE:
                        return m_essense;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_JIN_IRONFIST:
                        return jinGUID;
                    case NPC_ABOMINATION_OF_ANGER:
                        return AbominationGUID;
                    case NPC_POOL_OF_LIFE:
                        return LifePoolGUID;
                }

                return ObjectGuid::Empty;
            }

            bool IsWipe(float range, Unit* source) override
            {
                Map::PlayerList const &playerList = instance->GetPlayers();

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    Player* player = itr->GetSource();
                    if (!player)
                        continue;

                    if (player->IsAlive() && !player->IsGameMaster())
                        return false;
                }

                return true;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == 1)
                    {
                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1030, DATA_ABOMINATION_OF_ANGER, 0);
                    }
                    break;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "C F K " << GetBossSaveData() << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << m_essense;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* in) override
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

                if (dataHead1 == 'C' && dataHead2 == 'F' && dataHead3 == 'K')
                {
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_JIN_IRONFIST, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_POOL_OF_LIFE, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_ABOMINATION_OF_ANGER, chapterThree);
                    loadStream >> temp;
                    m_essense = temp; // count of progress bar
                    SetData(DATA_ESSENCE, m_essense);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_crypt_of_forgotten_kings_InstanceMapScript(map);
        }
};

void AddSC_instance_crypt_of_forgotten_kings()
{
    new instance_crypt_of_forgotten_kings();
}