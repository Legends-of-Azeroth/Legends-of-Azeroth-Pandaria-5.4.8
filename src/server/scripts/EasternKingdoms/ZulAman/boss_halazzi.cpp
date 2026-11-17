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
#include "zulaman.h"
#include "Random.h"

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_SPLIT   = 1,
    SAY_COMBINE = 2,
    SAY_ENRAGE  = 3,
    SAY_DEATH   = 4,
};

enum Spells
{
    SPELL_ENRAGE                    = 43139,
    SPELL_WATER_TOTEM               = 97499,
    SPELL_REFRESHING_STREAM         = 97502,
    SPELL_REFRESHING_STREAM_HEAL    = 97505,
    SPELL_LIGHTNING_TOTEM           = 43302,
    SPELL_LIGHTNING                 = 43301,
    SPELL_EARTH_SHOCK               = 43305,
    SPELL_FLAME_SHOCK               = 43303,
    SPELL_SUMMON_LYNX               = 43143,
    SPELL_TRANSFORM_HUMAN           = 43142,
    SPELL_TRANSFORM_HUMAN2          = 43573,
    SPELL_TRANSFORM_LYNX            = 43271,
    SPELL_DUAL_WIELD                = 29651,
    
    SPELL_FIXATE                    = 97486,
    SPELL_SHRED_ARMOR               = 43243,
    SPELL_LYNX_FLURRY               = 43290,
};
enum Adds
{
    NPC_SPIRIT_LYNX     = 24143,
    NPC_LIGHTNING_TOTEM = 24224,
    NPC_WATER_TOTEM     = 52755,
};

enum Events
{
    EVENT_ENRAGE            = 1,
    EVENT_EARTH_SHOCK       = 2,
    EVENT_FLAME_SHOCK       = 3,
    EVENT_WATER_TOTEM       = 4,
    EVENT_LIGHTNING_TOTEM   = 5,
   
    EVENT_FIXATE            = 6,
    EVENT_LYNX_FLURRY       = 7,
    EVENT_SHRED_ARMOR       = 8,

    EVENT_LIGHTNING         = 9,
};

enum Phases
{
    PHASE_LYNX  = 1,
    PHASE_HUMAN = 2,
};

enum Actions
{
    ACTION_COMBINE  = 1,
};

class boss_halazzi : public CreatureScript
{
    public:
        boss_halazzi(): CreatureScript("boss_halazzi") { }

        struct boss_halazziAI : public BossAI
        {
            boss_halazziAI(Creature* creature) : BossAI(creature, DATA_HALAZZI)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint8 phase;
            uint32 health;
            bool isLynx; 

            void Reset() override
            {
                _Reset();

                //DoCast(me, SPELL_DUAL_WIELD, true);
                phase = 0;
                isLynx = true;
                
                me->RemoveAurasDueToSpell(SPELL_TRANSFORM_HUMAN2);
                DoCast(me, SPELL_TRANSFORM_LYNX, true);

                me->GetMap()->SetWorldState(WORLDSTATE_TUNNEL_VISION, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                phase = 0;
                isLynx = true;
                health = me->GetHealth();
                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                events.ScheduleEvent(EVENT_WATER_TOTEM, urand(5000, 15000));
                DoZoneInCombat();
                instance->SetBossState(DATA_HALAZZI, IN_PROGRESS);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_TRANSFORM_LYNX)
                    EnterPhase(PHASE_LYNX);
                else if (spell->Id == SPELL_TRANSFORM_HUMAN2)
                    EnterPhase(PHASE_HUMAN);
            }

            void EnterPhase(Phases NextPhase)
            {
                if (!me->IsInCombat())
                    return;

                switch (NextPhase)
                {
                    case PHASE_LYNX:
                        Talk(SAY_COMBINE);
                        events.CancelEvent(EVENT_EARTH_SHOCK);
                        events.CancelEvent(EVENT_FLAME_SHOCK);
                        events.CancelEvent(EVENT_LIGHTNING_TOTEM);
                        events.ScheduleEvent(EVENT_ENRAGE, urand(3000, 7000));
                        summons.DespawnEntry(NPC_SPIRIT_LYNX);
                        me->RemoveAurasDueToSpell(SPELL_TRANSFORM_HUMAN2);
                        me->SetHealth(health);
                        break;                    
                    case PHASE_HUMAN:
                        Talk(SAY_SPLIT);
                        events.CancelEvent(EVENT_ENRAGE);
                        DoCast(me, SPELL_SUMMON_LYNX, true);
                        events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(5000, 20000));
                        events.ScheduleEvent(EVENT_FLAME_SHOCK, urand(5000, 20000));
                        events.ScheduleEvent(EVENT_LIGHTNING_TOTEM, urand(3000, 10000));
                    break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
            }
            
