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
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "spell_common.h"
#include "GridNotifiers.h"
#include "MoveSpline.h"
#include "Player.h"
#include "Random.h"

enum WarriorSpells
{
    WARRIOR_SPELL_DEFENSIVE_STANCE              = 71,
    WARRIOR_SPELL_LAST_STAND_TRIGGERED          = 12976,
    SPELL_WARRIOR_VICTORY_RUSH_HEAL             = 118779,
    SPELL_WARRIOR_VICTORIOUS_STATE              = 32216,
    SPELL_WARRIOR_VICTORIOUS_STATE_T15          = 138279,
    SPELL_WARRIOR_IMPENDING_VICTORY_HEAL        = 118340,
    SPELL_WARRIOR_GLYPH_OF_VICTORY_RUSH         = 58382,
    WARRIOR_SPELL_BLOODTHIRST                   = 23881,
    WARRIOR_SPELL_BLOODTHIRST_HEAL              = 117313,
    WARRIOR_SPELL_THUNDER_CLAP                  = 6343,
    WARRIOR_SPELL_WEAKENED_BLOWS                = 115798,
    WARRIOR_SPELL_BLOOD_AND_THUNDER             = 84615,
    SPELL_WARRIOR_SHOCKWAVE_STUN                = 132168,
    SPELL_WARRIOR_HEROIC_LEAP_LANDING           = 203780,
    SPELL_WARRIOR_HEROIC_LEAP_DAMAGE            = 52174,
    WARRIOR_SPELL_RALLYING_CRY                  = 97463,
    SPELL_WARRIOR_SWORD_AND_BOARD               = 50227,
    SPELL_WARRIOR_SHIELD_SLAM                   = 23922,
    WARRIOR_SPELL_MOCKING_BANNER_TAUNT          = 114198,
    WARRIOR_NPC_MOCKING_BANNER                  = 59390,
    WARRIOR_SPELL_BERZERKER_RAGE_EFFECT         = 23691,
    WARRIOR_SPELL_ENRAGE                        = 12880,
    SPELL_WARR_COLOSSUS_SMASH                   = 86346,
    SPELL_WARRIOR_SECOND_WIND_REGEN             = 16491,
    SPELL_WARRIOR_SECOND_WIND_DUMMY             = 125667,
    WARRIOR_SPELL_UNBRIDLED_WRATH_REGEN         = 29842,
    SPELL_WARRIOR_DRAGON_ROAR_KNOCK_BACK        = 118895,
    WARRIOR_SPELL_STORM_BOLT_STUN               = 132169,
    WARRIOR_SPELL_SHIELD_BLOCKC_TRIGGERED       = 132404,
    WARRIOR_SPELL_GLYPH_OF_HINDERING_STRIKES    = 58366,
    WARRIOR_SPELL_SLUGGISH                      = 129923,
    WARRIOR_SPELL_IMPENDING_VICTORY             = 103840,
    SPELL_WARRIOR_PVP_SET_4P                    = 133277,
    SPELL_WARRIOR_HEROIC_LEAP_SPEED             = 133278,
    SPELL_WARR_MORTAL_STRIKE                    = 12294,
    SPELL_WARR_TASTE_FOR_BLOOD_EFFECT           = 60503,
    SPELL_WARR_REVENGE                          = 6572,
    SPELL_WARR_OPPORTUNITY_STRIKE               = 76858,
    SPELL_WARR_BLOODBATH                        = 113344,
    SPELL_WARR_BLOODBATH_SLOW                   = 147531,
    SPELL_WARR_EXECUTE                          = 5308,
    SPELL_WARR_SUDDEN_EXECUTE                   = 139958,
    SPELL_WARR_SUDDEN_DEATH                     = 52437,
    SPELL_WARRIOR_MEAT_CLEAVER                  = 85739,
    SPELL_WARRIOR_RAGING_BLOW_ENABLER           = 131116,
    SPELL_WARRIOR_GLYPH_OF_COLOSSUS_SMASH       = 89003,
    SPELL_WARRIOR_GLYPH_OF_INTIMIDATING_SHOUT   = 63327,
    SPELL_WARRIOR_SHIELD_WALL_VISUAL_ALLIANCE   = 147925,
    SPELL_WARRIOR_SHIELD_WALL_VISUAL_HORDE      = 146127,
    SPELL_WARRIOR_SHIELD_WALL_VISUAL_WITH_SHIELD= 146128,
    SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_WITH_SHIELD = 146120,
    SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_ALLIANCE = 147923,
    SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_HORDE = 146122,
    SPELL_WARRIOR_SEASONED_SOLDIER              = 12712,
    SPELL_WARRIOR_OVERPOWER                     = 7384,
    SPELL_WARRIOR_DIE_BY_THE_SWORD              = 118038,
    SPELL_WARRIOR_GLYPH_OF_DIE_BY_THE_SWORD     = 58386,
    SPELL_WARRIOR_CHARGE_STUN                   = 7922,
    SPELL_WARRIOR_DOUBLE_TIME                   = 103827,
    SPELL_WARRIOR_WARBRINGER                    = 103828,
    SPELL_WARRIOR_WARBRINGER_STUN               = 105771,
    SPELL_WARRIOR_WARBRINGER_SLOW               = 137637,
    SPELL_WARRIOR_DOUBLE_TIME_MARKER            = 124184,
    SPELL_WARRIOR_DEMORALIZING_SHOUT            = 1160,
    SPELL_WARRIOR_SHIELD_BARRIER                = 112048,
    SPELL_WARRIOR_COLOSSUS_SMASH                = 86346,
    SPELL_WARRIOR_COLOSSAL_RAGE                 = 144438,
    SPELL_WARRIOR_DEATH_SENTENCE                = 144442,
    SPELL_WARRIOR_T16_PROTECTION_2P_BONUS       = 144503,
    SPELL_WARRIOR_T16_PROTECTION_4P_BONUS       = 144502,
    SPELL_WARRIOR_SHIELD_GUARD                  = 144551,
    SPELL_WARRIOR_RECKLESS_DEFENSE              = 144500,
    SPELL_WARRIOR_RECKLESS_DEFENSE_ENERGIZE     = 144499,
    SPELL_WARRIOR_DEEP_WOUNDS                   = 115767,
    SPELL_WARRIOR_GLYPH_OF_BLOODY_HEALING       = 58369,
    SPELL_WARRIOR_BLOODY_HEALING                = 126665,
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK = 12723,
    SPELL_WARRIOR_GLYPH_OF_SWEEPING_STRIKES     = 58384,
    SPELL_WARRIOR_GLYPH_OF_SWEEPING_STRIKES_PROC= 124333,
    SPELL_WARRIOR_SLAM                          = 1464,
    SPELL_WARRIOR_SLAM_PROC                     = 146361,
    SPELL_WARRIOR_GLYPH_OF_THE_BLAZING_TRAIL    = 123779,

    SPELL_WEAKENED_ARMOR                        = 113746,
    SPELL_PHYSICAL_VULNERABILITY                = 81326,
};

// 32216 - Victorious State
// 138280 - Item - Warrior T15 Protection 2P Bonus
class spell_warr_victorious_state : public SpellScript
{
    PrepareSpellScript(spell_warr_victorious_state);

    void HandleOnHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->RemoveSpellCooldown(WARRIOR_SPELL_IMPENDING_VICTORY, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_victorious_state::HandleOnHit);
    }
};

// Called by Heroic Strike - 78 and Cleave - 845
// Glyph of Hindering Strikes - 58366
class spell_warr_glyph_of_hindering_strikes : public SpellScriptLoader
{
    public:
        spell_warr_glyph_of_hindering_strikes() : SpellScriptLoader("spell_warr_glyph_of_hindering_strikes") { }

