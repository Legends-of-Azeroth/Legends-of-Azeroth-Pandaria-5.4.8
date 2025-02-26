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
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Pet.h"
#include "SpellHistory.h"
#include "Random.h"

enum WarlockSpells
{
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON             = 48018,
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON_DEMO        = 104135,
    SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT           = 48020,
    SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT_ANIM      = 104163,
    SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST         = 62388,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL        = 31117,
    SPELL_WARLOCK_GLYPH_OF_FEAR                     = 56244,
    SPELL_WARLOCK_FEAR_EFFECT                       = 118699,
    SPELL_WARLOCK_GLYPH_OF_FEAR_EFFECT              = 130616,
    SPELL_WARLOCK_CREATE_HEALTHSTONE                = 23517,
    SPELL_WARLOCK_CREATE_HEALTHSTONE_CHARGES        = 120038,
    SPELL_WARLOCK_DRAIN_LIFE_HEAL                   = 89653,
    SPELL_WARLOCK_SOULBURN                          = 74434,
    SPELL_WARLOCK_CORRUPTION                        = 172,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION               = 30108,
    SPELL_WARLOCK_SHADOWBURN                        = 17877,
    SPELL_WARLOCK_SHADOWBURN_ENERGIZE               = 125882,
    SPELL_WARLOCK_FEL_FLAME                         = 77799,
    SPELL_WARLOCK_RAIN_OF_FIRE                      = 104233,
    SPELL_WARLOCK_CONFLAGRATE                       = 17962,
    SPELL_WARLOCK_CONFLAGRATE_FIRE_AND_BRIMSTONE    = 108685,
    SPELL_WARLOCK_INCINERATE                        = 29722,
    SPELL_WARLOCK_INCINERATE_FIRE_AND_BRIMSTONE     = 114654,
    SPELL_WARLOCK_IMMOLATE                          = 348,
    SPELL_WARLOCK_IMMOLATE_FIRE_AND_BRIMSTONE       = 108686,
    SPELL_WARLOCK_FIRE_AND_BRIMSTONE                = 108683,
    SPELL_WARLOCK_DEMONIC_GATEWAY_TELE_GREEN        = 113896,
    SPELL_WARLOCK_DEMONIC_GATEWAY_TELE_PURPLE       = 120729,
    SPELL_WARLOCK_DEMONIC_GATEWAY_AURA              = 113901,
    SPELL_WARLOCK_DEMONIC_GATEWAY_COOLDOWN          = 113942,
    SPELL_WARLOCK_ITEM_WARLOCK_PVP_SET_4P_BONUS     = 143395,
    SPELL_WARLOCK_SOUL_SWAP_AURA                    = 86211,
    SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER              = 92795,
    SPELL_WARLOCK_DEMONIC_LEAP_JUMP                 = 109163,
    SPELL_WARLOCK_ITEM_S12_TIER_4                   = 131632,
    SPELL_WARLOCK_TWILIGHT_WARD_S12                 = 131623,
    SPELL_WARLOCK_TWILIGHT_WARD_METAMORPHOSIS_S12   = 131624,
    SPELL_WARLOCK_SOUL_LEECH_ABSORB                 = 108366,
    SPELL_WARLOCK_DARK_REGENERATION                 = 108359,
    SPELL_WARLOCK_FEL_ARMOR                         = 104938,
    SPELL_WARLOCK_DARK_BARGAIN_DOT                  = 110914,
    SPELL_WARLOCK_MOLTEN_CORE                       = 122355,
    SPELL_WARLOCK_MOLTEN_CORE_XERRATH               = 140074,
    SPELL_WARLOCK_MOLTEN_CORE_PASSIVE               = 122351,
    SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT          = 126090,
    SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT_XERRATH  = 140075,
    SPELL_WARLOCK_WILD_IMPS                         = 114592,
    SPELL_WARLOCK_WILD_IMP_SUMMON                   = 104317,
    SPELL_WARLOCK_DEMONIC_CALL                      = 114925,
    SPELL_WARLOCK_DECIMATE_AURA                     = 108869,
    SPELL_WARLOCK_GLYPH_OF_CONFLAGRATE              = 56235,
    SPELL_WARLOCK_SHIELD_OF_SHADOW                  = 115232,
    SPELL_WARLOCK_THREATENING_PRESENCE              = 112042,
    SPELL_WARLOCK_OVERRIDE_COMMAND_DEMON            = 119904,
    SPELL_WARLOCK_KIL_JAEDENS_CUNNING_PASSIVE       = 108507,
    SPELL_WARLOCK_IMP_SWARM                         = 104316,
    SPELL_WARLOCK_DISRUPTED_NETHER                  = 114736,
    SPELL_WARLOCK_GLYPH_OF_IMP_SWARM                = 56242,
    SPELL_WARLOCK_GLYPH_OF_SOULWELL                 = 58094,
    SPELL_WARLOCK_GLYPH_OF_SOULWELL_VISUAL          = 34145,
    SPELL_WARLOCK_SOULBURN_SEED_OF_CORRUPTION       = 86664,
    SPELL_WARLOCK_SOULBURN_CURSE                    = 103112,
    SPELL_WARLOCK_SOULBURN_DEMONIC_CIRCLE_TELEPORT  = 104243,
    SPELL_WARLOCK_SOULBURN_SOUL_SWAP                = 141931,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_BASE            = 93312,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_SEED            = 93313,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE1          = 104245,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE2          = 104250,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_TELEPORT        = 104249,
    SPELL_WARLOCK_SOULBURN_HEALTH_FUNNEL            = 103101,
    SPELL_WARLOCK_SOULBURN_OVERRIDE_SOUL_SWAP       = 104251,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_DUMMY          = 86664,
    SPELL_WARLOCK_SUMMON_DEMON_COOLDOWN             = 120465,
    SPELL_WARLOCK_SUMMON_FEL_IMP                    = 112866,
    SPELL_WARLOCK_SUMMON_VOIDLORD                   = 112867,
    SPELL_WARLOCK_SUMMON_SHIVARRA                   = 112868,
    SPELL_WARLOCK_SUMMON_OBSERVER                   = 112869,
    SPELL_WARLOCK_SUMMON_WRATHGUARD                 = 112870,
    SPELL_WARLOCK_SUMMON_ABYSSAL                    = 112921,
    SPELL_WARLOCK_SUMMON_TERRORGUARD                = 112927,
    SPELL_WARLOCK_METAMORPHOSIS                     = 103958,
    SPELL_WARLOCK_DEMONIC_FURY                      = 109145,
    SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE             = 104314,
    SPELL_WARLOCK_MAXIMUM_FURY                      = 131755,
    SPELL_WARLOCK_MASTER_DEMONOLOGIST_VISUAL        = 122738,
    SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST       = 77219,
    SPELL_WARLOCK_MASTER_DEMONOLOGIST               = 115556,
    SPELL_WARLOCK_SOULSHARD                         = 87388,
    SPELL_WARLOCK_SOULSHARD_ENERGIZE                = 95810,
    SPELL_WARLOCK_SHADOW_TRANCE                     = 17941,
    SPELL_WARLOCK_DARK_APOTHEOSIS                   = 114168,
    SPELL_WARLOCK_DEMONIC_SLASH                     = 114175,
    SPELL_WARLOCK_GLYPH_OF_FALLING_METEOR           = 56247,
    SPELL_WARLOCK_METEOR_SLAM                       = 124315,
    SPELL_WARLOCK_GLYPH_OF_DEMON_HUNTING            = 63303,
    SPELL_WARLOCK_GLYPH_OF_HEALTHSTONE              = 56224,
    SPELL_WARLOCK_PYROCLASM                         = 123686,
    SPELL_WARLOCK_CHAOSBOLT                         = 116858,
    SPELL_WARLOCK_BACKDRAFT                         = 117828,
    SPELL_WARLOCK_BACKLASH                          = 34936,
    SPELL_WARLOCK_BACKLASH_XERRATH                  = 140076,
    SPELL_WARLOCK_SOULSTONE_RESURRECT               = 95750,
    SPELL_WARLOCK_AGONY                             = 980,
    SPELL_WARLOCK_AGONY_INSTANT                     = 131737,
    SPELL_WARLOCK_CORRUPTION_DOT                    = 146739,
    SPELL_WARLOCK_CORRUPTION_INSTANT                = 131740,
    SPELL_WARLOCK_UNSTABLE_AFFLICITON               = 30108,
    SPELL_WARLOCK_UNSTABLE_AFFLICITON_INSTANT       = 131736,
    SPELL_WARLOCK_SEED_OF_CORRUPTION                = 27243,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_INSTANT        = 132566,
    SPELL_WARLOCK_PET_FEL_ENERGY                    = 123746,
    SPELL_WARLOCK_MASTERT_EMBERSTORM                = 77220,
    SPELL_WARLOCK_MASTERY_POTENT_AFFLICTION         = 77215,
    SPELL_WARLOCK_HAND_OF_GULDAN_DAMAGE             = 86040,
    SPELL_WARLOCK_HARVEST_LIFE                      = 108371,
    SPELL_WARLOCK_GLYPH_OF_DRAIN_LIFE               = 63302,
    SPELL_WARLOCK_SIPHON_LIFE                       = 63106,
    SPELL_WARLOCK_COMMAND_CAUTERIZE_MASTER          = 119905,
    SPELL_WARLOCK_COMMAND_DISARM                    = 119907,
    SPELL_WARLOCK_COMMAND_WHIPLASH                  = 119909,
    SPELL_WARLOCK_COMMAND_SPELL_LOCK                = 119910,
    SPELL_WARLOCK_COMMAND_OPTICAL_BLAST             = 119911,
    SPELL_WARLOCK_COMMAND_FELLASH                   = 119913,
    SPELL_WARLOCK_COMMAND_FELSTORM                  = 119914,
    SPELL_WARLOCK_COMMAND_WRATHSTORM                = 119915,
    SPELL_WARLOCK_HEALTH_FUNNEL_SOULBURN            = 104220,
    SPELL_WARLOCK_BLOOD_HORROR                      = 137143,
    SPELL_WARLOCK_HAVOC                             = 80240,
    SPELL_WARLOCK_MORTAL_COIL                       = 6789,
    SPELL_WARLOCK_SOUL_LINK                         = 108415,
    SPELL_WARLOCK_SOUL_LINK_HEAL                    = 108447,
    SPELL_WARLOCK_HOWL_OF_TERROR                    = 5484,
    SPELL_WARLOCK_HELLFIRE                          = 1949,
    SPELL_WARLOCK_T15_4P_BONUS                      = 138134,
    SPELL_WARLOCK_DEVOUR_MAGIC_HEAL_ENERGIZE        = 19658,
    SPELL_WARLOCK_GLYPH_OF_DEMON_TRAINING           = 56249,
    SPELL_WARLOCK_UNENDING_BREATH_WATERWALK         = 104242,
    SPELL_WARLOCK_SOULSHARDS_1_3                    = 104756,
    SPELL_WARLOCK_SOULSHARDS_2_3_4                  = 104759,
    SPELL_WARLOCK_SOULSHARDS_4                      = 123171,
    SPELL_WARLOCK_VERDANT_SHPERES_1_3               = 123728,
    SPELL_WARLOCK_VERDANT_SHPERES_2_3_4             = 123730,
    SPELL_WARLOCK_VERDANT_SHPERES_4                 = 123731,
    SPELL_WARLOCK_GLYPH_OF_VERDANT_SPHERES          = 56241,
    SPELL_WARLOCK_SHARDS_VISUAL_TEST                = 117197,
    SPELL_WARLOCK_SEARING_EMBERS_1                  = 116855,
    SPELL_WARLOCK_SEARING_EMBERS_2                  = 116920,
    SPELL_WARLOCK_EMBER_TEST                        = 116860,
    SPELL_WARLOCK_GLYPH_OF_EMBER_TAP                = 63304,
    SPELL_WARLOCK_THE_CODEX_OF_XERRATH              = 101508,
    SPELL_WARLOCK_CURSE_OF_EXHAUSTION               = 18223,
    SPELL_WARLOCK_DRAIN_SOUL                        = 1120,
    SPELL_WARLOCK_GLYPH_OF_SOUL_CONSUMPTION         = 58068,
    SPELL_WARLOCK_GRIMOIRE_OF_SACRIFICE             = 108503,
    SPELL_WARLOCK_SOUL_FIRE                         = 6353,
    SPELL_WARLOCK_SOUL_FIRE_DEMON                   = 104027,
    SPELL_WARLOCK_DESTRUCTIVE_INFLUENCE             = 145075,
    SPELL_WARLOCK_EMPOWERED_GRASP                   = 145082,
    SPELL_WARLOCK_FIERY_WRATH                       = 145085,
    SPELL_WARLOCK_T16_4P_BONUS                      = 145091,
    SPELL_WARLOCK_DARK_REFUND                       = 145159,
    SPELL_WARLOCK_HAND_OF_GULDAN_T16                = 145158,
    SPELL_WARLOCK_SHADOW_BOLT                       = 686,
    SPELL_WARLOCK_GLYPH_OF_SHADOW_BOLT              = 56240,
    SPELL_WARLOCK_PVP_SET_2P_BONUS                  = 131632,
    SPELL_WARLOCK_FURY_WARD                         = 119839,
    SPELL_WARLOCK_GLYPH_OF_LIFE_TAP                 = 63320,
    SPELL_WARLOCK_GLYPH_OF_GATEWAY_ATTUNEMENT       = 135557,
    SPELL_WARLOCK_GLYPH_OF_FELGUARD                 = 56246,

    SPELL_INFERNAL_IMMOLATION                       = 39007,
    SPELL_ABYSSAL_IMMOLATION                        = 19483,
};

enum Misc
{
    NPC_INFERNAL                            = 89,
    NPC_IMP                                 = 416,
    NPC_FELHUNTER                           = 417,
    NPC_VOIDWALKER                          = 1860,
    NPC_SUCCUBUS                            = 1863,
    NPC_FELGUARD                            = 17252,
    NPC_FEL_IMP                             = 58959,
    NPC_VOIDLORD                            = 58960,
    NPC_OBSERVER                            = 58964,
    NPC_SHIVARRA                            = 58963,
    NPC_WRATHGUARD                          = 58965,
    NPC_ABYSSAL                             = 58997,
    NPC_ABYSSAL_XERRATH                     = 70626,
    NPC_GATEWAY_PURPLE                      = 59271,
    NPC_GATEWAY_GREEN                       = 59262,
};

// 755 - Health Funnel
class spell_warl_health_funnel : public AuraScript
{
    PrepareAuraScript(spell_warl_health_funnel);

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
        {
            if (AuraEffect const* dummy = caster->GetAuraEffect(GetId(), EFFECT_2))
            {
                uint32 funnelDamage = caster->CountPctFromMaxHealth(dummy->GetAmount());
                uint32 funnelAbsorb = 0;
                caster->DealDamageMods(caster, funnelDamage, &funnelAbsorb);
                caster->SendSpellNonMeleeDamageLog(caster, GetId(), funnelDamage, GetSpellInfo()->GetSchoolMask(), funnelAbsorb, 0, false, 0, false);

                CleanDamage cleanDamage = CleanDamage(0, 0, BASE_ATTACK, MELEE_HIT_NORMAL);
                caster->DealDamage(caster, funnelDamage, &cleanDamage, NODAMAGE, GetSpellInfo()->GetSchoolMask(), GetSpellInfo(), true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_health_funnel::HandleTick, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 755 - Health Funnel
// 114189 - Health Funnel (glyphed)
class spell_warl_health_funnel_soulburn : public SpellScript
{
    PrepareSpellScript(spell_warl_health_funnel_soulburn);

    void HandleHit()
    {
        if (GetCaster()->HasAuraEffect(SPELL_WARLOCK_SOULBURN, EFFECT_2))
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_HEALTH_FUNNEL_SOULBURN, true);
            GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_health_funnel_soulburn::HandleHit);
    }
};

// Soulburn : Seed of Corruption - Damage - 87385
class spell_warl_soulburn_seed_of_corruption_damage : public SpellScriptLoader
{
    public:
        spell_warl_soulburn_seed_of_corruption_damage() : SpellScriptLoader("spell_warl_soulburn_seed_of_corruption_damage") { }

        class spell_warl_soulburn_seed_of_corruption_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulburn_seed_of_corruption_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (GetExplTargetUnit())
                    targets.remove(GetExplTargetUnit());
            }

            void HandleScript()
            {
                auto caster = GetCaster();
                if (!caster)
                    return;

                // Remove Soul Burn aura
                if (caster->HasAura(SPELL_WARLOCK_SEED_OF_CORRUPTION_DUMMY))
                    caster->RemoveAurasDueToSpell(SPELL_WARLOCK_SEED_OF_CORRUPTION_DUMMY);
            }

            void HandleAfterCast()
            {
                auto caster = GetCaster();
                if (!caster)
                    return;

                // Give Soul Shard
                if (caster->HasAura(SPELL_WARLOCK_SEED_OF_CORRUPTION_DUMMY))
                    caster->SetPower(POWER_SOUL_SHARDS, caster->GetPower(POWER_SOUL_SHARDS) + 100);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_soulburn_seed_of_corruption_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_warl_soulburn_seed_of_corruption_damage_SpellScript::HandleScript);
                OnCast += SpellCastFn(spell_warl_soulburn_seed_of_corruption_damage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soulburn_seed_of_corruption_damage_SpellScript();
        }
};

// 114790 - Seed of Corruption (Soulburn)
class spell_warl_soulburn_seed_of_corruption : public SpellScript
{
    PrepareSpellScript(spell_warl_soulburn_seed_of_corruption);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_SEED_OF_CORRUPTION_DUMMY, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_soulburn_seed_of_corruption::HandleHit);
    }
};

// 74434 - Soulburn
class spell_warl_soulburn_override : public AuraScript
{
    PrepareAuraScript(spell_warl_soulburn_override);

    void InitEffects(uint32& mask)
    {
        mask &= ~(1 << EFFECT_1);
        if (!GetUnitOwner()->HasSpell(SPELL_WARLOCK_SOULBURN_HEALTH_FUNNEL))
            mask &= ~(1 << EFFECT_2);
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warlock = GetUnitOwner();
        warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_BASE, true);            // Drain Life, Undending Breath and Harvest Life
        if (warlock->HasSpell(SPELL_WARLOCK_SOULBURN_CURSE))
        {
            warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE1, true);      // Curse of Enfeeblement, Curse of the Elements
            warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE2, true);      // Curse of Exhaustion
        }
        if (warlock->HasSpell(SPELL_WARLOCK_SOULBURN_SEED_OF_CORRUPTION))
            warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_SEED, true);        // Seed of Corruption
        if (warlock->HasSpell(SPELL_WARLOCK_SOULBURN_DEMONIC_CIRCLE_TELEPORT))
            warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_TELEPORT, true);    // Demonic Circle: Teleport
        if (warlock->HasSpell(SPELL_WARLOCK_SOULBURN_SOUL_SWAP))
            warlock->CastSpell(warlock, SPELL_WARLOCK_SOULBURN_OVERRIDE_SOUL_SWAP, true);   // Soul Swap
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warlok = GetUnitOwner();
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_BASE);
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_SEED);
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE1);
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_TELEPORT);
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_CURSE2);
        warlok->RemoveAura(SPELL_WARLOCK_SOULBURN_OVERRIDE_SOUL_SWAP);
    }

    void HandleOnDropModCharge(Spell* bySpell)
    {
        if (bySpell->GetSpellInfo()->GetCategory() == 1363)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARLOCK_SUMMON_DEMON_COOLDOWN, true);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_warl_soulburn_override::InitEffects);
        OnEffectApply += AuraEffectApplyFn(spell_warl_soulburn_override::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_soulburn_override::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        OnDropModCharge += AuraDropModChargeFn(spell_warl_soulburn_override::HandleOnDropModCharge);
    }
};

