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
 * Scripts for spells with SPELLFAMILY_HUNTER, SPELLFAMILY_PET and SPELLFAMILY_GENERIC spells used by hunter players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_hun_".
 */

#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Pet.h"
#include "Player.h"
#include "Battleground.h"
#include "SpellHistory.h"
#include "spell_common.h"
#include "Random.h"

enum HunterSpells
{
    SPELL_HUNTER_LAST_STAND                         = 53479,
    SPELL_HUNTER_HEART_OF_THE_PHOENIX_TRIGGERED     = 54114,
    SPELL_HUNTER_HEART_OF_THE_PHOENIX_DEBUFF        = 55711,
    HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED       = 54045,
    SPELL_HUNTER_POSTHASTE                          = 109215,
    SPELL_HUNTER_POSTHASTE_INCREASE_SPEED           = 118922,
    SPELL_HUNTER_NARROW_ESCAPE                      = 109298,
    SPELL_HUNTER_NARROW_ESCAPE_SELECTOR             = 115928,
    SPELL_HUNTER_NARROW_ESCAPE_ROOT                 = 128405,
    SPELL_HUNT_SERPENT_STING                        = 118253,
    SPELL_HUNT_SERPENT_SPREAD                       = 87935,
    HUNTER_SPELL_CHIMERA_SHOT_HEAL                  = 53353,
    SPELL_HUNTER_KILL_COMMAND_DAMAGE                = 83381,
    SPELL_HUNTER_KILL_COMMAND_CHARGE                = 118171,
    SPELL_HUNTER_IMPROVED_SERPENT_STING_AURA        = 82834,
    SPELL_HUNTER_IMPROVED_SERPENT_STING             = 83077,
    SEPLL_HUNTER_BINDING_SHOT_AREA                  = 109248,
    SPELL_HUNTER_BINDING_SHOT_LINK                  = 117405,
    SPELL_HUNTER_BINDING_SHOT_STUN                  = 117526,
    SPELL_HUNTER_BINDING_SHOT_IMMUNE                = 117553,
    SPELL_HUNTER_BINDING_SHOT_VISUAL_STUN           = 117614,
    SPELL_HUNTER_BINDING_SHOT_VISUAL_MISSLE         = 118306,
    HUNTER_SPELL_MASTERS_CALL                       = 62305,
    HUNTER_SPELL_MASTERS_CALL_TRIGGERED             = 62305,
    HUNTER_SPELL_MASTERS_CALL_TARGET                = 54216,
    SPELL_HUNT_COBRA_STRIKES_STACKS                 = 53257,
    SPELL_HUNTER_BEAST_CLEAVE_AURA                  = 115939,
    SPELL_HUNTER_BEAST_CLEAVE_PROC                  = 118455,
    SPELL_HUNTER_BEAST_CLEAVE_DAMAGE                = 118459,
    SPELL_HUNT_LYNX_RUSH_DOT                        = 120699,
    HUNTER_SPELL_FRENZY_STACKS                      = 19615,
    HUNTER_SPELL_FRENZY_ENERGIZE                    = 83468,
    HUNTER_SPELL_FOCUS_FIRE_READY                   = 88843,
    DIRE_BEAST_JADE_FOREST                          = 121118,
    DIRE_BEAST_KALIMDOR                             = 122802,
    DIRE_BEAST_EASTERN_KINGDOMS                     = 122804,
    DIRE_BEAST_OUTLAND                              = 122806,
    DIRE_BEAST_NORTHREND                            = 122807,
    DIRE_BEAST_KRASARANG_WILDS                      = 122809,
    DIRE_BEAST_VALLEY_OF_THE_FOUR_WINDS             = 122811,
    DIRE_BEAST_VALE_OF_THE_ETERNAL_BLOSSOM          = 126213,
    DIRE_BEAST_KUN_LAI_SUMMIT                       = 126214,
    DIRE_BEAST_TOWNLONG_STEPPES                     = 126215,
    DIRE_BEAST_DREAD_WASTES                         = 126216,
    DIRE_BEAST_DUNGEONS                             = 132764,
    SPELL_HUNTER_STAMPEDE_DAMAGE_REDUCTION          = 130201,
    HUNTER_SPELL_GLYPH_OF_STAMPEDE                  = 57902,
    SPELL_HUNTER_HUNTERS_MARK                       = 1130,
    SPELL_HUNT_GLYPH_OF_MISDIRECTION                = 56829,
    SPELL_HUNT_MISDIRECTION                         = 34477,
    SPELL_HUNT_MISDIRECTION_PROC                    = 35079,
    SPELL_HUNT_MISDIRECTION_SYMBISIS                = 110588,
    SPELL_HUNT_MISDIRECTION_SYMBISIS_PROC           = 110591,
    HUNTER_SPELL_TRACK_BEASTS                       = 1494,
    HUNTER_SPELL_TRACK_DEMONS                       = 19878,
    HUNTER_SPELL_TRACK_DRAGONKIN                    = 19879,
    HUNTER_SPELL_TRACK_ELEMENTALS                   = 19880,
    HUNTER_SPELL_TRACK_GIANTS                       = 19882,
    HUNTER_SPELL_TRACK_HUMANOIDS                    = 19883,
    HUNTER_SPELL_TRACK_UNDEAD                       = 19884,
    HUNTER_SPELL_TRACK_HIDDEN                       = 19885,
    SPELL_HUNT_GLAIVE_TOSS_AURA                     = 117050,
    SPELL_HUNT_GLAIVE_TOSS_RIGHT                    = 120755,
    SPELL_HUNT_GLAIVE_TOSS_LEFT                     = 120756,
    SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_LEFT    = 120761,
    SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_RIGHT   = 121414,
    HUNTER_SPELL_ASPECT_OF_THE_BEAST                = 61648,
    HUNTER_SPELL_WEAKENED_ARMOR                     = 113746,
    HUNTER_SPELL_GLYPH_OF_LIBERATION                = 132106,
    HUNTER_SPELL_GLYPH_OF_LIBERATION_HEAL           = 115927,
    SPELL_HUNTER_ENTRAPMENT                         = 64803,
    SPELL_HUNTER_ICE_TRAP                           = 13810,
    SPELL_HUNTER_ICE_TRAP_EFFECT                    = 135299,
    SPELL_HUNT_WILD_QUIVER                          = 76663,
    SPELL_HUNT_STEADY_SHOT_ENERGIZE                 = 77443,
    SPELL_HUNT_COBRA_SHOT_ENERGIZE                  = 91954,
    SPELL_HUNTER_STEADY_SHOT                        = 56641,
    SPELL_HUNT_ARCANE_SHOT                          = 3044,
    SPELL_HUNT_BLACK_ARROW                          = 3674,
    SPELL_HUNTER_GLYPH_OF_CAMOUFLAGE                = 119449,
    SPELL_HUNTER_CAMOUFLAGE_DRIVER                  = 51753,
    SPELL_HUNTER_CAMOUFLAGE_AURA                    = 51755,
    SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY            = 80325,
    SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_GLYPHED    = 119450,
    SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_DRIVER     = 80326,
    SPELL_HUNT_AUTO_SHOT                            = 75,
    SPELL_HUNT_GLYPH_OF_CHIMERA_SHOT                = 119447,
    SPELL_HUNT_FIRE                                 = 82926,
    SPELL_HUNT_STEADY_FOCUS                         = 53220,
    SPELL_HUNT_SPIRIT_BOND_HEAL                     = 149254,
    SPELL_HUNT_GLYPH_OF_ENDLESS_WRATH               = 119410,
    SPELL_HUNTER_TAMED_PET_PASSIVE_01               = 8875,
    SPELL_HUNTER_INVIGORATION_ENERGIZE              = 53398,
    SPELL_HUNTER_THRILL_OF_THE_HUNT_MOD             = 34720,
    SPELL_HUNTER_BLINK_STRIKES                      = 130392,
    SPELL_HUNTER_BLINK_STRIKE                       = 130393,
    SPELL_HUNTER_A_MUREDER_OF_CROWS                 = 131894,
    SPELL_HUNTER_A_MUREDER_OF_CROWS_DAMAGE          = 131900,
    SPELL_HUNTER_PIERCING_SHOTS                     = 63468,
    SPELL_HUNTER_EXPLOSIVE_SHOT                     = 53301,
    SPELL_HUNTER_GLYPH_OF_EXPLOSIVE_TRAP            = 119403,
    SPELL_HUNTER_SPIRIT_BEAST_BLESSING              = 127830,
    SPELL_HUNTER_ENERGIZE_FOCUS                     = 82716,
    SPELL_HUNTER_SPIKED_COLLAR                      = 53184,
    SPELL_HUNTER_FLARE                              = 132951,
    SPELL_HUNTER_THE_BEAST_WITHIN_EFFECT            = 34471,
    SPELL_HUNTER_THE_BEAST_WITHIN                   = 34692,
    SPELL_HUNTER_THE_BEAST_WITHIN_TRINKET           = 70029,
    SPELL_HUNTER_GLYPH_OF_MEND_PET                  = 19573,
    SPELL_HUNTER_GLYPH_OF_MEND_PET_DISPEL           = 24406,
    SPELL_HUNTER_GLYPH_OF_ASPECT_OF_THE_CHEETAH     = 119462,
    SPELL_HUNTER_GLYPH_OF_ASPECTS                   = 122492,
    SPELL_HUNTER_ASPECTS_COOLDOWN                   = 119522,
    SPELL_HUNTER_GLYPH_OF_DISTRACTING_SHOT          = 123632,
    SPELL_HUNTER_GLYPH_OF_DETERRENCE                = 56850,
    SPELL_HUNTER_CRIPPLING_POISON                   = 30981,
    SPELL_HUNTER_DEADLY_POISON                      = 34655,
    SPELL_HUNTER_MIND_NUMBING_POISON                = 25810,
    SPELL_HUNTER_BARRAGE                            = 120361,
    SPELL_HUNTER_BLACK_ICE                          = 83559,
    SPELL_HUNTER_GLYPH_OF_BLACK_ICE                 = 109263,
    SPELL_HUNTER_GLYPH_OF_BLACK_ICE_SUMMON_AT       = 122243,
    SPELL_HUNTER_RAPID_FIRE                         = 3045,
    SPELL_HUNTER_T16_4P_BONUS                       = 144641,
    SPELL_HUNTER_KEEN_EYE                           = 144659,
    SPELL_HUNTER_BRUTAL_KINSHIP                     = 144670,
    SPELL_HUNTER_BRUTAL_KINSHIP_PET                 = 145737,
    SPELL_HUNTER_LESSER_PROPORTION                  = 57894,

    SPELL_DRUID_ICE_TRAP                            = 135381,
    SPELL_DRUID_ICE_TRAP_EFFECT                     = 135382,
};

// Glyph of Aspect of the Beast - 125042
class spell_hun_glyph_of_aspect_of_the_beast : public SpellScriptLoader
{
    public:
        spell_hun_glyph_of_aspect_of_the_beast() : SpellScriptLoader("spell_hun_glyph_of_aspect_of_the_beast") { }

        class spell_hun_glyph_of_aspect_of_the_beast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_glyph_of_aspect_of_the_beast_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(HUNTER_SPELL_ASPECT_OF_THE_BEAST, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(HUNTER_SPELL_ASPECT_OF_THE_BEAST))
                        _player->RemoveSpell(HUNTER_SPELL_ASPECT_OF_THE_BEAST, false, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_hun_glyph_of_aspect_of_the_beast_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_hun_glyph_of_aspect_of_the_beast_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_glyph_of_aspect_of_the_beast_AuraScript();
        }
};

// Tracking - 118424
class spell_hun_tracking : public SpellScriptLoader
{
    public:
        spell_hun_tracking() : SpellScriptLoader("spell_hun_tracking") { }

        class spell_hun_tracking_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_tracking_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* _player = GetCaster()->ToPlayer())
                {
                    _player->LearnSpell(HUNTER_SPELL_TRACK_BEASTS,      false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_DEMONS,      false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_DRAGONKIN,   false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_ELEMENTALS,  false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_GIANTS,      false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_HUMANOIDS,   false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_UNDEAD,      false);
                    _player->LearnSpell(HUNTER_SPELL_TRACK_HIDDEN,      false);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_tracking_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_tracking_AuraScript();
        }
};

// Dash - 113073
class spell_hun_dash : public SpellScriptLoader
{
    public:
        spell_hun_dash() : SpellScriptLoader("spell_hun_dash") { }

        class spell_hun_dash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_dash_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_dash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_dash_SpellScript();
        }
};

// 121818 - Stampede
class spell_hunt_stampede : public SpellScript
{
    PrepareSpellScript(spell_hunt_stampede);

    void HandleHit()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        Unit* target = GetExplTargetUnit();
        if (!target)
            return;

