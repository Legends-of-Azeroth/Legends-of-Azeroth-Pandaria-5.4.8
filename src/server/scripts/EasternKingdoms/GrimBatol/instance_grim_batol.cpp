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

#include "grim_batol.h"
#include "ScriptPCH.h"

#define MAX_ENCOUNTER 4

class instance_grim_batol : public InstanceMapScript
{
    public:
        instance_grim_batol() : InstanceMapScript("instance_grim_batol", 670) { }

        struct instance_grim_batol_InstanceMapScript : public InstanceScript
        {
            instance_grim_batol_InstanceMapScript(InstanceMap *map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);
                    
                uiGeneralUmbrissGUID = ObjectGuid::Empty;
                uiForgemasterThrongusGUID = ObjectGuid::Empty;
                uiDrahgaShadowburnerGUID = ObjectGuid::Empty;
                uiErudaxGUID = ObjectGuid::Empty;
                archaeologyQuestAura = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (archaeologyQuestAura)
                    if (!player->HasAura(archaeologyQuestAura))
                        player->CastSpell(player, archaeologyQuestAura, true);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch(creature->GetEntry())
                {
                    case NPC_GENERAL_UMBRISS:
                        uiGeneralUmbrissGUID = creature->GetGUID();
                        break;
                    case NPC_FORGEMASTER_THRONGUS:
                        uiForgemasterThrongusGUID = creature->GetGUID();
                        break;
                    case NPC_DRAHGA_SHADOWBURNER:
                        uiDrahgaShadowburnerGUID = creature->GetGUID();
                        break;
                    case NPC_ERUDAX:
                        uiErudaxGUID = creature->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == uint32(-1))
                {
                    archaeologyQuestAura = data;
                    SaveToDB();
                    return;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const  override
            {
                switch (type)
                {
                    case DATA_GENERAL_UMBRISS:
                        return uiGeneralUmbrissGUID;
                    case DATA_FORGEMASTER_THRONGUS:
                        return uiForgemasterThrongusGUID;
                    case DATA_DRAHGA_SHADOWBURNER:
                        return uiDrahgaShadowburnerGUID;
                    case DATA_ERUDAX:
                        return uiErudaxGUID;
                }
                return ObjectGuid::Empty;
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

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "G B " << GetBossSaveData() << archaeologyQuestAura;

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

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'G' && dataHead2 == 'B')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    loadStream >> archaeologyQuestAura;
                }
                else 
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                ObjectGuid uiGeneralUmbrissGUID;
                ObjectGuid uiForgemasterThrongusGUID;
                ObjectGuid uiDrahgaShadowburnerGUID;
                ObjectGuid uiErudaxGUID;
                uint32 archaeologyQuestAura;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_grim_batol_InstanceMapScript(map);
        }
};

void AddSC_instance_grim_batol()
{
    new instance_grim_batol();
}
