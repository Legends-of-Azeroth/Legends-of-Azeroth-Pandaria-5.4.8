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

/*
 * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pal_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "spell_common.h"
#include "PetAI.h"
#include "Random.h"

enum PaladinSpells
{
    PALADIN_SPELL_DIVINE_PLEA                    = 54428,
    PALADIN_SPELL_JUDGMENT                       = 20271,
    PALADIN_SPELL_JUDGMENTS_OF_THE_BOLD          = 111529,
    PALADIN_SPELL_JUDGMENTS_OF_THE_WISE          = 105424,
    PALADIN_SPELL_PHYSICAL_VULNERABILITY         = 81326,
    PALADIN_SPELL_LONG_ARM_OF_THE_LAW            = 87172,
    PALADIN_SPELL_LONG_ARM_OF_THE_LAW_RUN_SPEED  = 87173,
    PALADIN_SPELL_BURDEN_OF_GUILT                = 54931,
    PALADIN_SPELL_BURDEN_OF_GUILT_DECREASE_SPEED = 110300,
    PALADIN_SPELL_HOLY_SHOCK_R1                  = 20473,
    PALADIN_SPELL_HOLY_SHOCK_R1_DAMAGE           = 25912,
    PALADIN_SPELL_HOLY_SHOCK_R1_HEALING          = 25914,
    SPELL_BLESSING_OF_LOWER_CITY_DRUID           = 37878,
    SPELL_BLESSING_OF_LOWER_CITY_PALADIN         = 37879,
    SPELL_BLESSING_OF_LOWER_CITY_PRIEST          = 37880,
    SPELL_BLESSING_OF_LOWER_CITY_SHAMAN          = 37881,
    SPELL_DIVINE_STORM                           = 53385,
    SPELL_DIVINE_STORM_DUMMY                     = 54171,
    SPELL_DIVINE_STORM_HEAL                      = 54172,
    SPELL_PALADIN_FORBEARANCE                    = 25771,
    PALADIN_SPELL_WORD_OF_GLORY                  = 85673,
    SPELL_PALADIN_WORD_OF_GLORY_HEAL             = 130551,
    SPELL_PALADIN_WORD_OF_GLORY_DAMAGE           = 130552,
    SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY         = 54936,
    SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY_EFFECT  = 115522,
    SPELL_PALADIN_CONSECRATION_DAMAGE            = 81297,
    SPELL_PALADIN_HOLY_PRISM_ALLY                = 114871,
    SPELL_PALADIN_HOLY_PRISM_ENEMY               = 114852,
    SPELL_PALADIN_HOLY_PRISM_DAMAGE_VISUAL       = 114862,
    SPELL_PALADIN_HOLY_PRISM_DAMAGE_VISUAL_AOE   = 121551,
    SPELL_PALADIN_HOLY_PRISM_HEAL_VISUAL         = 114870,
    SPELL_PALADIN_HOLY_PRISM_HEAL_VISUAL_AOE     = 121552,
    SPELL_PAL_ARCING_LIGHT_HEAL                  = 119952,
    SPELL_PAL_ARCING_LIGHT_DAMAGE                = 114919,
    SPELL_PALADIN_INQUISITION                    = 84963,
    PALADIN_SPELL_GLYPH_OF_BLINDING_LIGHT        = 54934,
    PALADIN_SPELL_BLINDING_LIGHT_CONFUSE         = 105421,
    PALADIN_SPELL_BLINDING_LIGHT_STUN            = 115752,
    PALADIN_SPELL_EXORCISM                       = 879,
    PALADIN_SPELL_MASS_EXORCISM                  = 122032,
    PALADIN_SPELL_SACRED_SHIELD                  = 65148,
    PALADIN_SPELL_ARDENT_DEFENDER_HEAL           = 66235,
    PALADIN_SPELL_TOWER_OF_RADIANCE_ENERGIZE     = 88852,
    PALADIN_SPELL_BEACON_OF_LIGHT                = 53563,
    SPELL_PALADIN_SELFLESS_HEALER                = 114250,
    SPELL_PALADIN_SELFLESS_HEALER_UI             = 128863,
    SPELL_PALADIN_ETERNAL_FLAME                  = 114163,
    SPELL_PALADIN_SHIELD_OF_THE_RIGHTEOUS_PROC   = 132403,
    SPELL_PALADIN_DIVINE_PURPOSE                 = 90174,
    SPELL_PALADIN_DIVINE_PURPOSE_TALENT          = 86172,
    PALADIN_SPELL_DIVINE_SHIELD                  = 642,
    PALADIN_SPELL_LAY_ON_HANDS                   = 633,
    PALADIN_SPELL_DIVINE_PROTECTION              = 498,
    PALADIN_SPELL_GLYPH_OF_FALLING_AVENGER       = 115931,
    PALADIN_SPELL_SACRED_CLEANSING               = 53551,
    PALADIN_SPELL_DAYBREAK_AURA                  = 88821,
    PALADIN_SPELL_DAYBREAK_PROC                  = 88819,
    PALADIN_SPELL_DAYBREAK_HEAL                  = 121129,
    PALADIN_SPELL_TOWER_OF_RADIANCE              = 85512,
    PALADIN_SPELL_HOLY_AVENGER                   = 105809,
    SPELL_PALADIN_PVP_HOLY_4P_BONUS              = 131665,
    PALADIN_SPELL_GLYPH_OF_DIVINE_STORM          = 63220,
    PALADIN_SPELL_GLYPH_OF_DIVINE_STORM_HEAL     = 115515,
    SPELL_PALADIN_AVENGERS_SHIELD                = 31935,
    SPELL_PAL_EXECUTION_SENTENCE                 = 114916,
    SPELL_PAL_STAY_OF_EXECUTION                  = 114917,
    SPELL_PAL_EXECUTION_SENTENCE_DISPEL_DAMAGE   = 146585,
    SPELL_PAL_EXECUTION_SENTENCE_DISPEL_HEAL     = 146586,
    SPELL_PALADIN_LIGHT_OF_THE_ANCIENT_KINGS     = 86678,
    SPELL_PALADIN_DAYBREAK                       = 121129,
    SPELL_PALADIN_HAND_OF_LIGHT                  = 96172,
    SPELL_PALADIN_GLYPH_OF_FINAL_WRATH           = 54935,
    SEPLL_PALADIN_GLYPH_OF_HOLY_WRATH            = 54923,
    SPELL_PALADIN_GLYPH_OF_DIVINITY              = 54939,
    SPELL_PALADIN_GLYPH_OF_DIVINITY_ENERGIZE     = 54986,
    SPELL_PALADIN_GLYPH_OF_HAMMER_OF_THE_RIGHTEOUS = 63219,
    SPELL_PALADIN_BASTION_OF_GLORY               = 114637,
    SPELL_PALADIN_FLASH_OF_LIGHT                 = 19750,
    SPELL_PALADIN_PROTECTOR_OF_THE_INNOCENT_HEAL = 115536,
    SPELL_PALADIN_ILLUMINATED_HEALING            = 86273,
    SPELL_PALADIN_BEACON_OF_LIGHT                = 53563,
    SPELL_PALADIN_BEACON_OF_LIGHT_HEAL           = 53652,
    SPELL_PALADIN_GLYPH_OF_DIVINE_SHIELD         = 146956,
    SPELL_PALADIN_GLYPH_OF_LIGHT_OF_DAWN         = 54940,
    SPELL_PALADIN_GLYPH_OF_THE_BATTLE_HEALER     = 119477,
    SPELL_PALADIN_HOLY_INSIGHT                   = 112859,
    SPELL_PALADIN_ANCIENT_POWER                  = 86700,
    SPELL_PALADIN_ANCIENT_FURY                   = 86704,
    SPELL_PALADIN_PVP_HOLY_P4_BONUS              = 131665,
    SPELL_PALADIN_PVP_HOLY_P14_ENERGIZE          = 141462,
    SPELL_PALADIN_JUDMENT                        = 20271,
    SPELL_PALADIN_HAMMER_OF_WRATH                = 24275,
    SPELL_PALADIN_GLYPH_OF_THE_FALLING_AVENGER   = 115931,
    SPELL_PALADIN_GLYPH_OF_DOUBLE_JEOPARDY       = 121027,
    SPELL_PALADIN_GLYPH_OF_RIGHTEOUS_RETREAT     = 115933,
    SPELL_PALADIN_T15_HOLY_2P_BONUS              = 138291,
    SPELL_PALADIN_T15_HOLY_4P_BONUS              = 138292,
    SPELL_PALADIN_T15_RETRIBUTION_2P_EXORCISM    = 138162,
    SPELL_PALADIN_T15_RETRIBUTION_4P_BONUS       = 138164,
    SPELL_PALADIN_TEMPLARS_VERDICT_OVERRIDE      = 138169,
    SPELL_PALADIN_T15_PROTECTION_2P_BONUS        = 138238,
    SPELL_PALADIN_T16_RETRIBUTION_2P_BONUS       = 144586,
    SPELL_PALADIN_T16_RETRIBUTION_4P_BONUS       = 144593,
    SPELL_PALADIN_BASTION_OF_POWER               = 144569,
    SPELL_PALADIN_DIVINE_CRUSADER                = 144595,
    SPELL_PALADIN_BLESSING_OF_THE_GUARDIANS      = 144581,
    SPELL_PALADIN_T16_PROTECTION_2P_BONUS        = 144580,
    SPELL_PALADIN_T16_PROTECTION_4P_BONUS        = 144566, 
    SPELL_PALADIN_SHIELD_OF_GLORY                = 138242,
    SPELL_PALADIN_HOLY_POWER_ENERGIZE            = 138248,
    SPELL_PALADIN_CENSURE                        = 31803,
    SPELL_PALADIN_SEAL_OF_TRUTH_PROC             = 42463,
    SPELL_PALADIN_EXORCISM_ENERGIZE              = 147715,
    SPELL_PALADIN_GLYPH_OF_DIVINE_PLEA           = 63223,
    SPELL_PALADIN_GLYPH_OF_HAND_OF_SACRIFICE     = 146957,
    SPELL_PALADIN_SWORD_OF_LIGHT_DAMAGE_BONUS    = 20113,
    SPELL_PALADIN_PURSUIT_OF_JUSTICE             = 114695,
    SPELL_PALADIN_GLYPH_OF_FOCUSED_WRATH         = 115738,
    SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_SWORD= 127755,
    SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_AXE  = 127756,
    SPELL_PALADIN_GLYPH_OF_THE_MOUNTED_KING      = 57958,
    SPELL_PALADIN_BLESSING_OF_KINGS              = 20217,
    PALADIN_SPELL_GLYPH_OF_CONTEMPLATION         = 121183,

    SPELL_DRUID_CONSECRATION_DAMAGE              = 110705,

    SPELL_WEAKENED_BLOWS                         = 115798,
};

class spell_pal_glyph_of_devotian_aura : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_devotian_aura() : SpellScriptLoader("spell_pal_glyph_of_devotian_aura") { }

        class spell_pal_glyph_of_devotian_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_glyph_of_devotian_aura_SpellScript);

            bool Load() override
            {
                return true;
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                if (GetCaster() && GetCaster()->HasAura(146955))
                {
                    targets.clear();
                    targets.push_back(GetCaster());
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_devotian_aura_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_glyph_of_devotian_aura_SpellScript();
        }
};

class spell_pal_glyph_of_devotian_trigger_aura : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_devotian_trigger_aura() : SpellScriptLoader("spell_pal_glyph_of_devotian_trigger_aura") { }

        class spell_pal_glyph_of_devotian_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_glyph_of_devotian_aura_SpellScript);

            bool Load() override
            {
                return true;
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                if (GetCaster() && GetCaster()->HasAura(146955))
                {
                    targets.clear();
                    targets.push_back(GetCaster());
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_devotian_aura_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_devotian_aura_SpellScript::CountTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_devotian_aura_SpellScript::CountTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_glyph_of_devotian_aura_SpellScript();
        }
};

// Called by Divine Storm - 53385
// Glyph of Divine Storm - 63220
class spell_pal_glyph_of_divine_storm : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_divine_storm() : SpellScriptLoader("spell_pal_glyph_of_divine_storm") { }

        class spell_pal_glyph_of_divine_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_glyph_of_divine_storm_SpellScript);

            void HandleAfterCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(PALADIN_SPELL_GLYPH_OF_DIVINE_STORM))
                        _player->CastSpell(_player, PALADIN_SPELL_GLYPH_OF_DIVINE_STORM_HEAL, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pal_glyph_of_divine_storm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_glyph_of_divine_storm_SpellScript();
        }
};

// Hand of Purity - 114039
class spell_pal_hand_of_purity : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_purity() : SpellScriptLoader("spell_pal_hand_of_purity") { }

        class spell_pal_hand_of_purity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_hand_of_purity_AuraScript);

            uint32 healtPct;

            bool Load() override
            {
                healtPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return GetUnitOwner()->ToPlayer();
            }

            void CalculateAmount(AuraEffect const* /*auraEffect*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect * /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                SpellInfo const* procSpell = dmgInfo.GetSpellInfo();
                if (!procSpell)
                    return;

                if (!procSpell->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !procSpell->HasAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT))
                    return;

                absorbAmount = CalculatePct(dmgInfo.GetDamage(), healtPct);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_purity_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_hand_of_purity_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_hand_of_purity_AuraScript();
        }
};

// Avenging Wrath - 31884
// Glyph of Avenging Wrath - 54927
class spell_pal_glyph_of_avenging_wrath : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_avenging_wrath() : SpellScriptLoader("spell_pal_glyph_of_avenging_wrath") { }

        class spell_pal_glyph_of_avenging_wrath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_glyph_of_avenging_wrath_AuraScript)

            void initEffects(uint32& effectMask)
            {
                auto const caster = GetCaster();
                if (!caster || !caster->HasAura(PALADIN_SPELL_GLYPH_OF_FALLING_AVENGER))
                    effectMask &= ~(1 << EFFECT_3);
            }

            void Register() override
            {
                OnInitEffects += AuraInitEffectsFn(spell_pal_glyph_of_avenging_wrath_AuraScript::initEffects);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_glyph_of_avenging_wrath_AuraScript();
        }
};

// 53600 - Shield of the Righteous
class spell_pal_shield_of_the_righteous : public SpellScript
{
    PrepareSpellScript(spell_pal_shield_of_the_righteous);

    void HandleHit()
    {
        // -30% damage taken for 3s
        if (Aura* proc = GetCaster()->GetAura(SPELL_PALADIN_SHIELD_OF_THE_RIGHTEOUS_PROC))
        {
            int32 duration = proc->GetDuration();
            proc->SetDuration(duration + 3 * IN_MILLISECONDS);
            proc->SetMaxDuration(duration + 3 * IN_MILLISECONDS);
        }
        else
            GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_SHIELD_OF_THE_RIGHTEOUS_PROC, true);

        GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_BASTION_OF_GLORY); // Not tirggered bc T16 4P can't proc from triggered
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_shield_of_the_righteous::HandleHit);
    }
};

// Called by Flash of Light - 19750 and Divine Light - 82326
// Tower of Radiance - 85512
class spell_pal_tower_of_radiance : public SpellScript
{
    PrepareSpellScript(spell_pal_tower_of_radiance);

    void HandleHit()
    {
        if (GetCaster()->HasAura(PALADIN_SPELL_TOWER_OF_RADIANCE))
            if (GetHitUnit()->HasAura(PALADIN_SPELL_BEACON_OF_LIGHT, GetCaster()->GetGUID()))
                GetCaster()->CastSpell(GetCaster(), PALADIN_SPELL_TOWER_OF_RADIANCE_ENERGIZE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_tower_of_radiance::HandleHit);
    }
};

// Sacred shield - 20925
class spell_pal_sacred_shield : public SpellScriptLoader
{
    public:
        spell_pal_sacred_shield() : SpellScriptLoader("spell_pal_sacred_shield") { }

        class spell_pal_sacred_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_sacred_shield_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* _player = GetCaster())
                    if (Unit* target = GetTarget())
                            _player->CastSpell(target, PALADIN_SPELL_SACRED_SHIELD, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_sacred_shield_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_sacred_shield_AuraScript();
        }
};

// Sacred shield absorb - 65148
class spell_pal_sacred_shield_absorb : public SpellScriptLoader
{
    public:
        spell_pal_sacred_shield_absorb() : SpellScriptLoader("spell_pal_sacred_shield_absorb") { }

        class spell_pal_sacred_shield_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_sacred_shield_absorb_AuraScript);

            void CalculateAmount(AuraEffect const* , float& amount, bool& )
            {
                if (GetCaster())
                    amount = int32(30 + GetCaster()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY) * 1.17f);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_sacred_shield_absorb_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_sacred_shield_absorb_AuraScript();
        }
};

// Emancipate - 121783
class spell_pal_emancipate : public SpellScriptLoader
{
    public:
        spell_pal_emancipate() : SpellScriptLoader("spell_pal_emancipate") { }

        class spell_pal_emancipate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_emancipate_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    std::list<Aura*> auraList;

                    for (auto itr : _player->GetAppliedAuras())
                    {
                        Aura *aura = itr.second->GetBase();
                        if (aura && aura->GetSpellInfo()->GetAllEffectsMechanicMask() & ((1<<MECHANIC_SNARE)|(1<<MECHANIC_ROOT)))
                            auraList.push_back(aura);
                    }

                    if (!auraList.empty())
                    {
                        Trinity::Containers::RandomResizeList(auraList, 1);
                        _player->RemoveAura(*auraList.begin());
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_emancipate_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_emancipate_SpellScript();
        }
};

// Blinding Light - 115750
class spell_pal_blinding_light : public SpellScriptLoader
{
    public:
        spell_pal_blinding_light() : SpellScriptLoader("spell_pal_blinding_light") { }

        class spell_pal_blinding_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blinding_light_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->HasAura(PALADIN_SPELL_GLYPH_OF_BLINDING_LIGHT))
                            _player->CastSpell(target, PALADIN_SPELL_BLINDING_LIGHT_STUN, true);
                        else
                            _player->CastSpell(target, PALADIN_SPELL_BLINDING_LIGHT_CONFUSE, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_blinding_light_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blinding_light_SpellScript();
        }
};

// Hand of Protection - 1022
class spell_pal_hand_of_protection : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_protection() : SpellScriptLoader("spell_pal_hand_of_protection") { }

        class spell_pal_hand_of_protection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_hand_of_protection_SpellScript);

            SpellCastResult CheckForbearance()
            {
                if (Unit* target = GetExplTargetUnit())
                    if (target->HasAura(SPELL_PALADIN_FORBEARANCE))
                        return SPELL_FAILED_TARGET_AURASTATE;

                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        _player->CastSpell(target, SPELL_PALADIN_FORBEARANCE, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_hand_of_protection_SpellScript::CheckForbearance);
                OnHit += SpellHitFn(spell_pal_hand_of_protection_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_hand_of_protection_SpellScript();
        }
};

// Ardent Defender - 31850
class spell_pal_ardent_defender : public SpellScriptLoader
{
    public:
        spell_pal_ardent_defender() : SpellScriptLoader("spell_pal_ardent_defender") { }

        class spell_pal_ardent_defender_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_ardent_defender_AuraScript);

            uint32 absorbPct, healPct;

            bool Load() override
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
                return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect *aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* victim = GetTarget();
                int32 remainingHealth = victim->GetHealth() - dmgInfo.GetDamage();
                // If damage kills us
                if (remainingHealth <= 0 && !victim->ToPlayer()->HasSpellCooldown(PALADIN_SPELL_ARDENT_DEFENDER_HEAL))
                {
                    // Cast healing spell, completely avoid damage
                    absorbAmount = dmgInfo.GetDamage();

                    int32 healAmount = int32(victim->CountPctFromMaxHealth(healPct));
                    victim->CastCustomSpell(victim, PALADIN_SPELL_ARDENT_DEFENDER_HEAL, &healAmount, NULL, NULL, true, NULL, aurEff);
                    victim->ToPlayer()->AddSpellCooldown(PALADIN_SPELL_ARDENT_DEFENDER_HEAL, Seconds(120));
                }
                else
                    absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register() override
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_ardent_defender_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ardent_defender_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_ardent_defender_AuraScript();
        }
};

class spell_pal_blessing_of_faith : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_faith() : SpellScriptLoader("spell_pal_blessing_of_faith") { }

        class spell_pal_blessing_of_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_of_faith_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_DRUID) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_PALADIN) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_PRIEST) || !sSpellMgr->GetSpellInfo(SPELL_BLESSING_OF_LOWER_CITY_SHAMAN))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 spell_id = 0;
                    switch (unitTarget->GetClass())
                    {
                        case CLASS_DRUID:   spell_id = SPELL_BLESSING_OF_LOWER_CITY_DRUID; break;
                        case CLASS_PALADIN: spell_id = SPELL_BLESSING_OF_LOWER_CITY_PALADIN; break;
                        case CLASS_PRIEST:  spell_id = SPELL_BLESSING_OF_LOWER_CITY_PRIEST; break;
                        case CLASS_SHAMAN:  spell_id = SPELL_BLESSING_OF_LOWER_CITY_SHAMAN; break;
                        default: return;                    // ignore for non-healing classes
                    }
                    Unit* caster = GetCaster();
                    caster->CastSpell(caster, spell_id, true);
                }
            }

            void Register() override
            {
                // add dummy effect spell handler to Blessing of Faith
                OnEffectHitTarget += SpellEffectFn(spell_pal_blessing_of_faith_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blessing_of_faith_SpellScript();
        }
};

// Holy Shock - 20473
class spell_pal_holy_shock : public SpellScriptLoader
{
    public:
        spell_pal_holy_shock() : SpellScriptLoader("spell_pal_holy_shock") { }

        class spell_pal_holy_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_holy_shock_SpellScript);

            enum
            {
                HOLY_SHOCK_DAMAGE   = 25912,
                HOLY_SHOCK_HEALING  = 25914,
                HOLY_SHOCK_ENERGIZE = 148976,
            };

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return sSpellMgr->GetSpellInfo(HOLY_SHOCK_DAMAGE)
                    && sSpellMgr->GetSpellInfo(HOLY_SHOCK_HEALING)
                    && sSpellMgr->GetSpellInfo(HOLY_SHOCK_ENERGIZE);
            }

            SpellCastResult CheckCast()
            {
                Unit const* const target = GetExplTargetUnit();
                if (!target)
                    return SPELL_FAILED_BAD_TARGETS;

                Unit const* const caster = GetCaster();
                if (!caster->IsFriendlyTo(target))
                {
                    if (!caster->IsValidAttackTarget(target))
                        return SPELL_FAILED_BAD_TARGETS;
                    if (!caster->isInFront(target))
                        return SPELL_FAILED_UNIT_NOT_INFRONT;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* const caster = GetCaster();
                Unit* const target = GetHitUnit();

                uint32 const spellId = caster->IsFriendlyTo(target)
                    ? HOLY_SHOCK_HEALING
                    : HOLY_SHOCK_DAMAGE;

                caster->CastSpell(target, spellId, true);
                // Add Holy Power
                caster->CastSpell(caster, HOLY_SHOCK_ENERGIZE, true);
            }

            void Register() override
            {
                // add dummy effect spell handler to Holy Shock
                OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_holy_shock_SpellScript();
        }
};

class spell_pal_divine_storm : public SpellScriptLoader
{
    public:
        spell_pal_divine_storm() : SpellScriptLoader("spell_pal_divine_storm") { }

        class spell_pal_divine_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_storm_SpellScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DIVINE_STORM_DUMMY))
                    return false;
                return true;
            }

            bool Load() override
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            void TriggerHeal()
            {
                Unit* caster = GetCaster();
                caster->CastCustomSpell(SPELL_DIVINE_STORM_DUMMY, SPELLVALUE_BASE_POINT0, (GetHitDamage() * healPct) / 100, caster, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pal_divine_storm_SpellScript::TriggerHeal);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_divine_storm_SpellScript();
        }
};

class spell_pal_righteous_defense : public SpellScriptLoader
{
    public:
        spell_pal_righteous_defense() : SpellScriptLoader("spell_pal_righteous_defense") { }

        class spell_pal_righteous_defense_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_righteous_defense_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (Unit* target = GetExplTargetUnit())
                {
                    if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_righteous_defense_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_righteous_defense_SpellScript();
        }
};

// 35395 - Crusader Strike
class spell_pal_crusader_strike : public SpellScript
{
    PrepareSpellScript(spell_pal_crusader_strike);

    SpellMissInfo hitResult = SPELL_MISS_NONE;

    void HandleHit()
    {
        if (GetHitUnit() != GetCaster())
        {
            hitResult = GetHitResult();
            if (hitResult == SPELL_MISS_NONE)
                GetCaster()->CastSpell(GetHitUnit(), SPELL_WEAKENED_BLOWS, true);
        }
    }

    void HandleEnergize(SpellEffIndex effect)
    {
        if (hitResult != SPELL_MISS_NONE)
            PreventDefaultEffect(effect);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_crusader_strike::HandleHit);
        OnEffectHitTarget += SpellEffectFn(spell_pal_crusader_strike::HandleEnergize, EFFECT_2, SPELL_EFFECT_ENERGIZE);
    }
};


// 42463 - Seal of Truth
class spell_pal_seal_of_truth_hotfix : public SpellScript
{
    PrepareSpellScript(spell_pal_seal_of_truth_hotfix);

    void HandleHit()
    {
        // https://worldofwarcraft.com/en-us/news/7207170
        // Seal of Truth and Censure now deal 80% less damage for Protection Paladins only.
        if (Player* paladin = GetCaster()->ToPlayer())
            if (paladin->GetTalentSpecialization() == SPEC_PALADIN_PROTECTION)
                SetHitDamage(GetHitDamage() * 0.2f);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_seal_of_truth_hotfix::HandleHit);
    }
};

// 31803 - Censure
class spell_pal_censure_hotfix : public AuraScript
{
    PrepareAuraScript(spell_pal_censure_hotfix);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        // https://worldofwarcraft.com/en-us/news/7207170
        // Seal of Truth and Censure now deal 80% less damage for Protection Paladins only.
        if (Unit* caster = GetCaster())
            if (Player* paladin = caster->ToPlayer())
                if (paladin->GetTalentSpecialization() == SPEC_PALADIN_PROTECTION)
                    amount *= 0.2f;
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_censure_hotfix::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 19740 - Blessing of Might, 20217 - Blessing of Knight
class spell_pal_blessing : public SpellScriptLoader
{
    public:
        spell_pal_blessing() : SpellScriptLoader("spell_pal_blessing") { }

        class spell_pal_blessing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_SpellScript);

            void HandleOnHit()
            {
                auto caster = GetCaster();
                if (auto target = GetHitUnit())
                {
                    if (target->IsInRaidWith(caster))
                    {
                        std::list<Unit*> playerList;
                        caster->GetPartyMembers(playerList);
                        // AddAura required to prevent infinite script calls loop
                        for (auto raidMember : playerList)
                            caster->AddAura(GetSpellInfo()->Id, raidMember);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_blessing_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_blessing_SpellScript();
        }
};

// Guardian of ancient kings
class spell_pal_guardian : public SpellScriptLoader
{
    public:
        spell_pal_guardian() : SpellScriptLoader("spell_pal_guardian") { }

        class spell_pal_guardian_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_guardian_SpellScript);

            void HandleOnHit()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (!caster)
                    return;

                // Choose which guardian to summon based on spec
                switch (caster->GetTalentSpecialization())
                {
                    case SPEC_PALADIN_HOLY:
                        caster->CastSpell(caster, 86674, true);
                        break;
                    default:
                        return;
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pal_guardian_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pal_guardian_SpellScript();
        }
};

// 24275 - Hammer of Wrath
// 20271 - Judgment
class spell_pal_sword_of_light_damage_bonus_holy : public SpellScript
{
    PrepareSpellScript(spell_pal_sword_of_light_damage_bonus_holy);

    void HandleHit()
    {
        if (AuraEffect const* bonus = GetCaster()->GetAuraEffect(SPELL_PALADIN_SWORD_OF_LIGHT_DAMAGE_BONUS, EFFECT_0))
            SetHitDamage(GetHitDamage() * (1.0f + bonus->GetFloatAmount() / 100.0f));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_sword_of_light_damage_bonus_holy::HandleHit);
    }
};

// 114694 - Pursuit of Justice
class spell_pal_pursuit_of_justice : public SpellScript
{
    PrepareSpellScript(spell_pal_pursuit_of_justice);

    void HandleCast()
    {
        int32 val = GetCaster()->GetPower(POWER_HOLY_POWER);
        if (val > 0)
            GetCaster()->CastCustomSpell(SPELL_PALADIN_PURSUIT_OF_JUSTICE, SPELLVALUE_BASE_POINT0, 15 + std::min(val, 3) * 5, GetCaster(), true);
        else
            GetCaster()->RemoveAurasDueToSpell(SPELL_PALADIN_PURSUIT_OF_JUSTICE);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pal_pursuit_of_justice::HandleCast);
    }
};

// Judgment - 20271
class spell_pal_judgment : public SpellScript
{
    PrepareSpellScript(spell_pal_judgment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return sSpellMgr->GetSpellInfo(PALADIN_SPELL_JUDGMENT) != nullptr;
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        Unit* unitTarget = GetHitUnit();

        if (!player || !unitTarget)
            return;

        // Selfless Healer in Holy Spec
        if (player->HasAura(85804) && player->GetTalentSpecialization() == SPEC_PALADIN_HOLY)
            player->CastSpell(player, 148502, true);

        if (player->HasAura(PALADIN_SPELL_JUDGMENTS_OF_THE_BOLD) || player->HasAura(PALADIN_SPELL_JUDGMENTS_OF_THE_WISE))
        {
            int32 power = 1;

            // Sanctified Wrath with Protection Spec
            if (player->HasAura(114232) && player->GetTalentSpecialization() == SPEC_PALADIN_PROTECTION)
                power = 2;

            if (player->HasAura(PALADIN_SPELL_HOLY_AVENGER))
                power = 3;

            player->ModifyPower(POWER_HOLY_POWER, power);
        }

        if (player->HasAura(PALADIN_SPELL_JUDGMENTS_OF_THE_BOLD))
            player->CastSpell(unitTarget, PALADIN_SPELL_PHYSICAL_VULNERABILITY, true);

        if (player->HasAura(PALADIN_SPELL_LONG_ARM_OF_THE_LAW))
            player->CastSpell(player, PALADIN_SPELL_LONG_ARM_OF_THE_LAW_RUN_SPEED, true);

        if (player->HasAura(PALADIN_SPELL_BURDEN_OF_GUILT))
            player->CastSpell(unitTarget, PALADIN_SPELL_BURDEN_OF_GUILT_DECREASE_SPEED, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_judgment::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 85673 - Word of Glory, 136494 - Word of Glory (glyphed)
class spell_pal_word_of_glory : public SpellScript
{
    PrepareSpellScript(spell_pal_word_of_glory);

    void HandleHit()
    {
        Unit* paladin = GetCaster();

        bool freeCast = paladin->HasAura(SPELL_PALADIN_DIVINE_PURPOSE) || paladin->HasAura(SPELL_PALADIN_BASTION_OF_POWER);
        int32 holyPower = freeCast ? 3 : paladin->GetPower(POWER_HOLY_POWER) + 1;
        holyPower = std::min(3, holyPower);

        uint32 spell = paladin->IsValidAttackTarget(GetHitUnit()) ? SPELL_PALADIN_WORD_OF_GLORY_DAMAGE : SPELL_PALADIN_WORD_OF_GLORY_HEAL;
        paladin->CastSpell(GetHitUnit(), spell, true);

        if (!freeCast && holyPower > 1)
            paladin->ModifyPower(POWER_HOLY_POWER, -(holyPower - 1));

        if (spell == SPELL_PALADIN_WORD_OF_GLORY_HEAL && paladin->HasAura(SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY))
            paladin->CastCustomSpell(SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY_EFFECT, SPELLVALUE_BASE_POINT0, 3 * holyPower, paladin, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_word_of_glory::HandleHit);
    }
};

static void HandleT15Protection2PBonus(Unit* paladin, int32 holyPower)
{
    int32 duration = 0;
    if (Aura* shield = paladin->GetAura(SPELL_PALADIN_SHIELD_OF_GLORY))
        duration = shield->GetDuration();

    paladin->CastSpell(paladin, SPELL_PALADIN_SHIELD_OF_GLORY, true);
    if (Aura* shield = paladin->GetAura(SPELL_PALADIN_SHIELD_OF_GLORY))
    {
        if (duration < 5 * IN_MILLISECONDS * holyPower)
        {
            shield->SetMaxDuration(5 * IN_MILLISECONDS * holyPower);
            shield->SetDuration(shield->GetMaxDuration());
        }
    }
}

// 130551 - Word of Glory (heal)
class spell_pal_word_of_glory_heal : public SpellScript
{
    PrepareSpellScript(spell_pal_word_of_glory_heal);

    void HandleHit()
    {
        bool freeCast = GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE) || GetCaster()->HasAura(SPELL_PALADIN_BASTION_OF_POWER);

        int32 holyPower = freeCast ? 3 : GetCaster()->GetPower(POWER_HOLY_POWER) + 1;
        holyPower = std::min(3, holyPower);

        int32 heal = GetHitHeal();
        if (GetHitUnit() == GetCaster()) // Bastion of Glory works only for self healing
            if (AuraEffect* bastion = GetCaster()->GetAuraEffect(SPELL_PALADIN_BASTION_OF_GLORY, EFFECT_0))
                AddPct(heal, bastion->GetAmount());

        SetHitHeal(heal * holyPower);

        if (GetCaster()->HasAura(SPELL_PALADIN_T15_PROTECTION_2P_BONUS))
            HandleT15Protection2PBonus(GetCaster(), holyPower);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_word_of_glory_heal::HandleHit);
    }
};

// 130552 - Harsh Word
class spell_pal_harsh_word : public SpellScript
{
    PrepareSpellScript(spell_pal_harsh_word);

    void HandleHit()
    {
        bool freeCast = GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE) || GetCaster()->HasAura(SPELL_PALADIN_BASTION_OF_POWER);
        int32 holyPower = freeCast ? 3 : GetCaster()->GetPower(POWER_HOLY_POWER) + 1;
        SetHitDamage(GetHitDamage() * std::min(3, holyPower));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_harsh_word::HandleHit);
    }
};

// 85416 - Grand Crusader
class spell_pal_grand_crusader : public SpellScript
{
    PrepareSpellScript(spell_pal_grand_crusader);

    void HandleCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_PALADIN_AVENGERS_SHIELD, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pal_grand_crusader::HandleCast);
    }
};

// 53385 - Divine Storm
// 53600 - Shield of the Righteous
// 84963 - Inquisition
// 85222 - Light of Dawn
// 85256 - Templar's Verdict
// 85673 - Word of Glory
// 114163 - Eternal Flame
// 136494 - Word of Glory
// 138165 - Templar's Verdict
class spell_pal_divine_purpose : public SpellScript
{
    PrepareSpellScript(spell_pal_divine_purpose);

    void HandleCast()
    {
        Unit* paladin = GetCaster();
        bool divinePurpose = paladin->HasAura(SPELL_PALADIN_DIVINE_PURPOSE);
        bool divineCrusader = paladin->HasAura(SPELL_PALADIN_DIVINE_CRUSADER);
        bool bastionOfPower = paladin->HasAura(SPELL_PALADIN_BASTION_OF_POWER);

        bool divineCrusadeConsumed = divineCrusader && GetSpellInfo()->Id == 53385;
        bool bastionOfPowerConsumed = bastionOfPower && (GetSpellInfo()->Id == 85673 || GetSpellInfo()->Id == 114163);

        // Remove buffs
        if (divineCrusadeConsumed)
            paladin->RemoveAurasDueToSpell(SPELL_PALADIN_DIVINE_CRUSADER);
        if (bastionOfPowerConsumed)
            paladin->RemoveAurasDueToSpell(SPELL_PALADIN_BASTION_OF_POWER);
        if (divinePurpose && !divineCrusadeConsumed && !bastionOfPowerConsumed)
            paladin->RemoveAurasDueToSpell(SPELL_PALADIN_DIVINE_PURPOSE);

        // Roll now
        if (paladin->HasAura(SPELL_PALADIN_T16_RETRIBUTION_4P_BONUS))
            if (roll_chance_i(25))
                paladin->CastSpell(paladin, SPELL_PALADIN_DIVINE_CRUSADER, true);

        if (paladin->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_TALENT))
            if (roll_chance_i(25))
                paladin->CastSpell(paladin, SPELL_PALADIN_DIVINE_PURPOSE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pal_divine_purpose::HandleCast);
    }
};

// 84963 - Inquisition
class spell_pal_inquisition : public SpellScript
{
    PrepareSpellScript(spell_pal_inquisition);

    bool m_divinePurpose = false;
    int32 m_holyPower = 0;
    int32 m_additional = 0;

    SpellCastResult CheckCast()
    {
        Unit* paladin = GetCaster();
        m_divinePurpose = paladin->HasAura(SPELL_PALADIN_DIVINE_PURPOSE);
        m_holyPower = m_divinePurpose ? 3 : paladin->GetPower(POWER_HOLY_POWER);
        m_holyPower = std::min(3, m_holyPower);

        if (Aura* inquisition = paladin->GetAura(SPELL_PALADIN_INQUISITION))
        {
            if (inquisition->GetDuration() <= 2000)
                m_additional = inquisition->GetDuration();
            if (inquisition->CalcMaxDuration(paladin) * m_holyPower < inquisition->GetDuration())
                return SPELL_FAILED_STRONGER_SPELL_IN_PROGRESS;
        }
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        Unit* paladin = GetCaster();
        Aura* inquisition = paladin->GetAura(SPELL_PALADIN_INQUISITION);
        if (!inquisition)
            return;

        inquisition->SetDuration(inquisition->GetMaxDuration() * m_holyPower + m_additional);

        if (!m_divinePurpose && m_holyPower > 1)
            paladin->ModifyPower(POWER_HOLY_POWER, -(m_holyPower - 1));
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_inquisition::CheckCast);
        OnHit += SpellHitFn(spell_pal_inquisition::HandleHit);
    }
};

// 85222 - Light of Dawn 
class spell_pal_light_of_dawn : public spell_smart_heal
{
    PrepareSpellScript(spell_pal_light_of_dawn);

    int32 m_holyPower = 0;
    bool m_divinePurpose = false;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 count = GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_LIGHT_OF_DAWN) ? 4 : 6;
        ApplySmartHeal(targets, count);
    }

    void HandleCast()
    {
        m_divinePurpose = GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE);
        m_holyPower = m_divinePurpose ? 3 : GetCaster()->GetPower(POWER_HOLY_POWER);
        m_holyPower = std::min(m_holyPower, 3);

        if (!m_divinePurpose && m_holyPower > 1)
            GetCaster()->ModifyPower(POWER_HOLY_POWER, -(m_holyPower - 1)); // One will be taken after cast
    }

    void HandleHit()
    {
        SetHitHeal(GetHitHeal() * m_holyPower);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_light_of_dawn::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnCast += SpellCastFn(spell_pal_light_of_dawn::HandleCast);
        OnHit += SpellHitFn(spell_pal_light_of_dawn::HandleHit);
    }
};

// 114157 - Execution Sentence
class spell_pal_execution_sentence : public SpellScript
{
    PrepareSpellScript(spell_pal_execution_sentence);

    void HandleCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
            caster->CastSpell(target, caster->IsFriendlyTo(target) ? SPELL_PAL_STAY_OF_EXECUTION : SPELL_PAL_EXECUTION_SENTENCE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pal_execution_sentence::HandleCast);
    }
};

// 114916 - Execution Sentence, 114917 - Stay of Execution 
class spell_pal_execution_sentence_effect : public AuraScript
{
    PrepareAuraScript(spell_pal_execution_sentence_effect);

    void CalcAmount(AuraEffect const*, float& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            int32 sph = caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
            int32 fromDummy = GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster);
            amount = sph * fromDummy / 1000.0f + 26.72716306f * amount;
        }
    }

    void CalcTick(AuraEffect const* eff, int32& amount)
    {
        float mult = 0.0f;
        switch (eff->GetTickNumber())
        {
            case 1: mult = 0.04115f; break;
            case 2: mult = 0.04525f; break;
            case 3: mult = 0.04975f; break;
            case 4: mult = 0.05475f; break;
            case 5: mult = 0.06025f; break;
            case 6: mult = 0.06625f; break;
            case 7: mult = 0.07295f; break;
            case 8: mult = 0.08025f; break;
            case 9: mult = 0.08815f; break;
            case 10: mult = 0.44125f; break;
        }
        amount *= mult;
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint32 spellId = GetId() == SPELL_PAL_EXECUTION_SENTENCE
            ? SPELL_PAL_EXECUTION_SENTENCE_DISPEL_DAMAGE
            : SPELL_PAL_EXECUTION_SENTENCE_DISPEL_HEAL;

        caster->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, eff->GetAmount() * 0.44125f, GetTarget(), true);
    }

    void Register() override
    {
        AuraType auraName = m_scriptSpellId == SPELL_PAL_EXECUTION_SENTENCE ? SPELL_AURA_PERIODIC_DAMAGE : SPELL_AURA_PERIODIC_HEAL;
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_execution_sentence_effect::CalcAmount, EFFECT_0, auraName);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_pal_execution_sentence_effect::CalcTick, EFFECT_0, auraName);
        OnEffectRemove += AuraEffectRemoveFn(spell_pal_execution_sentence_effect::HandleRemove, EFFECT_0, auraName, AURA_EFFECT_HANDLE_REAL);
    }
};

// 114918 - Light's Hammer (Periodic dummy)
class spell_pal_lights_hammer : public AuraScript
{
    PrepareAuraScript(spell_pal_lights_hammer);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->SetNativeDisplayId(11686);
        GetUnitOwner()->SetDisplayId(11686);
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* pal = GetUnitOwner()->GetOwner())
        {
            pal->CastSpell(*GetUnitOwner(), SPELL_PAL_ARCING_LIGHT_DAMAGE, true);
            pal->CastSpell(*GetUnitOwner(), SPELL_PAL_ARCING_LIGHT_HEAL, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pal_lights_hammer::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_lights_hammer::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 26573, 116467 - Consecration, 110701 - Consecration (symbiosis)
class spell_pal_consecration : public AuraScript
{
    PrepareAuraScript(spell_pal_consecration);

    bool Validate(SpellInfo const* spellInfo) override
    {
        if (spellInfo->Effects[EFFECT_2].Effect == SPELL_EFFECT_TRIGGER_SPELL)
            return sSpellMgr->GetSpellInfo(spellInfo->Effects[EFFECT_2].TriggerSpell) != nullptr;
        return true;
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* caster = GetCaster();
        if (caster != GetOwner()) // Glyphed version
            return;

        uint32 spellId = GetSpellInfo()->SpellFamilyName == SPELLFAMILY_DRUID ? SPELL_DRUID_CONSECRATION_DAMAGE : SPELL_PALADIN_CONSECRATION_DAMAGE;
        auto summonSpell = GetSpellInfo();
        if (GetSpellInfo()->Effects[EFFECT_2].Effect == SPELL_EFFECT_TRIGGER_SPELL) // Glyphed
            summonSpell = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[EFFECT_2].TriggerSpell);

        for (auto&& itr : caster->GetSummons())
            if (itr->GetEntry() == (uint32)summonSpell->Effects[EFFECT_2].MiscValue)
                caster->CastSpell(itr->GetPosition(), spellId, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_consecration::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 86674 - Ancient Healer (Proc driver for 86669)
class spell_pal_ancient_healer : public AuraScript
{
    PrepareAuraScript(spell_pal_ancient_healer);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 heal = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), 100);
        if (!heal)
            return;
        std::list<TempSummon*> summons;
        GetTarget()->GetSummons(summons, 46499);
        if (!summons.empty())
            summons.front()->CastCustomSpell(SPELL_PALADIN_LIGHT_OF_THE_ANCIENT_KINGS, SPELLVALUE_BASE_POINT0, heal, eventInfo.GetActionTarget(), true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pal_ancient_healer::HandleProc);
    }
};

// 88821 - Daybreak (Passive aura)
class spell_pal_daybreak_passive : public AuraScript
{
    PrepareAuraScript(spell_pal_daybreak_passive);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* spell = eventInfo.TriggeredBySpell();
        if (!spell)
            return false;
        return eventInfo.GetActionTarget() == spell->m_targets.GetUnitTarget();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_daybreak_passive::CheckProc);
    }
};

// 88819 - Daybreak (Proc buff)
class spell_pal_daybreak : public AuraScript
{
    PrepareAuraScript(spell_pal_daybreak);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        int32 heal = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), eff->GetAmount());
        GetTarget()->CastCustomSpell(SPELL_PALADIN_DAYBREAK, SPELLVALUE_BASE_POINT1, heal, eventInfo.GetActionTarget(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_daybreak::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 121129 - Daybreak (Heal)
class spell_pal_daybreak_heal : public SpellScript
{
    PrepareSpellScript(spell_pal_daybreak_heal);

    uint32 m_targetCount = 1;

    void HandleHeal(SpellEffIndex)
    {
        int32 heal = GetHitHeal();
        if (AuraEffect const* setBonus = GetCaster()->GetAuraEffect(SPELL_PALADIN_T15_HOLY_2P_BONUS, EFFECT_0))
            AddPct(heal, setBonus->GetAmount());
        SetHitHeal(heal / m_targetCount);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target)
        {
            return target->ToUnit()->IsSummon() && target->ToUnit()->ToTempSummon()->IsMinorGuardian();
        });
        targets.remove(GetExplTargetUnit());
        m_targetCount = targets.size();
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_daybreak_heal::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_pal_daybreak_heal::HandleHeal, EFFECT_1, SPELL_EFFECT_HEAL);
    }
};

// 114163 - Eternal Flame
class spell_pal_eternal_flame : public SpellScript
{
    PrepareSpellScript(spell_pal_eternal_flame);

    void HandleHit()
    {
        bool freeCast = GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE) || GetCaster()->HasAura(SPELL_PALADIN_BASTION_OF_POWER);
        
        int32 holyPower = freeCast ? 3 : GetCaster()->GetPower(POWER_HOLY_POWER) + 1;
        holyPower = std::min(3, holyPower);

        if (!freeCast && holyPower > 1)
            GetCaster()->ModifyPower(POWER_HOLY_POWER, -(holyPower - 1));

        int32 heal = GetHitHeal();
        AuraEffect* bastion = nullptr;
        if (GetHitUnit() == GetCaster()) // Bastion of Glory works only for self healing
            bastion = GetCaster()->GetAuraEffect(SPELL_PALADIN_BASTION_OF_GLORY, EFFECT_0);
        if (bastion)
            AddPct(heal, bastion->GetAmount());

        SetHitHeal(heal * holyPower);

        if (Aura* aura = GetHitAura())
        {
            if (AuraEffect* hot = aura->GetEffect(EFFECT_1))
            {
                int32 amount = hot->GetAmount() * holyPower;
                if (bastion)
                    AddPct(amount, bastion->GetAmount());
                if (GetHitUnit() == GetCaster())
                    amount *= 1.5f;
                hot->SetAmount(amount);
            }
        }

        if (GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY))
            GetCaster()->CastCustomSpell(SPELL_PALADIN_GLYPH_OF_WORD_OF_GLORY_EFFECT, SPELLVALUE_BASE_POINT0, 3 * holyPower, GetCaster(), true);

        if (GetCaster()->HasAura(SPELL_PALADIN_T15_PROTECTION_2P_BONUS))
            HandleT15Protection2PBonus(GetCaster(), holyPower);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_eternal_flame::HandleHit);
    }
};

// 76672 - Mastery: Hand of Light
class spell_pal_mastery_hand_of_light : public AuraScript
{
    PrepareAuraScript(spell_pal_mastery_hand_of_light);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        float damage = CalculatePct(eventInfo.GetDamage(), eff->GetFloatAmount());
        if (damage <= 0.0f || !eventInfo.GetActionTarget())
            return;

        if (AuraEffect const* inquisition = GetUnitOwner()->GetAuraEffect(SPELL_PALADIN_INQUISITION, EFFECT_0))
            AddPct(damage, inquisition->GetFloatAmount());
        if (AuraEffect const* exorcism = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_PALADIN_T15_RETRIBUTION_2P_EXORCISM, EFFECT_0))
            AddPct(damage, exorcism->GetFloatAmount());
        for (auto&& itr : eventInfo.GetActionTarget()->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
        {
            // 5% magical damge taken...
            if (itr->GetId() == 1490 || itr->GetId() == 24844 || itr->GetId() == 34889 || itr->GetId() == 93068)
            {
                AddPct(damage, itr->GetAmount());
                break;
            }
        }

        GetUnitOwner()->CastCustomSpell(SPELL_PALADIN_HAND_OF_LIGHT, SPELLVALUE_BASE_POINT0, damage,
            eventInfo.GetActionTarget(), true, nullptr, eff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_mastery_hand_of_light::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 119072 - Holy Wrath
class spell_pal_holy_wrath : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_wrath);

    std::list<WorldObject*> selected;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_FOCUSED_WRATH) && !targets.empty())
        {
            WorldObject* random = Trinity::Containers::SelectRandomContainerElement(targets);
                targets.remove_if([this](WorldObject* target)
            {
                return target->GetGUID() != GetCaster()->GetTarget();
            });

            if (targets.empty())
                targets.push_back(random);
        }

        selected = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = selected;
    }

    void HandleLaunch(SpellEffIndex index)
    {
        if (Creature* cr = GetHitCreature())
        {
            uint32 mask = CREATURE_TYPEMASK_DEMON_OR_UNDEAD;
            if (GetCaster()->HasAura(SEPLL_PALADIN_GLYPH_OF_HOLY_WRATH))
                mask |= (1 << (CREATURE_TYPE_DRAGONKIN - 1)) | (1 << (CREATURE_TYPE_ELEMENTAL - 1)) | (1 << (CREATURE_TYPE_ABERRATION - 1));

            if ((cr->GetCreatureTypeMask() & mask))
                return;
        }

        // Player in Lichborn is considered undead type
        if (Player* player = GetHitPlayer())
            if (player->HasAura(50397))
                return;

        PreventEffectForTarget(GetHitUnit(), index);
    }

    void HandleHit()
    {
        int32 damage = GetHitDamage();
        if (GetHitUnit()->HealthBelowPct(20))
            if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_FINAL_WRATH, EFFECT_0))
                AddPct(damage, eff->GetAmount());

        uint32 targetCount = selected.empty() ? 1 : uint32(selected.size());
        SetHitDamage(damage / targetCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_wrath::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_wrath::CopyTargets,   EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectLaunchTarget += SpellEffectFn(spell_pal_holy_wrath::HandleLaunch, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnHit += SpellHitFn(spell_pal_holy_wrath::HandleHit);
    }
};

// 879 - Exorcism
class spell_pal_exorcism : public SpellScript
{
    PrepareSpellScript(spell_pal_exorcism);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PALADIN_EXORCISM_ENERGIZE, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pal_exorcism::HandleHit);
    }
};

// 122032 - Exorcism
class spell_pal_exorcism_glyphed : public SpellScript
{
    PrepareSpellScript(spell_pal_exorcism_glyphed);

    void HandleHit()
    {
        if (GetHitUnit() != GetExplTargetUnit())
            SetHitDamage(GetHitDamage() / 4);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_exorcism_glyphed::HandleHit);
    }
};

// 63225 - Glyph of Inquisition
class spell_pal_glyph_of_inquisition : public AuraScript
{
    PrepareAuraScript(spell_pal_glyph_of_inquisition);

    void HandleProc(AuraEffect const* eff, ProcEventInfo&)
    {
        if (Aura* inquisition = GetUnitOwner()->GetAura(SPELL_PALADIN_INQUISITION))
        {
            int32 newDuration = std::min(inquisition->GetDuration() + eff->GetAmount() * IN_MILLISECONDS, 2 * MINUTE * IN_MILLISECONDS);
            inquisition->SetMaxDuration(newDuration);
            inquisition->RefreshDuration(false);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_glyph_of_inquisition::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 633 - Lay on Hands
class spell_pal_lay_on_hands : public SpellScript
{
    PrepareSpellScript(spell_pal_lay_on_hands);

    SpellCastResult CheckCast()
    {
        if (Unit* target = GetExplTargetUnit())
            if (target->HasAura(SPELL_PALADIN_FORBEARANCE))
                return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PALADIN_FORBEARANCE, true);
        if (GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_DIVINITY))
            GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_GLYPH_OF_DIVINITY_ENERGIZE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands::CheckCast);
        OnHit += SpellHitFn(spell_pal_lay_on_hands::HandleHit);
    }
};

// 88263 - Hammer of the Righteous 
class spell_pal_glyph_of_hammer_of_the_righteous : public SpellScript
{
    PrepareSpellScript(spell_pal_glyph_of_hammer_of_the_righteous);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_HAMMER_OF_THE_RIGHTEOUS))
        {
            if (Aura* aura = GetHitUnit()->GetAura(SPELL_WEAKENED_BLOWS, GetCaster()->GetGUID()))
            {
                aura->SetMaxDuration(aura->GetMaxDuration() * 1.5f);
                aura->RefreshDuration(false);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pal_glyph_of_hammer_of_the_righteous::HandleHit);
    }
};

// 114637 - Bastion of Glory
class spell_pal_bastion_of_glory : public AuraScript
{
    PrepareAuraScript(spell_pal_bastion_of_glory);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || eventInfo.GetActionTarget() != GetOwner()) // Only for self healing
            return false;

        if (eventInfo.GetSpellInfo()->Id == SPELL_PALADIN_FLASH_OF_LIGHT)
            if (GetUnitOwner()->HasAura(SPELL_PALADIN_SELFLESS_HEALER))
                return true;

        return eventInfo.GetSpellInfo()->Id == SPELL_PALADIN_WORD_OF_GLORY_HEAL
            || eventInfo.GetSpellInfo()->Id == SPELL_PALADIN_ETERNAL_FLAME;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_bastion_of_glory::CheckProc);
    }
};

// 19750 - Flash of Light
class spell_pal_flash_of_light: public SpellScript
{
    PrepareSpellScript(spell_pal_flash_of_light);

    void HandleHit()
    {
        if (GetHitUnit() == GetCaster())
            if (Aura* selflessHealer = GetCaster()->GetAura(SPELL_PALADIN_SELFLESS_HEALER))
                if (AuraEffect* bastion = GetCaster()->GetAuraEffect(SPELL_PALADIN_BASTION_OF_GLORY, EFFECT_2))
                    if (int32 heal = GetHitHeal())
                        SetHitHeal(AddPct(heal, bastion->GetAmount() * selflessHealer->GetStackAmount()));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_flash_of_light::HandleHit);
    }
};

// 19750 - Flash of Light, 82327 - Holy Radiance, 82326 - Divine Light
class spell_pal_selfless_healer: public SpellScript
{
    PrepareSpellScript(spell_pal_selfless_healer);

    void HandleHit()
    {
        if (GetHitUnit() != GetCaster())
            if (AuraEffect* selflessHealer = GetCaster()->GetAuraEffect(SPELL_PALADIN_SELFLESS_HEALER, EFFECT_1))
                if (int32 heal = GetHitHeal())
                    SetHitHeal(AddPct(heal, selflessHealer->GetAmount()));
    }

    void Register() override
    {
        OnHit+= SpellHitFn(spell_pal_selfless_healer::HandleHit);
    }
};

// 114250 - Selfless Healer
class spell_pal_selfless_healer_ui : public AuraScript
{
    PrepareAuraScript(spell_pal_selfless_healer_ui);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetStackAmount() >= 3)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PALADIN_SELFLESS_HEALER_UI, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pal_selfless_healer_ui::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// 93466 - Glyph of Protector of the Innocent
class spell_pal_glyph_of_protector_of_the_innocent : public AuraScript
{
    PrepareAuraScript(spell_pal_glyph_of_protector_of_the_innocent);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetActionTarget() != GetOwner();
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        int32 heal = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), eff->GetAmount());
        GetUnitOwner()->CastCustomSpell(SPELL_PALADIN_PROTECTOR_OF_THE_INNOCENT_HEAL, SPELLVALUE_BASE_POINT0, heal,
            eventInfo.GetActionTarget(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_glyph_of_protector_of_the_innocent::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_glyph_of_protector_of_the_innocent::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 76669 - Mastery: Illuminated Healing
class spell_pal_mastery_illuminated_healing : public AuraScript
{
    PrepareAuraScript(spell_pal_mastery_illuminated_healing);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id != SPELL_PALADIN_BEACON_OF_LIGHT_HEAL;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActionTarget())
            return;
        float cap = GetUnitOwner()->GetHealth() / 3.f;
        float absorb = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), eff->GetFloatAmount());
        AuraEffect* old = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_PALADIN_ILLUMINATED_HEALING, EFFECT_0, GetCasterGUID());
        if (!old)
        {
            GetUnitOwner()->CastCustomSpell(SPELL_PALADIN_ILLUMINATED_HEALING, SPELLVALUE_BASE_POINT0, std::min(absorb, cap),
                eventInfo.GetActionTarget(), true);
        }
        else
        {
            absorb += old->GetFloatAmount();
            old->ChangeAmount(std::min(absorb, cap));
            old->GetBase()->RefreshDuration(false);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_mastery_illuminated_healing::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_mastery_illuminated_healing::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 642 - Divine Shield, 110700 - Divine Shield (Symbiosis)
class spell_pal_divine_shield : public SpellScript
{
    PrepareSpellScript(spell_pal_divine_shield);

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_PALADIN_FORBEARANCE))
                return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void CalculateHeal(SpellEffIndex)
    {
        if (Aura* glyph = GetCaster()->GetAura(SPELL_PALADIN_GLYPH_OF_DIVINE_SHIELD))
            SetEffectValue(std::min(GetDispelCount() * glyph->GetEffect(EFFECT_0)->GetAmount(), glyph->GetEffect(EFFECT_0)->GetAmount() * glyph->GetEffect(EFFECT_1)->GetAmount()));
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_FORBEARANCE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_divine_shield::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pal_divine_shield::CalculateHeal, EFFECT_3, SPELL_EFFECT_HEAL_PCT);
        OnHit += SpellHitFn(spell_pal_divine_shield::HandleHit);
    }
};

// 53651 - Beacon of Light (Proc aura)
class spell_pal_beacon_of_light : public AuraScript
{
    PrepareAuraScript(spell_pal_beacon_of_light);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetTypeMask() & (PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS)) &&
            !(eventInfo.GetHitMask() & PROC_EX_INTERNAL_HOT))
            return false;
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetHealInfo()->GetHeal())
            return false;

        auto list = GetUnitOwner()->GetBoundAurasBySpellId(SPELL_PALADIN_BEACON_OF_LIGHT);
        return list && list->front()->GetUnitOwner() != eventInfo.GetActionTarget();
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        auto list = GetUnitOwner()->GetBoundAurasBySpellId(SPELL_PALADIN_BEACON_OF_LIGHT);

        Unit* beaconTarget = list->front()->GetUnitOwner();

        static std::map<uint32, int32> const map = 
        {
            { 635,      100 }, // Holy Light
            { 82327,    15  }, // Holy Radiance
            { 85222,    15  }, // Light of Dawn
            { 119952,   15  }, // Light's Hammer
            { 114871,   15  }, // Holy Pirsm
        };

        auto it = map.find(eventInfo.GetSpellInfo()->Id);
        int32 pct = it != map.end() ? it->second : 50;

        int32 heal = CalculatePct<int32>((int32)eventInfo.GetHealInfo()->GetHeal(), pct);
        if (AuraEffect const* setBonus = GetUnitOwner()->GetAuraEffect(SPELL_PALADIN_T15_HOLY_4P_BONUS, EFFECT_0))
            AddPct(heal, setBonus->GetAmount());
        TriggerCastFlags flags = TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_CASTER_AURAS);
        GetUnitOwner()->CastCustomSpell(SPELL_PALADIN_BEACON_OF_LIGHT_HEAL, SPELLVALUE_BASE_POINT0, heal, beaconTarget, flags, nullptr, eff);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (auto list = GetUnitOwner()->GetBoundAurasBySpellId(SPELL_PALADIN_BEACON_OF_LIGHT))
        {
            for (auto&& itr : *list)
            {
                // Must be only one aura
                itr->Remove();
                break;
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_beacon_of_light::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_beacon_of_light::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_pal_beacon_of_light::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 53563 - Beacon of Light
class spell_pal_beacon_of_light_remove : public AuraScript
{
    PrepareAuraScript(spell_pal_beacon_of_light_remove);

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (Unit* paladin = GetCaster())
            paladin->RemoveAurasDueToSpell(eff->GetSpellEffectInfo().TriggerSpell);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_pal_beacon_of_light_remove::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 20165 - Seal of Insight
class spell_pal_seal_of_insight : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_insight);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == 88263)
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& /*eventInfo*/)
    {
        float ap = GetUnitOwner()->GetTotalAttackPowerValue(BASE_ATTACK);
        float sp = GetUnitOwner()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
        float heal = 0.15f * ap + 0.15f * sp;
        if (GetUnitOwner()->HasAura(SPELL_PALADIN_HOLY_INSIGHT))
            heal *= 1.25f;
        SpellValueMod bp = SPELLVALUE_BASE_POINT0;
        if (AuraEffect const* glyph = GetUnitOwner()->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_THE_BATTLE_HEALER, EFFECT_0))
        {
            ApplyPct(heal, glyph->GetAmount());
            bp = SPELLVALUE_BASE_POINT1;
        }
        GetUnitOwner()->CastCustomSpell(eff->GetSpellEffectInfo().TriggerSpell, bp, heal, GetUnitOwner(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_insight::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_insight::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 86703 - Ancient Crusader, 86698 - Guardian of Ancient Kings
class spell_pal_ancient_crusader : public AuraScript
{
    PrepareAuraScript(spell_pal_ancient_crusader);

    void HandleProc(ProcEventInfo&)
    {
        if (Player* paladin = GetUnitOwner()->ToPlayer())
            paladin->CastSpell(paladin, SPELL_PALADIN_ANCIENT_POWER, true);
        else if (Unit* paladin = GetUnitOwner()->GetOwner())
            paladin->CastSpell(paladin, SPELL_PALADIN_ANCIENT_POWER, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pal_ancient_crusader::HandleProc);
    }
};

// 86698 - Guardian of Ancient Kings
class spell_pal_guardian_of_ancient_kings : public AuraScript
{
    PrepareAuraScript(spell_pal_guardian_of_ancient_kings);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* paladin = GetUnitOwner();
        paladin->m_Events.Schedule(1, [=]
        {
            if (Aura* ancientPower = paladin->GetAura(SPELL_PALADIN_ANCIENT_POWER))
            {
                CustomSpellValues ctx;
                ctx.SetMultiplier(ancientPower->GetStackAmount());
                paladin->CastCustomSpell(SPELL_PALADIN_ANCIENT_FURY, ctx, paladin, TRIGGERED_FULL_MASK);
                paladin->RemoveAura(ancientPower);
            }
        });
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_pal_guardian_of_ancient_kings::HandleRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 46506
struct npc_guardian_of_ancient_kings : public PetAI
{
    npc_guardian_of_ancient_kings(Creature* c) : PetAI(c)
    {
        me->InitCharmInfo();
        me->SetReactState(REACT_ASSIST);
    }
};

// 19750 - Flash of Light
class spell_pal_pvp_p14_holy: public SpellScript
{
    PrepareSpellScript(spell_pal_pvp_p14_holy);

    void HandleCast()
    {
        if (GetCaster()->GetAura(SPELL_PALADIN_PVP_HOLY_P4_BONUS))
            GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_PVP_HOLY_P14_ENERGIZE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pal_pvp_p14_holy::HandleCast);
    }
};

// 20164 - Seal of Justice
class spell_pal_seal_of_justice : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_justice);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo())
            return eventInfo.GetSpellInfo()->Id != SPELL_PALADIN_JUDMENT && eventInfo.GetSpellInfo()->Id != SPELL_PALADIN_HAMMER_OF_WRATH;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_justice::CheckProc);
    }
};

