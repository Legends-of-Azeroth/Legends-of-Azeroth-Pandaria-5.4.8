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
#include "shadowfang_keep.h"

static std::vector<DoorData> const doorData =
{
    {GO_COURTYARD_DOOR, DATA_ASHBURY,   DOOR_TYPE_PASSAGE,  BOUNDARY_NONE},
    {GO_SORCERER_DOOR,  DATA_VALDEN,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE},
    {GO_ARUGAL_DOOR,    DATA_VALDEN,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE},
    {GO_ARUGAL_DOOR,    DATA_GODFREY,   DOOR_TYPE_ROOM,     BOUNDARY_NONE},
};

class instance_shadowfang_keep : public InstanceMapScript
{
    public:
        instance_shadowfang_keep() : InstanceMapScript("instance_shadowfang_keep", 33) { }

        struct instance_shadowfang_keep_InstanceMapScript : public InstanceScript
        {
            instance_shadowfang_keep_InstanceMapScript(Map* map) : InstanceScript(map) { }

            void Initialize() override
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                uiAshburyGUID = ObjectGuid::Empty;
                uiSilverlaineGUID = ObjectGuid::Empty;
                uiSpringvaleGUID = ObjectGuid::Empty;
                uiValdenGUID = ObjectGuid::Empty;
                uiGodfreyGUID = ObjectGuid::Empty;
                followerGUID = ObjectGuid::Empty;

                uiFryeGUID = ObjectGuid::Empty;
                uiHummelGUID = ObjectGuid::Empty;
                uiBaxterGUID = ObjectGuid::Empty;
                spawnCrazedTimer  = 0;
                teamInInstance    = 0;