        class spell_warr_glyph_of_hindering_strikes_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_glyph_of_hindering_strikes_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        if (_player->HasAura(WARRIOR_SPELL_GLYPH_OF_HINDERING_STRIKES))
                            _player->CastSpell(target, WARRIOR_SPELL_SLUGGISH, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_glyph_of_hindering_strikes_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_glyph_of_hindering_strikes_SpellScript();
        }
};

// Stampeding Shout - 122294
class spell_warr_stampeding_shout : public SpellScriptLoader
{
    public:
        spell_warr_stampeding_shout() : SpellScriptLoader("spell_warr_stampeding_shout") { }

        class spell_warr_stampeding_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_stampeding_shout_SpellScript);

            void HandleOnHit()
            {
                if (GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        target->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_stampeding_shout_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_stampeding_shout_SpellScript();
        }
};

// Shield Block - 2565
class spell_warr_shield_block : public SpellScriptLoader
{
    public:
        spell_warr_shield_block() : SpellScriptLoader("spell_warr_shield_block") { }

        class spell_warr_shield_block_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shield_block_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->CastSpell(_player, WARRIOR_SPELL_SHIELD_BLOCKC_TRIGGERED, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_shield_block_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_shield_block_SpellScript();
        }
};

// 107570 - Storm Bolt
class spell_warr_storm_bolt : public SpellScript
{
    PrepareSpellScript(spell_warr_storm_bolt);

    void HandleLaunch(SpellEffIndex)
    {
        // Launch doesn't called if spell won't hit target
        GetCaster()->CastSpell(GetHitUnit(), WARRIOR_SPELL_STORM_BOLT_STUN, true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_storm_bolt::HandleLaunch, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

// Storm Bolt damage (145585, 107570)
class spell_warr_storm_bolt_damage : public SpellScriptLoader
{
    public:
        spell_warr_storm_bolt_damage() : SpellScriptLoader("spell_warr_storm_bolt_damage") { }

        class spell_warr_storm_bolt_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_storm_bolt_damage_SpellScript);

            void HandleOnHit()
            {
                if (Unit* unitTarget = GetHitUnit())
                    if (unitTarget->GetTypeId() != TYPEID_PLAYER && unitTarget->IsImmunedToSpellEffect(sSpellMgr->GetSpellInfo(WARRIOR_SPELL_STORM_BOLT_STUN), 0))
                        SetHitDamage(GetHitDamage() * 4);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_storm_bolt_damage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_storm_bolt_damage_SpellScript();
        }
};

class UnitAuraTypeCheck
{
public:
    UnitAuraTypeCheck(bool present, AuraType type) : _present(present), _type(type) {}
    bool operator()(Unit* unit) const
    {
        return unit->HasAuraType(_type) == _present;
    }

    bool operator()(WorldObject* object) const
    {
        return object->ToUnit() && object->ToUnit()->HasAuraType(_type) == _present;
    }

private:
    bool _present;
    AuraType _type;
};

// Staggering Shout - 107566
class spell_warr_staggering_shout : public SpellScriptLoader
{
    public:
        spell_warr_staggering_shout() : SpellScriptLoader("spell_warr_staggering_shout") { }

        class spell_warr_staggering_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_staggering_shout_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(UnitAuraTypeCheck(false, SPELL_AURA_MOD_DECREASE_SPEED));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_staggering_shout_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_staggering_shout_SpellScript();
        }
};

// Unbridled Wrath - 143268
class spell_warr_unbridled_wrath : public SpellScriptLoader
{
    public:
        spell_warr_unbridled_wrath() : SpellScriptLoader("spell_warr_unbridled_wrath") { }

        class spell_warr_unbridled_wrath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_unbridled_wrath_AuraScript);

