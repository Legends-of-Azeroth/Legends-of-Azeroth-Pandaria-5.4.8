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
 * Scripts for spells with SPELLFAMILY_GENERIC which cannot be included in AI script file
 * of creature using it or can't be bound to any player class.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_gen_"
 */

#include "ScriptMgr.h"
#include "Battleground.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "SkillDiscovery.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "DevTool.h"
#include "Guild.h"
#include "SpellHistory.h"
#include "Vehicle.h"
#include "CreatureTextMgr.h"
#include "Random.h"

class spell_gen_absorb0_hitlimit1 : public AuraScript
{
    PrepareAuraScript(spell_gen_absorb0_hitlimit1);

    uint32 limit;

    bool Load() override
    {
        // Max absorb stored in 1 dummy effect
        limit = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
        return true;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        absorbAmount = std::min(limit, absorbAmount);
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb0_hitlimit1::Absorb, EFFECT_0);
    }
};

// 28764 - Adaptive Warding (Frostfire Regalia Set)
enum AdaptiveWarding
{
    SPELL_GEN_ADAPTIVE_WARDING_FIRE         = 28765,
    SPELL_GEN_ADAPTIVE_WARDING_NATURE       = 28768,
    SPELL_GEN_ADAPTIVE_WARDING_FROST        = 28766,
    SPELL_GEN_ADAPTIVE_WARDING_SHADOW       = 28769,
    SPELL_GEN_ADAPTIVE_WARDING_ARCANE       = 28770
};

class spell_gen_adaptive_warding : public AuraScript
{
    PrepareAuraScript(spell_gen_adaptive_warding);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_FIRE) ||
            !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_NATURE) ||
            !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_FROST) ||
            !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_SHADOW) ||
            !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_ARCANE))
            return false;
        return true;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetDamageInfo()->GetSpellInfo()) // eventInfo.GetSpellInfo()
            return false;

        // find Mage Armor
        if (!GetTarget()->GetAuraEffect(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT, SPELLFAMILY_MAGE, flag128{ 0x10000000 }))
            return false;

        switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
        {
            case SPELL_SCHOOL_NORMAL:
            case SPELL_SCHOOL_HOLY:
                return false;
            default:
                break;
        }
        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId = 0;
        switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
        {
            case SPELL_SCHOOL_FIRE:
                spellId = SPELL_GEN_ADAPTIVE_WARDING_FIRE;
                break;
            case SPELL_SCHOOL_NATURE:
                spellId = SPELL_GEN_ADAPTIVE_WARDING_NATURE;
                break;
            case SPELL_SCHOOL_FROST:
                spellId = SPELL_GEN_ADAPTIVE_WARDING_FROST;
                break;
            case SPELL_SCHOOL_SHADOW:
                spellId = SPELL_GEN_ADAPTIVE_WARDING_SHADOW;
                break;
            case SPELL_SCHOOL_ARCANE:
                spellId = SPELL_GEN_ADAPTIVE_WARDING_ARCANE;
                break;
            default:
                return;
        }
        GetTarget()->CastSpell(GetTarget(), spellId, true, NULL, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_gen_adaptive_warding::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_gen_adaptive_warding::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

enum AlchemistStone
{
    ALECHEMIST_STONE_HEAL                   = 21399,
    ALECHEMIST_STONE_MANA                   = 21400
};

// 17619 - Alchemist Stone
class spell_gen_alchemist_stone : public AuraScript
{
    PrepareAuraScript(spell_gen_alchemist_stone);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(ALECHEMIST_STONE_HEAL) ||
            !sSpellMgr->GetSpellInfo(ALECHEMIST_STONE_MANA))
            return false;
        return true;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo()->GetSpellInfo()->SpellFamilyName == SPELLFAMILY_POTION;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId = 0;
        int32 bp = int32(eventInfo.GetDamageInfo()->GetDamage() * 0.4f);

        if (eventInfo.GetDamageInfo()->GetSpellInfo()->HasEffect(SPELL_EFFECT_HEAL))
            spellId = ALECHEMIST_STONE_HEAL;
        else if (eventInfo.GetDamageInfo()->GetSpellInfo()->HasEffect(SPELL_EFFECT_ENERGIZE))
            spellId = ALECHEMIST_STONE_MANA;

        if (!spellId)
            return;

        GetTarget()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, bp, GetTarget(), true, NULL, aurEff);
    }


    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_gen_alchemist_stone::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_gen_alchemist_stone::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

class spell_gen_allow_cast_from_item_only : public SpellScript
{
    PrepareSpellScript(spell_gen_allow_cast_from_item_only);

    SpellCastResult CheckRequirement()
    {
        if (!GetCastItem())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_allow_cast_from_item_only::CheckRequirement);
    }
};

enum AnimalBloodPoolSpell
{
    SPELL_ANIMAL_BLOOD                      = 46221,
    SPELL_SPAWN_BLOOD_POOL                  = 63471
};

// 46221 - Animal Blood
class spell_gen_animal_blood : public AuraScript
{
    PrepareAuraScript(spell_gen_animal_blood);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_SPAWN_BLOOD_POOL))
            return false;
        return true;
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Remove all auras with spell id 46221, except the one currently being applied
        while (Aura* aur = GetUnitOwner()->GetOwnedAura(SPELL_ANIMAL_BLOOD, ObjectGuid::Empty, ObjectGuid::Empty, 0, GetAura()))
            GetUnitOwner()->RemoveOwnedAura(aur);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetUnitOwner())
            if (owner->IsInWater())
                owner->CastSpell(owner, SPELL_SPAWN_BLOOD_POOL, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectRemoveFn(spell_gen_animal_blood::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gen_animal_blood::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63471 -Spawn Blood Pool
class spell_spawn_blood_pool : public SpellScript
{
    PrepareSpellScript(spell_spawn_blood_pool);

    void SetDest(SpellDestination& dest)
    {
        Unit* caster = GetCaster();
        LiquidData liquidStatus;
        /*ZLiquidStatus status*/ caster->GetMap()->GetLiquidStatus(caster->GetPhaseMask(), caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), MAP_ALL_LIQUIDS, &liquidStatus);

        Position summonPos = caster->GetPosition();
        summonPos.m_positionZ = liquidStatus.level;
        dest.Relocate(summonPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_spawn_blood_pool::SetDest, EFFECT_0, TARGET_DEST_CASTER);
    }
};

// 41337 Aura of Anger
class spell_gen_aura_of_anger : public AuraScript
{
    PrepareAuraScript(spell_gen_aura_of_anger);

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
            aurEff1->ChangeAmount(aurEff1->GetAmount() + 5);

        aurEff->SetAmount(100 * aurEff->GetTickNumber());
    }

    void Register() override
    {
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_aura_of_anger::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

enum ServiceUniform
{
    // Spells
    SPELL_SERVICE_UNIFORM                   = 71450,
    // Models
    MODEL_GOBLIN_MALE                       = 31002,
    MODEL_GOBLIN_FEMALE                     = 31003
};

class spell_gen_aura_service_uniform : public AuraScript
{
    PrepareAuraScript(spell_gen_aura_service_uniform);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_SERVICE_UNIFORM))
            return false;
        return true;
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Apply model goblin
        Unit* target = GetTarget();

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if (target->GetGender() == GENDER_MALE)
                target->SetDisplayId(MODEL_GOBLIN_MALE);
            else
                target->SetDisplayId(MODEL_GOBLIN_FEMALE);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (target->GetTypeId() == TYPEID_PLAYER)
            target->RestoreDisplayId();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectRemoveFn(spell_gen_aura_service_uniform::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gen_aura_service_uniform::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_gen_av_drekthar_presence : public AuraScript
{
    PrepareAuraScript(spell_gen_av_drekthar_presence);

    bool CheckAreaTarget(Unit* target)
    {
        switch (target->GetEntry())
        {
            // alliance
            case 14762: // Dun Baldar North Marshal
            case 14763: // Dun Baldar South Marshal
            case 14764: // Icewing Marshal
            case 14765: // Stonehearth Marshal
            case 11948: // Vandar Stormspike
            // horde
            case 14772: // East Frostwolf Warmaster
            case 14776: // Tower Point Warmaster
            case 14773: // Iceblood Warmaster
            case 14777: // West Frostwolf Warmaster
            case 11946: // Drek'thar
                return true;
            default:
                return false;
        }
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_drekthar_presence::CheckAreaTarget);
    }
};

enum GenericBandage
{
    SPELL_RECENTLY_BANDAGED                 = 11196
};

class spell_gen_bandage : public SpellScript
{
    PrepareSpellScript(spell_gen_bandage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_RECENTLY_BANDAGED))
            return false;
        return true;
    }

    SpellCastResult CheckCast()
    {
        if (Unit* target = GetExplTargetUnit())
        {
            if (target->HasAura(SPELL_RECENTLY_BANDAGED))
                return SPELL_FAILED_TARGET_AURASTATE;
        }
        return SPELL_CAST_OK;
    }

    void HandleScript()
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_RECENTLY_BANDAGED, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_bandage::CheckCast);
        AfterHit += SpellHitFn(spell_gen_bandage::HandleScript);
    }
};

enum Bonked
{
    SPELL_BONKED            = 62991,
    SPELL_FOAM_SWORD_DEFEAT = 62994,
    SPELL_ON_GUARD          = 62972
};

class spell_gen_bonked : public SpellScript
{
    PrepareSpellScript(spell_gen_bonked);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* target = GetHitPlayer())
        {
            Aura const* aura = GetHitAura();
            if (!(aura && aura->GetStackAmount() == 3))
                return;

            target->CastSpell(target, SPELL_FOAM_SWORD_DEFEAT, true);
            target->RemoveAurasDueToSpell(SPELL_BONKED);

            if (Aura const* aura = target->GetAura(SPELL_ON_GUARD))
            {
                if (Item* item = target->GetItemByGuid(aura->GetCastItemGUID()))
                    target->DestroyItemCount(item->GetEntry(), 1, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_bonked::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/* DOCUMENTATION: Break-Shield spells
    Break-Shield spells can be classified in three groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: NONE
            + EFFECT_2: NONE
        - Spells casted by players triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: FORCE_CAST
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE

    In the following script we handle the SCRIPT_EFFECT for effIndex EFFECT_0 and EFFECT_1.
        - When handling EFFECT_0 we're in the "Spells on vehicle bar used by players" case
          and we'll trigger "Spells casted by players triggered by script"
        - When handling EFFECT_1 we're in the "Spells casted by players triggered by script"
          or "Spells casted by NPCs on players" so we'll search for the first defend layer and drop it.
*/

enum BreakShieldSpells
{
    SPELL_BREAK_SHIELD_DAMAGE_2K                      = 62626,
    SPELL_BREAK_SHIELD_DAMAGE_10K                     = 64590,

    SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS         = 62575, // Also on ToC5 mounts
    SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE      = 64595,
    SPELL_BREAK_SHIELD_TRIGGER_UNK                    = 66480
};

class spell_gen_break_shield: public SpellScriptLoader
{
    public:
        spell_gen_break_shield(const char* name) : SpellScriptLoader(name) { }

        class spell_gen_break_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_break_shield_SpellScript)

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_BREAK_SHIELD_TRIGGER_UNK:
                            case SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_10K;
                                break;
                            case SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_2K;
                                break;
                            default:
                                return;
                        }

                        if (Unit* rider = GetCaster()->GetCharmer())
                            rider->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (Unit* rider = target->GetCharmer())
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_break_shield_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_break_shield_SpellScript();
        }
};

// 46394 Brutallus Burn
class spell_gen_burn_brutallus : public AuraScript
{
    PrepareAuraScript(spell_gen_burn_brutallus);

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        if (aurEff->GetTickNumber() % 11 == 0)
            aurEff->SetAmount(aurEff->GetAmount() * 2);
    }

    void Register() override
    {
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_burn_brutallus::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

enum CannibalizeSpells
{
    SPELL_CANNIBALIZE_TRIGGERED             = 20578
};

class spell_gen_cannibalize : public SpellScript
{
    PrepareSpellScript(spell_gen_cannibalize);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_CANNIBALIZE_TRIGGERED))
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
        GetCaster()->CastSpell(GetCaster(), SPELL_CANNIBALIZE_TRIGGERED, false);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_cannibalize::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_gen_cannibalize::CheckIfCorpseNear);
    }
};

enum ChaosBlast
{
    SPELL_CHAOS_BLAST                       = 37675
};

class spell_gen_chaos_blast : public SpellScript
{
    PrepareSpellScript(spell_gen_chaos_blast)

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_CHAOS_BLAST))
            return false;
        return true;
    }
    void HandleDummy(SpellEffIndex /* effIndex */)
    {
        int32 basepoints0 = 100;
        Unit* caster = GetCaster();

        if (Unit* target = GetHitUnit())
            caster->CastCustomSpell(target, SPELL_CHAOS_BLAST, &basepoints0, NULL, NULL, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_chaos_blast::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum Clone
{
    SPELL_NIGHTMARE_FIGMENT_MIRROR_IMAGE    = 57528
};

class spell_gen_clone : public SpellScript
{
    PrepareSpellScript(spell_gen_clone);

    void HandleScriptEffect(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        GetCaster()->CastSpell(GetHitUnit(), GetEffectValue(), true);
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_NIGHTMARE_FIGMENT_MIRROR_IMAGE)
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_clone::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
            OnEffectHitTarget += SpellEffectFn(spell_gen_clone::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_DUMMY);
        }
        else
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_clone::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            OnEffectHitTarget += SpellEffectFn(spell_gen_clone::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    }
};

enum CloneWeaponSpells
{
    SPELL_COPY_WEAPON_AURA                  = 41054,
    SPELL_COPY_WEAPON_2_AURA                = 63418,
    SPELL_COPY_WEAPON_3_AURA                = 69893,

    SPELL_COPY_OFFHAND_AURA                 = 45205,
    SPELL_COPY_OFFHAND_2_AURA               = 69896,

    SPELL_COPY_RANGED_AURA                  = 57594
};

class spell_gen_clone_weapon : public SpellScript
{
    PrepareSpellScript(spell_gen_clone_weapon);

    void HandleScriptEffect(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        Unit* caster = GetCaster();

        if (Unit* target = GetHitUnit())
            caster->CastSpell(target, GetEffectValue(), true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_clone_weapon::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class spell_gen_clone_weapon_aura : public SpellScriptLoader
{
    public:
        spell_gen_clone_weapon_aura() : SpellScriptLoader("spell_gen_clone_weapon_aura") { }

        class spell_gen_clone_weapon_auraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_clone_weapon_auraScript);

            uint32 prevItem;

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_2_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_3_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_2_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_RANGED_AURA))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();
                if (!caster)
                    return;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID);

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, mainItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID));
                        break;
                    }
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID) + 1;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, offItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1));
                        break;
                    }
                    case SPELL_COPY_RANGED_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID) + 2;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, rangedItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2));
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                        target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, prevItem);
                        break;
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                        target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, prevItem);
                        break;
                    case SPELL_COPY_RANGED_AURA:
                        target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, prevItem);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_clone_weapon_auraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_clone_weapon_auraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_clone_weapon_auraScript();
        }
};

