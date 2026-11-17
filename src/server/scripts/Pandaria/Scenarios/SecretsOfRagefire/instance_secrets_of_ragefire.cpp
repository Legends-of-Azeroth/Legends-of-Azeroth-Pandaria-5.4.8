/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "secrets_of_ragefire.h"
#include "ScenarioMgr.h"

class instance_secrets_of_ragefire : public InstanceMapScript
{
    public:
        instance_secrets_of_ragefire() : InstanceMapScript("instance_secrets_of_ragefire", 1131) { }

        struct instance_secrets_of_ragefire_InstanceMapScript : public InstanceScript
        {
            instance_secrets_of_ragefire_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            uint32 m_auiEncounter[4];
            uint32 chapterOne, chapterTwo, chapterThree, chapterFour;
            ObjectGuid detonatorGUID;
            ObjectGuid voltGUID;
            ObjectGuid newtGUID;
            ObjectGuid tickerGUID;
            ObjectGuid gritGUID;
            ObjectGuid patchGUID;
            ObjectGuid xorenthGUID;
            ObjectGuid elagloGUID;
            ObjectGuid entranceDoorGUID;
            ObjectGuid teleporterGUID;
            uint32 investigateCount;
            ObjectGuid artifactsGUID;
            ObjectGuid cratesGUID;
            ObjectGuid eggsGUID;
            ObjectGuid playerGUID;
            ObjectGuid teleporterControllerGUID;
            ObjectGuid mantidBombGUID;
            ObjectGuid mantidTargetGUID;
            uint32 batteryCount;
            uint32 yolkCount;
            uint32 cannonBallsCount;
            uint32 ponnyCount;
            std::vector<ObjectGuid> protoDrakeGUIDs;
            std::vector<ObjectGuid> bombCatalystGUIDs;

            void Initialize() override
            {
                SetBossNumber(4);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
                chapterOne       = 0;
                chapterTwo       = 0;
                chapterThree     = 0;
                chapterFour      = 0;
                detonatorGUID = ObjectGuid::Empty;
                voltGUID = ObjectGuid::Empty;
                newtGUID = ObjectGuid::Empty;
                tickerGUID = ObjectGuid::Empty;
                gritGUID = ObjectGuid::Empty;
                patchGUID = ObjectGuid::Empty;
                xorenthGUID = ObjectGuid::Empty;
                elagloGUID = ObjectGuid::Empty;
                entranceDoorGUID = ObjectGuid::Empty;
                teleporterGUID = ObjectGuid::Empty;
                investigateCount = 0;
                artifactsGUID = ObjectGuid::Empty;
                cratesGUID = ObjectGuid::Empty;
                eggsGUID = ObjectGuid::Empty;
                playerGUID = ObjectGuid::Empty;
                batteryCount     = 0;
                yolkCount        = 0;
                cannonBallsCount = 0;
                ponnyCount       = 0;
                mantidBombGUID = ObjectGuid::Empty;
                mantidTargetGUID = ObjectGuid::Empty;
                teleporterControllerGUID = ObjectGuid::Empty;
                events.Reset();
                protoDrakeGUIDs.clear();
                bombCatalystGUIDs.clear();

                events.ScheduleEvent(1, 3500);
                instance->SetWorldState(WORLDSTATE_FEW_PROUD_GOB_SQUAD, 1);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!playerGUID)
                    playerGUID = player->GetGUID();

                player->CastSpell(player, SPELL_GOBLIN_ILLUSION, true);
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1131, DATA_INFILTRATION, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_PATCH:
                        patchGUID = creature->GetGUID();
                        break;
                    case NPC_VOLT:
                        voltGUID = creature->GetGUID();
                        break;
                    case NPC_NEWT:
                        newtGUID = creature->GetGUID();
                        break;
                    case NPC_GRIT:
                        gritGUID = creature->GetGUID();
                        break;
                    case NPC_TICKER:
                        tickerGUID = creature->GetGUID();
                        break;
                    case NPC_DETONATOR:
                        creature->SetVisible(false);
                        detonatorGUID = creature->GetGUID();
                        break;
                    case NPC_DARK_SHAMAN_XORENTH:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                        xorenthGUID = creature->GetGUID();
                        break;
                    case NPC_OVERSEER_ELAGLO:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                        creature->SetVisible(false);
                        elagloGUID = creature->GetGUID();
                        break;
                    case NPC_PROTO_DRAKE_WHELP:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                        protoDrakeGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_PANDARIA_ARTIFACTS:
                        creature->SetVisible(false);
                        artifactsGUID = creature->GetGUID();
                        break;
                    case NPC_PROTO_DRAKE_EGGS:
                        creature->SetVisible(false);
                        eggsGUID = creature->GetGUID();
                        break;
                    case NPC_SUPPLY_CRATES:
                        creature->SetVisible(false);
                        cratesGUID = creature->GetGUID();
                        break;
                    case NPC_BATTERY:
                    case NPC_POOL_PONY:
                    case NPC_BROKEN_DRAKE_EGG:
                    case NPC_CANNON_BALLS:
                        creature->SetVisible(false);
                        bombCatalystGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_EMERGENCY_TELEPORTER:
                        teleporterControllerGUID = creature->GetGUID();
                        break;
                    case NPC_MANTID_BOMB:
                        mantidBombGUID = creature->GetGUID();
                        break;
                    case NPC_MANTID_BOMB_TARGET:
                        mantidTargetGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_IRON_GATE:
                        entranceDoorGUID = go->GetGUID();
                        break;
                    case GO_EMERGENCY_TELEPORTER:
                        teleporterGUID = go->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_INFILTRATION:
                        chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1131, DATA_RECONNAISSANCE, 0);