// 31884 - Avenging Wrath
class spell_pal_avenging_wrath : public AuraScript
{
    PrepareAuraScript(spell_pal_avenging_wrath);

    void InitEffects(uint32& effMask)
    {
        Unit* paladin = GetCaster();
        if (paladin && paladin->GetTypeId() == TYPEID_PLAYER)
        {
            if (paladin->ToPlayer()->GetTalentSpecialization() != SPEC_PALADIN_RETRIBUTION)
                effMask &= ~(1 << EFFECT_2);

            if (!paladin->HasAura(SPELL_PALADIN_GLYPH_OF_THE_FALLING_AVENGER))
                effMask &= ~(1 << EFFECT_3);
        }
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_pal_avenging_wrath::InitEffects);
    }
};

// 114232 - Sanctified Wrath
class spell_pal_sanctified_wrath : public AuraScript
{
    PrepareAuraScript(spell_pal_sanctified_wrath);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void InitEffects(uint32& effMask)
    {
        effMask = 0;
        switch (GetUnitOwner()->ToPlayer()->GetTalentSpecialization())
        {
            case SPEC_PALADIN_HOLY:
                effMask |= (1 << EFFECT_0);
                effMask |= (1 << EFFECT_4);
                break;
            case SPEC_PALADIN_PROTECTION:
                effMask |= (1 << EFFECT_1);
                effMask |= (1 << EFFECT_3);
                break;
            case SPEC_PALADIN_RETRIBUTION:
                effMask |= (1 << EFFECT_2);
                break;
        }
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_pal_sanctified_wrath::InitEffects);
    }
};

