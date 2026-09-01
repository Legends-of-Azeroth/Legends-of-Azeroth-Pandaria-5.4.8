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
 * Scripts for spells with SPELLFAMILY_MONK and SPELLFAMILY_GENERIC spells used by monk players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_monk_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "PetAI.h"
#include "Group.h"
#include "spell_common.h"
#include "Battleground.h"
#include "Unit.h"
#include "Random.h"

enum MonkSpells
{
    SPELL_MONK_LEGACY_OF_THE_EMPEROR_SINGLE     = 117667,
    SPELL_MONK_LEGACY_OF_THE_EMPEROR_RAID       = 117666,
    SPELL_MONK_FORTIFYING_BREW                  = 120954,
    SPELL_MONK_PROVOKE                          = 118635,
    SPELL_MONK_BLACKOUT_KICK                    = 100784,
    SPELL_MONK_BLACKOUT_KICK_DOT                = 128531,
    SPELL_MONK_BLACKOUT_KICK_HEAL               = 128591,
    SPELL_MONK_SHUFFLE                          = 115307,
    SPELL_MONK_ZEN_PILGRIMAGE                   = 126892,
    SPELL_MONK_ZEN_PILGRIMAGE_RETURN            = 126895,
    SPELL_MONK_DISABLE_ROOT                     = 116706,
    SPELL_MONK_DISABLE                          = 116095,
    SPELL_MONK_SOOTHING_MIST                    = 115175,
    SPELL_MONK_SOOTHING_MISTS_MISSILE           = 117899,
    SPELL_MONK_SOOTHING_MIST_ENERGIZE           = 116335,
    SPELL_MONK_SOOTHING_MIST_STATUE_TRIGGER     = 125962,
    SPELL_MONK_SOOTHING_MIST_STATUE_CHANNEL     = 125950,
    SPELL_MONK_SOOTHING_MIST_STATUE_HEAL        = 125953,
    SPELL_MONK_BREATH_OF_FIRE_DOT               = 123725,
    SPELL_MONK_BREATH_OF_FIRE_CONFUSED          = 123393,
    SPELL_MONK_ELUSIVE_BREW_STACKS              = 128939,
    SPELL_MONK_ELUSIVE_BREW                     = 115308,
    SPELL_MONK_KEG_SMASH_VISUAL                 = 123662,
    SPELL_MONK_KEG_SMASH_ENERGIZE               = 127796,
    SPELL_MONK_WEAKENED_BLOWS                   = 115798,
    SPELL_MONK_DIZZYING_HAZE                    = 116330,
    SPELL_MONK_STAGGER                          = 124255,
    SPELL_MONK_LIGHT_STAGGER                    = 124275,
    SPELL_MONK_MODERATE_STAGGER                 = 124274,
    SPELL_MONK_HEAVY_STAGGER                    = 124273,
    SPELL_MONK_ROLL                             = 109132,
    SPELL_MONK_ROLL_AURA                        = 107427,
    SPELL_MONK_CHI_TORPEDO_HEAL                 = 124040,
    SPELL_MONK_CHI_TORPEDO_DAMAGE               = 117993,
    SPELL_MONK_FLYING_SERPENT_KICK              = 101545,
    SPELL_MONK_FLYING_SERPENT_KICK_TRIGGER      = 115057,
    SPELL_MONK_FLYING_SERPENT_KICK_AOE          = 123586,
    SPELL_MONK_TIGEREYE_BREW                    = 116740,
    SPELL_MONK_TIGEREYE_BREW_STACKS             = 125195,
    SPELL_MONK_PLUS_ONE_TIGEREYE_BREW           = 125196,
    SPELL_MONK_TIGEREYE_BREW_FULL_STACKS_VISUAL = 137591,
    SPELL_MONK_SPEAR_HAND_STRIKE_SILENCE        = 116709,
    SPELL_MONK_CHI_BURST_DAMAGE                 = 148135,
    SPELL_MONK_CHI_BURST_HEAL                   = 130654,
    SPELL_MONK_ZEN_SPHERE_DAMAGE                = 124098,
    SPELL_MONK_ZEN_SPHERE_HEAL                  = 124081,
    SPELL_MONK_ZEN_SPHERE_DETONATE_HEAL         = 124101,
    SPELL_MONK_ZEN_SPHERE_DETONATE_DAMAGE       = 125033,
    SPELL_MONK_HEALING_ELIXIRS_AURA             = 122280,
    SPELL_MONK_HEALING_ELIXIRS_RESTORE_HEALTH   = 122281,
    SPELL_MONK_RENEWING_MIST                    = 115151,
    SPELL_MONK_RENEWING_MIST_HOT                = 119611,
    SPELL_MONK_RENEWING_MIST_JUMP               = 119607,
    SPELL_MONK_GLYPH_OF_RENEWING_MIST           = 123334,
    SPELL_MONK_SURGING_MIST_HEAL                = 116995,
    SPELL_MONK_ENVELOPING_MIST_HEAL             = 132120,
    SPELL_MONK_PLUS_ONE_MANA_TEA                = 123760,
    SPELL_MONK_MANA_TEA_STACKS                  = 115867,
    SPELL_MONK_MANA_TEA_REGEN                   = 115294,
    SPELL_MONK_TEACHINGS_OF_THE_MONASTERY       = 118672,
    SPELL_MONK_SPINNING_CRANE_KICK              = 101546,
    SPELL_MONK_SPINNING_CRANE_KICK_HEAL         = 117640,
    SPELL_MONK_SPINNING_CRANE_KICK_ENERGIZE     = 129881,
    SPELL_MONK_UPLIFT                           = 116670,
    SPELL_MONK_UPLIFT_ALLOWING_CAST             = 123757,
    SPELL_MONK_THUNDER_FOCUS_TEA                = 116680,
    SPELL_MONK_PATH_OF_BLOSSOM_AREATRIGGER      = 122035,
    SPELL_MONK_SPINNING_FIRE_BLOSSOM_DAMAGE     = 123408,
    SPELL_MONK_SPINNING_FIRE_BLOSSOM_MISSILE    = 118852,
    SPELL_MONK_SPINNING_FIRE_BLOSSOM_ROOT       = 123407,
    SPELL_MONK_TOUCH_OF_KARMA_REDIRECT_DAMAGE   = 124280,
    SPELL_TOUCH_OF_KARMA_INDICATOR              = 125174,
    SPELL_MONK_JADE_LIGHTNING_ENERGIZE          = 123333,
    SPELL_MONK_CHI_SPHERE                       = 121286,
    SPELL_MONK_CHI_SHERE_ENERGIZE               = 121283,
    SPELL_MONK_GLYPH_OF_ZEN_FLIGHT              = 125893,
    SPELL_MONK_ZEN_FLIGHT                       = 125883,
    SPELL_MONK_BEAR_HUG                         = 127361,
    SPELL_MONK_ITEM_2_S12_MISTWEAVER            = 131561,
    SPELL_MONK_ITEM_4_S12_MISTWEAVER            = 124487,
    SPELL_MONK_ZEN_FOCUS                        = 124488,
    SPELL_MONK_GRAPPLE_WEAPON_DPS_UPGRADE       = 123231,
    SPELL_MONK_GRAPPLE_WEAPON_TANK_UPGRADE      = 123232,
    SPELL_MONK_GRAPPLE_WEAPON_HEAL_UPGRADE      = 123234,
    SPELL_MONK_GLYPH_OF_BLACKOUT_KICK           = 132005,
    SPELL_MONK_CHI_WAVE_HEAL                    = 132463,
    SPELL_MONK_CHI_WAVE_DAMAGE                  = 132467,
    SPELL_MONK_CHI_WAVE_HEALING_BOLT            = 132464,
    SPELL_MONK_CHI_WAVE_TARGET_SELECTOR         = 132466,
    SPELL_MONK_ITEM_PVP_GLOVES_BONUS            = 124489,
    SPELL_MONK_GLYPH_OF_DETOX                   = 146954,
    SPELL_MONK_RISING_SUN_KICK_DEBUFF           = 130320,
    SPELL_MONK_COMBO_BREAKER_PALM               = 118864,
    SPELL_MONK_COMBO_BREAKER_KICK               = 116768,
    SPELL_MONK_EXPEL_HARM_DAMAGE                = 115129,
    SPELL_MONK_HEALING_SPHERE                   = 115460,
    SPELL_MONK_HEALING_SPHERE_HEAL              = 115464,
    SPELL_MONK_HEALING_SPHERE_STACK             = 124458,
    SPELL_MONK_HEALING_SPHERE_EXPIRE_HEAL       = 135914,
    SPELL_MONK_GIFT_OF_THE_OX_LEFT              = 124506,
    SPELL_MONK_GIFT_OF_THE_OX_RIGHT             = 124503,
    SPELL_MONK_GIFT_OF_THE_OX_HEAL              = 124507,
    SPELL_MONK_GLYPH_OF_FORTIFYING_BREW         = 124997,
    SPELL_MONK_ADAPTATION                       = 126050,
    SPELL_MONK_CRACKLING_JADE_LIGHTNING         = 117952,
    SPELL_MONK_CRACKLING_JADE_LIGHTNING_COOLDOWN= 117953,
    SPELL_MONK_CRACKLING_JADE_SHOCK             = 117962,
    SPELL_MONK_MASTERY_BOTTELED_FURY            = 115636,
    SPELL_MONK_POWER_STRIKES_TALENT             = 121817,
    SPELL_MONK_POWER_STRIKES                    = 129914,
    SPELL_MONK_EMINENCE                         = 126890,
    SPELL_MONK_EMINENCE_STATUE                  = 117895,
    SPELL_MONK_SERPENTS_ZEAL                    = 127722,
    SPELL_MONK_MUSCLE_MEMORY                    = 139598,
    SPELL_MONK_MUSCLE_MEMORY_BUFF               = 139597,
    SPELL_MONK_SANCTUARY_OF_THE_OX              = 126119,
    SPELL_MONK_BLACK_OX_STATUE_GUARD_SELECTOR   = 118605,
    SPELL_MONK_BLACK_OX_STATUE_GUARD            = 118604,
    SPELL_MONK_BLACK_OX_STATUE_GUARD_MAGIC      = 136070,
    SPELL_MONK_GLYPH_OF_GUARD                   = 123401,
    SPELL_MONK_POWER_GUARD                      = 118636,
    SPELL_MONK_GLYPH_OF_SPIRIT_ROLL             = 125154,
    SPELL_MONK_GLYPH_OF_WATER_ROLL              = 125901,
    SPELL_MONK_LEER_OF_THE_OX                   = 118585,
    SPELL_MONK_GIFT_OF_THE_SERPENT_SELECTOR     = 119036,
    SPELL_MONK_GIFT_OF_THE_SERPENT_SUMMON       = 119031,
    SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL         = 124041,
    SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL_EXPIRED = 135920,
    SPELL_MONK_GLYPH_OF_LIFE_COCOON             = 124989,
    SPELL_MONK_GLYPH_OF_NIMBLE_BREW             = 146952,
    SPELL_MONK_WINDWALKER_PVP_P4                = 124490,
    SPELL_MONK_STANCE_OF_THE_WISE_SERPENT_HASTE = 136336,
    SPELL_MONK_SEF_SUMMON_STORM_SPIRIT          = 138122,
    SPELL_MONK_SEF_SUMMON_EARTH_SPIRIT          = 138121,
    SPELL_MONK_SEF_SUMMON_FIRE_SPIRIT           = 138123,
    SPELL_MONK_STORM_EARTH_AND_FIRE             = 137639,
    SPELL_MONK_HEALING_ELIXIRS_TIMER            = 122280,
    SPELL_MONK_HEALING_ELIXIRS                  = 134563,
    SPELL_MONK_INTERNAL_MEDICINE                = 115451,
    SPELL_MONK_RING_OF_PEACE_AURA               = 140023,
    SPELL_MONK_RING_OF_PEACE_SILENCE            = 137460,
    SPELL_MONK_RING_OF_PEACE_DISARM             = 137461,
    SPELL_MONK_RESTLESS_PURSUIT                 = 124489,
    SPELL_MONK_MANA_TEA_DISPEL                  = 131561,
    SPELL_MONK_MANA_TEA_DRIVER                  = 115869,
    SPELL_MONK_STANCE_OF_THE_WISE_SERPENT       = 115070,
    SPELL_MONK_SPARRING                         = 116033,
    SPELL_MONK_SPARRING_COOLDOWN_MARKER         = 116087,
    SPELL_MONK_T15_MISTWEAVER_2P_BONUS          = 138290,
    SPELL_MONK_ENERGY_SPHERE_SUMMON_LEFT        = 138310,
    SPELL_MONK_ENERGY_SPHERE_SUMMON_RIGHT       = 140674,
    SPELL_MONK_ENERGY_SPHERE                    = 138311,
    SPELL_MONK_T15_WINDWALKER_4P_BONUS          = 138315,
    SPELL_MONK_T15_BREWMASTER_2P_BONUS          = 138231,
    SPELL_MONK_T15_BREWMASTER_4P_BONUS          = 138236,
    SPELL_MONK_STAGGERING                       = 138233,
    SPELL_MONK_T16_WINDWALKER_2P_BONUS          = 145004,
    SPELL_MONK_FOCUS_OF_XUEN                    = 145024,
    SPELL_MONK_PROTECTION_OF_NIUZAO             = 145051,
    SPELL_MONK_PURIFIED_HEALING                 = 145056,
    SPELL_MONK_YULONS_BARRIER                   = 145441,
    SPELL_MONK_MIST_WAVE                        = 145450,
    SPELL_MONK_AFTERLIFE_SUMMON                 = 117032,
    SPELL_MONK_AFTERLIFE_HEALING_SPHERE         = 125355,
    SPELL_MONK_WEAPON_OVERRIDE_DRIVER           = 108562,
    SPELL_MONK_GLYPH_OF_JAB                     = 125660,
    SPELL_MONK_2H_STAFF_OVERRIDE                = 108561,
    SPELL_MONK_2H_POLEARM_OVERRIDE              = 115697,
    SPELL_MONK_DW_AXES                          = 115689,
    SPELL_MONK_DW_MACES                         = 115694,
    SPELL_MONK_DW_SWORDS                        = 115696,
    SPELL_MONK_DEATH_NOTE                       = 121125,
    SPELL_MONK_TOUCH_OF_DEATH                   = 115080,

    SPELL_FLYING_KICK                           = 138104,
};

enum MonkSummons
{
    NPC_JADE_SERPENT_STATUE                     = 60849,
    NPC_BLACK_OX_STATUE                         = 61146,
    NPC_STORM_SPIRIT                            = 69680,
    NPC_EARTH_SPIRIT                            = 69792,
    NPC_FIRE_SPIRIT                             = 69791,
    NPC_TRANSCENDENCE_MARKER                    = 54569,
};

// 115098 - Chi Wave
class spell_monk_chi_wave : public SpellScript
{
    PrepareSpellScript(spell_monk_chi_wave);

    SpellCastResult CheckCast()
    {
        if (!GetCaster()->IsValidAttackTarget(GetExplTargetUnit(), GetSpellInfo()) && !GetCaster()->IsValidAssistTarget(GetExplTargetUnit()))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        if (Unit* target = GetExplTargetUnit())
        {
            uint32 spell = GetCaster()->IsValidAssistTarget(target) ? SPELL_MONK_CHI_WAVE_HEALING_BOLT : SPELL_MONK_CHI_WAVE_DAMAGE;
            GetCaster()->CastCustomSpell(spell, SPELLVALUE_BASE_POINT1, 7, target, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_chi_wave::CheckCast);
        OnCast += SpellCastFn(spell_monk_chi_wave::HandleCast);
    }
};

// 132464, 132467 - Chi Wave
class spell_monk_chi_wave_bolt : public SpellScript
{
    PrepareSpellScript(spell_monk_chi_wave_bolt);

    void HandleHit()
    {
        if (Unit* monk = GetOriginalCaster())
        {
            if (GetSpellInfo()->Id == SPELL_MONK_CHI_WAVE_HEALING_BOLT)
                monk->CastSpell(GetHitUnit(), SPELL_MONK_CHI_WAVE_HEAL, true);
            if (Aura const* hitAura = GetHitAura())
                if (AuraEffect const* eff = hitAura->GetEffect(EFFECT_1))
                    GetHitUnit()->CastCustomSpell(SPELL_MONK_CHI_WAVE_TARGET_SELECTOR, SPELLVALUE_BASE_POINT1, eff->GetAmount(),
                        GetHitUnit(), true, nullptr, eff, monk->GetGUID());
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_chi_wave_bolt::HandleHit);
    }
};

// 132466 - Chi Wave
class spell_monk_chi_wave_target_selector : public spell_smart_heal
{
    PrepareSpellScript(spell_monk_chi_wave_target_selector);

    int32 m_counter = 0; 

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_counter = GetSpellValue()->EffectBasePoints[EFFECT_1];
        auto trigger = GetSpell()->TriggeredByAuraSpell();
        if (!m_counter || !trigger)
        {
            targets.clear();
            return;
        }
        --m_counter;
        Unit* monk = GetOriginalCaster();
        bool isHostile = trigger->Id == SPELL_MONK_CHI_WAVE_HEALING_BOLT;
        auto damageSpell = sSpellMgr->GetSpellInfo(SPELL_MONK_CHI_WAVE_DAMAGE);
        targets.remove_if([=](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            if (!unit)
                return true;
            if (isHostile)
            {
                if (!monk->IsValidAttackTarget(unit, damageSpell))
                    return true;
            }
            else if (!monk->IsValidAssistTarget(unit))
                return true;

            return false;
        });

        if (!targets.empty())
        {
            targets.sort(Trinity::ObjectDistanceOrderPred{ GetCaster() });
            if (isHostile)
                targets.resize(1);
            else
                ApplySmartHeal(targets, 1);
        }
    }

    void HandleHit(SpellEffIndex)
    {
        auto trigger = GetSpell()->TriggeredByAuraSpell();
        Unit* monk = GetOriginalCaster();
        if (trigger && monk)
        {
            uint32 spell = trigger->Id == SPELL_MONK_CHI_WAVE_HEALING_BOLT ? SPELL_MONK_CHI_WAVE_DAMAGE : SPELL_MONK_CHI_WAVE_HEALING_BOLT;
            // The bolt must fly from the previous target (this is our current caster)
            GetCaster()->CastCustomSpell(spell, SPELLVALUE_BASE_POINT1, m_counter, GetHitUnit(), true, nullptr, nullptr, monk->GetGUID());
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_chi_wave_target_selector::SelectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_monk_chi_wave_target_selector::HandleHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Grapple Weapon - 117368
class spell_monk_grapple_weapon : public SpellScriptLoader
{
    public:
        spell_monk_grapple_weapon() : SpellScriptLoader("spell_monk_grapple_weapon") { }

        class spell_monk_grapple_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_grapple_weapon_SpellScript)

            void HandleBeforeHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->ToPlayer())
                        {
                            Item* mainItem = _player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                            Item* targetMainItem = target->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

                            if (mainItem && targetMainItem)
                            {
                                if (targetMainItem->GetTemplate()->ItemLevel > mainItem->GetTemplate()->ItemLevel)
                                {
                                    switch (_player->GetTalentSpecialization())
                                    {
                                        case SPEC_MONK_BREWMASTER:
                                            _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_TANK_UPGRADE, true);
                                            break;
                                        case SPEC_MONK_MISTWEAVER:
                                            _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_HEAL_UPGRADE, true);
                                            break;
                                        case SPEC_MONK_WINDWALKER:
                                            _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_DPS_UPGRADE, true);
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            }
                        }
                        else if (target->GetTypeId() == TYPEID_UNIT)
                        {
                            if (target->GetLevel() > _player->GetLevel())
                            {
                                switch (_player->GetTalentSpecialization())
                                {
                                    case SPEC_MONK_BREWMASTER:
                                        _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_TANK_UPGRADE, true);
                                        break;
                                    case SPEC_MONK_MISTWEAVER:
                                        _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_HEAL_UPGRADE, true);
                                        break;
                                    case SPEC_MONK_WINDWALKER:
                                        _player->CastSpell(_player, SPELL_MONK_GRAPPLE_WEAPON_DPS_UPGRADE, true);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_monk_grapple_weapon_SpellScript::HandleBeforeHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_grapple_weapon_SpellScript();
        }
};


// 101643 - Transcendence
class spell_monk_transcendence : public SpellScript
{
    PrepareSpellScript(spell_monk_transcendence);

    void RemoveOldPoint()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_TRANSCENDENCE_MARKER);
        if (!summons.empty())
            summons.front()->DespawnOrUnsummon();
    }

    void HandleCast()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_TRANSCENDENCE_MARKER);
        if (!summons.empty())
        {
            Unit* copy = summons.front();
            copy->SetUInt64Value(UNIT_FIELD_DEMON_CREATOR, GetCaster()->GetGUID());
            copy->SetUInt64Value(UNIT_FIELD_CREATED_BY, GetCaster()->GetGUID());
            GetCaster()->CastSpell(copy, 119051, true); // Transcendence Clone Target
            copy->m_Events.Schedule(1, [=]
            {
                copy->CastSpell(copy, 119052, false);   // Transcendence Clone Target Trigger
                copy->CastSpell(copy, 119053, false);   // Transcendence Clone Visual
                copy->PlayOneShotAnimKitId(2223);       // Levitating in Lotus Pose
                copy->SetAIAnimKitId(2223);
            });
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_monk_transcendence::RemoveOldPoint);
        AfterCast += SpellCastFn(spell_monk_transcendence::HandleCast);
    }
};

