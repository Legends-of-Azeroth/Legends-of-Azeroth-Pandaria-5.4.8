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
#include "CreatureTextMgr.h"
#include "scarlet_halls.h"

static std::vector<DoorData> const doorData =
{
    { GO_COMANDER_LINDON_EXIT,   DATA_COMANDER_LINDON,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_HOUNDMASTER_BRAUN_EXIT, BOSS_HOUNDMASTER_BRAUN, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_ARMSMASTER_HARLAN_EXIT, BOSS_ARMSMASTER_HARLAN, DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { BOSS_HOUNDMASTER_BRAUN,   CRITERIA_HOUNDMASTER_BRAUN   },
    { BOSS_ARMSMASTER_HARLAN,   CRITERIA_ARMSMASTER_HARLAN   },
    { BOSS_FLAMEWEAVER_KOEGLER, CRITERIA_FLAMEWEAVER_KOEGLER },
};

class instance_scarlet_halls : public InstanceMapScript
{
    public: instance_scarlet_halls() : InstanceMapScript("instance_scarlet_halls", 1001) { }

        struct instance_scarlet_halls_InstanceMapScript : public InstanceScript
        {
            instance_scarlet_halls_InstanceMapScript(Map* map) : InstanceScript(map) { }

            void Initialize() override
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);

                if (instance->IsChallengeDungeon())
                    LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);

                instance->SetWorldState(WORLDSTATE_HUMANE_SOCIETY, 1);
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

                switch (creature->GetEntry())
                {
                    case NPC_HOUNDMASTER_BRAUN:
                        HoundMaster_BraunGUID = creature->GetGUID();
                        break;
                    case NPC_ARMSMASTER_HARLAN:
                        ArmsMaster_HarlanGUID = creature->GetGUID();
                        break;
                    case NPC_FLAMEWEAVER_KOEGLER:
                        FlameWeaver_KoeglerGUID = creature->GetGUID();
                        break;
                    case NPC_COMANDER_LINDON:
                        LindonGUID = creature->GetGUID();
                        break;
                    case NPC_SCARLET_GUARDIAN:
                    case NPC_SERGEANT_VERDONE:
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_EXPLODING_SHOT_STALKER:
                        creature->SetDisplayId(11686);
                        creature->AddAura(114861, creature);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    case NPC_DRAGON_BREATH_TARGET:
                        creature->SetDisplayId(11686); // invisible
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    case NPC_HOODED_CRUSADER:
                        if (creature->GetDBTableGUIDLow() == 538235)
                            HoodedGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_COMANDER_LINDON_EXIT:
                    case GO_HOUNDMASTER_BRAUN_EXIT:
                    case GO_ARMSMASTER_HARLAN_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
                        break;
                }
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

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case BOSS_HOUNDMASTER_BRAUN:
                        return HoundMaster_BraunGUID;
                    case BOSS_ARMSMASTER_HARLAN:
                        return ArmsMaster_HarlanGUID;
                    case BOSS_FLAMEWEAVER_KOEGLER:
                        return FlameWeaver_KoeglerGUID;
                    case DATA_COMANDER_LINDON:
                        return LindonGUID;
                    case NPC_HOODED_CRUSADER:
                        return HoodedGUID;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                std::ostringstream saveStream;
                saveStream << "S H " << GetBossSaveData();
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

                if (dataHead1 == 'S' && dataHead2 == 'H')
                {
                    for (uint8 i = 0; i < MAX_TYPES; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }
                }

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            protected:
                ObjectGuid HoundMaster_BraunGUID;
                ObjectGuid ArmsMaster_HarlanGUID;
                ObjectGuid FlameWeaver_KoeglerGUID;
                ObjectGuid LindonGUID;
                ObjectGuid HoodedGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_scarlet_halls_InstanceMapScript(map);
        }
};

void AddSC_instance_scarlet_halls()
{
    new instance_scarlet_halls();
}
