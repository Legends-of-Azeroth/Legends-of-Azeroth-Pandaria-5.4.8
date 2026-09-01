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
#include "Spell.h"
#include "zulgurub.h"

enum ScriptTexts
{
    SAY_AGGRO       = 0,
    SAY_DEATH       = 1,
    SAY_KILL_FERAL  = 2,
    SAY_KILL        = 3,
    SAY_SPELL       = 4,
    SAY_TRANSFORM_1 = 5,
    SAY_TRANSFORM_2 = 6,
};

enum Spells
{
    SPELL_SHADOW_BOLT           = 96516,
    SPELL_WAVE_OF_AGONY_AOE     = 98269, // select target
    SPELL_WAVE_OF_AGONY_END     = 96461, // summon stalker
    SPELL_WAVE_OF_AGONY_START   = 96457, // summon wave stalker
    SPELL_WAVE_OF_AGONY_DMG     = 96460,
    SPELL_WAVE_OF_AGONY_VISUAL  = 96532,
    SPELL_WAVE_OF_AGONY_TRAJ    = 96542,
    SPELL_TEARS_OF_BLOOD        = 96438,
    SPELL_TEARS_OF_BLOOD_AURA   = 96935,
    SPELL_WAIL_OF_SORROW        = 96909,
    SPELL_LASH_OF_ANGUISH       = 96423,
    SPELL_RAGE_OF_THE_ANCIENTS  = 96896,
    SPELL_VENGEFUL_SMASH        = 96593,
    SPELL_CAMOUFLAGE            = 96594,
    SPELL_RAVAGE                = 96592,
    SPELL_PRIMAL_BLESSING       = 96559,
    SPELL_CAVE_IN               = 97380,
    SPELL_CAVE_IN_DUMMY         = 96935,
    SPELL_GAPING_WOUND          = 97355,
    SPELL_DARK_SLUMBER          = 96446,

    // achievement
    SPELL_CLEAR_ACHIEVEMENT     = 98840,
    SPELL_CAT_FED               = 98258,
    SPELL_BLOOD_FRENZY          = 98239,
    SPELL_CREATE_RAT_COVER      = 98177,
    SPELL_CREATE_RAT            = 98178,
    SPELL_THROW_RAT             = 98181,
    SPELL_HAS_RAT               = 98208,
    SPELL_RAT_LANDS             = 98216,
    SPELL_RAT_LURE              = 98238,
    SPELL_POUNCE_RAT            = 98241,
    SPELL_RAT_MUNCH             = 98242,
};

enum Events
{
    EVENT_SHADOW_BOLT       = 1,
    EVENT_WAVE_OF_AGONY     = 2,
    EVENT_TEARS_OF_BLOOD    = 3,
    EVENT_WAIL_OF_SORROW    = 4,
    EVENT_LASH_OF_ANGUISH   = 5,
    EVENT_CAMOUFLAGE        = 6,
    EVENT_RAVAGE            = 7,
    EVENT_GAPING_WOUND      = 8,
    EVENT_CONTINUE          = 9,
    EVENT_VENGEFUL_SMASH    = 10,
    EVENT_PRIMAL_BLESSING   = 11,
};

enum Adds
{
    NPC_PRIDE_OF_BETHEKK    = 52061,
    NPC_WAVE_OF_AGONY       = 52147,
    NPC_WAVE_OF_AGONY_END   = 52160,
    NPC_CAVE_IN_STALKER     = 52387,
    NPC_TEMPLE_RAT          = 53108,
};

enum
{
    ACTION_CAT_FED                  = 1,

    SPELL_CLEAR_ACHIEVEMENT_CREDIT  = 98840,
    WORLD_STATE_HERE_KITTY_KITTY    = 3410,
};

