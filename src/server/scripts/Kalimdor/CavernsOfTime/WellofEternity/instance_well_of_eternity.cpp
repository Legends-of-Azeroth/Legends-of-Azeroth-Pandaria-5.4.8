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

// 108469
#include "ScriptPCH.h"
#include "well_of_eternity.h"

#define MAX_ENCOUNTER 3

static std::vector<DoorData> const doordata =
{
    {GO_INVISIBLE_FIREWALL_DOOR, DATA_PEROTHARN, DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
};

class instance_well_of_eternity : public InstanceMapScript
{
    public:
        instance_well_of_eternity() : InstanceMapScript("instance_well_of_eternity", 939) { }

        struct instance_well_of_eternity_InstanceMapScript : public InstanceScript
        {
            instance_well_of_eternity_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);

                uiEventNozdormu = 0;
                uiEventDemon = 0;
                uiEventDemon2 = 0;
                uiEventIllidan1 = 0;

                uiIllidan1GUID = ObjectGuid::Empty;
                uiPerotharnGUID = ObjectGuid::Empty;
                uiIllidan2GUID = ObjectGuid::Empty;
                uiVarothenGUID = ObjectGuid::Empty;
                uiMannorothGUID = ObjectGuid::Empty;
                uiFireWallStalker = ObjectGuid::Empty;

                uiRoyalCacheGUID = ObjectGuid::Empty;
                uiMinorCacheGUID = ObjectGuid::Empty;
                uiCourtyardDoor1GUID = ObjectGuid::Empty;
                uiLargeFirewallDoor1GUID = ObjectGuid::Empty;
                uiSecondDemonDoors.clear();
                uiPerotharnDoors.clear();
                uiAfterPerotharnDoors.clear();

                uiBlockedPathLeft = false;
                uiBlockedPathRight = false;
                uiBlockedPathForward = false;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (uiEventIllidan1 == IN_PROGRESS)
                    player->CastSpell(player, 102994);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_ILLIDAN_1:
                        uiIllidan1GUID = creature->GetGUID();
                        break;
                    case NPC_PEROTHARN:
                        uiPerotharnGUID = creature->GetGUID();
                        if (GetData(DATA_EVENT_DEMON) == DONE)
                            creature->NearTeleportTo(perotharnPosCenter.GetPositionX(), perotharnPosCenter.GetPositionY(), perotharnPosCenter.GetPositionZ(), perotharnPosCenter.GetOrientation());
                        else
                            creature->NearTeleportTo(perotharnPosIntro.GetPositionX(), perotharnPosIntro.GetPositionY(), perotharnPosIntro.GetPositionZ(), perotharnPosIntro.GetOrientation());
                        creature->SetHomePosition(perotharnPosCenter);
                        break;
                    case NPC_ILLIDAN_2:
                        uiIllidan2GUID = creature->GetGUID();
                        break;
                    case NPC_VAROTHEN:
                        uiVarothenGUID = creature->GetGUID();
                        break;
                    case NPC_MANNOROTH:
                        uiMannorothGUID = creature->GetGUID();
                        break;
                    case NPC_FIRE_WALL_STALKER:
                        uiFireWallStalker = creature->GetGUID();
                        break;
                    default:
                        break;
                }                
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_ROYAL_CACHE:
                        uiRoyalCacheGUID = go->GetGUID();
                        break;
                    case GO_MINOR_CACHE:
                        uiMinorCacheGUID = go->GetGUID();
                        break;
                    case GO_INVISIBLE_FIREWALL_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_COURTYARD_DOOR_1:
                        uiCourtyardDoor1GUID = go->GetGUID();
                        if (uiEventDemon == DONE)
                            HandleGameObject(uiCourtyardDoor1GUID, true, go);
                        break;
                    case GO_LARGE_FIREWALL_DOOR:
                        if (go->GetPositionX() >= 3425.0f)
                        {
                            uiSecondDemonDoors.push_back(go->GetGUID());
                            HandleGameObject(go->GetGUID(), uiEventDemon2 == DONE, go);
                        }
                        else if (go->GetPositionX() <= 3200.0f)
                        {
                            uiLargeFirewallDoor1GUID = go->GetGUID();
                            HandleGameObject(uiLargeFirewallDoor1GUID, uiEventDemon == DONE || uiEventDemon == NOT_STARTED, go);
                        }
                        else
                        {
                            uiPerotharnDoors.push_back(go->GetGUID());
                            HandleGameObject(go->GetGUID(), GetBossState(DATA_PEROTHARN) != IN_PROGRESS, go);
                        }
                        break;
                    case GO_SMALL_FIREWALL_DOOR:
                        if (go->GetPositionX() <= 3340.0f)
                        {
                            uiPerotharnDoors.push_back(go->GetGUID());
                            HandleGameObject(go->GetGUID(), GetBossState(DATA_PEROTHARN) != IN_PROGRESS, go);
                        }
                        else
                        {
                            uiAfterPerotharnDoors.push_back(go->GetGUID());
                            HandleGameObject(go->GetGUID(), GetData(DATA_EVENT_ILLIDAN_1) != DONE || GetBossState(DATA_PEROTHARN) == DONE, go);
                        }
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_EVENT_NOZDORMU:
                        uiEventNozdormu = data;
                        if (data == DONE)
                            SaveToDB();
                        break;
                    case DATA_EVENT_DEMON:
                        uiEventDemon = data;
                        if (data == DONE)
                        {
                            HandleGameObject(uiCourtyardDoor1GUID, true);
                            HandleGameObject(uiLargeFirewallDoor1GUID, true);
                            SaveToDB();
                        }
                        else if (data == IN_PROGRESS)
                            HandleGameObject(uiLargeFirewallDoor1GUID, false);
                        else if (data == SPECIAL)
                            if (Creature* stalker = instance->GetCreature(uiFireWallStalker))
                                stalker->AI()->DoAction(1);
                        break;
                    case DATA_EVENT_DEMON_2:
                        uiEventDemon2 = data;
                        if (data == DONE)
                        {
                            if (!uiSecondDemonDoors.empty())
                                for (std::vector<ObjectGuid>::const_iterator itr = uiSecondDemonDoors.begin(); itr != uiSecondDemonDoors.end(); ++itr)
                                    HandleGameObject((*itr), true);
                            SaveToDB();
                        }
                        break;
                    case DATA_EVENT_ILLIDAN_1:
                        uiEventIllidan1 = data;
                        if (data == DONE)
                        {
                            if (!uiAfterPerotharnDoors.empty())
                                for (std::vector<ObjectGuid>::const_iterator itr = uiAfterPerotharnDoors.begin(); itr != uiAfterPerotharnDoors.end(); ++itr)
                                    HandleGameObject((*itr), false);
                            SaveToDB();
                        }
                        break;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_LEFT:
                        uiBlockedPathLeft = data;
                        break;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_RIGHT:
                        uiBlockedPathRight = data;
                        break;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_FORWARD:
                        uiBlockedPathForward = data;
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_EVENT_NOZDORMU:
                        return uiEventNozdormu;
                    case DATA_EVENT_DEMON:
                        return uiEventDemon;
                    case DATA_EVENT_DEMON_2:
                        return uiEventDemon2;
                    case DATA_EVENT_ILLIDAN_1:
                        return uiEventIllidan1;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_LEFT:
                        return uiBlockedPathLeft;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_RIGHT:
                        return uiBlockedPathRight;
                    case DATA_LEGION_DEMON_BLOCKED_PATH_FORWARD:
                        return uiBlockedPathForward;
                    default:
                        return 0;
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_EVENT_ILLIDAN_1:
                        return uiIllidan1GUID;
                    case DATA_PEROTHARN:
                        return uiPerotharnGUID;
                    case DATA_ROYAL_CACHE:
                        return uiRoyalCacheGUID;
                    case DATA_MINOR_CACHE:
                        return uiMinorCacheGUID;
                    case DATA_EVENT_ILLIDAN_2:
                        return uiIllidan2GUID;
                    case DATA_VAROTHEN:
                        return uiVarothenGUID;
                    case DATA_MANNOROTH:
                        return uiMannorothGUID;
                    default:
                        return ObjectGuid::Empty;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_PEROTHARN)
                {
                    if (!uiPerotharnDoors.empty())
                        for (std::vector<ObjectGuid>::const_iterator itr = uiPerotharnDoors.begin(); itr != uiPerotharnDoors.end(); ++itr)
                            HandleGameObject((*itr), state != IN_PROGRESS);
                    if (!uiAfterPerotharnDoors.empty())
                        for (std::vector<ObjectGuid>::const_iterator itr = uiAfterPerotharnDoors.begin(); itr != uiAfterPerotharnDoors.end(); ++itr)
                            HandleGameObject((*itr), state == DONE);
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "W o E " << GetBossSaveData() 
                    << uiEventNozdormu << " " << uiEventDemon << " " << uiEventIllidan1 << " " << uiEventDemon2 << " ";

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

                char dataHead1, dataHead2, dataHead3;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> dataHead3;

                if (dataHead1 == 'W' && dataHead2 == 'o' && dataHead3 == 'E')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 tmpEvent1;
                    loadStream >> tmpEvent1;
                    uiEventNozdormu = ((tmpEvent1 != DONE) ? NOT_STARTED : DONE);

                    uint32 tmpEvent2;
                    loadStream >> tmpEvent2;
                    uiEventDemon = ((tmpEvent2 != DONE) ? NOT_STARTED : DONE);

                    uint32 tmpEvent3;
                    loadStream >> tmpEvent3;
                    uiEventIllidan1 = ((tmpEvent3 != DONE) ? NOT_STARTED : DONE);
                    
                    uint32 tmpEvent4;
                    loadStream >> tmpEvent4;
                    uiEventDemon2 = ((tmpEvent4 != DONE) ? NOT_STARTED : DONE);
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiEventNozdormu;
                uint32 uiEventDemon;
                uint32 uiEventDemon2;
                uint32 uiEventIllidan1;

                ObjectGuid uiIllidan1GUID;
                ObjectGuid uiPerotharnGUID;
                ObjectGuid uiIllidan2GUID;
                ObjectGuid uiVarothenGUID;
                ObjectGuid uiMannorothGUID;
                ObjectGuid uiFireWallStalker;
                
                ObjectGuid uiRoyalCacheGUID;
                ObjectGuid uiMinorCacheGUID;
                ObjectGuid uiCourtyardDoor1GUID;
                ObjectGuid uiLargeFirewallDoor1GUID;
                std::vector<ObjectGuid> uiSecondDemonDoors;
                std::vector<ObjectGuid> uiAfterPerotharnDoors;
                std::vector<ObjectGuid> uiPerotharnDoors;
               
                bool uiBlockedPathLeft;
                bool uiBlockedPathRight;
                bool uiBlockedPathForward;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_well_of_eternity_InstanceMapScript(map);
        }
};

void AddSC_instance_well_of_eternity()
{
    new instance_well_of_eternity();
}