            bool checkProc(ProcEventInfo& /*eventInfo*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->HasSpell(143268) && (caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED) || caster->HasAuraType(SPELL_AURA_MOD_ROOT)))
                        return true;

                return false;
            }

            void onProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActionTarget(), WARRIOR_SPELL_UNBRIDLED_WRATH_REGEN, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_unbridled_wrath_AuraScript::checkProc);
                OnEffectProc += AuraEffectProcFn(spell_warr_unbridled_wrath_AuraScript::onProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_unbridled_wrath_AuraScript();
        }
};

// Berserker Rage - 18499
class spell_warr_berserker_rage : public SpellScriptLoader
{
    public:
        spell_warr_berserker_rage() : SpellScriptLoader("spell_warr_berserker_rage") { }

        class spell_warr_berserker_rage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_berserker_rage_SpellScript);

            void HandleOnHit()
            {
                GetCaster()->CastSpell(GetCaster(), WARRIOR_SPELL_ENRAGE, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_berserker_rage_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_berserker_rage_SpellScript();
        }
};

// Mocking Banner - 114192
class spell_warr_mocking_banner : public SpellScriptLoader
{
    public:
        spell_warr_mocking_banner() : SpellScriptLoader("spell_warr_mocking_banner") { }

        class spell_warr_mocking_banner_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_mocking_banner_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    std::list<Creature*> bannerList;
                    std::list<Creature*> tempList;

                    GetTarget()->GetCreatureListWithEntryInGrid(tempList, WARRIOR_NPC_MOCKING_BANNER, 30.0f);

                    bannerList = tempList;

                    // Remove other players banners
                    for (auto itr : tempList)
                    {
                        Unit* owner = itr->GetOwner();
                        if (owner && owner == player && itr->IsSummon())
                            continue;

                        bannerList.remove(itr);
                    }

                    for (auto itr : bannerList)
                        player->CastSpell(itr, WARRIOR_SPELL_MOCKING_BANNER_TAUNT, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_mocking_banner_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_mocking_banner_AuraScript();
        }
};

// Mortal strike - 12294
class spell_warr_mortal_strike : public SpellScriptLoader
{
    public:
        spell_warr_mortal_strike() : SpellScriptLoader("spell_warr_mortal_strike") { }

        class spell_warr_mortal_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_mortal_strike_AuraScript)

            enum
            {
                GLYPH_OF_MORTAL_STRIKE  = 58368
            };

            void HandleInitEffects(uint32& effectMask)
            {
                auto const caster = GetCaster();
                if (!caster || !caster->HasAura(GLYPH_OF_MORTAL_STRIKE))
                    effectMask &= ~(1 << EFFECT_4);
            }

            void Register() override
            {
                OnInitEffects += AuraInitEffectsFn(spell_warr_mortal_strike_AuraScript::HandleInitEffects);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_mortal_strike_AuraScript();
        }
};

// Rallying cry - 97462
class spell_warr_rallying_cry : public SpellScriptLoader
{
    public:
        spell_warr_rallying_cry() : SpellScriptLoader("spell_warr_rallying_cry") { }

        class spell_warr_rallying_cry_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_rallying_cry_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 basePoints0 = int32(GetHitUnit()->CountPctFromMaxHealth(GetEffectValue()));
                GetCaster()->CastCustomSpell(GetHitUnit(), WARRIOR_SPELL_RALLYING_CRY, &basePoints0, NULL, NULL, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_rallying_cry_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_rallying_cry_SpellScript();
        }
};

// 46968 - Shockwave
class spell_warr_shockwave : public SpellScript
{
    PrepareSpellScript(spell_warr_shockwave);

    uint32 m_targetCount = 0;

    void CountTargets(std::list<WorldObject*>& targetList)
    {
        m_targetCount = targetList.size();
    }

    void HandleCast()
    {
        if (m_targetCount >= 3)
            if (GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER)
                GetCaster()->ToPlayer()->ModifySpellCooldown(GetSpellInfo()->Id, -(GetSpellInfo()->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS));
    }

    void HandleHit(SpellEffIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_SHOCKWAVE_STUN, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_shockwave::CountTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        AfterCast += SpellCastFn(spell_warr_shockwave::HandleCast);
        OnEffectHitTarget += SpellEffectFn(spell_warr_shockwave::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Bloodthirst - 23881
class spell_warr_bloodthirst : public SpellScriptLoader
{
    public:
        spell_warr_bloodthirst() : SpellScriptLoader("spell_warr_bloodthirst") { }

        class spell_warr_bloodthirst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bloodthirst_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(WARRIOR_SPELL_BLOODTHIRST))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                if (Player* player = GetCaster()->ToPlayer())
                    if (GetHitUnit() && GetHitDamage())
                        player->CastSpell(player, WARRIOR_SPELL_BLOODTHIRST_HEAL, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_bloodthirst_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_bloodthirst_SpellScript();
        }
};

class spell_warr_last_stand : public SpellScriptLoader
{
    public:
        spell_warr_last_stand() : SpellScriptLoader("spell_warr_last_stand") { }

        class spell_warr_last_stand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_last_stand_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(WARRIOR_SPELL_LAST_STAND_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(30));
                    caster->CastCustomSpell(caster, WARRIOR_SPELL_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                }
            }

            void Register() override
            {
                // add dummy effect spell handler to Last Stand
                OnEffectHit += SpellEffectFn(spell_warr_last_stand_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_last_stand_SpellScript();
        }
};

// Thunder Clap - 6343
class spell_warr_thunder_clap : public SpellScriptLoader
{
    public:
        spell_warr_thunder_clap() : SpellScriptLoader("spell_warr_thunder_clap") { }

        class spell_warr_thunder_clap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_thunder_clap_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        _player->CastSpell(target, WARRIOR_SPELL_WEAKENED_BLOWS, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_warr_thunder_clap_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_thunder_clap_SpellScript();
        }
};

// 100 - Charge
class spell_warr_charge : public SpellScript
{
    PrepareSpellScript(spell_warr_charge);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(SPELL_WARRIOR_HEROIC_LEAP_LANDING))
            return SPELL_FAILED_NOT_READY;
        return SPELL_CAST_OK;
    }

    void HandleCharge(SpellEffIndex)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint32 stunSpellId = caster->HasAura(SPELL_WARRIOR_WARBRINGER) ? SPELL_WARRIOR_WARBRINGER_STUN : SPELL_WARRIOR_CHARGE_STUN;
        if (caster->HasAura(SPELL_WARRIOR_WARBRINGER))
            caster->CastSpell(target, SPELL_WARRIOR_WARBRINGER_SLOW, true);

        caster->CastSpell(target, stunSpellId, true);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (!GetCaster()->HasAura(SPELL_WARRIOR_DOUBLE_TIME) || !GetCaster()->HasAura(SPELL_WARRIOR_DOUBLE_TIME_MARKER))
            GetCaster()->EnergizeBySpell(GetCaster(), GetSpellInfo()->Id, GetEffectValue(), POWER_RAGE);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warr_charge::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_warr_charge::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
        OnEffectLaunch += SpellEffectFn(spell_warr_charge::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 145674 - Riposte
class spell_warr_riposte : public SpellScriptLoader
{
    public:
        spell_warr_riposte() : SpellScriptLoader("spell_warr_riposte") { }

        class spell_warr_riposte_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_riposte_AuraScript)

            void calculateAmount(AuraEffect const* , float& amount, bool& canBeRecalculated)
            {
                canBeRecalculated = false;

                Unit* const caster = GetCaster();
                if (!caster)
                    return;

                if (Player* player = caster->ToPlayer())
                {
                    uint32 rating = player->GetUInt32Value((uint16)PLAYER_FIELD_COMBAT_RATINGS + (uint16)CR_PARRY);
                    rating += player->GetUInt32Value((uint16)PLAYER_FIELD_COMBAT_RATINGS + (uint16)CR_DODGE);
                    amount = CalculatePct<float>((float)rating, 75);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_riposte_AuraScript::calculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_riposte_AuraScript();
        }
};

// Called by Revenge and Shield Slam - preventing rage gain bonus if not in Defensive Stance
class spell_warr_revenge_shield_slam : public SpellScriptLoader
{
    public:
        spell_warr_revenge_shield_slam() : SpellScriptLoader("spell_warr_revenge_shield_slam") { }

        class spell_warr_revenge_shield_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_revenge_shield_slam_SpellScript);

            void HandleCast(SpellEffIndex effIndex)
            {
                if (!GetCaster()->HasAura(WARRIOR_SPELL_DEFENSIVE_STANCE))
                    PreventHitEffect(effIndex);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_revenge_shield_slam_SpellScript::HandleCast, EFFECT_1, SPELL_EFFECT_ENERGIZE);
                OnEffectHitTarget += SpellEffectFn(spell_warr_revenge_shield_slam_SpellScript::HandleCast, EFFECT_2, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_revenge_shield_slam_SpellScript();
        }
};

// 7384 - Overpower
class spell_warr_overpower : public SpellScriptLoader
{
    public:
        spell_warr_overpower() : SpellScriptLoader("spell_warr_overpower") { }

        class spell_warr_overpower_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_overpower_SpellScript);

            enum
            {
                MORTAL_STRIKE           = 12294
            };

            void HandleCast(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetCaster()->ToPlayer();
                if (!player)
                    return;

                // Reduce Mortal Strike cooldown by 0.5 sec
                if (player->HasSpellCooldown(MORTAL_STRIKE))
                    player->ModifySpellCooldown(MORTAL_STRIKE, -500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_overpower_SpellScript::HandleCast, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_overpower_SpellScript();
        }
};

class spell_warr_unshackled_fury : public SpellScriptLoader
{
    public:
        spell_warr_unshackled_fury() : SpellScriptLoader("spell_warr_unshackled_fury") { }

        class spell_warr_unshackled_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_unshackled_fury_AuraScript);

            void CalculateAmount(const AuraEffect* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->HasAuraState(AURA_STATE_ENRAGE))
                        amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_unshackled_fury_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warr_unshackled_fury_AuraScript();
        }
};

class spell_warr_glyph_of_burning_anger : public SpellScriptLoader
{
    public: 
        spell_warr_glyph_of_burning_anger() : SpellScriptLoader("spell_warr_glyph_of_burning_anger") { }

        class spell_warr_glyph_of_burning_anger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_glyph_of_burning_anger_SpellScript);

            enum
            {
                SPELL_WARRIOR_GLYPH_OF_BURNING_ANGER = 115946,
                SPELL_WARRIOR_BURNING_ANGER_VISUAL = 115993
            };

            void HandleBurning(SpellEffIndex /*effIndex*/)
            {
                if (auto caster = GetCaster())
                    if (caster->HasAura(SPELL_WARRIOR_GLYPH_OF_BURNING_ANGER))
                        caster->CastSpell(GetHitUnit(), SPELL_WARRIOR_BURNING_ANGER_VISUAL, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_glyph_of_burning_anger_SpellScript::HandleBurning, EFFECT_0, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warr_glyph_of_burning_anger_SpellScript();
        }
};

// 56636 - Taste for Blood
class spell_warr_taste_for_blood : public AuraScript
{
    PrepareAuraScript(spell_warr_taste_for_blood);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor() == eventInfo.GetActionTarget()) // Energize effect
            return false;
        if (eventInfo.GetHitMask() & PROC_EX_DODGE)
            return true;
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_WARR_MORTAL_STRIKE)
            return true;
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 stacksToAdd = 0;
        if (eventInfo.GetHitMask() & PROC_EX_DODGE)
            stacksToAdd = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
        else if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_WARR_MORTAL_STRIKE)
            stacksToAdd = GetSpellInfo()->Effects[EFFECT_1].BasePoints;

        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_AURA_STACK, stacksToAdd);

        Unit* caster = eventInfo.GetActor();
        caster->CastCustomSpell(SPELL_WARR_TASTE_FOR_BLOOD_EFFECT, values, caster, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_taste_for_blood::CheckProc);
        OnProc += AuraProcFn(spell_warr_taste_for_blood::HandleProc);
    }
};