class spell_gen_count_pct_from_max_hp : public SpellScriptLoader
{
    public:
        spell_gen_count_pct_from_max_hp(char const* name, int32 damagePct = 0) : SpellScriptLoader(name), _damagePct(damagePct) { }

        class spell_gen_count_pct_from_max_hp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_count_pct_from_max_hp_SpellScript)

        public:
            spell_gen_count_pct_from_max_hp_SpellScript(int32 damagePct) : SpellScript(), _damagePct(damagePct) { }

            void RecalculateDamage()
            {
                if (!_damagePct)
                    _damagePct = GetHitDamage();

                SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(_damagePct));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_gen_count_pct_from_max_hp_SpellScript::RecalculateDamage);
            }

        private:
            int32 _damagePct;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_count_pct_from_max_hp_SpellScript(_damagePct);
        }

    private:
        int32 _damagePct;
};

// 63845 - Create Lance
enum CreateLanceSpells
{
    SPELL_CREATE_LANCE_ALLIANCE = 63914,
    SPELL_CREATE_LANCE_HORDE    = 63919
};

class spell_gen_create_lance : public SpellScriptLoader
{
    public:
        spell_gen_create_lance() : SpellScriptLoader("spell_gen_create_lance") { }

        class spell_gen_create_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_create_lance_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_LANCE_ALLIANCE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_CREATE_LANCE_HORDE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Player* target = GetHitPlayer())
                {
                    if (target->GetTeam() == ALLIANCE)
                        GetCaster()->CastSpell(target, SPELL_CREATE_LANCE_ALLIANCE, true);
                    else
                        GetCaster()->CastSpell(target, SPELL_CREATE_LANCE_HORDE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_create_lance_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_create_lance_SpellScript();
        }
};

class spell_gen_creature_permanent_feign_death : public SpellScriptLoader
{
    public:
        spell_gen_creature_permanent_feign_death() : SpellScriptLoader("spell_gen_creature_permanent_feign_death") { }

        class spell_gen_creature_permanent_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_creature_permanent_feign_death_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                if (target->GetTypeId() == TYPEID_UNIT)
                    target->ToCreature()->SetReactState(REACT_PASSIVE);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_creature_permanent_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_creature_permanent_feign_death_AuraScript();
        }
};

enum DalaranDisguiseSpells
{
    SPELL_SUNREAVER_DISGUISE_TRIGGER       = 69672,
    SPELL_SUNREAVER_DISGUISE_FEMALE        = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE          = 70974,

    SPELL_SILVER_COVENANT_DISGUISE_TRIGGER = 69673,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE  = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE    = 70972
};

class spell_gen_dalaran_disguise : public SpellScriptLoader
{
    public:
        spell_gen_dalaran_disguise(const char* name) : SpellScriptLoader(name) { }

        class spell_gen_dalaran_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dalaran_disguise_SpellScript);

            bool Validate(SpellInfo const* spellInfo) override
            {
                switch (spellInfo->Id)
                {
                    case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_FEMALE) ||
                            !sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_MALE))
                            return false;
                        break;
                    case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) ||
                            !sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                            return false;
                        break;
                }
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    uint8 gender = player->GetGender();

                    uint32 spellId = GetSpellInfo()->Id;

                    switch (spellId)
                    {
                        case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SUNREAVER_DISGUISE_FEMALE : SPELL_SUNREAVER_DISGUISE_MALE;
                            break;
                        case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SILVER_COVENANT_DISGUISE_FEMALE : SPELL_SILVER_COVENANT_DISGUISE_MALE;
                            break;
                        default:
                            break;
                    }

                    GetCaster()->CastSpell(player, spellId, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dalaran_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dalaran_disguise_SpellScript();
        }
};

class spell_gen_despawn_self : public SpellScriptLoader
{
    public:
        spell_gen_despawn_self() : SpellScriptLoader("spell_gen_despawn_self") { }

        class spell_gen_despawn_self_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_despawn_self_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_DUMMY || GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_SCRIPT_EFFECT)
                    GetCaster()->ToCreature()->DespawnOrUnsummon();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_despawn_self_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_despawn_self_SpellScript();
        }
};

enum DivineStormSpell
{
    SPELL_DIVINE_STORM      = 53385,
};

// 70769 Divine Storm!
class spell_gen_divine_storm_cd_reset : public SpellScriptLoader
{
    public:
        spell_gen_divine_storm_cd_reset() : SpellScriptLoader("spell_gen_divine_storm_cd_reset") { }

        class spell_gen_divine_storm_cd_reset_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_divine_storm_cd_reset_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DIVINE_STORM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->HasSpellCooldown(SPELL_DIVINE_STORM))
                    caster->RemoveSpellCooldown(SPELL_DIVINE_STORM, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_divine_storm_cd_reset_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_divine_storm_cd_reset_SpellScript();
        }
};

class spell_gen_ds_flush_knockback : public SpellScriptLoader
{
    public:
        spell_gen_ds_flush_knockback() : SpellScriptLoader("spell_gen_ds_flush_knockback") { }

        class spell_gen_ds_flush_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ds_flush_knockback_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                // Here the target is the water spout and determines the position where the player is knocked from
                if (Unit* target = GetHitUnit())
                {
                    if (Player* player = GetCaster()->ToPlayer())
                    {
                        float horizontalSpeed = 20.0f + (40.0f - GetCaster()->GetDistance(target));
                        float verticalSpeed = 8.0f;
                        // This method relies on the Dalaran Sewer map disposition and Water Spout position
                        // What we do is knock the player from a position exactly behind him and at the end of the pipe
                        player->KnockbackFrom(target->GetPositionX(), player->GetPositionY(), horizontalSpeed, verticalSpeed);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_ds_flush_knockback_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_ds_flush_knockback_SpellScript();
        }
};

enum DummyTrigger
{
    SPELL_PERSISTANT_SHIELD_TRIGGERED       = 26470,
    SPELL_PERSISTANT_SHIELD                 = 26467
};

class spell_gen_dummy_trigger : public SpellScriptLoader
{
    public:
        spell_gen_dummy_trigger() : SpellScriptLoader("spell_gen_dummy_trigger") { }

        class spell_gen_dummy_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dummy_trigger_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD_TRIGGERED) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    if (SpellInfo const* triggeredByAuraSpell = GetTriggeringSpell())
                        if (triggeredByAuraSpell->Id == SPELL_PERSISTANT_SHIELD_TRIGGERED)
                            caster->CastCustomSpell(target, SPELL_PERSISTANT_SHIELD_TRIGGERED, &damage, NULL, NULL, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dummy_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dummy_trigger_SpellScript();
        }
};

class spell_gen_dungeon_credit : public SpellScriptLoader
{
    public:
        spell_gen_dungeon_credit() : SpellScriptLoader("spell_gen_dungeon_credit") { }

        class spell_gen_dungeon_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dungeon_credit_SpellScript);

            bool Load() override
            {
                _handled = false;
                return true;
            }

            void CreditEncounter()
            {
                // This hook is executed for every target, make sure we only credit instance once
                if (_handled)
                    return;

                _handled = true;
                Unit* caster = GetCaster();
                if (InstanceScript* instance = caster->GetInstanceScript())
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, GetSpellInfo()->Id, caster);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_gen_dungeon_credit_SpellScript::CreditEncounter);
            }

        private:
            bool _handled;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_dungeon_credit_SpellScript();
        }
};

enum EluneCandle
{
    // Creatures
    NPC_OMEN                       = 15467,

    // Spells
    SPELL_ELUNE_CANDLE_OMEN_HEAD   = 26622,
    SPELL_ELUNE_CANDLE_OMEN_CHEST  = 26624,
    SPELL_ELUNE_CANDLE_OMEN_HAND_R = 26625,
    SPELL_ELUNE_CANDLE_OMEN_HAND_L = 26649,
    SPELL_ELUNE_CANDLE_NORMAL      = 26636
};

class spell_gen_elune_candle : public SpellScriptLoader
{
    public:
        spell_gen_elune_candle() : SpellScriptLoader("spell_gen_elune_candle") { }

        class spell_gen_elune_candle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_elune_candle_SpellScript);
            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HEAD) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_CHEST) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_R) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_L) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_NORMAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = 0;

                if (GetHitUnit()->GetEntry() == NPC_OMEN)
                {
                    switch (urand(0, 3))
                    {
                        case 0:
                            spellId = SPELL_ELUNE_CANDLE_OMEN_HEAD;
                            break;
                        case 1:
                            spellId = SPELL_ELUNE_CANDLE_OMEN_CHEST;
                            break;
                        case 2:
                            spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_R;
                            break;
                        case 3:
                            spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_L;
                            break;
                    }
                }
                else
                    spellId = SPELL_ELUNE_CANDLE_NORMAL;

                GetCaster()->CastSpell(GetHitUnit(), spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_elune_candle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_elune_candle_SpellScript();
        }
};

enum FishingSpells
{
    SPELL_FISHING_NO_FISHING_POLE   = 131476,
    SPELL_FISHING_WITH_POLE         = 131490
};

// 131474 - Fishing
class spell_gen_fishing : public SpellScriptLoader
{
    public:
        spell_gen_fishing() : SpellScriptLoader("spell_gen_fishing") { }

        class spell_gen_fishing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_fishing_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FISHING_NO_FISHING_POLE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_FISHING_WITH_POLE))
                    return false;
                return true;
            }

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 spellId;
                Item* mainHand = GetCaster()->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                if (!mainHand || mainHand->GetTemplate()->Class != ITEM_CLASS_WEAPON || mainHand->GetTemplate()->SubClass != ITEM_SUBCLASS_WEAPON_FISHING_POLE)
                    spellId = SPELL_FISHING_NO_FISHING_POLE;
                else
                    spellId = SPELL_FISHING_WITH_POLE;

                GetCaster()->CastSpell(GetCaster(), spellId, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_fishing_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_fishing_SpellScript();
        }
};

enum TransporterBackfires
{
    SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH     = 23444,
    SPELL_TRANSPORTER_EVIL_TWIN                 = 23445,
    SPELL_TRANSPORTER_MALFUNCTION_MISS          = 36902
};

class spell_gen_gadgetzan_transporter_backfire : public SpellScriptLoader
{
    public:
        spell_gen_gadgetzan_transporter_backfire() : SpellScriptLoader("spell_gen_gadgetzan_transporter_backfire") { }

        class spell_gen_gadgetzan_transporter_backfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gadgetzan_transporter_backfire_SpellScript)

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH) ||
                    !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_EVIL_TWIN) ||
                    !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_MISS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                int32 r = irand(0, 119);
                if (r < 20)                           // Transporter Malfunction - 1/6 polymorph
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH, true);
                else if (r < 100)                     // Evil Twin               - 4/6 evil twin
                    caster->CastSpell(caster, SPELL_TRANSPORTER_EVIL_TWIN, true);
                else                                    // Transporter Malfunction - 1/6 miss the target
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_MISS, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gadgetzan_transporter_backfire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gadgetzan_transporter_backfire_SpellScript();
        }
};

class spell_gen_gift_of_naaru : public AuraScript
{
    PrepareAuraScript(spell_gen_gift_of_naaru);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            amount = int32(CalculatePct(caster->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_0].BasePoints));
    }

    void CalculateTick(AuraEffect const*, int32& heal)
    {
        if (GetUnitOwner()->HasAura(134735) || GetUnitOwner()->GetBattlegorund())
            AddPct(heal, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_gift_of_naaru::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_gen_gift_of_naaru::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

enum GnomishTransporter
{
    SPELL_TRANSPORTER_SUCCESS                   = 23441,
    SPELL_TRANSPORTER_FAILURE                   = 23446
};

class spell_gen_gnomish_transporter : public SpellScriptLoader
{
    public:
        spell_gen_gnomish_transporter() : SpellScriptLoader("spell_gen_gnomish_transporter") { }

        class spell_gen_gnomish_transporter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gnomish_transporter_SpellScript)

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_SUCCESS) ||
                    !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_FAILURE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                GetCaster()->CastSpell(GetCaster(), roll_chance_i(50) ? SPELL_TRANSPORTER_SUCCESS : SPELL_TRANSPORTER_FAILURE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gnomish_transporter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gnomish_transporter_SpellScript();
        }
};

enum Interrupt
{
    SPELL_GEN_THROW_INTERRUPT           = 32747
};

// 32748 - Deadly Throw Interrupt
// 44835 - Maim Interrupt
class spell_gen_interrupt : public SpellScriptLoader
{
    public:
        spell_gen_interrupt() : SpellScriptLoader("spell_gen_interrupt") { }

        class spell_gen_interrupt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_interrupt_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GEN_THROW_INTERRUPT))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_GEN_THROW_INTERRUPT, true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_gen_interrupt_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_interrupt_AuraScript();
        }
};

enum GenericLifebloom
{
    SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL        = 43422,
    SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL              = 52552,
    SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL           = 53692,
    SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL           = 57763,
    SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL    = 66094
};

class spell_gen_lifebloom : public SpellScriptLoader
{
    public:
        spell_gen_lifebloom(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifebloom_AuraScript);

