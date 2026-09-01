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
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "PassiveAI.h"
#include "Battleground.h"
#include "Group.h"
#include "BattlePetMgr.h"
#include "spell_common.h"
#include "Random.h"

enum PriestSpells
{
    PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL               = 48153,
    SPELL_PRIEST_PENANCE                            = 47540,
    SPELL_PRIEST_PENANCE_DAMAGE_TRIGGER             = 47758,
    SPELL_PRIEST_PENANCE_HEAL_TRIGGER               = 47757,
    SPELL_PRIEST_PENANCE_DAMAGE                     = 47666,
    SPELL_PRIEST_PENANCE_HEAL                       = 47750,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH    = 107903,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH       = 107904,
    PRIEST_GLYPH_OF_SHADOW                          = 107906,
    PRIEST_VOID_SHIFT                               = 108968,
    PRIEST_LEAP_OF_FAITH                            = 73325,
    PRIEST_LEAP_OF_FAITH_JUMP                       = 110726,
    PRIEST_INNER_WILL                               = 73413,
    PRIEST_INNER_FIRE                               = 588,
    PRIEST_NPC_SHADOWY_APPARITION                   = 46954,
    PRIEST_SPELL_HALO_EFFECT_SHADOW                 = 120696,
    PRIEST_SPELL_HALO_EFFECT_HOLY                   = 120692,

    // Cascade
    SPELL_PRIEST_CASCADE_HOLY                       = 121135,
    SPELL_PRIEST_CASCADE_HOLY_DAMAGE                = 120785,
    SPELL_PRIEST_CASCADE_HOLY_SELECTOR              = 120786,
    SPELL_PRIEST_CASCADE_HOLY_COOLDOWN              = 120840,
    SPELL_PRIEST_CASCADE_HOLY_MISSILE               = 121146,
    SPELL_PRIEST_CASCADE_HOLY_HEAL                  = 121148,
    SPELL_PRIEST_CASCADE_SHADOW_MISSILE             = 127627,
    SPELL_PRIEST_CASCADE_SHADOW_DAMAGE              = 127628,
    SPELL_PRIEST_CASCADE_SHADOW_HEAL                = 127629,
    SPELL_PRIEST_CASCADE_SHADOW_SELECTOR            = 127630,
    SPELL_PRIEST_CASCADE_SHADOW_COOLDOWN            = 127631,

    SPELL_PRIEST_SHADOWFORM_STANCE                  = 15473,
    SPELL_PRIEST_SHADOW_WORD_PAIN                   = 589,
    SPELL_PRIEST_SHADOW_WORD_PAIN_SR                = 124464,
    SPELL_PRIEST_DEVOURING_PLAGUE                   = 2944,
    SPELL_PRIEST_DEVOURING_PLAGUE_SR                = 124467,
    SPELL_PRIEST_MIND_SEAR_SR                       = 124469,
    SPELL_PRIEST_DEVOURING_PLAGUE_HEAL              = 127626,
    SPELL_PRIEST_VAMPIRIC_TOUCH                     = 34914,
    SPELL_PRIEST_VAMPIRIC_TOUCH_SR                  = 124465,
    PRIEST_PHANTASM_AURA                            = 108942,
    PRIEST_PHANTASM_PROC                            = 114239,
    SPELL_PRIEST_SPIRIT_SHELL                       = 109964,
    SPELL_PRIEST_SPIRIT_SHELL_ABSORPTION            = 114908,
    PRIEST_ATONEMENT_AURA                           = 81749,
    SPELL_PRIEST_ATONEMENT_HEAL                     = 81751,
    SPELL_PRIEST_ATONEMENT_HEAL_CRIT                = 94472,
    SPELL_PRIEST_SOLACE_HEAL                        = 140815,
    SPELL_PRIEST_SOLACE_HEAL_CRIT                   = 140816,
    SPELL_PRIEST_SOLACE_ENERGIZE                    = 129253,
    PRIEST_INNER_FOCUS                              = 89485,
    PRIEST_STRENGTH_OF_SOUL_AURA                    = 89488,
    PRIEST_STRENGTH_OF_SOUL_REDUCE_TIME             = 89490,
    PRIEST_WEAKENED_SOUL                            = 6788,
    PRIEST_STRENGTH_OF_SOUL                         = 89488,
    SPELL_PRIEST_EVANGELISM                         = 81662,
    SPELL_PRIEST_EVANGELISM_STACKS                  = 81661,
    SPELL_PRIEST_ARCHANGEL                          = 81700,
    LIGHTWELL_CHARGES                               = 59907,
    LIGHTSPRING_RENEW                               = 126154,
    SPELL_PRIEST_SMITE                              = 585,
    SPELL_PRIEST_DIVINE_INSIGHT_TALENT              = 109175,
    PRIEST_SPELL_DIVINE_INSIGHT_DISCIPLINE          = 123266,
    SPELL_PRIEST_DIVINE_INSIGHT_HOLY                = 123267,
    PRIEST_PRAYER_OF_MENDING_RADIUS                 = 123262,
    PRIEST_FROM_DARKNESS_COMES_LIGHT_AURA           = 109186,
    SPELL_PRIEST_SURGE_OF_LIGHT                     = 114255,
    SPELL_PRIEST_SURGE_OF_LIGHT_DOUBLE              = 128654,
    SPELL_PRIEST_SURGE_OF_DARKNESS                  = 87160,
    SPELL_PRIEST_SURGE_OF_DARKNESS_DOUBLE           = 126083,
    SPELL_PRIEST_MIND_BLAST                         = 8092,
    PRIEST_SPELL_2P_S12_SHADOW                      = 92711,
    PRIEST_SPELL_DISPERSION_SPRINT                  = 129960,
    SEPLL_PRIEST_4P_S12_SHADOW                      = 131556,
    SPELL_PRIEST_SIN_AND_PUNISHMENT                 = 87204,
    SPELL_PRIEST_S12_2P_HEAL                        = 33333,
    PRIEST_SPELL_SOUL_OF_DIAMOND                    = 96219,
    PRIEST_SPELL_4P_S12_HEAL                        = 131566,
    SPELL_PRIEST_HOLY_SPARK                         = 131567,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_IMMUNITY      = 62371,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_FORM          = 27795,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_TALENT        = 20711,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_ROOT          = 27792,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_SHAPESHIFT    = 27827,
    SPELL_PRIEST_GLYPH_OF_THE_VALKYR                = 126094,
    PRIEST_SPELL_LEVITATE                           = 111758,
    SPELL_PRIEST_VOID_TENDRILS_SUMMON               = 127665,
    SPELL_PRIEST_VOID_TENDRILS_GRASP                = 114404,
    PRIEST_NPC_VOID_TENDRILS                        = 65282,
    PRIEST_NPC_PSYFIEND                             = 59190,
    PRIEST_SPELL_SPECTRAL_GUISE_CHARGES             = 119030,
    PRIEST_SPELL_POWER_WORD_SHIELD                  = 17,
    SPELL_PRIEST_DIVINE_AEGIS                       = 47515,
    SPELL_PRIEST_DIVINE_AEGIS_SHIELD                = 47753,
    SPELL_PRIEST_SHIELD_DISCIPLINE                  = 77484,
    SPELL_PRIEST_RAPID_RENEWAL                      = 95649,
    SPELL_PRIEST_RAPID_RENEWAL_HEAL                 = 63544,
    SPELL_PRIEST_GLYPH_OF_RENEW                     = 119872,
    SPELL_PRIEST_ECHO_OF_LIGHT_HEAL                 = 77489,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY_AREA           = 88685,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY_HEAL           = 88686,
    SPELL_PRIEST_MIND_FLY                           = 15407,
    SPELL_PRIEST_MIND_FLY_INSANITY                  = 129197,
    SPELL_PRIEST_MIND_FLY_SR                        = 124468,
    SPELL_PRIEST_MASTERY_SHADOWY_RECALL             = 77486,
    SPELL_PRIEST_GLYPH_OF_MIND_FLAY                 = 120585,
    SPELL_PRIEST_GLYPH_OF_MIND_FLAY_SPEED           = 120587,
    SPELL_PRIEST_VAMPIRIC_EMBRACE                   = 15290,
    SPELL_PRIEST_INNER_FOCUS                        = 89485,
    SPELL_PRIEST_GLYPH_OF_PSYCHIC_SCREAM            = 55676,
    SPELL_PRIEST_DIVINE_STAR_HOLY                   = 110744,
    SPELL_PRIEST_DIVINE_STAR_SHADOW                 = 122121,
    SPELL_PRIEST_DIVINE_STAR_SHADOW_DAMAGE          = 122128,
    SPELL_PRIEST_SHADOW_WORD_DEATH                  = 32379,
    SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE         = 125927,
    SPELL_PRIEST_SHADOW_WORD_DEATH_COOLDOWN         = 95652,
    SPELL_PRIEST_SHADOW_WORD_DEATH_SELF_DAMAGE      = 32409,
    SPELL_PRIEST_T13_2P_SHADOW                      = 105843,
    SPELL_PRIEST_RENEW                              = 139,
    SPELL_PRIEST_GLYPH_OF_PURIFY                    = 55677,
    SPELL_PRIEST_GLYPH_OF_PURIFY_HEAL               = 56131,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC              = 119864,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_DAMAGE       = 119856,
    SPELL_PRIEST_GLYPH_OF_REFLECTIVE_SHIELD         = 33202,
    SPELL_PRIEST_REFLECTIVE_SHIELD_DAMAGE           = 33619,
    SPELL_PRIEST_RAPTURE                            = 47536,
    SPELL_PRIEST_RAPTURE_ENERGIZE                   = 47755,
    SPELL_PRIEST_RAPTURE_COOLDOWN                   = 63853,
    SPELL_PRIEST_SHADOW_APPARITION                  = 147193,
    SPELL_PRIEST_SOLACE_AND_INSANITY                = 139139,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL                 = 126133,
    SPELL_PRIEST_LIGHTWELL_RENEW                    = 126154,
    SPELL_PRIEST_LIGHTWELL_RENEW_GLYPHED            = 7001,
    SPELL_PRIEST_PSYCHIC_TERROR                     = 113792,
    SPELL_PRIEST_POWER_WORD_BARRIER_EFF             = 81782,
    SPELL_PRIEST_PRAYER_OF_MEINDING                 = 33076,
    SPELL_PRIEST_PRAYER_OF_MENDING_DIVINE_INSIGHT   = 123259,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_MENDING         = 55685,
    SPELL_PRIEST_PRAYER_OF_MENDING_MISSILE          = 41637,
    SPELL_PRIEST_CHAKRA_SANCTUARY                   = 81206,
    SPELL_PRIEST_GUARDIAN_SPIRIT                    = 47788,
    SPELL_PRIEST_SPECTRAL_GUISE_STACKS              = 119030,
    SPELL_PRIEST_SPECTRAL_GUISE_STEALTH             = 119032,
    SPELL_PRIEST_T15_HEALER_2P_BONUS                = 138293,
    SPELL_PRIEST_CIRCLE_OF_HEALING                  = 34861,
    SPELL_PRIEST_T15_SHADOW_2P_BONUS                = 138156,
    SPELL_PRIEST_PIOUS_HEALER                       = 145330,
    SPELL_PRIEST_ABSOLUTION                         = 145336,
    SPELL_PRIEST_RESOLUTE_SPIRIT                    = 145374,
    SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD         = 55672,
    SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD_HEAL    = 56160,
    SPELL_PRIEST_GLYPH_OF_SMITE                     = 55692,
    SPELL_PRIEST_ANGELIC_FEATHER_SPEED              = 121557,
    SPELL_PRIEST_GLYPH_OF_INNER_SANCTUM             = 14771,
    SPELL_PRIEST_SPELL_WARDING                      = 91724,
    SPELL_PRIEST_GLYPH_OF_LEAP_OF_FAITH             = 119850,
    SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING         = 55675,
    SPELL_PRIEST_GLYPH_OF_BINDING_HEAL              = 63248,
    SPELL_PRIEST_ANGELIC_BULWARK                    = 114214,
    SPELL_PRIEST_ANGELIC_BULWARK_COOLDOWN           = 114216,
    SPELL_PRIEST_GLYPH_OF_THE_HEAVENS               = 120581,
    SPELL_PRIEST_GLYPH_OF_THE_HEAVENS_VISUAL        = 124433,
    SPELL_PREIST_GLYPH_OF_LEVITATE                  = 108939,
    SPELL_PRIEST_PATH_OF_THE_DEVOUT                 = 111757,
    SPELL_PRIEST_HOLY_WORD_CHASTISE                 = 88625,
    SPELL_PRIEST_GLYPH_OF_INSPIRED_HYMNS            = 147072,
    SPELL_PRIEST_GLYPH_OF_INSPIRED_HYMNS_VISUAL     = 147065,
    SPELL_PRIEST_GLYPH_OF_SHADOWY_FRIENDS           = 126745,
    SPELL_PRIEST_GLYPH_OF_CONFESSION                = 126123
};

// Power Word : Fortitude - 21562
class spell_pri_power_word_fortitude : public SpellScriptLoader
{
    public:
        spell_pri_power_word_fortitude() : SpellScriptLoader("spell_pri_power_word_fortitude") { }

        class spell_pri_power_word_fortitude_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_power_word_fortitude_SpellScript);

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
                OnHit += SpellHitFn(spell_pri_power_word_fortitude_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_power_word_fortitude_SpellScript();
        }
};

// 108920 - Void Tendrils
class spell_pri_void_tendrils : public SpellScript
{
    PrepareSpellScript(spell_pri_void_tendrils);

    void HandleHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_VOID_TENDRILS_SUMMON, true);
        auto summonSpell = sSpellMgr->GetSpellInfo(SPELL_PRIEST_VOID_TENDRILS_SUMMON);
        std::list<TempSummon*> tendrils;
        GetCaster()->GetSummons(tendrils, summonSpell->Effects[EFFECT_0].MiscValue);
        if (!tendrils.empty())
        {
            Unit* tendril = tendrils.back();
            tendril->CastSpell(GetHitUnit(), SPELL_PRIEST_VOID_TENDRILS_GRASP, true);
            tendril->m_Events.Schedule(200, [=]
            {
                if (Unit* target = ObjectAccessor::GetUnit(*tendril, tendril->GetGuidValue(UNIT_FIELD_CHANNEL_OBJECT)))
                {
                    if (!G3D::fuzzyEq(tendril->GetPositionX(), target->GetPositionX()) ||
                        !G3D::fuzzyEq(tendril->GetPositionY(), target->GetPositionY()) ||
                        !G3D::fuzzyEq(tendril->GetPositionZ(), target->GetPositionZ()))
                        tendril->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
                }
            });
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pri_void_tendrils::HandleHit);
    }
};

// 114404 - Void Tendril's Grasp
class spell_pri_void_tendrils_grasp : public AuraScript
{
    PrepareAuraScript(spell_pri_void_tendrils_grasp);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            if (Creature* tendril = caster->ToCreature())
                tendril->DespawnOrUnsummon(1);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_void_tendrils_grasp::HandleRemove, EFFECT_0, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Phantasm (proc) - 114239
class spell_pri_phantasm_proc : public SpellScriptLoader
{
    public:
        spell_pri_phantasm_proc() : SpellScriptLoader("spell_pri_phantasm_proc") { }

        class spell_pri_phantasm_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_phantasm_proc_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    _player->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_phantasm_proc_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_phantasm_proc_SpellScript();
        }
};

// 33076 - Prayer of Mending (S12 4P bonus - Heal)
class spell_pri_item_s12_2p_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_item_s12_2p_heal);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_PRIEST_S12_2P_HEAL))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_HOLY_SPARK, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_item_s12_2p_heal::HandleHit);
    }
};

// 131567 - Holy Spark
class spell_pri_holy_spark : public AuraScript
{
    PrepareAuraScript(spell_pri_holy_spark);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* actor = eventInfo.GetActor();
        return actor && actor->GetGUID() == GetCasterGUID();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_holy_spark::CheckProc);
    }
};