                        if (Creature* grit = instance->GetCreature(GetGuidData(NPC_GRIT)))
                            grit->AI()->DoAction(ACTION_RECONNAISSANCE);

                        // Activate items for investigate
                        for (auto&& itr : investigatedItems)
                        {
                            if (Creature* exploreItem = instance->GetCreature(GetGuidData(itr.first)))
                            {
                                exploreItem->SetVisible(true);
                                exploreItem->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }
                        }
                        break;
                    case DATA_RECONNAISSANCE:
                        if (++investigateCount > 2)
                        {
                            chapterTwo = DONE;

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1131, DATA_SEALED_GATE, 0);

                            if (Creature* grit = instance->GetCreature(GetGuidData(NPC_GRIT)))
                                grit->AI()->DoAction(ACTION_SEALED_GATES);
                        }
                        break;
                    case DATA_SEALED_GATE:
                        chapterThree = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1131, DATA_HOLDOUT, 0);

                        if (Creature* grit = instance->GetCreature(GetGuidData(NPC_GRIT)))
                            grit->AI()->DoAction(ACTION_EXPLOSIVE_GATES);
                        break;
                    case DATA_HOLDOUT:
                        chapterFour = data;

                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_GOB_SQUAD_DEFENDED, 1, GetScenarioGUID(), time(NULL), 0, 0));

                        if (Creature* grit = instance->GetCreature(GetGuidData(NPC_GRIT)))
                            grit->AI()->DoAction(ACTION_GOB_SQUAD_DEFENDED);

                        DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_HEROIC ? 625 : 649);
                        DoFinishLFGDungeon(instance->GetDifficulty() == SCENARIO_DIFFICULTY_NORMAL ? 625 : 649);
                        break;
                    case PROTDRAKES_DATA:
                        // Announce
                        if (Creature* elagro = instance->GetCreature(GetGuidData(NPC_OVERSEER_ELAGLO)))
                            elagro->AI()->Talk(TALK_INTRO);

                        for (auto&& itr : protoDrakeGUIDs)
                        {
                            if (Creature* protoDrake = instance->GetCreature(itr))
                            {
                                protoDrake->SetVisible(true);
                                protoDrake->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                                protoDrake->SetInCombatWithZone();
                            }
                        }
                        break;
                    case DATA_ALLOW_BRING_ITEMS:
                        for (auto&& itr : bombCatalystGUIDs)
                        {
                            if (Creature* catalystItem = instance->GetCreature(itr))
                            {
                                catalystItem->SetVisible(true);
                                catalystItem->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            }
                        }
                        break;
                    case CRITERIA_BATTERY:
                        batteryCount = data;
                        break;
                    case CRITERIA_POOL_PONY:
                        ponnyCount = data;
                        break;
                    case CRITERIA_EGGS:
                        yolkCount = data;
                        break;
                    case CRITERIA_CANNON_BALLS:
                        cannonBallsCount = data;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_INFILTRATION:
                        return chapterOne;
                    case DATA_RECONNAISSANCE:
                        return chapterTwo;
                    case DATA_SEALED_GATE:
                        return chapterThree;
                    case DATA_HOLDOUT:
                        return chapterFour;
                    case CRITERIA_BATTERY:
                        return batteryCount;
                    case CRITERIA_POOL_PONY:
                        return ponnyCount;
                    case CRITERIA_EGGS:
                        return yolkCount;
                    case CRITERIA_CANNON_BALLS:
                        return cannonBallsCount;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_PATCH:
                        return patchGUID;
                    case NPC_VOLT:
                        return voltGUID;
                    case NPC_NEWT:
                        return newtGUID;
                    case NPC_GRIT:
                        return gritGUID;
                    case NPC_TICKER:
                        return tickerGUID;
                    case NPC_DETONATOR:
                        return detonatorGUID;
                    case NPC_DARK_SHAMAN_XORENTH:
                        return xorenthGUID;
                    case NPC_OVERSEER_ELAGLO:
                        return elagloGUID;
                    case GO_IRON_GATE:
                        return entranceDoorGUID;
                    case GO_EMERGENCY_TELEPORTER:
                        return teleporterGUID;
                    case NPC_PANDARIA_ARTIFACTS:
                        return artifactsGUID;
                    case NPC_PROTO_DRAKE_EGGS:
                        return eggsGUID;
                    case NPC_SUPPLY_CRATES:
                        return cratesGUID;
                    case PLAYER_DATA:
                        return playerGUID;
                    case NPC_EMERGENCY_TELEPORTER:
                        return teleporterControllerGUID;
                    case NPC_MANTID_BOMB:
                        return mantidBombGUID;
                    case NPC_MANTID_BOMB_TARGET:
                        return mantidTargetGUID;
                }

                return ObjectGuid::Empty;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == 1)
                    {
                        if (Creature* grit = instance->GetCreature(GetGuidData(NPC_GRIT)))
                            grit->AI()->DoAction(ACTION_START_INTRO);
                    }
                    break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S O R " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << chapterFour;

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

                if (dataHead1 == 'S' && dataHead2 == 'O'&& dataHead3 == 'R')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_INFILTRATION, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_RECONNAISSANCE, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_SEALED_GATE, chapterThree);
                    loadStream >> temp; // chapterFour complete
                    chapterFour = temp;
                    SetData(DATA_HOLDOUT, chapterFour);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_secrets_of_ragefire_InstanceMapScript(map);
        }
};

void AddSC_instance_secrets_of_ragefire()
{
    new instance_secrets_of_ragefire();
}