// 119996 - Transcendence: Transfer
class spell_monk_transcendence_transfer : public SpellScript
{
    PrepareSpellScript(spell_monk_transcendence_transfer);

    SpellCastResult CheckCast()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_TRANSCENDENCE_MARKER);
        if (summons.empty())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        return GetCaster()->GetDistance(summons.front()) > 40.0f ? SPELL_FAILED_OUT_OF_RANGE : SPELL_CAST_OK;
    }

    void HandleCast()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_TRANSCENDENCE_MARKER);
        if (!summons.empty())
        {
            TempSummon* copy = summons.front();
            Position casterPos = GetCaster()->GetPosition();
            Position copyPos = copy->GetPosition();
            GetCaster()->NearTeleportTo(copyPos.GetPositionX(), copyPos.GetPositionY(), copyPos.GetPositionZ(), copyPos.GetOrientation());
            copy->NearTeleportTo(casterPos.GetPositionX(), casterPos.GetPositionY(), casterPos.GetPositionZ(), casterPos.GetOrientation());
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_transcendence_transfer::CheckCast);
        OnCast += SpellCastFn(spell_monk_transcendence_transfer::HandleCast);
    }
};

// Called by Thunder Focus Tea - 116680
// Item S12 4P - Mistweaver - 124487
class spell_monk_item_s12_4p_mistweaver : public SpellScriptLoader
{
    public:
        spell_monk_item_s12_4p_mistweaver() : SpellScriptLoader("spell_monk_item_s12_4p_mistweaver") { }

        class spell_monk_item_s12_4p_mistweaver_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_item_s12_4p_mistweaver_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(SPELL_MONK_ITEM_4_S12_MISTWEAVER))
                        _player->CastSpell(_player, SPELL_MONK_ZEN_FOCUS, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_item_s12_4p_mistweaver_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_item_s12_4p_mistweaver_SpellScript();
        }
};

// 122783 - Diffuse Magic
class spell_monk_diffuse_magic : public SpellScript
{
    PrepareSpellScript(spell_monk_diffuse_magic);

    void HandleCast()
    {
        Unit* monk = GetCaster();
        std::multimap<uint32, uint64> toBeRemoved;
        auto& auraList = monk->GetAppliedAuras();

        auto markToDispel = [&]()
        {
            for (auto&& it : auraList)
            {
                Aura* aura = it.second->GetBase();
                if (!(aura->GetSpellInfo()->GetDispelMask() & (1 << DISPEL_MAGIC)))
                    continue;

                if (aura->GetSpellInfo()->IsPositive() || aura->IsPassive() || aura->IsRemoved())
                    continue;

                Unit* caster = aura->GetCaster();
                if (!caster || caster->GetGUID() == monk->GetGUID())
                    continue;

                if (!caster->IsWithinDist(monk, 40.0f))
                    continue;

                if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY))
                    continue;

                // Don't apply auras here, it may cause invalidation of the auraList because of fucking scripts, which probably shouldn't work.
                toBeRemoved.emplace(aura->GetSpellInfo()->Id, caster->GetGUID());
            }
        };

        // Some shit here.
        // Seems what after dispel effects must be dispelled too, but not transfered to caster, like silence after Unstable Affliction dispel.
        // So, call this here and another time after actual dispel (and also dispel).
        markToDispel();

        for (auto&& it : toBeRemoved)
        {
            Aura* aura = monk->GetAura(it.first, it.second);
            if (!aura)       // Possible if it was removed with something else
                continue;

            Unit* caster = aura->GetCaster();
            if (!caster)    // Possible?
                continue;

            if (!aura->GetSpellInfo()->HasAttribute(SPELL_ATTR1_CANT_BE_REFLECTED))
            {
                if (Aura* addedAura = caster->AddAura(aura->GetSpellInfo()->Id, caster))
                {
                    AuraData data;
                    aura->SaveParameters(data);
                    addedAura->LoadParameters(data);
                    addedAura->SetMaxDuration(addedAura->GetDuration());
                }
            }

            uint32 charges = aura->GetSpellInfo()->HasAttribute(SPELL_ATTR7_DISPEL_CHARGES) ? aura->GetCharges() : aura->GetStackAmount();
            monk->RemoveAurasDueToSpellByDispel(aura->GetId(), GetSpellInfo()->Id, aura->GetCasterGUID(), monk, charges);
        }

        toBeRemoved.clear();
        markToDispel();

        for (auto&& it : toBeRemoved)
        {
            if (Aura* aura = monk->GetAura(it.first, it.second))
            {
                uint32 charges = aura->GetSpellInfo()->HasAttribute(SPELL_ATTR7_DISPEL_CHARGES) ? aura->GetCharges() : aura->GetStackAmount();
                monk->RemoveAurasDueToSpellByDispel(aura->GetId(), GetSpellInfo()->Id, aura->GetCasterGUID(), monk, charges);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_diffuse_magic::HandleCast);
    }
};


// Zen Flight - 125883
class spell_monk_zen_flight_check : public SpellScript
{
    PrepareSpellScript(spell_monk_zen_flight_check);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetMap()->IsBattlegroundOrArena())
            return SPELL_FAILED_NOT_IN_BATTLEGROUND;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_zen_flight_check::CheckCast);
    }
};

class aura_monk_glyph_of_zen_flight : public AuraScript
{
    PrepareAuraScript(aura_monk_glyph_of_zen_flight);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Zen Flight (Glyph) (Level 90)
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(125883);
        if (!SpellMgr::IsSpellValid(spellInfo))
            return;

        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        player->LearnSpell(125883, false);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Zen Flight (Glyph) (Level 90)
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(125883);
        if (!SpellMgr::IsSpellValid(spellInfo))
            return;

        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        player->RemoveSpell(125883);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(aura_monk_glyph_of_zen_flight::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(aura_monk_glyph_of_zen_flight::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 122470 - Touch of Karma 
class spell_monk_touch_of_karma : public AuraScript
{
    PrepareAuraScript(spell_monk_touch_of_karma);

    ObjectGuid targetGuid = ObjectGuid::Empty;

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* monk = GetCaster())
            amount = monk->GetMaxHealth();
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32&)
    {
        if (Unit* target = ObjectAccessor::GetUnit(*GetUnitOwner(), targetGuid))
            GetUnitOwner()->CastCustomSpell(SPELL_MONK_TOUCH_OF_KARMA_REDIRECT_DAMAGE, SPELLVALUE_BASE_POINT0, dmgInfo.GetDamage() / 9, target, true);
    }

    void HandleRemoveFromTarget(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* monk = GetCaster())
            monk->RemoveAurasDueToSpell(GetId());
    }

    void HandleRemoveFromCaster(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* target = ObjectAccessor::GetUnit(*GetUnitOwner(), targetGuid))
            target->RemoveAurasDueToSpell(GetSpellInfo()->Id, GetCasterGUID());
        GetUnitOwner()->RemoveAura(SPELL_TOUCH_OF_KARMA_INDICATOR);

    }

public:
    void SetRedirectTarget(ObjectGuid guid)
    {
        targetGuid = guid;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_touch_of_karma::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_touch_of_karma::Absorb, EFFECT_1);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_touch_of_karma::HandleRemoveFromTarget, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_touch_of_karma::HandleRemoveFromCaster, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 122470 - Touch of Karma
class spell_monk_touch_of_karma_target : public SpellScript
{
    PrepareSpellScript(spell_monk_touch_of_karma_target);

    ObjectGuid targetGuid = ObjectGuid::Empty;

    void HandleAfterHit()
    {
        if (GetCaster() == GetHitUnit() && !targetGuid)
            PreventHitAura();
    }

    void HandleHit(SpellEffIndex)
    {
        if (GetHitResult() == SPELL_MISS_NONE)
            targetGuid = GetHitUnit()->GetGUID();
    }

    void SetTarget(SpellEffIndex)
    {
        if (Aura* aura = GetCaster()->GetAura(GetSpellInfo()->Id, GetCaster()->GetGUID()))
        {
            if (auto script = dynamic_cast<spell_monk_touch_of_karma*>(aura->GetScriptByName("spell_monk_touch_of_karma")))
            {
                script->SetRedirectTarget(targetGuid);
                GetCaster()->CastSpell(GetCaster(), SPELL_TOUCH_OF_KARMA_INDICATOR, true);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_touch_of_karma_target::HandleAfterHit);
        OnEffectHitTarget += SpellEffectFn(spell_monk_touch_of_karma_target::HandleHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_monk_touch_of_karma_target::SetTarget, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Path of Blossom - 124336
class spell_monk_path_of_blossom : public SpellScriptLoader
{
    public:
        spell_monk_path_of_blossom() : SpellScriptLoader("spell_monk_path_of_blossom") { }

        class spell_monk_path_of_blossom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_monk_path_of_blossom_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (GetCaster())
                    GetCaster()->CastSpell(GetCaster(), SPELL_MONK_PATH_OF_BLOSSOM_AREATRIGGER, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_path_of_blossom_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_monk_path_of_blossom_AuraScript();
        }
};

// 124081 - Zen Sphere
class spell_monk_zen_sphere : public AuraScript
{
    PrepareAuraScript(spell_monk_zen_sphere);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Detonate();
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_ENEMY_SPELL && removeMode != AURA_REMOVE_BY_EXPIRE)
            return;

        Detonate();
    }

    void Detonate()
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(GetTarget(), SPELL_MONK_ZEN_SPHERE_DETONATE_HEAL, true);
            caster->CastSpell(GetTarget(), SPELL_MONK_ZEN_SPHERE_DETONATE_DAMAGE, true);
        }
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MONK_ZEN_SPHERE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_zen_sphere::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAPPLY);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_zen_sphere::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_zen_sphere::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// Energizing Brew - 115288
class spell_monk_energizing_brew : public SpellScriptLoader
{
    public:
        spell_monk_energizing_brew() : SpellScriptLoader("spell_monk_energizing_brew") { }

        class spell_monk_energizing_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_energizing_brew_SpellScript);

            SpellCastResult CheckFight()
            {
                if (!GetCaster()->IsInCombat())
                    return SPELL_FAILED_CASTER_AURASTATE;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_monk_energizing_brew_SpellScript::CheckFight);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_energizing_brew_SpellScript();
        }
};

// Spear Hand Strike - 116705
class spell_monk_spear_hand_strike : public SpellScriptLoader
{
    public:
        spell_monk_spear_hand_strike() : SpellScriptLoader("spell_monk_spear_hand_strike") { }

        class spell_monk_spear_hand_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_spear_hand_strike_SpellScript);

            void HandleOnHit()
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->isInFront(player))
                        {
                            player->CastSpell(target, SPELL_MONK_SPEAR_HAND_STRIKE_SILENCE, true);
                            player->AddSpellCooldown(116705, Seconds(15));
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_spear_hand_strike_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_spear_hand_strike_SpellScript();
        }
};

// Tiger's Lust - 116841
class spell_monk_tigers_lust : public SpellScriptLoader
{
    public:
        spell_monk_tigers_lust() : SpellScriptLoader("spell_monk_tigers_lust") { }

        class spell_monk_tigers_lust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_tigers_lust_SpellScript);

            void HandleOnHit()
            {
                if (GetCaster()->ToPlayer())
                    if (Unit* target = GetHitUnit())
                        target->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_monk_tigers_lust_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_tigers_lust_SpellScript();
        }
};

// 115008, 121828 - Chi Torpedo
class spell_monk_chi_torpedo : public SpellScript
{
    PrepareSpellScript(spell_monk_chi_torpedo);

    SpellCastResult CheckCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (player->HasUnitState(UNIT_STATE_ROOT))
                return SPELL_FAILED_ROOTED;

        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_MONK_ITEM_PVP_GLOVES_BONUS))
            GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_chi_torpedo::CheckCast);
        AfterCast += SpellCastFn(spell_monk_chi_torpedo::HandleCast);
    }
};

// 339
class sat_monk_chi_torpedo : public IAreaTrigger
{
    std::set<uint64> hit;
    Position lastPos;

    void OnInit() override
    {
        lastPos = GetCaster()->GetPosition();
    }

    void OnUpdate(uint32 diff) override
    {
        Unit* caster = GetCaster();

        std::list<Unit*> targets;
        Trinity::AnyUnitInObjectRangeCheck check{ caster, 4.0f };
        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher{ caster, targets, check };
        caster->VisitNearbyObject(4.0f, searcher);

        targets.remove_if([&](Unit* target)
        {
            if (target->IsTotem())
                return true;
            if (!caster->IsWithinLOSInMap(target))
                return true;
            if (hit.find(target->GetGUID()) != hit.end())
                return true;
            if (!lastPos.HasInArc(2 * M_PI / 3, target))
                return true;
            return false;
        });

        SpellInfo const* damageSpell = sSpellMgr->GetSpellInfo(SPELL_MONK_CHI_TORPEDO_DAMAGE);
        for (auto&& target : targets)
        {
            uint32 spellId = 0;
            if (caster->IsValidAttackTarget(target, damageSpell))
                caster->CastSpell(target, SPELL_MONK_CHI_TORPEDO_DAMAGE, true);
            else if (caster->IsValidAssistTarget(target))
                caster->CastSpell(target, SPELL_MONK_CHI_TORPEDO_HEAL, true);
            hit.insert(target->GetGUID());
        }
        lastPos = caster->GetPosition();
    }
};

// Breath of Fire - 115181
class spell_monk_breath_of_fire : public SpellScriptLoader
{
    public:
        spell_monk_breath_of_fire() : SpellScriptLoader("spell_monk_breath_of_fire") { }

        class spell_monk_breath_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monk_breath_of_fire_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Player* _player = caster->ToPlayer())
                    {
                        if (Unit* target = GetHitUnit())
                        {
                            // if Dizzying Haze is on the target, they will burn for an additionnal damage over 8s
                            if (target->HasAura(SPELL_MONK_DIZZYING_HAZE))
                            {
                                _player->CastSpell(target, SPELL_MONK_BREATH_OF_FIRE_DOT, true);

                                // Glyph of Breath of Fire
                                if (_player->HasAura(123394))
                                    _player->CastSpell(target, SPELL_MONK_BREATH_OF_FIRE_CONFUSED, true);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_monk_breath_of_fire_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monk_breath_of_fire_SpellScript();
        }
};

// 116095 - Disable
class spell_monk_disable_refresh : public AuraScript
{
    PrepareAuraScript(spell_monk_disable_refresh);

    void HandleTick(AuraEffect const* eff)
    {
        if (Unit* caster = GetCaster())
            if (caster->IsAlive())
                if (eff->GetTickNumber() % eff->GetTotalTicks() == 0) // last tick of duration
                    if (GetOwner()->GetExactDist(caster) < 10.0f)
                        RefreshDuration();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_disable_refresh::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 116095 - Disable
class spell_monk_disable : public SpellScript
{
    PrepareSpellScript(spell_monk_disable);

    bool m_root = false;

    void ResolveEffect(WorldObject*& target)
    {
        if (Unit* unit = target->ToUnit())
            m_root = unit->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED);
    }

    void HandleHit()
    {
        if (m_root)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MONK_DISABLE_ROOT, true);
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_monk_disable::ResolveEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnHit += SpellHitFn(spell_monk_disable::HandleHit);
    }
};

// Zen Pilgrimage - 126892
class spell_monk_zen_pilgrimage : public SpellScript
{
    PrepareSpellScript(spell_monk_zen_pilgrimage);

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
            if (player->GetAreaId() != 6081)
                player->SaveRecallPosition();
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_zen_pilgrimage::CheckCast);
        BeforeCast += SpellCastFn(spell_monk_zen_pilgrimage::HandleBeforeCast);
    }
};

// Zen Pilgrimage : Return - 126895
class spell_monk_zen_pilgrimage_return : public SpellScript
{
    PrepareSpellScript(spell_monk_zen_pilgrimage_return);

    void HandleCast()
    {
        if (GetCaster()->GetAreaId() != 6081)
            return;

        if (Player* player = GetCaster()->ToPlayer())
            player->TeleportTo(player->m_recallMap, player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_zen_pilgrimage_return::HandleCast);
    }
};

// Blackout Kick - 100784
class spell_monk_blackout_kick : public SpellScript
{
    PrepareSpellScript(spell_monk_blackout_kick);

    void HandleHit()
    {
        if (GetHitUnit() != GetExplTargetUnit())
            SetHitDamage(GetHitDamage() / 2);
    }

    void HandleTriggers()
    {
        Player* monk = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();
        Unit* target = GetHitUnit();
        if (target == monk || !monk)
            return;

        uint32 damage = GetHitDamage();
        if (!damage)
            return;

        Unit* caster = GetCaster(); // Real caster

        if (monk->GetTalentSpecialization() == SPEC_MONK_WINDWALKER && monk->HasAura(128595))
        {
            // Your Blackout Kick always deals 20% additional damage over 4 sec regardless of positioning but you're unable to trigger the healing effect.
            if (monk->HasAura(SPELL_MONK_GLYPH_OF_BLACKOUT_KICK))   // Monk here, not caster
            {
                caster->CastCustomSpell(SPELL_MONK_BLACKOUT_KICK_DOT, SPELLVALUE_BASE_POINT0, damage * 0.2f / 4, target,  true);
            }
            else
            {
                // If behind : 20% damage on DoT
                if (target->isInBack(caster))
                {
                    caster->CastCustomSpell(SPELL_MONK_BLACKOUT_KICK_DOT, SPELLVALUE_BASE_POINT0, damage * 0.2f / 4, target, true);
                }
                else // 20% damage on instant heal
                {
                    caster->CastCustomSpell(SPELL_MONK_BLACKOUT_KICK_HEAL, SPELLVALUE_BASE_POINT0, damage * 0.2f, caster, true);
                }
            }
        }
        else if (monk->GetTalentSpecialization() == SPEC_MONK_BREWMASTER && caster == monk) // Only for monk
        {
            if (Aura* shuffle = monk->GetAura(SPELL_MONK_SHUFFLE))
                shuffle->SetDuration(shuffle->GetDuration() + shuffle->GetMaxDuration());
            else
                monk->CastSpell(monk, SPELL_MONK_SHUFFLE, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_blackout_kick::HandleHit);
        AfterHit += SpellHitFn(spell_monk_blackout_kick::HandleTriggers);
    }
};

// 115078 - Paralysis
class spell_monk_paralysis : public SpellScript
{
    PrepareSpellScript(spell_monk_paralysis);

    void HandleHit()
    {
        if (Aura* aura = GetHitAura())
        {
            if (GetHitUnit()->isInBack(GetCaster()))
            {
                int32 duration = aura->GetMaxDuration();
                aura->SetMaxDuration(duration * 1.5);
                aura->SetDuration(duration * 1.5);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_paralysis::HandleHit);
    }
};

// 115078 - Paralysis
class spell_monk_paralysis_break : public AuraScript
{
    PrepareAuraScript(spell_monk_paralysis_break);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo())
        {
            switch (eventInfo.GetSpellInfo()->Id)
            {
                case SPELL_MONK_CHI_TORPEDO_DAMAGE:
                case SPELL_MONK_CHI_WAVE_DAMAGE:
                case SPELL_MONK_ZEN_SPHERE_DAMAGE:
                case SPELL_MONK_ZEN_SPHERE_DETONATE_DAMAGE:
                    return false;
            }
        }
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_paralysis_break::CheckProc);
    }
};

// 115921 - Legacy of the Emperor
class spell_monk_legacy_of_the_emperor : public SpellScript
{
    PrepareSpellScript(spell_monk_legacy_of_the_emperor);

    void HandleHit()
    {
        Player* monk = GetCaster()->ToPlayer();
        if (!monk)
            return;

        if (monk->GetGroup() && monk->IsInRaidWith(GetHitUnit()))
            monk->CastSpell(monk, SPELL_MONK_LEGACY_OF_THE_EMPEROR_RAID, true);
        else
            monk->CastSpell(GetHitUnit(), SPELL_MONK_LEGACY_OF_THE_EMPEROR_SINGLE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_legacy_of_the_emperor::HandleHit);
    }
};

// 145640 - Chi Brew
class spell_monk_chi_brew : public SpellScript
{
    PrepareSpellScript(spell_monk_chi_brew);