// 104223 - Curse of Exhaustion
// 104225 - Curse of the Elements
// 109468 - Curse of Enfeeblement
// 114790 - Seed of Corruption
// 114794 - Demonic Circle: Teleport (Soulburn)
class spell_soulburn_consumer : public SpellScript
{
    PrepareSpellScript(spell_soulburn_consumer);

    void HandleCast()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_soulburn_consumer::HandleCast);
    }
};

// 104223 - Curse of Exhaustion
class spell_warl_curse_of_exhaustion_soulburn : public SpellScript
{
    PrepareSpellScript(spell_warl_curse_of_exhaustion_soulburn);

    bool Validate(SpellInfo const*) override
    {
        return sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CURSE_OF_EXHAUSTION) != nullptr;
    }

    void HandleCast()
    {
        if (Player* warlock = GetCaster()->ToPlayer())
        {
            warlock->GetSpellHistory()->StartCooldown(sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CURSE_OF_EXHAUSTION), 0);
            uint32 cooldown = warlock->GetSpellCooldownDelay(SPELL_WARLOCK_CURSE_OF_EXHAUSTION);
            if (cooldown)
                warlock->GetSpellHistory()->SendCooldown(SPELL_WARLOCK_CURSE_OF_EXHAUSTION, cooldown);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_curse_of_exhaustion_soulburn::HandleCast);
    }
};

// Called by Create Soulwell - 29893
// Glyph of Soulwell - 58094
class spell_warl_glyph_of_soulwell : public SpellScriptLoader
{
    public:
        spell_warl_glyph_of_soulwell() : SpellScriptLoader("spell_warl_glyph_of_soulwell") { }

        class spell_warl_glyph_of_soulwell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_glyph_of_soulwell_SpellScript);

            void HandleAfterCast()
            {
                if (!GetCaster())
                    return;

                if (!GetExplTargetDest())
                    return;

                if (!GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_SOULWELL))
                    return;

                GetCaster()->CastSpell(GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY(), GetExplTargetDest()->GetPositionZ(), SPELL_WARLOCK_GLYPH_OF_SOULWELL_VISUAL, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_warl_glyph_of_soulwell_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_glyph_of_soulwell_SpellScript();
        }
};

// Singe Magic
class spell_warl_singe_magic : public SpellScriptLoader
{
    public:
        spell_warl_singe_magic() : SpellScriptLoader("spell_warl_singe_magic") { }

        class spell_warl_singe_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_singe_magic_SpellScript);

            SpellCastResult CheckAutoCast()
            {
                if (!GetCaster())
                    return SPELL_FAILED_DONT_REPORT;

                Unit* owner = GetCaster()->GetOwner();
                if (!owner)
                    return SPELL_FAILED_DONT_REPORT;

                if (!owner->HasCrowdControlAura(NULL, DISPEL_MAGIC))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckAutoCast += SpellCheckCastFn(spell_warl_singe_magic_SpellScript::CheckAutoCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_singe_magic_SpellScript();
        }
};

// Glyph of Imp Swarm - 56242
class spell_warl_glyph_of_imp_swarm : public SpellScriptLoader
{
    public:
        spell_warl_glyph_of_imp_swarm() : SpellScriptLoader("spell_warl_glyph_of_imp_swarm") { }

        class spell_warl_glyph_of_imp_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_glyph_of_imp_swarm_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(SPELL_WARLOCK_IMP_SWARM, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(SPELL_WARLOCK_IMP_SWARM))
                        _player->RemoveSpell(SPELL_WARLOCK_IMP_SWARM, false, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_warl_glyph_of_imp_swarm_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_warl_glyph_of_imp_swarm_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_glyph_of_imp_swarm_AuraScript();
        }
};

// 108482 - Unbound Will
class spell_warl_unbound_will : public SpellScript
{
    PrepareSpellScript(spell_warl_unbound_will);

    SpellCastResult CheckHealth()
    {
        if (GetCaster()->GetHealthPct() <= 20.0f)
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOT_ENOUGH_HEALTH);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        if (Player* warlock = GetCaster()->ToPlayer())
        {
            warlock->ModifyHealth(-int32(warlock->CountPctFromMaxHealth(20)));
            warlock->RemoveAurasWithMechanicOnImmunity(IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK);
            if (Pet* pet = warlock->GetPet())
            {
                pet->RemoveAurasWithMechanicOnImmunity(IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK);
                DispelChargesList dispelList;
                pet->GetDispellableAuraList(warlock, SpellInfo::GetDispelMask(DISPEL_MAGIC), dispelList);
                for (auto&& itr : dispelList)
                    if (int32 chance = itr.first->CalcDispelChance(pet, false))
                        if (roll_chance_i(chance))
                            pet->RemoveAurasDueToSpellByDispel(itr.first->GetId(), GetSpellInfo()->Id, itr.first->GetCasterGUID(), warlock, itr.second);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_unbound_will::CheckHealth);
        OnCast += SpellCastFn(spell_warl_unbound_will::HandleCast);
    }
};

// Kil'Jaeden's Cunning (passive with cooldown) - 119048
class spell_warl_kil_jaedens_cunning : public SpellScriptLoader
{
    public:
        spell_warl_kil_jaedens_cunning() : SpellScriptLoader("spell_warl_kil_jaedens_cunning") { }

        class spell_warl_kil_jaedens_cunning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_kil_jaedens_cunning_AuraScript);

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->RemoveAura(SPELL_WARLOCK_KIL_JAEDENS_CUNNING_PASSIVE);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_KIL_JAEDENS_CUNNING_PASSIVE, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_warl_kil_jaedens_cunning_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_warl_kil_jaedens_cunning_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_kil_jaedens_cunning_AuraScript();
        }
};

// Voidwalker : Shield of Shadow - 103130
class spell_warl_shield_of_shadow : public SpellScriptLoader
{
    public:
        spell_warl_shield_of_shadow() : SpellScriptLoader("spell_warl_shield_of_shadow") { }

        class spell_warl_shield_of_shadow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_shield_of_shadow_AuraScript);

            void OnUpdate(uint32 /*diff*/, AuraEffect* /*aurEff*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (Pet* pet = player->GetPet())
                    {
                        if (pet->GetEntry() == 1860) // Voidwalker
                        {
                            if (!pet->HasSpell(SPELL_WARLOCK_SHIELD_OF_SHADOW))
                                pet->AddSpell(SPELL_WARLOCK_SHIELD_OF_SHADOW);
                            if (!pet->HasSpell(SPELL_WARLOCK_THREATENING_PRESENCE))
                                pet->AddSpell(SPELL_WARLOCK_THREATENING_PRESENCE);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_warl_shield_of_shadow_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_shield_of_shadow_AuraScript();
        }
};

// 120451 - Flames of Xoroth
class spell_warl_flames_of_xoroth : public SpellScript
{
    PrepareSpellScript(spell_warl_flames_of_xoroth);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    SpellCastResult CheckPet()
    {
        if (!GetCaster())
            return SPELL_FAILED_DONT_REPORT;

        auto const player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_DONT_REPORT;

        if (player->GetPet())
            return SPELL_FAILED_ALREADY_HAVE_PET;

        if (!player->GetLastPetNumber(true))
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    uint32 NormalizePetEntry(uint32 entry)
    {
        switch (entry)
        {
            case NPC_FEL_IMP:
                if (!GetCaster()->HasSpell(SPELL_WARLOCK_SUMMON_FEL_IMP))
                    return NPC_IMP;
                break;
            case NPC_VOIDLORD:
                if (!GetCaster()->HasSpell(SPELL_WARLOCK_SUMMON_VOIDLORD))
                    return NPC_VOIDWALKER;
                break;
            case NPC_OBSERVER:
                if (!GetCaster()->HasSpell(SPELL_WARLOCK_SUMMON_OBSERVER))
                    return NPC_FELHUNTER;
                break;
            case NPC_SHIVARRA:
                if (!GetCaster()->HasSpell(SPELL_WARLOCK_SUMMON_SHIVARRA))
                    return NPC_SUCCUBUS;
                break;
            case NPC_WRATHGUARD:
                if (!GetCaster()->HasSpell(SPELL_WARLOCK_SUMMON_WRATHGUARD))
                    return NPC_FELGUARD;
                break;
        }
        return entry;
    }

    void HandleDummy(SpellEffIndex)
    {
        auto player = GetCaster()->ToPlayer();

        if (uint32 entry = player->GetLastPetNumber(true))
        {
            std::unique_ptr<Pet> newPet{ new Pet(player, SUMMON_PET) };
            if (!newPet->LoadPetFromDB(PET_LOAD_BY_ENTRY, NormalizePetEntry(entry)))
                return;

            // revive the pet if it is dead
            if (newPet->getDeathState() == DEAD || newPet->getDeathState() == CORPSE)
                newPet->setDeathState(ALIVE);

            newPet->ClearUnitState(uint32(UNIT_STATE_ALL_STATE & ~UNIT_STATE_IGNORE_PATHFINDING));
            newPet->SetFullHealth();
            newPet->SetPower(newPet->GetPowerType(), newPet->GetMaxPower(newPet->GetPowerType()));
            newPet.release();
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_flames_of_xoroth::CheckPet);
        OnEffectHitTarget += SpellEffectFn(spell_warl_flames_of_xoroth::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 686, 112092 - Shadow Bolt, 6353, 104027 - Soul Fire
class spell_warl_decimate : public SpellScript
{
    PrepareSpellScript(spell_warl_decimate);

    void HandleCast()
    {
        uint32 spellId = GetCaster()->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_MOLTEN_CORE_XERRATH : SPELL_WARLOCK_MOLTEN_CORE;
        if (GetExplTargetUnit() && GetExplTargetUnit()->HealthBelowPct(25))
        {
            if (GetCaster()->HasAura(SPELL_WARLOCK_DECIMATE_AURA))
            {
                bool refresh = GetCaster()->HasAura(spellId);

                GetCaster()->CastSpell(GetCaster(), spellId, true);
                if (GetSpellInfo()->SpellFamilyFlags[1] & 0x80 && refresh)
                {
                    // Spell mods will be restored due to ModStackAmount(), so decrease charges manually
                    if (Aura* aura = GetCaster()->GetAura(spellId))
                    {
                        if (aura->GetCharges() <= 2)
                            GetCaster()->RemoveAurasDueToSpell(GetCaster()->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT_XERRATH : SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT);
                        aura->ModCharges(-1);
                    }
                }
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_decimate::HandleCast);
    }
};

// Chaos Wave - 124916
class spell_warl_chaos_wave : public SpellScript
{
    PrepareSpellScript(spell_warl_chaos_wave);

    void HandleAfterCast()
    {
        uint32 spellId = GetCaster()->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_MOLTEN_CORE_XERRATH : SPELL_WARLOCK_MOLTEN_CORE;
        GetCaster()->CastSpell(GetCaster(), spellId, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_chaos_wave::HandleAfterCast);
    }
};

// 110013 - Dark Bargain
class spell_warl_dark_bargain_on_absorb : public AuraScript
{
    PrepareAuraScript(spell_warl_dark_bargain_on_absorb);

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& /*absorbAmount*/)
    {
        PreventDefaultAction();
        int32 damage = dmgInfo.GetDamage();
        dmgInfo.AbsorbDamage(dmgInfo.GetDamage());
        aurEff->SetAmount(aurEff->GetAmount() + damage);
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes /*mode*/)
    {
        SpellInfo const* dot = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DARK_BARGAIN_DOT);
        int32 damage = eff->GetAmount() * GetEffect(EFFECT_1)->GetAmount() / 100.0f / dot->GetMaxTicks();
        GetTarget()->CastCustomSpell(SPELL_WARLOCK_DARK_BARGAIN_DOT, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true);
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_warl_dark_bargain_on_absorb::Absorb, EFFECT_0);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_dark_bargain_on_absorb::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108359 - Dark Regeneration
class spell_warl_dark_regeneration : public AuraScript
{
    PrepareAuraScript(spell_warl_dark_regeneration);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Guardian* pet = GetTarget()->GetGuardianPet())
            pet->CastSpell(pet, SPELL_WARLOCK_DARK_REGENERATION, true);
    }

    void CalculaetTick(AuraEffect const*, int32& heal)
    {
        if (AuraEffect const* armor = GetTarget()->GetAuraEffect(SPELL_WARLOCK_FEL_ARMOR, EFFECT_2))
            AddPct(heal, armor->GetFloatAmount());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_dark_regeneration::HandleApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_warl_dark_regeneration::CalculaetTick, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// Sacrificial Pact - 108416
class spell_warl_sacrificial_pact : public SpellScriptLoader
{
    public:
        spell_warl_sacrificial_pact() : SpellScriptLoader("spell_warl_sacrificial_pact") { }

        class spell_warl_sacrificial_pact_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_sacrificial_pact_AuraScript);

            void CalculateAmount(AuraEffect const* , float& amount, bool& )
            {
                if (Unit* caster = GetCaster())
                {
                    if (!caster->GetGuardianPet())
                    {
                        int32 sacrifiedHealth = caster->CountPctFromCurHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                        caster->ModifyHealth(-sacrifiedHealth);
                        amount = CalculatePct(sacrifiedHealth, amount);
                    }
                    else if (caster->GetGuardianPet())
                    {
                        int32 sacrifiedHealth = caster->GetGuardianPet()->CountPctFromCurHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);
                        caster->GetGuardianPet()->ModifyHealth(-sacrifiedHealth);
                        amount = CalculatePct(sacrifiedHealth, amount);
                    }
                }
            }
            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_sacrificial_pact_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_sacrificial_pact_AuraScript();
        }
};

// Twilight Ward - 6229 and Twilight Ward (Metamorphosis) - 104048
class spell_warl_twilight_ward_s12 : public SpellScriptLoader
{
    public:
        spell_warl_twilight_ward_s12() : SpellScriptLoader("spell_warl_twilight_ward_s12") { }

        class spell_warl_twilight_ward_s12_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_twilight_ward_s12_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->HasAura(SPELL_WARLOCK_ITEM_S12_TIER_4))
                    {
                        if (GetSpellInfo()->Id == 6229)
                        {
                            if (_player->HasAura(GetSpellInfo()->Id))
                                _player->RemoveAura(GetSpellInfo()->Id);

                            _player->CastSpell(_player, SPELL_WARLOCK_TWILIGHT_WARD_S12, true);
                        }
                        else if (GetSpellInfo()->Id == 104048)
                        {
                            if (_player->HasAura(GetSpellInfo()->Id))
                                _player->RemoveAura(GetSpellInfo()->Id);

                            _player->CastSpell(_player, SPELL_WARLOCK_TWILIGHT_WARD_METAMORPHOSIS_S12, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warl_twilight_ward_s12_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_twilight_ward_s12_SpellScript();
        }
};

// 111400 - Burning Rush
class spell_warl_burning_rush : public AuraScript
{
    PrepareAuraScript(spell_warl_burning_rush);

    void HandleTick(AuraEffect const*)
    {
        if (GetUnitOwner()->GetHealthPct() <= 4.0f)
        {
            Remove();
            return;
        }

        int32 damage = GetUnitOwner()->CountPctFromMaxHealth(4);
        GetUnitOwner()->DealDamage(GetUnitOwner(), damage, nullptr, NODAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_burning_rush::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 17962 - Conflagrate
class spell_warl_conflagrate_aura : public SpellScript
{
    PrepareSpellScript(spell_warl_conflagrate_aura);

    void HandleAura(SpellEffIndex eff)
    {
        if (!GetHitUnit()->HasAura(SPELL_WARLOCK_IMMOLATE) && !GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_CONFLAGRATE))
            PreventEffectForTarget(GetHitUnit(), eff);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_conflagrate_aura::HandleAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 29341 - Shadowburn
class spell_warl_shadowburn : public AuraScript
{
    PrepareAuraScript(spell_warl_shadowburn);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (Player* warlock = caster ? caster->ToPlayer() : nullptr)
        {
            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode == AURA_REMOVE_BY_DEATH && warlock->IsHonorOrXPTarget(GetTarget()))
                warlock->SetPower(POWER_BURNING_EMBERS, GetCaster()->GetPower(POWER_BURNING_EMBERS) + 20); // Give 2 Burning Embers
            else if (removeMode == AURA_REMOVE_BY_EXPIRE)
                warlock->CastSpell(warlock, SPELL_WARLOCK_SHADOWBURN_ENERGIZE, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_warl_shadowburn::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Soul Harvest - 101976
class spell_warl_soul_harverst : public SpellScriptLoader
{
    public:
        spell_warl_soul_harverst() : SpellScriptLoader("spell_warl_soul_harverst") { }

        class spell_warl_soul_harverst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_soul_harverst_AuraScript);

            uint32 update;

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                update = 0;

                if (!sSpellMgr->GetSpellInfo(101976))
                    return false;
                return true;
            }

            void OnUpdate(uint32 diff)
            {
                update += diff;

                if (update >= 1000)
                {
                    if (Player* _player = GetCaster()->ToPlayer())
                    {
                        if (!_player->IsInCombat() && !_player->InArena() && _player->IsAlive())
                        {
                            _player->SetHealth(_player->GetHealth() + int32(_player->GetMaxHealth() / 50));

                            if (Pet* pet = _player->GetPet())
                                pet->SetHealth(pet->GetHealth() + int32(pet->GetMaxHealth() / 50));
                        }
                    }

                    update = 0;
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_warl_soul_harverst_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_soul_harverst_AuraScript();
        }
};

// 1454 - Life Tap
class spell_warl_life_tap : public SpellScript
{
    PrepareSpellScript(spell_warl_life_tap);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetHealthPct() <= 15.0f)
            return SPELL_FAILED_FIZZLE;
        return SPELL_CAST_OK;
    }

    void CalculateEnergize(SpellEffIndex /*effIndex*/)
    {
        int32 mana = CalculatePct(GetCaster()->GetMaxHealth(), GetEffectValue());
        SetEffectValue(mana);
        if (!GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_LIFE_TAP))
            GetCaster()->ModifyHealth(-mana);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_life_tap::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap::CalculateEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// 1454 - Life Tap
class spell_warl_life_tap_heal_absorb : public AuraScript
{
    PrepareAuraScript(spell_warl_life_tap_heal_absorb);

    void CalcualteAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = CalculatePct(GetUnitOwner()->GetMaxHealth(), amount);
        if (AuraEffect const* existing = GetUnitOwner()->GetAuraEffect(GetId(), EFFECT_2))
            amount += existing->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_life_tap_heal_absorb::CalcualteAmount, EFFECT_2, SPELL_AURA_SCHOOL_HEAL_ABSORB);
    }
};

// 5782 - Fear 
class spell_warl_fear : public SpellScript
{
    PrepareSpellScript(spell_warl_fear);

    void HandleHit()
    {
        Unit* warlock = GetCaster();
        Unit* caster = GetHitResult() == SPELL_MISS_REFLECT ? GetExplTargetUnit() : warlock;
        Unit* target = GetHitResult() == SPELL_MISS_REFLECT ? warlock : GetHitUnit();

        if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_FEAR))
            caster->CastSpell(target, SPELL_WARLOCK_GLYPH_OF_FEAR_EFFECT, true);
        else
            caster->CastSpell(target, SPELL_WARLOCK_FEAR_EFFECT, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_fear::HandleHit);
    }
};

// Updated 4.3.4
class spell_warl_banish : public SpellScriptLoader
{
    public:
        spell_warl_banish() : SpellScriptLoader("spell_warl_banish") { }

        class spell_warl_banish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_banish_SpellScript);

            bool Load() override
            {
                _removed = false;
                return true;
            }

            void HandleBanish()
            {
                // Casting Banish on a banished target will cancel the effect
                // Check if the target already has Banish, if so, do nothing.

                if (Unit* target = GetHitUnit())
                {
                    flag128 flags(0U, 0x08000000U);
                    if (target->GetAuraEffect(SPELL_AURA_SCHOOL_IMMUNITY, SPELLFAMILY_WARLOCK, flags))
                    {
                        // No need to remove old aura since its removed due to not stack by current Banish aura
                        PreventHitDefaultEffect(EFFECT_0);
                        PreventHitDefaultEffect(EFFECT_1);
                        PreventHitDefaultEffect(EFFECT_2);
                        _removed = true;
                    }
                }
            }

            void RemoveAura()
            {
                if (_removed)
                    PreventHitAura();
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_warl_banish_SpellScript::HandleBanish);
                AfterHit += SpellHitFn(spell_warl_banish_SpellScript::RemoveAura);
            }

            bool _removed;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_banish_SpellScript();
        }
};

