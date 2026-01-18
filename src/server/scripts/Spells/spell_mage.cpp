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
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ScriptedCreature.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Containers.h"
#include "Pet.h"
#include "spell_common.h"
#include "Random.h"

enum MageSpells
{
    SPELL_MAGE_COLD_SNAP                         = 11958,
    SPELL_MAGE_GLYPH_OF_ETERNAL_WATER            = 70937,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT  = 70908,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY  = 70907,
    SPELL_MAGE_GLYPH_OF_BLAST_WAVE               = 62126,
    SPELL_MAGE_ALTER_TIME_OVERRIDED              = 127140,
    SPELL_MAGE_ALTER_TIME                        = 110909,
    NPC_PAST_SELF                                = 58542,
    SPELL_MAGE_MANA_GEM_ENERGIZE                 = 10052,
    SPELL_MAGE_ARCANE_BRILLIANCE                 = 1459,
    SPELL_MAGE_INFERNO_BLAST                     = 108853,
    SPELL_MAGE_INFERNO_BLAST_IMPACT              = 118280,
    SPELL_MAGE_IGNITE                            = 12654,
    SPELL_MAGE_PYROBLAST                         = 11366,
    SPELL_MAGE_COMBUSTION_DOT                    = 83853,
    SPELL_MAGE_COMBUSTION_IMPACT                 = 118271,
    SPELL_MAGE_GLYPH_OF_COMBUSTION               = 56368,
    SPELL_MAGE_FROSTJAW                          = 102051,
    SPELL_MAGE_NETHER_TEMPEST_SELECTOR           = 114924,
    SPELL_MAGE_NETHER_TEMPEST_DIRECT_DAMAGE      = 114954,
    SPELL_MAGE_NETHER_TEMPEST_MISSILE            = 114956,
    SPELL_MAGE_LIVING_BOMB_TRIGGERED             = 44461,
    SPELL_MAGE_FROST_BOMB_TRIGGERED              = 113092,
    SPELL_MAGE_INVOKERS_ENERGY                   = 116257,
    SPELL_MAGE_INVOCATION                        = 114003,
    SPELL_MAGE_GLYPH_OF_EVOCATION                = 56380,
    SPELL_MAGE_BRAIN_FREEZE                      = 44549,
    SPELL_MAGE_BRAIN_FREEZE_TRIGGERED            = 57761,
    SPELL_MAGE_SLOW                              = 31589,
    SPELL_MAGE_ARCANE_CHARGE                     = 36032,
    SPELL_MAGE_ARCANE_BARRAGE_TRIGGERED          = 50273,
    SPELL_MAGE_PYROMANIAC_AURA                   = 132209,
    SPELL_MAGE_PYROMANIAC_DAMAGE_DONE            = 132210,
    SPELL_MAGE_MIRROR_IMAGE_SUMMON               = 58832,
    SPELL_MAGE_CAUTERIZE                         = 87023,
    SPELL_MAGE_CAUTERIZE_MARKER                  = 87024,
    SPELL_MAGE_ARCANE_MISSILES                   = 79683,
    SPELL_MAGE_ARCANE_MISSILES_DOUBLE            = 79808,
    SPELL_MAGE_ARCANE_MISSILES_CHANNEL           = 5143,
    SPELL_MAGE_INCANTERS_WARD_ENERGIZE           = 113842,
    SPELL_MAGE_INCANTERS_ABSORBTION              = 116267,
    SPELL_MAGE_INCANTERS_ABSORBTION_PASSIVE      = 118858,
    SPELL_MAGE_GLYPH_OF_ICE_BLOCK                = 115723,
    SPELL_MAGE_GLYPH_OF_ICE_BLOCK_IMMUNITY       = 115760,
    SPELL_MAGE_GLYPH_OF_ICE_BLOCK_FROST_NOVA     = 115757,
    SPELL_MAGE_GLYPH_OF_SLOW                     = 86209,
    SPELL_MAGE_GREATER_INVISIBILITY              = 110960,
    SPELL_MAGE_GREATER_INVISIBILITY_LESS_DAMAGE  = 113862,
    SPELL_MAGE_REMOVE_INVISIBILITY_REMOVED_TIMER = 122293,
    SPELL_MAGE_BLAST_WAVE_SNARE                  = 11113,
    SPELL_MAGE_ICE_BLOCK                         = 45438,
    SPELL_MAGE_CONE_OF_COLD                      = 120,
    SPELL_MAGE_FROST_NOVA                        = 122,
    SPELL_MAGE_RING_OF_FROST_FREEZE              = 82691,
    SPELL_MAGE_RING_OF_FROST_SUMMON              = 113724,
    SPELL_MAGE_RING_OF_FROST_OVERRIDEN_EFFECT    = 140384,
    SPELL_MAGE_RING_OF_FROST_MARKER              = 91264,
    SPELL_MAGE_PRESENCE_OF_MIND                  = 12043,
    SPELL_MAGE_GLYPH_OF_MIRROR_IMAGE             = 63093,
    SPELL_MAGE_SUMMON_IMAGES_FROST               = 58832,
    SPELL_MAGE_SUMMON_IMAGES_FIRE                = 88092,
    SPELL_MAGE_SUMMON_IMAGES_ARCANE              = 88091,
    SPELL_MAGE_BLINK                             = 65793,
    SPELL_MAGE_FINGERS_OF_FROST                  = 112965,
    SPELL_MAGE_FINGERS_OF_FROST_EFFECT           = 44544,
    SPELL_MAGE_FINGERS_OF_FROST_DOUBLE           = 126084,
    SPELL_MAGE_MASTERY_ICICLES                   = 76613,
    SPELL_MAGE_ICICILE_TRIGGER_AURA              = 148023,
    SPELL_MAGE_ICICLE_STORE                      = 148012,
    SPELL_MAGE_ICICLE_DAMAGE                     = 148022,
    SPELL_MAGE_ICICILE_VISUAL                    = 148017,
    SPELL_MAGE_RUNE_OF_POWER                     = 116011,
    SPELL_MAGE_RUNE_OF_POWER_AURA                = 116014,
    SPELL_MAGE_HEATING_UP                        = 48107,
    SPELL_MAGE_PYROBLAST_CLEARCASTING            = 48108,
    SPELL_MAGE_SCORCH                            = 2948,
    SPELL_MAGE_ICE_FLOES                         = 108839,
    SPELL_MAGE_ICY_VEINS                         = 12472,
    SPELL_MAGE_ICY_VEINS_GLYPHED                 = 131078,
    SPELL_MAGE_COUNTERSPELL                      = 2139,
    SPELL_MAGE_GLYPH_OF_RAPID_DISPLACEMENT       = 146659,
    SPELL_MAGE_GLYPH_OF_MOMENTUM                 = 56384,
    SPELL_MAGE_GLYPH_OF_MOMENTUM_BLINK           = 119415,
    SPELL_MAGE_GLYPH_OF_SPELLSTEAL               = 115713,
    SPELL_MAGE_GLYPH_OF_SPELLSTEAL_HEAL          = 115714,
    SPELL_MAGE_GLYPH_OF_RAPID_TELEPORTATION      = 89749,
    SPELL_MAGE_RAPID_TELEPORTATION               = 46989,
    SPELL_MAGE_T15_2P_BONUS                      = 138316,
    SPELL_MAGE_TIME_LORD                         = 138317,
    SPELL_MAGE_T15_4P_BONUS                      = 138376,
    SPELL_MAGE_T16_2P_BONUS                      = 145251,
    SPELL_MAGE_POTENT_FLAMES                     = 145254,
    SPELL_MAGE_FROZEN_THOUGHTS                   = 146557,
    SPELL_MAGE_T16_4P_BONUS                      = 145257,
    SPELL_MAGE_FIERY_ADEPT                       = 145261,
    SPELL_MAGE_FRIGID_BLAST                      = 145264,
    SPELL_MAGE_FROSTBOLT_HEAL                    = 126201,
    SPELL_MAGE_GLYPH_OF_INFERNO_BLAST            = 89926,
    SPELL_MAGE_INVISIBILITY                      = 32612,
    SPELL_MAGE_UNBOUND_WATER_ELEMENTAL_TRANSFORM = 147358,
    SPELL_MAGE_TEMPORAL_SHIELD_HEAL              = 115611,

    SPELL_FROZEN_ORB_DUMMY                       = 123605,
    SPELL_FROZEN_ORB_PERIODIC                    = 84717,
    SPELL_FROZEN_ORB_DAMAGE_TRIGGER              = 84718,
    SPELL_FROZEN_ORB_DAMAGE                      = 84721,
    SPELL_FROZEN_ORB_SELF_SNARE                  = 82736,
    SPELL_MIRROR_IMAGE_CLONE_ME                  = 45204,
    SPELL_MIRROR_IMAGE_COPY_WEAPONS              = 41054,
    SPELL_MIRROR_IMAGE_MASTERS_THREAT_LIST       = 58838,
    SPELL_DRUID_MIRROR_IMAGES                    = 110684,
};

#define ENTRY_WATER_ELEMENTAL   510

// Flamestrike - 2120
class spell_mage_flamestrike : public SpellScriptLoader
{
    public:
        spell_mage_flamestrike() : SpellScriptLoader("spell_mage_flamestrike") { }

        class spell_mage_flamestrike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_flamestrike_SpellScript);

            void HandleOnHit()
            {
                if (Player* caster = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (caster->GetTalentSpecialization() == SPEC_MAGE_FIRE)
                            caster->CastSpell(target, SPELL_MAGE_BLAST_WAVE_SNARE, true);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_flamestrike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_flamestrike_SpellScript();
        }
};

// Called by Arcane Blast - 30451
// Glyph of Slow - 86209
class spell_mage_glyph_of_slow : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_slow() : SpellScriptLoader("spell_mage_glyph_of_slow") { }

        class spell_mage_glyph_of_slow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_glyph_of_slow_SpellScript);

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (!caster->HasAura(SPELL_MAGE_GLYPH_OF_SLOW))
                        return;

                    if (Unit* target = GetHitUnit())
                    {
                        std::list<Unit*> targetList;
                        float radius = 50.0f;
                        bool found = false;

                        Trinity::NearestAttackableUnitInObjectRangeCheck u_check(caster, caster, radius);
                        Trinity::UnitListSearcher<Trinity::NearestAttackableUnitInObjectRangeCheck> searcher(caster, targetList, u_check);
                        caster->VisitNearbyObject(radius, searcher);

                        for (auto itr : targetList)
                            if (itr->HasAura(SPELL_MAGE_SLOW))
                                found = true;

                        if (found)
                            return;
                        else
                            caster->CastSpell(target, SPELL_MAGE_SLOW, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_glyph_of_slow_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_glyph_of_slow_SpellScript();
        }
};

// Called by Ice Block - 45438
// Glyph of Ice Block - 115723
class spell_mage_glyph_of_ice_block : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_ice_block() : SpellScriptLoader("spell_mage_glyph_of_ice_block") { }

        class spell_mage_glyph_of_ice_block_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_glyph_of_ice_block_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* _player = GetTarget()->ToPlayer())
                {
                    if (_player->HasAura(SPELL_MAGE_GLYPH_OF_ICE_BLOCK))
                    {
                        _player->CastSpell(_player, SPELL_MAGE_GLYPH_OF_ICE_BLOCK_FROST_NOVA, true);
                        _player->CastSpell(_player, SPELL_MAGE_GLYPH_OF_ICE_BLOCK_IMMUNITY, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_ice_block_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_glyph_of_ice_block_AuraScript();
        }
};

// 44457 - Living Bomb
// 112948 - Frost Bomb
// 114923 - Nether Tempest
class spell_mage_pyromaniac : public SpellScript
{
    PrepareSpellScript(spell_mage_pyromaniac);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_MAGE_PYROMANIAC_AURA))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_PYROMANIAC_DAMAGE_DONE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_pyromaniac::HandleHit);
    }
};