        if (player->HasAura(HUNTER_SPELL_GLYPH_OF_STAMPEDE))
        {
            if (Pet* oldPet = player->GetPet())
            {
                for (int8 i = PET_SLOT_ACTIVE_FIRST; i <= PET_SLOT_ACTIVE_LAST; ++i)
                {
                    if (i != player->GetCurrentPetSlot())
                    {
                        Pet* pet = new Pet{ player };
                        pet->SetTemporary();
                        Position pos = target->GetRandomNearPosition(MELEE_RANGE + target->GetCombatReach());
                        if (!pet->LoadPetFromDB(PET_LOAD_BY_ENTRY, oldPet->GetEntry(), &pos))
                        {
                            delete pet;
                            continue;
                        }
                        InitPet(pet);
                    }
                }
            }
        }
        else
        {
            for (int8 i = PET_SLOT_ACTIVE_FIRST; i <= PET_SLOT_ACTIVE_LAST; ++i)
            {
                if (i == player->GetCurrentPetSlot())
                    continue;

                Pet* pet = new Pet{ player };
                pet->SetTemporary();
                Position pos = target->GetRandomNearPosition(MELEE_RANGE + target->GetCombatReach());
                if (!pet->LoadPetFromDB(PET_LOAD_BY_SLOT, i, &pos))
                {
                    delete pet;
                    continue;
                }
                InitPet(pet);
            }
        }
    }

    void InitPet(Pet* pet)
    {
        GetCaster()->AddSummon(pet);                    // Do it manually because this is pet
        pet->SetReactState(REACT_ASSIST);
        pet->AI()->AttackStart(GetExplTargetUnit());
        pet->SetDuration(20 * IN_MILLISECONDS);
        if (GetExplTargetUnit()->GetCharmerOrOwnerPlayerOrPlayerItself() || pet->GetMap()->IsBattlegroundOrArena())
            pet->CastSpell(pet, SPELL_HUNTER_STAMPEDE_DAMAGE_REDUCTION, true);
        pet->SetFullHealth();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_stampede::HandleHit);
    }
};

// Dire Beast - 120679
class spell_hunt_dire_beast : public SpellScript
{
    PrepareSpellScript(spell_hunt_dire_beast);

    void HandleHit()
    {
        uint32 spellId = GetSpellId();
        ASSERT(spellId);
        GetCaster()->CastSpell(GetHitUnit(), spellId, true);
        if (auto const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
        {
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, spellInfo->Effects[EFFECT_0].MiscValue);
            if (!summons.empty())
                summons.front()->AI()->AttackStart(GetHitUnit());
        }
    }

    uint32 GetSpellId()
    {
        // Summon's skin is different function of Map or Zone ID
        switch (GetCaster()->GetZoneId())
        {
            case 5785: // The Jade Forest
                return DIRE_BEAST_JADE_FOREST;
            case 5805: // Valley of the Four Winds
                return DIRE_BEAST_VALLEY_OF_THE_FOUR_WINDS;
            case 5840: // Vale of Eternal Blossoms
                return DIRE_BEAST_VALE_OF_THE_ETERNAL_BLOSSOM;
            case 5841: // Kun-Lai Summit
                return DIRE_BEAST_KUN_LAI_SUMMIT;
            case 5842: // Townlong Steppes
                return DIRE_BEAST_TOWNLONG_STEPPES;
            case 6134: // Krasarang Wilds
                return DIRE_BEAST_KRASARANG_WILDS;
            case 6138: // Dread Wastes
                return DIRE_BEAST_DREAD_WASTES;
            default:
            {
                switch (GetCaster()->GetMapId())
                {
                    case 0: // Eastern Kingdoms
                        return DIRE_BEAST_EASTERN_KINGDOMS;
                    case 1: // Kalimdor
                        return DIRE_BEAST_KALIMDOR;
                    case 8: // Outland
                        return DIRE_BEAST_OUTLAND;
                    case 10: // Northrend
                        return DIRE_BEAST_NORTHREND;
                    default:
                        if (GetCaster()->GetMap()->IsDungeon())
                            return DIRE_BEAST_DUNGEONS;
                        return DIRE_BEAST_EASTERN_KINGDOMS;
                }
                break;
            }
        }
        return 0;
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_dire_beast::HandleHit);
    }
};

// 120694 - Dire Beast Focus Driver
class spell_hunt_dire_beast_focus_driver : public AuraScript
{
    PrepareAuraScript(spell_hunt_dire_beast_focus_driver);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (Unit* hunter = GetUnitOwner()->GetOwner())
            hunter->CastCustomSpell(SPELL_HUNTER_ENERGIZE_FOCUS, SPELLVALUE_BASE_POINT0, eff->GetAmount(), hunter, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hunt_dire_beast_focus_driver::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Focus Fire - 82692
class spell_hun_focus_fire : public SpellScriptLoader
{
    public:
        spell_hun_focus_fire() : SpellScriptLoader("spell_hun_focus_fire") { }

        class spell_hun_focus_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_focus_fire_SpellScript);

            SpellCastResult CheckFrenzy()
            {
                Unit* const caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                Pet* const pet = caster->ToPlayer()->GetPet();
                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (!pet->HasAura(HUNTER_SPELL_FRENZY_STACKS))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_focus_fire_SpellScript::CheckFrenzy);
            }
        };

        class spell_hun_focus_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_focus_fire_AuraScript)

            bool Load() override
            {
                Unit const* const caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return false;

                Pet const* const pet = caster->ToPlayer()->GetPet();
                if (!pet || !pet->HasAura(HUNTER_SPELL_FRENZY_STACKS))
                    return false;

                return true;
            }

            void CalculateAmount(AuraEffect const*, float& amount, bool&)
            {
                // Pointers are checked in CheckCast() and Load()
                Pet const* const pet = GetCaster()->ToPlayer()->GetPet();

                // Pet will have this aura otherwise spell is not casted at all
                Aura const* const aur = pet->GetAura(HUNTER_SPELL_FRENZY_STACKS);
                amount *= aur->GetStackAmount();
            }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Pointers are checked in CheckCast() and Load()
                Unit* const caster = GetCaster();
                Pet* const pet = caster->ToPlayer()->GetPet();

                Aura const* const aura = pet->GetAura(HUNTER_SPELL_FRENZY_STACKS);

                int32 const amount = aurEff->GetAmount() * aura->GetStackAmount();
                caster->CastCustomSpell(pet, HUNTER_SPELL_FRENZY_ENERGIZE, &amount, NULL, NULL, true);
                pet->RemoveAurasDueToSpell(HUNTER_SPELL_FRENZY_STACKS, pet->GetGUID());
                caster->RemoveAurasDueToSpell(HUNTER_SPELL_FRENZY_STACKS);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_focus_fire_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RANGED_HASTE);
                AfterEffectApply += AuraEffectApplyFn(spell_hun_focus_fire_AuraScript::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_focus_fire_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_focus_fire_SpellScript();
        }
};

// Frenzy - 19615
class spell_hun_frenzy : public SpellScriptLoader
{
    public:
        spell_hun_frenzy() : SpellScriptLoader("spell_hun_frenzy") { }

        class spell_hun_frenzy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_frenzy_SpellScript);

            void HandleOnHit()
            {
                if (Unit* caster = GetCaster())
                    if (caster->GetOwner())
                        if (Aura *frenzy = caster->GetAura(HUNTER_SPELL_FRENZY_STACKS))
                            if (frenzy->GetStackAmount() >= 5)
                                caster->GetOwner()->CastSpell(caster->GetOwner(), HUNTER_SPELL_FOCUS_FIRE_READY, true);
            }

            void Register() override
            {
               OnHit += SpellHitFn(spell_hun_frenzy_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_frenzy_SpellScript();
        }

        class spell_hun_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_frenzy_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetOwner())
                    if (GetTarget()->GetOwner()->HasAura(HUNTER_SPELL_FOCUS_FIRE_READY))
                        GetTarget()->GetOwner()->RemoveAura(HUNTER_SPELL_FOCUS_FIRE_READY);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectApplyFn(spell_hun_frenzy_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE_3, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_frenzy_AuraScript();
        }
};

// 118455 - Beast Cleave
class spell_hunt_beast_cleave_proc : public AuraScript
{
    PrepareAuraScript(spell_hunt_beast_cleave_proc);

    void HandleProc(AuraEffect const*, ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamage())
            return;

