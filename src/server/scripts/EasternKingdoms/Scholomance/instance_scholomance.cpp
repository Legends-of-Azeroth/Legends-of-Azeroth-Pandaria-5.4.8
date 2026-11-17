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
#include "scholomance.h"
#include "Random.h"

#define MAX_ENCOUNTER 5

static std::vector<DoorData> const doorData =
{
    { GO_CHILLHEART_EXIT_DOOR,       DATA_INSTRUCTOR_CHILLHEART, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_JANDICE_INTRO_DOOR,         DATA_JANDICE_BAROV,         DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_JANDICE_TO_RATTLEGORE_DOOR, DATA_JANDICE_BAROV,         DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_JANDICE_TO_RATTLEGORE_DOOR, DATA_RATTLEGORE,            DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_RATTLEGORE_EXIT_DOOR,       DATA_RATTLEGORE,            DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_LILIAN_INTRO_DOOR,          DATA_LILIAN_VOSS,           DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_LILIAN_CHEST_DOOR,          DATA_LILIAN_VOSS,           DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_LILIAN_TO_GANDLING_DOOR,    DATA_LILIAN_VOSS,           DOOR_TYPE_PASSAGE, BOUNDARY_NONE},
    { GO_GANDLING_PRE_INTRO_DOOR,    DATA_GRANDMASTER_GANDLING,  DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_GANDLING_INTRO_DOOR,        DATA_GRANDMASTER_GANDLING,  DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { DATA_INSTRUCTOR_CHILLHEART, CRITERIA_INSTRUCTOR_CHILLHEART },
    { DATA_JANDICE_BAROV,         CRITERIA_JANDICE_BAROV         },
    { DATA_RATTLEGORE,            CRITERIA_RATTLEGORE            },
    { DATA_LILIAN_VOSS,           CRITERIA_LILIAN_VOSS           },
    { DATA_GRANDMASTER_GANDLING,  CRITERIA_GRANDMASTER_GANDLING  },
};

class instance_scholomance : public InstanceMapScript
{
    public:
        instance_scholomance() : InstanceMapScript("instance_scholomance", 1007) { }

        struct instance_scholomance_InstanceMapScript : public InstanceScript
        {
            instance_scholomance_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_ENCOUNTER];
            std::unordered_map<uint32, ObjectGuid> m_mGoEntryGuidMap;

            void Initialize() override
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

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

                switch (creature->GetEntry())
                {
                    case NPC_INSTRUCTOR_CHILLHEART:
                        ChillHeartGUID = creature->GetGUID();
                        break;
                    case NPC_JANDICE_BAROV:
                        JandiceBarovGUID = creature->GetGUID();
                        break;
                    case NPC_RATTLEGORE:
                        RattleGoreGUID = creature->GetGUID();
                        break;
                    case NPC_LILIAN_VOSS:
                        LilianVossGUID = creature->GetGUID();
                        break;
                    case NPC_GRANDMASTER_GANDLING_2:
                        GandlingGUID = creature->GetGUID();
                        break;
                    case NPC_GRANDMASTER_GANDLING_1:
                        GandlingIntroGUID = creature->GetGUID();
                        break;
                    case NPC_BONE_PILE:
                        creature->SetVisible(false);
                        break;
                    case NPC_THEOLEN_KRASTINOV:
                        if (!instance->IsHeroic() || (instance->IsHeroic() && !roll_chance_i(10)))
                        {
                            creature->SetVisible(false); // make spawm with 10% proc
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        }
                        break;
                    case NPC_TALKING_SKULL:
                        SkullGUID = creature->GetGUID();
                        break;
                    case NPC_LILIAN_SOUL:
                        LilianSoulGUID = creature->GetGUID();
                        break;
                }
            }

            GameObject* GetGameObjectFromStorage(uint32 uiEntry)
            {
                GameObject* go = NULL;

                std::unordered_map<uint32, ObjectGuid>::iterator find = m_mGoEntryGuidMap.find(uiEntry);

                if (find != m_mGoEntryGuidMap.cend())
                    go = instance->GetGameObject(find->second);

                return go;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_CHILLHEART_INTRO_DOOR:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_ALCHEMY_BOTTLE_WHITE:
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_CHILLHEART_EXIT_DOOR:
                    case GO_JANDICE_INTRO_DOOR:
                    case GO_JANDICE_TO_RATTLEGORE_DOOR:
                    case GO_RATTLEGORE_EXIT_DOOR:
                    case GO_LILIAN_INTRO_DOOR:
                    case GO_LILIAN_CHEST_DOOR:
                    case GO_LILIAN_TO_GANDLING_DOOR:
                    case GO_GANDLING_PRE_INTRO_DOOR:
                    case GO_GANDLING_INTRO_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_COFFER_OF_FORGOTTEN_SOULS:
                    case GO_COFFER_OF_FORGOTTEN_SOULS_HC:
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
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
                        UpdateConditionInfo(creature,  ENEMIES_COUNT);
            }
            void Update(uint32 diff) override
            {
                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }


            void SetData(uint32 type, uint32 data) override 
            {
                m_auiEncounter[type] = data;
                SaveToDB();
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_INSTRUCTOR_CHILLHEART:
                        return ChillHeartGUID;
                    case DATA_JANDICE_BAROV:
                        return JandiceBarovGUID;
                    case DATA_RATTLEGORE:
                        return RattleGoreGUID;
                    case DATA_LILIAN_VOSS:
                        return LilianVossGUID;
                    case DATA_GRANDMASTER_GANDLING:
                        return GandlingGUID;
                    case DATA_GRANDMASTER_GANDLING_INTRO:
                        return GandlingIntroGUID;
                    case NPC_TALKING_SKULL:
                        return SkullGUID;
                    case NPC_LILIAN_SOUL:
                        return LilianSoulGUID;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state == DONE)
                {
                    switch (type)
                    {
                        case DATA_INSTRUCTOR_CHILLHEART:
                            HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GO_CHILLHEART_INTRO_DOOR));

                            if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->AI()->Talk(TALK_CHILLHEART_DEATH);
                            break;
                        case DATA_JANDICE_BAROV:
                            if (Creature* Gandling = instance->GetCreature(GetGuidData(DATA_GRANDMASTER_GANDLING_INTRO)))
                                Gandling->AI()->DoAction(ACTION_INTRO);

                            if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->AI()->Talk(TALK_JANDICE_DEATH);
                            break;
                        case DATA_RATTLEGORE:
                            if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->AI()->Talk(TALK_RATTLEGORE_DEATH);
                            break;
                        case DATA_LILIAN_VOSS:
                            if (GameObject* Chest = GetGameObjectFromStorage(instance->IsHeroic() ? GO_COFFER_OF_FORGOTTEN_SOULS_HC : GO_COFFER_OF_FORGOTTEN_SOULS))
                                Chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                            if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->AI()->Talk(TALK_LILIAN_DEFEAT);
                            break;
                        case DATA_GRANDMASTER_GANDLING:

                            if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->AI()->DoAction(ACTION_ALEXEI);
                            break;
                    }

                    if (Creature* TalkingSkull = instance->GetCreature(GetGuidData(NPC_TALKING_SKULL)))
                        TalkingSkull->AI()->DoAction(ACTION_COUNTINUE_MOVE);
                }
                return true;
            }

            std::string GetSaveData() override
            {
                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2] << ' ' << m_auiEncounter[3] << ' ' << m_auiEncounter[4];
                return saveStream.str();
            }

            bool isWipe()
            {
                Map::PlayerList const& PlayerList = instance->GetPlayers();

                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* plr = i->GetSource())
                            if (plr->IsAlive() && !plr->IsGameMaster())
                                return false;
                    }
                }
                return true;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);
                std::istringstream loadStream(in);

                loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4];

                // Do not load an encounter as "In Progress" - reset it instead.
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                    SetBossState(i, EncounterState(m_auiEncounter[i]));
                }

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        protected:
            ObjectGuid ChillHeartGUID;
            ObjectGuid JandiceBarovGUID;
            ObjectGuid RattleGoreGUID;
            ObjectGuid LilianVossGUID;
            ObjectGuid GandlingGUID;
            ObjectGuid GandlingIntroGUID;
            ObjectGuid LilianSoulGUID;
            ObjectGuid SkullGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_scholomance_InstanceMapScript(map);
        }
};

void AddSC_instance_scholomance()
{
    new instance_scholomance();
}