class CheckArcaneBarrageImpactPredicate
{
    public:
        CheckArcaneBarrageImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) { }

        bool operator()(Unit* target)
        {
            if (!_caster || !_mainTarget)
                return true;

            if (!_caster->IsValidAttackTarget(target))
                return true;

            if (!target->IsWithinLOSInMap(_caster))
                return true;

            if (!_caster->isInFront(target))
                return true;

            if (target->GetGUID() == _caster->GetGUID())
                return true;

            if (target->GetGUID() == _mainTarget->GetGUID())
                return true;

            return false;
        }

    private:
        Unit* _caster;
        Unit* _mainTarget;
};

// Slow - 31589
class spell_mage_slow : public SpellScriptLoader
{
    public:
        spell_mage_slow() : SpellScriptLoader("spell_mage_slow") { }

        class spell_mage_slow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_slow_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->GetTypeId() == TYPEID_PLAYER)
                        {
                            if (Aura *frostjaw = target->GetAura(SPELL_MAGE_SLOW, _player->GetGUID()))
                            {
                                // Only half time against players
                                frostjaw->SetDuration(frostjaw->GetMaxDuration() / 2);
                                frostjaw->SetMaxDuration(frostjaw->GetDuration());
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_slow_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_slow_SpellScript();
        }
};


class CheckNetherImpactPredicate
{
    public:
        CheckNetherImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) { }

        bool operator()(Unit* target)
        {
            if (!_caster || !_mainTarget)
                return true;

            if (!_caster->IsValidAttackTarget(target))
                return true;

            if (!target->IsWithinLOSInMap(_caster))
                return true;

            if (!_caster->isInFront(target))
                return true;

            if (target->GetGUID() == _caster->GetGUID())
                return true;

            if (target->GetGUID() == _mainTarget->GetGUID())
                return true;

            return false;
        }

    private:
        Unit* _caster;
        Unit* _mainTarget;
};

// Frostjaw - 102051
class spell_mage_frostjaw : public SpellScriptLoader
{
    public:
        spell_mage_frostjaw() : SpellScriptLoader("spell_mage_frostjaw") { }

        class spell_mage_frostjaw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_frostjaw_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->GetTypeId() == TYPEID_PLAYER)
                        {
                            if (Aura *frostjaw = target->GetAura(SPELL_MAGE_FROSTJAW, _player->GetGUID()))
                            {
                                // Only half time against players
                                frostjaw->SetDuration(frostjaw->GetMaxDuration() / 2);
                                frostjaw->SetMaxDuration(frostjaw->GetDuration());
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_frostjaw_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_frostjaw_SpellScript();
        }
};

// 11129 - Combustion
class spell_mage_combustion : public SpellScript
{
    PrepareSpellScript(spell_mage_combustion);

    void HandleHit()
    {
        Player* mage = GetCaster()->ToPlayer();
        Unit* target = GetHitUnit();
        if (!mage)
            return;

        mage->CastSpell(target, SPELL_MAGE_COMBUSTION_IMPACT, true);

        if (mage->HasSpellCooldown(SPELL_MAGE_INFERNO_BLAST))
        {
            mage->RemoveSpellCooldown(SPELL_MAGE_INFERNO_BLAST, true);
            mage->RemoveSpellCooldown(SPELL_MAGE_INFERNO_BLAST_IMPACT, true);
        }

        // 20% of ignite tick damage
        if (auto ignite = target->GetAuraEffect(SPELL_MAGE_IGNITE, EFFECT_0, mage->GetGUID()))
        {
            int32 damage = CalculatePct(ignite->GetAmount(), GetSpellInfo()->Effects[EFFECT_0].BasePoints);
            mage->CastCustomSpell(SPELL_MAGE_COMBUSTION_DOT, SPELLVALUE_BASE_POINT0, damage, target, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_combustion::HandleHit);
    }
};

// 108853 - Inferno Blast
class spell_mage_inferno_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_inferno_blast);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_INFERNO_BLAST_IMPACT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_inferno_blast::HandleHit);
    }
};

// 118280 - Inferno Blast
class spell_mage_inferno_blast_spread : public SpellScript
{
    PrepareSpellScript(spell_mage_inferno_blast_spread);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (GetExplTargetUnit())
            targets.remove(GetExplTargetUnit());

        targets.remove_if([=](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            return !unit || unit->HasBreakableByDamageCrowdControlAura();
        });

        size_t maxTargets = GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_INFERNO_BLAST) ? 4 : 3;

        if (targets.size() > maxTargets)
            Trinity::Containers::RandomResizeList(targets, maxTargets);
    }

    void HandleHit(SpellEffIndex)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetExplTargetUnit())
        {
            for (auto&& itr : unitTarget->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
            {
                if ((itr->GetCasterGUID() != caster->GetGUID())
                    || (itr->GetSpellInfo()->Id != SPELL_MAGE_IGNITE
                        && itr->GetSpellInfo()->Id != SPELL_MAGE_PYROBLAST
                        && itr->GetSpellInfo()->Id != SPELL_MAGE_COMBUSTION_DOT))
                    continue;

                caster->AddAuraForTarget(itr->GetBase(), GetHitUnit());
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_inferno_blast_spread::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_inferno_blast_spread::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Arcane Brillance - 1459
class spell_mage_arcane_brilliance : public SpellScriptLoader
{
    public:
        spell_mage_arcane_brilliance() : SpellScriptLoader("spell_mage_arcane_brilliance") { }

        class spell_mage_arcane_brilliance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_arcane_brilliance_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    _player->AddAura(SPELL_MAGE_ARCANE_BRILLIANCE, _player);

                    std::list<Unit*> memberList;
                    _player->GetPartyMembers(memberList);

                    for (auto itr : memberList)
                        _player->AddAura(SPELL_MAGE_ARCANE_BRILLIANCE, itr);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mage_arcane_brilliance_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_arcane_brilliance_SpellScript();
        }
};

// Cold Snap - 11958
class spell_mage_cold_snap : public SpellScriptLoader
{
    public:
        spell_mage_cold_snap() : SpellScriptLoader("spell_mage_cold_snap") { }

        class spell_mage_cold_snap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_cold_snap_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    // Resets cooldown of Ice Block, Frost Nova and Cone of Cold
                    player->RemoveSpellCooldown(SPELL_MAGE_ICE_BLOCK, true);
                    player->RemoveSpellCooldown(SPELL_MAGE_FROST_NOVA, true);
                    player->RemoveSpellCooldown(SPELL_MAGE_CONE_OF_COLD, true);
                }
            }

            void Register() override
            {
                // add dummy effect spell handler to Cold Snap
                OnEffectHit += SpellEffectFn(spell_mage_cold_snap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_cold_snap_SpellScript();
        }
};

class spell_mage_incanters_absorbtion_base_AuraScript : public AuraScript
{
    public:
        enum Spells
        {
            SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED = 44413,
            SPELL_MAGE_INCANTERS_ABSORBTION_R1 = 44394,
        };

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return sSpellMgr->GetSpellInfo(SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED)
                && sSpellMgr->GetSpellInfo(SPELL_MAGE_INCANTERS_ABSORBTION_R1);
        }

        void Trigger(AuraEffect *aurEff, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
        {
            Unit* target = GetTarget();

            if (AuraEffect *talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_MAGE_INCANTERS_ABSORBTION_R1, EFFECT_0))
            {
                int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
                target->CastCustomSpell(target, SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
            }
        }
};

// Incanter's Absorption
class spell_mage_incanters_absorbtion_absorb : public SpellScriptLoader
{
    public:
        spell_mage_incanters_absorbtion_absorb() : SpellScriptLoader("spell_mage_incanters_absorbtion_absorb") { }

        class spell_mage_incanters_absorbtion_absorb_AuraScript : public spell_mage_incanters_absorbtion_base_AuraScript
        {
            PrepareAuraScript(spell_mage_incanters_absorbtion_absorb_AuraScript);

            void Register() override
            {
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_incanters_absorbtion_absorb_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_incanters_absorbtion_absorb_AuraScript();
        }
};

// Incanter's Absorption
class spell_mage_incanters_absorbtion_manashield : public SpellScriptLoader
{
    public:
        spell_mage_incanters_absorbtion_manashield() : SpellScriptLoader("spell_mage_incanters_absorbtion_manashield") { }

        class spell_mage_incanters_absorbtion_manashield_AuraScript : public spell_mage_incanters_absorbtion_base_AuraScript
        {
            PrepareAuraScript(spell_mage_incanters_absorbtion_manashield_AuraScript);

            void Register() override
            {
                 AfterEffectManaShield += AuraEffectManaShieldFn(spell_mage_incanters_absorbtion_manashield_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_incanters_absorbtion_manashield_AuraScript();
        }
};

// 115610 - Temporal Shield
class spell_mage_temporal_shield : public AuraScript
{
    PrepareAuraScript(spell_mage_temporal_shield);

    uint32 damageTaken = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (uint32 damage = eventInfo.GetDamageInfo()->GetDamage())
            if (!GetUnitOwner()->IsFriendlyTo(eventInfo.GetActor()))
                damageTaken += damage;
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (damageTaken)
        {
            Unit* mage = GetUnitOwner();

            int32 heal = CalculatePct(damageTaken, GetSpellInfo()->Effects[EFFECT_0].CalcValue(mage));
            heal *= mage->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_PCT);
            if (mage->GetMap()->IsBattlegroundOrArena() && !mage->HasAura(134735))
                AddPct(heal, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));

            mage->CastCustomSpell(SPELL_MAGE_TEMPORAL_SHIELD_HEAL, SPELLVALUE_BASE_POINT0, heal / 3, mage, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_temporal_shield::HandleProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_temporal_shield::HandleRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Flameglow - 140468
class spell_mage_flameglow : public SpellScriptLoader
{
    public:
        spell_mage_flameglow() : SpellScriptLoader("spell_mage_flameglow") { }

        class spell_mage_flameglow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_flameglow_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect * /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();
                if (target->GetTypeId() != TYPEID_PLAYER)
                    return;

                int32 bonusPct = GetAura()->GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                int32 damagePct = GetAura()->GetSpellInfo()->Effects[EFFECT_2].BasePoints;
                // Calculate %s
                uint32 spellAbsorb = CalculatePct(target->ToPlayer()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE), bonusPct);
                uint32 absorbLimit  = CalculatePct(dmgInfo.GetDamage(), damagePct);

                absorbAmount = std::min(spellAbsorb, absorbLimit);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_flameglow_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_flameglow_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_flameglow_AuraScript();
        }
};

// 42208 - Blizzard
/// Updated 4.3.4
class spell_mage_blizzard : public SpellScriptLoader
{
    public:
        spell_mage_blizzard() : SpellScriptLoader("spell_mage_blizzard") { }

        class spell_mage_blizzard_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_blizzard_SpellScript);

            enum
            {
                SPELL_MAGE_BLIZZARD_SLOW = 12486
            };

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MAGE_BLIZZARD_SLOW))
                    return false;
                return true;
            }

            void AddChillEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                    caster->CastSpell(unitTarget, SPELL_MAGE_BLIZZARD_SLOW, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mage_blizzard_SpellScript::AddChillEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_blizzard_SpellScript();
        }
};