enum Soulshatter
{
    SPELL_SOULSHATTER   = 32835,
};

class spell_warl_soulshatter : public SpellScriptLoader
{
    public:
        spell_warl_soulshatter() : SpellScriptLoader("spell_warl_soulshatter") { }

        class spell_warl_soulshatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulshatter_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SOULSHATTER))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (target->CanHaveThreatList() && target->GetThreatManager().getThreat(caster) > 0.0f)
                        caster->CastSpell(target, SPELL_SOULSHATTER, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_soulshatter_SpellScript();
        }
};

// 30108 - Unstable Affliction
class spell_warl_unstable_affliction : public AuraScript
{
    PrepareAuraScript(spell_warl_unstable_affliction);

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (Unit* warlock = GetCaster())
        {
            if (AuraEffect const* aurEff = GetEffect(EFFECT_0))
            {
                Unit* target = dispelInfo->GetDispeller();
                int32 damage = aurEff->GetAmount() * 8;
                damage = target->SpellDamageBonusTaken(warlock, GetSpellInfo(), EFFECT_0, damage, SPELL_DIRECT_DAMAGE);
                warlock->ApplyResilience(target, &damage);
                if (target->HasAura(122783)) // Diffuse Magic some shitty bug/feature https://www.mmo-champion.com/threads/1902677-i-hate-revival?p=22951542&viewfull==1#post22951542
                    damage /= 0.1f;
                warlock->CastCustomSpell(SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL, SPELLVALUE_BASE_POINT0, damage, dispelInfo->GetDispeller(), true);
            }
        }
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_warl_unstable_affliction::HandleDispel);
    }
};

class spell_warl_haunt : public SpellScriptLoader
{
    public:
        spell_warl_haunt() : SpellScriptLoader("spell_warl_haunt") { }

        class spell_warl_haunt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_haunt_AuraScript);

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, 145159, true);
            }

            void Register() override
            {
                AfterDispel += AuraDispelFn(spell_warl_haunt_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warl_haunt_AuraScript();
        }
};

// 108563 - Backlash
class spell_warl_backlash : public AuraScript
{
    PrepareAuraScript(spell_warl_backlash);

    void HandleProc(ProcEventInfo&)
    {
        uint32 spellId = GetUnitOwner()->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_BACKLASH_XERRATH : SPELL_WARLOCK_BACKLASH;
        GetUnitOwner()->CastSpell(GetUnitOwner(), spellId, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_backlash::HandleProc);
    }
};

// 111397 - Blood Horror
class spell_warl_blood_horror: public SpellScript
{
    PrepareSpellScript(spell_warl_blood_horror);

    float HEALTH_COST_PCT = 5.0f;

    SpellCastResult CheckSpec()
    {
        if (GetCaster()->GetHealthPct() < HEALTH_COST_PCT)
            return SPELL_FAILED_FIZZLE;

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        GetCaster()->ModifyHealth(-int32(GetCaster()->CountPctFromMaxHealth(HEALTH_COST_PCT)));
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_blood_horror::CheckSpec);
        OnHit += SpellHitFn(spell_warl_blood_horror::HandleHit);
    }
};

// 111397 - Blood Horror
class spell_warl_blood_horror_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_blood_horror_aura);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo()->GetDamage())
            return false;
        if (eventInfo.TriggeredBySpell() && eventInfo.TriggeredBySpell()->IsTriggered()) // Why this can proc from triggered?
            return false;

        return eventInfo.GetActor() && (!eventInfo.GetActor()->IsSummon() || !eventInfo.GetActor()->GetCharmerOrOwnerPlayerOrPlayerItself());
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(eventInfo.GetActor(), SPELL_WARLOCK_BLOOD_HORROR, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_blood_horror_aura::CheckProc);
        OnProc += AuraProcFn(spell_warl_blood_horror_aura::HandleProc);
    }
};

// 108647 - Burning Embers
class spell_warl_burning_embers : public AuraScript
{
    PrepareAuraScript(spell_warl_burning_embers);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spell = eventInfo.GetSpellInfo();
        Unit* warlock = GetUnitOwner();

        bool isCrit = eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT;
        int8 embers = 0;

        if (eventInfo.GetProcTriggerContext()->Phase == ProcPhase::Cast)
        {
            switch (spell->Id)
            {
                case SPELL_WARLOCK_CONFLAGRATE:
                case SPELL_WARLOCK_INCINERATE:
                case SPELL_WARLOCK_FEL_FLAME:
                    embers = isCrit ? 2 : 1;
                    break;
                case SPELL_WARLOCK_CONFLAGRATE_FIRE_AND_BRIMSTONE:
                case SPELL_WARLOCK_INCINERATE_FIRE_AND_BRIMSTONE:
                case SPELL_WARLOCK_IMMOLATE_FIRE_AND_BRIMSTONE:
                    for (auto&& itr : eventInfo.TriggeredBySpell()->GetUnitTargets())
                        embers += itr.crit ? 2 : 1;
                    break;
                default:
                    break;
            }

            // Conflagrate now has a 15% chance to generate an additional Burning Ember.
            // Incinerate now has a 15 % chance to generate an additional Burning Ember.
            // https://worldofwarcraft.com/en-us/news/10932273
            if (spell->SpellFamilyFlags[1] & 0x00800040 && roll_chance_i(15))
                embers += 1;
        }
        else
        {
            switch (spell->Id)
            {
                case SPELL_WARLOCK_RAIN_OF_FIRE:
                    if (roll_chance_i(10))
                        embers = 1;
                    break;
                case SPELL_WARLOCK_IMMOLATE:
                case SPELL_WARLOCK_IMMOLATE_FIRE_AND_BRIMSTONE:
                    if (isCrit)
                        embers = 1;
                    break;
                default:
                    break;
            }
        }

        // Incinerate + T15 4P
        if (spell->SpellFamilyFlags[1] & 0x40 && warlock->HasAura(SPELL_WARLOCK_T15_4P_BONUS) && roll_chance_i(8))
            embers *= 2;

        if (embers)
            warlock->ModifyPower(POWER_BURNING_EMBERS, embers);
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->SetMaxPower(POWER_BURNING_EMBERS, GetUnitOwner()->GetCreatePowers(POWER_BURNING_EMBERS));
        GetUnitOwner()->SetPower(POWER_BURNING_EMBERS, GetUnitOwner()->GetCreatePowers(POWER_BURNING_EMBERS) / 4);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->SetMaxPower(POWER_BURNING_EMBERS, 0);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_burning_embers::HandleProc);
        OnEffectApply += AuraEffectApplyFn(spell_warl_burning_embers::HandleApply, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_burning_embers::HandleRemove, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 56240 - Glyph of Shadow Bolt
class spell_warl_glyph_of_shadow_bolt : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_shadow_bolt);

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
            if (warlock->GetSpecialization() == SPEC_WARLOCK_DEMONOLOGY)
                warlock->LearnSpell(effect->GetAmount(), true);
    }

    void HandleRemove(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (Player* caster = GetUnitOwner()->ToPlayer())
            caster->RemoveSpell(effect->GetAmount());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_glyph_of_shadow_bolt::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_glyph_of_shadow_bolt::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 104315 - Demonic Fury
class spell_warl_glyph_of_shadow_bolt_spec_fix : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_shadow_bolt_spec_fix);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
            if (AuraEffect const* glyph = warlock->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_SHADOW_BOLT, EFFECT_0))
                warlock->LearnSpell(glyph->GetAmount(), true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
            if (auto const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_GLYPH_OF_SHADOW_BOLT))
                warlock->RemoveSpell(spellInfo->Effects[EFFECT_0].BasePoints);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_glyph_of_shadow_bolt_spec_fix::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_glyph_of_shadow_bolt_spec_fix::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 112092 - Shadow Bolt
class spell_warl_shadow_bolt_glyphed : public SpellScript
{
    PrepareSpellScript(spell_warl_shadow_bolt_glyphed);

    int32 damage = 0;

    void CalculateDamage(SpellEffIndex)
    {
        auto const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SHADOW_BOLT);
        damage = spellInfo->Effects[EFFECT_0].CalcValue(GetCaster()) + spellInfo->Effects[EFFECT_2].BonusMultiplier * GetCaster()->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask());
        damage /= 3;
        SetEffectValue(damage);
    }

    void HandleTrigger(SpellEffIndex index)
    {
        PreventDefaultEffect(index);
        GetCaster()->CastCustomSpell(GetSpellInfo()->Effects[index].TriggerSpell, SPELLVALUE_BASE_POINT0, damage, GetHitUnit(), true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_shadow_bolt_glyphed::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_shadow_bolt_glyphed::HandleTrigger, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_shadow_bolt_glyphed::HandleTrigger, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 108499 - Grimoire of Supremacy
class spell_warl_grimoire_of_supremacy : public AuraScript
{
    PrepareAuraScript(spell_warl_grimoire_of_supremacy);

    std::vector<uint32> const spells = {
        SPELL_WARLOCK_SUMMON_FEL_IMP, SPELL_WARLOCK_SUMMON_VOIDLORD, SPELL_WARLOCK_SUMMON_SHIVARRA, SPELL_WARLOCK_SUMMON_OBSERVER, SPELL_WARLOCK_SUMMON_ABYSSAL, SPELL_WARLOCK_SUMMON_TERRORGUARD
    };

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Player* player = GetTarget()->ToPlayer();
        if (!player)
            return;

        for (auto&& spell : spells)
            player->LearnSpell(spell, true);
        if (player->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY)
            player->LearnSpell(SPELL_WARLOCK_SUMMON_WRATHGUARD, true);

        player->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Player* player = GetTarget()->ToPlayer();
        if (!player)
            return;

        for (auto&& spell : spells)
            player->RemoveSpell(spell);
        player->RemoveSpell(SPELL_WARLOCK_SUMMON_WRATHGUARD);

        player->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_grimoire_of_supremacy::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_grimoire_of_supremacy::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 203778 - Grimoire of Service (custom server-side part)
class spell_warl_grimoire_of_service_server_side : public AuraScript
{
    PrepareAuraScript(spell_warl_grimoire_of_service_server_side);

    std::vector<uint32> const GrimoireOfService{ 111859, 111895, 111896, 111897, 111898 };
    Player* warlock = nullptr;

    bool Load() override
    {
        warlock = GetOwner()->ToPlayer();
        return warlock != nullptr;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        for (auto&& it : GrimoireOfService)
            if (it != 111898 || warlock->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY)
                warlock->LearnSpell(it, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        for (auto&& it : GrimoireOfService)
            warlock->RemoveSpell(it);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_grimoire_of_service_server_side::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_grimoire_of_service_server_side::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108503 - Grimoire of Sacrifice
class spell_warl_grimoire_of_sacrifice : public AuraScript
{
    PrepareAuraScript(spell_warl_grimoire_of_sacrifice);

    void InitEffects(uint32& mask)
    {
        Unit* warlock = GetCaster();
        if (!warlock || warlock->GetTypeId() != TYPEID_PLAYER)
            return;
        // Remove always.
        mask &= ~(1 << EFFECT_9);
        mask &= ~(1 << EFFECT_10);
        switch (warlock->ToPlayer()->GetTalentSpecialization())
        {
            case SPEC_WARLOCK_AFFLICTION:
                mask &= ~(1 << EFFECT_3);
                mask &= ~(1 << EFFECT_4);
                break;
            case SPEC_WARLOCK_DEMONOLOGY:
                mask &= ~(1 << EFFECT_2);
                mask &= ~(1 << EFFECT_4);
                mask &= ~(1 << EFFECT_5);
                mask &= ~(1 << EFFECT_7);
                break;
            case SPEC_WARLOCK_DESTRUCTION:
                mask &= ~(1 << EFFECT_2);
                mask &= ~(1 << EFFECT_3);
                mask &= ~(1 << EFFECT_5);
                mask &= ~(1 << EFFECT_7);
                break;
            default:
                mask &= ~(1 << EFFECT_2);
                mask &= ~(1 << EFFECT_3);
                mask &= ~(1 << EFFECT_4);
                mask &= ~(1 << EFFECT_5);
                mask &= ~(1 << EFFECT_7);
                break;
        }
    }

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (!GetUnitOwner()->HasSpell(SPELL_WARLOCK_SOUL_LINK))
            amount = 0;
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_warl_grimoire_of_sacrifice::InitEffects);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_grimoire_of_sacrifice::CalculateAmount, EFFECT_6, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
    }
};

// 203781 - Server-side custom spell
class spell_warl_grimoire_of_sacrifice_health_fix : public AuraScript
{
    PrepareAuraScript(spell_warl_grimoire_of_sacrifice_health_fix);

    void Recalculate(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* effect = GetUnitOwner()->GetAuraEffect(SPELL_WARLOCK_GRIMOIRE_OF_SACRIFICE, EFFECT_6))
            effect->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_grimoire_of_sacrifice_health_fix::Recalculate, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_grimoire_of_sacrifice_health_fix::Recalculate, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108503 - Grimoire of Sacrifice
class spell_warl_grimoire_of_sacrifice_selector : public SpellScript
{
    PrepareSpellScript(spell_warl_grimoire_of_sacrifice_selector);

    void HandleHit(SpellEffIndex)
    {
        Player* warlock = GetCaster()->ToPlayer();
        if (!warlock)
            return;

        uint32 spell = 0;
        switch (GetHitUnit()->GetEntry())
        {
            case NPC_IMP:        spell = 132411; break;
            case NPC_VOIDWALKER: spell = 132413; break;
            case NPC_SUCCUBUS:   spell = 137706; break;
            case NPC_FELHUNTER:  spell = 132409; break;
            case NPC_FELGUARD:   spell = 132410; break;
            default:
                return;
        }
        
        GetHitUnit()->RemoveOwnedAura(SPELL_WARLOCK_OVERRIDE_COMMAND_DEMON);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_OVERRIDE_COMMAND_DEMON);
        warlock->CastSpell(warlock, SPELL_WARLOCK_OVERRIDE_COMMAND_DEMON, true);
        if (Aura* aura = warlock->GetAura(SPELL_WARLOCK_OVERRIDE_COMMAND_DEMON))
            if (AuraEffect* eff = aura->GetEffect(EFFECT_0))
                eff->ChangeAmount(spell);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_grimoire_of_sacrifice_selector::HandleHit, EFFECT_0, SPELL_EFFECT_INSTAKILL);
    }
};

// 114592 - Wild Imps
class spell_warl_wild_imp : public AuraScript
{
    PrepareAuraScript(spell_warl_wild_imp);

    void CalculatePeriodic(AuraEffect const*, bool&, int32& amplitude)
    {
        if (GetUnitOwner()->HasAura(SPELL_WARLOCK_GLYPH_OF_IMP_SWARM))
            amplitude += (4000 / GetUnitOwner()->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED));
    }

    void HandleTick(AuraEffect const*)
    {
        if (Player* player = GetOwner()->ToPlayer())
            if (player->HasAura(SPELL_WARLOCK_DISRUPTED_NETHER))
                PreventDefaultAction();
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_wild_imp::CalculatePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_wild_imp::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 123746 - Fel Energy
class spell_gen_fel_energy : public AuraScript
{
    PrepareAuraScript(spell_gen_fel_energy);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* pet = GetUnitOwner();
        pet->SetPowerType(POWER_ENERGY);
        pet->SetPower(POWER_ENERGY, pet->GetCreatePowers(POWER_ENERGY));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gen_fel_energy::HandleApply, EFFECT_0, SPELL_AURA_402, AURA_EFFECT_HANDLE_REAL);
    }
};

struct npc_warlock_guardian : public ScriptedAI
{
    npc_warlock_guardian(Creature* creature) : ScriptedAI(creature){ }

    void Reset() override
    {
        if (me->GetOwner() && me->GetOwner()->HasAura(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST) && !me->HasAura(SPELL_WARLOCK_MASTER_DEMONOLOGIST))
            me->CastSpell(me, SPELL_WARLOCK_MASTER_DEMONOLOGIST, true);
        if (!me->HasAura(SPELL_WARLOCK_PET_FEL_ENERGY))
            me->CastSpell(me, SPELL_WARLOCK_PET_FEL_ENERGY, true);
    }
};

// 55659 - Wild Imp, Summoned by 104317 - Wild Imp
struct npc_wild_imp : public npc_warlock_guardian
{
    uint32 remainedCasts = 10;
    bool toDespawn = false;

    AssistBehavior assist{ me };

    enum { SPELL_FEL_FIREBOLT = 104318 };

    npc_wild_imp(Creature *creature) : npc_warlock_guardian(creature)
    {
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void AttackStart(Unit* victim) override
    {
        AttackStartCaster(victim, 38.0f);
    }

    void UpdateAI(uint32 diff) override
    {
        if (toDespawn)
            return;

        if (!me->GetOwner() || !remainedCasts)
        {
            me->SetReactState(REACT_PASSIVE);
            me->StopMoving();
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            me->DespawnOrUnsummon(2000);
            toDespawn = true;
            return;
        }

        assist.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (Unit* victim = me->GetVictim())
        {
            AttackStart(victim);
            DoCast(victim, SPELL_FEL_FIREBOLT);
        }
    }

    void SpellCasted(Spell const* spell) override
    {
        if (spell->GetSpellInfo()->Id == SPELL_FEL_FIREBOLT)
            --remainedCasts;
    }
};

// Guardians summoned by Grimoire of Service (108501)
struct npc_grimoire_of_service_pet : public npc_warlock_guardian
{
    npc_grimoire_of_service_pet(Creature* me) : npc_warlock_guardian(me) { }

    bool needToCastFelstorm = false;
    bool needToSelectTarget = true;
    uint32 basicAttack = 0;

    void IsSummonedBy(Unit* owner) override
    {
        Reset();

        struct PetSpells
        {
            uint32 BasicAttack;
            uint32 SpecialAbility;
        };
        std::map<uint32, PetSpells> const map
        {
            { NPC_IMP,          { 3110,     89808 } },
            { NPC_VOIDWALKER,   { 3716,     17735 } },
            { NPC_FELHUNTER,    { 54049,    19647 } },
            { NPC_SUCCUBUS,     { 7814,     6358  } },
            { NPC_FELGUARD,     { 30213,    89766 } },
        };

        auto it = map.find(me->GetEntry());
        if (it != map.end())
        {
            basicAttack = it->second.BasicAttack;
            Unit* target = nullptr;
            if (me->GetEntry() == NPC_IMP)
                target = owner;
            if (Spell const* spell = owner->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                if (spell->m_targets.GetUnitTarget())
                {
                    AttackStart(spell->m_targets.GetUnitTarget());
                    if (me->GetEntry() != NPC_IMP)
                        target = spell->m_targets.GetUnitTarget();
                }
            }
            if (target)
            {
                me->CastSpell(target, it->second.SpecialAbility, false);
                if (me->GetEntry() == NPC_FELHUNTER)
                    me->CastSpell(target, 19505, true); // Devour Magic
            }
        }
        if (me->GetEntry() == NPC_FELGUARD)
            needToCastFelstorm = true;
    }

    void UpdateAI(uint32) override
    {
        if (needToSelectTarget)
        {
            if (Unit* owner = me->GetOwner())
                if (Unit* victim = ObjectAccessor::GetUnit(*me, owner->GetTarget()))
                    AttackStart(victim);
            needToSelectTarget = false;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (!UpdateVictim())
            return;

        if (needToCastFelstorm && me->GetVictim())
        {
            if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                me->CastSpell(me, 89751, true); // Felstorm
                needToCastFelstorm = false;
            }
        }

        DoMeleeAttackIfReady();
    }

    void AttackStart(Unit* victim) override
    {
        if (auto spellInfo = sSpellMgr->GetSpellInfo(basicAttack))
        {
            auto powerEntry = spellInfo->GetSpellPowerEntry(me);
            uint32 powerCost = powerEntry ? powerEntry->Cost : 0;
            uint32 powerType = powerEntry ? powerEntry->PowerType : 0;
            if (uint32(me->GetPower(Powers(powerType))) >= powerCost && !me->HasUnitState(UNIT_STATE_CASTING))
            {
                me->CastSpell(victim, basicAttack, false);
                if (me->HasUnitState(UNIT_STATE_CASTING))   // Imp.
                {
                    AttackStartCaster(victim, std::floor(spellInfo->GetMaxRange()));
                    return;
                }
            }
        }
        ScriptedAI::AttackStart(victim);
    }
};

// 11859 - Doomgaurd, 59000 - Terrorguard
struct npc_doomguard : public npc_warlock_guardian
{
    npc_doomguard(Creature* c) : npc_warlock_guardian(c) { }

    enum { SPELL_DOOMBOLT = 85692 };

    CasterMovement casterMovement = CasterMovement(me).Spell(SPELL_DOOMBOLT);

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        casterMovement.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoCastVictim(SPELL_DOOMBOLT);
    }

    void AttackStart(Unit* victim) override
    {
        if (me->Attack(victim, false))
            casterMovement.Chase(victim);
    }
};

// 89 - Infernal
// 58997 - Abyssal
// 70626 - Abyssal with The Codex of Xerrath
struct npc_warl_infernal : public npc_warlock_guardian
{
    npc_warl_infernal(Creature* c) : npc_warlock_guardian(c) { }

    bool defendWalock = false;

    bool SelectTarget()
    {
        if (defendWalock)
            return false;

        Unit* target = me->SelectNearestTarget(15.0f);
        if (target)
            AttackStart(target);
        else
            defendWalock = true;
        return target != nullptr;
    }

    void UpdateAI(uint32) override
    {
        if (!SelectTarget() && !UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// 18540 - Summon Doomguard, 112927 - Summon Terrorguard
class spell_warl_summon_doomguard : public SpellScript
{
    PrepareSpellScript(spell_warl_summon_doomguard);

    void HandleCast()
    {
        if (auto summonSpell = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[EFFECT_1].TriggerSpell))
        {
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, summonSpell->Effects[EFFECT_0].MiscValue);
            if (!summons.empty())
                summons.front()->AI()->AttackStart(GetExplTargetUnit());
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_summon_doomguard::HandleCast);
    }
};

// 85692 - Doom Bolt
class spell_warl_doombolt : public SpellScript
{
    PrepareSpellScript(spell_warl_doombolt);

    void HandleHit()
    {
        if (GetHitUnit()->HealthBelowPct(20))
            SetHitDamage(GetHitDamage() * 1.2f);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_doombolt::HandleHit);
    }
};

// 104330 - Server-side spell.
// Called by warlock's pet spells which generate demonic fury.
class spell_warl_demonic_fury_energize : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_fury_energize);

    void HandleEffect(SpellEffIndex effIndex)
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner)
            return;

        if (Player* player = owner->ToPlayer())
            if (player->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY)
                player->CastCustomSpell(SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE, SPELLVALUE_BASE_POINT1, GetSpellValue()->EffectBasePoints[effIndex], player, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_warl_demonic_fury_energize::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 122736 - Nonexistent spell
class spell_warl_demonic_fury_visual_controller : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_fury_visual_controller);

    void HandleCast()
    {
        // Demonic Fury visuals
        int32 val = GetCaster()->GetPower(POWER_DEMONIC_FURY);
        if (val >= 980)
        {
            if (!GetCaster()->HasAura(SPELL_WARLOCK_MAXIMUM_FURY))
                GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_MAXIMUM_FURY, true);
        }
        else if (val >= 500)
        {
            if (!GetCaster()->HasAura(SPELL_WARLOCK_MASTER_DEMONOLOGIST_VISUAL))
                GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_MASTER_DEMONOLOGIST_VISUAL, true);
        }
        else
        {
            GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_MAXIMUM_FURY);
            GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_MASTER_DEMONOLOGIST_VISUAL);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_demonic_fury_visual_controller::HandleCast);
    }
};

