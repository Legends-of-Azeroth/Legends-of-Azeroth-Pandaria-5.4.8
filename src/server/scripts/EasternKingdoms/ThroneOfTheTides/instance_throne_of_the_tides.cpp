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
#include "throne_of_the_tides.h"

#define MAX_ENCOUNTER 4

static std::vector<DoorData> const doordata =
{
    {GO_LADY_NAZJAR_DOOR,         DATA_LADY_NAZJAR,        DOOR_TYPE_ROOM, BOUNDARY_NONE},
    {GO_LADY_NAZJAR_DOOR,         DATA_COMMANDER_ULTHOK,   DOOR_TYPE_ROOM, BOUNDARY_NONE},
    {GO_COMMANDER_ULTHOK_DOOR,    DATA_COMMANDER_ULTHOK,   DOOR_TYPE_ROOM, BOUNDARY_NONE},
    {GO_ERUNAK_STONESPEAKER_DOOR, DATA_MINDBENDER_GHURSHA, DOOR_TYPE_ROOM, BOUNDARY_NONE},
    {GO_OZUMAT_DOOR,              DATA_OZUMAT,             DOOR_TYPE_ROOM, BOUNDARY_NONE},
};

class instance_throne_of_the_tides : public InstanceMapScript
{
    public:
        instance_throne_of_the_tides() : InstanceMapScript("instance_throne_of_the_tides", 643) { }

        struct instance_throne_of_the_tides_InstanceMapScript : public InstanceScript
        {
            instance_throne_of_the_tides_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);
                uiLadyNazjarGUID = ObjectGuid::Empty;
                uiCommanderUlthokGUID = ObjectGuid::Empty;
                uiErunakStonespeakerGUID = ObjectGuid::Empty;
                uiMindbenderGhurshaGUID = ObjectGuid::Empty;
                uiOzumatGUID = ObjectGuid::Empty;
                uiNeptulonGUID = ObjectGuid::Empty;
                uiLadyNazjarEventGUID = ObjectGuid::Empty;

                uiCoralesGUID = ObjectGuid::Empty;
                uiLadyNazjarDoorGUID = ObjectGuid::Empty;
                uiCommanderUlthokDoorGUID = ObjectGuid::Empty;
                uiMindebenderGhurshaDoorGUID = ObjectGuid::Empty;
                uiOzumatDoorGUID = ObjectGuid::Empty;
                uiControlSystemGUID = ObjectGuid::Empty;
                uiTentacleRightGUID = ObjectGuid::Empty;
                uiTentacleLeftGUID = ObjectGuid::Empty;
                uiInvisibleDoor1GUID = ObjectGuid::Empty;
                uiInvisibleDoor2GUID = ObjectGuid::Empty;
                uiNeptulonCache = ObjectGuid::Empty;

                archaeologyQuestAura = 0;