enum GlyphOfIllusion
{
    MAGE_GLYPH_OF_ILLUSION_ACTIVE = 131784
};

class spell_mage_glyph_of_illusion : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_illusion() : SpellScriptLoader("spell_mage_glyph_of_illusion") { }

        class spell_mage_glyph_of_illusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_glyph_of_illusion_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster()->ToPlayer())
                    GetCaster()->ToPlayer()->LearnSpell(MAGE_GLYPH_OF_ILLUSION_ACTIVE, false);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster()->ToPlayer())
                    GetCaster()->ToPlayer()->RemoveSpell(MAGE_GLYPH_OF_ILLUSION_ACTIVE);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_glyph_of_illusion_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_illusion_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_glyph_of_illusion_AuraScript();
        }
};

enum GlyphOfConjureFamiliar
{
    MAGE_GLYPH_OF_CONJURE_FAMILIAR_ACTIVE = 126578
};

class spell_mage_glyph_of_conjure_familiar : public SpellScriptLoader
{
    public:
        spell_mage_glyph_of_conjure_familiar() : SpellScriptLoader("spell_mage_glyph_of_conjure_familiar") { }

        class spell_mage_glyph_of_conjure_familiar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mage_glyph_of_conjure_familiar_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster()->ToPlayer())
                    GetCaster()->ToPlayer()->LearnSpell(MAGE_GLYPH_OF_CONJURE_FAMILIAR_ACTIVE, false);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster()->ToPlayer())
                    GetCaster()->ToPlayer()->RemoveSpell(MAGE_GLYPH_OF_CONJURE_FAMILIAR_ACTIVE);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_mage_glyph_of_conjure_familiar_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_conjure_familiar_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mage_glyph_of_conjure_familiar_AuraScript();
        }
};

class spell_mage_illusion : public SpellScriptLoader
{
    public:
        spell_mage_illusion() : SpellScriptLoader("spell_mage_illusion") { }

        class spell_mage_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_illusion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;
                
                target = Trinity::Containers::SelectRandomContainerElement(targets)->ToUnit();
            }

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = target)
                    target->CastSpell(caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            private:
                Unit* target;

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_illusion_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_mage_illusion_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mage_illusion_SpellScript();
        }
};

// 55342 - Mirror Image
// 110621 - Mirror Image (symbiosis)
class spell_mage_mirror_image : public SpellScript
{
    PrepareSpellScript(spell_mage_mirror_image);

    void HandleDummy(SpellEffIndex)
    {
        Unit* caster = GetCaster();

        uint32 spellId = GetSpellInfo()->Id == 110621 ? SPELL_DRUID_MIRROR_IMAGES : SPELL_MAGE_SUMMON_IMAGES_FROST;

        if (Player* player = caster->ToPlayer())
        {
            if (GetCaster()->HasSpell(SPELL_MAGE_GLYPH_OF_MIRROR_IMAGE))
            {
                switch (player->GetTalentSpecialization())
                {
                    case SPEC_MAGE_ARCANE:
                        spellId = SPELL_MAGE_SUMMON_IMAGES_ARCANE;
                        break;
                    case SPEC_MAGE_FIRE:
                        spellId = SPELL_MAGE_SUMMON_IMAGES_FIRE;
                        break;
                    case SPEC_MAGE_FROST:
                        spellId = SPELL_MAGE_SUMMON_IMAGES_FROST;
                        break;
                }
            }
        }

        caster->CastSpell(caster, spellId, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_mirror_image::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 131618 - 2P mage pvp
class spell_mage_2p_pvp : public AuraScript
{
    PrepareAuraScript(spell_mage_2p_pvp);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.TriggeredBySpell() && eventInfo.TriggeredBySpell()->IsSuccessfulInterrupt();
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->ModifySpellCooldown(SPELL_MAGE_COUNTERSPELL, eff->GetAmount());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_2p_pvp::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_2p_pvp::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 79684 Arcane Missiles (proc aura)
class spell_mage_arcane_missiles_proc : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_missiles_proc);

    bool CheckProc(ProcEventInfo& proc)
    {
        // Dunno why it has SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED, may be some spells have to be filtered manually
        if (!proc.TriggeredBySpell() || proc.TriggeredBySpell()->IsTriggered())
            return false;

        // No targets
        if (proc.TriggeredBySpell()->GetUnitTargets().empty())
            return false;

        // Damaging spells
        if (proc.GetSpellInfo()->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE) || proc.GetSpellInfo()->HasAura(SPELL_AURA_PERIODIC_DAMAGE))
            return true;

        if (proc.GetSpellInfo()->Id == 112948) // Frost Bomb
            return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_arcane_missiles_proc::CheckProc);
    }
};

// 79683 - Arcane Missiles!
class spell_mage_arcane_missiles_proc_visual : public visual_stacks_controller
{
    PrepareAuraScript(spell_mage_arcane_missiles_proc_visual);

    void Register() override
    {
        Initialize(SPELL_MAGE_ARCANE_MISSILES_DOUBLE);
        OnEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles_proc_visual::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_arcane_missiles_proc_visual::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 44544 - Fingers of Frost 
class spell_mage_fingers_of_frost : public visual_stacks_controller
{
    PrepareAuraScript(spell_mage_fingers_of_frost);

    void Register() override
    {
        Initialize(SPELL_MAGE_FINGERS_OF_FROST_DOUBLE);
        OnEffectApply += AuraEffectApplyFn(spell_mage_fingers_of_frost::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectApplyFn(spell_mage_fingers_of_frost::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 148023 - Icicles (periodic aura to trigger all stored icicles)
class spell_mastery_icicles_periodic : public AuraScript
{
    PrepareAuraScript(spell_mastery_icicles_periodic);

    ObjectGuid targetGuid = ObjectGuid::Empty;

public:
    void SetTarget(ObjectGuid target)
    {
        targetGuid = target;
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* mage = GetUnitOwner();
        Unit* target = ObjectAccessor::GetUnit(*mage, targetGuid);
        if (target && target->IsAlive())
        {
            for (int32 i = 0; i < 5; ++i)
            {
                if (AuraEffect* icicle = mage->GetAuraEffect(SPELL_MAGE_ICICLE_STORE + i, EFFECT_0))
                {
                    int32 amount = icicle->GetAmount();
                    mage->CastCustomSpell(target, SPELL_MAGE_ICICILE_VISUAL + i, &amount, nullptr, nullptr, true);
                    mage->CastCustomSpell(target, SPELL_MAGE_ICICLE_DAMAGE, &amount, nullptr, nullptr, true);
                    mage->RemoveAurasDueToSpell(SPELL_MAGE_ICICLE_STORE + i);
                    return;
                }
            }
        }
        SetDuration(0); // No more icicles or target died, remove aura.
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mastery_icicles_periodic::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 76613 - Mastery: Icicles
class spell_mage_mastery_icicles : public AuraScript
{
    PrepareAuraScript(spell_mage_mastery_icicles);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetProcTriggerContext()->SpellDamage)
            return;
        if (!eventInfo.GetActionTarget())
            return;

        float damage = eventInfo.GetDamageInfo()->GetDamage() + eventInfo.GetProcTriggerContext()->SpellDamage->absorb;
        if (!damage)
            return;

        Player* target = eventInfo.GetActionTarget()->GetAffectingPlayer();
        Player* mage = GetUnitOwner()->ToPlayer();
        if (target)
        {
            damage /= (1.0f + target->GetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT) / 100.0f);
            if (mage->GetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE))
                damage /= (1.0f + mage->GetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE) / 100.f);
        }
        
        damage = CalculatePct(damage, mastery->GetFloatAmount());
        for (int32 i = 0; i < 5; ++i)
        {
            if (mage->HasAura(SPELL_MAGE_ICICLE_STORE + i))
            {
                // If all icicles are stored, fire last and replace it with new
                if (i != 4)
                    continue;

                if (AuraEffect const* icicle = mage->GetAuraEffect(SPELL_MAGE_ICICLE_STORE, EFFECT_0))
                {
                    int32 amount = icicle->GetAmount();
                    mage->CastSpell(eventInfo.GetActionTarget(), SPELL_MAGE_ICICILE_VISUAL, true);
                    mage->CastCustomSpell(SPELL_MAGE_ICICLE_DAMAGE, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetActionTarget(), true);
                    mage->RemoveAurasDueToSpell(SPELL_MAGE_ICICLE_STORE);
                    mage->CastCustomSpell(SPELL_MAGE_ICICLE_STORE, SPELLVALUE_BASE_POINT0, damage, mage, true);
                    break;
                }
            }
            else
            {
                mage->CastCustomSpell(SPELL_MAGE_ICICLE_STORE + i, SPELLVALUE_BASE_POINT0, damage, mage, true);
                break;
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_mastery_icicles::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 30455 - Ice Lance
class spell_mage_ice_lance : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_lance);

    bool m_fingersOfFrost = false;

    void HandleCast()
    {
        Aura* fingers = GetCaster()->GetAura(SPELL_MAGE_FINGERS_OF_FROST_EFFECT);
        if (fingers)
        {
            m_fingersOfFrost = true;
            fingers->ModStackAmount(-1);
        }

        if (GetCaster()->HasAura(SPELL_MAGE_MASTERY_ICICLES))
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_MAGE_ICICILE_TRIGGER_AURA, true);
            if (Aura* aura = GetCaster()->GetAura(SPELL_MAGE_ICICILE_TRIGGER_AURA))
                if (auto* script = dynamic_cast<spell_mastery_icicles_periodic*>(aura->GetScriptByName("spell_mastery_icicles_periodic")))
                    script->SetTarget(GetExplTargetUnit()->GetGUID());
        }
    }

    void HandleHit()
    {
        Unit* caster = GetCaster();
        int32 damage = GetHitDamage();

        if (m_fingersOfFrost || GetHitUnit()->HasAuraState(AURA_STATE_FROZEN, GetSpellInfo(), caster))
            damage *= 4.0f;

        if (m_fingersOfFrost)
            damage *= 1.25f;

        SetHitDamage(damage);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_ice_lance::HandleCast);
        OnHit += SpellHitFn(spell_mage_ice_lance::HandleHit);
    }
};

// 44572 - Deep Freeze
class spell_mage_deep_freez : public SpellScript
{
    PrepareSpellScript(spell_mage_deep_freez);

    void HandleHit()
    {
        Unit* caster = GetCaster();
        if (Aura* aura = caster->GetAura(SPELL_MAGE_FINGERS_OF_FROST_EFFECT))
            aura->ModStackAmount(-1);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_deep_freez::HandleHit);
    }
};

// Called by Frostbolt  (116), Frostfire Bolt (44614) and Blizzard (42208)
// 126084 - Fingers of Frost (proc)
class spell_mage_fingers_of_frost_proc : public SpellScript
{
    PrepareSpellScript(spell_mage_fingers_of_frost_proc);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        int32 chance = 0;
        if (caster->HasSpell(SPELL_MAGE_FINGERS_OF_FROST))
        {
            SpellInfo const* info = sSpellMgr->GetSpellInfo(SPELL_MAGE_FINGERS_OF_FROST);
            switch (GetSpellInfo()->Id)
            {
                case 116:
                    chance = info->Effects[0].BasePoints;
                    if (AuraEffect const* mod = caster->GetAuraEffect(SPELL_MAGE_T15_4P_BONUS, EFFECT_2))
                        chance += mod->GetAmount();
                    break;
                case 44614:
                    chance = info->Effects[0].BasePoints;
                    break;
                case 42208:
                    chance = info->Effects[1].BasePoints;
                    break;
                default:
                    return;
            }
        }

