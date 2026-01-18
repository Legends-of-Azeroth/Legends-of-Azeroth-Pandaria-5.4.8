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
 * Scripts for spells with SPELLFAMILY_DEATHKNIGHT and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dk_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "Pet.h"
#include "Battleground.h"
#include "SpellHistory.h"
#include "Random.h"

enum DeathKnightSpells
{
    SPELL_DK_RUNIC_POWER_ENERGIZE               = 49088,
    DK_SPELL_ANTI_MAGIC_SHELL_TALENT            = 51052,
    DK_SPELL_CORPSE_EXPLOSION_TRIGGERED         = 43999,
    DK_SPELL_CORPSE_EXPLOSION_VISUAL            = 51270,
    DK_SPELL_GHOUL_EXPLODE                      = 47496,
    DK_SPELL_SCOURGE_STRIKE_TRIGGERED           = 70890,
    SPELL_DK_BLOOD_BOIL_TRIGGERED               = 65658,
    SPELL_DK_ITEM_T8_MELEE_4P_BONUS             = 64736,
    DK_SPELL_BLACK_ICE_R1                       = 49140,
    SPELL_DK_BLOOD_PLAGUE                       = 55078,
    SPELL_DK_FROST_FEVER                        = 55095,
    SPELL_DK_ROILING_BLOOD                      = 108170,
    SPELL_DK_PESTILENCE                         = 50842,
    SPELL_DK_PESTILENCE_BLOOD_BOIL              = 116617,
    SPELL_DK_PESTILENCE_VISUAL                  = 91939,
    SPELL_DK_CHILBLAINS                         = 50041,
    SPELL_DK_CHAINS_OF_ICE_ROOT                 = 96294,
    DK_SPELL_PLAGUE_LEECH                       = 123693,
    DK_SPELL_PERDITION                          = 123981,
    DK_SPELL_SHROUD_OF_PURGATORY                = 116888,
    DK_SPELL_PURGATORY_INSTAKILL                = 123982,
    DK_SPELL_BLOOD_RITES                        = 50034,
    SPELL_DK_DEATH_SIPHON_HEAL                  = 116783,
    SPELL_DK_GLYPH_OF_DEATHS_EMBRACE            = 58677,
    SPELL_DK_BLOOD_TAP                          = 45529,
    SPELL_DK_BLOOD_CHARGE                       = 114851,
    SPELL_DK_PILLAR_OF_FROST                    = 51271,
    SPELL_DK_SOUL_REAPER_HASTE                  = 114868,
    SPELL_DK_SOUL_REAPER_DAMAGE                 = 114867,
    SPELL_DK_REMORSELESS_WINTER_STUN            = 115001,
    DK_SPELL_CONVERSION                         = 119975,
    DK_SPELL_WEAKENED_BLOWS                     = 115798,
    DK_SPELL_SCARLET_FEVER                      = 81132,
    DK_SPELL_SCENT_OF_BLOOD_AURA                = 50421,
    DK_SPELL_CHAINS_OF_ICE                      = 45524,
    DK_SPELL_EBON_PLAGUEBRINGER                 = 51160,
    SPELL_DK_DESECRATED_GROUND                  = 118009,
    SPELL_DK_DESECRATED_GROUND_IMMUNE           = 115018,
    DK_SPELL_ASPHYXIATE                         = 108194,
    SPELL_DK_DARK_INFUSION_STACKS               = 91342,
    DK_SPELL_DARK_INFUSION_AURA                 = 93426,
    DK_NPC_WILD_MUSHROOM                        = 59172,
    DK_SPELL_RUNIC_CORRUPTION_REGEN             = 51460,
    DK_SPELL_RUNIC_EMPOWERMENT                  = 81229,
    DK_SPELL_GOREFIENDS_GRASP_GRIP_VISUAL       = 114869,
    DK_SPELL_GOREFIENDS_GRASP_JUMP              = 146599,
    DK_SPELL_DEATH_GRIP_ONLY_JUMP               = 49575,
    DK_SPELL_GLYPH_OF_CORPSE_EXPLOSION          = 127344,
    DK_SPELL_GLYPH_OF_HORN_OF_WINTER            = 58680,
    DK_SPELL_GLYPH_OF_HORN_OF_WINTER_EFFECT     = 121920,
    SPELL_DK_DEATH_AND_DECAY                    = 43265,
    SPELL_DK_DEATH_AND_DECAY_DAMAGE             = 52212,
    SPELL_DK_DEATH_COIL                         = 47541,
    SPELL_DK_DEATH_COIL_DAMAGE                  = 47632,
    SPELL_DK_DEATH_COIL_HEAL                    = 47633,
    SPELL_DK_T15_DPS_4P_BONUS                   = 138347,
    SPELL_DK_T15_BLOOD_4P_BONUS                 = 138197,
    SPELL_DK_T16_BLOOD_2P_BONUS                 = 144934,
    SPELL_DK_T16_DPS_4P_BONUS_HIDDEN            = 144909,
    SPELL_DK_BONE_WALL                          = 144948,
    SPELL_DK_RUNIC_POWER                        = 138214,
    SPELL_DK_DARK_TRANSFORMATION                = 63560,
    SPELL_DK_DARK_TRANSFORMATION_CASTER_AURA    = 93426,
    SPELL_DK_MASTER_OF_GHOULS                   = 52143,
    SPELL_DK_DEATH_GRIP                         = 49576,
    SPELL_DK_DEATH_GRIP_EFFECT                  = 49560,
    SPELL_DK_GLYPH_OF_SHIFTING_PRESENCES        = 58647,
    SPELL_DK_FESTERING_BLOOD                    = 146650,
    SPELL_DK_GLYPH_OF_DEATH_AND_DECAY           = 58629,
    SPELL_DK_DEATH_AND_DECAY_SLOW               = 143375,
    SPELL_DK_STRANGULATE                        = 47476,
    SPELL_DK_SUMMON_GARGOYLE                    = 49206,
    SPELL_DK_SUMMON_GARGOYLE_FEED               = 50514,
    SPELL_DK_GARGOYLE_STRIKE                    = 51963,
    SPELL_DK_DISMISS_GARGOYLE                   = 50515,
    SPELL_DK_MASTERY_DREAD_BLADE                = 77515,
    SPELL_DK_DEATH_STRIKE_HEAL                  = 45470,
    SPELL_DK_MASTERY_BLOOD_SHIELD               = 77513,
    SPELL_DK_BLOOD_SHIELD                       = 77535,
    SPELL_DK_GLYPH_OF_DARK_SUCCOR               = 101568,
    SPELL_DK_RELENTLESS_GRIP                    = 131547,
    SPELL_DK_DARK_SIMULACRUM_OVERRIDE           = 77616,
    SPELL_DK_DARK_SIMULACRUM_SPD                = 94984,
    SPELL_DK_GLYPH_OF_PILLAR_OF_FROST           = 58635,
    SPELL_DK_PILLAR_OF_FROST_GLYPHED            = 90259,
    SPELL_DK_GLYPH_OF_STRANGULATE               = 58618,
    SPELL_DK_RUNE_TAP                           = 48982,
    SPELL_DK_GLYPH_OF_RESILIENT_GRIP            = 59309,
    SPELL_DK_GLYPH_OF_TRANQUIL_GRIP             = 63335,
    SPELL_DK_WILD_MUSHROOM_PLAGUE_TRIGGER       = 113518,
    SPELL_DK_SKELETON_TRANSFORM                 = 147157,
    SPELL_DK_GLYPH_OF_REGENERATIVE_MAGIC        = 146648,
    SPELL_DK_GLYPH_OF_THE_LOUD_HORN             = 146646,
    SPELL_DK_GLYPH_OF_THE_LOUD_HORN_ENERGIZE    = 147078,
    SPELL_DK_ANTI_MAGIC_ZONE                    = 145629,
    SPELL_DK_GLYPH_OF_DEATH_COIL                = 63333,
    SPELL_DK_DEATH_BARRIER                      = 115635,

    SPELL_SANCTUARY                             = 54661,
    SPELL_BLOOD_BURST                           = 81280,
    SPELL_BLOOD_GORGED                          = 81277,

    SPELL_ROGUE_TRICKS_OF_THE_TRADE             = 57933,

    SPELL_DRUID_DEATH_COIL_DAMAGE               = 122283,
    SPELL_DRUID_DEATH_COIL_HEAL                 = 122284,
};

// 108199 - Gorefiend's Grasp
class spell_dk_gorefiends_grasp : public SpellScript
{
    PrepareSpellScript(spell_dk_gorefiends_grasp);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        targets.remove_if([=](WorldObject const* obj)
        {
            Unit const* target = obj->ToUnit();
            switch (target->GetCreatureType())
            {
                case CREATURE_TYPE_CRITTER:
                case CREATURE_TYPE_NON_COMBAT_PET:
                case CREATURE_TYPE_WILD_PET:
                    return true;
            }
            return false;
        });
    }

    void HandleHit(SpellEffIndex)
    {
        if (Unit* center = GetExplTargetUnit())
        {
            Unit* target = GetHitUnit();
            center->CastSpell(target, DK_SPELL_GOREFIENDS_GRASP_GRIP_VISUAL, true);
            target->CastSpell(center, DK_SPELL_GOREFIENDS_GRASP_JUMP, true);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_gorefiends_grasp::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_gorefiends_grasp::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_dk_gorefiends_grasp::HandleHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Called by Death Coil (damage) - 47632, Frost Strike - 49143 and Runic Strike - 56815
// Runic Empowerment - 81229
class spell_dk_runic_empowerment : public SpellScriptLoader
{
    public:
        spell_dk_runic_empowerment() : SpellScriptLoader("spell_dk_runic_empowerment") { }

        class spell_dk_runic_empowerment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_runic_empowerment_SpellScript);

            void HandleOnHit()
            {
                Player* const player = GetCaster()->ToPlayer();
                if (!player || !player->HasAura(DK_SPELL_RUNIC_EMPOWERMENT))
                    return;

                if (!roll_chance_i(45))
                    return;

                uint8 runes[MAX_RUNES];
                uint8* runesEnd = runes;

                for (uint8 i = 0; i < MAX_RUNES; ++i)
                    if (player->GetRune(i).State == RuneState::FullyDepleted)
                        *runesEnd++ = i;

                if (auto const count = std::distance(runes, runesEnd))
                {
                    uint8 const* itr = runes;
                    std::advance(itr, irand(0, count - 1));

                    player->ActivateRune(*itr);
                    player->ResyncRunes(MAX_RUNES);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_runic_empowerment_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_runic_empowerment_SpellScript();
        }
};

// Might of Ursoc - 113072
class spell_dk_might_of_ursoc : public SpellScriptLoader
{
    public:
        spell_dk_might_of_ursoc() : SpellScriptLoader("spell_dk_might_of_ursoc") { }

        class spell_dk_might_of_ursoc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_might_of_ursoc_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->GetHealthPct() < 15.0f)
                        caster->SetHealth(caster->CountPctFromMaxHealth(15));
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_might_of_ursoc_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_might_of_ursoc_AuraScript();
        }
};

// 118009 - Desecrated ground
class spell_dk_desecrated_ground : public AuraScript
{
    PrepareAuraScript(spell_dk_desecrated_ground);

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
            if (DynamicObject* dynObj = caster->GetDynObject(SPELL_DK_DESECRATED_GROUND))
                if (caster->GetDistance(dynObj) <= 8.0f)
                    caster->CastSpell(caster, SPELL_DK_DESECRATED_GROUND_IMMUNE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_desecrated_ground::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 73975 - Necrotic Strike
class spell_dk_necrotic_strike : public AuraScript
{
    PrepareAuraScript(spell_dk_necrotic_strike);

    void CalculateAmount(AuraEffect const* aurEff, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
        {
            amount = caster->GetTotalAttackPowerValue(BASE_ATTACK) * 2.25f;
            if (Player* deathknight = caster->ToPlayer())
                AddPct(amount, deathknight->GetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE));

            int32 absorbAmount = std::round(amount);
            caster->ApplyResilience(GetUnitOwner(), &absorbAmount);
            
            if (AuraEffect* necrotic = GetUnitOwner()->GetAuraEffect(73975, EFFECT_0, caster->GetGUID()))
                absorbAmount += necrotic->GetAmount();
            amount = int32(absorbAmount);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_necrotic_strike::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB);
    }
};

// Festering Strike - 85948
class spell_dk_festering_strike : public SpellScriptLoader
{
    public:
        spell_dk_festering_strike() : SpellScriptLoader("spell_dk_festering_strike") { }