        public:
            spell_gen_lifebloom_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                // final heal
                GetTarget()->CastSpell(GetTarget(), _spellId, true, NULL, aurEff, GetCasterGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_lifebloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_lifebloom_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

/* DOCUMENTATION: Charge spells
    Charge spells can be classified in four groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: NONE
        - Spells casted by player's mounts triggered by script:
            + EFFECT_0: CHARGE
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: APPLY_AURA
        - Spells casted by players on the target triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: CHARGE
            + EFFECT_2: SCRIPT_EFFECT

    In the following script we handle the SCRIPT_EFFECT and CHARGE
        - When handling SCRIPT_EFFECT:
            + EFFECT_0: Corresponds to "Spells on vehicle bar used by players" and we make player's mount cast
              the charge effect on the current target ("Spells casted by player's mounts triggered by script").
            + EFFECT_1 and EFFECT_2: Triggered when "Spells casted by player's mounts triggered by script" hits target,
              corresponding to "Spells casted by players on the target triggered by script" and "Spells casted by
              NPCs on players" and we check Defend layers and drop a charge of the first found.
        - When handling CHARGE:
            + Only launched for "Spells casted by player's mounts triggered by script", makes the player cast the
              damaging spell on target with a small chance of failing it.
*/

enum ChargeSpells
{
    SPELL_CHARGE_DAMAGE_8K5             = 62874,
    SPELL_CHARGE_DAMAGE_20K             = 68498,
    SPELL_CHARGE_DAMAGE_45K             = 64591,

    SPELL_CHARGE_CHARGING_EFFECT_8K5    = 63661,
    SPELL_CHARGE_CHARGING_EFFECT_20K_1  = 68284,
    SPELL_CHARGE_CHARGING_EFFECT_20K_2  = 68501,
    SPELL_CHARGE_CHARGING_EFFECT_45K_1  = 62563,
    SPELL_CHARGE_CHARGING_EFFECT_45K_2  = 66481,

    SPELL_CHARGE_TRIGGER_FACTION_MOUNTS = 62960,
    SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION = 68282,

    SPELL_CHARGE_MISS_EFFECT            = 62977,
};

class spell_gen_mounted_charge: public SpellScriptLoader
{
    public:
        spell_gen_mounted_charge() : SpellScriptLoader("spell_gen_mounted_charge") { }

        class spell_gen_mounted_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mounted_charge_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_20K_1;
                                break;
                            case SPELL_CHARGE_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_8K5;
                                break;
                            default:
                                return;
                        }

                        // If target isn't a training dummy there's a chance of failing the charge
                        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) && roll_chance_f(12.5f))
                            spellId = SPELL_CHARGE_MISS_EFFECT;

                        if (Unit* vehicle = GetCaster()->GetVehicleBase())
                            vehicle->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    case EFFECT_2:
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (Unit* rider = target->GetCharmer())
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CHARGE_CHARGING_EFFECT_8K5:
                        spellId = SPELL_CHARGE_DAMAGE_8K5;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_2:
                        spellId = SPELL_CHARGE_DAMAGE_20K;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_2:
                        spellId = SPELL_CHARGE_DAMAGE_45K;
                        break;
                    default:
                        return;
                }

                if (Unit* rider = GetCaster()->GetCharmer())
                    rider->CastSpell(GetHitUnit(), spellId, false);
                else
                    GetCaster()->CastSpell(GetHitUnit(), spellId, false);
            }

            void Register() override
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                if (spell->HasEffect(SPELL_EFFECT_SCRIPT_EFFECT))
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);

                if (spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_CHARGE)
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_mounted_charge_SpellScript();
        }
};

enum Netherbloom
{
    SPELL_NETHERBLOOM_POLLEN_1      = 28703
};

// 28702 - Netherbloom
class spell_gen_netherbloom : public SpellScriptLoader
{
    public:
        spell_gen_netherbloom() : SpellScriptLoader("spell_gen_netherbloom") { }

        class spell_gen_netherbloom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_netherbloom_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                for (uint8 i = 0; i < 5; ++i)
                    if (!sSpellMgr->GetSpellInfo(SPELL_NETHERBLOOM_POLLEN_1 + i))
                        return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* target = GetHitUnit())
                {
                    // 25% chance of casting a random buff
                    if (roll_chance_i(75))
                        return;

                    // triggered spells are 28703 to 28707
                    // Note: some sources say, that there was the possibility of
                    //       receiving a debuff. However, this seems to be removed by a patch.

                    // don't overwrite an existing aura
                    for (uint8 i = 0; i < 5; ++i)
                        if (target->HasAura(SPELL_NETHERBLOOM_POLLEN_1 + i))
                            return;

                    target->CastSpell(target, SPELL_NETHERBLOOM_POLLEN_1 + urand(0, 4), true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_netherbloom_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_netherbloom_SpellScript();
        }
};

enum NightmareVine
{
    SPELL_NIGHTMARE_POLLEN      = 28721
};

// 28720 - Nightmare Vine
class spell_gen_nightmare_vine : public SpellScriptLoader
{
    public:
        spell_gen_nightmare_vine() : SpellScriptLoader("spell_gen_nightmare_vine") { }

        class spell_gen_nightmare_vine_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_nightmare_vine_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NIGHTMARE_POLLEN))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* target = GetHitUnit())
                {
                    // 25% chance of casting Nightmare Pollen
                    if (roll_chance_i(25))
                        target->CastSpell(target, SPELL_NIGHTMARE_POLLEN, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_nightmare_vine_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_nightmare_vine_SpellScript();
        }
};

enum ObsidianArmor
{
    SPELL_GEN_OBSIDIAN_ARMOR_HOLY       = 27536,
    SPELL_GEN_OBSIDIAN_ARMOR_FIRE       = 27533,
    SPELL_GEN_OBSIDIAN_ARMOR_NATURE     = 27538,
    SPELL_GEN_OBSIDIAN_ARMOR_FROST      = 27534,
    SPELL_GEN_OBSIDIAN_ARMOR_SHADOW     = 27535,
    SPELL_GEN_OBSIDIAN_ARMOR_ARCANE     = 27540
};

// 27539 - Obsidian Armor
class spell_gen_obsidian_armor : public SpellScriptLoader
{
    public:
        spell_gen_obsidian_armor() : SpellScriptLoader("spell_gen_obsidian_armor") { }

        class spell_gen_obsidian_armor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_obsidian_armor_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_HOLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_FIRE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_NATURE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_FROST) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_SHADOW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_ARCANE))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                if (eventInfo.GetDamageInfo()->GetSpellInfo()) // eventInfo.GetSpellInfo()
                    return false;

                if (GetFirstSchoolInMask(eventInfo.GetSchoolMask()) == SPELL_SCHOOL_NORMAL)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                uint32 spellId = 0;
                switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
                {
                    case SPELL_SCHOOL_HOLY:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_HOLY;
                        break;
                    case SPELL_SCHOOL_FIRE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_FIRE;
                        break;
                    case SPELL_SCHOOL_NATURE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_NATURE;
                        break;
                    case SPELL_SCHOOL_FROST:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_FROST;
                        break;
                    case SPELL_SCHOOL_SHADOW:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_SHADOW;
                        break;
                    case SPELL_SCHOOL_ARCANE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_ARCANE;
                        break;
                    default:
                        return;
                }
                GetTarget()->CastSpell(GetTarget(), spellId, true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_gen_obsidian_armor_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_gen_obsidian_armor_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_obsidian_armor_AuraScript();
        }
};

enum TournamentPennantSpells
{
    SPELL_PENNANT_STORMWIND_ASPIRANT        = 62595,
    SPELL_PENNANT_STORMWIND_VALIANT         = 62596,
    SPELL_PENNANT_STORMWIND_CHAMPION        = 62594,
    SPELL_PENNANT_GNOMEREGAN_ASPIRANT       = 63394,
    SPELL_PENNANT_GNOMEREGAN_VALIANT        = 63395,
    SPELL_PENNANT_GNOMEREGAN_CHAMPION       = 63396,
    SPELL_PENNANT_SEN_JIN_ASPIRANT          = 63397,
    SPELL_PENNANT_SEN_JIN_VALIANT           = 63398,
    SPELL_PENNANT_SEN_JIN_CHAMPION          = 63399,
    SPELL_PENNANT_SILVERMOON_ASPIRANT       = 63401,
    SPELL_PENNANT_SILVERMOON_VALIANT        = 63402,
    SPELL_PENNANT_SILVERMOON_CHAMPION       = 63403,
    SPELL_PENNANT_DARNASSUS_ASPIRANT        = 63404,
    SPELL_PENNANT_DARNASSUS_VALIANT         = 63405,
    SPELL_PENNANT_DARNASSUS_CHAMPION        = 63406,
    SPELL_PENNANT_EXODAR_ASPIRANT           = 63421,
    SPELL_PENNANT_EXODAR_VALIANT            = 63422,
    SPELL_PENNANT_EXODAR_CHAMPION           = 63423,
    SPELL_PENNANT_IRONFORGE_ASPIRANT        = 63425,
    SPELL_PENNANT_IRONFORGE_VALIANT         = 63426,
    SPELL_PENNANT_IRONFORGE_CHAMPION        = 63427,
    SPELL_PENNANT_UNDERCITY_ASPIRANT        = 63428,
    SPELL_PENNANT_UNDERCITY_VALIANT         = 63429,
    SPELL_PENNANT_UNDERCITY_CHAMPION        = 63430,
    SPELL_PENNANT_ORGRIMMAR_ASPIRANT        = 63431,
    SPELL_PENNANT_ORGRIMMAR_VALIANT         = 63432,
    SPELL_PENNANT_ORGRIMMAR_CHAMPION        = 63433,
    SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT    = 63434,
    SPELL_PENNANT_THUNDER_BLUFF_VALIANT     = 63435,
    SPELL_PENNANT_THUNDER_BLUFF_CHAMPION    = 63436,
    SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT   = 63606,
    SPELL_PENNANT_ARGENT_CRUSADE_VALIANT    = 63500,
    SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION   = 63501,
    SPELL_PENNANT_EBON_BLADE_ASPIRANT       = 63607,
    SPELL_PENNANT_EBON_BLADE_VALIANT        = 63608,
    SPELL_PENNANT_EBON_BLADE_CHAMPION       = 63609
};

enum TournamentMounts
{
    NPC_STORMWIND_STEED                     = 33217,
    NPC_IRONFORGE_RAM                       = 33316,
    NPC_GNOMEREGAN_MECHANOSTRIDER           = 33317,
    NPC_EXODAR_ELEKK                        = 33318,
    NPC_DARNASSIAN_NIGHTSABER               = 33319,
    NPC_ORGRIMMAR_WOLF                      = 33320,
    NPC_DARK_SPEAR_RAPTOR                   = 33321,
    NPC_THUNDER_BLUFF_KODO                  = 33322,
    NPC_SILVERMOON_HAWKSTRIDER              = 33323,
    NPC_FORSAKEN_WARHORSE                   = 33324,
    NPC_ARGENT_WARHORSE                     = 33782,
    NPC_ARGENT_STEED_ASPIRANT               = 33845,
    NPC_ARGENT_HAWKSTRIDER_ASPIRANT         = 33844
};

enum TournamentQuestsAchievements
{
    ACHIEVEMENT_CHAMPION_STORMWIND          = 2781,
    ACHIEVEMENT_CHAMPION_DARNASSUS          = 2777,
    ACHIEVEMENT_CHAMPION_IRONFORGE          = 2780,
    ACHIEVEMENT_CHAMPION_GNOMEREGAN         = 2779,
    ACHIEVEMENT_CHAMPION_THE_EXODAR         = 2778,
    ACHIEVEMENT_CHAMPION_ORGRIMMAR          = 2783,
    ACHIEVEMENT_CHAMPION_SEN_JIN            = 2784,
    ACHIEVEMENT_CHAMPION_THUNDER_BLUFF      = 2786,
    ACHIEVEMENT_CHAMPION_UNDERCITY          = 2787,
    ACHIEVEMENT_CHAMPION_SILVERMOON         = 2785,
    ACHIEVEMENT_ARGENT_VALOR                = 2758,
    ACHIEVEMENT_CHAMPION_ALLIANCE           = 2782,
    ACHIEVEMENT_CHAMPION_HORDE              = 2788,

    QUEST_VALIANT_OF_STORMWIND              = 13593,
    QUEST_A_VALIANT_OF_STORMWIND            = 13684,
    QUEST_VALIANT_OF_DARNASSUS              = 13706,
    QUEST_A_VALIANT_OF_DARNASSUS            = 13689,
    QUEST_VALIANT_OF_IRONFORGE              = 13703,
    QUEST_A_VALIANT_OF_IRONFORGE            = 13685,
    QUEST_VALIANT_OF_GNOMEREGAN             = 13704,
    QUEST_A_VALIANT_OF_GNOMEREGAN           = 13688,
    QUEST_VALIANT_OF_THE_EXODAR             = 13705,
    QUEST_A_VALIANT_OF_THE_EXODAR           = 13690,
    QUEST_VALIANT_OF_ORGRIMMAR              = 13707,
    QUEST_A_VALIANT_OF_ORGRIMMAR            = 13691,
    QUEST_VALIANT_OF_SEN_JIN                = 13708,
    QUEST_A_VALIANT_OF_SEN_JIN              = 13693,
    QUEST_VALIANT_OF_THUNDER_BLUFF          = 13709,
    QUEST_A_VALIANT_OF_THUNDER_BLUFF        = 13694,
    QUEST_VALIANT_OF_UNDERCITY              = 13710,
    QUEST_A_VALIANT_OF_UNDERCITY            = 13695,
    QUEST_VALIANT_OF_SILVERMOON             = 13711,
    QUEST_A_VALIANT_OF_SILVERMOON           = 13696
};

class spell_gen_on_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_on_tournament_mount() : SpellScriptLoader("spell_gen_on_tournament_mount") { }

        class spell_gen_on_tournament_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_on_tournament_mount_AuraScript);

            uint32 _pennantSpellId;

            bool Load() override
            {
                _pennantSpellId = 0;
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* vehicle = caster->GetVehicleBase())
                    {
                        _pennantSpellId = GetPennatSpellId(caster->ToPlayer(), vehicle);
                        caster->CastSpell(caster, _pennantSpellId, true);
                    }
                }
            }

            void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->RemoveAurasDueToSpell(_pennantSpellId);
            }

            uint32 GetPennatSpellId(Player* player, Unit* mount)
            {
                switch (mount->GetEntry())
                {
                    case NPC_ARGENT_STEED_ASPIRANT:
                    case NPC_STORMWIND_STEED:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_STORMWIND) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_VALIANT;
                        else
                            return SPELL_PENNANT_STORMWIND_ASPIRANT;
                    }
                    case NPC_GNOMEREGAN_MECHANOSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_GNOMEREGAN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_VALIANT;
                        else
                            return SPELL_PENNANT_GNOMEREGAN_ASPIRANT;
                    }
                    case NPC_DARK_SPEAR_RAPTOR:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SEN_JIN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_VALIANT;
                        else
                            return SPELL_PENNANT_SEN_JIN_ASPIRANT;
                    }
                    case NPC_ARGENT_HAWKSTRIDER_ASPIRANT:
                    case NPC_SILVERMOON_HAWKSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SILVERMOON) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_VALIANT;
                        else
                            return SPELL_PENNANT_SILVERMOON_ASPIRANT;
                    }
                    case NPC_DARNASSIAN_NIGHTSABER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_DARNASSUS) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_VALIANT;
                        else
                            return SPELL_PENNANT_DARNASSUS_ASPIRANT;
                    }
                    case NPC_EXODAR_ELEKK:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THE_EXODAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_VALIANT;
                        else
                            return SPELL_PENNANT_EXODAR_ASPIRANT;
                    }
                    case NPC_IRONFORGE_RAM:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_IRONFORGE) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_VALIANT;
                        else
                            return SPELL_PENNANT_IRONFORGE_ASPIRANT;
                    }
                    case NPC_FORSAKEN_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_UNDERCITY) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_VALIANT;
                        else
                            return SPELL_PENNANT_UNDERCITY_ASPIRANT;
                    }
                    case NPC_ORGRIMMAR_WOLF:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_ORGRIMMAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_VALIANT;
                        else
                            return SPELL_PENNANT_ORGRIMMAR_ASPIRANT;
                    }
                    case NPC_THUNDER_BLUFF_KODO:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THUNDER_BLUFF) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_VALIANT;
                        else
                            return SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT;
                    }
                    case NPC_ARGENT_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ALLIANCE) || player->HasAchieved(ACHIEVEMENT_CHAMPION_HORDE))
                            return player->GetClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_CHAMPION : SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION;
                        else if (player->HasAchieved(ACHIEVEMENT_ARGENT_VALOR))
                            return player->GetClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_VALIANT : SPELL_PENNANT_ARGENT_CRUSADE_VALIANT;
                        else
                            return player->GetClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_ASPIRANT : SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT;
                    }
                    default:
                        return 0;
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_on_tournament_mount_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_on_tournament_mount_AuraScript::HandleRemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_on_tournament_mount_AuraScript();
        }
};