// 5857 - Hellfire
class spell_warl_hellfire : public SpellScript
{
    PrepareSpellScript(spell_warl_hellfire);

    size_t targetCount = 0;

    void CountTargets(std::list<WorldObject*>& list)
    {
        targetCount = list.size();
    }

    void HandleCast()
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_HELLFIRE);
        if (!spellInfo)
            return;

        float demonicfury = spellInfo->Effects[EFFECT_2].CalcValue(GetCaster());
        float perTarget = spellInfo->Effects[EFFECT_3].CalcValue(GetCaster());
        if (GetCaster()->HasAura(SPELL_WARLOCK_T15_4P_BONUS)) // Because int returned
            perTarget *= 1.1f;

        demonicfury += targetCount * perTarget;

        if (GetCaster()->GetShapeshiftForm() != FORM_METAMORPHOSIS)
            GetCaster()->CastCustomSpell(SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE, SPELLVALUE_BASE_POINT1, demonicfury, GetCaster(), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_hellfire::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        AfterCast += SpellCastFn(spell_warl_hellfire::HandleCast);
    }
};

// 77219 - Mastery: Master Demonologist
class spell_warl_mastery_master_demonologist : public AuraScript
{
    PrepareAuraScript(spell_warl_mastery_master_demonologist);

    void HandleApply(AuraEffect const* mastery, AuraEffectHandleModes)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        if (AuraEffect* metamorphosis = caster->GetAuraEffect(SPELL_WARLOCK_METAMORPHOSIS, EFFECT_1))
            metamorphosis->ChangeAmount(mastery->GetFloatAmount() * 3);
        if (AuraEffect* apotheosis = caster->GetAuraEffect(SPELL_WARLOCK_DARK_APOTHEOSIS, EFFECT_1))
            apotheosis->RecalculateAmount();
        if (AuraEffect* demonicFury = caster->GetAuraEffect(SPELL_WARLOCK_DEMONIC_FURY, EFFECT_1))
            demonicFury->ChangeAmount(mastery->GetFloatAmount());

        if (Pet* pet = caster->GetPet())
            if (AuraEffect* petBuff = pet->GetAuraEffect(SPELL_WARLOCK_MASTER_DEMONOLOGIST, EFFECT_0))
                petBuff->ChangeAmount(mastery->GetFloatAmount());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_mastery_master_demonologist::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// 115556 - Master Demonologist (Pet buff)
class spell_warl_master_demonologist : public AuraScript
{
    PrepareAuraScript(spell_warl_master_demonologist);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* target = GetTarget();

        target->m_Events.Schedule(1, [=]
        {
            Unit* owner = target->GetOwner();
            if (!owner)
                return;

            if (AuraEffect* petBuff = target->GetAuraEffect(SPELL_WARLOCK_MASTER_DEMONOLOGIST, EFFECT_0))
                if (AuraEffect const* mastery = owner->GetAuraEffect(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST, EFFECT_0))
                    petBuff->ChangeAmount(mastery->GetFloatAmount());
        });
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_master_demonologist::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 109145 - Demonic Fury
class spell_warl_demonic_fury : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_fury);

    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes)
    {
        Unit* owner = GetUnitOwner();
        if (AuraEffect const* mastery = owner->GetAuraEffect(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST, EFFECT_1))
            const_cast<AuraEffect*>(aurEff)->ChangeAmount(mastery->GetFloatAmount());
        owner->SetMaxPower(POWER_DEMONIC_FURY, owner->GetCreatePowers(POWER_DEMONIC_FURY));
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->SetMaxPower(POWER_DEMONIC_FURY, 0);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_fury::HandleApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_fury::HandleRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 103958 - Metamorphosis
class spell_warl_metamorphosis : public AuraScript
{
    PrepareAuraScript(spell_warl_metamorphosis);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();

        if (AuraEffect* demonicFury = caster->GetAuraEffect(SPELL_WARLOCK_DEMONIC_FURY, EFFECT_1))
            demonicFury->ChangeAmount(0);

        caster->m_Events.Schedule(1, [=]
        {
            if (AuraEffect* metamorph = caster->GetAuraEffect(SPELL_WARLOCK_METAMORPHOSIS, EFFECT_1))
                if (AuraEffect const* mastery = caster->GetAuraEffect(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST, EFFECT_0))
                    metamorph->ChangeAmount(mastery->GetFloatAmount() * 3);
        });
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (AuraEffect const* mastery = caster->GetAuraEffect(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST, EFFECT_0))
            if (AuraEffect* demonicFury = caster->GetAuraEffect(SPELL_WARLOCK_DEMONIC_FURY, EFFECT_1))
                demonicFury->ChangeAmount(mastery->GetFloatAmount());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_metamorphosis::HandleApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_metamorphosis::HandleRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 117198 - Soul Shards
class spell_warl_soul_shards : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_shards);

    void HandleAura(AuraEffect const*, AuraEffectHandleModes)
    {
        uint32 maxpower = GetAura()->IsRemoved() ? 0 : GetUnitOwner()->GetCreatePowers(POWER_SOUL_SHARDS);
        GetUnitOwner()->SetMaxPower(POWER_SOUL_SHARDS, maxpower);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_soul_shards::HandleAura, EFFECT_0, SPELL_AURA_TRIGGER_ON_POWER_LEVEL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_warl_soul_shards::HandleAura, EFFECT_0, SPELL_AURA_TRIGGER_ON_POWER_LEVEL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 17877 - Shadowburn, 116858 - Chaos Bolt 
class spell_warl_ember_consumer : public SpellScript
{
    PrepareSpellScript(spell_warl_ember_consumer);

    void HandleHit()
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), mastery->GetFloatAmount()));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_ember_consumer::HandleHit);
    }
};

// 114635 - Ember Tap
class spell_warl_ember_tap : public SpellScript
{
    PrepareSpellScript(spell_warl_ember_tap);

    void HandleEffect(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_EMBER_TAP))
        {
            PreventHitDefaultEffect(effIndex);
            return;
        }
    }

    void HandleHit()
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), mastery->GetFloatAmount()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_ember_tap::HandleEffect, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
        OnHit += SpellHitFn(spell_warl_ember_tap::HandleHit);
    }
};

// 114635 - Ember Tap
class spell_warl_glyph_of_ember_tap : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_ember_tap);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            AddPct(amount, mastery->GetFloatAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_glyph_of_ember_tap::CalculateAmount, EFFECT_2, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 104225 - Curse of the Elements, 108685 - Conflagrate, 108686 - Immolate, 109468 - Curse of Enfeeblement, 114654 - Incinerate
class spell_warl_fire_and_brimstone : public SpellScript
{
    PrepareSpellScript(spell_warl_fire_and_brimstone);

    void HandleCast()
    {
        if (GetCaster()->GetPower(POWER_BURNING_EMBERS) < 10)
            GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_FIRE_AND_BRIMSTONE);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_fire_and_brimstone::HandleCast);
    }
};

// 108685 - Conflagrate, 108686 - Immolate, 114654 - Incinerate 
class spell_warl_fire_and_brimstone_spell : public SpellScript
{
    PrepareSpellScript(spell_warl_fire_and_brimstone_spell);

    void HandleHit()
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            SetHitDamage(CalculatePct(GetHitDamage(), 35 * (100 + mastery->GetFloatAmount()) / 100));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_fire_and_brimstone_spell::HandleHit);
    }
};

// 108686 - Immolate
class spell_warl_fire_and_brimstone_immolate : public AuraScript
{
    PrepareAuraScript(spell_warl_fire_and_brimstone_immolate);

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            damage = CalculatePct(damage, 35 * (100  + mastery->GetFloatAmount()) / 100);
    }

    void Register() override
    {
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_warl_fire_and_brimstone_immolate::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 116858 - Chaos Bolt, 6353 - Soul Fire, 104027 - Soul Fire (Metamorphosis)
class spell_warl_always_cirt : public SpellScript
{
    PrepareSpellScript(spell_warl_always_cirt);

    float critMod = 0.0f;

    void CalculateCrit(SpellEffIndex)
    {
        float crit = GetCaster()->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + GetFirstSchoolInMask(SpellSchoolMask(GetSpellInfo()->SchoolMask)));
        crit += GetHitUnit()->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE, GetSpellInfo()->SchoolMask);
        crit += GetHitUnit()->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);
        critMod = std::max(crit, 0.0f);
    }

    void HandleHit()
    {
        float damage = GetHitDamage();
        AddPct(damage, critMod);
        SetHitDamage(damage);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_always_cirt::CalculateCrit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnHit += SpellHitFn(spell_warl_always_cirt::HandleHit);
    }
};

// 104316 - Imp Swarm
class spell_warl_imp_swarm : public SpellScript
{
    PrepareSpellScript(spell_warl_imp_swarm);

    SpellCastResult CheckSpec()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return SPELL_FAILED_DONT_REPORT;

        if (caster->GetTalentSpecialization() != SPEC_WARLOCK_DEMONOLOGY)
            return SPELL_FAILED_DONT_REPORT;

        return SPELL_CAST_OK;
    }

    void SummonImps(SpellEffIndex eff)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Unit* target = GetHitUnit())
            {
                for (int32 i = 0; i < GetSpellInfo()->Effects[eff].BasePoints; ++i)
                {
                    caster->AddAura(SPELL_WARLOCK_DEMONIC_CALL, caster);
                    caster->CastSpell(target, SPELL_WARLOCK_WILD_IMP_SUMMON, true);
                }
                caster->RemoveAura(SPELL_WARLOCK_DEMONIC_CALL);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_imp_swarm::CheckSpec);
        OnEffectHitTarget += SpellEffectFn(spell_warl_imp_swarm::SummonImps, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 104318 - Fel Firebolt (Wild Imp ability)
class spell_warl_fel_firebolt : public SpellScript
{
    PrepareSpellScript(spell_warl_fel_firebolt);

    void HandleHit()
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner)
            return;

        if (AuraEffect const* grimoire = owner->GetAuraEffect(SPELL_WARLOCK_GRIMOIRE_OF_SACRIFICE, EFFECT_3))
            SetHitDamage(GetHitDamage() * (1.0f + grimoire->GetFloatAmount() / 100.f));

        uint32 spellId = owner->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_MOLTEN_CORE_XERRATH : SPELL_WARLOCK_MOLTEN_CORE;
        if (owner->HasAura(SPELL_WARLOCK_MOLTEN_CORE_PASSIVE))
            if (roll_chance_i(8))
                owner->CastSpell(owner, spellId, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_fel_firebolt::HandleHit);
    }
};

// 47960 - Shadowflame 
class spell_warl_shadowflame : public AuraScript
{
    PrepareAuraScript(spell_warl_shadowflame);