// 121027 - Glyph of Double Jeopardy
class spell_pal_glyph_of_double_jeopardy : public AuraScript
{
    PrepareAuraScript(spell_pal_glyph_of_double_jeopardy);
    
    uint64 lastTargetGuid = 0;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Spell const* judment = GetUnitOwner()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            lastTargetGuid = judment->m_targets.GetUnitTargetGUID();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pal_glyph_of_double_jeopardy::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
public:
    uint64 GetLastTarget() const { return lastTargetGuid; }
};

// 20271 - Judgment
class spell_pal_glyph_of_double_jeopardy_judgment : public SpellScript
{
    PrepareSpellScript(spell_pal_glyph_of_double_jeopardy_judgment);

    void HandleHit()
    {
        if (AuraEffect* eff = GetCaster()->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_DOUBLE_JEOPARDY, EFFECT_0))
            if (auto script = dynamic_cast<spell_pal_glyph_of_double_jeopardy*>(eff->GetBase()->GetScriptByName("spell_pal_glyph_of_double_jeopardy")))
                if (script->GetLastTarget() != GetHitUnit()->GetGUID())
                    SetHitDamage(CalculatePct(GetHitDamage(), 100 + eff->GetAmount()));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_glyph_of_double_jeopardy_judgment::HandleHit);
    }
};