        if (roll_chance_i(chance))
            caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_EFFECT, true); // Main fingers of frost spell, also left side visual
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_fingers_of_frost_proc::HandleOnHit);
    }
};

// 86949 - Cauterize
class spell_mage_cauterize : public AuraScript
{
    PrepareAuraScript(spell_mage_cauterize);

    void Absorb(AuraEffect* eff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        absorbAmount = 0;

        Unit* target = GetTarget();

        if (dmgInfo.GetDamage() < target->GetHealth())
            return;

        if (target->HasAura(SPELL_MAGE_CAUTERIZE_MARKER))
            return;

        int bp1 = target->CountPctFromMaxHealth(GetEffect(EFFECT_1)->GetAmount());
        target->CastCustomSpell(target, SPELL_MAGE_CAUTERIZE, nullptr, &bp1, nullptr, true);
        target->CastSpell(target, SPELL_MAGE_CAUTERIZE_MARKER, true);

        absorbAmount = dmgInfo.GetDamage();
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize::Absorb, EFFECT_0);
    }
};

class spell_mage_evocation_SpellScript : public SpellScript
{
    PrepareSpellScript(spell_mage_evocation_SpellScript);

    void HandleOnHit()
    {
        auto caster = GetCaster();
        caster->EnergizeBySpell(caster, GetSpellInfo()->Id, int32(caster->GetMaxPower(POWER_MANA) * 0.15), POWER_MANA);
        caster->RemoveAurasDueToSpell(SPELL_MAGE_ARCANE_CHARGE);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_evocation_SpellScript::HandleOnHit);
    }
};

// 12051 - Evocation
class spell_mage_evocation : public AuraScript
{
    PrepareAuraScript(spell_mage_evocation);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (!caster->HasAura(SPELL_MAGE_GLYPH_OF_EVOCATION) || caster->HasAura(SPELL_MAGE_INVOCATION))
            if (AuraEffect* aurEff = GetAura()->GetEffect(EFFECT_1))
                aurEff->ChangeAmount(0);
    }

    void AfterRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        caster->RemoveAurasDueToSpell(SPELL_MAGE_ARCANE_CHARGE);
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_EXPIRE)
            return;

        if (caster->HasAura(SPELL_MAGE_INVOCATION))
        {
            caster->CastSpell(caster, SPELL_MAGE_INVOKERS_ENERGY, true);

            if (caster->HasAura(SPELL_MAGE_GLYPH_OF_EVOCATION))
            {
                int32 healthGain = caster->CountPctFromMaxHealth(10);
                auto spellInfo = sSpellMgr->GetSpellInfo(12051);
                healthGain = caster->SpellHealingBonusDone(caster, spellInfo, EFFECT_0, healthGain, HEAL);
                healthGain = caster->SpellHealingBonusTaken(caster, spellInfo, EFFECT_0, healthGain, HEAL);
                caster->HealBySpell(caster, spellInfo, healthGain, false);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_evocation::HandleApply, EFFECT_1, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_evocation::AfterRemove, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 36032 - Arcane Charge
class spell_mage_arcane_charge : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_charge);

    void CalculateModifier(AuraEffect const*, SpellModifier*& mod)
    {
        mod->value *= mod->charges;
    }

    void ChargeConsumed(Spell* bySpell)
    {
        if (bySpell->GetSpellInfo()->Id == SPELL_MAGE_ARCANE_MISSILES_CHANNEL)
            PreventDefaultAction();
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_mage_arcane_charge::CalculateModifier, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER);
        OnDropModCharge += AuraDropModChargeFn(spell_mage_arcane_charge::ChargeConsumed);
    }
};

// 5143 - Arcane Missiles
class spell_mage_arcane_missiles : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_missiles);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect const* bonus = GetUnitOwner()->GetAuraEffect(SPELL_MAGE_T16_4P_BONUS, EFFECT_0))
            if (roll_chance_i(bonus->GetAmount()))
                return;

        if (Aura* auraState = GetUnitOwner()->GetAura(SPELL_MAGE_ARCANE_MISSILES))
            auraState->ModStackAmount(-1);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* mage = GetUnitOwner();

        mage->CastSpell(mage, SPELL_MAGE_ARCANE_CHARGE, true);
        // Channel don't finished yet. Do it at next update or player will see interrupt error.
        mage->m_Events.Schedule(1, [=]
        {
            if (Aura* iceFloes = mage->GetAura(SPELL_MAGE_ICE_FLOES))
                iceFloes->ModStackAmount(-1);
        });
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_mage_arcane_missiles::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1449 - Arcane Explosion
class spell_mage_arcane_explosion : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_explosion)

    bool canGenerateCharge;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        canGenerateCharge = !targets.empty();
    }

    void HandleAfterCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster || caster->GetTalentSpecialization() != SPEC_MAGE_ARCANE)
            return;

        if (Aura* aura = caster->GetAura(SPELL_MAGE_ARCANE_CHARGE))
            aura->RefreshDuration();

        if (canGenerateCharge && roll_chance_i(GetSpellInfo()->Effects[EFFECT_1].BasePoints))
            caster->CastSpell(caster, SPELL_MAGE_ARCANE_CHARGE, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_arcane_explosion::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        AfterCast += SpellCastFn(spell_mage_arcane_explosion::HandleAfterCast);
    }
};

// 12846 - Mastery: Ignite
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        Unit* caster  = GetCaster();
        ObjectGuid targetGuid  = eventInfo.GetProcTarget()->GetGUID();
        if (mastery->GetFloatAmount() <= 0.0f)
            return;

        int32 damage = CalculatePct(eventInfo.GetDamage(), mastery->GetFloatAmount()) / 2.0f; // tick count
        if (!damage)
            return;

        caster->m_Events.Schedule(1000, [=]
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGuid))
                caster->CastCustomSpell(SPELL_MAGE_IGNITE, SPELLVALUE_BASE_POINT0, damage, target, true);
        });
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 116011 - Rune of Power
class spell_mage_rune_of_power : public SpellScript
{
    PrepareSpellScript(spell_mage_rune_of_power);

    void RemoveOldRune()
    {
        std::list<AreaTrigger*> runes;
        GetCaster()->GetAreaTriggerList(runes, SPELL_MAGE_RUNE_OF_POWER);
        while (runes.size() > 1)
        {
            runes.front()->Remove();
            runes.pop_front();
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_rune_of_power::RemoveOldRune);
    }
};

class sat_mage_rune_of_power : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* target) override
    {
        return m_caster == target;
    }

    void OnTriggeringApply(WorldObject* target) override
    {
        if (Player* player = target->ToPlayer())
            player->CastSpell(player, SPELL_MAGE_RUNE_OF_POWER_AURA, true);
    }

    void RemoveAuraIfNeed(Player* mage)
    {
        std::list<AreaTrigger*> runes;
        mage->GetAreaTriggerList(runes, SPELL_MAGE_RUNE_OF_POWER);
        for (auto&& it : runes)
            if (it != GetTarget())
                if (it->GetExactDist(mage) < GetRange())
                    return;
        mage->RemoveAura(SPELL_MAGE_RUNE_OF_POWER_AURA);
    }

    void OnTriggeringRemove(WorldObject* target) override
    {
        if (Player* player = target->ToPlayer())
            RemoveAuraIfNeed(player);
    }
};

// 114924 - Nether Tempest
class spell_mage_nether_tempest_selector : public SpellScript
{
    PrepareSpellScript(spell_mage_nether_tempest_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetUnit());
        targets.remove_if([=](WorldObject* target)
        {
            if (!target->ToUnit() || !target->ToUnit()->IsInCombat())
                return true;

            if (target->GetTypeId() == TYPEID_PLAYER)
                return false;

            if (!target->ToUnit()->GetThreatManager().getThreat(GetCaster()))
                return true;
            return false;
        });
        Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleHit(SpellEffIndex)
    {
        if (GetExplTargetUnit())
        {
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_NETHER_TEMPEST_DIRECT_DAMAGE, true);
            GetExplTargetUnit()->CastSpell(GetHitUnit(), SPELL_MAGE_NETHER_TEMPEST_MISSILE, true);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_nether_tempest_selector::SelectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_nether_tempest_selector::HandleHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 114923 - Nether Tempest
class spell_mage_nether_tempest : public AuraScript
{
    PrepareAuraScript(spell_mage_nether_tempest);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            caster->BindAura(GetAura());
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(GetTarget(), SPELL_MAGE_NETHER_TEMPEST_SELECTOR, true);

            if (auto list = caster->GetBoundAurasBySpellId(GetId()))
                if (GetAura() == list->back() && roll_chance_i(10) && caster->HasAura(SPELL_MAGE_BRAIN_FREEZE))
                    caster->CastSpell(caster, SPELL_MAGE_BRAIN_FREEZE_TRIGGERED, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_nether_tempest::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_nether_tempest::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 44457 - Living Bomb
class spell_mage_living_bomb : public AuraScript
{
    PrepareAuraScript(spell_mage_living_bomb);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetDuration() < 3100)   // Just little early than second to last tick because it will be updated at current cycle
            Explode(eff);
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes)
    {
       AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
       if (removeMode == AURA_REMOVE_BY_DEATH || removeMode == AURA_REMOVE_BY_EXPIRE)
            Explode(eff);
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
            HandleBrainFreeze(caster);
    }

    void Explode(AuraEffect const* eff)
    {
        if (Unit* caster = GetCaster())
        {
            CustomSpellValues ctx;
            ctx.SetMultiplier(eff->GetTotalTicks());
            caster->CastCustomSpell(SPELL_MAGE_LIVING_BOMB_TRIGGERED, ctx, GetTarget(), TRIGGERED_FULL_MASK);
            HandleBrainFreeze(caster);
        }
    }

    void HandleBrainFreeze(Unit* caster)
    {
        if (auto list = caster->GetBoundAurasBySpellId(GetId()))
            if (GetAura() == list->back() && roll_chance_i(25) && caster->HasAura(SPELL_MAGE_BRAIN_FREEZE))
                caster->CastSpell(caster, SPELL_MAGE_BRAIN_FREEZE_TRIGGERED, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_living_bomb::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_living_bomb::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 44448 - Pyroblast Clearcasting Driver
class spell_mage_pyroblast_crearcasting : public AuraScript
{
    PrepareAuraScript(spell_mage_pyroblast_crearcasting);

    bool m_delayedRemove = false;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return;
        if (!eventInfo.TriggeredBySpell()->m_cast_count)
            return;
        if (!eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget())
            return;
        if (!(eventInfo.GetDamageInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE))
            return;

        Unit* mage = GetUnitOwner();
        if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
        {
            m_delayedRemove = false;
            auto fn = [=]()
            {

                Aura* heatingUp = mage->GetAura(SPELL_MAGE_HEATING_UP);
                Aura* pyroblast = mage->GetAura(SPELL_MAGE_PYROBLAST_CLEARCASTING);

                if (!heatingUp)
                {
                    mage->CastSpell(mage, SPELL_MAGE_HEATING_UP, true);
                }
                else
                {
                    mage->CastSpell(mage, SPELL_MAGE_PYROBLAST_CLEARCASTING, true);
                    mage->RemoveAura(heatingUp);
                }
            };

            if (eventInfo.GetSpellInfo()->Speed > 0)
                fn();
            else
                mage->Schedule(Milliseconds(250), fn);
        }
        else
        {
            m_delayedRemove = true;
            Aura* thisAura = GetAura();
            uint32 spellId = thisAura->GetSpellInfo()->Id;
            // -- If you have Heating Up, and get a non-crit, then Heating Up gets removed, as expected.
            // -- However, if the non - crit came from a spell with a travel time, or from Scorch (which is supposed to be able to replace Fireball rotationally while moving),
            // then we delay the removal of Heating Up by 0.25 sec.
            // 300ms used only to be more lag-tolerance
            mage->Schedule(Milliseconds(300), [=]()
            {
                if (mage->GetAura(spellId) == thisAura) // Generally impossible, but ensures what we are use this script
                {
                    if (!m_delayedRemove)               // Pyroblast casted, 
                        return;
                    m_delayedRemove = false;
                }
                mage->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);
            });
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_pyroblast_crearcasting::HandleProc);
    }
};

// 11366 - Pyroblast
class spell_mage_pyroblast : public SpellScript
{
    PrepareSpellScript(spell_mage_pyroblast);

    bool amplify = false;

    void HandleCast()
    {
        if (Aura* pyroblast = GetCaster()->GetAura(SPELL_MAGE_PYROBLAST_CLEARCASTING))
        {
            auto it = GetSpell()->m_appliedMods.find(pyroblast);
            if (it == GetSpell()->m_appliedMods.end())
                return;

            amplify = true;
            if (AuraEffect* presence = GetCaster()->GetAuraEffect(SPELL_MAGE_PRESENCE_OF_MIND, EFFECT_0))
            {
                auto it = GetSpell()->m_appliedMods.find(presence->GetBase());
                if (it != GetSpell()->m_appliedMods.end())
                {
                    presence->GetSpellModifier()->charges = 1;
                    GetSpell()->m_appliedMods.erase(it);
                }
            }
            GetCaster()->RemoveAura(pyroblast);
        }
    }

    void HandleHit()
    {
        if (amplify)
            SetHitDamage(GetHitDamage() * 1.25f);
    }

    void HandleAfterHit()
    {
        if (amplify)
            if (Aura* dot = GetHitAura())
                dot->GetEffect(EFFECT_1)->ChangeAmount(dot->GetEffect(EFFECT_1)->GetAmount() * 1.25, false);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_pyroblast::HandleCast);
        OnHit += SpellHitFn(spell_mage_pyroblast::HandleHit);
        AfterHit += SpellHitFn(spell_mage_pyroblast::HandleAfterHit);
    }
};

// 127140 - Alter Time (overridden)
class spell_mage_alter_time_overridden : public SpellScript
{
    PrepareSpellScript(spell_mage_alter_time_overridden);

    void HandleAfterCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->RemoveAura(SPELL_MAGE_ALTER_TIME);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_alter_time_overridden::HandleAfterCast);
    }
};