            void DoAction(int32 action) override
            {
                if (action == ACTION_COMBINE)
                {
                    isLynx = true;
                    phase++;
                    me->SetHealth(health);
                    DoCast(me, SPELL_TRANSFORM_LYNX);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HealthBelowPct(60) && isLynx && phase == 0)
                {
                    isLynx = false;
                    phase++;
                    health = me->GetHealth();
                    DoCastAOE(SPELL_TRANSFORM_HUMAN);
                    return;
                }
                else if (me->HealthBelowPct(30) && isLynx && phase == 2)
                {
                    isLynx = false;
                    phase++;
                    health = me->GetHealth();
                    DoCastAOE(SPELL_TRANSFORM_HUMAN);
                    return;
                }
                else if (me->HealthBelowPct(20) && !isLynx && (phase == 1 || phase == 3))
                {
                    isLynx = true;
                    phase++;
                    me->SetHealth(health);
                    DoCast(me, SPELL_TRANSFORM_LYNX);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                     switch (eventId)
                     {
                        case EVENT_ENRAGE:
                            if (roll_chance_i(25))
                                Talk(SAY_ENRAGE);
                            DoCast(me, SPELL_ENRAGE);
                            events.ScheduleEvent(EVENT_ENRAGE, urand(18000, 20000));
                            break;
                        case EVENT_WATER_TOTEM:
                            DoCast(me, SPELL_WATER_TOTEM);
                            events.ScheduleEvent(EVENT_WATER_TOTEM, 30000);
                            break;
                        case EVENT_LIGHTNING_TOTEM:
                            DoCast(me, SPELL_LIGHTNING_TOTEM);
                            events.ScheduleEvent(EVENT_LIGHTNING_TOTEM, urand(20000, 25000));
                            break;
                        case EVENT_EARTH_SHOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_EARTH_SHOCK);
                            events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(10000, 20000));
                            break;
                        case EVENT_FLAME_SHOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_FLAME_SHOCK);
                            events.ScheduleEvent(EVENT_FLAME_SHOCK, urand(10000, 20000));
                            break;
                     }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_halazziAI>(creature);
        }
};

class npc_halazzi_lynx : public CreatureScript
{
    public:
        npc_halazzi_lynx() : CreatureScript("npc_halazzi_lynx") { }

        struct npc_halazzi_lynxAI : public ScriptedAI
        {
            npc_halazzi_lynxAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            bool bDespawn;
            EventMap events;

            void Reset() override
            {
                events.Reset();
                bDespawn = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHRED_ARMOR, urand(3000, 10000));
                events.ScheduleEvent(EVENT_LYNX_FLURRY, urand(5000, 8000));
                events.ScheduleEvent(EVENT_FIXATE, urand(6000, 9000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HealthBelowPct(20) && !bDespawn)
                {
                    bDespawn = true;
                    if (Creature* pHalazzi = me->FindNearestCreature(NPC_HALAZZI, 300.0f))
                        pHalazzi->AI()->DoAction(ACTION_COMBINE);
                    return;
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                     switch (eventId)
                     {
                        case EVENT_FIXATE:
                            DoResetThreat();
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoCastVictim(SPELL_FIXATE, true);
                                AttackStart(target);
                            }
                            events.ScheduleEvent(EVENT_FIXATE, 10000);
                            break;
                        case EVENT_LYNX_FLURRY:
                            DoCast(me, SPELL_LYNX_FLURRY);
                            events.ScheduleEvent(EVENT_LYNX_FLURRY, urand(12000, 20000));
                            break;
                        case EVENT_SHRED_ARMOR:
                            DoCastVictim(SPELL_SHRED_ARMOR);
                            events.ScheduleEvent(EVENT_SHRED_ARMOR, 7000);
                            break;
                     }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_halazzi_lynxAI>(creature);
        }
};

class npc_halazzi_water_totem : public CreatureScript
{
    public:
        npc_halazzi_water_totem() : CreatureScript("npc_halazzi_water_totem") { }

        struct npc_halazzi_water_totemAI : public ScriptedAI
        {
            npc_halazzi_water_totemAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_REFRESHING_STREAM, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_TUNNEL_VISION, 0);
            }
      };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_halazzi_water_totemAI>(creature);
        }
};

class npc_halazzi_lightning_totem : public CreatureScript
{
    public:
        npc_halazzi_lightning_totem() : CreatureScript("npc_halazzi_lightning_totem") { }

        struct npc_halazzi_lightning_totemAI : public ScriptedAI
        {
            npc_halazzi_lightning_totemAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }
            
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LIGHTNING, 1000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_TUNNEL_VISION, 0);
            }
            
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                     switch (eventId)
                     {
                        case EVENT_LIGHTNING:
                            DoCastVictim(SPELL_LIGHTNING);
                            events.ScheduleEvent(EVENT_LIGHTNING, 3000);
                            break;
                     }
                }
            }
      };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_halazzi_lightning_totemAI>(creature);
        }
};

void AddSC_boss_halazzi()
{
    new boss_halazzi();
    new npc_halazzi_lynx();
    new npc_halazzi_water_totem();
    new npc_halazzi_lightning_totem();
}
