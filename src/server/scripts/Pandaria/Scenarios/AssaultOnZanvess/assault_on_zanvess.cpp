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

#include "assault_on_zanvess.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

struct zanvess_klaxxi_typeAI : public ScriptedAI
{
    zanvess_klaxxi_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    EventMap nonCombatEvents;
    SummonList summons;
    InstanceScript* instance;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        me->setActive(true);
        me->setRegeneratingHealth(false);
        Reset();
    }
};

struct zanvess_assault_typeAI : public ScriptedAI
{
    zanvess_assault_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    EventMap events;
    SummonList summons;
    InstanceScript* instance;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
        me->setActive(true);
        Reset();

        AssaultByType(me->GetFaction() == 2550 ? true : false);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker->GetTypeId() != TYPEID_PLAYER && HealthBelowPct(75))
            damage = 0;
    }

    void AssaultByType(bool zanthik)
    {
        for (auto thLegion : zanthik ? Assault7tdLegion : AssaultZanthik)
        {
            if (Creature* Target = me->FindNearestCreature(thLegion, 30.0f, true))
            {
                me->Attack(Target, true);
                me->GetMotionMaster()->MoveChase(Target);
                break;
            }
        }

    }
};

class npc_zanvess_klaxxi_type_base : public CreatureScript
{
    public:
        npc_zanvess_klaxxi_type_base() : CreatureScript("npc_zanvess_klaxxi_type_base") { }

        struct npc_zanvess_klaxxi_type_baseAI : public zanvess_klaxxi_typeAI
        {
            npc_zanvess_klaxxi_type_baseAI(Creature* creature) : zanvess_klaxxi_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_klaxxi_type_baseAI(creature);
        }
};

class npc_zanvess_assault_type_base : public CreatureScript
{
    public:
        npc_zanvess_assault_type_base() : CreatureScript("npc_zanvess_assault_type_base") { }

        struct npc_zanvess_assault_type_baseAI : public zanvess_assault_typeAI
        {
            npc_zanvess_assault_type_baseAI(Creature* creature) : zanvess_assault_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_assault_type_baseAI(creature);
        }
};

// Korkron Gunship 67275, 68849
class npc_zanvess_korkron_gunship : public CreatureScript
{
    public:
        npc_zanvess_korkron_gunship() : CreatureScript("npc_zanvess_korkron_gunship") { }

        enum iEvents
        {
            EVENT_CHAPTER_2 = 1,
            EVENT_FLY_TO_SECOND_TOWERS,
            EVENT_FLY_TO_LAST_TOWERS,
            EVENT_FLY_TO_ISLAND,
            EVENT_LAUNCH_GUNSHIP,
        };

        struct npc_zanvess_korkron_gunshipAI : public CreatureAI
        {
            npc_zanvess_korkron_gunshipAI(Creature* creature) : CreatureAI(creature) 
            { 
                instance = me->GetInstanceScript();
                nalleyGUID = ObjectGuid::Empty;
            }

            TaskScheduler scheduler;
            ObjectGuid targetGUID;
            ObjectGuid nalleyGUID;
            EventMap events;
            Position pos;
            std::vector<Position> loopPath;
            InstanceScript* instance;
            float x, y;
            bool hasFirstTowersOffense;
            bool hasSecondTowersOffense;
            bool hasIslandTriggered;

