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
#include "WorldSession.h"
#include "halls_of_stone.h"
#include <Player.h>

static std::vector<DoorData> const doorData =
{
    { GO_SJONNIR_DOOR, DATA_BRANN_EVENT, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
};

class instance_halls_of_stone : public InstanceMapScript
{
    public:
        instance_halls_of_stone() : InstanceMapScript(HoSScriptName, 599) { }

        struct instance_halls_of_stone_InstanceMapScript : public InstanceScript
        {
            instance_halls_of_stone_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);

                KrystallusGUID = ObjectGuid::Empty;
                MaidenOfGriefGUID = ObjectGuid::Empty;
                SjonnirGUID = ObjectGuid::Empty;

                KaddrakGUID = ObjectGuid::Empty;
                AbedneumGUID = ObjectGuid::Empty;
                MarnakGUID = ObjectGuid::Empty;
                BrannGUID = ObjectGuid::Empty;

                TribunalConsoleGUID = ObjectGuid::Empty;
                TribunalChestGUID = ObjectGuid::Empty;
                TribunalSkyFloorGUID = ObjectGuid::Empty;
                KaddrakGoGUID = ObjectGuid::Empty;
                AbedneumGoGUID = ObjectGuid::Empty;
                MarnakGoGUID = ObjectGuid::Empty;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_KRYSTALLUS:
                        KrystallusGUID = creature->GetGUID();
                        break;
                    case NPC_MAIDEN:
                        MaidenOfGriefGUID = creature->GetGUID();
                        break;
                    case NPC_SJONNIR:
                        SjonnirGUID = creature->GetGUID();
                        break;
                    case NPC_MARNAK:
                        MarnakGUID = creature->GetGUID();
                        break;
                    case NPC_KADDRAK:
                        KaddrakGUID = creature->GetGUID();
                        break;
                    case NPC_ABEDNEUM:
                        AbedneumGUID = creature->GetGUID();
                        break;
                    case NPC_BRANN:
                        BrannGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_ABEDNEUM:
                        AbedneumGoGUID = go->GetGUID();
                        break;
                    case GO_MARNAK:
                        MarnakGoGUID = go->GetGUID();
                        break;
                    case GO_KADDRAK:
                        KaddrakGoGUID = go->GetGUID();
                        break;
                    case GO_TRIBUNAL_CONSOLE:
                        TribunalConsoleGUID = go->GetGUID();
                        break;
                    case GO_TRIBUNAL_CHEST:
                    case GO_TRIBUNAL_CHEST_HERO:
                        TribunalChestGUID = go->GetGUID();
                        if (GetBossState(DATA_BRANN_EVENT) == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_TRIBUNAL_SKY_FLOOR:
                        TribunalSkyFloorGUID = go->GetGUID();
                        break;
                    case GO_SJONNIR_DOOR:
                        AddDoor(go, true);
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MAIDEN_OF_GRIEF:
                        return MaidenOfGriefGUID;
                    case DATA_KRYSTALLUS:
                        return KrystallusGUID;
                    case DATA_SJONNIR:
                        return SjonnirGUID;
                    case DATA_KADDRAK:
                        return KaddrakGUID;
                    case DATA_MARNAK:
                        return MarnakGUID;
                    case DATA_ABEDNEUM:
                        return AbedneumGUID;
                    case DATA_GO_TRIBUNAL_CONSOLE:
                        return TribunalConsoleGUID;
                    case DATA_GO_KADDRAK:
                        return KaddrakGoGUID;
                    case DATA_GO_ABEDNEUM:
                        return AbedneumGoGUID;
                    case DATA_GO_MARNAK:
                        return MarnakGoGUID;
                    case DATA_GO_SKY_FLOOR:
                        return TribunalSkyFloorGUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_BRANN_EVENT:
                        if (state == DONE)
                            if (GameObject* go = instance->GetGameObject(TribunalChestGUID))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_INTERACT_COND);
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player /*= NULL*/) const override
            {
                if (_SkipCheckRequiredBosses(player))
                    return true;

                switch (bossId)
                {
                    case DATA_SJONNIR:
                        if (GetBossState(DATA_BRANN_EVENT) != DONE)
                            return false;
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
                saveStream << "H S " << GetBossSaveData();

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

                if (dataHead1 == 'H' && dataHead2 == 'S')
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
            ObjectGuid KrystallusGUID;
            ObjectGuid MaidenOfGriefGUID;
            ObjectGuid SjonnirGUID;

            ObjectGuid KaddrakGUID;
            ObjectGuid AbedneumGUID;
            ObjectGuid MarnakGUID;
            ObjectGuid BrannGUID;

            ObjectGuid TribunalConsoleGUID;
            ObjectGuid TribunalChestGUID;
            ObjectGuid TribunalSkyFloorGUID;
            ObjectGuid KaddrakGoGUID;
            ObjectGuid AbedneumGoGUID;
            ObjectGuid MarnakGoGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_halls_of_stone_InstanceMapScript(map);
        }
};

void AddSC_instance_halls_of_stone()
{
    new instance_halls_of_stone();
}
