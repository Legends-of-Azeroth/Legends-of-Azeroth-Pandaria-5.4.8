/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

/*
 * Scripts for spells with SPELLFAMILY_ROGUE and SPELLFAMILY_GENERIC spells used by rogue players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_rog_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "spell_common.h"
#include "Random.h"

enum RogueSpells
{
    ROGUE_SPELL_SHIV_TRIGGERED                   = 5940,
    SPELL_ROGUE_DEADLY_POISON                    = 2823,
    SPELL_ROGUE_WOUND_POISON                     = 8679,
    ROGUE_SPELL_MIND_NUMBLING_POISON             = 5761,
    ROGUE_SPELL_CRIPPLING_POISON                 = 3408,
    ROGUE_SPELL_CRIPPLING_POISON_DEBUFF          = 3409,
    ROGUE_SPELL_LEECHING_POISON                  = 108211,
    ROGUE_SPELL_LEECHING_POISON_DEBUFF           = 112961,
    ROGUE_SPELL_PARALYTIC_POISON                 = 108215,
    SPELL_ROGUE_PARALYTIC_POISON_DEBUFF          = 113952,
    ROGUE_SPELL_DEBILITATING_POISON              = 115196,
    ROGUE_SPELL_MIND_PARALYSIS                   = 115194,
    ROGUE_SPELL_LEECH_VITALITY                   = 116921,
    ROGUE_SPELL_PARTIAL_PARALYSIS                = 115197,
    SPELL_ROGUE_TOTAL_PARALYSIS                  = 113953,
    ROGUE_SPELL_DEADLY_POISON_DOT                = 2818,
    ROGUE_SPELL_DEADLY_POISON_INSTANT_DAMAGE     = 113780,
    SPELL_ROGUE_SLICE_AND_DICE                   = 5171,
    ROGUE_SPELL_MASTER_POISONER_AURA             = 58410,
    ROGUE_SPELL_MASTER_POISONER_DEBUFF           = 93068,
    ROGUE_SPELL_CRIMSON_TEMPEST_DOT              = 122233,
    ROGUE_SPELL_SHROUD_OF_CONCEALMENT_AURA       = 115834,
    SPELL_ROGUE_VENOMOUS_VIM                     = 51637,
    SPELL_ROGUE_VENOMOUS_WOUND                   = 79136,
    SPELL_ROGUE_VENOMOUS_WOUNDS                  = 79134,
    SPELL_ROG_GARROTE                            = 703,
    SPELL_ROG_RUPTURE                            = 1943,
    ROGUE_SPELL_CUT_TO_THE_CHASE_AURA            = 51667,
    ROGUE_SPELL_ADRENALINE_RUSH                  = 13750,
    SPELL_ROGUE_KILLING_SPREE                    = 51690,
    SPELL_ROGUE_KILLING_SPREE_DAMAGE_AURA        = 61851,
    SPELL_ROGUE_GLYPH_OF_KILLING_SPREE           = 63252,
    ROGUE_SPELL_REDIRECT                         = 73981,
    ROGUE_SPELL_SPRINT                           = 2983,
    ROGUE_SPELL_HEMORRHAGE_DOT                   = 89775,
    ROGUE_SPELL_SANGUINARY_VEIN                  = 79147,
    ROGUE_SPELL_SANGUINARY_VEIN_DEBUFF           = 124271,
    SPELL_ROG_NIGHTSTALKER_TALENT                = 14062,
    SPELL_ROG_NIGHTSTALKER                       = 130493,
    SPELL_ROG_SHADOW_FOCUS_TALENT                = 108209,
    SPELL_ROG_SHADOW_FOCUS                       = 112942,
    SPELL_ROGUE_NERVE_STRIKE                     = 108210,
    SPELL_ROGUE_NERVE_STRIKE_EFFECT              = 112947,
    ROGUE_SPELL_COMBAT_READINESS                 = 74001,
    ROGUE_SPELL_COMBAT_INSIGHT                   = 74002,
    SPELL_ROGUE_BLADE_FLURRY                     = 13877,
    SPELL_ROGUE_BLADE_FLURRY_DAMAGE              = 22482,
    SPELL_ROGUE_CHEATING_DEATH                   = 45182,
    SPELL_ROGUE_CHEATED_DEATH                    = 45181,
    ROGUE_SPELL_ENERGETIC_RECOVERY_AURA          = 79152,
    ROGUE_SPELL_GLYPH_OF_EXPOSE_ARMOR            = 56803,
    SPELL_ROG_WEAKENED_ARMOR                     = 113746,
    ROGUE_SPELL_DEADLY_BREW                      = 51626,
    ROGUE_SPELL_GLYPH_OF_HEMORRHAGE              = 56807,
    ROGUE_SPELL_KILLING_SPREE_TELEPORT           = 57840,
    ROGUE_SPELL_KILLING_SPREE_DAMAGE             = 57841,
    SPELL_ROG_MASTERY_MAIN_GAUCH                 = 76806,
    SPELL_ROG_MAIN_GAUCH                         = 86392,
    ROGUE_SPELL_GLYPH_OF_HEMORRAGHING_VEINS      = 146631,
    SPELL_ROGUE_SINISTER_STRIKE                  = 1752,
    SPELL_ROGUE_REVEALING_STRIKE                 = 84617,
    SPELL_ROGUE_REVEALED_WEAKNESS                = 115238,
    SPELL_ROGUE_THROW                            = 121733,
    SPELL_ROGUE_SHURIKEN_TOSS_AURA               = 137586,
    SPELL_ROGUE_SHURIKEN_TOSS                    = 114014,
    SPELL_ROGUE_SHURIKEN_TOSS_AUTO_MAIN_HAND     = 137584,
    SPELL_ROGUE_SHURIKEN_TOSS_AUTO_OFF_HAND      = 137585,
    SPELL_ROGUE_SEAL_FATE                        = 14190,
    SPELL_ROGUE_MUTILATE                         = 5374,
    SPELL_ROGUE_MUTILATE_OFF_HAND                = 27576,
    SPELL_ROGUE_CRIMSON_TEMPEST                  = 121411,
    SPELL_ROGUE_EVISCERATE                       = 2098,
    SPELL_ROGUE_KINDEY_SHOT                      = 408,
    SPELL_ROGUE_ENVENOM                          = 32645,
    SPELL_ROGUE_ANTICIPATION                     = 115189,
    SPELL_ROGUE_ANTICIPATION_COMBO_POINTS        = 115190,
    SPELL_ROGUE_HONOR_AMONG_THIEVES              = 51699,
    SPELL_ROG_MASTER_OF_SUBTLETY_TALENT          = 31223,
    SPELL_ROG_MASTER_OF_SUBTLETY                 = 31665,
    SPELL_ROG_MASTER_OF_SUBTLETY_TIMER           = 31666,
    SPELL_ROG_STEALTH                            = 1784,
    SPELL_ROG_VANISH                             = 11327,
    SPELL_ROGUE_SUBTERFUGE_VANISH                = 115193,
    SPELL_ROGUE_SUBTERFUGE                       = 115192,
    SPELL_ROGUE_SUBTERFUGE_STEALTH               = 115191,
    SPELL_ROGUE_SUBTERFUGE_TALENT                = 108208,
    SPELL_ROG_RELENTLESS_STRIKES_ENERGIZE        = 98440,
    SPELL_ROGUE_SHADOW_BLADES                    = 121471,
    SPELL_ROG_SHADOW_BLADE                       = 121473,
    SPELL_ROG_SHADOW_BLADE_OFF_HAND              = 121474,
    SPELL_ROG_SAP                                = 6770,
    SPELL_ROG_GLYPH_OF_BLIND                     = 91299,
    SPELL_ROG_DEADLY_THROW_SILENCE               = 137576,
    SPELL_ROG_GLYPH_OF_SHARPENED_KNIVES          = 146628,
    SPELL_ROG_TRICKS_OF_THE_TRADE_DAMAGE_MOD     = 57933,
    SPELL_ROG_TRICKS_OF_THE_TRADE_DUMMY          = 59628,
    SPELL_ROG_LEECHING_POISON_HEAL               = 112974,
    SPELL_ROG_GLYPH_OF_RECUPERATE                = 56806,
    SPELL_ROG_GLYPH_OF_RECOVERY                  = 146625,
    SPELL_ROGUE_T14_4P                           = 123122,
    SPELL_ROGUE_RESTLESS_BLADES                  = 79096,
    SPELL_ROGUE_RECUPERATE                       = 73651,
    SPELL_ROGUE_SMOKE_BOMB                       = 88611,
    SPELL_ROGUE_GLYPH_OF_DECOY                   = 56800,
    SPELL_ROGUE_DECOY                            = 89765,
    SPELL_ROGUE_T15_2P_BONUS                     = 138148,
    SPELL_ROGUE_T16_4P_BONUS                     = 145210,
    SPELL_ROGUE_SILENT_BLADES                    = 145193,
    SPELL_ROGUE_BLINDSIDE                        = 121153,
    SPELL_ROGUE_VENDETTA                         = 79140,
    SPELL_ROGUE_TOXICOLOGIST                     = 145249,
    SPELL_ROGUE_BACKSTAB                         = 53,
    SPELL_ROGUE_SLEIGHT_OF_HAND                  = 145211,
    SPELL_ROGUE_GLYPH_OF_GOUGE                   = 56809,
    SPELL_ROGUE_DIRTY_TRICKS                     = 108216,
    SPELL_ROUGE_GROWL                            = 113612,
    SPELL_ROGUE_GLYPH_OF_DISGUISE                = 63268,
    SPELL_ROGUE_DISGUISE                         = 121308,
    SPELL_ROGUE_CLOAK_AND_DAGGER                 = 138106,
    SPELL_ROGUE_CLOAK_AND_DAGGER_TELEPORT        = 132987,
    SPELL_ROGUE_SHADOW_DANCE                     = 51713,
    SPELL_ROGUE_AMBUSH                           = 8676,
    SPELL_ROGUE_GLYPH_OF_KICK                    = 56805,
    SPELL_ROGUE_SHROUD_OF_CONCEALMENT            = 114018,
};

// Called by Expose Armor - 8647
// Glyph of Expose Armor - 56803
class spell_rog_glyph_of_expose_armor : public SpellScriptLoader
{
    public:
        spell_rog_glyph_of_expose_armor() : SpellScriptLoader("spell_rog_glyph_of_expose_armor") { }

        class spell_rog_glyph_of_expose_armor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_glyph_of_expose_armor_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->HasAura(ROGUE_SPELL_GLYPH_OF_EXPOSE_ARMOR))
                        {
                            _player->CastSpell(target, SPELL_ROG_WEAKENED_ARMOR, true);
                            _player->CastSpell(target, SPELL_ROG_WEAKENED_ARMOR, true);
                            _player->CastSpell(target, SPELL_ROG_WEAKENED_ARMOR, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_glyph_of_expose_armor_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_glyph_of_expose_armor_SpellScript();
        }
};

// 113613 - Growl (Symbiosis)
class spell_rog_growl : public SpellScript
{
    PrepareSpellScript(spell_rog_growl);

    void HandleCast()
    {
        ObjectGuid guid = GetCaster()->GetTarget();
        if (Unit* target = ObjectAccessor::GetUnit(*GetCaster(), guid))
            if (GetCaster()->IsValidAttackTarget(target))
                GetCaster()->CastSpell(target, SPELL_ROUGE_GROWL, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_rog_growl::HandleCast);
    }
};

// Combat Readiness - 74001
class spell_rog_combat_readiness : public SpellScriptLoader
{
public:
    spell_rog_combat_readiness() : SpellScriptLoader("spell_rog_combat_readiness") { }

    class spell_rog_combat_readiness_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_combat_readiness_AuraScript);

        bool Load()
        {
            tickCount = 0;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return !(eventInfo.GetHitMask() & PROC_EX_ABSORB);
        }

        void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32& amplitude)
        {
            isPeriodic = true;
            amplitude = 1000;
        }

        void OnPeriodicTick(AuraEffect const* /*aurEff*/)
        {
            if (++tickCount >= 10)
                Remove(AURA_REMOVE_BY_EXPIRE);
        }

        void ResetTimerCheck(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            tickCount = 0;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_rog_combat_readiness_AuraScript::CheckProc);
            DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_rog_combat_readiness_AuraScript::CalcPeriodic, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_combat_readiness_AuraScript::OnPeriodicTick, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            OnEffectProc += AuraEffectProcFn(spell_rog_combat_readiness_AuraScript::ResetTimerCheck, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }

        uint8 tickCount;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_rog_combat_readiness_AuraScript();
    }
};