class spell_gen_oracle_wolvar_reputation : public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 factionId = GetSpellInfo()->Effects[effIndex].CalcValue();
                int32  repChange =  GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

                if (!factionEntry)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(factionEntry) < repChange)
                    player->GetReputationMgr().SetReputation(factionEntry, repChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};

enum OrcDisguiseSpells
{
    SPELL_ORC_DISGUISE_TRIGGER       = 45759,
    SPELL_ORC_DISGUISE_MALE          = 45760,
    SPELL_ORC_DISGUISE_FEMALE        = 45762
};

class spell_gen_orc_disguise : public SpellScriptLoader
{
    public:
        spell_gen_orc_disguise() : SpellScriptLoader("spell_gen_orc_disguise") { }

        class spell_gen_orc_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_orc_disguise_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_TRIGGER) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_MALE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_FEMALE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->GetGender();
                    if (!gender)
                        caster->CastSpell(target, SPELL_ORC_DISGUISE_MALE, true);
                    else
                        caster->CastSpell(target, SPELL_ORC_DISGUISE_FEMALE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_orc_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_orc_disguise_SpellScript();
        }
};

enum ParachuteSpells
{
    SPELL_PARACHUTE         = 45472,
    SPELL_PARACHUTE_BUFF    = 44795,
};

// 45472 Parachute
class spell_gen_parachute : public SpellScriptLoader
{
    public:
        spell_gen_parachute() : SpellScriptLoader("spell_gen_parachute") { }

        class spell_gen_parachute_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PARACHUTE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PARACHUTE_BUFF))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    if (target->IsFalling())
                    {
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
                        target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
                    }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_parachute_AuraScript();
        }
};

class spell_gen_remove_flight_auras : public SpellScriptLoader
{
    public:
        spell_gen_remove_flight_auras() : SpellScriptLoader("spell_gen_remove_flight_auras") { }

        class spell_gen_remove_flight_auras_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_remove_flight_auras_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->RemoveAurasByType(SPELL_AURA_FLY);
                    target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_remove_flight_auras_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_remove_flight_auras_SpellScript();
        }
};

enum Replenishment
{
    SPELL_REPLENISHMENT             = 57669,
    SPELL_INFINITE_REPLENISHMENT    = 61782
};

class spell_gen_replenishment : public SpellScriptLoader
{
    public:
        spell_gen_replenishment() : SpellScriptLoader("spell_gen_replenishment") { }

        class spell_gen_replenishment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_replenishment_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_REPLENISHMENT) ||
                    !sSpellMgr->GetSpellInfo(SPELL_INFINITE_REPLENISHMENT))
                    return false;
                return true;
            }

            bool Load() override
            {
                return GetUnitOwner()->GetPower(POWER_MANA);
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_REPLENISHMENT:
                        amount = GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.002f;
                        break;
                    case SPELL_INFINITE_REPLENISHMENT:
                        amount = GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.0025f;
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_replenishment_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_replenishment_AuraScript();
        }
};


enum RunningWildMountIds
{
    RUNNING_WILD_MODEL_MALE     = 29422,
    RUNNING_WILD_MODEL_FEMALE   = 29423,
    SPELL_ALTERED_FORM          = 97709
};

class spell_gen_running_wild : public SpellScriptLoader
{
    public:
        spell_gen_running_wild() : SpellScriptLoader("spell_gen_running_wild") { }

        class spell_gen_running_wild_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_running_wild_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sCreatureDisplayInfoStore.LookupEntry(RUNNING_WILD_MODEL_MALE))
                    return false;
                if (!sCreatureDisplayInfoStore.LookupEntry(RUNNING_WILD_MODEL_FEMALE))
                    return false;
                return true;
            }

            void HandleMount(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                PreventDefaultAction();

                target->Mount(target->GetGender() == GENDER_FEMALE ? RUNNING_WILD_MODEL_FEMALE : RUNNING_WILD_MODEL_MALE, 0, 0);

                // cast speed aura
                if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(aurEff->GetAmount()))
                    target->CastSpell(target, mountCapability->SpeedModSpell, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_running_wild_AuraScript::HandleMount, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_gen_running_wild_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_running_wild_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ALTERED_FORM))
                    return false;
                return true;
            }

            bool Load() override
            {
                // Definitely not a good thing, but currently the only way to do something at cast start
                // Should be replaced as soon as possible with a new hook: BeforeCastStart
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
                return false;
            }

            void Register() override
            {
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_running_wild_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_running_wild_SpellScript();
        }
};

class spell_gen_two_forms : public SpellScriptLoader
{
    public:
        spell_gen_two_forms() : SpellScriptLoader("spell_gen_two_forms") { }

        class spell_gen_two_forms_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_two_forms_SpellScript);

            SpellCastResult CheckCast()
            {
                if (GetCaster()->IsInCombat())
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                // Player cannot transform to human form if he is forced to be worgen for some reason (Darkflight)
                if (GetCaster()->GetAuraEffectsByType(SPELL_AURA_WORGEN_ALTERED_FORM).size() > 1)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleTransform(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();
                PreventHitDefaultEffect(effIndex);
                if (target->HasAuraType(SPELL_AURA_WORGEN_ALTERED_FORM))
                    target->RemoveAurasByType(SPELL_AURA_WORGEN_ALTERED_FORM);
                else    // Basepoints 1 for this aura control whether to trigger transform transition animation or not.
                    target->CastCustomSpell(SPELL_ALTERED_FORM, SPELLVALUE_BASE_POINT0, 1, target, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_two_forms_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_two_forms_SpellScript::HandleTransform, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_two_forms_SpellScript();
        }
};

class spell_gen_darkflight : public SpellScriptLoader
{
    public:
        spell_gen_darkflight() : SpellScriptLoader("spell_gen_darkflight") { }

        class spell_gen_darkflight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_darkflight_SpellScript);

            void TriggerTransform()
            {
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_darkflight_SpellScript::TriggerTransform);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_darkflight_SpellScript();
        }
};
enum SeaforiumSpells
{
    SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT  = 60937
};

class spell_gen_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_seaforium_blast() : SpellScriptLoader("spell_gen_seaforium_blast") { }

        class spell_gen_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT))
                    return false;
                return true;
            }

            bool Load() override
            {
                // OriginalCaster is always available in Spell::prepare
                return GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                // but in effect handling OriginalCaster can become NULL
                if (Unit* originalCaster = GetOriginalCaster())
                    if (GameObject* go = GetHitGObj())
                        if (go->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                            originalCaster->CastSpell(originalCaster, SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_seaforium_blast_SpellScript();
        }
};

enum SpectatorCheerTrigger
{
    EMOTE_ONE_SHOT_CHEER        = 4,
    EMOTE_ONE_SHOT_EXCLAMATION  = 5,
    EMOTE_ONE_SHOT_APPLAUD      = 21
};

uint8 const EmoteArray[3] = { EMOTE_ONE_SHOT_CHEER, EMOTE_ONE_SHOT_EXCLAMATION, EMOTE_ONE_SHOT_APPLAUD };

class spell_gen_spectator_cheer_trigger : public SpellScriptLoader
{
    public:
        spell_gen_spectator_cheer_trigger() : SpellScriptLoader("spell_gen_spectator_cheer_trigger") { }

        class spell_gen_spectator_cheer_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spectator_cheer_trigger_SpellScript)

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->HandleEmoteCommand(EmoteArray[urand(0, 2)]);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spectator_cheer_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_spectator_cheer_trigger_SpellScript();
        }
};

class spell_gen_spirit_healer_res : public SpellScriptLoader
{
    public:
        spell_gen_spirit_healer_res(): SpellScriptLoader("spell_gen_spirit_healer_res") { }

        class spell_gen_spirit_healer_res_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spirit_healer_res_SpellScript);

            bool Load() override
            {
                return GetOriginalCaster() && GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* originalCaster = GetOriginalCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    ObjectGuid guid = target->GetGUID();
                    WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 1 + 8);

                    data.WriteBit(guid[6]);
                    data.WriteBit(guid[5]);
                    data.WriteBit(guid[7]);
                    data.WriteBit(guid[1]);
                    data.WriteBit(guid[4]);
                    data.WriteBit(guid[2]);
                    data.WriteBit(guid[3]);
                    data.WriteBit(guid[0]);

                    data.WriteByteSeq(guid[0]);
                    data.WriteByteSeq(guid[4]);
                    data.WriteByteSeq(guid[2]);
                    data.WriteByteSeq(guid[3]);
                    data.WriteByteSeq(guid[7]);
                    data.WriteByteSeq(guid[6]);
                    data.WriteByteSeq(guid[5]);
                    data.WriteByteSeq(guid[1]);

                    originalCaster->GetSession()->SendPacket(&data);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spirit_healer_res_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_spirit_healer_res_SpellScript();
        }
};

enum SummonElemental
{
    SPELL_SUMMON_FIRE_ELEMENTAL  = 8985,
    SPELL_SUMMON_EARTH_ELEMENTAL = 19704
};

class spell_gen_summon_elemental : public SpellScriptLoader
{
    public:
        spell_gen_summon_elemental(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_summon_elemental_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_summon_elemental_AuraScript);

        public:
            spell_gen_summon_elemental_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (Unit* owner = GetCaster()->GetOwner())
                        owner->CastSpell(owner, _spellId, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (Unit* owner = GetCaster()->GetOwner())
                        if (owner->GetTypeId() == TYPEID_PLAYER) /// @todo this check is maybe wrong
                            owner->ToPlayer()->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
            }

            void Register() override
            {
                 AfterEffectApply += AuraEffectApplyFn(spell_gen_summon_elemental_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                 AfterEffectRemove += AuraEffectRemoveFn(spell_gen_summon_elemental_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_summon_elemental_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum TournamentMountsSpells
{
    SPELL_LANCE_EQUIPPED     = 62853
};

class spell_gen_summon_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_summon_tournament_mount() : SpellScriptLoader("spell_gen_summon_tournament_mount") { }

        class spell_gen_summon_tournament_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_summon_tournament_mount_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LANCE_EQUIPPED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfLanceEquiped()
            {
                if (GetCaster()->IsInDisallowedMountForm())
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (!GetCaster()->HasAura(SPELL_LANCE_EQUIPPED))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_summon_tournament_mount_SpellScript::CheckIfLanceEquiped);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_summon_tournament_mount_SpellScript();
        }
};


enum MountedDuelSpells
{
    SPELL_ON_TOURNAMENT_MOUNT = 63034,
    SPELL_MOUNTED_DUEL        = 62875
};

class spell_gen_tournament_duel : public SpellScriptLoader
{
    public:
        spell_gen_tournament_duel() : SpellScriptLoader("spell_gen_tournament_duel") { }

        class spell_gen_tournament_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tournament_duel_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ON_TOURNAMENT_MOUNT) ||
                    !sSpellMgr->GetSpellInfo(SPELL_MOUNTED_DUEL))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* rider = GetCaster()->GetCharmer())
                {
                    if (Player* playerTarget = GetHitPlayer())
                    {
                        if (playerTarget->HasAura(SPELL_ON_TOURNAMENT_MOUNT) && playerTarget->GetVehicleBase())
                            rider->CastSpell(playerTarget, SPELL_MOUNTED_DUEL, true);
                    }
                    else if (Unit* unitTarget = GetHitUnit())
                    {
                        if (unitTarget->GetCharmer() && unitTarget->GetCharmer()->GetTypeId() == TYPEID_PLAYER && unitTarget->GetCharmer()->HasAura(SPELL_ON_TOURNAMENT_MOUNT))
                            rider->CastSpell(unitTarget->GetCharmer(), SPELL_MOUNTED_DUEL, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tournament_duel_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_tournament_duel_SpellScript();
        }
};

class spell_gen_tournament_pennant : public SpellScriptLoader
{
    public:
        spell_gen_tournament_pennant() : SpellScriptLoader("spell_gen_tournament_pennant") { }

        class spell_gen_tournament_pennant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_tournament_pennant_AuraScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->GetVehicleBase())
                        caster->RemoveAurasDueToSpell(GetId());
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_tournament_pennant_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_tournament_pennant_AuraScript();
        }
};



enum PvPTrinketTriggeredSpells
{
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER         = 72752,
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF    = 72757
};

class spell_pvp_trinket_wotf_shared_cd : public SpellScript
{
    PrepareSpellScript(spell_pvp_trinket_wotf_shared_cd);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER) ||
            !sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF))
            return false;
        return true;
    }

    void HandleScript()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        auto cooldown = Milliseconds(sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER)->GetRecoveryTime());
        caster->AddSpellCooldown(GetSpellInfo()->Id, cooldown);
        caster->GetSpellHistory()->SendCooldown(GetSpellInfo()->Id, 0);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pvp_trinket_wotf_shared_cd::HandleScript);
    }
};

// 25281 - Turkey Marker
class spell_gen_turkey_marker : public AuraScript
{
    PrepareAuraScript(spell_gen_turkey_marker);

    std::list<TimeValue> applyTimes;