            void IsSummonedBy(Unit* summoner) override
            {
                hasFirstTowersOffense = false;
                hasSecondTowersOffense = false;
                hasIslandTriggered = false;
                instance = me->GetInstanceScript();
                x = 0.0f; y = 0.0f;
                targetGUID = summoner->GetGUID();
                me->SetCanFly(true);
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                me->SetSpeed(MOVE_RUN, 5.5f, true);

                pos = MoveToBattle[4];
                
                events.ScheduleEvent(EVENT_LAUNCH_GUNSHIP, 2 * IN_MILLISECONDS);
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                uint32 gunshipEntry = 0;

                if (instance && instance->GetData(DATA_FACTION))
                    gunshipEntry = NPC_KORKRON_GUNSHIP;
                else
                    gunshipEntry = NPC_SKYFIRE_GYRO_VEHICLE;

                if (TempSummon* Gunship = clicker->SummonCreature(gunshipEntry, *me, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    if (TempSummon* nalley = clicker->SummonCreature(NPC_NALLEY, *clicker, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        nalley->EnterVehicle(Gunship, 2);
                        nalleyGUID = nalley->GetGUID();
                    }

                    clicker->CastSpell(Gunship, VEHICLE_SPELL_RIDE_HARDCODED, true);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_FIRST_TOWER_OFFENSIVE:
                    {
                        if (hasFirstTowersOffense)
                            break;

                        hasFirstTowersOffense = true;
                        events.Reset();
                        me->StopMoving();

                        pos = moveToSecondTowers[1];
                        me->SetSpeed(MOVE_RUN, 5.5f, true);

                        Movement::MoveSplineInit init(me);
                        for (auto itr : moveToSecondTowers)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.5f, 7.5f), itr.GetPositionY() + frand(10.0f, 10.0f), itr.GetPositionZ()));

                        init.SetWalk(false);
                        init.SetFly();
                        init.Launch();
                        events.ScheduleEvent(EVENT_FLY_TO_SECOND_TOWERS, me->GetSplineDuration());
                        break;
                    }
                    case ACTION_SECOND_TOWER_OFFENSIVE:
                    {
                        if (hasSecondTowersOffense)
                            break;

                        hasSecondTowersOffense = true;
                        events.Reset();
                        me->StopMoving();

                        pos = moveToLastTowers[2];
                        me->SetSpeed(MOVE_RUN, 5.5f, true);

                        Movement::MoveSplineInit init(me);
                        for (auto itr : moveToLastTowers)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.5f, 7.5f), itr.GetPositionY() + frand(10.0f, 10.0f), itr.GetPositionZ()));

