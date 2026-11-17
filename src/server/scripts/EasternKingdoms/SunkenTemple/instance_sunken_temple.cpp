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
SDName: Instance_Sunken_Temple
SD%Complete: 100
SDComment:Place Holder
SDCategory: Sunken Temple
EndScriptData */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "sunken_temple.h"

class instance_sunken_temple : public InstanceMapScript
{
    public:
        instance_sunken_temple() : InstanceMapScript("instance_sunken_temple", 109) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_sunken_temple_InstanceMapScript(map);
        }

        struct instance_sunken_temple_InstanceMapScript : public InstanceScript
        {
            instance_sunken_temple_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 State, m_wCount;
            std::map<uint32, ObjectGuid> m_SunkenEncounters;
            uint32 m_auiEncounter[EncounterCount];

            void Initialize() override
            {
                SetBossNumber(EncounterCount);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
                State           = 0;
                m_wCount        = 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_AVATAR_OF_HAKKAR:
                    case NPC_JAMMALAN_PROPHET: 
                    case NPC_WEAVER:
                    case NPC_DREAMSCYTHE:
                    case NPC_MORPHAZ:
                    case NPC_HAZZAS:
                    case NPC_SHADE_OF_ERANIKUS:
                        m_SunkenEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
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
                if (type == DATA_WARDENS && data == DONE)
                {
                    m_wCount++;
                    SaveToDB();

                    if (m_wCount >= SPECIAL)
                    {
                        SaveToDB();
                        SetBossState(DATA_WARDENS, DONE);
                    }
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_WARDENS)
                    return m_wCount;

                return 0;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S T " << GetBossSaveData() << m_wCount;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_AVATAR_OF_HAKKAR:
                    case NPC_JAMMALAN_PROPHET:
                    case NPC_WEAVER:
                    case NPC_DREAMSCYTHE:
                    case NPC_MORPHAZ:
                    case NPC_HAZZAS:
                    case NPC_SHADE_OF_ERANIKUS:
                        return m_SunkenEncounters.find(type)->second;
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

                if (dataHead1 == 'S' && dataHead2 == 'T')
                {
                    for (uint32 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // wardens count complete
                    m_wCount = temp;
                    SetData(DATA_WARDENS, m_wCount);
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };
};

void AddSC_instance_sunken_temple()
{
    new instance_sunken_temple();
}
