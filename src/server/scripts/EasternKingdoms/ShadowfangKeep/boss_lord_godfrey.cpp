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
#include"Spell.h"
#include"shadowfang_keep.h"

enum ScriptTexts
{
    SAY_AGGRO_ALLIANCE = 0,
    SAY_AGGRO_HORDE    = 1,
    SAY_DEATH          = 2,
    SAY_KILL           = 3,
};

enum Spells
{
    SPELL_PISTOL_BARRAGE                    = 93520,
    SPELL_PISTOL_BARRAGE_VISUAL             = 96345,
    SPELL_PISTOL_BARRAGE_PERIODIC_DMG_AURA  = 93566,
    SPELL_PISTOL_BARRAGE_SUMMON_FORCECAST   = 96344,
    SPELL_PISTOL_BARRAGE_BULLET_VIS_LEFT    = 93557,
    SPELL_PISTOL_BARRAGE_BULLET_VIS_RIGHT   = 93558,
    SPELL_PISTOLS_VISUAL_PASSIVE            = 93597,
    SPELL_SUMMON_BLOODTHIRSTY_GHOULS        = 93707,
    SPELL_SUMMON_BLOODTHIRSTY_GHOULS_M      = 93709,
    SPELL_CURSED_BULLET                     = 93629,
    SPELL_CURSED_BULLET_H                   = 93629,
    SPELL_MORTAL_WOUND                      = 93675,
    SPELL_MORTAL_WOUND_H                    = 93675,
    SPELL_ACHIEVEMENT                       = 95929,
    SPELL_CLEAR_ACHIEVEMENT                 = 95930,
};

enum Events
{
     EVENT_PISTOL_BARRAGE                = 1,
     EVENT_SUMMON_BLOODTHIRSTY_GHOULS    = 2,
     EVENT_CURSED_BULLET                 = 3,
     EVENT_MORTAL_WOUND                  = 4,
};

enum Actions
{
    ACTION_REMOVE_BARRAGE = 0,
};

enum Adds
{
     NPC_BLOODTHRISTY_GHOUL     = 50561,
     NPC_PISTOL_BARRAGE         = 52065,
};

class boss_lord_godfrey : public CreatureScript
{
    public:
        boss_lord_godfrey() : CreatureScript("boss_lord_godfrey") { }

        struct boss_lord_godfreyAI : public BossAI
        {
            boss_lord_godfreyAI(Creature* creature) : BossAI(creature, DATA_GODFREY) { }

            ObjectGuid targetGUID;

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

                targetGUID = ObjectGuid::Empty;
                instance->DoResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEVEMENT_CRITERIA_CONDITION_NO_SPELL_HIT, SPELL_CLEAR_ACHIEVEMENT);
                instance->DoResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEVEMENT_CRITERIA_CONDITION_NO_SPELL_HIT, SPELL_CLEAR_ACHIEVEMENT);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_MORTAL_WOUND, 10000);
                events.ScheduleEvent(EVENT_CURSED_BULLET, 15000);
                events.ScheduleEvent(EVENT_SUMMON_BLOODTHIRSTY_GHOULS, 17000);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_PISTOL_BARRAGE, 22000);

                if (instance)
                    instance->GetData(DATA_TEAM) == ALLIANCE ? Talk(SAY_AGGRO_ALLIANCE) : Talk(SAY_AGGRO_HORDE);
                DoZoneInCombat();
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        if ((me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_CURSED_BULLET) ||
                            (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_CURSED_BULLET_H))
                            me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_REMOVE_BARRAGE)
                    me->RemoveChanneledCast(targetGUID);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_UNIT && victim->GetEntry() == NPC_BLOODTHRISTY_GHOUL)
                    DoCastAOE(SPELL_ACHIEVEMENT, true);
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
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
                        case EVENT_MORTAL_WOUND:
                            DoCast(me->GetVictim(), SPELL_MORTAL_WOUND);
                            events.ScheduleEvent(EVENT_MORTAL_WOUND, 10000);
                            break;
                        case EVENT_CURSED_BULLET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_CURSED_BULLET);
                            events.ScheduleEvent(EVENT_CURSED_BULLET, 15000);
                            break;
                        case EVENT_SUMMON_BLOODTHIRSTY_GHOULS:
                            DoCast(SPELL_SUMMON_BLOODTHIRSTY_GHOULS);
                            events.ScheduleEvent(EVENT_SUMMON_BLOODTHIRSTY_GHOULS, 17000);
                            break;
                        case EVENT_PISTOL_BARRAGE:
                            DoCastAOE(SPELL_PISTOL_BARRAGE_SUMMON_FORCECAST, true);
                            if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();

                                if (Creature* pistol = me->FindNearestCreature(NPC_PISTOL_BARRAGE, 60.0f))
                                    me->PrepareChanneledCast(me->GetAngle(pistol), SPELL_PISTOL_BARRAGE);
                            }
                            events.ScheduleEvent(EVENT_PISTOL_BARRAGE, 17000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_lord_godfreyAI>(creature);
        }
};