// 60503 - Taste for Blood (buff)
class spell_warr_taste_for_blood_effect : public AuraScript
{
    PrepareAuraScript(spell_warr_taste_for_blood_effect);

    void Prpare(ProcEventInfo&)
    {
        PreventDefaultAction();
    }

    void RemoveStack(ProcEventInfo&)
    {
        GetAura()->ModStackAmount(-1);
    }

    void Register() override
    {
        DoPrepareProc += AuraProcFn(spell_warr_taste_for_blood_effect::Prpare);
        AfterProc += AuraProcFn(spell_warr_taste_for_blood_effect::RemoveStack);
    }
};

// 5302 - Revenge!
class spell_warr_revenge : public SpellScript
{
    PrepareSpellScript(spell_warr_revenge);

    void HandleCast()
    {
        if (Player* caster = GetCaster()->ToPlayer())
            caster->RemoveSpellCooldown(SPELL_WARR_REVENGE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warr_revenge::HandleCast);
    }
};

// 76838 - Mastery: Strikes of Opportunity
class spell_warr_strikes_of_opportunity : public AuraScript
{
    PrepareAuraScript(spell_warr_strikes_of_opportunity);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo())
        {
            if (!eventInfo.TriggeredBySpell())  // Dots
                return;

            if (eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() != eventInfo.GetProcTarget()) // AOE
                return;

            SpellInfo const* procSpell = eventInfo.TriggeredBySpell()->GetSpellInfo();
            if (procSpell->SpellFamilyName != SPELLFAMILY_WARRIOR ||
                procSpell->SpellFamilyFlags & mastery->GetSpellEffectInfo().SpellClassMask) // Where are non damage spells and Deep Wounds.
                return;
        }

        if (eventInfo.GetActionTarget() == GetUnitOwner())
            return;

        if (!roll_chance_f(mastery->GetFloatAmount()))
            return;

        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_WARR_OPPORTUNITY_STRIKE, true, nullptr, mastery);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_strikes_of_opportunity::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 12292 - Bloodbath
class spell_warr_bloodbath : public AuraScript
{
    PrepareAuraScript(spell_warr_bloodbath)

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActionTarget() || eventInfo.GetActor() == eventInfo.GetActionTarget())
            return false;

        return eventInfo.GetSpellInfo() && eventInfo.GetDamageInfo()->GetDamage() != 0;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 damage = eventInfo.GetDamageInfo()->GetDamage();

        Unit* warrior = GetUnitOwner();
        Unit* target = eventInfo.GetActionTarget();
        float amount = CalculatePct(damage, aurEff->GetAmount()) / 6; // tick count
        warrior->CastCustomSpell(SPELL_WARR_BLOODBATH, SPELLVALUE_BASE_POINT0, amount, target, true);
        warrior->CastSpell(target, SPELL_WARR_BLOODBATH_SLOW, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_bloodbath::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_bloodbath::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29725 - Sudden Death
class spell_warr_sudden_death : public AuraScript
{
    PrepareAuraScript(spell_warr_sudden_death);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Player* warrior = GetUnitOwner()->ToPlayer();
        if (!warrior)
            return;

        if ((!eventInfo.GetSpellInfo() || eventInfo.GetSpellInfo()->Id == SPELL_WARR_OPPORTUNITY_STRIKE) && roll_chance_i(10))
        {
            warrior->CastSpell(warrior, SPELL_WARR_SUDDEN_DEATH, true);
            if (warrior->GetTypeId() == TYPEID_PLAYER)
                warrior->RemoveSpellCooldown(SPELL_WARR_COLOSSUS_SMASH, true);
        }

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_WARR_EXECUTE)
            warrior->CastSpell(warrior, SPELL_WARR_SUDDEN_EXECUTE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_sudden_death::HandleProc);
    }
};

// 34428 - Victory Rush
class spell_warr_victory_rush : public SpellScript
{
    PrepareSpellScript(spell_warr_victory_rush);

    void HandleHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            player->CastSpell(player, SPELL_WARRIOR_VICTORY_RUSH_HEAL, true);
            player->RemoveAurasDueToSpell(SPELL_WARRIOR_VICTORIOUS_STATE);
            player->RemoveAurasDueToSpell(SPELL_WARRIOR_VICTORIOUS_STATE_T15);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_victory_rush::HandleHit);
    }
};

// 118779 - Victory Rush
class spell_warr_victory_rush_heal : public SpellScript
{
    PrepareSpellScript(spell_warr_victory_rush_heal);

    void HandleHit()
    {
        if (AuraEffect const* glyph = GetCaster()->GetAuraEffect(SPELL_WARRIOR_GLYPH_OF_VICTORY_RUSH, EFFECT_0))
            SetHitHeal(GetHitHeal() * (1.0f + glyph->GetFloatAmount() / 100.0f));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_victory_rush_heal::HandleHit);
    }
};

// 103840 - Impending Victory
class spell_warr_impending_victory : public SpellScript
{
    PrepareSpellScript(spell_warr_impending_victory);

    void HandleHit()
    {
        CustomSpellValues ctx;
        float mult = 1.0f;
        if (AuraEffect const* victorious = GetCaster()->GetAuraEffect(SPELL_WARRIOR_VICTORIOUS_STATE_T15, EFFECT_0))
        {
            AddPct(mult, victorious->GetAmount());
            GetCaster()->RemoveAura(victorious->GetBase());
        }
        ctx.SetMultiplier(mult);

        GetCaster()->CastCustomSpell(SPELL_WARRIOR_IMPENDING_VICTORY_HEAL, ctx, GetCaster(), TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_impending_victory::HandleHit);
    }
};

// 3411 - Intervene
class spell_warr_intervene : public SpellScript
{
    PrepareSpellScript(spell_warr_intervene);

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();
        if (!target || GetCaster()->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_DONT_REPORT;
        if (target == GetCaster())
            return SPELL_FAILED_BAD_TARGETS;
        Player* player = target->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player || !player->IsInSameRaidWith(GetCaster()->ToPlayer()))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warr_intervene::CheckCast);
    }
};