                        init.SetWalk(false);
                        init.SetFly();
                        init.Launch();
                        events.ScheduleEvent(EVENT_FLY_TO_SECOND_TOWERS, me->GetSplineDuration());
                        break;
                    }
                    case ACTION_MOVE_TO_ISLAND:
                    {
                        if (hasIslandTriggered)
                            break;

                        hasIslandTriggered = true;
                        events.Reset();
                        me->StopMoving();

                        me->SetSpeed(MOVE_RUN, 5.5f, true);

                        Movement::MoveSplineInit init(me);
                        for (auto itr : beachPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.5f, 7.5f), itr.GetPositionY() + frand(10.0f, 10.0f), itr.GetPositionZ()));

                        init.SetWalk(false);
                        init.SetFly();
                        init.Launch();
                        events.ScheduleEvent(EVENT_FLY_TO_ISLAND, me->GetSplineDuration());
                        break;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAPTER_2:
                        {
                            if (instance && instance->GetData(DATA_ASSAULT_ON_ZANVESS) != DONE)
                                instance->SetData(DATA_ASSAULT_ON_ZANVESS, DONE);

                            loopPath.clear();
                            me->StopMoving();
                            me->SetSpeed(MOVE_RUN, 5.5f, true); // should be 2.5 , but actually player control vehicle
                            

                            Movement::MoveSplineInit pInit(me);
                            for (uint8 i = 0; i < 3; i++)
                            {
                                x = pos.GetPositionX() + frand(-80.0f, 80.0f);
                                y = pos.GetPositionY() + frand(-80.0f, 80.0f);
                                float z = me->GetPositionZ();
                                me->UpdateAllowedPositionZ(x, y, z);
                                pInit.Path().push_back(G3D::Vector3(x, y, z));
                            }

                            pInit.SetWalk(false);
                            pInit.Launch();
                            events.ScheduleEvent(EVENT_CHAPTER_2, me->GetSplineDuration() + 500);
                            break;
                        }
                        case EVENT_FLY_TO_SECOND_TOWERS:
                        {
                            loopPath.clear();
                            me->StopMoving();
                            me->SetSpeed(MOVE_RUN, 5.5f, true); // should be 2.5 , but actually player control vehicle

                            Movement::MoveSplineInit pInit(me);
                            for (uint8 i = 0; i < 3; i++)
                            {
                                x = pos.GetPositionX() + frand(-80.0f, 80.0f);
                                y = pos.GetPositionY() + frand(-80.0f, 80.0f);
                                float z = me->GetPositionZ();
                                me->UpdateAllowedPositionZ(x, y, z);
                                pInit.Path().push_back(G3D::Vector3(x, y, z));
                            }

                            pInit.SetWalk(false);
                            pInit.Launch();
                            events.ScheduleEvent(EVENT_FLY_TO_SECOND_TOWERS, me->GetSplineDuration() + 500);
                            break;
                        }
                        case EVENT_FLY_TO_ISLAND:
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, targetGUID))
                                owner->ExitVehicle();

                            if (Creature* nalley = ObjectAccessor::GetCreature(*me, nalleyGUID))
                                nalley->DespawnOrUnsummon();

                            me->DespawnOrUnsummon(1000);
                            break;
                        case EVENT_LAUNCH_GUNSHIP:
                        {
                            Movement::MoveSplineInit pInit(me);

                            if (instance && instance->GetData(DATA_FACTION))
                            {
                                for (auto itr : MoveToBattle)
                                    pInit.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.5f, 7.5f), itr.GetPositionY() + frand(10.0f, 10.0f), itr.GetPositionZ()));

                                pInit.SetWalk(false);
                                pInit.SetFly();
                                pInit.Launch();

                                events.ScheduleEvent(EVENT_CHAPTER_2, me->GetSplineDuration());
                            }
                            else
                            {
                                // up to sky
                                pInit.Path().push_back(G3D::Vector3(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10.0f));

                                for (auto itr : MoveToBattleforAlliance)
                                    pInit.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.5f, 7.5f), itr.GetPositionY() + frand(10.0f, 10.0f), itr.GetPositionZ()));

                                pInit.SetWalk(false);
                                pInit.SetFly();
                                pInit.Launch();

                                scheduler
                                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                                {
                                    me->StopMoving();
                                    me->GetMotionMaster()->MovePoint(1, MoveToBattle[4]);
                                    events.ScheduleEvent(EVENT_CHAPTER_2, me->GetSplineDuration());
                                });
                            }
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_korkron_gunshipAI(creature);
        }
};

// Nalley 68831
class npc_zanvess_nalley : public CreatureScript
{
    public:
        npc_zanvess_nalley() : CreatureScript("npc_zanvess_nalley") { }

        enum iEvents
        {
            EVENT_INTRO     = 1,
            EVENT_SPECIAL_1 = 2,
            EVENT_SPECIAL_2 = 3,
            EVENT_SPECIAL_3 = 4,
            EVENT_SPECIAL_4 = 5,
            EVENT_SPECIAL_5 = 6,
            EVENT_SPECIAL_6 = 7,
            EVENT_SPECIAL_7 = 8,
            EVENT_SPECIAL_8 = 9,
        };

        struct npc_zanvess_nalleyAI : public ScriptedAI
        {
            npc_zanvess_nalleyAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid targetGUID;
            EventMap nonCombatEvents;

