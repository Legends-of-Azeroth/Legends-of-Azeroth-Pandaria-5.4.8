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
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "Unit.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "Totem.h"
#include "PetAI.h"
#include "Battleground.h"
#include "spell_common.h"
#include "PassiveAI.h"
#include "Pet.h"
#include "Random.h"

enum ShamanSpells
{
    SPELL_SHA_LIGHTNING_SHIELD_AURA         = 324,
    SPELL_SHA_ASCENDANCE_ELEMENTAL          = 114050,
    SPELL_SHA_ASCENDANCE_RESTORATION        = 114052,
    SPELL_SHA_ASCENDANCE_ENHANCED           = 114051,
    SPELL_SHA_ASCENDANCE                    = 114049,
    SPELL_SHA_HEALING_RAIN                  = 73920,
    SPELL_SHA_HEALING_RAIN_TICK             = 73921,
    SPELL_SHA_HEALING_RAIN_VISUAL           = 147490,
    SPELL_SHA_EARTHQUAKE                    = 61882,
    SPELL_SHA_EARTHQUAKE_TICK               = 77478,
    SPELL_SHA_EARTHQUAKE_KNOCKING_DOWN      = 77505,
    SPELL_SHA_ELEMENTAL_BLAST               = 117014,
    SPELL_SHA_ELEMENTAL_BLAST_RATING_BONUS  = 118522,
    SPELL_SHA_ELEMENTAL_BLAST_NATURE_VISUAL = 118517,
    SPELL_SHA_ELEMENTAL_BLAST_FROST_VISUAL  = 118515,
    SPELL_SHA_ELEMENTAL_BLAST_EO_PROC       = 120588,
    SPELL_SHA_LAVA_LASH                     = 60103,
    SPELL_SHA_FLAME_SHOCK                   = 8050,
    SPELL_SHA_STORMSTRIKE                   = 17364,
    SPELL_SHA_LIGHTNING_SHIELD_ORB_DAMAGE   = 26364,
    SPELL_SHA_GLYPH_OF_HEALING_STREAM       = 119523,
    SPELL_SHA_LAVA_SURGE_CAST_TIME          = 77762,
    SPELL_SHA_FULMINATION                   = 88766,
    SPELL_SHA_FULMINATION_TRIGGERED         = 88767,
    SPELL_SHA_FULMINATION_INFO              = 95774,
    SPELL_SHA_ROLLING_THUNDER_AURA          = 88764,
    SPELL_SHA_ROLLING_THUNDER_ENERGIZE      = 88765,
    SPELL_SHA_UNLEASH_ELEMENTS              = 73680,
    SPELL_SHA_FIRE_NOVA                     = 1535,
    SPELL_SHA_FIRE_NOVA_TRIGGERED           = 8349,
    SPELL_SHA_TIDAL_WAVES                   = 51564,
    SPELL_SHA_TIDAL_WAVES_PROC              = 53390,
    SPELL_SHA_MANA_TIDE                     = 16191,
    SPELL_SHA_FROST_SHOCK_FREEZE            = 63685,
    SPELL_SHA_FROZEN_POWER                  = 63374,
    SPELL_SHA_MAIL_SPECIALIZATION_AGI       = 86099,
    SPELL_SHA_MAIL_SPECIALISATION_INT       = 86100,
    SPELL_SHA_UNLEASH_LIFE                  = 73685,
    SPELL_SHA_UNLEASH_EARTH                 = 73684,
    SPELL_SHA_UNLEASHED_FURY_TALENT         = 117012,
    SPELL_SHA_UNLEASHED_FURY_FLAMETONGUE    = 118470,
    SPELL_SHA_UNLEASHED_FURY_WINDFURY       = 118472,
    SPELL_SHA_UNLEASHED_FURY_EARTHLIVING    = 118473,
    SPELL_SHA_UNLEASHED_FURY_FROSTBRAND     = 118474,
    SPELL_SHA_UNLEASHED_FURY_ROCKBITER      = 118475,
    SPELL_SHA_STONE_BULWARK_ABSORB          = 114893,
    SPELL_SHA_STONE_BULWARK_TOTEM_PASSIVE   = 114889,
    SPELL_SHA_EARTHGRAB_IMMUNITY            = 116946,
    SPELL_SHA_EARTHBIND_FOR_EARTHGRAB_TOTEM = 116947,
    SPELL_SHA_EARTHGRAB_ROOT                = 64695,
    SPELL_SHA_ECHO_OF_THE_ELEMENTS          = 108283,
    SPELL_SHA_ANCESTRAL_GUIDANCE            = 114911,
    SPELL_SHA_GLYPH_OF_LAKESTRIDER          = 55448,
    SPELL_SHA_WATER_WALKING                 = 546,
    SPELL_SHA_GLYPH_OF_SHAMANISTIC_RAGE     = 63280,
    SPELL_SHA_GHOST_WOLF                    = 2645,
    SPELL_SHA_ITEM_T14_4P                   = 123124,
    SPELL_SHA_GLYPH_OF_HEALING_STREAM_TOTEM = 55456,
    SPELL_SHA_LIGHTNING_BOLT                = 403,
    SPELL_SHA_LIGHTNING_BOLT_EO_PROC        = 45284,
    SPELL_SHA_CHAIN_LIGHTNING               = 421,
    SPELL_SHA_CHAIN_LIGHTNING_EO_PROC       = 45297,
    SPELL_SHA_LAVA_BURST                    = 51505,
    SPELL_SHA_LAVA_BURST_EO_PROC            = 77451,
    SPELL_SHA_LAVA_BEAM                     = 114074,
    SPELL_SHA_LAVA_BEAM_EO_PROC             = 114738,
    SPELL_SHA_LIGHTNING_SHIELD_DAMAGE       = 26364,
    SPELL_SHA_EARTH_SHIELD_HEAL             = 379,
    SPELL_SHA_EARTH_SHIELD                  = 974,
    SPELL_SHA_RIPTIDE                       = 61295,
    SPELL_SHA_PURIFICATION                  = 16213,
    SPELL_SHA_T14_RESTOR_P4                 = 123135,
    SPELL_SHA_NATURES_GUARDIAN              = 31616,
    SPELL_SHA_SPIRIT_LINK                   = 98021,
    SPELL_SHA_SEARING_FLAMES                = 77657,
    SPELL_SHA_SEARING_FLAMES_EFFECT         = 77661,
    SPELL_SHA_GLYPH_OF_LAVA_LASH            = 55444,
    SPELL_SHA_LAVA_LASH_SPREAD_FLAME_SHOK   = 105792,
    SPELL_SHA_RUSHING_STREAMS               = 147074,
    SPELL_SHA_GLYPH_OF_LIGHTNING_SHIELD     = 101052,
    SPELL_SHA_GLYPH_OF_LIGHTNING_SHIELD_MOD = 142912,
    SPELL_SHA_GLYPH_OF_PURGING              = 147762,
    SPELL_SHA_MAELSTROM_WEAPON              = 53817,
    SPELL_SHA_MAELSTROM_WEAPON_FULL_STACKS  = 60349,
    SPELL_SHA_ANCESTRAL_AWAKENING           = 52752,
    SPELL_SHA_ANCESTRAL_AWAKENING_SELECTOR  = 52759,
    SPELL_SHA_ANCESTRAL_VIGOR               = 105284,
    SPELL_SHA_CHAIN_HEAL                    = 1064,
    SPELL_SHA_MASTERT_ENHANCED_ELEMENTS     = 77223,
    SPELL_SHA_ELEMENTAL_FURY                = 60188,
    SPELL_SHA_FLAMETONGUE_ATTACK            = 10444,
    SPELL_SHA_A15_4P                        = 131554,
    SPELL_SHA_FROSTFLAME_ATTACK             = 147732,
    SPELL_SHA_WATER_SHIELD                  = 52127,
    SPELL_SHA_RESURGENCE                    = 101033,
    SPELL_SHA_FLAMES_OF_LIFE                = 137808,
    SPELL_SHA_SPIRITWALKERS_AEGIS           = 131557,
    SPELL_SHA_SPIRITWALKERS_AEGIS_EFFECT    = 131558,
    SPELL_SHA_ENCHANCED_ELEMENTS            = 77223,
    SPELL_SHA_STORMLASH                     = 120687,
    SPELL_SHA_GLYPH_OF_CLEANSING_WATERS     = 55445,
    SPELL_SHA_CLEANSING_WATERS              = 86961,
    SPELL_SHA_ANCESTRAL_SWIFTNESS           = 16188,
    SPELL_SHA_HEALING_SURGE                 = 8004,
    SPELL_SHA_T15_ENHANCEMENT_2P_BONUS      = 138136,
    SPELL_SHA_T15_ENHANCEMENT_4P_BONUS      = 138141,
    SPELL_SHA_FERAL_SPIRIT                  = 51533,
    SPELL_SHA_T15_RESTORATION_2P_BONUS      = 138303,
    SPELL_SHA_HEALING_STREAM_TOTEM_BONUS    = 138304,
    SPELL_SHA_T15_RESTORATION_4P_BONUS      = 138305,
    SPELL_SHA_T15_ELEMENTAL_4P_BONUS        = 138144,
    SPELL_SHA_T16_RESTORATION_2P_BONUS      = 145378,
    SPELL_SHA_NATURES_BARRIER               = 145379,
    SPELL_SHA_T16_RESTORATION_4P_COSMETIC   = 145384,
    SPELL_SHA_T16_RESTORATION_4P_SELECTOR   = 145399,
    SPELL_SHA_ELEMENTAL_DISCHARGE           = 144999,
    SPELL_SHA_GLYPH_OF_HEALING_WAVE         = 55533,
    SPELL_SHA_GLYPH_OF_THUNDERSTORM         = 62132,
    SPELL_SHA_RESTORATIVE_MISTS             = 114083,
    SPELL_SHA_EARTH_ELEMENTAL_TOTEM         = 2062,
    SPELL_SHA_FIRE_ELEMENTAL_TOTEM          = 2894,
    SPELL_SHA_GLYPH_OF_SPIRIT_WOLF          = 147770,
    SPELL_SHA_GROUNDING_TOTEM_EFFECT        = 8178,
    SPELL_SHA_GROUNDING_TOTEM_EFFECT_REFLECT= 89523,
    SPELL_SHA_ELEMENTAL_TOTEM               = 34967,
    SPELL_SHA_ELEMENTAL_FAMILIAR_MARKER     = 148098,

    SPELL_WARLOCK_DRAIN_SOUL                = 1120,
    SPELL_ROUGE_SINISTER_STRIKE             = 1752,
};

enum
{
    NPC_FIRE_ELEMENTAL                      = 15438,
    NPC_EARTH_ELEMENTAL                     = 15352,
    NPC_PRIMAL_FIRE_ELEMENTAL               = 61029,
    NPC_PRIMAL_EARTH_ELEMENTAL              = 61056,
    NPC_HEALING_RAN_INVISIBLE_STALKER_DND   = 73400,
};

// Hex - 51514
class spell_sha_hex : public SpellScriptLoader
{
    public:
        spell_sha_hex() : SpellScriptLoader("spell_sha_hex") { }

        class spell_sha_hex_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_hex_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    if (target->IsMounted())
                    {
                        target->Dismount();
                        target->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    }

                    if (target->HasUnitState(UNIT_STATE_CASTING))
                    {
                        target->InterruptSpell(CURRENT_GENERIC_SPELL);
                        target->InterruptSpell(CURRENT_CHANNELED_SPELL);
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_sha_hex_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_hex_AuraScript();
        }
};

// Prowl - 113289
class spell_sha_prowl : public SpellScriptLoader
{
    public:
        spell_sha_prowl() : SpellScriptLoader("spell_sha_prowl") { }

        class spell_sha_prowl_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_prowl_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->CastSpell(_player, SPELL_SHA_GHOST_WOLF, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_prowl_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_prowl_SpellScript();
        }
};

// 30823 - Shamanistic Rage
class spell_sha_glyph_of_shamanistic_rage : public SpellScript
{
    PrepareSpellScript(spell_sha_glyph_of_shamanistic_rage);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_SHAMANISTIC_RAGE))
        {
            DispelChargesList dispelList;
            GetCaster()->GetDispellableAuraList(GetCaster(), DISPEL_ALL_MASK, dispelList);
            for (auto&& itr : dispelList)
                GetCaster()->RemoveAurasDueToSpellByDispel(itr.first->GetId(), GetSpellInfo()->Id, itr.first->GetCasterGUID(), GetCaster(), itr.second);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_glyph_of_shamanistic_rage::HandleHit);
    }
};

