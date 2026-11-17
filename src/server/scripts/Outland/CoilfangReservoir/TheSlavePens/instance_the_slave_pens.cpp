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

/*
This placeholder for the instance is needed for dungeon finding to be able
to give credit after the boss defined in lastEncounterDungeon is killed.
Without it, the party doing random dungeon won't get satchel of spoils and
gets instead the deserter debuff.
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "the_slave_pens.h"

class instance_the_slave_pens : public InstanceMapScript
{
    public:
        instance_the_slave_pens() : InstanceMapScript("instance_the_slave_pens", 547) { }

        struct instance_the_slave_pens_InstanceMapScript : public InstanceScript
        {
            instance_the_slave_pens_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_DATA];
            uint64 mennuGUID, RokmarGUID, QuagmirranGUID;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                SetBossNumber(MAX_DATA);

                mennuGUID = ObjectGuid::Empty;
                RokmarGUID = ObjectGuid::Empty;
                QuagmirranGUID = ObjectGuid::Empty;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_MENNU_THE_BETRAYER:
                        mennuGUID = creature->GetGUID();
                        break;
                    case NPC_ROKMAR_THE_CRACKLER:
                        RokmarGUID = creature->GetGUID();
                        break;
                    case NPC_QUAGMIRRAN:
                        QuagmirranGUID = creature->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            std::string GetSaveData() override
            {
                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2];

                return saveStream.str();
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

                for (uint8 i = 0; i < MAX_DATA; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_the_slave_pens_InstanceMapScript(map);
        }
};

void AddSC_instance_the_slave_pens()
{
    new instance_the_slave_pens();
}
