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
#include "blackrock_caverns.h"

#define MAX_ENCOUNTER 5

class instance_blackrock_caverns : public InstanceMapScript
{
    public:
        instance_blackrock_caverns() : InstanceMapScript("instance_blackrock_caverns", 645) { }

        struct instance_blackrock_cavernsInstanceMapScript : public InstanceScript
        {
            instance_blackrock_cavernsInstanceMapScript(InstanceMap* map) : InstanceScript(map) 
            { 
                SetBossNumber(MAX_ENCOUNTER);

                 uiRomoggGUID = ObjectGuid::Empty;
                 uiCorlaGUID = ObjectGuid::Empty;
                 uiKarshGUID = ObjectGuid::Empty;
                 uiBeautyGUID = ObjectGuid::Empty;
                 uiRazCrazedGUID = ObjectGuid::Empty;
                 uiLordObsidiusGUID = ObjectGuid::Empty;
                 archaeologyQuestAura = 0;
                 memset(m_uiPortalGUID, 0, sizeof(m_uiPortalGUID));
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
                    case NPC_ROMOGG:
                        uiRomoggGUID = creature->GetGUID();
                        break;
                    case NPC_CORLA:
                        uiCorlaGUID = creature->GetGUID();
                        break;
                    case NPC_KARSH:
                        uiKarshGUID = creature->GetGUID();
                        break;
                    case NPC_BEAUTY:
                        uiBeautyGUID = creature->GetGUID();
                        break;
                    case NPC_ASCENDANT_LORD_OBSIDIUS:
                        uiLordObsidiusGUID = creature->GetGUID();
                        break;
                    case NPC_RAZ_THE_CRAZED:
                        uiRazCrazedGUID = creature->GetGUID();
                        break;
                    case NPC_EVOLVED_TWILIGHT_ZEALOT:
                        creature->CastSpell(creature, SPELL_INVISIBLE_DETECT, true);
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch(go->GetEntry())
                {
                    case GO_PORTA1:
                        m_uiPortalGUID[0] = go->GetGUID();
                        if (GetData(DATA_ROMOGG) == DONE)
                            HandleGameObject(m_uiPortalGUID[0], true, go);
                        break;
                    case GO_PORTA2:
                        m_uiPortalGUID[1] = go->GetGUID();
                        if (GetData(DATA_CORLA) == DONE)
                            HandleGameObject(m_uiPortalGUID[1], true, go);
                        break;
                    case GO_PORTA3:
                        m_uiPortalGUID[2] = go->GetGUID();
                        if (GetData(DATA_KARSH) == DONE)
                            HandleGameObject(m_uiPortalGUID[2], true, go);
                        break;
                    case GO_PORTA4:
                        m_uiPortalGUID[3] = go->GetGUID();
                        if (GetData(DATA_BEAUTY) == DONE)
                            HandleGameObject(m_uiPortalGUID[3], true, go);
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch(data)
                {
                    case DATA_ROMOGG:
                        return uiRomoggGUID;
                    case DATA_CORLA:
                        return uiCorlaGUID;
                    case DATA_KARSH:
                        return uiKarshGUID;
                    case DATA_BEAUTY:
                        return uiBeautyGUID;
                    case DATA_ASCENDANT_LORD_OBSIDIUS:
                        return uiLordObsidiusGUID;
                    case NPC_RAZ_THE_CRAZED:
                        return uiRazCrazedGUID;
                }
                return ObjectGuid::Empty;
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

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_ROMOGG && state == DONE)
                    if (Creature* Raz = instance->GetCreature(GetGuidData(NPC_RAZ_THE_CRAZED)))
                        Raz->AI()->DoAction(ACTION_RAZ_BREAK_PRISON);

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "B C " << GetBossSaveData() << archaeologyQuestAura;

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

                if (dataHead1 == 'B' && dataHead2 == 'C')
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
                ObjectGuid uiRomoggGUID;
                ObjectGuid uiCorlaGUID;
                ObjectGuid uiKarshGUID;
                ObjectGuid uiBeautyGUID;
                ObjectGuid uiLordObsidiusGUID;
                ObjectGuid uiRazCrazedGUID;
                ObjectGuid m_uiPortalGUID[4];
                uint32 archaeologyQuestAura;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_blackrock_cavernsInstanceMapScript(map);
        }
};

void AddSC_instance_blackrock_caverns()
{
    new instance_blackrock_caverns();
}
