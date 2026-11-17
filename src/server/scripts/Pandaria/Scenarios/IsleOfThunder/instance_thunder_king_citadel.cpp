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
#include "thunder_king_citadel.h"
#include "ScenarioMgr.h"

class instance_thunder_king_citadel : public InstanceMapScript
{
    public:
        instance_thunder_king_citadel() : InstanceMapScript("instance_thunder_king_citadel", 1135) { }
    
        struct instance_thunder_king_citadel_InstanceScript : public InstanceScript
        {
            uint32 step1;
            uint32 step2;
            uint32 chestCounter;
            ObjectGuid playerGUID;
            ObjectGuid doorGUID;
            uint32 timeData;
            uint32 goldenChestCount;
            std::list<ObjectGuid> firstDoorPackGUIDs;
            std::list<ObjectGuid> secondDoorPackGUIDs;
            std::list<ObjectGuid> burialThroveGUIDs;
            bool hasGoldenChestAchieved;
    
            instance_thunder_king_citadel_InstanceScript(Map* map) : InstanceScript(map) { }
    
            void Initialize() override
            {
                step1            = 0;
                step2            = 0;
                chestCounter     = 0;
                playerGUID = ObjectGuid::Empty;
                doorGUID = ObjectGuid::Empty;
                timeData         = 0;
                goldenChestCount = 0;
                hasGoldenChestAchieved = false;
                firstDoorPackGUIDs.clear();
                secondDoorPackGUIDs.clear();
                burialThroveGUIDs.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!playerGUID)
                    playerGUID = player->GetGUID();

                sScenarioMgr->SendScenarioState(player, 1135, GetData(DATA_SPEAK_WITH_TAOSHI) == DONE ? DATA_REACH_THE_EXIT : DATA_SPEAK_WITH_TAOSHI, 0);

                // Apply time on logout
                if (GetData(DATA_SPEAK_WITH_TAOSHI) == DONE)
                    player->CastSpell(player, 140000, true);
            }
    
            void OnCreatureCreate(Creature* creature) override 
            {
                if (creature->GetEntry() == NPC_CLOUD_TILE_TRAP_BUNNY)
                {
                    creature->SetVisible(false);
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                }
            }
    
            void OnGameObjectCreate(GameObject* go) override 
            { 
                switch (go->GetEntry())
                {
                    case GO_ANCIENT_GATE:
                    case GO_ANCIENT_GATE_2:
                        firstDoorPackGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_ANCIENT_GATE_3:
                    case GO_ANCIENT_GATE_4:
                        secondDoorPackGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_ANCIENT_GATE_6:
                        doorGUID = go->GetGUID();
                        break;
                    case GO_LEI_SHEN_BURIAL_THROVE:
                        burialThroveGUIDs.push_back(go->GetGUID());
                        break;
                }
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
                    case DATA_SPEAK_WITH_TAOSHI:
                        step1 = data;
                        HandleGameObject(GetGuidData(GO_ANCIENT_GATE_6), true, NULL);

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1135, DATA_REACH_THE_EXIT, 0);
                        break;
                    case DATA_REACH_THE_EXIT:
                        step2 = data;
                        DoFinishLFGDungeon(620);
                        break;
                    case DATA_OPEN_FIRST_DOOR_PACK:
                        for (auto&& itr : firstDoorPackGUIDs)
                            HandleGameObject(itr, true, NULL);
                        break;
                    case DATA_OPEN_SECOND_DOOR_PACK:
                        for (auto&& itr : secondDoorPackGUIDs)
                            HandleGameObject(itr, true, NULL);
                        break;
                    case CHEST_DATA:
                        chestCounter += data;

                        if (Unit* target = ObjectAccessor::FindPlayer(GetGuidData(PLAYER_DATA)))
                            target->ModifyPower(POWER_ALTERNATE_POWER, data);
                        break;
                    case TIME_DATA:
                        timeData = data;
                        SaveToDB();
                        break;
                    case GOLDEN_CHEST_DATA:
                        if (++goldenChestCount > 9 && !hasGoldenChestAchieved)
                        {
                            hasGoldenChestAchieved = true;

                            if (Player* scenarioOwner = ObjectAccessor::FindPlayer(GetGuidData(PLAYER_DATA)))
                            {
                                for (uint8 i = 0; i < 10; i++)
                                    scenarioOwner->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_LOOTED_GOLDEN_CHEST_ACHIEV); // actually doesn`t work for unk reason
                            }
                        }
                        break;
                }

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }
    
            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SPEAK_WITH_TAOSHI:
                        return step1;
                    case DATA_REACH_THE_EXIT:
                        return step2;
                    case CHEST_DATA:
                        return chestCounter;
                    case TIME_DATA:
                        return timeData;
                }

                return 0;
            }
    
            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case PLAYER_DATA:
                        return playerGUID;
                    case GO_ANCIENT_GATE_6:
                        return doorGUID;
                }

                return ObjectGuid::Empty;
            }
    
            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;
    
                std::ostringstream saveStream;
                saveStream << "T K C " << step1 << ' ' << step2 << ' ' << timeData;
    
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
    
                if (dataHead1 == 'T' && dataHead2 == 'K' && dataHead3 == 'C')
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    step1 = tmpState;
                    SetData(DATA_SPEAK_WITH_TAOSHI, step1);
                    loadStream >> tmpState;
                    step2 = tmpState;
                    SetData(DATA_REACH_THE_EXIT, step2);
                    loadStream >> tmpState;
                    timeData = tmpState;
                    SetData(TIME_DATA, timeData);
                }
                else OUT_LOAD_INST_DATA_FAIL;
    
                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };
    
        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_thunder_king_citadel_InstanceScript(map);
        }
};

void AddSC_instance_thunder_king_citadel()
{
    new instance_thunder_king_citadel();
}