// 46953 - Sword and Board
class spell_warr_sword_and_board : public AuraScript
{
    PrepareAuraScript(spell_warr_sword_and_board);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& /*eventInfo*/)
    {
        if (roll_chance_i(eff->GetAmount()))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_SWORD_AND_BOARD, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_sword_and_board::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 50227 - Sword and Board
class spell_warr_sword_and_board_buff : public SpellScript
{
    PrepareSpellScript(spell_warr_sword_and_board_buff);

    void ResetCooldown()
    {
        if (Player* warrior = GetCaster()->ToPlayer())
            warrior->RemoveSpellCooldown(SPELL_WARRIOR_SHIELD_SLAM, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warr_sword_and_board_buff::ResetCooldown);
    }
};

// 118000 - Dragon Roar
class spell_warr_dragon_roar : public SpellScript
{
    PrepareSpellScript(spell_warr_dragon_roar);

    uint32 targetCount = 0;

    void CountTargets(std::list<WorldObject*>& targets)
    {
        targetCount = targets.size();
    }

    void HandleHit()
    {
        float damage = GetHitDamage();

        // Negate standard cap
        if (targetCount > 20)
            damage = damage * targetCount / 20;

        switch (targetCount)
        {
            case 1:
                break;
            case 2:
                damage *= 0.75f;
                break;
            case 3:
                damage *= 0.65f;
                break;
            case 4:
                damage *= 0.55f;
                break;
            default:
                if (targetCount >= 5 && targetCount <= 10)
                    damage *= 0.50f;
                else
                    damage = damage * 5 / (float)targetCount;
        }

        SetHitDamage(damage);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_DRAGON_ROAR_KNOCK_BACK, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_dragon_roar::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_warr_dragon_roar::HandleHit);
    }
};

// 6544 - Heroic leap
class spell_warr_heroic_leap : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_leap);

    SpellCastResult CheckElevation()
    {
        Unit* caster = GetCaster();

        WorldLocation const* dest = GetExplTargetDest();
        if (!dest)
            return SPELL_FAILED_DONT_REPORT;

        if (dest->GetPositionZ() > caster->GetPositionZ() + 5.0f)
            return SPELL_FAILED_NOPATH;
        else if (caster->HasAuraType(SPELL_AURA_MOD_ROOT))
            return SPELL_FAILED_ROOTED;
        else if (caster->GetMapId() == 1136 && dest->GetPositionZ() > -308.0f && dest->GetPositionZ() < -290.0f && dest->GetPositionX() > 1900.0f) // hack for siegecrafter blackfuse platforms
            return SPELL_FAILED_NOPATH;

        return SPELL_CAST_OK;
    }

    void HandleHit(SpellEffIndex index)
    {
        // I dunno why leap target is needed, but in sniffs it doesn't appears, but is in execute log.
        // I think, it is generally not needed for client.
        PreventEffect(index);
    }

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARRIOR_HEROIC_LEAP_LANDING, true);
        if (Aura* aura = GetCaster()->GetAura(SPELL_WARRIOR_HEROIC_LEAP_LANDING))
            aura->SetDuration(GetCaster()->movespline->Duration() + 200);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warr_heroic_leap::CheckElevation);
        OnEffectHit += SpellEffectFn(spell_warr_heroic_leap::HandleHit, EFFECT_0, SPELL_EFFECT_SUMMON);
        AfterCast += SpellCastFn(spell_warr_heroic_leap::HandleCast);
    }
};

// 52174 - Heroic Leap 
class spell_warr_heroic_leap_damage : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_leap_damage);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_WARRIOR_PVP_SET_4P))
            GetCaster()->CastSpell(GetCaster(), SPELL_WARRIOR_HEROIC_LEAP_SPEED, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warr_heroic_leap_damage::HandleCast);
    }
};

// 203780 - Server-side aura with AURA_FLAG_INTERRUPT_LANDING
class spell_warr_heroic_leap_landing_arua : public AuraScript
{
    PrepareAuraScript(spell_warr_heroic_leap_landing_arua);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_HEROIC_LEAP_DAMAGE, TRIGGERED_WITH_SPELL_START);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warr_heroic_leap_landing_arua::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 12880 - Enrage
class spell_warr_enrage_raging_blow : public SpellScript
{
    PrepareSpellScript(spell_warr_enrage_raging_blow);

    void HandleHit()
    {
        if (Player* warrior = GetCaster()->ToPlayer())
            if (warrior->GetTalentSpecialization() == SPEC_WARRIOR_FURY && warrior->GetLevel() >= 30)
                warrior->CastSpell(warrior, SPELL_WARRIOR_RAGING_BLOW_ENABLER, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warr_enrage_raging_blow::HandleHit);
    }
};

// 85288 - Raging Blow
class spell_warr_raging_blow : public SpellScript
{
    PrepareSpellScript(spell_warr_raging_blow);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Meat Cleaver chain targets are added to main spell, remove them
        targets.remove_if([this](WorldObject const* target) { return target != GetExplTargetWorldObject(); });
    }

    void HandleHit()
    {
        GetCaster()->RemoveAura(SPELL_WARRIOR_MEAT_CLEAVER);
        if (Aura* ragingBlow = GetCaster()->GetAura(SPELL_WARRIOR_RAGING_BLOW_ENABLER))
            ragingBlow->ModStackAmount(-1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_raging_blow::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_raging_blow::FilterTargets, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
        AfterHit += SpellHitFn(spell_warr_raging_blow::HandleHit);
    }
};


// 29838 - Second Wind
class spell_warr_second_wind : public AuraScript
{
    PrepareAuraScript(spell_warr_second_wind);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HealthBelowPctDamaged(35, eventInfo.GetDamageInfo()->GetDamage()) && !GetUnitOwner()->HasAura(SPELL_WARRIOR_SECOND_WIND_REGEN);
    }

    void HandleProc(ProcEventInfo&)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_SECOND_WIND_REGEN, true);
    }

    void HandlePeriodic(AuraEffect const*)
    {
        if (GetUnitOwner()->HealthBelowPct(35) && !GetUnitOwner()->HasAura(SPELL_WARRIOR_SECOND_WIND_REGEN))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_SECOND_WIND_REGEN, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_second_wind::CheckProc);
        OnProc += AuraProcFn(spell_warr_second_wind::HandleProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_second_wind::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 16491 - Second Wind
class spell_warr_second_wind_regen : public AuraScript
{
    PrepareAuraScript(spell_warr_second_wind_regen);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_SECOND_WIND_DUMMY, true);
    }

    void HandleTick(AuraEffect const*)
    {
        // I dunno wtf is this, but it was in sniffs. No it isn't control aura.
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_SECOND_WIND_DUMMY, true);
        if (!GetUnitOwner()->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
        {
            PreventDefaultAction();
            Remove();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warr_second_wind_regen::HandleApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_second_wind_regen::HandleTick, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 23922 - Shield Slam 
class spell_warr_shield_slam : public SpellScript
{
    PrepareSpellScript(spell_warr_shield_slam);

    void CalculateDamage(SpellEffIndex)
    {
        uint32 level = GetCaster()->GetLevel();
        float ap = GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK);
        float apmult = 0.35f;
        if (level >= 80)
            apmult += 0.4f;
        if (level >= 85)
            apmult += 0.75f;
        SetEffectValue(ap * apmult + GetEffectValue());
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_shield_slam::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 64380 - Shattering Throw, 113000 - Shattering Blow (symbiosis)
class spell_warr_shattering_throw : public SpellScript
{
    PrepareSpellScript(spell_warr_shattering_throw);

    void HandleHit()
    {
        GetHitUnit()->RemoveAurasWithMechanic(1 << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL, 115760); // Glyph of Ice Block
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_shattering_throw::HandleHit);
    }
};

// 86346 - Colossus Smash
class spell_warr_colossus_smash : public SpellScript
{
    PrepareSpellScript(spell_warr_colossus_smash);

    void HandleOnHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PHYSICAL_VULNERABILITY, true);
        if (GetCaster()->HasAura(SPELL_WARRIOR_GLYPH_OF_COLOSSUS_SMASH))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_WEAKENED_ARMOR, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_colossus_smash::HandleOnHit);
    }
};