// 35395 - Crusader Strike
class spell_pal_t15_retribution_4p_bonus : public SpellScript
{
    PrepareSpellScript(spell_pal_t15_retribution_4p_bonus);

    void HandleCast()
    {
        if (AuraEffect const* setBonus = GetCaster()->GetAuraEffect(SPELL_PALADIN_T15_RETRIBUTION_4P_BONUS, EFFECT_0))
            if (roll_chance_i(setBonus->GetAmount()))
                GetCaster()->CastSpell(GetCaster(), SPELL_PALADIN_TEMPLARS_VERDICT_OVERRIDE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pal_t15_retribution_4p_bonus::HandleCast);
    }
};

// 138250 - Divine Protection
class spell_pal_t15_protection_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t15_protection_4p_bonus);

    uint32 damageTaken = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        // TODO: When I create proper way to determine damage?...
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->IsPositive())
            return;

        damageTaken += eventInfo.GetDamageInfo()->GetDamage();
        uint32 needed = GetUnitOwner()->CountPctFromMaxHealth(20);
        if (damageTaken >= needed)
        {
            damageTaken -= needed;
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PALADIN_HOLY_POWER_ENERGIZE, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pal_t15_protection_4p_bonus::HandleProc);
    }
};

// 149150 - Divine Protection
class spell_pal_t16_protection_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t16_protection_2p_bonus);

    uint32 damageTaken = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & (PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS))
            return;

        damageTaken += eventInfo.GetDamage();
        if (eventInfo.GetProcTriggerContext()->MeleeDamage)
            damageTaken += eventInfo.GetProcTriggerContext()->MeleeDamage->absorb;
        else if (eventInfo.GetProcTriggerContext()->SpellDamage)
            damageTaken += eventInfo.GetProcTriggerContext()->SpellDamage->absorb;
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect const* bonus = GetUnitOwner()->GetAuraEffect(SPELL_PALADIN_T16_PROTECTION_2P_BONUS, EFFECT_0))
            if (int32 heal = CalculatePct(damageTaken, bonus->GetAmount()))
                GetUnitOwner()->CastCustomSpell(SPELL_PALADIN_BLESSING_OF_THE_GUARDIANS, SPELLVALUE_BASE_POINT0, heal / 10, GetUnitOwner(), true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pal_t16_protection_2p_bonus::HandleProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_pal_t16_protection_2p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 144566 - Item - Paladin T16 Protection 4P Bonus
class spell_pal_t16_protection_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t16_protection_4p_bonus);

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        if (GetUnitOwner()->GetAuraCount(SPELL_PALADIN_BASTION_OF_GLORY) >= 3)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PALADIN_BASTION_OF_POWER, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pal_t16_protection_4p_bonus::HandleProc);
    }
};

