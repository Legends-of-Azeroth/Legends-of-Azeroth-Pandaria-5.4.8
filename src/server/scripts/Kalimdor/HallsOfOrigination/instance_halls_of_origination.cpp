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
#include "halls_of_origination.h"

#define MAX_ENCOUNTER 7

static std::vector<DoorData> doorData =
{
    {GO_DOOR_ULDUM_14,              DATA_TEMPLE_GUARDIAN_ANHUUR, DOOR_TYPE_ROOM,    BOUNDARY_NONE},
    {GO_ANHUUR_BRIDGE,              DATA_TEMPLE_GUARDIAN_ANHUUR, DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_DOOR_ULDUM_15,              DATA_TEMPLE_GUARDIAN_ANHUUR, DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_ANHUUR_ELEVATOR,            DATA_TEMPLE_GUARDIAN_ANHUUR, DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    {GO_VAULT_OF_LIGHTS_ENTR_DOOR,  DATA_ANRAPHET,               DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
};

class instance_halls_of_origination : public InstanceMapScript
{
    public:
        instance_halls_of_origination() : InstanceMapScript("instance_halls_of_origination", 644) { }

        struct instance_halls_of_origination_InstanceMapScript: public InstanceScript
        {
            instance_halls_of_origination_InstanceMapScript(InstanceMap *map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                uiTempleGuardianAnhuurGUID = ObjectGuid::Empty;
                uiEarthragerPtahGUID = ObjectGuid::Empty;
                uiAnraphetGUID = ObjectGuid::Empty;
                uiIsisetGUID = ObjectGuid::Empty;
                uiAmmunaeGUID = ObjectGuid::Empty;
                uiSeteshGUID = ObjectGuid::Empty;
                uiRajhGUID = ObjectGuid::Empty;
                uiBrannGUID = ObjectGuid::Empty;

                uiWardensDone = 0;

                uiOriginationElevatorGUID = ObjectGuid::Empty;
                uiAnhuurBridgeGUID = ObjectGuid::Empty;
                uiAnraphetEntranceDoorGUID = ObjectGuid::Empty;
                uiAnraphetBossDoorGUID = ObjectGuid::Empty;

                archaeologyQuestAura = 0;
                instance->SetWorldState(WORLDSTATE_FASTER_THAN_THE_SPEED_OF_LIGHT, 1); // by default, controlled by achiev timer
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
                    case NPC_TEMPLE_GUARDIAN_ANHUUR:
                        uiTempleGuardianAnhuurGUID = creature->GetGUID();
                        break;
                    case NPC_EARTHRAGER_PTAH:
                        uiEarthragerPtahGUID = creature->GetGUID();
                        break;
                    case NPC_ANRAPHET:
                        uiAnraphetGUID = creature->GetGUID();
                        if (uiWardensDone >= 4)
                        {
                            creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                            creature->SetHomePosition(-203.93f, 368.71f, 75.92f, creature->GetOrientation());
                            //DoTeleportTo(-203.93f, 368.71f, 75.92f);
                            creature->GetMotionMaster()->MovePoint(0, -203.93f, 368.71f, 75.92f);
                        }
                        break;
                    case NPC_ISISET:
                        uiIsisetGUID = creature->GetGUID();
                        break;
                    case NPC_AMMUNAE:
                        uiAmmunaeGUID = creature->GetGUID();
                        break;
                    case NPC_SETESH:
                        uiSeteshGUID = creature->GetGUID();
                        break;
                    case NPC_RAJH:
                        uiRajhGUID = creature->GetGUID();
                        break;
                    case NPC_BRANN_BRONZEBEARD:
                        uiBrannGUID = creature->GetGUID();
                        if (GetBossState(DATA_ANRAPHET) == DONE)
                            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case NPC_BEETLE_STALKER:
                    case NPC_ALPHA_BEAM:
                        creature->SetDisplayId(11686);
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry()) 
                {
                    case GO_ORIGINATION_ELEVATOR:
                        uiOriginationElevatorGUID = go->GetGUID();
                        break;
                    case GO_ANHUUR_BRIDGE:
                    case GO_ANHUUR_ELEVATOR:
                    case GO_DOOR_ULDUM_14:
                    case GO_DOOR_ULDUM_15:                                           
                        AddDoor(go, true);
                        break;
                    case GO_VAULT_OF_LIGHTS_ENTR_DOOR:
                        uiAnraphetEntranceDoorGUID = go->GetGUID();
                        break;
                    case GO_VAULT_OF_LIGHTS_BOSS_DOOR:
                        uiAnraphetBossDoorGUID = go->GetGUID();
                        if (uiWardensDone >= 4)
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_TEMPLE_GUARDIAN_ANHUUR:
                        return uiTempleGuardianAnhuurGUID;
                    case DATA_EARTHRAGER_PTAH:
                        return uiEarthragerPtahGUID;
                    case DATA_ANRAPHET:
                        return uiAnraphetGUID;
                    case DATA_ISISET:
                        return uiIsisetGUID;
                    case DATA_AMMUNAE:
                        return uiAmmunaeGUID;
                    case DATA_SETESH:
                        return uiSeteshGUID;
                    case DATA_RAJH:
                        return uiRajhGUID;
                    case DATA_BRANN:
                        return uiBrannGUID;
                    case DATA_ANRAPHET_ENTRANCE_DOOR:
                        return uiAnraphetEntranceDoorGUID;
                    case DATA_ANRAPHET_BOSS_DOOR:
                        return uiAnraphetBossDoorGUID;
                }
                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_WARDENS:
                        uiWardensDone += data;
                        
                        if (uiWardensDone == 4)
                        {
                            DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_FASTER_THAN_LIGHT);
                            HandleGameObject(uiAnraphetBossDoorGUID, true);
                            if (Creature* pAnraphet = instance->GetCreature(uiAnraphetGUID))
                                pAnraphet->AI()->DoAction(1);
                        }

                        switch (uiWardensDone)
                        {
                            case 1:
                            case 2:
                            case 3:
                            case 4:
                                if (Creature* pBrann = instance->GetCreature(uiBrannGUID))
                                    pBrann->AI()->DoAction(uiWardensDone);
                                break;
                        }
                        break;
                    case uint32(-1):
                        archaeologyQuestAura = data;
                        break;
                    case DATA_ANRAPHET:
                        if (data == DONE)
                            if (GameObject* uiElevator = instance->GetGameObject(uiOriginationElevatorGUID))
                                DoRespawnGameObject(uiElevator->GetGUID(), 1 * DAY);
                        break;
                }
                SaveToDB();
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "H O" << GetBossSaveData() << uiWardensDone << " " << archaeologyQuestAura; 

                OUT_SAVE_INST_DATA_COMPLETE;
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

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'H' && dataHead2 == 'O')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                    uint32 wardens = 0;
                    loadStream >> wardens;
                    //uiWardensDone = wardens;
                    if (wardens > 4) wardens = 4;
                    SetData(DATA_WARDENS, wardens);
                    loadStream >> archaeologyQuestAura;
                }else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;
                ObjectGuid uiTempleGuardianAnhuurGUID;
                ObjectGuid uiEarthragerPtahGUID;
                ObjectGuid uiAnraphetGUID;
                ObjectGuid uiIsisetGUID;
                ObjectGuid uiAmmunaeGUID;
                ObjectGuid uiSeteshGUID;
                ObjectGuid uiRajhGUID;
                ObjectGuid uiBrannGUID;

                uint32 uiWardensDone;

                ObjectGuid uiOriginationElevatorGUID;
                ObjectGuid uiAnhuurBridgeGUID;
                ObjectGuid uiAnraphetEntranceDoorGUID;
                ObjectGuid uiAnraphetBossDoorGUID;

                uint32 archaeologyQuestAura;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_halls_of_origination_InstanceMapScript(map);
        }
};

void AddSC_instance_halls_of_origination()
{
    new instance_halls_of_origination();
}
