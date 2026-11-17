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
#include "scarlet_monastery.h"
#include "Log.h"
#include "Containers.h"
#include "LFGMgr.h"
#include "Group.h"

static std::vector<DoorData> const doorData =
{
    { GO_THALNOS_EXIT,       BOSS_THALNOS_THE_SOULRENDER, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    { GO_KORLOFF_EXIT,       BOSS_BROTHER_KORLOFF,        DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_WHITEMANE_ENTRANCE, BOSS_WHITEMANE,              DOOR_TYPE_SPAWN_HOLE, BOUNDARY_NONE }
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { BOSS_THALNOS_THE_SOULRENDER, CRITERIA_THALNOS_THE_SOULRENDER },
    { BOSS_BROTHER_KORLOFF,        CRITERIA_BROTHER_KORLOFF        },
    { BOSS_WHITEMANE,              CRITERIA_DURAND                 },
};

class instance_scarlet_monastery : public InstanceMapScript
{
    public:
        instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 1004) { }

        struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
        {
            instance_scarlet_monastery_InstanceMapScript(Map* map) : InstanceScript(map) { }
            std::set<uint64> guids;
            uint32 lfgDungeon;

            void Initialize() override
            {
                SetBossNumber(encounternumber);
                LoadDoorData(doorData);
                guids.clear();
                lfgDungeon = 0;

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
                if (instance->ToInstanceMap()->IsLFGMap() && !lfgDungeon)
                    if (Player* player = instance->GetFirstPlayerInInstance())
                        lfgDungeon = sLFGMgr->GetDungeon(player->GetGroup() ? player->GetGroup()->GetGUID() : player->GetGUID());

                if (instance->IsChallengeDungeon() && creature->isDead())
                    creature->Respawn();

                switch (creature->GetEntry())
                {
                    case NPC_THALNOS_THE_SOULRENDER:
                        ThalnosGUID = creature->GetGUID();
                        if (lfgDungeon == LFG_DUNGEON_HEADLESS_HORSEMAN)
                        {
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                            creature->SetVisible(false);
                        }
                        break;
                    case NPC_BROTHER_KORLOFF:
                        KorloffGUID = creature->GetGUID();
                        break;
                    case NPC_COMMANDER_DURAND:
                        DurandGUID = creature->GetGUID();
                        break;
                    case NPC_HIGH_INQUISITOR_WHITEMANE:
                        WhitemaneGUID = creature->GetGUID();
                        break;
                    case NPC_FALLEN_CRUSADER:
                        FallenCrusaderGUID = creature->GetGUID();
                        guids.insert(FallenCrusaderGUID);
                        break;
                    case NPC_HOODED_CRUSADER_OUTRO:
                        HoodedGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_SCARLET_JUDICATOR:
                        if (!creature->HasAura(128800)) // drunked at holl must be neutral [set like default]
                            creature->SetFaction(67);
                        break;
                    case NPC_PILE_OF_CORPSES:
                    case NPC_SCARLET_FLAMETHOWER:
                    case NPC_SCARLET_CENTURION:
                    case NPC_ZOMBIFIED_CORPSE:
                    case NPC_HOODED_CRUSADER_INTO:
                    case NPC_FRENZIED_SPIRIT:
                        if (lfgDungeon == LFG_DUNGEON_HEADLESS_HORSEMAN)
                        {
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                            creature->SetVisible(false);
                        }
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_THALNOS_EXIT:
                    case GO_KORLOFF_EXIT:
                        AddDoor(go, true);
                        break;
                    case GO_WHITEMANE_ENTRANCE:
                        AddDoor(go, true);
                        WhitemaneDoorGUID = go->GetGUID();
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;
                if (creature->GetEntry() == NPC_FALLEN_CRUSADER)
                    guids.erase(creature->GetGUID());

                if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                    UpdateConditionInfo(creature, ENEMIES_COUNT);
            }

            void Update(uint32 diff) override
            {
                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case BOSS_THALNOS_THE_SOULRENDER:
                        return ThalnosGUID;
                    case BOSS_BROTHER_KORLOFF:
                        return KorloffGUID;
                    case BOSS_DURAND:
                        return DurandGUID;
                    case BOSS_WHITEMANE:
                        return WhitemaneGUID;
                    case NPC_HOODED_CRUSADER_OUTRO:
                        return HoodedGUID;
                }
                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S C M " << GetBossSaveData();

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

                if (dataHead1 == 'S' && dataHead2 == 'C' && dataHead3 == 'M')
                {
                    for (uint8 i = 0; i < 3; ++i)
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

            bool isWipe()
            {
                for (auto&& itr : instance->GetPlayers())
                    if (Player* plr = itr.GetSource())
                        if (plr->IsAlive() && !plr->IsGameMaster())
                            return false;

                return true;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == BOSS_DURAND && state == SPECIAL)
                {
                    if (Creature* Durand = instance->GetCreature(DurandGUID))
                    {
                        Durand->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Durand->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Durand->SetReactState(REACT_AGGRESSIVE);
                        Durand->SetStandState(UNIT_STAND_STATE_STAND);
                        Durand->SetFullHealth();
                        if (Player* _player = Durand->FindNearestPlayer(VISIBLE_RANGE))
                            Durand->AI()->AttackStart(_player);
                    }
                }
                return true;
            }

        protected:
            ObjectGuid ThalnosGUID;
            ObjectGuid KorloffGUID;
            ObjectGuid DurandGUID;
            ObjectGuid WhitemaneGUID;
            ObjectGuid FallenCrusaderGUID;
            ObjectGuid WhitemaneDoorGUID;
            ObjectGuid HoodedGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_scarlet_monastery_InstanceMapScript(map);
        }
};

void AddSC_instance_scarlet_monastery()
{
    new instance_scarlet_monastery();
}
