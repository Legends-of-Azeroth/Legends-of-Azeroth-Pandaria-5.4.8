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
#include "secret_ingridient.h"
#include "ScenarioMgr.h"

class instance_secret_ingridient : public InstanceMapScript
{
    public:
        instance_secret_ingridient() : InstanceMapScript("instance_secret_ingridient", 1157) { }
    
        struct instance_secret_ingridient_InstanceMapScript : public InstanceScript
        {
            instance_secret_ingridient_InstanceMapScript(Map* map) : InstanceScript(map) { }
    
            EventMap events;
            uint32 m_auiEncounter[2];
            uint32 chapterOne, chapterTwo;
            uint32 availableCook;
            uint32 availableCookWhole;
            uint32 eatenCount;
            ObjectGuid ironPawGUID;
            ObjectGuid noodleStandGUID;
            ObjectGuid playerGUID;

            void Initialize() override
            {
                SetBossNumber(2);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne         = 0;
                chapterTwo         = 0;
                ironPawGUID = ObjectGuid::Empty;
                playerGUID = ObjectGuid::Empty;
                availableCook      = 0;
                availableCookWhole = 0;
                noodleStandGUID = ObjectGuid::Empty;
                eatenCount         = 0;

                events.Reset();
            }
    
            void OnPlayerEnter(Player* player) override
            {
                playerGUID = player->GetGUID();
                player->SetFacingTo(1.085341f); // required

                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1157, DATA_TUTORIAL, 0);
                player->CastSpell(player, SPELL_COOKING_NOODLE_TIME, true);

                if (Creature* ironPaw = instance->GetCreature(GetGuidData(NPC_SUNGSHIN_IRONPAW)))
                    ironPaw->AI()->DoAction(ACTION_INTRO);

                events.ScheduleEvent(1, 1000);
            }

            void OnPlayerLeave(Player* player) override
            {
                if (player)
                {
                    player->RemoveAurasDueToSpell(SPELL_VEHICLE_SET_UP);
                    player->RemoveAurasDueToSpell(SPELL_COOKING_NOODLE_TIME);
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SUNGSHIN_IRONPAW:
                        ironPawGUID = creature->GetGUID();
                        break;
                    case NPC_NOODLE_STAND:
                        noodleStandGUID = creature->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_AVAILABLE_COOK_NOODLE:
                        availableCook = data;
                        break;
                    case DATA_AVAILABLE_COOK_WHOLE:
                        availableCookWhole = data;

                        if (Player* target = ObjectAccessor::FindPlayer(GetGuidData(DATA_PLAYER)))
                            sScenarioMgr->SendScenarioState(target, 1157, DATA_NOODLE_TIME, 0);
                        break;
                    case DATA_EATEN_COUNT:
                        eatenCount = data;

                        if (eatenCount >= 15) // Finish Scenario
                        {
                            DoFinishLFGDungeon(745);

                            if (Player* target = ObjectAccessor::FindPlayer(GetGuidData(DATA_PLAYER)))
                                target->KilledMonsterCredit(NPC_INGRIDIENT_CREDIT);
                        }
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_AVAILABLE_COOK_NOODLE:
                        return availableCook;
                    case DATA_AVAILABLE_COOK_WHOLE:
                        return availableCookWhole;
                    case DATA_EATEN_COUNT:
                        return eatenCount;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_PLAYER:
                        return playerGUID;
                    case NPC_SUNGSHIN_IRONPAW:
                        return ironPawGUID;
                    case NPC_NOODLE_STAND:
                        return noodleStandGUID;
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
                        if (Player* passenger = ObjectAccessor::FindPlayer(playerGUID))
                            passenger->CastSpell(noodleSpawnPos.GetPositionX(), noodleSpawnPos.GetPositionY(), noodleSpawnPos.GetPositionZ(), SPELL_NOODLE_STAND, false);
                    }
                    break;
                }
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

                return true;
            }
    
            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S I " << chapterOne << ' ' << chapterTwo;

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

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'S' && dataHead2 == 'I')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_TUTORIAL, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_NOODLE_TIME, chapterTwo);
                }
                else OUT_LOAD_INST_DATA_FAIL;
    
                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_secret_ingridient_InstanceMapScript(map);
        }
};

void AddSC_instance_secret_ingridient()
{
    new instance_secret_ingridient();
}