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
#include "drak_tharon_keep.h"

enum Spells
{
    // skeletal spells (phase 1)
    SPELL_CURSE_OF_LIFE                           = 49527,
    SPELL_RAIN_OF_FIRE                            = 49518,
    SPELL_SHADOW_VOLLEY                           = 49528,
    SPELL_DECAY_FLESH                             = 49356, //casted at end of phase 1, starts phase 2
    // flesh spells (phase 2)
    SPELL_GIFT_OF_THARON_JA                       = 52509,
    SPELL_EYE_BEAM                                = 49544,
    SPELL_LIGHTNING_BREATH                        = 49537,
    SPELL_POISON_CLOUD                            = 49548,
    SPELL_RETURN_FLESH                            = 53463, //Channeled spell ending phase two and returning to phase 1. This ability will stun the party for 6 seconds.
    SPELL_DUMMY                                   = 49551,
    SPELL_DUMMY_TRIGGERED                         = 49375,
    SPELL_FLESH_VISUAL                            = 52582,
    SPELL_CLEAR_GIFT_OF_THAROJA                   = 53242,
    SPELL_ACHIEVEMENT_CHECK                       = 61863,
};

enum Events
{
    EVENT_PHASE                                   = 1,
    EVENT_RAIN_OF_FIRE,
    EVENT_SHADOW_VOLLEY,
    EVENT_CURSE_OF_LIFE,
    EVENT_LIGHTNING_BREATH,
    EVENT_EYE_BEAM,
    EVENT_POISON_CLOUD,
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_KILL                                      = 1,
    SAY_FLESH                                     = 2,
    SAY_SKELETON                                  = 3,
    SAY_DEATH                                     = 4,
};
enum Models
{
    MODEL_FLESH                                   = 27073,
    MODEL_SKELETON                                = 9784, // 27511
};
enum Phase
{
    PHASE_SKELETAL = 1,
    PHASE_GOING_FLESH,
    PHASE_FLESH,
    PHASE_GOING_SKELETAL,
};

class boss_tharon_ja : public CreatureScript
{
    public:
        boss_tharon_ja() : CreatureScript("boss_tharon_ja") { }

        struct boss_tharon_jaAI : public ScriptedAI
        {
            boss_tharon_jaAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                transformed = false;

                events.Reset();
                events.SetPhase(PHASE_SKELETAL);
                events.ScheduleEvent(EVENT_RAIN_OF_FIRE,  urand( 7000, 16000), 0, PHASE_SKELETAL);
                events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(10000, 21000), 0, PHASE_SKELETAL);
                events.ScheduleEvent(EVENT_CURSE_OF_LIFE, urand(12000, 23000), 0, PHASE_SKELETAL);

                me->SetDisplayId(me->GetNativeDisplayId());

