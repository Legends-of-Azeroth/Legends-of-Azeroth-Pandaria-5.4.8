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
#include "nexus.h"

#define NUMBER_OF_ENCOUNTERS      5

enum Factions
{
    FACTION_HOSTILE_FOR_ALL                       = 16
};

class instance_nexus : public InstanceMapScript
{
    public:
        instance_nexus() : InstanceMapScript("instance_nexus", 576) { }

        struct instance_nexus_InstanceMapScript : public InstanceScript
        {
            instance_nexus_InstanceMapScript(Map *map) : InstanceScript(map) { }

            uint32 m_auiEncounter[NUMBER_OF_ENCOUNTERS];

            ObjectGuid anomalusGUID;
            ObjectGuid keristraszaGUID;
            ObjectGuid commanderGUID;

            ObjectGuid AnomalusContainmentSphere;
            ObjectGuid OrmoroksContainmentSphere;
            ObjectGuid TelestrasContainmentSphere;

            std::string strInstData;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                anomalusGUID = ObjectGuid::Empty;
                keristraszaGUID = ObjectGuid::Empty;
                commanderGUID = ObjectGuid::Empty;

                AnomalusContainmentSphere = ObjectGuid::Empty;
                OrmoroksContainmentSphere = ObjectGuid::Empty;
                TelestrasContainmentSphere = ObjectGuid::Empty;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                Map::PlayerList const& players = instance->GetPlayers();
                uint32 TeamInInstance = 0;

                if (!players.isEmpty())
                {
                    if (Player* player = players.begin()->GetSource())
                        TeamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_ANOMALUS:
                        anomalusGUID = creature->GetGUID();
                        break;
                    case NPC_KERISTRASZA:
                        keristraszaGUID = creature->GetGUID();
                        break;
                    // Alliance npcs are spawned by default, if you are alliance, you will fight against horde npcs.
                    case NPC_ALLIANCE_BERSERKER:
                    {
                        if (ServerAllowsTwoSideGroups())
                            creature->SetFaction(FACTION_HOSTILE_FOR_ALL);
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HORDE_BERSERKER, HORDE);
                        break;
                    }
                    case NPC_ALLIANCE_RANGER:
                    {
                        if (ServerAllowsTwoSideGroups())
                            creature->SetFaction(FACTION_HOSTILE_FOR_ALL);
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HORDE_RANGER, HORDE);
                        break;
                    }
                    case NPC_ALLIANCE_CLERIC:
                    {
                        if (ServerAllowsTwoSideGroups())
                            creature->SetFaction(FACTION_HOSTILE_FOR_ALL);
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HORDE_CLERIC, HORDE);
                        break;
                    }
                    case NPC_ALLIANCE_COMMANDER:
                    {
                        if (ServerAllowsTwoSideGroups())
                            creature->SetFaction(FACTION_HOSTILE_FOR_ALL);
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HORDE_COMMANDER, HORDE);
                        break;
                    }
                    case NPC_COMMANDER_STOUTBEARD:
                    {
                        if (ServerAllowsTwoSideGroups())
                            creature->SetFaction(FACTION_HOSTILE_FOR_ALL);
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_COMMANDER_KOLURG, HORDE);
                        commanderGUID = creature->GetGUID();
                        break;
                    }
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_ANOMALUS_CONTAINMENT_SPHERE:
                    {
                        AnomalusContainmentSphere = go->GetGUID();
                        if (m_auiEncounter[1] == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case GO_ORMOROKS_CONTAINMENT_SPHERE:
                    {
                        OrmoroksContainmentSphere = go->GetGUID();
                        if (m_auiEncounter[2] == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    case GO_TELESTRAS_CONTAINMENT_SPHERE:
                    {
                        TelestrasContainmentSphere = go->GetGUID();
                        if (m_auiEncounter[0] == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    }
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_COMMANDER:            return m_auiEncounter[0];
                    case DATA_MAGUS_TELESTRA_EVENT: return m_auiEncounter[0];
                    case DATA_ANOMALUS_EVENT:       return m_auiEncounter[1];
                    case DATA_ORMOROK_EVENT:        return m_auiEncounter[2];
                    case DATA_KERISTRASZA_EVENT:    return m_auiEncounter[3];
                }
                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_COMMANDER:
                        m_auiEncounter[0] = data;
                        break;
                    case DATA_MAGUS_TELESTRA_EVENT:
                    {
                        if (data == DONE)
                        {
                            GameObject* Sphere = instance->GetGameObject(TelestrasContainmentSphere);
                            if (Sphere)
                                Sphere->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        m_auiEncounter[1] = data;
                        break;
                    }
                    case DATA_ANOMALUS_EVENT:
                    {
                        if (data == DONE)
                        {
                            if (GameObject* Sphere = instance->GetGameObject(AnomalusContainmentSphere))
                                Sphere->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        m_auiEncounter[2] = data;
                        break;
                    }
                    case DATA_ORMOROK_EVENT:
                    {
                        if (data == DONE)
                        {
                            if (GameObject* Sphere = instance->GetGameObject(OrmoroksContainmentSphere))
                                Sphere->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        m_auiEncounter[3] = data;
                        break;
                    }
                    case DATA_KERISTRASZA_EVENT:
                        m_auiEncounter[4] = data;
                        break;
                }

                if (data == DONE)
                {
                    OUT_SAVE_INST_DATA;

                    std::ostringstream saveStream;
                    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                        << m_auiEncounter[3] << " " << m_auiEncounter[4];

                    strInstData = saveStream.str();

                    SaveToDB();
                    OUT_SAVE_INST_DATA_COMPLETE;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_COMMANDER:                return commanderGUID;
                    case DATA_ANOMALUS:                 return anomalusGUID;
                    case DATA_KERISTRASZA:              return keristraszaGUID;
                    case ANOMALUS_CONTAINMET_SPHERE:    return AnomalusContainmentSphere;
                    case ORMOROKS_CONTAINMET_SPHERE:    return OrmoroksContainmentSphere;
                    case TELESTRAS_CONTAINMET_SPHERE:   return TelestrasContainmentSphere;
                }
                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                return strInstData;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                std::istringstream loadStream(in);
                loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4];

                for (uint8 i = 0; i < NUMBER_OF_ENCOUNTERS; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_nexus_InstanceMapScript(map);
        }
};

void AddSC_instance_nexus()
{
    new instance_nexus();
}