// 20154 - Seal of Righteousness
class spell_pal_seal_of_righteousness : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_righteousness);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.TriggeredBySpell())
            if (!eventInfo.GetActionTarget() || eventInfo.TriggeredBySpell()->m_targets.GetUnitTargetGUID() != eventInfo.GetActionTarget()->GetGUID())
                return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_righteousness::CheckProc);
    }
};

// 114165 - Holy Prism
class spell_pal_holy_prism : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_prism);

    SpellCastResult CheckCast()
    {
        if (!GetExplTargetUnit())
            return SPELL_FAILED_BAD_TARGETS;
        if (!GetCaster()->IsValidAttackTarget(GetExplTargetUnit()) && !GetCaster()->IsValidAssistTarget(GetExplTargetUnit()))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        Unit* paladin = GetCaster();
        Unit* target = GetHitUnit();

        if (paladin->IsValidAttackTarget(target))
        {
            paladin->CastSpell(target, SPELL_PALADIN_HOLY_PRISM_ENEMY, true);
            paladin->CastSpell(target, SPELL_PALADIN_HOLY_PRISM_DAMAGE_VISUAL, true);
        }
        else
        {
            paladin->CastSpell(target, SPELL_PALADIN_HOLY_PRISM_ALLY, true);
            paladin->CastSpell(target, SPELL_PALADIN_HOLY_PRISM_HEAL_VISUAL, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_holy_prism::CheckCast);
        OnHit += SpellHitFn(spell_pal_holy_prism::HandleHit);
    }
};

