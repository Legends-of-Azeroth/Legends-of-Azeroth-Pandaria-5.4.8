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

#include "lost_city_of_the_tolvir.h"
#include "ScriptPCH.h"

#define MAX_ENCOUNTER 5

enum eScriptText
{
    YELL_FREE                    = 4,
};

class instance_lost_city_of_the_tolvir : public InstanceMapScript
{
    public:
        instance_lost_city_of_the_tolvir() : InstanceMapScript("instance_lost_city_of_the_tolvir", 755) { }

        struct instance_lost_city_of_the_tolvir_InstanceMapScript : public InstanceScript
        {
            instance_lost_city_of_the_tolvir_InstanceMapScript(InstanceMap* map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);

                memset(&uiTunnelGUID, 0, sizeof(uiTunnelGUID));
                uiTunnelFlag = 0;
                uiHusamGUID = ObjectGuid::Empty;
                uiLockmawGUID = ObjectGuid::Empty;
                uiAughGUID = ObjectGuid::Empty;
                uiBarimGUID = ObjectGuid::Empty;
                uiBlazeGUID = ObjectGuid::Empty;
                uiSiamatGUID = ObjectGuid::Empty;
                uiHarbingerGUID = ObjectGuid::Empty;
                uiSiamatPlatformGUID = ObjectGuid::Empty;
                uiUpdateTimer = 7000;
                BosesIsDone = false;
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
                bool siamatAvailable = (GetBossState(DATA_GENERAL_HUSAM)==DONE) && (GetBossState(DATA_LOCKMAW)==DONE) && (GetBossState(DATA_HIGH_PROPHET_BARIM)==DONE);
            
                switch (creature->GetEntry())
                {
                    case BOSS_GENERAL_HUSAM:
                        uiHusamGUID = creature->GetGUID();
                        break;
                    case BOSS_LOCKMAW:
                        uiLockmawGUID = creature->GetGUID();
                        break;
                    case BOSS_AUGH:
                        uiAughGUID = creature->GetGUID();
                        break;
                    case BOSS_HIGH_PROPHET_BARIM:
                        uiBarimGUID = creature->GetGUID();
                        break;
                    case BOSS_SIAMAT:
                        uiSiamatGUID = creature->GetGUID();
                        if (siamatAvailable)
                            BosesIsDone = true;
                        break;
                    case NPC_WIND_TUNNEL:
                        {
                            creature->SetVisible(false);
                            creature->SetCanFly(true);
                            uiTunnelGUID[uiTunnelFlag] = creature->GetGUID();
                            ++uiTunnelFlag;

                            if (uiTunnelFlag >= 6)
                                uiTunnelFlag = 0;
                        }
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                if (go->GetEntry() == SIAMAT_PLATFORM)
                {
                    go->setActive(true);
                    uiSiamatPlatformGUID = go->GetGUID();
                }
            }

            void SiamatFree()
            {
                if (GameObject* platform = instance->GetGameObject(uiSiamatPlatformGUID))
                {
                    platform->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED);
                    platform->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DESTROYED);
                }

                for (int i = 0; i < 6; ++i)
                    if (Creature* tunnel = instance->GetCreature(uiTunnelGUID[i]))
                        tunnel->SetVisible(true);
            }

            void Update(uint32 diff) override
            {
                if (BosesIsDone)
                {
                    if (uiUpdateTimer <= diff)
                    {
                        BosesIsDone = false;
                        SiamatFree();

                        if (Creature* siamat = instance->GetCreature(uiSiamatGUID))
                            siamat->AI()->Talk(YELL_FREE);
                    }
                    else
                        uiUpdateTimer -= diff;
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

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GENERAL_HUSAM:
                        return uiSiamatGUID;
                    case DATA_LOCKMAW:
                        return uiLockmawGUID;
                    case DATA_AUGH:
                        return uiAughGUID;
                    case DATA_HIGH_PROPHET_BARIM:
                        return uiBarimGUID;
                    case DATA_BLAZE:
                        return uiBlazeGUID;
                    case DATA_HARBINGER:
                        return uiHarbingerGUID;
                    case DATA_SIAMAT:
                        return uiSiamatGUID;
                }
                return ObjectGuid::Empty;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_HARBINGER:
                        uiHarbingerGUID = data;
                        break;
                    case DATA_BLAZE:
                        uiBlazeGUID = data;
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                bool siamatAvailable = (GetBossState(DATA_GENERAL_HUSAM)==DONE) && (GetBossState(DATA_LOCKMAW)==DONE) && (GetBossState(DATA_HIGH_PROPHET_BARIM)==DONE);

                switch (type)
                {
                    case DATA_GENERAL_HUSAM:
                    case DATA_LOCKMAW:
                    case DATA_HIGH_PROPHET_BARIM:
                        if (state == DONE && siamatAvailable)
                            BosesIsDone = true;
                        break;
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "L S " << GetBossSaveData() << archaeologyQuestAura;

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

                if (dataHead1 == 'L' && dataHead2 == 'S')
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
            ObjectGuid uiTunnelGUID[6];
            ObjectGuid uiHusamGUID;
            ObjectGuid uiLockmawGUID;
            ObjectGuid uiAughGUID;
            ObjectGuid uiBarimGUID;
            ObjectGuid uiBlazeGUID;
            ObjectGuid uiHarbingerGUID;
            ObjectGuid uiSiamatGUID;
            ObjectGuid uiSiamatPlatformGUID;
            uint32 uiUpdateTimer;
            uint32 archaeologyQuestAura;
            uint8 uiTunnelFlag;
            bool BosesIsDone;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_lost_city_of_the_tolvir_InstanceMapScript(map);
        }
};

void AddSC_instance_lost_city_of_the_tolvir()
{
    new instance_lost_city_of_the_tolvir();
}
