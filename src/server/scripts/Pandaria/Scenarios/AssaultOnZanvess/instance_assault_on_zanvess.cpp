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
#include "assault_on_zanvess.h"
#include "ScenarioMgr.h"

class instance_assault_on_zanvess : public InstanceMapScript
{
    public:
        instance_assault_on_zanvess() : InstanceMapScript("instance_assault_on_zanvess", 1050) { }

        struct instance_assault_on_zanvess_InstanceMapScript : public InstanceScript
        {
            instance_assault_on_zanvess_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            ObjectGuid gongGUID, templeDoorGUID;
            uint32 m_auiEncounter[CHAPTERS];
            std::map<uint32, ObjectGuid> AssaultEncounters;
            std::vector<ObjectGuid> gyroGUIDs, NalleyGUIDs;
            uint32 chapterOne, chapterTwo, chapterThird, chapterFour;
            uint32 sonicTowerCount;
            uint32 alterTowerCount;
            uint32 teamValue;
            uint32 telvrakLinked;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne      = 0;
                chapterTwo      = 0;
                chapterThird    = 0;
                chapterFour     = 0;
                sonicTowerCount = 0;
                alterTowerCount = 0;
                teamValue       = 0;
                telvrakLinked   = 0;

                gyroGUIDs.clear();
                NalleyGUIDs.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!teamValue)
                    teamValue = player->GetTeam();