// Called by Ghost Wolf - 2645
// Glyph of Lakestrider - 55448
class spell_sha_glyph_of_lakestrider : public SpellScriptLoader
{
    public:
        spell_sha_glyph_of_lakestrider() : SpellScriptLoader("spell_sha_glyph_of_lakestrider") { }

        class spell_sha_glyph_of_lakestrider_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_glyph_of_lakestrider_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(SPELL_SHA_GLYPH_OF_LAKESTRIDER))
                        _player->CastSpell(_player, SPELL_SHA_WATER_WALKING, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_glyph_of_lakestrider_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_glyph_of_lakestrider_SpellScript();
        }
};

// Mail Specialization - 86529
class spell_sha_mail_specialization : public SpellScriptLoader
{
    public:
        spell_sha_mail_specialization() : SpellScriptLoader("spell_sha_mail_specialization") { }

        class spell_sha_mail_specialization_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_mail_specialization_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->GetTalentSpecialization() == SPEC_SHAMAN_ELEMENTAL
                            || _player->GetTalentSpecialization() == SPEC_SHAMAN_RESTORATION)
                        _player->CastSpell(_player, SPELL_SHA_MAIL_SPECIALISATION_INT, true);
                    else if (_player->GetTalentSpecialization() == SPEC_SHAMAN_ENHANCEMENT)
                        _player->CastSpell(_player, SPELL_SHA_MAIL_SPECIALIZATION_AGI, true);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->HasAura(SPELL_SHA_MAIL_SPECIALISATION_INT))
                        _player->RemoveAura(SPELL_SHA_MAIL_SPECIALISATION_INT);
                    else if (_player->HasAura(SPELL_SHA_MAIL_SPECIALIZATION_AGI))
                        _player->RemoveAura(SPELL_SHA_MAIL_SPECIALIZATION_AGI);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sha_mail_specialization_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sha_mail_specialization_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_mail_specialization_AuraScript();
        }
};

// Frost Shock - 8056
class spell_sha_frozen_power : public SpellScriptLoader
{
    public:
        spell_sha_frozen_power() : SpellScriptLoader("spell_sha_frozen_power") { }

        class spell_sha_frozen_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_frozen_power_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(8056))
                    return false;
                return true;
            }

            void HandleAfterHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        if (_player->HasAura(SPELL_SHA_FROZEN_POWER))
                            _player->CastSpell(target, SPELL_SHA_FROST_SHOCK_FREEZE, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sha_frozen_power_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_frozen_power_SpellScript();
        }
};

static void UnleashElemts(Player* player, uint32 enchantment, Unit* target)
{
    uint32 unleashSpell = 0;
    uint32 furySpell = 0;
    bool hasFuryTalent = player->HasAura(SPELL_SHA_UNLEASHED_FURY_TALENT);
    bool hostileTarget = player->IsValidAttackTarget(target);
    bool hostileSpell = true;
    switch (enchantment)
    {
        case 3345: // Earthliving Weapon
            unleashSpell = 73685; // Unleash Life
            hostileSpell = false;

            if (hasFuryTalent)
                furySpell = SPELL_SHA_UNLEASHED_FURY_EARTHLIVING;
            break;
        case 5: // Flametongue Weapon
            unleashSpell = 73683; // Unleash Flame

            if (hasFuryTalent)
                furySpell = SPELL_SHA_UNLEASHED_FURY_FLAMETONGUE;
            break;
        case 2: // Frostbrand Weapon
            unleashSpell = 73682; // Unleash Frost

            if (hasFuryTalent)
                furySpell = SPELL_SHA_UNLEASHED_FURY_FROSTBRAND;
            break;
        case 3021: // Rockbiter Weapon
            unleashSpell = 73684; // Unleash Earth

            if (hasFuryTalent)
                furySpell = SPELL_SHA_UNLEASHED_FURY_ROCKBITER;
            break;
        case 283: // Windfury Weapon
            unleashSpell = 73681; // Unleash Wind

            if (hasFuryTalent)
                furySpell = SPELL_SHA_UNLEASHED_FURY_WINDFURY;
            break;
        default:
            return;
    }

    if (hostileSpell && !hostileTarget)
        return;             // Don't allow to attack non-hostile targets

    if (!hostileSpell && hostileTarget)
        target = player;    // Heal ourselves instead of the enemy

    if (unleashSpell)
        player->CastSpell(target, unleashSpell, true);

    if (furySpell)
        player->CastSpell(target, furySpell, true);
}

// 73680 - Unleash Elements
class spell_sha_unleash_elements : public SpellScript
{
    PrepareSpellScript(spell_sha_unleash_elements);

    SpellCastResult CheckEnchant()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_DONT_REPORT;

        if (!GetExplTargetUnit())
            return SPELL_FAILED_BAD_TARGETS;

        Item* weapons[2];
        weapons[0] = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        weapons[1] = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        bool enchantFound = false;
        bool badTarget = false;

        for (auto&& weapon : weapons)
        {
            if (!weapon)
                continue;

            bool hostileSpell = true;

            switch (weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
            {
                case 3345:// Earthliving Weapon
                    if (player->IsValidAttackTarget(GetExplTargetUnit()))
                    {
                        badTarget = true;
                        break;
                    }
                    enchantFound = true;
                    break;
                case 5:   // Flametongue Weapon
                case 2:   // Frostbrand Weapon
                case 3021:// Rockbiter Weapon
                case 283: // Windfury Weapon
                    if (!player->IsValidAttackTarget(GetExplTargetUnit()))
                    {
                        badTarget = true;
                        break;
                    }
                    enchantFound = true;
                    break;
                default:
                    break;
            }
        }

        if (badTarget)
            return SPELL_FAILED_BAD_TARGETS;

        if (!enchantFound)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            Item* weapons[2];
            weapons[0] = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            weapons[1] = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

            for (auto&& weapon : weapons)
            {
                if (!weapon)
                    continue;

                UnleashElemts(player, weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT), GetHitUnit());

                // If weapons are enchanted by same enchantment, only one should be unleashed
                if (weapon == weapons[0] && weapons[1])
                    if (weapons[0]->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == weapons[1]->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                        return;
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_unleash_elements::CheckEnchant);
        OnHit += SpellHitFn(spell_sha_unleash_elements::HandleHit);
    }
};

// 331 - Healing Wave
// 8004 - Healing Surge
// 61295 - Riptide
// 77472 - Greater Healing Wave
// 126371 - Deluge
class spell_sha_unleashed_fury_earthliving : public SpellScript
{
    PrepareSpellScript(spell_sha_unleashed_fury_earthliving);

    void HandleHit()
    {
        if (AuraEffect* unleahedFury = GetCaster()->GetAuraEffect(SPELL_SHA_UNLEASHED_FURY_EARTHLIVING, EFFECT_0))
        {
            int32 heal = GetHitHeal();
            AddPct(heal, unleahedFury->GetAmount());
            SetHitHeal(heal);
            GetCaster()->RemoveAura(unleahedFury->GetBase());
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_unleashed_fury_earthliving::HandleHit);
    }
};

// 61295 - Riptide
class spell_sha_unleashed_fury_earthliving_riptide : public AuraScript
{
    PrepareAuraScript(spell_sha_unleashed_fury_earthliving_riptide);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect* unleahedFury = caster->GetAuraEffect(SPELL_SHA_UNLEASHED_FURY_EARTHLIVING, EFFECT_0))
                AddPct(amount, unleahedFury->GetAmount());
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_unleashed_fury_earthliving_riptide::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 118475 - Unleashed Fury
class spell_sha_unleashed_fury_rockbiter : public AuraScript
{
    PrepareAuraScript(spell_sha_unleashed_fury_rockbiter);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void CalculateAbsorb(AuraEffect* eff, DamageInfo& dmgInfo, uint32& absorbed)
    {
        if (dmgInfo.GetAttacker() && dmgInfo.GetAttacker()->HasAura(SPELL_SHA_UNLEASH_EARTH, GetCasterGUID()))
            absorbed = CalculatePct(dmgInfo.GetDamage(), eff->GetBaseAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_unleashed_fury_rockbiter::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_unleashed_fury_rockbiter::CalculateAbsorb, EFFECT_0);
    }
};

// 73682 - Unleash Frost
class spell_sha_unleash_frost : public AuraScript
{
    PrepareAuraScript(spell_sha_unleash_frost);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        for (auto&& itr : GetUnitOwner()->GetAppliedAuras())
        {
            SpellInfo const* spellInfo = itr.second->GetBase()->GetSpellInfo();
            if (spellInfo->SchoolMask & SPELL_SCHOOL_MASK_FROST && spellInfo->GetAllEffectsMechanicMask() & (1 << MECHANIC_SNARE))
            {
                amount = 70.0f;
                break;
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_unleash_frost::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED);
    }
};

// 8042 - Earth Shock
class spell_sha_fulmination : public SpellScript
{
    PrepareSpellScript(spell_sha_fulmination);

    void HandleCast()
    {
        Unit* shaman = GetCaster();
        Unit* target = GetExplTargetUnit();

        AuraEffect* fulminationAura = shaman->GetAuraEffect(SPELL_SHA_FULMINATION, EFFECT_0);
        if (!fulminationAura)
            return;

        Aura* lightningShield = shaman->GetAura(SPELL_SHA_LIGHTNING_SHIELD_AURA);
        if (!lightningShield)
            return;

        uint8 lsCharges = lightningShield->GetCharges();
        if (lightningShield->GetCharges() <= 1)
            return;

        uint8 usedCharges = lightningShield->GetCharges() - 1;

        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_SHA_LIGHTNING_SHIELD_ORB_DAMAGE))
        {
            int32 basePoints = shaman->CalculateSpellDamage(target, spellInfo, EFFECT_0);
            uint32 damage = usedCharges * shaman->SpellDamageBonusDone(target, spellInfo, EFFECT_0, basePoints, SPELL_DIRECT_DAMAGE);

            shaman->CastCustomSpell(SPELL_SHA_FULMINATION_TRIGGERED, SPELLVALUE_BASE_POINT0, damage, target, true);
            lightningShield->SetCharges(1);

            shaman->RemoveAura(SPELL_SHA_FULMINATION_INFO);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_fulmination::HandleCast);
    }
};

// 77756 - Lava Surge
class spell_sha_lava_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_lava_surge);

    void HandleProc(ProcEventInfo&)
    {
        if (Player* player = GetUnitOwner()->ToPlayer())
        {
            player->CastSpell(player, SPELL_SHA_LAVA_SURGE_CAST_TIME, true);
            player->RemoveSpellCooldown(SPELL_SHA_LAVA_BURST, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_lava_surge::HandleProc);
    }
};

// 51505 - Lava Burst
class spell_sha_lava_surge_cooldown_fix : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_surge_cooldown_fix);

    bool instant = false;

    void HandleStartCast()
    {
        instant = GetCaster()->HasAura(SPELL_SHA_LAVA_SURGE_CAST_TIME);
    }

    void HandleCast()
    {
        if (!instant && GetCaster()->HasAura(SPELL_SHA_LAVA_SURGE_CAST_TIME))
            GetCaster()->GetSpellHistory()->RemoveCooldown(SPELL_SHA_LAVA_BURST, true);
    }

    void Register() override
    {
        OnSpellStart += SpellCastFn(spell_sha_lava_surge_cooldown_fix::HandleStartCast);
        AfterCast += SpellCastFn(spell_sha_lava_surge_cooldown_fix::HandleCast);
    }
};

// Elemental Blast - 117014
class spell_sha_elemental_blast : public SpellScriptLoader
{
    public:
        spell_sha_elemental_blast() : SpellScriptLoader("spell_sha_elemental_blast") { }

        class spell_sha_elemental_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_elemental_blast_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHA_ELEMENTAL_BLAST))
                    return false;
                return true;
            }

            void HandleAfterCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    _player->CastSpell(_player, SPELL_SHA_ELEMENTAL_BLAST_RATING_BONUS, true);

                    if (Aura* const aura = _player->GetAura(SPELL_SHA_ELEMENTAL_BLAST_RATING_BONUS, _player->GetGUID()))
                    {
                        uint32 randomEffect = 0;

                        if (_player->GetTalentSpecialization() == SPEC_SHAMAN_ENHANCEMENT)
                            randomEffect = urand(0, 3);
                        else
                            randomEffect = urand(0, 2);

                        // Iterate over all aura effects
                        for (int i = 0; i < 4; ++i)
                        {
                            if (i == randomEffect)
                                aura->GetEffect(i)->ChangeAmount(3500);
                            else
                            {
                                aura->GetEffect(i)->ChangeAmount(0);
                            }
                        }
                    }

                    if (Unit* target = GetExplTargetUnit())
                    {
                        _player->CastSpell(target, SPELL_SHA_ELEMENTAL_BLAST_FROST_VISUAL, true);
                        _player->CastSpell(target, SPELL_SHA_ELEMENTAL_BLAST_NATURE_VISUAL, true);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sha_elemental_blast_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_elemental_blast_SpellScript();
        }
};