// 110909 - Alter Time
class spell_mage_alter_time : public AuraScript
{
    PrepareAuraScript(spell_mage_alter_time);

    friend class spell_mage_alter_time_cast;

    std::vector<AuraData> auras;
    uint32 health = 0;
    uint32 mana = 0;
    WorldLocation pos;

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (pos.GetMapId() == MAPID_INVALID)
            return;

        Player* player = GetTarget()->ToPlayer();
        if (!player)
            return;

        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_EXPIRE && removeMode != AURA_REMOVE_BY_DEFAULT)
            return;

        std::list<TempSummon*> pastSelf;
        player->GetSummons(pastSelf, NPC_PAST_SELF);
        bool found = false;
        for (auto&& it : pastSelf)
            if (player->IsWithinDistInMap(it, 100))
                found = true;

        if (!found)
            return;

        for (auto&& it : pastSelf)
            it->DespawnOrUnsummon();

        std::set<Aura*> applied;

        for (auto&& it : auras)
        {
            Aura* aura = player->GetAura(it.Id, it.CasterGuid);
            if (!aura)
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(it.Id);
                if (!spellInfo)
                    continue;

                if (spellInfo->TalentId)
                    if (!player->HasTalent(spellInfo->Id, player->GetActiveSpec()))
                        continue;
                if (spellInfo->Id == SPELL_MAGE_INVOKERS_ENERGY && !player->HasTalent(SPELL_MAGE_INVOCATION, player->GetActiveSpec()))
                    continue;

                aura = Aura::TryRefreshStackOrCreate(spellInfo, it.EffectMask, player, ObjectAccessor::GetUnit(*player, it.CasterGuid), it.BaseAmount, nullptr, it.CasterGuid);
                if (aura)
                    aura->ApplyForTargets();
            }
            if (aura)
            {
                aura->LoadParameters(it);
                applied.insert(aura);
            }
        }

        for (auto it = player->GetAppliedAuras().begin(); it != player->GetAppliedAuras().end();)
        {
            Aura* aura = it->second->GetBase();
            ++it;

            if (aura->IsRemoved())
                continue;

            if (!(aura->GetSpellInfo()->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE) && (aura->IsPassive() || aura->GetMaxDuration() == -1))
                continue;

            auto it2 = applied.find(aura);
            if (it2 == applied.end())
            {
                aura->Remove();
                it = player->GetAppliedAuras().begin();
            }
        }
        player->SetHealth(health);
        player->SetPower(POWER_MANA, mana);
        player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_alter_time::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108978 - Alter Time
class spell_mage_alter_time_cast : public SpellScript
{
    PrepareSpellScript(spell_mage_alter_time_cast);

    std::vector<AuraData> auras;
    uint32 health = 0;
    uint32 mana = 0;
    WorldLocation pos;

    bool Load() override
    {
        health = GetCaster()->GetHealth();
        mana = GetCaster()->GetPower(POWER_MANA);
        pos = GetCaster()->GetWorldLocation();
        pos.m_mapId = GetCaster()->GetMapId();
        for (auto&& it : GetCaster()->GetAppliedAuras())
        {
            Aura* aura = it.second->GetBase();
            if (aura->GetSpellInfo()->Id == 110909)
                continue;

            if (!(aura->GetSpellInfo()->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE) && (aura->IsPassive() || aura->GetMaxDuration() == -1))
                continue;

            auras.emplace_back();
            aura->SaveParameters(auras.back());
        }
        return true;
    }

    void HandleHit()
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_MAGE_ALTER_TIME))
        {
            if (auto script = dynamic_cast<spell_mage_alter_time*>(aura->GetScriptByName("spell_mage_alter_time")))
            {
                script->auras = auras;
                script->health = health;
                script->mana = mana;
                script->pos = pos;
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_alter_time_cast::HandleHit);
    }
};

// 58542 - Past Self. Summoned by Alter Time (108978)
struct npc_mage_past_self : public SpellDummyAI
{
    npc_mage_past_self(Creature* creature)
        : SpellDummyAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        summoner->CastSpell(me, 102284, true);
        summoner->CastSpell(me, 102288, true);
        me->CastSpell(me, 82978, true);
        me->CastSpell(me, 110951, true);
    }
};

// 136511 - Ring of Frost (Periodic)
class spell_mage_ring_of_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost);

    Position dest;

    bool Load() override
    {
        std::list<TempSummon*> summons;
        GetUnitOwner()->GetSummons(summons, 44199);
        if (summons.empty())
            return false;
        dest = summons.back()->GetPosition();
        return true;
    }

    void HandlePeriodic(AuraEffect const* eff)
    {
        PreventDefaultAction(); // 0 in tirgger, prevent spam

        GetUnitOwner()->CastSpell(dest, SPELL_MAGE_RING_OF_FROST_FREEZE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ring_of_frost::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 82691 - Ring of Frost (Freeze effect spell)
class spell_mage_ring_of_frost_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_freeze);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject const* target)
        {
            float const inRadius  = 4.0f;  // Radius 1 / 2
            float const outRadius = 7.25f; // Radius 1 + Radius 0 / 2

            Unit const* unit = target->ToUnit();
            if (!unit)
                return true;
            if (unit->HasAura(SPELL_MAGE_RING_OF_FROST_FREEZE))
                return true;
            auto dest = GetExplTargetDest();
            if (unit->GetExactDist2d(GetExplTargetDest()) > outRadius || unit->GetExactDist2d(GetExplTargetDest()) < inRadius)
                return true;
            if (std::abs(unit->GetPositionZ() - dest->GetPositionZ()) > 5.0f)
                return true;

            if (unit->HasAura(SPELL_MAGE_RING_OF_FROST_MARKER))
                return true;

            if (!unit->IsWithinLOS(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ()))
                return true;

            return false;
        });
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_RING_OF_FROST_MARKER, true);
        if (Aura* aura = GetHitUnit()->GetAura(SPELL_MAGE_RING_OF_FROST_MARKER))
        {
            aura->SetMaxDuration(10000);
            aura->SetDuration(10000);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ring_of_frost_freeze::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnHit += SpellHitFn(spell_mage_ring_of_frost_freeze::HandleHit);
    }
};

// 82691 - Ring of Frost
class spell_mage_ring_of_frost_freeze_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost_freeze_aura);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Aura* aura = GetUnitOwner()->GetAura(SPELL_MAGE_RING_OF_FROST_MARKER))
        {
            aura->SetMaxDuration(3000);
            aura->SetDuration(3000);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_ring_of_frost_freeze_aura::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 140376 - Ring of Frost
class spell_mage_ring_of_frost_overridden : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_overridden);

    void RealEffect()
    {
        Unit* mage = GetCaster();
        uint32 delay = 2 * IN_MILLISECONDS;
        if (Player* modOwner = mage->GetSpellModOwner())
            modOwner->ApplySpellMod(GetSpellInfo()->Id, SPELLMOD_CASTING_TIME, delay);
        delay *= mage->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED);
        Position dest = GetExplTargetDest()->GetPosition();
        mage->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST);
        mage->Schedule(Milliseconds(delay), [=]()
        {
            mage->CastSpell(dest, SPELL_MAGE_RING_OF_FROST_OVERRIDEN_EFFECT, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
        });
        mage->RemoveAurasDueToSpell(SPELL_MAGE_PRESENCE_OF_MIND);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_ring_of_frost_overridden::RealEffect);
    }
};

// 45322 - Frozen Orb. Summoned by Frozen Orb (84714)
struct npc_mage_frozen_orb : public SpellDummyAI
{
    bool fistHit = true;

    npc_mage_frozen_orb(Creature* creature)
        : SpellDummyAI(creature, true)
    {
    }

