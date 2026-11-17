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
#include "VMapFactory.h"
#include "arena_of_annihiliation.h"
#include "ScenarioMgr.h"

class instance_arena_of_annihiliation : public InstanceMapScript
{
    public:
        instance_arena_of_annihiliation() : InstanceMapScript("instance_arena_of_annihiliation", 1031) { }

        struct instance_arena_of_annihiliation_InstanceMapScript : public InstanceScript
        {
            instance_arena_of_annihiliation_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            ObjectGuid gongGUID, templeDoorGUID;
            uint32 m_auiEncounter[CHAPTERS];
            std::map<uint32, ObjectGuid> m_ArenaEncounters;
            std::map<uint32, uint32> m_GurthockDefeats;
            ObjectGuid makiGUID;
            bool hasAssassinActivated;
            bool hasChaganActivated;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                m_ArenaEncounters.clear();
                m_GurthockDefeats.clear();

                m_GurthockDefeats.insert(std::pair<uint32, uint32>(DATA_SCAR_SHELL, TALK_SCAR_SHELL_DEFEAT));
                m_GurthockDefeats.insert(std::pair<uint32, uint32>(DATA_JOL_GRUM,   TALK_JOL_GRUM_DEFEAT));
                m_GurthockDefeats.insert(std::pair<uint32, uint32>(DATA_LIUYANG,    TALK_LIUYANG_DEFEAT));
                m_GurthockDefeats.insert(std::pair<uint32, uint32>(DATA_CHAGAN,     TALK_CHAGAN_DEFEAT));
                m_GurthockDefeats.insert(std::pair<uint32, uint32>(DATA_ASSASSIN,   TALK_ASSASSIN_DEFEAT));
                makiGUID = ObjectGuid::Empty;
                hasAssassinActivated = false;
                hasChaganActivated   = false;
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1031, DATA_SCAR_SHELL - 1, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SCAR_SHELL:
                    case NPC_JOL_GRUM:
                    case NPC_LITTLE_LIUYANG:
                    case NPC_CHAGAN_FIREHOOF:
                    case NPC_CLOUDBENDER_KOBO:
                    case NPC_SATAY_BYU:
                        m_ArenaEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        creature->SetVisible(false);
                        creature->SetReactState(REACT_PASSIVE);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        break;
                    case NPC_GURGTHOCK:
                    case NPC_WODIN_TROLL_SERVANT:
                        m_ArenaEncounters.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_WORLD_INV_TRIGGER:
                        creature->SetDisplayId(11686);
                        break;
                    case NPC_MAKI_WATERBLADE:
                        makiGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_TIGER_TEMPLE_GONG:
                        gongGUID = go->GetGUID();
                        break;
                    case GO_TIGER_TEMPLE_DOOR:
                        templeDoorGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_CALL) // this data should call new boss
                    ActivateNewBossType(GetEncountersDoneCount(CHAPTERS));