// 7384 - Overpower
// 100130 - Wild Strike
class spell_warr_glyph_of_die_by_the_sword : public SpellScript
{
    PrepareSpellScript(spell_warr_glyph_of_die_by_the_sword);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_WARRIOR_GLYPH_OF_DIE_BY_THE_SWORD))
        {
            if (Aura* aura = GetCaster()->GetAura(SPELL_WARRIOR_DIE_BY_THE_SWORD))
            {
                int32 add = GetSpellInfo()->Id == SPELL_WARRIOR_OVERPOWER ? 1000 : 500;
                int32 duration = aura->GetDuration() + add;
                aura->SetDuration(duration);
                if (aura->GetMaxDuration() < duration)
                    aura->SetMaxDuration(duration);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warr_glyph_of_die_by_the_sword::HandleCast);
    }
};

// 78 - Heroic Strike
// 845 - Cleave
class spell_warr_heroic_strike_one_hand : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_strike_one_hand);

    void HandleHit()
    {
        if (Player* warrior = GetCaster()->ToPlayer())
            if (Item* item = warrior->GetWeaponForAttack(BASE_ATTACK))
                if (item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
                    SetHitDamage(GetHitDamage() * 1.4f);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_heroic_strike_one_hand::HandleHit);
    }
};

// 12950 - Meat Cleaver
class spell_warr_meat_cleaver : public AuraScript
{
    PrepareAuraScript(spell_warr_meat_cleaver);

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        Player* warrior = GetOwner()->ToPlayer();
        if (!warrior || warrior->HasSpellCooldown(SPELL_WARRIOR_MEAT_CLEAVER))
            return false;
        warrior->AddSpellCooldown(SPELL_WARRIOR_MEAT_CLEAVER, Milliseconds(100));
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_meat_cleaver::CheckProc);
    }
};

// 5246 - Intimidating Shout
class spell_warr_intimidating_shout : public SpellScript
{
    PrepareSpellScript(spell_warr_intimidating_shout);

    void RemoveDeepWounds()
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_WARRIOR_DEEP_WOUNDS, GetCaster()->GetGUID());
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warr_intimidating_shout::RemoveDeepWounds);
    }
};

// 58372 - Glyph of Rude Interruption
class spell_warr_glyph_of_rude_interruption : public AuraScript
{
    PrepareAuraScript(spell_warr_glyph_of_rude_interruption);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.TriggeredBySpell() && eventInfo.TriggeredBySpell()->IsSuccessfulInterrupt();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_glyph_of_rude_interruption::CheckProc);
    }
};

// 115768 - Deep Wounds
class spell_warr_deep_wounds_proc_driver : public AuraScript
{
    PrepareAuraScript(spell_warr_deep_wounds_proc_driver);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_WARRIOR_DEEP_WOUNDS, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_deep_wounds_proc_driver::HandleProc);
    }
};

// 115767 - Deep Wounds
class spell_warr_deep_wounds : public AuraScript
{
    PrepareAuraScript(spell_warr_deep_wounds);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
                if (player->GetTalentSpecialization() == SPEC_WARRIOR_ARMS)
                    amount *= 2;

            // Probably thunder clap and others also, but currently only this
            if (AuraEffect const* effect = caster->GetAuraEffect(SPELL_WARRIOR_SEASONED_SOLDIER, EFFECT_0))
                amount /= (1.0f + effect->GetAmount() / 100.0f);
        }
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_deep_wounds::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 115767 - Deep Wounds
class spell_warr_glyph_of_bloody_healing : public AuraScript
{
    PrepareAuraScript(spell_warr_glyph_of_bloody_healing);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* warrior = GetCaster())
            if (warrior->HasAura(SPELL_WARRIOR_GLYPH_OF_BLOODY_HEALING))
                warrior->CastSpell(warrior, SPELL_WARRIOR_BLOODY_HEALING, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* warrior = GetCaster())
            warrior->RemoveAurasDueToSpell(SPELL_WARRIOR_BLOODY_HEALING);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warr_glyph_of_bloody_healing::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// -746 - First Aid (All ranks)
class spell_warr_glyph_of_bloody_healing_effect : public AuraScript
{
    PrepareAuraScript(spell_warr_glyph_of_bloody_healing_effect);

    enum { SPELL_THE_DOCTOR_IS_IN = 118076 };

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARRIOR_BLOODY_HEALING))
                AddPct(amount, 20);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_glyph_of_bloody_healing_effect::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 871 - Shield Wall
// 23920 - Spell Reflection
class spell_warr_shield_visual : public AuraScript
{
    PrepareAuraScript(spell_warr_shield_visual);

    uint32 spellWithShield = 0;
    uint32 spellAlliance = 0;
    uint32 spellHorde = 0;

    bool Load() override
    {
        spellWithShield = GetSpellInfo()->Id == 871 ? SPELL_WARRIOR_SHIELD_WALL_VISUAL_WITH_SHIELD : SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_WITH_SHIELD;
        spellAlliance   = GetSpellInfo()->Id == 871 ? SPELL_WARRIOR_SHIELD_WALL_VISUAL_ALLIANCE : SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_ALLIANCE;
        spellHorde      = GetSpellInfo()->Id == 871 ? SPELL_WARRIOR_SHIELD_WALL_VISUAL_HORDE : SPELL_WARRIOR_SPELL_REFLECTION_VISUAL_HORDE;
        return true;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Player* warrior = GetUnitOwner()->ToPlayer())
        {
            uint32 spellId = 0;
            if (Item* offhand = warrior->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                if (offhand->GetTemplate()->Class == ITEM_CLASS_ARMOR && offhand->GetTemplate()->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
                    spellId = spellWithShield;

            if (!spellId)
                spellId = warrior->GetRaceMask() & RACEMASK_ALLIANCE ? spellAlliance : spellHorde;

            warrior->CastSpell(warrior, spellId, true);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(spellWithShield);
        GetUnitOwner()->RemoveAurasDueToSpell(spellAlliance);
        GetUnitOwner()->RemoveAurasDueToSpell(spellHorde);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warr_shield_visual::HandleApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warr_shield_visual::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 7381 - Berserker Stance Passive
class spell_warr_berserker_stance_passive : public AuraScript
{
    PrepareAuraScript(spell_warr_berserker_stance_passive);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        float damage = eventInfo.GetDamageInfo()->GetDamage();

        float pct = damage / GetUnitOwner()->GetMaxHealth() * 100.0f;
        GetUnitOwner()->ModifyPower(POWER_RAGE, int32(pct * 10));
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_berserker_stance_passive::HandleProc);
    }
};

// 23920 - Spell Reflection
// 114028 - Mass Spell Reflection
class spell_warr_spell_reflection : public AuraScript
{
    PrepareAuraScript(spell_warr_spell_reflection);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // http://eu.battle.net/wow/en/game/patch-notes/5-3-0
        // Spell Reflection is no longer consumed by spells from player pets and guardians.
        Unit* actor = eventInfo.GetActor();
        return actor && (!actor->GetOwnerGUID() || !actor->GetOwnerGUID().IsPlayer());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_spell_reflection::CheckProc);
    }
};

// 125565 - Demoralizing Shout
class spell_warr_demoralizing_shout : public AuraScript
{
    PrepareAuraScript(spell_warr_demoralizing_shout);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1.0f;
    }

    void HandleAbsorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetAttacker() && dmgInfo.GetAttacker()->HasAura(SPELL_WARRIOR_DEMORALIZING_SHOUT, GetCasterGUID()))
            absorbAmount = (dmgInfo.GetDamage() * 0.2f);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_demoralizing_shout::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_warr_demoralizing_shout::HandleAbsorb, EFFECT_0);
    }
};

