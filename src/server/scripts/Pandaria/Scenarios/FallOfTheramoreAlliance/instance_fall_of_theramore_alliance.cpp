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
#include "MapManager.h"
#include "fall_of_theramore_alliance.h"
#include "ScenarioMgr.h"

class instance_fall_of_theramore_alliance : public InstanceMapScript
{
    public:
        instance_fall_of_theramore_alliance() : InstanceMapScript("instance_fall_of_theramore_alliance", 1000) { }

        struct instance_fall_of_theramore_alliance_InstanceMapScript : public InstanceScript
        {
            instance_fall_of_theramore_alliance_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[CHAPTERS];
            std::list<ObjectGuid> beachPhaseGUIDs, ruinsPhaseGUIDs;
            ObjectGuid jainaGUID;
            uint32 wCaptainsSlain, wBarrelsExploded, wCaptainsSlain1, wTrashSlain1, wWyvernSlain, wGashnulSlain, wStolenStandards, wGateCrusherSlain, chapterOne, chapterTwo, chapterThree, chapterFour, chapterFive, chapterSix;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                beachPhaseGUIDs.clear();
                ruinsPhaseGUIDs.clear();

                wCaptainsSlain    = 0;
                wBarrelsExploded  = 0;
                wCaptainsSlain1   = 0;
                wTrashSlain1      = 0;
                wWyvernSlain      = 0;
                wGashnulSlain     = 0;
                wStolenStandards  = 0;
                wGateCrusherSlain = 0;
                jainaGUID = ObjectGuid::Empty;

                chapterOne        = 0;
                chapterTwo        = 0;
                chapterThree      = 0;
                chapterFour       = 0;
                chapterFive       = 0;
                chapterSix        = 0;
            }