const Position pridePos[16] = 
{
    {-11517.2f, -1646.82f, 44.4849f, 3.87463f},
    {-11519.2f, -1605.37f, 44.4849f, 3.56047f},
    {-11507.0f, -1644.55f, 44.4849f, 4.7822f},
    {-11519.7f, -1609.0f, 44.4849f, 2.26893f},
    {-11505.6f, -1607.56f, 44.4849f, 2.26893f},
    {-11504.6f, -1603.33f, 44.4849f, 3.87463f},
    {-11508.3f, -1607.37f, 44.4849f, 0.977384f},
    {-11508.7f, -1603.38f, 44.4849f, 5.41052f},
    {-11504.3f, -1645.56f, 44.4849f, 4.66003f},
    {-11523.2f, -1605.96f, 44.4849f, 5.41052f},
    {-11504.1f, -1650.26f, 44.4849f, 2.26893f},
    {-11506.6f, -1651.04f, 44.4849f, 1.6057f},
    {-11523.2f, -1609.31f, 44.4849f, 0.977384f},
    {-11521.8f, -1651.58f, 44.4849f, 0.977384f},
    {-11520.6f, -1646.0f, 44.4849f, 4.95674f},
    {-11518.1f, -1651.48f, 44.4849f, 2.26893f}
};

const Position cavePos[4] = 
{
    {-11506.63f, -1605.05f, 44.41f, 0.0f},
    {-11506.26f, -1643.40f, 44.41f, 0.0f},
    {-11520.27f, -1644.65f, 44.41f, 0.0f},
    {-11522.52f, -1610.81f, 44.41f, 0.0f}
};

class boss_kilnara : public CreatureScript
{
    public:
        boss_kilnara() : CreatureScript("boss_kilnara") { }

        struct boss_kilnaraAI : public BossAI
        {
            boss_kilnaraAI(Creature* creature) : BossAI(creature, DATA_KILNARA)
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
            }

            bool bTwoPhase;
            uint8 rats;

