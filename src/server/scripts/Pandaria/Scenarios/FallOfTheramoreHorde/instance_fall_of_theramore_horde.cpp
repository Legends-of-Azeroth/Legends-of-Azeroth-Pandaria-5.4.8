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
#include "fall_of_theramore_horde.h"
#include "ScenarioMgr.h"

class instance_fall_of_theramore_horde : public InstanceMapScript
{
    public:
        instance_fall_of_theramore_horde() : InstanceMapScript("instance_fall_of_theramore_horde", 999) { }

        struct instance_fall_of_theramore_horde_InstanceMapScript : public InstanceScript
        {
            instance_fall_of_theramore_horde_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[CHAPTERS];
            std::map<uint32, ObjectGuid> TheramoreEncounters;
            std::list<ObjectGuid> guardGUIDs;
            uint32 chapterOne, chapterTwo, chapterThree, chapterFour, chapterFive, barrelsExplode, gryponsDied, baldrucDied, tanksDestoyed, bigBessaDied, hedricDied;
            uint32 captainsSlain;
            EventMap m_mEvents;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                barrelsExplode = 0;
                gryponsDied    = 0;
                baldrucDied    = 0;
                chapterOne     = 0;
                chapterTwo     = 0;
                chapterThree   = 0;
                chapterFour    = 0;
                chapterFive    = 0;
                tanksDestoyed  = 0;
                bigBessaDied   = 0;
                hedricDied     = 0;
                captainsSlain  = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 999, DATA_TO_THE_WATERLINE, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_BLASTMASTER_SPARKFUSE:
                    case NPC_CAPTAIN_DROK:
                        TheramoreEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_SABOTAGED_TANK:
                        creature->CastSpell(creature, SPELL_TANK_EXPLOSION, true);
                        creature->DealDamage(creature, creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        break;
                    case NPC_THERAMORE_ARCANIST:
                    case NPC_THERAMORE_FOOTMAN:
                    case NPC_THERAMORE_GRYPHON:
                    case NPC_THERAMORE_MARKSMAN:
                    case NPC_THERAMORE_OFFICER:
                    case NPC_KNIGHT_OF_THERAMORE:
                    case NPC_BIG_BESSA:
                    case NPC_UNMANNED_TANK:
                    case NPC_BALDRUC:
                    case NPC_HEDRIC_EVENCANE:
                    case NPC_THERAMORE_FAITHFUL:
                        if (creature->GetPositionX() > -3869.0f)
                        {
                            guardGUIDs.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                        }
                        else
                            creature->SetFaction(1077);
                        break;
                    case NPC_THALEN_SONGWEAVER:
                        TheramoreEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        guardGUIDs.push_back(creature->GetGUID());
                        creature->SetVisible(false);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_BALDRUC:
                            baldrucDied = 1;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_BALDRUK_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            SetData(DATA_GRYPHON_DOWN, SPECIAL);
                            break;
                        case NPC_THERAMORE_GRYPHON:
                            gryponsDied++;

                            if (gryponsDied < 3)
                                SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_GRYPHONE_SLAIN, gryponsDied, GetScenarioGUID(), time(NULL), 0, 0));

                            SetData(DATA_GRYPHON_DOWN, SPECIAL);
                            break;
                        case NPC_BIG_BESSA:
                            bigBessaDied = 1;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_BIG_BESSA_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            SetData(DATA_TANKS_FOR_NOTHING, SPECIAL);
                            break;
                        case NPC_HEDRIC_EVENCANE:
                            hedricDied = 1;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_HEDRIK_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                            SetData(DATA_FLY_SPY, SPECIAL);
                            break;
                        case NPC_CAPTAIN_TELLERN:
                        case NPC_SPELLSHARPER_LANARA:
                        case NPC_CAPTAIN_DASHING:
                            captainsSlain++;

