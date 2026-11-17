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
#include "brewmoon_festival.h"
#include "ScenarioMgr.h"

class instance_brewmoon_festival : public InstanceMapScript
{
    public:
        instance_brewmoon_festival() : InstanceMapScript("instance_brewmoon_festival", 1051) { }

        struct instance_brewmoon_festival_InstanceMapScript : public InstanceScript
        {
            instance_brewmoon_festival_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[CHAPTERS];
            std::map<uint32, ObjectGuid> m_BrewmoonEncounters;
            std::map<uint64, uint32> m_ScoutsType;
            std::list<ObjectGuid> m_vilmanGuids, m_waterspiritGuids, m_yaungolAttack;
            uint32 chapterOne, chapterTwo, chapterThree, chapterFour, m_cBridge, m_cWest, m_cCarvens, s1_chapterTwo, s2_chapterTwo, s3_chapterTwo, m_currentType;
            EventMap m_mEvents;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne    = 0;
                chapterTwo    = 0;
                chapterThree  = 0;
                chapterFour   = 0;
                m_cBridge     = 0;
                m_cCarvens    = 0;
                m_cWest       = 0;
                s1_chapterTwo = 0;
                s2_chapterTwo = 0;
                s3_chapterTwo = 0;
                m_currentType = 1;

                m_vilmanGuids.clear();
                m_waterspiritGuids.clear();
                m_yaungolAttack.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1051, DATA_BREWMOON_FESTIVAL, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_APOTHECARY_CHENG:
                        creature->SetVisible(false);
                        m_BrewmoonEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_HUNGRY_VIRMEN:
                        m_vilmanGuids.push_back(creature->GetGUID());
                        m_BrewmoonEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_WATER_SPIRIT:
                        m_waterspiritGuids.push_back(creature->GetGUID());
                        m_BrewmoonEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        creature->CastSpell(creature, SPELL_WATER_SPIRIT_CHANNEL, false);
                        break;
                    case NPC_SPAWN_BURROWED:
                    case NPC_DEN_MOTHER_MOOF:
                    case NPC_LI_TE:
                    case NPC_PURE_WATER_GLOBE:
                    case NPC_TARTS_BOAT:
                    case NPC_ASSISTANT_TART:
                    case NPC_ASSISTANT_KIEU:
                    case NPC_SACK_OF_GRAIN:
                    case NPC_BREWMASTER_BOOF:
                    case NPC_WARBRINGER_QOBI:
                    case NPC_COMMANDER_HSIEH:
                    case NPC_MISTWEAVER_NIAN:
                        m_BrewmoonEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_SLG_GENERIC_MOP:
                        creature->SetDisplayId(11686);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_HUNGRY_VIRMEN:
                            for (std::list<ObjectGuid>::iterator it = m_vilmanGuids.begin(); it != m_vilmanGuids.end(); ++it)
                            {
                                if (*it == unit->GetGUID())
                                {
                                    m_vilmanGuids.erase(it);
                                    break;
                                }
                            }

                            if (m_vilmanGuids.empty())
                            {
                                if (Creature* m_uiBurrowed = instance->GetCreature(GetGuidData(NPC_SPAWN_BURROWED)))
                                    m_uiBurrowed->CastSpell(m_uiBurrowed, SPELL_BURROW_COSMETIC, false);
                            }

                            // Assistant kieu Intro
                            if (m_vilmanGuids.size() == 6)
                                if (Creature* Kieu = instance->GetCreature(GetGuidData(NPC_ASSISTANT_KIEU)))
                                    Kieu->AI()->DoAction(ACTION_INTRO);
                            break;
                        case NPC_WATER_SPIRIT:
                            for (std::list<ObjectGuid>::iterator it = m_waterspiritGuids.begin(); it != m_waterspiritGuids.end(); ++it)
                            {
                                if (*it == unit->GetGUID())
                                {
                                    m_waterspiritGuids.erase(it);
                                    break;
                                }
                            }

                            if (m_waterspiritGuids.empty())
                            {
                                if (Creature* m_uiLiTe = instance->GetCreature(GetGuidData(NPC_LI_TE)))
                                    m_uiLiTe->AI()->DoAction(ACTION_INTRO);
                            }
                            break;
                        case NPC_BATAARI_YAUNGOL:
                            if (GetData(DATA_SCOUTS_REPORT) == DONE)
                                break;