    enum 
    {
        SPELL_TURKEY_VENGEANCE = 25285
    };

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        // Store stack apply times, so we can pop them while they expire
        applyTimes.push_back(TimeValue::Now() + Milliseconds(GetMaxDuration()));

        // On stack 15 cast the achievement crediting spell
        if (GetStackAmount() >= 15)
            GetTarget()->CastSpell(GetTarget(), SPELL_TURKEY_VENGEANCE, true, nullptr, effect, GetCasterGUID());
    }

    void HandleTick(AuraEffect const*)
    {
        // Pop stack if it expired for us
        if (!applyTimes.empty())
            if (applyTimes.front() < TimeValue::Now())
                ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

enum FoamSword
{
    ITEM_FOAM_SWORD_GREEN   = 45061,
    ITEM_FOAM_SWORD_PINK    = 45176,
    ITEM_FOAM_SWORD_BLUE    = 45177,
    ITEM_FOAM_SWORD_RED     = 45178,
    ITEM_FOAM_SWORD_YELLOW  = 45179,
};

class spell_gen_upper_deck_create_foam_sword : public SpellScript
{
    PrepareSpellScript(spell_gen_upper_deck_create_foam_sword);

    void HandleScript(SpellEffIndex effIndex)
    {
        if (Player* player = GetHitPlayer())
        {
            static std::vector<uint32> const items = { ITEM_FOAM_SWORD_GREEN, ITEM_FOAM_SWORD_PINK, ITEM_FOAM_SWORD_BLUE, ITEM_FOAM_SWORD_RED, ITEM_FOAM_SWORD_YELLOW };
            // player can only have one of these items
            for (auto&& itemId : items)
                if (player->HasItemCount(itemId, 1, true))
                    return;

            CreateItem(effIndex, Trinity::Containers::SelectRandomContainerElement(items));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_upper_deck_create_foam_sword::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum VehicleScaling
{
    SPELL_GEAR_SCALING_ULDUAR       = 65266, // Salvaged Demolisher, Salvaged Chopper, Salvaged Siege Engine, Salvaged Siege Turret (Ulduar)
    SPELL_GEAR_SCALING_WINTERGRASP  = 65635, // Shredder, Tower Cannon, Wintergrasp Vehicle (Wintergrasp)
    SPELL_GEAR_SCALING_STRAND       = 65636, // Battleground Demolisher (Strand of the Ancients)
    SPELL_GEAR_SCALING_UNK          = 66666, // unk
    SPELL_GEAR_SCALING_OCULUS       = 66667, // Ruby Drake, Emerald Drake, Ember Drake (The Oculus)
    SPELL_GEAR_SCALING              = 66668,
};

class spell_gen_vehicle_scaling : public SpellScriptLoader
{
    public:
        spell_gen_vehicle_scaling() : SpellScriptLoader("spell_gen_vehicle_scaling") { }

        class spell_gen_vehicle_scaling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vehicle_scaling_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                if (!caster || !caster->ToPlayer())
                    return;

                float factor;
                uint16 baseItemLevel;

                // TODO: Reserach coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                        factor = 1.0f;
                        baseItemLevel = 205;
                        break;
                    case SPELL_GEAR_SCALING_STRAND:
                        factor = 1.0f;
                        baseItemLevel = 405;
                        break;
                    default:
                        factor = 1.0f;
                        baseItemLevel = 170;
                        break;
                }

                float avgILvl = caster->ToPlayer()->GetAverageItemLevel();
                if (avgILvl < baseItemLevel)
                    return;                     // TODO: Research possibility of scaling down

                if (aurEff->GetEffIndex() == EFFECT_1 && GetId() == SPELL_GEAR_SCALING_STRAND) // hack, idk how damage must be calculated
                    amount = 1000;
                else
                    amount = uint16((avgILvl - baseItemLevel) * factor);

                if (Unit* target = aurEff->GetBase()->GetUnitOwner())
                {
                    for (auto&& aura : target->GetAuraEffectsByType(aurEff->GetAuraType()))
                    {
                        switch (aura->GetId())
                        {
                            case SPELL_GEAR_SCALING_ULDUAR:
                            case SPELL_GEAR_SCALING_WINTERGRASP:
                            case SPELL_GEAR_SCALING_STRAND:
                            case SPELL_GEAR_SCALING_UNK:
                            case SPELL_GEAR_SCALING_OCULUS:
                            case SPELL_GEAR_SCALING:
                                if (aura->GetAmount() > amount)
                                    amount = 0;
                                else
                                    aura->ChangeAmount(0);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            void HandleAfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    for (auto&& aura : target->GetAuraEffectsByType(aurEff->GetAuraType()))
                    {
                        bool canBeRecalculated = aura->CanBeRecalculated();
                        aura->SetCanBeRecalculated(true);
                        aura->RecalculateAmount();
                        aura->SetCanBeRecalculated(canBeRecalculated);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_ALL, SPELL_AURA_ANY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_vehicle_scaling_AuraScript::HandleAfterRemove, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_vehicle_scaling_AuraScript();
        }
};

enum VendorBarkTrigger
{
    NPC_AMPHITHEATER_VENDOR     = 30098,
    SAY_AMPHITHEATER_VENDOR     = 0
};

class spell_gen_vendor_bark_trigger : public SpellScriptLoader
{
    public:
        spell_gen_vendor_bark_trigger() : SpellScriptLoader("spell_gen_vendor_bark_trigger") { }

        class spell_gen_vendor_bark_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_vendor_bark_trigger_SpellScript)

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Creature* vendor = GetCaster()->ToCreature())
                    if (vendor->GetEntry() == NPC_AMPHITHEATER_VENDOR)
                        vendor->AI()->Talk(SAY_AMPHITHEATER_VENDOR);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_vendor_bark_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_vendor_bark_trigger_SpellScript();
        }

};

class spell_gen_wg_water : public SpellScriptLoader
{
    public:
        spell_gen_wg_water() : SpellScriptLoader("spell_gen_wg_water") { }

        class spell_gen_wg_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wg_water_SpellScript);

            SpellCastResult CheckCast()
            {
                if (!GetSpellInfo()->CheckTargetCreatureType(GetCaster()))
                    return SPELL_FAILED_DONT_REPORT;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wg_water_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_wg_water_SpellScript();
        }
};

enum WhisperGulchYoggSaronWhisper
{
    SPELL_YOGG_SARON_WHISPER_DUMMY  = 29072
};

class spell_gen_whisper_gulch_yogg_saron_whisper : public SpellScriptLoader
{
    public:
        spell_gen_whisper_gulch_yogg_saron_whisper() : SpellScriptLoader("spell_gen_whisper_gulch_yogg_saron_whisper") { }

        class spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_YOGG_SARON_WHISPER_DUMMY))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell((Unit*)NULL, SPELL_YOGG_SARON_WHISPER_DUMMY, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript();
        }
};

enum SurveySpell
{
    GO_SURVEY_TOOL_RED    = 206590,
    GO_SURVEY_TOOL_YELLOW = 206589,
    GO_SURVEY_TOOL_GREEN  = 204272,

    SURVEY_FAR_DIST       = 80,
    SURVEY_MEDIUM_DIST    = 40,
    SURVEY_CLOSE_DIST     = 5
};

class spell_gen_survey : public SpellScriptLoader
{
    public:
        spell_gen_survey() : SpellScriptLoader("spell_gen_survey") { }

        class spell_gen_survey_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_survey_SpellScript);

            void HandleSurvey(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* player = GetCaster()->ToPlayer();
                if (!player)
                    return;

                if (GameObject* go = player->GetMap()->GetGameObject(player->m_ObjectSlot[1]))
                {
                    if (GetSpellInfo()->Id == go->GetSpellId())
                        go->SetSpellId(0);

                    player->RemoveGameObject(go, true);
                    player->m_ObjectSlot[1] = ObjectGuid::Empty;
                }

                ResearchDigsite* digsite = player->GetCurrentResearchDigsite();
                if (!digsite)
                    return;

                ArchaeologyFindInfo const* find = digsite->GetArchaeologyFind();
                if (!find)
                    return;

                GameObject* go = NULL;
                float dist = player->GetDistance(find->x, find->y, find->z);
                if (dist <= (float)SURVEY_CLOSE_DIST) // 5yd or less (archaeology find is dug)
                {
                    digsite->SelectNewArchaeologyFind(false);

                    // save only if digsite is not empty (if digsite is empty, it will be removed in UpdateResearchDigsites and replaced by new digsite)
                    if (!digsite->IsEmptyDigsite())
                        player->SaveResearchDigsiteToDB(digsite);

                    player->SendSurveryCastInfo(digsite, true); // needs to be send just before the digsite is removed
                    player->UpdateResearchDigsites();

                    go = player->SummonGameObject(find->goEntry, find->x, find->y, find->z, 0, { }, 30); // TODO: verify despawn time

                }
                else
                {
                    uint32 goEntry = 0;
                    if (dist > (float)SURVEY_CLOSE_DIST && dist <= (float)SURVEY_MEDIUM_DIST) // 6-40yd (green)
                        goEntry = GO_SURVEY_TOOL_GREEN;
                    else if (dist > (float)SURVEY_MEDIUM_DIST && dist <= (float)SURVEY_FAR_DIST) // 41-80yd (yellow)
                        goEntry = GO_SURVEY_TOOL_YELLOW;
                    else // more than 80yd (red)
                        goEntry = GO_SURVEY_TOOL_RED;

                    Position pos = player->GetNearPosition(3.0f, M_PI / 4);
                    float z = player->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, pos.m_positionZ + 2.0f);
                    float ang = pos.GetAngle(find->x, find->y);
                    go = player->SummonGameObject(goEntry, pos.m_positionX, pos.m_positionY, z, ang, { }, 30); // TODO: verify despawn time

                    player->SendSurveryCastInfo(digsite, false);
                }

                if (go)
                {
                    go->SetOwnerGUID(player->GetGUID());
                    go->SetSpellId(GetSpellInfo()->Id);
                    player->m_ObjectSlot[1] = go->GetGUID();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_survey_SpellScript::HandleSurvey, EFFECT_0, SPELL_EFFECT_SUMMON_OBJECT_SLOT2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_survey_SpellScript();
        }
};

class spell_gen_searching_for_artifacts : public SpellScriptLoader
{
    public:
        spell_gen_searching_for_artifacts() : SpellScriptLoader("spell_gen_searching_for_artifacts") { }

        class spell_gen_searching_for_artifacts_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_searching_for_artifacts_SpellScript);

            void HandleSkillUpdate(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                    if (GameObject* go = GetHitGObj())
                        if (!go->IsInSkillupList(player->GetGUID().GetCounter()) && player->UpdateCraftSkill(GetSpellInfo()->Id))
                            go->AddToSkillupList(player->GetGUID().GetCounter());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_searching_for_artifacts_SpellScript::HandleSkillUpdate, EFFECT_0, SPELL_EFFECT_OPEN_LOCK);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_searching_for_artifacts_SpellScript();
        }
};

class spell_gen_debug_move : public SpellScript
{
    PrepareSpellScript(spell_gen_debug_move);

    bool Load() override
    {
        if (Unit* caster = GetCaster())
            return caster->GetTypeId() == TYPEID_PLAYER;
        return false;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return SPELL_FAILED_BAD_TARGETS;

        Player* player = caster->ToPlayer();
        if (!player)
            return SPELL_FAILED_SPELL_UNAVAILABLE;

        Unit* target = player->GetSelectedUnit();
        if (!target)
            target = player;

        float x, y, z, o;
        if (!GetExplTargetDest())
            return SPELL_FAILED_NO_VALID_TARGETS;
        GetExplTargetDest()->GetPosition(x, y, z, o);

        DevToolSettings*& settings = sWorld->GetDevToolSettings(player->GetSession()->GetAccountId());
        if (!(settings ? settings : (settings = new DevToolSettings()))->Execute(sWorld->GetDevToolType(player->GetSession()->GetAccountId()), player, target, x, y, z, o))
            return SPELL_FAILED_ERROR;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_debug_move::CheckCast);
    }
};

// 136213 - Wrathion - OCL - Add Prismatic Socket Effect
class spell_gen_eye_of_the_black_prince : public SpellScript
{
    PrepareSpellScript(spell_gen_eye_of_the_black_prince);

    SpellCastResult CheckCast()
    {
        if (Item* item = GetExplTargetItem())
            if (item->IsEyeOfTheBlackPrinceAllowed())
                return SPELL_CAST_OK;

        return SPELL_FAILED_BAD_TARGETS;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_eye_of_the_black_prince::CheckCast);
    }
};

// Guild Perk Level 10
// -746 - First Aid (All ranks)
class spell_gen_the_doctor_is_in : public AuraScript
{
    PrepareAuraScript(spell_gen_the_doctor_is_in);

    enum { SPELL_THE_DOCTOR_IS_IN  = 118076 };

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (!player->InBattleground())
                    if (AuraEffect const* eff = caster->GetAuraEffect(SPELL_THE_DOCTOR_IS_IN, EFFECT_0))
                        AddPct(amount, eff->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_the_doctor_is_in::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// Guild Perk Level 11
// 83958 - Mobile Banking
enum MobileBankingData
{
    SPELL_SUMMON_CHEST_ALLIANCE             = 88304,
    SPELL_SUMMON_CHEST_HORDE                = 88306
};

class spell_gen_mobile_banking : public SpellScript
{
    PrepareSpellScript(spell_gen_mobile_banking);

    SpellCastResult ChechCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_DONT_REPORT;

        if (player->GetReputationRank(GUILD_REPUTATION_ID) < REP_FRIENDLY)
            return SPELL_FAILED_REPUTATION;

        if (player->InBattleground())
            return SPELL_FAILED_NOT_HERE;

        return SPELL_CAST_OK;
    }

    void Summon()
    {
        Player* caster = GetCaster()->ToPlayer();
        caster->CastSpell(caster, caster->GetTeamId() == TEAM_ALLIANCE ? SPELL_SUMMON_CHEST_ALLIANCE : SPELL_SUMMON_CHEST_HORDE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_mobile_banking::ChechCast);
        OnCast += SpellCastFn(spell_gen_mobile_banking::Summon);
    }
};

// Guild Perk Level 15
// 8326 - Ghost
class spell_gen_the_quick_and_the_dead : public AuraScript
{
    PrepareAuraScript(spell_gen_the_quick_and_the_dead);