                m_sFangIntro.clear();
                m_sFangPhasing.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!teamInInstance)
                    teamInInstance = player->GetTeam();
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!teamInInstance)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            teamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_BELMONT:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_IVAR, ALLIANCE);

                        // Exclude spawn at entrace
                        if (creature->GetPositionZ() > 82.47f || creature->GetPositionZ() < 79.23f)
                        {
                            m_sFangPhasing.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                            break;
                        }

                        // Quest Follower at Godfrey
                        if (creature->GetPositionZ() > 150.0f)
                            followerGUID = creature->GetGUID();

                        m_sFangIntro.push_back(creature->GetGUID());
                        break;
                    case NPC_GUARD_HORDE1:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_GUARD_ALLY, ALLIANCE);

                        if (creature->GetPositionZ() > 82.47f || creature->GetPositionZ() < 79.23f)
                        {
                            m_sFangPhasing.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                            break;
                        }

                        m_sFangIntro.push_back(creature->GetGUID());
                        break;
                    case NPC_GUARD_HORDE2:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_GUARD_ALLY, ALLIANCE);

                        if (creature->GetPositionZ() > 82.47f || creature->GetPositionZ() < 79.23f)
                        {
                            m_sFangPhasing.push_back(creature->GetGUID());
                            creature->SetVisible(false);
                            break;
                        }

                        m_sFangIntro.push_back(creature->GetGUID());
                        break;
                    case NPC_CROMUSH:
                        creature->SetVisible(false);
                        break;
                    case NPC_ASHBURY:
                        uiAshburyGUID = creature->GetGUID();
                        break;
                    case NPC_SILVERLAINE:
                        uiSilverlaineGUID = creature->GetGUID();
                        break;
                    case NPC_SPRINGVALE:
                        uiSpringvaleGUID = creature->GetGUID();
                        break;
                    case NPC_VALDEN:
                        uiValdenGUID = creature->GetGUID();
                        break;
                    case NPC_GODFREY:
                        uiGodfreyGUID = creature->GetGUID();
                        break;
                    case NPC_APOTHECARY_FRYE:
                        uiFryeGUID = creature->GetGUID();
                        break;
                    case NPC_APOTHECARY_HUMMEL:
                        uiHummelGUID = creature->GetGUID();
                        break;
                    case NPC_APOTHECARY_BAXTER:
                        uiBaxterGUID = creature->GetGUID();
                        break;
                }
            }

            // Set visible data in phase
            void HandleSetNextPhase(uint32 m_data)
            {
                std::list<Creature*> m_PhaseList;

                // Set invisible on another
                if (!m_sFangPhasing.empty())
                    for (auto m_phasingGUID : m_sFangPhasing)
                        if (Creature* m_guard = instance->GetCreature(m_phasingGUID))
                            m_guard->SetVisible(false);

                if (Creature* encounter = instance->GetCreature(GetGuidData(m_data)))
                {
                    for (auto m_phase : m_sFangEntry)
                        GetCreatureListWithEntryInGrid(m_PhaseList, encounter, m_phase, 28.0f);

                    if (!m_PhaseList.empty())
                    {
                        for (auto m_itr : m_PhaseList)
                        {
                            if (m_itr->AI()) // will work only for belmont cuz another guards doesn`t have AI
                                m_itr->AI()->DoAction(encounter->GetEntry());

                            m_itr->SetVisible(true);
                        }
                    }
                }

                // Set invislbe on intro guards (no way)
                if (m_data == DATA_ASHBURY)
                    if (!m_sFangIntro.empty())
                        for (auto m_phasingGUID : m_sFangIntro)
                            if (Creature* m_guard = instance->GetCreature(m_phasingGUID))
                                m_guard->SetVisible(false);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_COURTYARD_DOOR:
                    case GO_SORCERER_DOOR:
                    case GO_ARUGAL_DOOR:
                        AddDoor(go, true);
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_CROWN:
                        uiCrownEvent = data;
                        if (data == NOT_STARTED)
                            spawnCrazedTimer = urand(7000, 14000);
                        break;
                    case DATA_ASHBURY:
                    case DATA_SILVERLAINE:
                    case DATA_SPRINGVALE:
                    case DATA_VALDEN:
                    case DATA_GODFREY:
                        if (data == DONE)
                            HandleSetNextPhase(type);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_TEAM)
                    return teamInInstance;
                else if (type == DATA_CROWN)
                    return uiCrownEvent;

                return 0;
            }

            void Update(uint32 diff) override
            {
                if (GetData(DATA_CROWN) == IN_PROGRESS)
                {
                    if (spawnCrazedTimer <= diff)
                    {
                        if (Creature* hummel = instance->GetCreature(uiHummelGUID))
                            hummel->AI()->DoAction(3); // ACTION_SPAWN_CRAZED
                        spawnCrazedTimer = urand(2000, 5000);
                    }
                    else
                        spawnCrazedTimer -= diff;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S K " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ASHBURY:
                        return uiAshburyGUID;
                    case DATA_SILVERLAINE:
                        return uiSilverlaineGUID;
                    case DATA_SPRINGVALE:
                        return uiSpringvaleGUID;
                    case DATA_VALDEN:
                        return uiValdenGUID;
                    case DATA_GODFREY:
                        return uiGodfreyGUID;
                    case DATA_FRYE:
                        return uiFryeGUID;
                    case DATA_HUMMEL:
                        return uiHummelGUID;
                    case DATA_BAXTER:
                        return uiBaxterGUID;
                    case DATA_FOLLOWER:
                        return followerGUID;
                }
                return ObjectGuid::Empty;
            }

            void Load(const char* in) override
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

                if (dataHead1 == 'S' && dataHead2 == 'K')
                {
                    for (uint32 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        
        private:
            ObjectGuid uiAshburyGUID;
            ObjectGuid uiSilverlaineGUID;
            ObjectGuid uiSpringvaleGUID;
            ObjectGuid uiValdenGUID;
            ObjectGuid uiGodfreyGUID;
            ObjectGuid uiFryeGUID;
            ObjectGuid uiHummelGUID;
            ObjectGuid uiBaxterGUID;
            ObjectGuid followerGUID;
            uint32 spawnCrazedTimer;
            uint32 uiCrownEvent;
            uint32 teamInInstance;

            std::vector<ObjectGuid> m_sFangPhasing, m_sFangIntro;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_shadowfang_keep_InstanceMapScript(map);
        }
};

void AddSC_instance_shadowfang_keep()
{
    new instance_shadowfang_keep();
}