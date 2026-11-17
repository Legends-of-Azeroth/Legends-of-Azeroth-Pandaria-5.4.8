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
#include "zulaman.h"

#define MAX_ENCOUNTER 6

static std::vector<DoorData> doordata = 
{
    {GO_AKILZON_EXIT,           DATA_AKILZON,            DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_HALAZZI_ENTRANCE,       DATA_HALAZZI,            DOOR_TYPE_ROOM,     BOUNDARY_NONE},
    {GO_HALAZZI_EXIT,           DATA_HALAZZI,            DOOR_TYPE_PASSAGE,  BOUNDARY_NONE},
    {GO_MALACRASS_EXIT,         DATA_HEX_LORD_MALACRASS, DOOR_TYPE_PASSAGE,  BOUNDARY_NONE},
    {GO_DAAKARA_EXIT,           DATA_DAAKARA,            DOOR_TYPE_ROOM,     BOUNDARY_NONE},
};

class instance_zulaman : public InstanceMapScript
{
    public:
        instance_zulaman() : InstanceMapScript("instance_zulaman", 568) { }

        struct instance_zulaman_InstanceMapScript : public InstanceScript
        {
            instance_zulaman_InstanceMapScript(Map* map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);

                HexLordGateGUID = ObjectGuid::Empty;
                MainGateGUID = ObjectGuid::Empty;
                StrangeGongGUID = ObjectGuid::Empty;

                QuestTimer = 0;
                QuestMinute = 21;
                uiMainGate = 0;
                uiVendor1 = 0;
                uiVendor2 = 0;

                for (uint8 i = 0; i < 4; ++i)
                {
                    _hostages[i].realGUID = ObjectGuid::Empty;
                    _hostages[i].corpseGUID = ObjectGuid::Empty;
                    _hostages[i].lootGUID = ObjectGuid::Empty;
                    _hostages[i].state = 0;
                }
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
                switch (creature->GetEntry())
                {
                    case NPC_AMANISHI_TEMPEST:
                        AmanishiTempestGUID = creature->GetGUID();
                        break;
                    case NPC_BAKKALZU:
                        _hostages[0].realGUID = creature->GetGUID();
                        _hostages[0].state = 0;
                        break;
                    case NPC_BAKKALZU_CORPSE:
                        _hostages[0].corpseGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_HAZLEK:
                        _hostages[1].realGUID = creature->GetGUID();
                        _hostages[1].state = 0;
                        break;
                    case NPC_HAZLEK_CORPSE:
                        _hostages[1].corpseGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_NORKANI:
                        _hostages[2].realGUID = creature->GetGUID();
                        _hostages[2].state = 0;
                        break;
                    case NPC_NORKANI_CORPSE:
                        _hostages[2].corpseGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_KASHA:
                        _hostages[3].realGUID = creature->GetGUID();
                        _hostages[3].state = 0;
                        break;
                    case NPC_KASHA_CORPSE:
                        _hostages[3].corpseGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_STRANGE_GONG:
                        StrangeGongGUID = go->GetGUID();
                        if (uiMainGate == 1)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_MAIN_GATE:
                        MainGateGUID = go->GetGUID();
                        if (uiMainGate == 1)
                            HandleGameObject(MainGateGUID, true);
                        break;
                    case GO_AKILZON_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_HALAZZI_ENTRANCE:
                        AddDoor(go, true);
                        break;
                    case GO_HALAZZI_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_MALACRASS_ENTRANCE:
                        HexLordGateGUID = go->GetGUID(); 
                        if (GetBossesDone() >= 2)
                            HandleGameObject(HexLordGateGUID, true, go);
                        break;
                    case GO_MALACRASS_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_DAAKARA_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_HAZLEK_TRUNK:
                        _hostages[1].lootGUID = go->GetGUID();
                        break;
                    case GO_NORKANI_PACKAGE:
                        _hostages[2].lootGUID = go->GetGUID();
                        break;
                    case GO_KASHA_BAG:
                        //_hostages[3].lootGUID = go->GetGUID();
                        uiKashaBagGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 id, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(id, state))
                    return false;