    void HandleDummy(SpellEffIndex)
    {
        Player* monk = GetCaster()->ToPlayer();
        if (!monk)
            return;

        uint32 brewId = 0;
        uint32 visual = 0;
        uint32 stacks = 2;
        switch (monk->GetTalentSpecialization())
        {
            case SPEC_MONK_BREWMASTER:
                brewId = SPELL_MONK_ELUSIVE_BREW_STACKS;
                stacks = 5;
                break;
            case SPEC_MONK_MISTWEAVER:
                brewId = SPELL_MONK_MANA_TEA_STACKS;
                visual = SPELL_MONK_PLUS_ONE_MANA_TEA;
                if (roll_chance_f(monk->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + SPELL_SCHOOL_NATURE)))
                    stacks *= 2;
                break;
            case SPEC_MONK_WINDWALKER:
                brewId = SPELL_MONK_TIGEREYE_BREW_STACKS;
                visual = SPELL_MONK_PLUS_ONE_TIGEREYE_BREW;
                if (AuraEffect const* mastery = monk->GetAuraEffect(SPELL_MONK_MASTERY_BOTTELED_FURY, EFFECT_0))
                    if (roll_chance_f(mastery->GetFloatAmount()))
                        stacks *= 2;
                break;
            default:
                return;
        }

        for (uint32 i = 0; i < stacks; ++i)
        {
            monk->CastSpell(monk, brewId, true);
            if (visual)
                monk->CastSpell(monk, visual, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_monk_chi_brew::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 718 - Ring of Peace
class sat_monk_ring_of_peace : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
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

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            if (!target->HasAura(SPELL_MONK_RING_OF_PEACE_AURA))
                GetCaster()->CastSpell(target, SPELL_MONK_RING_OF_PEACE_AURA, true);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            if (!target->HasAura(SPELL_MONK_RING_OF_PEACE_AURA))
                GetCaster()->CastSpell(target, SPELL_MONK_RING_OF_PEACE_AURA, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        triggering->ToUnit()->RemoveAurasDueToSpell(SPELL_MONK_RING_OF_PEACE_AURA);
    }
};

// 140023 - Ring of Peace
class spell_monk_ring_of_peace_aura : public AuraScript
{
    PrepareAuraScript(spell_monk_ring_of_peace_aura);

    void HandleProc(ProcEventInfo&)
    {
        PreventDefaultAction();

        if (Unit* monk = GetCaster())
        {
            monk->CastSpell(GetTarget(), SPELL_MONK_RING_OF_PEACE_DISARM, true);
            monk->CastSpell(GetTarget(), SPELL_MONK_RING_OF_PEACE_SILENCE, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_ring_of_peace_aura::HandleProc);
    }
};

// 137639 - Storm, Earth, and Fire
class spell_monk_s_e_f_aura : public AuraScript
{
    PrepareAuraScript(spell_monk_s_e_f_aura);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (GetStackAmount() == 2)
        {
            Unit* monk = GetUnitOwner();
            uint32 id = GetId();
            uint32 effect = eff->GetEffIndex();
            monk->m_Events.Schedule(1, [=]()    // Couldn't be changed during this handler
            {
                if (AuraEffect* eff = monk->GetAuraEffect(id, effect))
                    eff->ChangeAmount(-45);
            });
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* monk = GetUnitOwner();
        std::list<TempSummon*> summons;
        monk->GetSummons(summons, NPC_STORM_SPIRIT);
        monk->GetSummons(summons, NPC_EARTH_SPIRIT);
        monk->GetSummons(summons, NPC_FIRE_SPIRIT);

        for (auto&& itr : summons)
            itr->AI()->DoAction(-1);

        monk->RemoveAurasDueToSpell(138080);
        monk->RemoveAurasDueToSpell(138081);
        monk->RemoveAurasDueToSpell(138083);
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return false;

        switch (eventInfo.GetSpellInfo()->Id)
        {
            case 100780: // TigerPalm
                return true;
            case 117952: // Crackling Jade Lightning
                return true;
            case 100784: // Blackout Kick
                return true;
            case 116847: // Rushing Jade Wind 
                return true;
            case 107428: // Rising Sun Kick
                return true;
            case 123986: // Chi Burst
                return true;
            case 101546: // Spinning Crane
                return true;
            case 115098: // Chi Wave
                return true;
            case 113656: // Fists of Fury
                return true;
            default:
                break;
        }

        if (eventInfo.TriggeredBySpell()->m_targets.GetItemTargetGUID())
            return false;
        if (eventInfo.TriggeredBySpell()->IsTriggered())
            return false;

        for (auto&& itr : eventInfo.GetSpellInfo()->Effects)
        {
            if (itr.Effect == SPELL_EFFECT_SCHOOL_DAMAGE ||
                itr.ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE)
                return true;
        }

        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        for (auto&& itr : GetUnitOwner()->GetSummons())
        {
            if (itr->GetEntry() != NPC_STORM_SPIRIT && itr->GetEntry() != NPC_EARTH_SPIRIT && itr->GetEntry() != NPC_FIRE_SPIRIT)
                continue;

            if (!itr->GetVictim())
                continue;

            if (itr->GetVictim() == eventInfo.GetActionTarget())
                continue;

            Unit* target = itr->GetVictim();
            if (eventInfo.GetSpellInfo()->Id == 124081) // Zen Sphere
                target = itr;

            itr->CastSpell(target, eventInfo.GetSpellInfo()->Id, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_s_e_f_aura::HandleApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectApply += AuraEffectApplyFn(spell_monk_s_e_f_aura::HandleApply, EFFECT_3, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectApply += AuraEffectApplyFn(spell_monk_s_e_f_aura::HandleApply, EFFECT_4, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_s_e_f_aura::HandleRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_monk_s_e_f_aura::CheckProc);
        OnProc += AuraProcFn(spell_monk_s_e_f_aura::HandleProc);
    }
};

static uint32 VisualForSEFSpell(uint32 spell)
{
    switch (spell)
    {
        case SPELL_MONK_SEF_SUMMON_STORM_SPIRIT: return 138080;
        case SPELL_MONK_SEF_SUMMON_FIRE_SPIRIT: return 138081;
        case SPELL_MONK_SEF_SUMMON_EARTH_SPIRIT: return 138083;
    }
    return 0;
}

static uint32 VisualForSEFEntry(uint32 entry)
{
    switch (entry)
    {
        case NPC_STORM_SPIRIT: return 138080;
        case NPC_FIRE_SPIRIT: return 138081;
        case NPC_EARTH_SPIRIT: return 138083;
    }
    return 0;
}

// 137639 - Storm, Earth, and Fire
class spell_monk_s_e_f : public SpellScript
{
    PrepareSpellScript(spell_monk_s_e_f);

    uint64 toDespawn = 0;
    bool despawnAll = false;

    SpellInfo const* SelectSpell(std::list<uint32>& list)
    {
        if (list.empty())
            return nullptr;
        uint32 spell = Trinity::Containers::SelectRandomContainerElement(list);
        list.remove(spell);
        return sSpellMgr->GetSpellInfo(spell);
    }

    SpellCastResult CheckCast()
    {
        if (Unit* target = GetExplTargetUnit())
        {
            for (auto&& itr : GetSpirits())
            {
                if (itr->GetAI()->GetGUID() == target->GetGUID())
                {
                    toDespawn = itr->GetGUID();
                    return SPELL_CAST_OK;
                }
            }

            if (Aura* aura = GetCaster()->GetAura(GetSpellInfo()->Id))
                if (aura->GetStackAmount() > 1)
                    despawnAll = true;
        }
        return SPELL_CAST_OK;
    }

    void HandleHit(SpellEffIndex)
    {
        if (toDespawn || despawnAll)
        {
            for (auto&& itr : GetSpirits())
            {
                if (itr->GetGUID() == toDespawn || despawnAll)
                {
                    itr->AI()->DoAction(-1);
                }
            }
            return;
        }

        std::list<uint32> spells{ SPELL_MONK_SEF_SUMMON_STORM_SPIRIT, SPELL_MONK_SEF_SUMMON_EARTH_SPIRIT, SPELL_MONK_SEF_SUMMON_FIRE_SPIRIT };

        if (Aura* aura = GetCaster()->GetAura(GetSpellInfo()->Id))
        {
            if (aura->GetStackAmount() < 2)
            {
                // Remove aura if there is another spirit used
                for (auto&& it : GetSpirits())
                {
                    if (it->GetEntry() == NPC_EARTH_SPIRIT)
                        spells.remove(SPELL_MONK_SEF_SUMMON_EARTH_SPIRIT);
                    if (it->GetEntry() == NPC_FIRE_SPIRIT)
                        spells.remove(SPELL_MONK_SEF_SUMMON_FIRE_SPIRIT);
                    if (it->GetEntry() == NPC_STORM_SPIRIT)
                        spells.remove(SPELL_MONK_SEF_SUMMON_STORM_SPIRIT);
                }

                // Remove aura if caster uses it
                for (auto&& id : { SPELL_MONK_SEF_SUMMON_STORM_SPIRIT, SPELL_MONK_SEF_SUMMON_EARTH_SPIRIT, SPELL_MONK_SEF_SUMMON_FIRE_SPIRIT })
                    if (GetCaster()->HasAura(VisualForSEFSpell(id)))
                        spells.remove(id);

                Summon(SelectSpell(spells));
            }
        }
        else if (SpellInfo const* spell = SelectSpell(spells))
        {
            // Visual for monk.
            GetCaster()->CastSpell(GetCaster(), VisualForSEFSpell(spell->Id), true);
            // Summon spirit and apply visual for it.
            Summon(SelectSpell(spells));
        }
    }

    std::list<TempSummon*> GetSpirits()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_STORM_SPIRIT);
        GetCaster()->GetSummons(summons, NPC_EARTH_SPIRIT);
        GetCaster()->GetSummons(summons, NPC_FIRE_SPIRIT);
        return summons;
    }

    void Summon(SpellInfo const* spell)
    {
        if (spell)
        {
            GetCaster()->CastSpell(GetHitUnit(), spell->Id, true);
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, spell->Effects[EFFECT_0].MiscValue);
            if (!summons.empty())
            {
                summons.front()->ToCreature()->AI()->SetGUID(GetHitUnit()->GetGUID());
                summons.front()->ToCreature()->AI()->AttackStart(GetHitUnit());
            }
        }
    }

    void PreventAura(WorldObject*& target)
    {
        if (toDespawn || despawnAll)
            target = nullptr;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_s_e_f::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_monk_s_e_f::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_monk_s_e_f::PreventAura, EFFECT_1, TARGET_UNIT_CASTER);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_monk_s_e_f::PreventAura, EFFECT_2, TARGET_UNIT_CASTER);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_monk_s_e_f::PreventAura, EFFECT_3, TARGET_UNIT_CASTER);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_monk_s_e_f::PreventAura, EFFECT_4, TARGET_UNIT_CASTER);
    }
};



// 69680, 69792, 69791
struct npc_monk_s_e_f_spirit : public ScriptedAI
{
    enum class State
    {
        Jumping,
        GettingRound,
        Attacking,
        Despawning,
    };

    enum
    {
        SPELL_FIXATE_AND_SCALIN = 138130,
        POINT_ATTACK_START      = 1000,
    };


    ObjectGuid victimGuid = ObjectGuid::Empty;
    TaskScheduler scheduler;
    State state = State::Jumping;

    npc_monk_s_e_f_spirit(Creature* c) : ScriptedAI(c) {}

    void InitializeAI() override
    {
        if (Unit* owner = me->GetOwner())
            owner->CastSpell(me, 119051, true); // Clone

        me->CastSpell(me, 119052, false);                               // Transcendence Clone Target Trigger
        me->CastSpell(me, VisualForSEFEntry(me->GetEntry()), true);     // Visual
        me->CastSpell(me, 138131, false);

        UpdateDamage();

        scheduler
            .Schedule(Seconds(1), [=](TaskContext ctx)
            {
                UpdateDamage();
                ctx.Repeat();
            })
            .Schedule(Seconds(5), [=](TaskContext)
            {
                if (state != State::Despawning)
                    AcquireTarget();
            });
    }


    void SetGUID(ObjectGuid guid, int32) override
    {
        victimGuid = guid;
        if (Unit* target = ObjectAccessor::GetUnit(*me, guid))
        {
            me->CastSpell(target, SPELL_FIXATE_AND_SCALIN, true);
            me->CastSpell(target, SPELL_FLYING_KICK, true);
        }
    }

    ObjectGuid GetGUID(int32) const override
    {
        return victimGuid;
    }

    void Unsummoned() override
    {
        if (Unit* owner = me->GetOwner())
            if (Aura* aura = owner->GetAura(SPELL_MONK_STORM_EARTH_AND_FIRE))
                aura->ModStackAmount(-1);
    }

    void DoAction(int32 action) override
    {
        if (action == -1)
            DoDespawn();
    }

    void AttackStart(Unit* target) override
    {
        if (state == State::Attacking)
        {
            if (target && me->Attack(target, true))
            {
                me->GetMotionMaster()->MoveChase(target, CONTACT_DISTANCE, M_PI);
                victimGuid = target->GetGUID();
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == EFFECT_MOTION_TYPE)
        {
            if (state == State::Jumping)
            {
                scheduler.Schedule(Milliseconds(1), [=](TaskContext)
                {
                    if (Unit* target = me->SelectVictim())
                    {
                        float x, y, z;
                        target->GetNearPoint(me, x, y, z, me->GetCombatReach(), CONTACT_DISTANCE, target->GetOrientation() + M_PI);
                        me->GetMotionMaster()->MovePoint(POINT_ATTACK_START, x, y, z);
                        state = State::GettingRound;
                    }
                    else
                        AcquireTarget();
                });
            }
            else if (state == State::Despawning)
                me->DespawnOrUnsummon();
        }
        if (type == POINT_MOTION_TYPE && id == POINT_ATTACK_START && state == State::GettingRound)
            scheduler.Schedule(Milliseconds(1), [=](TaskContext)
        {
            AcquireTarget();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (state != State::Attacking)
            return;

        Unit* target = ObjectAccessor::GetUnit(*me, victimGuid);
        if (!target || !target->IsAlive() || !UpdateVictim())
        {
            DoDespawn();
            return;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit*) override
    {
        me->DespawnOrUnsummon();
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override 
    {
        if (state == State::Attacking && me->IsAlive())
            DoDespawn();
    }

    void AcquireTarget()
    {
        state = State::Attacking;
        AttackStart(me->SelectVictim());
    }

    void UpdateDamage()
    {
        if (Unit* monk = me->GetOwner())
        {
            me->SetCanDualWield(monk->CanDualWield());

            uint32 damageMin = monk->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE);
            uint32 damageMax = monk->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE);

            if (damageMin != monk->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE))
            {
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, damageMin);
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, damageMax);
                me->UpdateDamagePhysical(BASE_ATTACK);
            }

            damageMin = monk->GetWeaponDamageRange(OFF_ATTACK, MINDAMAGE);
            damageMax = monk->GetWeaponDamageRange(OFF_ATTACK, MAXDAMAGE);

            if (damageMin != monk->GetWeaponDamageRange(OFF_ATTACK, MINDAMAGE))
            {
                me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, damageMin);
                me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, damageMax);
                me->UpdateDamagePhysical(OFF_ATTACK);
            }

            float pct = -(100 - monk->GetTotalAuraMultiplier(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE) * 100);
            if (AuraEffect* scaling = me->GetAuraEffect(SPELL_FIXATE_AND_SCALIN, EFFECT_1))
                if (scaling->GetFloatAmount() != pct)
                    scaling->ChangeAmount(pct);
        }
    }

    void DoDespawn()
    {
        if (me->IsAlive())
        {
            me->GetMotionMaster()->MovementExpired();
            me->CombatStop();
            me->CastSpell(me->GetOwner(), SPELL_FLYING_KICK, true);
            me->DespawnOrUnsummon(me->GetSplineDuration());
            state = State::Despawning;

        }
        else
            me->DespawnOrUnsummon();
    }
};

// 115450 - Detox
class spell_monk_detox : public SpellScript
{
    PrepareSpellScript(spell_monk_detox);

    void HandleDispelMagic(SpellEffIndex effIndex)
    {
        if (!GetCaster()->HasAura(SPELL_MONK_INTERNAL_MEDICINE))
            PreventDefaultEffect(effIndex);
    }

    void HandleHit()
    {
        if (!GetDispelCount() || !GetCaster()->HasAura(SPELL_MONK_GLYPH_OF_DETOX))
            PreventHitHeal();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_monk_detox::HandleDispelMagic, EFFECT_2, SPELL_EFFECT_DISPEL);
        OnHit += SpellHitFn(spell_monk_detox::HandleHit);
    }
};

struct spell_monk_specific_damage_base
{
    float CalculateDamage(Player* player, uint32 spellId) const
    {
        struct Modificator
        {
            Modificator(float val, bool brew) : Default(val), BrewmasterPenalty(brew) { }
            float Default;
            bool  BrewmasterPenalty;
        };

        static std::map<uint32, Modificator> const map =
        {
            { 100780, { 1.5f,                   false } }, // Jab
            { 108557, { 1.5f,                   false } }, // Jab (Staff)
            { 115698, { 1.5f,                   false } }, // Jab (Polearm)
            { 115687, { 1.5f,                   false } }, // Jab (Axes)
            { 115693, { 1.5f,                   false } }, // Jab (Maces)
            { 115695, { 1.5f,                   false } }, // Jab (Swords)
            { 121253, { 10.0f,                  false } }, // Keg Smash
            { 100784, { 7.12f,                  false } }, // Blackout Kick
            { 100787, { 3.0f,                   false } }, // Tiger Palm
            { 115073, { 2.1f,                   false } }, // Spinning Fire Blossom
            { 123408, { 2.1f,                   false } }, // Spinning Fire Blossom (glyphed)
            { 107428, { 14.4f * 0.89f,          false } }, // Rising Sun Kick
            { 124335, { 0.3f * 7.5f,            true  } }, // Swift Reflexes
            { 107270, { 1.59f * 1.75f / 1.59f,  true  } }, // Spinning Crane Kick
            { 148187, { 1.59f * 1.4f / 1.59f,   true  } }, // Rushing Jade Wind
            { 115072, { 7.0f,                   true  } }, // Expel Harm
            { 147489, { 7.0f,                   true  } }, // Expel Harm (Glyphed)
            { 113656, { 7.5f* 0.89f,            false } }, // Fists of Fury
        };

        auto it = map.find(spellId);
        if (it == map.end())
        {
            TC_LOG_ERROR("sql.sql", "spell_monk_specific_damage specifief in DB for spell %u but modificator not found", spellId);
            return 0.0f;
        }

        float spellCoeff = it->second.Default;
        float basemod = it->second.BrewmasterPenalty && player->HasSpell(120267) ? 0.4f : 1.0f;
        float apmod = it->second.BrewmasterPenalty && player->HasSpell(120267) ? 11.0f : 14.0f;

        auto mainItem = player->GetWeaponForAttack(BASE_ATTACK, true);
        auto offItem = player->GetWeaponForAttack(OFF_ATTACK, true);

        float mainMin = 0.0f;
        float mainMax = 0.0f;
        float offMin = 0.0f;
        float offMax = 0.0f;
        float dualmod = 0.898882275f;
        float offmod = 1.0f;

        if (mainItem)
        {
            player->GetItemDamage(mainMin, mainMax, mainItem);

            auto proto = mainItem->GetTemplate();
            mainMin = mainMin / proto->Delay * 1000.0f;
            mainMax = mainMax / proto->Delay * 1000.0f;
            if (proto->InventoryType == INVTYPE_2HWEAPON)
            {
                dualmod = 1.0f;
                offmod = 0.0f;
            }
        }

        if (offItem)
        {
            player->GetItemDamage(offMin, offMax, offItem);

            auto proto = offItem->GetTemplate();
            offMin = offMin / proto->Delay * 1000.0f / 2.0f;
            offMax = offMax / proto->Delay * 1000.0f / 2.0f;
        }
        else
        {
            offMin = mainMin / 2.0f;
            offMax = mainMax / 2.0f;
        }

        float apc = player->GetTotalAttackPowerValue(BASE_ATTACK) / apmod;

        float min = spellCoeff * (basemod * dualmod * (mainMin + offmod * offMin) + apc - 1);
        float max = spellCoeff * (basemod * dualmod * (mainMax + offmod * offMax) + apc + 1);

        return frand(min, max);
    }
};

// Called by spells specified in map.
class spell_monk_specific_damage : public SpellScript, public spell_monk_specific_damage_base
{
    PrepareSpellScript(spell_monk_specific_damage);
protected:
    void CalculateDamage(SpellEffIndex)
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            if (Unit* owner = GetCaster()->GetOwner())
                player = owner->ToPlayer();
        if (!player)
            return;

        SetEffectValue(spell_monk_specific_damage_base::CalculateDamage(player, GetSpellInfo()->Id));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_monk_specific_damage::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 121253 - Keg Smash
class spell_monk_keg_smash : public SpellScript
{
    PrepareSpellScript(spell_monk_keg_smash);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            caster->CastSpell(target, SPELL_MONK_KEG_SMASH_VISUAL, true);
            caster->CastSpell(target, SPELL_MONK_WEAKENED_BLOWS, true);
            caster->CastSpell(target, SPELL_MONK_DIZZYING_HAZE, true);
        }
    }

    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_KEG_SMASH_ENERGIZE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_keg_smash::HandleOnHit);
        AfterCast += SpellCastFn(spell_monk_keg_smash::HandleAfterCast);
    }
};