    void HandleTick(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            uint32 spellId = GetCaster()->HasAura(SPELL_WARLOCK_THE_CODEX_OF_XERRATH) ? SPELL_WARLOCK_MOLTEN_CORE_XERRATH : SPELL_WARLOCK_MOLTEN_CORE;
            if (caster->HasAura(SPELL_WARLOCK_MOLTEN_CORE_PASSIVE))
                if (roll_chance_i(8))
                    caster->CastSpell(caster, spellId, true);

            caster->CastCustomSpell(SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE, SPELLVALUE_BASE_POINT1, 2, caster, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_shadowflame::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 122355, 140074 - Molten Core
class spell_warl_molten_core : public AuraScript
{
    PrepareAuraScript(spell_warl_molten_core);

    uint32 viusalRight = 0;

    void AddCharge(uint8& charges)
    {
        charges = std::min(10, GetCharges() + 1);
        if (charges > 1)
            GetUnitOwner()->CastSpell(GetUnitOwner(), viusalRight, true);
    }

    void ChargeRemoved(Spell*)
    {
        if (GetCharges() == 2)  // Before drop
            GetUnitOwner()->RemoveAurasDueToSpell(viusalRight);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(viusalRight);
    }

    void Register() override
    {
        viusalRight = m_scriptSpellId == SPELL_WARLOCK_MOLTEN_CORE ? SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT : SPELL_WARLOCK_MOLTEN_CORE_VISUAL_RIGHT_XERRATH;
        OnRefreshCharges += AuraRefreshChargesFn(spell_warl_molten_core::AddCharge);
        OnDropModCharge += AuraDropModChargeFn(spell_warl_molten_core::ChargeRemoved);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_molten_core::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 27243 Seed of Corruption (Level 90), 114790 - Seed of Corruption (Soulburn)
class spell_warl_seed_of_corruption : public AuraScript
{
    PrepareAuraScript(spell_warl_seed_of_corruption);

    void CalculateAmountForTooltip(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* damage = GetEffect(EFFECT_0))
            amount = damage->GetFloatAmount();
    }

    void CalculateDamageToExplode(AuraEffect const* effect, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            amount += caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL) * effect->GetSpellEffectInfo().ScalingMultiplier;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && eventInfo.GetActor()->GetGUID() == GetCasterGUID();
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        float damage = eventInfo.GetDamageInfo()->GetDamage();
        if (AuraEffect* seed = GetEffect(EFFECT_2))
        {
            if (damage >= seed->GetFloatAmount())
                Explode();
            else
                seed->SetFloatAmount(seed->GetFloatAmount() - damage);
        }
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
            Explode();
    }

    void Explode()
    {
        GetAura()->Remove();
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetUnitOwner(), GetId() == 114790 ? 87385 : 27285, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_seed_of_corruption::CalculateAmountForTooltip, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_seed_of_corruption::CalculateDamageToExplode, EFFECT_2, SPELL_AURA_DUMMY);
        DoCheckProc += AuraCheckProcFn(spell_warl_seed_of_corruption::CheckProc);
        OnProc += AuraProcFn(spell_warl_seed_of_corruption::HandleProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_seed_of_corruption::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 87385 - Seed of Corruption (damage, soulburn)
class spell_warl_seed_of_corruption_soulburn_explode : public SpellScript
{
    PrepareSpellScript(spell_warl_seed_of_corruption_soulburn_explode);

    void RegainSoulShard()
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_WARLOCK_SOULSHARD, true);
    }

    void HandleExplode(SpellEffIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARLOCK_CORRUPTION, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_seed_of_corruption_soulburn_explode::RegainSoulShard);
        OnEffectHitTarget += SpellEffectFn(spell_warl_seed_of_corruption_soulburn_explode::HandleExplode, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_warl_dot_instant_damage_trigger : public AuraScript
{
protected:
    uint32 m_damagePct = 0;

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        m_damagePct = eff->GetAmount();
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* target = GetTarget();
        float coeff = 0.0;
        if (AuraEffect* mastery = caster->GetAuraEffect(SPELL_WARLOCK_MASTERY_POTENT_AFFLICTION, EFFECT_0))
            coeff = mastery->GetAmount();

        std::multimap<uint32, int32> toBeCasted;
        for (auto&& eff : target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
        {
            if (eff->GetCasterGUID() != GetCasterGUID())
                continue;

            uint32 spellId = 0;
            switch (eff->GetId())
            {
                case SPELL_WARLOCK_AGONY:              spellId = SPELL_WARLOCK_AGONY_INSTANT;               break;
                case SPELL_WARLOCK_CORRUPTION_DOT:     spellId = SPELL_WARLOCK_CORRUPTION_INSTANT;          break;
                case SPELL_WARLOCK_UNSTABLE_AFFLICITON:spellId = SPELL_WARLOCK_UNSTABLE_AFFLICITON_INSTANT; break;
                case SPELL_WARLOCK_SEED_OF_CORRUPTION: spellId = SPELL_WARLOCK_SEED_OF_CORRUPTION_INSTANT;  break;
            }
            if (spellId)
                toBeCasted.emplace(spellId, eff->GetAmount());
        }

        for (auto&& it : toBeCasted)
        {
            float damage = CalculatePct(it.second, m_damagePct);
            damage = damage * 100.0f / (100.0 + coeff); // It will be increased by the mastery yet one time.
            caster->CastCustomSpell(it.first, SPELLVALUE_BASE_POINT0, damage, target, true);
        }
    }
};

// 103103 - Malefic Grasp 
class spell_warl_malefic_grasp : public spell_warl_dot_instant_damage_trigger
{
    PrepareAuraScript(spell_warl_malefic_grasp);

    void HandleTick(AuraEffect const* effect)
    {
        if (Unit* warlock = GetCaster())
            warlock->SendPlaySpellVisual(25955, GetTarget()->GetGUID(), 20.0f);
        spell_warl_dot_instant_damage_trigger::HandleTick(effect);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_malefic_grasp::HandleApply, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_malefic_grasp::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 1120 - Drain Soul 
class spell_warl_drain_soul : public spell_warl_dot_instant_damage_trigger
{
    PrepareAuraScript(spell_warl_drain_soul);

    uint32 hitCounter = 0;

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (GetTarget()->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
            damage *= 2;
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* warlock = GetCaster())
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                warlock->CastSpell(warlock, SPELL_WARLOCK_SOULSHARD_ENERGIZE, true);
    }

    void HandleTick(AuraEffect const* eff)
    {
        if (++hitCounter == 2)
        {
            hitCounter = 0;
            if (Unit* warlock = GetCaster())
                warlock->CastSpell(warlock, SPELL_WARLOCK_SOULSHARD, true);
        }
        if (GetTarget()->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
            spell_warl_dot_instant_damage_trigger::HandleTick(eff);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_drain_soul::HandleApply, EFFECT_4, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_warl_drain_soul::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectApplyFn(spell_warl_drain_soul::HandleRemove, EFFECT_4, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_soul::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 980 - Agony
class spell_warl_agony : public AuraScript
{
    PrepareAuraScript(spell_warl_agony);

    void ModifyStacks(AuraEffect const* eff)
    {
        int32 stacks = GetStackAmount();
        if (stacks < 10)
        {
            ++stacks;
            float amount = eff->GetFloatAmount() * stacks / GetStackAmount();
            float crit = eff->GetPeriodicCrit();
            SetStackAmount(stacks);
            GetEffect(eff->GetEffIndex())->ChangeAmount(amount, false);
            GetEffect(eff->GetEffIndex())->SetPeriodicCrit(crit);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_agony::ModifyStacks, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 131737 - Agony
class spell_warl_agony_instant_tick : public SpellScript
{
    PrepareSpellScript(spell_warl_agony_instant_tick);

    void HandleHit()
    {
        if (AuraEffect* agony = GetHitUnit()->GetAuraEffect(SPELL_WARLOCK_AGONY, EFFECT_0))
        {
            int32 stacks = agony->GetBase()->GetStackAmount();
            if (stacks < 10)
            {
                ++stacks;
                float amount = agony->GetFloatAmount() * stacks / agony->GetBase()->GetStackAmount();
                float crit = agony->GetPeriodicCrit();
                agony->GetBase()->SetStackAmount(stacks);
                agony->ChangeAmount(amount, false);
                agony->SetPeriodicCrit(crit);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_agony_instant_tick::HandleHit);
    }
};

// 86121 - Soul Swap
class spell_warl_soul_swap : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap);

    SpellCastResult CheckCast()
    {
        if (Unit* currentTarget = GetExplTargetUnit())
        {
            SpellInfo const* flagInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER);
            for (Unit::AuraApplicationMap::const_iterator itr = currentTarget->GetAppliedAuras().begin(); itr != currentTarget->GetAppliedAuras().end(); ++itr)
            {
                SpellInfo const* spellProto = itr->second->GetBase()->GetSpellInfo();
                if (itr->second->GetBase()->GetCaster() == GetCaster())
                    if (spellProto->SpellFamilyName == SPELLFAMILY_WARLOCK && (spellProto->SpellFamilyFlags & flagInfo->Effects[0].SpellClassMask))
                        return SPELL_CAST_OK;
            }
        }
        return SPELL_FAILED_BAD_TARGETS;
    }

    void HandleHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        // Soul Swap override spell
        caster->CastSpell(caster, SPELL_WARLOCK_SOUL_SWAP_AURA, true);
        target->CastSpell(caster, SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_soul_swap::CheckCast);
        OnHit += SpellHitFn(spell_warl_soul_swap::HandleHit);
    }
};

// 86211 - Soul Swap (aura on cater)
class spell_warl_soul_swap_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_swap_aura);

    std::vector<AuraData> auras;
    uint64 targetGuid = ObjectGuid::Empty;

    void CopyDots(AuraEffect const*, AuraEffectHandleModes)
    {
        SpellInfo const* affecting = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER);
        if (!affecting)
            return;

        Unit* warlock = GetUnitOwner();
        flag128 classMask = affecting->Effects[EFFECT_0].SpellClassMask;
        if (Spell* soulswap = warlock->GetCurrentSpell(CURRENT_GENERIC_SPELL))
        {
            if (Unit* target = soulswap->m_targets.GetUnitTarget())
            {
                targetGuid = target->GetGUID();
                for (auto&& it : target->GetAppliedAuras())
                {
                    SpellInfo const* spellInfo = it.second->GetBase()->GetSpellInfo();
                    if (it.second->GetBase()->GetCaster() == warlock &&  spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && (spellInfo->SpellFamilyFlags & classMask))
                    {
                        auras.emplace_back();
                        it.second->GetBase()->SaveParameters(auras.back());
                    }
                }
            }
        }
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id != GetSpellInfo()->Effects[EFFECT_0].BasePoints)
            return;

        if (!(eventInfo.GetHitMask() & PROC_EX_NORMAL_HIT))
            return;

        Unit* warlock = GetUnitOwner();
        if (Unit* target = eventInfo.GetActionTarget())
        {
            for (auto&& it : auras)
            {
                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(it.Id))
                {
                    if (Aura* aura = Aura::TryRefreshStackOrCreate(spellInfo, it.EffectMask, target, warlock, it.BaseAmount))
                    {
                        aura->LoadParameters(it);
                        aura->SetMaxDuration(aura->GetDuration());
                    }
                }
            }
        }
        Remove();
    }

public:
    uint64 Target() const { return targetGuid; }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_soul_swap_aura::CopyDots, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_warl_soul_swap_aura::HandleProc);
    }
};

// 86213 - Soul Swap Exhale 
class spell_warl_soul_swap_exhale : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap_exhale);

    SpellCastResult CheckCast()
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_WARLOCK_SOUL_SWAP_AURA))
            if (auto script = dynamic_cast<spell_warl_soul_swap_aura*>(aura->GetScriptByName("spell_warl_soul_swap_aura")))
                if (script->Target() != GetExplTargetUnit()->GetGUID())
                    return SPELL_CAST_OK;
        return SPELL_FAILED_BAD_TARGETS;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_soul_swap_exhale::CheckCast);
    }
};

// 119678 - Soul Swap (Soulburn)
class spell_warl_soul_swap_soulburn : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap_soulburn);

    void HandleHit()
    {
        if (GetHitResult() != SPELL_MISS_NONE)
            return;

        Unit* warlock = GetCaster();
        Unit* target = GetHitUnit();
        warlock->AddAura(SPELL_WARLOCK_CORRUPTION_DOT, target);
        warlock->AddAura(SPELL_WARLOCK_UNSTABLE_AFFLICTION, target);
        warlock->AddAura(SPELL_WARLOCK_AGONY, target);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_soul_swap_soulburn::HandleHit);
    }
};

// 114168 - Dark Apotheosis 
class spell_warl_dark_apotheosis : public AuraScript
{
    PrepareAuraScript(spell_warl_dark_apotheosis);

    void CalculateDamageTaken(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* mastery = GetUnitOwner()->GetAuraEffect(SPELL_WARLOCK_MASTERY_MASTER_DEMONOLOGIST, EFFECT_0))
            amount = -(10 + mastery->GetFloatAmount() / (mastery->GetFloatAmount() + 200) * 100.0f);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->GetPower(POWER_DEMONIC_FURY) > 200)
            GetUnitOwner()->SetPower(POWER_DEMONIC_FURY, 200);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_dark_apotheosis::CalculateDamageTaken, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_dark_apotheosis::HandleRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 103965 - Metamorphosis
class spell_warl_metamorphosis_overrides : public AuraScript
{
    PrepareAuraScript(spell_warl_metamorphosis_overrides);

    void InitEffects(uint32& effectMask)
    {
        Unit* warlock = GetUnitOwner();
        if (warlock->HasAura(SPELL_WARLOCK_DARK_APOTHEOSIS))
        {
            effectMask &= ~(1 << EFFECT_1);
            effectMask &= ~(1 << EFFECT_3);
            effectMask &= ~(1 << EFFECT_14);
        }
        else
        {
            effectMask &= ~(1 << EFFECT_7);
            effectMask &= ~(1 << EFFECT_8);
            if (!warlock->HasSpell(124917))
                effectMask &= ~(1 << EFFECT_3);
            if (!warlock->HasSpell(116208))
                effectMask &= ~(1 << EFFECT_6);
        }
    }

    void SelectShadowBoltOverride(AuraEffect const*, float& amount, bool&)
    {
        Unit* warlock = GetUnitOwner();
        if (warlock->HasAura(SPELL_WARLOCK_DARK_APOTHEOSIS))
            amount = SPELL_WARLOCK_DEMONIC_SLASH;
        else if (!GetUnitOwner()->HasSpell(112089))
            amount = 0;
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_warl_metamorphosis_overrides::InitEffects);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_metamorphosis_overrides::SelectShadowBoltOverride, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    }
};

// 114925 - Demonic Calling
class spell_warl_demonic_calling : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_calling);

    bool CheckProc(ProcEventInfo&)
    {
        return !GetUnitOwner()->HasAura(SPELL_WARLOCK_DISRUPTED_NETHER);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARLOCK_WILD_IMP_SUMMON, true);
        if (AuraEffect* timer = GetUnitOwner()->GetAuraEffect(SPELL_WARLOCK_WILD_IMPS, EFFECT_0))
            timer->CalculatePeriodic(timer->GetCaster(), true);
        Remove();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_demonic_calling::CheckProc);
        OnProc += AuraProcFn(spell_warl_demonic_calling::HandleProc);
    }
};

// 63303 - Glyph of Demon Hunting 
class spell_warl_glyph_of_demon_hunting : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_demon_hunting);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
        {
            warlock->LearnSpell(SPELL_WARLOCK_DARK_APOTHEOSIS, false);
            warlock->LearnSpell(SPELL_WARLOCK_GLYPH_OF_DEMON_HUNTING, false);
        }
    }

    void HandleRemove(AuraEffect const* /*eff*/, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
        {
            warlock->RemoveSpell(SPELL_WARLOCK_DARK_APOTHEOSIS);
            warlock->RemoveSpell(SPELL_WARLOCK_GLYPH_OF_DEMON_HUNTING);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_glyph_of_demon_hunting::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_glyph_of_demon_hunting::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 109151 - Demonic Leap
class spell_warl_demonic_leap : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_leap);

    SpellCastResult CheckElevation()
    {
        Unit* warlock = GetCaster();
        if (warlock->HasAuraType(SPELL_AURA_MOD_ROOT))
            return SPELL_FAILED_ROOTED;

        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        Unit* warlock = GetCaster();
        if (!warlock->HasAura(SPELL_WARLOCK_METAMORPHOSIS) && !warlock->HasAura(SPELL_WARLOCK_DARK_APOTHEOSIS))
            warlock->CastSpell(warlock, SPELL_WARLOCK_METAMORPHOSIS, true);
        warlock->CastSpell(warlock, SPELL_WARLOCK_DEMONIC_LEAP_JUMP, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_demonic_leap::CheckElevation);
        AfterCast += SpellCastFn(spell_warl_demonic_leap::HandleCast);
    }
};

// 109163 - Demonic Leap (jump)
class spell_warl_demonic_leap_jump : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_leap_jump);

    void HandleGlyph()
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_WARLOCK_GLYPH_OF_FALLING_METEOR) && caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
        {
            float z = caster->GetPositionZ();
            caster->UpdateAllowedPositionZ(caster->GetPositionX(), caster->GetPositionY(), z);
            if (caster->GetPositionZ() - z >= 14.57f)
                caster->CastSpell(caster, SPELL_WARLOCK_METEOR_SLAM, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_demonic_leap_jump::HandleGlyph);
    }
};

// 6262 - Healthstone
class spell_warl_healthstone : public SpellScript
{
    PrepareSpellScript(spell_warl_healthstone);

    void Prepare(SpellEffIndex eff)
    {
        Unit* warlock = GetCaster();
        if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_HEALTHSTONE))
            PreventEffect(eff);
        else
            SetEffectValue(warlock->CountPctFromMaxHealth(20));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_healthstone::Prepare, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 6262 - Helthstone (Glyphed)
class spell_warl_healthstone_glyphed : public AuraScript
{
    PrepareAuraScript(spell_warl_healthstone_glyphed);

    void CalculateHeal(AuraEffect const* eff, float& amount, bool&)
    {
        if (!eff->GetBase()->GetDuration())
            return;
        int32 total = GetCaster()->CountPctFromMaxHealth(amount);
        amount = total / eff->GetTotalTicks();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_healthstone_glyphed::CalculateHeal, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 48018, 104135 - Demonic Circle: Summon
class spell_warl_demonic_circle_summon : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_summon);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveGameObject(GetId(), true);
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
    }

    void HandleDummyTick(AuraEffect const*)
    {
        Unit* warlock = GetUnitOwner();
        if (GameObject* circle = GetTarget()->GetGameObject(GetId()))
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT);

            if (warlock->IsWithinDist(circle, spellInfo->GetMaxRange(true)))
            {
                if (!warlock->HasAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST))
                    warlock->CastSpell(warlock, SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST, true);
            }
            else
                warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_warl_demonic_circle_summon::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_circle_summon::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 48020, 104136 - Demonic Circle: Teleport, 114794 - Soulburn: Demonic Circle: Teleport
class spell_warl_demonic_circle_teleport : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_teleport);

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            player->CastSpell(player, SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT_ANIM, true);
            GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON);
            if (!circle)
                circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON_DEMO);
            if (circle)
            {
                player->RemoveMovementImpairingAuras();
                player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_circle_teleport::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 117896 - Backdraft
class spell_warl_backdraft_proc_driver : public AuraScript
{
    PrepareAuraScript(spell_warl_backdraft_proc_driver);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell() || !eventInfo.GetActionTarget())
            return false;

        // Don't trigger from Havoc target
        return eventInfo.TriggeredBySpell()->m_targets.GetUnitTargetGUID() == eventInfo.GetActionTarget()->GetGUID();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_backdraft_proc_driver::CheckProc);
    }
};

// 117828 - Backdraft
class spell_warl_backdraft : public AuraScript
{
    PrepareAuraScript(spell_warl_backdraft);

    void InitEffects(uint32& effectMask)
    {
        Unit* warlock = GetCaster();
        if (!warlock || !warlock->HasAura(SPELL_WARLOCK_PYROCLASM))
            effectMask &= ~(1 << EFFECT_2);
    }

    void RefreshCharges(uint8& charges)
    {
        charges = std::min(GetCharges() + GetSpellInfo()->ProcCharges, 6U);

        if (charges >= 3 && HasEffect(EFFECT_2) && GetEffect(EFFECT_2)->GetSpellModifier()->charges == -1)
        {
            GetEffect(EFFECT_2)->ApplySpellMod(GetCaster(), true);
            GetEffect(EFFECT_2)->GetSpellModifier()->charges = 1;
        }
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_warl_backdraft::InitEffects);
        OnRefreshCharges += AuraRefreshChargesFn(spell_warl_backdraft::RefreshCharges);
    }
};

// 29722 - Incinerate, 116858 - Chaos Bolt
class spell_warl_backdraft_consumer : public SpellScript
{
    PrepareSpellScript(spell_warl_backdraft_consumer);

    void ConsumeCharges()
    {
        Unit* warlock = GetCaster();
        if (Aura* aura = warlock->GetAura(SPELL_WARLOCK_BACKDRAFT))
        {
            auto it = GetSpell()->m_appliedMods.find(aura);
            if (it != GetSpell()->m_appliedMods.end())
            {
                GetSpell()->m_appliedMods.erase(it);
                if (GetSpellInfo()->Id != SPELL_WARLOCK_CHAOSBOLT && (warlock->HasAura(SPELL_WARLOCK_BACKLASH) || warlock->HasAura(SPELL_WARLOCK_BACKLASH_XERRATH)))
                    return;

                aura->ModCharges(GetSpellInfo()->Id == SPELL_WARLOCK_CHAOSBOLT ? -3 : -1);
                if (aura->GetCharges() < 3 && aura->HasEffect(EFFECT_2))
                {
                    aura->GetEffect(EFFECT_2)->ApplySpellMod(warlock, false);
                    aura->GetEffect(EFFECT_2)->GetSpellModifier()->charges = -1;
                }
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_backdraft_consumer::ConsumeCharges);
    }
};

// 20707 - Soulstone
class spell_warl_soulstone : public SpellScript
{
    PrepareSpellScript(spell_warl_soulstone);