// Called by Dispersion - 47585
// Item : S12 2P bonus - Shadow
class spell_pri_item_s12_2p_shadow : public SpellScriptLoader
{
    public:
        spell_pri_item_s12_2p_shadow() : SpellScriptLoader("spell_pri_item_s12_2p_shadow") { }

        class spell_pri_item_s12_2p_shadow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_item_s12_2p_shadow_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(PRIEST_SPELL_2P_S12_SHADOW))
                        _player->CastSpell(_player, PRIEST_SPELL_DISPERSION_SPRINT, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_item_s12_2p_shadow_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_item_s12_2p_shadow_SpellScript();
        }
};

// 129250 - Power Word: Solace
class spell_pri_power_word_solace : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_solace);

    void HandleOnHit()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_SOLACE_ENERGIZE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_power_word_solace::HandleOnHit);
    }
};

// 139139 - Solace and Insanity
class spell_pri_solace_and_insanity : public AuraScript
{
    PrepareAuraScript(spell_pri_solace_and_insanity);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        int32 heal = eventInfo.GetDamageInfo()->GetDamage();
        uint32 spellId = SPELL_PRIEST_SOLACE_HEAL;
        if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
        {
            spellId = SPELL_PRIEST_SOLACE_HEAL_CRIT;
            heal /= 2; // FIXME
        }
        GetUnitOwner()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_solace_and_insanity::HandleProc);
    }
};

// Shadowfiend - 34433 or Mindbender - 123040
class spell_pri_shadowfiend : public SpellScriptLoader
{
    public:
        spell_pri_shadowfiend() : SpellScriptLoader("spell_pri_shadowfiend") { }

        class spell_pri_shadowfiend_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadowfiend_SpellScript);

            void HandleAfterHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetExplTargetUnit())
                    {
                        if (Guardian* pet = _player->GetGuardianPet())
                        {
                            pet->InitCharmInfo();
                            pet->SetReactState(REACT_DEFENSIVE);
                            pet->ToCreature()->AI()->AttackStart(target);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_shadowfiend_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadowfiend_SpellScript();
        }
};

// Called by Heal - 2050, Greater Heal - 2060 and Flash Heal - 2061
// Strength of Soul - 89488
class spell_pri_strength_of_soul : public SpellScriptLoader
{
    public:
        spell_pri_strength_of_soul() : SpellScriptLoader("spell_pri_strength_of_soul") { }

        class spell_pri_strength_of_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_strength_of_soul_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (_player->HasAura(PRIEST_STRENGTH_OF_SOUL))
                        {
                            if (Aura *weakenedSoul = target->GetAura(PRIEST_WEAKENED_SOUL, _player->GetGUID()))
                            {
                                if (weakenedSoul->GetDuration() > 2000)
                                    weakenedSoul->SetDuration(weakenedSoul->GetDuration() - 2000);
                                else
                                    target->RemoveAura(PRIEST_WEAKENED_SOUL);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_strength_of_soul_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_strength_of_soul_SpellScript();
        }
};

// 596 - Prayer of Healing
// 2050 - Heal
// 2060 - Greater Heal
// 2061 - Flash Heal
class spell_pri_spirit_shell: public SpellScript
{
    PrepareSpellScript(spell_pri_spirit_shell);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_PRIEST_SPIRIT_SHELL))
        {
            int32 absorb = GetHitHeal();

            SetHitHeal(0);

            // Multiple effects stack, so let's try to find this aura.
            if (AuraEffect const* shell = GetHitUnit()->GetAuraEffect(SPELL_PRIEST_SPIRIT_SHELL_ABSORPTION, EFFECT_0, GetCaster()->GetGUID()))
                absorb += shell->GetAmount();

            if (AuraEffect const* mastery = GetCaster()->GetAuraEffect(SPELL_PRIEST_SHIELD_DISCIPLINE, EFFECT_0))
                AddPct(absorb, mastery->GetFloatAmount());

            if (Player* player = GetCaster()->ToPlayer())
                AddPct(absorb, player->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + SPELL_SCHOOL_HOLY));

            absorb = std::min<int32>(absorb, GetCaster()->CountPctFromMaxHealth(60));
            GetCaster()->CastCustomSpell(SPELL_PRIEST_SPIRIT_SHELL_ABSORPTION, SPELLVALUE_BASE_POINT0, absorb, GetHitUnit(), true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_spirit_shell::HandleHit);
    }
};

// Called by Fade - 586
// Phantasm - 108942
class spell_pri_phantasm : public SpellScriptLoader
{
    public:
        spell_pri_phantasm() : SpellScriptLoader("spell_pri_phantasm") { }

        class spell_pri_phantasm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_phantasm_SpellScript);

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                    if (_player->HasAura(PRIEST_PHANTASM_AURA))
                        _player->CastSpell(_player, PRIEST_PHANTASM_PROC, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_phantasm_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_phantasm_SpellScript();
        }
};

// 81662 - Evangelism
class spell_pri_evangelism : public AuraScript
{
    PrepareAuraScript(spell_pri_evangelism);

    void HandleProc(ProcEventInfo& evetInfo)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_EVANGELISM_STACKS, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_evangelism::HandleProc);
    }
};

// 81700 - Archangel
class spell_pri_archangel : public SpellScript
{
    PrepareSpellScript(spell_pri_archangel);

    void HandleHit()
    {
        if (Aura* evangelism = GetCaster()->GetAura(SPELL_PRIEST_EVANGELISM_STACKS))
        {
            if (AuraEffect* archangel = GetCaster()->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
            {
                archangel->ChangeAmount(archangel->GetAmount() * evangelism->GetStackAmount());
                GetCaster()->RemoveAura(SPELL_PRIEST_EVANGELISM_STACKS);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_archangel::HandleHit);
    }
};

// Inner Fire - 588 or Inner Will - 73413
class spell_pri_inner_fire_or_will : public SpellScriptLoader
{
    public:
        spell_pri_inner_fire_or_will() : SpellScriptLoader("spell_pri_inner_fire_or_will") { }

        class spell_pri_inner_fire_or_will_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_inner_fire_or_will_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_INNER_FIRE) || !sSpellMgr->GetSpellInfo(PRIEST_INNER_WILL))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (GetSpellInfo()->Id == PRIEST_INNER_FIRE)
                    {
                        if (_player->HasAura(PRIEST_INNER_WILL))
                            _player->RemoveAura(PRIEST_INNER_WILL);
                    }
                    else if (GetSpellInfo()->Id == PRIEST_INNER_WILL)
                    {
                        if (_player->HasAura(PRIEST_INNER_FIRE))
                            _player->RemoveAura(PRIEST_INNER_FIRE);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_inner_fire_or_will_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_inner_fire_or_will_SpellScript();
        }
};

// 73325 - Leap of Faith, 110718 - Leap of Faith
class spell_pri_leap_of_faith : public SpellScript
{
    PrepareSpellScript(spell_pri_leap_of_faith);

    void HandleHit()
    {
        GetHitUnit()->CastSpell(GetCaster(), PRIEST_LEAP_OF_FAITH_JUMP, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pri_leap_of_faith::HandleHit);
    }
};

// 64044 - Psychic Horror
class spell_pri_psychic_horror : public SpellScript
{
    PrepareSpellScript(spell_pri_psychic_horror);

    uint32 orbs = 0;

    void Prepare()
    {
        orbs = GetCaster()->GetPower(POWER_SHADOW_ORBS);
        GetCaster()->SetPower(POWER_SHADOW_ORBS, 0);
    }

    void HandleHit()
    {
        if (Aura* psychicHorror = GetHitAura())
        {
            int32 newDuration = psychicHorror->GetMaxDuration() + orbs * IN_MILLISECONDS;
            psychicHorror->SetDuration(newDuration);
            psychicHorror->SetMaxDuration(newDuration);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_psychic_horror::Prepare);
        OnHit  += SpellHitFn(spell_pri_psychic_horror::HandleHit);
    }
};

// Guardian Spirit - 47788
class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL))
                    return false;
                return true;
            }

            bool Load() override
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect * /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
                // remove the aura now, we don't want 40% healing bonus
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, PRIEST_SPELL_GUARDIAN_SPIRIT_HEAL, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_2);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

// 47540 - Penance
class spell_pri_penance: public SpellScript
{
    PrepareSpellScript(spell_pri_penance);

    void HandleDummy(SpellEffIndex)
    {
        Unit* priest = GetCaster();
        Unit* target = GetHitUnit();
        if (!target->IsAlive())
            return;

        if (priest->IsFriendlyTo(target))
            priest->CastSpell(target, SPELL_PRIEST_PENANCE_HEAL_TRIGGER, false);
        else
        {
            priest->CastSpell(target, SPELL_PRIEST_PENANCE_DAMAGE_TRIGGER, false);
            if (priest->HasAura(SPELL_PRIEST_EVANGELISM))
                priest->CastSpell(priest, SPELL_PRIEST_EVANGELISM_STACKS, true);
        }

        if (priest->HasAura(SPELL_PRIEST_DIVINE_INSIGHT_TALENT))
            priest->CastSpell(priest, PRIEST_SPELL_DIVINE_INSIGHT_DISCIPLINE, true);
    }

    SpellCastResult CheckCast()
    {
        if (Unit* target = GetExplTargetUnit())
            if (!GetCaster()->IsFriendlyTo(target) && !GetCaster()->IsValidAttackTarget(target))
                return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_penance::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_pri_penance::CheckCast);
    }
};

enum PrayerOfMending
{
    SPELL_T9_HEALING_2_PIECE = 67201,
};

// Prayer of Mending Heal
class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

        class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetOriginalCaster())
                {
                    if (AuraEffect *aurEff = caster->GetAuraEffect(SPELL_T9_HEALING_2_PIECE, EFFECT_0))
                    {
                        int32 heal = GetHitHeal();
                        AddPct(heal, aurEff->GetAmount());
                        SetHitHeal(heal);
                    }
                }

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_prayer_of_mending_heal_SpellScript();
        }
};

// 15473 - Called by Shadow Form
class spell_pri_shadowform : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowform);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        uint32 spellId = GetUnitOwner()->HasAura(PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH;
        GetUnitOwner()->CastSpell(GetUnitOwner(), spellId, true);
        if (GetUnitOwner()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOWY_FRIENDS))
            if (Unit* minipet = GetUnitOwner()->ToPlayer()->GetBattlePetMgr().GetCurrentSummon())
                minipet->CastSpell(minipet, spellId, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
        if (Unit* minipet = GetUnitOwner()->ToPlayer()->GetBattlePetMgr().GetCurrentSummon())
        {
            minipet->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
            minipet->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform::HandleApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform::HandleRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 107906 - Glyph of Shadow
class spell_pri_glyph_of_shadow : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_shadow);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* priest = GetUnitOwner();
        if (priest->HasAura(SPELL_PRIEST_SHADOWFORM_STANCE))
        {
            priest->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
            priest->CastSpell(priest, SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH, true);

            if (priest->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOWY_FRIENDS))
            {
                if (Unit* minipet = priest->ToPlayer()->GetBattlePetMgr().GetCurrentSummon())
                {
                    minipet->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
                    minipet->CastSpell(minipet, SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH, true);
                }
            }
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* priest = GetUnitOwner();
        if (priest->HasAura(SPELL_PRIEST_SHADOWFORM_STANCE))
        {
            priest->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
            priest->CastSpell(priest, SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);

            if (priest->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOWY_FRIENDS))
            {
                if (Unit* minipet = priest->ToPlayer()->GetBattlePetMgr().GetCurrentSummon())
                {
                    minipet->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
                    minipet->CastSpell(minipet, SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_glyph_of_shadow::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_pri_glyph_of_shadow::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Levitate - 1706
class spell_pri_levitate : public SpellScriptLoader
{
    public:
        spell_pri_levitate() : SpellScriptLoader("spell_pri_levitate") { }

        class spell_pri_levitate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_levitate_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster())
                    if (GetHitUnit())
                        GetCaster()->CastSpell(GetHitUnit(), PRIEST_SPELL_LEVITATE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_levitate_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_levitate_SpellScript();
        }
};

static void HandleDivineAegis(Unit* priest, Unit* target, int32 heal, AuraEffect const* aegisPassive)
{
    int32 absorb = CalculatePct(heal, aegisPassive->GetAmount());

    // Mastery: Shield Discipline
    if (AuraEffect* mastery = priest->GetAuraEffect(SPELL_PRIEST_SHIELD_DISCIPLINE, EFFECT_0))
        AddPct(absorb, mastery->GetFloatAmount());

    // Multiple effects stack, so let's try to find this aura.
    if (AuraEffect const* aegisShield = target->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS_SHIELD, EFFECT_0, priest->GetGUID()))
        absorb += aegisShield->GetAmount();

    absorb = std::min<int32>(absorb, priest->CountPctFromMaxHealth(60));

    priest->CastCustomSpell(SPELL_PRIEST_DIVINE_AEGIS_SHIELD, SPELLVALUE_BASE_POINT0, absorb, target, true, nullptr, aegisPassive);

}

// 47515 - Divine Aegis
class spell_pri_divine_aegis : public AuraScript
{
    PrepareAuraScript(spell_pri_divine_aegis);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT;
    }

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        HandleDivineAegis(GetUnitOwner(), eventInfo.GetActionTarget(), eventInfo.GetHealInfo()->GetHeal(), effect);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_divine_aegis::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_divine_aegis::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 139 - Renew
class spell_pri_rapid_renewal : public AuraScript
{
    PrepareAuraScript(spell_pri_rapid_renewal);

    void HandleApply(AuraEffect const* eff, AuraEffectHandleModes)
    {
        if (Unit* priest = GetCaster())
        {
            if (AuraEffect const* rapidRenewal = priest->GetAuraEffect(SPELL_PRIEST_RAPID_RENEWAL, EFFECT_2))
            {
                int32 heal = eff->GetFloatAmount() * eff->GetTotalTicks();
                heal = GetTarget()->SpellHealingBonusTaken(priest, GetSpellInfo(), EFFECT_0, heal, HEAL);
                ApplyPct(heal, rapidRenewal->GetAmount());
                AddPct(heal, 15);
                priest->CastCustomSpell(SPELL_PRIEST_RAPID_RENEWAL_HEAL, SPELLVALUE_BASE_POINT0, heal, GetTarget(), true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_rapid_renewal::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 17 - Power Word: Shield, 123258 - Power Word : Shield (Divine Insight)
class spell_pri_power_word_shield : public AuraScript
{
    PrepareAuraScript(spell_pri_power_word_shield);

    void AfterRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Unit* caster = GetCaster();
        if (caster && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL)
        {
            if (AuraEffect const* raptureEff = caster->GetAuraEffectOfRankedSpell(SPELL_PRIEST_RAPTURE, EFFECT_0))
            {
                if (!caster->HasAura(SPELL_PRIEST_RAPTURE_COOLDOWN))
                {
                    int32 amount = GetSpellInfo()->CalcPowerCost(caster, GetSpellInfo()->GetSchoolMask());
                    // Priest T13 Healer 4P Bonus - Increase mana granted from Rapture by 100%
                    if (AuraEffect const* eff = caster->GetAuraEffect(105832, EFFECT_1))
                        AddPct(amount, eff->GetAmount());

                    caster->CastCustomSpell(SPELL_PRIEST_RAPTURE_ENERGIZE, SPELLVALUE_BASE_POINT0, amount, caster, true);
                    caster->CastSpell(caster, SPELL_PRIEST_RAPTURE_COOLDOWN, true);
                }
            }
        }
    }

    void ReflectDamage(AuraEffect* eff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        Unit* attacker = dmgInfo.GetAttacker();

        if (!attacker || attacker == target || (dmgInfo.GetSpellInfo() && dmgInfo.GetSpellInfo()->Id == SPELL_PRIEST_REFLECTIVE_SHIELD_DAMAGE))
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster == GetUnitOwner() && !attacker->IsTotem())
        {
            if (AuraEffect const* reflectiveShield = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_REFLECTIVE_SHIELD, EFFECT_0))
            {
                int32 damage = CalculatePct(absorbAmount, reflectiveShield->GetAmount());
                target->CastCustomSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_DAMAGE, SPELLVALUE_BASE_POINT0, damage, attacker, true);
            }
        }
    }

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // Divine Aegis
        if (AuraEffect const* aegis = caster->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS, EFFECT_0))
            if (roll_chance_f(caster->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + GetFirstSchoolInMask(GetSpellInfo()->GetSchoolMask()))))
                amount *= 2;
        if (AuraEffect const* glyph = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD, EFFECT_0))
        {
            int32 absorb = CalculatePct(amount, glyph->GetAmount());
            amount -= absorb;
            caster->CastCustomSpell(SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD_HEAL, SPELLVALUE_BASE_POINT0, absorb, GetUnitOwner(), true);
        }
    }

    void Register() override
    {
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_power_word_shield::ReflectDamage, EFFECT_0);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_power_word_shield::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 32592 - Mass Dispel
class spell_pri_mass_dispel : public SpellScriptLoader
{
    public:
        spell_pri_mass_dispel() : SpellScriptLoader("spell_pri_mass_dispel") { }