            void Reset() override
            {
                _Reset();

                for (uint8 i = 0; i < 16; ++i)
                    if (Creature* pPride = me->SummonCreature(NPC_PRIDE_OF_BETHEKK, pridePos[i]))
                    {
                        pPride->SetReactState(REACT_DEFENSIVE);
                        pPride->CastSpell(pPride, SPELL_DARK_SLUMBER, true);
                    }

                bTwoPhase = false;
                me->SetReactState(REACT_AGGRESSIVE);

                ResetAchievement();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                bTwoPhase = false;

                events.ScheduleEvent(EVENT_SHADOW_BOLT, 1000);
                events.ScheduleEvent(EVENT_WAVE_OF_AGONY, urand(18000, 25000));
                events.ScheduleEvent(EVENT_LASH_OF_ANGUISH, 10000);
                events.ScheduleEvent(EVENT_WAIL_OF_SORROW, urand(15000, 20000));
                events.ScheduleEvent(EVENT_TEARS_OF_BLOOD, urand(12000, 15000));
                instance->DoResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEVEMENT_CRITERIA_CONDITION_NO_SPELL_HIT, SPELL_CLEAR_ACHIEVEMENT);
                instance->DoResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEVEMENT_CRITERIA_CONDITION_NO_SPELL_HIT, SPELL_CLEAR_ACHIEVEMENT);
                instance->SetBossState(DATA_KILNARA, IN_PROGRESS);

                // Here, because playres can avoid spell due to dying/teleporting etc
                ResetAchievement();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                BossAI::JustSummoned(summon);
                if (summon->GetEntry() == NPC_WAVE_OF_AGONY_END)
                {
                    Talk(SAY_SPELL);
                    DoCast(summon, SPELL_WAVE_OF_AGONY_START, true);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetGUID() == attacker->GetGUID())
                    damage = 0;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(bTwoPhase? SAY_KILL_FERAL: SAY_KILL);
            }

            void SpellHit(Unit* caster, const SpellInfo* spellInfo) override
            {
                if (spellInfo->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
                {
                    if (Spell const* spell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        if (spell->m_spellInfo->Id == SPELL_SHADOW_BOLT)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    me->RemoveAurasDueToSpell(SPELL_TEARS_OF_BLOOD_AURA);
                    me->RemoveAurasDueToSpell(SPELL_TEARS_OF_BLOOD);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HealthBelowPct(50) && !bTwoPhase)
                {
                    bTwoPhase = true;
                    me->InterruptNonMeleeSpells(false);
                    events.Reset();
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    Talk(SAY_TRANSFORM_1);
                    DoCast(me, SPELL_CAVE_IN);
                    for (uint8 i = 0; i < 4; ++i)
                        if (Creature* pCave = me->SummonCreature(NPC_CAVE_IN_STALKER, cavePos[i], TEMPSUMMON_TIMED_DESPAWN, 5000ms))
                            pCave->CastSpell(pCave, SPELL_CAVE_IN_DUMMY);
                    summons.DoZoneInCombat(NPC_PRIDE_OF_BETHEKK);
                    events.ScheduleEvent(EVENT_PRIMAL_BLESSING, 6000);
                    events.ScheduleEvent(EVENT_CONTINUE, 7000);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_BOLT:
                            DoCastVictim(SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(7000, 8000));
                            break;
                        case EVENT_WAIL_OF_SORROW:
                            DoCast(me, SPELL_WAIL_OF_SORROW);
                            events.ScheduleEvent(EVENT_WAIL_OF_SORROW, urand(15000, 20000));
                            break;
                        case EVENT_LASH_OF_ANGUISH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_LASH_OF_ANGUISH);
                            events.ScheduleEvent(EVENT_LASH_OF_ANGUISH, urand(15000, 20000));
                            break;
                        case EVENT_TEARS_OF_BLOOD:
                            DoCast(me, SPELL_TEARS_OF_BLOOD);
                            events.ScheduleEvent(EVENT_TEARS_OF_BLOOD, urand(30000, 35000));
                            break;
                        case EVENT_WAVE_OF_AGONY:
                            DoCast(me, SPELL_WAVE_OF_AGONY_AOE);
                            events.ScheduleEvent(EVENT_WAVE_OF_AGONY, urand(30000, 35000));
                            break;
                        case EVENT_PRIMAL_BLESSING:
                            Talk(SAY_TRANSFORM_2);
                            DoCast(me, SPELL_PRIMAL_BLESSING, true);
                            DoCast(me, SPELL_RAGE_OF_THE_ANCIENTS, true);
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            events.ScheduleEvent(EVENT_VENGEFUL_SMASH, urand(4000, 10000));
                            events.ScheduleEvent(EVENT_RAVAGE, urand(2000, 6000));
                            break;
                        case EVENT_VENGEFUL_SMASH:
                            DoCast(me, SPELL_VENGEFUL_SMASH);
                            events.ScheduleEvent(EVENT_VENGEFUL_SMASH, urand(10000, 15000));
                            break;
                        case EVENT_RAVAGE:
                            DoCastVictim(SPELL_RAVAGE);
                            events.ScheduleEvent(EVENT_RAVAGE, urand(9000, 12000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_CAT_FED)
                {
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CAT_FED);

                    ++rats;
                    if (rats == 4)
                        me->GetMap()->SetWorldState(WORLD_STATE_HERE_KITTY_KITTY, 1);
                }
            }

            void ResetAchievement()
            {
                rats = 0;
                me->CastSpell(me, SPELL_CLEAR_ACHIEVEMENT_CREDIT, true);
                me->GetMap()->SetWorldState(WORLD_STATE_HERE_KITTY_KITTY, 0);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_kilnaraAI>(creature);
        }
};

class npc_kilnara_pride_of_bethekk : public CreatureScript
{
    public:
        npc_kilnara_pride_of_bethekk() : CreatureScript("npc_kilnara_pride_of_bethekk") { }

        struct npc_kilnara_pride_of_bethekkAI : public ScriptedAI
        {
            npc_kilnara_pride_of_bethekkAI(Creature* creature) : ScriptedAI(creature)  { }

            uint32 uiGapingWound;

            void Reset() override
            {
                uiGapingWound = urand(5000, 15000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiGapingWound <= diff)
                {
                    uiGapingWound = urand(5000, 15000);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        DoCast(target, SPELL_GAPING_WOUND);
                }
                else
                    uiGapingWound -= diff;

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_kilnara_pride_of_bethekkAI>(creature);
        }
};

class npc_kilnara_temple_rat : public CreatureScript
{
    public:
        npc_kilnara_temple_rat() : CreatureScript("npc_kilnara_temple_rat") { }

        struct npc_kilnara_temple_ratAI : public ScriptedAI
        {
            npc_kilnara_temple_ratAI(Creature* creature) : ScriptedAI(creature)  { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (me->GetEntry() == NPC_TEMPLE_RAT)
                    DoCast(me, SPELL_RAT_LURE, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_kilnara_temple_ratAI>(creature);
        }
};

class spell_kilnara_wave_of_agony_target : public SpellScriptLoader
{
    public:
        spell_kilnara_wave_of_agony_target() : SpellScriptLoader("spell_kilnara_wave_of_agony_target") { }

        class spell_kilnara_wave_of_agony_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kilnara_wave_of_agony_target_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            { 
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_WAVE_OF_AGONY_END, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_kilnara_wave_of_agony_target_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kilnara_wave_of_agony_target_SpellScript();
        }
};

class spell_kilnara_wave_of_agony_start : public SpellScriptLoader
{
    public:
        spell_kilnara_wave_of_agony_start() : SpellScriptLoader("spell_kilnara_wave_of_agony_start") { }

        class spell_kilnara_wave_of_agony_start_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kilnara_wave_of_agony_start_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetUnitOwner())
                    return;

                if (pStart = GetUnitOwner()->FindNearestCreature(NPC_WAVE_OF_AGONY, 300.0f))
                {
                    pStart->CastSpell(GetUnitOwner(), SPELL_WAVE_OF_AGONY_TRAJ, true);
                    pStart->DespawnOrUnsummon(500);
                }
                if (GetUnitOwner()->ToCreature())
                    GetUnitOwner()->ToCreature()->DespawnOrUnsummon(500);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kilnara_wave_of_agony_start_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            Creature* pStart;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kilnara_wave_of_agony_start_AuraScript();
        }
};

// 98178 - Create Rat
class spell_create_rat : public SpellScript
{
    PrepareSpellScript(spell_create_rat);

    SpellCastResult CheckCast()
    {
        if (GetExplTargetUnit())
            if (Player* player = GetExplTargetUnit()->ToPlayer())
                if (player->HasItemCount(GetSpellInfo()->Effects[EFFECT_0].ItemType, 1))
                    return SPELL_FAILED_DONT_REPORT;
        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        if (Creature* creature = GetCaster()->ToCreature())
            creature->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_create_rat::CheckCast);
        OnCast += SpellCastFn(spell_create_rat::HandleCast);
    }
};

// 98238 - Rat Lure
class spell_kilnara_rat_lure : public SpellScript
{
    PrepareSpellScript(spell_kilnara_rat_lure);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target)
        {
            Creature const* creature = target->ToCreature();
            if (!creature)
                return true;
            return creature->GetEntry() != NPC_PRIDE_OF_BETHEKK || !creature->IsAlive() || creature->HasAura(SPELL_DARK_SLUMBER) || creature->HasAura(SPELL_BLOOD_FRENZY);
        });

        if (targets.size() > 1)
            Trinity::RandomResizeList(targets, 1);
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        GetHitUnit()->CastSpell(GetCaster(), SPELL_POUNCE_RAT, true);

        ObjectGuid ratGuid = GetCaster()->GetGUID();
        GetHitUnit()->Schedule(Milliseconds(GetHitUnit()->GetSplineDuration()), [ratGuid](Unit* self)
        {
            self->CastSpell(self, SPELL_BLOOD_FRENZY, true);
            if (InstanceScript* instance = self->GetInstanceScript())
                if (Creature* kilnara = ObjectAccessor::GetCreature(*self, instance->GetGuidData(DATA_KILNARA)))
                    kilnara->AI()->DoAction(ACTION_CAT_FED);

            if (Creature* rat = ObjectAccessor::GetCreature(*self, ratGuid))
                rat->DespawnOrUnsummon();
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kilnara_rat_lure::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_kilnara_rat_lure::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_boss_kilnara()
{
    new boss_kilnara();
    new npc_kilnara_pride_of_bethekk();
    new npc_kilnara_temple_rat();
    new spell_kilnara_wave_of_agony_target();
    new spell_kilnara_wave_of_agony_start();
    new spell_script<spell_create_rat>("spell_create_rat");
    new spell_script<spell_kilnara_rat_lure>("spell_kilnara_rat_lure");
}