    void SelectEffect(SpellEffIndex)
    {
        Unit* target = GetExplTargetUnit();
        PreventEffectForTarget(target, target->IsAlive() ? EFFECT_1 : EFFECT_0);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_soulstone::SelectEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 6203 - Soulstone
class spell_warl_soulstone_resurrect : public SpellScript
{
    PrepareSpellScript(spell_warl_soulstone_resurrect);

    void Resurrect()
    {
        Unit* target = GetHitUnit();
        if (!target->IsAlive())
            GetCaster()->CastSpell(target, SPELL_WARLOCK_SOULSTONE_RESURRECT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_soulstone_resurrect::Resurrect);
    }
};

// 80240 - Havoc
class spell_warl_havoc : public AuraScript
{
    PrepareAuraScript(spell_warl_havoc);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes mode)
    {
        if (mode & AURA_EFFECT_HANDLE_REAL)
            GetAura()->SetStackAmount(GetAura()->GetMaxStackAmount());
        GetAura()->GetEffect(EFFECT_1)->ChangeAmount(2); // fuck...
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_havoc::HandleApply, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};


// 348 - Immolate
// 1490 - Curse of the Elements
// 6789 - Mortal Coil
// 17877 - Shadowburn
// 17962 - Conflagrate
// 29722 - Incinerate
// 77799 - Fel Flame
// 109466 - Curse of Enfeeblement
// 116858 - Chaos Bolt
// 132409 - Spell Lock
class spell_warl_havoc_target_selector : public SpellScript
{
    PrepareSpellScript(spell_warl_havoc_target_selector);

    bool handled = false;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        AuraEffect* havoc = caster->GetAuraEffect(SPELL_WARLOCK_HAVOC, EFFECT_1);
        if (!havoc)
            return;

        targets.clear();

        if (GetSpellInfo()->Id == SPELL_WARLOCK_CHAOSBOLT && havoc->GetBase()->GetStackAmount() < 3)
            return;

        float range = GetSpellInfo()->GetMaxRange();
        std::list<Unit*> units;
        caster->GetAttackableUnitListInRange(units, range * 2);
        units.remove_if(Trinity::UnitAuraCheck{ false, SPELL_WARLOCK_HAVOC, caster->GetGUID() });
        units.remove_if([=](Unit* target) { return target == caster; });
        if (!units.empty())
        {
            if (caster->IsWithinDistInMap(units.front(), range) && caster->IsWithinLOSInMap(units.front()) && units.front() != GetExplTargetUnit())
            {
                targets.push_back(units.front());
                handled = true;
                return;
            }
        }
    }

    void HandleCast()
    {
        if (handled)
        {
            Aura* havoc = GetCaster()->GetAura(SPELL_WARLOCK_HAVOC);
            if (!havoc)
                return;
            if (GetSpellInfo()->Id == SPELL_WARLOCK_CHAOSBOLT)
                havoc->ModStackAmount(-3);
            else
                havoc->ModStackAmount(-1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_havoc_target_selector::SelectTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_havoc_target_selector::SelectTargets, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_havoc_target_selector::SelectTargets, EFFECT_2, TARGET_UNIT_TARGET_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_havoc_target_selector::SelectTargets, EFFECT_3, TARGET_UNIT_TARGET_ENEMY);
        AfterCast += SpellCastFn(spell_warl_havoc_target_selector::HandleCast);
    }
};


// 104232 - Rain of Fire
class spell_warl_rain_of_fire : public AuraScript
{
    PrepareAuraScript(spell_warl_rain_of_fire);

    void HandleTick(AuraEffect const* eff)
    {
        if (GetCaster())
        {
            std::list<DynamicObject*> list;
            GetCaster()->GetDynObjectList(list, GetId());
            if (list.empty())
                return;

            for (auto&& dynObj : list)
                GetCaster()->CastSpell(*dynObj, eff->GetSpellEffectInfo().TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_rain_of_fire::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 108446 - Soul Link
class spell_warl_soul_link : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_link);

    uint32 healAmount = 0;

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        Unit* warlock = GetUnitOwner()->GetOwner();
        if (!warlock)
            return;

        healAmount += CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), eff->GetAmount());
    }

    void HandleTicK(AuraEffect const* effect)
    {
        Unit* warlock = GetUnitOwner()->GetOwner();
        if (!warlock)
            return;

        if (healAmount)
        {
            CustomSpellValues spellvalues;
            spellvalues.AddSpellMod(SPELLVALUE_BASE_POINT0, healAmount);
            spellvalues.AddSpellMod(SPELLVALUE_BASE_POINT1, healAmount);
            warlock->CastCustomSpell(SPELL_WARLOCK_SOUL_LINK_HEAL, spellvalues, warlock, TRIGGERED_FULL_MASK, nullptr, effect);
            healAmount = 0;
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_soul_link::HandleProc, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_soul_link::HandleTicK, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 108370 - Soul Leech
class spell_warl_soul_leech : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_leech);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* warlock = GetUnitOwner()->ToPlayer();
        if (!warlock)
            return;
        uint32 absorbPct = warlock->GetTalentSpecialization() == SPEC_WARLOCK_AFFLICTION ? 20 : 10;
        int32 absorb = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), absorbPct);

        if (!absorb)
            return;

        // http://ru.wowhead.com/spell=108370#comments:id=1811378
        if (AuraEffect* soulLeech = warlock->GetAuraEffect(SPELL_WARLOCK_SOUL_LEECH_ABSORB, EFFECT_0))
            absorb += soulLeech->GetAmount();

        // It caps at 15% of your maximum HP.
        absorb = std::min(absorb, int32(warlock->CountPctFromMaxHealth(15)));
        warlock->CastCustomSpell(SPELL_WARLOCK_SOUL_LEECH_ABSORB, SPELLVALUE_BASE_POINT0, absorb, warlock, true);

        if (Pet* pet = warlock->GetPet())
        {
            // Your pet's shield caps at 15% of its maximum HP, not yours.
            absorb = std::min(absorb, int32(pet->CountPctFromMaxHealth(15)));
            warlock->CastCustomSpell(SPELL_WARLOCK_SOUL_LEECH_ABSORB, SPELLVALUE_BASE_POINT0, absorb, pet, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_soul_leech::HandleProc);
    }
};

// 119904 - Override Command Demon
class spell_warl_override_command_demon : public AuraScript
{
    PrepareAuraScript(spell_warl_override_command_demon);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (Pet* pet = caster->ToPet())
                amount = SelectSpell(pet->GetEntry());
    }
    int32 SelectSpell(uint32 petEntry)
    {
        std::map<uint32, int32> const map =
        {
            { NPC_IMP,          SPELL_WARLOCK_COMMAND_CAUTERIZE_MASTER },
            { NPC_VOIDWALKER,   SPELL_WARLOCK_COMMAND_DISARM           },
            { NPC_FELHUNTER,    SPELL_WARLOCK_COMMAND_SPELL_LOCK       },
            { NPC_SUCCUBUS,     SPELL_WARLOCK_COMMAND_WHIPLASH         },
            { NPC_FELGUARD,     SPELL_WARLOCK_COMMAND_FELSTORM         },
            { NPC_FEL_IMP,      SPELL_WARLOCK_COMMAND_CAUTERIZE_MASTER },
            { NPC_VOIDLORD,     SPELL_WARLOCK_COMMAND_DISARM           },
            { NPC_OBSERVER,     SPELL_WARLOCK_COMMAND_OPTICAL_BLAST    },
            { NPC_SHIVARRA,     SPELL_WARLOCK_COMMAND_FELLASH          },
            { NPC_WRATHGUARD,   SPELL_WARLOCK_COMMAND_WRATHSTORM       },
        };
        auto it = map.find(petEntry);
        return it != map.end() ? it->second : 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_override_command_demon::CalculateAmount, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    }
};

// Abilities from Override Command Demon
// 119905, 119907, 119909, 119910, 119911, 119913, 119914, 119915
class spell_warl_command_demon : public SpellScript
{
    PrepareSpellScript(spell_warl_command_demon);

    SpellCastResult CheckCast()
    {
        Player* warlock = GetCaster()->ToPlayer();
        if (!warlock)
            return SPELL_FAILED_DONT_REPORT;
        Pet* pet = warlock->GetPet();
        if (!pet)
            return SPELL_FAILED_NO_PET;

        auto spellInfo = sSpellMgr->GetSpellInfo(GetRealSpellId());
        if (!spellInfo)
            return SPELL_FAILED_DONT_REPORT;

        Spell* spell = new Spell(pet, spellInfo, TRIGGERED_NONE);
        spell->m_targets = GetSpell()->m_targets;
        auto result = spell->CheckPetCast(spell->m_targets.GetUnitTarget());
        if (result != SPELL_CAST_OK)
            delete spell;
        else
            spell->prepare(&spell->m_targets);
        return result;
    }

    uint32 GetRealSpellId()
    {
        auto spell = GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
        if (spell)  // Felstorm and Wrathstorm.
            return spell;

        spell = GetSpellInfo()->Effects[0].BasePoints;
        return spell ? spell : 119899;  // All others and Cauterize Master.
    }

    // Only for Felstorm and Wrathstorm, because SPELL_EFFECT_FORCE_CAST doesn't consume energy.
    void SuppressDefault(SpellEffIndex effIndex)
    {
        PreventDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_command_demon::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_warl_command_demon::SuppressDefault, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

// Pet abilities for Command Demon
// 6360, 19647, 89751, 115781, 115831, 118093, 119899, 115770
class spell_warl_command_demon_cooldown : public SpellScript
{
    PrepareSpellScript(spell_warl_command_demon_cooldown);

    void AddCooldown()
    {
        std::map<uint32, int32> const map =
        {
            { 119899,   SPELL_WARLOCK_COMMAND_CAUTERIZE_MASTER },
            { 118093,   SPELL_WARLOCK_COMMAND_DISARM           },
            { 19647,    SPELL_WARLOCK_COMMAND_SPELL_LOCK       },
            { 6360,     SPELL_WARLOCK_COMMAND_WHIPLASH         },
            { 89751,    SPELL_WARLOCK_COMMAND_FELSTORM         },
            { 115781,   SPELL_WARLOCK_COMMAND_OPTICAL_BLAST    },
            { 115770,   SPELL_WARLOCK_COMMAND_FELLASH          },
            { 115831,   SPELL_WARLOCK_COMMAND_WRATHSTORM       },
        };

        auto it = map.find(GetSpellInfo()->Id);
        if (it == map.end())
            return;

        if (Pet* pet = GetCaster()->ToPet())
            if (Player* warlock = pet->GetOwner())
                warlock->AddCustomCooldown(it->second, Milliseconds(pet->GetSpellHistory()->GetCooldownDelay(GetSpellInfo()->Id)));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_command_demon_cooldown::AddCooldown);
    }
};

// 6201 - Create Healthstone, 34130 - Create Healthstone
class spell_warl_create_healthstone : public SpellScript
{
    PrepareSpellScript(spell_warl_create_healthstone);

    void CreateOrUpdate(SpellEffIndex index)
    {
        Player* caster = GetCaster()->ToPlayer();
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CREATE_HEALTHSTONE);
        if (!caster || !spellInfo)
            return;
        uint32 spellId = caster->HasItemCount(spellInfo->Effects[index].ItemType, 1) ?
            SPELL_WARLOCK_CREATE_HEALTHSTONE_CHARGES : SPELL_WARLOCK_CREATE_HEALTHSTONE;
        caster->CastSpell(caster, spellId, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_create_healthstone::CreateOrUpdate, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 143381 - Hand of Gul'dan
class spell_warl_hand_of_guldan : public SpellScript
{
    PrepareSpellScript(spell_warl_hand_of_guldan);

    void HandleHit(SpellEffIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARLOCK_HAND_OF_GULDAN_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_hand_of_guldan::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 123194 - Hand of Gul'dan (Glyphed)
class spell_warl_hand_of_guldan_glyphed : public SpellScript
{
    PrepareSpellScript(spell_warl_hand_of_guldan_glyphed);

    void HandleCast()
    {
        // This must be not here.
        Position const* pos = GetExplTargetDest();
        GetCaster()->SendPlaySpellVisual(21982, pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), GetSpellInfo()->Speed);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_hand_of_guldan_glyphed::HandleCast);
    }
};

// 111771 - Demonic Gateway
class spell_warl_demonic_gateway : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_gateway);

    SpellCastResult CheckCast()
    {
        WorldLocation const* dest = GetExplTargetDest();
        if (!dest)
            return SPELL_FAILED_DONT_REPORT;
        PathGenerator p{ GetCaster() };
        p.CalculatePath(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ());
        if (p.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
            return SPELL_FAILED_NOPATH;
        auto& path = p.GetPath();
        uint32 size = path.size();
        if (size < 2)
            return SPELL_FAILED_NOPATH;
        float dist = 0.0f;
        for (uint32 i = 1; i < size; ++i)
        {
            auto& begin = path[i - 1];
            auto& end = path[i];

            Position begPos{ begin.x, begin.y, begin.z };
            Position endPos{ end.x, end.y, end.z };
            dist += begPos.GetExactDist(&endPos);
        }
        float distMax = GetCaster()->GetExactDist(dest);
        distMax *= std::abs(GetCaster()->GetPositionZ() - dest->GetPositionZ()) > 10.0f ? 1.1f : 1.3f;
        if (dist > distMax)
            return SPELL_FAILED_NOPATH;
        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        Unit* warlock = GetCaster();
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_DEMONIC_GATEWAY_AURA);
        warlock->m_Events.Schedule(10000, [=] { warlock->CastSpell(warlock, SPELL_WARLOCK_DEMONIC_GATEWAY_AURA, true); });
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_demonic_gateway::CheckCast);
        OnCast += SpellCastFn(spell_warl_demonic_gateway::HandleCast);
    }
};

// 113890 - Demonic Gateway (purple), 113886 - Demonic Gateway (green)
class spell_warl_demonic_gateway_summon : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_gateway_summon);

    void SelectDestPurple(SpellDestination& dest)
    {
        dest.Relocate(*GetCaster());
    }

    void SelectDestGreen(SpellDestination& dest)
    {
        Position pos = GetExplTargetDest()->GetPosition();
        pos.SetOrientation(GetCaster()->GetOrientation());
        dest.Relocate(pos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_warl_demonic_gateway_summon::SelectDestPurple, EFFECT_0, TARGET_UNK_125);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_warl_demonic_gateway_summon::SelectDestGreen,  EFFECT_0, TARGET_UNK_138);
    }
};

const uint32 PurpleVisual[5] = { 113915, 113916, 113917, 113918, 113919 };
const uint32 GreenVisual[5]  = { 113903, 113911, 113912, 113913, 113914 };

// 113901 - Demonic Gateway
class spell_warl_demonic_gateway_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_gateway_aura);

    void ApplyCharges(uint8 charges)
    {
        Unit* warlock = GetTarget();
        std::list<TempSummon*> summons;
        warlock->GetSummons(summons, NPC_GATEWAY_PURPLE);
        for (auto&& gateway : summons)
            gateway->CastSpell(gateway, PurpleVisual[charges - 1], true);
        summons.clear();
        warlock->GetSummons(summons, NPC_GATEWAY_GREEN);
        for (auto&& gateway : summons)
            gateway->CastSpell(gateway, GreenVisual[charges - 1], true);
        GetEffect(EFFECT_0)->SetAmount(charges);
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        ApplyCharges(1);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warlock = GetTarget();
        std::list<TempSummon*> summons;
        warlock->GetSummons(summons, NPC_GATEWAY_PURPLE);
        warlock->GetSummons(summons, NPC_GATEWAY_GREEN);
        for (auto&& it : summons)
            it->DespawnOrUnsummon();
    }

    void HandlePeriodic(AuraEffect const*)
    {
        if (GetCharges() < 5)
        {
            SetCharges(GetCharges() + 1);
            ApplyCharges(GetCharges());
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_gateway_aura::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_gateway_aura::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_gateway_aura::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59271, 59262
struct npc_demonic_gateway : public ScriptedAI
{
    npc_demonic_gateway(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void InitializeAI() override
    {
        scheduler.Schedule(Milliseconds(200), [this](TaskContext ctx)
        {
            if (Unit* warlock = me->GetOwner())
                if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_GATEWAY_ATTUNEMENT))
                    if (warlock->IsInDist(me, 2.0f))
                        DoTeleportIfCan(warlock);
            ctx.Repeat();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    void OnSpellClick(Unit* clicker, bool&) override
    {
        // Crowd control, check only for spell click
        if (clicker->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_PACIFIED | UNIT_FLAG_CONFUSED | UNIT_FLAG_FLEEING | UNIT_FLAG_STUNNED))
            return;
        if (clicker->HasAuraWithMechanic(1 << MECHANIC_POLYMORPH))
            return;

        DoTeleportIfCan(clicker);
    }

    void DoTeleportIfCan(Unit* clicker)
    {
        if (clicker->GetTypeId() != TYPEID_PLAYER)
            return;

        // Cooldown
        if (clicker->HasAura(SPELL_WARLOCK_DEMONIC_GATEWAY_COOLDOWN))
            return;

        // Battleground flags
        if (clicker->HasAura(23333) || clicker->HasAura(23335))
            return;

        Unit* owner = me->GetOwner();
        if (!owner || !clicker->IsInRaidWith(owner))
            return;
        auto aura = owner->GetAura(SPELL_WARLOCK_DEMONIC_GATEWAY_AURA);
        if (!aura || !aura->GetCharges())
            return;

        std::list<TempSummon*> list;
        owner->GetSummons(list, (me->GetEntry() == NPC_GATEWAY_PURPLE ? NPC_GATEWAY_GREEN : NPC_GATEWAY_PURPLE));
        if (list.empty())
            return;

        uint8 charges = aura->GetCharges();
        Unit* target = list.front();
        me->RemoveAurasDueToSpell(me->GetEntry() == NPC_GATEWAY_PURPLE ? PurpleVisual[charges - 1] : GreenVisual[charges - 1]);
        target->RemoveAurasDueToSpell(target->GetEntry() == NPC_GATEWAY_PURPLE ? PurpleVisual[charges - 1] : GreenVisual[charges - 1]);

        aura->SetCharges(charges - 1);
        aura->GetEffect(EFFECT_0)->SetAmount(charges - 1);
        clicker->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
        uint32 tele = me->GetEntry() == NPC_GATEWAY_PURPLE ? SPELL_WARLOCK_DEMONIC_GATEWAY_TELE_PURPLE : SPELL_WARLOCK_DEMONIC_GATEWAY_TELE_GREEN;
        clicker->m_Events.Schedule(1, [=] { clicker->CastSpell(target, tele, true); });

        if (AuraEffect* bonus = owner->GetAuraEffect(SPELL_WARLOCK_ITEM_WARLOCK_PVP_SET_4P_BONUS, EFFECT_0))
        {
            if (Aura* cooldown = clicker->GetAura(SPELL_WARLOCK_DEMONIC_GATEWAY_COOLDOWN))
            {
                cooldown->SetMaxDuration(cooldown->GetMaxDuration() - bonus->GetAmount() * IN_MILLISECONDS);
                cooldown->SetDuration(cooldown->GetMaxDuration());
            }
        }
    }

    void Unsummoned() override
    {
        if (me->GetOwner())
            me->GetOwner()->RemoveAurasDueToSpell(SPELL_WARLOCK_DEMONIC_GATEWAY_AURA);
    }
};

// 689 - Drain Life, 89420 - Drain Life (Soulburn), 103990 - Drain Life (Metamorphosis)
class spell_warl_drain_life : public AuraScript
{
    PrepareAuraScript(spell_warl_drain_life);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN);
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
        {
            Player* warlock = caster->ToPlayer();
            if (!warlock)
                return;

            int32 basepoints = warlock->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints);

            if (Aura const* harvest = warlock->GetAura(SPELL_WARLOCK_HARVEST_LIFE))
                AddPct(basepoints, harvest->GetSpellInfo()->Effects[EFFECT_1].BasePoints);
            if (AuraEffect const* glyph = warlock->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_DRAIN_LIFE, EFFECT_0))
                AddPct(basepoints, glyph->GetAmount());

            if (warlock->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY && warlock->GetShapeshiftForm() != FORM_METAMORPHOSIS)
            {
                int32 demonicfury = GetSpellInfo()->Effects[EFFECT_2].CalcValue(warlock);
                warlock->CastCustomSpell(SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE, SPELLVALUE_BASE_POINT1, demonicfury, warlock, true);
            }

            warlock->CastCustomSpell(SPELL_WARLOCK_DRAIN_LIFE_HEAL, SPELLVALUE_BASE_POINT0, basepoints, warlock, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_life::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        AfterEffectApply += AuraEffectApplyFn(spell_warl_drain_life::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 56218 - Glyph of Siphon Life
class spell_warl_glyph_of_siphon_life : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_siphon_life);

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARLOCK_SIPHON_LIFE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_glyph_of_siphon_life::HandleProc);
    }
};

