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

enum ScriptTexts
{
    SAY_AGGRO           = 0,
    SAY_DEATH           = 1,
    SAY_INTRO           = 2,
    SAY_WIPE            = 3, 
    SAY_KILL            = 4,
    SAY_SEETHING_HATE   = 5,
    SAY_SKEWER          = 6,
};

enum Spells
{
    SPELL_SKEWER                = 104936,
    SPELL_SEETHING_HATE_DUMMY   = 105065,
    SPELL_SEETHING_HATE         = 105067,
    SPELL_SEETHING_HATE_DMG     = 105069,
    SPELL_BLADE_DANCE_CHARGE    = 105726,
    SPELL_BLADE_DANCE_DUMMY     = 106248,
    SPELL_BLADE_DANCE_SELF      = 105828,
    SPELL_BLADE_DANCE_AURA_1    = 105784,
    SPELL_BLADE_DANCE_AURA_2    = 104995,
    SPELL_BERSERK               = 47008,
};

enum Events
{
    EVENT_SKEWER                = 1,
    EVENT_SEETHING_HATE         = 2,
    EVENT_BLADE_DANCE           = 3,
    EVENT_BERSERK               = 4,
    EVENT_BLADE_DANCE_CHARGE    = 5,
    EVENT_BLADE_DANCE_AURA      = 6,
};

class boss_alizabal : public CreatureScript
{
    public:
        boss_alizabal() : CreatureScript("boss_alizabal") { }

        struct boss_alizabalAI : public BossAI
        {
            boss_alizabalAI(Creature* creature) : BossAI(creature, DATA_ALIZABAL) 
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
                introDone = false;
            }

            bool introDone;
            uint8 uiCharges;

            void Reset() override
            {
                _Reset();
                
                events.Reset();
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!introDone && me->IsWithinDistInMap(who, 70.0f))
                {
                    Talk(SAY_INTRO);
                    introDone = true;
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                Talk(SAY_WIPE);
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                    events.ScheduleEvent(EVENT_BLADE_DANCE_AURA, 500);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                uiCharges = 0;
                events.ScheduleEvent(EVENT_BERSERK, 300000);
                events.ScheduleEvent(EVENT_BLADE_DANCE, 25000);
                if (urand(0, 1))
                {
                    events.ScheduleEvent(EVENT_SKEWER, 8000);
                    events.ScheduleEvent(EVENT_SEETHING_HATE, 16000);
                }
                else
                {
                    events.ScheduleEvent(EVENT_SKEWER, 16000);
                    events.ScheduleEvent(EVENT_SEETHING_HATE, 8000);
                }
                Talk(SAY_AGGRO);
                instance->SetBossState(DATA_ALIZABAL, IN_PROGRESS);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim && victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SKEWER:
                            Talk(SAY_SKEWER);
                            DoCast(me->GetVictim(), SPELL_SKEWER);
                            events.ScheduleEvent(EVENT_SKEWER, 20500);
                            break;
                        case EVENT_SEETHING_HATE:
                            Talk(SAY_SEETHING_HATE);
                            DoCastAOE(SPELL_SEETHING_HATE_DUMMY);
                            events.ScheduleEvent(EVENT_SEETHING_HATE, 20500);
                            break;
                        case EVENT_BLADE_DANCE:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            DoCast(me, SPELL_BLADE_DANCE_SELF, true);
                            //DoCast(me, SPELL_BLADE_DANCE, true);
                            DoCastAOE(SPELL_BLADE_DANCE_DUMMY, true);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_BLADE_DANCE, 60000);
                            events.ScheduleEvent(EVENT_BLADE_DANCE_CHARGE, 4000);
                            if (urand(0, 1))
                            {
                                events.RescheduleEvent(EVENT_SKEWER, 23000);
                                events.RescheduleEvent(EVENT_SEETHING_HATE, 31000);
                            }
                            else
                            {
                                events.RescheduleEvent(EVENT_SKEWER, 31000);
                                events.RescheduleEvent(EVENT_SEETHING_HATE, 23000);
                            }
                            break;
                        case EVENT_BLADE_DANCE_CHARGE:
                            uiCharges++;
                            if (uiCharges >= 3)
                            {
                                uiCharges = 0;
                                me->InterruptNonMeleeSpells(false);
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            }
                            else
                            {
                                DoCastAOE(SPELL_BLADE_DANCE_DUMMY, true);
                                me->ClearUnitState(UNIT_STATE_CASTING);
                                events.ScheduleEvent(EVENT_BLADE_DANCE_CHARGE, 4000);
                            }
                            break;
                        case EVENT_BLADE_DANCE_AURA:
                            DoCast(me, SPELL_BLADE_DANCE_AURA_2, true);
                            DoCast(me, SPELL_BLADE_DANCE_AURA_1);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_alizabalAI>(creature);
        }
};

class spell_alizabal_seething_hate : public SpellScriptLoader
{
    public:
        spell_alizabal_seething_hate() : SpellScriptLoader("spell_alizabal_seething_hate") { }

        class spell_alizabal_seething_hate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_alizabal_seething_hate_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_SEETHING_HATE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_alizabal_seething_hate_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_alizabal_seething_hate_SpellScript();
        }
};

class spell_alizabal_blade_dance : public SpellScriptLoader
{
    public:
        spell_alizabal_blade_dance() : SpellScriptLoader("spell_alizabal_blade_dance") { }

        class spell_alizabal_blade_dance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_alizabal_blade_dance_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_BLADE_DANCE_CHARGE, true);
                GetCaster()->ClearUnitState(UNIT_STATE_CASTING);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_alizabal_blade_dance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_alizabal_blade_dance_SpellScript();
        }
};

class spell_alizabal_blade_dance_dmg : public SpellScriptLoader
{
    public:
        spell_alizabal_blade_dance_dmg() : SpellScriptLoader("spell_alizabal_blade_dance_dmg") { }

        class spell_alizabal_blade_dance_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_alizabal_blade_dance_dmg_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                PreventHitDamage();
                uint32 ticks = 1;
                if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_BLADE_DANCE_AURA_2, EFFECT_0))
                    ticks = std::max(aurEff->GetTickNumber(), ticks);

                SetHitDamage(urand(11875, 13125) * ticks);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_alizabal_blade_dance_dmg_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_alizabal_blade_dance_dmg_SpellScript();
        }
};

void AddSC_boss_alizabal()
{
    new boss_alizabal();
    new spell_alizabal_seething_hate();
    new spell_alizabal_blade_dance();
    new spell_alizabal_blade_dance_dmg();
}