// 112048 - Shield Barrier
class spell_warr_shield_barrier : public AuraScript
{
    PrepareAuraScript(spell_warr_shield_barrier);

    void CalculateAmount(AuraEffect const*, float& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        Unit* warrior = GetCaster();
        if (!warrior)
            return;

        int32 rageCost = 0;
        if (Spell const* currentSpell = warrior->FindCurrentSpellBySpellId(SPELL_WARRIOR_SHIELD_BARRIER))
            rageCost = currentSpell->GetPowerCost();

        int32 totalRage = warrior->GetPower(POWER_RAGE) + rageCost;
        int32 consumedRage = std::min(600, totalRage);

        warrior->SetPower(POWER_RAGE, totalRage - consumedRage);

        float ap = warrior->GetTotalAttackPowerValue(BASE_ATTACK);
        float strength = warrior->GetStat(STAT_STRENGTH);
        float stamina = warrior->GetStat(STAT_STAMINA);

        float m2 = GetSpellInfo()->Effects[EFFECT_1].CalcValue(warrior);
        float m3 = GetSpellInfo()->Effects[EFFECT_2].CalcValue(warrior);

        float rageMod = std::floor(consumedRage / 10.0f) / (rageCost / 10.0f);
        amount = std::max((ap - 2 * strength) * m2 / 100.0f, stamina * m3 / 100.0f) / 3.0f * rageMod;
    }

    void HanleAbsorb(AuraEffect*, DamageInfo& damageInfo, uint32& absorbAmount)
    {
        if (AuraEffect const* bonus = GetUnitOwner()->GetAuraEffect(SPELL_WARRIOR_T16_PROTECTION_2P_BONUS, EFFECT_0))
        {
            uint32 absorb = std::min(absorbAmount, damageInfo.GetDamage());
            int32 heal = CalculatePct(absorb, bonus->GetAmount());
            GetUnitOwner()->CastCustomSpell(SPELL_WARRIOR_SHIELD_GUARD, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_shield_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_warr_shield_barrier::HanleAbsorb, EFFECT_0);
    }
};

// 144436 - Item - Warrior T16 DPS 2P Bonus
class spell_warr_t16_dps_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_t16_dps_2p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget() && eventInfo.GetActionTarget()->HasAura(SPELL_WARRIOR_COLOSSUS_SMASH))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_COLOSSAL_RAGE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_t16_dps_2p_bonus::HandleProc);
    }
};

// 144441 - Item - Warrior T16 DPS 4P Bonus
class spell_warr_t16_dps_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_t16_dps_4p_bonus);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        // Energize effects
        if (eventInfo.GetActionTarget() == GetUnitOwner())
            return;

        if (roll_chance_i(effect->GetAmount()))
        {
            if (eventInfo.GetActionTarget()->HealthAbovePct(20))
                GetUnitOwner()->CastCustomSpell(SPELL_WARRIOR_DEATH_SENTENCE, SPELLVALUE_BASE_POINT1, 0, GetUnitOwner(), true);
            else
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_WARRIOR_DEATH_SENTENCE, true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_t16_dps_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 144503 - Item - Warrior T16 Protection 2P Bonus
class spell_warr_t16_protection_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_t16_protection_2p_bonus);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        uint32 blocked = 0;
        if (eventInfo.GetProcTriggerContext()->MeleeDamage)
            blocked += eventInfo.GetProcTriggerContext()->MeleeDamage->blocked_amount;
        else if (eventInfo.GetProcTriggerContext()->SpellDamage)
            blocked += eventInfo.GetProcTriggerContext()->SpellDamage->blocked;

        if (blocked)
        {
            int32 heal = CalculatePct(blocked, effect->GetAmount());
            GetUnitOwner()->CastCustomSpell(SPELL_WARRIOR_SHIELD_GUARD, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_t16_protection_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1160 - Demoralizing Shout
class spell_warr_t16_protection_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_t16_protection_4p_bonus);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* warrior = GetCaster();
        if (warrior && warrior->GetTypeId() == TYPEID_PLAYER)
        {
            if (warrior->HasAura(SPELL_WARRIOR_T16_PROTECTION_4P_BONUS) && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
            {
                // Cast only once
                if (warrior->ToPlayer()->HasSpellCooldown(SPELL_WARRIOR_RECKLESS_DEFENSE))
                    return;
                warrior->CastSpell(warrior, SPELL_WARRIOR_RECKLESS_DEFENSE, true);
                warrior->ToPlayer()->AddSpellCooldown(SPELL_WARRIOR_RECKLESS_DEFENSE, Milliseconds(100));
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warr_t16_protection_4p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 144500 - Reckless Defense
class spell_warr_reckless_defence : public AuraScript
{
    PrepareAuraScript(spell_warr_reckless_defence);

    float accumulator = 0.0f;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        accumulator += eventInfo.GetDamage() * 100.0f / GetUnitOwner()->GetMaxHealth();
        if (accumulator >= 1.0f)
        {
            int32 rage = accumulator;
            accumulator -= rage;
            GetUnitOwner()->CastCustomSpell(SPELL_WARRIOR_RECKLESS_DEFENSE_ENERGIZE, SPELLVALUE_BASE_POINT0, rage * 10, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_reckless_defence::HandleProc);
    }
};

// 12328 - Sweeping Strikes
class spell_warr_sweeping_strikes : public AuraScript
{
    PrepareAuraScript(spell_warr_sweeping_strikes);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        Unit* target = GetUnitOwner()->SelectNearbyTarget(eventInfo.GetActionTarget());
        if (!target)
            return;

        Unit* warrior = GetUnitOwner();

        bool bladeStormProc = false;
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == 50622 && warrior->GetTypeId() == TYPEID_PLAYER)
        {
            if (warrior->ToPlayer()->HasSpellCooldown(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK))
                return;

            bladeStormProc = true;
        }

        // Glyph of Sweeping Strikes
        if (warrior->HasAura(SPELL_WARRIOR_GLYPH_OF_SWEEPING_STRIKES))
            warrior->CastSpell(warrior, SPELL_WARRIOR_GLYPH_OF_SWEEPING_STRIKES_PROC, true);

        int32 damage = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), effect->GetAmount());

        damage /= GetDamageTakenCorrectiveFactor(warrior, eventInfo.GetActionTarget(), eventInfo.GetSpellInfo());

        warrior->CastCustomSpell(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK, SPELLVALUE_BASE_POINT0, damage, target, true, nullptr, effect);

        // Bladestorm must tick once per rotation
        if (bladeStormProc)
            warrior->ToPlayer()->AddSpellCooldown(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK, Milliseconds(500));

        // Slam bonus
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_WARRIOR_SLAM)
        {
            int32 slamDamage = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), 35);
            warrior->CastCustomSpell(SPELL_WARRIOR_SLAM_PROC, SPELLVALUE_BASE_POINT0, slamDamage, target, true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_sweeping_strikes::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 58385 - Glyph of Hamstring
class spell_warr_glyph_of_hamstring : public AuraScript
{
    PrepareAuraScript(spell_warr_glyph_of_hamstring);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;

        return eventInfo.TriggeredBySpell()->GetPowerCost() > 0;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_glyph_of_hamstring::CheckProc);
    }
};

// 126661 - Warrior Charge Drop Fire Periodic
class spell_warr_charge_drop_fire_periodic : public AuraScript
{
    PrepareAuraScript(spell_warr_charge_drop_fire_periodic);

    std::vector<G3D::Vector3> points;
    size_t lastIndex = 0;
    bool initialized = false;

