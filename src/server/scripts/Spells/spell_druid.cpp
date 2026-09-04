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
 * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dru_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Group.h"
#include "Containers.h"
#include "Battleground.h"
#include "PassiveAI.h"
#include "SpellHistory.h"
#include "spell_common.h"
#include "Random.h"

enum DruidSpells
{
    DRUID_INCREASED_MOONFIRE_DURATION       = 38414,
    DRUID_NATURES_SPLENDOR                  = 57865,
    DRUID_SURVIVAL_INSTINCTS                = 50322,
    SPELL_DRUID_SAVAGE_ROAR                 = 52610,
    SPELL_DRUID_SAVAGE_ROAR_GLYPHED         = 127538,
    SPELL_DRUID_SAVAGE_ROAR_EFFECT          = 62071,
    SPELL_DRUID_ITEM_T8_BALANCE_RELIC       = 64950,
    SPELL_DRUID_WRATH                       = 5176,
    SPELL_DRUID_STARFIRE                    = 2912,
    SPELL_DRUID_STARSURGE                   = 78674,
    SPELL_DRUID_ECLIPSE_ENERGIZE            = 89265,
    SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE    = 81070,
    SPELL_DRUID_STARSURGE_ENERGIZE          = 86605,
    SPELL_DRUID_SOLAR_ECLIPSE               = 48517,
    SPELL_DRUID_LUNAR_ECLIPSE               = 48518,
    SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE      = 107095,
    SPELL_DRUID_ECLIPSE_MARKER_LUNAR        = 67484,
    SPELL_DRUID_ECLIPSE_MARKER_SOLAR        = 67483,
    SPELL_DRUID_ASTRAL_INSIGHT              = 145138,
    SPELL_DRUID_STARFALL                    = 48505,
    SPELL_DRUID_NATURES_GRACE               = 16886,
    SPELL_DRUID_EUPHORIA                    = 81062,
    SPELL_DRUID_PROWL                       = 5215,
    SPELL_DRUID_WEAKENED_ARMOR              = 113746,
    SPELL_DRUID_GLYPH_OF_FRENZIED_REGEN     = 54810,
    SPELL_DRUID_FRENZIED_REGENERATION       = 22842,
    SPELL_DRUID_FRENZIED_REGEN_HEAL_TAKE    = 124769,
    SPELL_DRUID_CELESTIAL_ALIGNMENT         = 112071,
    SPELL_DRUID_ASTRAL_COMMUNION            = 127663,
    SPELL_DRUID_SUNFIRE                     = 93402,
    SPELL_DRUID_MOONFIRE                    = 8921,
    SPELL_DRUID_EFFLORESCENCE               = 81262,
    SPELL_DRUID_EFFLORESCENCE_TICK          = 81269,
    SPELL_DRUID_EFFLORESCENCE_TICK_TREANTS  = 142424,
    SPELL_DRUID_FAERIE_DECREASE_SPEED       = 102354,
    SPELL_DRUID_SKULL_BASH_MANA_COST        = 82365,
    SPELL_DRUID_SKULL_BASH_INTERUPT         = 93985,
    SPELL_DRUID_SKULL_BASH_CHARGE           = 93983,
    SPELL_DRUID_FORM_CAT_INCREASE_SPEED     = 113636,
    SPELL_DRUID_GLYPH_OF_REGROWTH           = 116218,
    SPELL_DRUID_REGROWTH                    = 8936,
    SPELL_DRUID_MARK_OF_THE_WILD            = 1126,
    SPELL_DRUID_OMEN_OF_CLARITY             = 113043,
    SPELL_DRUID_CLEARCASTING                = 16870,
    SPELL_DRUID_LIFEBLOOM                   = 33763,
    SPELL_DRUID_LIFEBLOOM_FINAL_HEAL        = 33778,
    SPELL_DRUID_CAT_FORM                    = 768,
    SPELL_DRUID_BEAR_FORM                   = 5487,
    SPELL_DRUID_BEAR_FORM_PASSIVE           = 1178,
    SPELL_DRUID_INFECTED_WOUNDS             = 58180,
    SPELL_DRUID_BEAR_HUG                    = 102795,
    SPELL_DRUID_RIP                         = 1079,
    SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT    = 132402,
    SPELL_DRUID_DASH                        = 1850,
    SPELL_DRUID_BERSERK_BEAR                = 50334,
    SPELL_DRUID_BERSERK_CAT                 = 106951,
    SPELL_DRUID_STAMPEDING_ROAR             = 106898,
    SPELL_DRUID_GLYPH_OF_STAMPEDING_ROAR    = 114300,
    SPELL_DRUID_URSOLS_VORTEX_AREA_TRIGGER  = 102793,
    SPELL_DRUID_URSOLS_VORTEX_SNARE         = 127797,
    SPELL_DRUID_URSOLS_VORTEX_JUMP_DEST     = 118283,
    SPELL_DRUID_CENARION_WARD               = 102352,
    SPELL_DRUID_NATURES_VIGIL_HEAL          = 124988,
    SPELL_DRUID_NATURES_VIGIL_DAMAGE        = 124991,
    SPELL_DRUID_SYMBIOSIS_FOR_CASTER        = 110309,
    SPELL_DRUID_SYMBIOSIS_DEATH_KNIGHT      = 110478,
    SPELL_DRUID_SYMBIOSIS_HUNTER            = 110479,
    SPELL_DRUID_SYMBIOSIS_MAGE              = 110482,
    SPELL_DRUID_SYMBIOSIS_MONK              = 110483,
    SPELL_DRUID_SYMBIOSIS_PALADIN           = 110484,
    SPELL_DRUID_SYMBIOSIS_PRIEST            = 110485,
    SPELL_DRUID_SYMBIOSIS_ROGUE             = 110486,
    SPELL_DRUID_SYMBIOSIS_SHAMAN            = 110488,
    SPELL_DRUID_SYMBIOSIS_WARLOCK           = 110490,
    SPELL_DRUID_SYMBIOSIS_WARRIOR           = 110491,
    SPELL_DRUID_SHATTERING_BLOW             = 112997,
    WARLOCK_DEMONIC_CIRCLE_SUMMON           = 48018,
    SPELL_DRUID_RAKE                        = 1822,
    SPELL_DRUID_CONSECRATION_DUMMY          = 81298,
    SPELL_DRUID_CONSECRATION_DAMAGE         = 110705,
    SPELL_DRUID_SHOOTING_STARS              = 93400,
    SPELL_DRUID_TIGERS_FURY                 = 5217,
    SPELL_DRUID_SOUL_OF_THE_FOREST          = 114107,
    SPELL_DRUID_SOUL_OF_THE_FOREST_HASTE    = 114108,
    SPELL_DRUID_SOUL_OF_THE_FOREST_ENERGIZE = 114113,
    SPELL_DRUID_SWIPE_CAT                   = 62078,
    SPELL_DRUID_MANGLE_BEAR                 = 33878,
    SPELL_DRUID_STAMPEDE                    = 81022,
    SPELL_DRUID_INCARNATION_KING_OF_JUNGLE  = 102543,
    SPELL_DRUID_GLYPH_OF_SHRED              = 114234,
    SPELL_DRUID_GLYPH_OF_SHRED_OVERRIDE     = 114235,
    SPELL_DRUID_INCARNATION_CHOSEN_OF_ELUNE = 122114,
    SPELL_DRUID_GLYPH_OF_BLOOMING           = 121840,
    SPELL_DRUID_GLYPH_OF_THE_TREANT         = 114282,
    SPELL_DRUID_REJUVENATION                = 774,
    SPELL_DRUID_TOOTH_AND_CLAW_AURA         = 135286,
    SPELL_DRUID_TOOTH_AND_CLAW_ABSORB       = 135597,
    SPELL_DRUID_TOOTH_AND_CLAW_VISUAL_AURA  = 135601,
    SPELL_DRUID_HARMONY                     = 100977,
    SPELL_DRUID_INCARNATION_AURA            = 117679,
    SPELL_DRUID_HEALING_TOUCH               = 5185,
    SPELL_DRUID_DREAM_OF_CENARIUS_TALENT    = 108373,
    SPELL_DRUID_DREAM_OF_CENARIUS_BALANCE   = 145151,
    SPELL_DRUID_DREAM_OF_CENARIUS_FERAL     = 145152,
    SPELL_DRUID_DREAM_OF_CENARIUS_GUARDIAN  = 145162,
    SPELL_DRUID_DREAM_OF_CENARIUS_RESTOR    = 145153,
    SPELL_DRUID_MUSHROOM_BIRTH              = 94081,
    SPELL_DRUID_MUSHROOM_INVISIBLE          = 92661,
    SPELL_DRUID_MUSHROOM_DEATH              = 116305,
    SPELL_DRUID_MUSHROOM_SUICIDE            = 116302,
    SPELL_DRUID_MUSHROOM_DAMAGE             = 78777,
    SPELL_DRUID_MUSHROOM_SLOW_VISUAL        = 94339,
    SPELL_DRUID_FUNGAL_GROWTH               = 81283,
    SPELL_DRUID_FUNGAL_GROWTH_SLOW          = 81282,
    SPELL_DRUID_MUSHROOM_HEAL               = 102792,
    SPELL_DRUID_MUSHROOM_OVERHEAL_AURA      = 138611,
    SPELL_DRUID_MUSHROOM_ACCUMULATOR        = 138616,
    SPELL_DRUID_MUSHROOM_BLOOM_UI_FLASH     = 138664,
    SPELL_DRUID_GLYPH_OF_EFFLORESCENCE      = 145529,
    SPELL_DRUID_HOTW_CAT                    = 123737,
    SPELL_DRUID_HOTW_BEAR                   = 123738,
    SPELL_DRUID_HOTW_BALANCE                = 108291,
    SPELL_DRUID_HOTW_FERAL                  = 108292,
    SPELL_DRUID_HOTW_GUARDIAN               = 108293,
    SPELL_DRUID_HOTW_RESTO                  = 108294,
    SPELL_DRUID_LEAVING_SEED                = 48504,
    SPELL_DRUID_LEAVING_SEED_HEAL           = 48503,
    SPELL_DRUID_GLYPH_OF_GUIDED_STARS       = 146655,
    SPELL_DRUID_STAMPEDE_TIMER              = 131538,
    SPELL_DRUID_LEADER_OF_THE_PACK_HEAL     = 34299,
    SPELL_DRUID_LEADER_OF_THE_PACK_ENERGIZE = 68285,
    SPELL_DRUID_GLYPH_OF_INNERVATE          = 54832,
    SPELL_DRUID_INNERVATE_SELFCASTED        = 54833,
    SPELL_DRUID_THICK_HIDE                  = 16931,
    SPELL_DRUID_GLYPH_OF_THE_STAG           = 114338,
    SPELL_DRUID_GLYPH_OF_THE_CHEETAH        = 131113,
    SPELL_DRUID_STAG_FORM                   = 115034,
    SPELL_DRUID_SOUND_EFFECT_STAG_FORM      = 126056,
    SPELL_DRUID_GLYPH_OF_FAE_SILENCE        = 114237,
    SPELL_DRUID_FAE_SILENCE                 = 114238,
    SPELL_DRUID_VENGEANCE                   = 132365,
    SPELL_DRUID_GLYPH_OF_REJUVENATION       = 17076,
    SPELL_DRUID_GLYPH_OF_REJUVENATION_EFFECT= 96206,
    SPELL_DRUID_NATURES_SWIFTNESS           = 132158,
    SPELL_DRUID_T15_GUARDIAN_4P_BONUS       = 138222,
    SPELL_DRUID_T15_RESTORATION_2P_BONUS    = 138284,
    SPELL_DRUID_T15_RESTORATION_4P_BONUS    = 138286,
    SPELL_DRUID_T16_GUARDIAN_2P_BONUS       = 144879,
    SPELL_DRUID_T16_GUARDIAN_4P_BONUS       = 144887,
    SPELL_DRUID_T16_FERAL_2P_BONUS          = 144864,
    SPELL_DRUID_FERAL_RAGE                  = 146874,
    SPELL_DRUID_TIGERS_FURY_COMBO           = 144859,
    SPELL_DRUID_LUNAR_BOLT                  = 144770,
    SPELL_DRUID_SOLAR_BOLT                  = 144772,
    SPELL_DRUID_T16_RESTORATION_4P_BONUS    = 144875,
    SPELL_DRUID_SPARK_OF_LIFE               = 144876,
    SPELL_DRUID_URSOCS_VIGOR                = 144888,
    SPELL_DRUID_PRIMAL_FURY_BEAR            = 16959,
    SPELL_DRUID_PRIMAL_FURY_CAT             = 16953,
    SPELL_DRUID_GLYPH_OF_PROWL              = 116186,
    SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE     = 67598,
    SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE_HEAL= 101024,
    SPELL_DRUID_GLYPH_OF_HURRICANE          = 54831,
    SPELL_DRUID_PREDATORY_SWIFTNESS         = 69369,
    SPELL_DRUID_SOLAR_BEAM                  = 78675,
    SPELL_DRUID_SOLAR_BEAM_SILENCE          = 81261,
    SPELL_DRUID_SOLAR_BEAM_SILENCE_SYMBIOSIS= 113287,
    SPELL_DRUID_TRACK_HUMANOIDS             = 5225,
    SPELL_DRUID_GLYPH_OF_THE_PREDATOR       = 114280,
    SPELL_DRUID_GLYPH_OF_THE_CHAMELEON      = 107059,
    SPELL_DRUID_GLYPH_OF_OMENS              = 54812,

    NPC_DRUID_WILD_MUSHROOM                 = 47649,
};

// 135597 - Tooth and Claw
class spell_dru_tooth_and_claw_absorb : public AuraScript
{
    PrepareAuraScript(spell_dru_tooth_and_claw_absorb);

    void HandleAbsorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (Unit* attacker = dmgInfo.GetAttacker())
        {
            if (!attacker->HasAura(SPELL_DRUID_TOOTH_AND_CLAW_VISUAL_AURA, GetCasterGUID()))
                PreventDefaultAction();
            else
            {
                attacker->RemoveAurasDueToSpell(SPELL_DRUID_TOOTH_AND_CLAW_VISUAL_AURA);
                Remove();
            }
        }
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_dru_tooth_and_claw_absorb::HandleAbsorb, EFFECT_0);
    }
};

// 6807 - Maul
class spell_dru_tooth_and_claw_trigger : public SpellScript
{
    PrepareSpellScript(spell_dru_tooth_and_claw_trigger);

    bool Load() override { return GetCaster()->HasAura(SPELL_DRUID_TOOTH_AND_CLAW_AURA); }

    void HandleCast()
    {
        int32 fromAp = (GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) - 2 * GetCaster()->GetStat(STAT_AGILITY)) * 2.2f * 0.4f;
        int32 fromStamina = GetCaster()->GetStat(STAT_STAMINA);
        int32 absorb = std::max(fromAp, fromStamina);
        GetCaster()->RemoveAura(SPELL_DRUID_TOOTH_AND_CLAW_AURA);
        GetCaster()->CastCustomSpell(SPELL_DRUID_TOOTH_AND_CLAW_ABSORB, SPELLVALUE_BASE_POINT0, absorb, GetCaster(), true);
        GetCaster()->CastCustomSpell(SPELL_DRUID_TOOTH_AND_CLAW_VISUAL_AURA, SPELLVALUE_BASE_POINT0, absorb, GetExplTargetUnit(), true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_tooth_and_claw_trigger::HandleCast);
    }
};

// 6807 - Maul
// 779, 62078 - Swipe
class spell_dru_swipe_and_maul_bleed : public SpellScript
{
    PrepareSpellScript(spell_dru_swipe_and_maul_bleed);

    void HandleHit()
    {
        if (GetHitUnit()->HasAuraState(AURA_STATE_BLEEDING))
            SetHitDamage(GetHitDamage() * 1.2);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_swipe_and_maul_bleed::HandleHit);
    }
};

// 145518 - Genesis
class spell_dru_genesis : public SpellScript
{
    PrepareSpellScript(spell_dru_genesis);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* druid = GetCaster();
        targets.remove_if([=](WorldObject* target)
        {
            Unit* unit = target->ToUnit();
            return !unit || !unit->IsInRaidWith(druid) || !unit->HasAura(SPELL_DRUID_REJUVENATION, druid->GetGUID());
        });
    }

    void HandleHit()
    {
        if (AuraEffect* rejuvenation = GetHitUnit()->GetAuraEffect(SPELL_DRUID_REJUVENATION, EFFECT_0, GetCaster()->GetGUID()))
        {
            int32 duration = rejuvenation->GetBase()->GetDuration() / 4;
            int32 amplitude = rejuvenation->GetAmplitude() / 4;
            int32 timer = CalculatePct(amplitude, float(rejuvenation->GetPeriodicTimer()) / rejuvenation->GetAmplitude()) + 1;

            rejuvenation->GetBase()->SetDuration(duration);
            rejuvenation->SetPeriodicTimer(timer);
            rejuvenation->SetAmplitude(amplitude);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_genesis::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_dru_genesis::HandleHit);
    }
};

