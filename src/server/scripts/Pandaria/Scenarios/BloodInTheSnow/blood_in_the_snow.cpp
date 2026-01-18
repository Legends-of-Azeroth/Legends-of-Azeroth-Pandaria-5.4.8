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

#include "blood_in_the_snow.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_FREE_SCOUT_STONEBEARD  = 141316,
    SPELL_DRAIN_BLOOD            = 112896,
    SPELL_THROW_AXE              = 132253,
    SPELL_SHOT                   = 85710,
    SPELL_DEVASTATING_LEAP       = 126903,
    SPELL_DEVASTATING_LEAP_EFF   = 126902,
    SPELL_FROSTBOLT              = 141619,
    SPELL_SUMMON_WATER_ELEMENTAL = 141380,
    SPELL_FREEZiNG_CIRCLE        = 141383,
    SPELL_FROST_NOVA             = 11831,
    SPELL_BLIZZARD               = 141379,
    SPELL_FROSTBOLT_VOLLEY       = 141620,
    SPELL_FREEZE_SOLID_LAUNCHER  = 141407,
    SPELL_FREEZE_SOLID_SUMMON    = 141409,
    SPELL_FREEZE_SOLID_EFF       = 141410,
    SPELL_HEKIMAS_BRAND          = 139555,
    SPELL_HEKIMAS_WISDOM         = 141423,
    SPELL_HEKIMAS_SCORN          = 142623,
    SPELL_HEKIMAS_SCORN_EFF      = 133837,
    SPELL_SHIMMERWEED            = 142715,
    SPELL_SHIMMERWEED_EFF        = 142714,
    SPELL_SHIMMERWEED_VISUAL     = 142718,
    SPELL_ZANDALARI_RAGE_EFF     = 141315,
    SPELL_FROST_SHOCK            = 46180,
    SPELL_FIREBALL               = 79854,
    SPELL_BLAST_WAVE             = 79857,
};

enum Events
{
    EVENT_DRAIN_BLOOD = 1,
    EVENT_THROW_AXE,
    EVENT_SHOT,
    EVENT_DEVASTATING_LEAP,
    EVENT_FROSTBOLT,
    EVENT_FREEZING_CIRCLE,
    EVENT_BLIZZARD,
    EVENT_WATER_ELEMENTAL,
    EVENT_FREEZE_SOLID,
    EVENT_HEKIMA_BRANDS,
    EVENT_HEKIMAS_WISDOM,
    EVENT_HEKIMAS_SCORN,
    EVENT_FROST_SHOCK,
    EVENT_FIREBALL,
    EVENT_BLAST_WAVE,
};

// Varian Wrynn 70799
struct npc_bloodsnow_varian_wrynn : public ScriptedAI
{
    npc_bloodsnow_varian_wrynn(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 delay;

    void Reset() override
    {
        delay = 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                Talk(TALK_INTRO);
                delay = 4600;

                me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI / 2));

                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4700), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5900), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6250), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7000), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        me->SetFacingToObject(moira);

                    Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6480), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 9300), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                    {
                        moira->SetFacingToObject(me);
                        moira->AI()->Talk(TALK_SPECIAL_5);
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7450), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_5);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
                {
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                        moira->AI()->Talk(TALK_SPECIAL_6);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1500), [this](TaskContext context)
                {
                    // Moira Move forward
                    if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN) : ObjectGuid::Empty))
                    {
                        float x, y;
                        GetPositionWithDistInOrientation(moira, 15.0f, Position::NormalizeOrientation(moira->GetOrientation() + M_PI / 2), x, y);
                        float z = moira->GetPositionZ();
                        moira->UpdateAllowedPositionZ(x, y, z);
                        moira->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
                });
                break;
            case ACTION_LAST_STAND:
                me->SetVisible(true);

                if (Creature* moira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN + 1) : ObjectGuid::Empty))
                {
                    float x, y;
                    GetPositionWithDistInOrientation(moira, 3.0f, moira->GetAngle(me), x, y);
                    moira->SetFacingToObject(me);
                    me->GetMotionMaster()->MovePoint(0, x, y, moira->GetPositionZ());

                    delay = me->GetSplineDuration();
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_6);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
                    {
                        if (Creature* newMoira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN + 1) : ObjectGuid::Empty))
                            newMoira->AI()->Talk(TALK_SPECIAL_8);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 6700), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_7);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 7000), [this](TaskContext context)
                    {
                        if (Creature* newMoira = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MOIRA_THAURISSAN + 1) : ObjectGuid::Empty))
                            newMoira->AI()->Talk(TALK_SPECIAL_9);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 7600), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_8);
                    });
                }
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Mountainer Grimbolt 70801
struct npc_bloodsnow_mountainer_grimbolt : public ScriptedAI
{
    npc_bloodsnow_mountainer_grimbolt(Creature* creature) : ScriptedAI(creature) 
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    uint32 delay;
    bool hasTriggered;

