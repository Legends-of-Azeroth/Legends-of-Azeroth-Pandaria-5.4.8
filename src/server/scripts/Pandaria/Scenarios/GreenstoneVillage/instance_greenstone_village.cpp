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
#include "greenstone_village.h"
#include "ScenarioMgr.h"

class instance_greenstone_village : public InstanceMapScript
{
    public:
        instance_greenstone_village() : InstanceMapScript("instance_greenstone_village", 1024) { }

        struct instance_greenstone_village_InstanceMapScript : public InstanceScript
        {
            instance_greenstone_village_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;

            uint32 chapterOne, chapterTwo, chapterThree, chapterFour, chapterFive;
            uint32 tempCount, m_tempType;
            uint32 m_auiEncounter[CHAPTERS];
            ObjectGuid HuiGUID, TzuGUID;
            std::map<uint32, ObjectGuid> VillageOwners;
            std::list<ObjectGuid> m_uiBarrels;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne   = 0;
                chapterTwo   = 0;
                chapterThree = 0;
                chapterFour  = 0;
                chapterFive  = 0;

                m_uiBarrels.clear();
                instance->SetWorldState(WORLDSTATE_SAVE_IT_FOR_LATER, 1);
                instance->SetWorldState(WORLDSTATE_PERFECT_DELIVERY, 1);
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1024, DATA_RESCUE_VILLAGE, 0);

                if (GetData(DATA_GREENSTONE_MASONS) < IN_PROGRESS)
                    return;

                if (!player->HasAura(SPELL_VOLATILE_GREENSTONE_BREW))
                    player->CastSpell(player, SPELL_VOLATILE_GREENSTONE_BREW, false);
                else // if we had server crash, then need remove old extra button
                {
                    player->RemoveAurasDueToSpell(SPELL_VOLATILE_GREENSTONE_BREW);
                    player->CastSpell(player, SPELL_VOLATILE_GREENSTONE_BREW, false);
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_VENGEFUL_HUI:
                        HuiGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_BREWMASTER_TZU:
                    case NPC_BEAST_OF_JADE:
                    case NPC_JADE_DESTROYER:
                    case NPC_BARREL_CHEST_HUO:
                    case NPC_STONECUTTER_LON:
                    case NPC_KIRI_JADE_EYES:
                        creature->SetVisible(false);
                        VillageOwners.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_COWARDLY_ZUE:
                    case NPC_MAYOR_LIN:
                    case NPC_LA_THE_GENTLE:
                    case NPC_WOODCARVER_LIUPO:
                    case NPC_PORTLY_SHUNG:
                    case NPC_SCRIBE_RINJI:
                    case NPC_GRACEFUL_SWAN:
                    case NPC_MEILA:
                        VillageOwners.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_BURGLED_BARREL:
                        m_uiBarrels.push_back(creature->GetGUID());
                        creature->SetVisible(false);
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_RESCUE_VILLAGE:
                        chapterOne = data;
                        SaveToDB();

                        if (chapterOne >= 7)
                        {
                            if (Creature* BrewmasterTzu = instance->GetCreature(VillageOwners.find(NPC_BREWMASTER_TZU)->second))
                                BrewmasterTzu->AI()->DoAction(ACTION_INTRO);

                            SetBossState(type, EncounterState(DONE));

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1024, DATA_RESCUE_DOJO, 0);
                        }
                        break;
                    case DATA_RESCUE_DOJO:
                        chapterTwo = data;

                        if (chapterTwo == DONE)
                        {
                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1024, DATA_RECOVER_BURGLED_BARRELS, 0);

                            // set active on barrels
                            for (auto&& itr : m_uiBarrels)
                                if (Creature* m_barrel = instance->GetCreature(itr))
                                    m_barrel->SetVisible(true);

                            if (Creature* Tzu = instance->GetCreature(GetGuidData(NPC_BREWMASTER_TZU)))
                                Tzu->GetMotionMaster()->MovePoint(0, TzuPath[0]);