        // No, can't pass as triggered by aura, bc aura owner is hunter
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_HUNTER_BEAST_CLEAVE_DAMAGE)
            return;

        int32 damage = eventInfo.GetDamageInfo()->GetDamage();
        damage *= 0.75;
        // Beast Cleave damage against nearby targets will no longer be affected by damage taken modifiers on the primary target.
        damage /= GetDamageTakenCorrectiveFactor(GetTarget(), eventInfo.GetActionTarget(), eventInfo.GetSpellInfo());
        GetTarget()->CastCustomSpell(SPELL_HUNTER_BEAST_CLEAVE_DAMAGE, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hunt_beast_cleave_proc::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 118459 - Beast Cleave
class spell_hunt_beast_cleave_damage : public SpellScript
{
    PrepareSpellScript(spell_hunt_beast_cleave_damage);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* victim = nullptr;
        Spell const* spell = GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (spell)
            victim = spell->m_targets.GetUnitTarget();
        else
            victim = GetCaster()->GetVictim();

        targets.remove(victim);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_beast_cleave_damage::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 109248 - Binding Shot
class spell_hunt_binding_shot_missile : public SpellScript
{
    PrepareSpellScript(spell_hunt_binding_shot_missile);

    void HandleCast()
    {
        GetCaster()->CastSpell(*GetExplTargetDest(), SPELL_HUNTER_BINDING_SHOT_VISUAL_MISSLE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_hunt_binding_shot_missile::HandleCast);
    }
};

// 117405 - Binding Shot
class spell_hunt_binding_shot : public AuraScript
{
    PrepareAuraScript(spell_hunt_binding_shot);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* hunter = GetCaster())
            hunter->BindAura(GetAura());
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
            return;

        if (Unit* caster = GetCaster())
        {
            DynamicObject* dynObj = caster->GetDynObject(SEPLL_HUNTER_BINDING_SHOT_AREA);
            if (!dynObj)
                return;

            Unit* target = GetTarget();
            if (!target->HasAura(SPELL_HUNTER_BINDING_SHOT_IMMUNE))
            {
                target->CastSpell(target, SPELL_HUNTER_BINDING_SHOT_STUN, true, nullptr, nullptr, GetCasterGUID());
                target->CastSpell(target, SPELL_HUNTER_BINDING_SHOT_IMMUNE, true);
                target->CastSpell(dynObj->GetPosition(), SPELL_HUNTER_BINDING_SHOT_VISUAL_STUN, true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectRemoveFn(spell_hunt_binding_shot::HandleApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_FROM_CASTER, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_binding_shot::HandleRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_FROM_CASTER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 109248 - Binding Shot
class spell_hunt_binding_shot_zone : public AuraScript
{
    PrepareAuraScript(spell_hunt_binding_shot_zone);

    void HandleTick(AuraEffect const*)
    {
        if (Unit* hunter = GetCaster())
        {
            if (DynamicObject* dynObj = hunter->GetDynObject(SEPLL_HUNTER_BINDING_SHOT_AREA))
            {
                hunter->CastSpell(*dynObj, SPELL_HUNTER_BINDING_SHOT_LINK, true);

                if (auto list = hunter->GetBoundAurasBySpellId(SPELL_HUNTER_BINDING_SHOT_LINK))
                    for (auto&& itr : std::list<Aura*>{ *list })
                        if (itr->GetDuration() != itr->GetMaxDuration())
                            itr->Remove();
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_binding_shot_zone::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 118253 - Serpent Sting
class spell_hunt_improved_serpent_sting : public AuraScript
{
    PrepareAuraScript(spell_hunt_improved_serpent_sting);

    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
        {
            if (AuraEffect const* improvedSting = caster->GetAuraEffect(SPELL_HUNTER_IMPROVED_SERPENT_STING_AURA, EFFECT_0))
            {
                int32 damage = aurEff->GetAmount() * aurEff->GetTotalTicks();
                damage = CalculatePct(damage, improvedSting->GetAmount());
                caster->CastCustomSpell(SPELL_HUNTER_IMPROVED_SERPENT_STING, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true);
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hunt_improved_serpent_sting::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// Chimera Shot - 53209
class spell_hun_chimera_shot : public SpellScriptLoader
{
    public:
        spell_hun_chimera_shot() : SpellScriptLoader("spell_hun_chimera_shot") { }

        class spell_hun_chimera_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_chimera_shot_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        AuraEffect const* serpentSting = target->GetAuraEffect(SPELL_HUNT_SERPENT_STING, EFFECT_0, _player->GetGUID());

                        if (serpentSting)
                            serpentSting->GetBase()->RefreshDuration();

                        _player->CastSpell(_player, HUNTER_SPELL_CHIMERA_SHOT_HEAL, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_hun_chimera_shot_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_chimera_shot_SpellScript();
        }
};

// 53478 - Last Stand
class spell_hunt_last_stand_pet : public SpellScript
{
    PrepareSpellScript(spell_hunt_last_stand_pet);

    void HandleDummy(SpellEffIndex)
    {
        Unit* caster = GetCaster();
        int32 health = int32(caster->CountPctFromMaxHealth(GetEffectValue()));
        CustomSpellValues ctx;
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, health);
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT1, GetEffectValue()); // Why the fuck base points are 23?
        caster->CastCustomSpell(SPELL_HUNTER_LAST_STAND, ctx, caster, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_last_stand_pet::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 53271 - Master's Call
class spell_hunt_masters_call: public SpellScript
{
    PrepareSpellScript(spell_hunt_masters_call);

    SpellCastResult CheckCast()
    {
        Player* hunter = GetCaster()->ToPlayer();
        if (!hunter)
            return SPELL_FAILED_DONT_REPORT;

        Unit* target = GetExplTargetUnit();
        if (!target)
            target = hunter;

        if (Pet* pet = hunter->GetPet())
        {
            if (!hunter->IsWithinCombatRange(pet, hunter->GetSpellMaxRangeForTarget(pet, GetSpellInfo())) ||
                !hunter->IsWithinCombatRange(target, hunter->GetSpellMaxRangeForTarget(target, GetSpellInfo())) ||
                !pet->IsWithinCombatRange(target, pet->GetSpellMaxRangeForTarget(target, GetSpellInfo())))
                return SPELL_FAILED_OUT_OF_RANGE;

            for (auto&& id : { HUNTER_SPELL_MASTERS_CALL_TRIGGERED, HUNTER_SPELL_MASTERS_CALL_TARGET })
            {
                std::unique_ptr<Spell> spell{ new Spell(pet, sSpellMgr->GetSpellInfo(id), TRIGGERED_NONE) };
                SpellCastResult result = spell->CheckPetCast(target);
                if (result != SPELL_CAST_OK)
                    return result;
            }
        }

        return SPELL_CAST_OK;
    }

    void HandleScript(SpellEffIndex)
    {
        Unit* target = GetHitUnit();
        target->CastSpell(target, HUNTER_SPELL_MASTERS_CALL_TRIGGERED, true);
        if (Unit* ally = GetExplTargetUnit())
            target->CastSpell(ally, GetSpellInfo()->Effects[EFFECT_0].CalcValue(), true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hunt_masters_call::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_hunt_masters_call::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Scatter Shot - 37506
class spell_hun_scatter_shot : public SpellScriptLoader
{
    public:
        spell_hun_scatter_shot() : SpellScriptLoader("spell_hun_scatter_shot") { }

        class spell_hun_scatter_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_scatter_shot_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                // break Auto Shot and autohit
                caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                caster->AttackStop();
                caster->SendAttackSwingCancelAttack();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_scatter_shot_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_scatter_shot_SpellScript();
        }
};

// 53302, 53303, 53304 Sniper Training
enum eSniperTrainingSpells
{
    SPELL_SNIPER_TRAINING_R1        = 53302,
    SPELL_SNIPER_TRAINING_BUFF_R1   = 64418,
};

class spell_hun_sniper_training : public SpellScriptLoader
{
    public:
        spell_hun_sniper_training() : SpellScriptLoader("spell_hun_sniper_training") { }

        class spell_hun_sniper_training_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_sniper_training_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SNIPER_TRAINING_R1) || !sSpellMgr->GetSpellInfo(SPELL_SNIPER_TRAINING_BUFF_R1))
                    return false;
                return true;
            }

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                if (aurEff->GetAmount() <= 0)
                {
                    Unit* caster = GetCaster();
                    uint32 spellId = SPELL_SNIPER_TRAINING_BUFF_R1 + GetId() - SPELL_SNIPER_TRAINING_R1;
                    if (Unit* target = GetTarget())
                        if (!target->HasAura(spellId))
                        {
                            SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(spellId);
                            Unit* triggerCaster = triggeredSpellInfo->NeedsToBeTriggeredByCaster(GetSpellInfo()) ? caster : target;
                            triggerCaster->CastSpell(target, triggeredSpellInfo, true, 0, aurEff);
                        }
                }
            }

            void HandleUpdatePeriodic(AuraEffect *aurEff)
            {
                if (Player* playerTarget = GetUnitOwner()->ToPlayer())
                {
                    int32 baseAmount = aurEff->GetBaseAmount();
                    int32 amount = playerTarget->isMoving() ?
                    playerTarget->CalculateSpellDamage(playerTarget, GetSpellInfo(), aurEff->GetEffIndex(), &baseAmount) :
                    aurEff->GetAmount() - 1;
                    aurEff->SetAmount(amount);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_sniper_training_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_hun_sniper_training_AuraScript::HandleUpdatePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hun_sniper_training_AuraScript();
        }
};

class spell_hun_pet_carrion_feeder : public SpellScriptLoader
{
    public:
        spell_hun_pet_carrion_feeder() : SpellScriptLoader("spell_hun_pet_carrion_feeder") { }

        class spell_hun_pet_carrion_feeder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_pet_carrion_feeder_SpellScript);

            bool Load() override
            {
                if (!GetCaster()->IsPet())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED, false);
            }

            void Register() override
            {
                // add dummy effect spell handler to pet's Last Stand
                OnEffectHit += SpellEffectFn(spell_hun_pet_carrion_feeder_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_hun_pet_carrion_feeder_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_pet_carrion_feeder_SpellScript();
        }
};

// 1515 - Tame Beast
class spell_hun_tame_beast : public SpellScriptLoader
{
    public:
        spell_hun_tame_beast() : SpellScriptLoader("spell_hun_tame_beast") { }

        class spell_hun_tame_beast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_tame_beast_SpellScript)

            void SendPetTameError(Player* player, PetTameError error)
            {
                WorldPacket data(SMSG_PET_TAME_FAILURE, 1);
                data << uint8(error);
                player->SendDirectMessage(&data);
            }

            SpellCastResult CheckCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (!caster || caster->GetClass() != CLASS_HUNTER)
                    return SPELL_FAILED_DONT_REPORT;

                if (!GetExplTargetUnit())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                Creature* target = GetExplTargetUnit()->ToCreature();
                if (!target)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (caster->GetSlotForNewPet() == -1)
                {
                    SendPetTameError(caster, PET_TAME_ERROR_TOO_MANY_PETS);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (target->GetLevel() > caster->GetLevel())
                {
                    SendPetTameError(caster, PET_TAME_ERROR_TOO_HIGH_LEVEL);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (!target->GetCreatureTemplate()->IsTameable(caster->CanTameExoticPets()))
                {
                    if (target->GetCreatureTemplate()->IsExotic())
                        SendPetTameError(caster, PET_TAME_ERROR_CANT_CONTROL_EXOTIC);
                    else
                        SendPetTameError(caster, PET_TAME_ERROR_NOT_TAMEABLE);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (caster->GetPetGUID())
                {
                    SendPetTameError(caster, PET_TAME_ERROR_ANOTHER_SUMMON_ACTIVE);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (caster->GetCharmGUID())
                {
                    SendPetTameError(caster, PET_TAME_ERROR_ANOTHER_SUMMON_ACTIVE);
                    return SPELL_FAILED_DONT_REPORT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_tame_beast_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_tame_beast_SpellScript();
        }
};

// Dust Cloud - 50285
class spell_hun_pet_dust_cloud : public SpellScriptLoader
{
    public:
        spell_hun_pet_dust_cloud() : SpellScriptLoader("spell_hun_pet_dust_cloud") { }

        class spell_hun_pet_dust_cloud_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_pet_dust_cloud_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                auto caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    for (uint32 i = 0; i < 3; ++i)
                        caster->CastSpell(target, HUNTER_SPELL_WEAKENED_ARMOR, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_pet_dust_cloud_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hun_pet_dust_cloud_SpellScript();
        }
};

// Explosive Trap - 13812
class spell_hunt_explosive_trap : public SpellScript
{
    PrepareSpellScript(spell_hunt_explosive_trap)

    void ResolveDamage(SpellEffIndex eff)
    {
        if (GetCaster()->GetCharmerOrOwnerOrSelf()->HasAura(SPELL_HUNTER_GLYPH_OF_EXPLOSIVE_TRAP))
            PreventEffect(eff);
    }

    void ResolveKnockback(SpellEffIndex eff)
    {
        if (!GetCaster()->GetCharmerOrOwnerOrSelf()->HasAura(SPELL_HUNTER_GLYPH_OF_EXPLOSIVE_TRAP))
            PreventEffect(eff);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_explosive_trap::ResolveDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectLaunch += SpellEffectFn(spell_hunt_explosive_trap::ResolveKnockback, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_explosive_trap::ResolveKnockback, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 63487 - Ice Trap
// 110601 - Ice Trap (Symbiosis)
class spell_hunt_ice_trap : public SpellScript
{
    PrepareSpellScript(spell_hunt_ice_trap);

    void HandleHit(SpellEffIndex)
    {
        if (Unit* owner = GetCaster()->GetOwner())
        {
            uint32 spellId = owner->GetClass() == CLASS_DRUID ? SPELL_DRUID_ICE_TRAP : SPELL_HUNTER_ICE_TRAP;
            owner->CastSpell(*GetCaster(), spellId, true);
            if (owner->HasAura(SPELL_HUNTER_GLYPH_OF_BLACK_ICE))
                owner->CastSpell(*GetCaster(), SPELL_HUNTER_GLYPH_OF_BLACK_ICE_SUMMON_AT, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_ice_trap::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 621, 628
class sat_hunt_ice_trap : public IAreaTriggerAura
{
    uint32 spellId = 0;

    void OnInit() override
    {
        spellId = GetAreaTrigger()->GetSpellId() == SPELL_HUNTER_ICE_TRAP ? SPELL_HUNTER_ICE_TRAP_EFFECT : SPELL_DRUID_ICE_TRAP_EFFECT;
    }

    bool CheckTriggering(WorldObject* object) override
    {
        return object->ToUnit() && GetCaster()->IsValidAttackTarget(object->ToUnit(), sSpellMgr->GetSpellInfo(spellId), nullptr, true);
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        object->ToUnit()->CastSpell(object->ToUnit(), spellId, true);
    }

    void OnTriggeringUpdate(WorldObject* object) override
    {
        Unit* unit = object->ToUnit();
        if (!unit->HasAura(spellId))
            unit->CastSpell(object->ToUnit(), spellId, true);
    }

    void OnTriggeringRemove(WorldObject* object) override
    {
        object->ToUnit()->RemoveAurasDueToSpell(spellId);
    }
};

// 347
class sat_hunt_ice_trap_black_ice : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object == GetCaster();
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_HUNTER_BLACK_ICE, true);
    }

    void OnTriggeringUpdate(WorldObject* object) override
    {
        if (!GetCaster()->HasAura(SPELL_HUNTER_BLACK_ICE))
            GetCaster()->CastSpell(GetCaster(), SPELL_HUNTER_BLACK_ICE, true);
    }

    void OnTriggeringRemove(WorldObject* object) override
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_HUNTER_BLACK_ICE);
    }
};

// 19387 - Entrapment
class spell_hunt_entrapment : public AuraScript
{
    PrepareAuraScript(spell_hunt_entrapment);

    void HandleProc(ProcEventInfo& event)
    {
        PreventDefaultAction();
        event.GetActor()->CastSpell(event.GetActionTarget(), SPELL_HUNTER_ENTRAPMENT, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_hunt_entrapment::HandleProc);
    }
};

// 76659 - Mastery: Wild Quiver
class spell_hunt_wild_quiver : public AuraScript
{
    PrepareAuraScript(spell_hunt_wild_quiver);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        float chance = mastery->GetFloatAmount();
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_HUNTER_BARRAGE)
            chance /= 5;
        if (roll_chance_f(chance))
            GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_HUNT_WILD_QUIVER, true, nullptr, mastery);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hunt_wild_quiver::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 117050 - Glaive Toss
class spell_hunt_glaive_toss : public AuraScript
{
    PrepareAuraScript(spell_hunt_glaive_toss);

public:
    std::set<uint64> targetsLeft;
    std::set<uint64> targetsRight;
    Position startPositon;
    Position destPosition;
    ObjectGuid mainTarget = ObjectGuid::Empty;
    TimeValue timeToTarget;
    TimeValue startTime;
    bool done = false;
    bool reverseLeft = false;
    bool reverseRight = false;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Spell const* spell = GetUnitOwner()->FindCurrentSpellBySpellId(GetSpellInfo()->Id))
        {
            mainTarget = spell->m_targets.GetUnitTargetGUID();
            startPositon = GetUnitOwner()->GetPosition();
            destPosition = spell->m_targets.GetDstPos()->GetPosition();
            float dist = startPositon.GetExactDist(&destPosition);
            timeToTarget = Milliseconds(uint32(dist / 24.0f * IN_MILLISECONDS));
            startTime = TimeValue::Now();
        }
    }

    void HandlePeriodic(AuraEffect const* eff)
    {
        if (!done)
        {
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_LEFT, true);
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_RIGHT, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hunt_glaive_toss::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_glaive_toss::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 120761, 121414 - Glaive Toss (damage)
class spell_hunt_glaive_toss_damage : public SpellScript
{
    PrepareSpellScript(spell_hunt_glaive_toss_damage);

    ObjectGuid mainTarget = ObjectGuid::Empty;
    std::list<WorldObject*> selected;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        spell_hunt_glaive_toss* script = nullptr;
        if (Aura const* aura = GetCaster()->GetAura(SPELL_HUNT_GLAIVE_TOSS_AURA))
            script = dynamic_cast<spell_hunt_glaive_toss*>(aura->GetScriptByName("spell_hunt_glaive_toss"));

        if (!script)
        {
            targets.clear();
            return;
        }

        mainTarget = script->mainTarget;
        auto diff = TimeValue::Now() - script->startTime;
        float dist = diff.ToMilliseconds() * 24.0f / IN_MILLISECONDS;
        auto& hit = GetSpellInfo()->Id == SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_LEFT ? script->targetsLeft : script->targetsRight;

        targets.remove_if([&](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            if (!target)
                return true;
            if (hit.find(target->GetGUID()) != hit.end())
                return true;
            if (!unit->IsInBetween(&script->startPositon, &script->destPosition, 5.0f))
                return true;
            if (unit->GetExactDist(&script->startPositon) > dist)
                return true;
            hit.insert(unit->GetGUID());
            return false;
        });
        if (diff >= script->timeToTarget)
        {
            if (script->reverseLeft && script->reverseRight)
            {
                script->done = true;
                return;
            }

            // Ensure that the main target will be hit
            Unit* target = ObjectAccessor::GetUnit(*GetCaster(), mainTarget);
            if (target && hit.find(mainTarget) == hit.end())
                if (std::find(targets.begin(), targets.end(), target) == targets.end())
                    targets.push_back(target);

            hit.clear();
            hit.insert(mainTarget);

            if (GetSpellInfo()->Id == SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_LEFT && !script->reverseLeft)
                script->reverseLeft = true;

            if (GetSpellInfo()->Id == SPELL_HUNT_GLAIVE_TOSS_DAMAGE_AND_SNARE_RIGHT && !script->reverseRight)
                script->reverseRight = true;

            // Do it one time only
            if (!script->reverseLeft || !script->reverseRight)
                return;

            if (target)
                script->startPositon = target->GetPosition();
            else
                script->startPositon = script->destPosition;
            script->destPosition = GetCaster()->GetPosition();
            float dist = script->startPositon.GetExactDist(&script->destPosition);
            script->timeToTarget = Milliseconds(uint32(dist / 24.0f * IN_MILLISECONDS));
            script->startTime = TimeValue::Now();
        }
        selected = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = selected;
    }

    void HandleHit()
    {
        if (GetHitUnit()->GetGUID() == mainTarget)
            SetHitDamage(GetHitDamage() * 4);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_glaive_toss_damage::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_glaive_toss_damage::CopyTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnHit += SpellHitFn(spell_hunt_glaive_toss_damage::HandleHit);
    }
};

// 120755, 120756 - Glaive Toss (Missile data)
class spell_hunt_glaive_toss_missile : public SpellScript
{
    PrepareSpellScript(spell_hunt_glaive_toss_missile);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.clear();
        targets.push_back(GetExplTargetUnit());
        targets.push_back(GetCaster());
    }

    void Register() override
    {
        auto target = sSpellMgr->GetSpellInfo(m_scriptSpellId)->Effects[EFFECT_0].TargetA.GetTarget();
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_glaive_toss_missile::SelectTarget, EFFECT_0, target);
    }
};

// 56641 - Steady Shot, 77767 - Cobra Shot
class spell_hunt_steady_cobra_shot : public SpellScript
{
    PrepareSpellScript(spell_hunt_steady_cobra_shot)

    void Energize()
    {
        Unit* target = GetExplTargetUnit();
        Unit* caster = GetCaster();
        if (!target)
            return;

        uint32 spellId = (GetSpellInfo()->Id == SPELL_HUNTER_STEADY_SHOT) ? SPELL_HUNT_STEADY_SHOT_ENERGIZE : SPELL_HUNT_COBRA_SHOT_ENERGIZE;
        if (SpellInfo const* const spell = sSpellMgr->GetSpellInfo(spellId))
        {
            int32 energizeValue = spell->Effects[EFFECT_0].BasePoints;

            // Steady Focus
            if (spellId == SPELL_HUNT_STEADY_SHOT_ENERGIZE)
                if (AuraEffect const* const steadyFocus = caster->GetAuraEffect(SPELL_HUNT_STEADY_FOCUS, EFFECT_1))
                    energizeValue += steadyFocus->GetAmount();

            // Hunter T13 2P Bonus
            if (caster->HasAura(105732))
                energizeValue *= 2;

            caster->CastCustomSpell(caster, spellId, &energizeValue, nullptr, nullptr, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_hunt_steady_cobra_shot::Energize);
    }
};

// 53260 - Cobra Strikes
class spell_hunt_cobra_strikes : public AuraScript
{
    PrepareAuraScript(spell_hunt_cobra_strikes);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->GetPetGUID() != 0
            && eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_HUNT_ARCANE_SHOT;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetUnitOwner()->CastCustomSpell(SPELL_HUNT_COBRA_STRIKES_STACKS, SPELLVALUE_AURA_STACK, 2, GetUnitOwner(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_cobra_strikes::CheckProc);
        OnProc += AuraProcFn(spell_hunt_cobra_strikes::HandleProc);
    }
};

// 53257 - Cobra Strikes.
// Called by 16827 - Claw (Basic Attack), 17253 - Bite (Basic Attack), 49966 - Smack (Basic Attack), 145625 - Bite
class spell_hunt_cobra_strikes_stacks : public SpellScript
{
    PrepareSpellScript(spell_hunt_cobra_strikes_stacks);

    void RemoveStack()
    {
        Unit* caster = GetCaster();
        for (auto&& auraOwner : { caster, caster->GetOwner() })
        {
            if (!auraOwner)
                continue;
            if (Aura* aura = auraOwner->GetAura(SPELL_HUNT_COBRA_STRIKES_STACKS))
            {
                uint8 stacks = aura->GetStackAmount();
                if (stacks < 2)
                    aura->Remove();
                else
                    aura->SetStackAmount(stacks - 1);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_hunt_cobra_strikes_stacks::RemoveStack);
    }
};

// 56343 - Lock and Load
class spell_hunt_lock_and_load : public AuraScript
{
    PrepareAuraScript(spell_hunt_lock_and_load);

    bool ChecProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_PERIODIC)
            return roll_chance_i(20);
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_lock_and_load::ChecProc);
    }
};

// 56453 - Lock and Load
class spell_hunt_lock_and_load_cooldown : public SpellScript
{
    PrepareSpellScript(spell_hunt_lock_and_load_cooldown);

    void HandleHit()
    {
        if (Player* hunter = GetCaster()->ToPlayer())
            hunter->RemoveSpellCooldown(SPELL_HUNTER_EXPLOSIVE_SHOT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_lock_and_load_cooldown::HandleHit);
    }
};

// 34026 - Kill Command
class spell_hunt_kill_command : public SpellScript
{
    PrepareSpellScript(spell_hunt_kill_command);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_DONT_REPORT;

        Pet* pet = GetCaster()->ToPlayer()->GetPet();
        if (!pet || pet->isDead())
            return SPELL_FAILED_NO_PET;

        if (!GetExplTargetUnit() || !pet->IsWithinDist(GetExplTargetUnit(), 25.0f, true))
            return SPELL_FAILED_DONT_REPORT;

        uint32 petUnitFlags = pet->GetUInt32Value(UNIT_FIELD_FLAGS);     // Get unit state
        if (petUnitFlags & UNIT_FLAG_STUNNED)
            return SPELL_FAILED_STUNNED;
        else if (petUnitFlags & UNIT_FLAG_CONFUSED)
            return SPELL_FAILED_CONFUSED;
        else if (petUnitFlags & UNIT_FLAG_FLEEING)
            return SPELL_FAILED_FLEEING;

        return SPELL_CAST_OK;
    }

    void HandleHit(SpellEffIndex)
    {
        Player* hunter = GetCaster()->ToPlayer();
        if (!hunter)
            return;

        if (Pet* pet = hunter->GetPet())
        {
            pet->ToCreature()->AI()->AttackStart(GetHitUnit());
            pet->CastSpell(GetHitUnit(), SPELL_HUNTER_KILL_COMMAND_DAMAGE, true);
            pet->CastSpell(GetHitUnit(), SPELL_HUNTER_KILL_COMMAND_CHARGE, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hunt_kill_command::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_hunt_kill_command::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 83381 - Kill Command (damage)
class spell_hunt_kill_command_damage : public SpellScript
{
    PrepareSpellScript(spell_hunt_kill_command_damage);

    void CalculateDamage(SpellEffIndex effIndex)
    {
        Unit* hunter = GetCaster()->GetOwner();
        if (!hunter)
            return;

        float rap = hunter->GetWeaponDamageRange(RANGED_ATTACK, MAXDAMAGE);
        SetEffectValue(GetSpellInfo()->Effects[effIndex].CalcValue(hunter) + rap * 0.5);
    }

    void HandleHit()
    {
        if (AuraEffect const* mark = GetHitUnit()->GetAuraEffect(SPELL_HUNTER_HUNTERS_MARK, EFFECT_1))
            SetHitDamage(GetHitDamage() + CalculatePct(GetHitDamage(), mark->GetAmount()));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_kill_command_damage::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnHit += SpellHitFn(spell_hunt_kill_command_damage::HandleHit);
    }
};

// 3044 - Arcane Shot, 53209 - Chimera Shot, 34026 - Kill Command, 53301 - Explosive Shot
class spell_hunt_hunters_mark_trigger : public SpellScript
{
    PrepareSpellScript(spell_hunt_hunters_mark_trigger);

    void HandleCast()
    {
        if (Unit* target = GetExplTargetUnit())
            if (GetCaster()->HasSpell(SPELL_HUNTER_HUNTERS_MARK))
                GetCaster()->CastSpell(target, SPELL_HUNTER_HUNTERS_MARK, true);
    }

    void Register() override
    {
        OnSpellStart += SpellCastFn(spell_hunt_hunters_mark_trigger::HandleCast);
    }
};

// 51753 - Camouflage (Aura of spell itself)
class spell_hunt_camouflage_driver : public AuraScript
{
    PrepareAuraScript(spell_hunt_camouflage_driver);

    Player* hunter = nullptr;

    bool Load() override
    {
        hunter = GetOwner()->ToPlayer();
        return hunter != nullptr;
    }

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (hunter->IsInCombat())
            SetDuration(6000);
            
        std::vector<Unit*> v = { hunter };
        if (Pet* pet = hunter->GetPet())
            v.push_back(pet);

        for (auto&& target : v)
        {
            target->CastSpell(target, SPELL_HUNTER_CAMOUFLAGE_AURA, true);
            if (hunter->IsInCombat())
                if (Aura* aura = target->GetAura(SPELL_HUNTER_CAMOUFLAGE_AURA))
                    aura->SetDuration(6000);
            target->CastSpell(target, SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_DRIVER, true);
        }
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        std::vector<Unit*> v = { hunter };
        if (Pet* pet = hunter->GetPet())
            v.push_back(pet);

        for (auto&& target : v)
        {
            target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_AURA);
            target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_DRIVER);
            target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY);
            target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_GLYPHED);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hunt_camouflage_driver::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_camouflage_driver::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 51755 - Camouflage (Aura in visbile slot)
class spell_hunt_camouflage : public AuraScript
{
    PrepareAuraScript(spell_hunt_camouflage);

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
        Unit* target = GetTarget();
        Player* hunter = target->ToPlayer();
        if (!hunter)
            if (Pet* pet = target->ToPet())
                hunter = pet->GetOwner();
        if (hunter)
            hunter->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_DRIVER);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_camouflage::HandleRemove, EFFECT_0, SPELL_AURA_INTERFERE_TARGETTING, AURA_EFFECT_HANDLE_REAL);
    }
};

// 80326 - Camouflage
class spell_hunt_camouflage_invisibility_driver : public AuraScript
{
    PrepareAuraScript(spell_hunt_camouflage_invisibility_driver);

    void ApplyInvisibility(AuraEffect const* eff)
    {
        Unit* target = GetTarget();
        if (Player* hunter = target->GetAffectingPlayer())
        {
            if (hunter->HasAura(SPELL_HUNTER_GLYPH_OF_CAMOUFLAGE))
            {
                if (!target->HasAura(SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_GLYPHED))
                    target->CastSpell(target, SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY_GLYPHED, true);
            }
            else if (!target->isMoving())
            {
                if (!target->HasAura(SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY))
                    target->CastSpell(target, SPELL_HUNTER_CAMOUFLAGE_INVISIBILITY, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_camouflage_invisibility_driver::ApplyInvisibility, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 77767 - Cobra Shot
class spell_hunt_cobra_shot : public SpellScript
{
    PrepareSpellScript(spell_hunt_cobra_shot);

    void HandleHit(SpellEffIndex effIndex)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        if (AuraEffect* eff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_HUNTER, flag128{ 0x4000 }, GetCaster()->GetGUID()))
        {
            Aura* serpentSting = eff->GetBase();
            int32 maxDuration = serpentSting->CalcMaxDuration();
            int32 newDuration = serpentSting->GetDuration() + GetSpellInfo()->Effects[effIndex].BasePoints * 1000;
            if (newDuration > maxDuration)
            {
                if (eff->GetPeriodicTimer() < eff->GetAmplitude())
                    maxDuration += eff->GetPeriodicTimer();
                newDuration = maxDuration;
                serpentSting->SetMaxDuration(maxDuration);
            }
            serpentSting->SetDuration(newDuration);
            serpentSting->RecalculateAmountOfEffects();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_cobra_shot::HandleHit, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 53353 - Chimera Shot (Heal)
class spell_hunt_chimera_shot_heal : public SpellScript
{
    PrepareSpellScript(spell_hunt_chimera_shot_heal);

    void HandleHit(SpellEffIndex)
    {
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_HUNT_GLYPH_OF_CHIMERA_SHOT, EFFECT_0))
            SetEffectValue(GetEffectValue() + eff->GetAmount());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_chimera_shot_heal::HandleHit, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
    }
};

// 34487 - Master Marksman
class spell_hunt_master_marksman : public AuraScript
{
    PrepareAuraScript(spell_hunt_master_marksman );

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !GetUnitOwner()->HasAura(SPELL_HUNT_FIRE);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_master_marksman::CheckProc);
    }
};

// 82925 - Ready, Set, Aim...
class spell_hunt_ready_set_aim : public AuraScript
{
    PrepareAuraScript(spell_hunt_ready_set_aim);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetStackAmount() >= 3)
        {
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNT_FIRE, true);
            Remove();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hunt_ready_set_aim::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// 82926 - Fire!
class spell_hunt_fire : public AuraScript
{
    PrepareAuraScript(spell_hunt_fire);

    bool CheckProc(ProcEventInfo&) { return false; }

    void CalculateSpellMod(AuraEffect const* eff, SpellModifier*& spellmod)
    {
        if (!spellmod)
        {
            spellmod = new SpellModifier{ GetAura(), GetEffect(eff->GetEffIndex()) };
            spellmod->charges = 1;
            spellmod->mask = eff->GetSpellEffectInfo().SpellClassMask;
            spellmod->op = SpellModOp(eff->GetMiscValue());
            spellmod->spellId = eff->GetBase()->GetId();
            spellmod->type = SPELLMOD_PCT;
            spellmod->dropPhase = SpellModDropPhase::OnCast;
        }
        spellmod->value = -100;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_fire::CheckProc);
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_hunt_fire::CalculateSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_hunt_fire::CalculateSpellMod, EFFECT_1, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2);
    }
};

// 82726 - Fervor
class spell_hunt_fervor : public SpellScript
{
    PrepareSpellScript(spell_hunt_fervor);

    void HandleCast()
    {
        if (Player* hunter = GetCaster()->ToPlayer())
            if (Pet* pet = hunter->GetPet())
                pet->CastSpell(pet, GetSpellInfo()->Id, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_hunt_fervor::HandleCast);
    }
};

// 2643 - Multi Shot
class spell_hunt_multishot : public SpellScript
{
    PrepareSpellScript(spell_hunt_multishot);

    void HandleHit()
    {
        if (Player* hunter = GetCaster()->ToPlayer())
            if (hunter->HasSpell(SPELL_HUNT_SERPENT_SPREAD))
                hunter->CastSpell(GetHitUnit(), SPELL_HUNT_SERPENT_STING, true);
    }

    void HandleCast()
    {
        if (Player* hunter = GetCaster()->ToPlayer())
            if (hunter->HasAura(SPELL_HUNTER_BEAST_CLEAVE_AURA))
                if (Pet* pet = hunter->GetPet())
                    hunter->CastSpell(pet, SPELL_HUNTER_BEAST_CLEAVE_PROC, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_multishot::HandleHit);
        OnCast += SpellCastFn(spell_hunt_multishot::HandleCast);
    }
};

// 53224 - Steady Focus
class spell_hunt_steady_focus : public AuraScript
{
    PrepareAuraScript(spell_hunt_steady_focus);

    uint32 counter = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell() || eventInfo.TriggeredBySpell()->IsTriggered())
            return;

        if (eventInfo.GetSpellInfo()->Id != SPELL_HUNTER_STEADY_SHOT)
        {
            counter = 0;
            return;
        }

        if (++counter == 2)
        {
            counter = 0;
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNT_STEADY_FOCUS, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_hunt_steady_focus::HandleProc);
    }
};

// 109260 - Aspect of the Iron Hawk.
class spell_hunt_aspect_of_the_iron_hawk : public AuraScript
{
    PrepareAuraScript(spell_hunt_aspect_of_the_iron_hawk);

    void ScrewThat(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->ForceValuesUpdateAtIndex(UNIT_FIELD_SHAPESHIFT_FORM);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hunt_aspect_of_the_iron_hawk::ScrewThat, EFFECT_0, SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_aspect_of_the_iron_hawk::ScrewThat, EFFECT_0, SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 34477 - Misdirection, 110588 - Misdirection (Symbiosis)
class spell_hunt_misdirection : public AuraScript
{
    PrepareAuraScript(spell_hunt_misdirection);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (GetOwner() == caster && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT) // Expired or canceled
            caster->ResetRedirectThreat();
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();
        if (GetOwner() != caster)  // Target buff.
            return;

        Unit* target = caster->GetRedirectThreatTarget();
        if (target)
            target->RemoveAurasDueToSpell(SPELL_HUNT_MISDIRECTION);

        uint32 spellId = GetId() == SPELL_HUNT_MISDIRECTION_SYMBISIS ? SPELL_HUNT_MISDIRECTION_SYMBISIS_PROC : SPELL_HUNT_MISDIRECTION_PROC;
        caster->CastSpell(caster, spellId, true);
        Remove();
        if (caster->HasAura(SPELL_HUNT_GLYPH_OF_MISDIRECTION) && target == caster->GetGuardianPet())
            caster->GetSpellHistory()->RemoveCooldown(SPELL_HUNT_MISDIRECTION, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hunt_misdirection::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_hunt_misdirection::HandleProc);
    }
};

// 35079 - Misdirection (proc), 110591 - Misdirection (Symbiosis) (proc)
class spell_hunt_misdirection_proc : public AuraScript
{
    PrepareAuraScript(spell_hunt_misdirection_proc);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (!GetUnitOwner()->HasAura(SPELL_HUNT_MISDIRECTION))  // Possible with glyph
            GetUnitOwner()->ResetRedirectThreat();
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hunt_misdirection_proc::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 17253 - Bite, 16827 - Claw, 49966 - Smack
class spell_hunt_pet_basic_attack: public SpellScript
{
    PrepareSpellScript(spell_hunt_pet_basic_attack);

    void CalculateDamage(SpellEffIndex)
    {
        Unit* hunter = GetCaster()->GetOwner();
        if (!hunter)
            return;

        float rap = hunter->GetTotalAttackPowerValue(RANGED_ATTACK);
        float damage = GetEffectValue() + rap * 0.168f;
        if (AuraEffect const* spikedCollar = GetCaster()->GetAuraEffect(SPELL_HUNTER_SPIKED_COLLAR, EFFECT_0))
            AddPct(damage, spikedCollar->GetAmount());
        if (GetCaster()->GetPower(POWER_FOCUS) >= GetSpell()->GetPowerCost() * 2)   // Focus not taken yet
        {
            damage *= 2;
            GetCaster()->ModifyPower(POWER_FOCUS, -GetSpell()->GetPowerCost()); // We can do it right now.
        }
        SetEffectValue(damage);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_pet_basic_attack::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 118694 - Spirit Bond
class spell_hunt_spirit_bond : public AuraScript
{
    PrepareAuraScript(spell_hunt_spirit_bond);

    void Heal(AuraEffect const*)
    {
        if (GetTarget() == GetUnitOwner())
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNT_SPIRIT_BOND_HEAL, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_spirit_bond::Heal, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 149254 - Spirit Bond
class spell_hunt_spirit_bond_heal : public SpellScript
{
    PrepareSpellScript(spell_hunt_spirit_bond_heal);

    void HandleHit()
    {
        if (Player* hunter = GetHitUnit()->GetAffectingPlayer())
            SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), hunter->GetFloatValue(PLAYER_FIELD_PVP_POWER_HEALING)));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_spirit_bond_heal::HandleHit);
    }
};

// 120697 - Lynx Rush
class spell_hunt_lynx_rush : public AuraScript
{
    PrepareAuraScript(spell_hunt_lynx_rush);

    GuidSet m_targets;

    void HandleTick(AuraEffect const*)
    {
        Unit* pet = GetTarget();

        Unit* target = nullptr;
        while (!m_targets.empty())
        {
            ObjectGuid guid = Trinity::Containers::SelectRandomContainerElement(m_targets);
            target = ObjectAccessor::GetUnit(*pet, guid);
            if (target)
                break;
            m_targets.erase(guid);
        }

        if (!target)
            return;

        Position pos;
        target->GetRandomContactPoint(pet, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 5.f, 5.f);
        pet->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ);

        pet->CastSpell(target, SPELL_HUNT_LYNX_RUSH_DOT, true);
    }
public:
    void AddTarget(Unit* target)
    {
        m_targets.insert(target->GetGUID());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_lynx_rush::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 120697 - Lynx Rush
class spell_hunt_lynx_rush_selector : public SpellScript
{
    PrepareSpellScript(spell_hunt_lynx_rush_selector);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Guardian* pet = GetCaster()->GetGuardianPet())
            targets.remove_if([=](WorldObject const* target) { return !target->IsWithinDist3d(pet, 10.f); });
        if (targets.empty())
            FinishCast(SPELL_FAILED_OUT_OF_RANGE);
    }

    void AddTarget(SpellEffIndex)
    {
        if (Guardian* pet = GetCaster()->GetGuardianPet())
            if (Aura* lynxRush = pet->GetAura(120697))
                if (spell_hunt_lynx_rush* script = dynamic_cast<spell_hunt_lynx_rush*>(lynxRush->GetScriptByName("spell_hunt_lynx_rush")))
                    script->AddTarget(GetHitUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_lynx_rush_selector::SelectTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_hunt_lynx_rush_selector::AddTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 120699 - Lynx Rush (DoT)
class spell_hunt_lynx_rush_dot : public AuraScript
{
    PrepareAuraScript(spell_hunt_lynx_rush_dot);

    void CalculateAmount(AuraEffect const* eff, float& amount, bool&)
    {
        if (Unit* pet = GetCaster())
            if (Unit* hunter = pet->GetOwner())
                amount += (CalculatePct(hunter->GetTotalAttackPowerValue(RANGED_ATTACK), 19.0f) / 5);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_lynx_rush_dot::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 109259 - Powershot
class spell_hunt_powershot : public SpellScript
{
    PrepareSpellScript(spell_hunt_powershot);

    bool m_firstTime = true;
    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (m_firstTime)
        {
            Unit* hunt = GetCaster();
            Unit* hitunit = GetExplTargetUnit();
            for (auto&& target : targets)
                if (target->ToUnit() && target->IsInBetween(hunt, hitunit, 2.5f))
                    m_targets.push_back(target);
            m_firstTime = false;
            m_targets.push_back(hitunit);
        }
        targets.assign(m_targets.begin(), m_targets.end());
    }

    void IgnoreTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
    }

    void HandleHit()
    {
        if (GetHitUnit() != GetExplTargetUnit())
            SetHitDamage(GetHitDamage() / 2);
    }

    void Register() override
    {
        // FIXME: Need to redone fucking Spell::EffectWeaponDmg
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::SelectTargets, EFFECT_1, TARGET_UNK_134);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::SelectTargets, EFFECT_2, TARGET_UNK_134);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::SelectTargets, EFFECT_3, TARGET_UNK_134);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::IgnoreTargets, EFFECT_4, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::SelectTargets, EFFECT_5, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_powershot::SelectTargets, EFFECT_6, TARGET_UNIT_DEST_AREA_ENEMY);
        OnHit += SpellHitFn(spell_hunt_powershot::HandleHit);
    }
};

// 19574 - Bestial Wrath
class spell_hunt_bestial_wrath : public AuraScript
{
    PrepareAuraScript(spell_hunt_bestial_wrath);

    void InitEffects(uint32& effectMask)
    {
        Pet* pet = GetUnitOwner()->ToPet();
        Player* owner = pet ? pet->GetOwner() : nullptr;
        if (!owner || !owner->HasAura(SPELL_HUNT_GLYPH_OF_ENDLESS_WRATH))
            effectMask &= ~(1 << EFFECT_3);
    }

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void HandleAbsorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (GetUnitOwner()->GetHealth() <= dmgInfo.GetDamage())
            absorbAmount = dmgInfo.GetDamage();
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_hunt_bestial_wrath::InitEffects);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_bestial_wrath::CalculateAmount, EFFECT_3, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_hunt_bestial_wrath::HandleAbsorb, EFFECT_3);
    }
};

// 19574 - Bestial Wrath
class spell_hunt_beast_within : public SpellScript
{
    PrepareSpellScript(spell_hunt_beast_within);

    void HandleHit()
    {
        if (Pet* pet = GetHitUnit()->ToPet())
        {
            if (Player* owner = pet->GetOwner())
            {
                if (owner->HasAura(SPELL_HUNTER_THE_BEAST_WITHIN))
                {
                    owner->CastSpell(owner, SPELL_HUNTER_THE_BEAST_WITHIN_EFFECT, true);
                    owner->CastSpell(owner, SPELL_HUNTER_THE_BEAST_WITHIN_TRINKET, true);
                }
                else
                {
                    pet->CastSpell(pet, SPELL_HUNTER_THE_BEAST_WITHIN_TRINKET, true);
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_beast_within::HandleHit);
    }
};

// 76657 - Mastery: Master of Beasts
class spell_hunt_mastery_master_of_the_beasts : public AuraScript
{
    PrepareAuraScript(spell_hunt_mastery_master_of_the_beasts);

    void RecalculatePetModifiers(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* hunt = GetUnitOwner()->ToPlayer())
        {
            if (Pet* pet = hunt->GetPet())
                if (Aura* petPassive = pet->GetAura(SPELL_HUNTER_TAMED_PET_PASSIVE_01))
                    petPassive->RecalculateAmountOfEffects();
            for (auto&& it : hunt->GetSummons())
                if (Aura* petPassive = it->GetAura(SPELL_HUNTER_TAMED_PET_PASSIVE_01))
                    petPassive->RecalculateAmountOfEffects();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hunt_mastery_master_of_the_beasts::RecalculatePetModifiers, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_mastery_master_of_the_beasts::RecalculatePetModifiers, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 53397 - Invigoration 
class spell_hunt_invigoration : public AuraScript
{
    PrepareAuraScript(spell_hunt_invigoration);

    Pet* m_owner;

    bool Load() override
    {
        m_owner = GetUnitOwner()->ToPet();
        return m_owner != nullptr;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Player* player = m_owner->GetOwner())
            player->CastSpell(player, SPELL_HUNTER_INVIGORATION_ENERGIZE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_hunt_invigoration::HandleProc);
    }
};

// 109306 - Thrill of the Hunt
class spell_hunt_thrill_of_the_hunt : public AuraScript
{
    PrepareAuraScript(spell_hunt_thrill_of_the_hunt);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* spell = eventInfo.TriggeredBySpell();
        if (!spell)
            return false;

        if (spell->m_targets.GetUnitTarget() != eventInfo.GetActionTarget())
            return false;

        return (spell->GetPowerType() == POWER_FOCUS && spell->GetPowerCost() > 0) ||
            spell->GetSpellInfo()->Id == SPELL_HUNTER_KILL_COMMAND_DAMAGE;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_thrill_of_the_hunt::CheckProc);
    }
};

// 120361 - Barrage damage
class spell_hunt_barrage : public SpellScript
{
    PrepareSpellScript(spell_hunt_barrage);

    ObjectGuid mainTarget;

    void GetMainTarget(SpellEffIndex)
    {
        mainTarget = GetHitUnit()->GetGUID();
    }

    void HandleHit()
    {
        if (GetHitUnit()->GetGUID() != mainTarget)
            SetHitDamage(GetHitDamage() / 2);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_barrage::GetMainTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnHit += SpellHitFn(spell_hunt_barrage::HandleHit);
    }
};

// 55709 - Heart of the Phoenix
class spell_hunt_heart_of_the_phoenix : public SpellScript
{
    PrepareSpellScript(spell_hunt_heart_of_the_phoenix);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->IsAlive())
            return SPELL_FAILED_TARGET_NOT_DEAD;

        if (GetCaster()->HasAura(SPELL_HUNTER_HEART_OF_THE_PHOENIX_DEBUFF))
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        Unit* caster = GetCaster();
        if (Unit* owner = caster->GetOwner())
        {
            owner->CastCustomSpell(SPELL_HUNTER_HEART_OF_THE_PHOENIX_TRIGGERED, SPELLVALUE_BASE_POINT0, 100, caster, true);
            caster->CastSpell(caster, SPELL_HUNTER_HEART_OF_THE_PHOENIX_DEBUFF, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hunt_heart_of_the_phoenix::CheckCast);
        OnHit += SpellHitFn(spell_hunt_heart_of_the_phoenix::HandleHit);
    }
};

// 130392 - Blink Strike
class spell_hunt_blink_strikes : public SpellScript
{
    PrepareSpellScript(spell_hunt_blink_strikes);

    // This is pet spell. Check will be runned two times.
    bool m_checked  = false;
    bool m_teleport = false;

    SpellCastResult CheckCast()
    {
        if (!GetExplTargetUnit())
            return SPELL_FAILED_DONT_REPORT;
        if (!m_checked)
        {
            m_checked = true;
            Pet* pet = GetCaster()->ToPet();
            Player* hunter = pet ? pet->GetOwner() : nullptr;
            if (hunter && hunter->HasAura(SPELL_HUNTER_BLINK_STRIKES))
            {
                if (pet->IsWithinMeleeRange(GetExplTargetUnit()))
                    return SPELL_CAST_OK;
                if (pet->HasAuraType(SPELL_AURA_MOD_ROOT) || GetExplTargetUnit()->IsWithinDist3d(pet, 10.f) || hunter->HasSpellCooldown(SPELL_HUNTER_BLINK_STRIKE))
                    return SPELL_FAILED_OUT_OF_RANGE;

                m_teleport = true;
            }
        }
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        if (m_teleport)
        {
            Pet* pet = GetCaster()->ToPet();
            Player* hunt = pet ? pet->GetOwner() : nullptr;
            if (hunt)
            {
                pet->CastSpell(GetHitUnit(), SPELL_HUNTER_BLINK_STRIKE, true);
                hunt->AddCustomCooldown(SPELL_HUNTER_BLINK_STRIKE, Seconds(20));
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hunt_blink_strikes::CheckCast);
        OnHit += SpellHitFn(spell_hunt_blink_strikes::HandleHit);
    }
};

// 131894 - A Murder of Crows
class spell_hunt_a_murder_of_crows : public SpellScript
{
    PrepareSpellScript(spell_hunt_a_murder_of_crows);

    void HandleCast()
    {
        if (GetExplTargetUnit() && GetExplTargetUnit()->HealthBelowPct(20))
            if (Player* hunter = GetCaster()->ToPlayer())
                hunter->ModifySpellCooldown(SPELL_HUNTER_A_MUREDER_OF_CROWS, -(GetSpellInfo()->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_hunt_a_murder_of_crows::HandleCast);
    }
};

// 131894 - A Murder of Crows
class spell_hunt_a_murder_of_crows_dot : public AuraScript
{
    PrepareAuraScript(spell_hunt_a_murder_of_crows_dot);

    void HandleTick(AuraEffect const* eff)
    {
        Unit* target = GetTarget();
        // Visual. This is wrong, I dunno how these two spells must be casted
        target->CastSpell(target, eff->GetTickNumber() % 2 ? 131951 : 131952, true);
        if (Unit* hunter = GetCaster())
            hunter->CastSpell(target, SPELL_HUNTER_A_MUREDER_OF_CROWS_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_a_murder_of_crows_dot::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 53238 - Piercing Shots
class spell_hunt_piercing_shots : public AuraScript
{
    PrepareAuraScript(spell_hunt_piercing_shots);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetActionTarget();
        float damage = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), eff->GetAmount()) / 8; // tick count
        GetUnitOwner()->CastCustomSpell(SPELL_HUNTER_PIERCING_SHOTS, SPELLVALUE_BASE_POINT0, damage, target, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hunt_piercing_shots::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 90361 - Spirit Mend
class spell_hunt_spirit_mend : public SpellScript
{
    PrepareSpellScript(spell_hunt_spirit_mend);

    void CalculateHeal(SpellEffIndex)
    {
        if (Unit* hunt = GetCaster()->GetOwner())
        {
            int32 damage = GetEffectValue() + hunt->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.35f * 0.5f;
            damage += hunt->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.61f * 0.5f;
            SetEffectValue(damage);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_spirit_mend::CalculateHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 90361 - Spirit Mend
class spell_hunt_spirit_mend_hot : public AuraScript
{
    PrepareAuraScript(spell_hunt_spirit_mend_hot);

    void CalculateHeal(AuraEffect const* eff, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* hunt = caster->GetOwner())
            {
                amount = amount + hunt->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.35f * 0.335f;
                amount += hunt->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.61f * 0.335f;
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_spirit_mend_hot::CalculateHeal, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 128997 - Spirit Beast Blessing
class spell_spirit_beast_blessing : public SpellScript
{
    PrepareSpellScript(spell_spirit_beast_blessing);

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_HUNTER_SPIRIT_BEAST_BLESSING, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_spirit_beast_blessing::HandleCast);
    }
};

// 510
class sat_hunt_flare : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* object) override
    {
        Unit* target = object->ToUnit();
        return target && target->IsAlive() && GetCaster()->IsValidAttackTarget(target, nullptr, nullptr, true) && GetAreaTrigger()->IsWithinLOSInMap(target);
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        object->ToUnit()->CastSpell(object->ToUnit(), SPELL_HUNTER_FLARE, true);
    }

    void OnTriggeringRemove(WorldObject* object) override
    {
        object->ToUnit()->RemoveAurasDueToSpell(SPELL_HUNTER_FLARE);
    }
};

// 136 - Mend Pet
class spell_hunt_glyph_of_mend_pet : public AuraScript
{
    PrepareAuraScript(spell_hunt_glyph_of_mend_pet);

    void HandleTick(AuraEffect const* eff)
    {
        if (Unit* hunter = GetCaster())
            if (hunter->HasAura(SPELL_HUNTER_GLYPH_OF_MEND_PET))
                hunter->CastSpell(GetTarget(), SPELL_HUNTER_GLYPH_OF_MEND_PET_DISPEL, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hunt_glyph_of_mend_pet::HandleTick, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 5118 - Aspect of the Cheetah
class spell_hunt_aspect_of_the_cheetah : public AuraScript
{
    PrepareAuraScript(spell_hunt_aspect_of_the_cheetah);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo())
            return !eventInfo.GetSpellInfo()->IsPositive();
        return true;
    }

    void HandleProc(ProcEventInfo&)
    {
        if (GetUnitOwner()->HasAura(SPELL_HUNTER_GLYPH_OF_ASPECT_OF_THE_CHEETAH))
        {
            PreventDefaultAction();
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_HUNTER_ASPECTS_COOLDOWN, TRIGGERED_WITH_SPELL_START);
            Remove();
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_aspect_of_the_cheetah::CheckProc);
        OnProc += AuraProcFn(spell_hunt_aspect_of_the_cheetah::HandleProc);
    }
};

// 781 - Disengage
class spell_hunt_posthaste : public SpellScript
{
    PrepareSpellScript(spell_hunt_posthaste);

    bool Load() override
    {
        if (GetCaster()->HasAura(SPELL_HUNTER_POSTHASTE))
        {
            GetCaster()->RemoveMovementImpairingAuras();
            return true;
        }
        return false;
    }

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_HUNTER_POSTHASTE_INCREASE_SPEED, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_hunt_posthaste::HandleCast);
    }
};

// 781 - Disengage
class spell_hunt_narrow_escape : public SpellScript
{
    PrepareSpellScript(spell_hunt_narrow_escape);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_HUNTER_NARROW_ESCAPE))
            GetCaster()->CastSpell(GetCaster(), SPELL_HUNTER_NARROW_ESCAPE_SELECTOR, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_hunt_narrow_escape::HandleCast);
    }
};

// 115928 - Narrow escape
class spell_hunt_narrow_escape_selector : public SpellScript
{
    PrepareSpellScript(spell_hunt_narrow_escape_selector);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_HUNTER_NARROW_ESCAPE_ROOT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_hunt_narrow_escape_selector::HandleHit);
    }
};

// 95725 - Froststorm Breath
class spell_hunt_froststorm_breath : public SpellScript
{
    PrepareSpellScript(spell_hunt_froststorm_breath);

    void CalculateDamage(SpellEffIndex)
    {
        if (Unit* hunter = GetCaster()->GetOwner())
            SetEffectValue(GetEffectValue() + hunter->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.144f);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_froststorm_breath::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 63900 - Thunderstomp
class spell_hunt_thunderstomp : public SpellScript
{
    PrepareSpellScript(spell_hunt_thunderstomp);

    void CalculateDamage(SpellEffIndex)
    {
        // Actually ${$M1+(($RAP*0.40)*0.0245)} but it seems to include standard ap modifier
        if (Unit* hunter = GetCaster()->GetOwner())
            SetEffectValue(GetEffectValue() + hunter->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.03428f);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_thunderstomp::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 95714 - Burrow Attack
class spell_hunt_burrow_attack : public SpellScript
{
    PrepareSpellScript(spell_hunt_burrow_attack);

    void CalculateDamage(SpellEffIndex)
    {
        if (Unit* hunter = GetCaster()->GetOwner())
            SetEffectValue(GetEffectValue() + hunter->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.4f * 0.288f);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_hunt_burrow_attack::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 5118 - Aspect of the Cheetah
// 61648 - Aspect of the Beast
// 13159 - Aspect of the Pack
// 13165 - Aspect of the Hawk
// 109260 - Aspect of the Iron Hawk
class spell_glyph_of_the_aspects : public SpellScript
{
    PrepareSpellScript(spell_glyph_of_the_aspects);

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_HUNTER_GLYPH_OF_ASPECTS);
    }

    void HandleCast()
    {
        static std::map<uint32, uint32> const map
        {
            { 13165,  122487 }, // Aspect of the Hawk
            { 109260, 122487 }, // Aspect of the Hawk
            { 5118,   122489 }, // Aspect of the Cheetah
            { 13159,  122490 }, // Aspect of the Pack
            { 61648,  130221 }, // Aspect of the Beast
        };

        std::list<TempSummon*> summons;
        for (auto&& itr : map)
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr.second))
                GetCaster()->GetSummons(summons, spellInfo->Effects[EFFECT_0].MiscValue);

        for (auto&& itr : summons)
            itr->UnSummon();

        auto itr = map.find(GetSpellInfo()->Id);
        if (itr != map.end())
            GetCaster()->CastSpell(GetCaster(), itr->second, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_glyph_of_the_aspects::HandleCast);
    }
};

// 2641 - Dismiss Pet
class spell_hunt_dissmiss_pet : public SpellScript
{
    PrepareSpellScript(spell_hunt_dissmiss_pet);

    SpellCastResult CheckCast()
    {
        // http://us.battle.net/wow/en/game/patch-notes/5-4-2 
        // Dismiss Pet can no longer be used while in an Arena match.
        if (Battleground* bg = GetCaster()->GetBattlegorund())
            if (bg->IsArena() && bg->GetStatus() == STATUS_IN_PROGRESS)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hunt_dissmiss_pet::CheckCast);
    }
};

// 127933 - Fireworks
class spell_hunt_fireworks : public SpellScript
{
    PrepareSpellScript(spell_hunt_fireworks);

    void HandleCast()
    {
        uint32 mainfire = roll_chance_i(50) ? 127936 : 127961; // Red Streak or Blue Dots
        uint32 sidefire = roll_chance_i(50) ? 127951 : 127937; // Yellow Rose or Roman Candle

        GetCaster()->CastSpell(GetCaster(), mainfire, true);
        GetCaster()->CastSpell(GetCaster(), sidefire, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_hunt_fireworks::HandleCast);
    }
};

// 125050 - Fetch
class spell_hunt_fetch : public SpellScript
{
    PrepareSpellScript(spell_hunt_fetch);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Player* hunter = GetCaster()->ToPlayer();
        targets.remove_if([=](WorldObject const* target)
        {
            Creature const* creature = target->ToCreature();
            if (!creature)
                return true;

            if (creature->IsAlive())
                return true;

            return !hunter->IsAllowedToLoot(creature);
        });

        targets.sort(Trinity::DistanceCompareOrderPred{ hunter });
        if (!targets.empty())
            targets.resize(1);
    }

    void HandleHit(SpellEffIndex)
    {
        if (!GetHitCreature())
            return;

        if (Unit* pet = GetCaster()->GetGuardianPet())
        {
            ObjectGuid guid = GetHitCreature()->GetGUID();
            pet->GetMotionMaster()->MovePoint(0, GetHitCreature()->GetPosition());
            pet->Schedule(Milliseconds(pet->GetSplineDuration()), [=]()
            {
                if (Creature* corpse = ObjectAccessor::GetCreature(*pet, guid))
                    pet->CastSpell(corpse, 125056);
            });
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_fetch::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_hunt_fetch::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 125056 - Fetch
class spell_hunt_fetch_loot : public SpellScript
{
    PrepareSpellScript(spell_hunt_fetch_loot);

    void HandleHit(SpellEffIndex)
    {
        if (!GetHitCreature())
            return;

        Player* hunter = GetCaster()->GetAffectingPlayer();
        if (!hunter)
            return;

        Unit* pet = GetCaster();

        std::list<Creature*> corpses;
        auto check = [=](Creature const* creature)
        {
            if (creature->IsAlive())
                return false;

            if (creature->GetGUID() == GetHitCreature()->GetGUID())
                return false;

            if (!pet->IsWithinDist(creature, 30.0f))
                return false;

            return hunter->IsAllowedToLoot(creature);
        };

        Trinity::CreatureListSearcher<decltype(check)> searcher{ pet, corpses, check };
        pet->VisitNearbyGridObject(30, searcher);

        WorldPacket data{ SMSG_AE_LOOT_TARGETS, 4 };
        data << uint32(corpses.size());
        hunter->SendDirectMessage(&data);

        hunter->SendLoot(GetHitCreature()->GetGUID(), LOOT_CORPSE);

        for (auto&& creature : corpses)
            hunter->SendLoot(creature->GetGUID(), LOOT_CORPSE, true);

        pet->GetCharmInfo()->SetIsFollowing(false);
        pet->GetCharmInfo()->SetIsReturning(false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_fetch_loot::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 20736 - Distracting Shot
class spell_hunt_glyph_of_distracting_shot : public SpellScript
{
    PrepareSpellScript(spell_hunt_glyph_of_distracting_shot);

    void HandleThreat(SpellEffIndex effIndex)
    {
        Unit* pet = GetCaster()->GetGuardianPet();
        if (GetCaster()->HasAura(SPELL_HUNTER_GLYPH_OF_DISTRACTING_SHOT) && pet)
        {
            PreventDefaultEffect(effIndex);
            GetHitUnit()->AddThreat(pet, GetEffectValue());
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hunt_glyph_of_distracting_shot::HandleThreat, EFFECT_0, SPELL_EFFECT_THREAT);
    }
};

// 93435 - Roar of Courage
// 127830 - Spirit Beast Blessing
class spell_hunt_mastery_roar_target_fix : public SpellScript
{
    PrepareSpellScript(spell_hunt_mastery_roar_target_fix);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target)
        {
            if (Unit* unit = target->ToUnit())
                if (Aura* blessing = unit->GetAura(19740)) // Blessing of Might
                    if (blessing->GetDuration() >= 60 * IN_MILLISECONDS)
                        return true;
            return false;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hunt_mastery_roar_target_fix::SelectTargets, EFFECT_0, sSpellMgr->GetSpellInfo(m_scriptSpellId)->Effects[EFFECT_0].TargetA.GetTarget());
    }
};

// 20736 - Distracting Shot
class spell_hunt_glyph_of_distracting_shot_taunt : public AuraScript
{
    PrepareAuraScript(spell_hunt_glyph_of_distracting_shot_taunt);

    bool glyphed = false;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTarget()->GetTypeId() != TYPEID_UNIT)
            return;

        Unit* hunter = GetCaster();
        if (!hunter || !hunter->HasAura(SPELL_HUNTER_GLYPH_OF_DISTRACTING_SHOT))
            return;

        if (Unit* pet = hunter->GetGuardianPet())
        {
            PreventDefaultAction();
            glyphed = true;
            GetTarget()->TauntApply(pet);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (glyphed)
        {
            PreventDefaultAction();

            if (Unit* hunter = GetCaster())
                if (Unit* pet = hunter->GetGuardianPet())
                    GetTarget()->TauntFadeOut(pet);
        }
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn(spell_hunt_glyph_of_distracting_shot_taunt::HandleApply,  EFFECT_1, SPELL_AURA_MOD_TAUNT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_hunt_glyph_of_distracting_shot_taunt::HandleRemove, EFFECT_1, SPELL_AURA_MOD_TAUNT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 19263, 67801, 110618, 148467 - Deterrence
class spell_hunt_deterrence : public AuraScript
{
    PrepareAuraScript(spell_hunt_deterrence);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        // 100 and -100, if attacker has +hit, he still can hit through deterrence
        amount *= 2;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_deterrence::CalculateAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_deterrence::CalculateAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    }
};

// 19263, 148467 - Deterrence
class spell_hunt_glyph_of_deterrence : public AuraScript
{
    PrepareAuraScript(spell_hunt_glyph_of_deterrence);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* glyph = GetUnitOwner()->GetAuraEffect(SPELL_HUNTER_GLYPH_OF_DETERRENCE, EFFECT_0))
            amount += glyph->GetFloatAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hunt_glyph_of_deterrence::CalculateAmount, EFFECT_4, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// 19921, 19833
struct npc_pet_hunter_snake : public ScriptedAI
{
    npc_pet_hunter_snake(Creature* creature) : ScriptedAI(creature) { }

    enum { NPC_HUNTER_VIPER = 19921 };

    void JustEngagedWith(Unit*) override { }

    void InitializeAI() override
    {
        spellTimer = 0;
        isViper = me->GetEntry() == NPC_HUNTER_VIPER;

        // Start attacking attacker of owner on first ai update after spawn - move in line of sight may choose better target
        if (!me->GetVictim() && me->IsSummon())
            if (Unit* owner = me->ToTempSummon()->GetSummoner())
                if (Unit* target = owner->getAttackerForHelper())
                    AttackStart(target);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!me->GetVictim() && me->CanCreatureAttack(who))
        {
            if (me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = me->GetAttackDistance(who);
            if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
            {
                if (!(rand() % 5))
                {
                    me->setAttackTimer(BASE_ATTACK, (rand() % 10) * 100);
                    spellTimer = (rand() % 10) * 100;
                    AttackStart(who);
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
        {
            me->InterruptNonMeleeSpells(false);
            return;
        }

        if (spellTimer <= diff)
        {
            if (isViper) // Viper
            {
                if (urand(0, 2) == 0) // 33% chance to cast
                {
                    uint32 spell;
                    if (urand(0, 1) == 0)
                        spell = SPELL_HUNTER_MIND_NUMBING_POISON;
                    else
                        spell = SPELL_HUNTER_CRIPPLING_POISON;

                    DoCastVictim(spell);
                }

                spellTimer = 3000;
            }
            else // Venomous Snake
            {
                if (urand(0, 2) == 0) // 33% chance to cast
                    DoCastVictim(SPELL_HUNTER_DEADLY_POISON);
                spellTimer = 1500 + (rand() % 5) * 100;
            }
        }
        else
            spellTimer -= diff;

        DoMeleeAttackIfReady();
    }

private:
    bool isViper = false;
    uint32 spellTimer = 0;
};

// 69887
struct npc_hunter_thunderhawk : public ScriptedAI
{
    enum { SPELL_LIGHTNING_BLAST = 138374 };

    AssistBehavior assist{ me };
    CasterMovement casterMovement = CasterMovement(me).Spell(SPELL_LIGHTNING_BLAST);

    npc_hunter_thunderhawk(Creature* c) : ScriptedAI(c) { }

    void InitializeAI() override
    {
        Unit* victim = me->SelectVictim();
        if (victim)
            AttackStart(victim);
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (!UpdateVictim())
            return;

        if (me->GetVictim() && me->GetVictim()->HasBreakableByDamageCrowdControlAura())
        {
            me->InterruptNonMeleeSpells(true);
            me->AttackStop();
        }

        casterMovement.Update(diff);

        DoCastVictim(SPELL_LIGHTNING_BLAST);
    }

    void AttackStart(Unit* victim) override
    {
        if (me->Attack(victim, false))
            casterMovement.Chase(victim);
    }
};

// 3355 - Freezing Trap
// 19503 - Scatter Shot
class spell_hunt_crowd_control_dot_hack : public AuraScript
{
    PrepareAuraScript(spell_hunt_crowd_control_dot_hack);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.TriggeredByPeriodic())
            if (eventInfo.GetSpellInfo()->Id == SPELL_HUNTER_PIERCING_SHOTS || eventInfo.GetSpellInfo()->Id == SPELL_HUNTER_EXPLOSIVE_SHOT)
                return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_crowd_control_dot_hack::CheckProc);
    }
};

// 144637 - Item - Hunter T16 2P Bonus
class spell_hunt_t16_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_hunt_t16_2p_bonus);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* hunter = GetUnitOwner()->ToPlayer();
        uint32 effIndex;
        switch (hunter->GetSpecialization())
        {
            case SPEC_HUNTER_BEAST_MASTERY: effIndex = EFFECT_1; break;
            case SPEC_HUNTER_MARKSMANSHIP:  effIndex = EFFECT_2; break;
            default:                        effIndex = EFFECT_3; break;
        }

        hunter->ModifySpellCooldown(SPELL_HUNTER_RAPID_FIRE, -GetSpellInfo()->Effects[effIndex].BasePoints * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_hunt_t16_2p_bonus::HandleProc);
    }
};

// 144641 - Item - Hunter T16 4P Bonus
class spell_hunt_t16_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_hunt_t16_4p_bonus);

    Player* hunter = nullptr;

    bool Load() override
    {
        hunter = GetOwner()->ToPlayer();
        return hunter != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (hunter->GetSpecialization() == SPEC_HUNTER_MARKSMANSHIP)
        {
            if (!eventInfo.TriggeredBySpell())
                return false;
            if (!(eventInfo.GetSpellInfo()->SpellFamilyFlags[0] & 0x00020000))
                return false;
            return eventInfo.TriggeredBySpell()->GetCastTime() == 0;
        }
        else if (hunter->GetSpecialization() == SPEC_HUNTER_BEAST_MASTERY)
        {
            return GetUnitOwner()->HasAura(SPELL_HUNTER_THE_BEAST_WITHIN_EFFECT);
        }
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (hunter->GetSpecialization() == SPEC_HUNTER_MARKSMANSHIP)
            hunter->CastSpell(hunter, SPELL_HUNTER_KEEN_EYE, true);
        else
        {
            hunter->CastSpell(hunter, SPELL_HUNTER_BRUTAL_KINSHIP, true);
            hunter->CastSpell(hunter, SPELL_HUNTER_BRUTAL_KINSHIP_PET, true);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_t16_4p_bonus::CheckProc);
        OnProc += AuraProcFn(spell_hunt_t16_4p_bonus::HandleProc);
    }
};

// 56453 - Lock and Load
class spell_hunt_t16_4p_bonus_survival : public AuraScript
{
    PrepareAuraScript(spell_hunt_t16_4p_bonus_survival);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (AuraEffect const* bonus = GetUnitOwner()->GetAuraEffect(SPELL_HUNTER_T16_4P_BONUS, EFFECT_0))
            if (roll_chance_i(bonus->GetAmount()))
                return false;
        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        // FIXME
        if (GetCharges() == 1)
        {
            PreventDefaultAction();
            SetDuration(0);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hunt_t16_4p_bonus_survival::CheckProc);
        OnProc += AuraProcFn(spell_hunt_t16_4p_bonus_survival::HandleProc);
    }
};

// 34471 - The Beast Within
class spell_hunt_remove_brutal_kinship : public AuraScript
{
    PrepareAuraScript(spell_hunt_remove_brutal_kinship);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_HUNTER_BRUTAL_KINSHIP);
        if (Unit* pet = GetUnitOwner()->GetGuardianPet())
            pet->RemoveAurasDueToSpell(SPELL_HUNTER_BRUTAL_KINSHIP_PET);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_remove_brutal_kinship::HandleRemove, EFFECT_0, SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 57870 - Glyph of Lesser Proportion
class spell_hunt_glyph_of_lesser_proportions : public AuraScript
{
    PrepareAuraScript(spell_hunt_glyph_of_lesser_proportions);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* pet = GetUnitOwner()->GetGuardianPet())
            pet->CastSpell(pet, SPELL_HUNTER_LESSER_PROPORTION);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* pet = GetUnitOwner()->GetGuardianPet())
            pet->RemoveAurasDueToSpell(SPELL_HUNTER_LESSER_PROPORTION);
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_hunt_glyph_of_lesser_proportions::HandleApply,  EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hunt_glyph_of_lesser_proportions::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_hunter_spell_scripts()
{
    new spell_hun_glyph_of_aspect_of_the_beast();
    new spell_hun_tracking();
    new spell_hun_dash();
    new spell_script<spell_hunt_stampede>("spell_hunt_stampede");
    new spell_script<spell_hunt_dire_beast>("spell_hunt_dire_beast");
    new aura_script<spell_hunt_dire_beast_focus_driver>("spell_hunt_dire_beast_focus_driver");
    new spell_hun_focus_fire();
    new spell_hun_frenzy();
    new aura_script<spell_hunt_beast_cleave_proc>("spell_hunt_beast_cleave_proc");
    new spell_script<spell_hunt_beast_cleave_damage>("spell_hunt_beast_cleave_damage");
    new spell_script<spell_hunt_binding_shot_missile>("spell_hunt_binding_shot_missile");
    new aura_script<spell_hunt_binding_shot>("spell_hunt_binding_shot");
    new aura_script<spell_hunt_binding_shot_zone>("spell_hunt_binding_shot_zone");
    new aura_script<spell_hunt_improved_serpent_sting>("spell_hunt_improved_serpent_sting");
    new spell_hun_chimera_shot();
    new spell_script<spell_hunt_last_stand_pet>("spell_hunt_last_stand_pet");
    new spell_script<spell_hunt_masters_call>("spell_hunt_masters_call");
    new spell_hun_scatter_shot();
    new spell_hun_sniper_training();
    new spell_hun_pet_carrion_feeder();
    new spell_hun_tame_beast();
    new spell_hun_pet_dust_cloud();
    new spell_script<spell_hunt_explosive_trap>("spell_hunt_explosive_trap");
    new spell_script<spell_hunt_ice_trap>("spell_hunt_ice_trap");
    new atrigger_script<sat_hunt_ice_trap>("sat_hunt_ice_trap");
    new atrigger_script<sat_hunt_ice_trap_black_ice>("sat_hunt_ice_trap_black_ice");
    new aura_script<spell_hunt_entrapment>("spell_hunt_entrapment");
    new aura_script<spell_hunt_wild_quiver>("spell_hunt_wild_quiver");
    new aura_script<spell_hunt_glaive_toss>("spell_hunt_glaive_toss");
    new spell_script<spell_hunt_glaive_toss_damage>("spell_hunt_glaive_toss_damage");
    new spell_script<spell_hunt_glaive_toss_missile>("spell_hunt_glaive_toss_missile");
    new spell_script<spell_hunt_steady_cobra_shot>("spell_hunt_steady_cobra_shot");
    new aura_script<spell_hunt_cobra_strikes>("spell_hunt_cobra_strikes");
    new spell_script<spell_hunt_cobra_strikes_stacks>("spell_hunt_cobra_strikes_stacks");
    new aura_script<spell_hunt_lock_and_load>("spell_hunt_lock_and_load");
    new spell_script<spell_hunt_lock_and_load_cooldown>("spell_hunt_lock_and_load_cooldown");
    new spell_script<spell_hunt_kill_command>("spell_hunt_kill_command");
    new spell_script<spell_hunt_kill_command_damage>("spell_hunt_kill_command_damage");
    new spell_script<spell_hunt_hunters_mark_trigger>("spell_hunt_hunters_mark_trigger");
    new aura_script<spell_hunt_camouflage_driver>("spell_hunt_camouflage_driver");
    new aura_script<spell_hunt_camouflage>("spell_hunt_camouflage");
    new aura_script<spell_hunt_camouflage_invisibility_driver>("spell_hunt_camouflage_invisibility_driver");
    new spell_script<spell_hunt_cobra_shot>("spell_hunt_cobra_shot");
    new spell_script<spell_hunt_chimera_shot_heal>("spell_hunt_chimera_shot_heal");
    new aura_script<spell_hunt_master_marksman>("spell_hunt_master_marksman");
    new aura_script<spell_hunt_ready_set_aim>("spell_hunt_ready_set_aim");
    new aura_script<spell_hunt_fire>("spell_hunt_fire");
    new spell_script<spell_hunt_fervor>("spell_hunt_fervor");
    new spell_script<spell_hunt_multishot>("spell_hunt_multishot");
    new aura_script<spell_hunt_steady_focus>("spell_hunt_steady_focus");
    new aura_script<spell_hunt_aspect_of_the_iron_hawk>("spell_hunt_aspect_of_the_iron_hawk");
    new aura_script<spell_hunt_misdirection>("spell_hunt_misdirection");
    new aura_script<spell_hunt_misdirection_proc>("spell_hunt_misdirection_proc");
    new spell_script<spell_hunt_pet_basic_attack>("spell_hunt_pet_basic_attack");
    new aura_script<spell_hunt_spirit_bond>("spell_hunt_spirit_bond");
    new spell_script<spell_hunt_spirit_bond_heal>("spell_hunt_spirit_bond_heal");
    new aura_script<spell_hunt_lynx_rush>("spell_hunt_lynx_rush");
    new spell_script<spell_hunt_lynx_rush_selector>("spell_hunt_lynx_rush_selector");
    new aura_script<spell_hunt_lynx_rush_dot>("spell_hunt_lynx_rush_dot");
    new spell_script<spell_hunt_powershot>("spell_hunt_powershot");
    new aura_script<spell_hunt_bestial_wrath>("spell_hunt_bestial_wrath");
    new spell_script<spell_hunt_beast_within>("spell_hunt_beast_within");
    new aura_script<spell_hunt_mastery_master_of_the_beasts>("spell_hunt_mastery_master_of_the_beasts");
    new aura_script<spell_hunt_invigoration>("spell_hunt_invigoration");
    new aura_script<spell_hunt_thrill_of_the_hunt>("spell_hunt_thrill_of_the_hunt");
    new spell_script<spell_hunt_barrage>("spell_hunt_barrage");
    new spell_script<spell_hunt_heart_of_the_phoenix >("spell_hunt_heart_of_the_phoenix");
    new spell_script<spell_hunt_blink_strikes>("spell_hunt_blink_strikes");
    new spell_script<spell_hunt_a_murder_of_crows>("spell_hunt_a_murder_of_crows");
    new aura_script<spell_hunt_a_murder_of_crows_dot>("spell_hunt_a_murder_of_crows_dot");
    new aura_script<spell_hunt_piercing_shots>("spell_hunt_piercing_shots");
    new spell_script<spell_hunt_spirit_mend>("spell_hunt_spirit_mend");
    new aura_script<spell_hunt_spirit_mend_hot>("spell_hunt_spirit_mend_hot");
    new spell_script<spell_spirit_beast_blessing>("spell_spirit_beast_blessing");
    new atrigger_script<sat_hunt_flare>("sat_hunt_flare");
    new aura_script<spell_hunt_glyph_of_mend_pet>("spell_hunt_glyph_of_mend_pet");
    new aura_script<spell_hunt_aspect_of_the_cheetah>("spell_hunt_aspect_of_the_cheetah");
    new spell_script<spell_hunt_posthaste>("spell_hunt_posthaste");
    new spell_script<spell_hunt_narrow_escape>("spell_hunt_narrow_escape");
    new spell_script<spell_hunt_narrow_escape_selector>("spell_hunt_narrow_escape_selector");
    new spell_script<spell_hunt_froststorm_breath>("spell_hunt_froststorm_breath");
    new spell_script<spell_hunt_thunderstomp>("spell_hunt_thunderstomp");
    new spell_script<spell_hunt_burrow_attack>("spell_hunt_burrow_attack");
    new spell_script<spell_glyph_of_the_aspects>("spell_glyph_of_the_aspects");
    new spell_script<spell_hunt_dissmiss_pet>("spell_hunt_dissmiss_pet");
    new spell_script<spell_hunt_fireworks>("spell_hunt_fireworks");
    new spell_script<spell_hunt_fetch>("spell_hunt_fetch");
    new spell_script<spell_hunt_fetch_loot>("spell_hunt_fetch_loot");
    new spell_script<spell_hunt_glyph_of_distracting_shot>("spell_hunt_glyph_of_distracting_shot");
    new spell_script<spell_hunt_mastery_roar_target_fix>("spell_hunt_mastery_roar_target_fix");
    new aura_script<spell_hunt_glyph_of_distracting_shot_taunt>("spell_hunt_glyph_of_distracting_shot_taunt");
    new aura_script<spell_hunt_deterrence>("spell_hunt_deterrence");
    new aura_script<spell_hunt_glyph_of_deterrence>("spell_hunt_glyph_of_deterrence");
    new creature_script<npc_pet_hunter_snake>("npc_pet_hunter_snake");
    new creature_script<npc_hunter_thunderhawk>("npc_hunter_thunderhawk");
    new aura_script<spell_hunt_crowd_control_dot_hack>("spell_hunt_crowd_control_dot_hack");
    new aura_script<spell_hunt_t16_2p_bonus>("spell_hunt_t16_2p_bonus");
    new aura_script<spell_hunt_t16_4p_bonus>("spell_hunt_t16_4p_bonus");
    new aura_script<spell_hunt_t16_4p_bonus_survival>("spell_hunt_t16_4p_bonus_survival");
    new aura_script<spell_hunt_remove_brutal_kinship>("spell_hunt_remove_brutal_kinship");
    new aura_script<spell_hunt_glyph_of_lesser_proportions>("spell_hunt_glyph_of_lesser_proportions");
}
