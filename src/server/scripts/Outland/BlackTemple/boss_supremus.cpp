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

/* ScriptData
SDName: Boss_Supremus
SD%Complete: 95
SDComment: Need to implement molten punch
SDCategory: Black Temple
EndScriptData */

#include "PassiveAI.h"
#include "ScriptPCH.h"
#include "black_temple.h"

enum Supremus
{
    EMOTE_NEW_TARGET            = 0,
    EMOTE_PUNCH_GROUND          = 1,
    EMOTE_GROUND_CRACK          = 2,

    // Spells
    SPELL_MOLTEN_PUNCH          = 40126,
    SPELL_HATEFUL_STRIKE        = 41926,
    SPELL_MOLTEN_FLAME          = 40980,
    SPELL_VOLCANIC_ERUPTION     = 40117,
    SPELL_VOLCANIC_SUMMON       = 40276,
    SPELL_BERSERK               = 45078,

    CREATURE_VOLCANO            = 23085,
    CREATURE_STALKER            = 23095,

    PHASE_STRIKE                = 1,
    PHASE_CHASE                 = 2,

    EVENT_BERSERK               = 1,
    EVENT_SWITCH_PHASE          = 2,
    EVENT_FLAME                 = 3,
    EVENT_VOLCANO               = 4,
    EVENT_SWITCH_TARGET         = 5,
    EVENT_HATEFUL_STRIKE        = 6,

    GCD_CAST                    = 1
};

class molten_flame : public CreatureScript
{
    public:
        molten_flame() : CreatureScript("molten_flame") { }

        struct molten_flameAI : public NullCreatureAI
        {
            molten_flameAI(Creature* creature) : NullCreatureAI(creature) { }

            void InitializeAI() override
            {
                float x, y, z;
                me->GetNearPoint(me, x, y, z, 1, 100, float(M_PI * 2 * rand_norm()));
                me->GetMotionMaster()->MovePoint(0, x, y, z);
                me->SetVisible(false);
                me->CastSpell(me,SPELL_MOLTEN_FLAME,true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new molten_flameAI(creature);
        }
};

class boss_supremus : public CreatureScript
{
    public:
        boss_supremus() : CreatureScript("boss_supremus") { }

        struct boss_supremusAI : public ScriptedAI
        {
            boss_supremusAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            SummonList summons;
            uint32 phase;

            void Reset() override
            {
                if (instance)
                {
                    if (me->IsAlive())
                    {
                        instance->SetData(DATA_SUPREMUS_EVENT, NOT_STARTED);
                        //ToggleDoors(true);
                    }
                    //else ToggleDoors(false);
                }

                phase = 0;

                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance || !instance->CheckRequiredBosses(DATA_SUPREMUS_EVENT, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->SetData(DATA_KICK_PLAYERS, 1);
                    return;
                }

                if (instance)
                    instance->SetData(DATA_SUPREMUS_EVENT, IN_PROGRESS);

                ChangePhase();
                events.ScheduleEvent(EVENT_BERSERK, 900000, GCD_CAST);
                events.ScheduleEvent(EVENT_FLAME, 20000, GCD_CAST);
            }

            void ChangePhase()
            {
                if (!phase || phase == PHASE_CHASE)
                {
                    phase = PHASE_STRIKE;
                    DummyEntryCheckPredicate pred;
                    summons.DoAction(EVENT_VOLCANO, pred);
                    events.ScheduleEvent(EVENT_HATEFUL_STRIKE, 5000, GCD_CAST, PHASE_STRIKE);
                    me->SetSpeed(MOVE_RUN, 1.2f);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
                }
                else
                {
                    phase = PHASE_CHASE;
                    events.ScheduleEvent(EVENT_VOLCANO, 5000, GCD_CAST, PHASE_CHASE);
                    events.ScheduleEvent(EVENT_SWITCH_TARGET, 10000, 0, PHASE_CHASE);
                    me->SetSpeed(MOVE_RUN, 0.9f);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
                }
                DoResetThreat();
                DoZoneInCombat();
                events.SetPhase(phase);
                events.ScheduleEvent(EVENT_SWITCH_PHASE, 60000, GCD_CAST);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->SetData(DATA_SUPREMUS_EVENT, DONE);
                    instance->HandleGameObject(instance->GetGuidData(DATA_GAMEOBJECT_SUPREMUS_DOORS), true);
                }
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            Unit* CalculateHatefulStrikeTarget()
            {
                uint32 health = 0;
                Unit* target = nullptr;

                ThreatContainer::StorageType const& threatlist = me->GetThreatManager().getThreatList();
                ThreatContainer::StorageType::const_iterator i = threatlist.begin();
                for (i = threatlist.begin(); i!= threatlist.end(); ++i)
                {
                    Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                    if (unit && me->IsWithinMeleeRange(unit))
                    {
                        if (unit->GetHealth() > health)
                        {
                            health = unit->GetHealth();
                            target = unit;
                        }
                    }
                }

                return target;
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
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_FLAME:
                            DoCast(me, SPELL_MOLTEN_PUNCH);
                            events.DelayEvents(1500, GCD_CAST);
                            events.ScheduleEvent(EVENT_FLAME, 20000, GCD_CAST);
                            break;
                        case EVENT_HATEFUL_STRIKE:
                            if (Unit* target = CalculateHatefulStrikeTarget())
                                DoCast(target, SPELL_HATEFUL_STRIKE);
                            events.DelayEvents(1000, GCD_CAST);
                            events.ScheduleEvent(EVENT_HATEFUL_STRIKE, 5000, GCD_CAST, PHASE_STRIKE);
                            break;
                        case EVENT_SWITCH_TARGET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 5000000.0f);
                                Talk(EMOTE_NEW_TARGET);
                            }
                            events.ScheduleEvent(EVENT_SWITCH_TARGET, 10000, 0, PHASE_CHASE);
                            break;
                        case EVENT_VOLCANO:
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 999, true);
                            if (!target) target = me->GetVictim();
                            if (target)
                            {
                                //DoCast(target, SPELL_VOLCANIC_SUMMON);//movement bugged
                                me->SummonCreature(CREATURE_VOLCANO,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,30000ms);
                                Talk(EMOTE_GROUND_CRACK);
                                events.DelayEvents(1500, GCD_CAST);
                            }
                            events.ScheduleEvent(EVENT_VOLCANO, 10000, GCD_CAST, PHASE_CHASE);
                            return;
                        }
                        case EVENT_SWITCH_PHASE:
                            ChangePhase();
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_supremusAI(creature);
        }
};

class npc_volcano : public CreatureScript
{
    public:
        npc_volcano() : CreatureScript("npc_volcano") { }

        struct npc_volcanoAI : public ScriptedAI
        {
            npc_volcanoAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                //DoCast(me, SPELL_VOLCANIC_ERUPTION);
                me->SetReactState(REACT_PASSIVE);
                wait = 3000;
            }

            uint32 wait;

            void JustEngagedWith(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void DoAction(int32 /*action*/) override
            {
                me->RemoveAura(SPELL_VOLCANIC_ERUPTION);
            }

            void UpdateAI(uint32 diff) override
            {
                if (wait <= diff)//wait 3secs before casting
                {
                    DoCast(me, SPELL_VOLCANIC_ERUPTION);
                    wait = 60000;
                }
                else wait -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_volcanoAI(creature);
        }
};

void AddSC_boss_supremus()
{
    new boss_supremus();
    new molten_flame();
    new npc_volcano();
}
