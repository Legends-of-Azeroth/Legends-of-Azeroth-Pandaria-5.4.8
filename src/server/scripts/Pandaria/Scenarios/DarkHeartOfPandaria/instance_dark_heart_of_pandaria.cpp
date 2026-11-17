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
#include "dark_heart_of_pandaria.h"
#include "ScenarioMgr.h"

class instance_dark_heart_of_pandaria : public InstanceMapScript
{
    public:
        instance_dark_heart_of_pandaria() : InstanceMapScript("instance_dark_heart_of_pandaria", 1144) { }
    
        struct instance_dark_heart_of_pandaria_InstanceMapScript : public InstanceScript
        {
            instance_dark_heart_of_pandaria_InstanceMapScript(Map* map) : InstanceScript(map) { }
    
            EventMap events;
    
            uint32 chapterOne, chapterTwo, chapterThree, chapterFour, chapterFive;
            uint32 m_auiEncounter[CHAPTERS];
            uint32 artifactCounter;
            uint32 greatArtifactCount;
            ObjectGuid urthargesGUID;
            ObjectGuid malkorokGUID;
            ObjectGuid grizzleGUID;
            ObjectGuid norushenGUID;
            ObjectGuid craftyGUID;
            ObjectGuid echoGUID;
            ObjectGuid strangeWallGUID;
            ObjectGuid heartOfYshaarjGUID;
            ObjectGuid heartControllerGUID;
            ObjectGuid additGrizzleGUID;

            std::vector<ObjectGuid> artifactGUIDs;
            std::vector<ObjectGuid> blossomCollisionsGUIDs;
            std::vector<ObjectGuid> yshaarjBloodGUIDs;
    
            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    
                chapterOne          = 0;
                chapterTwo          = 0;
                chapterThree        = 0;
                chapterFour         = 0;
                chapterFive         = 0;
                                    
                artifactCounter     = 0;
                urthargesGUID = ObjectGuid::Empty;
                malkorokGUID = ObjectGuid::Empty;
                grizzleGUID = ObjectGuid::Empty;
                norushenGUID = ObjectGuid::Empty;
                craftyGUID = ObjectGuid::Empty;
                echoGUID = ObjectGuid::Empty;
                strangeWallGUID = ObjectGuid::Empty;
                heartOfYshaarjGUID = ObjectGuid::Empty;
                heartControllerGUID = ObjectGuid::Empty;
                additGrizzleGUID = ObjectGuid::Empty;
                greatArtifactCount  = 0;

                artifactGUIDs.clear();
                blossomCollisionsGUIDs.clear();
                yshaarjBloodGUIDs.clear();
            }
    