        class spell_dk_festering_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_festering_strike_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (Aura *BP = target->GetAura(SPELL_DK_BLOOD_PLAGUE, _player->GetGUID()))
                        {
                            uint32 dur = BP->GetDuration() + 6000;
                            BP->SetDuration(dur);

                            if (dur > uint32(BP->GetMaxDuration()))
                                BP->SetMaxDuration(dur);

                            if (auto const PV = target->GetAura(81326, _player->GetGUID()))
                            {
                                PV->SetDuration(dur);
                                if (dur > uint32(PV->GetMaxDuration()))
                                    PV->SetMaxDuration(dur);
                            }
                        }
                        if (Aura *FF = target->GetAura(SPELL_DK_FROST_FEVER, _player->GetGUID()))
                        {
                            uint32 dur = FF->GetDuration() + 6000;
                            FF->SetDuration(dur);

                            if (dur > uint32(FF->GetMaxDuration()))
                                FF->SetMaxDuration(dur);
                        }
                        if (Aura *COI = target->GetAura(DK_SPELL_CHAINS_OF_ICE, _player->GetGUID()))
                        {
                            uint32 dur = COI->GetDuration() + 6000;
                            COI->SetDuration(dur);

                            if (dur > uint32(COI->GetMaxDuration()))
                                COI->SetMaxDuration(dur);
                        }
                    }
                }
            }
            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_festering_strike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_festering_strike_SpellScript();
        }
};

// 108200 - Remorseless Winter
class spell_dk_remorseless_winter_targets : public AuraScript
{
    PrepareAuraScript(spell_dk_remorseless_winter_targets);

public:
    std::set<uint64> untargetable;

    bool operator()(WorldObject const* target)
    {
        Unit const* unit = target->ToUnit();
        if (!unit)
            return true;
        if (Aura const* remorslessWinter = unit->GetAura(GetSpellInfo()->Id))
            if (remorslessWinter->GetStackAmount() >= 5)
                return true;
        if (untargetable.find(target->GetGUID()) != untargetable.end())
            return true;
        return false;
    }

    void Register() override { }
};

// 115000 - Remorseless Winter
class spell_dk_remorseless_winter : public SpellScript
{
    PrepareSpellScript(spell_dk_remorseless_winter);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Aura* aura = GetCaster()->GetAura(108200))
            if (auto script = dynamic_cast<spell_dk_remorseless_winter_targets*>(aura->GetScriptByName("spell_dk_remorseless_winter_targets")))
                targets.remove_if(*script);
    }

    void HandleHit()
    {
        if (Aura const* remorselessWinter = GetHitUnit()->GetAura(GetSpellInfo()->Id))
        {
            if (remorselessWinter->GetStackAmount() == 5 && !GetHitUnit()->HasAura(SPELL_DK_REMORSELESS_WINTER_STUN))
            {
                GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_REMORSELESS_WINTER_STUN, true);
                if (Aura* aura = GetCaster()->GetAura(108200))
                    if (auto script = dynamic_cast<spell_dk_remorseless_winter_targets*>(aura->GetScriptByName("spell_dk_remorseless_winter_targets")))
                        script->untargetable.insert(GetHitUnit()->GetGUID());
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_remorseless_winter::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_remorseless_winter::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_dk_remorseless_winter::HandleHit);
    }
};

// 51271 - Pillar of Frost
class spell_dk_pillar_of_frost : public AuraScript
{
    PrepareAuraScript(spell_dk_pillar_of_frost);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        PreventDefaultAction();

        Unit* dk = GetTarget();
        dk->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        dk->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        dk->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        if (dk->HasAura(SPELL_DK_GLYPH_OF_PILLAR_OF_FROST))
            dk->CastSpell(dk, SPELL_DK_PILLAR_OF_FROST_GLYPHED, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        PreventDefaultAction();

        Unit* dk = GetTarget();
        dk->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_GRIP, false);
        dk->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
        dk->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, false);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dk_pillar_of_frost::HandleApply, EFFECT_1, SPELL_AURA_APPLY_CREATURE_IMMUNITIES, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dk_pillar_of_frost::HandleRemove, EFFECT_1, SPELL_AURA_APPLY_CREATURE_IMMUNITIES, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47541 - Death Coil
// 56815 - Rune Strike
// 49143 - Frost Strike
class spell_dk_blood_charges : public SpellScript
{
    PrepareSpellScript(spell_dk_blood_charges);

    void HandleHit()
    {
        if (GetCaster()->HasSpell(SPELL_DK_BLOOD_TAP))
        {
            if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_DEATHS_EMBRACE) && !GetCaster()->IsValidAttackTarget(GetHitUnit(), GetSpellInfo()))
                return;

            GetCaster()->CastSpell(GetCaster(), SPELL_DK_BLOOD_CHARGE, true);
            GetCaster()->CastSpell(GetCaster(), SPELL_DK_BLOOD_CHARGE, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_blood_charges::HandleHit);
    }
};

// 45529 - Blood Tap
class spell_dk_blood_tap : public SpellScript
{
    PrepareSpellScript(spell_dk_blood_tap);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->ToPlayer())
        {
            Aura* bloodCharges = GetCaster()->ToPlayer()->GetAura(SPELL_DK_BLOOD_CHARGE);
            if (!bloodCharges)
                return SPELL_FAILED_DONT_REPORT;
            
            if (bloodCharges->GetStackAmount() < 5)
                return SPELL_FAILED_DONT_REPORT;

            bool cooldown = false;

            for (uint8 i = 0; i < MAX_RUNES; ++i)
            {
                if (GetCaster()->ToPlayer()->GetCurrentRune(i) == RUNE_DEATH || !GetCaster()->ToPlayer()->HasRuneCooldown(i))
                    continue;

                cooldown = true;
            }

            if (!cooldown)
                return SPELL_FAILED_DONT_REPORT;
        }

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (GetHitUnit())
            {
                if (Aura *bloodCharges = player->GetAura(SPELL_DK_BLOOD_CHARGE))
                {
                    int32 newAmount = bloodCharges->GetStackAmount();

                    if ((newAmount - 5) <= 0)
                        player->RemoveAura(SPELL_DK_BLOOD_CHARGE);
                    else
                        bloodCharges->SetStackAmount(newAmount - 5);

                    std::vector<uint8> runes;
                    for (uint8 i = 0; i < MAX_RUNES; i++)
                        if (player->HasRuneCooldown(i) && player->GetCurrentRune(i) != RUNE_DEATH)
                            runes.push_back(i);

                    ASSERT(!runes.empty());
                    uint32 rune = runes[urand(0, runes.size() - 1)];
                    player->AddRuneBySpell(rune, RUNE_DEATH, 45529);
                    player->ActivateRune(rune);
                    player->ResyncRunes(MAX_RUNES);
                }
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_blood_tap::CheckCast);
        OnHit += SpellHitFn(spell_dk_blood_tap::HandleHit);
    }
};

// 49998 - Death Strike
class spell_dk_death_strike : public SpellScript
{
    PrepareSpellScript(spell_dk_death_strike);

    void HandleDummy(SpellEffIndex)
    {
        int32 minPct = GetEffectValue();

        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_DK_GLYPH_OF_DARK_SUCCOR, EFFECT_0))
            if (GetCaster()->HasAura(48265) || GetCaster()->HasAura(48266)) // Only in frost/unholy presence
                minPct = eff->GetAmount();

        int32 min = GetCaster()->CountPctFromMaxHealth(minPct);
        int32 cur = GetCaster()->GetNpcDamageTakenInPastSecs(5) * 0.2f;
        int32 bp = std::max(min, cur);

        // Item - Death Knight T14 Blood 4P bonus
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(123080, EFFECT_0))
            AddPct(bp, eff->GetAmount());

        GetCaster()->CastCustomSpell(SPELL_DK_DEATH_STRIKE_HEAL, SPELLVALUE_BASE_POINT0, bp, GetCaster(), true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_dk_death_strike::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
    }
};

// Death Strike heal - 45470
class spell_dk_death_strike_heal : public SpellScript
{
    PrepareSpellScript(spell_dk_death_strike_heal);

    void HandleHit()
    {
        if (AuraEffect* scentOfBlood = GetCaster()->GetAuraEffect(DK_SPELL_SCENT_OF_BLOOD_AURA, EFFECT_0))
        {
            int32 heal = GetHitHeal();
            AddPct(heal, scentOfBlood->GetFloatAmount());
            SetHitHeal(heal);
        }

        GetCaster()->RemoveAura(DK_SPELL_SCENT_OF_BLOOD_AURA);

        // Must be here, not in the additional script
        if (AuraEffect const* mastry = GetCaster()->GetAuraEffect(SPELL_DK_MASTERY_BLOOD_SHIELD, EFFECT_0))
        {
            float shield = GetHitHeal();
            ApplyPct(shield, mastry->GetFloatAmount());

            if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_DK_BLOOD_SHIELD, EFFECT_0))
                shield += eff->GetFloatAmount();

            int32 absorb = std::min(int32(shield), int32(GetCaster()->GetMaxHealth()));
            GetCaster()->CastCustomSpell(SPELL_DK_BLOOD_SHIELD, SPELLVALUE_BASE_POINT0, absorb, GetCaster(), true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_death_strike_heal::HandleHit);
    }
};

// Purgatory - 116888
class spell_dk_purgatory : public SpellScriptLoader
{
    public:
        spell_dk_purgatory() : SpellScriptLoader("spell_dk_purgatory") { }

        class spell_dk_purgatory_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_purgatory_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                {
                    AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                    if (removeMode == AURA_REMOVE_BY_EXPIRE)
                        _player->CastSpell(_player, DK_SPELL_PURGATORY_INSTAKILL, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_dk_purgatory_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_purgatory_AuraScript();
        }
};

// Purgatory - 114556
class spell_dk_purgatory_absorb : public SpellScriptLoader
{
    public:
        spell_dk_purgatory_absorb() : SpellScriptLoader("spell_dk_purgatory_absorb") { }

        class spell_dk_purgatory_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_purgatory_absorb_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect * /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();

                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                // No damage received under Shroud of Purgatory
                if (target->ToPlayer()->HasAura(DK_SPELL_SHROUD_OF_PURGATORY))
                {
                    absorbAmount = dmgInfo.GetDamage();
                    return;
                }

                if (target->ToPlayer()->HasAura(DK_SPELL_PERDITION))
                    return;

                int32 bp = dmgInfo.GetDamage();

                target->CastCustomSpell(target, DK_SPELL_SHROUD_OF_PURGATORY, &bp, NULL, NULL, true);
                target->CastSpell(target, DK_SPELL_PERDITION, true);
                target->SetHealth(1);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_purgatory_absorb_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_purgatory_absorb_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_purgatory_absorb_AuraScript();
        }
};

// Unholy Blight - 115994
class spell_dk_unholy_blight : public SpellScriptLoader
{
    public:
        spell_dk_unholy_blight() : SpellScriptLoader("spell_dk_unholy_blight") { }

        class spell_dk_unholy_blight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_unholy_blight_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        _player->CastSpell(target, SPELL_DK_BLOOD_PLAGUE, true);
                        _player->CastSpell(target, SPELL_DK_FROST_FEVER, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_dk_unholy_blight_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_unholy_blight_SpellScript();
        }
};

// 45524 - Chains of Ice
class spell_dk_chilblains : public SpellScript
{
    PrepareSpellScript(spell_dk_chilblains);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_DK_CHILBLAINS))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_CHAINS_OF_ICE_ROOT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_chilblains::HandleHit);
    }
};

// 77575 - Outbreak
class spell_dk_outbreak : public SpellScript
{
    PrepareSpellScript(spell_dk_outbreak);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_BLOOD_PLAGUE, true);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_FROST_FEVER, true);

        // Dancing rune weapon
        if (GetCaster()->GetTypeId() == TYPEID_PLAYER)
            for (auto&& itr : GetCaster()->GetSummons())
                if (itr->GetEntry() == 27893)
                    itr->CastSpell(GetHitUnit(), GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_outbreak::HandleHit);
    }
};