// 774 - Rejuv
class spell_dru_t15_restoration_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t15_restoration_4p_bonus);

    void HandleTick(AuraEffect const* eff)
    {
        Unit* caster = GetCaster();
        if (!caster || eff->GetTickNumber() < 2)
            return;

        if (AuraEffect* setBonus = caster->GetAuraEffect(SPELL_DRUID_T15_RESTORATION_4P_BONUS, EFFECT_0))
        {
            AuraEffect* rejuv = const_cast<AuraEffect*>(eff);
            int32 amount = rejuv->GetAmount();
            AddPct(amount, setBonus->GetAmount());
            rejuv->ChangeAmount(amount);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_t15_restoration_4p_bonus::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// Glyph of the Treant - 125047
class spell_dru_glyph_of_the_treant : public SpellScriptLoader
{
    public:
        spell_dru_glyph_of_the_treant() : SpellScriptLoader("spell_dru_glyph_of_the_treant") { }

        class spell_dru_glyph_of_the_treant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_glyph_of_the_treant_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(SPELL_DRUID_GLYPH_OF_THE_TREANT, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(SPELL_DRUID_GLYPH_OF_THE_TREANT))
                        _player->RemoveSpell(SPELL_DRUID_GLYPH_OF_THE_TREANT, false, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_the_treant_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_dru_glyph_of_the_treant_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_glyph_of_the_treant_AuraScript();
        }
};

// Incarnation : Chosen of Elune - 102560
class spell_dru_incarnation_chosen_of_elune : public SpellScriptLoader
{
    public:
        spell_dru_incarnation_chosen_of_elune() : SpellScriptLoader("spell_dru_incarnation_chosen_of_elune") { }

        class spell_dru_incarnation_chosen_of_elune_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_incarnation_chosen_of_elune_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->CastSpell(_player, SPELL_DRUID_INCARNATION_CHOSEN_OF_ELUNE, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->RemoveAura(SPELL_DRUID_INCARNATION_CHOSEN_OF_ELUNE);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_dru_incarnation_chosen_of_elune_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_dru_incarnation_chosen_of_elune_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_incarnation_chosen_of_elune_AuraScript();
        }
};

// Called by Incarnation : Chosen of Elune - 102560, Incarnation : Son of Ursoc - 102558 and Incarnation : King of the Jungle - 102543
// Incarnation - Skins
class spell_dru_incarnation_skins : public AuraScript
{
    PrepareAuraScript(spell_dru_incarnation_skins);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->SetDisplayId(GetUnitOwner()->GetModelForForm(GetUnitOwner()->GetShapeshiftForm()));
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RestoreDisplayId();
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_dru_incarnation_skins::HandleApply,  EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_incarnation_skins::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Called by Berserk (cat) - 106951 and Tiger's Fury - 5217
// Glyph of Shred - 114234
class spell_dru_glyph_of_shred : public SpellScriptLoader
{
    public:
        spell_dru_glyph_of_shred() : SpellScriptLoader("spell_dru_glyph_of_shred") { }

        class spell_dru_glyph_of_shred_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_glyph_of_shred_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto target = GetTarget();
                if (target->HasAura(SPELL_DRUID_GLYPH_OF_SHRED))
                    target->CastSpell(target, SPELL_DRUID_GLYPH_OF_SHRED_OVERRIDE, true);
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                auto target = GetTarget();
                if (aurEff->GetSpellInfo()->Id == SPELL_DRUID_BERSERK_CAT && !target->HasAura(SPELL_DRUID_TIGERS_FURY))
                    target->RemoveAurasDueToSpell(SPELL_DRUID_GLYPH_OF_SHRED_OVERRIDE);
                else if (aurEff->GetSpellInfo()->Id == SPELL_DRUID_TIGERS_FURY && !target->HasAura(SPELL_DRUID_BERSERK_CAT))
                    target->RemoveAurasDueToSpell(SPELL_DRUID_GLYPH_OF_SHRED_OVERRIDE);
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case SPELL_DRUID_BERSERK_CAT:
                        OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_shred_AuraScript::OnApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_dru_glyph_of_shred_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case SPELL_DRUID_TIGERS_FURY:
                        OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_shred_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                        OnEffectRemove += AuraEffectRemoveFn(spell_dru_glyph_of_shred_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_glyph_of_shred_AuraScript();
        }
};

// Wild Charge (Moonkin) - 102383
class spell_dru_wild_charge_moonkin : public SpellScriptLoader
{
    public:
        spell_dru_wild_charge_moonkin() : SpellScriptLoader("spell_dru_wild_charge_moonkin") { }

        class spell_dru_wild_charge_moonkin_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_wild_charge_moonkin_SpellScript);

            SpellCastResult CheckFight()
            {
                if (GetCaster())
                {
                    if (!GetCaster()->IsInCombat())
                        return SPELL_FAILED_DONT_REPORT;
                }
                else
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dru_wild_charge_moonkin_SpellScript::CheckFight);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_wild_charge_moonkin_SpellScript();
        }
};

// Tiger's Fury - 5217
class spell_dru_tigers_fury : public SpellScriptLoader
{
    public:
        spell_dru_tigers_fury() : SpellScriptLoader("spell_dru_tigers_fury") { }

        class spell_dru_tigers_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_tigers_fury_SpellScript);

            SpellCastResult CheckBerzerk()
            {
                if (GetCaster())
                {
                    if (GetCaster()->HasAura(SPELL_DRUID_BERSERK_CAT))
                        return SPELL_FAILED_DONT_REPORT;
                }
                else
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dru_tigers_fury_SpellScript::CheckBerzerk);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_tigers_fury_SpellScript();
        }
};

// Play Death - 110597
class spell_dru_play_death : public SpellScriptLoader
{
    public:
        spell_dru_play_death() : SpellScriptLoader("spell_dru_play_death") { }

        class spell_dru_play_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_play_death_AuraScript);

            int32 health;
            int32 mana;

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                health = GetTarget()->GetHealth();
                mana = GetTarget()->GetPower(POWER_MANA);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (health && mana)
                {
                    GetTarget()->SetHealth(health);
                    GetTarget()->SetPower(POWER_MANA, mana);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dru_play_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_FEIGN_DEATH, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dru_play_death_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_FEIGN_DEATH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_play_death_AuraScript();
        }
};

// Life Tap - 122290
class spell_dru_life_tap : public SpellScriptLoader
{
    public:
        spell_dru_life_tap() : SpellScriptLoader("spell_dru_life_tap") { }

        class spell_dru_life_tap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_life_tap_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->ModifyHealth(-1 * int32(_player->CountPctFromMaxHealth(20)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_life_tap_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_life_tap_SpellScript();
        }
};

// Binary predicate for sorting Units based on value of duration of an Aura
class AuraDurationCompareOrderPred
{
    public:
        AuraDurationCompareOrderPred(uint64 caster, uint32 auraId, bool ascending = true) : m_caster(caster), m_aura(auraId), m_ascending(ascending) { }
        bool operator() (const Unit* a, const Unit* b) const
        {
            return m_ascending ? a->GetAura(m_aura, m_caster)->GetDuration() < b->GetAura(m_aura, m_caster)->GetDuration() :
                                    a->GetAura(m_aura, m_caster)->GetDuration() > b->GetAura(m_aura, m_caster)->GetDuration();
        }
    private:
        uint64 m_caster;
        uint32 m_aura;
        const bool m_ascending;
};

// Soul Swap - 110810
class spell_dru_soul_swap : public SpellScriptLoader
{
    public:
        spell_dru_soul_swap() : SpellScriptLoader("spell_dru_soul_swap") { }

        class spell_dru_soul_swap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_soul_swap_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        std::list<Unit*> tempList;
                        std::list<Unit*> targetList;

                        _player->GetAttackableUnitListInRange(tempList, 15.0f);

                        for (auto itr : tempList)
                        {
                            if (itr->GetGUID() == target->GetGUID())
                                continue;

                            if (itr->GetGUID() == _player->GetGUID())
                                continue;

                            if (!_player->IsValidAttackTarget(itr))
                                continue;

                            if (itr->HasAura(SPELL_DRUID_RIP, _player->GetGUID()) && itr->HasAura(SPELL_DRUID_RAKE, _player->GetGUID()))
                                targetList.push_back(itr);
                        }

                        if (!targetList.empty())
                        {
                            targetList.sort(AuraDurationCompareOrderPred(_player->GetGUID(), SPELL_DRUID_RIP));

                            for (auto itr : targetList)
                            {
                                int32 ripDuration = 0;
                                int32 ripMaxDuration = 0;
                                int32 ripAmount = 0;
                                int32 rakeDuration = 0;
                                int32 rakeMaxDuration = 0;
                                int32 rakeAmount = 0;

                                if (Aura *rip = itr->GetAura(SPELL_DRUID_RIP, _player->GetGUID()))
                                {
                                    ripDuration = rip->GetDuration();
                                    ripMaxDuration = rip->GetMaxDuration();
                                    ripAmount = rip->GetEffect(0)->GetAmount();
                                }
                                if (Aura *rake = itr->GetAura(SPELL_DRUID_RAKE, _player->GetGUID()))
                                {
                                    rakeDuration = rake->GetDuration();
                                    rakeMaxDuration = rake->GetMaxDuration();
                                    rakeAmount = rake->GetEffect(1)->GetAmount();
                                }

                                itr->RemoveAura(SPELL_DRUID_RIP, _player->GetGUID());
                                itr->RemoveAura(SPELL_DRUID_RAKE, _player->GetGUID());

                                _player->AddAura(SPELL_DRUID_RIP, target);
                                _player->AddAura(SPELL_DRUID_RAKE, target);

                                if (Aura *rip = target->GetAura(SPELL_DRUID_RIP, _player->GetGUID()))
                                {
                                    rip->SetDuration(ripDuration);
                                    rip->SetMaxDuration(ripMaxDuration);
                                    rip->GetEffect(0)->SetAmount(ripAmount);
                                }
                                if (Aura *rake = target->GetAura(SPELL_DRUID_RAKE, _player->GetGUID()))
                                {
                                    rake->SetDuration(rakeDuration);
                                    rake->SetMaxDuration(rakeMaxDuration);
                                    rake->GetEffect(1)->SetAmount(rakeAmount);
                                }

                                break;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_soul_swap_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_soul_swap_SpellScript();
        }
};

// 112970 - Demonic Circle: Teleport 
class spell_dru_demonic_circle_teleport : public SpellScript
{
    PrepareSpellScript(spell_dru_demonic_circle_teleport);

    GameObject* GetDemonicCircle()
    {
        Player* player = GetCaster()->ToPlayer();
        if (player && player->GetGroup())
            for (auto&& itr : *player->GetGroup())
                if (itr->HasAura(SPELL_DRUID_SYMBIOSIS_WARLOCK, player->GetGUID()))
                    return itr->GetGameObject(WARLOCK_DEMONIC_CIRCLE_SUMMON);
        return nullptr;
    }

    SpellCastResult CheckCast()
    {
        GameObject* circle = GetDemonicCircle();
        if (!circle)
            return SPELL_FAILED_DONT_REPORT;
        return GetCaster()->IsInDist(circle, GetSpellInfo()->GetMaxRange()) ? SPELL_CAST_OK : SPELL_FAILED_OUT_OF_RANGE;
    }

    void HandleHit()
    {
        if (GameObject* circle = GetDemonicCircle())
        {
            GetCaster()->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());
            GetCaster()->RemoveMovementImpairingAuras();
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_demonic_circle_teleport::CheckCast);
        OnHit += SpellHitFn(spell_dru_demonic_circle_teleport::HandleHit);
    }
};

// 112997 - Shattering Blow
class spell_dru_shattering_blow : public SpellScript
{
    PrepareSpellScript(spell_dru_shattering_blow);

    void HandleHit()
    {
        Unit* target = GetHitUnit();
        if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
        {
            target->RemoveAura(SPELL_DRUID_SHATTERING_BLOW);
            target->RemoveAurasWithMechanic(1 << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL, 115760); // Glyph of Ice Block
        }

        GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_CAT_FORM, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_shattering_blow::HandleHit);
    }
};

// Moonfire - 8921
class spell_dru_moonfire : public SpellScriptLoader
{
    public:
        spell_dru_moonfire() : SpellScriptLoader("spell_dru_moonfire") { }

        class spell_dru_moonfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_moonfire_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        if (_player->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT))
                            _player->CastSpell(target, SPELL_DRUID_SUNFIRE, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_moonfire_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_moonfire_SpellScript();
        }
};

// 124974 - Nature's Vigil
class spell_dru_natures_vigil : public AuraScript
{
    PrepareAuraScript(spell_dru_natures_vigil);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo())
            return;

        for (auto&& itr : eventInfo.GetSpellInfo()->Effects)
        {
            if (itr.Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA || itr.IsAreaAuraEffect() || itr.IsTargetingArea())
                return;

            if (itr.IsTargetingChain(eventInfo.GetActor()))
                return;
        }

        CustomSpellValues ctx;

        int32 bp = 0;
        bool isPositive = eventInfo.GetSpellInfo()->IsPositive();
        if (isPositive)
            bp = CalculatePct(eventInfo.GetHeal(), eff->GetAmount());
        else
        {
            bp = CalculatePct(eventInfo.GetDamageWithoutResilience(), eff->GetAmount());
            // Fake value. Set only to tell the script that spell triggered from damage done
            ctx.AddSpellMod(SPELLVALUE_BASE_POINT1, 1);
        }

        if (!bp)
            return;

        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, bp);

        Unit* druid = GetUnitOwner();

        // Healing from both damage and heal spells
        druid->CastCustomSpell(SPELL_DRUID_NATURES_VIGIL_HEAL, ctx, eventInfo.GetActionTarget(), TRIGGERED_FULL_MASK, nullptr, eff);

        if (isPositive)
            druid->CastCustomSpell(SPELL_DRUID_NATURES_VIGIL_DAMAGE, ctx, eventInfo.GetActionTarget(), TRIGGERED_FULL_MASK, nullptr, eff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_natures_vigil::HandleProc, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 124991 - Nature's Vigil
class spell_dru_natures_vigil_damage : public SpellScript
{
    PrepareSpellScript(spell_dru_natures_vigil_damage);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target)
        {
            if (!target->IsInDist(GetExplTargetDest(), 40.0f))
                return true;
            return !target->ToUnit() || target->ToUnit()->HasAuraType(SPELL_AURA_MOD_STEALTH) || target->ToUnit()->HasAuraType(SPELL_AURA_MOD_INVISIBILITY);
        });

        if (!targets.empty())
        {
            targets.sort(Trinity::ObjectDistanceOrderPred{ GetCaster() });
            targets.resize(1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_natures_vigil_damage::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 124988 - Nature's Vigil
class spell_dru_natures_vigil_heal : public spell_smart_heal
{
    PrepareSpellScript(spell_dru_natures_vigil_heal);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, targets.size());

        targets.remove_if([=](WorldObject* target)
        {
            return !target->IsInDist(GetExplTargetDest(), 40.0f);
        });

        ApplySmartHeal(targets, targets.size());

        if (!targets.empty())
        {
            Unit* unit = targets.front()->ToUnit();
            targets.sort(Trinity::ObjectDistanceOrderPred{ GetCaster() });
            for (auto&& target : targets)
            {
                if (!target->ToUnit()->IsFullHealth())
                {
                    unit = target->ToUnit();
                    break;
                }
            }
            targets.clear();
            targets.push_back(unit);
        }
    }

    void HandleHit()
    {
        // Fake value, if zero - triggered from heal and we must not apply modifiers
        if (!GetSpellValue()->EffectBasePoints[EFFECT_1])
            return;

        int32 heal = GetHitHeal();
        heal *= GetHitUnit()->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_PCT);

        if (GetHitUnit()->GetMap()->IsBattlegroundOrArena() && !GetHitUnit()->HasAura(134735))
            AddPct(heal, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));

        SetHitHeal(heal);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_natures_vigil_heal::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_dru_natures_vigil_heal::HandleHit);
    }
};

// Cenarion Ward - 102351
class spell_dru_cenarion_ward : public SpellScriptLoader
{
    public:
        spell_dru_cenarion_ward() : SpellScriptLoader("spell_dru_cenarion_ward") { }
        
        class aura_impl : public AuraScript
        {
            PrepareAuraScript(aura_impl);

            void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                Unit* caster = GetCaster();

                if (!caster)
                    return;

                if (Unit* victim = GetTarget())
                    caster->CastSpell(victim, SPELL_DRUID_CENARION_WARD, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(aura_impl::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new aura_impl();
        }
};

// 983, 984 - Solar Beam
class sat_druid_solar_beam : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering)
    {
        Unit* unit = triggering->ToUnit();
        if (!unit)
            return false;

        if (!m_target->IsWithinDistInMap(unit, m_range))
            return false;

        if (!m_caster->IsValidAttackTarget(unit, m_spellInfo, m_target))
            return false;

        return true;
    }

    void OnTriggeringApply(WorldObject* triggering)
    {
        uint32 spellId = GetSpellInfo()->Id == SPELL_DRUID_SOLAR_BEAM ? SPELL_DRUID_SOLAR_BEAM_SILENCE : SPELL_DRUID_SOLAR_BEAM_SILENCE_SYMBIOSIS;
        m_caster->AddAura(spellId, triggering->ToUnit());
    }

    void OnTriggeringRemove(WorldObject* triggering)
    {
        uint32 spellId = GetSpellInfo()->Id == SPELL_DRUID_SOLAR_BEAM ? SPELL_DRUID_SOLAR_BEAM_SILENCE : SPELL_DRUID_SOLAR_BEAM_SILENCE_SYMBIOSIS;
        triggering->ToUnit()->RemoveAurasDueToSpell(spellId, m_caster->GetGUID());
    }
};

// Dash - 1850
class spell_dru_dash : public SpellScriptLoader
{
    public:
        spell_dru_dash() : SpellScriptLoader("spell_dru_dash") { }

        class spell_dru_dash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_dash_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->HasAura(SPELL_DRUID_STAMPEDING_ROAR))
                        _player->RemoveAura(SPELL_DRUID_STAMPEDING_ROAR);

                    _player->RemoveMovementImpairingAuras();
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_dash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_dash_SpellScript();
        }
};

// 5221 - Shred
// 6785 - Ravage
// 33876 - Mangle (cat)
// 102545 - Ravage (PvP P4)
class spell_dru_rip_duration : public SpellScript
{
    PrepareSpellScript(spell_dru_rip_duration);

    void HandleHit()
    {
        // Each time you Shred, Ravage, or Mangle the target while in Cat Form ...
        if (GetCaster()->GetShapeshiftForm() == FORM_CAT)
        {
            if (Aura* rip = GetHitUnit()->GetAura(SPELL_DRUID_RIP, GetCaster()->GetGUID()))
            {
                int32 duration = rip->GetDuration();
                int32 maxDuration = rip->GetMaxDuration();
                int32 maxPossibleDuration = rip->GetCalculatedMaxDuration() + 6000;

                // ... the duration of your Rip on that target is extended by 2 sec, up to a maximum of 6 sec.
                if (maxDuration < maxPossibleDuration)
                {
                    int32 newMaxDuration = std::min(maxDuration + 2000, maxPossibleDuration);
                    int32 newDuration = std::min(duration + 2000, newMaxDuration);
                    rip->SetMaxDuration(newMaxDuration);
                    rip->SetDuration(newDuration);
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_rip_duration::HandleHit);
    }
};

// 62606 - Savage Defense
class spell_dru_savage_defense : public SpellScript
{
    PrepareSpellScript(spell_dru_savage_defense);

    void HandleOnHit()
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT))
        {
            aura->SetDuration(aura->GetDuration() + 6 * IN_MILLISECONDS);
            if (aura->GetDuration() > aura->GetMaxDuration())
                aura->SetMaxDuration(aura->GetDuration());
        }
        else
            GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_savage_defense::HandleOnHit);
    }
};