// 408 - Kidney Shot, 1833 - Cheap Shot
class spell_rog_nerve_strike : public AuraScript
{
    PrepareAuraScript(spell_rog_nerve_strike);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetCaster() && GetTarget())
            if (GetCaster()->HasAura(SPELL_ROGUE_NERVE_STRIKE))
                GetCaster()->CastSpell(GetTarget(), SPELL_ROGUE_NERVE_STRIKE_EFFECT, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_nerve_strike::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 112947 - Nerve Strike
class spell_rog_nerve_strike_effect : public AuraScript
{
    PrepareAuraScript(spell_rog_nerve_strike_effect);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (GetUnitOwner()->GetCharmerOrOwnerPlayerOrPlayerItself())
            amount /= 2;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_nerve_strike_effect::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

// 115834 - Shroud of Concealment
class spell_rog_shroud_of_concealment : public SpellScript
{
    PrepareSpellScript(spell_rog_shroud_of_concealment);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            if (!unit)
                return true;
            if (unit->IsVehicle() && unit->GetTypeId() != TYPEID_PLAYER)
                return true;
            return unit->IsInCombat() || unit->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_shroud_of_concealment::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

// Called by Wound Poison - 8680, Deadly Poison - 2818, Mind-Numbing Poison - 5760, Crippling Poison - 3409
// Paralytic Poison - 113952, Leeching Poison - 112961 and Deadly Poison : Instant damage - 113780
// Master Poisoner - 58410
class spell_rog_master_poisoner : public SpellScriptLoader
{
    public:
        spell_rog_master_poisoner() : SpellScriptLoader("spell_rog_master_poisoner") { }

        class spell_rog_master_poisoner_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_master_poisoner_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->HasAura(ROGUE_SPELL_MASTER_POISONER_AURA))
                            _player->CastSpell(target, ROGUE_SPELL_MASTER_POISONER_DEBUFF, true);

                        switch (GetSpellInfo()->Id)
                        {
                            case 5760:  // Mind-Numbling Poison
                            case 112961:// Leeching Poison
                            case 113952:// Paralytic Poison
                                break;
                            default:
                                return;
                        }

                        if (_player->HasAura(ROGUE_SPELL_DEADLY_BREW))
                            _player->CastSpell(target, ROGUE_SPELL_CRIPPLING_POISON_DEBUFF, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_master_poisoner_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_master_poisoner_SpellScript();
        }
};

// 5171 - Slice and Dice
class spell_rog_slice_and_dice : public SpellScript
{
    PrepareSpellScript(spell_rog_slice_and_dice);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleHit()
    {
        if (Aura* sliceAndDice = GetHitAura())
        {
            int32 comboPoints = GetCaster()->ToPlayer()->GetComboPoints();

            if (GetCaster()->HasAura(SPELL_ROGUE_T15_2P_BONUS))
                comboPoints += 1;

            int32 duration = 6000 + comboPoints * 6000;

            sliceAndDice->SetMaxDuration(duration);
            sliceAndDice->SetDuration(duration);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_slice_and_dice::HandleHit);
    }
};

// Called by Deadly Poison - 2818
// Deadly Poison : Instant damage - 113780
class spell_rog_deadly_poison_instant_damage : public SpellScriptLoader
{
    public:
        spell_rog_deadly_poison_instant_damage() : SpellScriptLoader("spell_rog_deadly_poison_instant_damage") { }

        class spell_rog_deadly_poison_instant_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deadly_poison_instant_damage_SpellScript);

            void HandleOnCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetExplTargetUnit())
                        if (target->HasAura(ROGUE_SPELL_DEADLY_POISON_DOT, _player->GetGUID()))
                            _player->CastSpell(target, ROGUE_SPELL_DEADLY_POISON_INSTANT_DAMAGE, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_rog_deadly_poison_instant_damage_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deadly_poison_instant_damage_SpellScript();
        }
};

// 108215 - Paralytic Poison
class spell_rog_paralytic_poison : public AuraScript
{
    PrepareAuraScript(spell_rog_paralytic_poison);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActionTarget() && !eventInfo.GetActionTarget()->HasAura(SPELL_ROGUE_TOTAL_PARALYSIS);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetActionTarget();
        if (Aura const* paralyticPoison = target->GetAura(SPELL_ROGUE_PARALYTIC_POISON_DEBUFF))
        {
            if (paralyticPoison->GetStackAmount() == 3)
            {
                PreventDefaultAction();
                target->RemoveAurasDueToSpell(SPELL_ROGUE_PARALYTIC_POISON_DEBUFF);
                GetUnitOwner()->CastSpell(target, SPELL_ROGUE_TOTAL_PARALYSIS, true);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_paralytic_poison::CheckProc);
        OnProc += AuraProcFn(spell_rog_paralytic_poison::HandleProc);
    }
};

// Shiv - 5938
class spell_rog_shiv : public SpellScriptLoader
{
    public:
        spell_rog_shiv() : SpellScriptLoader("spell_rog_shiv") { }

        class spell_rog_shiv_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_shiv_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->HasAura(ROGUE_SPELL_MIND_NUMBLING_POISON))
                            _player->CastSpell(target, ROGUE_SPELL_MIND_PARALYSIS, true);
                        else if (_player->HasAura(ROGUE_SPELL_CRIPPLING_POISON))
                            _player->CastSpell(target, ROGUE_SPELL_DEBILITATING_POISON, true);
                        else if (_player->HasAura(ROGUE_SPELL_LEECHING_POISON))
                            _player->CastSpell(_player, ROGUE_SPELL_LEECH_VITALITY, true);
                        else if (_player->HasAura(ROGUE_SPELL_PARALYTIC_POISON))
                            _player->CastSpell(target, ROGUE_SPELL_PARTIAL_PARALYSIS, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_shiv_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_shiv_SpellScript();
        }
};

// All Poisons
// Deadly Poison - 2823, Wound Poison - 8679, Mind-numbing Poison - 5761, Leeching Poison - 108211, Paralytic Poison - 108215 or Crippling Poison - 3408
class spell_rog_poisons : public SpellScriptLoader
{
    public:
        spell_rog_poisons() : SpellScriptLoader("spell_rog_poisons") { }

        class spell_rog_poisons_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_poisons_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    switch (GetSpellInfo()->Id)
                    {
                        case SPELL_ROGUE_WOUND_POISON:
                        {
                            if (_player->HasAura(SPELL_ROGUE_DEADLY_POISON))
                                _player->RemoveAura(SPELL_ROGUE_DEADLY_POISON);
                            break;
                        }
                        case ROGUE_SPELL_MIND_NUMBLING_POISON:
                        {
                            if (_player->HasAura(ROGUE_SPELL_CRIPPLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_CRIPPLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_LEECHING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_LEECHING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_PARALYTIC_POISON))
                                _player->RemoveAura(ROGUE_SPELL_PARALYTIC_POISON);
                            break;
                        }
                        case ROGUE_SPELL_CRIPPLING_POISON:
                        {
                            if (_player->HasAura(ROGUE_SPELL_MIND_NUMBLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_MIND_NUMBLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_LEECHING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_LEECHING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_PARALYTIC_POISON))
                                _player->RemoveAura(ROGUE_SPELL_PARALYTIC_POISON);
                            break;
                        }
                        case ROGUE_SPELL_LEECHING_POISON:
                        {
                            if (_player->HasAura(ROGUE_SPELL_MIND_NUMBLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_MIND_NUMBLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_CRIPPLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_CRIPPLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_PARALYTIC_POISON))
                                _player->RemoveAura(ROGUE_SPELL_PARALYTIC_POISON);
                            break;
                        }
                        case ROGUE_SPELL_PARALYTIC_POISON:
                        {
                            if (_player->HasAura(ROGUE_SPELL_MIND_NUMBLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_MIND_NUMBLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_CRIPPLING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_CRIPPLING_POISON);
                            if (_player->HasAura(ROGUE_SPELL_LEECHING_POISON))
                                _player->RemoveAura(ROGUE_SPELL_LEECHING_POISON);
                            break;
                        }
                        case SPELL_ROGUE_DEADLY_POISON:
                        {
                            if (_player->HasAura(SPELL_ROGUE_WOUND_POISON))
                                _player->RemoveAura(SPELL_ROGUE_WOUND_POISON);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rog_poisons_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_poisons_SpellScript();
        }
};

class spell_rog_deadly_poison : public SpellScriptLoader
{
    public:
        spell_rog_deadly_poison() : SpellScriptLoader("spell_rog_deadly_poison") { }

        class spell_rog_deadly_poison_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rog_deadly_poison_SpellScript);

            bool Load() override
            {
                _stackAmount = 0;
                // at this point CastItem must already be initialized
                return GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCastItem();
            }

            void HandleBeforeHit()
            {
                if (Unit* target = GetHitUnit())
                {
                    // Deadly Poison
                    flag128 flags(0x10000U, 0x80000U);
                    if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, flags, GetCaster()->GetGUID()))
                        _stackAmount = aurEff->GetBase()->GetStackAmount();
                }
            }

            void HandleAfterHit()
            {
                if (_stackAmount < 5)
                    return;

                Player* player = GetCaster()->ToPlayer();

                if (Unit* target = GetHitUnit())
                {

                    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

                    if (item == GetCastItem())
                        item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                    if (!item)
                        return;

                    // item combat enchantments
                    for (uint8 slot = 0; slot < MAX_ENCHANTMENT_SLOT; ++slot)
                    {
                        if (slot > PRISMATIC_ENCHANTMENT_SLOT || slot < PROP_ENCHANTMENT_SLOT_0)    // not holding enchantment id
                            continue;

                        SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(item->GetEnchantmentId(EnchantmentSlot(slot)));
                        if (!enchant)
                            continue;

                        for (uint8 s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
                        {
                            if (enchant->Type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                                continue;

                            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(enchant->SpellID[s]);
                            if (!spellInfo)
                            {
                                TC_LOG_ERROR("spells", "Player::CastItemCombatSpell Enchant %i, player (Name: %s, GUID: %u) cast unknown spell %i",
                                             enchant->ID, player->GetName().c_str(), player->GetGUID().GetCounter(), enchant->SpellID[s]);
                                continue;
                            }

                            // Proc only rogue poisons
                            if (spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE || spellInfo->Dispel != DISPEL_POISON)
                                continue;

                            // Do not reproc deadly
                            if (spellInfo->SpellFamilyFlags.IsEqual(0x10000, 0x80000, 0))
                                continue;

                            if (spellInfo->IsPositive())
                                player->CastSpell(player, enchant->SpellID[s], true, item);
                            else
                                player->CastSpell(target, enchant->SpellID[s], true, item);
                        }
                    }
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_rog_deadly_poison_SpellScript::HandleBeforeHit);
                AfterHit += SpellHitFn(spell_rog_deadly_poison_SpellScript::HandleAfterHit);
            }

            uint8 _stackAmount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rog_deadly_poison_SpellScript();
        }
};

