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
#include "pit_of_saron.h"

// position for Jaina and Sylvanas
Position const EventLeaderPos2   = { 1054.368f, 107.14620f, 628.4467f, 0.0f };

// Slaves For Alliance and Horde. Martin Victus and Gorkun Ironskull 
const uint32 NpcSlaveAlliance[3] = { 37591, 37572, 37575 };
const uint32 NpcSlaveHorde[3]    = { 37592, 37578, 37579 };

// Dead Garfrost
static const Position SlaveLeaderPos[3] =
{
    { 693.281555f, -169.690872f, 526.965454f, 1.485173f },
    { 696.024902f, -169.953308f, 526.870850f, 1.603771f },
    { 690.887512f, -169.970963f, 526.891357f, 1.269191f },
};

class instance_pit_of_saron : public InstanceMapScript
{
    public:
        instance_pit_of_saron() : InstanceMapScript(PoSScriptName, 658) { }

        struct instance_pit_of_saron_InstanceScript : public InstanceScript
        {
            instance_pit_of_saron_InstanceScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);

                _garfrostGUID = ObjectGuid::Empty;
                _krickGUID = ObjectGuid::Empty;
                _ickGUID = ObjectGuid::Empty;
                _tyrannusGUID = ObjectGuid::Empty;
                _rimefangGUID = ObjectGuid::Empty;
                _jainaOrSylvanas1GUID = ObjectGuid::Empty;
                _jainaOrSylvanas2GUID = ObjectGuid::Empty;
                _teamInInstance = 0;