// 114852, 114871 - Holy Prism
class spell_pal_holy_prism_effect : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_prism_effect);

    void HandleHit(SpellEffIndex)
    {
        if (GetSpellInfo()->Id == SPELL_PALADIN_HOLY_PRISM_ENEMY)
            GetExplTargetUnit()->CastSpell(GetHitUnit(), SPELL_PALADIN_HOLY_PRISM_DAMAGE_VISUAL_AOE, true);
        else
            GetExplTargetUnit()->CastSpell(GetHitUnit(), SPELL_PALADIN_HOLY_PRISM_HEAL_VISUAL_AOE, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        Trinity::RandomResizeList(targets, 5);
    }

    void UnusedEffect(std::list<WorldObject*>& target)
    {
        target.clear();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_holy_prism_effect::HandleHit, EFFECT_1, SPELL_EFFECT_ANY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_prism_effect::SelectTargets, EFFECT_1, sSpellMgr->GetSpellInfo(m_scriptSpellId)->Effects[EFFECT_1].TargetB.GetTarget());
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_prism_effect::UnusedEffect, EFFECT_2, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// 59578 - Art of War
class spell_pal_art_of_war : public SpellScript
{
    PrepareSpellScript(spell_pal_art_of_war);

    void HandleHit()
    {
        if (Player* paladin = GetCaster()->ToPlayer())
        {
            if (paladin->HasSpellCooldown(PALADIN_SPELL_EXORCISM))
                paladin->RemoveSpellCooldown(PALADIN_SPELL_EXORCISM, true);
            if (paladin->HasSpellCooldown(PALADIN_SPELL_MASS_EXORCISM))
                paladin->RemoveSpellCooldown(PALADIN_SPELL_MASS_EXORCISM, true);
            // Another shit with procs
            if (AuraEffect const* bonus = paladin->GetAuraEffect(SPELL_PALADIN_T16_RETRIBUTION_2P_BONUS, EFFECT_0))
                paladin->CastSpell(paladin, bonus->GetSpellEffectInfo().TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pal_art_of_war::HandleHit);
    }
};

// 13819 - Summon Warhorse
// 23214 - Summon Charger
class spell_pal_glyph_of_mounted_king : public SpellScript
{
    PrepareSpellScript(spell_pal_glyph_of_mounted_king);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_PALADIN_GLYPH_OF_THE_MOUNTED_KING))
            GetCaster()->CastCustomSpell(SPELL_PALADIN_BLESSING_OF_KINGS, SPELLVALUE_BASE_POINT1, 1, GetCaster(), true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pal_glyph_of_mounted_king::HandleCast);
    }
};