// Marked for Death - 137619
class spell_rog_marked_for_death : public SpellScriptLoader
{
    public:
        spell_rog_marked_for_death() : SpellScriptLoader("spell_rog_marked_for_death") { }

        class spell_rog_marked_for_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rog_marked_for_death_AuraScript );

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster() && GetTarget() && GetCaster()->GetTypeId() == TYPEID_PLAYER)
                    if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                        GetCaster()->ToPlayer()->RemoveSpellCooldown(GetId(), true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_rog_marked_for_death_AuraScript ::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rog_marked_for_death_AuraScript();
        }
};

// 76577 - Smoke Bomb
class spell_rog_smoke_bomb : public AuraScript
{
    PrepareAuraScript(spell_rog_smoke_bomb);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (DynamicObject* dyn = GetTarget()->GetDynObject(aurEff->GetId()))
            GetTarget()->CastSpell(*dyn, SPELL_ROGUE_SMOKE_BOMB, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_smoke_bomb::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 1856 - Vanish
class spell_rog_glyph_of_decoy : public SpellScript
{
    PrepareSpellScript(spell_rog_glyph_of_decoy);

    void HandleCast()
    {
        Unit* rogue = GetCaster();
        if (!rogue->HasAura(SPELL_ROGUE_GLYPH_OF_DECOY))
            return;

        rogue->CastSpell(rogue, SPELL_ROGUE_DECOY, true);
        for (auto&& summon : rogue->GetSummons())
        {
            if (summon->GetEntry() == 62261)
            {
                rogue->CastSpell(summon, 45204, true);  // Clone Me
                rogue->CastSpell(summon, 41054, true);  // Copy Weapon
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_rog_glyph_of_decoy::HandleCast);
    }
};

struct KillingSpreeInvalidTargetCheck
{
    KillingSpreeInvalidTargetCheck(Unit* caster) : m_caster(caster) { }

    bool operator()(WorldObject* target)
    {
        Unit* unit = target->ToUnit();
        return !unit
            || !m_caster->IsValidAttackTarget(unit)
            || unit->HasInvisibilityAura()
            || unit->HasStealthAura()
            || unit->GetCreatureType() == CREATURE_TYPE_CRITTER
            || unit->IsTotem()
            || unit->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET
            || unit->GetCreatureType() == CREATURE_TYPE_WILD_PET
            || unit->HasCrowdControlAuraType(SPELL_AURA_MOD_FEAR)
            || unit->HasCrowdControlAuraType(SPELL_AURA_MOD_FEAR_2)
            || unit->HasCrowdControlAuraType(SPELL_AURA_MOD_CONFUSE)
            || !m_caster->IsWithinLOSInMap(unit);
    }
private:
    Unit* m_caster;
};

// 51690 - Killing Spree
class spell_rog_killing_spree : public AuraScript
{
    PrepareAuraScript(spell_rog_killing_spree);

    std::set<ObjectGuid> m_targets;
    ObjectGuid m_mainTarget = ObjectGuid::Empty;
    bool m_glyphed = false;
    WorldLocation m_start;
    uint32 m_phase = 0;
    Player* m_caster = nullptr;

    bool Load() override
    {
        m_caster = GetUnitOwner()->ToPlayer();
        return m_caster != nullptr;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (m_caster->HasAura(SPELL_ROGUE_GLYPH_OF_KILLING_SPREE))
        {
            m_glyphed = true;
            m_start.Relocate(m_caster->GetPosition());
            m_start.m_mapId = m_caster->GetMapId();
            m_phase = m_caster->GetPhaseMask();
        }

        m_caster->CastSpell(m_caster, SPELL_ROGUE_KILLING_SPREE_DAMAGE_AURA, true);

        if (m_caster->HasAura(SPELL_ROGUE_SUBTERFUGE_STEALTH) || m_caster->HasAura(SPELL_ROGUE_SUBTERFUGE_VANISH))
        {
            if (!m_caster->HasAura(SPELL_ROGUE_SUBTERFUGE))
                m_caster->CastSpell(m_caster, SPELL_ROGUE_SUBTERFUGE, true);
        }
        else
            m_caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (m_glyphed)
        {
            WorldLocation loc{ m_start };
            uint32 phase = m_phase;
            m_caster->Schedule(Milliseconds(1), [=](Unit* rogue)
            {
                // Lets exclude teleport to:
                if (rogue->GetMapId() == loc.GetMapId() &&  // another map
                    (rogue->GetPhaseMask() & phase) &&      // another phase
                    rogue->GetExactDist(&loc) < 100.0f)     // and just inappropriate big distances
                    rogue->NearTeleportTo(loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation());
            });
        }
        m_caster->RemoveAurasDueToSpell(SPELL_ROGUE_KILLING_SPREE_DAMAGE_AURA);
    }

    void HandlePeriodic(AuraEffect const*)
    {
        KillingSpreeInvalidTargetCheck notValidTarget{ m_caster };
        Unit* target = nullptr;
        if (m_mainTarget)
        {
            target = ObjectAccessor::GetUnit(*m_caster, m_mainTarget);
            if (!target || !target->IsAlive())
                target = nullptr;
        }
        else
        {
            while (!m_targets.empty())
            {
                ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(m_targets);
                target = ObjectAccessor::GetUnit(*m_caster, guid);
                if (target && target->IsAlive() && !notValidTarget(target))
                    break;
                target = nullptr;
                m_targets.erase(guid);
            }
        }
        if (target)
        {
            m_caster->CastSpell(target, ROGUE_SPELL_KILLING_SPREE_TELEPORT, true);
            if (!m_mainTarget || !notValidTarget(target))
            {
                m_caster->CastSpell(target, ROGUE_SPELL_KILLING_SPREE_DAMAGE, true);
                if (Unit* selection = m_caster->GetSelectedUnit())
                    m_caster->Attack(selection, true);
            }
            else
                m_caster->AttackStop();
        }
        else
        {
            SetDuration(0);
            m_caster->AttackStop();
        }
    }
public:
    void AddTarget(Unit* unit)
    {
        m_targets.insert(unit->GetGUID());
        if (unit->GetGUID() == m_caster->GetTarget() || (!m_caster->GetTarget() && !m_mainTarget))
            if (!m_caster->HasAura(SPELL_ROGUE_BLADE_FLURRY))
                m_mainTarget = unit->GetGUID();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rog_killing_spree::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_killing_spree::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_killing_spree::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 51690 - Killing Spree
class spell_rog_killing_spree_target_selector : public SpellScript
{
    PrepareSpellScript(spell_rog_killing_spree_target_selector);

    SpellCastResult CheckCast()
    {
        Player* rogue = GetCaster()->ToPlayer();
        if (!rogue)
            return SPELL_FAILED_DONT_REPORT;
        if (Unit* target = rogue->GetSelectedUnit())
        {
            if (!rogue->IsValidAttackTarget(target))
                return SPELL_FAILED_BAD_TARGETS;
            if (!target->IsWithinDist3d(rogue, GetSpellInfo()->GetMaxRange()))
                return SPELL_FAILED_OUT_OF_RANGE;
            if (!GetCaster()->IsWithinLOSInMap(target))
                return SPELL_FAILED_LINE_OF_SIGHT;
        }
        return SPELL_CAST_OK;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(KillingSpreeInvalidTargetCheck{ GetCaster() });
        if (targets.empty() || GetCaster()->GetVehicleBase())
        {
            if (Player* rogue = GetCaster()->ToPlayer())
                rogue->GetGlobalCooldownMgr().CancelGlobalCooldown(GetSpellInfo());
            FinishCast(SPELL_FAILED_OUT_OF_RANGE);
            return;
        }
    }

    void AddTarget(SpellEffIndex)
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_ROGUE_KILLING_SPREE))
            if (spell_rog_killing_spree* script = dynamic_cast<spell_rog_killing_spree*>(aura->GetScriptByName("spell_rog_killing_spree")))
                script->AddTarget(GetHitUnit());
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_killing_spree_target_selector::CheckCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_killing_spree_target_selector::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_rog_killing_spree_target_selector::AddTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 57840 - Killing Spree, 36563 - Shadowstep
class spell_rog_killing_spree_teleport : public SpellScript
{
    PrepareSpellScript(spell_rog_killing_spree_teleport);

    void TargetSelect(SpellDestination& dest)
    {
        Unit* target = GetExplTargetUnit();
        if (!target)
            return;

        Position& pos = dest._position;

        // Immerseus
        if (target->GetEntry() != 71543)
        {
            pos = target->GetPosition();
            target->MovePositionToFirstCollision(pos, GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster()), M_PI, 1.5f);
            float newz = 1.5f;
            GetCaster()->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, &newz);
        }
        // If final point end up being too high (standing against a wall) or too low (standing against a hole) - just set destination to target's position
        if (abs(pos.GetPositionZ() - target->GetPositionZ()) > 2.5f)
            pos.Relocate(target);
    }

    void Register()
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_rog_killing_spree_teleport::TargetSelect, EFFECT_0, TARGET_DEST_TARGET_BACK);
    }
};

// 8676 - Ambush
class spell_rog_ambush : public SpellScript
{
    PrepareSpellScript(spell_rog_ambush);

    void HandleHit()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        Item* item = caster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        if (item && item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
            SetHitDamage(GetHitDamage() * 1.447);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_ambush::HandleHit);
    }
};

// 76806 - Mastery: Main Gauche
class spell_rog_main_gauch : public AuraScript
{
    PrepareAuraScript(spell_rog_main_gauch);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            return;

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->EquippedItemClass != ITEM_CLASS_WEAPON &&
            eventInfo.GetSpellInfo()->Id != SPELL_ROG_SHADOW_BLADE)
           return;

        if (roll_chance_f(mastery->GetFloatAmount()))
            eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), SPELL_ROG_MAIN_GAUCH, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_main_gauch::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 35551 - Combat Potency
class spell_rog_combat_potency : public AuraScript
{
    PrepareAuraScript(spell_rog_combat_potency);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        if (eventInfo.GetSpellInfo())
        {
            if (eventInfo.GetSpellInfo()->Id != SPELL_ROG_MAIN_GAUCH &&
                eventInfo.GetSpellInfo()->Id != SPELL_ROG_SHADOW_BLADE_OFF_HAND)
                return false;
        }
        else if (!(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK))
            return false;

