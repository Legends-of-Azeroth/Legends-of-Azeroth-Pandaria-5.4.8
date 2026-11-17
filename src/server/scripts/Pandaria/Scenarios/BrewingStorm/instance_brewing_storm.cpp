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
#include "brewing_storm.h"
#include "ScenarioMgr.h"

class instance_brewing_storm : public InstanceMapScript
{
    public:
        instance_brewing_storm() : InstanceMapScript("instance_brewing_storm", 1005) { }

        struct instance_brewing_storm_InstanceMapScript : public InstanceScript
        {
            instance_brewing_storm_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            uint64 gongGUID, templeDoorGUID;
            uint32 m_auiEncounter[CHAPTERS];
            uint32 m_chapterOne, m_chapterTwo, m_chapterThree;
            ObjectGuid blancheGUID;
            ObjectGuid borakhulaGUID;
            std::vector<ObjectGuid> villagerGUIDs;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                m_chapterOne   = 0;
                m_chapterTwo   = 0;
                m_chapterThree = 0;
                blancheGUID = ObjectGuid::Empty;
                borakhulaGUID = ObjectGuid::Empty;

                villagerGUIDs.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1005, DATA_MAKE_BOOMERS_BREW, 0);
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->GetEntry() == NPC_THUNDERPAW_GUARDIAN)
                {
                    if (Creature* blanche = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BLANCHE)))
                        blanche->AI()->SetData(TYPE_PARTY_OF_SIX, 0);
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_BREWMASTER_BLANCHE:
                        blancheGUID = creature->GetGUID();
                        break;
                    case NPC_BOROKHULA_THE_DESTROYER:
                        borakhulaGUID = creature->GetGUID();
                        break;
                    case NPC_BLANCHES_STILL:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                        break;
                    case NPC_LIGHTNING_TARGET_BUNNY:
                    case NPC_LIGHTNING_TARGET_BUNNY_2:
                        creature->SetDisplayId(17188);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        break;
                    case NPC_THUNDERPAW_INITIATE:
                        villagerGUIDs.push_back(creature->GetGUID());
                        break;
                }        
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_MAKE_BOOMERS_BREW:
                        m_chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1005, DATA_ROAD_TO_THUNDERPAW, 0);
                        break;
                    case DATA_ROAD_TO_THUNDERPAW:
                        m_chapterTwo = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1005, DATA_SAVE_THUNDERPAW_REFUGE, 0);

                        if (Creature* borokhula = instance->GetCreature(GetGuidData(NPC_BOROKHULA_THE_DESTROYER)))
                            borokhula->AI()->DoAction(ACTION_BOROKHULA_INIT);

                        for (auto&& itr : villagerGUIDs)
                            if (Creature* villager = instance->GetCreature(itr))
                                villager->SetFaction(1812);

                        for (auto&& itr : defendersSpawnPos)
                            instance->SummonCreature(NPC_THUNDERPAW_GUARDIAN, { itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ(), itr.GetOrientation() });
                        break;
                    case DATA_SAVE_THUNDERPAW_REFUGE:
                        m_chapterThree = data;

                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_BORAKHULA_DEFEAT, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_HEROIC ? 639 : 517);
                        DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_NORMAL ? 639 : 517);
                        break;
                }

                // Synch with data
                SetBossState(type, EncounterState(data));

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MAKE_BOOMERS_BREW:
                        return m_chapterOne;
                    case DATA_ROAD_TO_THUNDERPAW:
                        return m_chapterTwo;
                    case DATA_SAVE_THUNDERPAW_REFUGE:
                        return m_chapterThree;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                case NPC_BREWMASTER_BLANCHE:
                    return blancheGUID;
                case NPC_BOROKHULA_THE_DESTROYER:
                    return borakhulaGUID;
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

            void Update(uint32 diff) override
            {
                events.Update(diff);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "B S " << m_chapterOne << ' ' << m_chapterTwo << ' ' << m_chapterThree;

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

                if (dataHead1 == 'B' && dataHead2 == 'S')
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    m_chapterOne = tmpState; // Load First Chapter
                    SetData(DATA_MAKE_BOOMERS_BREW, m_chapterOne);
                    loadStream >> tmpState;
                    m_chapterTwo = tmpState; // Load Second Chapter
                    SetData(DATA_ROAD_TO_THUNDERPAW, m_chapterTwo);
                    loadStream >> tmpState;
                    m_chapterThree = tmpState; // Load Third Chapter
                    SetData(DATA_SAVE_THUNDERPAW_REFUGE, m_chapterThree);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_brewing_storm_InstanceMapScript(map);
        }
};

void AddSC_instance_brewing_storm()
{
    new instance_brewing_storm();
}