// 107428 - Rising Sun Kick
class spell_monk_rising_sun_kick : public SpellScript
{
    PrepareSpellScript(spell_monk_rising_sun_kick);

    void HandleHit()
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_MONK_RISING_SUN_KICK_DEBUFF, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_rising_sun_kick::HandleHit);
    }
};

// 137384 - Combo Breaker
class spell_monk_combo_breaker : public AuraScript
{
    PrepareAuraScript(spell_monk_combo_breaker);

    void OnProc(AuraEffect const* eff, ProcEventInfo& procEvent)
    {
        Unit* owner = GetUnitOwner();
        if (procEvent.GetActionTarget() == owner)  // Energize
            return;

        if (roll_chance_i(eff->GetAmount()))
            owner->CastSpell(owner, SPELL_MONK_COMBO_BREAKER_KICK, true);

        if (roll_chance_i(eff->GetAmount()))
            owner->CastSpell(owner, SPELL_MONK_COMBO_BREAKER_PALM, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_monk_combo_breaker::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 124255 - Stagger
class spell_monk_stagger : public AuraScript
{
    PrepareAuraScript(spell_monk_stagger);

    int32 tick = 0;
    float addAmount = 0.0f;

    void HandleTick(AuraEffect const* eff, int32&)
    {
        ++tick; // Damage already done

        AuraEffect* stagger = const_cast<AuraEffect*>(eff);
        if (uint32 damage = addAmount)
        {
            addAmount -= float(damage);

            Unit* monk = GetUnitOwner();

            int32 remain = (10 - tick) * stagger->GetAmount();
            int32 total = remain + damage;
            int32 amount = total / 10;

            stagger->SetAmount(amount);
            GetEffect(EFFECT_1)->SetAmount(total);

            uint32 spellId = 0;
            if (amount < int32(monk->CountPctFromMaxHealth(3)))
                spellId = SPELL_MONK_LIGHT_STAGGER;
            else if (amount < int32(monk->CountPctFromMaxHealth(6)))
                spellId = SPELL_MONK_MODERATE_STAGGER;
            else
                spellId = SPELL_MONK_HEAVY_STAGGER;

            if (Aura* dummy = monk->GetAura(spellId))
            {
                dummy->RefreshDuration();
                dummy->GetEffect(EFFECT_0)->SetAmount(amount);
                dummy->GetEffect(EFFECT_1)->SetAmount(total);
            }
            else
            {
                for (auto&& id : { SPELL_MONK_LIGHT_STAGGER, SPELL_MONK_MODERATE_STAGGER, SPELL_MONK_HEAVY_STAGGER })
                    monk->RemoveAurasDueToSpell(id);
                monk->CastCustomSpell(monk, spellId, &amount, &total, nullptr, true);
            }

            tick = 0;

            if (AuraEffect const* eff = monk->GetAuraEffect(SPELL_MONK_T15_BREWMASTER_4P_BONUS, EFFECT_0))
                if (roll_chance_i(eff->GetSpellInfo()->ProcChance))
                    monk->CastSpell(monk, eff->GetSpellEffectInfo().TriggerSpell, true);
        }
        else
        {
            if (tick == 9)
                Remove();
        }
    }

public:
    void AddAmount(float amount)
    {
        addAmount += amount;
    }

    void Register() override
    {
        // Because  in this hook changing amount doesn't affect current tick
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_monk_stagger::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 115069 - Stance of the Sturdy Ox, 115213 - Avert Harm
class spell_stance_of_the_sturdy_ox : public AuraScript
{
    PrepareAuraScript(spell_stance_of_the_sturdy_ox);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void HandleStagger(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (!dmgInfo.GetDamage() || (dmgInfo.GetSpellInfo() && dmgInfo.GetSpellInfo()->Id == SPELL_MONK_STAGGER))
            return;

        Unit* monk = GetUnitOwner();

        float pct = 20.0f;
        if (AuraEffect* mastery = monk->GetAuraEffect(117906, EFFECT_0))
            pct += mastery->GetFloatAmount();
        if (monk->HasAura(SPELL_MONK_FORTIFYING_BREW))
            pct += 20.0f;
        if (monk->HasAura(SPELL_MONK_SHUFFLE))
            pct += 20.0f;

        if (AuraEffect const* eff = monk->GetAuraEffect(SPELL_MONK_STAGGERING, EFFECT_0))
            pct += eff->GetAmount();

        if (pct > 100.0f)   // Real ?
            pct = 100.0f;

        uint32 damage = GetSpellInfo()->Id == 115213 ? absorbAmount : dmgInfo.GetDamage();
        float staggerAmount = CalculatePct(damage, pct);

        if (GetSpellInfo()->Id == 115213)
            dmgInfo.AbsorbSpilttedDamage(staggerAmount);
        else
            absorbAmount = uint32(staggerAmount);

        if (Aura* stagger = monk->GetAura(SPELL_MONK_STAGGER))
        {
            if (auto script = dynamic_cast<spell_monk_stagger*>(stagger->GetScriptByName("spell_monk_stagger")))
                script->AddAmount(staggerAmount);
        }
        else
        {
            int32 bp0 = staggerAmount / 10;
            int32 bp1 = staggerAmount;

            uint32 spellId = 0;
            if (bp0 < int32(monk->CountPctFromMaxHealth(3)))
                spellId = SPELL_MONK_LIGHT_STAGGER;
            else if (bp0 < int32(monk->CountPctFromMaxHealth(6)))
                spellId = SPELL_MONK_MODERATE_STAGGER;
            else
                spellId = SPELL_MONK_HEAVY_STAGGER;

            monk->CastCustomSpell(monk, spellId, &bp0, &bp1, nullptr, true);
            monk->CastCustomSpell(monk, SPELL_MONK_STAGGER, &bp0, &bp1, nullptr, true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_stance_of_the_sturdy_ox::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
        if (m_scriptSpellId == 115213)
            OnEffectSplit += AuraEffectSplitFn(spell_stance_of_the_sturdy_ox::HandleStagger, EFFECT_0);
        else
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_stance_of_the_sturdy_ox::HandleStagger, EFFECT_2);
    }
};

// 124275 - Light Stagger, 124274 - Moderate Stagger, 124273 - Heave Stagger
class spell_monk_stagger_dummy : public AuraScript
{
    PrepareAuraScript(spell_monk_stagger_dummy);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            GetUnitOwner()->RemoveAurasDueToSpell(SPELL_MONK_STAGGER);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_stagger_dummy::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 119582 - Purifying Brew
class spell_monk_purifying_brew : public SpellScript
{
    PrepareSpellScript(spell_monk_purifying_brew);

    void HandleHit()
    {
        Unit* monk = GetHitUnit();
        monk->RemoveAurasDueToSpell(SPELL_MONK_STAGGER);
        monk->RemoveAurasDueToSpell(SPELL_MONK_LIGHT_STAGGER);
        monk->RemoveAurasDueToSpell(SPELL_MONK_MODERATE_STAGGER);
        monk->RemoveAurasDueToSpell(SPELL_MONK_HEAVY_STAGGER);
    }

    void Register() override
    {
        // T16 4P proc must be handled first
        AfterHit += SpellHitFn(spell_monk_purifying_brew::HandleHit);
    }
};

// 115072 - Expel Harm 
class spell_monk_expel_harm : public spell_monk_specific_damage
{
    PrepareSpellScript(spell_monk_expel_harm);

protected:
    void HandleHit()
    {
        if (int32 damage = CalculatePct(GetHitHeal(), 50))
        {
            // Apply percent mods manually
            for (auto&& itr : GetCaster()->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE))
            {
                if (itr->GetMiscValue() & GetSpellInfo()->GetSchoolMask())
                {
                    if (itr->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(damage, itr->GetFloatAmount());
                    else if (!(itr->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && (itr->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(damage, itr->GetFloatAmount());
                    else if (GetCaster()->ToPlayer() && GetCaster()->ToPlayer()->HasItemFitToSpellRequirements(itr->GetSpellInfo()))
                        AddPct(damage, itr->GetFloatAmount());
                }
            }
            GetCaster()->CastCustomSpell(SPELL_MONK_EXPEL_HARM_DAMAGE, SPELLVALUE_BASE_POINT0, damage, GetCaster(), true);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_expel_harm::HandleHit);
        OnEffectLaunchTarget += SpellEffectFn(spell_monk_expel_harm::CalculateDamage, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 115460 - Healing Sphere
class spell_monk_healing_sphere : public SpellScript
{
    PrepareSpellScript(spell_monk_healing_sphere);

    void RemoveOldSphere()
    {
        std::list<AreaTrigger*> runes;
        GetCaster()->GetAreaTriggerList(runes, SPELL_MONK_HEALING_SPHERE);
        while (runes.size() > 2)
        {
            runes.front()->Remove();
            runes.pop_front();
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_healing_sphere::RemoveOldSphere);
    }
};

// 228
class sat_monk_healing_sphere : public IAreaTriggerOnce
{
    void OnInit() override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_HEALING_SPHERE_STACK, true);
    }

    void OnDestroy() override
    {
        if (Aura* stacks = GetCaster()->GetAura(SPELL_MONK_HEALING_SPHERE_STACK))
            stacks->ModStackAmount(-1);
    }

    void OnExpire() override
    {
        std::list<Unit*> targets;
        Trinity::AnyFriendlyUnitInObjectRangeCheck check(GetTarget(), GetCaster(), 6.0f);
        Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(GetTarget(), targets, check);
        GetTarget()->VisitNearbyObject(12.0f, searcher);
        if (targets.empty())
            return;
        targets.sort(Trinity::HealthPctOrderPred());
        GetCaster()->CastSpell(*targets.front(), SPELL_MONK_HEALING_SPHERE_EXPIRE_HEAL, true);
    }

    bool CheckTriggering(WorldObject* object) override
    {
        Unit* unit = object->ToUnit();
        return unit && (!unit->IsFullHealth() || unit->HasAuraType(SPELL_AURA_SCHOOL_HEAL_ABSORB)) && unit->IsFriendlyTo(GetCaster());
    }

    void OnTrigger(WorldObject* target) override
    {
        GetCaster()->CastSpell(target->ToUnit(), SPELL_MONK_HEALING_SPHERE_HEAL, true);
    }
};

// 124502 - Gift of the Ox
class spell_monk_gift_of_the_ox : public AuraScript
{
    PrepareAuraScript(spell_monk_gift_of_the_ox);

    bool left = true;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* monk = GetUnitOwner()->ToPlayer();
        if (!monk)
            return;

        float chance;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS)
        {
            chance = 10.0f;
        }
        else
        {
            chance = 3.0f;
            if (Item const* weapon = monk->GetWeaponForAttack(BASE_ATTACK))
                if (weapon->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
                    chance = 6.0f;

            chance *= (monk->GetAttackTime(BASE_ATTACK) / 1000.0f);
        }

        if (roll_chance_f(chance))
        {
            monk->CastSpell(monk, left ? SPELL_MONK_GIFT_OF_THE_OX_LEFT : SPELL_MONK_GIFT_OF_THE_OX_RIGHT, true);
            left = !left;
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_gift_of_the_ox::HandleProc);
    }
};

// 124503, 124506 - Gift of the Ox
class spell_monk_summon_gift_of_the_ox : public SpellScript
{
    PrepareSpellScript(spell_monk_summon_gift_of_the_ox);

    void SelectDest(SpellDestination& dest)
    {
        float min = GetSpellInfo()->Id == SPELL_MONK_GIFT_OF_THE_OX_RIGHT ? 260.0f : 45.0f;
        float max = GetSpellInfo()->Id == SPELL_MONK_GIFT_OF_THE_OX_RIGHT ? 315.0f : 100.0f;
        min = min * M_PI / 180.0f;
        max = max * M_PI / 180.0f;

        float angle = frand(min, max);
        float dist = frand(1.5f, 2.0f);

        Position pos = GetCaster()->GetFirstCollisionPosition(dist, angle);
        dest._position = WorldLocation(GetCaster()->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
    }

    void Register() override
    {
        auto target = m_scriptSpellId == SPELL_MONK_GIFT_OF_THE_OX_RIGHT ? TARGET_DEST_CASTER_RIGHT : TARGET_DEST_DEST_LEFT;
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_monk_summon_gift_of_the_ox::SelectDest, EFFECT_0, target);
    }
};

// 373
class sat_monk_gift_of_the_ox : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        if (object != GetCaster())
            return false;
        if (GetCaster()->IsFullHealth() && !GetCaster()->HasAuraType(SPELL_AURA_SCHOOL_HEAL_ABSORB))
            return false;
        return GetCaster()->IsInDist(GetTarget(), m_range);
    }

    void OnTrigger(WorldObject* target) override
    {
        GetCaster()->CastSpell(target->ToUnit(), SPELL_MONK_GIFT_OF_THE_OX_HEAL, true);
    }
};

// 120954 - Fortifying Brew
class spell_monk_fortifying_brew : public AuraScript
{
    PrepareAuraScript(spell_monk_fortifying_brew);

    void CalculateHealth(AuraEffect const*, float& amount, bool&)
    {
        amount = GetUnitOwner()->CountPctFromMaxHealth(GetUnitOwner()->HasAura(SPELL_MONK_GLYPH_OF_FORTIFYING_BREW) ? 10 : 20);
    }

    void CalculateMitigation(AuraEffect const*, float& amount, bool&)
    {
        if (GetUnitOwner()->HasAura(SPELL_MONK_GLYPH_OF_FORTIFYING_BREW))
            amount += 5;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_fortifying_brew::CalculateHealth, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_fortifying_brew::CalculateMitigation, EFFECT_3, SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT);
    }
};

// 128938 - Brewing: Elusive Brew 
class spell_monk_brewing_elusive_brew : public AuraScript
{
    PrepareAuraScript(spell_monk_brewing_elusive_brew);

    float stacks = 0.0f;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MELEE_AUTO_ATTACK && eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
        {
            Player* monk = GetUnitOwner()->ToPlayer();
            if (!monk)
                return;

            Item* weapon = monk->GetWeaponForAttack(BASE_ATTACK);
            float stackAmount;
            if (weapon && weapon->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
                stackAmount = 3.0f * (monk->GetAttackTime(BASE_ATTACK) / 1000.0f) / 3.6f;
            else
                stackAmount = 1.5f * (monk->GetAttackTime(BASE_ATTACK) / 1000.0f) / 2.6f;

            if (AuraEffect* brewStacks = monk->GetAuraEffect(SPELL_MONK_ELUSIVE_BREW_STACKS, EFFECT_0))
            {
                float total = brewStacks->GetUserData<float>() + stackAmount;
                int32 real = total;
                brewStacks->GetBase()->ModStackAmount(real);
                brewStacks->GetBase()->RefreshDuration();
                brewStacks->SetUserData<float>(total - real);
            }
            else
            {
                monk->CastCustomSpell(SPELL_MONK_ELUSIVE_BREW_STACKS, SPELLVALUE_AURA_STACK, stackAmount, monk, true);
                if (AuraEffect* brewStacks = monk->GetAuraEffect(SPELL_MONK_ELUSIVE_BREW_STACKS, EFFECT_0))
                    brewStacks->SetUserData<float>(stackAmount - brewStacks->GetBase()->GetStackAmount());
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_brewing_elusive_brew::HandleProc);
    }
};

// 115308 - Elusive Brew
class spell_monk_elusive_brew : public AuraScript
{
    PrepareAuraScript(spell_monk_elusive_brew);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        int32 stackAmount = 0;

        Unit* monk = GetCaster();
        Aura* brewStacks = monk->GetAura(SPELL_MONK_ELUSIVE_BREW_STACKS);
        if (!brewStacks)
            return;

        SetMaxDuration(brewStacks->GetStackAmount() * 1000);
        RefreshDuration();
        monk->RemoveAura(SPELL_MONK_ELUSIVE_BREW_STACKS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_elusive_brew::HandleApply, EFFECT_0, SPELL_AURA_MOD_DODGE_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 113656 - Fists of Fury
class spell_monk_fists_of_fury : public AuraScript, public spell_monk_specific_damage_base
{
    PrepareAuraScript(spell_monk_fists_of_fury);

    void CalculateDamage(AuraEffect const*, float& amount, bool&)
    {
        if (Player* monk = GetUnitOwner()->GetAffectingPlayer())
        {
            amount = spell_monk_specific_damage_base::CalculateDamage(monk, GetSpellInfo()->Id);

            float doneTotalMod = 1.0f;

            for (auto itr : GetUnitOwner()->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE))
            {
                if (itr->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
                {
                    if (itr->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(doneTotalMod, itr->GetFloatAmount());
                    else if (!(itr->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && (itr->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(doneTotalMod, itr->GetFloatAmount());
                    else if (GetUnitOwner() == monk && monk->HasItemFitToSpellRequirements(itr->GetSpellInfo()))
                        AddPct(doneTotalMod, itr->GetFloatAmount());
                }
            }

            amount *= doneTotalMod;
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        // Remove visual, because it has -1 duration
        auto action = [=](WorldObject* target)
        {
            if (Unit* unit = target->ToUnit())
                unit->RemoveAura(123154, GetCasterGUID());
        };
        Trinity::WorldObjectWorker<decltype(action)> worker{ GetUnitOwner(), action, GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER };
        GetUnitOwner()->VisitNearbyObject(10.0f, worker);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_fists_of_fury::CalculateDamage, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_fists_of_fury::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 117418 - Fists of Fury
class spell_monk_fists_of_fury_damage : public SpellScript
{
    PrepareSpellScript(spell_monk_fists_of_fury_damage);

    uint32 targetCount = 1;

    void CountTagets(std::list<WorldObject*>& targets)
    {
        targetCount = targets.size();
    }

    void HandleLaunch(SpellEffIndex)
    {
        if (AuraEffect const* effect = GetCaster()->GetAuraEffect(113656, EFFECT_1))
            SetEffectValue(effect->GetAmount());
    }

    void HandleHit()
    {
        int32 damage = GetHitDamage();

        float doneTotalMod = 1.0f;

        for (auto itr : GetCaster()->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE))
        {
            if (itr->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            {
                if (itr->GetSpellInfo()->EquippedItemClass == -1)
                    AddPct(doneTotalMod, itr->GetFloatAmount());
                else if (!(itr->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && (itr->GetSpellInfo()->EquippedItemSubClassMask == 0))
                    AddPct(doneTotalMod, itr->GetFloatAmount());
                else if (GetCaster()->ToPlayer() && GetCaster()->ToPlayer()->HasItemFitToSpellRequirements(itr->GetSpellInfo()))
                    AddPct(doneTotalMod, itr->GetFloatAmount());
            }
        }

        damage /= doneTotalMod;

        SetHitDamage(damage / targetCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_fists_of_fury_damage::CountTagets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
        OnEffectLaunchTarget += SpellEffectFn(spell_monk_fists_of_fury_damage::HandleLaunch, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnHit += SpellHitFn(spell_monk_fists_of_fury_damage::HandleHit);
    }
};

// 120086 - Fists of Fury
class spell_monk_fists_of_fury_stun : public SpellScript
{
    PrepareSpellScript(spell_monk_fists_of_fury_stun);

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
            targets.clear();
        else
            targets.remove_if(Trinity::UnitAuraCheck(true, GetSpellInfo()->Id));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_fists_of_fury_stun::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
    }
};

// 126046 - Adaptation
class spell_monk_adaptation : public AuraScript
{
    PrepareAuraScript(spell_monk_adaptation);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* monk = GetUnitOwner();
        if (monk->HasAuraType(SPELL_AURA_MOD_DISARM) || monk->HasAuraType(SPELL_AURA_MOD_DISARM_OFFHAND))
            for (auto&& it : eventInfo.GetSpellInfo()->Effects)
                if (it.ApplyAuraName == SPELL_AURA_MOD_DISARM || it.ApplyAuraName == SPELL_AURA_MOD_DISARM_OFFHAND)
                    return !monk->HasAura(SPELL_MONK_ADAPTATION);
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_adaptation::CheckProc);
    }
};

// 117959 - Crackling Jade Lightning 
class spell_monk_crackling_jade_lightning_knockback : public AuraScript
{
    PrepareAuraScript(spell_monk_crackling_jade_lightning_knockback);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Spell* spell = GetUnitOwner()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_CRACKLING_JADE_LIGHTNING)
                if (eventInfo.GetActor() == spell->GetExplTargetUnit())
                    return !GetUnitOwner()->HasAura(SPELL_MONK_CRACKLING_JADE_LIGHTNING_COOLDOWN);
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* monk = GetUnitOwner();
        monk->CastSpell(eventInfo.GetActor(), SPELL_MONK_CRACKLING_JADE_SHOCK, true);
        monk->CastSpell(monk, SPELL_MONK_CRACKLING_JADE_LIGHTNING_COOLDOWN, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_crackling_jade_lightning_knockback::CheckProc);
        OnProc += AuraProcFn(spell_monk_crackling_jade_lightning_knockback::HandleProc);
    }
};

// 115073 - Spinning Fire Blossom
class spell_monk_spinning_fire_blossom : public SpellScript
{
    PrepareSpellScript(spell_monk_spinning_fire_blossom);

    bool tenYards = false;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        float dist = 50.0f;
        WorldObject* closestTarget = nullptr;
        for (auto&& target : targets)
        {
            if (target->ToUnit()->HasStealthAura() || target->ToUnit()->HasInvisibilityAura())
                continue;

            if (target->GetDistance(caster) < dist)
            {
                dist = target->GetDistance(caster);
                closestTarget = target;
            }
        }

        targets.clear();
        if (closestTarget)
        {
            targets.push_back(closestTarget);
            tenYards = closestTarget->GetDistance(caster) > 10.0f;
        }
    }

    void HandleOnHit()
    {
        if (tenYards)
        {
            SetHitDamage(int32(GetHitDamage() * 1.5f));
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MONK_SPINNING_FIRE_BLOSSOM_ROOT, true);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_spinning_fire_blossom::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_129);
        OnHit += SpellHitFn(spell_monk_spinning_fire_blossom::HandleOnHit);
    }
};

// 123408 - Spinning Fire Blossom (glyphed)
class spell_monk_spinning_fire_blossom_glyphed : public SpellScript
{
    PrepareSpellScript(spell_monk_spinning_fire_blossom_glyphed);

    bool tenYards = false;

    SpellCastResult CheckTarget()
    {
        Unit* target = GetExplTargetUnit();
        if (!target || GetCaster()->IsFriendlyTo(target))
            return SPELL_FAILED_BAD_TARGETS;

        tenYards = GetCaster()->GetExactDist(target) > 10.0f;
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        if (tenYards)
            SetHitDamage(int32(GetHitDamage() * 1.5f));
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_spinning_fire_blossom_glyphed::CheckTarget);
        OnHit += SpellHitFn(spell_monk_spinning_fire_blossom_glyphed::HandleHit);
    }
};

class spell_monk_proc_chi_consumer : public AuraScript
{
    PrepareAuraScript(spell_monk_proc_chi_consumer);
protected:
    uint32 m_chiConsumed = 0;
    uint32 const m_chiPerStack = 4;
    Player* m_caster = nullptr;

    bool Load() override
    {
        m_caster = GetOwner()->ToPlayer();
        return m_caster != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* spell = eventInfo.TriggeredBySpell();
        if (!spell || spell->GetPowerType() != POWER_CHI)
            return false;

        if (spell->m_targets.GetUnitTarget() && eventInfo.GetActionTarget() != spell->m_targets.GetUnitTarget())
            return false;

        if (spell->GetSpellInfo()->Id == SPELL_MONK_UPLIFT)
        {
            if (m_caster->HasSpellCooldown(SPELL_MONK_MANA_TEA_STACKS))
                return false;
            m_caster->AddSpellCooldown(SPELL_MONK_MANA_TEA_STACKS, Milliseconds(100));
        }

        // https://www.mmo-champion.com/threads/1159132-Mistweaver-Stance-of-the-Fierce-Tiger
        if (GetSpellInfo()->Id == SPELL_MONK_MANA_TEA_DRIVER && !GetUnitOwner()->HasAura(SPELL_MONK_STANCE_OF_THE_WISE_SERPENT))
            return false;

        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        for (auto&& itr : eventInfo.GetSpellInfo()->SpellPowers)
            if (itr->PowerType == POWER_CHI)
                m_chiConsumed += itr->Cost;

        if (m_chiConsumed >= m_chiPerStack)
        {
            m_chiConsumed -= m_chiPerStack;
            DoProc();
        }
    }

    virtual void DoProc() = 0;

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_proc_chi_consumer::CheckProc);
        OnProc += AuraProcFn(spell_monk_proc_chi_consumer::HandleProc);
    }
};

// 123980 - Brewing: Tigereye Brew 
class spell_monk_brewing_tigereye_brew : public spell_monk_proc_chi_consumer
{
    void DoProc() override
    {
        int32 stacks = 1;
        if (AuraEffect* mastery = m_caster->GetAuraEffect(SPELL_MONK_MASTERY_BOTTELED_FURY, EFFECT_0))
            if (roll_chance_f(mastery->GetFloatAmount()))
                stacks = 2;
        m_caster->CastCustomSpell(SPELL_MONK_TIGEREYE_BREW_STACKS, SPELLVALUE_AURA_STACK, stacks, m_caster, true);
        m_caster->CastSpell(m_caster, SPELL_MONK_PLUS_ONE_TIGEREYE_BREW, true);
        if (stacks > 1)
            m_caster->CastSpell(m_caster, SPELL_MONK_PLUS_ONE_TIGEREYE_BREW, true);
    }
};

// 116740 - Tigereye Brew
class spell_monk_tigereye_brew: public SpellScript
{
    PrepareSpellScript(spell_monk_tigereye_brew);

    void HandleHit()
    {
        if (AuraEffect* brewStacks = GetCaster()->GetAuraEffect(SPELL_MONK_TIGEREYE_BREW_STACKS, EFFECT_2))
        {
            float amount = brewStacks->GetAmount();
            if (AuraEffect const* setBonus = GetCaster()->GetAuraEffect(SPELL_MONK_T15_WINDWALKER_4P_BONUS, EFFECT_0))
                amount = amount / 6.0f * 6.5f;
            brewStacks->GetBase()->ModStackAmount(-10);
            GetHitAura()->GetEffect(EFFECT_0)->ChangeAmount(amount);
            GetHitAura()->GetEffect(EFFECT_1)->ChangeAmount(amount);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_tigereye_brew::HandleHit);
    }
};

// 125195 - Tigereye Brew (caster aura state)
class spell_monk_tigereye_brew_stacks : public visual_stacks_controller
{
    PrepareAuraScript(spell_monk_tigereye_brew_stacks);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        GetEffect(eff->GetEffIndex())->SetAmount(std::min(GetStackAmount() * 6, 60));
        visual_stacks_controller::HandleApply(eff, mode);
    }

    void Register() override
    {
        Initialize(SPELL_MONK_TIGEREYE_BREW_FULL_STACKS_VISUAL, 10);
        OnEffectApply += AuraEffectApplyFn(spell_monk_tigereye_brew_stacks::HandleApply, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_tigereye_brew_stacks::HandleRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 115869 - Mana Tea Driver <DND>
class spell_monk_brewing_mana_tea : public spell_monk_proc_chi_consumer
{
    void DoProc() override
    {
        int32 stacks = 1;
        if (roll_chance_f(m_caster->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + SPELL_SCHOOL_NATURE)))
            stacks = 2;
        m_caster->CastCustomSpell(SPELL_MONK_MANA_TEA_STACKS, SPELLVALUE_AURA_STACK, stacks, m_caster, true);
        m_caster->CastSpell(m_caster, SPELL_MONK_PLUS_ONE_MANA_TEA, true);
        if (stacks > 1)
            m_caster->CastSpell(m_caster, SPELL_MONK_PLUS_ONE_MANA_TEA, true);
    }
};

// 115294 - Mana Tea
class spell_monk_mana_tea : public AuraScript
{
    PrepareAuraScript(spell_monk_mana_tea);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (Aura* manaTea = GetUnitOwner()->GetAura(SPELL_MONK_MANA_TEA_STACKS))
        {
            GetAura()->SetMaxDuration(eff->GetAmplitude() * manaTea->GetStackAmount());
            GetAura()->RefreshDuration(false);
        }
    }

    void HandleTick(AuraEffect const*)
    {
        if (Aura* manaTea = GetUnitOwner()->GetAura(SPELL_MONK_MANA_TEA_STACKS))
            manaTea->ModStackAmount(-1);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_mana_tea::HandleApply, EFFECT_0, SPELL_AURA_OBS_MOD_POWER, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_mana_tea::HandleTick, EFFECT_0, SPELL_AURA_OBS_MOD_POWER);
    }
};

// 123761 - Mana Tea
class spell_monk_mana_tea_glyphed : public SpellScript
{
    PrepareSpellScript(spell_monk_mana_tea_glyphed);

    SpellCastResult CheckCast()
    {
        Aura* manaTea = GetCaster()->GetAura(SPELL_MONK_MANA_TEA_STACKS);
        return manaTea && manaTea->GetStackAmount() >= 2 ? SPELL_CAST_OK : SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
    }

    void HandleCast()
    {
        if (Aura* manaTea = GetCaster()->GetAura(SPELL_MONK_MANA_TEA_STACKS))
            manaTea->ModStackAmount(-2);
    }

    void HandleHit(SpellEffIndex)
    {
        SetEffectValue(GetEffectValue() * 2);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_mana_tea_glyphed::CheckCast);
        OnCast += SpellCastFn(spell_monk_mana_tea_glyphed::HandleCast);
        OnEffectHitTarget += SpellEffectFn(spell_monk_mana_tea_glyphed::HandleHit, EFFECT_0, SPELL_EFFECT_ENERGIZE_PCT);
    }
};

// 100780, 108557, 115698, 115687, 115693, 115695 -  Jab,
// 116335 - Soothing Mist, 129881 - Spinning Crane Kick, 115072 - Expel Harm, 123333 - Crackling Jade Lightning
class spell_monk_power_strikes : public SpellScript
{
    PrepareSpellScript(spell_monk_power_strikes);

    void HandleHit(SpellEffIndex)
    {
        Unit* monk = GetCaster();
        if (Aura* aura = monk->GetAura(SPELL_MONK_POWER_STRIKES))
        {
            if (monk->GetPower(POWER_CHI) + GetEffectValue() >= monk->GetMaxPower(POWER_CHI))
                monk->CastSpell(monk, SPELL_MONK_CHI_SPHERE, true);
            else
                SetEffectValue(GetEffectValue() + 1);
            aura->Remove();

            if (AuraEffect* talent = monk->GetAuraEffect(SPELL_MONK_POWER_STRIKES_TALENT, EFFECT_1))
                talent->ResetPeriodic();
        }
    }

    void Register() override
    {
        SpellEffIndex index = EFFECT_1;
        switch (m_scriptSpellId)
        {
            case 116335: // Soothing Mist
            case 129881: // Spinning Crane Kick
            case 123333: // Crackling Jade Lightning
                index = EFFECT_0;
            default:
                break;
        }
        OnEffectHitTarget += SpellEffectFn(spell_monk_power_strikes::HandleHit, index, SPELL_EFFECT_ENERGIZE);
    }
};

// 336 created by 121286 - Chi Sphere
class sat_monk_chi_sphere : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object == GetCaster();
    }

    void OnTrigger(WorldObject*) override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_CHI_SHERE_ENERGIZE, true);
    }
};

// 122057 - Clash, 126449 - Clash (symbiosis)
class spell_monk_clash : public SpellScript
{
    PrepareSpellScript(spell_monk_clash);

    void HandleHit()
    {
        bool symbiosis = GetSpellInfo()->Id != 122057;
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        float dist = caster->GetExactDist(target) / 2;
        Position hitPos = caster->GetPosition();
        caster->MovePosition(hitPos, dist, caster->GetAngle(target) - caster->GetOrientation());
        caster->CastSpell(hitPos, symbiosis ? 126450 : 122235 , true);
        hitPos = target->GetPosition();
        target->MovePosition(hitPos, dist, target->GetAngle(caster) - target->GetOrientation());
        target->CastSpell(hitPos, symbiosis ? 126452 : 122252, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_monk_clash::HandleHit);
    }
};

// 122235 - Clash, 126450 - Clash (symbiosis)
class spell_monk_clash_caster_charge : public SpellScript
{
    PrepareSpellScript(spell_monk_clash_caster_charge);

    void HandleHit(SpellEffIndex)
    {
        bool symbiosis = GetSpellInfo()->Id != 122235;
        GetCaster()->CastSpell(GetCaster(), symbiosis ? 126451 : 122242, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_monk_clash_caster_charge::HandleHit, EFFECT_0, SPELL_EFFECT_CHARGE_DEST);
    }
};

// 61146
struct npc_black_ox_statue : public ScriptedAI
{
    npc_black_ox_statue(Creature* c) :
        ScriptedAI(c) { }

    void InitializeAI() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CastSpell(me->GetOwner(), SPELL_MONK_SANCTUARY_OF_THE_OX, true);
    }

    void Unsummoned() override
    {
        if (Unit* owner = me->GetOwner())
            owner->RemoveAurasDueToSpell(SPELL_MONK_SANCTUARY_OF_THE_OX);
    }

    void UpdateAI(uint32) override { }
};

// 115070 - Stance of the Wise Serpent
// 130324 - Eminence (Xuen)
class spell_monk_eminence_driver : public AuraScript
{
    PrepareAuraScript(spell_monk_eminence_driver);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo()->GetDamage())
            return false;

        Player* monk = GetUnitOwner()->GetAffectingPlayer();
        if (!monk)
            return false;

        if (monk->GetShapeshiftForm() != FORM_WISE_SERPENT)
            return false;

        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MELEE_AUTO_ATTACK)
            if (!monk->HasAura(SPELL_MONK_SERPENTS_ZEAL))
                return false;
        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 heal = CalculatePct(eventInfo.GetDamageWithoutResilience(), 25);

        Unit* monk = GetUnitOwner()->GetAffectingPlayer();
        std::list<TempSummon*> summons;
        monk->GetSummons(summons, NPC_JADE_SERPENT_STATUE);

        monk->CastCustomSpell(SPELL_MONK_EMINENCE, SPELLVALUE_BASE_POINT0, heal, monk, true);
        if (!summons.empty() && monk->IsWithinDistInMap(summons.front(), 100.f, true))
            monk->CastCustomSpell(SPELL_MONK_EMINENCE_STATUE, SPELLVALUE_BASE_POINT0, heal, summons.front(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_eminence_driver::CheckProc);
        OnProc += AuraProcFn(spell_monk_eminence_driver::HandleProc);
    }
};

// 115070 - Stance of the Wise Serpent
class spell_monk_stance_of_wise_serpent : public AuraScript
{
    PrepareAuraScript(spell_monk_stance_of_wise_serpent);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_STANCE_OF_THE_WISE_SERPENT_HASTE, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_MONK_STANCE_OF_THE_WISE_SERPENT_HASTE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_stance_of_wise_serpent::HandleApply, EFFECT_5, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_stance_of_wise_serpent::HandleRemove, EFFECT_5, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 115175 - Soothing Mist
class spell_monk_soothing_mist : public AuraScript
{
    PrepareAuraScript(spell_monk_soothing_mist);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (caster && caster->ToPlayer())
        {
            std::list<TempSummon*> summons;
            caster->GetSummons(summons, NPC_JADE_SERPENT_STATUE);
            if (!summons.empty() && caster->IsWithinDistInMap(summons.front(), 100.f, true))
                summons.front()->CastSpell(summons.front(), SPELL_MONK_SOOTHING_MIST_STATUE_TRIGGER, true);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (caster && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
        {
            std::list<TempSummon*> summons;
            caster->GetSummons(summons, NPC_JADE_SERPENT_STATUE);
            if (!summons.empty())
                summons.front()->InterruptSpell(CURRENT_CHANNELED_SPELL);
        }
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
            if (roll_chance_i(GetSpellInfo()->ProcChance))
                caster->CastSpell(caster, SPELL_MONK_SOOTHING_MIST_ENERGIZE, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_soothing_mist::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_soothing_mist::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_soothing_mist::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 116346 - Soothing Mists Trigger
class spell_monk_soothing_mist_tirgger : public AuraScript
{
    PrepareAuraScript(spell_monk_soothing_mist_tirgger);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_MONK_SOOTHING_MISTS_MISSILE, true);
        if (Player* monk = GetUnitOwner()->ToPlayer())
        {
            std::list<TempSummon*> summons;
            monk->GetSummons(summons, NPC_JADE_SERPENT_STATUE);
            if (!summons.empty())
            {
                Unit* statue = summons.front();
                if (Spell* spell = statue->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    if (Unit* target = ObjectAccessor::GetUnit(*statue, spell->m_targets.GetObjectTargetGUID()))
                        statue->CastSpell(target, SPELL_MONK_SOOTHING_MISTS_MISSILE, true);
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_soothing_mist_tirgger::HandleProc);
    }
};

// 125950 - Soothing Mist 
class spell_monk_soothing_mist_statue : public AuraScript
{
    PrepareAuraScript(spell_monk_soothing_mist_statue);

    void HandleTick(AuraEffect const*)
    {
        if (Unit* statue = GetCaster())
            if (Unit* monk = statue->GetOwner())
                monk->CastSpell(GetTarget(), SPELL_MONK_SOOTHING_MIST_STATUE_HEAL, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_soothing_mist_statue::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 125962 - Soothing Mist <Statue> Trigger 
class spell_monk_soothing_mist_statue_trigger : public SpellScript
{
    PrepareSpellScript(spell_monk_soothing_mist_statue_trigger);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (!targets.empty())
        {
            targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
            targets.resize(1);
        }
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MONK_SOOTHING_MIST_STATUE_CHANNEL, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_soothing_mist_statue_trigger::SelectTarget, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnHit += SpellHitFn(spell_monk_soothing_mist_statue_trigger::HandleHit);
    }
};

// 119611 - Renewing Mist 
class spell_monk_renewing_mist : public AuraScript
{
    PrepareAuraScript(spell_monk_renewing_mist);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        // Just calculate as it specified in 115151
        if (Unit* caster = GetCaster())
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_MONK_RENEWING_MIST))
                amount = caster->SpellHealingBonusDone(GetUnitOwner(), spellInfo, EFFECT_2, spellInfo->Effects[EFFECT_2].CalcValue(caster), DOT);
    }

    void RefreshCharges(uint8& charges)
    {
        // On re-apply the charges will be dropped =/
        charges = 3;
    }

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        SetCharges(eff->GetAmount());
        if (Unit* monk = GetCaster())
        {
            if (!GetAura()->IsBoundToCaster()) // Reapply
                monk->BindAura(GetAura());
            monk->CastSpell(monk, SPELL_MONK_UPLIFT_ALLOWING_CAST, true);

            if (AuraEffect const* setBonus = monk->GetAuraEffect(SPELL_MONK_T15_MISTWEAVER_2P_BONUS, EFFECT_0))
            {
                if (HasEffect(EFFECT_0))
                {
                    int32 pct = setBonus->GetAmount() * (3 - GetCharges());
                    int32 amount = GetEffect(EFFECT_0)->GetAmount();
                    AddPct(amount, pct);
                    GetEffect(EFFECT_0)->SetAmount(amount);
                }
            }
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* monk = GetCaster())
            if (!monk->GetBoundAurasBySpellId(GetId()))
                monk->RemoveAurasDueToSpell(SPELL_MONK_UPLIFT_ALLOWING_CAST);
    }

    void HandleTick(AuraEffect const*)
    {
        Unit* monk = GetCaster();
        if (!monk)
            return;

        monk->CastSpell(monk, SPELL_MONK_UPLIFT_ALLOWING_CAST, true);
        if (GetAura()->GetCharges() <= 1)
            return;

        monk->CastSpell(GetTarget(), SPELL_MONK_RENEWING_MIST_JUMP, true);
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_renewing_mist::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        OnRefreshCharges += AuraRefreshChargesFn(spell_monk_renewing_mist::RefreshCharges);
        OnEffectApply += AuraEffectApplyFn(spell_monk_renewing_mist::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_renewing_mist::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_renewing_mist::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 119607 - Renewing Mist
class spell_monk_renewing_mist_jump : public spell_smart_heal
{
    PrepareSpellScript(spell_monk_renewing_mist_jump);

    int32 m_charges = 0;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        uint64 guid = GetCaster()->GetGUID();
        Unit* target = GetExplTargetUnit();
        AuraEffect* triggeredBy = target ? target->GetAuraEffect(SPELL_MONK_RENEWING_MIST_HOT, EFFECT_1, guid) : nullptr;
        if (!triggeredBy || triggeredBy->GetAmount() < 1)
        {
            targets.clear();
            return;
        }

        targets.remove_if([](WorldObject const* target) { return target->ToUnit() == nullptr; });
        targets.remove_if(Trinity::UnitAuraCheck{ true, SPELL_MONK_RENEWING_MIST_HOT, guid });
        targets.remove_if([](WorldObject const* target)
        {
            return target->ToUnit()->IsSummon() && target->ToUnit()->ToTempSummon()->IsMinorGuardian();
        });
        targets.sort(Trinity::DistanceCompareOrderPred{ GetExplTargetUnit(), !GetCaster()->HasAura(SPELL_MONK_GLYPH_OF_RENEWING_MIST) });

        if (!targets.empty())
        {
            Unit* target = targets.front()->ToUnit();
            for (auto&& itr : targets)
            {
                if (!itr->ToUnit()->IsFullHealth())
                {
                    target = itr->ToUnit();
                    break;
                }
            }

            targets.clear();
            targets.push_back(target);

            m_charges = triggeredBy->GetAmount() - 1;
            triggeredBy->GetBase()->SetCharges(1);
        }
    }

    void HandleHit(SpellEffIndex)
    {
        GetCaster()->CastCustomSpell(SPELL_MONK_RENEWING_MIST_HOT, SPELLVALUE_BASE_POINT1, m_charges, GetHitUnit(), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_renewing_mist_jump::SelectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_monk_renewing_mist_jump::HandleHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 101546 - Spinning Crane Kick, 116847 - Rushing Jade Wind
class spell_monk_spinning_crane_kick : public AuraScript
{
    PrepareAuraScript(spell_monk_spinning_crane_kick);

    void HandleTick(AuraEffect const* eff)
    {
        if (eff->GetAmount() < 3)
            GetEffect(eff->GetEffIndex())->SetAmount(0);
        if (GetUnitOwner()->HasAura(SPELL_MONK_TEACHINGS_OF_THE_MONASTERY))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_SPINNING_CRANE_KICK_HEAL, true, nullptr, eff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_spinning_crane_kick::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 107270 - Spinning Crane Kick (damage), 117640 - Spinning Crane Kick (heal), 148187 - Rushing Jade Wind (damage)
class spell_monk_spinning_crane_kick_energize_tirgger : public SpellScript
{
    PrepareSpellScript(spell_monk_spinning_crane_kick_energize_tirgger);

    void HandleHit()
    {
        if (GetHitUnit() != GetCaster())    // For heal part
        {
            auto triggeredBy = GetSpell()->TriggeredByAuraSpell();
            if (!triggeredBy)
                return;
            if (AuraEffect* eff = GetCaster()->GetAuraEffect(triggeredBy->Id, EFFECT_0))
            {
                uint32 hitCount = eff->GetAmount();
                if (hitCount != 3)
                {
                    eff->SetAmount(++hitCount);
                    if (hitCount == 3)
                        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_SPINNING_CRANE_KICK_ENERGIZE, true);
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_spinning_crane_kick_energize_tirgger::HandleHit);
    }
};

// 107270 - Spinning Crane Kick, 148187 - Rushing Jade Wind
class spell_monk_spinning_crane_kick_damage : public SpellScript
{
    PrepareSpellScript(spell_monk_spinning_crane_kick_damage);

    uint32 m_counter = 0;

    void HandleHit()
    {
        Player* monk = GetCaster()->ToPlayer();
        if (!monk || !monk->HasSpell(SPELL_MONK_MUSCLE_MEMORY))
            return;

        if (m_counter != 3)
            if (++m_counter == 3)
                GetCaster()->CastSpell(GetCaster(), SPELL_MONK_MUSCLE_MEMORY_BUFF, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_spinning_crane_kick_damage::HandleHit);
    }
};

// 117640 - Spinning Crane Kick
class spell_monk_spinning_crane_kick_heal : public SpellScript
{
    PrepareSpellScript(spell_monk_spinning_crane_kick_heal);

    void HandleHit()
    {
        if (GetSpell()->TriggeredByAuraSpell() && GetSpell()->TriggeredByAuraSpell()->Id != SPELL_MONK_SPINNING_CRANE_KICK)
            SetHitHeal(GetHitHeal() * 0.8f);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_spinning_crane_kick_heal::HandleHit);
    }
};

// 116694 - Surging Mist
class spell_monk_surging_mist : public SpellScript
{
    PrepareSpellScript(spell_monk_surging_mist);

    bool Load() override
    {
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                AddTriggerFlag(TRIGGERED_CAST_DIRECTLY);
        return true;
    }

    void CastHeal(SpellEffIndex)
    {
        Unit* target = GetHitUnit();
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                if (Unit* channelTarget = spell->GetExplTargetUnit())
                    target = channelTarget;
        GetCaster()->CastSpell(target, SPELL_MONK_SURGING_MIST_HEAL, true);
    }
protected:
    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_monk_surging_mist::CastHeal, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 123273 - Surging Mist
class spell_monk_surging_mist_glyphed : public spell_monk_surging_mist
{
    PrepareSpellScript(spell_monk_surging_mist_glyphed);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Unit* preferedTarget = nullptr;
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                preferedTarget = ObjectAccessor::GetUnit(*GetCaster(), spell->m_targets.GetUnitTargetGUID());

        if (preferedTarget)
        {
            targets.clear();
            targets.push_back(preferedTarget);
        }
        else if (!targets.empty())
        {
            Spell::ApplySmartHeal(GetCaster(), targets, 1);
        }
    }

    void Register() override
    {
        spell_monk_surging_mist::Register();
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_surging_mist_glyphed::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 124682 - Enveloping Mist
class spell_monk_enveloping_mist : public SpellScript
{
    PrepareSpellScript(spell_monk_enveloping_mist);

    bool Load() override
    {
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                AddTriggerFlag(TRIGGERED_CAST_DIRECTLY);
        return true;
    }

    void HandleHit(SpellEffIndex)
    {
        Unit* target = GetHitUnit();
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                if (Unit* channelTarget = spell->GetExplTargetUnit())
                    target = channelTarget;
        GetCaster()->CastSpell(target, SPELL_MONK_ENVELOPING_MIST_HEAL, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_monk_enveloping_mist::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 116670 - Uplift
class spell_monk_thunder_focus_tea_uplift : public SpellScript
{
    PrepareSpellScript(spell_monk_thunder_focus_tea_uplift);

    std::list<WorldObject*> m_targets;

    void FilterTargetsRefresh(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck{ false, SPELL_MONK_RENEWING_MIST_HOT, GetCaster()->GetGUID() });
        m_targets = targets;
        if (!GetCaster()->HasAura(SPELL_MONK_THUNDER_FOCUS_TEA))
            targets.clear();
    }

    void FilterTargetsHeal(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void RefreshRenewingMist(SpellEffIndex)
    {
        if (Aura* aura = GetHitUnit()->GetAura(SPELL_MONK_RENEWING_MIST_HOT))
            aura->RefreshDuration();
    }

    void HandleCast()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_MONK_THUNDER_FOCUS_TEA);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_thunder_focus_tea_uplift::FilterTargetsRefresh, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_thunder_focus_tea_uplift::FilterTargetsHeal, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_monk_thunder_focus_tea_uplift::RefreshRenewingMist, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        AfterCast += SpellCastFn(spell_monk_thunder_focus_tea_uplift::HandleCast);
    }
};

// 116680 - Thunder Focus Tea
class spell_monk_thunder_focus_tea : public SpellScript
{
    PrepareSpellScript(spell_monk_thunder_focus_tea);

    bool Load() override
    {
        if (Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (spell->GetSpellInfo()->Id == SPELL_MONK_SOOTHING_MIST)
                AddTriggerFlag(TRIGGERED_CAST_DIRECTLY);
        return true;
    }
    
    void Register() override { }
};

// 122464 - Dematerialize 
class spell_monk_dematerialize : public AuraScript
{
    PrepareAuraScript(spell_monk_dematerialize);

    Player* m_owner = nullptr;

    bool Load() override
    {
        m_owner = GetOwner()->ToPlayer();
        return m_owner != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!m_owner->HasUnitState(UNIT_STATE_STUNNED) || !eventInfo.GetSpellInfo())
            return false;
        uint32 mask = 1 << MECHANIC_STUN;
        return eventInfo.GetSpellInfo()->Mechanic & mask || eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & mask;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_dematerialize::CheckProc);
    }
};

// 146953 - Glyph of Fortuitous Spheres
class spell_monk_glyph_of_fortuitous_spheres : public AuraScript
{
    PrepareAuraScript(spell_monk_glyph_of_fortuitous_spheres);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HealthBelowPct(25);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_glyph_of_fortuitous_spheres::CheckProc);
    }
};

// 126119 - Sanctuary of the Ox
class spell_monk_sanctaury_of_the_ox : public AuraScript
{
    PrepareAuraScript(spell_monk_sanctaury_of_the_ox);

    uint32 m_damageDealt = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        m_damageDealt += eventInfo.GetDamageInfo()->GetDamage();
        if (m_damageDealt >= 16 * GetUnitOwner()->GetTotalAttackPowerValue(BASE_ATTACK))
        {
            m_damageDealt = 0;
            if (Unit* statue = GetCaster())
                statue->CastSpell(statue, SPELL_MONK_BLACK_OX_STATUE_GUARD_SELECTOR, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_sanctaury_of_the_ox::HandleProc);
    }
};

// 123402, 115295 - Guard (monk ability)
// 118604, 136070 - Guard (statue proc)
class spell_monk_guard : public AuraScript
{
    PrepareAuraScript(spell_monk_guard)

    void calculateAmount(AuraEffect const* effect, float& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        amount += caster->GetTotalAttackPowerValue(BASE_ATTACK) * 1.971f;

        if (!(effect->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
            AddPct(amount, 10);

        if (AuraEffect const* powerGuard = caster->GetAuraEffect(SPELL_MONK_POWER_GUARD, EFFECT_0))
        {
            AddPct(amount, powerGuard->GetAmount());
            powerGuard->GetBase()->Remove();
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_guard::calculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 118605 - Guard
class spell_monk_black_ox_statue_guard_selector : public SpellScript
{
    PrepareSpellScript(spell_monk_black_ox_statue_guard_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Unit* monk = GetCaster()->GetOwner();
        Trinity::NotInRaidWithCheck check{ monk };
        targets.remove_if([&](WorldObject const* target) { return target == monk || check(target); });
        targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
        if (!targets.empty())
            targets.resize(1);
    }

    void HandleHit()
    {
        if (Unit* monk = GetCaster()->GetOwner())
            monk->CastSpell(GetHitUnit(), monk->HasAura(SPELL_MONK_GLYPH_OF_GUARD) ?
                SPELL_MONK_BLACK_OX_STATUE_GUARD_MAGIC : SPELL_MONK_BLACK_OX_STATUE_GUARD, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_black_ox_statue_guard_selector::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnHit += SpellHitFn(spell_monk_black_ox_statue_guard_selector::HandleHit);
    }
};

// 109132 - Roll, 121827 - Roll (wtf is this?)
class spell_monk_roll : public SpellScript
{
    PrepareSpellScript(spell_monk_roll);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasUnitState(UNIT_STATE_ROOT))
            return SPELL_FAILED_ROOTED;

        if (!GetCaster()->IsAlive() && !GetCaster()->HasAura(SPELL_MONK_GLYPH_OF_SPIRIT_ROLL))
            return SPELL_FAILED_CASTER_DEAD;

        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_ROLL_AURA, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_roll::CheckCast);
        OnCast += SpellCastFn(spell_monk_roll::HandleCast);
    }
};

// 107427 - Roll
class spell_monk_roll_aura : public AuraScript
{
    PrepareAuraScript(spell_monk_roll_aura);

    void InitEffects(uint32& effectMask)
    {
        if (!GetUnitOwner()->HasAura(SPELL_MONK_GLYPH_OF_WATER_ROLL))
            effectMask &= ~(1 << EFFECT_3);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_monk_roll_aura::InitEffects);
    }
};

// 115543 - Leer of the Ox
class spell_monk_leer_of_the_ox : public SpellScript
{
    PrepareSpellScript(spell_monk_leer_of_the_ox);

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();
        if (!target || !GetCaster()->IsValidAttackTarget(target))
            return SPELL_FAILED_NO_VALID_TARGETS;

        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_BLACK_OX_STATUE);
        if (summons.empty())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        if (!target->IsWithinDist3d(summons.front(), 40.f))
            return SPELL_FAILED_OUT_OF_RANGE;

        if (!target->IsWithinLOSInMap(summons.front()))
            return SPELL_FAILED_LINE_OF_SIGHT;
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_BLACK_OX_STATUE);
        if (!summons.empty())
            summons.front()->CastSpell(GetHitUnit(), SPELL_MONK_LEER_OF_THE_OX, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_leer_of_the_ox::CheckCast);
        OnHit += SpellHitFn(spell_monk_leer_of_the_ox::HandleHit);
    }
};

// 117907 - Mastery: Gift of the Serpent
class spell_monk_mastery_gift_of_the_serpent : public AuraScript
{
    PrepareAuraScript(spell_monk_mastery_gift_of_the_serpent);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo())
            return false;
        if (eventInfo.GetSpellInfo()->Id == SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL ||
            eventInfo.GetSpellInfo()->Id == SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL_EXPIRED)
            return false;
        return eventInfo.GetHealInfo()->GetHeal() > 0;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        static std::map<uint32, float> const map =
        {
            { 115151, 0.15f }, // Renewing Mist
            { 132463, 0.25f }, // Chi Wave
            { 124081, 0.25f }, // Zen Sphere (Periodic)
            { 124101, 0.15f }, //  Zen Sphere (Detonate)
            { 115175, 0.3f  }, // Soothing Mist
            { 125953, 0.15f }, // Soothing Mist (Statue)
            { 132120, 0.2f  }, // Enveloping Mist
            { 116670, 0.25f }, // Uplift
            { 115310, 0.15f }, // Revival
            { 126890, 0.2f  }, // Eminence
            { 117895, 0.1f  }, // Eminence (Statue)
            { 117640, 0.1f  }, // Spinning Crane Kick
        };

        float chance = eff->GetFloatAmount();
        auto it = map.find(eventInfo.GetSpellInfo()->Id);
        if (it != map.end())
            chance *= it->second;
        if (roll_chance_f(chance))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_GIFT_OF_THE_SERPENT_SELECTOR, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_mastery_gift_of_the_serpent::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_monk_mastery_gift_of_the_serpent::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 119036 - Gift of the Serpent
class spell_monk_gift_of_the_serpent_selector : public SpellScript
{
    PrepareSpellScript(spell_monk_gift_of_the_serpent_selector);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.sort(Trinity::HealthPctOrderWorldObjectPred{});
        if (!targets.empty())
            targets.resize(1);
    }

    void HandleHit()
    {
        if (!GetHitUnit()->IsFullHealth())
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MONK_GIFT_OF_THE_SERPENT_SUMMON, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_gift_of_the_serpent_selector::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnHit += SpellHitFn(spell_monk_gift_of_the_serpent_selector::HandleHit);
    }
};

// 119031 - Gift of the Serpent
class spell_monk_gif_of_the_serpent_summon : public SpellScript
{
    PrepareSpellScript(spell_monk_gif_of_the_serpent_summon);

    void PlacementCorrection(SpellDestination& dest)
    {
        Unit* target = GetSpell()->m_targets.GetUnitTarget();
        if (target && target->IsWithinDist3d(&dest._position, 1.0f))
            target->MovePositionToFirstCollision(dest._position, target->GetObjectSize() + 1.1f, target->GetAngle(&dest._position) - target->GetOrientation());
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_monk_gif_of_the_serpent_summon::PlacementCorrection, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// 321
struct sat_monk_gift_of_the_serpent : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        Unit* unit = object->ToUnit();
        return unit && (!unit->IsFullHealth() || unit->HasAuraType(SPELL_AURA_SCHOOL_HEAL_ABSORB)) && unit->IsFriendlyTo(GetCaster());
    }

    void OnTrigger(WorldObject* target) override
    {
        GetCaster()->CastSpell(target->ToUnit(), SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL, true);
    }

    void OnExpire() override
    {
        GetCaster()->CastSpell(*GetTarget(), SPELL_MONK_GIFT_OF_THE_SERPENT_HEAL_EXPIRED, true);
    }
};

// 122278 - Dampen Harm
class spell_monk_dampen_harm : public AuraScript
{
    PrepareAuraScript(spell_monk_dampen_harm);

    void HandleAbsorb(AuraEffect* eff, DamageInfo& damageInfo, uint32&)
    {
        PreventDefaultAction();
        if (damageInfo.GetDamage() >= GetUnitOwner()->CountPctFromMaxHealth(20))
        {
            damageInfo.AbsorbDamage(damageInfo.GetDamage() / 2);
            DropCharge();
        }
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_monk_dampen_harm::HandleAbsorb, EFFECT_0);
    }
};

// 115546 - Provoke
class spell_monk_provoke : public SpellScript
{
    PrepareSpellScript(spell_monk_provoke);

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();
        if (!target)
            return SPELL_FAILED_NO_VALID_TARGETS;
        else if (target->GetTypeId() == TYPEID_PLAYER)
            return SPELL_FAILED_TARGET_IS_PLAYER;
        else if (!target->IsWithinLOSInMap(GetCaster()))
            return SPELL_FAILED_LINE_OF_SIGHT;
        else if (GetCaster()->IsFriendlyTo(target))
        {
            std::list<TempSummon*> summons;
            GetCaster()->GetSummons(summons, NPC_BLACK_OX_STATUE);
            if (summons.empty() || target != summons.front())
                return SPELL_FAILED_TARGET_FRIENDLY;
        }
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MONK_PROVOKE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_provoke::CheckCast);
        OnHit += SpellHitFn(spell_monk_provoke::HandleHit);
    }
};

// 118635 - Provoke
class spell_monk_provoke_effect : public SpellScript
{
    PrepareSpellScript(spell_monk_provoke_effect);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, NPC_BLACK_OX_STATUE);
        if (summons.empty() || GetExplTargetUnit() != summons.front())
            targets.remove_if([=](WorldObject const* target) { return target != GetExplTargetUnit(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_provoke_effect::SelectTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 116849 - Life Cocoon 
class spell_monk_life_cocoon : public SpellScript
{
    PrepareSpellScript(spell_monk_life_cocoon);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetUInt32Value(UNIT_FIELD_FLAGS) & UNIT_FLAG_STUNNED)
            if (!GetCaster()->HasAura(SPELL_MONK_GLYPH_OF_LIFE_COCOON))
                return SPELL_FAILED_STUNNED;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_life_cocoon::CheckCast);
    }
};

// 137562 - Nimble Brew
class spell_monk_nimble_brew : public SpellScript
{
    PrepareSpellScript(spell_monk_nimble_brew);

    void HandleHit()
    {
        if (!GetDispelCount() || !GetCaster()->HasAura(SPELL_MONK_GLYPH_OF_NIMBLE_BREW))
            PreventHitHeal();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_nimble_brew::HandleHit);
    }
};

// 115080 - Touch of Death
class spell_monk_touch_of_death : public SpellScript
{
    PrepareSpellScript(spell_monk_touch_of_death);

    SpellCastResult CheckCast()
    {
        Unit* target = GetExplTargetUnit();
        if (target)
        {
            if (target->GetAffectingPlayer())
            {
                if (!GetCaster()->HasAura(SPELL_MONK_WINDWALKER_PVP_P4))
                    return SPELL_FAILED_BAD_TARGETS;
                if (target->GetHealthPct() > 10.0f)
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
            {
                if (target->GetTypeId() != TYPEID_UNIT) // Impossible?...
                    return SPELL_FAILED_BAD_TARGETS;
                if (target->ToCreature()->IsDungeonBoss() || target->GetHealth() > GetCaster()->GetHealth())
                    return SPELL_FAILED_BAD_TARGETS;
            }
            return SPELL_CAST_OK;
        }
        return SPELL_FAILED_NO_VALID_TARGETS;
    }

    void HandleHit()
    {
        if (GetHitResult() != SPELL_MISS_IMMUNE) // Is redirect by warrior possible?
            SetHitDamage(GetHitUnit()->GetTypeId() == TYPEID_PLAYER ? GetHitUnit()->GetMaxHealth() : GetCaster()->GetMaxHealth());
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_monk_touch_of_death::CheckCast);
        OnHit += SpellHitFn(spell_monk_touch_of_death::HandleHit);
    }
};

// 122280 - Healing Elixirs
class spell_monk_healing_elixirs : public AuraScript
{
    PrepareAuraScript(spell_monk_healing_elixirs);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HealthBelowPct(35);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Aura* healingElixirs = GetUnitOwner()->GetAura(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell))
            GetUnitOwner()->CastSpell(GetUnitOwner(), healingElixirs->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_healing_elixirs::CheckProc);
        OnProc += AuraProcFn(spell_monk_healing_elixirs::HandleProc);
    }
};

// 134563 - Healing Elixirs (Consuming via brew)
class spell_monk_healing_elixirs_acitve : public AuraScript
{
    PrepareAuraScript(spell_monk_healing_elixirs_acitve);