// Bear Hug - 102795
class spell_dru_bear_hug : public SpellScriptLoader
{
    public:
        spell_dru_bear_hug() : SpellScriptLoader("spell_dru_bear_hug") { }

        class spell_dru_bear_hug_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_bear_hug_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetShapeshiftForm() != FORM_BEAR)
                        caster->CastSpell(caster, SPELL_DRUID_BEAR_FORM, true);

                    amount = caster->CountPctFromMaxHealth(amount);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_bear_hug_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_bear_hug_AuraScript();
        }
};

// Ravage - 6785
class spell_dru_ravage : public SpellScriptLoader
{
    public:
        spell_dru_ravage() : SpellScriptLoader("spell_dru_ravage") { }

        class spell_dru_ravage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_ravage_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                        _player->CastSpell(target, SPELL_DRUID_INFECTED_WOUNDS, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_ravage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_ravage_SpellScript();
        }
};

// Mark of the Wild - 1126
class spell_dru_mark_of_the_wild : public SpellScriptLoader
{
    public:
        spell_dru_mark_of_the_wild() : SpellScriptLoader("spell_dru_mark_of_the_wild") { }

        class spell_dru_mark_of_the_wild_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_mark_of_the_wild_SpellScript);

            void HandleOnHit()
            {
                Unit* caster = GetCaster();
                if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    caster->AddAura(SPELL_DRUID_MARK_OF_THE_WILD, caster);

                    std::list<Unit*> memberList;
                    Player* plr = caster->ToPlayer();
                    plr->GetPartyMembers(memberList);

                    for (auto itr : memberList)
                        caster->AddAura(SPELL_DRUID_MARK_OF_THE_WILD, (itr));
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_mark_of_the_wild_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_mark_of_the_wild_SpellScript();
        }
};

// Called by Regrowth - 8936
// Glyph of Regrowth - 116218
class spell_dru_glyph_of_regrowth : public SpellScriptLoader
{
    public:
        spell_dru_glyph_of_regrowth() : SpellScriptLoader("spell_dru_glyph_of_regrowth") { }

        class spell_dru_glyph_of_regrowth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_glyph_of_regrowth_AuraScript);

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Increases the critical strike chance of your Regrowth by 40%, but removes the periodic component of the spell.
                if (GetCaster())
                    if (GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_REGROWTH))
                        GetTarget()->RemoveAura(SPELL_DRUID_REGROWTH, GetCaster()->GetGUID());
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                // Duration automatically refreshes to 6 sec each time Regrowth heals targets at or below 50% health
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetTarget())
                        if (target->GetHealthPct() < 50)
                            if (Aura *regrowth = target->GetAura(SPELL_DRUID_REGROWTH, caster->GetGUID()))
                                regrowth->RefreshDuration();
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_regrowth_AuraScript::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_glyph_of_regrowth_AuraScript::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_glyph_of_regrowth_AuraScript();
        }
};

// 768 - Cat Form
class spell_dru_cat_form : public AuraScript
{
    PrepareAuraScript(spell_dru_cat_form);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* druid = GetUnitOwner();
        if (!druid->HasAura(SPELL_DRUID_FORM_CAT_INCREASE_SPEED))
        {
            druid->CastSpell(druid, SPELL_DRUID_FORM_CAT_INCREASE_SPEED, true);
            druid->RemoveMovementImpairingAuras();
        }

        if (AuraEffect* dash = druid->GetAuraEffect(SPELL_DRUID_DASH, EFFECT_0))
            if (dash->GetAmount() == 0)
                dash->RecalculateAmount();

        if (druid->HasAura(SPELL_DRUID_SAVAGE_ROAR) || druid->HasAura(SPELL_DRUID_SAVAGE_ROAR_GLYPHED))
            druid->CastSpell(druid, SPELL_DRUID_SAVAGE_ROAR_EFFECT);

        druid->RemoveAurasDueToSpell(SPELL_DRUID_VENGEANCE);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* druid = GetUnitOwner();
        if (AuraEffect* dash = druid->GetAuraEffect(SPELL_DRUID_DASH, EFFECT_0))
            dash->ChangeAmount(0, false);

        if (druid->HasAura(SPELL_DRUID_PROWL))
            druid->RemoveAura(SPELL_DRUID_PROWL);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_cat_form::HandleApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_cat_form::HandleRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Skull Bash (cat) - 80965 and Skull Bash (bear) - 80964
class spell_dru_skull_bash : public SpellScriptLoader
{
    public:
        spell_dru_skull_bash() : SpellScriptLoader("spell_dru_skull_bash") { }

        class spell_dru_skull_bash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_skull_bash_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        _player->CastSpell(target, SPELL_DRUID_SKULL_BASH_MANA_COST, true);
                        _player->CastSpell(target, SPELL_DRUID_SKULL_BASH_INTERUPT, true);
                        _player->CastSpell(target, SPELL_DRUID_SKULL_BASH_CHARGE, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_skull_bash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_skull_bash_SpellScript();
        }
};

// 102355 - Faerie Swarm
class spell_dru_faerie_swarm : public SpellScript
{
    PrepareSpellScript(spell_dru_faerie_swarm);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_FAERIE_DECREASE_SPEED, true);
        GetCaster()->CastCustomSpell(SPELL_DRUID_WEAKENED_ARMOR, SPELLVALUE_AURA_STACK, 3, GetHitUnit(), true);

        if (GetCaster()->GetShapeshiftForm() != FORM_BEAR)
            SetHitDamage(0);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_faerie_swarm::HandleHit);
    }
};

// 127663 - Astral Communion
class spell_dru_astral_communion : public AuraScript
{
    PrepareAuraScript(spell_dru_astral_communion);

    bool hasAstralInsight = false;
    bool lunar = false;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        bool lunarMarker = GetUnitOwner()->HasAura(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);
        bool solarMarker = GetUnitOwner()->HasAura(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);

        if (lunarMarker || !solarMarker)
            lunar = true;

        if (GetUnitOwner()->HasAura(SPELL_DRUID_ASTRAL_INSIGHT))
        {
            hasAstralInsight = true;
            HandleTick(nullptr);
        }
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* druid = GetUnitOwner();
        if (druid->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT))
            return;

        int32 eclipse = hasAstralInsight ? 100 : 25;
        if (lunar)
            eclipse = -eclipse;

        druid->CastCustomSpell(SPELL_DRUID_ECLIPSE_ENERGIZE, SPELLVALUE_BASE_POINT0, eclipse, druid, true);

        if (hasAstralInsight)
        {
            druid->RemoveAurasDueToSpell(SPELL_DRUID_ASTRAL_INSIGHT);
            SetDuration(100);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_astral_communion::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_astral_communion::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 112071 - Celestial Alignment
class spell_dru_celestial_alignment : public SpellScript
{
    PrepareSpellScript(spell_dru_celestial_alignment);

    void HandleHit()
    {
        if (Player* druid = GetCaster()->ToPlayer())
        {
            if (!druid->HasAura(SPELL_DRUID_ECLIPSE_MARKER_LUNAR))
            {
                druid->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);
                druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_MARKER_LUNAR, true);
            }

            druid->SetPower(POWER_ECLIPSE, 0);
            druid->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
            druid->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE);

            druid->CastSpell(druid, SPELL_DRUID_NATURES_GRACE, true);
            druid->CastSpell(druid, SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE, true);
            druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, true);

            if (druid->HasSpellCooldown(SPELL_DRUID_STARFALL))
                druid->RemoveSpellCooldown(SPELL_DRUID_STARFALL, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_celestial_alignment::HandleHit);
    }
};

// Shooting Stars - 93400
class spell_dru_shooting_stars : public SpellScriptLoader
{
    public:
        spell_dru_shooting_stars() : SpellScriptLoader("spell_dru_shooting_stars") { }

        class spell_dru_shooting_stars_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_shooting_stars_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (GetHitUnit())
                        _player->RemoveSpellCooldown(SPELL_DRUID_STARSURGE, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_shooting_stars_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_shooting_stars_SpellScript();
        }
};

// 22842 - Frenzied Regeneration
class spell_dru_frenzied_regeneration : public SpellScript
{
    PrepareSpellScript(spell_dru_frenzied_regeneration);

    void HandleHit()
    {
        Unit* druid = GetCaster()->ToPlayer();
        if (!druid)
            return;

        if (druid->HasAura(SPELL_DRUID_GLYPH_OF_FRENZIED_REGEN))
        {
            SetHitHeal(0);
            druid->CastSpell(druid, SPELL_DRUID_FRENZIED_REGEN_HEAL_TAKE, true);
        }
        else
        {
            int32 rageused = std::min(600, GetCaster()->GetPower(POWER_RAGE));
            int32 ap = druid->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 agility = druid->GetStat(STAT_AGILITY);
            int32 stamina = druid->GetStat(STAT_STAMINA);

            bool triggered = GetSpell()->TriggeredByAuraSpell() && GetSpell()->TriggeredByAuraSpell()->Id == SPELL_DRUID_T16_GUARDIAN_2P_BONUS;
            if (triggered)
                rageused = 200;

            int32 healAmount = std::max(int32(2 * (ap - agility * 2)), int32(stamina * 2.5f));
            healAmount = rageused * healAmount / 600;

            if (!healAmount)
                GetSpell()->SetSpellValue(SPELLVALUE_BASE_POINT0, 0);

            healAmount = druid->SpellHealingBonusTaken(druid, GetSpellInfo(), EFFECT_0, healAmount, SPELL_DIRECT_DAMAGE);
            druid->ToPlayer()->ApplySpellMod(GetSpellInfo()->Id, SPELLMOD_DAMAGE, healAmount);

            SetHitHeal(healAmount);

            if (!triggered)
                druid->EnergizeBySpell(druid, GetSpellInfo()->Id, -rageused, POWER_RAGE);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_frenzied_regeneration::HandleHit);
    }
};

// Stampeding Roar - 106898, Stampeding Roar (cat) - 77764 and Stampeding Roar(bear) - 77761
class spell_dru_stampeding_roar_speed : public SpellScriptLoader
{
    public:
        spell_dru_stampeding_roar_speed() : SpellScriptLoader("spell_dru_stampeding_roar_speed") { }

        class spell_dru_stampeding_roar_speed_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_stampeding_roar_speed_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(SPELL_DRUID_DASH))
                        if (_player->HasAura(GetSpellInfo()->Id))
                            _player->RemoveAura(GetSpellInfo()->Id);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_stampeding_roar_speed_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_stampeding_roar_speed_SpellScript();
        }
};

// 106898 - Stampeding Roar, 77764 - Stampeding Roar (cat), 77761 - Stampeding Roar(bear)
class spell_dru_stampeding_roar : public SpellScript
{
    PrepareSpellScript(spell_dru_stampeding_roar);

    void HandleHit()
    {
        Unit* druid = GetCaster();
        GetHitUnit()->RemoveMovementImpairingAuras();

        if (!druid->HasAura(SPELL_DRUID_GLYPH_OF_STAMPEDING_ROAR) && druid->GetShapeshiftForm() != FORM_CAT && druid->GetShapeshiftForm() != FORM_BEAR)
            druid->CastSpell(druid, SPELL_DRUID_BEAR_FORM, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_stampeding_roar::HandleHit);
    }
};

// 29166, 54833 - Innervate
class spell_dru_innervate : public spirit_based_aura_effect
{
    PrepareAuraScript(spell_dru_innervate);

    void CalculateAmount(AuraEffect const* eff, float& amount, bool&)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        float spirit = GetSpiritWithoutShortDurationBonuses(caster);
        int32 spiritAmount = CalculatePct(spirit, 50);
        int32 manaAmount = CalculatePct(int32(caster->GetMaxPower(POWER_MANA) / eff->GetTotalTicks()), 8);
        amount = std::max(spiritAmount, manaAmount);
        if (AuraEffect const* eff = caster->GetAuraEffect(SPELL_DRUID_GLYPH_OF_INNERVATE, EFFECT_0))
            if (caster != GetUnitOwner() || GetId() == SPELL_DRUID_INNERVATE_SELFCASTED)
                AddPct(amount, -eff->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_innervate::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
    }
};

// 29166 - Innervate
class spell_dru_glyph_of_innervate : public SpellScript
{
    PrepareSpellScript(spell_dru_glyph_of_innervate);

    void HandleHit()
    {
        if (GetHitUnit() != GetCaster())
            if (GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_INNERVATE))
                GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_INNERVATE_SELFCASTED, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_glyph_of_innervate::HandleHit);
    }
};

// Teleport : Moonglade - 18960
class spell_dru_teleport_moonglade : public SpellScript
{
    PrepareSpellScript(spell_dru_teleport_moonglade);

    SpellCastResult CheckCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetMap()->IsDungeon())
                return SPELL_FAILED_NOT_HERE;

            if (Battleground* bg = player->GetBattleground())
                if (bg->IsBattleground() && bg->IsRatedBG())
                    return SPELL_FAILED_NOT_HERE;
        }

        return SPELL_CAST_OK;
    }

    void HandleBeforeCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (player->GetZoneId() != 493)
                player->SaveRecallPosition();
    }

    void SelectDest(SpellDestination& dest)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetZoneId() == 493)
            {
                Position pos = { player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO };
                dest._position.m_mapId = player->m_recallMap;
                dest.Relocate(pos);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_teleport_moonglade::CheckCast);
        BeforeCast += SpellCastFn(spell_dru_teleport_moonglade::HandleBeforeCast);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dru_teleport_moonglade::SelectDest, EFFECT_0, TARGET_UNIT_CASTER);
    }
};

// 6795 - Growl
// 106922 - Might of Ursoc
class spell_dru_growl : public SpellScript
{
    PrepareSpellScript(spell_dru_growl);

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_BEAR_FORM, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_growl::HandleCast);
    }
};

// 5215, 102547 - Prowl, 102280 - Displacer Beast, 1850 - Dash
class spell_dru_activate_cat_form : public SpellScript
{
    PrepareSpellScript(spell_dru_activate_cat_form);

    void HandleHit()
    {
        if (!GetCaster()->HasAura(SPELL_DRUID_CAT_FORM))
            GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_CAT_FORM, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_activate_cat_form::HandleHit);
    }
};

// 5176 - Wrath, 2912 - Starfire and 78674 - Starsurge
class spell_dru_eclipse : public SpellScript
{
    PrepareSpellScript(spell_dru_eclipse);

    bool Load() override
    {
        Player* caster = GetCaster()->ToPlayer();
        return caster && caster->GetTalentSpecialization() == SPEC_DRUID_BALANCE;
    }

    void HandleCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        Unit* target = GetExplTargetUnit();
        if (!target)
            return;

        if (GetSpellInfo()->Id == SPELL_DRUID_STARSURGE)
        {
            auto const& mods = GetAppliedSpellMods();
            AuraEffect* aurEff = caster->GetAuraEffect(SPELL_DRUID_SHOOTING_STARS, EFFECT_0);

            if (aurEff && mods.find(aurEff->GetBase()) == mods.end())
                caster->ToPlayer()->RemoveSpellCooldown(GetSpellInfo()->Id, true);
        }

        if (!caster->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT))
        {
            // Power stored in EFFECT_1
            int32 eclipse = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
            int32 playersEclipse = caster->GetPower(POWER_ECLIPSE);

            bool lunarMarker = caster->HasAura(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);
            bool solarMarker = caster->HasAura(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);

            // Wrath must have negative, Starsurge depends on actual direction of Eclipse
            if (GetSpellInfo()->Id == SPELL_DRUID_WRATH || (
                GetSpellInfo()->Id == SPELL_DRUID_STARSURGE && (lunarMarker || !solarMarker)))
                eclipse = -eclipse;

            // - Is Lunar + is Solar
            // Solar Energy
            if (eclipse > 0 && lunarMarker)
                return;

            // Lunar energy
            if (eclipse < 0 && solarMarker)
                return;

            if (caster->HasAura(SPELL_DRUID_EUPHORIA) && !caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && !caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE))
                eclipse *= 2;

            caster->CastCustomSpell(SPELL_DRUID_ECLIPSE_ENERGIZE, SPELLVALUE_BASE_POINT0, eclipse, caster, true);
        }
    }

    void HandleHit()
    {
        Player* player = GetCaster()->ToPlayer();
        Unit* target = GetHitUnit();

        // Your crits also increase moonfire/sunfire duration by 2s depending on spell
        if (GetSpell()->IsCritForTarget(target))
        {
            if (GetSpellInfo()->Id != SPELL_DRUID_WRATH)
            {
                if (AuraEffect* moonfire = target->GetAuraEffect(SPELL_DRUID_MOONFIRE, EFFECT_0, GetCaster()->GetGUID()))
                {
                    Aura* aura = moonfire->GetBase();
                    aura->SetDuration(aura->GetDuration() + moonfire->GetAmplitude());
                    if (aura->GetMaxDuration() < aura->GetDuration())
                        aura->SetMaxDuration(aura->GetDuration());
                }
            }
            if (GetSpellInfo()->Id != SPELL_DRUID_STARFIRE)
            {
                if (AuraEffect* sunfire = target->GetAuraEffect(SPELL_DRUID_SUNFIRE, EFFECT_0, GetCaster()->GetGUID()))
                {
                    Aura* aura = sunfire->GetBase();
                    aura->SetDuration(aura->GetDuration() + sunfire->GetAmplitude());
                    if (aura->GetMaxDuration() < aura->GetDuration())
                        aura->SetMaxDuration(aura->GetDuration());
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_eclipse::HandleHit);
        AfterCast += SpellCastFn(spell_dru_eclipse::HandleCast);
    }
};

// 89265 - Eclipse Energy
class spell_dru_eclipse_energize : public SpellScript
{
    PrepareSpellScript(spell_dru_eclipse_energize);

    void HandleHit(SpellEffIndex)
    {
        int32 currentPower = GetHitUnit()->GetPower(POWER_ECLIPSE);
        int32 mod = GetEffectValue();
        if ((currentPower == 100 && mod > 0) ||
            (currentPower == -100 && mod < 0))
            mod = 0;

        if (mod > 0)
        {
            GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DRUID_ECLIPSE_MARKER_SOLAR, true);
            if (currentPower + mod > 100)
                mod = 100 - currentPower;
        }
        else if (mod < 0)
        {
            GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DRUID_ECLIPSE_MARKER_LUNAR, true);
            if (currentPower + mod < -100)
                mod = -(100 + currentPower);
        }

        if (currentPower + mod > 0)
        {
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE);
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE);
        }
        else if (currentPower + mod < 0)
        {
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
        }
        else
        {
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE);
            GetHitUnit()->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE);
        }

        SetEffectValue(mod);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_eclipse_energize::HandleHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// 48517 - Eclipse(Solar)