        class spell_pri_mass_dispel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mass_dispel_SpellScript);

            bool _hasImmunity;

            bool Load() override
            {
                _hasImmunity = false;
                return true;
            }

            void CheckAuras()
            {
                // Check if Immunity aura is on target
                if (Unit* target = GetHitUnit())
                    if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
                        _hasImmunity = true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                // Glyph of Mass Dispel - Prevent dispel effect when target has immunity and no glyph is applied
                if (_hasImmunity)
                    if (!GetCaster()->HasAura(55691))
                        PreventHitEffect(effIndex);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_mass_dispel_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DISPEL);
                BeforeHit += SpellHitFn(spell_pri_mass_dispel_SpellScript::CheckAuras);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mass_dispel_SpellScript();
        }
};

// Mind Blast - 8092
class spell_pri_mind_blast : public SpellScriptLoader
{
    public:
        spell_pri_mind_blast() : SpellScriptLoader("spell_pri_mind_blast") { }

        class spell_pri_mind_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_blast_SpellScript)

            enum
            {
                SPELL_DIVINE_INSIGHT_PROC = 124430,
            };

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                // Handle Divine Insight if it procced while casting current Mind Blast
                Spell::UsedSpellMods const& mods = GetAppliedSpellMods();
                AuraEffect* aurEff = caster->GetAuraEffect(SPELL_DIVINE_INSIGHT_PROC, EFFECT_0);

                if (!aurEff || mods.find(aurEff->GetBase()) != mods.end())
                    return;

                caster->ToPlayer()->RemoveSpellCooldown(GetSpellInfo()->Id, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_mind_blast_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_blast_SpellScript();
        }
};

// Confession - 126123
class spell_pri_confession : public SpellScriptLoader
{
    public:
        spell_pri_confession() : SpellScriptLoader("spell_pri_confession") { }

        class spell_pri_confession_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_confession_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 confessions[57] = 
                {
                    64978,   // [%s] confesses: I once took a bow that a hunter wanted.
                    64991,   // [%s] confesses: I have "borrowed" things from my guild bank.
                    65005,   // [%s] confesses: I always forget to gem my gear.
                    65006,   // [%s] confesses: I spent six months chasing the Time-Lost Proto-Drake.
                    65007,   // [%s] confesses: I went into Alterac Valley and didn't help my team at all.
                    65008,   // [%s] confesses: I died to an elevator once. Maybe more than once.
                    65009,   // [%s] confesses: I never use the Lightwell.
                    65010,   // [%s] confesses: I always wanted to be a paladin.
                    65011,   // [%s] confesses: Troll toes sort of creep me out.
                    65012,   // [%s] confesses: I have stood in the fire.
                    65013,   // [%s] confesses: I told my raid leader that I was ready, but I wasn't really ready.
                    65014,   // [%s] confesses: I fell off of Dalaran.
                    65016,   // [%s] confesses: I asked a friend for gold to buy my first mount.
                    65018,   // [%s] confesses: I said I had been in the dungeon before, but I had no idea what I was doing. It was embarassing.
                    65019,   // [%s] confesses: I once punched a gnome. No reason. I was just having a bad day.
                    65020,   // [%s] confesses: I outbid a friend on an auction for something I didn't really want.
                    65021,   // [%s] confesses: I go into dungeons not to make Azeroth a better place, but just for loot.
                    65022,   // [%s] confesses: I wasn't really at the opening of Ahn'Qiraj. I just read about it.
                    65023,   // [%s] confesses: I don't really have a clue who the Sin'dorei are.
                    65024,   // [%s] confesses: I don't treat all of my mounts equally.
                    65025,   // [%s] confesses: I dabble in archaeology, but I'm just not that interested in history.
                    65026,   // [%s] confesses: The draenei laugh really annoys me.
                    65027,   // [%s] confesses: I haven't been in a barber shop in months. Goblins with scissors. Shudder.
                    65028,   // [%s] confesses: I thought pandaren were a type of furbolg.
                    65029,   // [%s] confesses: Oh, I took the candle.
                    65030,   // [%s] confesses: This is just a setback.
                    65031,   // [%s] confesses: I light things on fire and yell BY FIRE BE PURGED when nobody is looking.
                    65032,   // [%s] confesses: I don't really remember you in the mountains.
                    65033,   // [%s] confesses: I sometimes forget if Northrend is north or south of here.
                    65034,   // [%s] confesses: I find all these names with so many apostrophes so confusing.
                    65035,   // [%s] confesses: I forgot the Sunwell.
                    65036,   // [%s] confesses: I really wasn't prepared. Who knew?
                    65037,   // [%s] confesses: I sometimes wonder if tauren taste like... you know.
                    65038,   // [%s] confesses: I ask for the Light to give me strength, but I'm not sure if it really does.
                    65039,   // [%s] confesses: I sometimes use my mount to travel really short distances. I mean REALLY short.
                    65040,   // [%s] confesses: I don't know if Milhouse is a good guy or not.
                    65041,   // [%s] confesses: I know he's a jerk, but there's something about Garrosh....
                    65042,   // [%s] confesses: I saw a mage cast a spell once and my jaw really did drop at the damage.
                    65044,   // [%s] confesses: For a long time, I thought the plural of anecdote WAS data.
                    65045,   // [%s] confesses: Sometimes I ask for a warlock to summon me when I'm really not that far away.
                    65046,   // [%s] confesses: Sometimes when I'm questing, I want to be alone, so I pretend I can't hear my friends.
                    74300,   // [%s] confesses: When my allies get mind controlled, I try really hard to kill them.
                    74301,   // [%s] confesses: I don't use the strongest battle pets. Just the cutest ones.
                    74302,   // [%s] confesses: I get heart of the wild and the other heart of the wild confused.
                    74303,   // [%s] confesses: I always ask someone else how to reforge my gear.
                    74304,   // [%s] confesses: I inspect others to see if they are inspecting me.
                    74305,   // [%s] confesses: When pandaren tell me to slooow doooown, I just want to punch them.
                    74306,   // [%s] confesses: I always sell all of my old gear to the same vendor. Dude's rich or something.
                    74307,   // [%s] confesses: I care more about transmogrification gear than upgrade gear.
                    74308,   // [%s] confesses: I can't decide between Horde and Alliance. Horde has goblin mages with quad-pigtails but Alliance has dwarf priests with twirly braids.
                    76092,   // [%s] confesses: I haven't visited Sunsong Ranch in months. I can just imagine weeds and virmen everywhere.
                    76093,   // [%s] confesses: I think one time I tried to kill Lei Shi instead of Lei Shen.
                    76094,   // [%s] confesses: I used to think Medivh was a druid because of all the feathers.
                    76095,   // [%s] confesses: I still carry around Argent Crusade Scorugestones. You never know when they'll be useful again.
                    76096,   // [%s] confesses: I feel bad for my ground mounts. I hardly ever use them anymore.
                    76097,   // [%s] confesses: I'm going to totally ninja that hunter's loot.
                    76098    // [%s] confesses: I am confused why female draenei like gnomes.                    
                };

                auto target = GetHitUnit();
                if (GetCaster() != target)
                {
                    BroadcastText const* bct = sObjectMgr->GetBroadcastText(confessions[urand(0, 56)]);
                    Player* _player = target->ToPlayer();
                    LocaleConstant loc_idx = _player->GetSession()->GetSessionDbLocaleIndex();
                    std::string baseText = "";
                    if (bct)
                        baseText = bct->GetText(loc_idx, _player->GetGender());

                    target->TextEmote(baseText.c_str(), nullptr); 
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_confession_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_confession_SpellScript();
        }
};

// Glyph of Corpse Explosion - 126152
class spell_pri_glyph_of_confession : public SpellScriptLoader
{
    public:
        spell_pri_glyph_of_confession() : SpellScriptLoader("spell_pri_glyph_of_confession") { }
        class spell_pri_glyph_of_confession_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_glyph_of_confession_AuraScript);
            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    _player->LearnSpell(SPELL_PRIEST_GLYPH_OF_CONFESSION, false);
            }
            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* _player = GetTarget()->ToPlayer())
                    if (_player->HasSpell(SPELL_PRIEST_GLYPH_OF_CONFESSION))
                        _player->RemoveSpell(SPELL_PRIEST_GLYPH_OF_CONFESSION, false, false);
            }
            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_pri_glyph_of_confession_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_pri_glyph_of_confession_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };
        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_glyph_of_confession_AuraScript();
        }
};


// 2944 - Devouring Plague
class spell_pri_devouring_plague : public SpellScript
{
    PrepareSpellScript(spell_pri_devouring_plague);

    int32 orbs = 0;

    void Prepare()
    {
        if (GetCaster()->GetTypeId() == TYPEID_PLAYER)
            orbs = GetCaster()->GetPower(POWER_SHADOW_ORBS);
    }

    void HandleDamage(SpellEffIndex)
    {
        // Instant damage equal to amount of shadow orb
        SetHitDamage(GetHitDamage() * orbs);
    }

    void ModifyAmount()
    {
        if (AuraEffect* dot = GetHitUnit()->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_1, GetCaster()->GetGUID()))
        {
            // Increase DOT damage and heal pct.
            dot->SetAmount(dot->GetAmount() * orbs);
            // Periodic dummy effect. Sniffs tells that it keep % of damage from the shadow orbs
            if (AuraEffect* dummy = GetHitUnit()->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_2, GetCaster()->GetGUID()))
            {
                dummy->SetAmount(orbs * 100);
                dummy->SetAmplitude(dot->GetAmplitude());
                dummy->SetPeriodicTimer(dot->GetPeriodicTimer());
            }
        }
    }

    void RemoveShadowOrbs()
    {
        GetCaster()->SetPower(POWER_SHADOW_ORBS, 0);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_devouring_plague::Prepare);
        OnEffectHitTarget += SpellEffectFn(spell_pri_devouring_plague::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterHit += SpellHitFn(spell_pri_devouring_plague::ModifyAmount);
        AfterCast += SpellCastFn(spell_pri_devouring_plague::RemoveShadowOrbs);
    }
};

// 2944 - Devouring Plague (heal effect)
class spell_pri_devouring_plague_heal : public AuraScript
{
    PrepareAuraScript(spell_pri_devouring_plague_heal);