                            if (uint32 m_val = m_ScoutsType.find(unit->GetGUID())->second)
                            {
                                switch (m_val)
                                {
                                    case TYPE_PASSAGE:
                                        m_cCarvens++;
                                        SetData(DATA_CARVENS, m_cCarvens);
                                        break;
                                    case TYPE_BRIDGE:
                                        m_cBridge++;
                                        SetData(DATA_BRIDGE, m_cBridge);
                                        break;
                                    case TYPE_WEST:
                                        m_cWest++;
                                        SetData(DATA_WEST, m_cWest);
                                        break;
                                }

                                if (m_cCarvens >= 5 && m_cBridge >= 4 && m_cWest >= 6)
                                    SetData(DATA_SCOUTS_REPORT, DONE);
                            }
                            break;
                        case NPC_BATAARI_FLAMECALLER:
                        case NPC_BATAARI_OUTRUNNER:
                            for (std::list<ObjectGuid>::iterator it = m_yaungolAttack.begin(); it != m_yaungolAttack.end(); ++it)
                            {
                                if (*it == unit->GetGUID())
                                {
                                    m_yaungolAttack.erase(it);
                                    break;
                                }
                            }

                            if (m_yaungolAttack.empty())
                            {
                                HandleYaungolAssault(NPC_BATAARI_WAR_YETI, m_currentType, true);
                                m_currentType++;
                            }
                            break;
                    }
                }
            }

            // Using at chapter Scouts Report and Yaungol Attack
            void HandleScoutAssault(Position m_pos, uint32 m_type, uint8 m_count, uint32 m_dataType, bool notSave = false)
            {
                for (uint8 i = 0; i < m_count; i++)
                {
                    if (Creature* m_temp = instance->SummonCreature(NPC_BATAARI_YAUNGOL, m_pos))
                    {
                        if (!notSave)
                            m_ScoutsType.insert(std::pair<uint64, uint32>(m_temp->GetGUID(), m_type));

                        m_temp->AI()->SetData(m_dataType, NOT_STARTED);
                    }
                }
            }

            // Using at chapter Yaungol Attack
            void HandleYaungolAssault(uint32 m_creatureId, uint32 m_dataType, bool Yeti = false)
            {
                Position m_pos;

                switch (m_dataType)
                {
                    case TYPE_BRIDGE:
                        m_pos = AssaultPath1[0];
                        break;
                    case TYPE_PASSAGE:
                        m_pos = AssaultPath2[0];
                        break;
                    case TYPE_WEST:
                        m_pos = AssaultPath3[0];
                        break;
                }

                uint8 m_count = Yeti ? 1 : urand(2, 3);

                for (uint8 i = 0; i < m_count; i++)
                {
                    if (Creature* m_temp = instance->SummonCreature(m_creatureId, m_pos))
                    {
                        if (!Yeti)
                            m_yaungolAttack.push_back(m_temp->GetGUID());

                        m_temp->AI()->SetData(m_dataType, NOT_STARTED);
                    }
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_BREWMOON_FESTIVAL:
                    {
                        switch (data)
                        {
                            case NPC_DEN_MOTHER_MOOF:
                                if (Creature* Kieu = instance->GetCreature(GetGuidData(NPC_ASSISTANT_KIEU)))
                                    Kieu->AI()->DoAction(ACTION_BREWMOON_FESTIVAL);
                                break;
                            case NPC_LI_TE:
                                if (Creature* m_globe = instance->GetCreature(GetGuidData(NPC_PURE_WATER_GLOBE)))
                                    m_globe->CastSpell(m_globe, SPELL_SPAWN_WATER_GLOBE, false);

                                if (Creature* Tart = instance->GetCreature(GetGuidData(NPC_ASSISTANT_TART)))
                                    Tart->AI()->DoAction(ACTION_BREWMOON_FESTIVAL);
                                break;
                            case NPC_KARSAR_BLOODLETTER:
                                break;
                        }
                        chapterOne++;

                        if (chapterOne == DONE)
                        {
                            SaveToDB();
                            SetBossState(DATA_BREWMOON_FESTIVAL, DONE);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1051, DATA_SCOUTS_REPORT, 0);

                            // Festival Event
                            if (Creature* Brewmaster = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BOOF)))
                                Brewmaster->AI()->DoAction(ACTION_INTRO);

                            // Yaungol Scouts
                            m_mEvents.ScheduleEvent(EVENT_SCOUTS, 40 * IN_MILLISECONDS);
                            m_mEvents.ScheduleEvent(EVENT_BLOATS, urand(45 * IN_MILLISECONDS, 47 * IN_MILLISECONDS));
                        }
                        break;
                    }
                    case DATA_SCOUTS_REPORT:
                    {
                        chapterTwo = data;
                        SaveToDB();
                        SetBossState(DATA_SCOUTS_REPORT, DONE);

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1051, DATA_YAUNGOL_ATTACK, 0);

                        m_mEvents.ScheduleEvent(EVENT_YAUNGOLS_ATTACK, 6 * IN_MILLISECONDS);
                        break;
                    }
                    case DATA_YAUNGOL_ATTACK:
                    {
                        chapterThree++;
                        SaveToDB();

                        if (chapterThree == 1)
                            if (Creature* bo = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BOOF)))
                                bo->AI()->Talk(TALK_SPECIAL_7);

                        if (chapterThree >= DONE)
                        {
                            SaveToDB();
                            SetBossState(DATA_YAUNGOL_ATTACK, DONE);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1051, DATA_WARBRINGER_QOBI, 0);

                            m_mEvents.ScheduleEvent(EVENT_QOBI_ARRIVED, 2 * IN_MILLISECONDS);
                        }
                        else
                            m_mEvents.ScheduleEvent(EVENT_YAUNGOLS_ATTACK, 3 * IN_MILLISECONDS);
                        break;
                    }
                    case DATA_WARBRINGER_QOBI:
                        chapterFour = data;
                        SetBossState(DATA_WARBRINGER_QOBI, EncounterState(data));
                        if (chapterFour == DONE)
                            DoFinishLFGDungeon(539);

                        if (Creature* bo = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BOOF)))
                            bo->AI()->Talk(TALK_SPECIAL_8);
                        break;
                    case DATA_CARVENS:
                    case DATA_BRIDGE:
                    case DATA_WEST:
                        SaveToDB();
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SCOUTS:
                            HandleScoutAssault(AssaultPath2[0], TYPE_PASSAGE, 3, TYPE_PASSAGE);
                            HandleScoutAssault(AssaultPath1[0], TYPE_BRIDGE, 4, TYPE_BRIDGE);
                            HandleScoutAssault(AssaultPath3[0], TYPE_WEST, 6, TYPE_WEST);
                            break;
                        case EVENT_BLOATS:
                            HandleScoutAssault(AssaultPath5[0], TYPE_PASSAGE, 2, TYPE_BLOAT);
                            break;
                        case EVENT_YAUNGOLS_ATTACK:
                            HandleYaungolAssault(NPC_BATAARI_FLAMECALLER, m_currentType);
                            HandleYaungolAssault(NPC_BATAARI_OUTRUNNER, m_currentType);
                            urand(0, 1) ? HandleScoutAssault(AssaultPath2[0], TYPE_PASSAGE, 2, TYPE_PASSAGE) : HandleScoutAssault(AssaultPath1[0], TYPE_BRIDGE, 2, TYPE_BRIDGE);
                            break;
                        case EVENT_QOBI_ARRIVED:
                            instance->SummonCreature(NPC_WARBRINGER_QOBI, AssaultPath4[0]);
                            break;
                    }
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_BREWMOON_FESTIVAL:
                        return chapterOne;
                    case DATA_SCOUTS_REPORT:
                        return chapterTwo;
                    case DATA_YAUNGOL_ATTACK:
                        return chapterThree;
                    case DATA_WARBRINGER_QOBI:
                        return chapterFour;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_APOTHECARY_CHENG:
                    case NPC_SPAWN_BURROWED:
                    case NPC_LI_TE:
                    case NPC_PURE_WATER_GLOBE:
                    case NPC_TARTS_BOAT:
                    case NPC_ASSISTANT_TART:
                    case NPC_ASSISTANT_KIEU:
                    case NPC_SACK_OF_GRAIN:
                    case NPC_BREWMASTER_BOOF:
                    case NPC_WARBRINGER_QOBI:
                    case NPC_COMMANDER_HSIEH:
                    case NPC_MISTWEAVER_NIAN:
                        return m_BrewmoonEncounters.find(type)->second;
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
                saveStream << "B F " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour << ' ' << m_cCarvens << ' ' << m_cBridge << ' ' << m_cWest;

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

                if (dataHead1 == 'B' && dataHead2 == 'F')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_BREWMOON_FESTIVAL, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_SCOUTS_REPORT, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_YAUNGOL_ATTACK, chapterThree);
                    loadStream >> temp; // Barrels progress
                    chapterFour = temp;
                    SetData(DATA_WARBRINGER_QOBI, chapterFour);
                    loadStream >> temp; // Carvens c2 progress
                    m_cCarvens = temp;
                    SetData(DATA_CARVENS, m_cCarvens);
                    loadStream >> temp; // Bridge c2 progress
                    m_cBridge = temp;
                    SetData(DATA_BRIDGE, m_cBridge);
                    loadStream >> temp; // West c2 progress
                    m_cWest = temp;
                    SetData(DATA_WEST, m_cWest);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_brewmoon_festival_InstanceMapScript(map);
        }
};

void AddSC_instance_brewmoon_festival()
{
    new instance_brewmoon_festival();
}