                memset(m_uiEvents, 0, sizeof(m_uiEvents));
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!uiTeamInInstance)
                    uiTeamInInstance = player->GetTeam();

                if (archaeologyQuestAura)
                    if (!player->HasAura(archaeologyQuestAura))
                        player->CastSpell(player, archaeologyQuestAura, true);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!uiTeamInInstance)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            uiTeamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_LADY_NAZJAR_EVENT:
                        uiLadyNazjarEventGUID = creature->GetGUID();
                        break;
                    case NPC_LADY_NAZJAR:
                        uiLadyNazjarGUID = creature->GetGUID();
                        break;
                    case NPC_COMMANDER_ULTHOK:
                        uiCommanderUlthokGUID = creature->GetGUID();
                        TC_LOG_ERROR("scripts", "ulthok1");
                        if (GetData(DATA_COMMANDER_ULTHOK_EVENT) == DONE)
                        {
                            TC_LOG_ERROR("scripts", "ulthok2");
                            creature->SetPhaseMask(PHASEMASK_NORMAL, true);
                        }
                        else
                        {
                            TC_LOG_ERROR("scripts", "ulthok3");
                            creature->SetPhaseMask(2, true);
                        }
                        break;
                    case NPC_ERUNAK_STONESPEAKER:
                        uiErunakStonespeakerGUID = creature->GetGUID();
                        break;
                    case NPC_MINDBENDER_GHURSHA:
                        uiMindbenderGhurshaGUID = creature->GetGUID();
                        break;
                    case NPC_OZUMAT:
                        uiOzumatGUID = creature->GetGUID();
                        break;
                    case NPC_NEPTULON:
                        uiNeptulonGUID = creature->GetGUID();
                        break;
                    case NPC_CAPTAIN_TAYLOR:
                        if (uiTeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_LEGIONNAIRE_NAZGRIM);
                        break;
                    case NPC_LEGIONNAIRE_NAZGRIM:
                        if (uiTeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_CAPTAIN_TAYLOR);
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_CORALES:
                        uiCoralesGUID = go->GetGUID();
                        if (GetBossState(DATA_LADY_NAZJAR) == DONE)
                        {
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            go->SetPhaseMask(2, true);
                        }
                        break;
                    case GO_LADY_NAZJAR_DOOR:
                        uiLadyNazjarDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_COMMANDER_ULTHOK_DOOR:
                        uiCommanderUlthokDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_ERUNAK_STONESPEAKER_DOOR:
                        uiMindebenderGhurshaDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_OZUMAT_DOOR:
                        uiOzumatDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_CONTROL_SYSTEM:
                        uiControlSystemGUID = go->GetGUID();
                        break;
                    case GO_TENTACLE_RIGHT:
                        uiTentacleRightGUID = go->GetGUID();
                        if (GetBossState(DATA_COMMANDER_ULTHOK) == DONE)
                            go->SetPhaseMask(2, true);
                        break;
                    case GO_TENTACLE_LEFT:
                        uiTentacleLeftGUID = go->GetGUID();
                        if (GetBossState(DATA_COMMANDER_ULTHOK) == DONE)
                            go->SetPhaseMask(2, true);
                        break;
                    case GO_INVISIBLE_DOOR_1:
                        uiInvisibleDoor1GUID = go->GetGUID();
                        if (GetBossState(DATA_COMMANDER_ULTHOK) == DONE)
                            go->SetPhaseMask(2, true);
                        break;
                    case GO_INVISIBLE_DOOR_2:
                        uiInvisibleDoor2GUID = go->GetGUID();
                        if (GetBossState(DATA_COMMANDER_ULTHOK) == DONE)
                            go->SetPhaseMask(2, true);
                        break;
                    case GO_NEPTULON_CACHE:
                    case GO_NEPTULON_CACHE_H:
                        uiNeptulonCache = go->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_LADY_NAZJAR_EVENT:
                        m_uiEvents[0] = data;
                        break;
                    case DATA_COMMANDER_ULTHOK_EVENT:
                        m_uiEvents[1] = data;
                        break;
                    case DATA_NEPTULON_EVENT:
                        m_uiEvents[2] = data;
                        break;
                    case uint32(-1):
                        archaeologyQuestAura = data;
                        SaveToDB();
                        break;
                }
                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_LADY_NAZJAR_EVENT:
                        return m_uiEvents[0];
                    case DATA_COMMANDER_ULTHOK_EVENT:
                        return m_uiEvents[1];
                    case DATA_NEPTULON_EVENT:
                        return m_uiEvents[2];
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_LADY_NAZJAR:
                        return uiLadyNazjarGUID;
                    case DATA_COMMANDER_ULTHOK:
                        return uiCommanderUlthokGUID;
                    case DATA_ERUNAK_STONESPEAKER:
                        return uiErunakStonespeakerGUID;
                    case DATA_MINDBENDER_GHURSHA:
                        return uiMindbenderGhurshaGUID;
                    case DATA_OZUMAT:
                        return uiOzumatGUID;
                    case DATA_NEPTULON:
                        return uiNeptulonGUID;
                    case DATA_LADY_NAZJAR_EVENT:
                        return uiLadyNazjarEventGUID;
                    case DATA_CORALES:
                        return uiCoralesGUID;
                }
                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_LADY_NAZJAR:
                        break;
                    case DATA_COMMANDER_ULTHOK:
                        if (state == DONE)
                        {
                            if (GameObject* pTentacleRight = instance->GetGameObject(uiTentacleRightGUID))
                                pTentacleRight->SetPhaseMask(2, true);
                            if (GameObject* pTentacleLeft = instance->GetGameObject(uiTentacleLeftGUID))
                                pTentacleLeft->SetPhaseMask(2, true);
                            if (GameObject* pInvisibleDoor1 = instance->GetGameObject(uiInvisibleDoor1GUID))
                                pInvisibleDoor1->SetPhaseMask(2, true);
                            if (GameObject* pInvisibleDoor2 = instance->GetGameObject(uiInvisibleDoor2GUID))
                                pInvisibleDoor2->SetPhaseMask(2, true);
                        }
                        break;
                    case DATA_MINDBENDER_GHURSHA:
                        break;
                    case DATA_OZUMAT:
                        if (state == DONE)
                            DoRespawnGameObject(uiNeptulonCache, DAY);
                        break;
                 }
                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "T o t T " << GetBossSaveData() << m_uiEvents[0] << " " << m_uiEvents[1] << " " << m_uiEvents[2] << " " << archaeologyQuestAura;

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in) override
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

                if (dataHead1 == 'T' && dataHead2 == 'o' && dataHead3 == 't' && dataHead4 == 'T')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        uint32 tmpEvent;
                        loadStream >> tmpEvent;
                        if (tmpEvent == IN_PROGRESS || tmpEvent > SPECIAL)
                        tmpEvent = NOT_STARTED;
                        m_uiEvents[i] = tmpEvent;
                    }
                    loadStream >> archaeologyQuestAura;
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                ObjectGuid uiLadyNazjarGUID;
                ObjectGuid uiCommanderUlthokGUID;
                ObjectGuid uiErunakStonespeakerGUID;
                ObjectGuid uiMindbenderGhurshaGUID;
                ObjectGuid uiOzumatGUID;
                ObjectGuid uiNeptulonGUID;
                ObjectGuid uiLadyNazjarEventGUID;
                ObjectGuid uiCoralesGUID;
                ObjectGuid uiLadyNazjarDoorGUID;
                ObjectGuid uiCommanderUlthokDoorGUID;
                ObjectGuid uiMindebenderGhurshaDoorGUID;
                ObjectGuid uiOzumatDoorGUID;
                ObjectGuid uiControlSystemGUID;
                ObjectGuid uiTentacleRightGUID;
                ObjectGuid uiTentacleLeftGUID;
                ObjectGuid uiInvisibleDoor1GUID;
                ObjectGuid uiInvisibleDoor2GUID;
                ObjectGuid uiNeptulonCache;
                uint32 uiTeamInInstance;
                uint32 archaeologyQuestAura;
                uint32 m_uiEvents[3];
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_throne_of_the_tides_InstanceMapScript(map);
        }
};

void AddSC_instance_throne_of_the_tides()
{
    new instance_throne_of_the_tides();
}