                if (instance)
                    instance->SetData(DATA_THARON_JA_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(SAY_AGGRO, who);

                if (instance)
                    instance->SetData(DATA_THARON_JA_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(SAY_KILL, victim);
            }

            void JustDied(Unit* killer) override
            {
                DoCastAOE(SPELL_ACHIEVEMENT_CHECK, true);

                Talk(SAY_DEATH, killer);

                DoCastAOE(SPELL_CLEAR_GIFT_OF_THAROJA, true);

                if (instance)
                    instance->SetData(DATA_THARON_JA_EVENT, DONE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!transformed && me->HealthBelowPctDamaged(50, damage))
                {
                    transformed = true;

                    me->InterruptNonMeleeSpells(false);
                    DoCastAOE(SPELL_DECAY_FLESH);

                    Talk(SAY_FLESH);

                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();

                    events.Reset();
                    events.SetPhase(PHASE_GOING_FLESH);
                    events.ScheduleEvent(EVENT_PHASE, 3500, 0, PHASE_GOING_FLESH);
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FLESH_VISUAL:
                        if (events.IsInPhase(PHASE_GOING_FLESH))
                            me->SetDisplayId(MODEL_FLESH);
                        else
                            me->DeMorph();
                        break;
                    case SPELL_DUMMY_TRIGGERED:
                        me->InterruptNonMeleeSpells(false);
                        DoCastAOE(SPELL_RETURN_FLESH);

                        Talk(SAY_SKELETON);

                        events.Reset();
                        events.SetPhase(PHASE_GOING_SKELETAL);
                        events.ScheduleEvent(EVENT_PHASE, 3000, 0, PHASE_GOING_SKELETAL);
                        break;
                }
            }
            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_GIFT_OF_THARON_JA:
                        target->SetDisplayId(MODEL_SKELETON);
                        break;
                    case SPELL_RETURN_FLESH:
                    case SPELL_CLEAR_GIFT_OF_THAROJA:
                        target->DeMorph();
                        target->RemoveAurasDueToSpell(SPELL_GIFT_OF_THARON_JA);
                        break;
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
                    switch (eventId)
                    {
                        case EVENT_PHASE:
                            if (events.IsInPhase(PHASE_GOING_FLESH))
                            {
                                DoCastAOE(SPELL_GIFT_OF_THARON_JA);
                                DoCast(me, SPELL_FLESH_VISUAL, true);
                                DoCast(me, SPELL_DUMMY, true);

                                me->InitializeReactState();
                                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                    AttackStart(target);

                                events.Reset();
                                events.SetPhase(PHASE_FLESH);
                                events.ScheduleEvent(EVENT_LIGHTNING_BREATH, 4500, 0, PHASE_FLESH);
                                events.ScheduleEvent(EVENT_POISON_CLOUD, 8000, 0, PHASE_FLESH);
                                events.ScheduleEvent(EVENT_EYE_BEAM, urand(13000, 15000), 0, PHASE_FLESH);
                            }
                            else if (events.IsInPhase(PHASE_GOING_SKELETAL))
                            {
                                DoCast(me, SPELL_FLESH_VISUAL, true);

                                events.Reset();
                                events.SetPhase(PHASE_SKELETAL);
                                events.ScheduleEvent(EVENT_RAIN_OF_FIRE,  urand( 7000, 16000), 0, PHASE_SKELETAL);
                                events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(10000, 21000), 0, PHASE_SKELETAL);
                                events.ScheduleEvent(EVENT_CURSE_OF_LIFE, urand(12000, 23000), 0, PHASE_SKELETAL);
                            }
                            break;
                        case EVENT_RAIN_OF_FIRE:
                            DoCastAOE(SPELL_RAIN_OF_FIRE);
                            events.ScheduleEvent(EVENT_RAIN_OF_FIRE, 17000, 0, PHASE_SKELETAL);
                            break;
                        case EVENT_SHADOW_VOLLEY:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            events.ScheduleEvent(EVENT_SHADOW_VOLLEY, 17000, 0, PHASE_SKELETAL);
                            break;
                        case EVENT_CURSE_OF_LIFE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30, true))
                                DoCast(target, SPELL_CURSE_OF_LIFE);
                            events.ScheduleEvent(EVENT_CURSE_OF_LIFE, 18000, 0, PHASE_SKELETAL);
                            break;
                        case EVENT_LIGHTNING_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30, true))
                                DoCast(target, SPELL_LIGHTNING_BREATH);
                            events.ScheduleEvent(EVENT_LIGHTNING_BREATH, urand(10000, 13000), 0, PHASE_FLESH);
                            break;
                        case EVENT_EYE_BEAM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 35, true))
                                DoCast(target, SPELL_EYE_BEAM);
                            // Impossible to repeat
                            break;
                        case EVENT_POISON_CLOUD:
                            DoCastAOE(SPELL_POISON_CLOUD);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                if (events.IsInPhase(PHASE_SKELETAL) || events.IsInPhase(PHASE_FLESH))
                    DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            EventMap events;
            bool transformed;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_tharon_jaAI(creature);
        }
};

class cond_draktharon_keep_drakkari_infighting : public ConditionScript
{
    public:
        cond_draktharon_keep_drakkari_infighting() : ConditionScript("cond_draktharon_keep_drakkari_infighting") { }

        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[1])
                if (Unit* unit = source.mConditionTargets[1]->ToUnit())
                    if (Unit* victim = unit->GetVictim())
                        if (victim->GetFaction() == 1814 || victim->GetFaction() == 1965)
                            return true;

            return false;
        }
};

void AddSC_boss_tharon_ja()
{
    new boss_tharon_ja();
    new cond_draktharon_keep_drakkari_infighting();
}