class spell_dru_eclipse_solar : public SpellScript
{
    PrepareSpellScript(spell_dru_eclipse_solar);

    void HandleCast()
    {
        Unit* druid = GetCaster();
        druid->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);
        druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_MARKER_LUNAR, true);
        druid->CastSpell(druid, SPELL_DRUID_NATURES_GRACE, true);               // Cast Nature's Grace
        druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, true);    // Cast Eclipse - Give 35% of POWER_MANA
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_eclipse_solar::HandleCast);
    }
};

//  48518 - Eclipse (Lunar)
class spell_dru_eclipse_lunar : public SpellScript
{
    PrepareSpellScript(spell_dru_eclipse_lunar);

    void HandleCast()
    {
        Unit* druid = GetCaster();

        druid->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);
        druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_MARKER_SOLAR, true);
        druid->CastSpell(druid, SPELL_DRUID_NATURES_GRACE, true);               // Cast Nature's Grace
        druid->CastSpell(druid, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, true);    // Cast Eclipse - Give 35% of POWER_MANA
        druid->CastSpell(druid, SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE, true);

        // Entering Lunar Eclipse restarts Starfall cooldown
        druid->GetSpellHistory()->RemoveCooldown(SPELL_DRUID_STARFALL, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_eclipse_lunar::HandleCast);
    }
};

// 99 - Disorienting Roar
// 339 - Entangling Roots
// 770 - Faerie Fire
// 5211 - Mighty Bash
// 33786 - Cyclone
// 102355 - Faerie Swarm
// 102359 - Mass Entanglement
// 102793 - Ursol's Vortex
// 132469 - Typhoon
class spell_dru_glyph_of_omens : public SpellScript
{
    PrepareSpellScript(spell_dru_glyph_of_omens);

    bool Load() override
    {
        return  GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_OMENS);
    }

    void HandleCast()
    {
        Player* druid = GetCaster()->ToPlayer();
        if (!druid->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && !druid->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && !druid->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT))
        {
            bool lunarMarker = GetCaster()->HasAura(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);
            bool solarMarker = GetCaster()->HasAura(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);

            int32 eclipse = 10;
            if (lunarMarker || !solarMarker)
                eclipse = -eclipse;

            druid->CastCustomSpell(SPELL_DRUID_ECLIPSE_ENERGIZE, SPELLVALUE_BASE_POINT0, eclipse, druid, true);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_glyph_of_omens::HandleCast);
    }
};

// 79577 - Eclipse
class spell_dru_eclipse_remove : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_remove);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* druid = GetUnitOwner();
        druid->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);
        druid->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);

        druid->SetPower(POWER_ECLIPSE, 0);
        druid->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
        druid->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE);
        druid->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE_OVERRIDE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_eclipse_remove::HandleRemove, EFFECT_0, SPELL_AURA_PROC_ON_POWER_AMOUNT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 48517 - Eclipse (Solar)
// 48518 - Eclipse (Lunar)
class spell_dru_eclipse_effect : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_effect);

    void CalculateAmount(AuraEffect const*, float&, bool& canBeRecalulated)
    {
        canBeRecalulated = false;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_eclipse_effect::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

class spell_dru_t10_restoration_4p_bonus : public SpellScriptLoader
{
    public:
        spell_dru_t10_restoration_4p_bonus() : SpellScriptLoader("spell_dru_t10_restoration_4p_bonus") { }

        class spell_dru_t10_restoration_4p_bonus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_t10_restoration_4p_bonus_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster()->ToPlayer()->GetGroup())
                {
                    targets.clear();
                    targets.push_back(GetCaster());
                }
                else
                {
                    targets.remove(GetExplTargetUnit());
                    std::list<Unit*> tempTargets;
                    for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                        if ((*itr)->GetTypeId() == TYPEID_PLAYER && GetCaster()->IsInRaidWith((*itr)->ToUnit()))
                            tempTargets.push_back((*itr)->ToUnit());

                    if (tempTargets.empty())
                    {
                        targets.clear();
                        FinishCast(SPELL_FAILED_DONT_REPORT);
                        return;
                    }

                    Unit* target = Trinity::Containers::SelectRandomContainerElement(tempTargets);
                    targets.clear();
                    targets.push_back(target);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_t10_restoration_4p_bonus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_t10_restoration_4p_bonus_SpellScript();
        }
};

// 40121 - Swift Flight Form (Passive)
class spell_dru_swift_flight_passive : public SpellScriptLoader
{
    public:
        spell_dru_swift_flight_passive() : SpellScriptLoader("spell_dru_swift_flight_passive") { }

        class spell_dru_swift_flight_passive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_swift_flight_passive_AuraScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Player* caster = GetCaster()->ToPlayer())
                    if (caster->GetSkillValue(SKILL_RIDING) >= 375)
                        amount = 310;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_passive_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_swift_flight_passive_AuraScript();
        }
};

// 52610, 127538 - Savage Roar
class spell_dru_savage_roar : public AuraScript
{
    PrepareAuraScript(spell_dru_savage_roar);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->GetShapeshiftForm() == FORM_CAT)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_SAVAGE_ROAR_EFFECT, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_SAVAGE_ROAR_EFFECT);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_savage_roar::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_savage_roar::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 52610, 127538 - Savage Roar
class spell_dru_savage_roar_duration_check : public SpellScript
{
    PrepareSpellScript(spell_dru_savage_roar_duration_check);

    SpellCastResult CheckCast()
    {
        if (Player* druid = GetCaster()->ToPlayer())
        {
            if (Aura* savageRoar = GetCaster()->GetAura(GetSpellInfo()->Id))
            {
                int32 newDuration = 12000;
                if (savageRoar->HasEffect(EFFECT_2))
                    newDuration += savageRoar->GetEffect(EFFECT_2)->GetPeriodicTimer();
                if (int32 cp = GetCaster()->ToPlayer()->GetComboPoints())
                    newDuration += (cp * 6000);

                if (savageRoar->GetDuration() > newDuration)
                    return SPELL_FAILED_STRONGER_SPELL_IN_PROGRESS;
            }
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_savage_roar_duration_check::CheckCast);
    }
};

// Survival Instincts - 61336
class spell_dru_survival_instincts : public SpellScriptLoader
{
    public:
        spell_dru_survival_instincts() : SpellScriptLoader("spell_dru_survival_instincts") { }

        class spell_dru_survival_instincts_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_survival_instincts_AuraScript);

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), DRUID_SURVIVAL_INSTINCTS, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(DRUID_SURVIVAL_INSTINCTS);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dru_survival_instincts_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dru_survival_instincts_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_survival_instincts_AuraScript();
        }
};

// 145108 - Ysera's Gift
class spell_dru_yseras_gift : public SpellScriptLoader
{
    public:
        spell_dru_yseras_gift() : SpellScriptLoader("spell_dru_yseras_gift") { }

        class spell_dru_yseras_gift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_yseras_gift_AuraScript)

            enum
            {
                YSERAS_GIFT_HEAL_SELF = 145109,
                YSERAS_GIFT_HEAL_ALLY = 145110
            };

            void OnTick(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                auto const caster = GetCaster();
                if (!caster || caster->isDead() || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                uint32 spellId = YSERAS_GIFT_HEAL_SELF;

                if (caster->GetHealth() == caster->GetMaxHealth()) {
                    // No point in casting anything if character is not in group
                    if (!caster->ToPlayer()->GetGroup())
                        return;
                    spellId = YSERAS_GIFT_HEAL_ALLY;
                }

                // It seems that it heals for 5% of druid's health for both cases
                int32 const bp0 = caster->CountPctFromMaxHealth(aurEff->GetAmount());
                caster->CastCustomSpell(caster, spellId, &bp0, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_yseras_gift_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dru_yseras_gift_AuraScript();
        }
};

// Ysera's Gift (ally heal) - 145110
class spell_dru_yseras_gift_heal_ally : public SpellScriptLoader
{
    public:
        spell_dru_yseras_gift_heal_ally() : SpellScriptLoader("spell_dru_yseras_gift_heal_ally") { }

        class spell_dru_yseras_gift_heal_ally_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_yseras_gift_heal_ally_SpellScript)

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetCaster());

                if (targets.empty())
                    return;

                // Can't use Trinity::HealthPctOrderPred for WorldObject types
                auto const mostInjuredItr = std::min_element(targets.cbegin(), targets.cend(),
                    [](WorldObject const* a, WorldObject const* b)
                    {
                        // This spell can not target anything except units, so no check
                        return a->ToUnit()->GetHealthPct() < b->ToUnit()->GetHealthPct();
                    });

                auto const mostInjured = (*mostInjuredItr)->ToUnit();
                targets.clear();

                // Do not cast anything if all group members are at full health
                if (mostInjured->GetHealth() != mostInjured->GetMaxHealth())
                    targets.emplace_back(mostInjured);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_yseras_gift_heal_ally_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dru_yseras_gift_heal_ally_SpellScript();
        }
};

// 18562 - Swiftmend
class spell_dru_swiftmend : public SpellScript
{
    PrepareSpellScript(spell_dru_swiftmend);

    void HandleHit()
    {
        if (!GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_EFFLORESCENCE))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_EFFLORESCENCE, true);
        if (GetCaster()->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST))
            GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_SOUL_OF_THE_FOREST_HASTE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_swiftmend::HandleHit);
    }
};

// 81262, 142423 - Efflorescence
class spell_dru_efflorescence : public AuraScript
{
    PrepareAuraScript(spell_dru_efflorescence);

    void CalculatePeriodic(AuraEffect const*, bool&, int32& amplitude)
    {
        if (Unit* owner = GetUnitOwner()->GetOwner())
            if (owner->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST_HASTE))
                amplitude *= 2;
    }

    void HandleTick(AuraEffect const*)
    {
        if (Player* druid = GetUnitOwner()->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (DynamicObject* dynObj = GetUnitOwner()->GetDynObject(GetSpellInfo()->Id))
            {
                uint32 spellId = GetSpellInfo()->Id == SPELL_DRUID_EFFLORESCENCE ? SPELL_DRUID_EFFLORESCENCE_TICK : SPELL_DRUID_EFFLORESCENCE_TICK_TREANTS;
                druid->CastSpell(*dynObj, spellId, true);
            }
        }
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_dru_efflorescence::CalculatePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_efflorescence::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 81269 - Efflorescence (heal effect)
class spell_dru_efflorescence_heal : public spell_smart_heal
{
    PrepareSpellScript(spell_dru_efflorescence_heal);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 count = GetCaster()->HasAura(SPELL_DRUID_T15_RESTORATION_2P_BONUS) ? 4 : 3;
        ApplySmartHeal(targets, count);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_efflorescence_heal::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 770 - Faerie Fire
class spell_dru_faerie_fire : public SpellScript
{
    PrepareSpellScript(spell_dru_faerie_fire);

    void HandleOnHit()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;
        Unit* target = GetHitUnit();
        player->CastSpell(target, SPELL_DRUID_WEAKENED_ARMOR, true);
        if (Aura* aura = target->GetAura(SPELL_DRUID_WEAKENED_ARMOR))
            aura->ModStackAmount(2);

        if (player->GetShapeshiftForm() == FORM_BEAR)
        {
            if (roll_chance_i(25))
                player->RemoveSpellCooldown(SPELL_DRUID_MANGLE_BEAR, true);
        }
        else
            SetHitDamage(0);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_faerie_fire::HandleOnHit);
    }
};

// 33745 - Lacerate
class spell_dru_lacerate : public SpellScript
{
    PrepareSpellScript(spell_dru_lacerate);

    void HandleOnHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (roll_chance_i(25))
                player->RemoveSpellCooldown(SPELL_DRUID_MANGLE_BEAR, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_lacerate::HandleOnHit);
    }
};

// 16961 - Primal Fury
class spell_dru_primal_fury : public AuraScript
{
    PrepareAuraScript(spell_dru_primal_fury);

    void HandleProc(ProcEventInfo& procEvent)
    {
        PreventDefaultAction();

        Unit* druid = GetUnitOwner();
        SpellInfo const* spellInfo = procEvent.GetSpellInfo();
        switch (druid->GetShapeshiftForm())
        {
            case FORM_BEAR:
                if (!spellInfo || spellInfo->Id == SPELL_DRUID_MANGLE_BEAR)
                    druid->CastSpell(druid, SPELL_DRUID_PRIMAL_FURY_BEAR, true);
                break;
            case FORM_CAT:
                if (spellInfo)
                    druid->CastSpell(procEvent.GetProcTarget(), SPELL_DRUID_PRIMAL_FURY_CAT, true);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_primal_fury::HandleProc);
    }
};

// 22568 - Ferocious Bite
class spell_dru_ferocious_bite : public SpellScript
{
    PrepareSpellScript(spell_dru_ferocious_bite);

    void HandleHit()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        int32 energyCost = GetSpell()->GetPowerCost();

        if (int32 energyBoost = player->GetPower(POWER_ENERGY))
        {
            energyBoost = std::min(energyBoost, 25);
            SetHitDamage(GetHitDamage() * (1.0f + energyBoost / 25.0f));
            player->ModifyPower(POWER_ENERGY, -energyBoost);
            energyCost += energyBoost;
        }

        if (AuraEffect const* glyph = GetCaster()->GetAuraEffect(SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE, EFFECT_1))
            GetCaster()->CastCustomSpell(SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE_HEAL, SPELLVALUE_BASE_POINT0, energyCost * 2 / glyph->GetAmount(), GetCaster(), true);

        Unit* target = GetHitUnit();
        if (target->GetHealthPct() < 25.0f)
            if (Aura* rip = target->GetAura(SPELL_DRUID_RIP, player->GetGUID()))
                rip->RefreshDuration(false);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_ferocious_bite::HandleHit);
    }
};

// 77495 - Mastery: Harmony
class spell_dru_harmony : public AuraScript
{
    PrepareAuraScript(spell_dru_harmony);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& procEvent)
    {
        Unit* caster = GetCaster();
        int32 bp = mastery->GetAmount();
        caster->CastCustomSpell(caster, 100977, &bp, &bp, NULL, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_harmony::HandleProc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// 33891 - Incarnation: Tree of Life
class spell_dru_tree_of_life : public SpellScript
{
    PrepareSpellScript(spell_dru_tree_of_life);

    void HandleCast()
    {
        Unit* caster = GetCaster();
        if (!caster->HasAura(SPELL_DRUID_INCARNATION_AURA))
            caster->CastSpell(caster, SPELL_DRUID_INCARNATION_AURA, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_tree_of_life::HandleCast);
    }
};

// 48438 - Wild Growth
class spell_dru_wild_growth : public spell_smart_heal
{
    PrepareSpellScript(spell_dru_wild_growth);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        uint32 targetCount = GetSpellInfo()->Effects[EFFECT_2].CalcValue(GetCaster());
        if (GetCaster()->GetShapeshiftForm() == FORM_TREE)
            targetCount += 2;
        targets.remove_if(Trinity::NotInRaidWithCheck{ GetCaster() });
        ApplySmartHeal(targets, targetCount);
        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_DRUID_T16_RESTORATION_4P_BONUS))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_SPARK_OF_LIFE, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::CopyTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_dru_wild_growth::HandleHit);
    }
};

// 48438 - Wild Growth
class spell_dru_wild_growth_heal : public AuraScript
{
    PrepareAuraScript(spell_dru_wild_growth_heal);

    void CalculateTick(AuraEffect const* eff, int32& amount)
    {
        // Wild Growth = amount + (6 - 2*doneTicks) * ticks* amount / 100
        float middle = (eff->GetTotalTicks() - 1) / 2.0f;
        float diff = middle - (eff->GetTickNumber() - 1);
        float pct = diff / float(eff->GetTotalTicks());
        int32 addition = int32(amount) * pct;

        // Item - Druid T10 Restoration 2P Bonus
        if (Unit* caster = GetCaster())
            if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(70658, 0))
                addition += abs(int32((addition * aurEff->GetAmount()) / 50)); // divided by 50 instead of 100 because calculated as for every 2 tick

        amount = std::max(int32(amount) + addition, 0);
    }

    void Register() override
    {
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_dru_wild_growth_heal::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 5185 - Healing Touch 
class spell_dru_healing_touch_dream_of_cenarius : public SpellScript
{
    PrepareSpellScript(spell_dru_healing_touch_dream_of_cenarius);

    void CalculateDamage(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_DRUID_DREAM_OF_CENARIUS_GUARDIAN))
        {
            int32 spd = GetCaster()->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());
            int32 ap = GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK);

            // And benefit from Attack Power instead of Spell Power.
            // Just add difference betweed them.
            SetEffectValue(GetEffectValue() + (ap - spd) * GetSpellInfo()->Effects[effIndex].BonusMultiplier);
        }
    }

    void HandleHit()
    {
        if (Player* druid = GetCaster()->ToPlayer())
        {
            if (AuraEffect const* eff = druid->GetAuraEffect(SPELL_DRUID_DREAM_OF_CENARIUS_TALENT, EFFECT_1))
            {
                switch (druid->GetTalentSpecialization())
                {
                    case SPEC_DRUID_BALANCE:
                        druid->CastSpell(druid, SPELL_DRUID_DREAM_OF_CENARIUS_BALANCE, true);
                        break;
                    case SPEC_DRUID_FERAL:
                        druid->CastSpell(druid, SPELL_DRUID_DREAM_OF_CENARIUS_FERAL, true);
                        break;
                    case SPEC_DRUID_GUARDIAN:
                        break;
                    default:    // No proc, no benefits.
                        return;
                }
                SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), eff->GetAmount()));
            }
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_healing_touch_dream_of_cenarius::CalculateDamage, EFFECT_0, SPELL_EFFECT_HEAL);
        OnHit += SpellHitFn(spell_dru_healing_touch_dream_of_cenarius::HandleHit);
    }
};

// 5176 - Wrath
class spell_dru_wrath_dream_of_cenarius : public SpellScript
{
    PrepareSpellScript(spell_dru_wrath_dream_of_cenarius);

    void HandleHit()
    {
        if (Player* druid = GetCaster()->ToPlayer())
            if (druid->GetTalentSpecialization() == SPEC_DRUID_RESTORATION)
                if (AuraEffect const* eff = druid->GetAuraEffect(SPELL_DRUID_DREAM_OF_CENARIUS_TALENT, EFFECT_2))
                    SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), eff->GetAmount()));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_wrath_dream_of_cenarius::HandleHit);
    }
};