    void OnTick(AuraEffect const* eff)
    {
        if (Unit* priest = GetCaster())
            priest->CastCustomSpell(SPELL_PRIEST_DEVOURING_PLAGUE_HEAL, SPELLVALUE_BASE_POINT0, eff->GetAmount() / 100, priest, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_devouring_plague_heal::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 109142 - Twist of Fate
class spell_pri_twist_of_fate : public AuraScript
{
    PrepareAuraScript(spell_pri_twist_of_fate);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& procEvent)
    {
        Unit* unit = procEvent.GetActionTarget();
        if (!unit)
            return;

        if (unit->GetHealthPct() >= aurEff->GetAmount())
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_twist_of_fate::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 77485 - Mastery: Echo of Light
class spell_pri_echo_of_light : public AuraScript
{
    PrepareAuraScript(spell_pri_echo_of_light);

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetActionTarget();
        int32 heal = eventInfo.GetHealInfo()->GetHeal();
        SpellInfo const* bySpell = eventInfo.GetSpellInfo();
        if (!target || !heal || !bySpell)
            return;

        float amount = CalculatePct(float(heal), eff->GetFloatAmount()) / 2; // tick count, hotfixed by spell mod
        GetCaster()->CastCustomSpell(SPELL_PRIEST_ECHO_OF_LIGHT_HEAL, SPELLVALUE_BASE_POINT0, amount, target, true, nullptr,eff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_echo_of_light::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 88685 - Holy Word : Sanctuary
class spell_pri_holy_word_sanctuary : public AuraScript
{
    PrepareAuraScript(spell_pri_holy_word_sanctuary);

    float mult = 1.0f;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* priest = GetCaster())
            if (AuraEffect const* bonus = priest->GetAuraEffect(SPELL_PRIEST_ABSOLUTION, EFFECT_0))
                AddPct(mult, bonus->GetAmount());
    }

    void OnTick(AuraEffect const*)
    {
        if (Unit* caster = GetCaster())
        {
            if (DynamicObject* dynObj = caster->GetDynObject(SPELL_PRIEST_HOLY_WORD_SANCTUARY_AREA))
            {
                CustomSpellValues ctx;
                ctx.SetMultiplier(mult);
                caster->CastCustomSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(),
                    SPELL_PRIEST_HOLY_WORD_SANCTUARY_HEAL, ctx, true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_holy_word_sanctuary::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_holy_word_sanctuary::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 77486 - Mastery: Shadowy Recall
class spell_pri_shadowy_recall : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowy_recall);

    void HandleProc(AuraEffect const* mastery, ProcEventInfo& eventInfo)
    {
        AuraEffect const* byAuraEffect = eventInfo.GetProcTriggerContext()->TriggeredByAuraEffect;
        if (!byAuraEffect)
            return;

        if (!roll_chance_f(mastery->GetFloatAmount()))
            return;

        uint32 spellId = 0;
        switch (byAuraEffect->GetId())
        {
            case SPELL_PRIEST_SHADOW_WORD_PAIN: spellId = SPELL_PRIEST_SHADOW_WORD_PAIN_SR; break;
            case SPELL_PRIEST_VAMPIRIC_TOUCH:   spellId = SPELL_PRIEST_VAMPIRIC_TOUCH_SR; break;
            case SPELL_PRIEST_DEVOURING_PLAGUE: spellId = SPELL_PRIEST_DEVOURING_PLAGUE_SR; break;
            case SPELL_PRIEST_MIND_FLY:
            case SPELL_PRIEST_MIND_FLY_INSANITY:spellId = SPELL_PRIEST_MIND_FLY_SR; break;
            default:
                return;
        }

        int32 bp = byAuraEffect->GetAmount();
        Unit* priest = GetUnitOwner();
        priest->CastSpell(eventInfo.GetActionTarget(), spellId, true);

        switch (byAuraEffect->GetId())
        {
            case SPELL_PRIEST_VAMPIRIC_TOUCH:
                priest->EnergizeBySpell(priest, SPELL_PRIEST_VAMPIRIC_TOUCH, CalculatePct(priest->GetMaxPower(POWER_MANA), 2), POWER_MANA);
                break;
            case SPELL_PRIEST_DEVOURING_PLAGUE:
                if (AuraEffect const* plague = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_2, priest->GetGUID()))
                    priest->CastCustomSpell(SPELL_PRIEST_DEVOURING_PLAGUE_HEAL, SPELLVALUE_BASE_POINT0, plague->GetAmount() / 100, priest, true);
                break;
        }
   }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_shadowy_recall::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 20711 - Spirit of Redemption
class spell_pri_spirit_of_redemption : public AuraScript
{
    PrepareAuraScript(spell_pri_spirit_of_redemption);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void Absorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (Player* priest = GetOwner()->ToPlayer())
        {
            if (dmgInfo.GetDamage() < priest->GetHealth())
                return;

            if (priest->HasAura(SPELL_PRIEST_GUARDIAN_SPIRIT))
                return;

            if (priest->HasAura(SPELL_PRIEST_SPIRIT_OF_REDEMPTION_FORM))
            {
                /*
                Unit const* attacker = dmgInfo.GetAttacker();
                uint32 attackerId = 0;
                if (attacker)
                    attackerId = attacker->GetTypeId() == TYPEID_PLAYER ? attacker->GetGUIDLow() : attacker->GetEntry();
                char const* attackerType = "<unknown>";
                if (attacker)
                    attackerType = attacker->GetTypeId() == TYPEID_PLAYER ? "player" : "creature";

                TC_LOG_ERROR("shitlog", "spell_pri_spirit_of_redemption, owner: %u, attacker: %u (%s), spell: %u", priest->GetGUIDLow(), attackerId, attackerType, dmgInfo.GetSpellInfo() ? dmgInfo.GetSpellInfo()->Id : 0);
                */
                absorbAmount = dmgInfo.GetDamage();
                return;
            }

            if (dmgInfo.GetAttacker() && dmgInfo.GetAttacker()->GetTypeId() == TYPEID_PLAYER)
            {
                dmgInfo.GetAttacker()->ToPlayer()->RewardPlayerAndGroupAtKill(priest, false);
                if (Battleground* bg = priest->GetBattlegorund())
                    bg->HandleKillPlayer(priest, dmgInfo.GetAttacker()->ToPlayer());
            }

            auto& auras = priest->GetAppliedAuras();
            for (auto itr = auras.begin(); itr != auras.end();)
            {
                if (!itr->second->IsPositive() && itr->second->GetBase()->GetCasterGUID() != priest->GetGUID())
                {
                    priest->RemoveAura(itr);
                    itr = auras.begin();
                }
                else
                    ++itr;
            }
            priest->CastSpell(priest, SPELL_PRIEST_SPIRIT_OF_REDEMPTION_ROOT, true);
            priest->CastSpell(priest, SPELL_PRIEST_SPIRIT_OF_REDEMPTION_FORM, true);
            priest->CastSpell(priest, SPELL_PRIEST_SPIRIT_OF_REDEMPTION_IMMUNITY, true);
            priest->CastSpell(priest, SPELL_PRIEST_SPIRIT_OF_REDEMPTION_SHAPESHIFT, true);

            if (priest->HasAura(SPELL_PRIEST_GLYPH_OF_THE_VALKYR))
                priest->SetDisplayId(43573);

            if (Battleground* bg = priest->GetBattleground())
                if (bg->IsArena())
                    bg->CheckArenaWinConditions();

            absorbAmount = dmgInfo.GetDamage();
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_spirit_of_redemption::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_spirit_of_redemption::Absorb, EFFECT_0);
    }
};

// 27827 - Spirit of Redemption (Shapeshift)
class spell_pri_spirit_of_redemption_form : public AuraScript
{
    PrepareAuraScript(spell_pri_spirit_of_redemption_form);

    bool needDie = true;

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        // disable breath/etc timers
        player->StopMirrorTimers();

        // set stand state (expected in this form)
        if (!player->IsStandState())
            player->SetStandState(UNIT_STAND_STATE_STAND);

        if (player->duel)
        {
            needDie = false;
            player->DuelComplete(DUEL_WON);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        Player* priest = GetUnitOwner()->ToPlayer();

        priest->RemoveAura(SPELL_PRIEST_SPIRIT_OF_REDEMPTION_IMMUNITY);
        priest->RemoveAura(SPELL_PRIEST_SPIRIT_OF_REDEMPTION_FORM);
        priest->RemoveAura(SPELL_PRIEST_SPIRIT_OF_REDEMPTION_ROOT);

        if (needDie)
        {
            if (!priest->GetBattlegorund() || (sWorld->getBoolConfig(CONFIG_DURABILITY_LOSS_IN_PVP)))
            {
                double baseLoss = sWorld->getRate(RATE_DURABILITY_LOSS_ON_DEATH);
                uint32 loss = uint32(baseLoss - (baseLoss * priest->GetTotalAuraMultiplier(SPELL_AURA_MOD_DURABILITY_LOSS)));
                priest->DurabilityLossAll(baseLoss, false);
                priest->SendDurabilityLoss(priest, loss);
            }
            priest->setDeathState(JUST_DIED);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectRemoveFn(spell_pri_spirit_of_redemption_form::HandleApply, EFFECT_0, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_spirit_of_redemption_form::HandleRemove, EFFECT_0, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
    }
};

// 129176 - Shadow Word: Death 
class spell_pri_shadow_word_death_glyphed : public SpellScript
{
    PrepareSpellScript(spell_pri_shadow_word_death_glyphed);

    bool m_below20 = false;

    void HandleBeforeHit()
    {
        Unit* target = GetHitUnit();
        if (target && target->HealthBelowPct(20))
        {
            m_below20 = true;
            SetHitDamage(GetHitDamage() * 4);
        }
    }

    void HandleAfterHit()
    {
        Player* priest = GetCaster()->ToPlayer();
        if (!priest)
            return;

        TimeValue lastCooldown;

        if (m_below20 && priest->GetTalentSpecialization() == SPEC_PRIEST_SHADOW)
        {
            lastCooldown = Milliseconds(priest->GetSpellCooldownDelay(GetSpellInfo()->Id));

            bool cooldown = priest->HasAura(SPELL_PRIEST_SHADOW_WORD_DEATH_COOLDOWN);
            if (!cooldown)
                priest->CastSpell(priest, SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE, true);

            if (GetHitUnit()->IsAlive() && !cooldown)
            {
                priest->RemoveSpellCooldown(GetSpellInfo()->Id, true);
                priest->CastSpell(priest, SPELL_PRIEST_SHADOW_WORD_DEATH_COOLDOWN, true);
            }
        }

        if (GetHitUnit()->IsAlive())
        {
            uint32 damage = GetHitDamage();
            if (AuraEffect const* eff = priest->GetAuraEffect(SPELL_PRIEST_T13_2P_SHADOW, EFFECT_1))
                AddPct(damage, -eff->GetAmount());
            priest->CastCustomSpell(SPELL_PRIEST_SHADOW_WORD_DEATH_SELF_DAMAGE, SPELLVALUE_BASE_POINT0, damage, priest, true);
        }
        else if (priest->GetTalentSpecialization() == SPEC_PRIEST_SHADOW)
        {
            if (lastCooldown != TimeValue::zero() && TimeValue::Now() - Seconds(6) > lastCooldown)
                priest->CastSpell(priest, SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE, true);
        }
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_pri_shadow_word_death_glyphed::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_pri_shadow_word_death_glyphed::HandleAfterHit);
    }
};

// 32379 - Shadow Word: Death 
class spell_pri_shadow_word_death : public SpellScript
{
    PrepareSpellScript(spell_pri_shadow_word_death);

    void HandleAfterHit()
    {
        Player* priest = GetCaster()->ToPlayer();
        if (!priest || priest->GetTalentSpecialization() != SPEC_PRIEST_SHADOW)
            return;

        bool cooldown = priest->HasAura(SPELL_PRIEST_SHADOW_WORD_DEATH_COOLDOWN);
        if (!cooldown)
            priest->CastSpell(priest, SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE, true);

        if (GetHitUnit()->IsAlive() && !cooldown)
        {
            priest->RemoveSpellCooldown(GetSpellInfo()->Id, true);
            priest->CastSpell(priest, SPELL_PRIEST_SHADOW_WORD_DEATH_COOLDOWN, true);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pri_shadow_word_death::HandleAfterHit);
    }
};

// 95740 - Shadow Orbs
class spell_pri_shadow_orbs : public AuraScript
{
    PrepareAuraScript(spell_pri_shadow_orbs);

    void HandleAura(AuraEffect const*, AuraEffectHandleModes)
    {
        uint32 maxpower = GetAura()->IsRemoved() ? 0 : GetUnitOwner()->GetCreatePowers(POWER_SHADOW_ORBS);
        GetUnitOwner()->SetMaxPower(POWER_SHADOW_ORBS, maxpower);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_shadow_orbs::HandleAura, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_pri_shadow_orbs::HandleAura, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 108945 - Angelic Bulwark
class spell_pri_angelic_bulwark : public AuraScript
{
    PrepareAuraScript(spell_pri_angelic_bulwark);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetUnitOwner()->HealthBelowPct(30) && !GetUnitOwner()->HasAura(SPELL_PRIEST_ANGELIC_BULWARK_COOLDOWN);
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* priest = GetUnitOwner();
        int32 absorb = priest->CountPctFromMaxHealth(20);
        if (priest->GetMap()->IsBattlegroundOrArena() || priest->HasAura(134735))
            AddPct(absorb, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));
        priest->CastCustomSpell(SPELL_PRIEST_ANGELIC_BULWARK, SPELLVALUE_BASE_POINT0, absorb, priest, true);
        priest->CastSpell(priest, SPELL_PRIEST_ANGELIC_BULWARK_COOLDOWN, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_angelic_bulwark::CheckProc);
        OnProc += AuraProcFn(spell_pri_angelic_bulwark::HandleProc);
    }
};

// 109175 - Divine Insight (Talent)
class spell_pri_divine_insight_talent : public AuraScript
{
    PrepareAuraScript(spell_pri_divine_insight_talent);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Player* priest = GetUnitOwner()->ToPlayer())
            return priest->GetTalentSpecialization() == SPEC_PRIEST_SHADOW;
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_divine_insight_talent::CheckProc);
    }
};

// 124430 - Divine Insight (Shadow proc)
class spell_pri_divine_insight_shadow : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_insight_shadow);

    void ResetCooldown()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_PRIEST_MIND_BLAST, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_divine_insight_shadow::ResetCooldown);
    }
};

// 2060 - Greater Heal, 596 - Prayer of Healing
// 109175 - Divine Insight (Holy)
class spell_pri_divine_insight_holy : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_insight_holy);

    void HandlCast()
    {
        if (Player* priest = GetCaster()->ToPlayer())
            if (priest->GetTalentSpecialization() == SPEC_PRIEST_HOLY)
                if (priest->HasAura(SPELL_PRIEST_DIVINE_INSIGHT_TALENT))
                    if (roll_chance_i(40))
                        priest->CastSpell(priest, SPELL_PRIEST_DIVINE_INSIGHT_HOLY, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_divine_insight_holy::HandlCast);
    }
};

// 123262 - Prayer of Mending
class spell_pri_prayer_of_mending_divine_insight : public AuraScript
{
    PrepareAuraScript(spell_pri_prayer_of_mending_divine_insight);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* missle = eventInfo.TriggeredBySpell();
        return missle && missle->GetSpellInfo()->Id == SPELL_PRIEST_PRAYER_OF_MENDING_MISSILE &&
            eventInfo.GetActor() && eventInfo.GetActor() == missle->GetOriginalCaster();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_prayer_of_mending_divine_insight::CheckProc);
    }
};

// 45243 - Focused Will
class spell_pri_focused_will : public AuraScript
{
    PrepareAuraScript(spell_pri_focused_will);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
            return true;

        return eventInfo.GetDamageInfo()->GetDamage() >= GetUnitOwner()->CountPctFromMaxHealth(10);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_focused_will::CheckProc);
    }
};

// 15407 - Mind Flay
class spell_pri_mind_fly_selector : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_fly_selector);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(SPELL_PRIEST_SOLACE_AND_INSANITY) && GetExplTargetUnit() &&
            GetExplTargetUnit()->HasAura(SPELL_PRIEST_DEVOURING_PLAGUE, GetCaster()->GetGUID()))
        {
            Unit* priest = GetCaster();
            Unit* target = GetExplTargetUnit();
            priest->m_Events.Schedule(1, [=]()
            {
                priest->CastSpell(target, SPELL_PRIEST_MIND_FLY_INSANITY);
            });
            return SPELL_FAILED_DONT_REPORT;
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_mind_fly_selector::CheckCast);
    }
};

// 15407 - Mind Flay, 129197 - Mind Flay (Insanity)
class spell_pri_mind_flay : public AuraScript
{
    PrepareAuraScript(spell_pri_mind_flay)

    void InitEffects(uint32& effectMask)
    {
        // Glyph of Mind Flay - Remove slow effect
        Unit* priest = GetCaster();
        if (priest && priest->HasAura(SPELL_PRIEST_GLYPH_OF_MIND_FLAY))
            effectMask &= ~(1 << EFFECT_1);
    }

    void HandleTick(AuraEffect const*)
    {
        if (Unit* priest = GetCaster())
            if (priest->HasAura(SPELL_PRIEST_GLYPH_OF_MIND_FLAY))
                priest->CastSpell(priest, SPELL_PRIEST_GLYPH_OF_MIND_FLAY_SPEED, true);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_pri_mind_flay::InitEffects);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_mind_flay::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 129197 - Mind Flay (Insanity)
class spell_pri_mind_flay_insanity : public AuraScript
{
    PrepareAuraScript(spell_pri_mind_flay_insanity);

    void CalulcateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (AuraEffect const* eff = GetUnitOwner()->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_2, GetCasterGUID()))
        {
            float pct = CalculatePct(33.0f, eff->GetAmount());
            AddPct(amount, pct);
        }
    }

    void Register() override
    {
        AfterEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_mind_flay_insanity::CalulcateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 124468 - Mind Flay
class spell_pri_mind_flay_shadowy_recall : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_flay_shadowy_recall);

    void HandleHit()
    {
        if (GetCaster()->HasAura(SPELL_PRIEST_SOLACE_AND_INSANITY))
        {
            if (AuraEffect const* eff = GetHitUnit()->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_2, GetCaster()->GetGUID()))
            {
                uint32 damage = GetHitDamage();
                float pct = CalculatePct(33.0f, eff->GetAmount());
                AddPct(damage, pct);
                SetHitDamage(damage);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_mind_flay_shadowy_recall::HandleHit);
    }
};

// 15286 - Vampiric Embrace
class spell_pri_vampiric_embrace : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_embrace);

    int32 healing = 0;

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetSchoolMask() & SPELL_SCHOOL_MASK_SHADOW))
            return false;

        return eventInfo.GetSpellInfo() && !eventInfo.GetSpellInfo()->IsAoESpell();
    }

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        uint32 damage = eventInfo.GetDamageWithoutResilience();
        healing += CalculatePct(damage, effect->GetAmount());
    }

    void HandleTick(AuraEffect const* effect)
    {
        if (healing)
        {
            GetUnitOwner()->CastCustomSpell(SPELL_PRIEST_VAMPIRIC_EMBRACE, SPELLVALUE_BASE_POINT0, healing, GetUnitOwner(), true, nullptr, effect);
            healing = 0;
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_vampiric_embrace::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 15290 - Vampiric Embrace
class spell_pri_vampiric_embrace_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_vampiric_embrace_heal);

    uint32 m_targetsCount = 0;

    void CountTargets(std::list<WorldObject*>& targets)
    {
        m_targetsCount = targets.size();
    }

    void HandleHit()
    {
        if (!m_targetsCount)
            return;

        uint32 divider = std::min(m_targetsCount, uint32(15));
        float heal = float(GetHitHeal()) / divider;
        SetHitHeal(heal);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_vampiric_embrace_heal::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnHit += SpellHitFn(spell_pri_vampiric_embrace_heal::HandleHit);
    }
};