// 20217 - Blessing of Kings
class spell_pal_glyph_of_mounted_king_filter : public SpellScript
{
    PrepareSpellScript(spell_pal_glyph_of_mounted_king_filter);

    void SelectTarget(std::list<WorldObject*>& list)
    {
        if (GetSpellValue()->EffectBasePoints[EFFECT_1] == 1)
        {
            list.clear();
            list.push_back(GetCaster());
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_mounted_king_filter::SelectTarget, EFFECT_0, TARGET_UNIT_TARGET_OR_UNIT_PARTY);
    }
};

// Divine Plea - 54428
class spell_pal_divine_plea : public spirit_based_aura_effect
{
    PrepareAuraScript(spell_pal_divine_plea);

    void CalculateAmount(AuraEffect const* aurEff, float& amount, bool&)
    {
        float manaPct = CalculatePct(GetUnitOwner()->GetMaxPower(POWER_MANA), GetSpellInfo()->Effects[EFFECT_1].BasePoints);
        float minAmount = manaPct / aurEff->GetTotalTicks();
        if (GetUnitOwner()->HasAura(SPELL_PALADIN_GLYPH_OF_DIVINE_PLEA))
            minAmount /= 2.0f;

        float spirit = GetSpiritWithoutShortDurationBonuses(GetUnitOwner());
        amount = CalculatePct(spirit, amount);
        amount = std::max(amount, minAmount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_divine_plea::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
    }
};

// 54943 - Glyph of Blessed Life
class spell_pal_glyph_of_blessed_life : public AuraScript
{
    PrepareAuraScript(spell_pal_glyph_of_blessed_life);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        uint32 mechanicMask = (1 << MECHANIC_STUN) | (1 << MECHANIC_FEAR) | (1 << MECHANIC_ROOT);
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & mechanicMask;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_glyph_of_blessed_life::CheckProc);
    }
};

