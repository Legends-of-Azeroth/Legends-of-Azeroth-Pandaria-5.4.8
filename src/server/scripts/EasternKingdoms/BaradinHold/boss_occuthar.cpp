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
#include "baradin_hold.h"

enum Spells
{
    SPELL_BERSERK                   = 47008, 
    SPELL_SEARING_SHADOW            = 96913, 

    SPELL_FOCUSED_FIRE              = 96872,
    SPELL_FOCUSED_FIRE_DUMMY        = 96886,
    SPELL_FOCUSED_FIRE_1            = 96884,

    SPELL_EYE_OF_OCCUTHAR           = 96920, 
    SPELL_EYE_OF_OCCUTHAR_VEHICLE   = 96932,

    SPELL_EYE_OF_OCCUTHAR_DMG       = 96942,

    SPELL_OCCUTHARS_DESTRUCTION     = 96968,
};

enum Events
{
    EVENT_BERSERK           = 1,
    EVENT_SEARING_SHADOWS   = 2,
    EVENT_EYE_OF_OCCUTHAR   = 3,
    EVENT_FOCUSED_FIRE      = 4,
};

enum Adds
{
    NPC_EYE_OF_OCCUTHAR = 52389,
    NPC_EYESTALK        = 52369,
};

class boss_occuthar : public CreatureScript
{
    public:
        boss_occuthar() : CreatureScript("boss_occuthar") { }

        struct boss_occutharAI : public BossAI
        {
            boss_occutharAI(Creature* creature) : BossAI(creature, DATA_OCCUTHAR) 
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

            void Reset() override
            {
                _Reset();
                
                events.Reset();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BERSERK, 300000);
                events.ScheduleEvent(EVENT_EYE_OF_OCCUTHAR, 59000);
                events.ScheduleEvent(EVENT_FOCUSED_FIRE, 26000);
                events.ScheduleEvent(EVENT_SEARING_SHADOWS, 24000); 
                instance->SetBossState(DATA_OCCUTHAR, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
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
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SEARING_SHADOWS:
                            DoCast(me->GetVictim(), SPELL_SEARING_SHADOW);
                            events.ScheduleEvent(EVENT_SEARING_SHADOWS, 24000);
                            break;
                        case EVENT_FOCUSED_FIRE:
                            DoCastAOE(SPELL_FOCUSED_FIRE);
                            events.ScheduleEvent(EVENT_FOCUSED_FIRE, 14000);
                            break;
                        case EVENT_EYE_OF_OCCUTHAR:
                            DoCastAOE(SPELL_EYE_OF_OCCUTHAR);
                            events.RescheduleEvent(EVENT_FOCUSED_FIRE, 26000);
                            events.ScheduleEvent(EVENT_EYE_OF_OCCUTHAR, 59000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_occutharAI>(creature);
        }
};

class npc_occuthar_eyestalk : public CreatureScript
{
    public:
        npc_occuthar_eyestalk() : CreatureScript("npc_occuthar_eyestalk") { }

        struct npc_occuthar_eyestalkAI : public ScriptedAI
        {
            npc_occuthar_eyestalkAI(Creature* creature) : ScriptedAI(creature)
            {
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (Creature* pOccuthar = me->FindNearestCreature(NPC_OCCUTHAR, 200.0f))
                {
                    if (pOccuthar->IsInCombat())
                        me->SetInCombatWithZone();

                    pOccuthar->CastSpell(me, SPELL_FOCUSED_FIRE_1, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_occuthar_eyestalkAI>(creature);
        }
};

class npc_occuthar_eye_of_occuthar : public CreatureScript
{
    public:
        npc_occuthar_eye_of_occuthar() : CreatureScript("npc_occuthar_eye_of_occuthar") { }

        struct npc_occuthar_eye_of_occutharAI : public ScriptedAI
        {
            npc_occuthar_eye_of_occutharAI(Creature* creature) : ScriptedAI(creature)
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
                me->SetReactState(REACT_PASSIVE);
                uiDestructionTimer = 9500;
                bDestruction = false;
                SetCombatMovement(false);
            }

            uint32 uiDestructionTimer;
            bool bDestruction;

            void UpdateAI(uint32 diff) override
            {
                if (uiDestructionTimer <= diff && !bDestruction)
                {
                    bDestruction = true;
                    DoCastAOE(SPELL_OCCUTHARS_DESTRUCTION, true);
                }
                else
                    uiDestructionTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_occuthar_eye_of_occutharAI>(creature);
        }
};

class spell_occuthar_eye_of_occuthar : public SpellScriptLoader
{
    public:
        spell_occuthar_eye_of_occuthar() : SpellScriptLoader("spell_occuthar_eye_of_occuthar") { }

        class spell_occuthar_eye_of_occuthar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_occuthar_eye_of_occuthar_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_EYE_OF_OCCUTHAR_VEHICLE, true);
                
                if (Creature* pEye = GetCaster()->SummonCreature(NPC_EYE_OF_OCCUTHAR, GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ(), GetCaster()->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 11000ms))
                    pEye->CastSpell(GetCaster(), SPELL_EYE_OF_OCCUTHAR_DMG, true); 
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_occuthar_eye_of_occuthar_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_occuthar_eye_of_occuthar_SpellScript();
        }
};

void AddSC_boss_occuthar()
{
    new boss_occuthar();
    new npc_occuthar_eyestalk();
    new npc_occuthar_eye_of_occuthar();
    new spell_occuthar_eye_of_occuthar();
}