                            if (captainsSlain < 4)
                                SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_ALLIANCE_CAPTAIN_SLAINED, captainsSlain, GetScenarioGUID(), time(NULL), 0, 0));

                            SetData(DATA_TO_THE_WATERLINE, SPECIAL + 3);
                            break;
                    }
                }
            }

            // Thx blizzard :/
            void HandleActivateSpawnRegion(std::vector<uint32> guardsEntry)
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
                    case DATA_TO_THE_WATERLINE:
                        if (data < 7)
                            barrelsExplode = data;

                        if (barrelsExplode < 7)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_BARRELS_EXPLOSIVE, barrelsExplode, GetScenarioGUID(), time(NULL), 0, 0));

                        if (barrelsExplode >= 6 && captainsSlain >= 3)
                        {
                            SetBossState(type, DONE);
                            chapterOne = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 999, DATA_THE_BLASTMASTER, 0);
                        }
                        SaveToDB();
                        break;
                    case DATA_THE_BLASTMASTER:
                        SetBossState(type, DONE);
                        chapterTwo = DONE;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 999, DATA_GRYPHON_DOWN, 0);

                        SaveToDB();
                        break;
                    case DATA_GRYPHON_DOWN:
                        if (baldrucDied && gryponsDied >= 2)
                        {
                            SetBossState(type, DONE);
                            chapterThree = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 999, DATA_TANKS_FOR_NOTHING, 0);
                        }
                        SaveToDB();
                        break;
                    case DATA_TANKS_FOR_NOTHING:
                        if (data == IN_PROGRESS && tanksDestoyed < 3)
                        {
                            tanksDestoyed++;
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_TANKS_EXPLOSIVE, tanksDestoyed, GetScenarioGUID(), time(NULL), 0, 0));
                        }

                        if (bigBessaDied && tanksDestoyed >= 3)
                        {
                            SetBossState(type, DONE);
                            chapterFour = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 999, DATA_FLY_SPY, 0);
                        }
                        SaveToDB();
                        break;
                    case DATA_FLY_SPY:
                        if (hedricDied && data == DONE)
                        {
                            SetBossState(DATA_FLY_SPY, DONE);
                            DoFinishLFGDungeon(567);
                        }
                        break;
                }
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_TO_THE_WATERLINE:
                        return barrelsExplode < 6 ? barrelsExplode : chapterOne;
                    case DATA_THE_BLASTMASTER:
                        return chapterTwo;
                    case DATA_GRYPHON_DOWN:
                        return chapterThree;
                    case DATA_TANKS_FOR_NOTHING:
                        return chapterFour;
                    case DATA_FLY_SPY:
                        return chapterFive;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_BLASTMASTER_SPARKFUSE:
                    case NPC_THALEN_SONGWEAVER:
                    case NPC_CAPTAIN_DROK:
                        return TheramoreEncounters.find(type)->second;
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
                if (state == DONE)
                {
                    switch (type)
                    {
                        case DATA_TO_THE_WATERLINE:
                            if (Creature* SparkFuse = instance->GetCreature(GetGuidData(NPC_BLASTMASTER_SPARKFUSE)))
                                SparkFuse->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            break;
                        case DATA_THE_BLASTMASTER:
                        {
                            std::vector<uint32> tempRegion = { NPC_THERAMORE_FOOTMAN, NPC_THERAMORE_ARCANIST, NPC_THERAMORE_MARKSMAN, NPC_THERAMORE_OFFICER, NPC_THERAMORE_GRYPHON, NPC_KNIGHT_OF_THERAMORE, NPC_BALDRUC, NPC_THERAMORE_FAITHFUL };
                            HandleActivateSpawnRegion(tempRegion);
                            break;
                        }
                        case DATA_GRYPHON_DOWN:
                        {
                            std::vector<uint32> tempRegion = { NPC_BIG_BESSA, NPC_UNMANNED_TANK };
                            HandleActivateSpawnRegion(tempRegion);
                            break;
                        }
                        case DATA_TANKS_FOR_NOTHING:
                        {
                            std::vector<uint32> tempRegion = { NPC_THALEN_SONGWEAVER, NPC_HEDRIC_EVENCANE };
                            HandleActivateSpawnRegion(tempRegion);
                            break;
                        }
                        case DATA_FLY_SPY:
                            break;
                    }
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "F O T H " << barrelsExplode << ' ' << gryponsDied << ' ' << baldrucDied << ' ' << tanksDestoyed << ' ' << bigBessaDied << ' ' << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour << ' ' << chapterFive;

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

                if (dataHead1 == 'F' && dataHead2 == 'O' && dataHead3 == 'T' && dataHead4 == 'H')
                {
                    uint32 temp = 0;
                    loadStream >> temp;
                    barrelsExplode = temp;
                    loadStream >> temp;
                    gryponsDied = temp;
                    loadStream >> temp;
                    baldrucDied = temp;
                    loadStream >> temp;
                    tanksDestoyed = temp;
                    loadStream >> temp;
                    bigBessaDied = temp;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_TO_THE_WATERLINE, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_THE_BLASTMASTER, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_GRYPHON_DOWN, chapterThree);
                    loadStream >> temp; // chapterFour complete
                    chapterFour = temp;
                    SetData(DATA_TANKS_FOR_NOTHING, chapterFour);
                    loadStream >> temp; // last chapter complete
                    chapterFive = temp;
                    SetData(DATA_FLY_SPY, chapterFive);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_fall_of_theramore_horde_InstanceMapScript(map);
        }
};

void AddSC_instance_fall_of_theramore_horde()
{
    new instance_fall_of_theramore_horde();
}