// 63106 - Siphon Life
class spell_warl_glyph_of_siphon_life_heal : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_siphon_life_heal);

    void HealCorrection()
    {
        SetHitHeal(GetHitHeal() / 100); // 50% in base points, reduce it to 0.5%
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_glyph_of_siphon_life_heal::HealCorrection);
    }
};

// 116858 - Chaos Bolt (DoT)
class spell_warl_chaos_bolt_dot : public AuraScript
{
    PrepareAuraScript(spell_warl_chaos_bolt_dot);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect* mastery = GetCaster()->GetAuraEffect(SPELL_WARLOCK_MASTERT_EMBERSTORM, EFFECT_0))
            AddPct(amount, mastery->GetFloatAmount());

        float crit = GetCaster()->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + GetFirstSchoolInMask(SpellSchoolMask(GetSpellInfo()->SchoolMask)));
        crit += GetUnitOwner()->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE, GetSpellInfo()->SchoolMask);
        crit += GetUnitOwner()->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);
        crit = std::max(crit, 0.0f);
        AddPct(amount, crit);
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_chaos_bolt_dot::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 3110 - Firebolt
// 3716 - Torment
// 7814 - Lash of Pain
// 30213 - Legion Strike
// 54049 - Shadow Bite
// 115625 - Mortal Cleave
// 115746 - Felbolt
// 115748 - Bladedance
// 115778 - Tongue Lash
class spell_warl_pet_auto_cast : public SpellScript
{
    PrepareSpellScript(spell_warl_pet_auto_cast);

    SpellCastResult CheckAutoCast()
    {
        int32 min = 25;
        if (GetSpellInfo()->Id == 30213 || GetSpellInfo()->Id == 115625)
            min = 40;
        if (GetCaster()->GetPower(POWER_ENERGY) < GetSpell()->GetPowerCost() + min)
            return SPELL_FAILED_DONT_REPORT;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_pet_auto_cast::CheckAutoCast);
    }
};

// 17767 - Shadow Bulwark
class spell_warl_shadow_bulwark : public SpellScript
{
    PrepareSpellScript(spell_warl_shadow_bulwark);

    SpellCastResult CheckAutoCast()
    {
        if (GetCaster()->GetHealthPct() >= 20)
            return SPELL_FAILED_DONT_REPORT;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_shadow_bulwark::CheckAutoCast);
    }
};

// 17735 - Suffering
class spell_warl_suffering : public SpellScript
{
    PrepareSpellScript(spell_warl_suffering);

    SpellCastResult CheckAutoCast()
    {
        if (GetExplTargetUnit() && GetExplTargetUnit()->GetVictim() == GetCaster())
            return SPELL_FAILED_DONT_REPORT;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_suffering::CheckAutoCast);
    }
};

// 6358 - Seduction
// 115268 - Mesmerize
class spell_warl_seduction : public SpellScript
{
    PrepareSpellScript(spell_warl_seduction);

    SpellCastResult CheckAutoCast()
    {
        if (Unit* warlock = GetCaster()->GetOwner())
        {
            for (auto&& itr : warlock->GetAppliedAuras())
            {
                if (itr.second->IsPositive())
                    continue;

                Aura* aura = itr.second->GetBase();

                if (aura->HasEffectType(SPELL_AURA_MOD_FEAR) || aura->HasEffectType(SPELL_AURA_MOD_FEAR_2) ||
                    aura->HasEffectType(SPELL_AURA_MOD_CONFUSE) || aura->HasEffectType(SPELL_AURA_MOD_SILENCE) ||
                    aura->HasEffectType(SPELL_AURA_MOD_STUN))
                {
                    if (Unit* caster = aura->GetCaster())
                    {
                        GetSpell()->m_targets.SetUnitTarget(caster);
                        return SPELL_CAST_OK;
                    }
                }
            }
        }

        // Never cast to current target
        return SPELL_FAILED_DONT_REPORT;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_seduction::CheckAutoCast);
    }
};

// 115232 - Shadow Shield
// 115236 - Void Shield
class spell_warl_shadow_shield : public SpellScript
{
    PrepareSpellScript(spell_warl_shadow_shield);

    SpellCastResult CheckAutoCast()
    {
        // Cast handled in PetAI
        return SPELL_FAILED_DONT_REPORT;
    }

    void HandleHit()
    {
        if (Aura* aura = GetHitAura())
            aura->SetStackAmount(3);
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_shadow_shield::CheckAutoCast);
        AfterHit += SpellHitFn(spell_warl_shadow_shield::HandleHit);
    }
};

// 115232 - Shadow Shield
// 115236 - Void Shield
class spell_warl_shadow_shield_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_shadow_shield_aura);

    uint32 stack1 = 0;
    uint32 stack2 = 0;
    uint32 stack3 = 0;
    bool duringDropStacks = false;

    bool Load() override
    {
        stack1 = GetSpellInfo()->Id == 115232 ? 115245 : 115241;
        stack2 = GetSpellInfo()->Id == 115232 ? 115246 : 115242;
        stack3 = GetSpellInfo()->Id == 115232 ? 115247 : 115243;
        return true;
    }

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes modes)
    {
        if (!duringDropStacks)
        {
            HandleRemove(effect, modes);

            GetUnitOwner()->CastSpell(GetUnitOwner(), stack1, true);
            GetUnitOwner()->CastSpell(GetUnitOwner(), stack2, true);
            GetUnitOwner()->CastSpell(GetUnitOwner(), stack3, true);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(stack1);
        GetUnitOwner()->RemoveAurasDueToSpell(stack2);
        GetUnitOwner()->RemoveAurasDueToSpell(stack3);
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        duringDropStacks = true;
        uint32 oldStacks = GetAura()->GetStackAmount();
        GetAura()->ModStackAmount(-1);
        duringDropStacks = false;

        if (oldStacks == 3)
            GetUnitOwner()->RemoveAurasDueToSpell(stack3);
        else if (oldStacks == 2)
            GetUnitOwner()->RemoveAurasDueToSpell(stack2);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_shadow_shield_aura::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectApplyFn(spell_warl_shadow_shield_aura::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_warl_shadow_shield_aura::HandleProc);
    }
};

// 103964 - Touch of Chaos
class spell_warl_touch_of_chaos : public SpellScript
{
    PrepareSpellScript(spell_warl_touch_of_chaos);

    void HandleHit(SpellEffIndex)
    {
        if (Aura* corrution = GetHitUnit()->GetAura(SPELL_WARLOCK_CORRUPTION_DOT, GetCaster()->GetGUID()))
        {
            int32 tick = corrution->GetEffect(EFFECT_0)->GetAmplitude();
            int32 max = corrution->GetCalculatedMaxDuration() * 1.5;
            int32 newDuration = std::min(corrution->GetDuration() + 6000, max);
            corrution->SetMaxDuration(newDuration);
            corrution->SetDuration(corrution->GetMaxDuration());
            corrution->RecalculateAmountOfEffects();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_touch_of_chaos::HandleHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 112891 - Server-side spell
class spell_warl_howl_of_terror_cooldown : public AuraScript
{
    PrepareAuraScript(spell_warl_howl_of_terror_cooldown);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
            warlock->ModifySpellCooldown(SPELL_WARLOCK_HOWL_OF_TERROR, -1 * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_howl_of_terror_cooldown::HandleProc);
    }
};

// 30213 - Legion Strike
// 115625 - Mortal Cleave
class spell_warl_legion_strike : public SpellScript
{
    PrepareSpellScript(spell_warl_legion_strike);

    uint32 targetsCount = 1;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targetsCount = targets.size();
    }

    void HandleHit()
    {
        if (targetsCount)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_legion_strike::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_warl_legion_strike::HandleHit);
    }
};

// 19505 - Devour Magic
// 115284 - Clone Magic
class spell_warl_devour_magic : public SpellScript
{
    PrepareSpellScript(spell_warl_devour_magic);

    void HandleHit()
    {
        if (GetDispelCount())
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_DEVOUR_MAGIC_HEAL_ENERGIZE, true);
            if (Unit* warlock = GetCaster()->GetOwner())
                if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_DEMON_TRAINING))
                    GetCaster()->CastSpell(warlock, SPELL_WARLOCK_DEVOUR_MAGIC_HEAL_ENERGIZE, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_devour_magic::HandleHit);
    }
};

// 19658 - Devour Magic
class spell_warl_devour_magic_heal : public SpellScript
{
    PrepareSpellScript(spell_warl_devour_magic_heal);

    void CalculateAmount(SpellEffIndex)
    {
        if (GetCaster()->GetEntry() == NPC_OBSERVER)
        {
            if (Unit* owner = GetCaster()->GetOwner())
            {
                int32 spd = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                SetEffectValue(GetEffectValue() + spd * 0.15f);
            }
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_devour_magic_heal::CalculateAmount, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 56249 - Glyph of Demon Training
class spell_warl_glyph_of_demon_training : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_demon_training);

public:
    uint32 bolts = 3;

    void Register() override { }
};

// 6358 - Seduction
// 115268 - Mesmerize
class spell_warl_glyph_of_demon_training_seduction : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_demon_training_seduction);

    void HandleHit()
    {
        if (Unit* warlock = GetCaster()->GetOwner())
        {
            Unit* target = GetHitUnit();
            if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_DEMON_TRAINING))
            {
                target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, 0, target->GetAura(32409)); // SW:D shall not be removed.
                target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_glyph_of_demon_training_seduction::HandleHit);
    }
};

// 3110 - Firebolt
// 115746 - Felbolt
class spell_warl_glyph_of_demon_trainig_firebolt : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_demon_trainig_firebolt);

    uint32 boltsleft = 3;

    SpellCastResult CheckCast()
    {
        if (Unit* warlock = GetCaster()->GetOwner())
        {
            if (Aura* aura = warlock->GetAura(SPELL_WARLOCK_GLYPH_OF_DEMON_TRAINING))
            {
                if (auto script = dynamic_cast<spell_warl_glyph_of_demon_training*>(aura->GetScriptByName("spell_warl_glyph_of_demon_training")))
                {
                    if (script->bolts == 3 && GetCaster()->GetPower(POWER_ENERGY) < GetSpell()->GetPowerCost() * 3 + 10)
                        return SPELL_FAILED_DONT_REPORT;

                    --script->bolts;
                    if (!script->bolts)
                        script->bolts = 3;
                }
            }
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_warl_glyph_of_demon_trainig_firebolt::CheckCast);
    }
};

// 103967 - Carrion Swarm
class spell_warl_glyph_of_carrion_swarm : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_carrion_swarm);

    void HandleHit(SpellEffIndex index)
    {
        if (GetCaster()->HasAura(GetSpellInfo()->Effects[index].TriggerSpell))
            PreventDefaultEffect(index);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_glyph_of_carrion_swarm::HandleHit, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 5697 - Unending Breath
class spell_warl_unending_breath_soulburn : public SpellScript
{
    PrepareSpellScript(spell_warl_unending_breath_soulburn);

    void HandleCast()
    {
        if (Aura* soulburn = GetCaster()->GetAura(SPELL_WARLOCK_SOULBURN))
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_UNENDING_BREATH_WATERWALK, true);
            GetCaster()->RemoveAura(soulburn);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_unending_breath_soulburn::HandleCast);
    }
};

// 117197 nonexistent spell
class spell_warl_soul_shards_visual_test : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_shards_visual_test);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleCast()
    {
        if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_WARLOCK_DEMONOLOGY)
            return;

        int32 shards;
        if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_WARLOCK_AFFLICTION)
            shards = int32(std::floor(GetCaster()->GetPower(POWER_SOUL_SHARDS) / 100));
        else
            shards = int32(std::floor(GetCaster()->GetPower(POWER_BURNING_EMBERS) / 10));

        bool spheres = GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_VERDANT_SPHERES);

        uint32 spell_1_3    = spheres ? SPELL_WARLOCK_VERDANT_SHPERES_1_3   : SPELL_WARLOCK_SOULSHARDS_1_3;
        uint32 spell_2_3_4  = spheres ? SPELL_WARLOCK_VERDANT_SHPERES_2_3_4 : SPELL_WARLOCK_SOULSHARDS_2_3_4;
        uint32 spell_4      = spheres ? SPELL_WARLOCK_VERDANT_SHPERES_4     : SPELL_WARLOCK_SOULSHARDS_4;
        
        switch (shards)
        {
            case 0:
                GetCaster()->RemoveAurasDueToSpell(spell_1_3);
                GetCaster()->RemoveAurasDueToSpell(spell_2_3_4);
                GetCaster()->RemoveAurasDueToSpell(spell_4);
                break;
            case 1:
                if (!GetCaster()->HasAura(spell_1_3))
                    GetCaster()->CastSpell(GetCaster(), spell_1_3);

                GetCaster()->RemoveAurasDueToSpell(spell_2_3_4);
                GetCaster()->RemoveAurasDueToSpell(spell_4);
                break;
            case 2:
                if (!GetCaster()->HasAura(spell_2_3_4))
                    GetCaster()->CastSpell(GetCaster(), spell_2_3_4);

                GetCaster()->RemoveAurasDueToSpell(spell_1_3);
                GetCaster()->RemoveAurasDueToSpell(spell_4);
                break;
            case 3:
                if (!GetCaster()->HasAura(spell_1_3))
                    GetCaster()->CastSpell(GetCaster(), spell_1_3);
                if (!GetCaster()->HasAura(spell_2_3_4))
                    GetCaster()->CastSpell(GetCaster(), spell_2_3_4);

                GetCaster()->RemoveAurasDueToSpell(spell_4);
                break;
            case 4:
                if (!GetCaster()->HasAura(spell_2_3_4))
                    GetCaster()->CastSpell(GetCaster(), spell_2_3_4);
                if (!GetCaster()->HasAura(spell_4))
                    GetCaster()->CastSpell(GetCaster(), spell_4);

                GetCaster()->RemoveAurasDueToSpell(spell_1_3);
                break;
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_soul_shards_visual_test::HandleCast);
    }
};

// 116860 - Ember Test
class spell_warl_ember_test : public SpellScript
{
    PrepareSpellScript(spell_warl_ember_test);

    void HandleCast()
    {
        if (GetCaster()->ToPlayer()->GetSpecialization() != SPEC_WARLOCK_DESTRUCTION)
            return;

        int32 embers = int32(std::floor(GetCaster()->GetPower(POWER_BURNING_EMBERS) / 10));
        if (embers >= 3)
        {
            if (!GetCaster()->HasAura(SPELL_WARLOCK_SEARING_EMBERS_2))
                GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_SEARING_EMBERS_2, true);
        }
        else
        {
            if (embers >= 2)
            {
                if (!GetCaster()->HasAura(SPELL_WARLOCK_SEARING_EMBERS_1))
                    GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_SEARING_EMBERS_1, true);
                GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SEARING_EMBERS_2);
            }
            else
            {
                GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SEARING_EMBERS_1);
                GetCaster()->RemoveAurasDueToSpell(SPELL_WARLOCK_SEARING_EMBERS_2);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_ember_test::HandleCast);
    }
};

static void HandleGlyphOfFelguard(Player const* warlock, TempSummon* guard)
{
    std::list<uint32> items;
    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* item = warlock->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            if (item->GetTemplate()->Class == ITEM_CLASS_WEAPON && (
                item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 || item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2 || (
                item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM && guard->GetEntry() == NPC_WRATHGUARD)))
                    items.push_back(item->GetEntry());

    if (items.empty())
        return;

    if (guard->GetEntry() == NPC_WRATHGUARD)
    {
        auto itr = items.begin();
        guard->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, *itr);
        if (items.size() > 1)
            guard->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, *(++itr));
    }
    else
    {
        uint32 item = Trinity::Containers::SelectRandomContainerElement(items);
        guard->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, item);
    }
}

// 30146 - Summon Felguard
// 112870 - Summon Wrathguard
// 111898 - Grimoire: Felguard
class spell_warl_glyph_of_felguard : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_felguard);

    void HandleCast()
    {
        Player* warlock = GetCaster()->ToPlayer();
        if (!warlock || !warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_FELGUARD))
            return;

        TempSummon* guard = warlock->GetPet();
        if (GetSpellInfo()->Effects[EFFECT_0].Effect == SPELL_EFFECT_SUMMON_PET)
        {
            guard = warlock->GetPet();
        }
        else
        {
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, GetSpellInfo()->Effects[EFFECT_1].MiscValue);
            if (!summons.empty())
                guard = summons.back();
        }

        if (guard)
            HandleGlyphOfFelguard(warlock, guard);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_warl_glyph_of_felguard::HandleCast);
    }
};

// 6962 - Pet Summoned
class spell_warl_glyph_of_felguard_loading : public SpellScript
{
    PrepareSpellScript(spell_warl_glyph_of_felguard_loading);

    void HandleCast()
    {
        if (Player* warlock = GetCaster()->ToPlayer())
            if (warlock->HasAura(SPELL_WARLOCK_GLYPH_OF_FELGUARD))
                if (Pet* pet = warlock->GetPet())
                    if (pet->GetEntry() == NPC_FELGUARD || pet->GetEntry() == NPC_WRATHGUARD)
                        HandleGlyphOfFelguard(warlock, pet);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_glyph_of_felguard_loading::HandleCast);
    }
};

// 56241 - Glyph of Verdant Spheres
class spell_warl_glyph_of_verdant_spheres : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_verdant_spheres);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warlock = GetUnitOwner();
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULSHARDS_1_3);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULSHARDS_2_3_4);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULSHARDS_4);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SEARING_EMBERS_1);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_SEARING_EMBERS_2);

        warlock->CastSpell(warlock, SPELL_WARLOCK_SHARDS_VISUAL_TEST, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warlock = GetUnitOwner();

        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_VERDANT_SHPERES_1_3);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_VERDANT_SHPERES_2_3_4);
        warlock->RemoveAurasDueToSpell(SPELL_WARLOCK_VERDANT_SHPERES_4);

        if (warlock->ToPlayer()->GetSpecialization() == SPEC_WARLOCK_AFFLICTION)
            warlock->CastSpell(warlock, SPELL_WARLOCK_SHARDS_VISUAL_TEST, true);
        else if (warlock->ToPlayer()->GetSpecialization() == SPEC_WARLOCK_DESTRUCTION)
            warlock->CastSpell(warlock, SPELL_WARLOCK_EMBER_TEST, true);
    }


    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_warl_glyph_of_verdant_spheres::HandleApply,  EFFECT_1, SPELL_AURA_TRIGGER_ON_POWER_LEVEL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_glyph_of_verdant_spheres::HandleRemove, EFFECT_1, SPELL_AURA_TRIGGER_ON_POWER_LEVEL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 58070 - Glyph of Soul Consumption