// 73510 - Mind Spike
class spell_pri_mind_spike : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_spike);

    bool m_surge = false;

    void HandleCast()
    {
        if (Aura* surge = GetCaster()->GetAura(SPELL_PRIEST_SURGE_OF_DARKNESS))
        {
            surge->ModStackAmount(-1);
            m_surge = true;
        }
    }

    void HandleHit()
    {
        Unit* target = GetHitUnit();
        if (m_surge)
            SetHitDamage(GetHitDamage() * 1.5f);
        else
        {
            for (auto&& spell : { SPELL_PRIEST_SHADOW_WORD_PAIN, SPELL_PRIEST_DEVOURING_PLAGUE, SPELL_PRIEST_VAMPIRIC_TOUCH })
                target->RemoveAurasDueToSpell(spell, GetCaster()->GetGUID());
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_mind_spike::HandleCast);
        OnHit += SpellHitFn(spell_pri_mind_spike::HandleHit);
    }
};

// 2061 - Flash Heal
class spell_pri_surge_of_light_consumer : public SpellScript
{
    PrepareSpellScript(spell_pri_surge_of_light_consumer);

    void RemoveStack()
    {
        if (Aura* surgeOfLight = GetCaster()->GetAura(SPELL_PRIEST_SURGE_OF_LIGHT))
            surgeOfLight->ModStackAmount(-1);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_surge_of_light_consumer::RemoveStack);
    }
};

// 109186 - From Darkness, Comes Light
class spell_pri_from_darkness_comes_light : public AuraScript
{
    PrepareAuraScript(spell_pri_from_darkness_comes_light);

    Player* priest = nullptr;

    bool Load() override
    {
        priest = GetOwner()->ToPlayer();
        return priest != nullptr;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->SpellFamilyName != SPELLFAMILY_PRIEST)
            return false;

        switch (priest->GetTalentSpecialization())
        {
            case SPEC_PRIEST_SHADOW:        // Shadow, 20% from Vampiric Touch tick
                if (eventInfo.GetProcTriggerContext()->Phase != ProcPhase::Hit)
                    return false;
                if (spellInfo->Id == SPELL_PRIEST_VAMPIRIC_TOUCH ||
                    spellInfo->Id == SPELL_PRIEST_VAMPIRIC_TOUCH_SR)
                    return roll_chance_i(20);
                return false;
            case SPEC_PRIEST_DISCIPLINE:    // 15% from healing spells or Smite
            case SPEC_PRIEST_HOLY:
            {
                if (eventInfo.GetProcTriggerContext()->Phase != ProcPhase::Cast)
                    return false;
                if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_PERIODIC)
                    return false;

                bool isHeal = (spellInfo->AttributesEx8 & SPELL_ATTR8_HEALING_SPELL)
                    || spellInfo->Id == 34861  // Circle of Healing
                    || spellInfo->Id == 33076  // Prayer of Mending
                    || spellInfo->Id == 88684  // Holy Word: Serenity
                    || spellInfo->Id == 110744 // Divine Star
                    || spellInfo->Id == 122121;// Divine Star

                if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)
                    if (spellInfo->Id != 47540 && spellInfo->Id != 33076) // Penance, Prayer of Mending
                        return false;

                if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS && !isHeal)
                    return false;
                if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG && spellInfo->Id != SPELL_PRIEST_SMITE)
                    return false;
                return roll_chance_i(15);
            }
            default:    // No spec - no proc
                return false;
        }
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        uint32 spellId = priest->GetTalentSpecialization() == SPEC_PRIEST_SHADOW ? SPELL_PRIEST_SURGE_OF_DARKNESS : SPELL_PRIEST_SURGE_OF_LIGHT;
        priest->CastSpell(priest, spellId, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_from_darkness_comes_light::CheckProc);
        OnProc += AuraProcFn(spell_pri_from_darkness_comes_light::HandleProc);
    }
};

// 34914 - Vampiric Touch
class spell_pri_vampiric_touch : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_touch);

    void HandleTick(AuraEffect const*)
    {
        if (Unit* priest = GetCaster())
            priest->EnergizeBySpell(priest, GetSpellInfo()->Id, CalculatePct(priest->GetMaxPower(POWER_MANA), 2), POWER_MANA);
    }

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (Unit* priest = GetCaster())
            if (Unit* dispeller = dispelInfo->GetDispeller())
                if (priest->HasAura(SEPLL_PRIEST_4P_S12_SHADOW))
                    dispeller->CastSpell(dispeller, SPELL_PRIEST_SIN_AND_PUNISHMENT, true, nullptr, nullptr, priest->GetGUID());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_vampiric_touch::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        AfterDispel += AuraDispelFn(spell_pri_vampiric_touch::HandleDispel);
    }
};

// 87160 - Surge of Darkness, 114255 - Surge of Light
class spell_pri_surge_of : public visual_stacks_controller
{
    PrepareAuraScript(spell_pri_surge_of);

    void Register() override
    {
        uint32 spellId = m_scriptSpellId == SPELL_PRIEST_SURGE_OF_LIGHT ? SPELL_PRIEST_SURGE_OF_LIGHT_DOUBLE : SPELL_PRIEST_SURGE_OF_DARKNESS_DOUBLE;
        Initialize(spellId);
        OnEffectApply += AuraEffectApplyFn(spell_pri_surge_of::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_surge_of::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// 92297 - Train of Thought
class spell_pri_train_of_thought : public AuraScript
{
    PrepareAuraScript(spell_pri_train_of_thought);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Player* priest = GetUnitOwner()->ToPlayer())
        {
            if (eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_SMITE)
                priest->ModifySpellCooldown(SPELL_PRIEST_PENANCE, -500);
            else
                priest->ModifySpellCooldown(SPELL_PRIEST_INNER_FOCUS, -5000);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_train_of_thought::HandleProc);
    }
};

// 33371 - Glyph of Mind Spike
class spell_pri_glyph_of_mind_spike : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_mind_spike);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.TriggeredBySpell() && eventInfo.TriggeredBySpell()->GetCastTime() > 0;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_glyph_of_mind_spike::CheckProc);
    }
};

// 108968 - Void Shift
class spell_pri_void_shift : public SpellScript
{
    PrepareSpellScript(spell_pri_void_shift);

    SpellCastResult CheckTarget()
    {
        if (GetExplTargetUnit() && GetExplTargetUnit()->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void SwapHealth()
    {
        if (GetHitResult() != SPELL_MISS_NONE)
            return;

        Unit* priest = GetCaster();
        Unit* target = GetHitUnit();

        float priestptc = priest->GetHealthPct();
        float targetptc = target->GetHealthPct();
        float& min = priestptc > targetptc ? targetptc : priestptc;
        min = std::max(min, 25.f);
        std::swap(priestptc, targetptc);
        for (auto&& player : { priest, target })
        {
            uint32 health = player->GetMaxHealth() / 100.f * (player == priest ? priestptc : targetptc);
            auto valtype = player->GetHealth() > health ? SPELLVALUE_BASE_POINT0 : SPELLVALUE_BASE_POINT1;
            int32 val = std::abs(int32(player->GetHealth()) - int32(health));
            priest->CastCustomSpell(118594, valtype, val, player, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_void_shift::CheckTarget);
        OnHit += SpellHitFn(spell_pri_void_shift::SwapHealth);
    }
};

// 124467 - Devouring Plague (Mastery tick)
class spell_pri_devouring_plague_shadowy_recall : public SpellScript
{
    PrepareSpellScript(spell_pri_devouring_plague_shadowy_recall);

    void HandleHit()
    {
        Unit* target = GetHitUnit();
        if (auto dummy = target->GetAuraEffect(SPELL_PRIEST_DEVOURING_PLAGUE, EFFECT_2))
            SetHitDamage(CalculatePct(GetHitDamage(), dummy->GetAmount()));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_devouring_plague_shadowy_recall::HandleHit);
    }
};

// 8122 - Psychic Scream
class spell_pri_psychic_scream : public AuraScript
{
    PrepareAuraScript(spell_pri_psychic_scream);

    void InitEffect(uint32& mask)
    {
        Unit* caster = GetCaster();
        if (caster && caster->HasAura(SPELL_PRIEST_GLYPH_OF_PSYCHIC_SCREAM))
            mask &= ~(1 << EFFECT_0);
        else
            mask &= ~(1 << EFFECT_1);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_pri_psychic_scream::InitEffect);
    }
};

// 110744, 122121 - Divine Star
class spell_pri_divine_star : public AuraScript
{
    PrepareAuraScript(spell_pri_divine_star);

    MissileData m_data;

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        uint32 spellId = GetSpellInfo()->Effects[EFFECT_1].TriggerSpell;
        BasicEvent* evt = GetUnitOwner()->m_Events.FindEvent([=](BasicEvent const* e)
        {
            SpellEvent const* se = dynamic_cast<SpellEvent const*>(e);
            return se && se->GetSpell()->GetSpellInfo()->Id == spellId;
        });
        if (evt)
            m_data.Initialize(static_cast<SpellEvent const*>(evt)->GetSpell(), 24.f);
    }

    void HandleTick(AuraEffect const* eff)
    {
        m_data.Update(getMSTime(), GetUnitOwner(), eff);
    }

public:
    MissileData* GetData() { return m_data.Initialized() ? &m_data : nullptr; }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_divine_star::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_divine_star::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 58880, 122127 - Divine Star
class spell_pri_divine_star_dummy : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_star_dummy);

    void SelectDest(SpellDestination& dest)
    {
        bool inAir = false;
        if (GetCaster()->HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_FALLING))
        {
            float z = GetCaster()->GetMap()->GetHeight(GetCaster()->GetPhaseMask(), GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ(), true);
            if (std::abs(z - GetCaster()->GetPositionZ()) > 10.0f)
                inAir = true;
        }
        dest._position = GetCaster()->GetWorldLocation();
        GetCaster()->MovePositionToFirstCollosionBySteps(dest._position, 24.0f, 0.0f, 2.0f, inAir);
        dest._position.RelocateOffset(0.0f, 6.0f);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_pri_divine_star_dummy::SelectDest, EFFECT_0, TARGET_UNK_138);
    }
};

// 110745, 122128 - Divine Star
class spell_pri_divine_star_damage_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_star_damage_heal);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        MissileData* data = nullptr;
        uint32 auraId = GetSpellInfo()->Id == SPELL_PRIEST_DIVINE_STAR_SHADOW_DAMAGE ? SPELL_PRIEST_DIVINE_STAR_SHADOW : SPELL_PRIEST_DIVINE_STAR_HOLY;
        if (Aura* trigger = GetCaster()->GetAura(auraId))
            if (spell_pri_divine_star* script = dynamic_cast<spell_pri_divine_star*>(trigger->GetScriptByName("spell_pri_divine_star")))
                data = script->GetData();

        if (!data)
        {
            targets.clear();
            return;
        }

        for (auto&& it = targets.begin(); it != targets.end();)
        {
            WorldObject* target = *it;
            if (data->CheckTarget(*it, 4.f))
                ++it;
            else
                targets.erase(it++);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_divine_star_damage_heal::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_divine_star_damage_heal::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 81749 - Atonement
class spell_pri_atonement : public AuraScript
{
    PrepareAuraScript(spell_pri_atonement);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Power Word: Solace has same mask as Holy Fire
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id != 129250;
    }

    void HandleProc(AuraEffect const* eff, ProcEventInfo& eventInfo)
    {
        if (int32 heal = CalculatePct(eventInfo.GetDamageWithoutResilience(), eff->GetAmount()))
        {
            uint32 spellId = SPELL_PRIEST_ATONEMENT_HEAL;
            if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
            {
                spellId = SPELL_PRIEST_ATONEMENT_HEAL_CRIT;
                heal /= 2; // FIXME
            }
            if (eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_SMITE && eventInfo.GetActionTarget())
                if (AuraEffect const* glyph = GetUnitOwner()->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_SMITE, EFFECT_0))
                    if (eventInfo.GetActionTarget()->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, flag128{ 0x100000 }, GetUnitOwner()->GetGUID()))
                        ApplyPct(heal, 100 - glyph->GetAmount());

            // Atonement from tirggered spells can't tirgger proc, do it in next update
            GetUnitOwner()->Schedule(Milliseconds(1), [=](Unit* self)
            {
                self->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, heal, self, true);
            });
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_atonement::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_atonement::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 81751, 94472 - Atonement, 140815, 140816 - Power Word: Solace
class spell_pri_atonement_heal : public spell_smart_heal
{
    PrepareSpellScript(spell_pri_atonement_heal);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, 1);
    }

    void HandleHit()
    {
        if (GetHitUnit() == GetCaster())
            SetHitHeal(GetHitHeal() / 2);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_atonement_heal::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_pri_atonement_heal::HandleHit);
    }
};

// 81208 - Chakra: Serenity
class spell_pri_chakra_serenity : public AuraScript
{
    PrepareAuraScript(spell_pri_chakra_serenity);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Aura* renew = eventInfo.GetActionTarget()->GetAura(SPELL_PRIEST_RENEW, GetCasterGUID()))
        {
            renew->RefreshTimers(false);
            if (GetUnitOwner()->HasAura(SPELL_PRIEST_GLYPH_OF_RENEW))
            {
                renew->SetMaxDuration(renew->GetMaxDuration() + renew->GetEffect(EFFECT_0)->GetAmplitude());
                renew->SetDuration(renew->GetMaxDuration());
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_chakra_serenity::HandleProc);
    }
};

// 112833 - Spectral Guise
class spell_pri_spectral_guise : public SpellScript
{
    PrepareSpellScript(spell_pri_spectral_guise);

    void HandleCast()
    {
        Unit* priest = GetCaster();
        for (auto&& summon : priest->GetSummons())
        {
            if (summon->GetEntry() == GetSpellInfo()->Effects[EFFECT_0].MiscValue)
            {
                summon->CastSpell(summon, SPELL_PRIEST_SPECTRAL_GUISE_STACKS, true);
                summon->CastSpell(summon, 58838, true);  // Inherit Master's Threat List
                summon->CastSpell(summon, SPELL_PRIEST_SPECTRAL_GUISE_STEALTH, true);
                priest->CastSpell(summon, 45204, true);  // Clone Me
                priest->CastSpell(summon, 41054, true);  // Copy Weapon

                if (priest->GetShapeshiftForm() == FORM_SHADOW)
                    summon->CastSpell(summon, 107903, true); // Didn't find in sinffs, may be wrong.
            }
        }
        if (!priest->GetInstanceScript() || !priest->GetInstanceScript()->IsEncounterInProgress())
            priest->CombatStop();
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pri_spectral_guise::HandleCast);
    }
};

// 119032 - Spectral Guise (Invisibility)
class spell_pri_spectral_guise_remove : public AuraScript
{
    PrepareAuraScript(spell_pri_spectral_guise_remove);


    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        std::list<TempSummon*> summons;
        GetUnitOwner()->GetSummons(summons, 59607);
        if (!summons.empty())
            summons.front()->DespawnOrUnsummon(1);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_spectral_guise_remove::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

// 119030 - Spectral Guise
class spell_pri_spectral_guise_stacks : public AuraScript
{
    PrepareAuraScript(spell_pri_spectral_guise_stacks);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !(eventInfo.GetHitMask() & PROC_EX_INTERNAL_DOT);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* priest = GetUnitOwner()->GetOwner())
            priest->RemoveAurasDueToSpell(SPELL_PRIEST_SPECTRAL_GUISE_STEALTH);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_spectral_guise_stacks::CheckProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_spectral_guise_stacks::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);

    }
};

// 527 - Purify
class spell_pri_glyph_of_purify : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_purify);

    void HandleHit()
    {
        if (GetDispelCount() && GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_PURIFY))
        {
            int32 heal = GetHitUnit()->CountPctFromMaxHealth(5);
            int32 max = GetCaster()->CountPctFromCurHealth(10); // May be not blizzlike, but to prevent healing some bosses for millions
            heal = std::min(heal, max);

            GetCaster()->CastCustomSpell(SPELL_PRIEST_GLYPH_OF_PURIFY_HEAL, SPELLVALUE_BASE_POINT0, heal, GetHitUnit(), true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_glyph_of_purify::HandleHit);
    }
};