// Earthquake : Ticks - 77478
class spell_sha_earthquake_tick : public SpellScriptLoader
{
    public:
        spell_sha_earthquake_tick() : SpellScriptLoader("spell_sha_earthquake_tick") { }

        class spell_sha_earthquake_tick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthquake_tick_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHA_EARTHQUAKE_TICK))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                // With a 10% chance of knocking down affected targets
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        if (roll_chance_i(10))
                            _player->CastSpell(target, SPELL_SHA_EARTHQUAKE_KNOCKING_DOWN, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sha_earthquake_tick_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_earthquake_tick_SpellScript();
        }
};

// Earthquake - 61882
class spell_sha_earthquake : public SpellScriptLoader
{
    public:
        spell_sha_earthquake() : SpellScriptLoader("spell_sha_earthquake") { }

        class spell_sha_earthquake_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthquake_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                if (DynamicObject* dynObj = GetCaster()->GetDynObject(SPELL_SHA_EARTHQUAKE))
                    GetCaster()->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), SPELL_SHA_EARTHQUAKE_TICK, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthquake_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sha_earthquake_AuraScript();
        }
};

// 73920 - Healing Rain
class spell_sha_healing_rain : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_rain);

    void HandleOnHit()
    {
        if (WorldLocation const* loc = GetExplTargetDest())
        {
            std::list<TempSummon*> old;
            GetCaster()->GetSummons(old, NPC_HEALING_RAN_INVISIBLE_STALKER_DND);
            for (auto&& it : old)
                it->UnSummon();

            TempSummon* summon = GetCaster()->SummonCreature(NPC_HEALING_RAN_INVISIBLE_STALKER_DND, *loc);
            if (summon)
                summon->CastSpell(summon, SPELL_SHA_HEALING_RAIN_VISUAL);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_healing_rain::HandleOnHit);
    }
};

// 73920 - Healing Rain
class spell_sha_healing_rain_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_healing_rain_aura);

    bool m_unleashLife = false;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* shaman = GetCaster())
        {
            if (Aura* unleashLife = shaman->GetAura(SPELL_SHA_UNLEASH_LIFE))
            {
                m_unleashLife = true;
                shaman->RemoveAura(unleashLife);
            }
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* shaman = GetCaster())
        {
            std::list<TempSummon*> summons;
            shaman->GetSummons(summons, NPC_HEALING_RAN_INVISIBLE_STALKER_DND);
            if (!summons.empty())
                summons.front()->UnSummon();
        }
    }

    void HandlePeriodic(AuraEffect const*)
    {
        if (Unit* shaman = GetCaster())
        {
            std::list<TempSummon*> summons;
            shaman->GetSummons(summons, NPC_HEALING_RAN_INVISIBLE_STALKER_DND);
            if (!summons.empty())
            {
                CustomSpellValues ctx;
                if (m_unleashLife)
                    ctx.SetMultiplier(1.3f);
                shaman->CastCustomSpell(SPELL_SHA_HEALING_RAIN_TICK, ctx, summons.front(), TRIGGERED_FULL_MASK);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_healing_rain_aura::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_sha_healing_rain_aura::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_rain_aura::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Ascendance - 114049
class spell_sha_ascendance : public SpellScriptLoader
{
    public:
        spell_sha_ascendance() : SpellScriptLoader("spell_sha_ascendance") { }

        class spell_sha_ascendance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_ascendance_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHA_ASCENDANCE))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->GetTalentSpecialization() == SPEC_NONE)
                    {
                        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_SELECT_TALENT_SPECIAL);
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }

                    return SPELL_CAST_OK;
                }

                return SPELL_CAST_OK;
            }

            void HandleAfterCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    switch(_player->GetTalentSpecialization())
                    {
                        case SPEC_SHAMAN_ELEMENTAL:
                            // Remove current spell cooldown
                            _player->RemoveSpellCooldown(51505, true);
                            _player->CastSpell(_player, SPELL_SHA_ASCENDANCE_ELEMENTAL, true);
                            break;
                        case SPEC_SHAMAN_ENHANCEMENT:
                            _player->RemoveSpellCooldown(SPELL_SHA_STORMSTRIKE, true);
                            _player->RemoveSpellCooldown(115356, true);
                            _player->CastSpell(_player, SPELL_SHA_ASCENDANCE_ENHANCED, true);
                            break;
                        case SPEC_SHAMAN_RESTORATION:
                            _player->CastSpell(_player, SPELL_SHA_ASCENDANCE_RESTORATION, true);
                            break;
                        default:
                            break;
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_sha_ascendance_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_sha_ascendance_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_ascendance_SpellScript();
        }
};

class EarthenPowerTargetSelector
{
    public:
        EarthenPowerTargetSelector() { }

        bool operator() (WorldObject* target)
        {
            if (!target->ToUnit())
                return true;

            if (!target->ToUnit()->HasAuraWithMechanic(1 << MECHANIC_SNARE))
                return true;

            return false;
        }
};

// Lava Burst - 51505
class spell_sha_lava_burst : public SpellScriptLoader
{
    public:
        spell_sha_lava_burst() : SpellScriptLoader("spell_sha_lava_burst") { }

        class spell_sha_lava_burst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_lava_burst_SpellScript)

            enum
            {
                SPELL_FLAME_SHOCK       = 8050,
                SPELL_LAVA_SURGE_PROC   = 77762,
            };

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                Unit* const target = GetHitUnit();
                if (!target || !target->HasAura(SPELL_FLAME_SHOCK, GetCaster()->GetGUID()))
                    return;

                SetHitDamage(GetHitDamage() * 1.5f);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_lava_burst_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sha_lava_burst_SpellScript();
        }
};

// Lava Burst (Mastery Proc) - 77451
class spell_sha_lava_burst_mastery_proc : public SpellScriptLoader
{
    class script_impl : public SpellScript
    {
        PrepareSpellScript(script_impl)

        enum
        {
            SPELL_FLAME_SHOCK = 8050,
        };

        void HandleOnHit(SpellEffIndex /*effIndex*/)
        {
            Unit * const target = GetHitUnit();
            if (target && target->HasAura(SPELL_FLAME_SHOCK, GetCaster()->GetGUID()))
                SetHitDamage(GetHitDamage() * 1.5f);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(script_impl::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

public:
    spell_sha_lava_burst_mastery_proc() : SpellScriptLoader("spell_sha_lava_burst_mastery_proc") { }

    SpellScript * GetSpellScript() const { return new script_impl(); }
};

// 52007 - Earthliving Weapon (Passive)
class spell_sha_earthliving_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_earthliving_weapon);

    void HandleProc(ProcEventInfo& event)
    {
        Unit* target = event.GetActionTarget();
        if (target && !target->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
            if (!roll_chance_i(20))
                PreventDefaultAction();
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_earthliving_weapon::HandleProc);
    }
};

// 77222 - Mastery: Elemental Overload
class spell_sha_elemental_oveload : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_oveload);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        uint32 spellId = 0;
        float chance = mastery->GetFloatAmount();
        switch (eventInfo.GetSpellInfo()->Id)
        {
            case SPELL_SHA_LIGHTNING_BOLT:  spellId = SPELL_SHA_LIGHTNING_BOLT_EO_PROC;  break;
            case SPELL_SHA_LAVA_BURST:      spellId = SPELL_SHA_LAVA_BURST_EO_PROC;      break;
            case SPELL_SHA_ELEMENTAL_BLAST: spellId = SPELL_SHA_ELEMENTAL_BLAST_EO_PROC; break;
            case SPELL_SHA_CHAIN_LIGHTNING:
            case SPELL_SHA_LAVA_BEAM:
                spellId = eventInfo.GetSpellInfo()->Id == SPELL_SHA_LAVA_BEAM ? SPELL_SHA_LAVA_BEAM_EO_PROC : SPELL_SHA_CHAIN_LIGHTNING_EO_PROC;
                chance /= 3;
                break;
            default:
                return;
        }

        if (!roll_chance_f(chance))
            return;

        eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), spellId, true, nullptr, mastery);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_elemental_oveload::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 88764 - Rolling Thunder
class spell_sha_rolling_thunder : public AuraScript
{
    PrepareAuraScript(spell_sha_rolling_thunder)

    void HandleProc(ProcEventInfo&)
    {
        Player* player = GetOwner()->ToPlayer();
        if (!player)
            return;

        if (Aura* lightningShield = player->GetAura(SPELL_SHA_LIGHTNING_SHIELD_AURA))
        {
            uint8 const maxCharges = 7;

            uint8 chargesCount = lightningShield->GetCharges();

            uint8 add = player->HasAura(SPELL_SHA_ITEM_T14_4P) ? 2 : 1;
            chargesCount += add;
            chargesCount = std::min(chargesCount, maxCharges);
            lightningShield->SetCharges(chargesCount);
            lightningShield->RefreshDuration();

            // refresh to handle Fulmination visual
            if (chargesCount == maxCharges && player->HasAura(SPELL_SHA_FULMINATION))
                player->CastSpell(player, SPELL_SHA_FULMINATION_INFO, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_rolling_thunder::HandleProc);
    }
};

// 55453 - Glyph of Telluric Currents
class spell_sha_glyph_of_telluric_currents : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_telluric_currents);

    void HandleProc(ProcEventInfo&)
    {
        Player* player = GetOwner()->ToPlayer();
        if (!player)
            return;

        float ptc = player->GetTalentSpecialization() == SPEC_SHAMAN_ENHANCEMENT ? 10.0f : 2.0f;
        int32 bp = CalculatePct(player->GetMaxPower(POWER_MANA), ptc);
        player->CastCustomSpell(player, 82987, &bp, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_glyph_of_telluric_currents::HandleProc);
    }
};

// 324 - Lightning Shield
class spell_sha_lighting_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_lighting_shield);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor() && eventInfo.GetActor()->IsTotem())
        {
            PreventDefaultAction();
            return;
        }

        GetAura()->SetUsingCharges(false);
        if (GetUnitOwner()->HasAura(SPELL_SHA_GLYPH_OF_LIGHTNING_SHIELD))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_SHA_GLYPH_OF_LIGHTNING_SHIELD_MOD, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_lighting_shield::HandleProc);
    }
};

// 974 - Earth Shield 
class spell_sha_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_earth_shield);

    void CalculateAmount(AuraEffect const* eff, float& amount, bool&)
    {
        if (Unit* shaman = GetCaster())
        {
            amount = shaman->SpellHealingBonusDone(GetUnitOwner(), GetSpellInfo(), eff->GetEffIndex(), amount, HEAL);
            if (shaman->HasSpell(SPELL_SHA_PURIFICATION))
                AddPct(amount, 25);
        }
    }

    bool CheckProc(ProcEventInfo& procEvent)
    {
        if (procEvent.GetTypeMask() & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
            return false;

        return procEvent.GetActor() != GetTarget();
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& procEvent)
    {
        CustomSpellValues ctx;
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, eff->GetAmount());
        ctx.SetHealingBonusCalculated();
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_EARTH_SHIELD_HEAL, ctx, GetTarget(), TRIGGERED_FULL_MASK, nullptr, eff, GetCasterGUID());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_earth_shield::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
        DoCheckProc += AuraCheckProcFn(spell_sha_earth_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 108283 - Echo of the Elements
class spell_sha_echo_of_the_elements : public AuraScript
{
    PrepareAuraScript(spell_sha_echo_of_the_elements);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = GetUnitOwner()->ToPlayer();
        Unit* target = eventInfo.GetActionTarget();

        SpellInfo const* procSpell = eventInfo.TriggeredBySpell()->GetSpellInfo();
        if (!procSpell)
            return;

        if (eventInfo.TriggeredBySpell()->IsTriggered())
            if (!(procSpell->SpellFamilyFlags[2] & 0x00008000))
                return;

        if (eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() != target) // AOE and Chain Lightning side damage.
            return;

        int32 chance = 6;

        // Devs told that proc chance is 6% for Elemental and Restoration specs and 30% for Enhancement.
        if (player->GetTalentSpecialization() == SPEC_SHAMAN_ENHANCEMENT && procSpell->Id != SPELL_SHA_ELEMENTAL_BLAST)
            chance = 30;

        if (!(eventInfo.GetDamageInfo()->GetDamageType() == SPELL_DIRECT_DAMAGE) && !(eventInfo.GetDamageInfo()->GetDamageType() == HEAL))
            return;

        if (!roll_chance_i(chance))
            return;

        Player* targetPlayer = target->GetAffectingPlayer();
        if (targetPlayer && player->IsHostileTo(target))
            player->GetSpellHistory()->StartProcCooldown(GetId(), Seconds(4));

        player->CastCustomSpell(procSpell->Id, SPELLVALUE_SPELL_GO_GLAGS, CAST_FLAG_NO_COOLDOWN, target, true, nullptr, GetEffect(EFFECT_0));
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_echo_of_the_elements::HandleProc);
    }
};

