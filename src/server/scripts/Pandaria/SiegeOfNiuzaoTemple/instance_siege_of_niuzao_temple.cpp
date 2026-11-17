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

#include "siege_of_niuzao_temple.h"

static std::vector<DoorData> const doorData =
{
    {GO_TEMPLE_INVIS_DOOR,          BOSS_PAVALAK,          DOOR_TYPE_PASSAGE,       BOUNDARY_NONE   },
    {GO_WIND_WALL,                  BOSS_PAVALAK,          DOOR_TYPE_PASSAGE,       BOUNDARY_NONE   },
    {GO_FIRE_WALL,                  BOSS_VOJAK,            DOOR_TYPE_PASSAGE,       BOUNDARY_NONE   },
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { BOSS_JINBAK,   CRITERIA_JINBAK  },
    { BOSS_VOJAK,    CRITERIA_VOJAK   },
    { BOSS_PAVALAK,  CRITERIA_PAVALAK },
    { BOSS_NERONOK,  CRITERIA_NERONOK },
};

class instance_siege_of_niuzao_temple : public InstanceMapScript
{
    public:
        instance_siege_of_niuzao_temple() : InstanceMapScript("instance_siege_of_niuzao_temple", 1011) { }

        struct instance_siege_of_niuzao_temple_InstanceScript : public InstanceScript
        {
            instance_siege_of_niuzao_temple_InstanceScript(Map* map) : InstanceScript(map) { }

            void Initialize() override
            {
                invisDoorGUID[0] = ObjectGuid::Empty;
                invisDoorGUID[1] = ObjectGuid::Empty;
                vojakDoor = ObjectGuid::Empty;
                vojakDoorGUID = ObjectGuid::Empty;
                jinbakDoorGUID = ObjectGuid::Empty;
                SetBossNumber(TOTAL_ENCOUNTERS);
                LoadDoorData(doorData);

                if (instance->IsChallengeDungeon())
                    LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (instance->IsChallengeDungeon())
                    SendChallengeInfo(player, SCENARIO_ID);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (instance->IsChallengeDungeon() && creature->isDead())
                    creature->Respawn();
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_TEMPLE_INVIS_DOOR:
                        invisDoorGUID[invisDoorGUID[0] != 0] = go->GetGUID();
                        break;
                    case GO_WIND_WALL:
                        AddDoor(go, true);
                        break;                    
                    case GO_FIRE_WALL:
                        vojakDoor = go->GetGUID();
                        break;
                    case GO_DOOR:
                        vojakDoorGUID = go->GetGUID();
                        break;
                    case GO_HARDENED_RESIN:
                        jinbakDoorGUID = go->GetGUID();

                        // SetBossState couldn`t work because GO possible load after load data.
                        if (GetBossState(BOSS_JINBAK) == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case BOSS_JINBAK:
                        if (state == DONE)
                            if (GameObject * go = instance->GetGameObject(jinbakDoorGUID))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case BOSS_PAVALAK:
                        if (state == DONE)
                            for (int i = 0; i < 2; ++i)
                                if (GameObject * go = instance->GetGameObject(invisDoorGUID[i]))
                                    go->Delete();
                        break;
                    case BOSS_VOJAK:
                        if (state == DONE)
                            if (GameObject * go = instance->GetGameObject(vojakDoor))
                                go->Delete();
                        break;
                    default:
                        break;
                }

                return true;
            }

            void SetData(uint32 type, uint32 /*data*/) override
            {
                if (type == DATA_VOJAK_DOOR)
                    if (GameObject * go = instance->GetGameObject(vojakDoorGUID))
                        go->UseDoorOrButton(DAY, false);
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (!InstanceScript::CheckRequiredBosses(bossId, player))
                    return false;

                return true;
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                    if (Creature* creature = unit->ToCreature())
                        UpdateConditionInfo(creature, ENEMIES_COUNT);
            }

            void Update(uint32 diff) override
            {
                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }

            void ProcessEvent(WorldObject* /*source*/, uint32 eventId) override { }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "N T " << GetBossSaveData();

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

                if (dataHead1 == 'N' && dataHead2 == 'T')
                {
                    for (uint32 i = 0; i < TOTAL_ENCOUNTERS; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                } else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        private:
            ObjectGuid vojakDoor;
            ObjectGuid vojakDoorGUID;
            ObjectGuid jinbakDoorGUID;
            ObjectGuid invisDoorGUID[2];

        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_siege_of_niuzao_temple_InstanceScript(map);
        }
};

void AddSC_instance_siege_of_niuzao_temple()
{
    new instance_siege_of_niuzao_temple();
}