// 20066 - Repentance
// 105421 - Blinding Light
class spell_pal_censure_control_fix : public AuraScript
{
    PrepareAuraScript(spell_pal_censure_control_fix);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Any damage from sources other than Censure will awaken the target.
        if (eventInfo.GetSpellInfo())
            return eventInfo.GetSpellInfo()->Id != SPELL_PALADIN_CENSURE;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_censure_control_fix::CheckProc);
    }
};

// 6940 - Hand of Sacrifice
class spell_pal_hand_of_sacrifice : public AuraScript
{
    PrepareAuraScript(spell_pal_hand_of_sacrifice);

    void CalculateSplitDamage(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_PALADIN_GLYPH_OF_HAND_OF_SACRIFICE))
                amount = 0;
    }

    void CalculateDamageReduction(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (!caster->HasAura(SPELL_PALADIN_GLYPH_OF_HAND_OF_SACRIFICE))
                amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_sacrifice::CalculateSplitDamage, EFFECT_0, SPELL_AURA_SPLIT_DAMAGE_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_sacrifice::CalculateDamageReduction, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// 203782 - Custom spell
class spell_pal_glyph_of_bladed_judgement : public AuraScript
{
    PrepareAuraScript(spell_pal_glyph_of_bladed_judgement);

    void CalculateAmount(AuraEffect const*, float&, bool&)
    {
        Player* paladin = GetUnitOwner()->ToPlayer();

        paladin->RemoveAurasDueToSpell(SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_SWORD);
        paladin->RemoveAurasDueToSpell(SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_AXE);

        if (Item* weapon = paladin->GetWeaponForAttack(BASE_ATTACK))
        {
            if (weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_SWORD || weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2)
                paladin->CastSpell(paladin, SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_SWORD, true);
            else if (weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_AXE || weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_AXE2)
                paladin->CastSpell(paladin, SPELL_PALADIN_GLYPH_OF_BLADED_JUDGEMENT_AXE, true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_glyph_of_bladed_judgement::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Glyph of Corpse Explosion - 125043
class spell_pal_glyph_of_contemplation : public SpellScriptLoader
{
    public:
        spell_pal_glyph_of_contemplation() : SpellScriptLoader("spell_pal_glyph_of_contemplation") { }

        class spell_pal_glyph_of_contemplation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_glyph_of_contemplation_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(PALADIN_SPELL_GLYPH_OF_CONTEMPLATION, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(PALADIN_SPELL_GLYPH_OF_CONTEMPLATION))
                        _player->RemoveSpell(PALADIN_SPELL_GLYPH_OF_CONTEMPLATION, false, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_pal_glyph_of_contemplation_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_pal_glyph_of_contemplation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pal_glyph_of_contemplation_AuraScript();
        }
};

void AddSC_paladin_spell_scripts()
{
    new spell_pal_glyph_of_devotian_aura();
    new spell_pal_glyph_of_devotian_trigger_aura();
    new spell_pal_glyph_of_divine_storm();
    new spell_pal_hand_of_purity();
    new spell_pal_glyph_of_avenging_wrath();
    new spell_script<spell_pal_shield_of_the_righteous>("spell_pal_shield_of_the_righteous");
    new spell_script<spell_pal_tower_of_radiance>("spell_pal_tower_of_radiance");
    new spell_pal_sacred_shield();
    new spell_pal_sacred_shield_absorb();
    new spell_pal_emancipate();
    new spell_pal_blinding_light();
    new spell_pal_hand_of_protection();
    new spell_pal_ardent_defender();
    new spell_pal_blessing_of_faith();
    new spell_pal_holy_shock();
    new spell_pal_divine_storm();
    new spell_pal_righteous_defense();
    new spell_script<spell_pal_crusader_strike>("spell_pal_crusader_strike");
    new spell_script<spell_pal_seal_of_truth_hotfix>("spell_pal_seal_of_truth_hotfix");
    new aura_script<spell_pal_censure_hotfix>("spell_pal_censure_hotfix");
    new spell_pal_avenging_wrath();
    new spell_pal_glyph_of_double_jeopardy_judgment();
    new spell_pal_blessing();
    new spell_pal_guardian();
    new spell_script<spell_pal_sword_of_light_damage_bonus_holy>("spell_pal_sword_of_light_damage_bonus_holy");
    new spell_script<spell_pal_pursuit_of_justice>("spell_pal_pursuit_of_justice");
    new spell_script<spell_pal_judgment>("spell_pal_judgment");
    new spell_script<spell_pal_word_of_glory>("spell_pal_word_of_glory");
    new spell_script<spell_pal_word_of_glory_heal>("spell_pal_word_of_glory_heal");
    new spell_script<spell_pal_harsh_word>("spell_pal_harsh_word");
    new spell_script<spell_pal_grand_crusader>("spell_pal_grand_crusader");
    new spell_script<spell_pal_divine_purpose>("spell_pal_divine_purpose");
    new spell_script<spell_pal_inquisition>("spell_pal_inquisition");
    new spell_script<spell_pal_light_of_dawn>("spell_pal_light_of_dawn");
    new spell_script<spell_pal_execution_sentence>("spell_pal_execution_sentence");
    new aura_script<spell_pal_execution_sentence_effect>("spell_pal_execution_sentence_effect");
    new aura_script<spell_pal_lights_hammer>("spell_pal_lights_hammer");
    new aura_script<spell_pal_consecration>("spell_pal_consecration");
    new aura_script<spell_pal_ancient_healer>("spell_pal_ancient_healer");
    new aura_script<spell_pal_daybreak_passive>("spell_pal_daybreak_passive");
    new aura_script<spell_pal_daybreak>("spell_pal_daybreak");
    new spell_script<spell_pal_daybreak_heal>("spell_pal_daybreak_heal");
    new spell_script<spell_pal_eternal_flame>("spell_pal_eternal_flame");
    new aura_script<spell_pal_mastery_hand_of_light>("spell_pal_mastery_hand_of_light");
    new spell_script<spell_pal_holy_wrath>("spell_pal_holy_wrath");
    new spell_script<spell_pal_exorcism>("spell_pal_exorcism");
    new spell_script<spell_pal_exorcism_glyphed>("spell_pal_exorcism_glyphed");
    new aura_script<spell_pal_glyph_of_inquisition>("spell_pal_glyph_of_inquisition");
    new spell_script<spell_pal_lay_on_hands>("spell_pal_lay_on_hands");
    new spell_script<spell_pal_glyph_of_hammer_of_the_righteous>("spell_pal_glyph_of_hammer_of_the_righteous");
    new aura_script<spell_pal_bastion_of_glory>("spell_pal_bastion_of_glory");
    new spell_script<spell_pal_flash_of_light>("spell_pal_flash_of_light");
    new spell_script<spell_pal_selfless_healer>("spell_pal_selfless_healer");
    new aura_script<spell_pal_selfless_healer_ui>("spell_pal_selfless_healer_ui");
    new aura_script<spell_pal_glyph_of_protector_of_the_innocent>("spell_pal_glyph_of_protector_of_the_innocent");
    new aura_script<spell_pal_mastery_illuminated_healing>("spell_pal_mastery_illuminated_healing");
    new spell_script<spell_pal_divine_shield>("spell_pal_divine_shield");
    new aura_script<spell_pal_beacon_of_light>("spell_pal_beacon_of_light");
    new aura_script<spell_pal_beacon_of_light_remove>("spell_pal_beacon_of_light_remove");
    new aura_script<spell_pal_seal_of_insight>("spell_pal_seal_of_insight");
    new aura_script<spell_pal_ancient_crusader>("spell_pal_ancient_crusader");
    new aura_script<spell_pal_guardian_of_ancient_kings>("spell_pal_guardian_of_ancient_kings");
    new creature_script<npc_guardian_of_ancient_kings>("npc_guardian_of_ancient_kings");
    new spell_script<spell_pal_pvp_p14_holy>("spell_pal_pvp_p14_holy");
    new aura_script<spell_pal_seal_of_justice>("spell_pal_seal_of_justice");
    new aura_script<spell_pal_avenging_wrath>("spell_pal_avenging_wrath");
    new aura_script<spell_pal_sanctified_wrath>("spell_pal_sanctified_wrath");
    new aura_script<spell_pal_glyph_of_double_jeopardy>("spell_pal_glyph_of_double_jeopardy");
    new spell_script<spell_pal_glyph_of_double_jeopardy_judgment>("spell_pal_glyph_of_double_jeopardy_judgment");
    new spell_script<spell_pal_t15_retribution_4p_bonus>("spell_pal_t15_retribution_4p_bonus");
    new aura_script<spell_pal_t15_protection_4p_bonus>("spell_pal_t15_protection_4p_bonus");
    new aura_script<spell_pal_t16_protection_2p_bonus>("spell_pal_t16_protection_2p_bonus");
    new aura_script<spell_pal_t16_protection_4p_bonus>("spell_pal_t16_protection_4p_bonus");
    new aura_script<spell_pal_seal_of_righteousness>("spell_pal_seal_of_righteousness");
    new spell_script<spell_pal_holy_prism>("spell_pal_holy_prism");
    new spell_script<spell_pal_holy_prism_effect>("spell_pal_holy_prism_effect");
    new spell_script<spell_pal_art_of_war>("spell_pal_art_of_war");
    new spell_script<spell_pal_glyph_of_mounted_king>("spell_pal_glyph_of_mounted_king");
    new spell_script<spell_pal_glyph_of_mounted_king_filter>("spell_pal_glyph_of_mounted_king_filter");
    new aura_script<spell_pal_divine_plea>("spell_pal_divine_plea");
    new aura_script<spell_pal_glyph_of_blessed_life>("spell_pal_glyph_of_blessed_life");
    new aura_script<spell_pal_censure_control_fix>("spell_pal_censure_control_fix");
    new aura_script<spell_pal_hand_of_sacrifice>("spell_pal_hand_of_sacrifice");
    new aura_script<spell_pal_glyph_of_bladed_judgement>("spell_pal_glyph_of_bladed_judgement");
    new spell_pal_glyph_of_contemplation();
}