// 50462 - Anti-Magic Shell (on raid member)
class spell_dk_anti_magic_shell_raid : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_raid() : SpellScriptLoader("spell_dk_anti_magic_shell_raid") { }

        class spell_dk_anti_magic_shell_raid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_raid_AuraScript);

            uint32 absorbPct;

            bool Load() override
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                // TODO: this should absorb limited amount of damage, but no info on calculation formula
                amount = -1;
            }

            void Absorb(AuraEffect * /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register() override
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_raid_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_raid_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_anti_magic_shell_raid_AuraScript();
        }
};

// 50462 - Anti-Magic Shell (on raid member)
class spell_dk_deaths_advance: public SpellScriptLoader
{
    public:
        spell_dk_deaths_advance() : SpellScriptLoader("spell_dk_deaths_advance") { }

        class spell_dk_deaths_advance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_deaths_advance_AuraScript);

            uint32 absorbPct;

            bool Load() override
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateEffect(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (caster->HasAura(90259))
                    amount = 0;
            }

            void Register() override
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_deaths_advance_AuraScript::CalculateEffect, EFFECT_0, SPELL_AURA_MOD_SPEED_NOT_STACK);
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_deaths_advance_AuraScript::CalculateEffect, EFFECT_1, SPELL_AURA_MOD_MINIMUM_SPEED);
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_deaths_advance_AuraScript::CalculateEffect, EFFECT_1, SPELL_AURA_MOD_SPEED_NOT_STACK);
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_deaths_advance_AuraScript::CalculateEffect, EFFECT_0, SPELL_AURA_MOD_MINIMUM_SPEED);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_deaths_advance_AuraScript();
        }

        class spell_dk_deaths_advance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_deaths_advance_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetSpellInfo()->Id == 96268)
                    return SPELL_CAST_OK;

                if (GetCaster()->HasAura(90259))
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_deaths_advance_SpellScript::CheckClass);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_deaths_advance_SpellScript();
        }
};

// 48707 - Anti-Magic Shell
// 110570 - Anti-Magic Shell (symbiosis)
class spell_dk_anti_magic_shell : public AuraScript
{
    PrepareAuraScript(spell_dk_anti_magic_shell);

    uint32 absorbPct = 0;

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        absorbPct = amount;
        if (Unit* caster = GetCaster())
            amount = caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster));
    }

    void Absorb(AuraEffect* shield, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        absorbAmount = std::min(CalculatePct(dmgInfo.GetDamage(), absorbPct), uint32(shield->GetAmount()));
    }

    void Trigger(AuraEffect*, DamageInfo&, uint32& absorbAmount)
    {
        // In sniffs it is about 0.70-0.96%, but heavy pve spells almost always 0.88, I dunno
        int32 amount = std::round(CalculatePct(float(absorbAmount), 0.88f));
        GetUnitOwner()->CastCustomSpell(SPELL_DK_RUNIC_POWER_ENERGIZE, SPELLVALUE_BASE_POINT0, amount, GetUnitOwner(), true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell::Absorb, EFFECT_0);

        if (m_scriptSpellId != 48707)
            return;
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell::Trigger, EFFECT_0);
    }
};

// 50461 - Anti-Magic Zone
class spell_dk_anti_magic_zone : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_zone() : SpellScriptLoader("spell_dk_anti_magic_zone") { }

        class spell_dk_anti_magic_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_zone_AuraScript);

            uint32 absorbPct;

            bool Load() override
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_ANTI_MAGIC_SHELL_TALENT))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = 136800;
                if (Player* player = GetCaster()->ToPlayer())
                     amount += int32(player->GetStat(STAT_STRENGTH) * 4);
            }

            void Absorb(AuraEffect * /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register() override
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_zone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_zone_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_anti_magic_zone_AuraScript();
        }
};

// 47496 - Explode, Ghoul spell for Corpse Explosion
class spell_dk_ghoul_explode : public SpellScriptLoader
{
    public:
        spell_dk_ghoul_explode() : SpellScriptLoader("spell_dk_ghoul_explode") { }

        class spell_dk_ghoul_explode_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_ghoul_explode_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(DK_SPELL_CORPSE_EXPLOSION_TRIGGERED))
                    return false;
                return true;
            }

            void Suicide(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    // Corpse Explosion (Suicide)
                    unitTarget->CastSpell(unitTarget, DK_SPELL_CORPSE_EXPLOSION_TRIGGERED, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_ghoul_explode_SpellScript::Suicide, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_ghoul_explode_SpellScript();
        }
};

// Death Gate - 52751
class spell_dk_death_gate : public SpellScriptLoader
{
    public:
        spell_dk_death_gate() : SpellScriptLoader("spell_dk_death_gate") { }

        class spell_dk_death_gate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetCaster()->GetClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, GetEffectValue(), false);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_SpellScript::CheckClass);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_death_gate_SpellScript();
        }
};

class spell_dk_death_pact : public SpellScriptLoader
{
    public:
        spell_dk_death_pact() : SpellScriptLoader("spell_dk_death_pact") { }

        class spell_dk_death_pact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_pact_SpellScript);

            SpellCastResult CheckCast()
            {
                // Check if we have valid targets, otherwise skip spell casting here
                if (Player* player = GetCaster()->ToPlayer())
                    for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                        if (Creature* undeadPet = (*itr)->ToCreature())
                            if (undeadPet->IsAlive() &&
                                undeadPet->GetOwnerGUID() == player->GetGUID() &&
                                undeadPet->GetCreatureType() == CREATURE_TYPE_UNDEAD &&
                                undeadPet->IsWithinDist(player, 100.0f, false))
                                return SPELL_CAST_OK;

                return SPELL_FAILED_NO_PET;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* unit_to_add = NULL;
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                {
                    if (Unit* unit = (*itr)->ToUnit())
                    if (unit->GetOwnerGUID() == GetCaster()->GetGUID() && unit->GetCreatureType() == CREATURE_TYPE_UNDEAD)
                    {
                        unit_to_add = unit;
                        break;
                    }
                }

                targets.clear();
                if (unit_to_add)
                    targets.push_back(unit_to_add);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_pact_SpellScript::CheckCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_death_pact_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_death_pact_SpellScript();
        }
};

// Scourge Strike - 55090
class spell_dk_scourge_strike : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike() : SpellScriptLoader("spell_dk_scourge_strike") { }

        class spell_dk_scourge_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_SpellScript);
            float multiplier;

            bool Load() override
            {
                multiplier = 1.0f;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    multiplier = (GetEffectValue() * unitTarget->GetDiseasesByCaster(caster->GetGUID()) / 100.f);
                    // Death Knight T8 Melee 4P Bonus
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_ITEM_T8_MELEE_4P_BONUS, EFFECT_0))
                        AddPct(multiplier, aurEff->GetAmount());
                }
            }

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = GetHitDamage() * multiplier;

                    if (AuraEffect *aurEff = caster->GetAuraEffectOfRankedSpell(DK_SPELL_BLACK_ICE_R1, EFFECT_0))
                        AddPct(bp, aurEff->GetAmount());

                    // Mastery: Dreadblade
                    if (AuraEffect *aurEff = caster->GetAuraEffect(77515, EFFECT_0))
                        AddPct(bp, aurEff->GetAmount());

                    caster->CastCustomSpell(unitTarget, DK_SPELL_SCOURGE_STRIKE_TRIGGERED, &bp, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_dk_scourge_strike_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_scourge_strike_SpellScript();
        }
};

// 49576 - Death Grip
class spell_dk_death_grip : public SpellScript
{
    PrepareSpellScript(spell_dk_death_grip);

    void HandleHit()
    {
        if (GetHitResult() == SPELL_MISS_REFLECT)
            GetExplTargetUnit()->CastSpell(GetCaster(), SPELL_DK_DEATH_GRIP_EFFECT, true);
        else if (GetHitResult() == SPELL_MISS_NONE)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_DEATH_GRIP_EFFECT, true);
        else if (GetHitResult() == SPELL_MISS_IMMUNE)
        {
            if (Creature* creature = GetHitUnit()->ToCreature())
            {
                if (creature->GetCreatureTemplate()->MechanicImmuneMask & (1 << (MECHANIC_GRIP - 1)) && !GetCaster()->HasAura(SPELL_DK_GLYPH_OF_TRANQUIL_GRIP))
                {
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_DEATH_GRIP_EFFECT, true); // Cast taunt anyway
                    return;
                }
            }

            // Okay, total immune
            if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_RESILIENT_GRIP))
                GetCaster()->ToPlayer()->RemoveSpellCooldown(SPELL_DK_DEATH_GRIP, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_death_grip::HandleHit);
    }
};

// 49560 - Death Grip
class spell_dk_death_grip_effect : public SpellScript
{
    PrepareSpellScript(spell_dk_death_grip_effect);

    void HandleDummy(SpellEffIndex)
    {
        // Deterrence
        if (GetHitUnit()->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) < 100)
            GetHitUnit()->CastSpell(*GetExplTargetDest(), GetEffectValue(), true);
    }

    void HandleLaunch(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_TRANQUIL_GRIP))
            PreventEffectForTarget(GetHitUnit(), effIndex);
    }

    void HandleTaunt(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_TRANQUIL_GRIP))
            PreventDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_effect::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectLaunchTarget += SpellEffectFn(spell_dk_death_grip_effect::HandleLaunch, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_effect::HandleTaunt, EFFECT_2, SPELL_EFFECT_ATTACK_ME);
    }
};

// Glyph of Corpse Explosion - 59336
class spell_dk_glyph_of_corpse_explosion : public SpellScriptLoader
{
    public:
        spell_dk_glyph_of_corpse_explosion() : SpellScriptLoader("spell_dk_glyph_of_corpse_explosion") { }

        class spell_dk_glyph_of_corpse_explosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_glyph_of_corpse_explosion_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(DK_SPELL_GLYPH_OF_CORPSE_EXPLOSION, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(DK_SPELL_GLYPH_OF_CORPSE_EXPLOSION))
                        _player->RemoveSpell(DK_SPELL_GLYPH_OF_CORPSE_EXPLOSION, false, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_dk_glyph_of_corpse_explosion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_dk_glyph_of_corpse_explosion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_glyph_of_corpse_explosion_AuraScript();
        }
};

// Glyph of Horn of Winter - 58680
// Called by Horn of Winter - 57330
class spell_dk_glyph_of_horn_of_winter : public SpellScriptLoader
{
    public:
        spell_dk_glyph_of_horn_of_winter() : SpellScriptLoader("spell_dk_glyph_of_horn_of_winter") { }

        class spell_dk_glyph_of_horn_of_winter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_glyph_of_horn_of_winter_SpellScript);

            void HandleAfterCast()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (!_player->IsInCombat() && _player->HasAura(DK_SPELL_GLYPH_OF_HORN_OF_WINTER))
                        _player->CastSpell(_player, DK_SPELL_GLYPH_OF_HORN_OF_WINTER_EFFECT, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dk_glyph_of_horn_of_winter_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_glyph_of_horn_of_winter_SpellScript();
        }
};

// 145677 - Riposte
class spell_dk_riposte : public SpellScriptLoader
{
    public:
        spell_dk_riposte() : SpellScriptLoader("spell_dk_riposte") { }

        class spell_dk_riposte_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_riposte_AuraScript)

            void CalculateAmount(AuraEffect const* , float& amount, bool& canBeRecalculated)
            {
                canBeRecalculated = false;

                Unit* const caster = GetCaster();
                if (!caster)
                    return;

                if (Player const* const player = caster->ToPlayer())
                {
                    uint32 rating = player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_PARRY);
                    rating += player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_DODGE);
                    amount = CalculatePct(rating, 75);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_riposte_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_riposte_AuraScript();
        }
};

// Plague Strike - 45462
class spell_dk_plague_strike : public SpellScriptLoader
{
    public:
        spell_dk_plague_strike() : SpellScriptLoader("spell_dk_plague_strike") { }

        class spell_dk_plague_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_plague_strike_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* const caster = GetCaster();
                if (!caster || !caster->HasAura(51160))
                    return;

                caster->CastSpell(GetHitUnit(), SPELL_DK_FROST_FEVER, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_plague_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dk_plague_strike_SpellScript();
        }
};

