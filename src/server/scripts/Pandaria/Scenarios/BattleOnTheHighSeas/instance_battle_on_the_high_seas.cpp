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
#include "battle_on_the_high_seas.h"
#include "ScenarioMgr.h"

class instance_battle_on_the_high_seas : public InstanceMapScript
{
    public:
        instance_battle_on_the_high_seas() : InstanceMapScript("instance_battle_on_the_high_seas", 1099) { }

        struct instance_battle_on_the_high_seas_InstanceMapScript : public InstanceScript
        {
            instance_battle_on_the_high_seas_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            uint32 m_auiEncounter[4];
            uint32 chapterOne, chapterTwo, chapterThree, chapterFour;
            uint32 factionId;
            uint32 explosiveBarrelsCount;
            uint32 plantBarrelCount;
            ObjectGuid hodgsonGUID;
            ObjectGuid hagmanGUID;
            ObjectGuid hordeMainTransportCannonGUID;
            ObjectGuid hordeCaptainGUID;
            ObjectGuid hordeRopeGUID;
            ObjectGuid hordeOutTransportCannonGUID;
            ObjectGuid rapierGUID;
            ObjectGuid allianceRopeGUID;
            std::vector<ObjectGuid> fireBunnyGUIDs;
            std::vector<ObjectGuid> allianceToHordeTransportGUIDs;
            std::vector<ObjectGuid> barrelGUIDs;
            std::vector<ObjectGuid> plantGUIDs;

            void Initialize() override
            {
                SetBossNumber(4);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
                chapterOne                   = 0;
                chapterTwo                   = 0;
                chapterThree                 = 0;
                chapterFour                  = 0;
                hodgsonGUID = ObjectGuid::Empty;
                hagmanGUID = ObjectGuid::Empty;
                hordeMainTransportCannonGUID = ObjectGuid::Empty;
                factionId                    = 0;
                hordeCaptainGUID = ObjectGuid::Empty;
                hordeRopeGUID = ObjectGuid::Empty;
                hordeOutTransportCannonGUID = ObjectGuid::Empty;
                explosiveBarrelsCount        = 0;
                plantBarrelCount             = 0;
                rapierGUID = ObjectGuid::Empty;
                allianceRopeGUID = ObjectGuid::Empty;

                fireBunnyGUIDs.clear();
                allianceToHordeTransportGUIDs.clear();
                barrelGUIDs.clear();
                plantGUIDs.clear();

                events.ScheduleEvent(1, 2000);
                instance->SetWorldState(WORLDSTATE_KEEP_THOSE_BOMBS_AWAY, 1);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!factionId)
                    factionId = player->GetTeam();

                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1099, DATA_BOARDING_PARTY, 0, GetData(FACTION_DATA) ? CRITERIA_TREE_ID_BOARDING_PARTY_HORDE : CRITERIA_TREE_ID_BOARDING_PARTY_ALLIANCE, GetData(FACTION_DATA) ? SCENARIO_ID_NAVAL_BATTLE_HORDE : SCENARIO_ID_NAVAL_BATTLE_ALLIANCE);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_FIRE_BUNNY:
                        creature->SetVisible(false);
                        fireBunnyGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_ADMIRAL_HAGMAN:
                        hagmanGUID = creature->GetGUID();
                        break;
                    case NPC_ADMIRAL_HODGSON:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        hodgsonGUID = creature->GetGUID();
                        break;
                    case NPC_TRANSPORT_CANNON_H_A_1:
                        creature->SetVisible(false);
                        hordeMainTransportCannonGUID = creature->GetGUID();
                        break;
                    case NPC_TRANSPORT_CANNON_A_H_1:
                    case NPC_TRANSPORT_CANNON_A_H_2:
                    case NPC_TRANSPORT_CANNON_A_H_3:
                        allianceToHordeTransportGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_HORDE_CAPTAIN:
                        hordeCaptainGUID = creature->GetGUID();
                        break;
                    case NPC_EXPLOSIVE_BARREL_TRIGGER:
                        barrelGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_PLANT_EXPLOSIVES:
                        creature->SetVisible(false);
                        plantGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_TRANSPORT_CANNON_2_H_A_1:
                        creature->SetVisible(false);
                        hordeOutTransportCannonGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override 
            {
                switch (go->GetEntry())
                {
                    case GO_ROPE_PILE_TO_ALLIANCE_SHIP:
                        hordeRopeGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_RAPIER:
                        rapierGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_ROPE_PILE_TO_HORDE_SHIP:
                        allianceRopeGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_BOARDING_PARTY:
                        chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1099, DATA_EXPLOSIVES_ACQUICITION, 0);

                        if (Creature* hagman = instance->GetCreature(GetGuidData(NPC_ADMIRAL_HAGMAN)))
                            hagman->AI()->Talk(TALK_SPECIAL_1);

                        // Actiate Transport cannon
                        if (Creature* transportCannon = instance->GetCreature(GetGuidData(NPC_TRANSPORT_CANNON_H_A_1)))
                        {
                            transportCannon->SetVisible(true);
                            transportCannon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }

                        // Also despawn cross transport cannons for unavailable to come back
                        for (auto&& itr : allianceToHordeTransportGUIDs)
                        {
                            if (Creature* transportCannon = instance->GetCreature(itr))
                                transportCannon->SetVisible(false);
                        }
                        
                        // Summon new wave on next ship
                        if (Creature* hordeCaptain = instance->GetCreature(GetGuidData(NPC_HORDE_CAPTAIN)))
                        {
                            for (auto&& itr : invSecondAssaultSpawnPos)
                                hordeCaptain->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN);

                            hordeCaptain->AI()->DoAction(ACTION_START_INTRO);
                        }