// 528 - Dispel Magic
class spell_pri_glyph_of_dispel_magic : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_dispel_magic);

    void HandleHit()
    {
        if (GetDispelCount())
            if (GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC))
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_DAMAGE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_glyph_of_dispel_magic::HandleHit);
    }
};

// 28305, 123050 - Mana Leech
class spell_pri_mana_leech : public AuraScript
{
    PrepareAuraScript(spell_pri_mana_leech);

    uint32 m_spell = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* priest = GetUnitOwner()->GetOwner())
            GetUnitOwner()->CastSpell(priest, m_spell, true);
    }

    void Register() override
    {
        m_spell = m_scriptSpellId == 123050 ? 123051 : 34650;
        OnProc += AuraProcFn(spell_pri_mana_leech::HandleProc);
    }
};

// 78203 - Shadowy Apparitions
// 138158 - Item - Priest T15 Shadow 4P Bonus1
class spell_pri_shadowy_appartions : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowy_appartions);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        // Hotfix (2013-03-05): "Shadowy Apparitions will no longer pursue targets that are actively stealthed."
        if (eventInfo.GetActionTarget() && GetUnitOwner()->CanSeeOrDetect(eventInfo.GetActionTarget()))
            GetUnitOwner()->CastSpell(eventInfo.GetActionTarget(), SPELL_PRIEST_SHADOW_APPARITION, true, nullptr, GetEffect(EFFECT_0));
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_shadowy_appartions::HandleProc);
    }
};

// 147193 - Shadowy Apparition
class spell_pri_shadowy_appartion : public SpellScript
{
    PrepareSpellScript(spell_pri_shadowy_appartion);

    void HandleCast()
    {
        typedef std::array<uint32, 2> Visual;
        static std::map<uint32, Visual> const visual
        {
            //  race                      male   female
            { RACE_HUMAN,               { 33554, 33566 } },
            { RACE_DWARF,               { 33569, 33570 } },
            { RACE_NIGHTELF,            { 33571, 33572 } },
            { RACE_UNDEAD_PLAYER,       { 33573, 33574 } },
            { RACE_TAUREN,              { 33575, 33576 } },
            { RACE_GNOME,               { 33577, 33578 } },
            { RACE_TROLL,               { 33579, 33580 } },
            { RACE_GOBLIN,              { 33581, 33582 } },
            { RACE_BLOODELF,            { 33583, 33584 } },
            { RACE_DRAENEI,             { 33585, 33586 } },
            { RACE_WORGEN,              { 33587, 33588 } },
            { RACE_PANDAREN_NEUTRAL,    { 33589, 33590 } },
            { RACE_PANDAREN_ALLIANCE,   { 33589, 33590 } },
            { RACE_PANDAREN_HORDE,      { 33589, 33590 } },
        };

        auto it = visual.find(GetCaster()->GetRace());
        if (it != visual.end() && GetExplTargetUnit())
            GetCaster()->SendPlaySpellVisual(it->second[GetCaster()->GetGender()], GetExplTargetUnit()->GetGUID(), GetSpellInfo()->Speed);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_shadowy_appartion::HandleCast);
    }
};

// 31897 - Lightwell 
struct npc_pri_lightwell : public NullCreatureAI
{
    npc_pri_lightwell(Creature* creature)
        : NullCreatureAI(creature)
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_JUMP_DEST, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_JUMP, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 73325, true); // Leap of faith
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_OBS_MOD_HEALTH, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_ABSORB, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MECHANIC_IMMUNITY, true);
    }
};

// 64571 - Lightwell
struct npc_pri_lightwell_periodic : public npc_pri_lightwell
{
    npc_pri_lightwell_periodic(Creature* creature) : npc_pri_lightwell(creature) { }

    TaskScheduler m_schedulder;

    void InitializeAI() override
    {
        m_schedulder
            .Schedule(Seconds(1), [=](TaskContext ctx)
            {
                HealGroup();
                ctx.Repeat();
            });
    }

    void TryHeal(Unit* caster, Unit* target)
    {
        if (target->HealthBelowPct(50) && !target->HasAura(SPELL_PRIEST_LIGHTWELL_RENEW) &&
            target->IsWithinDistInMap(me, 40.0f) && target->IsWithinLOSInMap(me))
            caster->CastSpell(target, SPELL_PRIEST_LIGHTWELL_RENEW, true);
    }

    void HealGroup()
    {
        Unit* owner = me->GetOwner();
        Player* priest = owner ? owner->ToPlayer() : nullptr;
        if (!priest)
            return;

        if (priest->GetGroup())
        {
            for (auto&& member : *priest->GetGroup())
                TryHeal(priest, member);
        }
        else
            TryHeal(priest, priest);
    }

    void UpdateAI(uint32 diff) override
    {
        m_schedulder.Update(diff);
    }
};

// 59907, 126150 - Lightwell Charges
class spell_pri_lightwell_charges : public AuraScript
{
    PrepareAuraScript(spell_pri_lightwell_charges);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        SetStackAmount(GetAura()->GetMaxStackAmount());
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (TempSummon* lightwell = GetUnitOwner()->ToTempSummon())
            lightwell->UnSummon();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_lightwell_charges::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_lightwell_charges::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 60123 - Lightwell Renew
class spell_pri_lightwell_renew_trigger : public SpellScript
{
    PrepareSpellScript(spell_pri_lightwell_renew_trigger);

    void HandleHit()
    {
        if (GetCaster()->IsSummon())
            if (Unit* owner = GetCaster()->GetOwner())
                if (GetHitUnit()->IsInRaidWith(owner))
                    owner->CastSpell(GetHitUnit(), SPELL_PRIEST_LIGHTWELL_RENEW_GLYPHED, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_lightwell_renew_trigger::HandleHit);
    }
};

// 7001, 126154 - Lightwell Renew
class spell_pri_lightwell_renew : public SpellScript
{
    PrepareSpellScript(spell_pri_lightwell_renew);

    void HandleCast()
    {
        uint32 summonId = GetSpellInfo()->Id == SPELL_PRIEST_LIGHTWELL_RENEW_GLYPHED ? 31897 : 64571;
        uint32 auraId   = GetSpellInfo()->Id == SPELL_PRIEST_LIGHTWELL_RENEW_GLYPHED ? 59907 : 126150;
        std::list<TempSummon*> summons;
        GetCaster()->GetSummons(summons, summonId);
        if (!summons.empty())
            if (Aura* charges = summons.front()->GetAura(auraId))
                charges->ModStackAmount(-1);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_lightwell_renew::HandleCast);
    }
};

// 7001, 126154 - Lightwell Renew
class spell_pri_lightwell_renew_hot : public AuraScript
{
    PrepareAuraScript(spell_pri_lightwell_renew_hot);

    uint32 m_receivedDamage = 0;

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (GetSpellInfo()->Id == SPELL_PRIEST_LIGHTWELL_RENEW_GLYPHED) // Glyphed
            amount *= 1.5f;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        m_receivedDamage += eventInfo.GetDamageInfo()->GetDamage();
        if (m_receivedDamage >= GetUnitOwner()->CountPctFromMaxHealth(30))
            Remove();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_lightwell_renew_hot::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        OnProc += AuraProcFn(spell_pri_lightwell_renew_hot::HandleProc);
    }
};

struct PsychicTerrorInvalidTargetCheck
{
    PsychicTerrorInvalidTargetCheck(Unit* caster) : m_caster(caster){ }

    bool operator()(Unit const* target)
    {
        switch (target->GetCreatureType())
        {
            case CREATURE_TYPE_CRITTER:
            case CREATURE_TYPE_NON_COMBAT_PET:
            case CREATURE_TYPE_WILD_PET:
                return true;
        }

        if (target->HasBreakableByDamageCrowdControlAura())
            return true;

        if (target->HasInvisibilityAura() || target->HasStealthAura())
            return true;

        if (m_caster->HasVisionObscured(target))
            return true;

        return !m_caster->IsWithinLOSInMap(target);
    }

private:
    Unit* m_caster;
};

// 59190 - Psy Fiend
struct npc_pri_psy_fiend : public PassiveAI
{
    npc_pri_psy_fiend(Creature* creature) : PassiveAI(creature) { }

    std::set<uint64> Guids;
    std::map<uint64, uint32> Attackers;

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!me->HasUnitState(UNIT_STATE_CASTING))
        {
            std::list<Unit*> targets;
            Trinity::AnyUnfriendlyNoTotemUnitInObjectRangeCheck check{ me, me, 20.0f };
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyNoTotemUnitInObjectRangeCheck> searcher{ me, targets, check };
            me->VisitNearbyObject(20.0f, searcher);
            targets.remove_if(PsychicTerrorInvalidTargetCheck{ me });
            targets.remove_if([=](Unit const* target) { return Guids.find(target->GetGUID()) != Guids.end(); });
            if (!targets.empty())
                me->CastSpell(me, SPELL_PRIEST_PSYCHIC_TERROR);
        }

        PassiveAI::UpdateAI(diff);
    }
};

// 113792 - Psychic Terror
class spell_pri_psychic_terror : public SpellScript
{
    PrepareSpellScript(spell_pri_psychic_terror);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        PsychicTerrorInvalidTargetCheck check{ GetCaster() };
        targets.remove_if([&](WorldObject const* obj)
        {
            Unit const* target = obj->ToUnit();
            if (!target)
                return true;
            return check(target);
        });
        if (GetCaster()->GetAI())
        {
            if (auto script = dynamic_cast<npc_pri_psy_fiend*>(GetCaster()->GetAI()))
            {
                targets.remove_if([=](WorldObject const* obj) { return script->Guids.find(obj->GetGUID()) != script->Guids.end(); });
                targets.sort([=](WorldObject const* target1, WorldObject const* target2)
                {
                    auto it1 = script->Attackers.find(target1->GetGUID());
                    auto it2 = script->Attackers.find(target2->GetGUID());

                    if (it1 == script->Attackers.end())
                        return false;
                    if (it2 == script->Attackers.end())
                        return true;
                    return it1->second > it2->second;
                });
            }
        }
        if (!targets.empty())
            targets.resize(1);
    }

    void HandleHit()
    {
        if (GetCaster()->GetAI())
            if (auto script = dynamic_cast<npc_pri_psy_fiend*>(GetCaster()->GetAI()))
                script->Guids.insert(GetHitUnit()->GetGUID());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_psychic_terror::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_pri_psychic_terror::HandleHit);
    }
};

// 113792 - Psychic Terror
class spell_pri_psychic_terror_aura : public AuraScript
{
    PrepareAuraScript(spell_pri_psychic_terror_aura);

    void InitEffects(uint32& effectMask)
    {
        if (Unit* psyfiend = GetCaster())
            if (Unit* priest = psyfiend->GetOwner())
                if (!priest->HasAura(SPELL_PRIEST_GLYPH_OF_PSYCHIC_SCREAM))
                    effectMask &= ~(1 << EFFECT_1);
    }

    void Register() override
    {
        OnInitEffects += AuraInitEffectsFn(spell_pri_psychic_terror_aura::InitEffects);
    }
};


// 114164 - Psyfiend Hit Me Driver
class spell_pri_psyfiend_hit_me_driver : public AuraScript
{
    PrepareAuraScript(spell_pri_psyfiend_hit_me_driver);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActor())
            return;

        uint32 damage = eventInfo.GetProcTriggerContext()->UnmitigatedDamage;
        std::list<TempSummon*> summons;
        GetUnitOwner()->GetSummons(summons, 59190);
        if (!summons.empty())
            if (TempSummon* psyfiend = summons.front())
                if (auto script = dynamic_cast<npc_pri_psy_fiend*>(psyfiend->AI()))
                    script->Attackers[eventInfo.GetActor()->GetGUID()] += damage;
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_psyfiend_hit_me_driver::HandleProc);
    }
};
    
// 48045 - Mind Sear
class spell_pri_mind_sear_check : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_sear_check);

    SpellCastResult CheckCast()
    {
        if (GetExplTargetUnit() == GetCaster())
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_mind_sear_check::CheckCast);
    }
};

// 49821 - Mind Sear
class spell_pri_mind_sear : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_sear);

    void HandleHit()
    {
        if (AuraEffect const* mastery = GetCaster()->GetAuraEffect(SPELL_PRIEST_MASTERY_SHADOWY_RECALL, EFFECT_0))
            if (roll_chance_f(mastery->GetFloatAmount()))
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_MIND_SEAR_SR, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_mind_sear::HandleHit);
    }
};

// 120517, 120644 - Halo
class spell_pri_halo : public SpellScript
{
    PrepareSpellScript(spell_pri_halo);

    void HandleCast()
    {
        uint32 spellId  = GetSpellInfo()->Id == 120517 ? 120630 : 120643; // Visual, Holy - Shadow
        GetCaster()->CastSpell(GetCaster(), spellId, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_halo::HandleCast);
    }
};

// 120630, 120643 - Halo
class spell_pri_halo_visual : public SpellScript
{
    PrepareSpellScript(spell_pri_halo_visual);

    void SelectDest(SpellDestination& dest)
    {
        /*
        local radius = (progress * 35)

        local curAngle = progress * 0
        if (missileIndex > 0) then
            local anglePerMissile = 360 / (missileCount)
            curAngle = curAngle + anglePerMissile * missileIndex
        end

        transFront = (sin(curAngle) * radius) - distanceToFirePos
        transRight = (cos(curAngle) * radius)
        scale = math.max(0.001, progress*2.2)
        */
        dest._position.RelocateOffset(0.0f, 35.0f);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_pri_halo_visual::SelectDest, EFFECT_0, TARGET_DEST_CASTER);
    }
};

// 120692, 120696 - Halo
class spell_pri_halo_effect : public SpellScript
{
    PrepareSpellScript(spell_pri_halo_effect);

    void ResolveHeal(SpellEffIndex eff)
    {
        if (GetCaster()->IsValidAttackTarget(GetHitUnit(), GetSpellInfo()))
            PreventEffect(eff);
    }