// Unholy Frenzy - 49016
class spell_dk_unholy_frenzy : public SpellScriptLoader
{
    public:
        spell_dk_unholy_frenzy() : SpellScriptLoader("spell_dk_unholy_frenzy") { }
        
        class spell_dk_unholy_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_unholy_frenzy_AuraScript)

            enum
            {
                GLYPH_OF_UNHOLY_FRENZY  = 58616
            };

            void onInitEffects(uint32& effectMask)
            {
                auto const caster = GetCaster();
                if (caster && caster->HasAura(GLYPH_OF_UNHOLY_FRENZY))
                    effectMask &= ~(1 << EFFECT_1);
            }

            void Register() override
            {
                OnInitEffects += AuraInitEffectsFn(spell_dk_unholy_frenzy_AuraScript::onInitEffects);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_unholy_frenzy_AuraScript();
        }
};

// Soul Reaper - 114868
class spell_dk_soul_reaper_effect : public SpellScriptLoader
{
    public:
        spell_dk_soul_reaper_effect() : SpellScriptLoader("spell_dk_soul_reaper_effect") { }
        
        class spell_dk_soul_reaper_effect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_soul_reaper_effect_AuraScript)

            enum
            {
                GLYPH_OF_SWIFT_DEATH  = 146645
            };

            void onInitEffects(uint32& effectMask)
            {
                auto const caster = GetCaster();
                if (caster && !caster->HasAura(GLYPH_OF_SWIFT_DEATH))
                    effectMask &= ~(1 << EFFECT_1);
            }

            void Register() override
            {
                OnInitEffects += AuraInitEffectsFn(spell_dk_soul_reaper_effect_AuraScript::onInitEffects);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dk_soul_reaper_effect_AuraScript();
        }
};

// 43265 - Death and Decay
class spell_dk_death_and_decay : public AuraScript
{
    PrepareAuraScript(spell_dk_death_and_decay);

    void CalculateDamage(AuraEffect const* eff, float& amount, bool&)
    {
        // Just for tooltip
        if (Unit* deathknight = GetCaster())
            amount = deathknight->SpellDamageBonusDone(deathknight, sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_AND_DECAY_DAMAGE), EFFECT_0, amount, SPELL_DIRECT_DAMAGE);
    }

    void HandleTick(AuraEffect const* eff)
    {
        if (Player* deathknight = GetOwner()->ToPlayer())
            if (DynamicObject* dnd = deathknight->GetDynObject(SPELL_DK_DEATH_AND_DECAY))
                deathknight->CastSpell(*dnd, SPELL_DK_DEATH_AND_DECAY_DAMAGE, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_and_decay::CalculateDamage, EFFECT_1, SPELL_AURA_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_death_and_decay::HandleTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 43265 - Death and Decay
class spell_dk_glyph_of_death_and_decay : public SpellScript
{
    PrepareSpellScript(spell_dk_glyph_of_death_and_decay);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_DEATH_AND_DECAY))
            GetCaster()->CastSpell(*GetExplTargetDest(), SPELL_DK_DEATH_AND_DECAY_SLOW, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dk_glyph_of_death_and_decay::HandleCast);
    }
};

// 47541 - Death Coil
// 122282 - Death Coil (symbiosis)
class spell_dk_death_coil : public SpellScript
{
    PrepareSpellScript(spell_dk_death_coil);

    void HandleHit(SpellEffIndex)
    {
        uint32 healSpell = GetCaster()->GetClass() == CLASS_DRUID ? SPELL_DRUID_DEATH_COIL_HEAL : SPELL_DK_DEATH_COIL_HEAL;
        uint32 dmgSpell = GetCaster()->GetClass() == CLASS_DRUID ? SPELL_DRUID_DEATH_COIL_DAMAGE : SPELL_DK_DEATH_COIL_DAMAGE;
        Unit* dk = GetCaster();
        Unit* target = GetHitUnit();
        if (dk->IsValidAssistTarget(target))
        {
            if (target->GetCreatureType() == CREATURE_TYPE_UNDEAD)
                dk->CastCustomSpell(healSpell, SPELLVALUE_BASE_POINT0, GetEffectValue() * 3.5f, target, true);
            else
            {
                int32 absorb = (GetEffectValue() + dk->GetTotalAttackPowerValue(BASE_ATTACK) * 0.514);
                if (AuraEffect const* mastery = dk->GetAuraEffect(SPELL_DK_MASTERY_DREAD_BLADE, EFFECT_0))
                    AddPct(absorb, mastery->GetFloatAmount());
                dk->CastCustomSpell(SPELL_DK_DEATH_BARRIER, SPELLVALUE_BASE_POINT0, absorb, target, true);
            }
        }
        else if (dk->IsValidAttackTarget(target))
            dk->CastCustomSpell(dmgSpell, SPELLVALUE_BASE_POINT0, GetEffectValue(), target, true);
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (caster->IsValidAttackTarget(target))
            {
                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
                if (target->GetTypeId() == TYPEID_PLAYER && !target->IsPvP())
                {
                    if (caster->GetTypeId() != TYPEID_PLAYER || !caster->ToPlayer()->duel || caster->ToPlayer()->duel->opponent != target)
                        return SPELL_FAILED_BAD_TARGETS;
                }
            }
            else if (caster->IsValidAssistTarget(target))
            {
                if (target->GetCreatureType() != CREATURE_TYPE_UNDEAD && !caster->HasAura(SPELL_DK_GLYPH_OF_DEATH_COIL))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_TARGET_MUST_BE_UNDEAD);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }
            }
            else
                return SPELL_FAILED_BAD_TARGETS;
        }
        else
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register()
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_death_coil::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 49222 - Bone Shield
class spell_dk_bone_shield_check : public SpellScript
{
    PrepareSpellScript(spell_dk_bone_shield_check);

    SpellCastResult CheckCast()
    {
        if (Aura* shield = GetCaster()->GetAura(GetSpellInfo()->Id))
            if (shield->GetCharges() > 6)
                return SPELL_FAILED_STRONGER_SPELL_IN_PROGRESS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_bone_shield_check::CheckCast);
    }
};

// 49222 - Bone Shield
// 122285 - Bone Shield (symbiosis)
class spell_dk_bone_shield : public AuraScript
{
    PrepareAuraScript(spell_dk_bone_shield);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
            return false;
        return eventInfo.GetDamageInfo()->GetDamage() > 0;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_bone_shield::CheckProc);
    }
};

// 123693 - Plague Leech
class spell_dk_plague_leech : public SpellScript
{
    PrepareSpellScript(spell_dk_plague_leech);

    SpellCastResult CheckRunes()
    {
        Unit* target = GetExplTargetUnit();
        Player* dk = GetCaster()->ToPlayer();
        if (target && dk)
        {
            uint8 diseases = target->GetDiseasesByCaster(GetCaster()->GetGUID());
            uint8 requiredDiseases = 2;
            if (diseases < requiredDiseases)
            {
                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_BOTH_FROST_FEVER_BLOOD_PLAGUE);
                return SPELL_FAILED_CUSTOM_ERROR;
            }
            for (uint8 i = 0; i < MAX_RUNES; ++i)
                if (dk->GetRune(i).State == RuneState::FullyDepleted)
                    return SPELL_CAST_OK;
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NO_FULLY_DEPLETED_RUNES);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_FAILED_DONT_REPORT;
    }

    void HandleCast()
    {
        Player* dk = GetCaster()->ToPlayer();
        if (!dk)
            return;

        int32 restored = 0;
        for (auto&& it : GetRunesPrioForSpec(dk))
        {
            int32 rune = GetRuneOnCd(dk, it);
            if (rune != -1)
            {
                dk->ActivateRune(uint8(rune));
                dk->ConvertRune(uint8(rune), RUNE_DEATH);
                ++restored;
                if (restored == 2)
                    break;
            }
        }

        dk->ResyncRunes(MAX_RUNES);
    }

    void HandleHit()
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_DK_BLOOD_PLAGUE, GetCaster()->GetGUID());
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_DK_FROST_FEVER, GetCaster()->GetGUID());
    }

    std::array<uint32, 3> GetRunesPrioForSpec(Player* dk)
    {
        switch (dk->GetTalentSpecialization())
        {
            case SPEC_DEATH_KNIGHT_BLOOD:
            case SPEC_DEATH_KNIGHT_FROST:
                return{ 2, 4, 0 };
            default:
                return{ 0, 2, 4 };
        }
    }

    int32 GetRuneOnCd(Player const* dk, uint8 rune)
    {
        if (dk->GetRune(rune).State == RuneState::FullyDepleted)
            return rune;
        if (dk->GetRune(rune + 1).State == RuneState::FullyDepleted)
            return rune + 1;
        return -1;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_plague_leech::CheckRunes);
        OnCast += SpellCastFn(spell_dk_plague_leech::HandleCast);
        OnHit += SpellHitFn(spell_dk_plague_leech::HandleHit);
    }
};

// 49530 - Sudden Doom
class spell_dk_sudden_doom : public AuraScript
{
    PrepareAuraScript(spell_dk_sudden_doom);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_sudden_doom::CheckProc);
    }
};

// 66192 - Threat of Thassarian
class spell_dk_threat_of_tassarian : public AuraScript
{
    PrepareAuraScript(spell_dk_threat_of_tassarian);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !GetUnitOwner()->HasOffhandWeapon())
            return;

        uint32 spell;
        switch (eventInfo.GetSpellInfo()->Id)
        {
            case 49020: spell = 66198; break; // Obliterate
            case 49143: spell = 66196; break; // Frost Strike
            case 45462: spell = 66216; break; // Plague Strike
            case 49998: spell = 66188; break; // Death Strike
            default:
                return;
        }
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), spell, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_threat_of_tassarian::HandleProc);
    }
};

// 55233 - Vampiric Blood
class spell_dk_vampiric_blood : public AuraScript
{
    PrepareAuraScript(spell_dk_vampiric_blood);

    void CalculateAmount(AuraEffect const* eff, float& amount, bool&)
    {
        amount = GetUnitOwner()->CountPctFromMaxHealth(amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_vampiric_blood::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_HEALTH);
    }
};

// 130736 - Soul Reaper (unholy), 130735 - Soul Reaper (frost), 114866 - Soul Reaper (blood)
class spell_dk_soul_reaper : public AuraScript
{
    PrepareAuraScript(spell_dk_soul_reaper);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* dk = GetCaster())
        {
            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode == AURA_REMOVE_BY_DEATH)
                dk->CastSpell(dk, SPELL_DK_SOUL_REAPER_HASTE, true);
            else if (removeMode == AURA_REMOVE_BY_EXPIRE)
            {
                int32 ptc = 35;
                if (AuraEffect* bonus = dk->GetAuraEffect(SPELL_DK_T15_DPS_4P_BONUS, EFFECT_0))
                    ptc = bonus->GetAmount();
                if (GetTarget()->HealthBelowPct(ptc))
                    dk->CastSpell(GetTarget(), SPELL_DK_SOUL_REAPER_DAMAGE, true);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_dk_soul_reaper::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 49184 - Howling Blast 
class spell_dk_howling_blast : public SpellScript
{
    PrepareSpellScript(spell_dk_howling_blast);

    void HandleHit()
    {
        if (GetHitUnit() != GetExplTargetUnit())
            SetHitDamage(GetHitDamage() / 2);

        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_FROST_FEVER, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_howling_blast::HandleHit);
    }
};

// 56835 - Reaping, 50034 - Blood Rites
class spell_dk_rune_converter : public AuraScript
{
    PrepareAuraScript(spell_dk_rune_converter);

    Player* dk = nullptr;