    void Reset() override
    {
        delay = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (!hasTriggered && actionId == ACTION_START_INTRO)
        {
            hasTriggered = true;
            delay = 2500;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_MOUNTAINEERS, DONE);

                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Roasting Spit 70597
struct npc_bloodsnow_roasting_spit : public ScriptedAI
{
    npc_bloodsnow_roasting_spit(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
         // Init delay
        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (Creature* stonebeard = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SCOUT_STONEBEARD) : ObjectGuid::Empty))
                stonebeard->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
        });
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (Creature* stonebeard = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SCOUT_STONEBEARD) : ObjectGuid::Empty))
            clicker->CastSpell(stonebeard, SPELL_FREE_SCOUT_STONEBEARD, false);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Scout Stonebeard 70602
struct npc_bloodsnow_scout_stonebeard : public ScriptedAI
{
    npc_bloodsnow_scout_stonebeard(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;
    uint32 delay;

    void Reset() override
    {
        hasTriggered = false;
        delay = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (!hasTriggered && actionId == ACTION_START_INTRO)
        {
            hasTriggered = true;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->ExitVehicle();
            me->GetMotionMaster()->MoveJump(stonebeardJumpPos.GetPositionX(), stonebeardJumpPos.GetPositionY(), stonebeardJumpPos.GetPositionZ(), 20.0f, 15.0f);

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_SAVE_SCOUT_STONEBEARD, DONE);

            delay = 2480;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 8700), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 8550), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });

            scheduler
                .Schedule(Milliseconds(delay += 8470), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Frostmane Flesh-Eater 70746
struct npc_bloodsnow_frostmane_flesh_eater : public customCreatureAI
{
    npc_bloodsnow_frostmane_flesh_eater(Creature* creature) : customCreatureAI(creature) 
    { 
        scoutGUID = ObjectGuid::Empty;
    }

    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DRAIN_BLOOD, urand(2.5 * IN_MILLISECONDS, 5.5 * IN_MILLISECONDS));
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_DRAIN_BLOOD, 6.5 * IN_MILLISECONDS, EVENT_DRAIN_BLOOD, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Frostmane Headhunter 70473
struct npc_bloodsnow_frostmane_headhunter : public customCreatureAI
{
    npc_bloodsnow_frostmane_headhunter(Creature* creature) : customCreatureAI(creature) 
    {
        scoutGUID = ObjectGuid::Empty;
    }

    bool allowAttack;
    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();
        allowAttack = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THROW_AXE, 1.5 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_THROW_AXE)
            {
                if (Unit* vict = me->GetVictim())
                {
                    if (me->GetExactDist2d(vict) > 12.5f)
                    {
                        allowAttack = false;
                        DoCast(vict, SPELL_THROW_AXE);
                        DoStartNoMovement(vict);
                    }
                    else
                    {
                        allowAttack = true;
                        DoStartMovement(vict);
                    }
                }

                events.ScheduleEvent(EVENT_THROW_AXE, 1.5 * IN_MILLISECONDS);
            }
            break;
        }

        if (allowAttack)
            DoMeleeAttackIfReady();
    }
};

// Frostmane Snowstalker 70463
struct npc_bloodsnow_frostmane_snowstalker : public customCreatureAI
{
    npc_bloodsnow_frostmane_snowstalker(Creature* creature) : customCreatureAI(creature) 
    {
        scoutGUID = ObjectGuid::Empty;
    }