            //Now it`s hacky cuz we haven`t tools for scenario but it should be like that:
            // At scenario entrance players should be teleported to REAL theramore isle and play cinematic
            // After cinematic end players should be teleported to SCENARIO.
            // Why: If we`ll play cinematic in SCENARIO then players`ll look onto bomb throw to destroyed theramore (lol, it`s realy funny cuz bomb location not in crater xD)
            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1000, DATA_WELCOME_TO_THERAMORE, 0);

                player->CastSpell(player, SPELL_THERAMORE_EXPLOSION_CINEMATIC, true);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_LEAKED_OIL_DRUM:
                    case NPC_ROKNAH_GRUNT:
                    case NPC_ROKNAH_SKRIMISHER:
                    case NPC_ROKNAH_FELCASTER:
                    case NPC_ROKNAH_HEADHUNTER:
                    case NPC_ROKNAH_LOA_SINGER:
                    case NPC_ROKNAH_HAG:
                    case NPC_ROKNAH_RIDER:
                    case NPC_AIRSHIP_MARINE:
                    case NPC_AIRSHIP_CREWMAN:
                    case NPC_VICIOUS_WYVERN:
                    case NPC_SKY_CAPTAIN_DAZRIP:
                    case NPC_CAPTAIN_MOUSSON:
                    case NPC_CAPTAIN_KORTHOK:
                    case NPC_GASHNUL:
                    case NPC_ROKNAH_WAVE_CALLER:
                        if ((creature->GetAreaId() == 6500 || creature->GetPositionX() < -3861.92f) && !HasInWaveGuard(creature->GetDBTableGUIDLow()))
                        {
                            beachPhaseGUIDs.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                        }
                        else if (creature->GetAreaId() == 6502 && creature->GetPositionX() > -3861.92f)
                        {
                            ruinsPhaseGUIDs.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                        }
                        else
                            creature->SetFaction(14);

                        break;
                    case NPC_JAINA_PROUDMOORE:
                        ruinsPhaseGUIDs.push_back(creature->GetGUID());
                        creature->SetVisible(false);
                        jainaGUID = creature->GetGUID();
                        break;
                }
            }

            bool HasInWaveGuard(uint32 creatureDBGuid)
            {
                for (auto&& guid : wGuids)
                    if (creatureDBGuid == guid)
                        return true;

                return false;
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_SERGEANT_GRUD:
                        case NPC_WAVE_CALLER_ZULGA:
                            SetData(DATA_WELCOME_TO_THERAMORE, SPECIAL);
                            break;
                        case NPC_CAPTAIN_KORTHOK:
                        case NPC_CAPTAIN_MOUSSON:
                        case NPC_SKY_CAPTAIN_DAZRIP:
                            SetData(DATA_SWEEP_THE_HARBOR, SPECIAL + 1);
                            break;
                        case NPC_ROKNAH_GRUNT:
                        case NPC_ROKNAH_SKRIMISHER:
                        case NPC_ROKNAH_FELCASTER:
                        case NPC_ROKNAH_HEADHUNTER:
                        case NPC_ROKNAH_LOA_SINGER:
                        case NPC_ROKNAH_HAG:
                        case NPC_ROKNAH_RIDER:
                        case NPC_AIRSHIP_MARINE:
                        case NPC_AIRSHIP_CREWMAN:
                            if (GetData(DATA_SWEEP_THE_HARBOR) == DONE && GetData(DATA_INTO_THE_RUINS) == DONE)
                                break;

                            SetData(GetData(DATA_SWEEP_THE_HARBOR) == DONE ? DATA_INTO_THE_RUINS : DATA_SWEEP_THE_HARBOR, SPECIAL + 2);
                            break;
                        case NPC_VICIOUS_WYVERN:
                            wWyvernSlain++;

                            if (wWyvernSlain < 3)
                                SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_WYVERN_SLAIN, wWyvernSlain, GetScenarioGUID(), time(NULL), 0, 0));

                            SetData(DATA_INTO_THE_RUINS, SPECIAL);
                            break;
                        case NPC_GASHNUL:
                            wGashnulSlain++;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_GASHNUL_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            SetData(DATA_INTO_THE_RUINS, SPECIAL);
                            break;
                        case NPC_GATECRUSHER:
                            wGateCrusherSlain++;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_GATECRUSHER_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            SetData(DATA_DESTROY_THE_DESTROYER, SPECIAL);
                            break;
                    }
                }
            }

            // Thx blizzard :/
            void HandleActivateSpawnRegion(std::vector<uint32> guardsEntry, std::list<ObjectGuid> guardGUIDs)
            {
                for (auto&& guardItr : guardGUIDs)
                {
                    if (Creature* guard = instance->GetCreature(guardItr))
                    {
                        for (auto&& entry : guardsEntry)
                        {
                            if (guard->GetEntry() == entry)
                            {
                                guard->AI()->DoAction(ACTION_ACTIVATE_BY_REGION);
                                break;
                            }
                        }
                    }

                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_WELCOME_TO_THERAMORE:
                        wCaptainsSlain++;

                        if (wCaptainsSlain >= 2 && GetData(DATA_WELCOME_TO_THERAMORE) != DONE)
                        {
                            SetBossState(type, DONE);
                            chapterOne = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1000, DATA_SWEEP_THE_HARBOR, 0);

                            wTrashSlain1 = 0;
                        }
                        break;
                    case DATA_SWEEP_THE_HARBOR:
                        switch (data)
                        {
                            case SPECIAL: // barrels exploded
                                wBarrelsExploded++;

                                if (wBarrelsExploded < 4)
                                    SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_HORDE_SHIP_TORCHED, wBarrelsExploded, GetScenarioGUID(), time(NULL), 0, 0));
                                break;
                            case SPECIAL + 1: // captains slain
                                wCaptainsSlain1++;

                                if (wCaptainsSlain1 < 4)
                                    SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_HORDE_CAPTAINS_SLAIN, wCaptainsSlain1, GetScenarioGUID(), time(NULL), 0, 0));
                                break;
                            case SPECIAL + 2: // trash slain
                                wTrashSlain1++;

                                if (wTrashSlain1 < 11)
                                    SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_ROKNAH_SOLDERS_SLAIN, wTrashSlain1, GetScenarioGUID(), time(NULL), 0, 0));
                                break;
                        }

                        if (wBarrelsExploded >= 3 && wCaptainsSlain1 >= 3 && wTrashSlain1 >= 10 && GetData(DATA_SWEEP_THE_HARBOR) != DONE)
                        {
                            SetBossState(type, DONE);
                            chapterTwo = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1000, DATA_INTO_THE_RUINS, 0);
                        }
                        break;
                    case DATA_INTO_THE_RUINS:
                        if (wWyvernSlain >= 2 && wGashnulSlain)
                        {
                            SetBossState(type, DONE);
                            chapterThree = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1000, DATA_LADY_PROUDMOORE, 0);
                        }
                        break;
                    case DATA_LADY_PROUDMOORE:
                        SetBossState(type, DONE);
                        chapterFour = DONE;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1000, DATA_DESTROY_THE_DESTROYER, 0);
                        break;
                    case DATA_DESTROY_THE_DESTROYER:
                        if (data == SPECIAL + 1)
                        {
                            wStolenStandards++;

                            if (wStolenStandards < 4)
                                SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_BANNER_FOUND, wStolenStandards, GetScenarioGUID(), time(NULL), 0, 0));
                        }

                        if (wStolenStandards >= 3 && wGateCrusherSlain)
                        {
                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1000, DATA_LAST_STAND, 0);

                            chapterFive = DONE;
                        }
                        break;
                    case DATA_LAST_STAND:
                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_DEFEAT_ROHNAH, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_JAINA_PROTECTED, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        DoFinishLFGDungeon(566);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WELCOME_TO_THERAMORE:
                        return chapterOne;
                    case DATA_SWEEP_THE_HARBOR:
                        return chapterTwo;
                    case DATA_INTO_THE_RUINS:
                        return chapterThree;
                    case DATA_LADY_PROUDMOORE:
                        return chapterFour;
                    case DATA_DESTROY_THE_DESTROYER:
                        return chapterFive;
                    case DATA_LAST_STAND:
                        return chapterSix;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                if (type == NPC_JAINA_PROUDMOORE)
                    return jainaGUID;

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

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (state == DONE)
                {
                    switch (type)
                    {
                        case DATA_WELCOME_TO_THERAMORE:
                        {
                            std::vector<uint32> tempRegion = { NPC_AIRSHIP_CREWMAN, NPC_AIRSHIP_MARINE, NPC_ROKNAH_HAG, NPC_ROKNAH_GRUNT, NPC_ROKNAH_FELCASTER, NPC_ROKNAH_HEADHUNTER, NPC_ROKNAH_LOA_SINGER, NPC_ROKNAH_RIDER, NPC_ROKNAH_SKRIMISHER, NPC_LEAKED_OIL_DRUM, NPC_SKY_CAPTAIN_DAZRIP, NPC_CAPTAIN_KORTHOK, NPC_CAPTAIN_MOUSSON };
                            HandleActivateSpawnRegion(tempRegion, beachPhaseGUIDs);
                            break;
                        }
                        case DATA_SWEEP_THE_HARBOR:
                        {
                            std::vector<uint32> tempRegion = { NPC_ROKNAH_HAG, NPC_ROKNAH_GRUNT, NPC_ROKNAH_FELCASTER, NPC_ROKNAH_HEADHUNTER, NPC_ROKNAH_LOA_SINGER, NPC_ROKNAH_RIDER, NPC_ROKNAH_SKRIMISHER, NPC_VICIOUS_WYVERN, NPC_GASHNUL, NPC_ROKNAH_WAVE_CALLER };
                            HandleActivateSpawnRegion(tempRegion, ruinsPhaseGUIDs);
                            break;
                        }
                        case DATA_INTO_THE_RUINS:
                        {
                            std::vector<uint32> tempRegion = { NPC_JAINA_PROUDMOORE };
                            HandleActivateSpawnRegion(tempRegion, ruinsPhaseGUIDs);
                            break;
                        }
                        case DATA_LADY_PROUDMOORE:
                        {
                            instance->SummonCreature(NPC_GATECRUSHER, SiegeMachineSpawnPos);
                            break;
                        }
                    }
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "F O T A ";

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

                if (dataHead1 == 'F' && dataHead2 == 'O' && dataHead3 == 'T' && dataHead4 == 'A')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_WELCOME_TO_THERAMORE, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_SWEEP_THE_HARBOR, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_INTO_THE_RUINS, chapterThree);
                    loadStream >> temp; // chapterFour complete
                    chapterFour = temp;
                    SetData(DATA_LADY_PROUDMOORE, chapterFour);
                    loadStream >> temp; // chapterFive complete
                    chapterFive = temp;
                    SetData(DATA_DESTROY_THE_DESTROYER, chapterFive);
                    loadStream >> temp; // last chapter complete
                    chapterSix = temp;
                    SetData(DATA_LAST_STAND, chapterSix);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_fall_of_theramore_alliance_InstanceMapScript(map);
        }
};

void AddSC_instance_fall_of_theramore_alliance()
{
    new instance_fall_of_theramore_alliance();
}