    enum
    {
        SPELL_THE_QUICK_AND_THE_DEAD        = 83950,
        SPELL_THE_QUICK_AND_THE_DEAD_SPEED  = 84559,
    };

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->HasAura(SPELL_THE_QUICK_AND_THE_DEAD) && !GetUnitOwner()->GetBattlegorund())
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_THE_QUICK_AND_THE_DEAD_SPEED, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_THE_QUICK_AND_THE_DEAD_SPEED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gen_the_quick_and_the_dead::HandleApply, EFFECT_0, SPELL_AURA_GHOST, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_gen_the_quick_and_the_dead::HandleRemove, EFFECT_0, SPELL_AURA_GHOST, AURA_EFFECT_HANDLE_REAL);
    }
};

// Guild Perk Level 25
// 83968 - Mass Resurrection 
class spell_gen_mass_resurrection : public SpellScript
{
    PrepareSpellScript(spell_gen_mass_resurrection);

    SpellCastResult ChechCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_DONT_REPORT;

        if (Map* map = player->GetMap())
        {
            if (map->Instanceable())
            {
                if (map->IsChallengeDungeon())
                    return SPELL_FAILED_NOT_HERE;

                for (auto&& itr : map->GetPlayers())
                    if (itr.GetSource() && itr.GetSource()->IsAlive() && itr.GetSource()->IsInCombat())
                        return SPELL_FAILED_TARGET_IN_COMBAT;
            }
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gen_mass_resurrection::ChechCast);
    }
};

// 125117 - Sliced Peaches, 125122 - Rice Pudding
class spell_gen_item_crafted_kill_credit : public SpellScript
{
    PrepareSpellScript(spell_gen_item_crafted_kill_credit);

    void HandleCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            static std::map<uint32, uint32> const map =
            {
                { 125117, 64216 },
                { 125122, 64234 },
            };
            auto it = map.find(GetSpellInfo()->Id);
            if (it != map.end())
                player->KilledMonsterCredit(it->second);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_gen_item_crafted_kill_credit::HandleCast);
    }
};

// 60893 - Northrend Alchemy Research
// 61177 - Northrend Inscription Research
// 61288 - Minor Inscription Research
// 125557 - Imperial Silk
// 138646 - Lightning Steel Ingot
// 143011 - Celestial Cloth
// 140040 - Magnificence of Leather
// 140041 - Magnificence of Scales
// 142976 - Hardened Magnificent Hide
// 143255 - Balanced Trillium Ingot
// 138882, 138885, 138888, 138890, 138876, 138878, 138889, 138891, 138877, 138879, 138883, 138884 - Blacksmith "Reborn" reciepes
class spell_gen_profession_research : public SpellScript
{
    PrepareSpellScript(spell_gen_profession_research);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleScript(SpellEffIndex)
    {
        if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
            return;

        Player* caster = GetCaster()->ToPlayer();
        uint32 spellId = GetSpellInfo()->Id;

        // learn random explicit discovery recipe (if any)
        if (uint32 discoveredSpellId = GetExplicitDiscoverySpell(spellId, caster))
            caster->LearnSpell(discoveredSpellId, false);

        caster->UpdateCraftSkill(spellId);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_profession_research::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 131759 - Secrets of the Stone
// 131686 - Primordial Ruby
// 131593 - River's Heart
// 131695 - Sun's Radiance
// 131688 - Wild Jade
// 131690 - Vermilion Onyx
// 131691 - Imperial Amethyst
class spell_gen_jewelcrafting_research : public SpellScript
{
    PrepareSpellScript(spell_gen_jewelcrafting_research);

    enum
    {
        DISCOVERY_RED       = 131686,
        DISCOVERY_BLUE      = 131593,
        DISCOVERY_YELLOW    = 131695,
        DISCOVERY_GREEN     = 131688,
        DISCOVERY_ORANGE    = 131690,
        DISCOVERY_PURPLE    = 131691,
    };

    uint32 m_red = 0;
    uint32 m_bule = 0;
    uint32 m_yellow = 0;
    uint32 m_green = 0;
    uint32 m_orange = 0;
    uint32 m_purple = 0;

    void HandleScript(SpellEffIndex)
    {
        Player* caster = GetCaster()->ToPlayer();
        uint32 spellId = GetSpellInfo()->Id;

        m_red    = GetExplicitDiscoverySpell(DISCOVERY_RED,    caster);
        m_bule   = GetExplicitDiscoverySpell(DISCOVERY_BLUE,   caster);
        m_yellow = GetExplicitDiscoverySpell(DISCOVERY_YELLOW, caster);
        m_green  = GetExplicitDiscoverySpell(DISCOVERY_GREEN,  caster);
        m_orange = GetExplicitDiscoverySpell(DISCOVERY_ORANGE, caster);
        m_purple = GetExplicitDiscoverySpell(DISCOVERY_PURPLE, caster);

        if (!m_red && !m_bule && !m_yellow && !m_green && !m_orange && !m_purple)
            return;

        uint32 discoveredSpell = 0;

        if (spellId == DISCOVERY_RED && m_red)
            discoveredSpell = m_red;
        else if (spellId == DISCOVERY_BLUE && m_bule)
            discoveredSpell = m_bule;
        else if (spellId == DISCOVERY_YELLOW && m_yellow)
            discoveredSpell = m_yellow;
        else if (spellId == DISCOVERY_GREEN && m_green)
            discoveredSpell = m_green;
        else if (spellId == DISCOVERY_ORANGE && m_orange)
            discoveredSpell = m_orange;
        else if (spellId == DISCOVERY_PURPLE && m_purple)
            discoveredSpell = m_purple;
        else
        {
            std::list<uint32> spells{ m_red, m_bule, m_yellow, m_green, m_orange, m_purple };
            spells.remove(0);
            ASSERT(!spells.empty());
            auto it = spells.begin();
            std::advance(it, urand(0, spells.size() - 1));
            discoveredSpell = *it;
        }
        GetCaster()->ToPlayer()->LearnSpell(discoveredSpell, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_jewelcrafting_research::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 112996 - Scroll of Wisdom
class spell_gen_scroll_of_wisdom : public SpellScript
{
    PrepareSpellScript(spell_gen_scroll_of_wisdom);

    void HandleScript(SpellEffIndex)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        uint32 discoveredSpell = GetExplicitDiscoverySpell(GetSpellInfo()->Id, caster);
        if (!discoveredSpell)
            discoveredSpell = GetExplicitDiscoverySpell(61177, caster);  // Northrend Inscription Research

        if (discoveredSpell)
            caster->LearnSpell(discoveredSpell, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_scroll_of_wisdom::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

static uint32 GetStatForFood(Player* target)
{
    switch (target->GetRoleForGroup(target->GetTalentSpecialization()))
    {
        case ROLES_TANK:
            return STAT_STAMINA;
        case ROLES_HEALER:
            return STAT_SPIRIT;
        default:
            break;
    }

    std::vector<float> stats{ target->GetStat(STAT_STRENGTH), target->GetStat(STAT_AGILITY), 0,
        target->GetStat(STAT_INTELLECT), target->GetStat(STAT_SPIRIT) };

    auto it = std::max_element(stats.begin(), stats.end());
    return std::distance(stats.begin(), it);

}

class spell_gen_banquet_base : public SpellScript
{
protected:
    void HandleHit()
    {
        if (Player* target = GetHitUnit()->ToPlayer())
            target->CastSpell(target, GetSpellId(GetStatForFood(target)), true);
    }

    virtual uint32 GetSpellId(uint32) = 0;
};

// 126532 - Banquet of the Grill
// 126541 - Banquet of the Wok
// 126542 - Banquet of the Pot
// 126543 - Banquet of the Steamer
// 126544 - Banquet of the Oven
// 126545 - Banquet of the Brew
class spell_gen_banquet_of_the_x : public spell_gen_banquet_base
{
    PrepareSpellScript(spell_gen_banquet_of_the_x);

    uint32 GetSpellId(uint32 stat) override
    {
        switch (stat)
        {
            case STAT_STRENGTH:
                if (GetSpellInfo()->Id == 126532)   // Banquet of the Grill
                    return 104284;  // 275 strength
                return 104263;      // 250 strength
            case STAT_AGILITY:
                if (GetSpellInfo()->Id == 126541)   // Banquet of the Wok
                    return 104287;  // 275 agility
                return 104286;      // 250 agility
            case STAT_STAMINA:
                if (GetSpellInfo()->Id == 126544)   // Banquet of the Oven
                    return 104295;  // 415 stamina
                return 104294;      // 375 stamina
            case STAT_INTELLECT:
                if (GetSpellInfo()->Id == 126542)   // Banquet of the Pot
                    return 104290;  // 275 intillect
                return 104266;      // 250 intellect
            case STAT_SPIRIT:
                if (GetSpellInfo()->Id == 126542)   // Banquet of the Pot
                    return 104266;  // 275 intillect
                if (GetSpellInfo()->Id == 126543)   // Banquet of the Steamer
                    return 104292;  // 275 spirit
                return 104291;      // 250 spirit
            default:
                break;
        }
        return 0;
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_gen_banquet_of_the_x::HandleHit);
    }
};

// 104924 - Pandaren Banquet
class spell_gen_pandaren_banquet : public spell_gen_banquet_base
{
    PrepareSpellScript(spell_gen_pandaren_banquet);

    uint32 GetSpellId(uint32 stat) override
    {
        switch (stat)
        {
            case STAT_STRENGTH:
                return 104284;  // 275 strength
            case STAT_AGILITY:
                return 104287;  // 275 agility
            case STAT_STAMINA:
                return 104295;  // 415 stamina
            case STAT_INTELLECT:
                return 104290;  // 275 intillect
            case STAT_SPIRIT:
                return 104292;  // 275 spirit
            default:
                break;
        }
        return 0;
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_gen_pandaren_banquet::HandleHit);
    }
};

enum
{
    SPELL_COOKING_NOODLE_CART_VISUAL_TIER_1         = 145973,
    SPELL_COOKING_NOODLE_CART_VISUAL_TIER_2         = 146079,
    SPELL_COOKING_NOODLE_CART_VISUAL_TIER_3         = 146080,
    SPELL_CLONE_ME_NO_WEAPONS                       = 149716,
    SPELL_VENDOR_CHEF_APPAREL                       = 148947, // Apparently this spell must trigger visual kit
    SPELL_POOF                                      = 146083,

    NPC_NOODLE_CART_VENDOR_TIER_1 = 72758,
    NPC_NOODLE_CART_VENDOR_TIER_2 = 72794,
    NPC_NOODLE_CART_VENDOR_TIER_3 = 72796,
};

// 145166 - Noodle Cart
// 145169 - Deluxe Noodle Cart
// 145196 - Pandaren Treasure Noodle Cart
class spell_gen_noodle_cart : public SpellScript
{
    PrepareSpellScript(spell_gen_noodle_cart);

    uint32 GetSpell(uint32 stat) const
    {
        switch (stat)
        {
            case STAT_STRENGTH:
                return 104271;  // 275 strength
            case STAT_AGILITY:
                return 104274;  // 275 agility
            case STAT_STAMINA:
                return 104282;  // 415 stamina
            case STAT_INTELLECT:
                return 104276;  // 275 intillect
            case STAT_SPIRIT:
                return 104279;  // 275 spirit
            default:
                break;
        }
        return 0;
    }

    uint32 GetSpellVisualKit()
    {
        typedef std::array<uint32, 2> VisualKit;
        static std::map<uint32, VisualKit> const map
        {
            //                            male      female
            { RACE_NIGHTELF,            { 39986,    39987 } },
            { RACE_DRAENEI,             { 39988,    39989 } },
            { RACE_WORGEN,              { 39990,    39991 } },
            { RACE_TAUREN,              { 39998,    39999 } },
            { RACE_PANDAREN_ALLIANCE,   { 40002,    40003 } },
            { RACE_PANDAREN_HORDE,      { 40002,    40003 } },
        };

        auto itr = map.find(GetCaster()->GetRace());
        if (itr != map.end())
            return itr->second[GetCaster()->GetGender()];
        return 39980;
    }

    void HandleCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
        if (!spellInfo)
            return;

        player->CastSpell(player, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
        player->CastSpell(player, GetSpell(GetStatForFood(player)), true);

        for (auto&& summon : player->GetSummons())
        {
            if (summon->GetEntry() == (uint32)spellInfo->Effects[EFFECT_1].MiscValue)
            {
                uint32 vendorId = 0, cartVisual = 0;
                switch (GetSpellInfo()->Id)
                {
                    case 145166: vendorId = NPC_NOODLE_CART_VENDOR_TIER_1; cartVisual = SPELL_COOKING_NOODLE_CART_VISUAL_TIER_1; break;
                    case 145169: vendorId = NPC_NOODLE_CART_VENDOR_TIER_2; cartVisual = SPELL_COOKING_NOODLE_CART_VISUAL_TIER_2; break;
                    case 145196: vendorId = NPC_NOODLE_CART_VENDOR_TIER_3; cartVisual = SPELL_COOKING_NOODLE_CART_VISUAL_TIER_3; break;
                }
                Creature* vendor = player->SummonCreature(vendorId, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 180000);
                if (!vendor)
                    return;

                player->EnterVehicle(summon, 0);
                player->CastSpell(vendor, SPELL_CLONE_ME_NO_WEAPONS, true);

                vendor->CastSpell(vendor, SPELL_VENDOR_CHEF_APPAREL, true);
                vendor->SendPlaySpellVisualKit(GetSpellVisualKit(), 0, 0);
                vendor->EnterVehicle(summon, 1);

                summon->CastSpell(summon, cartVisual, true);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_gen_noodle_cart::HandleCast);
    }
};

// 146570 - Wave
class spell_gen_noodle_cart_wave : public SpellScript
{
    PrepareSpellScript(spell_gen_noodle_cart_wave);

    struct TextBuilder
    {
        TextBuilder(Player* source, int32 id, WorldObject* target) : m_source(source), m_textId(id), m_target(target) { }

        size_t operator()(WorldPacket* data, LocaleConstant locale) const
        {
            std::string text = sObjectMgr->GetTrinityString(m_textId, locale);
            return ChatHandler::BuildChatPacket(*data, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, m_source->GetGUID(), m_target ? m_target->GetGUID() : ObjectGuid::Empty, text, 0,
                m_source->GetNameForLocaleIdx(locale), m_target ? m_target->GetNameForLocaleIdx(locale) : "");
        }

        Player* m_source;
        int32 m_textId;
        WorldObject* m_target;
    };

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner)
        {
            targets.clear();
            return;
        }

        targets.remove(owner);

        if (!targets.empty())
            Trinity::RandomResizeList(targets, 1);
        else if (targets.empty())
        {
            Player* player = owner->ToPlayer();
            sCreatureTextMgr->SendChatPacket(player, TextBuilder(player, LANG_NOODLE_CART_WAVE_NO_TARGET, nullptr), CHAT_MSG_MONSTER_SAY);
        }
    }

    void HandleHit()
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner)
            return;

        Player* player = owner->ToPlayer();
        Player* target = GetHitUnit()->ToPlayer();
        sCreatureTextMgr->SendChatPacket(player, TextBuilder(player, LANG_NOODLE_CART_WAVE_TARGET, target), CHAT_MSG_MONSTER_SAY);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_noodle_cart_wave::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnHit += SpellHitFn(spell_gen_noodle_cart_wave::HandleHit);
    }
};

// 145973 - 5.4 Tradeskills - Cooking Noodle Cart - Tier 1 - 10p Spawn
// 146079 - 5.4 Tradeskills - Cooking Noodle Cart - Tier 2 - 10p Spawn
// 146080 - 5.4 Tradeskills - Cooking Noodle Cart - Tier 3 - 10p Spawn
class spell_gen_noodle_cart_visual : public AuraScript
{
    PrepareAuraScript(spell_gen_noodle_cart_visual);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            std::list<TempSummon*> summons;
            owner->GetSummons(summons, NPC_NOODLE_CART_VENDOR_TIER_1);
            owner->GetSummons(summons, NPC_NOODLE_CART_VENDOR_TIER_2);
            owner->GetSummons(summons, NPC_NOODLE_CART_VENDOR_TIER_3);
            for (auto&& itr : summons)
                itr->UnSummon();
            owner->SetStandState(UNIT_STAND_STATE_STAND);
        }
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_POOF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_gen_noodle_cart_visual::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 126535 - Food
// 104922 - Food
// 144961 - Food
class spell_gen_noodle_soup : public AuraScript
{
    PrepareAuraScript(spell_gen_noodle_soup);

    uint32 GetBuffLevel() const
    {
        switch (GetSpellInfo()->Id)
        {
            case 126535: return 0;
            case 104922: return 1;
            case 144961: return 2;
        }
        return 0;
    }

    uint32 GetSpell(uint32 stat) const
    {
        typedef std::array<uint32, 3> FoodBuffs;
        static std::map<uint32, FoodBuffs> const buffs
        {
            //                     250     275     300
            { STAT_STRENGTH,    { 104267, 104271, 104272 } },
            { STAT_AGILITY,     { 104273, 104274, 104275 } },
            { STAT_STAMINA,     { 104281, 104282, 104283 } },
            { STAT_INTELLECT,   { 104264, 104276, 104277 } },
            { STAT_SPIRIT,      { 104278, 104279, 104280 } },
        };

        auto itr = buffs.find(stat);
        if (itr == buffs.end())
            return 0;
        return itr->second[GetBuffLevel()];
    }

    void HandleTick(AuraEffect const*)
    {
        PreventDefaultAction();

        if (Player* player = GetUnitOwner()->ToPlayer())
            player->CastSpell(GetUnitOwner(), GetSpell(GetStatForFood(player)), true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_noodle_soup::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 127250 - Ancient Knowledge
class spell_gen_ancient_knowledge : public AuraScript
{
    PrepareAuraScript(spell_gen_ancient_knowledge);

    void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
    {
        if (Player* player = GetAura()->GetOwner()->ToPlayer())
            if (player->GetLevel() > 84)
                amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_ancient_knowledge::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_XP_QUEST_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_ancient_knowledge::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_XP_PCT);
    }
};

// 57353 / 71354 - Heirloom Experience Bonus 5% / 10%
class spell_gen_heirloom_experience_bonus : public AuraScript
{
    PrepareAuraScript(spell_gen_heirloom_experience_bonus);

    void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
    {
        if (Aura* aura = GetAura())
            if (Player* player = aura->GetOwner()->ToPlayer())
                if (Item* item = player->GetItemByGuid(aura->GetCastItemGUID()))
                    if (ItemTemplate const* itemTemp = item->GetTemplate())
                        if (ScalingStatDistributionEntry const* ssd = sScalingStatDistributionStore.LookupEntry(itemTemp->ScalingStatDistribution))
                            if (ssd && ssd->MaxLevel < DEFAULT_MAX_LEVEL && ssd->MaxLevel < player->GetLevel())
                                amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_heirloom_experience_bonus::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_XP_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_heirloom_experience_bonus::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_XP_QUEST_PCT);
    }
};

// 58984 - Shadowmeld
class spell_gen_shadowmeld : public SpellScript
{
    PrepareSpellScript(spell_gen_shadowmeld);

    void HandleHit(SpellEffIndex)
    {
        Unit *caster = GetCaster();
        if (!caster->GetInstanceScript() || !caster->GetInstanceScript()->IsEncounterInProgress()) //Don't leave combat if you are in combat with a boss
            caster->CombatStop();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_shadowmeld::HandleHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Free Action Potion - 6615
class spell_gen_free_action_potion : public SpellScript
{
    PrepareSpellScript(spell_gen_free_action_potion);

    void HandleAfterHit()
    {
        if (!GetCaster() || !GetHitUnit())
            return;

        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetLevel() >= 77)
            {
                if (Aura* aura = player->GetAura(6615, player->GetGUID()))
                {
                    int32 duration = (30 - ((player->GetLevel() - 77) * 3)) * IN_MILLISECONDS;
                    if (duration < 0)
                        duration = 5000;

                    aura->SetDuration(duration);
                    aura->SetMaxDuration(duration);
                }
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_gen_free_action_potion::HandleAfterHit);
    }
};

// Elixir of Wandering Spirits - 147412
class spell_gen_elixir_of_wandering_spirits : public SpellScript
{
    PrepareSpellScript(spell_gen_elixir_of_wandering_spirits);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        std::vector<uint32> wanderingSpiritsAuras = { 147402, 147403, 147405, 147406, 147407, 147409, 147410, 147411 };

        for (auto&& aura : wanderingSpiritsAuras)
            caster->RemoveAura(aura);

        std::list<TempSummon*> summons;
        caster->GetSummons(summons, 73523);
        for (auto&& summon : summons)
            summon->DespawnOrUnsummon();

        caster->CastSpell(caster, Trinity::Containers::SelectRandomContainerElement(wanderingSpiritsAuras), true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_elixir_of_wandering_spirits::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 137595 - Lightning Strike Charges Trigger
class spell_gen_lightning_strike_charges_trigger : public AuraScript
{
    PrepareAuraScript(spell_gen_lightning_strike_charges_trigger);

    enum { SPELL_CAPACITANCE = 137596 };

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget())
            GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_CAPACITANCE, true);
    }
 
    void Register() override
    {
        OnProc += AuraProcFn(spell_gen_lightning_strike_charges_trigger::HandleProc);
    }
};

// 137596 - Capacitance
class spell_gen_capacitance : public AuraScript
{
    PrepareAuraScript(spell_gen_capacitance);

    enum
    {
        SPELL_LIGHTNING_STRIKE_MELEE  = 137597,
        SPELL_LIGHTNING_STRIKE_RANGED = 141004,
    };

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetAura()->GetStackAmount() >= GetAura()->GetMaxStackAmount())
        {
            if (Unit* target = ObjectAccessor::GetUnit(*GetUnitOwner(), GetUnitOwner()->GetTarget()))
            {
                uint32 spell = GetUnitOwner()->GetClass() == CLASS_HUNTER ? SPELL_LIGHTNING_STRIKE_RANGED : SPELL_LIGHTNING_STRIKE_MELEE;
                GetUnitOwner()->CastSpell(target, spell, true);
            }
            Remove();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gen_capacitance::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 137248 - Clearcasting Trigger
class spell_gen_clearcasting_trigger : public AuraScript
{
    PrepareAuraScript(spell_gen_clearcasting_trigger);

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        uint32 spellId = 0;
        switch (GetUnitOwner()->GetClass())
        {
            case CLASS_DRUID:  spellId = 137247; break;
            case CLASS_PALADIN:spellId = 137288; break;
            case CLASS_PRIEST: spellId = 137323; break;
            case CLASS_SHAMAN: spellId = 137326; break;
            case CLASS_MONK:   spellId = 137331; break;
            default:
                return;
        }
        GetUnitOwner()->CastSpell(GetUnitOwner(), spellId, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_gen_clearcasting_trigger::HandleProc);
    }
};

// 101184 - Leyara's Locket
class spell_gen_leyaras_locket : public SpellScript
{
    PrepareSpellScript(spell_gen_leyaras_locket);

    void HandleScript()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        caster->CastSpell(caster, caster->GetGender() == GENDER_MALE ? 101185 : 101186);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_gen_leyaras_locket::HandleScript);
    }
};

enum HauntedMementoEnums
{
    SPELL_FORCE_PLAYER_TO_REMOVE_HAUNTED_MEMENTO    = 54042,
};

class spell_throw_haunted_memento : public SpellScript
{
    PrepareSpellScript(spell_throw_haunted_memento);