                _dontLookUpEligible = true;
                instance->SetWorldState(WORLDSTATE_DONT_LOOK_UP, 1);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!_teamInInstance)
                    _teamInInstance = player->GetTeam();
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!_teamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            _teamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_GARFROST:
                        _garfrostGUID = creature->GetGUID();
                        break;
                    case NPC_KRICK:
                        _krickGUID = creature->GetGUID();
                        break;
                    case NPC_ICK:
                        _ickGUID = creature->GetGUID();
                        break;
                    case NPC_TYRANNUS:
                        _tyrannusGUID = creature->GetGUID();
                        break;
                    case NPC_RIMEFANG:
                        _rimefangGUID = creature->GetGUID();
                        break;
                    case NPC_TYRANNUS_EVENTS:
                        _tyrannusEventGUID = creature->GetGUID();
                        break;
                    case NPC_SYLVANAS_PART1:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JAINA_PART1, ALLIANCE);
                        _jainaOrSylvanas1GUID = creature->GetGUID();
                        break;
                    case NPC_SYLVANAS_PART2:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JAINA_PART2, ALLIANCE);
                        _jainaOrSylvanas2GUID = creature->GetGUID();
                        break;
                    case NPC_KILARA:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ELANDRA, ALLIANCE);
                        break;
                    case NPC_KORALEN:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_KORLAEN, ALLIANCE);
                        break;
                    case NPC_CHAMPION_1_HORDE:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_1_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_CHAMPION_2_HORDE:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_2_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_CHAMPION_3_HORDE: // No 3rd set for Alliance?
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_2_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_1:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_1, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_2:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_2, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_3:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_3, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_4:
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_4, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_1_HORDE:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_1_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_2_HORDE:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_2_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_3_HORDE:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_3_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_RESCUED_SLAVE_HORDE:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_RESCUED_SLAVE_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_MARTIN_VICTUS_1:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MARTIN_VICTUS_1, ALLIANCE);
                        break;
                    case NPC_MARTIN_VICTUS_2:
                        if (_teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MARTIN_VICTUS_2, ALLIANCE);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                case GO_ICE_WALL:
                    uiIceWall = go->GetGUID();
                    if (GetBossState(DATA_GARFROST) == DONE && GetBossState(DATA_ICK) == DONE)
                        HandleGameObject(ObjectGuid::Empty, true, go);
                    break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_ICK:
                        switch (state)
                        {
                            case DONE:
                            //if (Creature* summoner = instance->GetCreature(_garfrostGUID))
                            //{
                            //if (_teamInInstance == ALLIANCE)
                            //    summoner->SummonCreature(NPC_JAINA_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                            //else
                            //        summoner->SummonCreature(NPC_SYLVANAS_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                            //}
                            if (GetBossState(DATA_GARFROST) == DONE)
                                HandleGameObject(uiIceWall, true, NULL);
                        }
                        break;
                    case DATA_GARFROST:
                        switch (state)
                        {
                            case DONE:
                                if (Creature* summoner = instance->GetCreature(_garfrostGUID))
                                {
                                    for (int i = 0; i < 3; ++i)
                                    {
                                        if (_teamInInstance == ALLIANCE)
                                            summoner->SummonCreature(NpcSlaveAlliance[i], SlaveLeaderPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                                        else
                                            summoner->SummonCreature(NpcSlaveHorde[i], SlaveLeaderPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                                    }
                                }
                                if (GetBossState(DATA_ICK)==DONE)
                                    HandleGameObject(uiIceWall, true, NULL);
                        }
                        break;
                    case DATA_TYRANNUS:
                        if (state == DONE)
                        {
                            if (Creature* summoner = instance->GetCreature(_tyrannusGUID))
                            {
                                if (_teamInInstance == ALLIANCE)
                                    summoner->SummonCreature(NPC_JAINA_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                                else
                                    summoner->SummonCreature(NPC_SYLVANAS_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                            }
                        }
                        break;
                    default:
                        break;
                }
                return true;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_CAVE_GUARDIAN_SPAWN:
                        if (_dontLookUpEligible)
                            _caveGuardianSet.insert(data);
                        break;
                    case DATA_CAVE_GUARDIAN_KILL:
                        if (_dontLookUpEligible)
                        {
                            _caveGuardianSet.erase(data);
                            // Killed last cave guardian
                            if (_caveGuardianSet.empty())
                            {
                                DistributeDontLookUpAchievement(data);
                                _dontLookUpEligible = false;
                            }
                        }
                        break;
                    case DATA_DONT_LOOK_UP_FAIL:
                        _dontLookUpEligible = false;
                        instance->SetWorldState(WORLDSTATE_DONT_LOOK_UP, 0);
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_TEAM_IN_INSTANCE:
                        return _teamInInstance;
                    default:
                        break;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GARFROST:
                        return _garfrostGUID;
                    case DATA_KRICK:
                        return _krickGUID;
                    case DATA_ICK:
                        return _ickGUID;
                    case DATA_TYRANNUS:
                        return _tyrannusGUID;
                    case DATA_RIMEFANG:
                        return _rimefangGUID;
                    case DATA_TYRANNUS_EVENT:
                        return _tyrannusEventGUID;
                    case DATA_JAINA_SYLVANAS_1:
                        return _jainaOrSylvanas1GUID;
                    case DATA_JAINA_SYLVANAS_2:
                        return _jainaOrSylvanas2GUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "P S " << GetBossSaveData();

                saveStream << " " << _dontLookUpEligible;

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

                if (dataHead1 == 'P' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }
                    loadStream >> _dontLookUpEligible;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void DistributeDontLookUpAchievement(ObjectGuid guardianGUID)
            {
                Map::PlayerList const& players = instance->GetPlayers();
                if (players.isEmpty())
                    return;

                // First cycle for radius check
                for (auto&& ref : players)
                {
                    if (Player* player = ref.GetSource())
                    {
                        // GM is here
                        if (player->IsGameMaster())
                            continue;

                        // Get last killed guardian
                        Creature* guardian = ObjectAccessor::GetCreature(*player, guardianGUID);
                        if (!guardian || !guardian->IsWithinDist(player, 40.0f))                             // All players must be in 40 yards radius from last guardian
                            return;
                    }
                }

                // Second cycle for distribute achievement
                for (auto&& ref : players)
                {
                    if (Player* player = ref.GetSource())
                    {
                        // GM is here
                        if (player->IsGameMaster())
                            continue;

                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 72845);
                    }
                }
            }

        private:
            ObjectGuid _garfrostGUID;
            ObjectGuid _krickGUID;
            ObjectGuid _ickGUID;
            ObjectGuid _tyrannusGUID;
            ObjectGuid _rimefangGUID;

            ObjectGuid _tyrannusEventGUID;
            ObjectGuid _jainaOrSylvanas1GUID;
            ObjectGuid _jainaOrSylvanas2GUID;
            ObjectGuid uiIceWall;

            uint32 _teamInInstance;

            std::set<ObjectGuid> _caveGuardianSet;
            bool _dontLookUpEligible;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_pit_of_saron_InstanceScript(map);
        }
};

void AddSC_instance_pit_of_saron()
{
    new instance_pit_of_saron();
}