// 108373 - Dream of Cenarius
class spell_dur_dream_of_cenarius : public AuraScript
{
    PrepareAuraScript(spell_dur_dream_of_cenarius);

    Player* druid = nullptr;

    bool Load() override
    {
        druid = GetOwner()->ToPlayer();
        return druid != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        switch (druid->GetTalentSpecialization())
        {
            case SPEC_DRUID_RESTORATION:
                return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_DRUID_WRATH;
            case SPEC_DRUID_GUARDIAN:
                return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_DRUID_MANGLE_BEAR &&
                    eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT &&
                    roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
        }
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        switch (druid->GetTalentSpecialization())
        {
            case SPEC_DRUID_GUARDIAN:
                druid->CastSpell(druid, SPELL_DRUID_DREAM_OF_CENARIUS_GUARDIAN, true);
                break;
            case SPEC_DRUID_RESTORATION:
                druid->CastCustomSpell(SPELL_DRUID_DREAM_OF_CENARIUS_RESTOR, SPELLVALUE_BASE_POINT0,
                    eventInfo.GetDamageInfo()->GetDamage(), eventInfo.GetActionTarget(), true);
                break;
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dur_dream_of_cenarius::CheckProc);
        OnProc += AuraProcFn(spell_dur_dream_of_cenarius::HandleProc);
    }
};

// 88747 - Wild Mushroom (Balance), 145205 - Wild Mushroom (Restoration), 147349 - Wild Mushroom (Restoration with glyph)
class spell_dru_wild_mushroom : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_mushroom);

    int32 m_amount0 = 0;
    int32 m_amount1 = 0;

    void GetBonus()
    {
        Unit* druid = GetCaster();
        std::list<TempSummon*> mushrooms;
        druid->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
        if (mushrooms.empty())
            return;

        if (druid->HasAura(SPELL_DRUID_MUSHROOM_OVERHEAL_AURA))
        {
            if (Aura const* aura = mushrooms.front()->GetAura(SPELL_DRUID_MUSHROOM_ACCUMULATOR))
            {
                if (aura->HasEffect(EFFECT_0))
                    m_amount0 = aura->GetEffect(EFFECT_0)->GetAmount();
                if (aura->HasEffect(EFFECT_1))
                    m_amount1 = aura->GetEffect(EFFECT_1)->GetAmount();
            }
        }
    }

    void HandleCast()
    {
        Unit* druid = GetCaster();
        std::list<TempSummon*> mushrooms;
        druid->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
        if (!mushrooms.empty())
        {
            TempSummon* mushroom = mushrooms.back();
            mushroom->SetMaxHealth(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
            mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_BIRTH, true);
            mushroom->m_Events.Schedule(GetSpellInfo()->Effects[EFFECT_2].BasePoints * 1000,
                [=]
            {
                mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_INVISIBLE, true);
            });

            if (druid->HasAura(SPELL_DRUID_MUSHROOM_OVERHEAL_AURA))
            {
                // Apply grow buff for restor.
                CustomSpellValues vals;
                vals.AddSpellMod(SPELLVALUE_BASE_POINT0, m_amount0);
                vals.AddSpellMod(SPELLVALUE_BASE_POINT1, m_amount1);
                druid->CastCustomSpell(SPELL_DRUID_MUSHROOM_ACCUMULATOR, vals, mushroom);
                // With glyph efflorescence lasts as long as mushroom is active.
                if (druid->HasAura(SPELL_DRUID_GLYPH_OF_EFFLORESCENCE))
                {
                    mushroom->CastSpell(mushroom, SPELL_DRUID_EFFLORESCENCE, mushroom);
                    // Both auras, one is UnitAura, one is DynObjectAura
                    auto bounds = mushroom->GetAppliedAuras().equal_range(SPELL_DRUID_EFFLORESCENCE);
                    for (auto it = bounds.first; it != bounds.second; ++it)
                    {
                        if (it->second->GetBase()->GetCasterGUID() == mushroom->GetGUID())
                        {
                            it->second->GetBase()->SetMaxDuration(mushroom->GetTimer());
                            it->second->GetBase()->SetDuration(mushroom->GetTimer());
                        }
                    }
                }
            }

            // And now remove a summon that is over the limit.
            if (mushrooms.size() > size_t(GetSpellInfo()->Effects[EFFECT_1].BasePoints))
                mushrooms.front()->DespawnOrUnsummon();
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_dru_wild_mushroom::GetBonus);
        AfterCast += SpellCastFn(spell_dru_wild_mushroom::HandleCast);
    }
};

class spell_dru_wild_mushroom_action : public SpellScript
{
protected:
    SpellCastResult CheckCast()
    {
        std::list<TempSummon*> mushrooms;
        GetCaster()->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
        if (mushrooms.empty())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        for (auto&& mushroom : mushrooms)
            if (mushroom->IsAlive() && GetCaster()->IsWithinDistInMap(mushroom, 40.0f))
                return SPELL_CAST_OK;
        return SPELL_FAILED_OUT_OF_RANGE;
    }
};

// 88751 - Wild Mushroom: Detonate
class spell_dru_wild_mushroom_detonate : public spell_dru_wild_mushroom_action
{
    PrepareSpellScript(spell_dru_wild_mushroom_detonate);

    void HandleCast()
    {
        Unit* druid = GetCaster();
        std::list<TempSummon*> mushrooms;
        druid->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
        for (auto&& mushroom : mushrooms)
        {
            if (mushroom->IsAlive() && druid->IsWithinDistInMap(mushroom, 40.0f))
            {
                druid->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_DAMAGE, true);
                druid->CastSpell(*mushroom, SPELL_DRUID_FUNGAL_GROWTH, true);
                mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_DEATH, true);
                mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_SUICIDE, true);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_wild_mushroom_detonate::CheckCast);
        OnCast += SpellCastFn(spell_dru_wild_mushroom_detonate::HandleCast);
    }
};

// 102791 - Wild Mushroom: Bloom
class spell_dru_wild_mushroom_bloom : public spell_dru_wild_mushroom_action
{
    PrepareSpellScript(spell_dru_wild_mushroom_bloom);

    void HandleCast()
    {
        Unit* druid = GetCaster();
        std::list<TempSummon*> mushrooms;
        druid->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
        for (auto&& mushroom : mushrooms)
        {
            if (mushroom->IsAlive() && druid->IsWithinDistInMap(mushroom, 40.0f))
            {
                druid->CastSpell(*mushroom, SPELL_DRUID_MUSHROOM_HEAL, true);
                mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_DEATH, true);
                mushroom->CastSpell(mushroom, SPELL_DRUID_MUSHROOM_SUICIDE, true);
            }
        }
        druid->RemoveAurasDueToSpell(SPELL_DRUID_MUSHROOM_BLOOM_UI_FLASH);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_wild_mushroom_bloom::CheckCast);
        OnCast += SpellCastFn(spell_dru_wild_mushroom_bloom::HandleCast);
    }
};

// 102792 - Wild Mushroom: Bloom 
class spell_dru_wild_mushroom_heal : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_mushroom_heal);

    int32 bonusHeal = 0;

    void SelectTagets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target)
        { 
            float x, y, z;
            GetExplTargetDest()->GetPosition(x, y, z);
            if (!target->IsWithinLOS(x, y, z))
                return true;
            return target->GetEntry() == NPC_DRUID_WILD_MUSHROOM;
        });
        if (!targets.empty())
        {
            Unit* druid = GetCaster();
            std::list<TempSummon*> mushrooms;
            druid->GetSummons(mushrooms, NPC_DRUID_WILD_MUSHROOM);
            if (!mushrooms.empty())
                if (AuraEffect const* eff = mushrooms.front()->GetAuraEffect(SPELL_DRUID_MUSHROOM_ACCUMULATOR, EFFECT_1))
                    bonusHeal = eff->GetAmount() / int32(targets.size());
        }
    }

    void CalculateHeal(SpellEffIndex)
    {
        SetEffectValue(GetEffectValue() + bonusHeal);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_mushroom_heal::SelectTagets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_wild_mushroom_heal::CalculateHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 138611 - Wild Mushroom
class spell_dru_wild_mushroom_overheal : public AuraScript
{
    PrepareAuraScript(spell_dru_wild_mushroom_overheal);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 over = eventInfo.GetHealInfo()->GetOverheal();
        Player* druid = GetUnitOwner()->ToPlayer();
        if (!over || !druid)
            return;

        if (druid->GetMap()->IsBattlegroundOrArena())
            if (Battleground* bg = druid->GetBattleground())
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                    return;

        std::list<TempSummon*> summons;
        druid->GetSummons(summons, NPC_DRUID_WILD_MUSHROOM);
        if (summons.empty())
            return;

        Unit* mushroom = summons.front();
        AuraEffect* scale = mushroom->GetAuraEffect(SPELL_DRUID_MUSHROOM_ACCUMULATOR, EFFECT_0);
        AuraEffect* bonus = mushroom->GetAuraEffect(SPELL_DRUID_MUSHROOM_ACCUMULATOR, EFFECT_1);
        if (!scale || !bonus)
            return;

        int32 max = druid->GetMaxHealth() * 2;
        int32 val = std::min(bonus->GetAmount() + over, max);
        if (bonus->GetAmount() != val)
        {
            int32 scaleAmount = val * 100.f / max;
            scale->ChangeAmount(scaleAmount);
            bonus->ChangeAmount(val);
            if (val == max && !druid->HasAura(SPELL_DRUID_MUSHROOM_BLOOM_UI_FLASH))
                druid->CastSpell(druid, SPELL_DRUID_MUSHROOM_BLOOM_UI_FLASH, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_wild_mushroom_overheal::HandleProc);
    }
};

// 47649
struct npc_wild_mushroom : public NullCreatureAI
{
    npc_wild_mushroom(Creature* me) : NullCreatureAI(me) { }

    void Unsummoned() override
    {
        if (me->IsSummon() && !me->ToTempSummon()->GetTimer())
            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                summoner->RemoveAurasDueToSpell(SPELL_DRUID_MUSHROOM_BLOOM_UI_FLASH);
    }

    void JustDied(Unit*) override
    {
        if (me->IsSummon())
            me->DespawnOrUnsummon(1);
    }
};

// 43484
struct npc_fungal_growth : public SpellDummyAI
{
    npc_fungal_growth(Creature* me) : SpellDummyAI(me)
    {
        me->CastSpell(me, SPELL_DRUID_MUSHROOM_SLOW_VISUAL, true);
        me->CastSpell(me, SPELL_DRUID_FUNGAL_GROWTH_SLOW,  true);
    }
};

// 33763 - Lifebloom
class spell_dru_lifebloom : public AuraScript
{
    PrepareAuraScript(spell_dru_lifebloom);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->GetShapeshiftForm() != FORM_TREE)
            {
                if (auto auras = caster->GetBoundAurasBySpellId(GetId()))
                {
                    Aura* last = auras->back();
                    if (last->GetDuration() >= 2000)
                        SetStackAmount(last->GetStackAmount());
                    last->Remove();
                }
                caster->BindAura(GetAura());
            }
        }
    }

    void CalulateAmount(AuraEffect const* eff, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            amount = caster->SpellHealingBonusDone(GetUnitOwner(), GetSpellInfo(), eff->GetEffIndex(), amount, HEAL);
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        int32 amount = eff->GetAmount();
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_DRUID_GLYPH_OF_BLOOMING))
                AddPct(amount, 50);
        CustomSpellValues ctx;
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, amount);
        ctx.SetHealingBonusCalculated();
        GetUnitOwner()->CastCustomSpell(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, ctx, GetUnitOwner(), TRIGGERED_FULL_MASK, nullptr, nullptr, GetCasterGUID());
    }

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (AuraEffect const* eff = GetEffect(EFFECT_1))
            GetUnitOwner()->CastCustomSpell(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, SPELLVALUE_BASE_POINT0, eff->GetAmount(), GetUnitOwner(), true, nullptr, nullptr, GetCasterGUID());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_lifebloom::CalulateAmount, EFFECT_1, SPELL_AURA_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_dru_lifebloom::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_lifebloom::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterDispel += AuraDispelFn(spell_dru_lifebloom::HandleDispel);
    }
};

// 8936 - Regrowth, 50464 - Nourish, 5185 - Healing Touch
class spell_dru_lifebloom_refresh : public SpellScript
{
    PrepareSpellScript(spell_dru_lifebloom_refresh);

    void HandleHit()
    {
        if (!GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_BLOOMING))
            if (Aura* lifebloom = GetHitUnit()->GetAura(SPELL_DRUID_LIFEBLOOM, GetCaster()->GetGUID()))
                lifebloom->RefreshTimers(true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_lifebloom_refresh::HandleHit);
    }
};

// 62078 - Swipe
class spell_dru_swipe : public SpellScript
{
    PrepareSpellScript(spell_dru_swipe);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Player* druid = GetCaster()->ToPlayer())
            targets.remove_if([=](WorldObject* target) { return target->GetGUID() != druid->GetTarget(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_swipe::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 314. Summoned by 102793 - Ursol's Vortex
class sat_druid_ursols_vortex : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        Unit* unit = triggering->ToUnit();
        return unit && m_caster->IsValidAttackTarget(unit);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        m_caster->CastSpell(triggering->ToUnit(), SPELL_DRUID_URSOLS_VORTEX_SNARE, true);
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (!triggering->ToUnit()->HasAura(SPELL_DRUID_URSOLS_VORTEX_SNARE))
            GetCaster()->CastSpell(triggering->ToUnit(), SPELL_DRUID_URSOLS_VORTEX_SNARE, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        Unit* target = triggering->ToUnit();
        bool immunedToKnockback = false;
        if (Player* player = target->ToPlayer())
            immunedToKnockback = player->IsImmueToPvpKnockback();

        if (!target->HasAura(SPELL_DRUID_URSOLS_VORTEX_JUMP_DEST) && !m_duringRemoveFromWorld)
            if (auto spellInfo = sSpellMgr->GetSpellInfo(SPELL_DRUID_URSOLS_VORTEX_JUMP_DEST))
                if (!immunedToKnockback && !target->IsImmunedToSpell(spellInfo, MAX_EFFECT_MASK) && 
                    !target->IsImmunedToDamage(spellInfo) && !GetCaster()->MagicSpellHitResult(target, spellInfo)) // Self casted
                    target->CastSpell(*GetTarget(), SPELL_DRUID_URSOLS_VORTEX_JUMP_DEST, true);

        target->RemoveAurasDueToSpell(SPELL_DRUID_URSOLS_VORTEX_SNARE);
    }
};

// 108291, 108292, 108293, 108294 - Heart of the Wild
class spell_dru_heart_of_the_wild : public AuraScript
{
    PrepareAuraScript(spell_dru_heart_of_the_wild);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        switch (GetSpellInfo()->Id)
        {
            case SPELL_DRUID_HOTW_BALANCE:
            case SPELL_DRUID_HOTW_RESTO:
                if (GetUnitOwner()->HasAura(SPELL_DRUID_CAT_FORM))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_HOTW_CAT, true);
                else if (GetUnitOwner()->HasAura(SPELL_DRUID_BEAR_FORM))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_HOTW_BEAR, true);
                break;
            case SPELL_DRUID_HOTW_FERAL:
                if (GetUnitOwner()->HasAura(SPELL_DRUID_BEAR_FORM))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_HOTW_BEAR, true);
                break;
            case SPELL_DRUID_HOTW_GUARDIAN:
                if (GetUnitOwner()->HasAura(SPELL_DRUID_CAT_FORM))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_HOTW_CAT, true);
                break;
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_HOTW_CAT);
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_HOTW_BEAR);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_heart_of_the_wild::HandleApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_heart_of_the_wild::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 768 - Cat Form, 5487 - Bear Form 
class spell_dru_heart_of_the_wild_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_heart_of_the_wild_bonus);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* druid = GetOwner()->ToPlayer())
        {
            switch (GetSpellInfo()->Id)
            {
                case SPELL_DRUID_CAT_FORM:
                    if (druid->GetTalentSpecialization() != SPEC_DRUID_FERAL &&
                        (druid->HasAura(SPELL_DRUID_HOTW_BALANCE) || druid->HasAura(SPELL_DRUID_HOTW_GUARDIAN) || druid->HasAura(SPELL_DRUID_HOTW_RESTO)))
                        druid->CastSpell(druid, SPELL_DRUID_HOTW_CAT, true);
                    break;
                case SPELL_DRUID_BEAR_FORM:
                    if (druid->GetTalentSpecialization() != SPEC_DRUID_GUARDIAN &&
                        (druid->HasAura(SPELL_DRUID_HOTW_BALANCE) || druid->HasAura(SPELL_DRUID_HOTW_FERAL) || druid->HasAura(SPELL_DRUID_HOTW_RESTO)))
                        druid->CastSpell(druid, SPELL_DRUID_HOTW_BEAR, true);
                    break;
            }
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        switch (GetSpellInfo()->Id)
        {
            case SPELL_DRUID_CAT_FORM:
                GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_HOTW_CAT);
                break;
            case SPELL_DRUID_BEAR_FORM:
                GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_HOTW_BEAR);
                break;
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_heart_of_the_wild_bonus::HandleApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_heart_of_the_wild_bonus::HandleRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 127538 - Savage Roar (Glyphed)
class spell_dru_savage_roar_glyphed : public SpellScript
{
    PrepareSpellScript(spell_dru_savage_roar_glyphed);

    void HandleCast()
    {
        Player* druid = GetCaster()->ToPlayer();
        if (druid)
        {
            if (druid->GetComboPoints())
            {
                if (Aura* roar = druid->GetAura(GetSpellInfo()->Id))
                {
                    roar->SetMaxDuration(roar->CalcMaxDuration(druid) + druid->GetComboPoints() * 6000);
                    roar->SetDuration(roar->GetMaxDuration());
                }
                druid->ClearComboPoints();
            }
            else if (roll_chance_i(10))
                druid->CastSpell(druid, SPELL_DRUID_PREDATORY_SWIFTNESS, true);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dru_savage_roar_glyphed::HandleCast);
    }
};