            void IsSummonedBy(Unit* summoner) override
            {
                targetGUID = summoner->GetGUID();
                nonCombatEvents.ScheduleEvent(EVENT_INTRO, 2 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_NALLEY_DEFENDERS:
                        Talk(TALK_SPECIAL_3);
                        nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_3, 4 * IN_MILLISECONDS);
                        break;
                    case ACTION_BEACH:
                        Talk(TALK_SPECIAL_9);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                            Talk(TALK_INTRO);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_1, 4.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_1:
                            Talk(TALK_SPECIAL_1);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_2, 6 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_2:
                            Talk(TALK_SPECIAL_2);
                            break;
                        case EVENT_SPECIAL_4:
                            Talk(TALK_SPECIAL_4);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_5, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_5:
                            Talk(TALK_SPECIAL_5);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_6, 8.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_6:
                            Talk(TALK_SPECIAL_6);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_7, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_7:
                            Talk(TALK_SPECIAL_7);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_8, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_8:
                            Talk(TALK_SPECIAL_8);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_nalleyAI(creature);
        }
};

// Sonic Control Tower 67279, 67788
class npc_zanvess_sonic_control_tower : public CreatureScript
{
    public:
        npc_zanvess_sonic_control_tower() : CreatureScript("npc_zanvess_sonic_control_tower") { }

        enum iEvents
        {
            EVENT_INIT = 1,
        };

        struct npc_zanvess_sonic_control_towerAI : public zanvess_klaxxi_typeAI
        {
            npc_zanvess_sonic_control_towerAI(Creature* creature) : zanvess_klaxxi_typeAI(creature) { }

            ObjectGuid targetGUID;
            InstanceScript* instance;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                nonCombatEvents.ScheduleEvent(EVENT_INIT, 1.5 * IN_MILLISECONDS);
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                    instance->SetData(DATA_DEFENSES_OF_ZANVESS, instance->GetData(DATA_DEFENSES_OF_ZANVESS) + 1);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INIT)
                        DoCast(me, SPELL_SHIELD_BEAM);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_sonic_control_towerAI(creature);
        }
};

// Scenario Controller 67272
class npc_zanvess_scenario_controller : public CreatureScript
{
    public:
        npc_zanvess_scenario_controller() : CreatureScript("npc_zanvess_scenario_controller") { }

        enum iEvents
        {
            EVENT_INIT = 1,
        };

        struct npc_zanvess_scenario_controllerAI : public zanvess_klaxxi_typeAI
        {
            npc_zanvess_scenario_controllerAI(Creature* creature) : zanvess_klaxxi_typeAI(creature) { }

            ObjectGuid targetGUID;

            void Reset() override
            {
                nonCombatEvents.ScheduleEvent(EVENT_INIT, 1.5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INIT)
                        DoCast(me, SPELL_ISLAND_SHIELD);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_scenario_controllerAI(creature);
        }
};

// Scorpid Relocator 67784
struct npc_zanvess_scorpid_relocator : public zanvess_klaxxi_typeAI
{
    npc_zanvess_scorpid_relocator(Creature* creature) : zanvess_klaxxi_typeAI(creature) { }

    Position spawnPos;
    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void Reset() override
    {
        spawnPos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() };

        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (TempSummon* sonicTower = me->SummonCreature(NPC_SONIC_CONTROL_TOWER, *me, TEMPSUMMON_MANUAL_DESPAWN))
            {
                targetGUID = sonicTower->GetGUID();
                sonicTower->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                sonicTower->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
            }
        });

        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            Position pos = me->GetRandomPoint(spawnPos, 10.0f);
            me->GetMotionMaster()->MovePoint(0, pos);

            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* sTower = ObjectAccessor::GetCreature(*me, targetGUID))
            sTower->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Zanthik Bruralizer 67774
class npc_zanvess_zanthik_brutalizer : public CreatureScript
{
    public:
        npc_zanvess_zanthik_brutalizer() : CreatureScript("npc_zanvess_zanthik_brutalizer") { }

        enum iSpells
        {
            SPELL_STOMP = 133859,
        };

        enum iEvents
        {
            EVENT_STOMP = 1,
        };