                        events.ScheduleEvent(2, 14000);
                        break;
                    case DATA_EXPLOSIVES_ACQUICITION:
                        chapterTwo = data;

                        if (data == IN_PROGRESS)
                        {
                            // Allow to use Zipline
                            if (GameObject* rope = instance->GetGameObject(GetGuidData(GO_ROPE_PILE_TO_ALLIANCE_SHIP)))
                                rope->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                            // Set barrels selectable for spell (idk why it not selectable at start)
                            for (auto&& itr : barrelGUIDs)
                                if (Creature* barrel = instance->GetCreature(itr))
                                    barrel->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                        else
                        {
                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1099, DATA_SMITHEREENS, 0, GetData(FACTION_DATA) ? CRITERIA_TREE_ID_SMITHEREENS_HORDE : CRITERIA_TREE_ID_SMITHEREENS_ALLIANCE, GetData(FACTION_DATA) ? SCENARIO_ID_NAVAL_BATTLE_HORDE : SCENARIO_ID_NAVAL_BATTLE_ALLIANCE);

                            // Allow to use plant Explosives
                            for (auto&& itr : plantGUIDs)
                            {
                                if (Creature* plant = instance->GetCreature(itr))
                                {
                                    plant->SetVisible(true);
                                    plant->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                }
                            }

                            // Announe from captain
                            if (Creature* hordeCaptain = instance->GetCreature(GetGuidData(NPC_HORDE_CAPTAIN)))
                                hordeCaptain->AI()->Talk(TALK_SPECIAL_3);
                        }
                        break;
                    case DATA_SMITHEREENS:
                        chapterThree = data;

                        if (data == IN_PROGRESS)
                        {
                            if (Creature* hordeCaptain = instance->GetCreature(GetGuidData(NPC_HORDE_CAPTAIN)))
                            {
                                hordeCaptain->AI()->Talk(TALK_SPECIAL_4);

                                // Start time to explosive
                                hordeCaptain->AI()->DoAction(ACTION_EXPLOSIVE_SHIP);

                                // Allow to use rope on this ship
                                if (GameObject* rope = instance->GetGameObject(GetGuidData(GO_ROPE_PILE_TO_HORDE_SHIP)))
                                    rope->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                            }
                        }
                        else
                        {
                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1099, DATA_DEFEAT_THE_ADMIRAL, 0, GetData(FACTION_DATA) ? CRITERIA_TREE_ID_DEFEAT_ADMIRAL_HORDE : CRITERIA_TREE_ID_DEFEAT_ADMIRAL_ALLIANCE, GetData(FACTION_DATA) ? SCENARIO_ID_NAVAL_BATTLE_HORDE : SCENARIO_ID_NAVAL_BATTLE_ALLIANCE);

                            // Allow to use last transport cannon
                            if (Creature* transportCannon = instance->GetCreature(GetGuidData(NPC_TRANSPORT_CANNON_2_H_A_1)))
                            {
                                transportCannon->SetVisible(true);
                                transportCannon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            }

                            // Set in fire enemy ship
                            for (auto&& itr : fireBunnyGUIDs)
                                if (Creature* fireBunny = instance->GetCreature(itr))
                                    fireBunny->SetVisible(true);

