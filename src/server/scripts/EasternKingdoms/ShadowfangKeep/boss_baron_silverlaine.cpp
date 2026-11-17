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

#include"ScriptPCH.h"
#include"shadowfang_keep.h"

enum ScriptTexts
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,
    SAY_KILL  = 2,
};

enum Events
{
    EVENT_VEIL_OF_SHADOW    = 1,
    EVENT_CURSED_VEIL_H     = 3,
};

enum Spells
{
    SPELL_VEIL_OF_SHADOW        = 23224,
    SPELL_CURSED_VEIL_H         = 93956,
    SPELL_SUMMON_WORGEN_SPIRIT  = 93857,

    SPELL_NANDOS_T              = 93899,
    SPELL_ODO_T                 = 93864,
    SPELL_RETHILGORE_T          = 93927,
    SPELL_RAZORCLAW_T           = 93924,
};

enum Adds
{
    NPC_RETHILGORE_DUMMY = 50085,
    NPC_RAZORCLAW_DUMMY  = 51080,
    NPC_ODO_DUMMY        = 50934,
    NPC_NANDOS_DUMMY     = 51047,
};

class boss_baron_silverlaine : public CreatureScript
{
    public:
        boss_baron_silverlaine() : CreatureScript("boss_baron_silverlaine") { }

        struct boss_baron_silverlaineAI : public BossAI
        {
            boss_baron_silverlaineAI(Creature* creature) : BossAI(creature, DATA_SILVERLAINE) { }

            uint8 phase;

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

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                phase = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, 12000);
                DoZoneInCombat();
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
                instance->SetData(DATA_SILVERLAINE, DONE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(70) && phase == 0)
                {
                    phase = 1;
                    DoCast(SPELL_SUMMON_WORGEN_SPIRIT);
                    return;
                }
                if (HealthBelowPct(35) && phase == 1)
                {
                    phase = 2;
                    DoCast(SPELL_SUMMON_WORGEN_SPIRIT);
                    return;
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
                    if (eventId == EVENT_VEIL_OF_SHADOW)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_VEIL_OF_SHADOW);

                        events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, 12000);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
         };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_baron_silverlaineAI>(creature);
        }
};

// Silverlaine Worgens 50857, 50851, 50834, 50869
class npc_silverlaine_worgen : public CreatureScript
{
    public:
        npc_silverlaine_worgen() : CreatureScript("npc_silverlaine_worgen") { }

        enum iEvents
        {
            EVENT_SPECIAL_1 = 1,
            EVENT_SPECIAL_2 = 2,
        };

        enum iSpells
        {
            // Odo
            SPELL_HOWLING_RAGE         = 93931,
            SPELL_BLINDING_SHADOWS     = 93952,

            // Nandos
            SPELL_SUMMON_LUPINE_SPIRIT = 94199,
            SPELL_CLAW                 = 93861,

            // Rethilgore
            SPELL_SOUL_DRAIN           = 93863,

            // Razorclaw
            SPELL_RAVAGE               = 93914,
        };

        struct npc_silverlaine_worgenAI : public ScriptedAI
        {
            npc_silverlaine_worgenAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            uint32 spell_type;
            SummonList summons;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* _silverlaine = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SILVERLAINE)))
                        if (Unit* target = _silverlaine->AI()->SelectTarget(SELECT_TARGET_RANDOM))
                            AttackStart(target);

                switch (me->GetEntry())
                {
                    case NPC_ODO_BLINDWATCHER:
                        spell_type = SPELL_BLINDING_SHADOWS;
                        break;
                    case NPC_WOLF_MASTER_NANDOS:
                        spell_type = SPELL_CLAW;
                        break;
                    case NPC_RETHILGORE:
                        spell_type = SPELL_SOUL_DRAIN;
                        break;
                    case NPC_RAZORCLAW_BUTHER:
                        spell_type = SPELL_RAVAGE;
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SPECIAL_1, urand(5 * IN_MILLISECONDS, 8.5 * IN_MILLISECONDS));

                switch (me->GetEntry())
                {
                    case NPC_ODO_BLINDWATCHER:
                        events.ScheduleEvent(EVENT_SPECIAL_2, 12 * IN_MILLISECONDS);
                        break;
                    case NPC_WOLF_MASTER_NANDOS:
                        for (uint8 i = 0; i < 3; ++i)
                            DoCast(me, SPELL_SUMMON_LUPINE_SPIRIT);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                DoZoneInCombat();
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
                        case EVENT_SPECIAL_1:
                            if (spell_type)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                    me->CastSpell(target, spell_type, false);
                                else if (Unit* target = me->GetVictim())
                                    me->CastSpell(target, spell_type, false);
                            }

                            events.ScheduleEvent(EVENT_SPECIAL_1, urand(9 * IN_MILLISECONDS, 14.8 * IN_MILLISECONDS));
                            break;
                        case EVENT_SPECIAL_2:
                            DoCast(me, SPELL_HOWLING_RAGE);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_silverlaine_worgenAI>(creature);
        }
};

class npc_silverlaine_worgen_spirit : public CreatureScript
{
    public:
        npc_silverlaine_worgen_spirit() : CreatureScript("npc_silverlaine_worgen_spirit") { }

        struct npc_silverlaine_worgen_spiritAI : public ScriptedAI
        {
            npc_silverlaine_worgen_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_spell;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_ODO_DUMMY:
                        m_spell = SPELL_ODO_T;
                        break;
                    case NPC_RETHILGORE_DUMMY:
                        m_spell = SPELL_RETHILGORE_T;
                        break;
                    case NPC_NANDOS_DUMMY:
                        m_spell = SPELL_NANDOS_T;
                        break;
                    case NPC_RAZORCLAW_DUMMY:
                        m_spell = SPELL_RAZORCLAW_T;
                        break;
                }

                if (instance && m_spell)
                    if (Creature* m_silverlaine = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SILVERLAINE)))
                        m_silverlaine->CastSpell(me, m_spell, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_silverlaine_worgen_spiritAI>(creature);
        }
};

class spell_silverlaine_summon_worgen_spirit : public SpellScriptLoader
{
    public:
        spell_silverlaine_summon_worgen_spirit() : SpellScriptLoader("spell_silverlaine_summon_worgen_spirit") { }

        class spell_silverlaine_summon_worgen_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_silverlaine_summon_worgen_spirit_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (!GetCaster())
                    return;

                switch (urand(0, 3))
                {
                    case 0:
                        GetCaster()->CastSpell(GetCaster(), 93925, true);
                        break;
                    case 1:
                        GetCaster()->CastSpell(GetCaster(), 93921, true);
                        break;
                    case 2:
                        GetCaster()->CastSpell(GetCaster(), 93859, true);
                        break;
                    case 3:
                        GetCaster()->CastSpell(GetCaster(), 93896, true);
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_silverlaine_summon_worgen_spirit_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_silverlaine_summon_worgen_spirit_SpellScript();
        }
};

void AddSC_boss_baron_silverlaine()
{
    new boss_baron_silverlaine();
    new npc_silverlaine_worgen();
    new npc_silverlaine_worgen_spirit();
    new spell_silverlaine_summon_worgen_spirit();
}