    void HandleExtraEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (Unit* caster = GetCaster())
                target->CastSpell(caster, SPELL_FORCE_PLAYER_TO_REMOVE_HAUNTED_MEMENTO, true);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_throw_haunted_memento::HandleExtraEffect, EFFECT_0, SPELL_EFFECT_CREATE_ITEM);
    }
};

enum MusicBox
{
    SOUND_LAMENT_OF_THE_HIGHBORNE = 15095,
};

class spell_item_sylvanas_music_box : public SpellScript
{
    PrepareSpellScript(spell_item_sylvanas_music_box);

    bool Load()
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (Player* target = GetHitPlayer())
            target->PlayDirectSound(SOUND_LAMENT_OF_THE_HIGHBORNE, target);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_sylvanas_music_box::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 127249 - Flesh to Stone
class spell_gen_flesh_to_stone : public AuraScript
{
    PrepareAuraScript(spell_gen_flesh_to_stone);

    void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetUnitOwner())
            if (CreatureTemplate const* creatureTemplate = sObjectMgr->GetCreatureTemplate(aurEff->GetMiscValue()))
            {
                CreatureModel const* model2 = creatureTemplate->GetModelByIdx(1);
                owner->SetDisplayId(model2->CreatureDisplayID);
            }
    }

    void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetUnitOwner())
        {
            owner->CastSpell(owner, 129782);
            if (GameObject* statue = owner->FindNearestGameObject(214992, 10.0f))
                statue->Delete();
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gen_flesh_to_stone::HandleApplyEffect, EFFECT_2, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_gen_flesh_to_stone::HandleRemoveEffect, EFFECT_2, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

enum LadyJainaShaolmara
{
    QUEST_THE_ASSAULT_ON_ZEB_TULA  = 32212,
    QUEST_THE_ASSAULT_ON_SHAOLMARA = 32644,
};

// 140295 - Kirin Tor Beacon / 140300 - Sunreaver Beacon
class spell_gen_isle_of_thunder_beacon : public SpellScript
{
    PrepareSpellScript(spell_gen_isle_of_thunder_beacon);

    void HandleScript()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetTeam() == HORDE && GetSpellInfo()->Id == 140300)
            {
                if (player->GetQuestStatus(QUEST_THE_ASSAULT_ON_ZEB_TULA) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_THE_ASSAULT_ON_ZEB_TULA) == QUEST_STATUS_INCOMPLETE)
                    player->CastSpell(player, GetSpellInfo()->Effects[EFFECT_0].BasePoints);
                else
                    player->CastSpell(player, GetSpellInfo()->Effects[EFFECT_1].BasePoints);
            }
            else if (player->GetTeam() == ALLIANCE && GetSpellInfo()->Id == 140295)
            {
                if (player->GetQuestStatus(QUEST_THE_ASSAULT_ON_SHAOLMARA) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_THE_ASSAULT_ON_SHAOLMARA) == QUEST_STATUS_INCOMPLETE)
                    player->CastSpell(player, GetSpellInfo()->Effects[EFFECT_0].BasePoints);
                else
                    player->CastSpell(player, GetSpellInfo()->Effects[EFFECT_1].BasePoints);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_gen_isle_of_thunder_beacon::HandleScript);
    }
};

// 138021 - Portal Effect: Isle of the Thunder King / 138023 - Portal to the Isle of Thunder
class spell_gen_portal_to_isle_of_thunder : public SpellScript
{
    PrepareSpellScript(spell_gen_portal_to_isle_of_thunder);

    void SelectDest(SpellDestination& dest)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            Position pos;
            if (player->GetTeam() == HORDE && GetSpellInfo()->Id == 138021)
            {
                if (player->GetQuestStatus(QUEST_THE_ASSAULT_ON_ZEB_TULA) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_ASSAULT_ON_ZEB_TULA) == QUEST_STATUS_NONE)
                    pos = { 6707.11f, 6487.88f, 4.7136f, 4.7334f };
                else
                    pos = { 7253.94f, 6279.22f, 19.381f, 3.9156f };
            }
            else if (player->GetTeam() == ALLIANCE && GetSpellInfo()->Id == 138023)
            {
                if (player->GetQuestStatus(QUEST_THE_ASSAULT_ON_SHAOLMARA) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_ASSAULT_ON_SHAOLMARA) == QUEST_STATUS_NONE)
                    pos = { 5672.19f, 6215.18f, 12.967f, 5.4575f };
                else
                    pos = { 6122.92f, 4981.45f, 36.893f, 1.5784f };
            }
            else
            {
                TC_LOG_ERROR("shit", "spell_gen_portal_to_isle_of_thunder: Player %u, team %u, spell %u.", player->GetGUID().GetCounter(), player->GetTeam(), GetSpellInfo()->Id);
                return;
            }
            dest._position.m_mapId = 1064;
            dest.Relocate(pos);
        }
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_gen_portal_to_isle_of_thunder::SelectDest, EFFECT_0, TARGET_UNIT_CASTER);
    }
};

// 118358 - Drink
class spell_gen_cobo_cola : public AuraScript
{
    PrepareAuraScript(spell_gen_cobo_cola);

    bool allowRegen = true;

    void CalculatePeriodic(AuraEffect const* /*effect*/, bool&, int32& amplitude)
    {
        if (GetUnitOwner()->GetMap()->IsBattleArena())
        {
            allowRegen = false;
            amplitude = 1950; // Derp
        }
    }

    void CalculateAmount(AuraEffect const* /*effect*/, float& amount, bool&)
    {
        if (!allowRegen)
            amount = 0;
    }