    void ResolveDamge(SpellEffIndex eff)
    {
        if (!GetCaster()->IsValidAttackTarget(GetHitUnit(), GetSpellInfo()))
            PreventEffect(eff);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_halo_effect::ResolveHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_halo_effect::ResolveDamge, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 657, 658
struct sat_pri_halo : public IAreaTriggerAura
{
    std::set<uint64> m_targets;

    bool CheckTriggering(WorldObject* object) override
    {
        if (!object->ToUnit() || !object->ToUnit()->IsAlive())
            return false;
        if (std::abs(object->GetPositionZ() - GetTarget()->GetPositionZ()) > 10.0f)
            return false;

        float dist = (5500 - GetAreaTrigger()->GetDuration()) * 10.f / 1000.0f;
        dist = std::min(dist, 30.0f);
        if (GetTarget()->GetExactDist2d(object) - object->GetObjectSize() > dist)
            return false;

        if (!GetCaster()->CanSeeOrDetect(object))
            return false;
        float x, y, z;
        GetTarget()->GetPosition(x, y, z);
        z += 5.0f;
        if (!object->IsWithinLOS(x, y, z))
            return false;
        return m_targets.find(object->GetGUID()) == m_targets.end();
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        m_targets.insert(object->GetGUID());
        Unit* target = object->ToUnit();

        uint32 spellId = GetAreaTrigger()->GetSpellId() == 120517 ? 120692 : 120696; // Holy - Shadow

        float dist = std::max(GetTarget()->GetExactDist2d(object), 1.5f);   // Confirmed that halo heals priest, but the amount is unknown
        float coeff = dist / 25.0f;
        coeff = std::min(coeff, 1.0f);

        CustomSpellValues ctx;
        ctx.SetMultiplier(coeff);
        GetCaster()->CastCustomSpell(spellId, ctx, target, TRIGGERED_FULL_MASK);
    }
};

// 1489. Summoned by 62618 - Power Word : Barrier
class sat_priest_power_word_barrier : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        Unit* unit = triggering->ToUnit();
        return unit && unit->IsFriendlyTo(m_caster);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        triggering->ToUnit()->CastSpell(triggering->ToUnit(), SPELL_PRIEST_POWER_WORD_BARRIER_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            target->RemoveAurasDueToSpell(SPELL_PRIEST_POWER_WORD_BARRIER_EFF);
    }
};

// 33110 - Prayer of Mending
class spell_pri_glyph_of_prayer_of_mending : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_prayer_of_mending);

    void HandleHit()
    {
        Unit* priest = GetOriginalCaster();
        if (!priest)
            return;

        if (AuraEffect const* glyph = priest->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_MENDING, EFFECT_0))
            for (auto&& effect : GetHitUnit()->GetAuraEffectsByType(SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE))
                if (effect->GetCasterGUID() == priest->GetGUID())
                    if (effect->GetBase()->GetCharges() == 4)
                        SetHitHeal(GetHitHeal() * (1.0f + glyph->GetAmount() / 100.f));

        if (AuraEffect const* sanctuary = priest->GetAuraEffect(SPELL_PRIEST_CHAKRA_SANCTUARY, EFFECT_0))
            SetHitHeal(GetHitHeal() * (1.0f + sanctuary->GetAmount() / 100.0f));

        if (auto const* targetInfo = GetSpell()->GetTargetInfo(GetHitUnit()->GetGUID()))
        {
            if (targetInfo->crit)
            {
                if (AuraEffect const* aegis = priest->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS, EFFECT_0))
                {
                    uint32 heal = GetHitHeal();
                    // Manually apply pvp modifiers
                    priest->ApplyPvPHealMods(GetHitUnit(), heal, GetSpellInfo());

                    HandleDivineAegis(priest, GetHitUnit(), heal, aegis);
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_glyph_of_prayer_of_mending::HandleHit);
    }
};

// 121135, 127632 - Cascade
class spell_pri_cascade : public SpellScript
{
    PrepareSpellScript(spell_pri_cascade);

    SpellCastResult CheckCast()
    {
        if (GetExplTargetUnit())
        {
            bool positive = GetCaster()->IsValidAssistTarget(GetExplTargetUnit());
            bool negative = GetCaster()->IsValidAttackTarget(GetExplTargetUnit(), sSpellMgr->GetSpellInfo(SPELL_PRIEST_CASCADE_HOLY_DAMAGE));

            if (!positive && !negative)
                return SPELL_FAILED_BAD_TARGETS;
        }
        return SPELL_CAST_OK;
    }

    void HandleHit()
    {
        uint32 spellId = 0;
        uint32 cooldown = 0;
        bool positive = GetCaster()->IsValidAssistTarget(GetHitUnit());
        if (GetSpellInfo()->Id == SPELL_PRIEST_CASCADE_HOLY)
        {
            spellId = positive ? SPELL_PRIEST_CASCADE_HOLY_MISSILE : SPELL_PRIEST_CASCADE_HOLY_DAMAGE;
            cooldown = SPELL_PRIEST_CASCADE_HOLY_COOLDOWN;
        }
        else
        {
            spellId = positive ? SPELL_PRIEST_CASCADE_SHADOW_MISSILE : SPELL_PRIEST_CASCADE_SHADOW_DAMAGE;
            cooldown = SPELL_PRIEST_CASCADE_SHADOW_COOLDOWN;
        }

        GetCaster()->CastSpell(GetHitUnit(), cooldown, true);
        GetCaster()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT1, 4, GetHitUnit(), true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_cascade::CheckCast);
        OnHit += SpellHitFn(spell_pri_cascade::HandleHit);
    }
};

// 121146, 127627, 120785, 127628 - Cascade
class spell_pri_cascade_missile : public SpellScript
{
    PrepareSpellScript(spell_pri_cascade_missile);

    void HandleHit()
    {
        Unit* priest = GetOriginalCaster();
        if (!priest)
            return;

        bool ligth = GetSpellInfo()->Id == SPELL_PRIEST_CASCADE_HOLY_MISSILE || GetSpellInfo()->Id == SPELL_PRIEST_CASCADE_HOLY_DAMAGE;
        uint32 heal = ligth ? SPELL_PRIEST_CASCADE_HOLY_HEAL : SPELL_PRIEST_CASCADE_SHADOW_HEAL;
        if (GetSpellInfo()->Id == SPELL_PRIEST_CASCADE_HOLY_MISSILE || GetSpellInfo()->Id == SPELL_PRIEST_CASCADE_SHADOW_MISSILE)
            GetHitUnit()->CastSpell(GetHitUnit(), heal, true, nullptr, nullptr, priest->GetGUID());

        uint32 selector = ligth ? SPELL_PRIEST_CASCADE_HOLY_SELECTOR : SPELL_PRIEST_CASCADE_SHADOW_SELECTOR;
        if (Aura const* aura = GetHitAura())
        {
            if (AuraEffect const* eff = aura->GetEffect(EFFECT_1))
            {
                if (!eff->GetAmount())
                    return;
                // Each bolt splits to two bolts
                GetHitUnit()->CastCustomSpell(selector, SPELLVALUE_BASE_POINT1, eff->GetAmount(), GetHitUnit(), true, nullptr, eff, priest->GetGUID());
                GetHitUnit()->CastCustomSpell(selector, SPELLVALUE_BASE_POINT1, eff->GetAmount(), GetHitUnit(), true, nullptr, eff, priest->GetGUID());
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_cascade_missile::HandleHit);
    }
};

// 120786, 127630 - Cascade
class spell_pri_cascade_selector : public SpellScript
{
    PrepareSpellScript(spell_pri_cascade_selector);

    int32 m_counter = 0;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        Unit* priest = GetOriginalCaster();
        m_counter = GetSpellValue()->EffectBasePoints[EFFECT_1];
        auto trigger = GetSpell()->TriggeredByAuraSpell();
        if (!m_counter || !trigger || !priest)
        {
            targets.clear();
            return;
        }
        --m_counter;
        bool heal = trigger->Id == SPELL_PRIEST_CASCADE_HOLY_MISSILE || trigger->Id == SPELL_PRIEST_CASCADE_SHADOW_MISSILE;
        uint32 cooldown = trigger->Id == SPELL_PRIEST_CASCADE_HOLY_MISSILE || trigger->Id == SPELL_PRIEST_CASCADE_HOLY_DAMAGE ?
            SPELL_PRIEST_CASCADE_HOLY_COOLDOWN : SPELL_PRIEST_CASCADE_SHADOW_COOLDOWN;
        targets.remove_if([=](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            if (!unit)
                return true;
            if (unit->HasAura(cooldown, priest->GetGUID()))
                return true;
            if (Unit const* unit = target->ToUnit())
                if (TempSummon const* summon = unit->ToTempSummon())
                    if (!summon->IsGuardian() || static_cast<Guardian const*>(summon)->IsMinorGuardian())
                        return true;

            if (!GetCaster()->IsWithinLOSInMap(unit))
                return true;

            return heal ? !priest->_IsValidAssistTarget(unit, sSpellMgr->GetSpellInfo(SPELL_PRIEST_CASCADE_HOLY_HEAL)) :
                !priest->IsValidAttackTarget(unit, sSpellMgr->GetSpellInfo(SPELL_PRIEST_CASCADE_HOLY_DAMAGE));
        });

        if (!targets.empty())
        {
            targets.sort(Trinity::ObjectDistanceOrderPred{ GetCaster(), false });
            targets.resize(1);
        }
    }

    void HandleHit(SpellEffIndex)
    {
        auto trigger = GetSpell()->TriggeredByAuraSpell();
        Unit* priest = GetOriginalCaster();
        uint32 cooldown = trigger->Id == SPELL_PRIEST_CASCADE_HOLY_MISSILE || trigger->Id  == SPELL_PRIEST_CASCADE_HOLY_DAMAGE ?
            SPELL_PRIEST_CASCADE_HOLY_COOLDOWN : SPELL_PRIEST_CASCADE_SHADOW_COOLDOWN;

        priest->CastSpell(GetHitUnit(), cooldown, true);
        GetCaster()->CastCustomSpell(trigger->Id, SPELLVALUE_BASE_POINT1, m_counter, GetHitUnit(), true, nullptr, nullptr, priest->GetGUID());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_cascade_selector::SelectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_cascade_selector::HandleHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 33076, 123259 - Prayer of Mending
class spell_pri_prayer_of_mending : public SpellScript
{
    PrepareSpellScript(spell_pri_prayer_of_mending);

    void CalculateHeal(SpellEffIndex)
    {
        int32 heal = GetEffectValue();
        heal = GetCaster()->SpellHealingBonusDone(GetHitUnit(), GetSpellInfo(), EFFECT_0, heal, HEAL, 1);
        SetEffectValue(heal);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_prayer_of_mending::CalculateHeal, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE);
    }
};

// 33076, 123259 - Prayer of Mending
class spell_pri_t15_healer_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_pri_t15_healer_2p_bonus);

    void CalculateHeal(SpellEffIndex)
    {
        Unit* priest = GetOriginalCaster();
        if (!priest)
            return;

        if (!GetTriggeringSpell() || GetTriggeringSpell()->Id != GetSpellInfo()->Id)
            return;

        int32 heal = GetEffectValue();
        if (AuraEffect const* setBonus = priest->GetAuraEffect(SPELL_PRIEST_T15_HEALER_2P_BONUS, EFFECT_0))
            AddPct(heal, setBonus->GetAmount());

        GetSpell()->SetSpellValue(SPELLVALUE_BASE_POINT0, heal);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_t15_healer_2p_bonus::CalculateHeal, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 138301 - Item - Priest T15 Healer 4P Bonus
class spell_pri_t15_healer_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t15_healer_4p_bonus);

    bool CheckProc(ProcEventInfo& evnetInfo)
    {
        if (evnetInfo.GetSpellInfo()->Id != SPELL_PRIEST_PENANCE_HEAL &&
            evnetInfo.GetSpellInfo()->Id != SPELL_PRIEST_PENANCE_DAMAGE &&
            evnetInfo.GetSpellInfo()->Id != SPELL_PRIEST_CIRCLE_OF_HEALING)
            return false;

        // Chance is overridden in DB, becasue we can't filter this spells by spell family flags
        // and this handler calls after chance roll
        return roll_chance_i(40);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_t15_healer_4p_bonus::CheckProc);
    }
};

// 148859 - Shadowy Apparition
class spell_pri_t15_shadow_2p_bonus : public SpellScript
{
    PrepareSpellScript(spell_pri_t15_shadow_2p_bonus);

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_PRIEST_T15_SHADOW_2P_BONUS);
    }

    void HandleHit()
    {
        AuraEffect const* setBonus = GetCaster()->GetAuraEffect(SPELL_PRIEST_T15_SHADOW_2P_BONUS, EFFECT_0);
        if (!setBonus)
            return;

        if (!roll_chance_i(setBonus->GetAmount()))
            return;

        auto const& effects = GetHitUnit()->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
        for (auto&& effect : effects)
        {
            if (effect->GetCasterGUID() == GetCaster()->GetGUID())
            {
                if (effect->GetId() == SPELL_PRIEST_SHADOW_WORD_PAIN || effect->GetId() == SPELL_PRIEST_VAMPIRIC_TOUCH)
                {
                    int32 tickDuration = effect->GetAmplitude();
                    Aura* aura = effect->GetBase();
                    if (aura->GetMaxDuration() > aura->GetDuration() + tickDuration)
                        aura->SetMaxDuration(aura->GetMaxDuration() + tickDuration);
                    aura->SetDuration(aura->GetDuration() + tickDuration);
                }
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_t15_shadow_2p_bonus::HandleHit);
    }
};

// 145306 - Item - Priest T16 Healer 2P Bonus
class spell_pri_t16_healer_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t16_healer_2p_bonus);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_ARCHANGEL)
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_PIOUS_HEALER, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_t16_healer_2p_bonus::HandleProc);
    }
};

// 145334 - Item - Priest T16 Healer 4P Bonus
class spell_pri_t16_healer_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t16_healer_4p_bonus);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        switch (eventInfo.GetSpellInfo()->Id)
        {
            case SPELL_PRIEST_CIRCLE_OF_HEALING:
            case SPELL_PRIEST_PRAYER_OF_MEINDING:
            case SPELL_PRIEST_PRAYER_OF_MENDING_DIVINE_INSIGHT:
                if (GetUnitOwner()->ToPlayer()->GetSpecialization() == SPEC_PRIEST_HOLY)
                    GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_ABSOLUTION, true);
                break;
            case SPELL_PRIEST_SPIRIT_SHELL:
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_RESOLUTE_SPIRIT, true);
                break;
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_t16_healer_4p_bonus::HandleProc);
    }
};

// 145336 - Absolution
class spell_pri_absolution : public AuraScript
{
    PrepareAuraScript(spell_pri_absolution);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.TriggeredBySpell())
            return;
        if (eventInfo.TriggeredBySpell()->m_appliedMods.find(GetAura()) != eventInfo.TriggeredBySpell()->m_appliedMods.end())
            Remove();
        if (eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_HOLY_WORD_SANCTUARY_AREA)
            Remove();
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_absolution::HandleProc);
    }
};

// 145179 - Item - Priest T16 Shadow 4P Bonus
class spell_pri_t16_shadow_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t16_shadow_4p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTriggerContext()->TriggeredBySpell && eventInfo.GetProcTriggerContext()->TriggeredBySpell->GetPowerType() == POWER_SHADOW_ORBS;
    }

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 orbs = GetUnitOwner()->GetPower(POWER_SHADOW_ORBS) + 1;
        GetUnitOwner()->CastCustomSpell(effect->GetSpellEffectInfo().TriggerSpell, SPELLVALUE_BASE_POINT0, orbs * effect->GetAmount(), GetUnitOwner(), true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_t16_shadow_4p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_t16_shadow_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 585 - Smite
class spell_pri_glyph_of_smite : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_smite);

    void HandleHit()
    {
        if (AuraEffect const* glyph = GetCaster()->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_SMITE, EFFECT_0))
        {
            if (GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, flag128{ 0x100000 }, GetCaster()->GetGUID()))
            {
                int32 damage = GetHitDamage();
                AddPct(damage, glyph->GetAmount());
                SetHitDamage(damage);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_glyph_of_smite::HandleHit);
    }
};

// 121536 - Angelic Feather
class spell_pri_angelic_feather : public SpellScript
{
    PrepareSpellScript(spell_pri_angelic_feather);

    void HandleCast()
    {
        std::list<AreaTrigger*> feathers;
        GetCaster()->GetAreaTriggerList(feathers, GetSpellInfo()->Id);
        while (feathers.size() > 2)
        {
            feathers.front()->Remove();
            feathers.pop_front();
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_angelic_feather::HandleCast);
    }
};

// 337
class sat_pri_angelic_feather : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* target)
    {
        return target->GetTypeId() == TYPEID_PLAYER && GetCaster()->IsValidAssistTarget(target->ToPlayer());
    }

    void OnTrigger(WorldObject* target)
    {
        target->ToUnit()->CastSpell(target->ToUnit(), SPELL_PRIEST_ANGELIC_FEATHER_SPEED, true);
    }
};

// 588 - Inner Fire
class spell_pri_glyph_of_inner_sanctum : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_inner_sanctum);

    void HandleCast()
    {
        if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_INNER_SANCTUM, EFFECT_0))
            GetCaster()->CastCustomSpell(SPELL_PRIEST_SPELL_WARDING, SPELLVALUE_BASE_POINT0, eff->GetAmount(), GetCaster(), true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_glyph_of_inner_sanctum::HandleCast);
    }
};

// 73325 - Leap of Faith
class spell_pri_glyph_of_leap_of_faith : public SpellScript
{
    PrepareSpellScript(spell_pri_glyph_of_leap_of_faith);