class spell_warl_glyph_of_soul_consumption : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_soul_consumption);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetActionTarget())
            return false;

        if (eventInfo.GetDamage() < eventInfo.GetActionTarget()->GetHealth())
            return false;

        if (!GetOwner()->ToPlayer()->IsHonorOrXPTarget(eventInfo.GetActionTarget()))
            return false;

        switch (GetOwner()->ToPlayer()->GetTalentSpecialization())
        {
            case SPEC_WARLOCK_AFFLICTION:
                return GetUnitOwner()->FindCurrentSpellBySpellId(SPELL_WARLOCK_DRAIN_SOUL) != nullptr;
            case SPEC_WARLOCK_DEMONOLOGY:
                return GetUnitOwner()->GetShapeshiftForm() == FORM_METAMORPHOSIS
                    || GetUnitOwner()->HasAura(SPELL_WARLOCK_DARK_APOTHEOSIS);
            case SPEC_WARLOCK_DESTRUCTION:
                return eventInfo.GetSpellInfo()->Id == SPELL_WARLOCK_CHAOSBOLT;
        }
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARLOCK_GLYPH_OF_SOUL_CONSUMPTION, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_glyph_of_soul_consumption::CheckProc);
        OnProc += AuraProcFn(spell_warl_glyph_of_soul_consumption::HandleProc);
    }
};

// 108505 - Archimonde's Darkness
class spell_warl_archimondes_darkness : public AuraScript
{
    PrepareAuraScript(spell_warl_archimondes_darkness);

    uint32 const spells[3] = { 113858, 113860, 113861 };

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        for (auto&& spell : spells)
        {
            // TODO: Send charges here
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* warlock = GetUnitOwner()->ToPlayer())
        {
            for (auto&& spell : spells)
            {
                auto cooldown = warlock->GetSpellHistory()->GetChargeCooldown(spell);
                if (cooldown != TimeValue::zero())
                {
                    warlock->RemoveSpellCooldown(spell);
                    warlock->AddCustomCooldown(spell, cooldown);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_archimondes_darkness::HandleApply, EFFECT_0, SPELL_AURA_MOD_CHARGES, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_archimondes_darkness::HandleRemove, EFFECT_0, SPELL_AURA_MOD_CHARGES, AURA_EFFECT_HANDLE_REAL);
    }
};

// 145072 - Item - Warlock T16 2P Bonus
class spell_warl_t16_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warl_t16_2p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* warlock = GetUnitOwner();
        int32 chance = 0.0f;
        uint32 spellId = 0;

        switch (eventInfo.GetSpellInfo()->Id)
        {
            case SPELL_WARLOCK_CONFLAGRATE:
            case SPELL_WARLOCK_CONFLAGRATE_FIRE_AND_BRIMSTONE:
                if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                {
                    chance = GetSpellInfo()->Effects[EFFECT_3].CalcValue(warlock);
                    spellId = SPELL_WARLOCK_DESTRUCTIVE_INFLUENCE;
                }
                break;
            case SPELL_WARLOCK_UNSTABLE_AFFLICITON:
                if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                {
                    chance = GetSpellInfo()->Effects[EFFECT_1].CalcValue(warlock);
                    spellId = SPELL_WARLOCK_EMPOWERED_GRASP;
                }
                break;
            case SPELL_WARLOCK_SOUL_FIRE:
            case SPELL_WARLOCK_SOUL_FIRE_DEMON:
            {
                chance = GetSpellInfo()->Effects[EFFECT_3].CalcValue(warlock);
                spellId = SPELL_WARLOCK_FIERY_WRATH;
                break;
            }
        }
        if (spellId && roll_chance_i(chance))
            warlock->CastSpell(warlock, spellId, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_t16_2p_bonus::HandleProc);
    }
};

// 145091 - Item - Warlock T16 4P Bonus
class spell_warl_t16_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warl_t16_4p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_WARLOCK_HAND_OF_GULDAN_T16, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_t16_4p_bonus::HandleProc);
    }
};

// 48181 - Haunt
class spell_warl_haunt_t16 : public AuraScript
{
    PrepareAuraScript(spell_warl_haunt_t16);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* warlock = GetCaster())
            if (AuraEffect const* bonus = warlock->GetAuraEffect(SPELL_WARLOCK_T16_4P_BONUS, EFFECT_0))
                if (roll_chance_i(bonus->GetAmount()))
                    warlock->CastSpell(warlock, SPELL_WARLOCK_DARK_REFUND, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_haunt_t16::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 114168 - Dark Apotheosis
class spell_warl_fury_ward_fix : public AuraScript
{
    PrepareAuraScript(spell_warl_fury_ward_fix);

    void HandleBonus(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* effect = GetUnitOwner()->GetAuraEffect(SPELL_WARLOCK_PVP_SET_2P_BONUS, EFFECT_0))
            effect->RecalculateAmount();
        if (AuraEffect* effect = GetUnitOwner()->GetAuraEffect(SPELL_WARLOCK_PVP_SET_2P_BONUS, EFFECT_1))
            effect->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectRemoveFn(spell_warl_fury_ward_fix::HandleBonus, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_fury_ward_fix::HandleBonus, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 131632 - Item - Warlock PvP Set 2P Bonus
class spell_warl_twilight_ward_fix : public AuraScript
{
    PrepareAuraScript(spell_warl_twilight_ward_fix);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (GetUnitOwner()->HasAura(SPELL_WARLOCK_DARK_APOTHEOSIS))
            amount = 0.0f;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_twilight_ward_fix::CalculateAmount, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_twilight_ward_fix::CalculateAmount, EFFECT_1, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    }
};

// 124315 - Meteor Slam
class spell_warl_meteor_slam : public AuraScript
{
    PrepareAuraScript(spell_warl_meteor_slam);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
            if (Player* warlock = GetUnitOwner()->ToPlayer())
                warlock->SetFallInformation(0, warlock->GetPositionZ());
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_meteor_slam::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 146739 - Corruption
class spell_warl_corruption : public AuraScript
{
    PrepareAuraScript(spell_warl_corruption);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* warlock = GetCaster())
            warlock->BindAura(GetAura());
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* caster = GetCaster();
        if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
            return;

        Player* warlock = caster->ToPlayer();
        if (warlock->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY) // Generate 4 demonic fury each tick.
            warlock->CastCustomSpell(SPELL_WARLOCK_DEMONIC_FURY_ENERGIZE, SPELLVALUE_BASE_POINT1, 4, warlock, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_corruption::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_corruption::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 108558 - Nightfall
class spell_warl_nightfall : public AuraScript
{
    PrepareAuraScript(spell_warl_nightfall);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.TriggeredByPeriodic())
            if (auto list = GetUnitOwner()->GetBoundAurasBySpellId(eventInfo.TriggeredByPeriodic()->GetId()))
                if (eventInfo.TriggeredByPeriodic() && eventInfo.TriggeredByPeriodic()->GetBase() == list->back() && roll_chance_i(10))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARLOCK_SHADOW_TRANCE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warl_nightfall::HandleProc);
    }
};

void AddSC_warlock_spell_scripts()
{
    new aura_script<spell_warl_health_funnel>("spell_warl_health_funnel");
    new spell_script<spell_warl_health_funnel_soulburn>("spell_warl_health_funnel_soulburn");
    new spell_warl_soulburn_seed_of_corruption_damage();
    new spell_script<spell_warl_soulburn_seed_of_corruption>("spell_warl_soulburn_seed_of_corruption");
    new aura_script<spell_warl_soulburn_override>("spell_warl_soulburn_override");
    new spell_script<spell_soulburn_consumer>("spell_soulburn_consumer");
    new spell_script<spell_warl_curse_of_exhaustion_soulburn>("spell_warl_curse_of_exhaustion_soulburn");
    new spell_warl_glyph_of_soulwell();
    new spell_warl_glyph_of_imp_swarm();
    new spell_script<spell_warl_unbound_will>("spell_warl_unbound_will");
    new spell_warl_kil_jaedens_cunning();
    new spell_warl_shield_of_shadow();
    new spell_script<spell_warl_flames_of_xoroth>("spell_warl_flames_of_xoroth");
    new spell_warl_soul_link();
    new spell_script<spell_warl_decimate>("spell_warl_decimate");
    new spell_script<spell_warl_chaos_wave>("spell_warl_chaos_wave");
    new aura_script<spell_warl_dark_bargain_on_absorb>("spell_warl_dark_bargain_on_absorb");
    new aura_script<spell_warl_dark_regeneration>("spell_warl_dark_regeneration");
    new spell_warl_sacrificial_pact();
    new spell_warl_twilight_ward_s12();
    new aura_script<spell_warl_burning_rush>("spell_warl_burning_rush");
    new spell_script<spell_warl_conflagrate_aura>("spell_warl_conflagrate_aura");
    new aura_script<spell_warl_shadowburn>("spell_warl_shadowburn");
    new aura_script<spell_warl_burning_embers>("spell_warl_burning_embers");
    new spell_warl_soul_harverst();
    new spell_script<spell_warl_life_tap>("spell_warl_life_tap");
    new aura_script<spell_warl_life_tap_heal_absorb>("spell_warl_life_tap_heal_absorb");
    new spell_script<spell_warl_fear>("spell_warl_fear");
    new spell_warl_banish();
    new spell_warl_soulshatter();
    new aura_script<spell_warl_unstable_affliction>("spell_warl_unstable_affliction");
    new aura_script<spell_warl_backlash>("spell_warl_backlash");
    new spell_script<spell_warl_blood_horror>("spell_warl_blood_horror");
    new aura_script<spell_warl_blood_horror_aura>("spell_warl_blood_horror_aura");
    new spell_warl_haunt();
    new spell_warl_singe_magic();
    new aura_script<spell_warl_glyph_of_shadow_bolt>("spell_warl_glyph_of_shadow_bolt");
    new aura_script<spell_warl_glyph_of_shadow_bolt_spec_fix>("spell_warl_glyph_of_shadow_bolt_spec_fix");
    new spell_script<spell_warl_shadow_bolt_glyphed>("spell_warl_shadow_bolt_glyphed");
    new aura_script<spell_warl_grimoire_of_supremacy>("spell_warl_grimoire_of_supremacy");
    new aura_script<spell_warl_grimoire_of_service_server_side>("spell_warl_grimoire_of_service_server_side");
    new aura_script<spell_warl_grimoire_of_sacrifice>("spell_warl_grimoire_of_sacrifice");
    new aura_script<spell_warl_grimoire_of_sacrifice_health_fix>("spell_warl_grimoire_of_sacrifice_health_fix");
    new spell_script<spell_warl_grimoire_of_sacrifice_selector>("spell_warl_grimoire_of_sacrifice_selector");
    new aura_script<spell_warl_wild_imp>("spell_warl_wild_imp");
    new aura_script<spell_gen_fel_energy>("spell_gen_fel_energy");
    new creature_script<npc_warlock_guardian>("npc_warlock_guardian");
    new creature_script<npc_wild_imp>("npc_wild_imp");
    new creature_script<npc_grimoire_of_service_pet>("npc_grimoire_of_service_pet");
    new creature_script<npc_doomguard>("npc_doomguard");
    new creature_script<npc_warl_infernal>("npc_warl_infernal");
    new spell_script<spell_warl_summon_doomguard>("spell_warl_summon_doomguard");
    new spell_script<spell_warl_doombolt>("spell_warl_doombolt");
    new spell_script<spell_warl_demonic_fury_energize>("spell_warl_demonic_fury_energize");
    new spell_script<spell_warl_demonic_fury_visual_controller>("spell_warl_demonic_fury_visual_controller");
    new spell_script<spell_warl_hellfire>("spell_warl_hellfire");
    new aura_script<spell_warl_mastery_master_demonologist>("spell_warl_mastery_master_demonologist");
    new aura_script<spell_warl_master_demonologist>("spell_warl_master_demonologist");
    new aura_script<spell_warl_demonic_fury>("spell_warl_demonic_fury");
    new aura_script<spell_warl_metamorphosis>("spell_warl_metamorphosis");
    new aura_script<spell_warl_soul_shards>("spell_warl_soul_shards");
    new spell_script<spell_warl_ember_consumer>("spell_warl_ember_consumer");
    new spell_script<spell_warl_ember_tap>("spell_warl_ember_tap");
    new aura_script<spell_warl_glyph_of_ember_tap>("spell_warl_glyph_of_ember_tap");
    new spell_script<spell_warl_fire_and_brimstone>("spell_warl_fire_and_brimstone");
    new spell_script<spell_warl_fire_and_brimstone_spell>("spell_warl_fire_and_brimstone_spell");
    new aura_script<spell_warl_fire_and_brimstone_immolate>("spell_warl_fire_and_brimstone_immolate");
    new spell_script<spell_warl_always_cirt>("spell_warl_always_cirt");
    new spell_script<spell_warl_imp_swarm>("spell_warl_imp_swarm");
    new spell_script<spell_warl_fel_firebolt>("spell_warl_fel_firebolt");
    new aura_script<spell_warl_shadowflame>("spell_warl_shadowflame");
    new aura_script<spell_warl_molten_core>("spell_warl_molten_core");
    new aura_script<spell_warl_seed_of_corruption>("spell_warl_seed_of_corruption");
    new spell_script<spell_warl_seed_of_corruption_soulburn_explode>("spell_warl_seed_of_corruption_soulburn_explode");
    new aura_script<spell_warl_malefic_grasp>("spell_warl_malefic_grasp");
    new aura_script<spell_warl_drain_soul>("spell_warl_drain_soul");
    new aura_script<spell_warl_agony>("spell_warl_agony");
    new spell_script<spell_warl_agony_instant_tick>("spell_warl_agony_instant_tick");
    new spell_script<spell_warl_soul_swap>("spell_warl_soul_swap");
    new aura_script<spell_warl_soul_swap_aura>("spell_warl_soul_swap_aura");
    new spell_script<spell_warl_soul_swap_exhale>("spell_warl_soul_swap_exhale");
    new spell_script<spell_warl_soul_swap_soulburn>("spell_warl_soul_swap_soulburn");
    new aura_script<spell_warl_dark_apotheosis>("spell_warl_dark_apotheosis");
    new aura_script<spell_warl_metamorphosis_overrides>("spell_warl_metamorphosis_overrides");
    new aura_script<spell_warl_demonic_calling>("spell_warl_demonic_calling");
    new aura_script<spell_warl_glyph_of_demon_hunting>("spell_warl_glyph_of_demon_hunting");
    new spell_script<spell_warl_demonic_leap>("spell_warl_demonic_leap");
    new spell_script<spell_warl_demonic_leap_jump>("spell_warl_demonic_leap_jump");
    new spell_script<spell_warl_healthstone>("spell_warl_healthstone");
    new aura_script<spell_warl_healthstone_glyphed>("spell_warl_healthstone_glyphed");
    new aura_script<spell_warl_demonic_circle_summon>("spell_warl_demonic_circle_summon");
    new aura_script<spell_warl_demonic_circle_teleport>("spell_warl_demonic_circle_teleport");
    new aura_script<spell_warl_backdraft_proc_driver>("spell_warl_backdraft_proc_driver");
    new aura_script<spell_warl_backdraft>("spell_warl_backdraft");
    new spell_script<spell_warl_backdraft_consumer>("spell_warl_backdraft_consumer");
    new spell_script<spell_warl_soulstone>("spell_warl_soulstone");
    new spell_script<spell_warl_soulstone_resurrect>("spell_warl_soulstone_resurrect");
    new aura_script<spell_warl_havoc>("spell_warl_havoc");
    new spell_script<spell_warl_havoc_target_selector>("spell_warl_havoc_target_selector");
    new aura_script<spell_warl_rain_of_fire>("spell_warl_rain_of_fire");
    new aura_script<spell_warl_soul_link>("spell_warl_soul_link");
    new aura_script<spell_warl_soul_leech>("spell_warl_soul_leech");
    new aura_script<spell_warl_override_command_demon>("spell_warl_override_command_demon");
    new spell_script<spell_warl_command_demon>("spell_warl_command_demon");
    new spell_script<spell_warl_command_demon_cooldown>("spell_warl_command_demon_cooldown");
    new spell_script<spell_warl_create_healthstone>("spell_warl_create_healthstone");
    new spell_script<spell_warl_hand_of_guldan>("spell_warl_hand_of_guldan");
    new spell_script<spell_warl_hand_of_guldan_glyphed>("spell_warl_hand_of_guldan_glyphed");
    new spell_script<spell_warl_demonic_gateway>("spell_warl_demonic_gateway");
    new spell_script<spell_warl_demonic_gateway_summon>("spell_warl_demonic_gateway_summon");
    new aura_script<spell_warl_demonic_gateway_aura>("spell_warl_demonic_gateway_aura");
    new creature_script<npc_demonic_gateway>("npc_demonic_gateway");
    new aura_script<spell_warl_drain_life>("spell_warl_drain_life");
    new aura_script<spell_warl_glyph_of_siphon_life>("spell_warl_glyph_of_siphon_life");
    new spell_script<spell_warl_glyph_of_siphon_life_heal>("spell_warl_glyph_of_siphon_life_heal");
    new aura_script<spell_warl_chaos_bolt_dot>("spell_warl_chaos_bolt_dot");
    new spell_script<spell_warl_pet_auto_cast>("spell_warl_pet_auto_cast");
    new spell_script<spell_warl_shadow_bulwark>("spell_warl_shadow_bulwark");
    new spell_script<spell_warl_suffering>("spell_warl_suffering");
    new spell_script<spell_warl_seduction>("spell_warl_seduction");
    new spell_script<spell_warl_shadow_shield>("spell_warl_shadow_shield");
    new aura_script<spell_warl_shadow_shield_aura>("spell_warl_shadow_shield_aura");
    new spell_script<spell_warl_touch_of_chaos>("spell_warl_touch_of_chaos");
    new aura_script<spell_warl_howl_of_terror_cooldown>("spell_warl_howl_of_terror_cooldown");
    new spell_script<spell_warl_legion_strike>("spell_warl_legion_strike");
    new spell_script<spell_warl_devour_magic>("spell_warl_devour_magic");
    new spell_script<spell_warl_devour_magic_heal>("spell_warl_devour_magic_heal");
    new aura_script<spell_warl_glyph_of_demon_training>("spell_warl_glyph_of_demon_training");
    new spell_script<spell_warl_glyph_of_demon_training_seduction>("spell_warl_glyph_of_demon_training_seduction");
    new spell_script<spell_warl_glyph_of_demon_trainig_firebolt>("spell_warl_glyph_of_demon_trainig_firebolt");
    new spell_script<spell_warl_glyph_of_carrion_swarm>("spell_warl_glyph_of_carrion_swarm");
    new spell_script<spell_warl_unending_breath_soulburn>("spell_warl_unending_breath_soulburn");
    new spell_script<spell_warl_soul_shards_visual_test>("spell_warl_soul_shards_visual_test");
    new spell_script<spell_warl_ember_test>("spell_warl_ember_test");
    new spell_script<spell_warl_glyph_of_felguard>("spell_warl_glyph_of_felguard");
    new spell_script<spell_warl_glyph_of_felguard_loading>("spell_warl_glyph_of_felguard_loading");
    new aura_script<spell_warl_glyph_of_verdant_spheres>("spell_warl_glyph_of_verdant_spheres");
    new aura_script<spell_warl_glyph_of_soul_consumption>("spell_warl_glyph_of_soul_consumption");
    new aura_script<spell_warl_archimondes_darkness>("spell_warl_archimondes_darkness");
    new aura_script<spell_warl_t16_2p_bonus>("spell_warl_t16_2p_bonus");
    new aura_script<spell_warl_t16_4p_bonus>("spell_warl_t16_4p_bonus");
    new aura_script<spell_warl_haunt_t16>("spell_warl_haunt_t16");
    new aura_script<spell_warl_fury_ward_fix>("spell_warl_fury_ward_fix");
    new aura_script<spell_warl_twilight_ward_fix>("spell_warl_twilight_ward_fix");
    new aura_script<spell_warl_meteor_slam>("spell_warl_meteor_slam");
    new aura_script<spell_warl_corruption>("spell_warl_corruption");
    new aura_script<spell_warl_nightfall>("spell_warl_nightfall");
}