// 48500 - Living Seed
class spell_dru_living_seed : public AuraScript
{
    PrepareAuraScript(spell_dru_living_seed);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        int32 amount = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), eff->GetAmount());
        if (!amount)
            return;
        AuraEffect* exists = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_DRUID_LEAVING_SEED, EFFECT_0);
        if (exists)
            amount += exists->GetAmount();
        amount = std::min(amount, int32(GetUnitOwner()->GetMaxHealth() / 2));
        GetUnitOwner()->CastCustomSpell(SPELL_DRUID_LEAVING_SEED, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetActionTarget(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_living_seed::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 48504 - Living Seed
class spell_dru_living_seed_heal : public AuraScript
{
    PrepareAuraScript(spell_dru_living_seed_heal);

    void HandleProc(AuraEffect const* eff, ProcEventInfo&)
    {
        GetUnitOwner()->CastCustomSpell(SPELL_DRUID_LEAVING_SEED_HEAL, SPELLVALUE_BASE_POINT0, eff->GetAmount(), GetUnitOwner(), true,
            nullptr, nullptr, GetCasterGUID());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_living_seed_heal::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 48505 - Starfall
class spell_dru_starfall : public AuraScript
{
    PrepareAuraScript(spell_dru_starfall);

    void HandleTick(AuraEffect const*)
    {
        // Shapeshifting into an animal form or mounting cancels the effect
        if (GetUnitOwner()->GetCreatureType() == CREATURE_TYPE_BEAST || GetUnitOwner()->IsMounted())
        {
            PreventDefaultAction();
            Remove();
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_starfall::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 50286 - Starfall
class spell_dru_starfall_damage : public SpellScript
{
    PrepareSpellScript(spell_dru_starfall_damage);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* druid = GetCaster();

        // Any effect which causes you to lose control of your character will supress the starfall effect.
        if (druid->HasUnitState(UNIT_STATE_CONTROLLED))
        {
            targets.clear();
            return;
        }

        bool glyphed = druid->HasAura(SPELL_DRUID_GLYPH_OF_GUIDED_STARS);
        targets.remove_if([=](WorldObject* target)
        {
            if (!druid->CanSeeOrDetect(target))
                return true;
            if (Creature* creature = target->ToCreature())
                if (!creature->GetThreatManager().IsThreateningTo(druid))
                    return true;
            if (Unit* unit = target->ToUnit())
            {
                if (glyphed)
                    if (!unit->HasAura(SPELL_DRUID_SUNFIRE, druid->GetGUID()) && !unit->HasAura(SPELL_DRUID_MOONFIRE, druid->GetGUID()))
                        return true;
                if (unit->HasBreakableByDamageCrowdControlAura())
                    return true;
            }
            return false;
        });
    }

    void HandleHit(SpellEffIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_starfall_damage::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_dru_starfall_damage::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 77758 - Thrash (Bear form)
class spell_dru_thrash_bear : public SpellScript
{
    PrepareSpellScript(spell_dru_thrash_bear);

    void HandleHit()
    {
        if (Player* druid = GetCaster()->ToPlayer())
            if (druid->HasSpellCooldown(SPELL_DRUID_MANGLE_BEAR))
                if (roll_chance_i(25))
                    druid->RemoveSpellCooldown(SPELL_DRUID_MANGLE_BEAR, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_thrash_bear::HandleHit);
    }
};

// 8921 - Moonfire, 93402 - Sunfire
class spell_dru_shooting_stars_trigger : public AuraScript
{
    PrepareAuraScript(spell_dru_shooting_stars_trigger);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* druid = GetCaster())
            druid->BindAura(GetAura());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_shooting_stars_trigger::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 93399 - Shooting Stars
class spell_dru_shooting_stars_proc : public AuraScript
{
    PrepareAuraScript(spell_dru_shooting_stars_proc);

    Player* druid = nullptr;
    std::vector<Unit*> uniqueTargets;

    bool Load() override
    {
        druid = GetOwner()->ToPlayer();
        return druid != nullptr;
    }

    void AddTarget(Unit* target)
    {
        for (auto&& itr : uniqueTargets)
            if (itr == target)
                return;
        uniqueTargets.push_back(target);
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (auto list = druid->GetBoundAurasBySpellId(SPELL_DRUID_MOONFIRE))
            for (auto&& itr : *list)
                AddTarget(itr->GetUnitOwner());
        if (auto list = druid->GetBoundAurasBySpellId(SPELL_DRUID_SUNFIRE))
            for (auto&& itr : *list)
                AddTarget(itr->GetUnitOwner());
        if (uniqueTargets.empty())
        {
            TC_LOG_ERROR("misc", "spell_dru_shooting_stars_proc::CheckProc - Shit happened! GUID: %u", druid->GetGUID().GetCounter());
            return false;
        }
        size_t targetCount = uniqueTargets.size();
        uniqueTargets.clear();
        float chance = GetSpellInfo()->ProcChance;
        druid->ApplySpellMod(GetId(), SPELLMOD_CHANCE_OF_SUCCESS, chance);
        return roll_chance_f(chance * sqrt(targetCount) / targetCount);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_shooting_stars_proc::CheckProc);
    }
};

// 114107 - Soul of the Forest
class spell_dru_soul_of_the_forest : public AuraScript
{
    PrepareAuraScript(spell_dru_soul_of_the_forest);

    void HandleProc(AuraEffect const* eff, ProcEventInfo&)
    {
        if (Player* druid = GetUnitOwner()->ToPlayer())
            if (druid->GetTalentSpecialization() == SPEC_DRUID_BALANCE)
                if (roll_chance_i(eff->GetAmount()))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_ASTRAL_INSIGHT, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_soul_of_the_forest::HandleProc, EFFECT_2, SPELL_AURA_DUMMY);
    }
};

// 33878 - Mangle (Bear)
class spell_dru_soul_of_the_forest_mangle : public SpellScript
{
    PrepareSpellScript(spell_dru_soul_of_the_forest_mangle);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER &&
            GetCaster()->ToPlayer()->GetTalentSpecialization() == SPEC_DRUID_GUARDIAN;
    }

    void CalculateValue(SpellEffIndex)
    {
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_DRUID_SOUL_OF_THE_FOREST, EFFECT_4))
        {
            int32 val = GetEffectValue();
            SetEffectValue(AddPct(val, eff->GetAmount()));
        }
    }

    void HandleHit()
    {
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_DRUID_SOUL_OF_THE_FOREST, EFFECT_5))
        {
            int32 damage = GetHitDamage();
            SetHitDamage(AddPct(damage, eff->GetAmount()));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_soul_of_the_forest_mangle::CalculateValue, EFFECT_2, SPELL_EFFECT_ENERGIZE);
        OnHit += SpellHitFn(spell_dru_soul_of_the_forest_mangle::HandleHit);
    }
};

// 22568 - Ferocious Bite, 22570 - Maim, 52610 - Savage Roar, 127538 - Savage Roar, 1079 - Rip
class spell_dru_soul_of_the_forest_feral : public SpellScript
{
    PrepareSpellScript(spell_dru_soul_of_the_forest_feral);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER &&
            GetCaster()->ToPlayer()->GetTalentSpecialization() == SPEC_DRUID_FERAL;
    }

    void HandleHit()
    {
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_DRUID_SOUL_OF_THE_FOREST, EFFECT_0))
        {
            int32 energy = GetCaster()->ToPlayer()->GetComboPoints() * eff->GetAmount();
            GetCaster()->CastCustomSpell(SPELL_DRUID_SOUL_OF_THE_FOREST_ENERGIZE, SPELLVALUE_BASE_POINT0, energy, GetCaster(), true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_soul_of_the_forest_feral::HandleHit);
    }
};

// 5221 - Shred, 114236 - Shred! (during Tiger's Fury with Glyph of Shred)
class spell_dru_shred : public SpellScript
{
    PrepareSpellScript(spell_dru_shred);

    void HandleHit()
    {
        for (auto&& it : GetHitUnit()->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
        {
            if (it->GetSpellInfo()->GetAllEffectsMechanicMask() & (1 << MECHANIC_BLEED))
            {
                SetHitDamage(GetHitDamage() * 1.2f);
                break;
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_shred::HandleHit);
    }
};

// 131537 - Item - Druid PvP Set Feral 4P Bonus
class spell_dru_item_pvp_feral_4p : public AuraScript
{
    PrepareAuraScript(spell_dru_item_pvp_feral_4p);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_STAMPEDE_TIMER, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_STAMPEDE);
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_STAMPEDE_TIMER);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == 102545 && GetUnitOwner()->HasAura(SPELL_DRUID_STAMPEDE))
        {
            GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_STAMPEDE);
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_STAMPEDE_TIMER, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_item_pvp_feral_4p::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_item_pvp_feral_4p::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_dru_item_pvp_feral_4p::HandleProc);
    }
};

// 81022 - Stampede
class spell_dru_stampede_effect : public AuraScript
{
    PrepareAuraScript(spell_dru_stampede_effect);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_STAMPEDE_TIMER, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_stampede_effect::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 131538 - Stampede
class spell_dru_stampede_timer : public AuraScript
{
    PrepareAuraScript(spell_dru_stampede_timer);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_STAMPEDE, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_stampede_timer::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 81022 - Stampede, 102543 - Incarnation: King of the Jungle
class spell_dru_override_ravage : public AuraScript
{
    PrepareAuraScript(spell_dru_override_ravage);

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        for (auto&& effect : GetUnitOwner()->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS))
            if (effect->GetAmount() == eff->GetAmount())
                effect->GetBase()->SetNeedClientUpdateForTargets();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_override_ravage::HandleRemove, EFFECT_ALL, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 17007 - Leader of the Pack
class spell_dru_leader_of_the_pack : public AuraScript
{
    PrepareAuraScript(spell_dru_leader_of_the_pack);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_LEADER_OF_THE_PACK_HEAL, true);
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_LEADER_OF_THE_PACK_ENERGIZE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_leader_of_the_pack::HandleProc);
    }
};

// 5487 - Bear Form
class spell_dru_bear_form : public AuraScript
{
    PrepareAuraScript(spell_dru_bear_form);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = GetUnitOwner()->HasAura(SPELL_DRUID_THICK_HIDE) ? 330.0f : 120.0f;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_bear_form::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_BASE_RESISTANCE_PCT);
    }
};

class spell_dru_force_of_nature_base : public SpellScript
{
    PrepareSpellScript(spell_dru_force_of_nature_base);

    virtual void HandleSummon(TempSummon* treant) = 0;

    void HandleCast()
    {
        std::list<TempSummon*> treants;
        uint32 entry = GetSpellInfo()->Effects[EFFECT_1].MiscValue;
        GetCaster()->GetSummons(treants, GetSpellInfo()->Effects[EFFECT_1].MiscValue);
        if (!treants.empty())
        {
            TempSummon* treant = treants.back();
            treant->CastSpell(treant, 37846, TRIGGERED_WITH_SPELL_START);           // Visual
            HandleSummon(treant);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dru_force_of_nature_base::HandleCast);
    }
};

// 33831 - Force of Nature
class spell_dru_force_of_nature_balance : public spell_dru_force_of_nature_base
{
    void HandleSummon(TempSummon* treant) override
    {
        treant->CastSpell(GetExplTargetUnit(), 113770, true);   // Entangling Roots
        treant->AI()->AttackStart(GetExplTargetUnit());
    }
};

// 102693 - Force of Nature
class spell_dru_force_of_nature_restoration : public spell_dru_force_of_nature_base
{
    void HandleSummon(TempSummon* treant) override
    {
        treant->CastSpell(GetExplTargetUnit(), 142421);         // Swiftmend
    }
};

// 102703 - Force of Nature
class spell_dru_force_of_nature_feral : public spell_dru_force_of_nature_base
{
    void HandleSummon(TempSummon* treant) override
    {
        treant->CastSpell(GetExplTargetUnit(), 113770, true);   // Entangling Roots
        treant->CastSpell(GetExplTargetUnit(), 142431, true);   // Fixate
        treant->CastSpell(GetExplTargetUnit(), 150017, TRIGGERED_WITH_SPELL_START);  // Rake
        if (GetExplTargetUnit()->HasAura(150017, treant->GetGUID()))    // Not missed
            treant->InitStats(16000);                                   // A little more time to ensure the aura make last tick
        treant->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster()) + 1);
        treant->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster()) + 1);
        treant->UpdateAttackPowerAndDamage();
        treant->AI()->AttackStart(GetExplTargetUnit());
    }
};

// 102706 - Force of Nature
class spell_dru_force_of_nature_guradian : public spell_dru_force_of_nature_base
{
    void HandleSummon(TempSummon* treant) override
    {
        treant->CastSpell(GetExplTargetUnit(), 113830, true);   // Taunt
        treant->CastSpell(GetExplTargetUnit(), 142431, true);   // Fixate
        treant->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster()) * 0.2f + 1);
        treant->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster()) * 0.2f + 1);
        treant->UpdateAttackPowerAndDamage();
        treant->AI()->AttackStart(GetExplTargetUnit());
    }
};

// 1964
struct npc_force_of_nature_balance : public ScriptedAI
{
    enum { SPELL_WRATH = 113769 };

    CasterMovement m_casterMovement = CasterMovement(me).Spell(SPELL_WRATH);

    npc_force_of_nature_balance(Creature* c) : ScriptedAI(c) { }

    void AttackStart(Unit* target) override
    {
        if (target && me->Attack(target, false))
            m_casterMovement.Chase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        m_casterMovement.Update(diff);

        if (!UpdateVictim())
            return;

        DoCastVictim(SPELL_WRATH);
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }
};

// 54983
struct npc_force_of_nature_restoration : public ScriptedAI
{
    enum { SPELL_HEALING_TOUCH = 113828 };

    npc_force_of_nature_restoration(Creature* c) : ScriptedAI(c) { }

    void UpdateAI(uint32 diff) override
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoCast(me, SPELL_HEALING_TOUCH);
    }
};

// 150017 - Rake
class spell_dru_force_of_nature_rake : public AuraScript
{
    PrepareAuraScript(spell_dru_force_of_nature_rake);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            if (Unit* caster = GetCaster())
                if (TempSummon* treant = caster->ToTempSummon())
                    treant->UnSummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_force_of_nature_rake::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 52610, 127538 - Savage Roar, 5217 - Tiger's Fury
class spell_dru_cat_form_check : public SpellScript
{
    PrepareSpellScript(spell_dru_cat_form_check);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetShapeshiftForm() != FORM_CAT)
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_IN_CAT_FORM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_cat_form_check::CheckCast);
    }
};

// 783 - Travel Form
class spell_dru_travel_form : public AuraScript
{
    PrepareAuraScript(spell_dru_travel_form);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* druid = GetUnitOwner();
        druid->m_Events.Schedule(1, [=]
        {
            if (!druid->HasAura(SPELL_DRUID_GLYPH_OF_THE_CHEETAH))
            {
                if (druid->HasAura(SPELL_DRUID_GLYPH_OF_THE_STAG))
                    druid->CastSpell(druid, SPELL_DRUID_STAG_FORM, true);
                druid->CastSpell(druid, SPELL_DRUID_SOUND_EFFECT_STAG_FORM, true);
            }
        });
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DRUID_STAG_FORM);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_travel_form::HandleApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_travel_form::HandleRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 770 - Faerie Fire, 102355 - Faerie Swarm
class spell_dru_glyph_of_fae_silence : public SpellScript
{
    PrepareSpellScript(spell_dru_glyph_of_fae_silence);

    void HandleHit()
    {
        if (GetCaster()->GetShapeshiftForm() == FORM_BEAR && GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_FAE_SILENCE))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_FAE_SILENCE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_glyph_of_fae_silence::HandleHit);
    }
};

// 1178 - Bear Form Passive
class spell_dru_bear_form_passive : public AuraScript
{
    PrepareAuraScript(spell_dru_bear_form_passive);

    void CalculateCooldownModAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect* glyph = GetUnitOwner()->GetAuraEffect(SPELL_DRUID_GLYPH_OF_FAE_SILENCE, EFFECT_0))
            amount += 9 * IN_MILLISECONDS;
    }

    void CalculateRageFromDamageAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect* setBonus = GetUnitOwner()->GetAuraEffect(SPELL_DRUID_T15_GUARDIAN_4P_BONUS, EFFECT_0))
            AddPct(amount, setBonus->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_bear_form_passive::CalculateCooldownModAmount, EFFECT_2, SPELL_AURA_ADD_FLAT_MODIFIER);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_bear_form_passive::CalculateRageFromDamageAmount, EFFECT_4, SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT);
    }
};

// 114237 - Glyph of Fae Silence
class spell_dru_glyph_of_fae_silence_cooldown : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_fae_silence_cooldown);

    void HandleEffect(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* bearForm = GetUnitOwner()->GetAuraEffect(SPELL_DRUID_BEAR_FORM_PASSIVE, EFFECT_2))
            bearForm->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_fae_silence_cooldown::HandleEffect, EFFECT_0, SPELL_AURA_NONE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_dru_glyph_of_fae_silence_cooldown::HandleEffect, EFFECT_0, SPELL_AURA_NONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 138222 - Item - Druid T15 Guardian 4P Bonus
class spell_dru_t15_guardian_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t15_guardian_4p_bonus);

    void HandleEffect(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* bearForm = GetUnitOwner()->GetAuraEffect(SPELL_DRUID_BEAR_FORM_PASSIVE, EFFECT_4))
            bearForm->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_t15_guardian_4p_bonus::HandleEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_dru_t15_guardian_4p_bonus::HandleEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 22812 - Barkskin
class spell_dru_t16_guardian_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t16_guardian_2p_bonus);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* druid = GetUnitOwner();
        if (AuraEffect const* bonus = druid->GetAuraEffect(SPELL_DRUID_T16_GUARDIAN_2P_BONUS, EFFECT_0))
        {
            if (Aura* aura = druid->GetAura(SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT))
            {
                aura->SetDuration(aura->GetDuration() + 3000);
                if (aura->GetDuration() > aura->GetMaxDuration())
                    aura->SetMaxDuration(aura->GetDuration());
            }
            else
            {
                druid->CastSpell(druid, SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT, true);
                if (Aura* aura = druid->GetAura(SPELL_DRUID_SAVAGE_DEFENSE_DODGE_PCT))
                {
                    aura->SetDuration(3000);
                    aura->SetMaxDuration(3000);
                }
            }
            druid->CastSpell(druid, SPELL_DRUID_FRENZIED_REGENERATION, true, nullptr, bonus);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_t16_guardian_2p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_REDUCE_PUSHBACK, AURA_EFFECT_HANDLE_REAL);
    }
};

// 22842 - Frenzied Regeneration
// 62606 - Savage Defense
class spell_dur_t16_guardian_4p_bonus : public SpellScript
{
    PrepareSpellScript(spell_dur_t16_guardian_4p_bonus);