        float offHandSpeed = player->GetAttackTime(OFF_ATTACK) / IN_MILLISECONDS;
        if (Item* offItem = player->GetWeaponForAttack(OFF_ATTACK))
            offHandSpeed = offItem->GetTemplate() ? (float)offItem->GetTemplate()->Delay / 1000.f : offHandSpeed;

        return roll_chance_f(20.0f * offHandSpeed / 1.4f);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_combat_potency::CheckProc);
    }
};

// 79147 - Sanguinary Vein
// Called by Rupture - 1943, Garrote - 703 and Crimson Tempest - 122233 and Hemorrhage - 89775 (required for glyph handling)
class spell_rog_sanguinary_vein : public AuraScript
{
    PrepareAuraScript(spell_rog_sanguinary_vein);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        if (!caster || !target)
            return;

        // Glyph of Hemorrhaging Veins
        if (GetId() == ROGUE_SPELL_HEMORRHAGE_DOT && !caster->HasAura(146631))
            return;

        if (caster->HasAura(79147))
            caster->CastSpell(target, ROGUE_SPELL_SANGUINARY_VEIN_DEBUFF, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* target = GetTarget();

        for (auto&& it : { SPELL_ROG_RUPTURE, SPELL_ROG_GARROTE, ROGUE_SPELL_CRIMSON_TEMPEST_DOT })
            if (target->HasAura(it, GetCasterGUID()))
                return;

        // Glyph of Hemorrhaging Veins
        if (caster->HasAura(ROGUE_SPELL_GLYPH_OF_HEMORRAGHING_VEINS) && target->HasAura(ROGUE_SPELL_HEMORRHAGE_DOT, GetCasterGUID()))
            return;

        target->RemoveAurasDueToSpell(ROGUE_SPELL_SANGUINARY_VEIN_DEBUFF, GetCasterGUID());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rog_sanguinary_vein::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_sanguinary_vein::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1752 - Sinister Strike 
class spell_rog_sinister_strike : public SpellScript
{
    PrepareSpellScript(spell_rog_sinister_strike);

    void HandleHit()
    {
        if (GetHitUnit()->HasAura(SPELL_ROGUE_REVEALING_STRIKE, GetCaster()->GetGUID()))
            if (roll_chance_i(20))
                GetCaster()->CastSpell(GetHitUnit(), SPELL_ROGUE_REVEALED_WEAKNESS, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_sinister_strike::HandleHit);
    }
};

// 408 - Kidney Shot
class spell_rog_kidney_shot : public AuraScript
{
    PrepareAuraScript(spell_rog_kidney_shot);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTarget()->GetAura(SPELL_ROGUE_REVEALING_STRIKE, GetCasterGUID()))
            GetAura()->SetDuration(GetAura()->GetDuration() * 1.35f);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rog_kidney_shot::HandleApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 121411 - Crimson Tempest
class spell_rog_crimson_tempest : public SpellScript
{
    PrepareSpellScript(spell_rog_crimson_tempest);

    void HandleHit()
    {
        if (Player* rogue = GetCaster()->ToPlayer())
        {
            if (Unit* target = GetHitUnit())
            {
                int32 damage = CalculatePct(GetHitDamage() + GetAbsorbedDamage(), 240) / 6; // 240% / 6 (tick count)
                for (auto&& itr : GetHitUnit()->GetAuraEffectsByType(SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT))
                    if ((1 << itr->GetMiscValue()) & (1 << MECHANIC_BLEED))
                        AddPct(damage, itr->GetFloatAmount());

                AddPct(damage, rogue->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC, MECHANIC_BLEED));

                rogue->CastCustomSpell(ROGUE_SPELL_CRIMSON_TEMPEST_DOT, SPELLVALUE_BASE_POINT0, damage, target, true);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_rog_crimson_tempest::HandleHit);
    }
};

// 13877 - Blade Flurry
class spell_rog_blade_flurry : public AuraScript
{
    PrepareAuraScript(spell_rog_blade_flurry);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* rogue = GetTarget()->ToPlayer();
        Unit* target = eventInfo.GetActionTarget();
        if (!rogue)
            return;

        if (eventInfo.GetSpellInfo())
        {
            if (eventInfo.GetSpellInfo()->IsAoESpell())
                return;
            switch (eventInfo.GetSpellInfo()->Id)
            {
                case 26679:  // Deadly throw
                case 114014: // Shuriken Toss
                case 137584: // Shuriken Toss
                case 137585: // Shuriken Toss Off - hand
                case 140308: // Shadow Shuriken Toss
                case 140309: // Shadow Shuriken Toss Off - hand
                case 121733: // Throw
                    return;
                default:
                    break;
            }
        }

        int32 damage = eventInfo.GetDamage();
        if (eventInfo.GetProcTriggerContext()->MeleeDamage)
            damage += eventInfo.GetProcTriggerContext()->MeleeDamage->absorb;
        else if (eventInfo.GetProcTriggerContext()->SpellDamage)
            damage += eventInfo.GetProcTriggerContext()->SpellDamage->absorb;

        damage = CalculatePct(damage, 40.0f);
        if (!damage)
            return;

        damage /= GetDamageTakenCorrectiveFactor(rogue, target, eventInfo.GetSpellInfo());

        rogue->CastCustomSpell(SPELL_ROGUE_BLADE_FLURRY_DAMAGE, SPELLVALUE_BASE_POINT0, damage, target, true, nullptr, GetEffect(EFFECT_0));
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_blade_flurry::HandleProc);
    }
};

// 22482 - Blade Flurry (damage) 
class spell_rog_blade_flurry_damage : public SpellScript
{
    PrepareSpellScript(spell_rog_blade_flurry_damage);

    void SelectTargets(std::list<WorldObject*>& /*targets*/)
    {
        RemoveUnitTarget(GetExplTargetUnit()->GetGUID());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_blade_flurry_damage::SelectTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
    }
};

// Hack for Blade Flurry that shoud not apply posions (I think, temporarily).
class spell_rog_poison_proc : public AuraScript
{
    PrepareAuraScript(spell_rog_poison_proc);

    bool CheckProc(ProcEventInfo& procEvent)
    {
        if (!procEvent.GetSpellInfo())  // Auto attack.
            return true;

        switch (procEvent.GetSpellInfo()->Id)
        {
            case SPELL_ROGUE_SHURIKEN_TOSS:
            case SPELL_ROGUE_SHURIKEN_TOSS_AUTO_MAIN_HAND:
            case SPELL_ROGUE_SHURIKEN_TOSS_AUTO_OFF_HAND:
                return GetSpellInfo()->Id == SPELL_ROGUE_DEADLY_POISON // Only lethal poisons.
                    || GetSpellInfo()->Id == SPELL_ROGUE_WOUND_POISON;
            case SPELL_ROGUE_BLADE_FLURRY_DAMAGE:
            case SPELL_ROGUE_THROW:
                return false;
            default:
                break;
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_poison_proc::CheckProc);
    }
};

// 31230 - Cheat Death 
class spell_rog_cheat_death : public AuraScript
{
    PrepareAuraScript(spell_rog_cheat_death);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void Absorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (Unit* target = GetTarget())
        {
            if (dmgInfo.GetDamage() < target->GetHealth())
                return;

            if (target->HasAura(SPELL_ROGUE_CHEATED_DEATH))
                return;

            target->CastSpell(target, SPELL_ROGUE_CHEATING_DEATH, true);
            target->CastSpell(target, SPELL_ROGUE_CHEATED_DEATH, true);

            uint32 health10 = target->CountPctFromMaxHealth(10);

            // hp > 10% - absorb hp till 10%
            if (target->GetHealth() > health10)
                absorbAmount = dmgInfo.GetDamage() - target->GetHealth() + health10;
            // hp lower than 10% - absorb everything
            else
                absorbAmount = dmgInfo.GetDamage();
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_cheat_death::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_rog_cheat_death::Absorb, EFFECT_0);
    }
};

// 114014 - Shuriken Toss
class spell_rog_shuriken_toss : public SpellScript
{
    PrepareSpellScript(spell_rog_shuriken_toss);

    void HandleLaunch(SpellEffIndex)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (target && caster->GetDistance(target) > 10.f)
            if (GetSpell()->GetTargetInfo(target->GetGUID())->missCondition != SPELL_MISS_IMMUNE)
                if (!caster->HasAura(SPELL_ROGUE_SHADOW_BLADES))
                    caster->CastSpell(caster, SPELL_ROGUE_SHURIKEN_TOSS_AURA, true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_rog_shuriken_toss::HandleLaunch, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 137586 - Shuriken Toss (Aura)
class spell_rog_shuriken_toss_aura : public AuraScript
{
    PrepareAuraScript(spell_rog_shuriken_toss_aura);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && GetUnitOwner() != eventInfo.GetActionTarget())
            if (GetUnitOwner()->IsWithinMeleeRange(eventInfo.GetActionTarget()))    // Melee range reached, remove masterOfSubtlety. 
                Remove();
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_rog_shuriken_toss_aura::HandleProc);
    }
};

// 1329 - Mutilate
class spell_rog_mutilate : public SpellScript
{
    PrepareSpellScript(spell_rog_mutilate);

    void HandleCast()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_ROGUE_SILENT_BLADES);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_rog_mutilate::HandleCast);
    }
};

// 14190 - Seal Fate
class spell_rog_seal_fate : public AuraScript
{
    PrepareAuraScript(spell_rog_seal_fate);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    bool CheckProc(ProcEventInfo& procEvent)
    {
        if (procEvent.GetSpellInfo()->Id == SPELL_ROGUE_MUTILATE || procEvent.GetSpellInfo()->Id == SPELL_ROGUE_MUTILATE_OFF_HAND)
        {
            if (GetUnitOwner()->ToPlayer()->HasSpellCooldown(SPELL_ROGUE_SEAL_FATE))
                return false;
            GetUnitOwner()->ToPlayer()->AddSpellCooldown(SPELL_ROGUE_SEAL_FATE, Milliseconds(100));
        }
        if (procEvent.GetSpellInfo()->RangeEntry && procEvent.GetSpellInfo()->RangeEntry->type != SPELL_RANGE_MELEE)
            return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_seal_fate::CheckProc);
    }
};

// 114015 - Anticipation
class spell_rog_anticipation : public AuraScript
{
    PrepareAuraScript(spell_rog_anticipation);

    void HandleProc(ProcEventInfo& procEvent)
    {
        if (!procEvent.GetActionTarget())
            return;

        if (procEvent.GetSpellInfo() && procEvent.GetSpellInfo()->Id == SPELL_ROGUE_ENVENOM && procEvent.GetActionTarget() == GetUnitOwner())
            return;

        GetUnitOwner()->Schedule(Milliseconds(1), [=](Unit* rogue)
        {
            if (Aura* aura = rogue->GetAura(SPELL_ROGUE_ANTICIPATION))
            {
                if (Unit* target = ObjectAccessor::GetUnit(*rogue, rogue->GetTarget()))
                {
                    if (rogue->IsValidAttackTarget(target, sSpellMgr->GetSpellInfo(SPELL_ROGUE_ANTICIPATION_COMBO_POINTS)))
                    {
                        rogue->CastCustomSpell(SPELL_ROGUE_ANTICIPATION_COMBO_POINTS, SPELLVALUE_BASE_POINT0, aura->GetStackAmount(), target, true);
                        rogue->RemoveAura(aura);
                    }
                }
            }
        });
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_anticipation::HandleProc);
    }
};