    TaskScheduler scheduler;
    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (Creature* mauler = me->FindNearestCreature(NPC_FROSTMANE_MAULER, 8.0f))
                DoCast(mauler, VEHICLE_SPELL_RIDE_HARDCODED);
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHOT, 1 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SHOT)
            {
                if (Unit* vict = me->GetVictim())
                {
                    if (me->GetExactDist2d(vict) > 40.5f)
                        DoStartMovement(vict);
                    else
                        DoStartNoMovement(vict);

                    DoCast(vict, SPELL_SHOT);
                }

                events.ScheduleEvent(EVENT_SHOT, 1.6 * IN_MILLISECONDS);
            }
            break;
        }
    }
};

// Frostmane Berserker 70471
struct npc_bloodsnow_frostmane_berserker : public customCreatureAI
{
    npc_bloodsnow_frostmane_berserker(Creature* creature) : customCreatureAI(creature) 
    {
        scoutGUID = ObjectGuid::Empty;
    }

    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DEVASTATING_LEAP, urand(6.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_DEVASTATING_LEAP, 11.5 * IN_MILLISECONDS, EVENT_DEVASTATING_LEAP, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Frostmane Bonechiller 70465, Bonechiller Barafu 70468
struct npc_bloodsnow_frostmane_bonechiller : public customCreatureAI
{
    npc_bloodsnow_frostmane_bonechiller(Creature* creature) : customCreatureAI(creature) 
    {
        scoutGUID = ObjectGuid::Empty;
    }

    bool hasElementalTriggered;
    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FROSTBOLT, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FREEZING_CIRCLE, 8.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BLIZZARD, 16 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;

        if (HealthBelowPct(50) && !hasElementalTriggered)
        {
            hasElementalTriggered = true;
            events.ScheduleEvent(EVENT_WATER_ELEMENTAL, 1 * IN_MILLISECONDS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_FROSTBOLT)
            {
                if (Unit* vict = me->GetVictim())
                {
                    if (me->GetExactDist2d(vict) > 35.5f)
                        DoStartMovement(vict);
                    else
                        DoStartNoMovement(vict);

                    DoCast(vict, me->GetEntry() == NPC_BONECHILLER_BARAFU ? SPELL_FROSTBOLT_VOLLEY : SPELL_FROSTBOLT);
                }

                events.ScheduleEvent(EVENT_FROSTBOLT, me->GetEntry() == NPC_BONECHILLER_BARAFU ? 3.1 * IN_MILLISECONDS : 2.1 * IN_MILLISECONDS);
            }

            ExecuteTargetEvent(SPELL_BLIZZARD, 20 * IN_MILLISECONDS, EVENT_BLIZZARD, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_FREEZiNG_CIRCLE, 12.5 * IN_MILLISECONDS, EVENT_FREEZING_CIRCLE, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_SUMMON_WATER_ELEMENTAL, 0, EVENT_WATER_ELEMENTAL, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Farastu 70474
struct npc_bloodsnow_farastu : public customCreatureAI
{
    npc_bloodsnow_farastu(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FREEZE_SOLID, urand(9.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_COLD_CAVE, DONE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FREEZE_SOLID_LAUNCHER, 15 * IN_MILLISECONDS, EVENT_FREEZE_SOLID, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Freeze Solid 70987
struct npc_bloodsnow_freeze_solid : public customCreatureAI
{
    npc_bloodsnow_freeze_solid(Creature* creature) : customCreatureAI(creature) { }

    ObjectGuid affectedTargetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        affectedTargetGUID = summoner->GetGUID();
        DoCast(summoner, SPELL_FREEZE_SOLID_EFF);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* affectedTarget = ObjectAccessor::GetUnit(*me, affectedTargetGUID))
            affectedTarget->RemoveAurasDueToSpell(SPELL_FREEZE_SOLID_EFF);

        me->DisappearAndDie();
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Hekima the Wise 70544
struct npc_bloodsnow_hekima_the_wise : public customCreatureAI
{
    npc_bloodsnow_hekima_the_wise(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasYell1;
    bool hasYell2;
    bool hasYell3;

    void Reset() override
    {
        events.Reset();
        hasYell1 = false;
        hasYell2 = false;
        hasYell3 = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_HEKIMA_BRANDS, urand(3.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_HEKIMAS_WISDOM, 14 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HEKIMAS_SCORN, 8.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(TALK_SPECIAL_4);

        if (me->GetInstanceScript())
        {
            me->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_HEKIMA_DEFEATER, 1, me->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));
            me->GetInstanceScript()->SetData(DATA_VICTORY_OF_DEATH, IN_PROGRESS);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;

        if (!hasYell1 && HealthBelowPct(60))
        {
            hasYell1 = true;
            Talk(TALK_SPECIAL_1);
        }

        if (!hasYell2 && HealthBelowPct(40))
        {
            hasYell2 = true;
            Talk(TALK_SPECIAL_2);
        }

        if (!hasYell3 && HealthBelowPct(20))
        {
            hasYell3 = true;
            Talk(TALK_SPECIAL_3);
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_LAST_STAND)
        {
            me->SetVisible(true);

            scheduler
                .Schedule(Milliseconds(9600), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });
        }
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_FLESH_EATER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_HEADHUNTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_SNOWSTALKER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_BERSERKER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_BONECHILLER, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_HEKIMAS_BRAND, 9 * IN_MILLISECONDS, EVENT_HEKIMA_BRANDS, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_HEKIMAS_WISDOM, 20 * IN_MILLISECONDS, EVENT_HEKIMAS_WISDOM, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_HEKIMAS_SCORN, 12 * IN_MILLISECONDS, EVENT_HEKIMAS_SCORN, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Frostmane Witch Doctor 70464
struct npc_bloodsnow_frostmane_witch_doctor : public customCreatureAI
{
    npc_bloodsnow_frostmane_witch_doctor(Creature* creature) : customCreatureAI(creature) 
    {
        scoutGUID = ObjectGuid::Empty;
    }

    ObjectGuid scoutGUID;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FROST_SHOCK, urand(2.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_HEKIMAS_WISDOM, 14 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_FLESH_EATER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_HEADHUNTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_SNOWSTALKER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_BERSERKER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FROSTMANE_BONECHILLER, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FROST_SHOCK, 9 * IN_MILLISECONDS, EVENT_FROST_SHOCK, eventId);
            ExecuteTargetEvent(SPELL_HEKIMAS_WISDOM, 20 * IN_MILLISECONDS, EVENT_HEKIMAS_WISDOM, eventId, PRIORITY_ALLY);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Iron Forge Guardian 70275, 70709
struct npc_bloodsnow_ironforge_guardian : public customCreatureAI
{
    npc_bloodsnow_ironforge_guardian(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (me->GetEntry() == NPC_PYROMANCE_FLAMEHEARTH)
        {
            events.ScheduleEvent(EVENT_FIREBALL, 1 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BLAST_WAVE, urand(9.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (HealthBelowPct(65))
            damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FIREBALL, 3.1 * IN_MILLISECONDS, EVENT_FIREBALL, eventId);
            ExecuteTargetEvent(SPELL_BLAST_WAVE, 9 * IN_MILLISECONDS, EVENT_BLAST_WAVE, eventId, PRIORITY_SELF);
            break;
        }

        if (me->GetEntry() != NPC_PYROMANCE_FLAMEHEARTH)
            DoMeleeAttackIfReady();
    }
};

// Shimmerweed Basket 71440
struct npc_bloodsnow_shimmerweed_basket : public customCreatureAI
{
    npc_bloodsnow_shimmerweed_basket(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SHIMMERWEED);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Scout Boldbrew 70744
struct npc_bloodsnow_scout_boldbrew : public customCreatureAI
{
    npc_bloodsnow_scout_boldbrew(Creature* creature) : customCreatureAI(creature) 
    {
        prisonersCount = 0;
        delay = 0;
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    uint32 prisonersCount;
    uint32 delay;
    bool hasTriggered;

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            std::list<Creature*> prisonersList;
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_FLESH_EATER, 20.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_HEADHUNTER, 20.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_SNOWSTALKER, 20.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_BERSERKER, 20.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_BONECHILLER, 20.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_ZANDALARI_RAGE_BANNER, 20.0f);

            prisonersList.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

            for (auto&& itr : prisonersList)
                itr->AI()->SetGUID(me->GetGUID());

            prisonersCount = prisonersList.size();
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && --prisonersCount < 1 && !hasTriggered)
        {
            hasTriggered = true;

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_SAVE_SCOUT_BOLD_BREW, DONE);

            delay = 1900;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6150), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Scout Forgefellow 70745
struct npc_bloodsnow_scout_forgefellow : public customCreatureAI
{
    npc_bloodsnow_scout_forgefellow(Creature* creature) : customCreatureAI(creature)
    {
        prisonersCount = 0;
        delay = 0;
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    uint32 prisonersCount;
    uint32 delay;
    bool hasTriggered;

    void Reset() override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData64(me->GetEntry()) != me->GetGUID())
            return;

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            std::list<Creature*> prisonersList;
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_FLESH_EATER, 30.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_HEADHUNTER, 30.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_SNOWSTALKER, 30.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_BERSERKER, 30.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_FROSTMANE_BONECHILLER, 30.0f);
            GetCreatureListWithEntryInGrid(prisonersList, me, NPC_BONECHILLER_BARAFU, 30.0f);

            prisonersList.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

            for (auto&& itr : prisonersList)
                itr->AI()->SetGUID(me->GetGUID());

            prisonersCount = prisonersList.size();
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && --prisonersCount < 1 && !hasTriggered)
        {
            hasTriggered = true;

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_SAVE_SCOUT_FORGEFELLOW, DONE);

            delay = 3700;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 9700), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Zandalari Rage Banner 70789
struct npc_bloodsnow_zandalari_rage_banner : public customCreatureAI
{
    npc_bloodsnow_zandalari_rage_banner(Creature* creature) : customCreatureAI(creature)
    {
        scoutGUID = ObjectGuid::Empty;
    }

    ObjectGuid scoutGUID;

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        scoutGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!scoutGUID)
            return;

        if (Creature* scout = ObjectAccessor::GetCreature(*me, scoutGUID))
            scout->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Free Scout Stonebeard 141316
class spell_bloodsnow_free_scout_stonebeard : public SpellScript
{
    PrepareSpellScript(spell_bloodsnow_free_scout_stonebeard);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* stoneBeard = GetHitCreature())
        {
            stoneBeard->AI()->DoAction(ACTION_START_INTRO);

            if (Creature* spit = ObjectAccessor::GetCreature(*stoneBeard, stoneBeard->GetInstanceScript() ? stoneBeard->GetInstanceScript()->GetGuidData(NPC_ROASTING_SPIT) : ObjectGuid::Empty))
                spit->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_bloodsnow_free_scout_stonebeard::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Freeze Solid Launcher 141407
class spell_bloodsnow_freeze_solid_launcher : public AuraScript
{
    PrepareAuraScript(spell_bloodsnow_freeze_solid_launcher)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->CastSpell(owner, SPELL_FREEZE_SOLID_SUMMON, true);

            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->AI()->Talk(TALK_INTRO, owner);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_bloodsnow_freeze_solid_launcher::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Freeze Solid Eff 141410
class spell_bloodsnow_freeze_solid_eff : public AuraScript
{
    PrepareAuraScript(spell_bloodsnow_freeze_solid_eff)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* caster = GetCaster())
            if (caster->IsAlive() && caster->ToCreature())
                caster->ToCreature()->DisappearAndDie();
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_bloodsnow_freeze_solid_eff::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Zandalari Rage Eff 141315
class spell_bloodsnow_zandalari_rage_eff : public SpellScript
{
    PrepareSpellScript(spell_bloodsnow_zandalari_rage_eff);

    std::list<WorldObject*> copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->HasAura(SPELL_PERM_FEIGN_DEATH); });
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_FROSTMANE_FLESH_EATER && target->GetEntry() != NPC_FROSTMANE_HEADHUNTER && target->GetEntry() != NPC_FROSTMANE_SNOWSTALKER && target->GetEntry() != NPC_FROSTMANE_BERSERKER && target->GetEntry() != NPC_FROSTMANE_BONECHILLER; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bloodsnow_zandalari_rage_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bloodsnow_zandalari_rage_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bloodsnow_zandalari_rage_eff::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Hekimas Wisdom 141423
class spell_bloodsnow_hekimas_wisdom : public SpellScript
{
    PrepareSpellScript(spell_bloodsnow_hekimas_wisdom);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (caster->GetEntry() == NPC_HEKIMA_THE_WISE)
                caster->GetMap()->SetWorldState(WORLDSTATE_HEKIMAS_HEAL_HALTER, 0);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_bloodsnow_hekimas_wisdom::HandleAfterCast);
    }
};

// 997. Summoned by 142715 - Shimmerweed
class sat_bloodsnow_shimmerweed : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            itr->CastSpell(itr, SPELL_SHIMMERWEED_EFF, true);

            if (Unit* caster = GetCaster())
                caster->RemoveAurasDueToSpell(SPELL_SHIMMERWEED_VISUAL);

            // Achievement Controll
            if (itr->GetInstanceScript())
                itr->GetInstanceScript()->SetData(DATA_SHIMMERWEED, 1);
        }
    }
};

// Areatrigger 9037
class AreaTrigger_at_bloodsnow_grimbolt : public AreaTriggerScript
{
    public:
        AreaTrigger_at_bloodsnow_grimbolt() : AreaTriggerScript("AreaTrigger_at_bloodsnow_grimbolt") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* grimbolt = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_MOUNTAINER_GRIMBOLT) : ObjectGuid::Empty))
                grimbolt->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

void AddSC_blood_in_the_snow()
{
    new creature_script<npc_bloodsnow_varian_wrynn>("npc_bloodsnow_varian_wrynn");
    new creature_script<npc_bloodsnow_mountainer_grimbolt>("npc_bloodsnow_mountainer_grimbolt");
    new creature_script<npc_bloodsnow_roasting_spit>("npc_bloodsnow_roasting_spit");
    new creature_script<npc_bloodsnow_scout_stonebeard>("npc_bloodsnow_scout_stonebeard");
    new creature_script<npc_bloodsnow_frostmane_flesh_eater>("npc_bloodsnow_frostmane_flesh_eater");
    new creature_script<npc_bloodsnow_frostmane_headhunter>("npc_bloodsnow_frostmane_headhunter");
    new creature_script<npc_bloodsnow_frostmane_snowstalker>("npc_bloodsnow_frostmane_snowstalker");
    new creature_script<npc_bloodsnow_frostmane_berserker>("npc_bloodsnow_frostmane_berserker");
    new creature_script<npc_bloodsnow_frostmane_bonechiller>("npc_bloodsnow_frostmane_bonechiller");
    new creature_script<npc_bloodsnow_farastu>("npc_bloodsnow_farastu");
    new creature_script<npc_bloodsnow_freeze_solid>("npc_bloodsnow_freeze_solid");
    new creature_script<npc_bloodsnow_hekima_the_wise>("npc_bloodsnow_hekima_the_wise");
    new creature_script<npc_bloodsnow_frostmane_witch_doctor>("npc_bloodsnow_frostmane_witch_doctor");
    new creature_script<npc_bloodsnow_ironforge_guardian>("npc_bloodsnow_ironforge_guardian");
    new creature_script<npc_bloodsnow_shimmerweed_basket>("npc_bloodsnow_shimmerweed_basket");
    new creature_script<npc_bloodsnow_scout_boldbrew>("npc_bloodsnow_scout_boldbrew");
    new creature_script<npc_bloodsnow_scout_forgefellow>("npc_bloodsnow_scout_forgefellow");
    new creature_script<npc_bloodsnow_zandalari_rage_banner>("npc_bloodsnow_zandalari_rage_banner");

    new spell_script<spell_bloodsnow_free_scout_stonebeard>("spell_bloodsnow_free_scout_stonebeard");
    new aura_script<spell_bloodsnow_freeze_solid_launcher>("spell_bloodsnow_freeze_solid_launcher");
    new aura_script<spell_bloodsnow_freeze_solid_eff>("spell_bloodsnow_freeze_solid_eff");
    new spell_script<spell_bloodsnow_zandalari_rage_eff>("spell_bloodsnow_zandalari_rage_eff");
    new spell_script<spell_bloodsnow_hekimas_wisdom>("spell_bloodsnow_hekimas_wisdom");
    new atrigger_script<sat_bloodsnow_shimmerweed>("sat_bloodsnow_shimmerweed");
    new AreaTrigger_at_bloodsnow_grimbolt();
}