    bool CheckProc(ProcEventInfo&)
    {
        return !GetUnitOwner()->IsFullHealth();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_healing_elixirs_acitve::CheckProc);
    }
};

// 122281 - Healing Elixirs
class spell_monk_healing_elixirs_heal : public SpellScript
{
    PrepareSpellScript(spell_monk_healing_elixirs_heal);

    void HandleCast()
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_MONK_HEALING_ELIXIRS))
            aura->Remove();
        if (AuraEffect* timer = GetCaster()->GetAuraEffect(SPELL_MONK_HEALING_ELIXIRS_TIMER, EFFECT_0))
            timer->ResetPeriodic(true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_monk_healing_elixirs_heal::HandleCast);
    }
};

// 109132 - Roll, 115008 - Chi Torpedo, 101545 - Flying Serpent Kick
class spell_monk_restless_pursuit : public SpellScript
{
    PrepareSpellScript(spell_monk_restless_pursuit);

    void HandleCast()
    {
        if  (GetCaster()->HasAura(SPELL_MONK_RESTLESS_PURSUIT))
            GetCaster()->RemoveAurasWithMechanic(1 << MECHANIC_SNARE);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_restless_pursuit::HandleCast);
    }
};

// 119611 - Renewing Mist 
class spell_monk_mana_tea_dispel : public AuraScript
{
    PrepareAuraScript(spell_monk_mana_tea_dispel);

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (Unit* monk = GetCaster())
        {
            if (monk->HasAura(SPELL_MONK_MANA_TEA_DISPEL))
            {
                monk->CastSpell(monk, SPELL_MONK_MANA_TEA_STACKS, true);
                monk->CastSpell(monk, SPELL_MONK_PLUS_ONE_MANA_TEA, true);
            }
        }
    }

    void Register() override
    {
        OnDispel += AuraDispelFn(spell_monk_mana_tea_dispel::HandleDispel);
    }
};