// 114052 - Water Ascendant
class spell_sha_water_ascendant : public AuraScript
{
    PrepareAuraScript(spell_sha_water_ascendant);

    uint32 amountHealed = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetHeal()) || !eventInfo.GetSpellInfo()->IsPositive())
            return;

        amountHealed += eventInfo.GetHeal();
    }

    void HandleTick(AuraEffect const* effect)
    {
        if (amountHealed)
        {
            GetUnitOwner()->CastCustomSpell(SPELL_SHA_RESTORATIVE_MISTS, SPELLVALUE_BASE_POINT0, amountHealed, GetUnitOwner(), true, nullptr, effect);
            amountHealed = 0;
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_water_ascendant::HandleProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_water_ascendant::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 117013 - Primal Elementalist
class spell_sha_primal_elementalist : public AuraScript
{
    PrepareAuraScript(spell_sha_primal_elementalist);

    bool Validate(SpellInfo const*) override
    {
        return sSpellMgr->GetSpellInfo(SPELL_SHA_FIRE_ELEMENTAL_TOTEM) && sSpellMgr->GetSpellInfo(SPELL_SHA_EARTH_ELEMENTAL_TOTEM);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        std::list<TempSummon*> summons;
        GetUnitOwner()->GetSummons(summons, sSpellMgr->GetSpellInfo(SPELL_SHA_FIRE_ELEMENTAL_TOTEM)->Effects[EFFECT_0].MiscValue);
        GetUnitOwner()->GetSummons(summons, sSpellMgr->GetSpellInfo(SPELL_SHA_EARTH_ELEMENTAL_TOTEM)->Effects[EFFECT_0].MiscValue);
        for (auto&& itr : summons)
            itr->UnSummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_primal_elementalist::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 114083 - Restorative Mists
class spell_sha_restorative_mists : public SpellScript
{
    PrepareSpellScript(spell_sha_restorative_mists);

    uint32 targetsCount = 0;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target)
        {
            return target->ToUnit()->IsSummon() && target->ToUnit()->ToTempSummon()->IsMinorGuardian();
        });
        targetsCount = targets.size();
    }

    void HandleHit()
    {
        if (!targetsCount)
            return;
        SetHitHeal(GetHitHeal() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_restorative_mists::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnHit += SpellHitFn(spell_sha_restorative_mists::HandleHit);
    }
};

// 108282 - Conductivity
class spell_sha_conductivity : public AuraScript
{
    PrepareAuraScript(spell_sha_conductivity);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_SHA_CHAIN_HEAL) // Chain heal
            if (eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() != eventInfo.GetActionTarget())
                return false;

        // Mask can't filter this.
        return eventInfo.GetSpellInfo()->Id != 370 && eventInfo.GetSpellInfo()->Id != 57984 && eventInfo.GetSpellInfo()->Id != 126371;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (AuraEffect* eff = GetCaster()->GetAuraEffect(SPELL_SHA_HEALING_RAIN, EFFECT_1))
        {
            Aura* aura = eff->GetBase();
            if (aura->GetMaxDuration() < 40 * IN_MILLISECONDS)
            {
                int32 add = eff->GetAmplitude() * 2;
                int32 maxDuration = static_cast<int32>(std::min((uint32)aura->GetMaxDuration() + add, 40 * IN_MILLISECONDS));
                aura->SetMaxDuration(maxDuration);
                aura->SetDuration(aura->GetDuration() + add);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_conductivity::CheckProc);
        OnProc += AuraProcFn(spell_sha_conductivity::HandleProc);
    }
};

// 1064 - Chain Heal
class spell_sha_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_chain_heal);

    bool firstHeal = true;
    bool riptide = false;

    void HandleHeal(SpellEffIndex)
    {
        if (firstHeal)
        {
            riptide = GetHitUnit()->HasAura(SPELL_SHA_RIPTIDE, GetCaster()->GetGUID());
            firstHeal = false;
        }
        if (riptide)
           SetHitHeal(GetHitHeal() * 1.25);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 64695 - Earthgrab
class spell_sha_earthgrab : public SpellScript
{
    PrepareSpellScript(spell_sha_earthgrab);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        targets.remove_if([=](WorldObject* obj)
        {
            if (Unit* target = obj->ToUnit())
            {
                if (target->HasAura(SPELL_SHA_EARTHGRAB_IMMUNITY))
                {
                    caster->CastSpell(target, SPELL_SHA_EARTHBIND_FOR_EARTHGRAB_TOTEM, true);
                    return true;
                }
            }
            return false;
        });
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_SHA_EARTHGRAB_IMMUNITY, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthgrab::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_sha_earthgrab::HandleHit);
    }
};

// 51564 - Tidal Waves
class spell_sha_tidal_waves : public AuraScript
{
    PrepareAuraScript(spell_sha_tidal_waves);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 bp0 = -(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
        int32 bp1 = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
        if (AuraEffect* eff = GetUnitOwner()->GetAuraEffect(SPELL_SHA_T14_RESTOR_P4, EFFECT_0))
        {
            bp0 -= eff->GetAmount();
            bp1 += eff->GetAmount();
        }
        GetUnitOwner()->CastCustomSpell(GetUnitOwner(), SPELL_SHA_TIDAL_WAVES_PROC, &bp0, &bp1, nullptr, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_tidal_waves::HandleProc);
    }
};

// 30884 - Nature's Guardian
class spell_sha_natures_guardian : public AuraScript
{
    PrepareAuraScript(spell_sha_natures_guardian);

    Player* m_caster = nullptr;

    bool Load() override
    {
        m_caster = GetUnitOwner()->ToPlayer();
        return m_caster != nullptr;
    }

    bool CheckProc(ProcEventInfo&)
    {
        // Cooldown need to be checked manually, because SPELL_AURA_PROC_TRIGGER_SPELL and not existing spell (Yes I'm lazy to add it to spell_dbc)
        return m_caster->HealthBelowPct(30) && !m_caster->HasSpellCooldown(SPELL_SHA_NATURES_GUARDIAN);
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_NATURES_GUARDIAN, SPELLVALUE_BASE_POINT0, eff->GetAmount(), GetUnitOwner(), true);
        if (eventInfo.GetActor() && eventInfo.GetActor()->CanHaveThreatList())
            eventInfo.GetActor()->GetThreatManager().modifyThreatPercent(GetUnitOwner(), -10.f); // No idea
        m_caster->AddSpellCooldown(SPELL_SHA_NATURES_GUARDIAN, Seconds(30));
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_natures_guardian::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_natures_guardian::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 114893 - Stone Bulwark
class spell_sha_stone_bulwark : public AuraScript
{
    PrepareAuraScript(spell_sha_stone_bulwark);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect* triggerEffect = caster->GetAuraEffect(SPELL_SHA_STONE_BULWARK_TOTEM_PASSIVE, EFFECT_0))
        {
            amount = triggerEffect->GetFloatAmount();
            if (triggerEffect->GetTickNumber() == 1)
                amount *= 4;
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_stone_bulwark::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 114889 - Stone Bulwark Totem Passive
class spell_sha_stone_bulwark_totem_passive : public AuraScript
{
    PrepareAuraScript(spell_sha_stone_bulwark_totem_passive);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Unit* owner = caster->GetOwner())
        {
            float ap = owner->GetTotalAttackPowerValue(BASE_ATTACK);
            float sp = owner->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            amount = std::max(0.625f * ap, sp) * 0.875f;
        }
    }

    void HandleTick(AuraEffect const* effect)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Unit* owner = caster->GetOwner())
        {
            if (AuraEffect* shield = owner->GetAuraEffect(SPELL_SHA_STONE_BULWARK_ABSORB, EFFECT_0, GetCasterGUID()))
                if (shield->GetAmount() >= effect->GetAmount() && shield->GetBase()->GetDuration() > 2000)
                    PreventDefaultAction();
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_stone_bulwark_totem_passive::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_stone_bulwark_totem_passive::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 108270 - Stone Bulwark Totem, 108280 - Healing Tide Totem
class spell_sha_hp_based_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_hp_based_totem);

    void CalculateHealth(SpellEffIndex)
    {
        SetEffectValue(GetCaster()->CountPctFromMaxHealth(10));
    }
 
    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_sha_hp_based_totem::CalculateHealth, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// 108287 - Totemic Projection
class spell_sha_totemic_projection : public SpellScript
{
    PrepareSpellScript(spell_sha_totemic_projection);

    void MoveTotems(SpellEffIndex)
    {
        Unit* shaman = GetCaster();
        for (int32 i = 0; i < 4; ++i)
        {
            Position pos = GetHitDest()->GetPosition();
            Creature* totem = shaman->GetMap()->GetCreature(shaman->m_SummonSlot[SUMMON_SLOT_TOTEM + i]);
            if (totem && totem->IsTotem())
            {
                uint32 spellId = totem->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
                if (!spellInfo)
                    return;

                float angle = spellInfo->Effects[EFFECT_0].TargetA.CalcDirectionAngle();
                float objSize = shaman->GetObjectSize();
                float dist = spellInfo->Effects[EFFECT_0].CalcRadius(shaman);
                if (dist < objSize)
                    dist = objSize;

                totem->MovePositionToFirstCollision(pos, dist, angle);
                totem->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), totem->GetOrientation());
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_sha_totemic_projection::MoveTotems, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 98020 - Spirit Link
class spell_sha_spirit_link : public SpellScript
{
    PrepareSpellScript(spell_sha_spirit_link);

    float m_expectedHealthPct = 0.f;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target) { return target->GetTypeId() != TYPEID_PLAYER; });
        for (auto&& it : targets)
            if (Unit* target = it->ToUnit())
                m_expectedHealthPct += target->GetHealthPct();
        m_expectedHealthPct /= targets.size();
    }

    void HandleHit()
    {
        Unit* target = GetHitUnit();
        int32 expected = (target->GetMaxHealth() * m_expectedHealthPct) / 100.f;
        int32 health = target->GetHealth();
        int32 diff = expected - health;
        int32 val = std::abs(diff);
        if (val < 2)
            return;
        SpellValueMod mod = diff < 0 ? SPELLVALUE_BASE_POINT0 : SPELLVALUE_BASE_POINT1;
        GetCaster()->CastCustomSpell(SPELL_SHA_SPIRIT_LINK, mod, val, target, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_spirit_link::SelectTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnHit += SpellHitFn(spell_sha_spirit_link::HandleHit);
    }
};

// 2062 - Earth Elemental Totem, 2894 - Fire Elemental Totem 
class spell_sha_elemental_totem_summon : public SpellScript
{
    PrepareSpellScript(spell_sha_elemental_totem_summon);

    void HandleCast()
    {
        uint32 normal = GetSpellInfo()->Id == SPELL_SHA_FIRE_ELEMENTAL_TOTEM ? 117663 : 117753;
        uint32 primal = GetSpellInfo()->Id == SPELL_SHA_FIRE_ELEMENTAL_TOTEM ? 118291 : 118323;
        uint32 spellId = GetCaster()->HasAura(117013) ? primal : normal;
        GetCaster()->CastSpell(GetCaster(), spellId, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_sha_elemental_totem_summon::HandleCast);
    }
};

// 60103 - Lava Lash
class spell_sha_lava_lash : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash);

    void HandleHit()
    {
        Player* shaman = GetCaster()->ToPlayer();
        if (!shaman)
            return;

        Item const* offhand = shaman->GetWeaponForAttack(OFF_ATTACK);
        if (!offhand)
            return;

        if (GetHitUnit()->HasAura(SPELL_SHA_FLAME_SHOCK) && !GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_LAVA_LASH))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_SHA_LAVA_LASH_SPREAD_FLAME_SHOK, true);

        int32 damage = GetHitDamage();

        int32 bonusPct = 0;

        // Damage is increased by 40% if your off-hand weapon is enchanted with Flametongue.
        if (offhand->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 5)
            bonusPct += 40;

        if (Aura* searingFlame = shaman->GetAura(SPELL_SHA_SEARING_FLAMES_EFFECT))
        {
            bonusPct += searingFlame->GetStackAmount() * 20;
            shaman->RemoveAura(searingFlame);
        }

        AddPct(damage, bonusPct);

        SetHitDamage(damage);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_lava_lash::HandleHit);
    }
};