    void HandleHit()
    {
        // Probably spell must not be casted?
        if (GetSpellInfo()->Id == SPELL_DRUID_FRENZIED_REGENERATION && !GetSpellValue()->EffectBasePoints[EFFECT_0])
            return;

        if (AuraEffect const* bonus = GetCaster()->GetAuraEffect(SPELL_DRUID_T16_GUARDIAN_4P_BONUS, EFFECT_0))
        {
            float ap = GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 heal = CalculatePct(ap, bonus->GetAmount()) / 8;
            GetCaster()->CastCustomSpell(SPELL_DRUID_URSOCS_VIGOR, SPELLVALUE_BASE_POINT0, heal, GetCaster(), true);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dur_t16_guardian_4p_bonus::HandleHit);
    }
};

// 135700 - Clearcasting
class spell_dru_t16_feral_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_dru_t16_feral_2p_bonus);

    void HandleCast()
    {
        // Where are proc but Clearcasting (135700) has SPELL_ATTR3_CANT_TRIGGER_PROC
        if (AuraEffect const* bonus = GetCaster()->GetAuraEffect(SPELL_DRUID_T16_FERAL_2P_BONUS, EFFECT_0))
            GetCaster()->CastSpell(GetCaster(), bonus->GetSpellEffectInfo().TriggerSpell, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_t16_feral_2p_bonus::HandleCast);
    }
};

// 144841 - Item - Druid T16 Feral 4P Bonus
class spell_dru_t16_feral_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t16_feral_4p_bonus);

    void HandleProc(ProcEventInfo&)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DRUID_FERAL_RAGE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_t16_feral_4p_bonus::HandleProc);
    }
};

// 146874 - Feral Rage
class spell_dru_feral_rage : public AuraScript
{
    PrepareAuraScript(spell_dru_feral_rage);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActionTarget() || !eventInfo.GetSpellInfo())
            return false;
        if (eventInfo.GetSpellInfo()->Id == SPELL_DRUID_SAVAGE_ROAR_GLYPHED) // Special case
            return true;
        return eventInfo.GetSpellInfo()->SpellFamilyName == SPELLFAMILY_DRUID && eventInfo.GetSpellInfo()->NeedsComboPoints();
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        ObjectGuid targetGuid = eventInfo.GetActionTarget()->GetGUID();
        if (GetUnitOwner() == eventInfo.GetActionTarget())  // Savage Roar
            targetGuid = GetUnitOwner()->GetTarget();

        // Deleay needs bc current spell not consumed cp yet
        GetUnitOwner()->Schedule(Milliseconds(1), [=](Unit* self)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*self, targetGuid))
                self->CastSpell(target, SPELL_DRUID_TIGERS_FURY_COMBO, true);
        });
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_feral_rage::CheckProc);
        OnProc += AuraProcFn(spell_dru_feral_rage::HandleProc);
    }
};

// 144767 - Item - Druid T16 Balance 2P Bonus
class spell_dru_t16_balance_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t16_balance_2p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;
        if (eventInfo.TriggeredBySpell()->IsTriggered()) // Starfall porbably?
            return false;
        return eventInfo.GetActionTarget() != nullptr;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* druid  = GetUnitOwner();
        Unit* target = eventInfo.GetActionTarget();
        if ((druid->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) || druid->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT)) && eventInfo.GetSpellInfo()->SchoolMask & SPELL_SCHOOL_MASK_ARCANE)
            druid->CastSpell(target, SPELL_DRUID_LUNAR_BOLT, true);
        else if ((druid->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) || druid->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT)) && eventInfo.GetSpellInfo()->SchoolMask & SPELL_SCHOOL_MASK_NATURE)
            druid->CastSpell(target, SPELL_DRUID_SOLAR_BOLT, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t16_balance_2p_bonus::CheckProc);
        OnProc += AuraProcFn(spell_dru_t16_balance_2p_bonus::HandleProc);
    }
};

// 144871 - Sage Mender
class spell_dru_sage_mender : public AuraScript
{
    PrepareAuraScript(spell_dru_sage_mender);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Remove();
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_sage_mender::HandleProc);
    }
};

// 84840 - Vengeance
class spell_dru_vengeance : public AuraScript
{
    PrepareAuraScript(spell_dru_vengeance);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->GetShapeshiftForm() == FORM_BEAR;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_vengeance::CheckProc);
    }
};

// 774 - Rejuvenation
class spell_dru_glyph_of_rejuvenation : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_rejuvenation);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->HasAura(SPELL_DRUID_GLYPH_OF_REJUVENATION))
            {
                caster->BindAura(GetAura());
                if (auto auras = caster->GetBoundAurasBySpellId(GetId()))
                    if (auras->size() >= 3)
                        caster->CastSpell(caster, SPELL_DRUID_GLYPH_OF_REJUVENATION_EFFECT, true);
            }
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
        {
            auto auras = caster->GetBoundAurasBySpellId(GetId());
            if (!auras || auras->size() < 3)
                caster->RemoveAurasDueToSpell(SPELL_DRUID_GLYPH_OF_REJUVENATION_EFFECT);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_rejuvenation::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_dru_glyph_of_rejuvenation::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 54825 - Glyph of Healing Touch
class spell_dru_glyph_of_healing_touch : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_healing_touch);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (Player* druid = GetUnitOwner()->ToPlayer())
            druid->ModifySpellCooldown(SPELL_DRUID_NATURES_SWIFTNESS, -eff->GetAmount() * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_healing_touch::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 110309 - Symbiosis
class spell_dru_symbiosis_caster_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_symbiosis_caster_aura);

    uint32 targetSpellId = 0;
    ObjectGuid targetGuid = ObjectGuid::Empty;

public:
    void Bind(uint32 overrideSpellId, uint32 targetSpell, ObjectGuid target)
    {
        GetEffect(EFFECT_0)->ChangeAmount(overrideSpellId);
        targetSpellId = targetSpell;
        targetGuid = target;
    }

    void HandleRemove(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (Unit* target = ObjectAccessor::GetUnit(*GetUnitOwner(), targetGuid))
            target->RemoveAurasDueToSpell(targetSpellId, GetUnitOwner()->GetGUID());
        GetUnitOwner()->RemoveAurasDueToSpell(effect->GetAmount());
        if (auto const* spellInfo = sSpellMgr->GetSpellInfo(effect->GetAmount()))
        {
            for (auto&& effect : spellInfo->Effects)
            {
                if (effect.Effect == SPELL_EFFECT_SUMMON)
                {
                    std::list<TempSummon*> summons;
                    GetUnitOwner()->GetSummons(summons, effect.MiscValue);
                    for (auto&& itr : summons)
                        itr->UnSummon();
                }
            }
        }
    }

    void HandleUpdate(uint32)
    {
        Unit* caster = GetCaster();
        Player* druid = caster ? caster->ToPlayer() : nullptr;
        Player* target = ObjectAccessor::GetPlayer(*GetUnitOwner(), targetGuid);
        if (!druid || !target || !target->IsInSameRaidWith(druid))
            Remove();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_symbiosis_caster_aura::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
        OnAuraUpdate += AuraUpdateFn(spell_dru_symbiosis_caster_aura::HandleUpdate);
    }
};

// 110478, 110479, 110482, 110483, 110484, 110485, 110486, 110488, 110490, 110491
class spell_dru_symbiosis_target_check : public AuraScript
{
    PrepareAuraScript(spell_dru_symbiosis_target_check);

    void HandleUpdate(uint32)
    {
        Unit* caster = GetCaster();
        Player* druid = caster ? caster->ToPlayer() : nullptr;
        Player* target = GetUnitOwner()->ToPlayer();
        if (!druid || !target || !target->IsInSameRaidWith(druid) || target->GetMapId() != druid->GetMapId())
            Remove();
    }

    void HandleRemove(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_DRUID_SYMBIOSIS_FOR_CASTER);
        GetUnitOwner()->RemoveAurasDueToSpell(effect->GetAmount());
        if (auto const* spellInfo = sSpellMgr->GetSpellInfo(effect->GetAmount()))
        {
            for (auto&& effect : spellInfo->Effects)
            {
                if (effect.Effect == SPELL_EFFECT_SUMMON)
                {
                    std::list<TempSummon*> summons;
                    GetUnitOwner()->GetSummons(summons, effect.MiscValue);
                    for (auto&& itr : summons)
                        itr->UnSummon();
                }
            }
        }
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_dru_symbiosis_target_check::HandleUpdate);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_symbiosis_target_check::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 110309 - Symbiosis
class spell_dru_symbiosis : public SpellScript
{
    PrepareSpellScript(spell_dru_symbiosis);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    SpellCastResult CheckCast()
    {
        Player* target = GetExplTargetUnit() ? GetExplTargetUnit()->ToPlayer() : nullptr;
        if (!target || target->GetClass() == CLASS_DRUID || target->GetSpecialization() == SPEC_NONE)
            return SPELL_FAILED_BAD_TARGETS;
        if (!GetCaster()->IsInRaidWith(target))
            return SPELL_FAILED_TARGET_NOT_IN_PARTY;
        return SPELL_CAST_OK;
    }

    void HandleScriptEffect(SpellEffIndex)
    {
        Player* druid = GetCaster()->ToPlayer();
        Player* target = GetHitUnit()->ToPlayer();
        Aura* symbiosis = druid->GetAura(SPELL_DRUID_SYMBIOSIS_FOR_CASTER);
        if (!druid || !target || !symbiosis)
            return;

        uint32 targetSpell = 0;
        int32  bpTarget = 0;

        if (target->GetSpecialization() == SPEC_NONE || druid->GetSpecialization() == SPEC_NONE)
            return;

        if (auto scirpt = dynamic_cast<spell_dru_symbiosis_caster_aura*>(symbiosis->GetScriptByName("spell_dru_symbiosis_caster_aura")))
        {
            if (int32 casterSpell = GetSpells(targetSpell, bpTarget, target))
            {
                scirpt->Bind(casterSpell, targetSpell, target->GetGUID());
                if (bpTarget && targetSpell)
                    druid->CastCustomSpell(targetSpell, SPELLVALUE_BASE_POINT0, bpTarget, target, true);
            }
        }
    }

    uint32 GetSpells(uint32& targetSpell, int32& bpTarget, Player* target)
    {
        Specializations specId = GetCaster()->ToPlayer()->GetSpecialization();

        switch (target->GetClass())
        {
            case CLASS_DEATH_KNIGHT:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_DEATH_KNIGHT;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 113516;      // Wild Mushroom: Plague
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_TANK)
                    bpTarget = 113072;      // Might of Ursoc

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110570;      // Anti-Magic Shell
                    case SPEC_DRUID_GUARDIAN:
                        return 122285;      // Bone Shield
                    case SPEC_DRUID_FERAL:
                        return 122282;      // Death Coil
                    case SPEC_DRUID_RESTORATION:
                        return 110575;      // Icebound Fortitude
                }
                break;
            }
            case CLASS_HUNTER:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_HUNTER;
                bpTarget = 113073;          // Dash

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110588;      // Misdirection
                    case SPEC_DRUID_GUARDIAN:
                        return 110600;      // Ice Trap
                    case SPEC_DRUID_FERAL:
                        return 110597;      // Play Dead
                    case SPEC_DRUID_RESTORATION:
                        return 110617;      // Deterrence
                }
                break;
            }
            case CLASS_MAGE:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_MAGE;
                bpTarget = 113074;          // Healing Touch

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110621;      // Mirror Image
                    case SPEC_DRUID_GUARDIAN:
                        return 110694;      // Frost Armor
                    case SPEC_DRUID_FERAL:
                        return 110693;      // Frost Nova
                    case SPEC_DRUID_RESTORATION:
                        return 110696;      // Ice Block
                }
                break;
            }
            case CLASS_MONK:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_MONK;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_TANK)
                    bpTarget = 113306;      // Survival Instincts
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 127361;      // Bear Hug
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_HEALER)
                    bpTarget = 113275;      // Entangling Roots

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 126458;      // Grapple Weapon
                    case SPEC_DRUID_GUARDIAN:
                        return 126453;      // Elusive Brew
                    case SPEC_DRUID_FERAL:
                        return 126449;      // Clash
                    case SPEC_DRUID_RESTORATION:
                        return 126456;      // Fortifying Brew
                }
                break;
            }
            case CLASS_PALADIN:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_PALADIN;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_TANK)
                    bpTarget = 113075;      // Barkskin
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 122287;      // Wrath
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_HEALER)
                    bpTarget = 113269;      // Rebirth

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110698;      // Hammer of Justice
                    case SPEC_DRUID_GUARDIAN:
                        return 110701;      // Consecration
                    case SPEC_DRUID_FERAL:
                        return 110700;      // Divine Shield
                    case SPEC_DRUID_RESTORATION:
                        return 122288;      // Cleanse
                }
                break;
            }
            case CLASS_PRIEST:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_PRIEST;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 113277;      // Tranquility
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_HEALER)
                    bpTarget = 113506;      // Cyclone

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110707;      // Mass Dispel
                    case SPEC_DRUID_GUARDIAN:
                        return 110717;      // Fear Ward
                    case SPEC_DRUID_FERAL:
                        return 110715;      // Dispersion
                    case SPEC_DRUID_RESTORATION:
                        return 110718;      // Leap of Faith
                }
                break;
            }
            case CLASS_ROGUE:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_ROGUE;
                bpTarget = 113613;          // Growl

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110788;      // Cloak of Shadows
                    case SPEC_DRUID_GUARDIAN:
                        return 122289;      // Feint
                    case SPEC_DRUID_FERAL:
                        return 110730;      // Redirect
                    case SPEC_DRUID_RESTORATION:
                        return 110791;      // Evasion
                }
                break;
            }
            case CLASS_SHAMAN:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_SHAMAN;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_HEALER)
                    bpTarget = 113289;      // Prowl
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 113286;      // Solar Beam

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 110802;      // Purge
                    case SPEC_DRUID_GUARDIAN:
                        return 110803;      // Lightning Shield
                    case SPEC_DRUID_FERAL:
                        return 110807;      // Feral Spirit
                    case SPEC_DRUID_RESTORATION:
                        return 110806;      // Spiritwalker's Grace
                }
                break;
            }
            case CLASS_WARLOCK:
            {
                bpTarget = 113295;          // Rejuvenation
                targetSpell = SPELL_DRUID_SYMBIOSIS_WARLOCK;

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 122291;      // Unending Resolve
                    case SPEC_DRUID_GUARDIAN:
                        return 122290;      // Life Tap
                    case SPEC_DRUID_FERAL:
                        return 110810;      // Soul Swap
                    case SPEC_DRUID_RESTORATION:
                        return 112970;      // Demonic Circle : Teleport
                }

                break;
            }
            case CLASS_WARRIOR:
            {
                targetSpell = SPELL_DRUID_SYMBIOSIS_WARRIOR;

                if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_TANK)
                    bpTarget = 122286;      // Savage Defense
                else if (target->GetRoleForGroup(target->GetSpecialization()) == ROLES_DPS)
                    bpTarget = 122294;      // Stampeding Shout

                switch (specId)
                {
                    case SPEC_DRUID_BALANCE:
                        return 122292;      // Intervene
                    case SPEC_DRUID_GUARDIAN:
                        return 113002;      // Spell Reflection
                    case SPEC_DRUID_FERAL:
                        return 112997;      // Shattering Blow
                    case SPEC_DRUID_RESTORATION:
                        return 113004;      // Intimidating Roar
                }
                break;
            }
            default:
                break;
        }
        return 0;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_symbiosis::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_dru_symbiosis::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 126456 - Fortifying Brew
class spell_dru_symbiosis_fortifying_brew : public AuraScript
{
    PrepareAuraScript(spell_dru_symbiosis_fortifying_brew);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = GetUnitOwner()->CountPctFromMaxHealth(amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_symbiosis_fortifying_brew::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH);
    }
};

// 108300 - Killer Instinct
class spell_dru_killer_instinct : public AuraScript
{
    PrepareAuraScript(spell_dru_killer_instinct);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Player* druid = GetUnitOwner()->ToPlayer())
            amount = druid->GetStat(STAT_INTELLECT);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_killer_instinct::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

// 46832 - Sudden Eclipse
class spell_dru_sudden_eclipse : public AuraScript
{
    PrepareAuraScript(spell_dru_sudden_eclipse);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* druid = GetUnitOwner();
        if (!druid->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && !druid->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && !druid->HasAura(SPELL_DRUID_CELESTIAL_ALIGNMENT))
        {
            bool lunarMarker = druid->HasAura(SPELL_DRUID_ECLIPSE_MARKER_LUNAR);
            bool solarMarker = druid->HasAura(SPELL_DRUID_ECLIPSE_MARKER_SOLAR);

            int32 eclipse = 20;
            if (lunarMarker || !solarMarker)
                eclipse = -eclipse;

            druid->CastCustomSpell(SPELL_DRUID_ECLIPSE_ENERGIZE, SPELLVALUE_BASE_POINT0, eclipse, druid, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dru_sudden_eclipse::HandleProc);
    }
};

// 5215 - Prowl
class spell_dru_prowl : public AuraScript
{
    PrepareAuraScript(spell_dru_prowl);

    void CalculateDecreaseSpeed(AuraEffect const*, float& amount, bool&)
    {
        if (GetUnitOwner()->HasAura(SPELL_DRUID_GLYPH_OF_PROWL))
            amount = 0;
    }

    void CalculateIncreaseSpeed(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* effect = GetUnitOwner()->GetAuraEffect(SPELL_ELUSIVENESS, EFFECT_0))
            amount = effect->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_prowl::CalculateDecreaseSpeed, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_prowl::CalculateIncreaseSpeed, EFFECT_2, SPELL_AURA_MOD_SPEED_ALWAYS);
    }
};

// 16914 - Hurricane
// 106996 - Astral Storm
class spell_dru_glyph_of_hurricane : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_hurricane);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit const* druid = GetCaster())
            if (AuraEffect const* glyph = druid->GetAuraEffect(SPELL_DRUID_GLYPH_OF_HURRICANE, EFFECT_0))
                amount = glyph->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_glyph_of_hurricane::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED);
    }
};

// 23218 - Shapeshift Movement Speed
class spell_dru_shapeshift_move_speed : public AuraScript
{
    PrepareAuraScript(spell_dru_shapeshift_move_speed);

