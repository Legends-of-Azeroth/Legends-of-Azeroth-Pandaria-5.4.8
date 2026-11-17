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
#include "sunken_temple.h"

enum ScriptTexts
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,
    SAY_KILL  = 2,
};

enum Spells
{
    SPELL_HEALING_WAVE      = 12492,
    SPELL_FLAMESTRIKE       = 12468,
    SPELL_EARTHGRAB_TOTEM   = 8376,
    SPELL_HEX               = 12479,
    SPELL_HEX_EFFECT        = 12480,
    SPELL_CURSE_OF_WEAKNESS = 12493,
    SPELL_SHADOW_WORD_PAIN  = 11639,
    SPELL_SHADOWBOLT        = 12471,
    SPELL_EARTHGRAB         = 8377,
};

enum Events
{
    EVENT_INTRO          = 1,
    EVENT_HEALING_WAVE   = 2,
    EVENT_FLAMESTRIKE    = 3,
    EVENT_HEX            = 4,
    EVENT_CURSE_WEAKNESS = 5,
    EVENT_SWP            = 6,
    EVENT_SHADOWBOLT     = 7,
    EVENT_EARTHGRAB      = 8,
};

class boss_jammalan_prophet : public CreatureScript
{
    public:
        boss_jammalan_prophet() : CreatureScript("boss_jammalan_prophet") { }

        struct boss_jammalan_prophetAI : public BossAI
        {
            boss_jammalan_prophetAI(Creature* creature) : BossAI(creature, DATA_JAMMAL) { }

            void InitializeAI() override
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
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_INTRO, 2 * IN_MILLISECONDS);
                DoZoneInCombat();
                instance->SetBossState(DATA_JAMMAL, IN_PROGRESS);

                events.ScheduleEvent(EVENT_HEALING_WAVE, urand(11 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEX, 15.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FLAMESTRIKE, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_EARTHGRAB, urand(6 * IN_MILLISECONDS, 26 * IN_MILLISECONDS));
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->AddUnitState(UNIT_STATE_ROOT);
                summon->CastSpell(summon, SPELL_EARTHGRAB, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SetData(DATA_JAMMAL, DONE);
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
                        case EVENT_HEALING_WAVE:
                            DoCast(me, SPELL_HEALING_WAVE);
                            events.ScheduleEvent(EVENT_HEALING_WAVE, urand(15.5 * IN_MILLISECONDS, 19.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_HEX:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterPredicate(me)))
                                DoCast(target, SPELL_HEX);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0,80.0f,true))
                                DoCast(target, SPELL_HEX);

                            events.ScheduleEvent(EVENT_HEX, 15.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FLAMESTRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterPredicate(me)))
                                DoCast(target, SPELL_FLAMESTRIKE);

                            events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        case EVENT_EARTHGRAB:
                            DoCast(me, SPELL_EARTHGRAB_TOTEM);
                            events.ScheduleEvent(EVENT_EARTHGRAB, urand(16 * IN_MILLISECONDS, 36 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct CasterPredicate 
                {
                    public:
                        CasterPredicate(Creature const* me) : _me(me) { }

                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 6.0f;
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jammalan_prophetAI(creature);
        }
};

// Ogom Wretched 5711
class npc_ogom_wretched : public CreatureScript
{
    public:
        npc_ogom_wretched() : CreatureScript("npc_ogom_wretched") { }

        struct npc_ogom_wretchedAI : public ScriptedAI
        {
            npc_ogom_wretchedAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CURSE_WEAKNESS, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SWP, urand(5 * IN_MILLISECONDS, 5.5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOWBOLT, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }

            void HandleCastOnTarget(uint32 m_spellId)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    DoCast(target, m_spellId);
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
                        case EVENT_CURSE_WEAKNESS:
                            HandleCastOnTarget(SPELL_CURSE_OF_WEAKNESS);
                            events.ScheduleEvent(EVENT_CURSE_WEAKNESS, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_SWP:
                            HandleCastOnTarget(SPELL_SHADOW_WORD_PAIN);
                            events.ScheduleEvent(EVENT_SWP, urand(5 * IN_MILLISECONDS, 5.5*IN_MILLISECONDS));
                            break;
                        case EVENT_SHADOWBOLT:
                            HandleCastOnTarget(SPELL_SHADOWBOLT);
                            events.ScheduleEvent(EVENT_SHADOWBOLT, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ogom_wretchedAI(creature);
        }
};

// Hex of Jammalan 12479
class spell_hex_of_jammalan : public SpellScriptLoader
{
    public:
        spell_hex_of_jammalan() : SpellScriptLoader("spell_hex_of_jammalan") { }

        class spell_hex_of_jammalan_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hex_of_jammalan_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(owner, SPELL_HEX_EFFECT, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_hex_of_jammalan_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hex_of_jammalan_AuraScript();
        }
};

// Hex of Jammalan 12480
class spell_hex_of_jammalan_transformed : public SpellScriptLoader
{
    public:
        spell_hex_of_jammalan_transformed() : SpellScriptLoader("spell_hex_of_jammalan_transformed") { }

        class spell_hex_of_jammalan_transformed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hex_of_jammalan_transformed_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* owner = GetOwner()->ToPlayer())
                        owner->SetCharmedBy(caster, CHARM_TYPE_CONVERT);
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* owner = GetOwner()->ToPlayer())
                        owner->RemoveCharmedBy(caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_hex_of_jammalan_transformed_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_hex_of_jammalan_transformed_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hex_of_jammalan_transformed_AuraScript();
        }
};

void AddSC_jammalan_prophet()
{
    new boss_jammalan_prophet();
    new npc_ogom_wretched();
    new spell_hex_of_jammalan();
    new spell_hex_of_jammalan_transformed();
}