    void HandleHit()
    {
        if (GetCaster()->HasSpell(SPELL_PRIEST_GLYPH_OF_LEAP_OF_FAITH))
            GetHitUnit()->RemoveMovementImpairingAuras();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_glyph_of_leap_of_faith::HandleHit);
    }
};

// 19236 - Desperate Prayer
class spell_pri_desperate_prayer : public SpellScript
{
    PrepareSpellScript(spell_pri_desperate_prayer);

    void HandleHit()
    {
        // Apply Battle Fatigue anyway...
        if (GetHitUnit()->HasAura(134735) || GetHitUnit()->GetBattlegorund())
        {
            int32 heal = GetHitHeal();
            AddPct(heal, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));
            SetHitHeal(heal);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_desperate_prayer::HandleHit);
    }
};

// 34861 - Circle of Healing
class spell_pri_circle_of_healing : public spell_smart_heal
{
    PrepareSpellScript(spell_pri_circle_of_healing);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::NotInRaidWithCheck{ GetCaster() });
        uint32 count = GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING) ? 6 : 5;
        ApplySmartHeal(targets, count);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_circle_of_healing::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 32546 - Binding Heal
class spell_pri_binding_heal : public spell_smart_heal
{
    PrepareSpellScript(spell_pri_binding_heal);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        auto explicitTarget = GetExplTargetUnit();
        if (!GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_BINDING_HEAL))
        {
            targets.clear();
            targets.push_back(explicitTarget);
            return;
        }

        targets.remove(explicitTarget);
        targets.remove(GetCaster());

        ApplySmartHeal(targets, 1);

        targets.push_back(explicitTarget);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_binding_heal::SelectTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 6346 - Fear Ward
class spell_pri_fear_ward : public AuraScript
{
    PrepareAuraScript(spell_pri_fear_ward);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        uint32 const mechanic = (1 << MECHANIC_FEAR) | (1 << MECHANIC_TURN);
        if (eventInfo.GetSpellInfo())
            for (auto&& itr : eventInfo.GetSpellInfo()->Effects)
                if (itr.ApplyAuraName == SPELL_AURA_MOD_FEAR || itr.ApplyAuraName == SPELL_AURA_MOD_FEAR_2)
                    return eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & mechanic;
        return false;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        // Shit in Unit::ProcDamageAndSpellFor
        // if (procSpell && procSpell->Mechanic == uint32(triggeredByAura->GetMiscValue()))
        //    takeCharges = true;
        // I don't wan't change it for one spell
        Remove();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_fear_ward::CheckProc);
        OnProc += AuraProcFn(spell_pri_fear_ward::HandleProc);
    }
};

// 111758 - Levitate
class spell_pri_glyph_of_heavens : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_heavens);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* priest = GetCaster())
            if (priest->HasAura(SPELL_PRIEST_GLYPH_OF_THE_HEAVENS))
                GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_GLYPH_OF_THE_HEAVENS_VISUAL, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_GLYPH_OF_THE_HEAVENS_VISUAL);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_glyph_of_heavens::HandleApply, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_pri_glyph_of_heavens::HandleRemove, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 111758 - Levitate
class spell_pri_glyph_of_levitate : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_levitate);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner() == GetCaster() && GetUnitOwner()->HasAura(SPELL_PREIST_GLYPH_OF_LEVITATE))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_PATH_OF_THE_DEVOUT, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Aura* aura = GetUnitOwner()->GetAura(SPELL_PRIEST_PATH_OF_THE_DEVOUT))
        {
            aura->SetMaxDuration(10 * IN_MILLISECONDS);
            aura->SetDuration(10 * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_glyph_of_levitate::HandleApply, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_pri_glyph_of_levitate::HandleRemove, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 33076 - Prayer of Mending
class spell_pri_prayer_of_mending_proc : public AuraScript
{
    PrepareAuraScript(spell_pri_prayer_of_mending_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetHitMask() & PROC_EX_ABSORB)
            if (!eventInfo.GetDamage())
                return false;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_prayer_of_mending_proc::CheckProc);
    }
};

// 81209 - Chakra: Chastise
class spell_pri_chakra_chastise : public AuraScript
{
    PrepareAuraScript(spell_pri_chakra_chastise);

    void HandleProc(ProcEventInfo&)
    {
        if (Player* priest = GetUnitOwner()->ToPlayer())
            priest->RemoveSpellCooldown(SPELL_PRIEST_HOLY_WORD_CHASTISE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_pri_chakra_chastise::HandleProc);
    }
};

// 64901 - Hymn of Hope
// 64843 - Divine Hymn
class spell_pri_glyph_of_inspired_hymns : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_inspired_hymns);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetUnitOwner()->HasAura(SPELL_PRIEST_GLYPH_OF_INSPIRED_HYMNS))
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_PRIEST_GLYPH_OF_INSPIRED_HYMNS_VISUAL, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_PRIEST_GLYPH_OF_INSPIRED_HYMNS_VISUAL);
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn(spell_pri_glyph_of_inspired_hymns::HandleApply,  EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_pri_glyph_of_inspired_hymns::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_priest_spell_scripts()
{
    new spell_pri_power_word_fortitude();
    new spell_script<spell_pri_void_tendrils>("spell_pri_void_tendrils");
    new aura_script<spell_pri_void_tendrils_grasp>("spell_pri_void_tendrils_grasp");
    new spell_pri_phantasm_proc();
    new spell_script<spell_pri_item_s12_2p_heal>("spell_pri_item_s12_2p_heal");
    new aura_script<spell_pri_holy_spark>("spell_pri_holy_spark");
    new spell_pri_item_s12_2p_shadow();
    new spell_script<spell_pri_power_word_solace>("spell_pri_power_word_solace");
    new aura_script<spell_pri_solace_and_insanity>("spell_pri_solace_and_insanity");
    new spell_pri_shadowfiend();
    new spell_pri_strength_of_soul();
    new spell_script<spell_pri_spirit_shell>("spell_pri_spirit_shell");
    new spell_pri_devouring_plague();
    new spell_pri_phantasm();
    new spell_pri_inner_fire_or_will();
    new spell_script<spell_pri_leap_of_faith>("spell_pri_leap_of_faith");
    new spell_script<spell_pri_psychic_horror>("spell_pri_psychic_horror");
    new spell_pri_guardian_spirit();
    new spell_script<spell_pri_penance>("spell_pri_penance");
    new spell_pri_prayer_of_mending_heal();
    new aura_script<spell_pri_rapid_renewal>("spell_pri_rapid_renewal");
    new aura_script<spell_pri_shadowform>("spell_pri_shadowform");
    new aura_script<spell_pri_glyph_of_shadow>("spell_pri_glyph_of_shadow");
    new aura_script<spell_pri_evangelism>("spell_pri_evangelism");
    new spell_script<spell_pri_archangel>("spell_pri_archangel");
    new spell_pri_levitate();
    new aura_script<spell_pri_divine_aegis>("spell_pri_divine_aegis");
    new aura_script<spell_pri_power_word_shield>("spell_pri_power_word_shield");
    new spell_pri_mass_dispel();
    new spell_pri_mind_blast();
    new spell_pri_confession();
    new spell_pri_glyph_of_confession();
    new spell_script<spell_pri_devouring_plague>("spell_pri_devouring_plague");
    new aura_script<spell_pri_devouring_plague_heal>("spell_pri_devouring_plague_heal");
    new aura_script<spell_pri_twist_of_fate>("spell_pri_twist_of_fate");
    new aura_script<spell_pri_echo_of_light>("spell_pri_echo_of_light");
    new aura_script<spell_pri_holy_word_sanctuary>("spell_pri_holy_word_sanctuary");
    new aura_script<spell_pri_shadowy_recall>("spell_pri_shadowy_recall");
    new aura_script<spell_pri_spirit_of_redemption>("spell_pri_spirit_of_redemption");
    new aura_script<spell_pri_spirit_of_redemption_form>("spell_pri_spirit_of_redemption_form");
    new spell_script<spell_pri_shadow_word_death>("spell_pri_shadow_word_death");
    new spell_script<spell_pri_shadow_word_death_glyphed>("spell_pri_shadow_word_death_glyphed");
    new aura_script<spell_pri_shadow_orbs>("spell_pri_shadow_orbs");
    new aura_script<spell_pri_angelic_bulwark>("spell_pri_angelic_bulwark");
    new aura_script<spell_pri_divine_insight_talent>("spell_pri_divine_insight_talent");
    new spell_script<spell_pri_divine_insight_shadow>("spell_pri_divine_insight_shadow");
    new spell_script<spell_pri_divine_insight_holy>("spell_pri_divine_insight_holy");
    new aura_script<spell_pri_prayer_of_mending_divine_insight>("spell_pri_prayer_of_mending_divine_insight");
    new aura_script<spell_pri_focused_will>("spell_pri_focused_will");
    new spell_script<spell_pri_mind_fly_selector>("spell_pri_mind_fly_selector");
    new aura_script<spell_pri_mind_flay>("spell_pri_mind_flay");
    new aura_script<spell_pri_mind_flay_insanity>("spell_pri_mind_flay_insanity");
    new spell_script<spell_pri_mind_flay_shadowy_recall>("spell_pri_mind_flay_shadowy_recall");
    new aura_script<spell_pri_vampiric_embrace>("spell_pri_vampiric_embrace");
    new spell_script<spell_pri_vampiric_embrace_heal>("spell_pri_vampiric_embrace_heal");
    new spell_script<spell_pri_mind_spike>("spell_pri_mind_spike");
    new spell_script<spell_pri_surge_of_light_consumer>("spell_pri_surge_of_light_consumer");
    new aura_script<spell_pri_from_darkness_comes_light>("spell_pri_from_darkness_comes_light");
    new aura_script<spell_pri_vampiric_touch>("spell_pri_vampiric_touch");
    new aura_script<spell_pri_surge_of>("spell_pri_surge_of");
    new aura_script<spell_pri_train_of_thought>("spell_pri_train_of_thought");
    new aura_script<spell_pri_glyph_of_mind_spike>("spell_pri_glyph_of_mind_spike");
    new spell_script<spell_pri_void_shift>("spell_pri_void_shift");
    new spell_script<spell_pri_devouring_plague_shadowy_recall>("spell_pri_devouring_plague_shadowy_recall");
    new aura_script<spell_pri_psychic_scream>("spell_pri_psychic_scream");
    new aura_script<spell_pri_divine_star>("spell_pri_divine_star");
    new spell_script<spell_pri_divine_star_dummy>("spell_pri_divine_star_dummy");
    new spell_script<spell_pri_divine_star_damage_heal>("spell_pri_divine_star_damage_heal");
    new aura_script<spell_pri_atonement>("spell_pri_atonement");
    new spell_script<spell_pri_atonement_heal>("spell_pri_atonement_heal");
    new aura_script<spell_pri_chakra_serenity>("spell_pri_chakra_serenity");
    new spell_script<spell_pri_spectral_guise>("spell_pri_spectral_guise");
    new aura_script<spell_pri_spectral_guise_remove>("spell_pri_spectral_guise_remove");
    new aura_script<spell_pri_spectral_guise_stacks>("spell_pri_spectral_guise_stacks");
    new spell_script<spell_pri_glyph_of_purify>("spell_pri_glyph_of_purify");
    new spell_script<spell_pri_glyph_of_dispel_magic>("spell_pri_glyph_of_dispel_magic");
    new aura_script<spell_pri_mana_leech>("spell_pri_mana_leech");
    new aura_script<spell_pri_shadowy_appartions>("spell_pri_shadowy_appartions");
    new spell_script<spell_pri_shadowy_appartion>("spell_pri_shadowy_appartion");
    new creature_script<npc_pri_lightwell>("npc_pri_lightwell");
    new creature_script<npc_pri_lightwell_periodic>("npc_pri_lightwell_periodic");
    new aura_script<spell_pri_lightwell_charges>("spell_pri_lightwell_charges");
    new spell_script<spell_pri_lightwell_renew_trigger>("spell_pri_lightwell_renew_trigger");
    new spell_script<spell_pri_lightwell_renew>("spell_pri_lightwell_renew");
    new aura_script<spell_pri_lightwell_renew_hot>("spell_pri_lightwell_renew_hot");
    new creature_script<npc_pri_psy_fiend>("npc_pri_psy_fiend");
    new spell_script<spell_pri_psychic_terror>("spell_pri_psychic_terror");
    new aura_script<spell_pri_psychic_terror_aura>("spell_pri_psychic_terror_aura");
    new aura_script<spell_pri_psyfiend_hit_me_driver>("spell_pri_psyfiend_hit_me_driver");
    new spell_script<spell_pri_mind_sear_check>("spell_pri_mind_sear_check");
    new spell_script<spell_pri_mind_sear>("spell_pri_mind_sear");
    new spell_script<spell_pri_halo>("spell_pri_halo");
    new spell_script<spell_pri_halo_visual>("spell_pri_halo_visual");
    new spell_script<spell_pri_halo_effect>("spell_pri_halo_effect");
    new atrigger_script<sat_pri_halo>("sat_pri_halo");
    new atrigger_script<sat_priest_power_word_barrier>("sat_priest_power_word_barrier");
    new spell_script<spell_pri_glyph_of_prayer_of_mending>("spell_pri_glyph_of_prayer_of_mending");
    new spell_script<spell_pri_cascade>("spell_pri_cascade");
    new spell_script<spell_pri_cascade_missile>("spell_pri_cascade_missile");
    new spell_script<spell_pri_cascade_selector>("spell_pri_cascade_selector");
    new spell_script<spell_pri_prayer_of_mending>("spell_pri_prayer_of_mending");
    new spell_script<spell_pri_t15_healer_2p_bonus>("spell_pri_t15_healer_2p_bonus");
    new aura_script<spell_pri_t15_healer_4p_bonus>("spell_pri_t15_healer_4p_bonus");
    new spell_script<spell_pri_t15_shadow_2p_bonus>("spell_pri_t15_shadow_2p_bonus");
    new aura_script<spell_pri_shadowy_appartions>("spell_pri_t15_shadow_p4_bonus");
    new aura_script<spell_pri_t16_healer_2p_bonus>("spell_pri_t16_healer_2p_bonus");
    new aura_script<spell_pri_t16_healer_4p_bonus>("spell_pri_t16_healer_4p_bonus");
    new aura_script<spell_pri_absolution>("spell_pri_absolution");
    new aura_script<spell_pri_t16_shadow_4p_bonus>("spell_pri_t16_shadow_4p_bonus");
    new spell_script<spell_pri_glyph_of_smite>("spell_pri_glyph_of_smite");
    new spell_script<spell_pri_angelic_feather>("spell_pri_angelic_feather");
    new atrigger_script<sat_pri_angelic_feather>("sat_pri_angelic_feather");
    new spell_script<spell_pri_glyph_of_inner_sanctum>("spell_pri_glyph_of_inner_sanctum");
    new spell_script<spell_pri_glyph_of_leap_of_faith>("spell_pri_glyph_of_leap_of_faith");
    new spell_script<spell_pri_desperate_prayer>("spell_pri_desperate_prayer");
    new spell_script<spell_pri_circle_of_healing>("spell_pri_circle_of_healing");
    new spell_script<spell_pri_binding_heal>("spell_pri_binding_heal");
    new aura_script<spell_pri_fear_ward>("spell_pri_fear_ward");
    new aura_script<spell_pri_glyph_of_heavens>("spell_pri_glyph_of_heavens");
    new aura_script<spell_pri_glyph_of_levitate>("spell_pri_glyph_of_levitate");
    new aura_script<spell_pri_prayer_of_mending_proc>("spell_pri_prayer_of_mending_proc");
    new aura_script<spell_pri_chakra_chastise>("spell_pri_chakra_chastise");
    new aura_script<spell_pri_glyph_of_inspired_hymns>("spell_pri_glyph_of_inspired_hymns");
}