                if (data == DONE)
                    SaveToDB();
            }

            void ActivateNewBossType(uint32 m_data)
            {
                switch (m_data)
                {
                    case DATA_SCAR_SHELL - 1: // begun
                    {
                        if (Creature* ScarShell = instance->GetCreature(GetGuidData(NPC_SCAR_SHELL)))
                            ScarShell->AI()->DoAction(ACTION_INTRO);

                        if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                            Gurgthock->AI()->Talk(TALK_SCAR_SHELL_INTRO);

                        break;
                    }
                    case DATA_SCAR_SHELL:
                    {
                        if (Creature* JolGrum = instance->GetCreature(GetGuidData(NPC_JOL_GRUM)))
                            JolGrum->AI()->DoAction(ACTION_INTRO);

                        if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                            Gurgthock->AI()->Talk(TALK_JOL_GRUM_INTRO);

                        break;
                    }
                    case DATA_JOL_GRUM:
                    {
                        if (Creature* Liuyang = instance->GetCreature(GetGuidData(NPC_LITTLE_LIUYANG)))
                            Liuyang->AI()->DoAction(ACTION_INTRO);

                        if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                            Gurgthock->AI()->Talk(TALK_LIUYANG_INTRO);

                        break;
                    }
                    case DATA_LIUYANG:
                    {
                        if (hasChaganActivated)
                            break;

                        hasChaganActivated = true;

                        if (Creature* Chagan = instance->GetCreature(GetGuidData(NPC_CHAGAN_FIREHOOF)))
                            Chagan->AI()->DoAction(ACTION_INTRO);

                        if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                            Gurgthock->AI()->Talk(TALK_CHAGAN_INTRO);

                        break;
                    }
                    case DATA_CHAGAN:
                    {
                        if (hasAssassinActivated)
                            break;

                        hasAssassinActivated = true;

                        std::vector<uint32> assassinChallengers = { NPC_SATAY_BYU, NPC_CLOUDBENDER_KOBO, NPC_MAKI_WATERBLADE};
                        uint32 assassinChallenger = Trinity::Containers::SelectRandomContainerElement(assassinChallengers);

                        if (Creature* Gurghock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                        {
                            Gurghock->AI()->Talk(TALK_ASSASSIN_INTRO);

                            if (Creature* assassin = instance->GetCreature(GetGuidData(assassinChallenger)))
                                assassin->AI()->DoAction(ACTION_INTRO);
                            else if (Creature* maki = Gurghock->SummonCreature(assassinChallenger, makiSpawnPos, TEMPSUMMON_MANUAL_DESPAWN)) // in case if maki was select
                                maki->AI()->DoAction(ACTION_INTRO);
                        }

                        break;
                    }
                }

                if (GameObject* m_door = instance->GetGameObject(templeDoorGUID))
                    m_door->UseDoorOrButton(5 * IN_MILLISECONDS);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type)
                    return GetBossState(type);

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_SCAR_SHELL:
                    case NPC_JOL_GRUM:
                    case NPC_LITTLE_LIUYANG:
                    case NPC_CHAGAN_FIREHOOF:
                    case NPC_CLOUDBENDER_KOBO:
                    case NPC_SATAY_BYU:
                    case NPC_GURGTHOCK:
                    case NPC_WODIN_TROLL_SERVANT:
                        return m_ArenaEncounters.find(type)->second;
                    case NPC_MAKI_WATERBLADE:
                        return makiGUID;
                }

                return ObjectGuid::Empty;
            }

            bool IsWipe(float range, Unit* source) override
            {
                Map::PlayerList const &playerList = instance->GetPlayers();

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    Player* player = itr->GetSource();
                    if (!player)
                        continue;

                    if (player->IsAlive() && !player->IsGameMaster())
                        return false;
                }

                return true;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state == DONE)
                {
                    switch (type)
                    {
                        case DATA_SCAR_SHELL:
                        case DATA_JOL_GRUM:
                        case DATA_LIUYANG:
                        case DATA_CHAGAN:
                            if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                                Gurgthock->AI()->Talk(m_GurthockDefeats.find(type)->second);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1031, type, 0);

                            events.ScheduleEvent(1, 2500);

                            if (GameObject* gong = instance->GetGameObject(gongGUID))
                                gong->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                            break;
                        case DATA_ASSASSIN:
                            if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                                Gurgthock->AI()->Talk(m_GurthockDefeats.find(type)->second);

                            events.ScheduleEvent(2, 2500);

                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_LAST_CHALLENGER, 1, GetScenarioGUID(), time(NULL), 0, 0));

                            DoFinishLFGDungeon(511);
                            break;
                    }
                }

                return true;
            }

            void Update(uint32 diff) override 
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case 1:
                            if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                                Gurgthock->AI()->Talk(TALK_NEW_CHALLENGE);
                            break;
                        case 2:
                            if (Creature* Gurgthock = instance->GetCreature(GetGuidData(NPC_GURGTHOCK)))
                                Gurgthock->AI()->Talk(TALK_DONE);

                            if (Creature* Wodin = instance->GetCreature(GetGuidData(NPC_WODIN_TROLL_SERVANT)))
                            {
                                Wodin->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                                Wodin->GetMotionMaster()->MoveJump(Wodin->GetPositionX() - 2.0f, Wodin->GetPositionY(), 639.69f, 5.0f, 3.0f, EVENT_JUMP);
                            }
                            break;
                    }
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "A O A " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* in) override
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

                if (dataHead1 == 'A' && dataHead2 == 'O' && dataHead3=='A')
                {
                    for (uint8 i = 0; i < CHAPTERS; ++i)
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
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_arena_of_annihiliation_InstanceMapScript(map);
        }
};

void AddSC_instance_instance_arena_of_annihiliation()
{
    new instance_arena_of_annihiliation();
}