                if (teamValue == HORDE)
                {
                    if (Creature* admiral = instance->GetCreature(GetGuidData(NPC_REAR_ADMIRAL_ZIGG)))
                        admiral->AI()->Talk(TALK_INTRO, player);
                }
                else
                {
                    if (Creature* rodgers = instance->GetCreature(GetGuidData(NPC_SKY_GENERAL_RODGER)))
                        rodgers->AI()->Talk(TALK_INTRO, player);
                }

                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1050, DATA_ASSAULT_ON_ZANVESS - 1, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!teamValue)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            teamValue = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_SLG_GENERIC_MOP:
                    case NPC_SHIELD_CHANNEL_POINT:
                        creature->SetDisplayId(11686);
                        break;
                    case NPC_KORKRON_GUNSHIP:
                    case NPC_SKYFIRE_GYRO_VEHICLE:
                        if (!creature->GetDBTableGUIDLow())
                            gyroGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_SCENARIO_CONTROLLER:
                    case NPC_COMMANDER_TELVRAK:
                    case NPC_REAR_ADMIRAL_ZIGG:
                    case NPC_SKY_GENERAL_RODGER:
                        AssaultEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_NALLEY:
                        if (!creature->GetDBTableGUIDLow())
                            NalleyGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_7TH_LEGION_INFILTRATOR:
                        if (teamValue == ALLIANCE)
                            creature->UpdateEntry(NPC_KORKRON_INFILTRATOR, ALLIANCE);
                        break;
                    case NPC_7TH_LIGION_PRIEST:
                        if (teamValue == ALLIANCE)
                            creature->UpdateEntry(NPC_KORKRON_PRIEST, ALLIANCE);
                        break;
                    case NPC_TEAM_LEADER_SCOOTER:
                        if (teamValue == ALLIANCE)
                            creature->UpdateEntry(NPC_SQUAD_LEADER_BOSH, ALLIANCE);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                switch (unit->GetEntry())
                {
                    case NPC_SONIC_CONTROL_TOWER:
                        sonicTowerCount++;

                        if (sonicTowerCount >= 3) // fly to second towers
                        {
                            for (auto&& itr : gyroGUIDs)
                                if (Creature* gunship = instance->GetCreature(itr))
                                    gunship->AI()->DoAction(ACTION_FIRST_TOWER_OFFENSIVE);

                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_KYPARITE_SONIC_TOWER, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        }
                        break;
                    case NPC_SONIC_CONTROL_TOWER_2:
                        if (++alterTowerCount >= 3) // fly to last towers
                        {
                            for (auto&& itr : gyroGUIDs)
                                if (Creature* gunship = instance->GetCreature(itr))
                                    gunship->AI()->DoAction(ACTION_SECOND_TOWER_OFFENSIVE);

                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_WHISPERING_STONES_SONIC_TOWER, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        }
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_ASSAULT_ON_ZANVESS:
                        SetBossState(type, EncounterState(data));
                        chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1050, type, 0);

                        for (auto&& sNalley : NalleyGUIDs)
                            if (Creature* Nalley = instance->GetCreature(sNalley))
                                Nalley->AI()->DoAction(ACTION_NALLEY_DEFENDERS);
                        break;
                    case DATA_DEFENSES_OF_ZANVESS:
                        chapterTwo++;

                        if (chapterTwo >= 12)
                        {
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_VENOMSTING_PITS_SONIC_TOWER, 1, GetScenarioGUID(), time(NULL), 0, 0));

                            SetBossState(type, DONE);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1050, type, 0);

                            for (auto&& itr : gyroGUIDs)
                                if (Creature* gunship = instance->GetCreature(itr))
                                    gunship->AI()->DoAction(ACTION_MOVE_TO_ISLAND);

                            if (Creature* controller = instance->GetCreature(GetGuidData(NPC_SCENARIO_CONTROLLER)))
                                controller->RemoveAurasDueToSpell(SPELL_ISLAND_SHIELD);

                            DoCastSpellOnPlayers(SPELL_STRAFING_RAN);

                            for (auto&& sNalley : NalleyGUIDs)
                                if (Creature* Nalley = instance->GetCreature(sNalley))
                                    Nalley->AI()->DoAction(ACTION_BEACH);
                        }
                        SaveToDB();
                        break;
                    case DATA_HEART_OF_ZANVESS:
                        chapterThird++;

                        if (chapterThird >= 2)
                        {
                            SetBossState(type, DONE);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1050, type, 0);

                            if (Creature* CommanderTelvrak = instance->GetCreature(GetGuidData(NPC_COMMANDER_TELVRAK)))
                                CommanderTelvrak->AI()->DoAction(ACTION_TELVRAK_ASSAULT);
                        }

                        SaveToDB();

                        break;
                    case DATA_TELVRAK_LINKED:
                        telvrakLinked = data;
                        break;
                    case DATA_WEAPON_OF_ZANVESS:
                        chapterFour = data;
                        SetBossState(type, EncounterState(data));
                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_DEFEAT_COMMANDER_TELVRAK, 1, GetScenarioGUID(), time(NULL), 0, 0));

                        // Scenario Complete
                        uint32 lfgEntry = GetData(DATA_FACTION) ? 537 : 593;
                        DoFinishLFGDungeon(lfgEntry);
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ASSAULT_ON_ZANVESS:
                        return chapterOne;
                    case DATA_DEFENSES_OF_ZANVESS:
                        return chapterTwo;
                    case DATA_HEART_OF_ZANVESS:
                        return chapterThird;
                    case DATA_WEAPON_OF_ZANVESS:
                        return chapterFour;
                    case DATA_FACTION:
                        return teamValue == HORDE ? 1 : 0;
                    case DATA_TELVRAK_LINKED:
                        return telvrakLinked;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_SCENARIO_CONTROLLER:
                    case NPC_COMMANDER_TELVRAK:
                    case NPC_REAR_ADMIRAL_ZIGG:
                    case NPC_SKY_GENERAL_RODGER:
                        return AssaultEncounters.find(type)->second;
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
                saveStream << "A O Z " << chapterOne << ' ' << chapterTwo << ' ' << chapterThird << ' ' << chapterFour;

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

                if (dataHead1 == 'A' && dataHead2 == 'O' && dataHead3 == 'Z')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_ASSAULT_ON_ZANVESS, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_DEFENSES_OF_ZANVESS, chapterTwo);
                    loadStream >> temp; // chapterThird complete
                    chapterThird = temp;
                    SetData(DATA_HEART_OF_ZANVESS, chapterThird);
                    loadStream >> temp; // chapterFour complete
                    chapterFour = temp;
                    SetData(DATA_WEAPON_OF_ZANVESS, chapterFour);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_assault_on_zanvess_InstanceMapScript(map);
        }
};

void AddSC_instance_assault_on_zanvess()
{
    new instance_assault_on_zanvess();
}