// 105792 - Lava Lash
class spell_sha_lava_lash_spread_flame_shock : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash_spread_flame_shock);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        uint64 casterGuid = GetCaster()->GetGUID();
        Unit* target = GetExplTargetUnit();
        if (!target || !target->HasAura(SPELL_SHA_FLAME_SHOCK, casterGuid))
        {
            targets.clear();
            return;
        }
        targets.remove_if(Trinity::UnitAuraCheck{ true, SPELL_SHA_FLAME_SHOCK, casterGuid });
        if (targets.size() > 4)
            targets.resize(4);
    }

    void HandleHit()
    {
        GetCaster()->AddAura(SPELL_SHA_FLAME_SHOCK, GetHitUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_lava_lash_spread_flame_shock::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnHit += SpellHitFn(spell_sha_lava_lash_spread_flame_shock::HandleHit);
    }
};

// 52042 - Healing Stream
class spell_sha_healing_stream : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_stream);

    ObjectGuid bonusTarget = ObjectGuid::Empty;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Unit* shaman = GetCaster()->GetOwner();
        if (!shaman)
        {
            targets.clear();
            return;
        }
        targets.remove_if(Trinity::NotInRaidWithCheck{ shaman });
        targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
        size_t targetCount = shaman->HasAura(SPELL_SHA_RUSHING_STREAMS) ? 2 : 1;

        if (shaman->HasAura(SPELL_SHA_T15_RESTORATION_2P_BONUS) && targets.size() > targetCount)
        {
            auto itr = targets.begin();
            std::advance(itr, targetCount);
            bonusTarget = (*itr)->GetGUID();
        }

        if (targets.size() > targetCount)
            targets.resize(targetCount);
    }

    void HandleHit()
    {
        if (Unit* shaman = GetCaster()->GetOwner())
            if (shaman->HasAura(SPELL_SHA_GLYPH_OF_HEALING_STREAM_TOTEM))
                shaman->CastSpell(GetHitUnit(), SPELL_SHA_GLYPH_OF_HEALING_STREAM, true);
    }

    void HandleCast()
    {
        if (bonusTarget)
            if (Unit* target = ObjectAccessor::GetUnit(*GetCaster(), bonusTarget))
                GetCaster()->CastSpell(target, SPELL_SHA_HEALING_STREAM_TOTEM_BONUS, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_healing_stream::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_sha_healing_stream::HandleHit);
        AfterCast += SpellCastFn(spell_sha_healing_stream::HandleCast);
    }
};

// 33757 - Windfury Weapon
class spell_sha_windfury_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon);

    Player* m_caster = nullptr;

    bool Load() override
    {
        m_caster = GetOwner()->ToPlayer();
        return m_caster != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Item const* item = nullptr;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)
            item = m_caster->GetWeaponForAttack(BASE_ATTACK);
        else if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            item = m_caster->GetWeaponForAttack(OFF_ATTACK);
        return item && item->GetGUID() == GetAura()->GetCastItemGUID();
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        uint32 spellId = eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK ? 33750 : 25504;
        SpellInfo const* windfurySpellInfo = sSpellMgr->GetSpellInfo(8232);
        if (!windfurySpellInfo)
        {
            TC_LOG_ERROR("server", "spell_sha_windfurry_weapon::HandleProc Windfury (%u) spell not found", spellId);
            return;
        }

        int32 extraAttackPower = m_caster->CalculateSpellDamage(eventInfo.GetActionTarget(), windfurySpellInfo, EFFECT_1);
        int32 extraDamage = extraAttackPower / 14.0f * m_caster->GetAttackTime(BASE_ATTACK) / 1000.0f;
        for (auto i = 0; i < 3; ++i)
            m_caster->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, extraDamage, eventInfo.GetActionTarget(), true, nullptr, eff);

        if (AuraEffect const* setBonus = m_caster->GetAuraEffect(SPELL_SHA_T15_ENHANCEMENT_4P_BONUS, EFFECT_0))
            m_caster->GetSpellHistory()->ModifyCooldown(SPELL_SHA_FERAL_SPIRIT, -setBonus->GetAmount() * IN_MILLISECONDS);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_windfury_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_windfury_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 108285 - Call of the Elements
class spell_sha_call_of_the_elements : public SpellScript
{
    PrepareSpellScript(spell_sha_call_of_the_elements);

    void HandleOnHit()
    {
        if (Player* shaman = GetCaster()->ToPlayer())
        {
            shaman->GetSpellHistory()->RemoveCooldowns([](SpellHistory::CooldownEntry const& entry)
            {
                switch (entry.SpellId)
                {
                    case 2484:
                    case 5394:
                    case 8143:
                    case 8177:
                    case 51485:
                    case 108269:
                    case 108270:
                    case 108273:
                        return true;
                }
                return false;
            });
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_call_of_the_elements::HandleOnHit);
    }
};

// 1535 - Fire Nova
class spell_sha_fire_nova : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck{ false, SPELL_SHA_FLAME_SHOCK, GetCaster()->GetGUID() });
        if (targets.empty())
            FinishCast(SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW);
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_SHA_FIRE_NOVA_TRIGGERED, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_fire_nova::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_sha_fire_nova::HandleHit);
    }
};

// 370 - Purge
class spell_sha_glyph_of_purging : public SpellScript
{
    PrepareSpellScript(spell_sha_glyph_of_purging);

    void HandleHit()
    {
        if (GetDispelCount() && GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_PURGING))
            GetCaster()->CastSpell(GetCaster(), SPELL_SHA_MAELSTROM_WEAPON, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_glyph_of_purging::HandleHit);
    }
};

// 17364 - Stormstrike
class spell_sha_stormstrike : public SpellScript
{
    PrepareSpellScript(spell_sha_stormstrike);

    void CheckWeapon(SpellEffIndex index)
    {
        if (Player* shaman = GetCaster()->ToPlayer())
            if (!shaman->GetWeaponForAttack(OFF_ATTACK))
                PreventEffect(index);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_sha_stormstrike::CheckWeapon, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 118905 - Static Charge
class spell_sha_static_charge : public SpellScript
{
    PrepareSpellScript(spell_sha_static_charge);

    void HandleCast()
    {
        if (Totem* totem = GetCaster()->ToTotem())
            totem->DespawnOrUnsummon();
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_static_charge::HandleCast);
    }
};

// 51558 - Ancestral Awakening
// 138305 - Item - Shaman T15 Restoration 4P Bonus
class spell_sha_ancestral_awakening : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_awakening);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;
        if (!eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() || eventInfo.GetSpellInfo()->Id == SPELL_SHA_CHAIN_HEAL)
            return false;

        if (GetAura()->GetId() == SPELL_SHA_T15_RESTORATION_4P_BONUS)
            return !(eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT);
        return eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        int32 heal = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), eff->GetAmount());
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_ANCESTRAL_AWAKENING_SELECTOR, SPELLVALUE_BASE_POINT0, heal, eventInfo.GetActionTarget(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_ancestral_awakening::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_awakening::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 52759 - Ancestral Awakening
class spell_sha_ancestral_awakening_selector : public SpellScript
{
    PrepareSpellScript(spell_sha_ancestral_awakening_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
        if (!targets.empty())
            targets.resize(1);
    }

    void HandleHit(SpellEffIndex)
    {
        GetCaster()->CastCustomSpell(SPELL_SHA_ANCESTRAL_AWAKENING, SPELLVALUE_BASE_POINT0, GetEffectValue(), GetHitUnit(), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ancestral_awakening_selector::SelectTarget, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnEffectHitTarget += SpellEffectFn(spell_sha_ancestral_awakening_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 117663, 118291 - Fire Elemental Totem, 3599 - Searing Totem
class spell_sha_searing_flames_summon : public SpellScript
{
    PrepareSpellScript(spell_sha_searing_flames_summon);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_SHA_SEARING_FLAMES))
        {
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, GetSpellInfo()->Effects[EFFECT_0].MiscValue);
            if (!summons.empty())
                summons.front()->CastSpell(summons.front(), 203755, true);  // Proc aura
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_searing_flames_summon::HandleCast);
    }
};

// 203755 - Searing Flames proc aura
class spell_sha_searing_flames_proc_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_searing_flames_proc_aura);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* shaman = GetUnitOwner()->GetOwner())
            shaman->CastSpell(shaman, SPELL_SHA_SEARING_FLAMES_EFFECT, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_searing_flames_proc_aura::HandleProc);
    }
};

// 108281 - Ancestral Guidance
class spell_sha_ancestral_guidance : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_guidance);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        bool isHeal = eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
        int32 amount = isHeal ? eventInfo.GetHeal() : eventInfo.GetDamageWithoutResilience();
        int32 index = isHeal ? EFFECT_1 : EFFECT_0;
        amount = CalculatePct(amount, GetSpellInfo()->Effects[index].BasePoints);
        // Apply battle fatigue and dampening if procs from damage. If procs from heal it is already included.
        if (!isHeal)
        {
            if (GetUnitOwner()->HasAura(134735) || GetUnitOwner()->GetBattlegorund())
               AddPct(amount, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));

            if (AuraEffect* aurEff = GetUnitOwner()->GetAuraEffect(SPELL_DAMPENING, EFFECT_0))
                AddPct(amount, -aurEff->GetAmount());
        }
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_ANCESTRAL_GUIDANCE, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetActionTarget(), true, nullptr, eff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_guidance::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 114911 - Ancestral Guidance
class spell_sha_acestral_guidance_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_acestral_guidance_heal);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::NotInRaidWithCheck{ GetCaster() });
        targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
        if (targets.size() > 3)
            targets.resize(3);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_acestral_guidance_heal::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 16213 - Purification
class spell_sha_purification : public AuraScript
{
    PrepareAuraScript(spell_sha_purification);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 amount = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), 10);
        Unit* target = eventInfo.GetActionTarget();
        int32 max = (target->GetMaxHealth() / target->GetModifierValue(UNIT_MOD_HEALTH, TOTAL_PCT) - target->GetModifierValue(UNIT_MOD_HEALTH, TOTAL_VALUE)) * 0.1f;
        amount = std::min(max, amount);
        if (AuraEffect* existing = target->GetAuraEffect(SPELL_SHA_ANCESTRAL_VIGOR, EFFECT_0))
        {
            existing->ChangeAmount(std::min(existing->GetAmount() + amount, max));
            existing->GetBase()->RefreshDuration(false);
        }
        else
            GetUnitOwner()->CastCustomSpell(SPELL_SHA_ANCESTRAL_VIGOR, SPELLVALUE_BASE_POINT0, amount, target, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_purification::HandleProc);
    }
};

// 51527 - Static Shock
class spell_sha_static_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_static_shock);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!GetUnitOwner()->HasAura(SPELL_SHA_LIGHTNING_SHIELD_AURA))
            return false;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MELEE_AUTO_ATTACK)
            return GetUnitOwner()->HasAura(SPELL_SHA_UNLEASHED_FURY_WINDFURY);
        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_SHA_LIGHTNING_SHIELD_DAMAGE, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_static_shock::CheckProc);
        OnProc += AuraProcFn(spell_sha_static_shock::HandleProc);
    }
};

// 77223 - Mastery: Enhanced Elements
class spell_sha_enchanced_elements : public AuraScript
{
    PrepareAuraScript(spell_sha_enchanced_elements);

    void HandleApply(AuraEffect const* mastery, AuraEffectHandleModes)
    {
        std::list<TempSummon*> elementals;
        GetUnitOwner()->GetSummons(elementals, NPC_FIRE_ELEMENTAL);
        GetUnitOwner()->GetSummons(elementals, NPC_EARTH_ELEMENTAL);
        GetUnitOwner()->GetSummons(elementals, NPC_PRIMAL_FIRE_ELEMENTAL);
        GetUnitOwner()->GetSummons(elementals, NPC_PRIMAL_EARTH_ELEMENTAL);

        for (auto&& elem : elementals)
            if (Aura* aura = elem->GetAura(SPELL_SHA_ELEMENTAL_TOTEM))
                aura->RecalculateAmountOfEffects();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_enchanced_elements::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// 34967 - Elemental Totem
class spell_sha_elemental_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_totem);

    void CalculateDamageBonus(AuraEffect const*, float& amount, bool&)
    {
        if (TempSummon* summon = GetUnitOwner()->ToTempSummon())
            if (Unit* shaman = summon->GetSummoner())
                if (AuraEffect const* eff = shaman->GetAuraEffect(SPELL_SHA_MASTERT_ENHANCED_ELEMENTS, EFFECT_0))
                    amount = eff->GetFloatAmount();
    }