    bool CheckAreaTarget(Unit* target)
    {
        if (Map* map = target->FindMap())
            map->GetAreaId(target->GetPhaseMask(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

        return true;
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_dru_shapeshift_move_speed::CheckAreaTarget);
    }
};

// 50334 - Berserk
class spell_dru_berserk_bear : public AuraScript
{
    PrepareAuraScript(spell_dru_berserk_bear);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* druid = GetOwner()->ToPlayer())
            druid->RemoveSpellCooldown(SPELL_DRUID_MANGLE_BEAR, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_berserk_bear::HandleApply, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 5225 - Track Humanoids
// 114280 - Glyph of the Predator
class spell_dru_glyph_of_the_predator : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_the_predator);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->HasAura(GetSpellInfo()->Id == SPELL_DRUID_GLYPH_OF_THE_PREDATOR ? SPELL_DRUID_TRACK_HUMANOIDS : SPELL_DRUID_GLYPH_OF_THE_PREDATOR))
            GetUnitOwner()->SetFlag(PLAYER_FIELD_TRACK_CREATURE_MASK, 1 << (CREATURE_TYPE_BEAST - 1));
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        // Remove anyway
        GetUnitOwner()->RemoveFlag(PLAYER_FIELD_TRACK_CREATURE_MASK, 1 << (CREATURE_TYPE_BEAST - 1));
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_dru_glyph_of_the_predator::HandleApply,  EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_glyph_of_the_predator::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 107059 - Glyph of the Chameleon
class spell_dru_glyph_of_the_chameleon_fix : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_the_chameleon_fix);

public:
    uint32 LastModelId = 0;

    void CalculateSpellMod(AuraEffect const*, SpellModifier*& spellMod)
    {
        if (spellMod)
        {
            delete spellMod;
            spellMod = nullptr;
        }
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_dru_glyph_of_the_chameleon_fix::CalculateSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// 768 - Cat Form
// 5487 - Bear Form
class spell_dru_glyph_of_the_chameleon : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_the_chameleon);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Aura* glyph = GetUnitOwner()->GetAura(SPELL_DRUID_GLYPH_OF_THE_CHAMELEON))
        {
            if (auto script = dynamic_cast<spell_dru_glyph_of_the_chameleon_fix*>(glyph->GetScriptByName("spell_dru_glyph_of_the_chameleon_fix")))
            {
                std::set<uint32> models;
                if (GetUnitOwner()->GetShapeshiftForm() == FORM_CAT)
                {
                    static std::map<uint32, std::set<uint32>> const catModels
                    {
                        { RACE_NIGHTELF, { 29405, 29406, 29407, 29408, 892   } },
                        { RACE_TROLL,    { 33668, 33667, 33666, 33665, 33669 } },
                        { RACE_WORGEN,   { 33662, 33661, 33664, 33663, 33660 } },
                        { RACE_TAUREN,   { 29409, 29410, 29411, 29412, 8571  } },
                    };
                    auto itr = catModels.find(GetUnitOwner()->GetRace());
                    if (itr == catModels.end())
                        return;
                    models = itr->second;
                }
                else
                {
                    static std::map<uint32, std::set<uint32>> const bearModels
                    {
                        { RACE_NIGHTELF, { 29413, 29414, 29416, 29415, 2281  } },
                        { RACE_TROLL,    { 33657, 33659, 33656, 33658, 33655 } },
                        { RACE_WORGEN,   { 33652, 33651, 33654, 33653, 33650 } },
                        { RACE_TAUREN,   { 29418, 29419, 29420, 29421, 2289  } },
                    };
                    auto itr = bearModels.find(GetUnitOwner()->GetRace());
                    if (itr == bearModels.end())
                        return;
                    models = itr->second;
                }

                // Make it "more" random
                models.erase(script->LastModelId);
                uint32 modelId = Trinity::Containers::SelectRandomContainerElement(models);
                GetUnitOwner()->SetDisplayId(modelId);
                script->LastModelId = modelId;
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_glyph_of_the_chameleon::HandleApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 113004 - Intimidating Roar
class spell_dru_intimidating_roar : public SpellScript
{
    PrepareSpellScript(spell_dru_intimidating_roar);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_intimidating_roar::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_intimidating_roar::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Celestial Alignment - 112071
class spell_druid_celestial_alignment : public SpellScriptLoader
{
    public:
        spell_druid_celestial_alignment() : SpellScriptLoader("spell_druid_celestial_alignment") { }

        class spell_druid_celestial_alignment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_druid_celestial_alignment_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                Player* player = target->ToPlayer();
                if (!player)
                    return;

                player->SetPower(POWER_ECLIPSE, 0);

                player->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
                player->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE);
                player->RemoveAurasDueToSpell(67483);  // markers
                player->RemoveAurasDueToSpell(67484);

                player->CastSpell(player, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, true);
                player->CastSpell(player, SPELL_DRUID_NATURES_GRACE, true); // Cast Nature's Grace
                player->RemoveSpellCooldown(SPELL_DRUID_STARFALL, true);
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                Player* player = target->ToPlayer();
                if (!player)
                    return;

                player->RemoveAura(SPELL_DRUID_NATURES_GRACE);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_druid_celestial_alignment_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_druid_celestial_alignment_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_druid_celestial_alignment_AuraScript();
        }
};

// Astral Communion - 127663
class spell_druid_astral_communion : public SpellScriptLoader
{
    public:
        spell_druid_astral_communion() : SpellScriptLoader("spell_druid_astral_communion") { }

        class spell_druid_astral_communion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_druid_astral_communion_AuraScript);

            int32 direction;
            bool Load() override
            {
                direction = 1;
                return true;
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
            {
                if (Unit* caster = GetCaster())
                {
                    direction = caster->HasAura(67483) ? 1 : -1;

                    if (Aura* aura = caster->GetAura(145138))
                    {
                        int32 _amount = 100 * direction;
                        caster->CastCustomSpell(caster, 89265, &_amount, nullptr, nullptr, true);
                        aura->Remove(AURA_REMOVE_BY_DEFAULT);
                        GetAura()->Remove();
                    }
                }
            }

            void OnTick(AuraEffect const* aurEff)
            {
                Player* player = GetTarget()->ToPlayer();
                if (!player)
                    return;

                if (player->HasAura(112071))
                    return;

                int32 mod = aurEff->GetAmount() * direction;
                // energize
                player->CastCustomSpell(player, 89265, &mod, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_druid_astral_communion_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_druid_astral_communion_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_druid_astral_communion_AuraScript();
        }
};

void AddSC_druid_spell_scripts()
{
    new aura_script<spell_dru_tooth_and_claw_absorb>("spell_dru_tooth_and_claw_absorb");
    new spell_script<spell_dru_tooth_and_claw_trigger>("spell_dru_tooth_and_claw_trigger");
    new spell_script<spell_dru_swipe_and_maul_bleed>("spell_dru_swipe_and_maul_bleed");
    new spell_script<spell_dru_genesis>("spell_dru_genesis");
    new spell_dru_glyph_of_the_treant();
    new spell_dru_incarnation_chosen_of_elune();
    new aura_script<spell_dru_incarnation_skins>("spell_dru_incarnation_skins");
    new spell_dru_glyph_of_shred();
    new spell_dru_wild_charge_moonkin();
    new spell_dru_tigers_fury();
    new spell_dru_play_death();
    new spell_dru_life_tap();
    new spell_dru_soul_swap();
    new spell_script<spell_dru_demonic_circle_teleport>("spell_dru_demonic_circle_teleport");
    new spell_script<spell_dru_shattering_blow>("spell_dru_shattering_blow");
    new spell_dru_moonfire();
    new aura_script<spell_dru_natures_vigil>("spell_dru_natures_vigil");
    new spell_script<spell_dru_natures_vigil_damage>("spell_dru_natures_vigil_damage");
    new spell_script<spell_dru_natures_vigil_heal>("spell_dru_natures_vigil_heal");
    new spell_dru_cenarion_ward();
    new atrigger_script<sat_druid_solar_beam>("sat_druid_solar_beam");
    new spell_dru_dash();
    new spell_script<spell_dru_rip_duration>("spell_dru_rip_duration");
    new spell_script<spell_dru_savage_defense>("spell_dru_savage_defense");
    new spell_dru_bear_hug();
    new spell_dru_ravage();
    new spell_dru_mark_of_the_wild();
    new spell_dru_glyph_of_regrowth();
    new aura_script<spell_dru_cat_form>("spell_dru_cat_form");
    new spell_dru_skull_bash();
    new spell_script<spell_dru_faerie_swarm>("spell_dru_faerie_swarm");
    new aura_script<spell_dru_astral_communion>("spell_dru_astral_communion");
    new spell_dru_shooting_stars();
    new spell_script<spell_dru_celestial_alignment>("spell_dru_celestial_alignment");
    new spell_script<spell_dru_frenzied_regeneration>("spell_dru_frenzied_regeneration");
    new spell_dru_stampeding_roar_speed();
    new spell_script<spell_dru_stampeding_roar>("spell_dru_stampeding_roar");
    new aura_script<spell_dru_innervate>("spell_dru_innervate");
    new spell_script<spell_dru_glyph_of_innervate>("spell_dru_glyph_of_innervate");
    new spell_script<spell_dru_teleport_moonglade>("spell_dru_teleport_moonglade");
    new spell_script<spell_dru_growl>("spell_dru_growl");
    new spell_script<spell_dru_activate_cat_form>("spell_dru_activate_cat_form");
    new spell_script<spell_dru_eclipse>("spell_dru_eclipse");
    new spell_script<spell_dru_eclipse_energize>("spell_dru_eclipse_energize");
    new spell_script<spell_dru_eclipse_solar>("spell_dru_eclipse_solar");
    new spell_script<spell_dru_eclipse_lunar>("spell_dru_eclipse_lunar");
    new spell_script<spell_dru_glyph_of_omens>("spell_dru_glyph_of_omens");
    new aura_script<spell_dru_eclipse_remove>("spell_dru_eclipse_remove");
    new aura_script<spell_dru_eclipse_effect>("spell_dru_eclipse_effect");
    new spell_dru_t10_restoration_4p_bonus();
    new spell_dru_swift_flight_passive();
    new aura_script<spell_dru_savage_roar>("spell_dru_savage_roar");
    new spell_script<spell_dru_savage_roar_duration_check>("spell_dru_savage_roar_duration_check");
    new spell_dru_survival_instincts();
    new spell_dru_yseras_gift();
    new spell_dru_yseras_gift_heal_ally();
    new aura_script<spell_dru_t15_restoration_4p_bonus>("spell_dru_t15_restoration_4p_bonus");
    new spell_script<spell_dru_swiftmend>("spell_dru_swiftmend");
    new aura_script<spell_dru_efflorescence>("spell_dru_efflorescence");
    new spell_script<spell_dru_efflorescence_heal>("spell_dru_efflorescence_heal");
    new spell_script<spell_dru_faerie_fire>("spell_dru_faerie_fire");
    new spell_script<spell_dru_lacerate>("spell_dru_lacerate");
    new aura_script<spell_dru_primal_fury>("spell_dru_primal_fury");
    new spell_script<spell_dru_ferocious_bite>("spell_dru_ferocious_bite");
    new aura_script<spell_dru_harmony>("spell_dru_harmony");
    new spell_script<spell_dru_tree_of_life>("spell_dru_tree_of_life");
    new spell_script<spell_dru_wild_growth>("spell_dru_wild_growth");
    new aura_script<spell_dru_wild_growth_heal>("spell_dru_wild_growth_heal");
    new spell_script<spell_dru_healing_touch_dream_of_cenarius>("spell_dru_healing_touch_dream_of_cenarius");
    new spell_script<spell_dru_wrath_dream_of_cenarius>("spell_dru_wrath_dream_of_cenarius");
    new aura_script<spell_dur_dream_of_cenarius>("spell_dur_dream_of_cenarius");
    new spell_script<spell_dru_wild_mushroom>("spell_dru_wild_mushroom");
    new spell_script<spell_dru_wild_mushroom_detonate>("spell_dru_wild_mushroom_detonate");
    new spell_script<spell_dru_wild_mushroom_bloom>("spell_dru_wild_mushroom_bloom");
    new spell_script<spell_dru_wild_mushroom_heal>("spell_dru_wild_mushroom_heal");
    new aura_script<spell_dru_wild_mushroom_overheal>("spell_dru_wild_mushroom_overheal");
    new creature_script<npc_wild_mushroom>("npc_wild_mushroom");
    new creature_script<npc_fungal_growth>("npc_fungal_growth");
    new aura_script<spell_dru_lifebloom>("spell_dru_lifebloom");
    new spell_script<spell_dru_lifebloom_refresh>("spell_dru_lifebloom_refresh");
    new spell_script<spell_dru_swipe>("spell_dru_swipe");
    new atrigger_script<sat_druid_ursols_vortex>("sat_druid_ursols_vortex");
    new aura_script<spell_dru_heart_of_the_wild>("spell_dru_heart_of_the_wild");
    new aura_script<spell_dru_heart_of_the_wild_bonus>("spell_dru_heart_of_the_wild_bonus");
    new spell_script<spell_dru_savage_roar_glyphed>("spell_dru_savage_roar_glyphed");
    new aura_script<spell_dru_living_seed>("spell_dru_living_seed");
    new aura_script<spell_dru_living_seed_heal>("spell_dru_living_seed_heal");
    new aura_script<spell_dru_starfall>("spell_dru_starfall");
    new spell_script<spell_dru_starfall_damage>("spell_dru_starfall_damage");
    new spell_script<spell_dru_thrash_bear>("spell_dru_thrash_bear");
    new aura_script<spell_dru_shooting_stars_trigger>("spell_dru_shooting_stars_trigger");
    new aura_script<spell_dru_shooting_stars_proc>("spell_dru_shooting_stars_proc");
    new aura_script<spell_dru_soul_of_the_forest>("spell_dru_soul_of_the_forest");
    new spell_script<spell_dru_soul_of_the_forest_mangle>("spell_dru_soul_of_the_forest_mangle");
    new spell_script<spell_dru_soul_of_the_forest_feral>("spell_dru_soul_of_the_forest_feral");
    new spell_script<spell_dru_shred>("spell_dru_shred");
    new aura_script<spell_dru_item_pvp_feral_4p>("spell_dru_item_pvp_feral_4p");
    new aura_script<spell_dru_stampede_effect>("spell_dru_stampede_effect");
    new aura_script<spell_dru_stampede_timer>("spell_dru_stampede_timer");
    new aura_script<spell_dru_override_ravage>("spell_dru_override_ravage");
    new aura_script<spell_dru_leader_of_the_pack>("spell_dru_leader_of_the_pack");
    new aura_script<spell_dru_bear_form>("spell_dru_bear_form");
    new spell_script<spell_dru_force_of_nature_balance>("spell_dru_force_of_nature_balance");
    new spell_script<spell_dru_force_of_nature_restoration>("spell_dru_force_of_nature_restoration");
    new spell_script<spell_dru_force_of_nature_feral>("spell_dru_force_of_nature_feral");
    new spell_script<spell_dru_force_of_nature_guradian>("spell_dru_force_of_nature_guradian");
    new creature_script<npc_force_of_nature_balance>("npc_force_of_nature_balance");
    new creature_script<npc_force_of_nature_restoration>("npc_force_of_nature_restoration");
    new aura_script<spell_dru_force_of_nature_rake>("spell_dru_force_of_nature_rake");
    new spell_script<spell_dru_cat_form_check>("spell_dru_cat_form_check");
    new aura_script<spell_dru_travel_form>("spell_dru_travel_form");
    new spell_script<spell_dru_glyph_of_fae_silence>("spell_dru_glyph_of_fae_silence");
    new aura_script<spell_dru_bear_form_passive>("spell_dru_bear_form_passive");
    new aura_script<spell_dru_glyph_of_fae_silence_cooldown>("spell_dru_glyph_of_fae_silence_cooldown");
    new aura_script<spell_dru_t15_guardian_4p_bonus>("spell_dru_t15_guardian_4p_bonus");
    new aura_script<spell_dru_t16_guardian_2p_bonus>("spell_dru_t16_guardian_2p_bonus");
    new spell_script<spell_dur_t16_guardian_4p_bonus>("spell_dur_t16_guardian_4p_bonus");
    new spell_script<spell_dru_t16_feral_2p_bonus>("spell_dru_t16_feral_2p_bonus");
    new aura_script<spell_dru_t16_feral_4p_bonus>("spell_dru_t16_feral_4p_bonus");
    new aura_script<spell_dru_feral_rage>("spell_dru_feral_rage");
    new aura_script<spell_dru_t16_balance_2p_bonus>("spell_dru_t16_balance_2p_bonus");
    new aura_script<spell_dru_sage_mender>("spell_dru_sage_mender");
    new aura_script<spell_dru_vengeance>("spell_dru_vengeance");
    new aura_script<spell_dru_glyph_of_rejuvenation>("spell_dru_glyph_of_rejuvenation");
    new aura_script<spell_dru_glyph_of_healing_touch>("spell_dru_glyph_of_healing_touch");
    new aura_script<spell_dru_symbiosis_caster_aura>("spell_dru_symbiosis_caster_aura");
    new aura_script<spell_dru_symbiosis_target_check>("spell_dru_symbiosis_target_check");
    new spell_script<spell_dru_symbiosis>("spell_dru_symbiosis");
    new aura_script<spell_dru_symbiosis_fortifying_brew>("spell_dru_symbiosis_fortifying_brew");
    new aura_script<spell_dru_killer_instinct>("spell_dru_killer_instinct");
    new aura_script<spell_dru_sudden_eclipse>("spell_dru_sudden_eclipse");
    new aura_script<spell_dru_prowl>("spell_dru_prowl");
    new aura_script<spell_dru_glyph_of_hurricane>("spell_dru_glyph_of_hurricane");
    new aura_script<spell_dru_shapeshift_move_speed>("spell_dru_shapeshift_move_speed");
    new aura_script<spell_dru_berserk_bear>("spell_dru_berserk_bear");
    new aura_script<spell_dru_glyph_of_the_predator>("spell_dru_glyph_of_the_predator");
    new aura_script<spell_dru_glyph_of_the_chameleon_fix>("spell_dru_glyph_of_the_chameleon_fix");
    new aura_script<spell_dru_glyph_of_the_chameleon>("spell_dru_glyph_of_the_chameleon");
    new spell_script<spell_dru_intimidating_roar>("spell_dru_intimidating_roar");
    new spell_druid_celestial_alignment();
    new spell_druid_astral_communion();
}