    bool Load() override
    {
        dk = GetOwner()->ToPlayer();
        return dk && dk->GetClass() == CLASS_DEATH_KNIGHT;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* bySpell = eventInfo.TriggeredBySpell();
        if (!bySpell)
            return false;

        SpellRuneCostEntry const* runeCostData = sSpellRuneCostStore.LookupEntry(bySpell->GetSpellInfo()->RuneCostID);
        if (!runeCostData)
            return false;

        for (uint8 i = 0; i < MAX_RUNES; ++i)
        {
            auto rune = dk->GetCurrentRune(i);
            if (rune != RUNE_DEATH && runeCostData->RuneCost[rune] > 0 && dk->GetLastUsedRuneMask() & (1 << i))
                return true;
        }

        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Spell const* bySpell = eventInfo.TriggeredBySpell();

        if (AuraEffect* eff = GetEffect(EFFECT_0))
        {
            SpellRuneCostEntry const* runeCostData = sSpellRuneCostStore.LookupEntry(bySpell->GetSpellInfo()->RuneCostID);

            // Reset amplitude - set death rune remove timer to 30s
            eff->ResetPeriodic(true);
            for (uint8 i = 0; i < MAX_RUNES; ++i)
            {
                auto rune = dk->GetCurrentRune(i);
                if (rune != RUNE_DEATH && runeCostData->RuneCost[rune] > 0 && dk->GetLastUsedRuneMask() & (1 << i))
                    dk->AddRuneBySpell(i, RUNE_DEATH, GetId());
            }
        }
    }

    void HandleTick(AuraEffect const*)
    {
        dk->RemoveRunesBySpell(GetId());
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        dk->RemoveRunesBySpell(GetId());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_rune_converter::CheckProc);
        OnProc += AuraProcFn(spell_dk_rune_converter::HandleProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_rune_converter::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_dk_rune_converter::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 50452 - Blood Parasite
class spell_dk_blood_parasite : public SpellScript
{
    PrepareSpellScript(spell_dk_blood_parasite);

    void SelectDest(SpellDestination& dest)
    {
        WorldObject* tar = GetExplTargetWorldObject();
        float angle = tar->GetAngle(GetCaster()) - tar->GetOrientation();
        tar->MovePosition(dest._position, tar->GetObjectSize(), frand(angle - M_PI / 2, angle + M_PI / 2));
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dk_blood_parasite::SelectDest, EFFECT_0, TARGET_DEST_TARGET_ENEMY);
    }
};

// 28017
struct npc_bloodworm : public ScriptedAI
{
    npc_bloodworm(Creature* c) : ScriptedAI(c) { }

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetCreateHealth(summoner->CountPctFromMaxHealth(15));
        me->SetMaxHealth(me->GetCreateHealth());
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->GetVictim())
        {
            if (Unit* owner = me->GetOwner())
                if (Unit* victim = owner->getAttackerForHelper())
                    AttackStart(victim);
        }
        if (me->GetVictim())
            DoMeleeAttackIfReady();
    }

    void SpellHit(Unit*, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_BLOOD_GORGED)
        {
            if (Aura* aura = me->GetAura(SPELL_BLOOD_GORGED))
            {
                if (roll_chance_f(aura->GetStackAmount() * 7.0f)) // wild 
                {
                    int32 heal = me->CountPctFromMaxHealth(aura->GetSpellInfo()->Effects[EFFECT_1].BasePoints) * aura->GetStackAmount();
                    me->CastCustomSpell(SPELL_BLOOD_BURST, SPELLVALUE_BASE_POINT0, heal, me, true);
                }
            }
        }
    }
};

// 49572 - Shadow Infusion
class spell_dk_shadow_infusion_proc : public AuraScript
{
    PrepareAuraScript(spell_dk_shadow_infusion_proc);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Pet* ghoul = GetOwner()->ToPlayer()->GetPet())
            if (ghoul->HasAura(SPELL_DK_DARK_TRANSFORMATION))
                return false;
        if (Battleground* bg = GetOwner()->GetBattlegorund())
            if (bg->GetStatus() == STATUS_WAIT_JOIN)
                return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_shadow_infusion_proc::CheckProc);
    }
};

// 91342 - Shadow Infusion
class spell_dk_shadow_infusion : public AuraScript
{
    PrepareAuraScript(spell_dk_shadow_infusion);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetStackAmount() == GetSpellInfo()->StackAmount)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DK_DARK_TRANSFORMATION_CASTER_AURA, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_DK_DARK_TRANSFORMATION_CASTER_AURA);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dk_shadow_infusion::HandleApply, EFFECT_2, SPELL_AURA_408, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_dk_shadow_infusion::HandleRemove, EFFECT_2, SPELL_AURA_408, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63560 - Dark transformation
class spell_dk_dark_transformation : public SpellScript
{
    PrepareSpellScript(spell_dk_dark_transformation);

    void HandleOnHit()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_DK_DARK_INFUSION_STACKS);
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_DK_DARK_INFUSION_STACKS);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_dark_transformation::HandleOnHit);
    }
};

// 47481 - Gnaw, 47468 - Claw, 47484 - Huddle, 47482 - Leap
class spell_dk_ghoul_ability : public SpellScript
{
    PrepareSpellScript(spell_dk_ghoul_ability);

    void HandleHit()
    {
        struct SpellPair { uint32 Normal, DarkTrasformation; };
        std::map<uint32, SpellPair> const map =
        {
            { 47481, { 91800, 91797 } },    // Gnaw & Monstrous Blow
            { 47468, { 91776, 91778 } },    // Claw & Sweeping Claws
            { 47484, { 91838, 91837 } },    // Huddle & Putrid Bulwark
            { 47482, { 91809, 91802 } },    // Leap & Shambling Rush
        };
        auto it = map.find(m_scriptSpellId);
        if (it != map.end())
        {
            Unit* ghoul = GetCaster();
            uint32 spell = ghoul->HasAura(SPELL_DK_DARK_TRANSFORMATION) ? it->second.DarkTrasformation : it->second.Normal;
            ghoul->CastSpell(GetHitUnit(), spell, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_ghoul_ability::HandleHit);
    }
};

// 46584 - Raise Dead
class spell_dk_raise_dead : public SpellScript
{
    PrepareSpellScript(spell_dk_raise_dead);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_DONT_REPORT;
        if (Unit* pet = GetCaster()->ToPlayer()->GetGuardianPet())
            if (pet->IsAlive())
                return SPELL_FAILED_ALREADY_HAVE_SUMMON;
        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        int32 index = GetCaster()->HasAura(SPELL_DK_MASTER_OF_GHOULS) ? EFFECT_1 : EFFECT_0;
        GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[index].BasePoints, true);
        if (GetCaster()->HasAura(58640))
        {
            if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_DEATH_KNIGHT_UNHOLY)
            {
                if (Unit* pet = GetCaster()->ToPlayer()->GetPet())
                    pet->CastSpell(pet, 121916, true); // Geist Transform
            }
            else
            {
                if (Unit* minion = GetCaster()->ToPlayer()->GetFirstMinionByEntry(26125))
                    minion->CastSpell(minion, 121916, true); // Geist Transform
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_raise_dead::CheckCast);
        OnCast += SpellCastFn(spell_dk_raise_dead::HandleCast);
    }
};

// 58640 Glyph of the Geist
class aura_glyph_of_the_geist : public AuraScript
{
    PrepareAuraScript(aura_glyph_of_the_geist);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_DEATH_KNIGHT_UNHOLY)
        {
            Unit* pet = GetCaster()->ToPlayer()->GetPet();
            if (pet && pet->GetEntry() == 26125)
                pet->CastSpell(pet, 121916, true); // Geist Transform
        }
        else
        {
            if (Unit* minion = GetCaster()->ToPlayer()->GetFirstMinionByEntry(26125))
                minion->CastSpell(minion, 121916, true); // Geist Transform
        }
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_DEATH_KNIGHT_UNHOLY)
        {
            Unit* pet = GetCaster()->ToPlayer()->GetPet();
            if (pet && pet->GetEntry() == 26125)
                pet->RemoveAurasDueToSpell(121916);
        }
        else
        {
            if (Unit* minion = GetCaster()->ToPlayer()->GetFirstMinionByEntry(26125))
                minion->RemoveAurasDueToSpell(121916);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(aura_glyph_of_the_geist::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(aura_glyph_of_the_geist::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108196 - Death Siphon
class spell_dk_death_siphon : public SpellScript
{
    PrepareSpellScript(spell_dk_death_siphon);

    void HandleHit()
    {
        int32 damage = GetHitDamage();
        if (Player* target = GetHitUnit()->GetAffectingPlayer())
            if (Player* caster = GetCaster()->GetAffectingPlayer())
                damage /= (1.0f + target->GetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT) / 100.0f);

        if (int32 heal = CalculatePct(damage, GetSpellInfo()->Effects[EFFECT_1].BasePoints))
            GetCaster()->CastCustomSpell(SPELL_DK_DEATH_SIPHON_HEAL, SPELLVALUE_BASE_POINT0, heal, GetCaster(), true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dk_death_siphon::HandleHit);
    }
};

// 59327 - Glyph of Unholy Command
class spell_dk_glyph_of_unholy_command : public AuraScript
{
    PrepareAuraScript(spell_dk_glyph_of_unholy_command);

    void HandleProc(ProcEventInfo&)
    {
        if (Player* dk = GetOwner()->ToPlayer())
            dk->RemoveSpellCooldown(SPELL_DK_DEATH_GRIP, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_glyph_of_unholy_command::HandleProc);
    }
};

// 48263 - Blood Presence, 48266 - Frost Presence, 48265 - Unholy Presence
class spell_dk_presence : public SpellScript
{
    PrepareSpellScript(spell_dk_presence);

    void ResetRunicPower()
    {
        uint32 amount = 0;
        if (Aura* glyph = GetCaster()->GetAura(SPELL_DK_GLYPH_OF_SHIFTING_PRESENCES))
            amount = CalculatePct(GetCaster()->GetPower(POWER_RUNIC_POWER), glyph->GetSpellInfo()->Effects[EFFECT_0].BasePoints);
        GetCaster()->SetPower(POWER_RUNIC_POWER, amount);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dk_presence::ResetRunicPower);
    }
};

// 48721 - Blood Boil
class spell_dk_blood_boil : public SpellScript
{
    PrepareSpellScript(spell_dk_blood_boil);

    void HandleHit()
    {
        Unit* caster = GetCaster();
        Player* deathknight = caster->GetAffectingPlayer();
        if (!deathknight)
            return;

        Unit* target = GetHitUnit();

        if (deathknight->HasAura(DK_SPELL_SCARLET_FEVER))
        {
            caster->CastSpell(target, DK_SPELL_WEAKENED_BLOWS, true);

            if (Aura* aura = target->GetAura(SPELL_DK_BLOOD_PLAGUE, caster->GetGUID()))
                aura->RefreshDuration();
            if (Aura* aura = target->GetAura(SPELL_DK_FROST_FEVER, caster->GetGUID()))
                aura->RefreshDuration();
        }
        if (deathknight->HasAura(SPELL_DK_FESTERING_BLOOD) ||
            target->HasAura(SPELL_DK_FROST_FEVER) || target->HasAura(SPELL_DK_BLOOD_PLAGUE))
        {
            SetHitDamage(int32(GetHitDamage() * 1.5f));

            if (deathknight->HasAura(SPELL_DK_ROILING_BLOOD))
                caster->CastSpell(target, SPELL_DK_PESTILENCE_BLOOD_BOIL, true);
        }

        // Dancing rune weapon
        if (caster == deathknight)
            for (auto&& itr : caster->GetSummons())
                if (itr->GetEntry() == 27893)
                    itr->CastSpell(itr, GetSpellInfo()->Id);
    }

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_DK_BLOOD_BOIL_TRIGGERED, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_blood_boil::HandleHit);
    }
};

// 108194 - Asphyxiate
class spell_dk_asphyxiate : public SpellScript
{
    PrepareSpellScript(spell_dk_asphyxiate);

    void HandleHit()
    {
        if (GetSpell()->GetDiminishingLevel() != DIMINISHING_LEVEL_IMMUNE && !GetHitUnit()->HasUnitState(UNIT_STATE_STUNNED))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_STRANGULATE, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dk_asphyxiate::HandleHit);
    }
};

// 49206 - Summon Gargoyle
class spell_dk_summon_gargoyle : public SpellScript
{
    PrepareSpellScript(spell_dk_summon_gargoyle);

    void SelectDest(SpellDestination& dest)
    {
        dest._position.RelocateOffset(M_PI, 10, 10);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dk_summon_gargoyle::SelectDest, EFFECT_0, TARGET_DEST_CASTER_FRONT_LEFT);
    }
};

// 27829
struct npc_ebon_gargoyle : ScriptedAI
{
    npc_ebon_gargoyle(Creature* creature) : ScriptedAI(creature) { }