    void CalculateCritBonus(AuraEffect const*, float& amount, bool&)
    {
        if (TempSummon* summon = GetUnitOwner()->ToTempSummon())
            if (Unit* shaman = summon->GetSummoner())
                if (AuraEffect const* eff = shaman->GetAuraEffect(SPELL_SHA_ELEMENTAL_FURY, EFFECT_0))
                    amount = eff->GetFloatAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_elemental_totem::CalculateCritBonus, EFFECT_1, SPELL_AURA_MOD_CRIT_DAMAGE_BONUS);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_elemental_totem::CalculateDamageBonus, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

// 10400 - Flametongue Weapon (Passive)
class spell_sha_flametongue_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_flametongue_weapon);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        Player* shaman = GetUnitOwner()->ToPlayer();
        Unit* target = eventInfo.GetProcTarget();

        Item* castItem = GetUnitOwner()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!castItem || !castItem->IsEquipped())
            return;

        WeaponAttackType attType = WeaponAttackType(Player::GetAttackBySlot(castItem->GetSlot()));
        if ((attType != BASE_ATTACK && attType != OFF_ATTACK)
            || (attType == BASE_ATTACK && eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            || (attType == OFF_ATTACK && eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK))
            return;

        float fireOnHit = float(CalculatePct(GetSpellInfo()->Effects[EFFECT_0].CalcValue(), 1.0f));

        float spBonus = (float)(shaman->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE) + target->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_FIRE));

        // 1.3speed = 5%, 2.6speed = 10%, 4.0 speed = 15%, so, 1.0speed = 3.84%
        ApplyPct(spBonus, 3.84f);

        float baseWeaponSpeed = shaman->GetAttackTime(attType) / 1000.0f;
        // Value1: add the tooltip damage by swingspeed + Value2: add spelldmg by swingspeed
        int32 dmg = int32((fireOnHit * baseWeaponSpeed) + (spBonus * baseWeaponSpeed));

        shaman->CastCustomSpell(SPELL_SHA_FLAMETONGUE_ATTACK, SPELLVALUE_BASE_POINT0, dmg, target, true, castItem, eff);
        if (shaman->HasAura(SPELL_SHA_A15_4P))
            shaman->CastSpell(target, SPELL_SHA_FROSTFLAME_ATTACK, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_flametongue_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 16196 - Resurgence
class spell_sha_resurgence : public AuraScript
{
    PrepareAuraScript(spell_sha_resurgence);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HasAura(SPELL_SHA_WATER_SHIELD);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        float mult;
        switch (eventInfo.GetSpellInfo()->Id)
        {
            case 331:   // Healing Wave
            case 77472: // Greater Healing Wave
                mult = 1.0f;
                break;
            case 8004:  // Healing Surge
            case 61295: // Riptide
            case 73685: // Unleash Life
                mult = 0.6f;
                break;
            case 1064:  // Chain Heal
                mult = 0.333f;
                break;
            default:
                return;
        }
        CustomSpellValues ctx;
        ctx.SetMultiplier(mult);
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_RESURGENCE, ctx, GetUnitOwner(), TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_resurgence::CheckProc);
        OnProc += AuraProcFn(spell_sha_resurgence::HandleProc);
    }
};

// 55447 - Glyph of Flame Shock
class spell_sha_glyph_of_flame_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_flame_shock);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction(); // "Not existing spell" spam 

        int32 damage = eventInfo.GetDamageWithoutResilience();
        int32 heal = CalculatePct(damage, eff->GetAmount());
        GetUnitOwner()->CastCustomSpell(SPELL_SHA_FLAMES_OF_LIFE, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_flame_shock::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 79206 - Spiritwalker's Grace
class spell_sha_spiritwalkers_grace : public SpellScript
{
    PrepareSpellScript(spell_sha_spiritwalkers_grace);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_SHA_SPIRITWALKERS_AEGIS))
            GetCaster()->CastSpell(GetCaster(), SPELL_SHA_SPIRITWALKERS_AEGIS_EFFECT, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_sha_spiritwalkers_grace::HandleCast);
    }
};

// 8178 - Grounding Totem Effect
// 89523 - Grounding Totem
class spell_sha_grounding_totem_effect : public AuraScript
{
    PrepareAuraScript(spell_sha_grounding_totem_effect);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;

        if (GetId() == SPELL_SHA_GROUNDING_TOTEM_EFFECT && eventInfo.TriggeredBySpell()->IsSpellRedirecterd())
            return true;
        if (GetId() == SPELL_SHA_GROUNDING_TOTEM_EFFECT_REFLECT && (eventInfo.GetHitMask() & PROC_EX_REFLECT))
            return true;
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Totem* totem = GetUnitOwner()->ToTotem())
            totem->UnSummon();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_grounding_totem_effect::CheckProc);
        OnProc += AuraProcFn(spell_sha_grounding_totem_effect::HandleProc);
    }
};

// 29264 - Spirit Wolf, 58859 - Spirit Wolf (Symbiosis)
struct npc_sha_spirit_wolf : public ScriptedAI
{
    npc_sha_spirit_wolf(Creature* c) : ScriptedAI(c) { }

    TaskScheduler scheduler;
    AssistBehavior assist{ me };

    enum
    {
        SPELL_SPIRIT_BYTE       = 58859,
        SPELL_SPIRIT_WOLF_LEAP  = 58867,
    };

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);

        scheduler.Schedule(Seconds(4), [=](TaskContext ctx)
        {
            DoCastVictim(SPELL_SPIRIT_BYTE);
            ctx.Repeat(Seconds(7));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (!UpdateVictim())
            return;

        scheduler.Update(diff);

        if (me->GetVictim() && me->GetDistance(me->GetVictim()) > MELEE_RANGE)
            DoCastVictim(SPELL_SPIRIT_WOLF_LEAP);

        DoMeleeAttackIfReady();
    }

    void EnterEvadeMode() override
    {
        scheduler.CancelAll();

        ScriptedAI::EnterEvadeMode();
    }
};

// 58877 - Spirit Hunt
class spell_sha_spirit_hunt : public AuraScript
{
    PrepareAuraScript(spell_sha_spirit_hunt);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (Unit* shaman = GetUnitOwner()->GetOwner())
        {
            int32 damage = eventInfo.GetDamageWithoutResilience();
            int32 heal = CalculatePct(damage, eff->GetAmount());
            GetUnitOwner()->CastCustomSpell(58879, SPELLVALUE_BASE_POINT0, heal, shaman, true);
            GetUnitOwner()->CastCustomSpell(58879, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spirit_hunt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 120676 - Stormlash Totem
class spell_sha_stormlash_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_stormlash_totem);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // If it�s periodic damage, it doesn�t Stormlash, unless it�s Mind Flay, Malefic Grasp, or Drain Soul.
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_PERIODIC)
            if (!eventInfo.GetSpellInfo()->IsChanneled())
                return false;

        Unit* actor = eventInfo.GetActor();
        if (!actor)
            return false;

        if (actor->IsPet() && actor->ToPet()->IsTemporary())
            return false;

        return actor->GetTypeId() == TYPEID_PLAYER || actor->IsPet() || actor->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* actor = eventInfo.GetActor();
        Player* player = actor->GetAffectingPlayer();
        if (!player)
            return;

        WeaponAttackType attack = actor->GetClass() == CLASS_HUNTER ? RANGED_ATTACK : BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            attack = OFF_ATTACK;

        int32 spdDamage = actor->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * 0.6f;
        int32 apDamage = actor->GetTotalAttackPowerValue(attack) * 0.4f;

        if (actor->GetEntry() == NPC_FIRE_ELEMENTAL || actor->GetEntry() == NPC_PRIMAL_FIRE_ELEMENTAL)
            apDamage = 0;

        float damage = std::max(spdDamage, apDamage);

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id != actor->GetAutoAttackSpell(attack))
        {
            uint32 castTime = eventInfo.GetSpellInfo()->CalcCastTime(actor->GetLevel());
            if (castTime > 1500)
                damage *= castTime / 1500.0f;
            switch (eventInfo.GetSpellInfo()->Id)
            {
                case SPELL_SHA_LIGHTNING_BOLT:
                case SPELL_SHA_LAVA_BURST:
                case SPELL_WARLOCK_DRAIN_SOUL:
                    damage *= 2.0f;
                    break;
                case SPELL_ROUGE_SINISTER_STRIKE:
                    damage *= 0.5;
                    break;
            }
        }
        else
        {
            damage *= 0.4f;
            uint32 attackTimer = BASE_ATTACK_TIME;
            if (eventInfo.GetActor() == player)
            {
                if (Item* item = player->GetWeaponForAttack(attack))
                    attackTimer = item->GetTemplate()->Delay;
            }
            else
            {
                if (TempSummon* summon = actor->ToTempSummon())
                    attackTimer = summon->GetBaseAttackTimer();
            }

            damage *= attackTimer / 1000.0f / 2.6f;

            if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
                damage *= 0.5f;
        }

        if (actor != player) // Pets
            damage *= 0.2f;

        float delta = CalculatePct(damage, 15);
        damage += frand(-delta, delta);
        actor->CastCustomSpell(SPELL_SHA_STORMLASH, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetProcTarget(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_stormlash_totem::CheckProc);
        OnProc += AuraProcFn(spell_sha_stormlash_totem::HandleProc);
    }
};

// 77130 - Purify Spirit
class spell_sha_glyph_of_cleansing_waters : public SpellScript
{
    PrepareSpellScript(spell_sha_glyph_of_cleansing_waters);

    void HandleHit()
    {
        if (GetDispelCount() && GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_CLEANSING_WATERS))
            GetCaster()->CastCustomSpell(SPELL_SHA_CLEANSING_WATERS, SPELLVALUE_BASE_POINT0, GetCaster()->CountPctFromMaxHealth(5), GetHitUnit(), true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_glyph_of_cleansing_waters::HandleHit);
    }
};

// 403 - Lightning Bolt
// 421 - Chain Lightning
// 1064 - Chain Heal
// 8004 - Healing Surge
// 51514 - Hex
// 73920 - Healing Rain
// 76780 - Bind Elemental
class spell_sha_maelstrom_weapon_haste_fix : public SpellScript
{
    PrepareSpellScript(spell_sha_maelstrom_weapon_haste_fix);

    void HandleCast(SpellEffIndex)
    {
        if (GetCaster()->HasAura(SPELL_SHA_ANCESTRAL_SWIFTNESS))
        {
            if (Aura* maelstrom = GetCaster()->GetAura(SPELL_SHA_MAELSTROM_WEAPON))
            {
                auto itr = GetSpell()->m_appliedMods.find(maelstrom);
                if (itr != GetSpell()->m_appliedMods.end())
                    GetSpell()->m_appliedMods.erase(itr);
            }
        }
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_SHA_HEALING_RAIN)
            OnEffectLaunch += SpellEffectFn(spell_sha_maelstrom_weapon_haste_fix::HandleCast, EFFECT_0, SPELL_EFFECT_ANY);
        if (m_scriptSpellId == SPELL_SHA_HEALING_SURGE || m_scriptSpellId == SPELL_SHA_CHAIN_HEAL)
            OnEffectHit += SpellEffectFn(spell_sha_maelstrom_weapon_haste_fix::HandleCast, EFFECT_0, SPELL_EFFECT_ANY);
        else
            OnEffectLaunchTarget += SpellEffectFn(spell_sha_maelstrom_weapon_haste_fix::HandleCast, EFFECT_0, SPELL_EFFECT_ANY);
    }
};

// 17364 - Stormstrike
// 115356 - Stormblast
class spell_sha_t15_enhancement_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_sha_t15_enhancement_2p_bonus);

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_SHA_T15_ENHANCEMENT_2P_BONUS);
    }

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SHA_MAELSTROM_WEAPON, true);
        GetCaster()->CastSpell(GetCaster(), SPELL_SHA_MAELSTROM_WEAPON, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_sha_t15_enhancement_2p_bonus::HandleCast);
    }
};

// 138146 - Lightning Strike
class spell_sha_lightning_strike : public SpellScript
{
    PrepareSpellScript(spell_sha_lightning_strike);

    size_t targetsCount = 0;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* target)
        {
            return target->ToUnit() && target->ToUnit()->HasBreakableByDamageCrowdControlAura();
        });
        targetsCount = targets.size();
    }

    void HandleHit()
    {
        if (targetsCount)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_lightning_strike::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnHit += SpellHitFn(spell_sha_lightning_strike::HandleHit);
    }
};

