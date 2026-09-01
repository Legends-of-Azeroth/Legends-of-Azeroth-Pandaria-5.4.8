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

#include "azjol_nerub.h"
#include "PassiveAI.h"

enum Spells
{
    SPELL_CARRION_BEETLES                         = 53520,
    SPELL_SUMMON_CARRION_BEETLES                  = 53521,
    SPELL_LEECHING_SWARM                          = 53467,
    SPELL_POUND                                   = 53472,
    SPELL_POUND_H                                 = 59433,
    SPELL_POUND_DAMAGE                            = 53509,
    SPELL_POUND_DAMAGE_H                          = 59432,
    SPELL_SUBMERGE                                = 53421,
    SPELL_EMERGE                                  = 53500,
    SPELL_IMPALE_DMG                              = 53454,
    SPELL_IMPALE_DMG_H                            = 59446,
    SPELL_IMPALE_VISUAL                           = 53455,
    SPELL_IMPALE_AURA                             = 53456,
    SPELL_IMPALE_TRIGGER                          = 53457,
    SPELL_IMPALE_SUMMON                           = 53458,
};

enum Creatures
{
    NPC_GUARDIAN                                  = 29216,
    NPC_VENOMANCER                                = 29217,
    NPC_DATTER                                    = 29213,
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_SLAY                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_LOCUST                                    = 3,
    SAY_SUBMERGE                                  = 4,
};

enum Achievement
{
    ACHIEV_TIMED_START_EVENT                      = 20381,
};

enum Phases
{
    PHASE_MELEE                                   = 0,
    PHASE_UNDERGROUND                             = 1,
};

const Position SpawnPoint[2] =
{
    { 550.7f, 282.8f, 224.3f, 0.0f },
    { 551.1f, 229.4f, 224.3f, 0.0f },
};

const Position SpawnPointGuardian[2] =
{
    { 550.348633f, 316.006805f, 234.2947f, 0.0f },
    { 550.188660f, 324.264557f, 237.7412f, 0.0f },
};

class boss_anub_arak : public CreatureScript
{
    public:
        boss_anub_arak() : CreatureScript("boss_anub_arak") { }

        struct boss_anub_arakAI : public ScriptedAI
        {
            boss_anub_arakAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            bool bGuardianSummoned;
            bool bVenomancerSummoned;
            bool bDatterSummoned;
            uint8 uiPhase;
            uint32 uiUndergroundPhase;
            uint32 uiCarrionBeetlesTimer;
            uint32 uiLeechingSwarmTimer;
            uint32 uiPoundTimer;
            uint32 uiSubmergeTimer;
            uint32 uiUndergroundTimer;
            uint32 uiVenomancerTimer;
            uint32 uiDatterTimer;
            SummonList lSummons;