    enum Points
    {
        POINT_DESCEND   = 1,
        POINT_ASCEND    = 2,
    };

    uint32 m_forcedAggroTimer = 3000;
    float m_originalSpeedRate;
    Position m_originalPosition;
    TaskScheduler m_scheduler;
    CasterMovement m_casterMovement = CasterMovement(me).Spell(SPELL_DK_GARGOYLE_STRIKE);
    AssistBehavior m_assist{ me };

    void InitializeAI() override
    {
        Unit* owner = me->GetOwner();
        if (!owner)
            return;

        static uint32 const buffImmunity[] = { 5697, 10060, 53480 };
        for (auto&& buff : buffImmunity)
            me->ApplySpellImmune(0, IMMUNITY_ID, buff, true);

        // Prepare for descend animation
        m_originalSpeedRate = me->GetSpeedRate(MOVE_RUN);
        m_originalPosition = me->GetPosition();
        m_originalPosition.RelocateOffset(M_PI, -10.0f, -10.0f);
        me->SetFlying(true);
        me->SetSpeed(MOVE_RUN, 0.75f, true);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_scheduler.SetValidator([=]() { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void IsSummonedBy(Unit* summoner) override
    {
        me->GetMotionMaster()->MovePoint(POINT_DESCEND, m_originalPosition.GetPositionX(), m_originalPosition.GetPositionY(), m_originalPosition.GetPositionZ(), false, MOTION_SLOT_CRITICAL);
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type == POINT_MOTION_TYPE && point == POINT_DESCEND && m_forcedAggroTimer)
        {
            m_forcedAggroTimer = 0;
            me->SetFlying(false);
            me->SetSpeed(MOVE_RUN, m_originalSpeedRate, true);
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            std::list<Unit*> targets;
            Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30);
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
            me->VisitNearbyObject(30, searcher);
            targets.remove_if(Trinity::UnitAuraCheck{ false, SPELL_DK_SUMMON_GARGOYLE });

            if (!targets.empty() && ShouldAttackTarget(targets.front()))
                AttackStart(targets.front());
        }
    }

    void DoSearchTarget()
    {
        Unit* owner = me->GetOwner();
        if (!owner)
            return;

        // Find victim of Summon Gargoyle spell
        std::list<Unit*> targets;
        if (Unit* assistTarget = owner->getAttackerForHelper())
            targets.push_back(assistTarget);
        Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30);
        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
        me->VisitNearbyObject(30, searcher);
        for (auto&& target : targets)
        {
            if (!ShouldAttackTarget(target))
                continue;

            if (!target->HasAura(SPELL_DK_SUMMON_GARGOYLE, owner->GetGUID()) && !owner->AddAura(SPELL_DK_SUMMON_GARGOYLE, target))
                continue;

            AttackStart(target);
            break;
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        m_scheduler.Schedule(Milliseconds(1), std::bind(&npc_ebon_gargoyle::DoAttack, this, std::placeholders::_1));
    }

    void EnterEvadeMode() override { }

    void JustDied(Unit*) override
    {
        // Stop Feeding Gargoyle when it dies
        if (Unit* owner = me->GetOwner())
            owner->RemoveAurasDueToSpell(SPELL_DK_SUMMON_GARGOYLE_FEED);
    }

    void AttackStart(Unit *target) override
    {
        if (m_forcedAggroTimer)
            return;

        if (target && !target->HasAura(SPELL_DK_SUMMON_GARGOYLE))
        {
            if (!ShouldAttackTarget(me->GetVictim()))
            {
                DoSearchTarget();
                return;
            }
        }

        if (target && me->Attack(target, false))
            m_casterMovement.Chase(target);
    }

    void SpellHit(Unit* source, SpellInfo const* spell) override
    {
        // Fly away when dismissed
        if (spell->Id != SPELL_DK_DISMISS_GARGOYLE || !me->IsAlive())
            return;

        Unit* owner = me->GetOwner();

        if (!owner || owner != source)
            return;

        // Stop Fighting
        me->InterruptNonMeleeSpells(false);
        me->AttackStop();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->CastSpell(me, SPELL_SANCTUARY, true);
        me->SetReactState(REACT_PASSIVE);

        // Fly Away
        me->SetFlying(true);
        me->SetSpeed(MOVE_RUN, 0.75f, true);
        Position pos = me->GetPosition();
        pos.RelocateOffset(0, 20, 40);
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MovePoint(POINT_ASCEND, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), false, MOTION_SLOT_CRITICAL);

        // Despawn as soon as possible
        me->DespawnOrUnsummon(4 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_forcedAggroTimer)
        {
            if (m_forcedAggroTimer <= diff)
            {
                MovementInform(POINT_MOTION_TYPE, POINT_DESCEND);
                if (me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CRITICAL))
                    me->GetMotionMaster()->MovementExpired();
            }
            else
                m_forcedAggroTimer -= diff;
        }

        if (me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CRITICAL) || me->HasUnitState(UNIT_STATE_CONTROLLED))
            return;

        if (!ShouldAttackTarget(me->GetVictim()))
        {
            me->InterruptNonMeleeSpells(false);
            me->AttackStop();
            DoSearchTarget();
        }

        m_assist.Update(diff);
        m_casterMovement.Update(diff); // Before UpdateVictim() to allow returning to owner

        if (!UpdateVictim())
            return;

        if (!ShouldAttackTarget(me->GetVictim()))
        {
            me->InterruptNonMeleeSpells(false);
            return;
        }

        m_scheduler.Update(diff);
    }

    void DoAttack(TaskContext ctx)
    {
        if (me->HasSpellCooldown(SPELL_DK_GARGOYLE_STRIKE))
        {
            ctx.Repeat(Milliseconds(500));
            return;
        }
        DoCastVictim(SPELL_DK_GARGOYLE_STRIKE);
        uint32 casttime = me->GetCurrentSpellCastTime(SPELL_DK_GARGOYLE_STRIKE);
        uint32 gcd = std::max(1000, std::min(1500, int32(float(1500) * me->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED))));
        if (casttime < gcd)
            casttime = gcd;
        auto timer = Milliseconds((casttime ? casttime : 500) + GetAISpellInfo(SPELL_DK_GARGOYLE_STRIKE)->realCooldown);
        ctx.Repeat(timer);
    }

    bool ShouldAttackTarget(Unit* target)
    {
        if (!target)
            return false;

        if (target->GetSchoolImmunityMask() == SPELL_SCHOOL_MASK_ALL)
            return false;

        if (target->HasBreakableByDamageCrowdControlAura(me))
            return false;

        return me->IsValidAttackTarget(target);
    }
};

// 51963 - Gargoyle Strike
class spell_dk_gargoyle_strike : public SpellScript
{
    PrepareSpellScript(spell_dk_gargoyle_strike);

    void HandleHit()
    {
        if (Unit* dk = GetCaster()->GetOwner())
        {
            if (AuraEffect const* eff = dk->GetAuraEffect(SPELL_DK_MASTERY_DREAD_BLADE, EFFECT_0))
            {
                int32 damage = GetHitDamage();
                AddPct(damage, eff->GetFloatAmount());
                SetHitDamage(damage);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_gargoyle_strike::HandleHit);
    }
};

// 131542 - Relentless Grip
class spell_dk_relentless_grip : public AuraScript
{
    PrepareAuraScript(spell_dk_relentless_grip);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DK_RELENTLESS_GRIP, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_relentless_grip::HandleProc);
    }
};

// 77606 - Dark Simulacrum
class spell_dk_dark_simulacrum : public AuraScript
{
    PrepareAuraScript(spell_dk_dark_simulacrum);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();
        if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
            return false;

        Spell const* spell = eventInfo.TriggeredBySpell();
        if (!spell)
            return false;

        if (!spell->m_cast_count)   // Spell was not casted directly
            return false;

        if (spell->GetPowerType() != POWER_MANA)    // Only spells which costs mana
            return false;

        // But no guarantee, POWER_MANA returns also if no cost, need to fix this
        bool found = false;
        for (auto&& it : spell->GetSpellInfo()->SpellPowers)
            if (it->PowerType == POWER_MANA)
                found = true;
        if (!found)
            return false;

        // Against nonplayers, only absorbs some harmful spells. ¯\_(ツ)_/¯
        if (Creature* spellCaster = GetUnitOwner()->ToCreature())
            if (spellCaster->IsDungeonBoss() || spellCaster->isWorldBoss() || spellCaster->GetCreatureTemplate()->rank == CREATURE_ELITE_WORLDBOSS)
                return false;

        SpellInfo const* spellInfo = spell->GetSpellInfo();

        if (spellInfo->GetCategory() == 47) // Combat states
            return false;

        if (spellInfo->IsPassive() || spell->GetSpellInfo()->GetMaxDuration() == -1) // Permanent buffs
            return false;

        if (!caster->ToPlayer()->HasItemFitToSpellRequirements(spellInfo))
            return false;

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_MAIN_HAND)
        {
            Item* item = caster->ToPlayer()->GetWeaponForAttack(BASE_ATTACK);
            if (!item || !item->IsFitToSpellRequirements(spellInfo))
                return false;
        }

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_REQ_OFFHAND)
        {
            Item* item = caster->ToPlayer()->GetWeaponForAttack(OFF_ATTACK);
            if (!item->IsFitToSpellRequirements(spellInfo))
                return false;
        }
        
        for (auto&& effect : spell->GetSpellInfo()->Effects)
        {
            switch (effect.Effect)
            {
                case SPELL_EFFECT_SUMMON:
                case SPELL_EFFECT_SUMMON_PET:
                case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
                case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
                case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
                case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
                case SPELL_EFFECT_SUMMON_OBJECT_WILD:
                    return false;
                default:
                    break;
            }
            switch (effect.ApplyAuraName)
            {
                case SPELL_AURA_MOD_SHAPESHIFT:
                    return false;
                default:
                    break;
            }
        }
        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* dk = GetCaster())
        {
            uint32 overrideSpellId = eventInfo.GetSpellInfo()->Id;
            uint32 spd = GetUnitOwner()->SpellBaseDamageBonusDone(SpellSchoolMask(eventInfo.GetSpellInfo()->SchoolMask));
            dk->CastCustomSpell(SPELL_DK_DARK_SIMULACRUM_OVERRIDE, SPELLVALUE_BASE_POINT0, overrideSpellId, dk, true);
            CustomSpellValues ctx;
            ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, spd);
            ctx.AddSpellMod(SPELLVALUE_BASE_POINT1, spd);
            dk->CastCustomSpell(SPELL_DK_DARK_SIMULACRUM_SPD, ctx, dk, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_dark_simulacrum::CheckProc);
        OnProc += AuraProcFn(spell_dk_dark_simulacrum::HandleProc);
    }
};

// 49028 - Dancing Rune Weapon
class spell_dk_dancing_rune_weapon : public SpellScript
{
    PrepareSpellScript(spell_dk_dancing_rune_weapon);

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), 81256, true);
        for (auto&& itr : GetCaster()->GetSummons())
        {
            if (itr->GetEntry() == GetSpellInfo()->Effects[EFFECT_0].MiscValue)
            {
                itr->CastSpell(itr, 53160, true);           //
                GetCaster()->CastSpell(itr, 63416, true);   // Copy Weapon
                itr->CastSpell(itr, 51906, true);
                itr->AI()->AttackStart(GetExplTargetUnit());
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dk_dancing_rune_weapon::HandleCast);
    }
};

// 49028 - Dancing Rune Weapon
class spell_dk_dancing_rune_weapon_driver : public AuraScript
{
    PrepareAuraScript(spell_dk_dancing_rune_weapon_driver);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!eventInfo.TriggeredBySpell())
            return;

        if (eventInfo.TriggeredBySpell()->IsTriggered() || eventInfo.GetSpellInfo()->IsPositive())
            return;

        if (eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() != eventInfo.GetActionTarget())
            return;

        for (auto&& itr : eventInfo.GetSpellInfo()->Effects)
        {
            if (itr.Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA || itr.IsAreaAuraEffect() || itr.IsTargetingArea())
                return;
        }

        for (auto&& itr : GetUnitOwner()->GetSummons())
            if (itr->GetEntry() == GetSpellInfo()->Effects[EFFECT_0].MiscValue)
                itr->CastSpell(itr->GetVictim(), eventInfo.GetSpellInfo()->Id, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_dancing_rune_weapon_driver::HandleProc);
    }
};