    void Reset() override
    {
        if (Unit* mage = me->GetOwner())
        {
            me->CastSpell(me, SPELL_FROZEN_ORB_DUMMY, true);
            me->CastSpell(me, SPELL_FROZEN_ORB_PERIODIC, true);
            me->SetSpeed(MOVE_RUN, 1.4285f, true);    // 10 in sniffs 10 / 7 ~ 1.4285
            me->SetSpeed(MOVE_WALK, 1.4285f, true);
            Position pos = mage->GetPosition();
            mage->MovePositionToFirstCollosionBySteps(pos, 110.0f, 0.0f, 10.0f);
            me->GetMotionMaster()->MovePoint(0, pos);
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        Unit* mage = me->GetOwner();
        if (mage && fistHit && spell->Id == SPELL_FROZEN_ORB_DAMAGE_TRIGGER)
        {
            me->CastSpell(me, SPELL_FROZEN_ORB_SELF_SNARE, true);
            mage->CastSpell(mage, SPELL_MAGE_FINGERS_OF_FROST_EFFECT, true);
            // After hitting a target periodc timer has to be changed to 1 second
            if (AuraEffect* aura = me->GetAuraEffect(SPELL_FROZEN_ORB_PERIODIC, EFFECT_0))
            {
                aura->SetPeriodicTimer(spell->Effects[EFFECT_0].BasePoints);
                aura->SetAmplitude(spell->Effects[EFFECT_0].BasePoints);
            }
            fistHit = false;
        }
    }
};

// 84718 - Frozen Orb (Orb damage trigger)
class spell_mage_frozen_orb_damage_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_frozen_orb_damage_trigger);

    void HandleCast()
    {
        Unit* orb = GetCaster();
        if (Unit* mage = orb->GetOwner())
            mage->CastSpell(orb, SPELL_FROZEN_ORB_DAMAGE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_frozen_orb_damage_trigger::HandleCast);
    }
};

// 84721 - Frozen Orb (Damage)
class spell_mage_frozen_orb_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_frozen_orb_damage);

    void HandleHit()
    {
        if (roll_chance_i(15))
            GetCaster()->CastSpell(GetCaster(), SPELL_MAGE_FINGERS_OF_FROST_EFFECT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_frozen_orb_damage::HandleHit);
    }
};

// 33395 - Frost Nova (Water Elemental)
class spell_mage_pet_frost_nova : public SpellScript
{
    PrepareSpellScript(spell_mage_pet_frost_nova);

    void HandleOnHit()
    {
        if (Unit* mage = GetCaster()->GetOwner())
            if (mage->HasSpell(SPELL_MAGE_FINGERS_OF_FROST))
                mage->CastSpell(mage, SPELL_MAGE_FINGERS_OF_FROST_EFFECT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_pet_frost_nova::HandleOnHit);
    }
};

// 118859 - Incanter's Ward (Cooldown marker)
class spell_mage_incanters_ward_cooldown : public AuraScript
{
    PrepareAuraScript(spell_mage_incanters_ward_cooldown);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAura(SPELL_MAGE_INCANTERS_ABSORBTION_PASSIVE);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            if (!caster->HasAura(SPELL_MAGE_INCANTERS_ABSORBTION_PASSIVE))
                caster->CastSpell(caster, SPELL_MAGE_INCANTERS_ABSORBTION_PASSIVE, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_incanters_ward_cooldown::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_incanters_ward_cooldown::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1463 - Incanter's Ward
class spell_mage_incanters_ward : public AuraScript
{
    PrepareAuraScript(spell_mage_incanters_ward);

    float absorbTotal = 0.0f;
    float absorbtionAmount = 0.0f;

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        absorbtionAmount = float(eff->GetAmount());
    }

    void HandleAbsorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* mage = GetUnitOwner();
        uint32 absorbed = std::min(dmgInfo.GetDamage(), absorbAmount);
        absorbTotal = std::min(absorbTotal + absorbed, absorbtionAmount);

        if (AuraEffect const* eff = GetEffect(EFFECT_1))
        {
            float pct = eff->GetAmount() * absorbed / absorbtionAmount;
            int32 manaGain = CalculatePct(mage->GetMaxPower(POWER_MANA), pct);

            mage->CastCustomSpell(SPELL_MAGE_INCANTERS_WARD_ENERGIZE, SPELLVALUE_BASE_POINT0, manaGain, mage, true);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* mage = GetUnitOwner();
        int32 cap = sSpellMgr->GetSpellInfo(SPELL_MAGE_INCANTERS_ABSORBTION)->Effects[EFFECT_0].BasePoints;
        int32 damageGain = CalculatePct(cap, ((absorbTotal / absorbtionAmount) * 100.0f));
        if (!damageGain)
            return;

        mage->CastCustomSpell(SPELL_MAGE_INCANTERS_ABSORBTION, SPELLVALUE_BASE_POINT0, damageGain, mage, true);
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn(spell_mage_incanters_ward::HandleApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_incanters_ward::HandleAbsorb, EFFECT_0);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_incanters_ward::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 66 - Invisibility
class spell_mage_invisibility_initial : public AuraScript
{
    PrepareAuraScript(spell_mage_invisibility_initial);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            if (!(GetUnitOwner()->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_PACIFIED | UNIT_FLAG_CONFUSED | UNIT_FLAG_FLEEING | UNIT_FLAG_STUNNED)))
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MAGE_INVISIBILITY, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_invisibility_initial::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 32612 - Invisibility
class spell_mage_invisibility : public AuraScript
{
    PrepareAuraScript(spell_mage_invisibility);

protected:
    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* target = GetTarget();
        if (Player* mage = target->ToPlayer())
        {
            mage->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, 16384); // TODO: Need more research
            if (Pet* pet = mage->GetPet())
                pet->CastSpell(pet, GetId(), true);
        }
        if (!target->GetInstanceScript() || !target->GetInstanceScript()->IsEncounterInProgress())
            target->CombatStop();
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* mage = GetTarget()->ToPlayer())
            mage->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, 0);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_invisibility::HandleApply, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_invisibility::HandleRemove, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 110960 - Greater Invisibility
class spell_mage_greater_invisibility: public spell_mage_invisibility
{
    PrepareAuraScript(spell_mage_greater_invisibility);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        spell_mage_invisibility::HandleApply(eff, mode);
        Unit* target = GetTarget();
        uint32 remains = 2;
        for (auto&& type : { SPELL_AURA_PERIODIC_DAMAGE, SPELL_AURA_PERIODIC_DAMAGE_PERCENT })
        {
            while (!target->GetAuraEffectsByType(type).empty() && remains)
            {
                AuraEffect const* eff = target->GetAuraEffectsByType(type).front();
                eff->GetBase()->Remove();
                --remains;
            }
        }
        target->CastSpell(target, SPELL_MAGE_GREATER_INVISIBILITY_LESS_DAMAGE, true);  // In sniffs it is not triggered, but it's broke aura =/
        target->GetThreatManager().resetAllAggro();
    }