// 51701 - Honor Among Thieves
class spell_rog_honor_among_thieves : public AuraScript
{
    PrepareAuraScript(spell_rog_honor_among_thieves);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
            return false;

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_ROGUE_THROW)
            return false;

        return GetUnitOwner()->IsInCombat();
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        // We need to check this because proc cooldown applied to the actor and not to the aura owner (and it's okay, e.g. stormlash)
        if (GetUnitOwner()->ToPlayer()->HasSpellCooldown(SPELL_ROGUE_HONOR_AMONG_THIEVES))
            return;

        GetUnitOwner()->Schedule(Milliseconds(1), [=](Unit* rogue)
        {
            if (!rogue->IsInCombat())
                return;

            Unit* target = rogue->ToPlayer()->GetComboTarget();
            if (!target)
                target = rogue->GetVictim();
            if (!target)
            {
                Unit* selectedUnit = ObjectAccessor::GetUnit(*rogue, rogue->GetTarget());
                if (selectedUnit && rogue->IsValidAttackTarget(selectedUnit))
                    target = selectedUnit;
            }

            if (target)
            {
                rogue->CastSpell(target, SPELL_ROGUE_HONOR_AMONG_THIEVES, true);
                rogue->ToPlayer()->AddSpellCooldown(SPELL_ROGUE_HONOR_AMONG_THIEVES, Seconds(2));
            }
        });
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_honor_among_thieves::CheckProc);
        OnProc += AuraProcFn(spell_rog_honor_among_thieves::HandleProc);
    }
};

// 51667 - Cut to the Chase
class spell_rog_cut_to_the_chase : public AuraScript
{
    PrepareAuraScript(spell_rog_cut_to_the_chase);

    void HandleProc(ProcEventInfo&)
    {
        if (Aura* sliceAndDice = GetCaster()->GetAura(SPELL_ROGUE_SLICE_AND_DICE))
        {
            sliceAndDice->SetMaxDuration(36 * IN_MILLISECONDS);
            sliceAndDice->SetDuration(36 * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_cut_to_the_chase::HandleProc);
    }
};

// 16511 - Hemorrhage
class spell_rog_hemorrhage : public SpellScript
{
    PrepareSpellScript(spell_rog_hemorrhage);

    void HandleOnHit()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        if (Item* weapon = caster->GetWeaponForAttack(BASE_ATTACK))
            if (weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                SetHitDamage(GetHitDamage() * 1.45f);
    }

    void HandleAfterHit()
    {
        if (GetCaster()->HasAura(ROGUE_SPELL_GLYPH_OF_HEMORRHAGE))
            if (!GetHitUnit()->HasAuraState(AURA_STATE_BLEEDING))
                return;

        // After all the damage modifications.
        GetCaster()->CastCustomSpell(ROGUE_SPELL_HEMORRHAGE_DOT, SPELLVALUE_BASE_POINT0, GetHitDamage() / 2.0f / 8.0f, GetHitUnit(), true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_hemorrhage::HandleOnHit);
        AfterHit += SpellHitFn(spell_rog_hemorrhage::HandleAfterHit);
    }
};

// 51723 - Fan of Knives
class spell_rog_fan_of_knives : public SpellScript
{
    PrepareSpellScript(spell_rog_fan_of_knives)

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        Unit* target = targets.empty() ? nullptr : targets.front()->ToUnit();
        targets.clear();

        if (Player* rogue = GetCaster()->ToPlayer())
        {
            if (rogue->GetComboTarget())
                target = rogue->GetComboTarget();
            else if (Unit* comboTarget = ObjectAccessor::GetUnit(*rogue, rogue->GetTarget()))
            {
                if (rogue->IsValidAttackTarget(comboTarget))
                    target = comboTarget;
            }
        }

        if (target)
            targets.push_back(target);
    }

    void HandleHit()
    {
        Unit* rogue = GetCaster();
        if (rogue->HasAura(SPELL_ROG_GLYPH_OF_SHARPENED_KNIVES))
            rogue->CastSpell(GetHitUnit(), SPELL_ROG_WEAKENED_ARMOR, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_fan_of_knives::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_rog_fan_of_knives::HandleHit);
    }
};

// 1856 - Vanish
class spell_rog_vanish_initial : public SpellScript
{
    PrepareSpellScript(spell_rog_vanish_initial);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(34709))
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_vanish_initial::CheckCast);
    }
};

// 18461. Triggered from Vanish (1856).
class spell_rog_vanish_trigger : public SpellScript
{
    PrepareSpellScript(spell_rog_vanish_trigger);

    void Vanish()
    {
        Unit* rogue = GetHitUnit();
        rogue->RemoveMovementImpairingAuras(true);
        rogue->RemoveAurasByType(SPELL_AURA_MOD_STALKED);

        UnitList targets;
        Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(rogue, rogue, rogue->GetMap()->GetVisibilityRange());
        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(rogue, targets, u_check);
        rogue->VisitNearbyObject(rogue->GetMap()->GetVisibilityRange(), searcher);
        for (auto&& it : targets)
        {
            if (!it->HasUnitState(UNIT_STATE_CASTING))
                continue;

            for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
            {
                if (it->GetCurrentSpell(i)
                    && it->GetCurrentSpell(i)->m_targets.GetUnitTargetGUID() == rogue->GetGUID())
                {
                    it->InterruptSpell(CurrentSpellTypes(i), false);
                }
            }
        }

        // If this spell is given to an NPC, it must handle the rest using its own AI
        if (rogue->GetTypeId() != TYPEID_PLAYER)
            return;

        uint32 spellId = rogue->HasSpell(SPELL_ROGUE_SUBTERFUGE_TALENT) ? SPELL_ROGUE_SUBTERFUGE_VANISH : SPELL_ROG_VANISH;
        rogue->CastSpell(rogue, spellId, true);
        if (!rogue->GetInstanceScript() || !rogue->GetInstanceScript()->IsEncounterInProgress())
            rogue->CombatStop();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_vanish_trigger::Vanish);
    }
};

class spell_rog_invisibility_base : public AuraScript
{
protected:
    void CalculateIncreaseSpeed(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* effect = GetUnitOwner()->GetAuraEffect(SPELL_ELUSIVENESS, EFFECT_0))
            amount += effect->GetAmount();
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* rogue = GetUnitOwner();
        if (Aura* talent = rogue->GetAura(SPELL_ROG_MASTER_OF_SUBTLETY_TALENT))
            rogue->CastCustomSpell(SPELL_ROG_MASTER_OF_SUBTLETY, SPELLVALUE_BASE_POINT0, talent->GetSpellInfo()->Effects[0].BasePoints, rogue, true);

        if (rogue->HasAura(SPELL_ROG_NIGHTSTALKER_TALENT))
            rogue->CastSpell(rogue, SPELL_ROG_NIGHTSTALKER, true);

        if (rogue->HasAura(SPELL_ROG_SHADOW_FOCUS_TALENT))
            rogue->CastSpell(rogue, SPELL_ROG_SHADOW_FOCUS, true);

        rogue->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);    // Needs for vanish + subterfuge
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* rogue = GetUnitOwner();
        if (Aura* masterOfSubtlety = rogue->GetAura(SPELL_ROG_MASTER_OF_SUBTLETY))
        {
            masterOfSubtlety->SetMaxDuration(6000);
            masterOfSubtlety->RefreshDuration();
        }

        if (Aura* nightstalker = rogue->GetAura(SPELL_ROG_NIGHTSTALKER))
            nightstalker->SetDuration(100); // Set duration instead of remove as stealth removal is done before spell-cast damage calculation

        rogue->RemoveAura(SPELL_ROG_SHADOW_FOCUS);
        rogue->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);    // Needs for vanish + subterfuge

        if (!rogue->HasStealthAura())
            rogue->RemoveOwnedAura(SPELL_ROGUE_SHROUD_OF_CONCEALMENT);
    }
};

// 1784 - Stealth
class spell_rog_stealth : public spell_rog_invisibility_base
{
    PrepareAuraScript(spell_rog_stealth);

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_stealth::CalculateIncreaseSpeed, EFFECT_2, SPELL_AURA_MOD_SPEED_ALWAYS);
        OnEffectApply += AuraEffectApplyFn(spell_rog_stealth::HandleApply, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_stealth::HandleRemove, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_rog_stealth_subterfuge_trigger : public spell_rog_invisibility_base
{
protected:
    bool immediatly = false;

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            Spell const* procSpell = eventInfo.TriggeredBySpell();
            // Food...And not procs from buffs gained by consumable items
            if (procSpell && procSpell->m_CastItem && !procSpell->TriggeredByAuraSpell())
            {
                ItemTemplate const* proto = eventInfo.TriggeredBySpell()->m_CastItem->GetTemplate();
                if (proto->Class == ITEM_CLASS_CONSUMABLE && proto->SubClass == ITEM_SUBCLASS_FOOD_DRINK)
                {
                    immediatly = true;
                    return true;
                }
            }
            if (eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK)
            {
                if (GetId() == SPELL_ROGUE_SUBTERFUGE_VANISH) // Vanish gives tolerance to removing from incoming damage.
                    return false;
                if (!eventInfo.GetDamageInfo()->GetDamage() && eventInfo.GetActor() && !eventInfo.GetActor()->IsValidAttackTarget(GetUnitOwner(), spellInfo, nullptr, true))
                    return false;

                if (spellInfo->Id == SPELL_ROG_SAP)
                    immediatly = true;

                if (spellInfo->GetAllEffectsMechanicMask() & ((1 << MECHANIC_DISORIENTED) | (1 << MECHANIC_FREEZE)))
                    immediatly = true;

                if (immediatly)
                    return true;
            }
            else if (eventInfo.GetTypeMask() & DONE_HIT_PROC_FLAG_MASK)
            {
                if (spellInfo->AttributesEx & SPELL_ATTR1_NOT_BREAK_STEALTH)
                    return false;
                if (spellInfo->IsPositive())
                {
                    // Master of subtlety, trinket procs (buff)
                    if (eventInfo.TriggeredBySpell() && eventInfo.TriggeredBySpell()->IsTriggered())
                        return false;
                    if (spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE) // Mounts, may be need to change.
                        return false;
                }
                else if (eventInfo.TriggeredBySpell())
                {
                    // Attacker procs from weapon.
                    if (eventInfo.TriggeredBySpell()->IsTriggered() && spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE)
                        return false;
                }
                if (spellInfo->Id == GetId())
                    return false;
            }
        }
        return !GetUnitOwner()->HasAura(SPELL_ROGUE_SUBTERFUGE);
    }

    void HandleProc(ProcEventInfo&)
    {
        if (!immediatly)
        {
            PreventDefaultAction();
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_ROGUE_SUBTERFUGE, true);
        }
    }
};