// 51906 - Death Knight Rune Weapon Scaling
class spell_dk_rune_weapon_scaling : public AuraScript
{
    PrepareAuraScript(spell_dk_rune_weapon_scaling);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* dk = caster->GetOwner())
            {
                float mult = dk->GetTotalAuraMultiplier(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                if (AuraEffect const* glyph = dk->GetAuraEffect(63330, EFFECT_1))
                    AddPct(mult, glyph->GetAmount());
                amount = (mult - 1.0f) * 100;
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_rune_weapon_scaling::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

// 51462 - Runic Corruption
class spell_dk_runic_corruption : public AuraScript
{
    PrepareAuraScript(spell_dk_runic_corruption);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), 51460, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_runic_corruption::HandleProc);
    }
};

// 51124 - Killing Machine
class spell_dk_t15_dps_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dk_t15_dps_4p_bonus);

    void CalculateSpellMod(AuraEffect const*, SpellModifier*& spellmod)
    {
        if (spellmod && GetUnitOwner()->HasAura(SPELL_DK_T15_DPS_4P_BONUS))
            spellmod->mask[2] |= 0x04000000;    // Soul Reaper
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_dk_t15_dps_4p_bonus::CalculateSpellMod, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 69880
struct npc_dk_fallen_zandaliari : public ScriptedAI
{
    enum { SPELL_STRIKE = 138537 };

    npc_dk_fallen_zandaliari(Creature* c) : ScriptedAI(c) { }

    AssistBehavior assist{ me };

    void IsSummonedBy(Unit*) override
    {
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
        if (me->GetPower(POWER_ENERGY) >= 40)
            DoCastVictim(SPELL_STRIKE);
    }
};

// 49222 - Bone Shield
class spell_dk_t15_blood_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dk_t15_blood_4p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (GetUnitOwner()->HasAura(SPELL_DK_T15_BLOOD_4P_BONUS))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DK_RUNIC_POWER, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_t15_blood_4p_bonus::HandleProc);
    }
};

// 144901 - Death Shroud
class spel_dk_death_shroud_duration : public SpellScript
{
    PrepareSpellScript(spel_dk_death_shroud_duration);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleHit()
    {
        if (Aura* aura = GetHitAura())
        {
            if (GetCaster()->ToPlayer()->GetSpecialization() == SPEC_DEATH_KNIGHT_UNHOLY)
                aura->SetMaxDuration(15000);
            else
            {
                int32 hnd = 1;
                auto weapon = GetCaster()->ToPlayer()->GetWeaponForAttack(BASE_ATTACK);
                if (weapon && weapon->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
                    hnd = 2;

                aura->SetMaxDuration((hnd * 2 + 4) * IN_MILLISECONDS);
            }

            aura->SetDuration(aura->GetMaxDuration());
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spel_dk_death_shroud_duration::HandleHit);
    }
};

// 144901 - Death Shroud
class spell_dk_death_shroud : public AuraScript
{
    PrepareAuraScript(spell_dk_death_shroud);

    bool haste = false;

    bool Load() override
    {
        if (GetOwner()->GetTypeId() != TYPEID_PLAYER)
            return false;

        haste = GetOwner()->ToPlayer()->GetRatingBonusValue(CR_HASTE_MELEE) > GetOwner()->ToPlayer()->GetRatingBonusValue(CR_MASTERY);
        return true;
    }

    void CalculateHaste(AuraEffect const*, float& amount, bool&)
    {
        if (!haste)
            amount = 0.0f;
    }

    void CalculateMastery(AuraEffect const*, float& amount, bool&)
    {
        if (haste)
            amount = 0.0f;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_shroud::CalculateHaste, EFFECT_0, SPELL_AURA_MOD_RATING);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_shroud::CalculateMastery, EFFECT_1, SPELL_AURA_MOD_RATING);
    }
};

// 144907 - Item - Death Knight T16 DPS 4P Bonus
class spell_dk_t16_dps_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dk_t16_dps_4p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_DK_DEATH_COIL)
        {
            if (Unit* ghoul = GetUnitOwner()->GetGuardianPet())
            {
                if (Aura* darkTransformation = ghoul->GetAura(SPELL_DK_DARK_TRANSFORMATION))
                {
                    int32 duration = darkTransformation->GetDuration();
                    duration += 2000;
                    if (darkTransformation->GetMaxDuration() < duration)
                        darkTransformation->SetMaxDuration(duration);
                    darkTransformation->SetDuration(duration);
                }
            }
        }
        else if (eventInfo.GetSpellInfo()->Id == SPELL_DK_PILLAR_OF_FROST)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_DK_T16_DPS_4P_BONUS_HIDDEN, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_t16_dps_4p_bonus::HandleProc);
    }
};

// 144934 - Item - Death Knight T16 Blood 2P Bonus
class spell_dk_t16_blood_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dk_t16_blood_2p_bonus);

    uint32 counter = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* deathknight = GetUnitOwner();
        if (++counter == 10 && deathknight->HasAura(SPELL_DK_T16_BLOOD_2P_BONUS))
        {
            counter = 0;
            deathknight->CastSpell(deathknight, SPELL_DK_BONE_WALL, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_t16_blood_2p_bonus::HandleProc);
    }
};

// 144950 - Item - Death Knight T16 Blood 4P Bonus
class spell_dk_t16_blood_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dk_t16_blood_4p_bonus);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER && GetUnitOwner()->GetClass() == CLASS_DEATH_KNIGHT;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* deathknight = GetUnitOwner()->ToPlayer();
        for (uint32 rune = 0; rune < MAX_RUNES; ++rune)
        {
            auto runeType = deathknight->GetRune(rune).BaseRune;
            if (runeType == RUNE_FROST || runeType == RUNE_UNHOLY)
            {
                deathknight->ActivateRune(rune);
                deathknight->ConvertRune(rune, RUNE_DEATH);
            }
        }

        deathknight->ResyncRunes(MAX_RUNES);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_dk_t16_blood_4p_bonus::HandleProc);
    }
};

// 147620 - Frozen Power
class spell_dk_frozen_power : public SpellScript
{
    PrepareSpellScript(spell_dk_frozen_power);

    void HandleHit()
    {
        if (Player* deathknight = GetCaster()->ToPlayer())
            if (Item const* weapon = deathknight->GetWeaponForAttack(BASE_ATTACK))
                if (weapon->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
                    return;

        SetHitDamage(GetHitDamage() / 2);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_frozen_power::HandleHit);
    }
};

// 50842 - Pestilence
// 116617 - Pestilence
class spell_dk_pestilence : public SpellScript
{
    PrepareSpellScript(spell_dk_pestilence);

    void HandleHit(SpellEffIndex)
    {
        Unit* hitTarget = GetHitUnit();
        Unit* mainTarget = GetExplTargetUnit();
        if (hitTarget != mainTarget)
        {
            bool visual = false;
            if (mainTarget->HasAura(SPELL_DK_BLOOD_PLAGUE))
            {
                GetCaster()->CastSpell(hitTarget, SPELL_DK_BLOOD_PLAGUE, true);
                visual = true;
            }
            if (mainTarget->HasAura(SPELL_DK_FROST_FEVER))
            {
                GetCaster()->CastSpell(hitTarget, SPELL_DK_FROST_FEVER, true);
                visual = true;
            }
            if (visual)
                mainTarget->CastSpell(hitTarget, SPELL_DK_PESTILENCE_VISUAL, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_pestilence::HandleHit, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 47476 - Strangulate
class spell_dk_glyph_of_strangulate : public SpellScript
{
    PrepareSpellScript(spell_dk_glyph_of_strangulate);

    bool m_interrupted = false;

    void Prepare()
    {
        if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_STRANGULATE))
            for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                if (Spell* spell = GetHitUnit()->GetCurrentSpell(CurrentSpellTypes(i)))
                    if (spell->GetSpellInfo()->PreventionType & SPELL_PREVENTION_TYPE_SILENCE)
                        m_interrupted = true;
    }

    void HandleHit()
    {
        if (m_interrupted)
        {
            if (Aura* strangulate = GetHitAura())
            {
                strangulate->SetMaxDuration(strangulate->GetMaxDuration() + 2000);
                strangulate->SetDuration(strangulate->GetDuration() + 2000);
            }
        }
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_dk_glyph_of_strangulate::Prepare);
        AfterHit += SpellHitFn(spell_dk_glyph_of_strangulate::HandleHit);
    }
};

// 81164 - Will of the Necropolis
class spell_dk_will_of_the_necropolis : public AuraScript
{
    PrepareAuraScript(spell_dk_will_of_the_necropolis);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HealthBelowPctDamaged(30, eventInfo.GetDamageInfo()->GetDamage());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_will_of_the_necropolis::CheckProc);
    }
};

// 96171 - Will of the Necropolis
class spell_dk_will_of_the_necropolis_rune_tap : public SpellScript
{
    PrepareSpellScript(spell_dk_will_of_the_necropolis_rune_tap);

    void HandleCast()
    {
        if (Player* dk = GetCaster()->ToPlayer())
            dk->RemoveSpellCooldown(SPELL_DK_RUNE_TAP, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dk_will_of_the_necropolis_rune_tap::HandleCast);
    }
};

// 81136 - Crimson Scourge
class spell_dk_crimson_scourge : public AuraScript
{
    PrepareAuraScript(spell_dk_crimson_scourge);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActionTarget() && eventInfo.GetActionTarget()->HasAura(SPELL_DK_BLOOD_PLAGUE, GetCasterGUID());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_crimson_scourge::CheckProc);
    }
};

// 113517 - Wild Mushroom: Plague
class spell_dk_wild_mushroom_plague_periodic : public AuraScript
{
    PrepareAuraScript(spell_dk_wild_mushroom_plague_periodic);

    void HandleTick(AuraEffect const*)
    {
        Unit* totem = GetUnitOwner();
        if (Unit* dk = totem->GetOwner())
            dk->CastSpell(totem, SPELL_DK_WILD_MUSHROOM_PLAGUE_TRIGGER, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_wild_mushroom_plague_periodic::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 113518 - Wild Mushroom: Plague
class spell_dk_wild_mushroom_plague_trigger : public SpellScript
{
    PrepareSpellScript(spell_dk_wild_mushroom_plague_trigger);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_BLOOD_PLAGUE, true);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_FROST_FEVER, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_wild_mushroom_plague_trigger::HandleHit);
    }
};

// 24207
struct npc_dk_army_of_the_dead_ghoul : public ScriptedAI
{
    npc_dk_army_of_the_dead_ghoul(Creature* c) : ScriptedAI(c) { }

    enum { SPELL_CLAW = 91776, };

    TaskScheduler scheduler;
    AssistBehavior assist{ me };
    float followAngle = M_PI / 2.0f;

