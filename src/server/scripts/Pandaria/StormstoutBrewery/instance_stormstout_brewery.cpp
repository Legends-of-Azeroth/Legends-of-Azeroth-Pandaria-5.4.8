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
#include "stormstout_brewery.h"

static std::vector<DoorData> const doorData =
{
    { GO_OOK_EXIT_DOOR,   DATA_OOK_OOK,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_HOPTAL_ENTRANCE, DATA_HOPTALLUS, DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_YANJU_ENTRANCE,  DATA_YAN_ZHU,   DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

// sudsy brew wall of suds
// fizzy brew carbonation
// blackout brew blackout
// bloating brew bloat
// yeasty brew yeasty adds
// bubbling brew bubble shield

struct AddSpellPair
{
    uint32 entry_1;
    uint32 entry_2;
    uint32 spell_1;
    uint32 spell_2;
};

static const AddSpellPair yanzhuPairs[3] =
{
    { NPC_YEASTY_ALEMENTAL,  NPC_BUBBLING_ALEMENTAL, SPELL_YEASTY_BREW,   SPELL_BUBBLING_BREW },
    { NPC_FIZZY_ALEMENTAL,   NPC_SUDSY_ALEMENTAL,    SPELL_FIZZY_BREW,    SPELL_SUDSY_BREW    },
    { NPC_BLOATED_ALEMENTAL, NPC_STOUT_ALEMENTAL,    SPELL_BLOATING_BREW, SPELL_BLACKOUT_BREW }
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { DATA_OOK_OOK,   CRITERIA_OOK_OOK   },
    { DATA_HOPTALLUS, CRITERIA_HOPTALLUS },
    { DATA_YAN_ZHU,   CRITERIA_YAN_ZHU   },
};

static const Position ookOokDoorPos = { -766.863f, 1391.67f, 146.739f, 0.298219f };

class instance_stormstout_brewery : public InstanceMapScript
{
    public:
        instance_stormstout_brewery() : InstanceMapScript("instance_stormstout_brewery", 961) { }

        struct instance_stormstout_brewery_InstanceMapScript : public InstanceScript
        {
            instance_stormstout_brewery_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            std::unordered_map<uint32, uint32> yanzhuAuraMap;
            std::vector<ObjectGuid> hozenGuidsVector;
            std::vector<ObjectGuid> bouncerGuidsVector;
            std::list<Player*> payersInList;
            ObjectGuid okOokGUID, hoptallusGUID, yanzhuGUID, ookOokDoorGUID, uncleGaoGUID, chenYanzhuGUID;
            uint32 hozenSlain;

            void Initialize() override
            {
                InitializeYanzhuAdds(2);

                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);

                payersInList.clear();
                hozenSlain     = 0;
                okOokGUID = ObjectGuid::Empty;
                hoptallusGUID = ObjectGuid::Empty;
                yanzhuGUID = ObjectGuid::Empty;
                uncleGaoGUID = ObjectGuid::Empty;
                chenYanzhuGUID = ObjectGuid::Empty;

                events.ScheduleEvent(1, 10000);
                events.ScheduleEvent(2, 10000);
                events.ScheduleEvent(3, 10000);

                if (instance->IsChallengeDungeon())
                    LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (instance->IsChallengeDungeon())
                    SendChallengeInfo(player, SCENARIO_ID);

                if (GetData(DATA_HOZEN_SLAIN) >= 40)
                    return;

                if (!player->HasAura(SPELL_BANANA_BAR))
                    player->CastSpell(player, SPELL_BANANA_BAR, false);
                else // if we had server crash, then need remove old bar
                {
                    player->RemoveAurasDueToSpell(SPELL_BANANA_BAR);
                    player->CastSpell(player, SPELL_BANANA_BAR, false);
                }

                if (hozenSlain > 0)
                    player->SetPower(POWER_ALTERNATE_POWER, hozenSlain);
            }

            bool InitializeYanzhuAdds(int n)
            {
                // Can never be higher than 2.
                if (n < 0)
                    return false;

                // uint32 add = urand(0, 1);
                // SetData(n + 3, add); // broken in 24459 FIXME!

                yanzhuAuraMap.insert(std::make_pair(yanzhuPairs[n].entry_1, yanzhuPairs[n].spell_1));
                yanzhuAuraMap.insert(std::make_pair(yanzhuPairs[n].entry_2, yanzhuPairs[n].spell_2));

                return InitializeYanzhuAdds(n - 1);
            }

            uint32 GetAddToSummonEntry(uint32 type) // always uses second value, broken in 24459 FIXME
            {
                return GetData(type) ? yanzhuPairs[type - 3].entry_2 : yanzhuPairs[type - 3].entry_1;
            }

            uint32 GetAffectedSpellToAdd(uint32 type)
            {
                std::unordered_map<uint32, uint32>::iterator find = yanzhuAuraMap.find(GetAddToSummonEntry(type));
                if (find != yanzhuAuraMap.cend())
                    return find->second;

                return 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (instance->IsChallengeDungeon() && creature->isDead())
                    creature->Respawn();

                switch (creature->GetEntry())
                {
                    case NPC_HOPTALLUS:
                        hoptallusGUID = creature->GetGUID();
                        break;
                    case NPC_UNCLE_GAO:
                        uncleGaoGUID = creature->GetGUID();
                        break;
                    case NPC_OOK_OOK:
                        okOokGUID = creature->GetGUID();
                        break;
                    case NPC_CHEN_YANZHU:
                        chenYanzhuGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        break;
                    case NPC_YAN_ZHU:
                        yanzhuGUID = creature->GetGUID();
                        // Sudsy brew currently disabled until targeting type is fixed
                        for (int i = 3; i < 5 + 1; ++i)
                            creature->AddAura(GetAffectedSpellToAdd(i) == SPELL_SUDSY_BREW ? SPELL_FIZZY_BREW : GetAffectedSpellToAdd(i), creature);
                        break;
                    case NPC_HOZEN_CLINGER:
                        creature->SetCanFly(true);
                        break;
                    case NPC_HOZEN_PARTY_ANIMAL:
                    case NPC_HOZEN_PARTY_ANIMAL2:
                    case NPC_HOZEN_PARTY_ANIMAL3:
                        hozenGuidsVector.push_back(creature->GetGUID());
                        if (uint32 hatId = Trinity::Containers::SelectRandomContainerElement(hats))
                            creature->CastSpell(creature, hatId, false);
                        break;
                    case NPC_HOZEN_BOUNCER:
                        bouncerGuidsVector.push_back(creature->GetGUID());
                        break;
                    case NPC_PURPOSE_BUNNY_GROUND:
                    {
                        creature->RemoveAurasDueToSpell(128571);

                        uint32 delay = 0;
                        ObjectGuid creatureGUID = creature->GetGUID();
                        creature->m_Events.Schedule(delay += 1500, 1, [this, creatureGUID]()
                        {
                            if (Creature* trigger = instance->GetCreature(creatureGUID))
                                if (Creature* flyTrigger = trigger->FindNearestCreature(NPC_PURPOSE_BUNNY_FLYING, 30.0f, true))
                                    trigger->CastSpell(flyTrigger, SPELL_GUSHING_BREW, true);
                        });
                        break;
                    }
                    case NPC_DRUNKEN_HOZEN_BRAWLER:
                        creature->setRegeneratingHealth(false);
                        creature->SetHealth((uint32)creature->GetMaxHealth() * 0.1f);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_DRUNKEN_HOZEN_BRAWLER:
                        case NPC_INFLAMED_HOZEN_BRAWLER:
                        case NPC_SLEEPY_HOZEN_BRAWLER:
                        case NPC_SODDEN_HOZEN_BRAWLER:
                        case NPC_HOZEN_PARTY_ANIMAL:
                        case NPC_HOZEN_PARTY_ANIMAL2:
                        case NPC_HOZEN_PARTY_ANIMAL3:
                            hozenSlain++;
                            SetData(DATA_HOZEN_SLAIN, hozenSlain);
                            payersInList.clear();
                            GetPlayerListInGrid(payersInList, unit, 200.0f);

                            for (auto&& itr : payersInList)
                                if (itr->HasAura(SPELL_BANANA_BAR) && itr->GetPower(POWER_ALTERNATE_POWER) + 1 < 40)
                                    itr->SetPower(POWER_ALTERNATE_POWER, itr->GetPower(POWER_ALTERNATE_POWER) + 1);
                            break;
                    }
                }

                if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                    if (Creature* creature = unit->ToCreature())
                        UpdateConditionInfo(creature, ENEMIES_COUNT);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_OOK_EXIT_DOOR:
                    case GO_INVIS_DOOR:
                    case GO_HOPTAL_ENTRANCE:
                    case GO_YANJU_ENTRANCE:
                        AddDoor(go, true);
                        break;
                    case GO_OOK_DOOR:
                        ookOokDoorGUID = go->GetGUID();
                        if (GetBossState(DATA_OOK_OOK) == DONE)
                            go->AddObjectToRemoveList();
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                if (go->GetEntry() == GO_OOK_DOOR)
                    ookOokDoorGUID = ObjectGuid::Empty;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case 1:
                        {
                            events.ScheduleEvent(1, 7000);

                            if (hozenSlain >= 40 && GetBossState(DATA_OOK_OOK) != DONE)
                            {
                                events.CancelEvent(1);

                                if (Creature* ookOok = instance->GetCreature(GetGuidData(DATA_OOK_OOK)))
                                {
                                    SetBossState(DATA_OOK_OOK, SPECIAL);
                                    ookOok->AI()->DoAction(0);

                                    for (auto&& itr : hozenGuidsVector)
                                        if (Creature* creature = instance->GetCreature(itr))
                                            if (creature->AI() && creature->IsAlive())
                                                creature->AI()->DoAction(0);

                                    for (auto&& itr : instance->GetPlayers())
                                        if (Player* player = itr.GetSource())
                                            if (player->IsAlive() && !player->IsGameMaster())
                                                player->CombatStop(true);

                                    payersInList.clear();
                                    GetPlayerListInGrid(payersInList, ookOok, 200.0f);

                                    if (!payersInList.empty())
                                        for (auto&& itr : payersInList)
                                            if (itr->HasAura(SPELL_BANANA_BAR))
                                                itr->RemoveAura(SPELL_BANANA_BAR);
                                }
                            }
                            break;
                        }
                        case 2:
                        {
                            events.ScheduleEvent(2, 3000);

                            if (GetBossState(DATA_OOK_OOK) == DONE)
                            {
                                events.CancelEvent(2);

                                int32 number = 0;
                                for (auto&& itr : bouncerGuidsVector)
                                {
                                    if (Creature* creature = instance->GetCreature(itr))
                                    {
                                        creature->AI()->DoAction(number);
                                        number++;
                                    }
                                }
                            }
                            break;
                        }
                        case 3:
                        {
                            if (GetBossState(DATA_HOPTALLUS) == SPECIAL)
                                break;

                            events.ScheduleEvent(3, urand(12 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));

                            if (Creature* hoppy = instance->GetCreature(GetGuidData(DATA_HOPTALLUS)))
                                hoppy->AI()->DoAction(0);
                            break;
                        }
                    }
                }

                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_HOZEN_SLAIN:
                        hozenSlain = data;
                        SaveToDB();
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_HOZEN_SLAIN:
                        return hozenSlain;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_OOK_OOK:
                        return okOokGUID;
                    case DATA_HOPTALLUS:
                        return hoptallusGUID;
                    case DATA_YAN_ZHU:
                        return yanzhuGUID;
                    case GO_OOK_DOOR:
                        return ookOokDoorGUID;
                    case NPC_UNCLE_GAO:
                        return uncleGaoGUID;
                    case NPC_CHEN_YANZHU:
                        return chenYanzhuGUID;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_OOK_OOK)
                {
                    if (state == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(GetGuidData(GO_OOK_DOOR)))
                            go->AddObjectToRemoveList();
                    }
                    else
                    {
                        if (!instance->GetGameObject(GetGuidData(GO_OOK_DOOR)))
                        {
                            if (Creature* ookOok = instance->GetCreature(GetGuidData(DATA_OOK_OOK)))
                                if (GameObject* go = ookOok->SummonGameObject(GO_OOK_DOOR, ookOokDoorPos.GetPositionX(), ookOokDoorPos.GetPositionY(), ookOokDoorPos.GetPositionZ(), ookOokDoorPos.GetOrientation(), { }, 14400))
                                {
                                    go->SetGoState(GO_STATE_ACTIVE);
                                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                                }
                        }
                    }
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S S B " << GetBossSaveData() << hozenSlain;

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

                if (dataHead1 == 'S' && dataHead2 == 'S' && dataHead3 == 'B')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // Hozen Party event complete
                    hozenSlain = temp;
                    SetData(DATA_HOZEN_SLAIN, hozenSlain);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_stormstout_brewery_InstanceMapScript(map);
        }
};

// 7755 ( usually exit), might be 7998
class AreaTrigger_at_stormstout_intro : public AreaTriggerScript
{
    public:
        AreaTrigger_at_stormstout_intro() : AreaTriggerScript("at_stormstout_intro") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* chen = GetClosestCreatureWithEntry(player, NPC_CHEN_STORMSTOUT, 20.f))
            {
                chen->AI()->DoAction(0);
                return true;
            }

            return false;
        }
};

//7781 (just after stairs, currently unknown)

//8366
class AreaTrigger_at_uncle_gao : public AreaTriggerScript
{
    public:
        AreaTrigger_at_uncle_gao() : AreaTriggerScript("at_uncle_gao") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* gao = GetClosestCreatureWithEntry(player, NPC_UNCLE_GAO, 42.f))
            {
                gao->AI()->DoAction(4);
                return true;
            }
            return false;
        }
};


void AddSC_instance_stormstout_brewery()
{
    new instance_stormstout_brewery();
    new AreaTrigger_at_stormstout_intro();
    //new AreaTrigger_at_uncle_gao();
}