// 115191 - Stealth (Subterfuge)
class spell_rog_stealth_subterfuge : public spell_rog_stealth_subterfuge_trigger
{
    PrepareAuraScript(spell_rog_stealth_subterfuge);

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_stealth_subterfuge::CalculateIncreaseSpeed, EFFECT_2, SPELL_AURA_MOD_SPEED_ALWAYS);
        OnEffectApply += AuraEffectApplyFn(spell_rog_stealth_subterfuge::HandleApply, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_stealth_subterfuge::HandleRemove, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_rog_stealth_subterfuge::CheckProc);
        OnProc += AuraProcFn(spell_rog_stealth_subterfuge::HandleProc);
    }
};

// 11327 - Vanish, 115193 - Vanish
class spell_rog_vanish : public spell_rog_stealth_subterfuge_trigger
{
    PrepareAuraScript(spell_rog_vanish);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->HasAura(SPELL_AURA_MOD_STEALTH))
            return false;
        return spell_rog_stealth_subterfuge_trigger::CheckProc(eventInfo);
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            Unit* owner = GetUnitOwner();
            uint32 spellId = owner->HasSpell(SPELL_ROGUE_SUBTERFUGE_TALENT) ? SPELL_ROGUE_SUBTERFUGE_STEALTH : SPELL_ROG_STEALTH;
            if (!owner->HasAura(spellId))
                owner->CastSpell(owner, spellId, true);
        }
        else
            spell_rog_invisibility_base::HandleRemove(eff, mode);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_vanish::CalculateIncreaseSpeed, EFFECT_2, SPELL_AURA_MOD_SPEED_ALWAYS);
        OnEffectApply += AuraEffectApplyFn(spell_rog_vanish::HandleApply, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_vanish::HandleRemove, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_rog_vanish::CheckProc);
        OnProc += AuraProcFn(spell_rog_vanish::HandleProc);
    }
};

// 79134 - Venomous Wounds
class spell_rog_venomous_wounds : public AuraScript
{
    PrepareAuraScript(spell_rog_venomous_wounds);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredByPeriodic() || !eventInfo.GetActionTarget())
            return false;
        if (eventInfo.GetSpellInfo()->Id != SPELL_ROG_GARROTE && eventInfo.GetSpellInfo()->Id != SPELL_ROG_RUPTURE)
            return false;
        if (eventInfo.GetSpellInfo()->Id == SPELL_ROG_GARROTE && eventInfo.GetActionTarget()->HasAura(SPELL_ROG_RUPTURE, GetCasterGUID()))
            return false;

        for (auto&& it : eventInfo.GetActionTarget()->GetAppliedAuras())
        {
            if (it.second->GetBase()->GetCasterGUID() == GetCasterGUID())
                if (it.second->GetBase()->GetSpellInfo()->Dispel == DISPEL_POISON)
                    return true;
        }
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetProcTarget();
        Unit* caster = GetUnitOwner();
        caster->CastSpell(target, SPELL_ROGUE_VENOMOUS_WOUND, true);
        caster->CastCustomSpell(SPELL_ROGUE_VENOMOUS_VIM, SPELLVALUE_BASE_POINT0, 10, caster, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_venomous_wounds::CheckProc);
        OnProc += AuraProcFn(spell_rog_venomous_wounds::HandleProc);
    }
};

// 1943 - Rupture
class spell_rog_rupture : public AuraScript
{
    PrepareAuraScript(spell_rog_rupture);

    void CalcAmount(AuraEffect const*, float& amount, bool&)
    {
        Unit* caster = GetCaster();
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (!player)
            return;

        int32 ap = player->GetTotalAttackPowerValue(BASE_ATTACK);
        float cpmod = 0.0f;

        switch (player->GetComboPoints())
        {
            case 1: cpmod = 0.025f; break;
            case 2: cpmod = 0.04f;  break;
            case 3: cpmod = 0.05f;  break;
            case 4: cpmod = 0.056f; break;
            case 5: cpmod = 0.062f; break;
            default:
                return;
        }
        amount += (ap * cpmod);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (caster && caster->HasAura(SPELL_ROGUE_VENOMOUS_WOUNDS))
        {
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
            {
                int32 energy = 5 * GetAura()->GetDuration() / 1000; // 5 for second
                caster->CastCustomSpell(SPELL_ROGUE_VENOMOUS_VIM, SPELLVALUE_BASE_POINT0, energy, caster, true);
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_rupture::CalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_rupture::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 84654 - Bandit's Guile
class spell_rog_bandits_guile : public AuraScript
{
    PrepareAuraScript(spell_rog_bandits_guile);

    uint32 m_counter = 0;

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() ||
            (eventInfo.GetSpellInfo()->Id != SPELL_ROGUE_REVEALING_STRIKE && eventInfo.GetSpellInfo()->Id != SPELL_ROGUE_SINISTER_STRIKE))
            return false;

        return !GetUnitOwner()->HasAura(84747); // Deep
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        ++m_counter;

        Unit* rogue = GetUnitOwner();

        uint32 level = 0;
        if (rogue->HasAura(84745))         // Shallow
            level = 1;
        else if (rogue->HasAura(84746))    // Moderate
            level = 2;

        if (m_counter == 4)
        {
            m_counter = 0;

            if (level)
                rogue->RemoveAurasDueToSpell(84745 + level - 1);

            rogue->CastSpell(rogue, 84745 + level, true);
            rogue->CastCustomSpell(84748, SPELLVALUE_BASE_POINT0, (level + 1) * 10, rogue, true);
        }
        else if (level)
        {
            rogue->CastSpell(rogue, 84745 + level - 1, true);
            rogue->CastCustomSpell(84748, SPELLVALUE_BASE_POINT0, level * 10, rogue, true);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_bandits_guile::CheckProc);
        OnProc += AuraProcFn(spell_rog_bandits_guile::HandleProc);
    }
};

// 79096 - Restless Blades
class spell_rog_restless_blades : public AuraScript
{
    PrepareAuraScript(spell_rog_restless_blades);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo&)
    {
        if (Player* rogue = GetUnitOwner()->ToPlayer())
        {
            int32 cooldownReduction = aurEff->GetAmount() * rogue->GetComboPoints();

            for (auto&& it : { 13750, 51690, 73981, 121471, 2983 })
                rogue->ModifySpellCooldown(it, -cooldownReduction);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_restless_blades::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 14181 - Relentless Strikes
class spell_rog_relentless_strikes : public SpellScript
{
    PrepareSpellScript(spell_rog_relentless_strikes);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_ROG_RELENTLESS_STRIKES_ENERGIZE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_relentless_strikes::HandleHit);
    }
};

// 121411 - Crimson Tempest, 51723 - Fan of Knives  
class spell_rog_subterfuge_cast_trigger : public SpellScript
{
    PrepareSpellScript(spell_rog_subterfuge_cast_trigger);

    void ActivateSubterfuge()
    {
        Unit* rogue = GetCaster();
        if (rogue->HasAura(SPELL_ROGUE_SUBTERFUGE_STEALTH) || rogue->HasAura(SPELL_ROGUE_SUBTERFUGE_VANISH))
            if (!rogue->HasAura(SPELL_ROGUE_SUBTERFUGE))
                rogue->CastSpell(rogue, SPELL_ROGUE_SUBTERFUGE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_rog_subterfuge_cast_trigger::ActivateSubterfuge);
    }
};

// 2094 - Blind
class spell_rog_blind : public SpellScript
{
    PrepareSpellScript(spell_rog_blind);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_ROG_GLYPH_OF_BLIND))
        {
            Unit* target = GetHitUnit();
            bool dirtyTricks = GetCaster()->HasAura(SPELL_ROGUE_DIRTY_TRICKS);
            for (auto&& type : { SPELL_AURA_PERIODIC_DAMAGE, SPELL_AURA_PERIODIC_DAMAGE_PERCENT, SPELL_AURA_PERIODIC_LEECH })
            {
                auto& auras = target->GetAuraEffectsByType(type);
                for (auto itr = auras.begin(); itr != auras.end();)
                {
                    auto spellInfo = (*itr)->GetSpellInfo();
                    auto auraApp = (*itr)->GetBase()->GetApplicationOfTarget(target->GetGUID());
                    if (auraApp->IsPositive() || spellInfo->Id == 32409) // SW:D shall not be removed.
                    {
                        ++itr;
                        continue;
                    }

                    if (dirtyTricks && (*itr)->GetCasterGUID() == GetCaster()->GetGUID())
                    {
                        if (spellInfo->Dispel == DISPEL_POISON || spellInfo->GetAllEffectsMechanicMask() & (1 << MECHANIC_BLEED) ||
                            spellInfo->Id == SPELL_ROGUE_VENOMOUS_WOUND)
                        {
                            ++itr;
                            continue;
                        }
                    }
                    target->RemoveAura(auraApp->GetBase());
                    itr = auras.begin();
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_blind::HandleHit);
    }
};

// 26679 - Deadly Throw 
class spell_rog_deadly_throw : public SpellScript
{
    PrepareSpellScript(spell_rog_deadly_throw);

    void HandleHit()
    {
        if (Player* rogue = GetCaster()->ToPlayer())
            if (rogue->GetComboPoints() >= 3)
                rogue->CastSpell(GetHitUnit(), SPELL_ROG_DEADLY_THROW_SILENCE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_deadly_throw::HandleHit);
    }
};

// 73981 - Redirect and 110730 - Redirect
class spell_rog_redirect : public SpellScript
{
    PrepareSpellScript(spell_rog_redirect);