            void OnPlayerEnter(Player* player) override
            {
                // Alliance players to goblin
                if (player && player->GetTeam() == ALLIANCE)
                    player->CastSpell(player, player->GetGender() == GENDER_MALE ? SPELL_GOBLIN_ILLUSION_MALE : SPELL_GOBLIN_ILLISION_FEMALE, true);

                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1144, STEP_TALK_WITH_LEADER, 0);
            }
    
            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_URTHARGES_THE_DESTROYER:
                        urthargesGUID = creature->GetGUID();
                        break;
                    case NPC_MALKOROK:
                        malkorokGUID = creature->GetGUID();
                        break;
                    case NPC_GRIZZLE_GEARSLIP:
                        if (creature->GetDBTableGUIDLow())
                            grizzleGUID = creature->GetGUID();
                        else
                            additGrizzleGUID = creature->GetGUID();
                        break;
                    case NPC_NORUSHEN:
                        norushenGUID = creature->GetGUID();
                        break;
                    case NPC_CRAFTY_THE_AMBITIOUS:
                        craftyGUID = creature->GetGUID();
                        break;
                    case NPC_BOOK_ARTIFACT:
                    case NPC_VASE_ARTIFACT:
                    case NPC_SCROLL_ARTIFACT:
                    case NPC_CRATE_OF_ARTIFACTS:
                        creature->SetVisible(false);
                        artifactGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_ECHO_OF_YSHAARJ:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        echoGUID = creature->GetGUID();
                        break;
                    case NPC_QUIVERING_BLOOD:
                    case NPC_BLOOD_OF_YSHAARJ:
                    case NPC_MOUTH_OF_TERROR:
                        if (creature->GetAreaId() == 6749)
                        {
                            creature->SetVisible(false);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                            yshaarjBloodGUIDs.push_back(creature->GetGUID());
                        }
                        break;
                    case NPC_HEART_CONTROLLER:
                        heartControllerGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_BLOSSOM_INVISIBLE_WALL:
                    case GO_BOULDERS:
                        blossomCollisionsGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_HEART_OF_YSHAARJ:
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        heartOfYshaarjGUID = go->GetGUID();
                        break;
                    case GO_STRANGE_WALL:
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        strangeWallGUID = go->GetGUID();
                        break;
                }
            }
    
            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case STEP_TALK_WITH_LEADER:
                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1144, STEP_URTHARGES_DEFEATED, 0);

                        DoAddAuraOnPlayers(SPELL_URTHARGES_SHIELD);
                        chapterOne = data;
                        break;
                    case STEP_URTHARGES_DEFEATED:
                        DoRemoveAurasDueToSpellOnPlayers(SPELL_URTHARGES_SHIELD);
                        chapterTwo = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1144, STEP_STUFF_POCKETS, 0);

                        // Respawn Artifacts
                        for (auto&& itr : artifactGUIDs)
                        {
                            if (Creature* artifact = instance->GetCreature(itr))
                            {
                                artifact->SetVisible(true);
                                artifact->CastSpell(artifact, invArtifactsType.find(artifact->GetEntry())->second[0], true);
                                artifact->CastSpell(artifact, invArtifactsType.find(artifact->GetEntry())->second[1], true);
                            }
                        }
                        break;
                    case STEP_STUFF_POCKETS:
                        for (auto&& itr : blossomCollisionsGUIDs)
                            if (GameObject* wall = instance->GetGameObject(itr))
                                wall->Delete();

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1144, STEP_END_OF_MINE, 0);

                        chapterThree = data;
                        break;
                    case STEP_END_OF_MINE:
                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1144, STEP_ECHO_DEFEATED, 0);

                        chapterFour = data;
                        break;
                    case STEP_ECHO_DEFEATED:
                        chapterFive = data;

                        if (chapterFive == SPECIAL)
                        {
                            for (auto&& itr : yshaarjBloodGUIDs)
                            {
                                if (Creature* yshaarjBlood = instance->GetCreature(itr))
                                {
                                    yshaarjBlood->SetVisible(true);
                                    yshaarjBlood->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                                    yshaarjBlood->RemoveChanneledCast();
                                }
                            }
                        }
                        else
                        {
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_DEFEAT_ECHO_OF_YSHAARJ, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_HEROIC ? 624 : 647);
                            DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_NORMAL ? 624 : 647);

                            // Interaction Scene
                            if (Creature* norushen = instance->GetCreature(GetGuidData(NPC_NORUSHEN)))
                                norushen->SummonCreature(NPC_GRIZZLE_GEARSLIP, gearslipPath[0], TEMPSUMMON_MANUAL_DESPAWN);
                        }
                        break;
                    case DATA_ARTIFACT:
                        artifactCounter += data;

                        if (data > 4 && ++greatArtifactCount > 2) // great artifact
                            if (Creature* grizzle = instance->GetCreature(GetGuidData(NPC_GRIZZLE_GEARSLIP)))
                                grizzle->CastSpell(grizzle, SPELL_ACCELERATED_ARCHEOLOGY, true);

                        if (artifactCounter < 51)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_ARTIFACTS_STOLEN, artifactCounter, GetScenarioGUID(), time(NULL), 0, 0));

                        if (artifactCounter >= 50 && GetData(STEP_STUFF_POCKETS) != DONE)
                            SetData(STEP_STUFF_POCKETS, DONE);
                        break;
                }
    
                if (data == DONE)
                    SaveToDB();
            }
    
            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case STEP_TALK_WITH_LEADER:
                        return chapterOne;
                    case STEP_URTHARGES_DEFEATED:
                        return chapterTwo;
                    case STEP_STUFF_POCKETS:
                        return chapterThree;
                    case STEP_END_OF_MINE:
                        return chapterFour;
                    case STEP_ECHO_DEFEATED:
                        return chapterFive;
                    case DATA_ARTIFACT:
                        return artifactCounter;
                }
    
                return 0;
            }
    
            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_URTHARGES_THE_DESTROYER:
                        return urthargesGUID;
                    case NPC_MALKOROK:
                        return malkorokGUID;
                    case NPC_GRIZZLE_GEARSLIP:
                        return grizzleGUID;
                    case NPC_NORUSHEN:
                        return norushenGUID;
                    case NPC_CRAFTY_THE_AMBITIOUS:
                        return craftyGUID;
                    case NPC_ECHO_OF_YSHAARJ:
                        return echoGUID;
                    case GO_HEART_OF_YSHAARJ:
                        return heartOfYshaarjGUID;
                    case GO_STRANGE_WALL:
                        return strangeWallGUID;
                    case NPC_HEART_CONTROLLER:
                        return heartControllerGUID;
                    case NPC_GRIZZLE_GEARSLIP + 1:
                        return additGrizzleGUID;
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
            }
    
            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;
    
                std::ostringstream saveStream;
                saveStream << "D H P " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour << ' ' << chapterFive;
    
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
    
                if (dataHead1 == 'D' && dataHead2 == 'H' && dataHead3 == 'P')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(STEP_TALK_WITH_LEADER, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(STEP_URTHARGES_DEFEATED, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(STEP_STUFF_POCKETS, chapterThree);
                    loadStream >> temp;
                    chapterFour = temp; // chapterFour complete
                    SetData(STEP_END_OF_MINE, chapterFour);
                    loadStream >> temp;
                    chapterFour = temp; // chapterFive complete
                    SetData(STEP_ECHO_DEFEATED, chapterFive);
                }
                else OUT_LOAD_INST_DATA_FAIL;
    
                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };
    
        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_dark_heart_of_pandaria_InstanceMapScript(map);
        }
};

void AddSC_instance_dark_heart_of_pandaria()
{
    new instance_dark_heart_of_pandaria();
}