                            SetBossState(type, EncounterState(DONE));
                        }
                        break;
                    case DATA_RECOVER_BURGLED_BARRELS:
                        chapterThree = data;
                        SaveToDB();

                        if (chapterThree < 7)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_STOLEN_BREW, chapterThree, GetScenarioGUID(), time(NULL), 0, 0));

                        if (chapterThree == TO_BE_DECIDED + 1)
                        {
                            if (Creature* Tzu = instance->GetCreature(GetGuidData(NPC_BREWMASTER_TZU)))
                            {
                                Tzu->AI()->Talk(TALK_SPECIAL_3);
                                Tzu->AI()->Talk(TALK_SPECIAL_4); // ann
                                DoCastSpellOnPlayers(SPELL_VOLATILE_GREENSTONE_BREW);
                            }

                            // set active on npc in masson
                            for (auto&& itr : VillageOwners)
                                if (Creature* m_owner = instance->GetCreature(itr.second))
                                    m_owner->SetVisible(true);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1024, DATA_GREENSTONE_MASONS, 0);

                            SetBossState(type, EncounterState(DONE));
                        }
                        break;
                    case DATA_GREENSTONE_MASONS:
                        chapterFour = data;
                        SaveToDB();

                        if (chapterFour == DONE)
                        {
                            if (Creature* Hui = instance->GetCreature(GetGuidData(NPC_VENGEFUL_HUI)))
                                Hui->AI()->DoAction(ACTION_INTRO);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1024, DATA_DEFEAT_VENGEFUL_HUI, 0);

                            SetBossState(type, EncounterState(DONE));
                        }
                        break;
                    case DATA_DEFEAT_VENGEFUL_HUI:
                        chapterFive = data;
                        if (chapterFive == DONE)
                        {
                            DoFinishLFGDungeon(492);
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_HUI_DEFEATED, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        }
                        SetBossState(type, EncounterState(DONE));
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_RESCUE_VILLAGE:
                        return chapterOne;
                    case DATA_RESCUE_DOJO:
                        return chapterTwo;
                    case DATA_RECOVER_BURGLED_BARRELS:
                        return chapterThree;
                    case DATA_GREENSTONE_MASONS:
                        return chapterFour;
                    case DATA_DEFEAT_VENGEFUL_HUI:
                        return chapterFive;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_VENGEFUL_HUI:
                        return HuiGUID;
                    case NPC_COWARDLY_ZUE:
                    case NPC_MAYOR_LIN:
                    case NPC_LA_THE_GENTLE:
                    case NPC_WOODCARVER_LIUPO:
                    case NPC_PORTLY_SHUNG:
                    case NPC_SCRIBE_RINJI:
                    case NPC_GRACEFUL_SWAN:
                    case NPC_MEILA:
                    case NPC_BREWMASTER_TZU:
                    case NPC_BEAST_OF_JADE:
                    case NPC_JADE_DESTROYER:
                    case NPC_BARREL_CHEST_HUO:
                    case NPC_STONECUTTER_LON:
                    case NPC_KIRI_JADE_EYES:
                        return VillageOwners.find(type)->second;
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
                saveStream << "G V " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour << ' ' << chapterFive;

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

                if (dataHead1 == 'G' && dataHead2 == 'V')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_RESCUE_VILLAGE, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_RESCUE_DOJO, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_RECOVER_BURGLED_BARRELS, chapterThree);
                    loadStream >> temp;
                    chapterFour = temp; // chapterFour complete
                    SetData(DATA_GREENSTONE_MASONS, chapterFour);
                    loadStream >> temp;
                    chapterFour = temp; // chapterFive complete
                    SetData(DATA_DEFEAT_VENGEFUL_HUI, chapterFour);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_greenstone_village_InstanceMapScript(map);
        }
};

void AddSC_instance_greenstone_village()
{
    new instance_greenstone_village();
}