    SpellCastResult CheckCast()
    {
        Player* rogue = GetCaster()->ToPlayer();
        if (!rogue)
            return SPELL_FAILED_DONT_REPORT;
        if (!rogue->GetComboPoints())
            return SPELL_FAILED_NO_COMBO_POINTS;
        if (!rogue->GetComboTarget() || (rogue->GetComboTarget() && rogue->GetComboTarget()->GetGUID() == GetSpell()->m_targets.GetUnitTargetGUID()))
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleOnHit()
    {
        Player* rogue = GetCaster()->ToPlayer();
        if (!rogue)
            return;

        uint8 cp = rogue->GetComboPoints();
        rogue->ClearComboPoints();
        rogue->AddComboPoints(GetHitUnit(), cp, GetSpell());
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_redirect::CheckCast);
        OnHit += SpellHitFn(spell_rog_redirect::HandleOnHit);
    }
};

// 57934 - Tricks of the Trade
class spell_rog_tricks_of_the_trade : public AuraScript
{
    PrepareAuraScript(spell_rog_tricks_of_the_trade);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* rogue = GetUnitOwner();
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
            rogue->ResetRedirectThreat();
        else
        {
            rogue->CastSpell(rogue, SPELL_ROG_TRICKS_OF_THE_TRADE_DUMMY, true);
            if (Unit* target = rogue->GetRedirectThreatTarget())
                rogue->CastSpell(target, SPELL_ROG_TRICKS_OF_THE_TRADE_DAMAGE_MOD, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_tricks_of_the_trade::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 59628 - Tricks of the Trade
class spell_rog_tricks_of_the_trade_dummy : public AuraScript
{
    PrepareAuraScript(spell_rog_tricks_of_the_trade_dummy);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->ResetRedirectThreat();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_tricks_of_the_trade_dummy::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 112961 - Leeching Poison
class spell_rog_leeching_poison : public AuraScript
{
    PrepareAuraScript(spell_rog_leeching_poison);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor()->GetGUID() != GetCasterGUID())
            return;

        int32 heal = CalculatePct(eventInfo.GetDamageWithoutResilience(), eff->GetAmount());
        if (!heal)
            return;

        Unit* rogue = eventInfo.GetActor();
        rogue->CastCustomSpell(SPELL_ROG_LEECHING_POISON_HEAL, SPELLVALUE_BASE_POINT0, heal, rogue, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_leeching_poison::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Recuperate - 73651
class spell_rog_recuperate : public AuraScript
{
    PrepareAuraScript(spell_rog_recuperate);

    void InitEffects(uint32& effectMask)
    {
        if (!GetUnitOwner()->HasAura(SPELL_ROG_GLYPH_OF_RECOVERY))
            effectMask &= ~(1 << EFFECT_1);
    }

    void CalculateAmount(AuraEffect const*, float& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        if (AuraEffect const* eff = GetUnitOwner()->GetAuraEffect(SPELL_ROG_GLYPH_OF_RECUPERATE, EFFECT_0))
            amount += (eff->GetAmount() / 1000);
        amount = GetUnitOwner()->CountPctFromMaxHealth(amount);
    }

    void TickCorrection(AuraEffect const*, int32& heal)
    {
        if (!HasEffect(EFFECT_1))
            return;

        // Exclude Recuperate itself
        if (AuraEffect const* eff = GetUnitOwner()->GetAuraEffect(SPELL_ROG_GLYPH_OF_RECOVERY, EFFECT_0))
            heal /= (1 + eff->GetAmount() / 100.f);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_rog_recuperate::InitEffects);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_recuperate::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_rog_recuperate::TickCorrection, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 121471 - Shadow Blades
class spell_rog_shadow_blades : public AuraScript
{
    PrepareAuraScript(spell_rog_shadow_blades);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetTarget()->HasAura(SPELL_ROGUE_SHURIKEN_TOSS_AURA))
            return;
        GetTarget()->SetAutoattackOverrideSpell(eff->GetSpellEffectInfo().TriggerSpell, eff->GetSpellEffectInfo().MiscValue);
        if (AuraEffect const* eff = GetTarget()->GetAuraEffect(SPELL_ROGUE_T14_4P, EFFECT_0))
        {
            uint32 bonus = eff->GetAmount() * 1000;
            if (GetTarget()->HasAura(SPELL_ROGUE_RESTLESS_BLADES))
                bonus /= 2;
            GetAura()->SetMaxDuration(GetMaxDuration() + bonus);
            GetAura()->RefreshDuration(false);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (!GetTarget()->HasAura(SPELL_ROGUE_SHURIKEN_TOSS_AURA))
            GetTarget()->SetAutoattackOverrideSpell(0, 0);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rog_shadow_blades::HandleApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_2, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_rog_shadow_blades::HandleRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_2, AURA_EFFECT_HANDLE_REAL);
    }
};

// 79140 - Vendetta
class spell_rog_vendetta : public AuraScript
{
    PrepareAuraScript(spell_rog_vendetta);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* rogue = GetCaster())
            rogue->BindAura(GetAura());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rog_vendetta::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_FROM_CASTER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 84601 - Assassin's Resolve (10 lvl, rogue has it 100%)
class spell_rog_assasination_respec : public AuraScript
{
    PrepareAuraScript(spell_rog_assasination_respec);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* rogue = GetCaster())
            if (auto* list = rogue->GetBoundAurasBySpellId(SPELL_ROGUE_VENDETTA))
                list->back()->Remove();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_rog_assasination_respec::HandleRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_MAX_POWER_FLAT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 32645 - Envenom
class spell_rog_envemon : public SpellScript
{
    PrepareSpellScript(spell_rog_envemon);

    void CalculateDamage(SpellEffIndex)
    {
        if (Player* rogue = GetCaster()->ToPlayer())
        {
            int32 dmg = (rogue->GetTotalAttackPowerValue(BASE_ATTACK) * 0.112f + GetEffectValue()) * rogue->GetComboPoints();
            SetEffectValue(dmg);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_rog_envemon::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 14185 - Preparation
class spell_rog_preparation : public SpellScript
{
    PrepareSpellScript(spell_rog_preparation);

    void HandleCast()
    {
        if (Player* rogue = GetCaster()->ToPlayer())
        {
            rogue->GetSpellHistory()->RemoveCooldowns([](SpellHistory::CooldownEntry const& entry)
            {
                switch (entry.SpellId)
                {
                    case 1856:  // Vanish
                    case 2983:  // Sprint
                    case 5277:  // Evasion
                    case 51722: // Dismantle
                        return true;
                }
                return false;
            });
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_rog_preparation::HandleCast);
    }
};

// 63254 - Glyph of Deadly Momentum
class spell_rog_glyph_of_deadly_momentum : public AuraScript
{
    PrepareAuraScript(spell_rog_glyph_of_deadly_momentum);

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        if (Aura* snd = GetUnitOwner()->GetAura(SPELL_ROGUE_SLICE_AND_DICE))
            snd->RefreshDuration();
        if (Aura* rec = GetUnitOwner()->GetAura(SPELL_ROGUE_RECUPERATE))
            rec->RefreshDuration();
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_glyph_of_deadly_momentum::HandleProc);
    }
};

// 1943 - Rupture
// 32645 - Envenom
// 73651 - Recuperate
class spell_rog_t15_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_rog_t15_2p_bonus);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCaster()->HasAura(SPELL_ROGUE_T15_2P_BONUS);
    }

    void HanldeHit()
    {
        int32 minDuration = GetSpellInfo()->GetDuration();
        int32 maxDuratiron = GetSpellInfo()->GetMaxDuration();
        if (minDuration == -1 || maxDuratiron == -1)
            return;

        Player* rouge = GetCaster()->ToPlayer();

        int32 duration = minDuration + (maxDuratiron - minDuration) * (GetCaster()->ToPlayer()->GetComboPoints() + 1) / 5;

        rouge->ApplySpellMod(GetSpellInfo()->Id, SPELLMOD_DURATION, duration);

        if (Aura* aura = GetHitAura())
        {
            aura->SetMaxDuration(duration);
            aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_t15_2p_bonus::HanldeHit);
    }
};

// 145185 - Item - Rogue T16 2P Bonus
class spell_rog_t16_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_rog_t16_2p_bonus);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    int32 GetAmount()
    {
        switch (GetUnitOwner()->ToPlayer()->GetSpecialization())
        {
            case SPEC_ROGUE_SUBTLETY:
                return -2;
            case SPEC_ROGUE_ASSASSINATION:
                return -6;
            default:
                return -15;
        }
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        GetUnitOwner()->CastCustomSpell(SPELL_ROGUE_SILENT_BLADES, SPELLVALUE_BASE_POINT0, GetAmount(), GetUnitOwner(), true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_t16_2p_bonus::HandleProc);
    }
};

// 145193 - Silent Blades
class spell_rog_silent_blades : public AuraScript
{
    PrepareAuraScript(spell_rog_silent_blades);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        Spell const* bySpell = eventInfo.TriggeredBySpell();
        if (!bySpell)
            return;

        if (!effect->IsAffectingSpell(bySpell->GetSpellInfo()))
            return;

        if (bySpell->m_appliedMods.find(GetAura()) == bySpell->m_appliedMods.end())
            return;

        if (Aura* aura = GetUnitOwner()->GetAura(SPELL_ROGUE_BLINDSIDE))
            if (bySpell->m_appliedMods.find(aura) != bySpell->m_appliedMods.end())
                return;

        Remove();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_silent_blades::HandleProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 145210 - Item - Rogue T16 4P Bonus
class spell_rog_t16_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_rog_t16_4p_bonus);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetActionTarget())
            return;

        Player* rogue = GetUnitOwner()->ToPlayer();
        if (rogue->GetSpecialization() == SPEC_ROGUE_ASSASSINATION)
        {
            if (eventInfo.GetActionTarget()->HasAura(SPELL_ROGUE_VENDETTA))
                rogue->CastSpell(rogue, SPELL_ROGUE_TOXICOLOGIST, true);
        }
        else if (rogue->GetSpecialization() == SPEC_ROGUE_SUBTLETY)
        {
            if (eventInfo.GetSpellInfo()->Id == SPELL_ROGUE_BACKSTAB && roll_chance_f(GetSpellInfo()->Effects[EFFECT_2].BasePoints))
                rogue->CastSpell(rogue, SPELL_ROGUE_SLEIGHT_OF_HAND, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_rog_t16_4p_bonus::HandleProc);
    }
};

// 57841 - Killing Spree
// 57842 - Killing Spree Off-Hand
class spell_rog_t16_4p_bonus_killing_spree : public SpellScript
{
    PrepareSpellScript(spell_rog_t16_4p_bonus_killing_spree);

    void HandleHit()
    {
        if (AuraEffect const* bonus = GetCaster()->GetAuraEffect(SPELL_ROGUE_T16_4P_BONUS, EFFECT_0))
        {
            if (AuraEffect const* killingSpreePeriodic = GetCaster()->GetAuraEffect(SPELL_ROGUE_KILLING_SPREE, EFFECT_0))
            {
                int32 damage = GetHitDamage();
                int32 pct = killingSpreePeriodic->GetTickNumber() * bonus->GetAmount();
                AddPct(damage, pct);
                SetHitDamage(damage);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_t16_4p_bonus_killing_spree::HandleHit);
    }
};

// 1776 - Gouge
class spell_rog_glyph_of_gouge : public SpellScript
{
    PrepareSpellScript(spell_rog_glyph_of_gouge);

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;
        // Target must be facing you
        if (!GetCaster()->HasAura(SPELL_ROGUE_GLYPH_OF_GOUGE) && !target->HasInArc(static_cast<float>(M_PI), GetCaster()))
            return SPELL_FAILED_NOT_INFRONT;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_glyph_of_gouge::CheckCast);
    }
};

// 1776 - Gouge
// 2094 - Blind
class spell_rog_dirty_tricks : public AuraScript
{
    PrepareAuraScript(spell_rog_dirty_tricks);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* rogue = GetCaster();
        if (rogue && rogue == eventInfo.GetActor())
        {
            if (eventInfo.GetSpellInfo() && rogue->HasAura(SPELL_ROGUE_DIRTY_TRICKS))
            {
                if (eventInfo.GetSpellInfo()->Dispel == DISPEL_POISON || eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & (1 << MECHANIC_BLEED))
                    return false;
                if (eventInfo.GetSpellInfo()->Id == SPELL_ROGUE_VENOMOUS_WOUND)
                    return false;
            }
        }
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_dirty_tricks::CheckProc);
    }
};

// 116921 - Leech Vitality
// 112974 - Leeching Poison
class spell_rog_glyph_of_recovery_fix : public SpellScript
{
    PrepareSpellScript(spell_rog_glyph_of_recovery_fix);

    void HandleHit()
    {
        if (AuraEffect* effect = GetCaster()->GetAuraEffect(SPELL_ROGUE_RECUPERATE, EFFECT_1))
            SetHitHeal(GetHitHeal() * (1.0f + effect->GetAmount() / 100.0f));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_glyph_of_recovery_fix::HandleHit);
    }
};

// 921 - Pick Pocket
class spell_rog_glyph_of_disguise : public SpellScript
{
    PrepareSpellScript(spell_rog_glyph_of_disguise);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_ROGUE_GLYPH_OF_DISGUISE))
            GetHitUnit()->CastSpell(GetCaster(), SPELL_ROGUE_DISGUISE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_rog_glyph_of_disguise::HandleHit);
    }
};