    void HandleRemove(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        spell_mage_invisibility::HandleRemove(eff, mode);

        Unit* target = GetTarget();

        target->CastSpell(target, SPELL_MAGE_REMOVE_INVISIBILITY_REMOVED_TIMER, true); // Also not triggered in sniffs
        Aura* timer = target->GetAura(SPELL_MAGE_REMOVE_INVISIBILITY_REMOVED_TIMER);
        if (Aura* aura = target->GetAura(SPELL_MAGE_GREATER_INVISIBILITY_LESS_DAMAGE))
        {
            if (timer)
            {
                // SPELLMOD_DURATION of Glyph of Combustion affects this aura, probably we can just get rid of FamilyFlags but whatever
                if (target->HasAura(SPELL_MAGE_GLYPH_OF_COMBUSTION))
                {
                    timer->SetMaxDuration(timer->GetMaxDuration() / 2);
                    timer->SetDuration(timer->GetMaxDuration());
                }
                aura->SetMaxDuration(timer->GetDuration());
                aura->SetDuration(timer->GetDuration());
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_greater_invisibility::HandleApply, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_greater_invisibility::HandleRemove, EFFECT_1, SPELL_AURA_MOD_INVISIBILITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 113862 - Greater Invisibility
class spell_mage_greater_invisibility_damage_reduction : public AuraScript
{
    PrepareAuraScript(spell_mage_greater_invisibility_damage_reduction);

    void HandleUpdate(uint32 diff, AuraEffect*)
    {
        if (!GetUnitOwner()->HasAura(SPELL_MAGE_GREATER_INVISIBILITY) && !GetUnitOwner()->HasAura(SPELL_MAGE_GREATER_INVISIBILITY_LESS_DAMAGE))
            Remove();
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_mage_greater_invisibility_damage_reduction::HandleUpdate, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// 35009
class spell_mage_invisibility_threat_reduction: public SpellScript
{
    PrepareSpellScript(spell_mage_invisibility_threat_reduction);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([this](WorldObject* obj)
        {
            if (Unit* target = obj->ToUnit())
                if (target->IsInCombat() && target->CanHaveThreatList() && target->GetThreatManager().getThreat(GetCaster()) > 0)
                    return false;

            return true;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_invisibility_threat_reduction::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 112948 - Frost Bomb 
class spell_mage_frost_bomb_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_frost_bomb_aura);

    void AfterRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(GetTarget(), SPELL_MAGE_FROST_BOMB_TRIGGERED, true);

            if (caster->HasAura(SPELL_MAGE_BRAIN_FREEZE))
                caster->CastSpell(caster, SPELL_MAGE_BRAIN_FREEZE_TRIGGERED, true);
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_frost_bomb_aura::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 113092 - Frost Bomb 
class spell_mage_frost_bomb : public SpellScript
{
    PrepareSpellScript(spell_mage_frost_bomb);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_frost_bomb::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 42955 - Conjure Refreshment
class spell_mage_conjure_refreshment : public SpellScript
{
    PrepareSpellScript(spell_mage_conjure_refreshment);

    struct ConjureRefreshmentData
    {
        uint32 MinLevel;
        uint32 MaxLevel;
        uint32 SpellId;
    };

    std::vector<ConjureRefreshmentData> const m_conjureData =
    {
        { 33, 43, 92739 },
        { 44, 53, 92799 },
        { 54, 63, 92802 },
        { 64, 73, 92805 },
        { 74, 79, 74625 },
        { 80, 84, 92822 },
        { 85, 89, 92727 },
        { 90, STRONG_MAX_LEVEL, 116130 }
    };

    void HandleCast()
    {
        uint8 level = GetCaster()->GetLevel();
        for (auto&& it : m_conjureData)
        {
            if (level < it.MinLevel || level > it.MaxLevel)
                continue;
            GetCaster()->CastSpell(GetCaster(), it.SpellId, TRIGGERED_WITH_SPELL_START);
            break;
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_conjure_refreshment::HandleCast);
    }
};

// 43987 - Conjure Refreshment Table
class spell_mage_conjure_refreshment_table : public SpellScript
{
    PrepareSpellScript(spell_mage_conjure_refreshment_table);

    void HandleCast()
    {
        uint32 level = GetCaster()->GetLevel();
        uint32 spellId = 120056;
        if (level >= 80)
            spellId = 120055;
        if (level >= 85)
            spellId = 120054;
        if (level >= 90)
            spellId = 120053;
        GetCaster()->CastSpell(GetCaster(), spellId, TRIGGERED_WITH_SPELL_START);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_conjure_refreshment_table::HandleCast);
    }
};

// 31216, 47243, 47244, 58433 - Mirror Image
struct npc_mage_mirror_image : ScriptedAI
{
    npc_mage_mirror_image(Creature* creature) : ScriptedAI(creature) { }

    uint32 spellId = 0;
    CasterMovement casterMovement{ me };
    AssistBehavior assist{ me };

    void InitializeAI() override
    {
        switch (me->GetEntry())
        {
            case 31216: spellId = 59638;  break; // Frost bolt
            case 47243: spellId = 88084;  break; // Arcane Blast
            case 47244: spellId = 88082;  break; // Fireball
            case 58433: spellId = 110691; break; // Wrath (druid symbiosis)
        }

        casterMovement.Spell(spellId);

        Unit* owner = me->GetOwner();
        if (!owner)
            return;

        me->CastSpell(me, SPELL_MIRROR_IMAGE_MASTERS_THREAT_LIST, true);
        owner->CastSpell(me, SPELL_MIRROR_IMAGE_CLONE_ME, true);
        owner->CastSpell(me, SPELL_MIRROR_IMAGE_COPY_WEAPONS, true);
    }

    void AttackStart(Unit* victim) override
    {
        if (me->Attack(victim, false))
            casterMovement.Chase(victim);
    }

    void UpdateAI(uint32 diff) override
    {
        assist.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->GetVictim()->GetSchoolImmunityMask() == SPELL_SCHOOL_MASK_ALL || me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
        {
            me->InterruptNonMeleeSpells(false);
            EnterEvadeMode();
            return;
        }

        casterMovement.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoCastVictim(spellId);
    }

    // Do not reload Creature templates on evade mode enter - prevent visual lost
    void EnterEvadeMode() override
    {
        if (me->IsInEvadeMode() || !me->IsAlive())
            return;

        Unit* owner = me->GetCharmerOrOwner();

        me->CombatStop(true);
        if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
    }
};

// 116 - Frostbolt, 44614 - Frostfire Bolt, 30455 - Ice Lance, 31707 - Waterbolt
class spell_mage_glyph_of_icy_veins : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_icy_veins);

    void HandleCast()
    {
        Player* mage = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!mage)
            return;
        if (mage->HasAura(SPELL_MAGE_ICY_VEINS_GLYPHED))
        {
            Unit* caster = GetCaster();
            ObjectGuid guid = GetSpell()->m_targets.GetUnitTargetGUID();
            uint32 spell = GetSpellId();

            float mult = 0.4f;   // Primiry mod
            if (GetSpellInfo()->Id == 30455)
            {
                bool fingers = mage->HasAura(SPELL_MAGE_FINGERS_OF_FROST_EFFECT);
                caster->IcyVeinsHack = fingers;
                if (fingers || GetExplTargetUnit()->HasAuraState(AURA_STATE_FROZEN))
                    mult *= 4.0f;
                if (fingers)
                    mult *= 1.25f;
            }
            if (GetSpellInfo()->Id == 44614)
                caster->IcyVeinsHack = mage->HasAura(SPELL_MAGE_BRAIN_FREEZE_TRIGGERED);

            auto castFn = [=]
            {
                if (Unit* target = ObjectAccessor::GetUnit(*caster, guid))
                {
                    CustomSpellValues ctx;
                    ctx.SetMultiplier(mult);
                    caster->CastCustomSpell(spell, ctx, target, TRIGGERED_FULL_MASK);
                }
            };

            caster->m_Events.Schedule(400, [=]
            {
                castFn();
                caster->m_Events.Schedule(400, [=]()
                {
                    castFn();
                    caster->IcyVeinsHack = false;
                });
            });
        }
    }

    uint32 GetSpellId()
    {
        switch (GetSpellInfo()->Id)
        {
            case 116:   return 131079; // Frostbolt
            case 30455: return 131080; // Ice Lance
            case 44614: return 131081; // Frostfire Bolt
            case 31707: return 131581; // Waterbolt
        }
        return 0;
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_glyph_of_icy_veins::HandleCast);
    }
};

// 12472, 131078 - Icy Veins
class spell_mage_icy_veins_cooldown : public SpellScript
{
    PrepareSpellScript(spell_mage_icy_veins_cooldown);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleCast()
    {
        uint32 otherSpell = GetSpellInfo()->Id == SPELL_MAGE_ICY_VEINS ? SPELL_MAGE_ICY_VEINS_GLYPHED : SPELL_MAGE_ICY_VEINS;
        uint32 cooldown = GetCaster()->ToPlayer()->GetSpellCooldownDelay(GetSpellInfo()->Id);
        GetCaster()->ToPlayer()->AddCustomCooldown(otherSpell, Milliseconds(cooldown));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_icy_veins_cooldown::HandleCast);
    }
};

// 44614 - Frostfire Bolt
class spell_frostfire_bolt : public SpellScript
{
    PrepareSpellScript(spell_frostfire_bolt);

    void HandleCast()
    {
        if (Aura* brainFreez = GetCaster()->GetAura(SPELL_MAGE_BRAIN_FREEZE_TRIGGERED))
        {
            auto it = GetSpell()->m_appliedMods.find(brainFreez);
            if (it == GetSpell()->m_appliedMods.end())
                return;

            if (AuraEffect* presence = GetCaster()->GetAuraEffect(SPELL_MAGE_PRESENCE_OF_MIND, EFFECT_0))
            {
                auto it = GetSpell()->m_appliedMods.find(presence->GetBase());
                if (it != GetSpell()->m_appliedMods.end())
                {
                    presence->GetSpellModifier()->charges = 1;
                    GetSpell()->m_appliedMods.erase(it);
                }
            }

            GetCaster()->RemoveAura(brainFreez);

            if (Aura const* bonus = GetCaster()->GetAura(SPELL_MAGE_T16_4P_BONUS))
                if (roll_chance_i(bonus->GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster())))
                    GetCaster()->CastSpell(GetExplTargetUnit(), SPELL_MAGE_FRIGID_BLAST, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_frostfire_bolt::HandleCast);
    }
};

// 146976 - Glyph of the Unbound Elemental
class spell_aura_glyph_of_unbound_elemental : public AuraScript
{
    PrepareAuraScript(spell_aura_glyph_of_unbound_elemental);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Guardian* pet = GetUnitOwner()->GetGuardianPet())
            pet->CastSpell(pet, SPELL_MAGE_UNBOUND_WATER_ELEMENTAL_TRANSFORM, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Guardian* pet = GetUnitOwner()->GetGuardianPet())
            pet->RemoveAurasDueToSpell(SPELL_MAGE_UNBOUND_WATER_ELEMENTAL_TRANSFORM);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_aura_glyph_of_unbound_elemental::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_aura_glyph_of_unbound_elemental::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1953, 119415 - Blink
class spell_mage_glyph_of_rapid_displacement : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_rapid_displacement);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_RAPID_DISPLACEMENT) && GetCaster()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED))
            return SPELL_FAILED_STUNNED;
        return SPELL_CAST_OK;
    }

    void HandleEffect(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_RAPID_DISPLACEMENT))
            PreventEffectForTarget(GetHitUnit(), effIndex);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_glyph_of_rapid_displacement::CheckCast);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_glyph_of_rapid_displacement::HandleEffect, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_glyph_of_rapid_displacement::HandleEffect, EFFECT_2, SPELL_EFFECT_APPLY_AURA);
    }
};

// 1953 - Blink
class spell_mage_glyph_of_momentum : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_momentum);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_MOMENTUM))
            GetCaster()->CastSpell(GetCaster(), SPELL_MAGE_GLYPH_OF_MOMENTUM_BLINK, true);
    }

    void PreventBlink(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_MOMENTUM))
            PreventEffect(effIndex);
    }

    void PreventAura(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_MOMENTUM))
            PreventEffectForTarget(GetHitUnit(), effIndex);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_glyph_of_momentum::HandleCast);
        OnEffectHitTarget += SpellEffectFn(spell_mage_glyph_of_momentum::PreventBlink, EFFECT_0, SPELL_EFFECT_LEAP);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_glyph_of_momentum::PreventAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_glyph_of_momentum::PreventAura, EFFECT_2, SPELL_EFFECT_APPLY_AURA);
    }
};

// 119415 - Blink
class spell_mage_glyph_of_momentum_blink : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_momentum_blink);

    void SelectDest(SpellDestination& dest)
    {
        // FUCK YOU BLIZZARD
        // This spell casted in OnCast of spell_mage_glyph_of_momentum and spell SelectSpellTargets already called.
        // And both this spells have to have same destination. Let's just override it.
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            spell->m_targets.SetDst(dest);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_mage_glyph_of_momentum_blink::SelectDest, EFFECT_0, TARGET_DEST_CASTER_MOVEMENT_DIR);
    }
};

// 30449 - Spellsteal
class spell_mage_glyph_of_spellsteal : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_spellsteal);

    void HandleHit()
    {
        if (GetDispelCount() > 0 && GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_SPELLSTEAL))
            GetCaster()->CastSpell(GetCaster(), SPELL_MAGE_GLYPH_OF_SPELLSTEAL_HEAL, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_glyph_of_spellsteal::HandleHit);
    }
};

// Oof, search by "portal:" or "teleport:" and effect 1 is SPELL_EFFECT_SCRIPT_EFFECT
class spell_mage_glyph_of_rapid_teleportation : public SpellScript
{
    PrepareSpellScript(spell_mage_glyph_of_rapid_teleportation);

    void HandleHit(SpellEffIndex)
    {
        if (GetHitUnit()->HasAura(SPELL_MAGE_GLYPH_OF_RAPID_TELEPORTATION))
        {
            // Will be handled after teleport ack.
            GetHitUnit()->Schedule(Milliseconds(1), [](Unit* self)
            {
                self->CastSpell(self, SPELL_MAGE_RAPID_TELEPORTATION, true);
            });
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_glyph_of_rapid_teleportation::HandleHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 44425 - Arcane Barrage
class spell_mage_arcane_barrage : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_barrage);

    void HandleHit()
    {
        if (GetHitUnit() != GetExplTargetUnit())
            SetHitDamage(GetHitDamage() / 2);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_arcane_barrage::HandleHit);
    }
};

// 110909 - Alter Time
class spell_mage_t15_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_mage_t15_2p_bonus);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* mage = GetUnitOwner();
        if (mage->HasAura(SPELL_MAGE_T15_2P_BONUS))
        {
            // Can't do it right now because of Alter Time effect
            mage->m_Events.Schedule(1, [=]
            {
                mage->CastSpell(mage, SPELL_MAGE_TIME_LORD, true);
            });
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_t15_2p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// 57761 - Brain Freeze
// 48108 - Pyroblast!
class spell_mage_t16_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_mage_t16_2p_bonus);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->HasAura(SPELL_MAGE_T16_2P_BONUS) && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
        {
            uint32 spellId = GetId() == SPELL_MAGE_BRAIN_FREEZE_TRIGGERED ? SPELL_MAGE_FROZEN_THOUGHTS : SPELL_MAGE_POTENT_FLAMES;
            GetUnitOwner()->CastSpell(GetUnitOwner(), spellId, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_t16_2p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 145257 - Pyroblast!
class spell_mage_t16_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_mage_t16_4p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_INFERNO_BLAST)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MAGE_FIERY_ADEPT, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_t16_4p_bonus::HandleProc);
    }
};