                            // Activate Enemy Admiral
                            if (Creature* hodgson = instance->GetCreature(GetGuidData(NPC_ADMIRAL_HODGSON)))
                            {
                                hodgson->SetVisible(true);
                                hodgson->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                            }
                        }
                        break;
                    case DATA_DEFEAT_THE_ADMIRAL:
                    {
                        chapterFour = data;
                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_DEFEAT_ADMIRAL_HODGSON, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        uint32 scenarioEntryByDifficulty = GetData(FACTION_DATA) ? 654 : 655;

                        if (instance->GetDifficulty() == SCENARIO_DIFFICULTY_HEROIC)
                            scenarioEntryByDifficulty = GetData(FACTION_DATA) ? 652 : 588;

                        DoFinishLFGDungeon(scenarioEntryByDifficulty);
                        break;
                    }
                    case HORDE_ASSAULT_DATA:
                        for (auto&& itr : allianceToHordeTransportGUIDs)
                        {
                            if (Creature* hagman = instance->GetCreature(GetGuidData(NPC_ADMIRAL_HAGMAN)))
                                if (Creature* transport = instance->GetCreature(itr))
                                    if (Creature* assaulter = hagman->SummonCreature(urand(0, 1) ? NPC_ALLIANCE_CANNONEER : NPC_ALLIANCE_SWASHBUCKLER, *transport, TEMPSUMMON_MANUAL_DESPAWN))
                                        assaulter->AI()->DoAction(ACTION_START_INTRO);
                        }
                        break;
                    case EXPLOSIVES_COUNT_DATA:
                        if (++explosiveBarrelsCount < 4)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_EXPLOSIVES_ACQUIRED, explosiveBarrelsCount, GetScenarioGUID(), time(NULL), 0, 0));

                        if (explosiveBarrelsCount > 2)
                            SetData(DATA_EXPLOSIVES_ACQUICITION, DONE);
                        break;
                    case PLANT_COUNT_DATA:
                        if (++plantBarrelCount < 4)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_EXPLOSIVES_PRIMED, plantBarrelCount, GetScenarioGUID(), time(NULL), 0, 0));

                        if (plantBarrelCount > 2)
                            SetData(DATA_SMITHEREENS, IN_PROGRESS);
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_BOARDING_PARTY:
                        return chapterOne;
                    case DATA_EXPLOSIVES_ACQUICITION:
                        return chapterTwo;
                    case DATA_SMITHEREENS:
                        return chapterThree;
                    case DATA_DEFEAT_THE_ADMIRAL:
                        return chapterFour;
                    case FACTION_DATA:
                        return factionId == HORDE ? 1 : 0;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_ADMIRAL_HAGMAN:
                        return hagmanGUID;
                    case NPC_ADMIRAL_HODGSON:
                        return hodgsonGUID;
                    case NPC_TRANSPORT_CANNON_H_A_1:
                        return hordeMainTransportCannonGUID;
                    case NPC_HORDE_CAPTAIN:
                        return hordeCaptainGUID;
                    case GO_ROPE_PILE_TO_ALLIANCE_SHIP:
                        return hordeRopeGUID;
                    case NPC_TRANSPORT_CANNON_2_H_A_1:
                        return hordeOutTransportCannonGUID;
                    case GO_RAPIER:
                        return rapierGUID;
                    case GO_ROPE_PILE_TO_HORDE_SHIP:
                        return allianceRopeGUID;
                }

                return ObjectGuid::Empty;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == 1)
                    {
                        if (GetData(FACTION_DATA)) // Init horde scenario
                        {
                            if (Creature* hagman = instance->GetCreature(GetGuidData(NPC_ADMIRAL_HAGMAN)))
                            {
                                for (auto&& itr : firstAllianceAssaultSpawnPos)
                                    hagman->SummonCreature(NPC_ALLIANCE_SWASHBUCKLER, itr, TEMPSUMMON_MANUAL_DESPAWN);
                            }
                        }
                    }
                    break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "B O H S " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour;

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

                char dataHead1, dataHead2, dataHead3, dataHead4;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> dataHead3 >> dataHead4;

                if (dataHead1 == 'B' && dataHead2 == 'O'&& dataHead3 == 'H' && dataHead4 == 'S')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_BOARDING_PARTY, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_EXPLOSIVES_ACQUICITION, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_SMITHEREENS, chapterThree);
                    loadStream >> temp; // chapterFour complete
                    chapterFour = temp;
                    SetData(DATA_DEFEAT_THE_ADMIRAL, chapterFour);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_battle_on_the_high_seas_InstanceMapScript(map);
        }
};

void AddSC_instance_battle_on_the_high_seas()
{
    new instance_battle_on_the_high_seas();
}