    void HandlePeriodic(AuraEffect const*)
    {
        PreventDefaultAction();

        // Can't do it in apply handler because spline is not launched yet.
        if (!initialized)
        {
            if (!GetUnitOwner()->movespline->Initialized())
                return;

            initialized = true;


            // Not blizzlike :(
            if (!GetCaster()->GetSplineDuration())
                return;

            GetAura()->SetDuration(GetCaster()->GetSplineDuration());
            GetAura()->SetMaxDuration(GetCaster()->GetSplineDuration());

            std::vector<G3D::Vector3> path = GetUnitOwner()->movespline->_Spline().getPoints();
            path.erase(path.begin());
            path.pop_back();

            Movement::MoveSplineInitArgs args;
            args.HasVelocity = true;
            args.velocity = 1.0f;
            args.path = path;

            Movement::MoveSpline spline;
            spline.Initialize(args);

            while (!spline.Finalized())
            {
                spline.updateState(1000);
                points.push_back(spline.ComputePosition());
            }
        }

        if (points.empty())
            return;

        float progress = float(GetAura()->GetMaxDuration() - GetAura()->GetDuration()) / float(GetAura()->GetMaxDuration());

        size_t newIndex = size_t(points.size() * progress);

        // Another hack. Trail shouldn't be under the player's feet, only behind
        if (newIndex < 2)
            return;

        newIndex -= 1;

        for (size_t i = lastIndex; i < newIndex ; ++i)
            GetUnitOwner()->SendPlaySpellVisual(26423, points[i].x, points[i].y, points[i].z, 1.0f);

        lastIndex = newIndex;
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_charge_drop_fire_periodic::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 100 - Charge
class spell_warr_glyph_of_blazing_trail : public SpellScript
{
    PrepareSpellScript(spell_warr_glyph_of_blazing_trail);

    void HandleGlyph(SpellEffIndex index)
    {
        if (!GetCaster()->HasAura(SPELL_WARRIOR_GLYPH_OF_THE_BLAZING_TRAIL))
            PreventEffect(index);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_warr_glyph_of_blazing_trail::HandleGlyph, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

void AddSC_warrior_spell_scripts()
{
    new spell_script<spell_warr_victorious_state>("spell_warr_victorious_state");
    new spell_warr_glyph_of_hindering_strikes();
    new spell_warr_stampeding_shout();
    new spell_warr_shield_block();
    new spell_script<spell_warr_storm_bolt>("spell_warr_storm_bolt");
    new spell_warr_staggering_shout();
    new spell_warr_unbridled_wrath();
    new spell_warr_berserker_rage();
    new spell_warr_mocking_banner();
    new spell_warr_mortal_strike();
    new spell_warr_rallying_cry();
    new spell_script<spell_warr_shockwave>("spell_warr_shockwave");
    new spell_warr_bloodthirst();
    new spell_warr_last_stand();
    new spell_warr_thunder_clap();
    new spell_script<spell_warr_charge>("spell_warr_charge");
    new spell_warr_riposte();
    new spell_warr_revenge_shield_slam();
    new spell_warr_overpower();
    new spell_warr_unshackled_fury();
    new spell_warr_storm_bolt_damage();
    new spell_warr_glyph_of_burning_anger();
    new aura_script<spell_warr_taste_for_blood>("spell_warr_taste_for_blood");
    new aura_script<spell_warr_taste_for_blood_effect>("spell_warr_taste_for_blood_effect");
    new spell_script<spell_warr_revenge>("spell_warr_revenge");
    new aura_script<spell_warr_strikes_of_opportunity>("spell_warr_strikes_of_opportunity");
    new aura_script<spell_warr_bloodbath>("spell_warr_bloodbath");
    new aura_script<spell_warr_sudden_death>("spell_warr_sudden_death");
    new spell_script<spell_warr_victory_rush>("spell_warr_victory_rush");
    new spell_script<spell_warr_victory_rush_heal>("spell_warr_victory_rush_heal");
    new spell_script<spell_warr_impending_victory>("spell_warr_impending_victory");
    new spell_script<spell_warr_intervene>("spell_warr_intervene");
    new aura_script<spell_warr_sword_and_board>("spell_warr_sword_and_board");
    new spell_script<spell_warr_sword_and_board_buff>("spell_warr_sword_and_board_buff");
    new spell_script<spell_warr_dragon_roar>("spell_warr_dragon_roar");
    new spell_script<spell_warr_heroic_leap>("spell_warr_heroic_leap");
    new spell_script<spell_warr_heroic_leap_damage>("spell_warr_heroic_leap_damage");
    new aura_script<spell_warr_heroic_leap_landing_arua>("spell_warr_heroic_leap_landing_arua");
    new spell_script<spell_warr_enrage_raging_blow>("spell_warr_enrage_raging_blow");
    new spell_script<spell_warr_raging_blow>("spell_warr_raging_blow");
    new aura_script<spell_warr_second_wind>("spell_warr_second_wind");
    new aura_script<spell_warr_second_wind_regen>("spell_warr_second_wind_regen");
    new spell_script<spell_warr_shield_slam>("spell_warr_shield_slam");
    new spell_script<spell_warr_shattering_throw>("spell_warr_shattering_throw");
    new spell_script<spell_warr_colossus_smash>("spell_warr_colossus_smash");
    new spell_script<spell_warr_glyph_of_die_by_the_sword>("spell_warr_glyph_of_die_by_the_sword");
    new spell_script<spell_warr_heroic_strike_one_hand>("spell_warr_heroic_strike_one_hand");
    new aura_script<spell_warr_meat_cleaver>("spell_warr_meat_cleaver");
    new spell_script<spell_warr_intimidating_shout>("spell_warr_intimidating_shout");
    new aura_script<spell_warr_glyph_of_rude_interruption>("spell_warr_glyph_of_rude_interruption");
    new aura_script<spell_warr_deep_wounds_proc_driver>("spell_warr_deep_wounds_proc_driver");
    new aura_script<spell_warr_deep_wounds>("spell_warr_deep_wounds");
    new aura_script<spell_warr_glyph_of_bloody_healing>("spell_warr_glyph_of_bloody_healing");
    new aura_script<spell_warr_glyph_of_bloody_healing_effect>("spell_warr_glyph_of_bloody_healing_effect");
    new aura_script<spell_warr_shield_visual>("spell_warr_shield_visual");
    new aura_script<spell_warr_berserker_stance_passive>("spell_warr_berserker_stance_passive");
    new aura_script<spell_warr_spell_reflection>("spell_warr_spell_reflection");
    new aura_script<spell_warr_demoralizing_shout>("spell_warr_demoralizing_shout");
    new aura_script<spell_warr_shield_barrier>("spell_warr_shield_barrier");
    new aura_script<spell_warr_t16_dps_2p_bonus>("spell_warr_t16_dps_2p_bonus");
    new aura_script<spell_warr_t16_dps_4p_bonus>("spell_warr_t16_dps_4p_bonus");
    new aura_script<spell_warr_t16_protection_2p_bonus>("spell_warr_t16_protection_2p_bonus");
    new aura_script<spell_warr_t16_protection_4p_bonus>("spell_warr_t16_protection_4p_bonus");
    new aura_script<spell_warr_reckless_defence>("spell_warr_reckless_defence");
    new aura_script<spell_warr_sweeping_strikes>("spell_warr_sweeping_strikes");
    new aura_script<spell_warr_glyph_of_hamstring>("spell_warr_glyph_of_hamstring");
    new aura_script<spell_warr_charge_drop_fire_periodic>("spell_warr_charge_drop_fire_periodic");
    new spell_script<spell_warr_glyph_of_blazing_trail>("spell_warr_glyph_of_blazing_trail");
}