// 123904 - Invoke Xuen, the White Tiger
class spell_monk_invoke_xuen : public SpellScript
{
    PrepareSpellScript(spell_monk_invoke_xuen);

    void HandleHit()
    {
        Player* monk = GetCaster()->ToPlayer();
        if (!monk)
            return;

        for (auto&& summon : monk->GetSummons())
        {
            if (summon->GetEntry() == GetSpellInfo()->Effects[EFFECT_0].MiscValue)
            {
                summon->CastSpell(summon, 36747, true);     // Spawn Smoke (Druid)
                summon->AI()->AttackStart(GetExplTargetUnit());
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_monk_invoke_xuen::HandleHit);
    }
};

// 124007 - Tiger Leap
class spell_monk_xuen_tiger_leap : public SpellScript
{
    PrepareSpellScript(spell_monk_xuen_tiger_leap);

    SpellCastResult CheckAutoCast()
    {
        if (!GetExplTargetUnit())
            return SPELL_FAILED_DONT_REPORT;
        if (GetCaster()->GetDistance(GetExplTargetUnit()) <= MELEE_RANGE)
            return SPELL_FAILED_DONT_REPORT;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_monk_xuen_tiger_leap::CheckAutoCast);
    }
};

// 130793 - Provoke
class spell_monk_xuen_provoke : public SpellScript
{
    PrepareSpellScript(spell_monk_xuen_provoke);