                if (state == DONE)
                {
                    switch (id)
                    {
                        case DATA_AKILZON:
                        case DATA_NALORAKK:
                        case DATA_JANALAI:
                        case DATA_HALAZZI:
                        {
                            if (id == DATA_AKILZON) QuestMinute += 15;
                            else if (id == DATA_NALORAKK) QuestMinute += 10;
                            DoUpdateWorldState(3106, QuestMinute);

                            if (_hostages[id].state == 0)
                            {
                                _hostages[id].state = 1;
                                if (_hostages[id].lootGUID)
                                    DoRespawnGameObject(_hostages[id].lootGUID, DAY);
                            }

                            uint8 _bosses = GetBossesDone();

                            if (_bosses >= 2)
                                HandleGameObject(HexLordGateGUID, true);

                            if (_bosses >= 4)
                            {
                                if (QuestMinute)
                                {
                                    DoCastSpellOnPlayers(SPELL_ZULAMAN_ACHIEVEMENT);
                                    if (uiKashaBagGUID)
                                        DoRespawnGameObject(uiKashaBagGUID, DAY);
                                    QuestMinute = 0;
                                    DoUpdateWorldState(3104, 0);
                                }
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_MAIN_GATE:
                        uiMainGate = data;
                        if (data == 1)
                        {
                            HandleGameObject(MainGateGUID, true);
                            if (GameObject* go = instance->GetGameObject(StrangeGongGUID))
                                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                            SaveToDB();
                        }
                        break;
                    case DATA_VENDOR_1:
                        uiVendor1 = data;
                        SaveToDB();
                        break;
                    case DATA_VENDOR_2:
                        uiVendor2 = data;
                        SaveToDB();
                        break;
                    case uint32(-1):
                        archaeologyQuestAura = data;
                        SaveToDB();
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MAIN_GATE:
                        return MainGateGUID;
                    case DATA_TEMPEST:
                        return AmanishiTempestGUID;
                    case DATA_VENDOR_1:
                        return uiVendor1;
                    case DATA_VENDOR_2:
                        return uiVendor2;
                    default: 
                        return 0;
                }
            }

            void Update(uint32 diff) override
            {
                if (QuestMinute)
                {
                    if (QuestTimer <= diff)
                    {
                        QuestMinute--;
                        SaveToDB();
                        QuestTimer += 60000;
                        if (QuestMinute)
                        {
                            DoUpdateWorldState(3104, 1);
                            DoUpdateWorldState(3106, QuestMinute);
                        } 
                        else
                        {
                            DoUpdateWorldState(3104, 0);
                            for (uint8 i = 0; i < 4; ++i)
                            {
                                if (Creature* pHostage = instance->GetCreature(_hostages[i].realGUID))
                                {
                                    if (_hostages[i].state != 0)
                                        continue;

                                    pHostage->DespawnOrUnsummon();
                                    if (Creature* pCorpse = instance->GetCreature(_hostages[i].corpseGUID))
                                        pCorpse->SetVisible(true);
                                    _hostages[i].state = 2;
                                }
                            }
                                    
                        }

                    }
                    QuestTimer -= diff;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream ss;
                ss << "ZA " << GetBossSaveData() << uiMainGate << " " << QuestMinute << " " << uiVendor1 << " " << uiVendor2 << " " << archaeologyQuestAura;

                OUT_SAVE_INST_DATA_COMPLETE;
                return ss.str();
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

                if (dataHead1 == 'Z' && dataHead2 == 'A')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                    loadStream >> uiMainGate;
                    loadStream >> QuestMinute;
                    DoUpdateWorldState(3104, QuestMinute);
                    loadStream >> uiVendor1;
                    loadStream >> uiVendor2;
                    loadStream >> archaeologyQuestAura;
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        private:
            ObjectGuid HexLordGateGUID;
            ObjectGuid MainGateGUID;
            ObjectGuid StrangeGongGUID;
            ObjectGuid AmanishiTempestGUID;
            ObjectGuid uiKashaBagGUID;

            uint32 uiMainGate;
            uint32 uiVendor1;
            uint32 uiVendor2;
            uint32 QuestTimer;
            uint16 QuestMinute;

            uint32 archaeologyQuestAura;

            uint8 GetBossesDone()
            {
                uint8 _bosses = 0;
                for (uint8 i = 0; i < 4; ++i)
                    if (GetBossState(i) == DONE)
                        _bosses++;
                return _bosses;
            }

            struct Hostage
            {
                ObjectGuid realGUID;
                ObjectGuid corpseGUID;
                ObjectGuid lootGUID;
                uint8 state; // 0 - neutral, 1 - saved, 2 - killed
            };

            Hostage _hostages[4];
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_zulaman_InstanceMapScript(map);
        }
};

void AddSC_instance_zulaman()
{
    new instance_zulaman();
}