    void HandleTick(AuraEffect const* effect)
    {
        if (effect->GetTickNumber() == 3 && !allowRegen)
        {
            allowRegen = true;
            GetEffect(EFFECT_0)->RecalculateAmount();
        }
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_gen_cobo_cola::CalculatePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_cobo_cola::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_cobo_cola::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 43681 - Inactive
class spell_gen_inactive : public AuraScript
{
    PrepareAuraScript(spell_gen_inactive);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
                GetTarget()->ToPlayer()->LeaveBattleground();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_gen_inactive::HandleRemove, EFFECT_0, SPELL_AURA_MOD_CURRENCY_GAIN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 139164 Standing at Attention
class spell_gen_standing_at_attention : public AuraScript
{
    PrepareAuraScript(spell_gen_standing_at_attention);

    void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->ModifyAuraState(AuraStateType(AURA_STATE_HEALTH_ABOVE_75_PERCENT), true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gen_standing_at_attention::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 34709 - Shadow Sight
class spell_shadow_sight : public AuraScript
{
    PrepareAuraScript(spell_shadow_sight);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
        GetUnitOwner()->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_shadow_sight::HandleApply, EFFECT_0, SPELL_AURA_DETECT_STEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_gen_readiness_base : public AuraScript
{
    PrepareAuraScript(spell_gen_readiness_base);

    uint32 spellId = 0;

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        spellId = GetSpell();
        if (spellId)
            GetUnitOwner()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, -effect->GetAmount(), GetUnitOwner(), true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (spellId)
            GetUnitOwner()->RemoveAurasDueToSpell(spellId);
    }

    virtual uint32 GetSpell() = 0;

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gen_readiness_base::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_gen_readiness_base::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 145955 - Readiness
class spell_gen_readiness_strength_dps : public spell_gen_readiness_base
{
    PrepareAuraScript(spell_gen_readiness_strength_dps);

    uint32 GetSpell() override
    {
        switch (GetUnitOwner()->ToPlayer()->GetSpecialization())
        {
            case SPEC_DEATH_KNIGHT_FROST:   return 145959;
            case SPEC_DEATH_KNIGHT_UNHOLY:  return 145960;
            case SPEC_DRUID_FERAL:          return 145961;
            case SPEC_PALADIN_RETRIBUTION:  return 145978;
            case SPEC_WARRIOR_ARMS:         return 145990;
            case SPEC_WARRIOR_FURY:         return 145991;

            default: return 0;
        }
        return 0;
    }
};

// 146019 - Readiness
class spell_gen_readiness_agility_dps : public spell_gen_readiness_base
{
    uint32 GetSpell() override
    {
        switch (GetUnitOwner()->ToPlayer()->GetSpecialization())
        {
            case SPEC_DRUID_FERAL:          return 145961;
            case SPEC_HUNTER_BEAST_MASTERY: return 145964;
            case SPEC_HUNTER_MARKSMANSHIP:  return 145965;
            case SPEC_HUNTER_SURVIVAL:      return 145966;
            case SPEC_MONK_WINDWALKER:      return 145969;
            case SPEC_ROGUE_ASSASSINATION:  return 145983;
            case SPEC_ROGUE_COMBAT:         return 145984;
            case SPEC_ROGUE_SUBTLETY:       return 145985;
            case SPEC_SHAMAN_ENHANCEMENT:   return 145986;

            default: return 0;
        }
        return 0;
    }
};

// 146025 - Readiness
class spell_gen_readiness_tank : public spell_gen_readiness_base
{
    PrepareAuraScript(spell_gen_readiness_strength_dps);

    uint32 GetSpell() override
    {
        switch (GetUnitOwner()->ToPlayer()->GetSpecialization())
        {
            case SPEC_DEATH_KNIGHT_BLOOD:   return 145958;
            case SPEC_DRUID_GUARDIAN:       return 145962;
            case SPEC_MONK_BREWMASTER:      return 145967;
            case SPEC_PALADIN_PROTECTION:   return 145976;
            case SPEC_WARRIOR_PROTECTION:   return 145992;

            default: return 0;
        }
        return 0;
    }
};

// 143333 - Water Strider Water Walking
class spell_gen_water_strider_water_walking : public AuraScript
{
    PrepareAuraScript(spell_gen_water_strider_water_walking);

    bool CheckArea(Unit* target)
    {
        return target->GetBattlegorund() == nullptr;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor() != GetUnitOwner())
            Remove();
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_water_strider_water_walking::CheckArea);
        OnProc += AuraProcFn(spell_gen_water_strider_water_walking::HandleProc);
    }
};

// 52415 - Carrying Seaforium
class spell_carrying_seaforium : public AuraScript
{
    PrepareAuraScript(spell_carrying_seaforium);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
        GetUnitOwner()->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_carrying_seaforium::HandleApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 129023 - Surgical Alterations
class spell_gen_surgical_alterations : public AuraScript
{
    PrepareAuraScript(spell_gen_surgical_alterations);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->Yell(player->GetSession()->GetTrinityString(LANG_KRASTINOVS_BAG_OF_HORRORS_USE), LANG_UNIVERSAL);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Player* player = GetOwner()->ToPlayer())
            player->Yell(player->GetSession()->GetTrinityString(urand(LANG_KRASTINOVS_BAG_OF_HORRORS_1, LANG_KRASTINOVS_BAG_OF_HORRORS_7)), LANG_UNIVERSAL);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gen_surgical_alterations::HandleApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_gen_surgical_alterations::HandleProc);
    }
};

// Deserter - 26013
class spell_gen_deserter : public SpellScript
{
    PrepareSpellScript(spell_gen_deserter);

    void HandleScript()
    {
        uint32 duration = 300 * IN_MILLISECONDS;
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (uint8 mod = player->GetDeserterMod())
            {
                mod += 1; // because 300 * 1 = 300, we need 600
                if (mod > 4)
                    mod = 4;

                duration *= mod;
            }

            if (Aura* aura = player->GetAura(GetSpellInfo()->Id, player->GetGUID()))
            {
                aura->SetDuration(duration);
                aura->SetMaxDuration(duration);
            }
        }
    }

    void Register()
    {
        AfterCast += SpellCastFn(spell_gen_deserter::HandleScript);
    }
};

// 69041 - Rocket Barrage (Goblin Racial)
class spell_gen_rocket_barrage : public SpellScript
{
    PrepareSpellScript(spell_gen_rocket_barrage);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            // Get highest attack power
            uint32 ap = std::max(caster->GetTotalAttackPowerValue(BASE_ATTACK), caster->GetTotalAttackPowerValue(RANGED_ATTACK));
            SetHitDamage(1 + (0.25f * ap) + (0.429f * caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE)) + (caster->GetLevel() * 2));
        }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_rocket_barrage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

void AddSC_generic_spell_scripts()
{
    new aura_script<spell_gen_absorb0_hitlimit1>("spell_gen_absorb0_hitlimit1");
    new aura_script<spell_gen_adaptive_warding>("spell_gen_adaptive_warding");
    new aura_script<spell_gen_alchemist_stone>("spell_gen_alchemist_stone");
    new spell_script<spell_gen_allow_cast_from_item_only>("spell_gen_allow_cast_from_item_only");
    new aura_script<spell_gen_animal_blood>("spell_gen_animal_blood");
    new spell_script<spell_spawn_blood_pool>("spell_spawn_blood_pool");
    new aura_script<spell_gen_aura_of_anger>("spell_gen_aura_of_anger");
    new aura_script<spell_gen_aura_service_uniform>("spell_gen_aura_service_uniform");
    new aura_script<spell_gen_av_drekthar_presence>("spell_gen_av_drekthar_presence");
    new spell_script<spell_gen_bandage>("spell_gen_bandage");
    new spell_script<spell_gen_bonked>("spell_gen_bonked");
    new spell_gen_break_shield("spell_gen_break_shield");
    new spell_gen_break_shield("spell_gen_tournament_counterattack");
    new aura_script<spell_gen_burn_brutallus>("spell_gen_burn_brutallus");
    new spell_script<spell_gen_cannibalize>("spell_gen_cannibalize");
    new spell_script<spell_gen_chaos_blast>("spell_gen_chaos_blast");
    new spell_script<spell_gen_clone>("spell_gen_clone");
    new spell_script<spell_gen_clone_weapon>("spell_gen_clone_weapon");
    new spell_gen_clone_weapon_aura();
    new spell_gen_count_pct_from_max_hp("spell_gen_default_count_pct_from_max_hp");
    new spell_gen_count_pct_from_max_hp("spell_gen_50pct_count_pct_from_max_hp", 50);
    new spell_gen_create_lance();
    new spell_gen_creature_permanent_feign_death();
    new spell_gen_dalaran_disguise("spell_gen_sunreaver_disguise");
    new spell_gen_dalaran_disguise("spell_gen_silver_covenant_disguise");
    new spell_gen_despawn_self();
    new spell_gen_divine_storm_cd_reset();
    new spell_gen_ds_flush_knockback();
    new spell_gen_dummy_trigger();
    new spell_gen_dungeon_credit();
    new spell_gen_elune_candle();
    new spell_gen_fishing();
    new spell_gen_gadgetzan_transporter_backfire();
    new aura_script<spell_gen_gift_of_naaru>("spell_gen_gift_of_naaru");
    new spell_gen_gnomish_transporter();
    new spell_gen_interrupt();
    new spell_gen_lifebloom("spell_hexlord_lifebloom", SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_tur_ragepaw_lifebloom", SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_cenarion_scout_lifebloom", SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_twisted_visage_lifebloom", SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_faction_champion_dru_lifebloom", SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_mounted_charge();
    new spell_gen_netherbloom();
    new spell_gen_obsidian_armor();
    new spell_gen_on_tournament_mount();
    new spell_gen_oracle_wolvar_reputation();
    new spell_gen_orc_disguise();
    new spell_gen_parachute();
    new spell_gen_remove_flight_auras();
    new spell_gen_replenishment();
    // Running Wild
    new spell_gen_running_wild();
    new spell_gen_two_forms();
    new spell_gen_darkflight();
    /*                          */
    new spell_gen_seaforium_blast();
    new spell_gen_spectator_cheer_trigger();
    new spell_gen_spirit_healer_res();
    new spell_gen_summon_elemental("spell_gen_summon_fire_elemental", SPELL_SUMMON_FIRE_ELEMENTAL);
    new spell_gen_summon_elemental("spell_gen_summon_earth_elemental", SPELL_SUMMON_EARTH_ELEMENTAL);
    new spell_gen_summon_tournament_mount();
    new spell_gen_tournament_duel();
    new spell_gen_tournament_pennant();
    new spell_script<spell_pvp_trinket_wotf_shared_cd>("spell_pvp_trinket_wotf_shared_cd");
    new aura_script<spell_gen_turkey_marker>("spell_gen_turkey_marker");
    new spell_script<spell_gen_upper_deck_create_foam_sword>("spell_gen_upper_deck_create_foam_sword");
    new spell_gen_vehicle_scaling();
    new spell_gen_vendor_bark_trigger();
    new spell_gen_wg_water();
    new spell_gen_whisper_gulch_yogg_saron_whisper();
    new spell_gen_survey();
    new spell_gen_searching_for_artifacts();

    new spell_script<spell_gen_debug_move>("spell_gen_debug_move");
    new spell_script<spell_gen_eye_of_the_black_prince>("spell_gen_eye_of_the_black_prince");
    new aura_script<spell_gen_the_doctor_is_in>("spell_gen_the_doctor_is_in");
    new spell_script<spell_gen_mobile_banking>("spell_gen_mobile_banking");
    new aura_script<spell_gen_the_quick_and_the_dead>("spell_gen_the_quick_and_the_dead");
    new spell_script<spell_gen_mass_resurrection>("spell_gen_mass_resurrection");
    new spell_script<spell_gen_item_crafted_kill_credit>("spell_gen_item_crafted_kill_credit");
    new spell_script<spell_gen_profession_research>("spell_gen_profession_research");
    new spell_script<spell_gen_jewelcrafting_research>("spell_gen_jewelcrafting_research");
    new spell_script<spell_gen_scroll_of_wisdom>("spell_gen_scroll_of_wisdom");
    new spell_script<spell_gen_banquet_of_the_x>("spell_gen_banquet_of_the_x");
    new spell_script<spell_gen_pandaren_banquet>("spell_gen_pandaren_banquet");
    new spell_script<spell_gen_noodle_cart>("spell_gen_noodle_cart");
    new spell_script<spell_gen_noodle_cart_wave>("spell_gen_noodle_cart_wave");
    new aura_script<spell_gen_noodle_cart_visual>("spell_gen_noodle_cart_visual");
    new aura_script<spell_gen_noodle_soup>("spell_gen_noodle_soup");
    new aura_script<spell_gen_ancient_knowledge>("spell_gen_ancient_knowledge");
    new aura_script<spell_gen_heirloom_experience_bonus>("spell_gen_heirloom_experience_bonus");
    new spell_script<spell_gen_shadowmeld>("spell_gen_shadowmeld");
    new spell_script<spell_gen_free_action_potion>("spell_gen_free_action_potion");
    new spell_script<spell_gen_elixir_of_wandering_spirits>("spell_gen_elixir_of_wandering_spirits");
    new aura_script<spell_gen_lightning_strike_charges_trigger>("spell_gen_lightning_strike_charges_trigger");
    new aura_script<spell_gen_capacitance>("spell_gen_capacitance");
    new aura_script<spell_gen_clearcasting_trigger>("spell_gen_clearcasting_trigger");
    new spell_script<spell_gen_leyaras_locket>("spell_gen_leyaras_locket");
    new spell_script<spell_throw_haunted_memento>("spell_throw_haunted_memento");
    new spell_script<spell_item_sylvanas_music_box>("spell_item_sylvanas_music_box");
    new aura_script<spell_gen_flesh_to_stone>("spell_gen_flesh_to_stone");
    new spell_script<spell_gen_isle_of_thunder_beacon>("spell_gen_isle_of_thunder_beacon");
    new spell_script<spell_gen_portal_to_isle_of_thunder>("spell_gen_portal_to_isle_of_thunder");
    new aura_script<spell_gen_cobo_cola>("spell_gen_cobo_cola");
    new aura_script<spell_gen_inactive>("spell_gen_inactive");
    new aura_script<spell_gen_standing_at_attention>("spell_gen_standing_at_attention");
    new aura_script<spell_shadow_sight>("spell_shadow_sight");
    new aura_script<spell_gen_water_strider_water_walking>("spell_gen_water_strider_water_walking");
    new aura_script<spell_carrying_seaforium>("spell_carrying_seaforium");
    new aura_script<spell_gen_surgical_alterations>("spell_gen_surgical_alterations");
    new spell_script<spell_gen_deserter>("spell_gen_deserter");
	new spell_script<spell_gen_rocket_barrage>("spell_gen_rocket_barrage");
}