    bool CanTaunt(Unit const* target)
    {
        if (!target->IsInCombat())
            return false;
        if (target->GetVictim() == GetCaster())
            return false;

        if (Creature const* mob = target->ToCreature())
        {
            if (mob->isWorldBoss() || mob->IsDungeonBoss())
                return false;
            if (Unit* owner = GetCaster()->GetOwner())
                if (owner->GetVictim() == mob)
                    return false;
        }
        return true;
    }

    SpellCastResult CheckAutoCast()
    {
        if (GetExplTargetUnit() && CanTaunt(GetExplTargetUnit()))
            return SPELL_CAST_OK;

        std::list<Unit*> units;
        GetCaster()->GetAttackableUnitListInRange(units, 30.0f);
        units.remove_if([=](Unit const* target) { return !CanTaunt(target); });
        if (units.empty())
            return SPELL_FAILED_DONT_REPORT;
        GetSpell()->m_targets.SetUnitTarget(units.front());
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckAutoCast += SpellCheckCastFn(spell_monk_xuen_provoke::CheckAutoCast);
    }
};

// 123999 - Crackling Tiger Lightning Driver
class spell_monk_crackling_tiger_lightning : public AuraScript
{
    PrepareAuraScript(spell_monk_crackling_tiger_lightning);

    void HandleTick(AuraEffect const*)
    {
        if (GetCaster()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED | UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING))
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_monk_crackling_tiger_lightning::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 63508
struct npc_monk_xuen : public PetAI
{
    npc_monk_xuen(Creature* c) : PetAI(c) { }

    void IsSummonedBy(Unit* summoner) override
    {
        Player* monk = summoner->ToPlayer();
        if (!monk || monk->GetSpecialization() != SPEC_MONK_BREWMASTER)
            return;

        for (uint8 i = 0; i < 4; ++i)
        {
            if (!me->GetCharmInfo()->GetCharmSpell(i)->GetAction())
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(130793))
                {
                    me->GetCharmInfo()->GetCharmSpell(i)->SetActionAndType(spellInfo->Id, ACT_DISABLED);
                    me->GetCharmInfo()->AddSpellToActionBar(spellInfo, ACT_DISABLED);
                }
                break;
            }
        }
    }

    void Reset() override
    {
        me->CastSpell(me, 123995, true); // Invoke Xuen, the White Tiger mod threat
        me->CastSpell(me, 123999, true); // Crackling Tiger Lightning Driver
        me->CastSpell(me, 130324, true); // Eminence
    }
};

// 116023 - Sparring
class spell_monk_sparring : public AuraScript
{
    PrepareAuraScript(spell_monk_sparring);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & DONE_HIT_PROC_FLAG_MASK)
        {
            return GetUnitOwner()->GetAura(SPELL_MONK_SPARRING);
        }
        else
        {
            if (!eventInfo.GetActor())
                return false;
            return eventInfo.GetActor()->isInFront(GetUnitOwner()) && !GetUnitOwner()->HasAura(SPELL_MONK_SPARRING_COOLDOWN_MARKER);
        }
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_SPARRING, true);
        if (eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_SPARRING_COOLDOWN_MARKER, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_sparring::CheckProc);
        OnProc += AuraProcFn(spell_monk_sparring::HandleProc);
    }
};

// 1315
class sat_monk_chi_burst : public IAreaTriggerAura
{
    void OnInit() override
    {
        AreaTrigger* at = GetAreaTrigger();
        float sin = std::sin(GetCaster()->GetOrientation());
        float cos = std::cos(GetCaster()->GetOrientation());

        Position last = at->GetPosition();
        float dist = 0.0f;

        std::vector<Position> path;
        for (uint32 i = 0; i < 16; ++i)
        {
            float x = at->GetPositionX() + cos * 2 * i;
            float y = at->GetPositionY() + sin * 2 * i;
            float z = at->GetPositionZ();

            at->UpdateAllowedPositionZ(x, y, z);
            if (i > 1 && !at->IsWithinLOS(x, y, z)) // Oh shit...
                break;

            path.push_back({ x, y, z });
            dist += path.back().GetExactDist(&last);
            last = path.back();
        }

        GetAreaTrigger()->InitSpline(path, dist / GetSpellInfo()->Speed * 1000.f);
    }

    bool CheckTriggering(WorldObject* target) override { return false; }
};

// 1316
class sat_monk_chi_burst_real : public sat_monk_chi_burst
{
    std::set<uint64> guids;

    bool CheckTriggering(WorldObject* target) override
    {
        return target->ToUnit() && guids.find(target->GetGUID()) == guids.end();
    }

    void OnTriggeringApply(WorldObject* target) override
    {
        Unit* unit = target->ToUnit();
        guids.insert(target->GetGUID());
        auto const* damageSpell = sSpellMgr->GetSpellInfo(SPELL_MONK_CHI_BURST_DAMAGE);
        if (GetCaster()->IsValidAttackTarget(unit, damageSpell, nullptr, true))
            GetCaster()->CastSpell(unit, SPELL_MONK_CHI_BURST_DAMAGE, true);
        else if (GetCaster()->IsValidAssistTarget(unit))
            GetCaster()->CastSpell(unit, SPELL_MONK_CHI_BURST_HEAL, true);
    }
};

// 138177 - Item - Monk T15 Windwalker 2P Bonus
class spell_monk_t15_windwalker_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t15_windwalker_2p_bonus);

    bool right = false;

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo())
            return false;
        for (auto&& itr : eventInfo.GetSpellInfo()->Effects)
            if (itr.Effect == SPELL_EFFECT_ENERGIZE && itr.MiscValue == POWER_CHI)
                return true;

        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        uint32 spell = right ? SPELL_MONK_ENERGY_SPHERE_SUMMON_RIGHT : SPELL_MONK_ENERGY_SPHERE_SUMMON_LEFT;
        GetUnitOwner()->CastSpell(GetUnitOwner(), spell, true);
        right = !right;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_t15_windwalker_2p_bonus::CheckProc);
        OnProc += AuraProcFn(spell_monk_t15_windwalker_2p_bonus::HandleProc);
    }
};

// 138310, 140674 - Energy Sphere
class spell_monk_summon_energy_sphere : public SpellScript
{
    PrepareSpellScript(spell_monk_summon_energy_sphere);

    void SelectDest(SpellDestination& dest)
    {
        float min = GetSpellInfo()->Id == SPELL_MONK_ENERGY_SPHERE_SUMMON_RIGHT ? 200.0f : 20.0f;
        float max = GetSpellInfo()->Id == SPELL_MONK_ENERGY_SPHERE_SUMMON_RIGHT ? 340.0f : 160.0f;
        min = min * M_PI / 180.0f;
        max = max * M_PI / 180.0f;

        float angle = frand(min, max);
        float dist = frand(GetCaster()->GetObjectSize() + 1.5f, GetCaster()->GetObjectSize() + 4.0f);

        Position pos = GetCaster()->GetFirstCollisionPosition(dist, angle);
        dest._position = WorldLocation(GetCaster()->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
    }

    void Register() override
    {
        auto target = m_scriptSpellId == SPELL_MONK_ENERGY_SPHERE_SUMMON_RIGHT ? TARGET_DEST_CASTER_RIGHT : TARGET_DEST_CASTER_LEFT;
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_monk_summon_energy_sphere::SelectDest, EFFECT_0, target);
    }
};

// 796
struct sat_monk_energy_sphere : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* target) override
    {
        return target == GetCaster();
    }

    void OnTrigger(WorldObject* target) override
    {
        target->ToUnit()->CastSpell(target->ToUnit(), SPELL_MONK_ENERGY_SPHERE, true);
    }
};

// 115308 - Elusive Brew
class spell_monk_t15_brewmaster_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t15_brewmaster_2p_bonus);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->HasAura(SPELL_MONK_T15_BREWMASTER_2P_BONUS, EFFECT_0))
        {
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_STAGGERING, true);
            if (Aura* aura = GetUnitOwner()->GetAura(SPELL_MONK_STAGGERING))
            {
                aura->SetMaxDuration(GetMaxDuration());
                aura->SetDuration(GetMaxDuration());
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_t15_brewmaster_2p_bonus::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DODGE_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 116768 - Combo Breaker: Blackout Kick
// 118864 - Combo Breaker: Tiger Palm
class spell_monk_t16_windwalker_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_windwalker_2p_bonus);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Aura const* bonus = GetUnitOwner()->GetAura(SPELL_MONK_T16_WINDWALKER_2P_BONUS))
        {
            SpellEffIndex index = GetId() == SPELL_MONK_COMBO_BREAKER_PALM ? EFFECT_0 : EFFECT_1;
            amount = bonus->GetSpellInfo()->Effects[index].CalcValue(GetUnitOwner());
        }
    }

    void Register() override
    {
        SpellEffIndex index = m_scriptSpellId == SPELL_MONK_COMBO_BREAKER_PALM ? EFFECT_1 : EFFECT_2;
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_t16_windwalker_2p_bonus::CalculateAmount, index, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// 145022 - Item - Monk T16 Windwalker 4P Bonus
class spell_monk_t16_windwalker_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_windwalker_4p_bonus);

    uint32 consumed = 0;

    void HandleProc(ProcEventInfo&)
    {
        if (Aura const* brew = GetUnitOwner()->GetAura(SPELL_MONK_TIGEREYE_BREW_STACKS))
        {
            consumed += std::min<uint32>(brew->GetStackAmount(), 10);
            if (consumed >= 10)
            {
                consumed -= 10;
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_FOCUS_OF_XUEN, true);
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_t16_windwalker_4p_bonus::HandleProc);
    }
};

// 120273 - Tiger Strikes
class spell_monk_tiger_strikes : public AuraScript
{
    PrepareAuraScript(spell_monk_tiger_strikes);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        uint32 spellId = eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK ? 120278 : 120274;
        GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), spellId, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_tiger_strikes::HandleProc);
    }
};

// 145049 - Item - Monk T16 Brewmaster 2P Bonus
class spell_monk_t16_brewmaster_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_brewmaster_2p_bonus);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* target = eventInfo.GetActionTarget();
        if (!target)
            return;

        Unit* monk = GetUnitOwner();

        uint32 auraId = monk->HasAura(SPELL_MONK_GLYPH_OF_GUARD) ?
            SPELL_MONK_BLACK_OX_STATUE_GUARD_MAGIC : SPELL_MONK_BLACK_OX_STATUE_GUARD;
        AuraEffect* shield = target->GetAuraEffect(auraId, EFFECT_0, monk->GetGUID());
        if (!shield)
            return;
        int32 absorb = CalculatePct(shield->GetAmount(), effect->GetAmount());
        GetUnitOwner()->CastCustomSpell(SPELL_MONK_PROTECTION_OF_NIUZAO, SPELLVALUE_BASE_POINT0, absorb, monk, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_monk_t16_brewmaster_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 145055 - Item - Monk T16 Brewmaster 4P Bonus
class spell_monk_t16_brewmaster_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_brewmaster_4p_bonus);

    void HandleProc(AuraEffect const* effect, ProcEventInfo&)
    {
        if (AuraEffect const* stagger = GetUnitOwner()->GetAuraEffect(SPELL_MONK_STAGGER, EFFECT_1))
        {
            int32 heal = CalculatePct(stagger->GetAmount(), effect->GetAmount());
            GetUnitOwner()->CastCustomSpell(SPELL_MONK_PURIFIED_HEALING, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_monk_t16_brewmaster_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 145439 - Item - Monk T16 Mistweaver 2P Bonus
class spell_monk_t16_mistweaver_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_mistweaver_2p_bonus);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        // Yes, because of DEST and zero unit targets it can proc without the target
        if (Unit* target = eventInfo.GetActionTarget())
            if (int32 absorb = CalculatePct(eventInfo.GetHeal(), effect->GetAmount()))
                GetUnitOwner()->CastCustomSpell(SPELL_MONK_YULONS_BARRIER, SPELLVALUE_BASE_POINT0, absorb, target, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_monk_t16_mistweaver_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 145449 - Item - Monk T16 Mistweaver 4P Bonus
class spell_monk_t16_mistweaver_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_monk_t16_mistweaver_4p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* monk = GetUnitOwner();
        // Why target ally?...
        std::list<Unit*> units;
        Trinity::AnyFriendlyUnitInObjectRangeCheck check{ monk, monk, 50.0f};
        Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher{ monk, units, check };
        monk->VisitNearbyObject(50.f, searcher);
        units.remove_if([](Unit const* target) { return target->IsSummon() && target->ToTempSummon()->IsMinorGuardian(); });

        if (units.empty())
            return;

        units.sort(Trinity::HealthPctOrderPred{});
        Unit* target = units.front();
        if (target->GetHealth() == target->GetMaxHealth()) // Simulate random
            target = Trinity::Containers::SelectRandomContainerElement(units);

        monk->CastSpell(target, SPELL_MONK_MIST_WAVE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_t16_mistweaver_4p_bonus::HandleProc);
    }
};

// 131523 - Zen Meditation
class spell_monk_zen_meditation : public AuraScript
{
    PrepareAuraScript(spell_monk_zen_meditation);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & (PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS))
            Remove();
        // !eventInfo.TriggeredBySpell() if split damage
        else if (!eventInfo.TriggeredBySpell() || !eventInfo.TriggeredBySpell()->IsSpellRedirecterd())
            PreventDefaultAction();
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_zen_meditation::HandleProc);
    }
};

// 127361 - Bear Hug
class spell_monk_bear_hug : public AuraScript
{
    PrepareAuraScript(spell_monk_bear_hug);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (Unit* caster = GetCaster())
            amount = caster->CountPctFromMaxHealth(amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_bear_hug::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 146951 - Glyph of Rapid Rolling
class spell_monk_glyph_of_rapid_rolling : public AuraScript
{
    PrepareAuraScript(spell_monk_glyph_of_rapid_rolling);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !GetUnitOwner()->HasAura(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_glyph_of_rapid_rolling::CheckProc);
    }
};

// 116092 - Afterlife
class spell_monk_afterlife : public AuraScript
{
    PrepareAuraScript(spell_monk_afterlife);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_MONK_BLACKOUT_KICK)
        {
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_MONK_CHI_SPHERE, true);
        }
        else
        {
            Position dest = GetUnitOwner()->GetPosition();
            float angle = rand_norm() * 2 * M_PI;
            GetUnitOwner()->MovePositionToFirstCollision(dest, 4.0f + GetUnitOwner()->GetObjectSize(), angle);
            GetUnitOwner()->CastSpell(dest, SPELL_MONK_AFTERLIFE_SUMMON, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_afterlife::HandleProc);
    }
};

// 123393 - Breath of Fire
class spell_monk_glyph_of_breath_of_fire_fix : public AuraScript
{
    PrepareAuraScript(spell_monk_glyph_of_breath_of_fire_fix);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_MONK_BREATH_OF_FIRE_DOT)
            return false;
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_monk_glyph_of_breath_of_fire_fix::CheckProc);
    }
};

