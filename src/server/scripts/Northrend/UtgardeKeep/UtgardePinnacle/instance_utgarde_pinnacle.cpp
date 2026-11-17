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
#include "utgarde_pinnacle.h"

#define MAX_ENCOUNTER     4

/* Utgarde Pinnacle encounters:
 0 - Svala Sorrowgrave
 1 - Gortok Palehoof
 2 - Skadi the Ruthless
 3 - King Ymiron
*/

enum GameObjects
{
    GO_SKADI_THE_RUTHLESS_DOOR                 = 192173,
    GO_KING_YMIRON_DOOR                        = 192174,
    GO_GORK_PALEHOOF_SPHERE                    = 188593
};

class instance_utgarde_pinnacle : public InstanceMapScript
{
    public:
        instance_utgarde_pinnacle() : InstanceMapScript("instance_utgarde_pinnacle", 575) { }

        struct instance_pinnacle : public InstanceScript
        {
            instance_pinnacle(Map* map) : InstanceScript(map) { }

            ObjectGuid uiSvalaSorrowgrave;
            ObjectGuid uiGortokPalehoof;
            ObjectGuid uiSkadiTheRuthless;
            ObjectGuid uiGrauf;
            ObjectGuid uiKingYmiron;

            ObjectGuid uiSkadiTheRuthlessDoor;
            ObjectGuid uiKingYmironDoor;
            ObjectGuid uiGortokPalehoofSphere;

            ObjectGuid uiFrenziedWorgen;
            ObjectGuid uiRavenousFurbolg;
            ObjectGuid uiFerociousRhino;
            ObjectGuid uiMassiveJormungar;
            ObjectGuid uiPalehoofOrb;

            ObjectGuid uiSvala;
            ObjectGuid uiFlameBrazier_1;
            ObjectGuid uiFlameBrazier_2;

            uint32 m_auiEncounter[MAX_ENCOUNTER];
        
            ObjectGuid uiDoodad_Utgarde_Mirror_FX01;

            std::string str_data;

