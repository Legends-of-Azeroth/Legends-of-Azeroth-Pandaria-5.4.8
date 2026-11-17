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
#include "blackwing_descent.h"

#define MAX_ENCOUNTER 6

static std::vector<DoorData> const doorData =
{
    {GO_INNER_CHAMBER_DOOR,           DATA_MAGMAW,          DOOR_TYPE_PASSAGE,       BOUNDARY_NONE},
    {GO_INNER_CHAMBER_DOOR,           DATA_OMNOTRON,        DOOR_TYPE_PASSAGE,       BOUNDARY_NONE},
};

class instance_blackwing_descent : public InstanceMapScript
{
    public:
        instance_blackwing_descent() : InstanceMapScript("instance_blackwing_descent", 669) { }

        struct instance_blackwing_descent_InstanceMapScript : public InstanceScript
        {
            instance_blackwing_descent_InstanceMapScript(Map* map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                omnotronHealth = 0;
                uiBileotron800Active = 0;
                uiMagmawGUID = ObjectGuid::Empty;
                uiMagmawHeadGUID = ObjectGuid::Empty;
                uiMaloriakGUID = ObjectGuid::Empty;
                uiAberCount = 18;
                uiChimaeronGUID = ObjectGuid::Empty;
                uiBileotron800GUID = ObjectGuid::Empty;
                uiAtramedesGUID = ObjectGuid::Empty;
                uiInnerChamberDoorGUID = ObjectGuid::Empty;
                uiLordNefarianGUID = ObjectGuid::Empty;
                uiOnyxiaGUID = ObjectGuid::Empty;
                uiNefarianGUID = ObjectGuid::Empty;
                uiNefarianFloorGUID = ObjectGuid::Empty;
                memset(&m_uiOmnotronGUID, 0, sizeof(m_uiOmnotronGUID));
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!uiTeamInInstance)
                    uiTeamInInstance = player->GetTeam();
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
                    case NPC_MAGMAW:
                        uiMagmawGUID = creature->GetGUID();
                        break;
                    case NPC_MAGMAW_HEAD:
                        uiMagmawHeadGUID = creature->GetGUID();
                        break;
                    case NPC_OMNOTRON:
                        uiOmnotronControllerGUID = creature->GetGUID();
                        break;                    
                    case NPC_ARCANOTRON:
                        m_uiOmnotronGUID[0] = creature->GetGUID();
                        break;
                    case NPC_ELECTRON:
                        m_uiOmnotronGUID[1] = creature->GetGUID();
                        break;
                    case NPC_MAGMATRON:
                        m_uiOmnotronGUID[2] = creature->GetGUID();
                        break;
                    case NPC_TOXITRON:
                        m_uiOmnotronGUID[3] = creature->GetGUID();
                        break;
                    case NPC_MALORIAK:
                        uiMaloriakGUID = creature->GetGUID();
                        break;
                    case NPC_CHIMAERON:
                        uiChimaeronGUID = creature->GetGUID();
                        break;
                    case NPC_BILE_O_TRON_800:
                        uiBileotron800GUID = creature->GetGUID();
                        break;
                    case NPC_ATRAMEDES:
                        uiAtramedesGUID = creature->GetGUID();
                        break;
                    case NPC_LORD_VICTOR_NEFARIAN:
                        uiLordNefarianGUID = creature->GetGUID();
                        break;
                    case NPC_NEFARIAN:
                        uiNefarianGUID = creature->GetGUID();
                        break;
                    case NPC_ONYXIA:
                        uiOnyxiaGUID = creature->GetGUID();
                        break;
                    case NPC_ORB_OF_CULMINATION_STALKER:
                        uiOrbOfCulminationStalkerGUID = creature->GetGUID();
                        if (CheckRequiredBosses(DATA_NEFARIAN))
                            creature->CastSpell(creature, SPELL_DRAGON_ORB, true);
                        if (GetBossState(DATA_NEFARIAN) == DONE)
                            creature->SetVisible(false);
                        break;
                }
            }

            void OnGameObjectCreate(GameObject *go) override
            {
                switch (go->GetEntry())
                {
                    case GO_INNER_CHAMBER_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_NEFARIAN_FLOOR:
                        uiNefarianFloorGUID = go->GetGUID();
                        if (GetBossState(DATA_NEFARIAN) == DONE)
                            go->SetGoState(GO_STATE_READY);
                        break;
                    case GO_ORB_OF_CULMINATION:
                        uiOrbOfCulminationGUID = go->GetGUID();
                        if (GetBossState(DATA_NEFARIAN) == DONE)
                            go->SetRespawnTime(WEEK);
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MAGMAW:
                        return uiMagmawGUID;
                    case DATA_MAGMAW_HEAD:
                        return uiMagmawHeadGUID;
                    case DATA_OMNOTRON:
                        return uiOmnotronControllerGUID;
                    case DATA_ARCANOTRON:
                        return m_uiOmnotronGUID[0];
                    case DATA_ELECTRON:
                        return m_uiOmnotronGUID[1];
                    case DATA_MAGMATRON:
                        return m_uiOmnotronGUID[2];
                    case DATA_TOXITRON:
                        return m_uiOmnotronGUID[3];
                    case DATA_MALORIAK:
                        return uiMaloriakGUID;
                    case DATA_CHIMAERON:
                        return uiChimaeronGUID;
                    case DATA_BILE_O_TRON_800:
                        return uiBileotron800GUID;
                    case DATA_ATRAMEDES:
                        return uiAtramedesGUID;
                    case DATA_INNER_CHAMBER_DOOR:
                        return uiInnerChamberDoorGUID;
                    case DATA_NEFARIAN:
                        return uiNefarianGUID;
                    case DATA_ONYXIA:
                        return uiOnyxiaGUID;
                    case DATA_NEFARIAN_FLOOR:
                        return uiNefarianFloorGUID;
                }
                return ObjectGuid::Empty;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_HEALTH_OMNOTRON_SHARED:
                        return omnotronHealth;
                        break;
                    case DATA_MALORIAK_ABERRATIONS:
                        return uiAberCount;
                        break;
                    case DATA_BILE_O_TRON_800:
                        return uiBileotron800Active;
                        break;
                }
                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (CheckRequiredBosses(DATA_NEFARIAN))
                {
                    if (Creature* stalker = instance->GetCreature(uiOrbOfCulminationStalkerGUID))
                    {
                        EncounterState state = GetBossState(DATA_NEFARIAN);
                        if (state == NOT_STARTED || state == FAIL || state == TO_BE_DECIDED)
                            stalker->CastSpell(stalker, SPELL_DRAGON_ORB, true);
                    }
                }

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_HEALTH_OMNOTRON_SHARED:
                        omnotronHealth = data;
                        break;
                    case DATA_MALORIAK_ABERRATIONS:
                        uiAberCount = data;
                        break;
                    case DATA_BILE_O_TRON_800:
                        uiBileotron800Active = data;
                        break;
                }
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (_SkipCheckRequiredBosses(player))
                    return true;
            
                switch (bossId)
                {
                    case DATA_CHIMAERON:
                    case DATA_MALORIAK:
                    case DATA_ATRAMEDES:
                        if (GetBossState(DATA_MAGMAW) != DONE ||
                            GetBossState(DATA_OMNOTRON) != DONE)
                            return false;
                        break;
                    case DATA_NEFARIAN:
                        if (GetBossState(DATA_MAGMAW) != DONE ||
                            GetBossState(DATA_OMNOTRON) != DONE ||
                            GetBossState(DATA_MALORIAK) != DONE ||
                            GetBossState(DATA_CHIMAERON) != DONE ||
                            GetBossState(DATA_ATRAMEDES) != DONE)
                            return false;
                    default:   
                        break;
                }
                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "B D " << GetBossSaveData();

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

                if (dataHead1 == 'B' && dataHead2 == 'D')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                } 
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 omnotronHealth;
                uint32 uiAberCount;
                uint32 uiBileotron800Active;
                ObjectGuid uiMagmawGUID;
                ObjectGuid uiMagmawHeadGUID;
                ObjectGuid uiMaloriakGUID;
                ObjectGuid m_uiOmnotronGUID[4];
                ObjectGuid uiOmnotronControllerGUID;
                ObjectGuid uiChimaeronGUID;
                ObjectGuid uiBileotron800GUID;
                ObjectGuid uiAtramedesGUID;
                ObjectGuid uiInnerChamberDoorGUID;
                ObjectGuid uiLordNefarianGUID;
                ObjectGuid uiNefarianGUID;
                ObjectGuid uiOnyxiaGUID;
                ObjectGuid uiOrbOfCulminationStalkerGUID;
                ObjectGuid uiNefarianFloorGUID;
                ObjectGuid uiOrbOfCulminationGUID;
                uint32 uiTeamInInstance;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
           return new instance_blackwing_descent_InstanceMapScript(map);
        }
};

void AddSC_instance_blackwing_descent()
{
    new instance_blackwing_descent();
}