// 703 - Garrote
// 1833 - Cheap Shot
// 8676 - Ambush
class spell_rog_cloak_and_dagger : public SpellScript
{
    PrepareSpellScript(spell_rog_cloak_and_dagger);

    SpellCastResult CheckCast()
    {
        if (!GetExplTargetUnit())
            return SPELL_FAILED_DONT_REPORT;

        if (GetCaster()->HasAura(SPELL_ROGUE_CLOAK_AND_DAGGER))
        {
            if (GetCaster()->HasAuraType(SPELL_AURA_MOD_ROOT))
            {
                if (!GetCaster()->IsWithinMeleeRange(GetExplTargetUnit()) || !GetCaster()->HasInArc(static_cast<float>(M_PI), GetExplTargetUnit()))
                    return SPELL_FAILED_ROOTED;
                if (GetSpellInfo()->Id == SPELL_ROGUE_AMBUSH && GetExplTargetUnit()->HasInArc(M_PI, GetCaster()))
                    return SPELL_FAILED_ROOTED;
            }
        }
        else if (GetSpellInfo()->Id == SPELL_ROGUE_AMBUSH)
            if (GetExplTargetUnit() && GetExplTargetUnit()->HasInArc(M_PI, GetCaster()))
                return SPELL_FAILED_NOT_BEHIND;

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_ROGUE_CLOAK_AND_DAGGER))
            if (((GetCaster()->HasAura(SPELL_ROGUE_SHADOW_DANCE) || GetCaster()->HasAura(SPELL_ROGUE_SUBTERFUGE_STEALTH)) || !GetCaster()->HasAuraType(SPELL_AURA_MOD_STEALTH)))
                GetCaster()->CastSpell(GetHitUnit(), SPELL_ROGUE_CLOAK_AND_DAGGER_TELEPORT, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_cloak_and_dagger::CheckCast);
        OnHit += SpellHitFn(spell_rog_cloak_and_dagger::HandleHit);
    }
};

// 1766 - Kick
class spell_rog_glyph_of_kick : public SpellScript
{
    PrepareSpellScript(spell_rog_glyph_of_kick);

    void HandleHit()
    {
        if (GetSpell()->IsSuccessfulInterrupt() && GetCaster()->GetTypeId() == TYPEID_PLAYER)
            if (AuraEffect const* glyph = GetCaster()->GetAuraEffect(SPELL_ROGUE_GLYPH_OF_KICK, EFFECT_1))
                GetCaster()->ToPlayer()->ModifySpellCooldown(GetSpellInfo()->Id, -glyph->GetAmount());
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_rog_glyph_of_kick::HandleHit);
    }
};

// 1725 - Distract
class spell_rog_distract : public SpellScript
{
    PrepareSpellScript(spell_rog_distract);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
        {
            auto check = [this](Unit* unit)
            {
                return unit->IsInDist(GetExplTargetDest(), 10.0f) && GetCaster()->IsValidAttackTarget(unit) && !unit->IsTotem();
            };
            std::list<Unit*> list;
            Trinity::UnitListSearcher<decltype(check)> searcher{ GetCaster(), list, check };
            GetCaster()->VisitNearbyObject(100.0f, searcher);

            if (list.empty())
                return;

            for (auto&& itr : list)
                if (!itr->IsInCombat())
                    return;

            FinishCast(SPELL_FAILED_TARGET_AFFECTING_COMBAT);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_distract::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 5171 - Slice and Dice
// 73651 - Recuperate
class spell_rog_duration_check : public SpellScript
{
    PrepareSpellScript(spell_rog_duration_check);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->ToPlayer())
        {
            if (Aura* existing = GetCaster()->GetAura(GetSpellInfo()->Id))
            {
                if (existing->GetDuration() > GetCaster()->CalcSpellDuration(GetSpellInfo()))
                    return SPELL_FAILED_STRONGER_SPELL_IN_PROGRESS;
            }
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_duration_check::CheckCast);
    }
};

// 36554 - Shadowstep
class spell_rog_shadowstep : public SpellScript
{
    PrepareSpellScript(spell_rog_shadowstep);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasUnitState(UNIT_STATE_ROOT))
            return SPELL_FAILED_ROOTED;
        if (GetExplTargetUnit() && GetExplTargetUnit()->GetEntry() == 27894) // Antipersonnel Cannon (Strand of the Ancients)
            return SPELL_FAILED_BAD_TARGETS;
        if (GetExplTargetUnit() == GetCaster()) // When "auto self cast" enabled client chose self...
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_shadowstep::CheckCast);
    }
};

void AddSC_rogue_spell_scripts()
{
    new spell_rog_glyph_of_expose_armor();
    new spell_script<spell_rog_growl>("spell_rog_growl");
    new spell_rog_combat_readiness();
    new aura_script<spell_rog_nerve_strike>("spell_rog_nerve_strike");
    new aura_script<spell_rog_nerve_strike_effect>("spell_rog_nerve_strike_effect");
    new spell_script<spell_rog_shroud_of_concealment>("spell_rog_shroud_of_concealment");
    new spell_rog_master_poisoner();
    new spell_script<spell_rog_slice_and_dice>("spell_rog_slice_and_dice");
    new spell_rog_deadly_poison_instant_damage();
    new aura_script<spell_rog_paralytic_poison>("spell_rog_paralytic_poison");
    new spell_rog_shiv();
    new spell_rog_poisons();
    new spell_rog_deadly_poison();
    new spell_rog_marked_for_death();
    new aura_script<spell_rog_smoke_bomb>("spell_rog_smoke_bomb");
    new spell_script<spell_rog_glyph_of_decoy>("spell_rog_glyph_of_decoy");
    new aura_script<spell_rog_killing_spree>("spell_rog_killing_spree");
    new spell_script<spell_rog_killing_spree_target_selector>("spell_rog_killing_spree_target_selector");
    new spell_script<spell_rog_killing_spree_teleport>("spell_rog_killing_spree_teleport");
    new spell_script<spell_rog_ambush>("spell_rog_ambush");
    new aura_script<spell_rog_main_gauch>("spell_rog_main_gauch");
    new aura_script<spell_rog_combat_potency>("spell_rog_combat_potency");
    new aura_script<spell_rog_sanguinary_vein>("spell_rog_sanguinary_vein");
    new spell_script<spell_rog_sinister_strike>("spell_rog_sinister_strike");
    new aura_script<spell_rog_kidney_shot>("spell_rog_kidney_shot");
    new spell_script<spell_rog_crimson_tempest>("spell_rog_crimson_tempest");
    new aura_script<spell_rog_blade_flurry>("spell_rog_blade_flurry");
    new spell_script<spell_rog_blade_flurry_damage>("spell_rog_blade_flurry_damage");
    new aura_script<spell_rog_poison_proc>("spell_rog_poison_proc");
    new aura_script<spell_rog_cheat_death>("spell_rog_cheat_death");
    new spell_script<spell_rog_shuriken_toss>("spell_rog_shuriken_toss");
    new aura_script<spell_rog_shuriken_toss_aura>("spell_rog_shuriken_toss_aura");
    new aura_script<spell_rog_seal_fate>("spell_rog_seal_fate");
    new spell_script<spell_rog_mutilate>("spell_rog_mutilate");
    new aura_script<spell_rog_anticipation>("spell_rog_anticipation");
    new aura_script<spell_rog_honor_among_thieves>("spell_rog_honor_among_thieves");
    new aura_script<spell_rog_cut_to_the_chase>("spell_rog_cut_to_the_chase");
    new spell_script<spell_rog_hemorrhage>("spell_rog_hemorrhage");
    new spell_script<spell_rog_fan_of_knives>("spell_rog_fan_of_knives");
    new spell_script<spell_rog_vanish_initial>("spell_rog_vanish_initial");
    new spell_script<spell_rog_vanish_trigger>("spell_rog_vanish_trigger");
    new aura_script<spell_rog_stealth>("spell_rog_stealth");
    new aura_script<spell_rog_stealth_subterfuge>("spell_rog_stealth_subterfuge");
    new aura_script<spell_rog_vanish>("spell_rog_vanish");
    new aura_script<spell_rog_venomous_wounds>("spell_rog_venomous_wounds");
    new aura_script<spell_rog_rupture>("spell_rog_rupture");
    new aura_script<spell_rog_bandits_guile>("spell_rog_bandits_guile");
    new aura_script<spell_rog_restless_blades>("spell_rog_restless_blades");
    new spell_script<spell_rog_relentless_strikes>("spell_rog_relentless_strikes");
    new spell_script<spell_rog_subterfuge_cast_trigger>("spell_rog_subterfuge_cast_trigger");
    new spell_script<spell_rog_blind>("spell_rog_blind");
    new spell_script<spell_rog_deadly_throw>("spell_rog_deadly_throw");
    new spell_script<spell_rog_redirect>("spell_rog_redirect");
    new aura_script<spell_rog_tricks_of_the_trade>("spell_rog_tricks_of_the_trade");
    new aura_script<spell_rog_tricks_of_the_trade_dummy>("spell_rog_tricks_of_the_trade_dummy");
    new aura_script<spell_rog_leeching_poison>("spell_rog_leeching_poison");
    new aura_script<spell_rog_recuperate>("spell_rog_recuperate");
    new aura_script<spell_rog_shadow_blades>("spell_rog_shadow_blades");
    new aura_script<spell_rog_vendetta>("spell_rog_vendetta");
    new aura_script<spell_rog_assasination_respec>("spell_rog_assasination_respec");
    new spell_script<spell_rog_envemon>("spell_rog_envemon");
    new spell_script<spell_rog_preparation>("spell_rog_preparation");
    new aura_script<spell_rog_glyph_of_deadly_momentum>("spell_rog_glyph_of_deadly_momentum");
    new spell_script<spell_rog_t15_2p_bonus>("spell_rog_t15_2p_bonus");
    new aura_script<spell_rog_t16_2p_bonus>("spell_rog_t16_2p_bonus");
    new aura_script<spell_rog_silent_blades>("spell_rog_silent_blades");
    new aura_script<spell_rog_t16_4p_bonus>("spell_rog_t16_4p_bonus");
    new spell_script<spell_rog_t16_4p_bonus_killing_spree>("spell_rog_t16_4p_bonus_killing_spree");
    new spell_script<spell_rog_glyph_of_gouge>("spell_rog_glyph_of_gouge");
    new aura_script<spell_rog_dirty_tricks>("spell_rog_dirty_tricks");
    new spell_script<spell_rog_glyph_of_recovery_fix>("spell_rog_glyph_of_recovery_fix");
    new spell_script<spell_rog_glyph_of_disguise>("spell_rog_glyph_of_disguise");
    new spell_script<spell_rog_cloak_and_dagger>("spell_rog_cloak_and_dagger");
    new spell_script<spell_rog_glyph_of_kick>("spell_rog_glyph_of_kick");
    new spell_script<spell_rog_distract>("spell_rog_distract");
    new spell_script<spell_rog_duration_check>("spell_rog_duration_check");
    new spell_script<spell_rog_shadowstep>("spell_rog_shadowstep");
}