            void Initialize() override
            {
                SetBossNumber(MAX_ENCOUNTER);
                uiFlameBrazier_1 = ObjectGuid::Empty;
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                   m_auiEncounter[i] = NOT_STARTED;
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SVALA_SORROWGRAVE:
                        uiSvalaSorrowgrave = creature->GetGUID();
                        break;
                    case NPC_GORTOK_PALEHOOF:
                        uiGortokPalehoof = creature->GetGUID();
                        break;
                    case NPC_SKADI_RUTHLESS:
                        uiSkadiTheRuthless = creature->GetGUID();
                        break;
                    case NPC_GRAUF:
                        uiGrauf = creature->GetGUID();
                        break;
                    case NPC_KING_YMIRON:
                        uiKingYmiron = creature->GetGUID();
                        if (GetData(DATA_SKADI_THE_RUTHLESS_EVENT) == DONE)
                        {
                            creature->SetReactState(REACT_AGGRESSIVE);
                            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        else
                        {
                            creature->SetReactState(REACT_PASSIVE);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        break;
                    case NPC_FRENZIED_WORGEN:
                        uiFrenziedWorgen = creature->GetGUID();
                        break;
                    case NPC_RAVENOUS_FURBOLG: 
                        uiRavenousFurbolg = creature->GetGUID();
                        break;
                    case NPC_MASSIVE_JORMUNGAR:
                        uiMassiveJormungar = creature->GetGUID();
                        break;
                    case NPC_FEROCIOUS_RHINO: 
                        uiFerociousRhino = creature->GetGUID();
                        break;
                    case NPC_SVALA: 
                        uiSvala = creature->GetGUID();
                        break;
                    case NPC_PALEHOOF_ORB:
                        uiPalehoofOrb = creature->GetGUID();
                        break;
                    case NPC_FLAME_BRAZIER:
                        (uiFlameBrazier_1 ? uiFlameBrazier_2 : uiFlameBrazier_1) = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_SKADI_THE_RUTHLESS_DOOR:
                        uiSkadiTheRuthlessDoor = go->GetGUID();
                        if (m_auiEncounter[2] == DONE) HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_KING_YMIRON_DOOR:
                        uiKingYmironDoor = go->GetGUID();
                        if (m_auiEncounter[3] == DONE) HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GORK_PALEHOOF_SPHERE:
                        uiGortokPalehoofSphere = go->GetGUID();
                        if (m_auiEncounter[1] == DONE)
                        {
                            HandleGameObject(ObjectGuid::Empty, true, go);
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        break;
                    case 191745:
                        uiDoodad_Utgarde_Mirror_FX01 = go->GetGUID();
                        break;
                }
            }

            // Dirty hack, I don't feel like rewriting this for all the bosses
            bool SetBossState(uint32 id, EncounterState state) override
            {
                if (id == DATA_SKADI_THE_RUTHLESS)
                {
                    SetData(DATA_SKADI_THE_RUTHLESS_EVENT, state);
                    return true;
                }

                ASSERT(false);
                return false;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_SVALA_SORROWGRAVE_EVENT:
                        m_auiEncounter[0] = data;
                        break;
                    case DATA_GORTOK_PALEHOOF_EVENT:
                        m_auiEncounter[1] = data;
                        break;
                    case DATA_SKADI_THE_RUTHLESS_EVENT:
                        if (data == DONE)
                        {
                            HandleGameObject(uiSkadiTheRuthlessDoor, true);
                            if (Creature* creature = instance->GetCreature(uiKingYmiron))
                            {
                                creature->SetReactState(REACT_AGGRESSIVE);
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            }
                        }
                        m_auiEncounter[2] = data;
                        InstanceScript::SetBossState(DATA_SKADI_THE_RUTHLESS, (EncounterState)data);
                        break;
                    case DATA_KING_YMIRON_EVENT:
                        if (data == DONE)
                            HandleGameObject(uiKingYmironDoor, true);
                        m_auiEncounter[3] = data;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SVALA_SORROWGRAVE_EVENT:        return m_auiEncounter[0];
                    case DATA_GORTOK_PALEHOOF_EVENT:          return m_auiEncounter[1];
                    case DATA_SKADI_THE_RUTHLESS_EVENT:       return m_auiEncounter[2];
                    case DATA_KING_YMIRON_EVENT:              return m_auiEncounter[3];
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SVALA_SORROWGRAVE:           return uiSvalaSorrowgrave;
                    case DATA_GORTOK_PALEHOOF:             return uiGortokPalehoof;
                    case DATA_SKADI_THE_RUTHLESS:          return uiSkadiTheRuthless;
                    case DATA_GRAUF:                       return uiGrauf;
                    case DATA_KING_YMIRON:                 return uiKingYmiron;
                    case DATA_NPC_FRENZIED_WORGEN:         return uiFrenziedWorgen;
                    case DATA_NPC_RAVENOUS_FURBOLG:        return uiRavenousFurbolg;
                    case DATA_NPC_MASSIVE_JORMUNGAR:       return uiMassiveJormungar;
                    case DATA_NPC_FEROCIOUS_RHINO:         return uiFerociousRhino;
                    case DATA_NPC_ORB:                     return uiPalehoofOrb;
                    case DATA_SVALA:                       return uiSvala;
                    case DATA_GORTOK_PALEHOOF_SPHERE:      return uiGortokPalehoofSphere;
                    case DATA_FLAME_BRAZIER_1:             return uiFlameBrazier_1;
                    case DATA_FLAME_BRAZIER_2:             return uiFlameBrazier_2;
                    case DATA_DOODAD_UTGARDE_MIRROR_FX01:  return uiDoodad_Utgarde_Mirror_FX01;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "U P " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                    << m_auiEncounter[2] << " " << m_auiEncounter[3];

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

                char dataHead1, dataHead2;
                uint16 data0, data1, data2, data3;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

                if (dataHead1 == 'U' && dataHead2 == 'P')
                {
                    m_auiEncounter[0] = data0;
                    m_auiEncounter[1] = data1;
                    m_auiEncounter[2] = data2;
                    m_auiEncounter[3] = data3;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)
                            m_auiEncounter[i] = NOT_STARTED;

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_pinnacle(map);
        }
};

void AddSC_instance_utgarde_pinnacle()
{
    new instance_utgarde_pinnacle();
}