class spell_godfrey_summon_bloodthirsty_ghouls : public SpellScriptLoader
{
    public:
        spell_godfrey_summon_bloodthirsty_ghouls() : SpellScriptLoader("spell_godfrey_summon_bloodthirsty_ghouls") { }

        class spell_godfrey_summon_bloodthirsty_ghouls_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_godfrey_summon_bloodthirsty_ghouls_AuraScript);

            void HandleDummyTick(AuraEffect const* /*aurEff*/)
            {
                GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_BLOODTHIRSTY_GHOULS_M, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_godfrey_summon_bloodthirsty_ghouls_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_godfrey_summon_bloodthirsty_ghouls_AuraScript();
        }
};

class spell_godfrey_pistol_barrage : public SpellScriptLoader
{
    public:
        spell_godfrey_pistol_barrage() : SpellScriptLoader("spell_godfrey_pistol_barrage") { }

        class spell_godfrey_pistol_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_godfrey_pistol_barrage_AuraScript);

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Creature* target = caster->FindNearestCreature(NPC_PISTOL_BARRAGE, 60))
                    {
                        float x, y, z;
                        caster->GetPosition(x, y, z);
                        float angle = caster->GetAngle(target) + ((aurEff->GetTickNumber() % 2) ? 1 : -1) * (float)rand_norm() * M_PI / 4;
                        x += 60 * cos(angle);
                        y += 60 * sin(angle);
                        z += 1;
                        caster->CastSpell(x, y, z, (aurEff->GetTickNumber() % 2) ? SPELL_PISTOL_BARRAGE_BULLET_VIS_LEFT : SPELL_PISTOL_BARRAGE_BULLET_VIS_RIGHT, true);
                    }
                }
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Creature* target = caster->FindNearestCreature(NPC_PISTOL_BARRAGE, 60))
                    {
                        caster->SetFacingToObject(target);
                        caster->CastSpell(caster, SPELL_PISTOLS_VISUAL_PASSIVE, true);
                        caster->CastSpell(caster, SPELL_PISTOL_BARRAGE_VISUAL, true);
                        caster->CastSpell(caster, SPELL_PISTOL_BARRAGE_PERIODIC_DMG_AURA, true);
                    }
                }
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->RemoveAurasDueToSpell(SPELL_PISTOL_BARRAGE_PERIODIC_DMG_AURA);
                    caster->RemoveAurasDueToSpell(SPELL_PISTOLS_VISUAL_PASSIVE);

                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->DoAction(ACTION_REMOVE_BARRAGE);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_godfrey_pistol_barrage_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectApply += AuraEffectApplyFn(spell_godfrey_pistol_barrage_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_godfrey_pistol_barrage_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_godfrey_pistol_barrage_AuraScript();
        }
};

void AddSC_boss_lord_godfrey()
{
    new boss_lord_godfrey();
    new spell_godfrey_summon_bloodthirsty_ghouls();
    new spell_godfrey_pistol_barrage();
}