            void Reset() override
            {
                uiCarrionBeetlesTimer = 8 * IN_MILLISECONDS;
                uiLeechingSwarmTimer = 20 * IN_MILLISECONDS;
                uiPoundTimer = 15 * IN_MILLISECONDS;

                uiPhase = PHASE_MELEE;
                uiUndergroundPhase = 0;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveAura(SPELL_SUBMERGE);

                lSummons.DespawnAll();

                if (instance)
                {
                    instance->SetData(DATA_ANUBARAK_EVENT, NOT_STARTED);
                    instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                if (instance)
                {
                    instance->SetData(DATA_ANUBARAK_EVENT, IN_PROGRESS);
                    instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_POUND:
                        DoCast(target, SPELL_POUND_DAMAGE, true);
                        break;
                    case SPELL_POUND_H:
                        DoCast(target, SPELL_POUND_DAMAGE_H, true);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetExactDist2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) >= 30.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (uiPhase)
                {
                    case PHASE_UNDERGROUND:
                        if (!bGuardianSummoned)
                        {
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                if (Creature* Guardian = me->SummonCreature(NPC_GUARDIAN,SpawnPointGuardian[i],TEMPSUMMON_CORPSE_DESPAWN,0ms))
                                {
                                    Guardian->AddThreat(me->GetVictim(), 0.0f);
                                    DoZoneInCombat(Guardian);
                                }
                            }
                            bGuardianSummoned = true;
                        }

                        if (!bVenomancerSummoned)
                        {
                            if (uiVenomancerTimer <= diff)
                            {
                                if (uiUndergroundPhase > 1)
                                {
                                    for (uint8 i = 0; i < 2; ++i)
                                    {
                                        if (Creature* Venomancer = me->SummonCreature(NPC_VENOMANCER,SpawnPoint[i],TEMPSUMMON_CORPSE_DESPAWN,0ms))
                                        {
                                            Venomancer->AddThreat(me->GetVictim(), 0.0f);
                                            DoZoneInCombat(Venomancer);
                                        }
                                    }
                                    bVenomancerSummoned = true;
                                }
                            } else uiVenomancerTimer -= diff;
                        }

                        if (!bDatterSummoned)
                        {
                            if (uiDatterTimer <= diff)
                            {
                                if (uiUndergroundPhase > 2)
                                {
                                    for (uint8 i = 0; i < 2; ++i)
                                    {
                                        if (Creature* Datter = me->SummonCreature(NPC_DATTER,SpawnPoint[i],TEMPSUMMON_CORPSE_DESPAWN,0ms))
                                        {
                                            Datter->AddThreat(me->GetVictim(), 0.0f);
                                            DoZoneInCombat(Datter);
                                        }
                                    }
                                    bDatterSummoned = true;
                                }
                            } else uiDatterTimer -= diff;
                        }

                        if (uiUndergroundTimer <= diff)
                        {
                            me->RemoveAurasDueToSpell(SPELL_IMPALE_AURA);
                            me->RemoveAura(SPELL_SUBMERGE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                            DoCast(me, SPELL_EMERGE, false);
                            uiPhase = PHASE_MELEE;

                            // Delay spells so Emerge cast would finish correctly
                            uiCarrionBeetlesTimer += 1250;
                            uiLeechingSwarmTimer += 1250;
                            uiPoundTimer += 1250;
                        } else uiUndergroundTimer -= diff;
                        break;

                    case PHASE_MELEE:
                        if (((uiUndergroundPhase == 0 && HealthBelowPct(75))
                            || (uiUndergroundPhase == 1 && HealthBelowPct(50))
                            || (uiUndergroundPhase == 2 && HealthBelowPct(25)))
                            && !me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            bGuardianSummoned = false;
                            bVenomancerSummoned = false;
                            bDatterSummoned = false;

                            uiUndergroundTimer = 39 * IN_MILLISECONDS; // Should change depending on when the adds are killed
                            uiVenomancerTimer = 25 * IN_MILLISECONDS;
                            uiDatterTimer = 32 * IN_MILLISECONDS;

                            DoCast(me, SPELL_SUBMERGE, false);
                            DoCast(me, SPELL_IMPALE_AURA, true);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);

                            uiPhase = PHASE_UNDERGROUND;
                            ++uiUndergroundPhase;
                            break; // Don't execute other spells, they might interrupt Submerge
                        }

                        if (uiCarrionBeetlesTimer <= diff)
                        {
                            if (!me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                DoCast(me, SPELL_CARRION_BEETLES);
                                uiCarrionBeetlesTimer = 25 * IN_MILLISECONDS;
                            }
                        } else uiCarrionBeetlesTimer -= diff;

                        if (uiLeechingSwarmTimer <= diff)
                        {
                            DoCast(me, SPELL_LEECHING_SWARM, true);
                            uiLeechingSwarmTimer = 19 * IN_MILLISECONDS;
                        } else uiLeechingSwarmTimer -= diff;

                        if (uiPoundTimer <= diff)
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                                break;

                            DoCastVictim(DUNGEON_MODE(SPELL_POUND, SPELL_POUND_H));
                            uiPoundTimer = 16500;
                        } else uiPoundTimer -= diff;

                        DoMeleeAttackIfReady();
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                lSummons.DespawnAll();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (instance)
                    instance->SetData(DATA_ANUBARAK_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                if (me->IsInCombat())
                    lSummons.Summon(summon);
                else
                    summon->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_anub_arakAI(creature);
        }
};

class npc_impale_target : public CreatureScript
{
    public:
        npc_impale_target() : CreatureScript("npc_impale_target") { }

        struct npc_impale_targetAI : TriggerAI
        {
            npc_impale_targetAI(Creature* creature) : TriggerAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* anubarak = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ANUBARAK)))
                        anubarak->AI()->JustSummoned(me);

                DoCast(me, SPELL_IMPALE_VISUAL, true);
                me->m_Events.Schedule(3000, [this]()
                {
                    me->RemoveAurasDueToSpell(SPELL_IMPALE_VISUAL);
                    DoCast(me, me->GetMap()->IsHeroic() ? SPELL_IMPALE_DMG_H : SPELL_IMPALE_DMG, true);
                });
                me->DespawnOrUnsummon(10000);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_impale_targetAI(creature);
        }
};

class spell_carrion_beetles : public SpellScriptLoader
{
    public:
        spell_carrion_beetles() : SpellScriptLoader("spell_carrion_beetles") { }

        class spell_carrion_beetles_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_carrion_beetles_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
                    for (uint8 i = 0; i < 2; ++i)
                        target->CastSpell(target, SPELL_SUMMON_CARRION_BEETLES, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_carrion_beetles_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_carrion_beetles_AuraScript();
        }
};

void AddSC_boss_anub_arak()
{
    new boss_anub_arak;
    new npc_impale_target();
    new spell_carrion_beetles();
}