    void InitializeAI() override
    {
        me->SetPowerType(POWER_ENERGY);
        if (me->HasUnitTypeMask(UNIT_MASK_MINION))
        {
            auto ghoul = ((Minion*)me);
            Unit* dk = ghoul->GetOwner();
            uint32 count = 0;
            for (auto&& itr : dk->GetSummons())
                if (itr->GetEntry() == me->GetEntry())
                    ++count;
            followAngle = M_PI / 2.0f + M_PI * count / 7.0f;
        }

        scheduler.Schedule(Milliseconds(100), [=](TaskContext ctx)
        {
            if (me->GetPower(POWER_ENERGY) >= 40 && me->GetVictim() && me->IsWithinMeleeRange(me->GetVictim()))
            {
                DoCastVictim(SPELL_CLAW);
                ctx.Repeat(Milliseconds(1500));
            }
            else
                ctx.Repeat(Milliseconds(100));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void EnterEvadeMode() override
    {
        if (!_EnterEvadeMode())
            return;

        if (me->HasUnitTypeMask(UNIT_MASK_MINION))
        {
            auto ghoul = ((Minion*)me);
            ghoul->SetFollowAngle(followAngle);
        }

        if (Unit* owner = me->GetCharmerOrOwner())
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, 3.0f, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
        else
        {
            me->AddUnitState(UNIT_STATE_EVADE);
            me->GetMotionMaster()->MoveTargetedHome();
        }

        Reset();
    }
};

// 146652 - Glyph of the Skeleton
class spell_dk_glyph_of_the_skeleton : public AuraScript
{
    PrepareAuraScript(spell_dk_glyph_of_the_skeleton);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Guardian* ghoul = GetUnitOwner()->GetGuardianPet())
            ghoul->CastSpell(ghoul, SPELL_DK_SKELETON_TRANSFORM, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Guardian* ghoul = GetUnitOwner()->GetGuardianPet())
            ghoul->RemoveAurasDueToSpell(SPELL_DK_SKELETON_TRANSFORM);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dk_glyph_of_the_skeleton::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_dk_glyph_of_the_skeleton::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 48707 - Anti-Magic Shell
class spell_dk_glyph_of_regenerative_magic : public AuraScript
{
    PrepareAuraScript(spell_dk_glyph_of_regenerative_magic);

    float amount = 0.0f;

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        amount = effect->GetAmount();
    }

    void HandleRemove(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (AuraEffect const* glyph = GetUnitOwner()->GetAuraEffect(SPELL_DK_GLYPH_OF_REGENERATIVE_MAGIC, EFFECT_0))
        {
            float pct = CalculatePct(effect->GetAmount() / amount, glyph->GetAmount()) * 100;
            int32 delay = GetUnitOwner()->GetSpellHistory()->GetCooldownDelay(GetId());
            ApplyPct(delay, pct);
            if (delay > 0)
                GetUnitOwner()->GetSpellHistory()->ModifyCooldown(GetId(), -delay);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dk_glyph_of_regenerative_magic::HandleApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_dk_glyph_of_regenerative_magic::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 55078 - Blood Plague
// 55095 - Frost Fever
class spell_dk_tricks_of_the_trade_fix : public AuraScript
{
    PrepareAuraScript(spell_dk_tricks_of_the_trade_fix);

    void CalcuateAmount(AuraEffect const*, float& amount, bool&)
    {
        // http://eu.battle.net/wow/en/game/patch-notes/5-3-0
        // Blood Plague no longer benefits from the damage increase granted by Tricks of the Trade.
        // Frost Fever no longer benefits from the damage increase granted by Tricks of the Trade.
        if (Unit* caster = GetCaster())
            for (auto&& itr : caster->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE))
                if (itr->GetSpellInfo()->Id == SPELL_ROGUE_TRICKS_OF_THE_TRADE)
                    amount = std::ceil(amount / (1.0f + std::abs(itr->GetAmount()) / 100.0f));
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_tricks_of_the_trade_fix::CalcuateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 49509 - Scent of Blood
class spell_dk_scent_of_blood_main_hand : public AuraScript
{
    PrepareAuraScript(spell_dk_scent_of_blood_main_hand);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_scent_of_blood_main_hand::CheckProc);
    }
};

// 57330 - Horn of Winter
class spell_dk_glyph_of_the_loud_horn : public SpellScript
{
    PrepareSpellScript(spell_dk_glyph_of_the_loud_horn);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_DK_GLYPH_OF_THE_LOUD_HORN))
            GetCaster()->CastSpell(GetCaster(), SPELL_DK_GLYPH_OF_THE_LOUD_HORN_ENERGIZE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dk_glyph_of_the_loud_horn::HandleCast);
    }
};

// 1193. 51052 - Anti-Magic Zone
class sat_dk_anti_magic_zone : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* target) override
    {
        return target->ToUnit() && GetCaster()->IsInRaidWith(target->ToUnit()) && GetTarget()->IsInDist(target, m_range);
    }

    void OnTriggeringApply(WorldObject* target) override
    {
        target->ToUnit()->CastSpell(target->ToUnit(), SPELL_DK_ANTI_MAGIC_ZONE, true);
    }

    void OnTriggeringRemove(WorldObject* target) override
    {
        target->ToUnit()->RemoveAurasDueToSpell(SPELL_DK_ANTI_MAGIC_ZONE);
    }
};

// 53822 - Death Gate
class spell_dk_death_gate_eff : public SpellScript
{
    PrepareSpellScript(spell_dk_death_gate_eff);

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
            if (player->GetAreaId() != 4281)
                player->SaveRecallPosition();
    }

    void SelectDest(SpellDestination& dest)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetAreaId() == 4281)
            {
                Position pos = { player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO };
                dest._position.m_mapId = player->m_recallMap;
                dest.Relocate(pos);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_eff::CheckCast);
        BeforeCast += SpellCastFn(spell_dk_death_gate_eff::HandleBeforeCast);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dk_death_gate_eff::SelectDest, EFFECT_0, TARGET_UNIT_CASTER);
    }
};

void AddSC_deathknight_spell_scripts()
{
    new spell_script<spell_dk_gorefiends_grasp>("spell_dk_gorefiends_grasp");
    new spell_dk_runic_empowerment();
    new spell_dk_might_of_ursoc();
    new aura_script<spell_dk_desecrated_ground>("spell_dk_desecrated_ground");
    new aura_script<spell_dk_necrotic_strike>("spell_dk_necrotic_strike");
    new spell_dk_festering_strike();
    new aura_script<spell_dk_remorseless_winter_targets>("spell_dk_remorseless_winter_targets");
    new spell_script<spell_dk_remorseless_winter>("spell_dk_remorseless_winter");
    new aura_script<spell_dk_pillar_of_frost>("spell_dk_pillar_of_frost");
    new spell_script<spell_dk_blood_charges>("spell_dk_blood_charges");
    new spell_script<spell_dk_blood_tap>("spell_dk_blood_tap");
    new spell_script<spell_dk_death_strike>("spell_dk_death_strike");
    new spell_script<spell_dk_death_strike_heal>("spell_dk_death_strike_heal");
    new spell_dk_purgatory();
    new spell_dk_purgatory_absorb();
    new spell_dk_unholy_blight();
    new spell_script<spell_dk_chilblains>("spell_dk_chilblains");
    new spell_script<spell_dk_outbreak>("spell_dk_outbreak");
    new spell_dk_anti_magic_shell_raid();
    new aura_script<spell_dk_anti_magic_shell>("spell_dk_anti_magic_shell");
    new spell_dk_anti_magic_zone();
    new spell_dk_ghoul_explode();
    new spell_dk_death_gate();
    new spell_dk_death_pact();
    new spell_dk_scourge_strike();
    new spell_script<spell_dk_death_grip>("spell_dk_death_grip");
    new spell_script<spell_dk_death_grip_effect>("spell_dk_death_grip_effect");
    new spell_dk_riposte();
    new spell_dk_plague_strike();
    new spell_dk_unholy_frenzy();
    new spell_dk_glyph_of_horn_of_winter();
    new spell_dk_soul_reaper_effect();
    new spell_dk_deaths_advance();
    new aura_script<spell_dk_death_and_decay>("spell_dk_death_and_decay");
    new spell_script<spell_dk_glyph_of_death_and_decay>("spell_dk_glyph_of_death_and_decay");
    new spell_script<spell_dk_death_coil>("spell_dk_death_coil");
    new spell_script<spell_dk_bone_shield_check>("spell_dk_bone_shield_check");
    new aura_script<spell_dk_bone_shield>("spell_dk_bone_shield");
    new spell_script<spell_dk_plague_leech>("spell_dk_plague_leech");
    new aura_script<spell_dk_sudden_doom>("spell_dk_sudden_doom");
    new aura_script<spell_dk_threat_of_tassarian>("spell_dk_threat_of_tassarian");
    new aura_script<spell_dk_vampiric_blood>("spell_dk_vampiric_blood");
    new aura_script<spell_dk_soul_reaper>("spell_dk_soul_reaper");
    new spell_script<spell_dk_howling_blast>("spell_dk_howling_blast");
    new aura_script<spell_dk_rune_converter>("spell_dk_rune_converter");
    new spell_script<spell_dk_blood_parasite>("spell_dk_blood_parasite");
    new creature_script<npc_bloodworm>("npc_bloodworm");
    new aura_script<spell_dk_shadow_infusion_proc>("spell_dk_shadow_infusion_proc");
    new aura_script<spell_dk_shadow_infusion>("spell_dk_shadow_infusion");
    new spell_script<spell_dk_dark_transformation>("spell_dk_dark_transformation");
    new spell_script<spell_dk_ghoul_ability>("spell_dk_ghoul_ability");
    new spell_script<spell_dk_raise_dead>("spell_dk_raise_dead");
    new spell_script<spell_dk_death_siphon>("spell_dk_death_siphon");
    new aura_script<spell_dk_glyph_of_unholy_command>("spell_dk_glyph_of_unholy_command");
    new spell_script<spell_dk_presence>("spell_dk_presence");
    new spell_script<spell_dk_blood_boil>("spell_dk_blood_boil");
    new spell_script<spell_dk_asphyxiate>("spell_dk_asphyxiate");
    new spell_script<spell_dk_summon_gargoyle>("spell_dk_summon_gargoyle");
    new creature_script<npc_ebon_gargoyle>("npc_ebon_gargoyle");
    new spell_script<spell_dk_gargoyle_strike>("spell_dk_gargoyle_strike");
    new aura_script<spell_dk_relentless_grip>("spell_dk_relentless_grip");
    new aura_script<spell_dk_dark_simulacrum>("spell_dk_dark_simulacrum");
    new spell_script<spell_dk_dancing_rune_weapon>("spell_dk_dancing_rune_weapon");
    new aura_script<spell_dk_dancing_rune_weapon_driver>("spell_dk_dancing_rune_weapon_driver");
    new aura_script<spell_dk_rune_weapon_scaling>("spell_dk_rune_weapon_scaling");
    new aura_script<spell_dk_runic_corruption>("spell_dk_runic_corruption");
    new aura_script<spell_dk_t15_dps_4p_bonus>("spell_dk_t15_dps_4p_bonus");
    new creature_script<npc_dk_fallen_zandaliari>("npc_dk_fallen_zandaliari");
    new aura_script<spell_dk_t15_blood_4p_bonus>("spell_dk_t15_blood_4p_bonus");
    new spell_script<spel_dk_death_shroud_duration>("spel_dk_death_shroud_duration");
    new aura_script<spell_dk_death_shroud>("spell_dk_death_shroud");
    new aura_script<spell_dk_t16_dps_4p_bonus>("spell_dk_t16_dps_4p_bonus");
    new aura_script<spell_dk_t16_blood_2p_bonus>("spell_dk_t16_blood_2p_bonus");
    new aura_script<spell_dk_t16_blood_4p_bonus>("spell_dk_t16_blood_4p_bonus");
    new spell_script<spell_dk_frozen_power>("spell_dk_frozen_power");
    new spell_script<spell_dk_pestilence>("spell_dk_pestilence");
    new spell_script<spell_dk_glyph_of_strangulate>("spell_dk_glyph_of_strangulate");
    new aura_script<spell_dk_will_of_the_necropolis>("spell_dk_will_of_the_necropolis");
    new spell_script<spell_dk_will_of_the_necropolis_rune_tap>("spell_dk_will_of_the_necropolis_rune_tap");
    new aura_script<spell_dk_crimson_scourge>("spell_dk_crimson_scourge");
    new aura_script<spell_dk_wild_mushroom_plague_periodic>("spell_dk_wild_mushroom_plague_periodic");
    new spell_script<spell_dk_wild_mushroom_plague_trigger>("spell_dk_wild_mushroom_plague_trigger");
    new creature_script<npc_dk_army_of_the_dead_ghoul>("npc_dk_army_of_the_dead_ghoul");
    new aura_script<spell_dk_glyph_of_the_skeleton>("spell_dk_glyph_of_the_skeleton");
    new aura_script<spell_dk_glyph_of_regenerative_magic>("spell_dk_glyph_of_regenerative_magic");
    new aura_script<spell_dk_tricks_of_the_trade_fix>("spell_dk_tricks_of_the_trade_fix");
    new aura_script<spell_dk_scent_of_blood_main_hand>("spell_dk_scent_of_blood_main_hand");
    new spell_script<spell_dk_glyph_of_the_loud_horn>("spell_dk_glyph_of_the_loud_horn");
    new atrigger_script<sat_dk_anti_magic_zone>("sat_dk_anti_magic_zone");
    new spell_script<spell_dk_death_gate_eff>("spell_dk_death_gate_eff");
    new aura_script<aura_glyph_of_the_geist>("aura_glyph_of_the_geist");
}