        struct npc_zanvess_zanthik_brutalizerAI : public zanvess_assault_typeAI
        {
            npc_zanvess_zanthik_brutalizerAI(Creature* creature) : zanvess_assault_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_STOMP, 5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_STOMP)
                    {
                        DoCast(me, SPELL_STOMP);
                        events.ScheduleEvent(EVENT_STOMP, urand(8.5 * IN_MILLISECONDS, 13.9 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_zanthik_brutalizerAI(creature);
        }
};

// Zanthik Ambermancer 62838
class npc_zanvess_zanthik_ambermancer : public CreatureScript
{
    public:
        npc_zanvess_zanthik_ambermancer() : CreatureScript("npc_zanvess_zanthik_ambermancer") { }

        enum iSpells
        {
            SPELL_HOLLY_SMITE   = 134004,
            SPELL_GIFT_OF_AMBER = 125021,
            SPELL_SONIC_SHIELD  = 125001,
        };

        enum iEvents
        {
            EVENT_GIFT_OF_AMBER = 1,
            EVENT_SONIC_SHIELD  = 2,
        };

        struct npc_zanvess_zanthik_ambermancerAI : public zanvess_assault_typeAI
        {
            npc_zanvess_zanthik_ambermancerAI(Creature* creature) : zanvess_assault_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_SONIC_SHIELD);
                events.ScheduleEvent(EVENT_SONIC_SHIELD, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_GIFT_OF_AMBER, 8 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GIFT_OF_AMBER:
                            me->InterruptNonMeleeSpells(true);

                            if (Unit* target = ObjectAccessor::GetCreature(*me, FriendlyTargetGUID()))
                                DoCast(target, SPELL_GIFT_OF_AMBER);

                            events.ScheduleEvent(EVENT_GIFT_OF_AMBER, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_SONIC_SHIELD:
                            me->InterruptNonMeleeSpells(true);
                            DoCast(me, SPELL_SONIC_SHIELD);
                            events.ScheduleEvent(EVENT_SONIC_SHIELD, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }

                if (Unit* victim = me->GetVictim())
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoCast(victim, SPELL_HOLLY_SMITE);
            }

        private:
            ObjectGuid FriendlyTargetGUID()
            {
                std::list<Creature*> friendList;
                GetCreatureListWithEntryInGrid(friendList, me, NPC_ZANTHIK_AMBERMANCER, 30.0f);
                GetCreatureListWithEntryInGrid(friendList, me, NPC_ZANTHIK_BRUTALIZER, 30.0f);

                if (!friendList.empty())
                    if (Creature* AmberTarget = Trinity::Containers::SelectRandomContainerElement(friendList))
                        return AmberTarget->GetGUID();

                return ObjectGuid::Empty;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_zanthik_ambermancerAI(creature);
        }
};

// 7th Legion Infiltrator 67727
class npc_zanvess_7th_legion_infiltrator : public CreatureScript
{
    public:
        npc_zanvess_7th_legion_infiltrator() : CreatureScript("npc_zanvess_7th_legion_infiltrator") { }

        enum iSpells
        {
            SPELL_RUSH = 134002,
        };

        enum iEvents
        {
            EVENT_RUSH = 1,
        };

        struct npc_zanvess_7th_legion_infiltratorAI : public zanvess_assault_typeAI
        {
            npc_zanvess_7th_legion_infiltratorAI(Creature* creature) : zanvess_assault_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_RUSH);
                events.ScheduleEvent(EVENT_RUSH, 10 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_RUSH)
                    {
                        DoCast(me, SPELL_RUSH);
                        events.ScheduleEvent(EVENT_RUSH, 10 * IN_MILLISECONDS);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_7th_legion_infiltratorAI(creature);
        }
};

// 7th Legion Priest 67892
class npc_zanvess_7th_legion_priest : public CreatureScript
{
    public:
        npc_zanvess_7th_legion_priest() : CreatureScript("npc_zanvess_7th_legion_priest") { }

        enum iSpells
        {
            SPELL_HOLLY_SMITE      = 134004,
            SPELL_HEAL             = 134003,
            SPELL_SHADOW_WORD_PAIN = 134006,
        };

        enum iEvents
        {
            EVENT_SHADOW_WORD = 1,
            EVENT_HEAL        = 2,
        };

        struct npc_zanvess_7th_legion_priestAI : public zanvess_assault_typeAI
        {
            npc_zanvess_7th_legion_priestAI(Creature* creature) : zanvess_assault_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HEAL, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOW_WORD, 8 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_WORD:
                            me->InterruptNonMeleeSpells(true);

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_SHADOW_WORD_PAIN);

                            events.ScheduleEvent(EVENT_SHADOW_WORD, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_HEAL:
                            me->InterruptNonMeleeSpells(true);

                            if (Unit* target = ObjectAccessor::GetCreature(*me, LowestHpTargetGUID()))
                                DoCast(target, SPELL_HEAL);

                            events.ScheduleEvent(EVENT_HEAL, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }

                if (Unit* victim = me->GetVictim())
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoCast(victim, SPELL_HOLLY_SMITE);
            }

            private:
                ObjectGuid LowestHpTargetGUID()
                {
                    ObjectGuid targetGUID = ObjectGuid::Empty;
                    float pctHealth = 100.0f;
                    std::list<Creature*> friendList;

                    GetCreatureListWithEntryInGrid(friendList, me, NPC_7TH_LEGION_INFILTRATOR, 30.0f);
                    GetCreatureListWithEntryInGrid(friendList, me, NPC_7TH_LIGION_PRIEST, 30.0f);

                    if (!friendList.empty())
                    {
                        for (auto Legion7th : friendList)
                        {
                            if (Legion7th->GetHealthPct() < pctHealth)
                            {
                                pctHealth = Legion7th->GetHealthPct();
                                targetGUID = Legion7th->GetGUID();
                            }
                        }
                    }

                    return targetGUID;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_7th_legion_priestAI(creature);
        }
};

// Team Leader Scooter 67810
class npc_zanvess_team_leader_scooter : public CreatureScript
{
    public:
        npc_zanvess_team_leader_scooter() : CreatureScript("npc_zanvess_team_leader_scooter") { }

        enum iSpells
        {
            SPELL_WHIRLWIND                = 133845,
            SPELL_FIXATE                   = 133843,
            SPELL_DEVASTATING_SMASH        = 133820,
        };

        enum iEvents
        {
            EVENT_WHIRLWIND = 1,
            EVENT_FIXATE    = 2,
            EVENT_SMASH     = 3,
        };

        struct npc_zanvess_team_leader_scooterAI : public zanvess_assault_typeAI
        {
            npc_zanvess_team_leader_scooterAI(Creature* creature) : zanvess_assault_typeAI(creature) { }

            EventMap events;
            ObjectGuid targetGUID;

            void Reset() override
            {
                events.Reset();
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FIXATE, urand(8 * IN_MILLISECONDS, 29 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WHIRLWIND, 14 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SMASH, urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                {
                    instance->SetData(DATA_HEART_OF_ZANVESS, instance->GetData(DATA_HEART_OF_ZANVESS) + 1);
                    instance->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_COMANDER_SCOOTER, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_WHIRLWIND:
                        me->ClearUnitState(UNIT_STATE_CASTING);
                        break;
                    case ACTION_DEVASTATING_SMASH:
                        me->RemoveChanneledCast(targetGUID);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIXATE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_FIXATE);

                            events.ScheduleEvent(EVENT_FIXATE, urand(18 * IN_MILLISECONDS, 29 * IN_MILLISECONDS));
                            break;
                        case EVENT_WHIRLWIND:
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_WHIRLWIND, 14 * IN_MILLISECONDS);
                            break;
                        case EVENT_SMASH:
                            if (Unit* vict = me->GetVictim())
                            {
                                me->InterruptNonMeleeSpells(true);

                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                {
                                    targetGUID = vict->GetGUID();
                                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_DEVASTATING_SMASH);
                                }
                                else
                                {
                                    targetGUID = vict->GetGUID();
                                    me->PrepareChanneledCast(me->GetAngle(vict), SPELL_DEVASTATING_SMASH);
                                }
                            }

                            events.ScheduleEvent(EVENT_SMASH, urand(11 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_team_leader_scooterAI(creature);
        }
};

// Commander Telvrak 67879
class npc_zanvess_commander_telvrak : public CreatureScript
{
    public:
        npc_zanvess_commander_telvrak() : CreatureScript("npc_zanvess_commander_telvrak") { }

        enum iSpells
        {
            SPELL_SWAP_WEAPON = 133966, // at combat reach it`ll select a random weapon, and at next use`ll switch between this
            SPELL_SWIFTNESS   = 133970, // when creature use daggers
            SPELL_BLADEMASTER = 133969, // when creature use sword
        };

        enum iEvents
        {
            EVENT_SWAP_WEAPON = 1,
            EVENT_CALL_SWARM  = 2,
        };

        struct npc_zanvess_commander_telvrakAI : public ScriptedAI
        {
            npc_zanvess_commander_telvrakAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            SummonList summons;
            uint32 weaponData;
            InstanceScript* instance;

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_FOR_THE_SWARM, 1);
                summons.DespawnAll();
                events.Reset();
                weaponData = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_SWAP_WEAPON);
                events.ScheduleEvent(EVENT_SWAP_WEAPON, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CALL_SWARM, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_SWAP_WEAPON)
                    return weaponData;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_SWAP_WEAPON)
                    weaponData = data;
            }

            void JustDied(Unit* killer) override
            {
                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_WEAPON_OF_ZANVESS, DONE);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_TELVRAK_ASSAULT:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                        break;
                    case ACTION_SWORD:
                        SetData(TYPE_SWAP_WEAPON, 1);;
                        me->RemoveAurasDueToSpell(SPELL_SWIFTNESS);
                        DoCast(me, SPELL_BLADEMASTER);
                        SetEquipmentSlots(false, TYPE_KLAXXI_SWORD, 0, EQUIP_NO_CHANGE);
                        break;
                    case ACTION_DAGGERS:
                        SetData(TYPE_SWAP_WEAPON, 2);
                        me->RemoveAurasDueToSpell(SPELL_BLADEMASTER);
                        DoCast(me, SPELL_SWIFTNESS);
                        SetEquipmentSlots(false, TYPE_KLAXXI_DAGGER, TYPE_KLAXXI_DAGGER, EQUIP_NO_CHANGE);
                        break;
                }
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_FOR_THE_SWARM, 0);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void HandleSummonSwarm()
            {
                for (uint8 i = 0; i < 6; i++)
                {
                    if (TempSummon* zathik = me->SummonCreature(NPC_ZANTHIK_SWARMER, urand(0, 1) ? SwarmSpawnPoints[0] : SwarmSpawnPoints[1], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (Player* itr = zathik->FindNearestPlayer(80.0f))
                        {
                            zathik->Attack(itr, true);
                            zathik->GetMotionMaster()->MoveChase(itr);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWAP_WEAPON:
                            DoCast(me, SPELL_SWAP_WEAPON);
                            events.ScheduleEvent(EVENT_SWAP_WEAPON, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_CALL_SWARM:
                            HandleSummonSwarm();
                            events.ScheduleEvent(EVENT_CALL_SWARM, urand(17 * IN_MILLISECONDS, 28.5 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zanvess_commander_telvrakAI(creature);
        }
};

// Zarthik Guardian 67710
struct npc_zanvess_zarthik_guardian : public ScriptedAI
{
    npc_zanvess_zarthik_guardian(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->setRegeneratingHealth(false);
        DoCast(me, SPELL_PROTECTIVE_SHELL);
    }

    void EnterEvadeMode() override 
    {
        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Strafing Ran Trigger 67838
struct npc_zanvess_strafing_ran_trigger : public ScriptedAI
{
    npc_zanvess_strafing_ran_trigger(Creature* creature) : ScriptedAI(creature) 
    { 
        summonerGUID = ObjectGuid::Empty;
    }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (Unit* summoner = ObjectAccessor::GetUnit(*me, summonerGUID))
                summoner->CastSpell(me, SPELL_STRAFING_RAN_MISSLE, true);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Whirlwind 133845
class spell_zanvess_whirlwind : public AuraScript
{
    PrepareAuraScript(spell_zanvess_whirlwind);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_WHIRLWIND);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_zanvess_whirlwind::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Devastating Smash 133820
class spell_zanvess_devastating_smash : public SpellScript
{
    PrepareSpellScript(spell_zanvess_devastating_smash);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_DEVASTATING_SMASH);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_zanvess_devastating_smash::HandleAfterCast);
    }
};

// Swap Weapon 133966
class spell_zanvess_swap_weapon : public SpellScript
{
    PrepareSpellScript(spell_zanvess_swap_weapon);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (uint32 wData = caster->ToCreature()->AI()->GetData(TYPE_SWAP_WEAPON))
                caster->AI()->DoAction(wData > 1 ? ACTION_SWORD : ACTION_DAGGERS);
            else
                caster->AI()->DoAction(urand(0, 1) ? ACTION_SWORD : ACTION_DAGGERS);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_zanvess_swap_weapon::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Bombard 133557
class spell_zanvess_bombard : public SpellScript
{
    PrepareSpellScript(spell_zanvess_bombard);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        // tooltip miss or should calculate by anyway. in 5.4.8 has ~320-350k 
        SetHitDamage(GetHitDamage() * 10);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_zanvess_bombard::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// AreaTrigger 8683
class AreaTrigger_at_zanvess_heart : public AreaTriggerScript
{
    public:
        AreaTrigger_at_zanvess_heart() : AreaTriggerScript("AreaTrigger_at_zanvess_heart") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_TELVRAK_LINKED))
                    return true;

                if (instance->GetData(DATA_HEART_OF_ZANVESS) < 2)
                {
                    instance->SetData(DATA_TELVRAK_LINKED, 1);
                    instance->SetData(DATA_HEART_OF_ZANVESS, instance->GetData(DATA_HEART_OF_ZANVESS) + 1);
                    instance->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_REACH_HEART_OF_ZANVESS, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));

                    if (Creature* Telvrak = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_COMMANDER_TELVRAK)))
                        Telvrak->AI()->Talk(TALK_INTRO);
                }
            }

            return false;
        }
};

void AddSC_assault_on_zanvess()
{
    new npc_zanvess_klaxxi_type_base();
    new npc_zanvess_assault_type_base();
    new npc_zanvess_korkron_gunship();
    new npc_zanvess_nalley();
    new npc_zanvess_sonic_control_tower();
    new npc_zanvess_scenario_controller();
    new creature_script<npc_zanvess_scorpid_relocator>("npc_zanvess_scorpid_relocator");
    new npc_zanvess_zanthik_brutalizer();
    new npc_zanvess_zanthik_ambermancer();
    new npc_zanvess_7th_legion_infiltrator();
    new npc_zanvess_7th_legion_priest();
    new npc_zanvess_team_leader_scooter();
    new npc_zanvess_commander_telvrak();
    new creature_script<npc_zanvess_zarthik_guardian>("npc_zanvess_zarthik_guardian");
    new creature_script<npc_zanvess_strafing_ran_trigger>("npc_zanvess_strafing_ran_trigger");

    new aura_script<spell_zanvess_whirlwind>("spell_zanvess_whirlwind");
    new spell_script<spell_zanvess_devastating_smash>("spell_zanvess_devastating_smash");
    new spell_script<spell_zanvess_swap_weapon>("spell_zanvess_swap_weapon");
    new spell_script<spell_zanvess_bombard>("spell_zanvess_bombard");
    new AreaTrigger_at_zanvess_heart();
}