// 126060 - Desperate Measures
class spell_monk_desperate_measures : public AuraScript
{
    PrepareAuraScript(spell_monk_desperate_measures);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        for (auto&& spell : { 115072, 147489 }) // Expel Harm and glyphed version
            GetCaster()->ToPlayer()->RemoveSpellCooldown(spell, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_monk_desperate_measures::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 115213 - Avert Harm
class spell_monk_avert_harm : public AuraScript
{
    PrepareAuraScript(spell_monk_avert_harm);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (GetUnitOwner()->GetHealthPct() <= 10)
            Remove();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_avert_harm::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
        OnProc += AuraProcFn(spell_monk_avert_harm::HandleProc);
    }
};

// 108562 - Monk Weapon Override Driver
class spell_monk_weapon_override_driver : public AuraScript
{
    PrepareAuraScript(spell_monk_weapon_override_driver);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void CalculateAmount(AuraEffect const*, float&, bool&)
    {
        Player* monk = GetUnitOwner()->ToPlayer();

        monk->RemoveAurasDueToSpell(SPELL_MONK_2H_POLEARM_OVERRIDE);
        monk->RemoveAurasDueToSpell(SPELL_MONK_2H_STAFF_OVERRIDE);
        monk->RemoveAurasDueToSpell(SPELL_MONK_DW_AXES);
        monk->RemoveAurasDueToSpell(SPELL_MONK_DW_MACES);
        monk->RemoveAurasDueToSpell(SPELL_MONK_DW_SWORDS);

        if (monk->HasAura(SPELL_MONK_GLYPH_OF_JAB))
            return;

        if (Item* weapon = GetUnitOwner()->ToPlayer()->GetWeaponForAttack(BASE_ATTACK))
        {
            uint32 spell = 0;
            switch (weapon->GetTemplate()->SubClass)
            {
                case ITEM_SUBCLASS_WEAPON_POLEARM:  spell = SPELL_MONK_2H_POLEARM_OVERRIDE;  break;
                case ITEM_SUBCLASS_WEAPON_STAFF:    spell = SPELL_MONK_2H_STAFF_OVERRIDE; break;
                case ITEM_SUBCLASS_WEAPON_AXE:      spell = SPELL_MONK_DW_AXES; break;
                case ITEM_SUBCLASS_WEAPON_MACE:     spell = SPELL_MONK_DW_MACES; break;
                case ITEM_SUBCLASS_WEAPON_SWORD:    spell = SPELL_MONK_DW_SWORDS; break;
                default:
                    break;
            }

            if (spell)
                monk->CastSpell(monk, spell, true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_monk_weapon_override_driver::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 125660 - Glyph of Jab
class spell_monk_glyph_of_jab : public AuraScript
{
    PrepareAuraScript(spell_monk_glyph_of_jab);

    void HandleGlyph(AuraEffect const*, AuraEffectHandleModes)
    {
        if (AuraEffect* driver = GetUnitOwner()->GetAuraEffect(SPELL_MONK_WEAPON_OVERRIDE_DRIVER, EFFECT_0))
            driver->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn(spell_monk_glyph_of_jab::HandleGlyph, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_monk_glyph_of_jab::HandleGlyph, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 115080 - Touch of Death
class spell_monk_touch_of_death_expel_harm_notification_driver : public AuraScript
{
    PrepareAuraScript(spell_monk_touch_of_death_expel_harm_notification_driver);

    SpellInfo const* touchOfDeath = nullptr;

    bool Load() override
    {
        touchOfDeath = sSpellMgr->GetSpellInfo(SPELL_MONK_TOUCH_OF_DEATH);
        return touchOfDeath != nullptr && GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* target = eventInfo.GetActionTarget())
        {
            if (target->GetGUID() != GetUnitOwner()->GetTarget())
                return;

            if (target->GetAffectingPlayer())
                return;

            if (GetOwner()->ToPlayer()->HasSpellCooldown(SPELL_MONK_TOUCH_OF_DEATH))
                return;

            if (GetUnitOwner()->IsValidAttackTarget(target, touchOfDeath))
            {
                Unit* monk = GetUnitOwner();
                if (target->GetHealth() > monk->GetHealth())
                    monk->RemoveAurasDueToSpell(SPELL_MONK_DEATH_NOTE);
                else
                    monk->CastSpell(monk, SPELL_MONK_DEATH_NOTE, true);
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_monk_touch_of_death_expel_harm_notification_driver::HandleProc);
    }
};

// 115057 - Flying Serpent Kick
class spell_monk_flying_serpent_kick : public AuraScript
{
    PrepareAuraScript(spell_monk_flying_serpent_kick);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTarget()->HasAura(SPELL_MONK_FLYING_SERPENT_KICK))
            GetTarget()->RemoveAura(SPELL_MONK_FLYING_SERPENT_KICK);

        if (GetTarget()->HasAura(SPELL_MONK_ITEM_PVP_GLOVES_BONUS))
            GetTarget()->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_MONK_FLYING_SERPENT_KICK_AOE, true);
    }

    void Register() override
    {
        AfterEffectApply  += AuraEffectApplyFn (spell_monk_flying_serpent_kick::HandleApply,  EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_monk_flying_serpent_kick::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 147489 - Expel Harm
class spell_monk_glyph_of_targeted_expulsion : public spell_monk_expel_harm
{
    PrepareSpellScript(spell_monk_glyph_of_targeted_expulsion);

    void HandleHeal()
    {
        if (GetHitUnit() != GetCaster())
            SetHitHeal(GetHitHeal() / 2);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_monk_glyph_of_targeted_expulsion::HandleHeal);
        AfterHit += SpellHitFn(spell_monk_glyph_of_targeted_expulsion::HandleHit);
        OnEffectLaunchTarget += SpellEffectFn(spell_monk_glyph_of_targeted_expulsion::CalculateDamage, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 115310 - Revival
class spell_monk_revival : public SpellScript
{
    PrepareSpellScript(spell_monk_revival);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target)
        {
            return target->ToUnit()->IsSummon() && target->ToUnit()->ToTempSummon()->IsMinorGuardian();
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_revival::SelectTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

// 100780 - Jab
class spell_monk_jab : public SpellScript
{
    PrepareSpellScript(spell_monk_jab);

    void HandleCast()
    {
        uint32 visualId = urand(0, 1) ? 22600 : 22468;
        GetCaster()->SendPlaySpellVisual(visualId, GetCaster()->GetGUID(), 0);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_jab::HandleCast);
    }
};

// 100780, 108557, 115687, 115693, 115695, 115698 - Jab
class spell_monk_muscle_memory : public SpellScript
{
    PrepareSpellScript(spell_monk_muscle_memory);

    void HandleCast()
    {
        if (GetCaster()->HasSpell(SPELL_MONK_MUSCLE_MEMORY))
            GetCaster()->CastSpell(GetCaster(), SPELL_MONK_MUSCLE_MEMORY_BUFF, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_monk_muscle_memory::HandleCast);
    }
};

// 116330 - Dizzying Haze
class spell_monk_dizzying_haze : public SpellScript
{
    PrepareSpellScript(spell_monk_dizzying_haze);

    void CalculateThreat(SpellEffIndex)
    {
        SetEffectValue(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_monk_dizzying_haze::CalculateThreat, EFFECT_2, SPELL_EFFECT_THREAT);
    }
};

// 119392 - Charging Ox Wave
class spell_monk_charging_ox_wave : public SpellScript
{
    PrepareSpellScript(spell_monk_charging_ox_wave);

    std::list<WorldObject*> selected;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject const* target)
        {
            return !GetCaster()->HasInLine(target, 4.0f);
        });
        selected = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = selected;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_charging_ox_wave::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_129);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_monk_charging_ox_wave::CopyTargets,   EFFECT_1, TARGET_UNIT_CONE_ENEMY_129);
    }
};

// 125084 - Charging Ox Wave
class spell_monk_charging_ox_wave_visual : public SpellScript
{
    PrepareSpellScript(spell_monk_charging_ox_wave_visual);

    void SelectDest(SpellDestination& dest)
    {
        GetCaster()->MovePositionToFirstCollosionBySteps(dest._position, 30.0f, 0, 2.0f, true);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_monk_charging_ox_wave_visual::SelectDest, EFFECT_0, TARGET_DEST_CASTER_FRONT);
    }
};

// 383 summoned by Afterlife (117032)
class sat_monk_afterlife_healing_shpere : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* target) override
    {
        return target == GetCaster();
    }

    void OnTrigger(WorldObject* target) override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_AFTERLIFE_HEALING_SPHERE, true);
    }
};

// 388
class sat_monk_glyph_of_flying_serpent_kick : public IAreaTriggerOnce
{
    void OnInit() override
    {
        float sin = std::sin(GetCaster()->GetOrientation());
        float cos = std::cos(GetCaster()->GetOrientation());

        float speed = GetCaster()->GetSpeed(MOVE_RUN);

        Position last = GetCaster()->GetPosition();
        float dist = 0.0f;

        std::vector<Position> path;
        for (uint32 i = 0; i < 50; ++i)
        {
            float x = GetCaster()->GetPositionX() + cos * 2 * i;
            float y = GetCaster()->GetPositionY() + sin * 2 * i;
            float z = GetCaster()->GetPositionZ();

            GetCaster()->UpdateAllowedPositionZ(x, y, z);
            if (i > 1 && !GetCaster()->IsWithinLOS(x, y, z)) // Oh shit...
                break;

            path.push_back({ x, y, z });
            dist += path.back().GetExactDist(&last);
            last = path.back();
        }

        GetAreaTrigger()->InitSpline(path, (dist / speed) * 1000);
    }

    bool CheckTriggering(WorldObject* target) override
    {
        int32 current = GetAreaTrigger()->GetCurrentSplineIndex();
        auto& path = GetAreaTrigger()->GetPath();

        if (Unit* unit = target->ToUnit())
        {
            if (GetCaster()->IsValidAttackTarget(unit, sSpellMgr->GetSpellInfo(SPELL_MONK_FLYING_SERPENT_KICK_AOE)))
            {
                if (unit->IsInDist(path.front().x, path.front().y, path.front().z, 2 + unit->GetObjectSize()) && 
                    !GetCaster()->HasInArc(M_PI, unit) && !GetAreaTrigger()->HasInArc(M_PI, unit))
                    return false;

                // Okay straight forward check, include only target's combat reach
                float dist = 3.0f;
                if (unit->GetObjectSize() + dist < MELEE_RANGE)
                    dist += unit->GetObjectSize();
                if (unit->IsInDist(GetCaster(), dist))
                    return true;

                // Now, player's position can be not updated at this time, try to predict next position
                if (current + 1 < int32(path.size()))
                {
                    if (unit->IsInDist(path[current].x, path[current].y, path[current].z, 3.0f) &&
                        unit->IsInDist(path[current + 1].x, path[current + 1].y, path[current + 1].z, 2))
                        return true;
                }
            }
        }
        return false;
    }

    void OnTrigger(WorldObject* target) override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MONK_FLYING_SERPENT_KICK_TRIGGER, true);
    }
};

void AddSC_monk_spell_scripts()
{
    new spell_script<spell_monk_chi_wave>("spell_monk_chi_wave");
    new spell_script<spell_monk_chi_wave_bolt>("spell_monk_chi_wave_bolt");
    new spell_script<spell_monk_chi_wave_target_selector>("spell_monk_chi_wave_target_selector");
    new spell_monk_grapple_weapon();
    new spell_script<spell_monk_transcendence_transfer>("spell_monk_transcendence_transfer");
    new spell_script<spell_monk_transcendence>("spell_monk_transcendence");
    new spell_monk_item_s12_4p_mistweaver();
    new spell_script<spell_monk_diffuse_magic>("spell_monk_diffuse_magic");
    new spell_script<spell_monk_zen_flight_check>("spell_monk_zen_flight_check");
    new aura_script<aura_monk_glyph_of_zen_flight>("aura_monk_glyph_of_zen_flight");
    new aura_script<spell_monk_touch_of_karma>("spell_monk_touch_of_karma");
    new spell_script<spell_monk_touch_of_karma_target>("spell_monk_touch_of_karma_target");
    new spell_monk_path_of_blossom();
    new aura_script<spell_monk_zen_sphere>("spell_monk_zen_sphere");
    new spell_monk_energizing_brew();
    new spell_monk_spear_hand_strike();
    new spell_monk_tigers_lust();
    new spell_script<spell_monk_chi_torpedo>("spell_monk_chi_torpedo");
    new atrigger_script<sat_monk_chi_torpedo>("sat_monk_chi_torpedo");
    new spell_monk_breath_of_fire();
    new aura_script<spell_monk_disable_refresh>("spell_monk_disable_refresh");
    new spell_script<spell_monk_disable>("spell_monk_disable");
    new spell_script<spell_monk_zen_pilgrimage>("spell_monk_zen_pilgrimage");
    new spell_script<spell_monk_zen_pilgrimage_return>("spell_monk_zen_pilgrimage_return");
    new spell_script<spell_monk_blackout_kick>("spell_monk_blackout_kick");
    new spell_script<spell_monk_legacy_of_the_emperor>("spell_monk_legacy_of_the_emperor");
    new spell_script<spell_monk_paralysis>("spell_monk_paralysis");
    new aura_script<spell_monk_paralysis_break>("spell_monk_paralysis_break");
    new spell_script<spell_monk_chi_brew>("spell_monk_chi_brew");
    new atrigger_script<sat_monk_ring_of_peace>("sat_monk_ring_of_peace");
    new aura_script<spell_monk_ring_of_peace_aura>("spell_monk_ring_of_peace_aura");
    new aura_script<spell_monk_s_e_f_aura>("spell_monk_s_e_f_aura");
    new spell_script<spell_monk_s_e_f>("spell_monk_s_e_f");
    new creature_script<npc_monk_s_e_f_spirit>("npc_monk_s_e_f_spirit");
    new spell_script<spell_monk_detox>("spell_monk_detox");
    new spell_script<spell_monk_specific_damage>("spell_monk_specific_damage");
    new spell_script<spell_monk_keg_smash>("spell_monk_keg_smash");
    new spell_script<spell_monk_rising_sun_kick>("spell_monk_rising_sun_kick");
    new aura_script<spell_monk_combo_breaker>("spell_monk_combo_breaker");
    new aura_script<spell_stance_of_the_sturdy_ox>("spell_stance_of_the_sturdy_ox");
    new aura_script<spell_monk_stagger>("spell_monk_stagger");
    new aura_script<spell_monk_stagger_dummy>("spell_monk_stagger_dummy");
    new spell_script<spell_monk_purifying_brew>("spell_monk_purifying_brew");
    new spell_script<spell_monk_expel_harm>("spell_monk_expel_harm");
    new spell_script<spell_monk_healing_sphere>("spell_monk_healing_sphere");
    new atrigger_script<sat_monk_healing_sphere>("sat_monk_healing_sphere");
    new aura_script<spell_monk_gift_of_the_ox>("spell_monk_gift_of_the_ox");
    new spell_script<spell_monk_summon_gift_of_the_ox>("spell_monk_summon_gift_of_the_ox");
    new atrigger_script<sat_monk_gift_of_the_ox>("sat_monk_gift_of_the_ox");
    new aura_script<spell_monk_fortifying_brew>("spell_monk_fortifying_brew");
    new aura_script<spell_monk_brewing_elusive_brew>("spell_monk_brewing_elusive_brew");
    new aura_script<spell_monk_elusive_brew>("spell_monk_elusive_brew");
    new aura_script<spell_monk_fists_of_fury>("spell_monk_fists_of_fury");
    new spell_script<spell_monk_fists_of_fury_damage>("spell_monk_fists_of_fury_damage");
    new spell_script<spell_monk_fists_of_fury_stun>("spell_monk_fists_of_fury_stun");
    new aura_script<spell_monk_adaptation>("spell_monk_adaptation");
    new aura_script<spell_monk_crackling_jade_lightning_knockback>("spell_monk_crackling_jade_lightning_knockback");
    new spell_script<spell_monk_spinning_fire_blossom>("spell_monk_spinning_fire_blossom");
    new spell_script<spell_monk_spinning_fire_blossom_glyphed>("spell_monk_spinning_fire_blossom_glyphed");
    new aura_script<spell_monk_brewing_tigereye_brew>("spell_monk_brewing_tigereye_brew");
    new spell_script<spell_monk_tigereye_brew>("spell_monk_tigereye_brew");
    new aura_script<spell_monk_tigereye_brew_stacks>("spell_monk_tigereye_brew_stacks");
    new aura_script<spell_monk_brewing_mana_tea>("spell_monk_brewing_mana_tea");
    new aura_script<spell_monk_mana_tea>("spell_monk_mana_tea");
    new spell_script<spell_monk_mana_tea_glyphed>("spell_monk_mana_tea_glyphed");
    new spell_script<spell_monk_power_strikes>("spell_monk_power_strikes");
    new atrigger_script<sat_monk_chi_sphere>("sat_monk_chi_sphere");
    new spell_script<spell_monk_clash>("spell_monk_clash");
    new spell_script<spell_monk_clash_caster_charge>("spell_monk_clash_caster_charge");
    new creature_script<npc_black_ox_statue>("npc_black_ox_statue");
    new aura_script<spell_monk_eminence_driver>("spell_monk_eminence_driver");
    new aura_script<spell_monk_stance_of_wise_serpent>("spell_monk_stance_of_wise_serpent");
    new aura_script<spell_monk_soothing_mist>("spell_monk_soothing_mist");
    new aura_script<spell_monk_soothing_mist_tirgger>("spell_monk_soothing_mist_tirgger");
    new aura_script<spell_monk_soothing_mist_statue>("spell_monk_soothing_mist_statue");
    new spell_script<spell_monk_soothing_mist_statue_trigger>("spell_monk_soothing_mist_statue_trigger");
    new aura_script<spell_monk_renewing_mist>("spell_monk_renewing_mist");
    new spell_script<spell_monk_renewing_mist_jump>("spell_monk_renewing_mist_jump");
    new aura_script<spell_monk_spinning_crane_kick>("spell_monk_spinning_crane_kick");
    new spell_script<spell_monk_spinning_crane_kick_energize_tirgger>("spell_monk_spinning_crane_kick_energize_tirgger");
    new spell_script<spell_monk_spinning_crane_kick_damage>("spell_monk_spinning_crane_kick_damage");
    new spell_script<spell_monk_spinning_crane_kick_heal>("spell_monk_spinning_crane_kick_heal");
    new spell_script<spell_monk_surging_mist>("spell_monk_surging_mist");
    new spell_script<spell_monk_surging_mist_glyphed>("spell_monk_surging_mist_glyphed");
    new spell_script<spell_monk_enveloping_mist>("spell_monk_enveloping_mist");
    new spell_script<spell_monk_thunder_focus_tea_uplift>("spell_monk_thunder_focus_tea_uplift");
    new spell_script<spell_monk_thunder_focus_tea>("spell_monk_thunder_focus_tea");
    new aura_script<spell_monk_dematerialize>("spell_monk_dematerialize");
    new aura_script<spell_monk_glyph_of_fortuitous_spheres>("spell_monk_glyph_of_fortuitous_spheres");
    new aura_script<spell_monk_sanctaury_of_the_ox>("spell_monk_sanctaury_of_the_ox");
    new aura_script<spell_monk_guard>("spell_monk_guard");
    new spell_script<spell_monk_black_ox_statue_guard_selector>("spell_monk_black_ox_statue_guard_selector");
    new spell_script<spell_monk_roll>("spell_monk_roll");
    new aura_script<spell_monk_roll_aura>("spell_monk_roll_aura");
    new spell_script<spell_monk_leer_of_the_ox>("spell_monk_leer_of_the_ox");
    new aura_script<spell_monk_mastery_gift_of_the_serpent>("spell_monk_mastery_gift_of_the_serpent");
    new spell_script<spell_monk_gift_of_the_serpent_selector>("spell_monk_gift_of_the_serpent_selector");
    new spell_script<spell_monk_gif_of_the_serpent_summon>("spell_monk_gif_of_the_serpent_summon");
    new atrigger_script<sat_monk_gift_of_the_serpent>("sat_monk_gift_of_the_serpent");
    new aura_script<spell_monk_dampen_harm>("spell_monk_dampen_harm");
    new spell_script<spell_monk_provoke>("spell_monk_provoke");
    new spell_script<spell_monk_provoke_effect>("spell_monk_provoke_effect");
    new spell_script<spell_monk_life_cocoon>("spell_monk_life_cocoon");
    new spell_script<spell_monk_nimble_brew>("spell_monk_nimble_brew");
    new spell_script<spell_monk_touch_of_death>("spell_monk_touch_of_death");
    new aura_script<spell_monk_healing_elixirs>("spell_monk_healing_elixirs");
    new aura_script<spell_monk_healing_elixirs_acitve>("spell_monk_healing_elixirs_acitve");
    new spell_script<spell_monk_healing_elixirs_heal>("spell_monk_healing_elixirs_heal");
    new spell_script<spell_monk_restless_pursuit>("spell_monk_restless_pursuit");
    new aura_script<spell_monk_mana_tea_dispel>("spell_monk_mana_tea_dispel");
    new spell_script<spell_monk_invoke_xuen>("spell_monk_invoke_xuen");
    new spell_script<spell_monk_xuen_tiger_leap>("spell_monk_xuen_tiger_leap");
    new spell_script<spell_monk_xuen_provoke>("spell_monk_xuen_provoke");
    new aura_script<spell_monk_crackling_tiger_lightning>("spell_monk_crackling_tiger_lightning");
    new pet_script<npc_monk_xuen>("npc_monk_xuen");
    new aura_script<spell_monk_sparring>("spell_monk_sparring");
    new atrigger_script<sat_monk_chi_burst>("sat_monk_chi_burst");
    new atrigger_script<sat_monk_chi_burst_real>("sat_monk_chi_burst_real");
    new aura_script<spell_monk_t15_windwalker_2p_bonus>("spell_monk_t15_windwalker_2p_bonus");
    new spell_script<spell_monk_summon_energy_sphere>("spell_monk_summon_energy_sphere");
    new atrigger_script<sat_monk_energy_sphere>("sat_monk_energy_sphere");
    new aura_script<spell_monk_t15_brewmaster_2p_bonus>("spell_monk_t15_brewmaster_2p_bonus");
    new aura_script<spell_monk_t16_windwalker_2p_bonus>("spell_monk_t16_windwalker_2p_bonus");
    new aura_script<spell_monk_t16_windwalker_4p_bonus>("spell_monk_t16_windwalker_4p_bonus");
    new aura_script<spell_monk_t16_brewmaster_2p_bonus>("spell_monk_t16_brewmaster_2p_bonus");
    new aura_script<spell_monk_t16_brewmaster_4p_bonus>("spell_monk_t16_brewmaster_4p_bonus");
    new aura_script<spell_monk_t16_mistweaver_2p_bonus>("spell_monk_t16_mistweaver_2p_bonus");
    new aura_script<spell_monk_t16_mistweaver_4p_bonus>("spell_monk_t16_mistweaver_4p_bonus");
    new aura_script<spell_monk_tiger_strikes>("spell_monk_tiger_strikes");
    new aura_script<spell_monk_zen_meditation>("spell_monk_zen_meditation");
    new aura_script<spell_monk_bear_hug>("spell_monk_bear_hug");
    new aura_script<spell_monk_glyph_of_rapid_rolling>("spell_monk_glyph_of_rapid_rolling");
    new aura_script<spell_monk_afterlife>("spell_monk_afterlife");
    new aura_script<spell_monk_glyph_of_breath_of_fire_fix>("spell_monk_glyph_of_breath_of_fire_fix");
    new aura_script<spell_monk_desperate_measures>("spell_monk_desperate_measures");
    new aura_script<spell_monk_avert_harm>("spell_monk_avert_harm");
    new aura_script<spell_monk_weapon_override_driver>("spell_monk_weapon_override_driver");
    new aura_script<spell_monk_glyph_of_jab>("spell_monk_glyph_of_jab");
    new aura_script<spell_monk_touch_of_death_expel_harm_notification_driver>("spell_monk_touch_of_death_expel_harm_notification_driver");
    new aura_script<spell_monk_flying_serpent_kick>("spell_monk_flying_serpent_kick");
    new spell_script<spell_monk_glyph_of_targeted_expulsion>("spell_monk_glyph_of_targeted_expulsion");
    new spell_script<spell_monk_revival>("spell_monk_revival");
    new spell_script<spell_monk_jab>("spell_monk_jab");
    new spell_script<spell_monk_muscle_memory>("spell_monk_muscle_memory");
    new spell_script<spell_monk_dizzying_haze>("spell_monk_dizzying_haze");
    new spell_script<spell_monk_charging_ox_wave>("spell_monk_charging_ox_wave");
    new spell_script<spell_monk_charging_ox_wave_visual>("spell_monk_charging_ox_wave_visual");
    new atrigger_script<sat_monk_afterlife_healing_shpere>("sat_monk_afterlife_healing_shpere");
    new atrigger_script<sat_monk_glyph_of_flying_serpent_kick>("sat_monk_glyph_of_flying_serpent_kick");
}
