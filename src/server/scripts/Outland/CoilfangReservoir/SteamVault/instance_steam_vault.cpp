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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "steam_vault.h"

class go_main_chambers_access_panel : public GameObjectScript
{
    public:
        go_main_chambers_access_panel() : GameObjectScript("go_main_chambers_access_panel") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();
            if (!instance)
                return false;

            if (go->GetEntry() == GO_ACCESS_PANEL_HYDRO && (instance->GetBossState(DATA_HYDROMANCER_THESPIA) == DONE || instance->GetBossState(DATA_HYDROMANCER_THESPIA) == SPECIAL))
            {
                instance->SetBossState(DATA_HYDROMANCER_THESPIA, SPECIAL);
                go->SetGoState(GO_STATE_ACTIVE);
            }

            if (go->GetEntry() == GO_ACCESS_PANEL_MEK && (instance->GetBossState(DATA_MEKGINEER_STEAMRIGGER) == DONE || instance->GetBossState(DATA_MEKGINEER_STEAMRIGGER) == SPECIAL))
            {
                instance->SetBossState(DATA_MEKGINEER_STEAMRIGGER, SPECIAL);
                go->SetGoState(GO_STATE_ACTIVE);
            }

            return true;
        }
};

class instance_steam_vault : public InstanceMapScript
{
    public:
        instance_steam_vault() : InstanceMapScript(SteamVaultScriptName, 545) { }

        struct instance_steam_vault_InstanceMapScript : public InstanceScript
        {
            instance_steam_vault_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);

                ThespiaGUID = ObjectGuid::Empty;
                MekgineerGUID = ObjectGuid::Empty;
                KalithreshGUID = ObjectGuid::Empty;

                MainChambersDoorGUID = ObjectGuid::Empty;
                MekDoor              = ObjectGuid::Empty;
                HydroDoor            = ObjectGuid::Empty;
                DistillerState       = 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_HYDROMANCER_THESPIA:
                        ThespiaGUID = creature->GetGUID();
                        break;
                    case NPC_MEKGINEER_STEAMRIGGER:
                        MekgineerGUID = creature->GetGUID();
                        break;
                    case NPC_WARLORD_KALITHRESH:
                        KalithreshGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_MAIN_CHAMBERS_DOOR:
                        MainChambersDoorGUID = go->GetGUID();
                        break;
                    case GO_ACCESS_PANEL_HYDRO:
                        HydroDoor = go->GetGUID();
                        break;
                    case GO_ACCESS_PANEL_MEK:
                        MekDoor = go->GetGUID();
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_HYDROMANCER_THESPIA:
                        return ThespiaGUID;
                    case DATA_MEKGINEER_STEAMRIGGER:
                        return MekgineerGUID;
                    case DATA_WARLORD_KALITHRESH:
                        return KalithreshGUID;
                    case GO_ACCESS_PANEL_HYDRO:
                        return HydroDoor;
                    case GO_ACCESS_PANEL_MEK:
                        return MekDoor;
                }
                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_DISTILLER)
                    DistillerState = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_DISTILLER)
                    return DistillerState;
                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_HYDROMANCER_THESPIA:
                        if (state == SPECIAL)
                        {
                            if (GetBossState(DATA_MEKGINEER_STEAMRIGGER) == SPECIAL)
                                HandleGameObject(MainChambersDoorGUID, true);

                            TC_LOG_DEBUG("scripts", "Instance Steamvault: Access panel used.");
                        }
                        break;
                    case DATA_MEKGINEER_STEAMRIGGER:
                        if (state == SPECIAL)
                        {
                            if (GetBossState(DATA_HYDROMANCER_THESPIA) == SPECIAL)
                                HandleGameObject(MainChambersDoorGUID, true);

                            TC_LOG_DEBUG("scripts", "Instance Steamvault: Access panel used.");
                        }
                        break;
                    default:
                        break;
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S V " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* str) override
            {
                if (!str)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(str);

                char dataHead1, dataHead2;

                std::istringstream loadStream(str);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'S' && dataHead2 == 'V')
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

            protected:
                ObjectGuid ThespiaGUID;
                ObjectGuid MekgineerGUID;
                ObjectGuid KalithreshGUID;

                ObjectGuid MainChambersDoorGUID;
                ObjectGuid MekDoor;
                ObjectGuid HydroDoor;
                uint8 DistillerState;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_steam_vault_InstanceMapScript(map);
        }
};

void AddSC_instance_steam_vault()
{
    new go_main_chambers_access_panel();
    new instance_steam_vault();
}