// 116 - Frostbolt
// 131079 - Frostbolt (Glyph of Icy Veins)
class spell_mage_frostbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt);

    bool heal = false;

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();

        if (!target)
            return SPELL_FAILED_NO_VALID_TARGETS;

        // Attacking
        Unit* caster = GetCaster();
        if (caster->IsValidAttackTarget(target))
        {
            if (!caster->HasInArc(static_cast<float>(M_PI), target))
                return SPELL_FAILED_UNIT_NOT_INFRONT;
            return SPELL_CAST_OK;
        }

        heal = true;
        if (Unit* pet = GetCaster()->GetGuardianPet())
        {
            if (pet->GetEntry() == ENTRY_WATER_ELEMENTAL)
            {
                GetSpell()->m_targets.SetUnitTarget(pet);
                return SPELL_CAST_OK;
            }
        }

        return SPELL_FAILED_BAD_TARGETS;
    }

    void HandleDamage(SpellEffIndex effIndex)
    {
        if (heal)
            PreventEffectForTarget(GetHitUnit(), effIndex);
    }

    void HandleHeal(SpellEffIndex effIndex)
    {
        if (heal)
            if (Unit* pet = GetCaster()->GetGuardianPet())
                GetCaster()->CastSpell(pet, SPELL_MAGE_FROSTBOLT_HEAL, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_frostbolt::CheckCast);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_frostbolt::HandleDamage, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_frostbolt::HandleDamage, EFFECT_ALL, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_mage_frostbolt::HandleHeal, EFFECT_ALL, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 118 - Polymorph
class spell_mage_polymorph : public AuraScript
{
    PrepareAuraScript(spell_mage_polymorph);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* mage = GetCaster())
        {
            uint32 modelId = 0;
            uint32 soundSpell = 61634;          // Normal sheep sound

            if (mage->HasAura(52648))           // Glyph of the Penguin
            {
                modelId = 27433;
                soundSpell = 61635;
            }
            else if (mage->HasAura(57927))      // Glyph of the Monkey
            {
                modelId = 29963;
                soundSpell = 89729;
            }
            else if (mage->HasAura(57924))      // Glyph of the Percupine
            {
                const uint32 models[]{ 40119, 40122, 40123 };
                modelId = models[urand(0, 2)];
                soundSpell = 126834;
            }
            else if (mage->HasAura(58136))      // Glyph of the Bear Cub
            {
                modelId = 23948;
                soundSpell = 127578;
            }
            GetUnitOwner()->CastSpell(GetUnitOwner(), soundSpell, true);
            if (modelId)
                GetUnitOwner()->SetDisplayId(modelId);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mage_polymorph::HandleApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 118 - Polymorph
// 28271 - Polymorph (turtle)
// 28272 - Polymorph (pig, not available?)
// 61025 - Polymorph (snake, not available?)
// 61305 - Polymorph (cat)
// 61721 - Polymorph (rabbit)
// 61780 - Polymorph (turkey, not available?)
class spell_mage_exclusive_polymorph : public AuraScript
{
    PrepareAuraScript(spell_mage_exclusive_polymorph);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* mage = GetCaster();
        if (!mage)
            return;

        uint32 const spells[]{ 118, 28271, 28272, 61025, 61305, 61721, 61780 };
        for (auto&& spell : spells)
            if (GetId() != spell)
                if (auto* list = mage->GetBoundAurasBySpellId(spell))
                    list->back()->Remove(); // Only one aura, no need to iterate
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_exclusive_polymorph::HandleApply, EFFECT_ALL, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 98397 - Glyph of Armors
class spell_mage_glyph_of_armors : public AuraScript
{
    PrepareAuraScript(spell_mage_glyph_of_armors);

    void Recalculate(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* mageArmor = GetUnitOwner()->GetAuraEffect(6117, EFFECT_1))
            mageArmor->RecalculateAmount();
        if (AuraEffect* moltenArmor = GetUnitOwner()->GetAuraEffect(7321, EFFECT_1))
            moltenArmor->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_glyph_of_armors::Recalculate, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_mage_glyph_of_armors::Recalculate, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 31661 - Dragon's Breath
class spell_mage_dragons_breath : public AuraScript
{
    PrepareAuraScript(spell_mage_dragons_breath);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_MAGE_NETHER_TEMPEST_DIRECT_DAMAGE)
            return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_dragons_breath::CheckProc);
    }
};

void AddSC_mage_spell_scripts()
{
    new spell_mage_flamestrike();
    new spell_mage_glyph_of_slow();
    new spell_mage_glyph_of_ice_block();
    new spell_script<spell_mage_pyromaniac>("spell_mage_pyromaniac");
    new spell_mage_slow();
    new spell_mage_frostjaw();
    new spell_script<spell_mage_combustion>("spell_mage_combustion");
    new spell_script<spell_mage_inferno_blast>("spell_mage_inferno_blast");
    new spell_script<spell_mage_inferno_blast_spread>("spell_mage_inferno_blast_spread");
    new spell_mage_arcane_brilliance();
    new spell_mage_evocation();
    new spell_mage_cold_snap();
    new spell_mage_incanters_absorbtion_absorb();
    new spell_mage_incanters_absorbtion_manashield();
    new aura_script<spell_mage_temporal_shield>("spell_mage_temporal_shield");
    new spell_mage_flameglow();
    new spell_mage_blizzard();
    new spell_mage_glyph_of_illusion();
    new spell_mage_glyph_of_conjure_familiar();
    new spell_mage_illusion();
    new spell_script<spell_mage_mirror_image>("spell_mage_mirror_image");
    new aura_script<spell_mage_2p_pvp>("spell_mage_2p_pvp");
    new aura_script<spell_mage_arcane_missiles_proc>("spell_mage_arcane_missiles_proc");
    new aura_script<spell_mage_arcane_missiles_proc_visual>("spell_mage_arcane_missiles_proc_visual");
    new aura_script<spell_mage_fingers_of_frost>("spell_mage_fingers_of_frost");
    new aura_script<spell_mastery_icicles_periodic>("spell_mastery_icicles_periodic");
    new aura_script<spell_mage_mastery_icicles>("spell_mage_mastery_icicles");
    new spell_script<spell_mage_ice_lance>("spell_mage_ice_lance");
    new spell_script<spell_mage_deep_freez>("spell_mage_deep_freez");
    new spell_script<spell_mage_fingers_of_frost_proc>("spell_mage_fingers_of_frost_proc");
    new aura_script<spell_mage_cauterize>("spell_mage_cauterize");
    new aura_script<spell_mage_evocation>("spell_mage_evocation");
    new aura_script<spell_mage_arcane_charge>("spell_mage_arcane_charge");
    new aura_script<spell_mage_arcane_missiles>("spell_mage_arcane_missiles");
    new spell_script<spell_mage_arcane_explosion>("spell_mage_arcane_explosion");
    new aura_script<spell_mage_ignite>("spell_mage_ignite");
    new spell_script<spell_mage_rune_of_power>("spell_mage_rune_of_power");
    new atrigger_script<sat_mage_rune_of_power>("sat_mage_rune_of_power");
    new spell_script<spell_mage_nether_tempest_selector>("spell_mage_nether_tempest_selector");
    new aura_script<spell_mage_nether_tempest>("spell_mage_nether_tempest");
    new aura_script<spell_mage_living_bomb>("spell_mage_living_bomb");
    new aura_script<spell_mage_pyroblast_crearcasting>("spell_mage_pyroblast_crearcasting");
    new spell_script<spell_mage_pyroblast>("spell_mage_pyroblast");
    new spell_script<spell_mage_alter_time_overridden>("spell_mage_alter_time_overridden");
    new aura_script<spell_mage_alter_time>("spell_mage_alter_time");
    new spell_script<spell_mage_alter_time_cast>("spell_mage_alter_time_cast");
    new creature_script<npc_mage_past_self>("npc_mage_past_self");
    new aura_script<spell_mage_ring_of_frost>("spell_mage_ring_of_frost");
    new spell_script<spell_mage_ring_of_frost_freeze>("spell_mage_ring_of_frost_freeze");
    new aura_script<spell_mage_ring_of_frost_freeze_aura>("spell_mage_ring_of_frost_freeze_aura");
    new spell_script<spell_mage_ring_of_frost_overridden>("spell_mage_ring_of_frost_overridden");
    new creature_script<npc_mage_frozen_orb>("npc_mage_frozen_orb");
    new spell_script<spell_mage_frozen_orb_damage_trigger>("spell_mage_frozen_orb_damage_trigger");
    new spell_script<spell_mage_frozen_orb_damage>("spell_mage_frozen_orb_damage");
    new spell_script<spell_mage_pet_frost_nova>("spell_mage_pet_frost_nova");
    new aura_script<spell_mage_incanters_ward_cooldown>("spell_mage_incanters_ward_cooldown");
    new aura_script<spell_mage_incanters_ward>("spell_mage_incanters_ward");
    new aura_script<spell_mage_invisibility_initial>("spell_mage_invisibility_initial");
    new aura_script<spell_mage_invisibility>("spell_mage_invisibility");
    new aura_script<spell_mage_greater_invisibility>("spell_mage_greater_invisibility");
    new aura_script<spell_mage_greater_invisibility_damage_reduction>("spell_mage_greater_invisibility_damage_reduction");
    new spell_script<spell_mage_invisibility_threat_reduction>("spell_mage_invisibility_threat_reduction");
    new aura_script<spell_mage_frost_bomb_aura>("spell_mage_frost_bomb_aura");
    new spell_script<spell_mage_frost_bomb>("spell_mage_frost_bomb");
    new spell_script<spell_mage_conjure_refreshment>("spell_mage_conjure_refreshment");
    new spell_script<spell_mage_conjure_refreshment_table>("spell_mage_conjure_refreshment_table");
    new creature_script<npc_mage_mirror_image>("npc_mage_mirror_image");
    new spell_script<spell_mage_glyph_of_icy_veins>("spell_mage_glyph_of_icy_veins");
    new spell_script<spell_mage_icy_veins_cooldown>("spell_mage_icy_veins_cooldown");
    new spell_script<spell_frostfire_bolt>("spell_frostfire_bolt");
    new aura_script<spell_aura_glyph_of_unbound_elemental>("spell_aura_glyph_of_unbound_elemental");
    new spell_script<spell_mage_glyph_of_rapid_displacement>("spell_mage_glyph_of_rapid_displacement");
    new spell_script<spell_mage_glyph_of_momentum>("spell_mage_glyph_of_momentum");
    new spell_script<spell_mage_glyph_of_momentum_blink>("spell_mage_glyph_of_momentum_blink");
    new spell_script<spell_mage_glyph_of_spellsteal>("spell_mage_glyph_of_spellsteal");
    new spell_script<spell_mage_glyph_of_rapid_teleportation>("spell_mage_glyph_of_rapid_teleportation");
    new spell_script<spell_mage_arcane_barrage>("spell_mage_arcane_barrage");
    new aura_script<spell_mage_t15_2p_bonus>("spell_mage_t15_2p_bonus");
    new aura_script<spell_mage_t16_2p_bonus>("spell_mage_t16_2p_bonus");
    new aura_script<spell_mage_t16_4p_bonus>("spell_mage_t16_4p_bonus");
    new spell_script<spell_mage_frostbolt>("spell_mage_frostbolt");
    new aura_script<spell_mage_polymorph>("spell_mage_polymorph");
    new aura_script<spell_mage_exclusive_polymorph>("spell_mage_exclusive_polymorph");
    new aura_script<spell_mage_glyph_of_armors>("spell_mage_glyph_of_armors");
    new aura_script<spell_mage_dragons_breath>("spell_mage_dragons_breath");
}