// 51505 - Lava Burst
class spell_sha_t15_elemental_4p_bonus : public SpellScript
{
    PrepareSpellScript(spell_sha_t15_elemental_4p_bonus);

    void HandleCast()
    {
        if (Player* shaman = GetCaster()->ToPlayer())
            if (AuraEffect const* setBonus = shaman->GetAuraEffect(SPELL_SHA_T15_ELEMENTAL_4P_BONUS, EFFECT_0))
                shaman->GetSpellHistory()->ModifyCooldown(SPELL_SHA_ASCENDANCE, -setBonus->GetAmount() * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_sha_t15_elemental_4p_bonus::HandleCast);
    }
};

// 144964 - Flow of the Elements
class spell_sha_flow_elements : public AuraScript
{
    PrepareAuraScript(spell_sha_flow_elements);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* shaman = GetUnitOwner()->ToPlayer();
        if (!shaman || !eventInfo.GetActionTarget())
            return false;

        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* shaman = GetUnitOwner()->ToPlayer();
        uint32 enchants[]{ 2, 5, 283 }; // Frostbrand Weapon, Flametongue Weapon, Windfury Weapon
        UnleashElemts(shaman, enchants[urand(0, 2)], eventInfo.GetActionTarget());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flow_elements::CheckProc);
        OnProc += AuraProcFn(spell_sha_flow_elements::HandleProc);
    }
};

// 144966 - Item - Shaman T16 Enhancement 4P Bonus
class spell_sha_t16_enchancement_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t16_enchancement_4p_bonus);

    void HandleProc(ProcEventInfo&)
    {
        if (Player* shaman = GetUnitOwner()->ToPlayer())
        {
            shaman->RemoveSpellCooldown(SPELL_SHA_LAVA_LASH, true);
            shaman->CastCustomSpell(SPELL_SHA_SEARING_FLAMES_EFFECT, SPELLVALUE_AURA_STACK, 5, shaman, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_t16_enchancement_4p_bonus::HandleProc);
    }
};

// 379 - Earth Shield
class spell_sha_t16_restoration_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_sha_t16_restoration_2p_bonus);

    void HandleHit()
    {
        if (Unit* shaman = GetOriginalCaster())
        {
            if (AuraEffect const* bonus = shaman->GetAuraEffect(SPELL_SHA_T16_RESTORATION_2P_BONUS, EFFECT_0))
            {
                int32 heal = GetHitHeal();
                AddPct(heal, bonus->GetAmount());
                shaman->CastCustomSpell(SPELL_SHA_NATURES_BARRIER, SPELLVALUE_BASE_POINT0, heal, GetHitUnit(), true);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_t16_restoration_2p_bonus::HandleHit);
    }
};

// 72473
struct npc_sha_spiritwalker_champion : public PassiveAI
{
    npc_sha_spiritwalker_champion(Creature* c) : PassiveAI(c) { }

    void IsSummonedBy(Unit* summoner) override
    {
        summoner->CastSpell(me, SPELL_CLONE_ME, true);
        summoner->CastSpell(me, SPELL_COPY_WEAPONS, true);
        me->CastSpell(me, SPELL_SHA_T16_RESTORATION_4P_COSMETIC, true);

        me->SetMaxPower(POWER_MANA, summoner->GetMaxPower(POWER_MANA));
        me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
    }

    void EnterEvadeMode() override 
    {
        Unit* owner = me->GetCharmerOrOwner();
        if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
    }
};

// 145394 - Item - Shaman T16 Restoration 4P Heal Trigger
class spell_sha_t16_restoration_4p_heal_trigger : public AuraScript
{
    PrepareAuraScript(spell_sha_t16_restoration_4p_heal_trigger);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo())
            return false;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_PERIODIC)  // Dunno why it is present
            return false;

        if (!(eventInfo.GetSpellInfo()->GetExplicitTargetMask() & TARGET_FLAG_UNIT_MASK))   // Targeted
            return false;
        return eventInfo.GetSpellInfo()->CalcCastTime(GetUnitOwner()->GetLevel()) > 0;      // With cast time
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        uint32 spell = eventInfo.GetSpellInfo()->Id;
        for (auto&& itr : GetUnitOwner()->GetSummons())
            if (itr->GetEntry() == 72473) // Spiritwalker Champion
                itr->CastCustomSpell(SPELL_SHA_T16_RESTORATION_4P_SELECTOR, SPELLVALUE_BASE_POINT0, spell, itr, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_t16_restoration_4p_heal_trigger::CheckProc);
        OnProc += AuraProcFn(spell_sha_t16_restoration_4p_heal_trigger::HandleProc);
    }
};

// 145399 - Item - Shaman T16 Restoration 4P Heal Target Dummy
class spell_sha_t16_restoration_4p_selector : public spell_smart_heal
{
    PrepareSpellScript(spell_sha_t16_restoration_4p_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, 1);
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), GetSpellValue()->EffectBasePoints[EFFECT_0], true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_t16_restoration_4p_selector::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnHit += SpellHitFn(spell_sha_t16_restoration_4p_selector::HandleHit);
    }
};

// 144998 - Item - Shaman T16 Elemental 2P Bonus
class spell_sha_t16_elemental_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t16_elemental_2p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetActionTarget())
            return false;
        if (eventInfo.GetSpellInfo()->Id != SPELL_SHA_FULMINATION_TRIGGERED)
            return false;
        Aura* lightningShield = GetUnitOwner()->GetAura(SPELL_SHA_LIGHTNING_SHIELD_AURA);
        return lightningShield && lightningShield->GetCharges() > 1;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* shaman = GetUnitOwner();
        Aura* lightningShield = shaman->GetAura(SPELL_SHA_LIGHTNING_SHIELD_AURA);
        if (!lightningShield)
            return;

        int32 charges = lightningShield->GetCharges() - 1;
        shaman->CastSpell(eventInfo.GetActionTarget(), SPELL_SHA_ELEMENTAL_DISCHARGE, true, nullptr, GetEffect(EFFECT_0));
        if (Aura* aura = eventInfo.GetActionTarget()->GetAura(SPELL_SHA_ELEMENTAL_DISCHARGE))
        {
            aura->SetMaxDuration(2 * charges * IN_MILLISECONDS);
            aura->SetDuration(2 * charges * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_t16_elemental_2p_bonus::CheckProc);
        OnProc += AuraProcFn(spell_sha_t16_elemental_2p_bonus::HandleProc);
    }
};

// 72260
struct npc_sha_lightning_elemental : public ScriptedAI
{
    npc_sha_lightning_elemental(Creature* c) : ScriptedAI(c) { }

    enum { SPELL_LIGHTNING_BLAST = 145002 };

    CasterMovement casterMovement = CasterMovement(me).Spell(SPELL_LIGHTNING_BLAST);

    void AttackStart(Unit* target) override
    {
        if (me->Attack(target, false))
            casterMovement.Chase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        casterMovement.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoCastVictim(SPELL_LIGHTNING_BLAST);
    }
};

// 15352
struct npc_sha_earth_elemental : public ScriptedAI
{
    npc_sha_earth_elemental(Creature* creature) : ScriptedAI(creature) { }

    AssistBehavior assist{ me };
    TaskScheduler scheduler;

    enum { SPELL_SHAMAN_ANGEREDEARTH = 36213, };

    void Reset() override
    {
        scheduler.CancelAll();
        scheduler.Schedule(Milliseconds(1), [this](TaskContext ctx)
        {
            ctx.Repeat(Milliseconds(5000));

            if (Unit* target = me->GetVictim())
                if (target->GetThreatManager().getCurrentVictim())
                    if (Unit* tank = target->GetThreatManager().getCurrentVictim()->getTarget())
                        if (tank->GetTypeId() == TYPEID_PLAYER && tank->ToPlayer()->GetRoleForGroup() == ROLES_TANK)
                            return;

            DoCastVictim(SPELL_SHAMAN_ANGEREDEARTH);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (!UpdateVictim())
            return;

        scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }
};

// 15438
struct npc_sha_fire_elemental : public ScriptedAI
{
    npc_sha_fire_elemental(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        SPELL_SHAMAN_FIREBLAST  = 57984,
        SPELL_SHAMAN_FIRENOVA   = 12470,
        SPELL_SHAMAN_FIRESHIELD = 13376
    };

    AssistBehavior assist{ me };
    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler.CancelAll();
        scheduler
            .Schedule(Milliseconds(1), [this](TaskContext ctx)
            {
                DoCastVictim(SPELL_SHAMAN_FIRESHIELD);
                ctx.Repeat(Milliseconds(2000));
            })
            .Schedule(Milliseconds(5000), Milliseconds(20000), [this](TaskContext ctx)
            {
                DoCastVictim(SPELL_SHAMAN_FIRENOVA);
                ctx.Repeat(Milliseconds(5000), Milliseconds(20000));
            })
            .Schedule(Milliseconds(5000), Milliseconds(20000), [this](TaskContext ctx)
            {
                DoCastVictim(SPELL_SHAMAN_FIREBLAST);
                ctx.Repeat(Milliseconds(5000), Milliseconds(20000));
            });
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        scheduler.Update(diff);

        DoMeleeAttackIfReady();
    }
};

// 55440 - Glyph of Healing Wave
class spell_sha_glyph_of_healing_wave : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_healing_wave);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget() != GetUnitOwner())
            if (int32 heal = CalculatePct(eventInfo.GetHeal(), GetEffect(EFFECT_0)->GetAmount()))
                GetUnitOwner()->CastCustomSpell(SPELL_SHA_GLYPH_OF_HEALING_WAVE, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_glyph_of_healing_wave::HandleProc);
    }
};

// 53817 - Maelstrom Weapon
class spell_sha_maelstrom_weapon_visual : public AuraScript
{
    PrepareAuraScript(spell_sha_maelstrom_weapon_visual);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetAura()->GetStackAmount() == GetAura()->GetMaxStackAmount())
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_SHA_MAELSTROM_WEAPON_FULL_STACKS, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_SHA_MAELSTROM_WEAPON_FULL_STACKS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_maelstrom_weapon_visual::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAPPLY);
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_maelstrom_weapon_visual::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 51490 - Thunderstorm
class spell_sha_glyph_of_thunder : public SpellScript
{
    PrepareSpellScript(spell_sha_glyph_of_thunder);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_THUNDERSTORM))
            PreventDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_glyph_of_thunder::HandleHit, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 16191 - Mana Tide
class spell_sha_mana_tide : public spirit_based_aura_effect
{
    PrepareAuraScript(spell_sha_mana_tide);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        uint32 spirit = 0;
        if (Unit* totem = GetCaster())
            if (Unit* shaman = totem->GetOwner())
                spirit = GetSpiritWithoutShortDurationBonuses(shaman);
        amount = CalculatePct(spirit, amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_mana_tide::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

// 16246 - Clearcasting
class spell_sha_clearcasting : public AuraScript
{
    PrepareAuraScript(spell_sha_clearcasting);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;
        for (auto&& itr : eventInfo.GetSpellInfo()->SpellPowers)
            if (itr->PowerType == POWER_MANA)
                return true;
        return false;
    }

    void CalculateSpellMod(AuraEffect const* effect, SpellModifier*& mod)
    {
        if (mod)
            mod->mask[0] |= 0x40; // Healing Wave
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_clearcasting::CheckProc);
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_sha_clearcasting::CalculateSpellMod, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// 58057 - Glyph of Totemic Encirclement
class spell_sha_glyph_of_totemic_encirclement : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_totemic_encirclement);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->Schedule(Milliseconds(100), [](Unit* shaman)
        {
            const uint32 spells[]{ 120217, 120218, 120214, 120219 };
            for (uint32 i = SUMMON_SLOT_TOTEM; i < SUMMON_SLOT_MAX_TOTEM; ++i)
                if (!shaman->m_SummonSlot[i])
                    shaman->CastSpell(shaman, spells[i - SUMMON_SLOT_TOTEM], true);
        });
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_glyph_of_totemic_encirclement::HandleProc);
    }
};

// 147781 - Glyph of Eternal Earth
class spell_sha_glyph_of_eternal_earth : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_eternal_earth);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (roll_chance_i(15))
        {
            if (auto list = GetUnitOwner()->GetBoundAurasBySpellId(SPELL_SHA_EARTH_SHIELD)) // It's singletarget, bound by default
                if (list->front()->GetCharges() < 9)
                    list->front()->ModCharges(1);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_sha_glyph_of_eternal_earth::HandleProc);
    }
};

// 2645 - Ghost Wolf
class spell_sha_glyph_of_spirit_wolf : public SpellScript
{
    PrepareSpellScript(spell_sha_glyph_of_spirit_wolf);

