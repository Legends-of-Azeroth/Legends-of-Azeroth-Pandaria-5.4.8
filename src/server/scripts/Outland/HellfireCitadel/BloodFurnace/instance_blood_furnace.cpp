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

/* ScriptData
SDName: Instance_Blood_Furnace
SD%Complete: 85
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "blood_furnace.h"
#include "CreatureAI.h"

#define ENTRY_SEWER1                 181823
#define ENTRY_SEWER2                 181766
#define MAX_ENCOUNTER                   3

class instance_blood_furnace : public InstanceMapScript
{
    public:
        instance_blood_furnace()
            : InstanceMapScript("instance_blood_furnace", 542) { }

        struct instance_blood_furnace_InstanceMapScript : public InstanceScript
        {
            instance_blood_furnace_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 teamInInstance;

            ObjectGuid The_MakerGUID;
            ObjectGuid BroggokGUID;
            ObjectGuid Kelidan_The_BreakerGUID;

            ObjectGuid Door1GUID;
            ObjectGuid Door2GUID;
            ObjectGuid Door3GUID;
            ObjectGuid Door4GUID;
            ObjectGuid Door5GUID;
            ObjectGuid Door6GUID;

            ObjectGuid PrisonCell1GUID;
            ObjectGuid PrisonCell2GUID;
            ObjectGuid PrisonCell3GUID;
            ObjectGuid PrisonCell4GUID;
            ObjectGuid PrisonCell5GUID;
            ObjectGuid PrisonCell6GUID;
            ObjectGuid PrisonCell7GUID;
            ObjectGuid PrisonCell8GUID;

            std::set<ObjectGuid> PrisonersCell5;
            std::set<ObjectGuid> PrisonersCell6;
            std::set<ObjectGuid> PrisonersCell7;
            std::set<ObjectGuid> PrisonersCell8;

            uint8 PrisonerCounter5;
            uint8 PrisonerCounter6;
            uint8 PrisonerCounter7;
            uint8 PrisonerCounter8;

            ObjectGuid BroggokLeverGUID;

            uint32 m_auiEncounter[MAX_ENCOUNTER];
            std::string str_data;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                The_MakerGUID = ObjectGuid::Empty;
                BroggokGUID = ObjectGuid::Empty;
                Kelidan_The_BreakerGUID = ObjectGuid::Empty;

                Door1GUID = ObjectGuid::Empty;
                Door2GUID = ObjectGuid::Empty;
                Door3GUID = ObjectGuid::Empty;
                Door4GUID = ObjectGuid::Empty;
                Door5GUID = ObjectGuid::Empty;
                Door6GUID = ObjectGuid::Empty;

                PrisonCell1GUID = ObjectGuid::Empty;
                PrisonCell2GUID = ObjectGuid::Empty;
                PrisonCell3GUID = ObjectGuid::Empty;
                PrisonCell4GUID = ObjectGuid::Empty;
                PrisonCell5GUID = ObjectGuid::Empty;
                PrisonCell6GUID = ObjectGuid::Empty;
                PrisonCell7GUID = ObjectGuid::Empty;
                PrisonCell8GUID = ObjectGuid::Empty;

                PrisonersCell5.clear();
                PrisonersCell6.clear();
                PrisonersCell7.clear();
                PrisonersCell8.clear();

                PrisonerCounter5 = 0;
                PrisonerCounter6 = 0;
                PrisonerCounter7 = 0;
                PrisonerCounter8 = 0;

                BroggokLeverGUID = ObjectGuid::Empty;
                teamInInstance   = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!teamInInstance)
                    teamInInstance = player->GetTeam();
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case 17381:
                        The_MakerGUID = creature->GetGUID();
                        break;
                    case 17380:
                        BroggokGUID = creature->GetGUID();
                        break;
                    case 17377:
                        Kelidan_The_BreakerGUID = creature->GetGUID();
                        break;
                    case 17398:
                        StorePrisoner(creature);
                        break;
                    case NPC_THRALLMAR_SCRYER:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HONOR_HOLD_ARCHER, ALLIANCE);
                        break;
                    case NPC_CAZAREZ:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_GUNNY, ALLIANCE);
                        break;
                    case NPC_THRALLMAR_INVADER:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_HONOR_HOLD_RECON, ALLIANCE);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->GetTypeId() == TYPEID_UNIT && unit->GetEntry() == 17398)
                    PrisonerDied(unit->GetGUID());
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                 if (go->GetEntry() == 181766)                //Final exit door
                     Door1GUID = go->GetGUID();
                 if (go->GetEntry() == 181811)               //The Maker Front door
                     Door2GUID = go->GetGUID();
                 if (go->GetEntry() == 181812)                //The Maker Rear door
                     Door3GUID = go->GetGUID();
                 if (go->GetEntry() == 181822)               //Broggok Front door
                     Door4GUID = go->GetGUID();
                 if (go->GetEntry() == 181819)               //Broggok Rear door
                     Door5GUID = go->GetGUID();
                 if (go->GetEntry() == 181823)               //Kelidan exit door
                     Door6GUID = go->GetGUID();

                 if (go->GetEntry() == 181813)               //The Maker prison cell front right
                     PrisonCell1GUID = go->GetGUID();
                 if (go->GetEntry() == 181814)               //The Maker prison cell back right
                     PrisonCell2GUID = go->GetGUID();
                 if (go->GetEntry() == 181816)               //The Maker prison cell front left
                     PrisonCell3GUID = go->GetGUID();
                 if (go->GetEntry() == 181815)               //The Maker prison cell back left
                     PrisonCell4GUID = go->GetGUID();
                 if (go->GetEntry() == 181821)               //Broggok prison cell front right
                     PrisonCell5GUID = go->GetGUID();
                 if (go->GetEntry() == 181818)               //Broggok prison cell back right
                     PrisonCell6GUID = go->GetGUID();
                 if (go->GetEntry() == 181820)               //Broggok prison cell front left
                     PrisonCell7GUID = go->GetGUID();
                 if (go->GetEntry() == 181817)               //Broggok prison cell back left
                     PrisonCell8GUID = go->GetGUID();

                 if (go->GetEntry() == 181982)
                     BroggokLeverGUID = go->GetGUID();       //Broggok lever
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                     case DATA_THE_MAKER:            return The_MakerGUID;
                     case DATA_BROGGOK:              return BroggokGUID;
                     case DATA_KELIDAN_THE_MAKER:    return Kelidan_The_BreakerGUID;
                     case DATA_DOOR1:                return Door1GUID;
                     case DATA_DOOR2:                return Door2GUID;
                     case DATA_DOOR3:                return Door3GUID;
                     case DATA_DOOR4:                return Door4GUID;
                     case DATA_DOOR5:                return Door5GUID;
                     case DATA_DOOR6:                return Door6GUID;
                     case DATA_PRISON_CELL1:         return PrisonCell1GUID;
                     case DATA_PRISON_CELL2:         return PrisonCell2GUID;
                     case DATA_PRISON_CELL3:         return PrisonCell3GUID;
                     case DATA_PRISON_CELL4:         return PrisonCell4GUID;
                     case DATA_PRISON_CELL5:         return PrisonCell5GUID;
                     case DATA_PRISON_CELL6:         return PrisonCell6GUID;
                     case DATA_PRISON_CELL7:         return PrisonCell7GUID;
                     case DATA_PRISON_CELL8:         return PrisonCell8GUID;
                     case DATA_BROGGOK_LEVER:        return BroggokLeverGUID;
                }
                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                 switch (type)
                 {
                     case TYPE_THE_MAKER_EVENT:
                         m_auiEncounter[0] = data;
                         break;
                     case TYPE_BROGGOK_EVENT:
                         m_auiEncounter[1] = data;
                         UpdateBroggokEvent(data);
                         break;
                     case TYPE_KELIDAN_THE_BREAKER_EVENT:
                         m_auiEncounter[2] = data;
                         break;
                 }

                if (data == DONE)
                {
                    OUT_SAVE_INST_DATA;

                    std::ostringstream saveStream;
                    saveStream << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2];

                    str_data = saveStream.str();

                    SaveToDB();
                    OUT_SAVE_INST_DATA_COMPLETE;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_THE_MAKER_EVENT:             return m_auiEncounter[0];
                    case TYPE_BROGGOK_EVENT:               return m_auiEncounter[1];
                    case TYPE_KELIDAN_THE_BREAKER_EVENT:   return m_auiEncounter[2];
                }
                return 0;
            }

            const char* Save()
            {
                return str_data.c_str();
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
                loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS || m_auiEncounter[i] == FAIL)
                        m_auiEncounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void UpdateBroggokEvent(uint32 data)
            {
                switch (data)
                {
                    case IN_PROGRESS:
                        ActivateCell(DATA_PRISON_CELL5);
                        HandleGameObject(Door4GUID, false);
                        break;
                    case NOT_STARTED:
                        ResetPrisons();
                        HandleGameObject(Door5GUID, false);
                        HandleGameObject(Door4GUID, true);
                        if (GameObject* lever = instance->GetGameObject(BroggokLeverGUID))
                            lever->Respawn();
                        break;
                }
            }

            void ResetPrisons()
            {
                PrisonerCounter5 = PrisonersCell5.size();
                ResetPrisoners(PrisonersCell5);
                HandleGameObject(PrisonCell5GUID, false);

                PrisonerCounter6 = PrisonersCell6.size();
                ResetPrisoners(PrisonersCell6);
                HandleGameObject(PrisonCell6GUID, false);

                PrisonerCounter7 = PrisonersCell7.size();
                ResetPrisoners(PrisonersCell7);
                HandleGameObject(PrisonCell7GUID, false);

                PrisonerCounter8 = PrisonersCell8.size();
                ResetPrisoners(PrisonersCell8);
                HandleGameObject(PrisonCell8GUID, false);
            }

            void ResetPrisoners(const std::set<ObjectGuid>& prisoners)
            {
                for (std::set<ObjectGuid>::const_iterator i = prisoners.begin(); i != prisoners.end(); ++i)
                    if (Creature* prisoner = instance->GetCreature(*i))
                        ResetPrisoner(prisoner);
            }

            void ResetPrisoner(Creature* prisoner)
            {
                if (!prisoner->IsAlive())
                    prisoner->Respawn(true);
                prisoner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            }

            void StorePrisoner(Creature* creature)
            {
                float posX = creature->GetPositionX();
                float posY = creature->GetPositionY();

                if (posX >= 405.0f && posX <= 423.0f)
                {
                    if (posY >= 106.0f && posY <= 123.0f)
                    {
                        PrisonersCell5.insert(creature->GetGUID());
                        ++PrisonerCounter5;
                    }
                    else if (posY >= 76.0f && posY <= 91.0f)
                    {
                        PrisonersCell6.insert(creature->GetGUID());
                        ++PrisonerCounter6;
                    }
                    else return;
                }
                else if (posX >= 490.0f && posX <= 506.0f)
                {
                    if (posY >= 106.0f && posY <= 123.0f)
                    {
                        PrisonersCell7.insert(creature->GetGUID());
                        ++PrisonerCounter7;
                    }
                    else if (posY >= 76.0f && posY <= 91.0f)
                    {
                        PrisonersCell8.insert(creature->GetGUID());
                        ++PrisonerCounter8;
                    }
                    else
                        return;
                }
                else
                    return;

                ResetPrisoner(creature);
            }

            void PrisonerDied(ObjectGuid guid)
            {
                if (PrisonersCell5.find(guid) != PrisonersCell5.end() && --PrisonerCounter5 <= 0)
                    ActivateCell(DATA_PRISON_CELL6);
                else if (PrisonersCell6.find(guid) != PrisonersCell6.end() && --PrisonerCounter6 <= 0)
                    ActivateCell(DATA_PRISON_CELL7);
                else if (PrisonersCell7.find(guid) != PrisonersCell7.end() && --PrisonerCounter7 <= 0)
                    ActivateCell(DATA_PRISON_CELL8);
                else if (PrisonersCell8.find(guid) != PrisonersCell8.end() && --PrisonerCounter8 <= 0)
                    ActivateCell(DATA_DOOR5);
            }

            void ActivateCell(uint8 id)
            {
                switch (id)
                {
                    case DATA_PRISON_CELL5:
                        HandleGameObject(PrisonCell5GUID, true);
                        ActivatePrisoners(PrisonersCell5);
                        break;
                    case DATA_PRISON_CELL6:
                        HandleGameObject(PrisonCell6GUID, true);
                        ActivatePrisoners(PrisonersCell6);
                        break;
                    case DATA_PRISON_CELL7:
                        HandleGameObject(PrisonCell7GUID, true);
                        ActivatePrisoners(PrisonersCell7);
                        break;
                    case DATA_PRISON_CELL8:
                        HandleGameObject(PrisonCell8GUID, true);
                        ActivatePrisoners(PrisonersCell8);
                        break;
                    case DATA_DOOR5:
                        HandleGameObject(Door5GUID, true);
                        if (Creature* broggok = instance->GetCreature(BroggokGUID))
                            broggok->AI()->DoAction(ACTION_ACTIVATE_BROGGOK);
                        break;
                }
            }

            void ActivatePrisoners(const std::set<ObjectGuid>& prisoners)
            {
                for (std::set<ObjectGuid>::const_iterator i = prisoners.begin(); i != prisoners.end(); ++i)
                    if (Creature* prisoner = instance->GetCreature(*i))
                    {
                        prisoner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                        prisoner->SetInCombatWithZone();
                    }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_blood_furnace_InstanceMapScript(map);
        }
};

void AddSC_instance_blood_furnace()
{
    new instance_blood_furnace();
}

