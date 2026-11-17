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
#include "proving_grounds.h"
#include "ScenarioMgr.h"

class instance_proving_grounds : public InstanceMapScript
{
    public:
        instance_proving_grounds() : InstanceMapScript("instance_proving_grounds", 1148) { }

        struct instance_proving_grounds_InstanceMapScript : public InstanceScript
        {
            instance_proving_grounds_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;

            uint32 chapterOne, chapterTwo, chapterThree;
            uint32 m_auiEncounter[2];
            ObjectGuid rotunGUID;
            ObjectGuid controllerGUID;
            ObjectGuid playerGUID;
            ObjectGuid sikariGUID;
            ObjectGuid otoGUID;
            ObjectGuid kiGUID;
            ObjectGuid soliGUID;
            ObjectGuid kavanGUID;
            std::vector<ObjectGuid> barricadeGUIDs;
            std::vector<ObjectGuid> berserkerGUIDs;

            void Initialize() override
            {
                SetBossNumber(2);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne     = 0;
                chapterTwo     = 0;
                chapterThree   = 0;
                rotunGUID = ObjectGuid::Empty;
                controllerGUID = ObjectGuid::Empty;
                playerGUID = ObjectGuid::Empty;
                sikariGUID = ObjectGuid::Empty;
                otoGUID = ObjectGuid::Empty;
                kiGUID = ObjectGuid::Empty;
                soliGUID = ObjectGuid::Empty;
                kavanGUID = ObjectGuid::Empty;

                barricadeGUIDs.clear();
                berserkerGUIDs.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!playerGUID)
                    playerGUID = player->GetGUID();

                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1148, DATA_SELECT_A_TRIAL, 0);
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->ToPlayer()) // fail trial (only for tank trial btw, healr & dps couldn`t die)
                {
                    if (Creature* controller = instance->GetCreature(GetGuidData(NPC_PROVING_GROUNDS_2)))
                        controller->AI()->DoAction(ACTION_FAIL_TRIAL);
                }
            }

            void OnCreatureCreate(Creature* creature) override 
            {
                switch (creature->GetEntry())
                {
                    case NPC_TRIAL_MASTER_ROTUN:
                        rotunGUID = creature->GetGUID();
                        break;
                    case NPC_PROVING_GROUNDS_2:
                        controllerGUID = creature->GetGUID();
                        break;
                    case NPC_BERSERKING:
                        berserkerGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_SIKARI_THE_MISTWEAVER:
                        sikariGUID = creature->GetGUID();
                        break;
                    case NPC_OTO_THE_PROTECTOR:
                        otoGUID = creature->GetGUID();
                        break;
                    case NPC_KI_THE_ASSASSIN:
                        kiGUID = creature->GetGUID();
                        break;
                    case NPC_SOOLI_THE_SURVIVALIST:
                        soliGUID = creature->GetGUID();
                        break;
                    case NPC_KAVAN_THE_ARCANIST:
                        kavanGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override 
            {
                if (go->GetEntry() == GO_PROVING_GROUNDS_BARRICADE) // using only for tank trial
                {
                    barricadeGUIDs.push_back(go->GetGUID());
                    go->SetPhaseMask(2, true)/*SetDisplayId(0)*/;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_SELECT_A_TRIAL:
                        chapterOne = data;
                        break;
                    case DATA_PREPARE_TO_TRIAL:
                        chapterTwo = data;
                        break;
                    case DATA_TRIAL:
                        chapterThree = data;

                        // Remove Berserker Buff from arena
                        for (auto&& itr : berserkerGUIDs)
                            if (Creature* berserkerTrigger = instance->GetCreature(itr))
                                berserkerTrigger->DespawnOrUnsummon();

                        if (chapterThree == DONE)
                        {
                            // Reward player for current trial
                            if (Creature* controller = instance->GetCreature(GetGuidData(NPC_PROVING_GROUNDS_2)))
                            {
                                if (uint32 trialEntry = controller->AI()->GetData(TRIAL_DATA)) // get last completed trial and reward by achiev (handle, cuz it haven`t any criteria or not implemented in core)
                                {
                                    switch (trialEntry)
                                    {
                                        case TRIAL_HEAL_BRONZE:
                                        case TRIAL_HEAL_SILVER:
                                        case TRIAL_HEAL_GOLD:
                                            if (Creature* sooli = instance->GetCreature(GetGuidData(NPC_SOOLI_THE_SURVIVALIST)))
                                                sooli->AI()->Talk(TALK_HEALER_BRONZE_OUTRO);
                                            break;
                                        default:
                                            // Compelete current trial
                                            if (Creature* rotun = instance->GetCreature(GetGuidData(NPC_TRIAL_MASTER_ROTUN)))
                                                rotun->AI()->Talk(TALK_TRIAL_DONE);
                                            break;
                                    }

                                    if (Player* trialOwner = ObjectAccessor::FindPlayer(GetGuidData(PLAYER_DATA)))
                                    {
                                        if (trialEntry < TRIAL_DD_ENDLESS)
                                        {
                                            AchievementEntry const* AchievProveYourself = sAchievementStore.LookupEntry(invRewardType.find(trialEntry)->second);
                                            if (AchievProveYourself)
                                                trialOwner->CompletedAchievement(AchievProveYourself);
                                        }
                                        else // reward endless trial by achiev per each 10td waves done
                                        {
                                            std::vector<uint32> endlessTrialRewards;

                                            switch (trialEntry)
                                            {
                                                case TRIAL_DD_ENDLESS:
                                                    if (controller->AI()->GetData(WAVE_DATA) > 9)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_DD_ENDLESS_10);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 19)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_DD_ENDLESS_20);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 29)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_DD_ENDLESS_30);
                                                    break;
                                                case TRIAL_HEAL_ENDLESS:
                                                    if (controller->AI()->GetData(WAVE_DATA) > 9)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_HEALER_ENDLESS_10);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 19)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_HEALER_ENDLESS_20);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 29)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_HEALER_ENDLESS_30);
                                                    break;
                                                case TRIAL_TANK_ENDLESS:
                                                    if (controller->AI()->GetData(WAVE_DATA) > 9)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_TANK_ENDLESS_10);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 19)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_TANK_ENDLESS_20);

                                                    if (controller->AI()->GetData(WAVE_DATA) > 29)
                                                        endlessTrialRewards.push_back(ACHIEV_PROVING_YOURSELF_TANK_ENDLESS_30);
                                                    break;
                                            }

                                            for (auto&& itr : endlessTrialRewards)
                                            {
                                                AchievementEntry const* AchievProveYourself = sAchievementStore.LookupEntry(itr);
                                                if (AchievProveYourself)
                                                    trialOwner->CompletedAchievement(AchievProveYourself);
                                            }
                                        }

                                        // Check Aditional Condition for non-spec achiev
                                        if (trialEntry == TRIAL_DD_SILVER && trialOwner->GetRoleForGroup(trialOwner->GetTalentSpecialization()) != ROLES_DPS || trialEntry == TRIAL_TANK_SILVER && trialOwner->GetRoleForGroup(trialOwner->GetTalentSpecialization()) != ROLES_TANK || trialEntry == TRIAL_HEAL_SILVER && trialOwner->GetRoleForGroup(trialOwner->GetTalentSpecialization()) != ROLES_HEALER)
                                        {
                                            AchievementEntry const* AchievDoingWrong = sAchievementStore.LookupEntry(ACHIEV_YOU_ARE_DOING_THIS_WRONG);
                                            if (AchievDoingWrong)
                                                trialOwner->CompletedAchievement(AchievDoingWrong);
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case BARRICADE_DATA:
                        for (auto&& itr : barricadeGUIDs)
                            if (GameObject* barricade = instance->GetGameObject(itr))
                                barricade->SetPhaseMask(data == DONE ? 2 : 1, true)/*SetDisplayId(data == DONE ? 0 : 11686)*/;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SELECT_A_TRIAL:
                        return chapterOne;
                    case DATA_PREPARE_TO_TRIAL:
                        return chapterTwo;
                    case DATA_TRIAL:
                        return chapterThree;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_TRIAL_MASTER_ROTUN:
                        return rotunGUID;
                    case NPC_PROVING_GROUNDS_2:
                        return controllerGUID;
                    case PLAYER_DATA:
                        return playerGUID;
                    case NPC_SIKARI_THE_MISTWEAVER:
                        return sikariGUID;
                    case NPC_OTO_THE_PROTECTOR:
                        return otoGUID;
                    case NPC_KI_THE_ASSASSIN:
                        return kiGUID;
                    case NPC_SOOLI_THE_SURVIVALIST:
                        return soliGUID;
                    case NPC_KAVAN_THE_ARCANIST:
                        return kavanGUID;
                }

                return ObjectGuid::Empty;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "P G " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree;

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

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'P' && dataHead2 == 'G')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_SELECT_A_TRIAL, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_PREPARE_TO_TRIAL, chapterTwo);
                    loadStream >> temp; // Trial complete
                    chapterThree = temp;
                    SetData(DATA_TRIAL, chapterThree);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_proving_grounds_InstanceMapScript(map);
        }
};

void AddSC_instance_proving_grounds()
{
    new instance_proving_grounds();
}