    SpellCastResult CheckCast()
    {
        if (!GetCaster()->IsAlive() && !GetCaster()->HasAura(SPELL_SHA_GLYPH_OF_SPIRIT_WOLF))
            return SPELL_FAILED_CASTER_DEAD;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_glyph_of_spirit_wolf::CheckCast);
    }
};

struct
{
    uint32 Entry;
    uint32 Spell;
} ElementalFamiliar[] = 
{
    { 73556, 148105 },  // Earth
    { 73559, 148111 },  // Water
    { 73560, 148112 },  // Fire
};

// 148118 - Summon Elemental Familiar
class spell_sha_summon_elemental_familiar : public SpellScript
{
    PrepareSpellScript(spell_sha_summon_elemental_familiar);

    void HandleCast()
    {
        std::vector<uint32> familiars;
        std::list<TempSummon*> summons;
        // Simulate "random"
        for (auto&& itr : ElementalFamiliar)
        {
            familiars.push_back(itr.Entry);
            summons.clear();
            GetCaster()->GetSummons(summons, itr.Entry);
            if (!summons.empty())
                familiars.pop_back();
            for (auto&& itr : summons)
                itr->DespawnOrUnsummon();
        }

        // FIXME: Spells do not exist
        if (!familiars.empty())
        {
            Position pos = GetCaster()->GetPosition();
            GetCaster()->MovePositionToFirstCollision(pos, 5.0f, M_PI / 2);

            if (TempSummon* familiar = GetCaster()->GetMap()->SummonCreature(familiars[urand(0, familiars.size() - 1)], pos, sSummonPropertiesStore.LookupEntry(64)))
            {
                familiar->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);
                familiar->SetFaction(GetCaster()->GetFaction());
                familiar->SetOwnerGUID(GetCaster()->GetGUID());
                familiar->SetUInt64Value(UNIT_FIELD_DEMON_CREATOR, GetCaster()->GetGUID());
                familiar->AI()->EnterEvadeMode();
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_sha_summon_elemental_familiar::HandleCast);
    }
};

// 148101 - Elemental Familiar
class spell_sha_elemental_familiar_selector : public SpellScript
{
    PrepareSpellScript(spell_sha_elemental_familiar_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Trinity::UnitAuraCheck auraCheck{ false, SPELL_SHA_ELEMENTAL_FAMILIAR_MARKER };
        targets.remove_if([=](WorldObject* target)
        {
            if (target->GetEntry() == GetCaster()->GetEntry())
                return false;
            return auraCheck(target);
        });
        if (targets.size() > 1)
        {
            targets.push_front(Trinity::Containers::SelectRandomContainerElement(targets));
            targets.resize(1);
        }
    }

    void HandleHit()
    {
        for (auto&& itr : ElementalFamiliar)
        {
            if (itr.Entry == GetCaster()->GetEntry())
            {
                GetCaster()->SetFacingToObject(GetHitUnit());
                GetCaster()->CastSpell(GetHitUnit(), itr.Spell);
                break;
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_elemental_familiar_selector::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnHit += SpellHitFn(spell_sha_elemental_familiar_selector::HandleHit);
    }
};

/*######
## npc_capacitor_totem
######*/

/*######
## npc_capacitor_totem
######*/

class npc_capacitor_totem : public CreatureScript
{
public:
    npc_capacitor_totem() : CreatureScript("npc_capacitor_totem") { }

    struct npc_capacitor_totemAI : public ScriptedAI
    {
        bool despawn = false;

        npc_capacitor_totemAI(Creature* creature) : ScriptedAI(creature) { }

        void UpdateAI(uint32 const /*diff*/) override
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (!despawn)
            {
                me->CastSpell(me, 118905, false);
                despawn = true;
            }
            else
                me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_capacitor_totemAI(creature);
    }
};

void AddSC_shaman_spell_scripts()
{
    new npc_capacitor_totem();
    new spell_sha_hex();
    new spell_sha_prowl();
    new spell_script<spell_sha_glyph_of_shamanistic_rage>("spell_sha_glyph_of_shamanistic_rage");
    new spell_sha_glyph_of_lakestrider();
    new spell_sha_mail_specialization();
    new spell_sha_frozen_power();
    new spell_script<spell_sha_unleash_elements>("spell_sha_unleash_elements");
    new spell_script<spell_sha_unleashed_fury_earthliving>("spell_sha_unleashed_fury_earthliving");
    new aura_script<spell_sha_unleashed_fury_earthliving_riptide>("spell_sha_unleashed_fury_earthliving_riptide");
    new aura_script<spell_sha_unleashed_fury_rockbiter>("spell_sha_unleashed_fury_rockbiter");
    new aura_script<spell_sha_unleash_frost>("spell_sha_unleash_frost");
    new spell_script<spell_sha_fulmination>("spell_sha_fulmination");
    new aura_script<spell_sha_lava_surge>("spell_sha_lava_surge");
    new spell_script<spell_sha_lava_surge_cooldown_fix>("spell_sha_lava_surge_cooldown_fix");
    new spell_sha_elemental_blast();
    new spell_sha_earthquake_tick();
    new spell_sha_earthquake();
    new spell_script<spell_sha_healing_rain>("spell_sha_healing_rain");
    new aura_script<spell_sha_healing_rain_aura>("spell_sha_healing_rain_aura");
    new spell_sha_ascendance();
    new spell_sha_lava_burst();
    new spell_sha_lava_burst_mastery_proc();
    new aura_script<spell_sha_earthliving_weapon>("spell_sha_earthliving_weapon");
    new aura_script<spell_sha_elemental_oveload>("spell_sha_elemental_oveload");
    new aura_script<spell_sha_rolling_thunder>("spell_sha_rolling_thunder");
    new aura_script<spell_sha_glyph_of_telluric_currents>("spell_sha_glyph_of_telluric_currents");
    new aura_script<spell_sha_lighting_shield>("spell_sha_lighting_shield");
    new aura_script<spell_sha_earth_shield>("spell_sha_earth_shield");
    new aura_script<spell_sha_echo_of_the_elements>("spell_sha_echo_of_the_elements");
    new aura_script<spell_sha_water_ascendant>("spell_sha_water_ascendant");
    new aura_script<spell_sha_primal_elementalist>("spell_sha_primal_elementalist");
    new spell_script<spell_sha_restorative_mists>("spell_sha_restorative_mists");
    new aura_script<spell_sha_conductivity>("spell_sha_conductivity");
    new spell_script<spell_sha_chain_heal>("spell_sha_chain_heal");
    new spell_script<spell_sha_earthgrab>("spell_sha_earthgrab");
    new aura_script<spell_sha_tidal_waves>("spell_sha_tidal_waves");
    new aura_script<spell_sha_natures_guardian>("spell_sha_natures_guardian");
    new aura_script<spell_sha_stone_bulwark>("spell_sha_stone_bulwark");
    new aura_script<spell_sha_stone_bulwark_totem_passive>("spell_sha_stone_bulwark_totem_passive");
    new spell_script<spell_sha_hp_based_totem>("spell_sha_hp_based_totem");
    new spell_script<spell_sha_totemic_projection>("spell_sha_totemic_projection");
    new spell_script<spell_sha_spirit_link>("spell_sha_spirit_link");
    new spell_script<spell_sha_elemental_totem_summon>("spell_sha_elemental_totem_summon");
    new spell_script<spell_sha_lava_lash>("spell_sha_lava_lash");
    new spell_script<spell_sha_lava_lash_spread_flame_shock>("spell_sha_lava_lash_spread_flame_shock");
    new spell_script<spell_sha_healing_stream>("spell_sha_healing_stream");
    new aura_script<spell_sha_windfury_weapon>("spell_sha_windfury_weapon");
    new spell_script<spell_sha_call_of_the_elements>("spell_sha_call_of_the_elements");
    new spell_script<spell_sha_fire_nova>("spell_sha_fire_nova");
    new spell_script<spell_sha_glyph_of_purging>("spell_sha_glyph_of_purging");
    new spell_script<spell_sha_stormstrike>("spell_sha_stormstrike");
    new spell_script<spell_sha_static_charge>("spell_sha_static_charge");
    new aura_script<spell_sha_ancestral_awakening>("spell_sha_ancestral_awakening");
    new aura_script<spell_sha_ancestral_awakening>("spell_sha_t15_restoration_4p_bonus");
    new spell_script<spell_sha_ancestral_awakening_selector>("spell_sha_ancestral_awakening_selector");
    new spell_script<spell_sha_searing_flames_summon>("spell_sha_searing_flames_summon");
    new aura_script<spell_sha_searing_flames_proc_aura>("spell_sha_searing_flames_proc_aura");
    new aura_script<spell_sha_ancestral_guidance>("spell_sha_ancestral_guidance");
    new spell_script<spell_sha_acestral_guidance_heal>("spell_sha_acestral_guidance_heal");
    new aura_script<spell_sha_purification>("spell_sha_purification");
    new aura_script<spell_sha_static_shock>("spell_sha_static_shock");
    new aura_script<spell_sha_enchanced_elements>("spell_sha_enchanced_elements");
    new aura_script<spell_sha_elemental_totem>("spell_sha_elemental_totem");
    new aura_script<spell_sha_flametongue_weapon>("spell_sha_flametongue_weapon");
    new aura_script<spell_sha_resurgence>("spell_sha_resurgence");
    new aura_script<spell_sha_glyph_of_flame_shock>("spell_sha_glyph_of_flame_shock");
    new spell_script<spell_sha_spiritwalkers_grace>("spell_sha_spiritwalkers_grace");
    new aura_script<spell_sha_grounding_totem_effect>("spell_sha_grounding_totem_effect");
    new creature_script<npc_sha_spirit_wolf>("npc_sha_spirit_wolf");
    new aura_script<spell_sha_spirit_hunt>("spell_sha_spirit_hunt");
    new aura_script<spell_sha_stormlash_totem>("spell_sha_stormlash_totem");
    new spell_script<spell_sha_glyph_of_cleansing_waters>("spell_sha_glyph_of_cleansing_waters");
    new spell_script<spell_sha_maelstrom_weapon_haste_fix>("spell_sha_maelstrom_weapon_haste_fix");
    new spell_script<spell_sha_t15_enhancement_2p_bonus>("spell_sha_t15_enhancement_2p_bonus");
    new spell_script<spell_sha_lightning_strike>("spell_sha_lightning_strike");
    new aura_script<spell_sha_flow_elements>("spell_sha_flow_elements");
    new spell_script<spell_sha_t15_elemental_4p_bonus>("spell_sha_t15_elemental_4p_bonus");
    new aura_script<spell_sha_t16_enchancement_4p_bonus>("spell_sha_t16_enchancement_4p_bonus");
    new spell_script<spell_sha_t16_restoration_2p_bonus>("spell_sha_t16_restoration_2p_bonus");
    new creature_script<npc_sha_spiritwalker_champion>("npc_sha_spiritwalker_champion");
    new aura_script<spell_sha_t16_restoration_4p_heal_trigger>("spell_sha_t16_restoration_4p_heal_trigger");
    new spell_script<spell_sha_t16_restoration_4p_selector>("spell_sha_t16_restoration_4p_selector");
    new aura_script<spell_sha_t16_elemental_2p_bonus>("spell_sha_t16_elemental_2p_bonus");
    new creature_script<npc_sha_lightning_elemental>("npc_sha_lightning_elemental");
    new creature_script<npc_sha_earth_elemental>("npc_sha_earth_elemental");
    new creature_script<npc_sha_fire_elemental>("npc_sha_fire_elemental");
    new aura_script<spell_sha_glyph_of_healing_wave>("spell_sha_glyph_of_healing_wave");
    new aura_script<spell_sha_maelstrom_weapon_visual>("spell_sha_maelstrom_weapon_visual");
    new spell_script<spell_sha_glyph_of_thunder>("spell_sha_glyph_of_thunder");
    new aura_script<spell_sha_mana_tide>("spell_sha_mana_tide");
    new aura_script<spell_sha_clearcasting>("spell_sha_clearcasting");
    new aura_script<spell_sha_glyph_of_totemic_encirclement>("spell_sha_glyph_of_totemic_encirclement");
    new aura_script<spell_sha_glyph_of_eternal_earth>("spell_sha_glyph_of_eternal_earth");
    new spell_script<spell_sha_glyph_of_spirit_wolf>("spell_sha_glyph_of_spirit_wolf");
    new spell_script<spell_sha_summon_elemental_familiar>("spell_sha_summon_elemental_familiar");
    new spell_script<spell_sha_elemental_familiar_selector>("spell_sha_elemental_familiar_selector");
}
