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
 * Scripts for spells with SPELLFAMILY_GENERIC spells used by items.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_item_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SkillDiscovery.h"
#include "Battleground.h"
#include "Chat.h"
#include "CreatureTextMgr.h"
#include "Creature.h"
#include "GridNotifiers.h"
#include "spell_common.h"
#include "Random.h"

// Generic script for handling item dummy effects which trigger another spell.
class spell_item_trigger_spell : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;

    public:
        spell_item_trigger_spell(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_item_trigger_spell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_trigger_spell_SpellScript);
        private:
            uint32 _triggeredSpellId;

        public:
            spell_item_trigger_spell_SpellScript(uint32 triggeredSpellId) : SpellScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Item* item = GetCastItem())
                    caster->CastSpell(caster, _triggeredSpellId, true, item);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_trigger_spell_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_trigger_spell_SpellScript(_triggeredSpellId);
        }
};

enum AegisOfPreservation
{
    SPELL_AEGIS_HEAL   = 23781
};

// 23780 - Aegis of Preservation
class spell_item_aegis_of_preservation : public SpellScriptLoader
{
    public:
        spell_item_aegis_of_preservation() : SpellScriptLoader("spell_item_aegis_of_preservation") { }

        class spell_item_aegis_of_preservation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_aegis_of_preservation_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AEGIS_HEAL))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_AEGIS_HEAL, true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_aegis_of_preservation_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_aegis_of_preservation_AuraScript();
        }
};

// 26400 - Arcane Shroud
class spell_item_arcane_shroud : public SpellScriptLoader
{
    public:
        spell_item_arcane_shroud() : SpellScriptLoader("spell_item_arcane_shroud") { }

        class spell_item_arcane_shroud_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_arcane_shroud_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                int32 diff = GetUnitOwner()->GetLevel() - 60;
                if (diff > 0)
                    amount += 2 * diff;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_arcane_shroud_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_THREAT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_arcane_shroud_AuraScript();
        }
};

// 64411 - Blessing of Ancient Kings (Val'anyr, Hammer of Ancient Kings)
enum BlessingOfAncientKings
{
    SPELL_PROTECTION_OF_ANCIENT_KINGS   = 64413
};

class spell_item_blessing_of_ancient_kings : public SpellScriptLoader
{
    public:
        spell_item_blessing_of_ancient_kings() : SpellScriptLoader("spell_item_blessing_of_ancient_kings") { }

        class spell_item_blessing_of_ancient_kings_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_blessing_of_ancient_kings_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PROTECTION_OF_ANCIENT_KINGS))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                int32 absorb = int32(CalculatePct(eventInfo.GetHealInfo()->GetHeal(), 15.0f));
                if (AuraEffect* protEff = eventInfo.GetProcTarget()->GetAuraEffect(SPELL_PROTECTION_OF_ANCIENT_KINGS, 0, eventInfo.GetActor()->GetGUID()))
                {
                    // The shield can grow to a maximum size of 20,000 damage absorbtion
                    protEff->SetAmount(std::min<int32>(protEff->GetAmount() + absorb, 20000));

                    // Refresh and return to prevent replacing the aura
                    protEff->GetBase()->RefreshDuration();
                }
                else
                    GetTarget()->CastCustomSpell(SPELL_PROTECTION_OF_ANCIENT_KINGS, SPELLVALUE_BASE_POINT0, absorb, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_blessing_of_ancient_kings_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_item_blessing_of_ancient_kings_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_blessing_of_ancient_kings_AuraScript();
        }
};

// 8342  - Defibrillate (Goblin Jumper Cables) have 33% chance on success
// 22999 - Defibrillate (Goblin Jumper Cables XL) have 50% chance on success
// 54732 - Defibrillate (Gnomish Army Knife) have 67% chance on success
enum Defibrillate
{
    SPELL_GOBLIN_JUMPER_CABLES_FAIL     = 8338,
    SPELL_GOBLIN_JUMPER_CABLES_XL_FAIL  = 23055
};

class spell_item_defibrillate : public SpellScriptLoader
{
    public:
        spell_item_defibrillate(char const* name, uint8 chance, uint32 failSpell = 0) : SpellScriptLoader(name), _chance(chance), _failSpell(failSpell) { }

        class spell_item_defibrillate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_defibrillate_SpellScript);

        public:
            spell_item_defibrillate_SpellScript(uint8 chance, uint32 failSpell) : SpellScript(), _chance(chance), _failSpell(failSpell) { }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (_failSpell && !sSpellMgr->GetSpellInfo(_failSpell))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                if (roll_chance_i(_chance))
                {
                    PreventHitDefaultEffect(effIndex);
                    if (_failSpell)
                        GetCaster()->CastSpell(GetCaster(), _failSpell, true, GetCastItem());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_defibrillate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_RESURRECT);
            }

        private:
            uint8 _chance;
            uint32 _failSpell;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_defibrillate_SpellScript(_chance, _failSpell);
        }

    private:
        uint8 _chance;
        uint32 _failSpell;
};

enum DesperateDefense
{
    SPELL_DESPERATE_RAGE    = 33898
};

// 33896 - Desperate Defense
class spell_item_desperate_defense : public SpellScriptLoader
{
    public:
        spell_item_desperate_defense() : SpellScriptLoader("spell_item_desperate_defense") { }

        class spell_item_desperate_defense_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_desperate_defense_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DESPERATE_RAGE))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_DESPERATE_RAGE, true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_item_desperate_defense_AuraScript::HandleProc, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_desperate_defense_AuraScript();
        }
};

// http://www.wowhead.com/item=6522 Deviate Fish
// 8063 Deviate Fish
enum DeviateFishSpells
{
    SPELL_SLEEPY            = 8064,
    SPELL_INVIGORATE        = 8065,
    SPELL_SHRINK            = 8066,
    SPELL_PARTY_TIME        = 8067,
    SPELL_HEALTHY_SPIRIT    = 8068,
};

class spell_item_deviate_fish : public SpellScriptLoader
{
    public:
        spell_item_deviate_fish() : SpellScriptLoader("spell_item_deviate_fish") { }

        class spell_item_deviate_fish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_deviate_fish_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                for (uint32 spellId = SPELL_SLEEPY; spellId <= SPELL_HEALTHY_SPIRIT; ++spellId)
                    if (!sSpellMgr->GetSpellInfo(spellId))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = urand(SPELL_SLEEPY, SPELL_HEALTHY_SPIRIT);
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_deviate_fish_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_deviate_fish_SpellScript();
        }
};

// http://www.wowhead.com/item=47499 Flask of the North
// 67019 Flask of the North
enum FlaskOfTheNorthSpells
{
    SPELL_FLASK_OF_THE_NORTH_SP = 67016,
    SPELL_FLASK_OF_THE_NORTH_AP = 67017,
    SPELL_FLASK_OF_THE_NORTH_STR = 67018,
};

class spell_item_flask_of_the_north : public SpellScriptLoader
{
    public:
        spell_item_flask_of_the_north() : SpellScriptLoader("spell_item_flask_of_the_north") { }

        class spell_item_flask_of_the_north_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_flask_of_the_north_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_SP) || !sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_AP) || !sSpellMgr->GetSpellInfo(SPELL_FLASK_OF_THE_NORTH_STR))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                std::vector<uint32> possibleSpells;
                switch (caster->GetClass())
                {
                    case CLASS_WARLOCK:
                    case CLASS_MAGE:
                    case CLASS_PRIEST:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        break;
                    case CLASS_DEATH_KNIGHT:
                    case CLASS_WARRIOR:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_STR);
                        break;
                    case CLASS_ROGUE:
                    case CLASS_HUNTER:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_AP);
                        break;
                    case CLASS_DRUID:
                    case CLASS_PALADIN:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_STR);
                        break;
                    case CLASS_SHAMAN:
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_SP);
                        possibleSpells.push_back(SPELL_FLASK_OF_THE_NORTH_AP);
                        break;
                }

                caster->CastSpell(caster, possibleSpells[irand(0, (possibleSpells.size() - 1))], true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_flask_of_the_north_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_flask_of_the_north_SpellScript();
        }
};

// http://www.wowhead.com/item=10645 Gnomish Death Ray
// 13280 Gnomish Death Ray
enum GnomishDeathRay
{
    SPELL_GNOMISH_DEATH_RAY_SELF = 13493,
    SPELL_GNOMISH_DEATH_RAY_TARGET = 13279,
};

class spell_item_gnomish_death_ray : public SpellScriptLoader
{
    public:
        spell_item_gnomish_death_ray() : SpellScriptLoader("spell_item_gnomish_death_ray") { }

        class spell_item_gnomish_death_ray_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gnomish_death_ray_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GNOMISH_DEATH_RAY_SELF) || !sSpellMgr->GetSpellInfo(SPELL_GNOMISH_DEATH_RAY_TARGET))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (urand(0, 99) < 15)
                        caster->CastSpell(caster, SPELL_GNOMISH_DEATH_RAY_SELF, true, NULL);    // failure
                    else
                        caster->CastSpell(target, SPELL_GNOMISH_DEATH_RAY_TARGET, true, NULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_gnomish_death_ray_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_gnomish_death_ray_SpellScript();
        }
};

// http://www.wowhead.com/item=27388 Mr. Pinchy
// 33060 Make a Wish
enum MakeAWish
{
    SPELL_MR_PINCHYS_BLESSING       = 33053,
    SPELL_SUMMON_MIGHTY_MR_PINCHY   = 33057,
    SPELL_SUMMON_FURIOUS_MR_PINCHY  = 33059,
    SPELL_TINY_MAGICAL_CRAWDAD      = 33062,
    SPELL_MR_PINCHYS_GIFT           = 33064,
};

class spell_item_make_a_wish : public SpellScriptLoader
{
    public:
        spell_item_make_a_wish() : SpellScriptLoader("spell_item_make_a_wish") { }

        class spell_item_make_a_wish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_make_a_wish_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MR_PINCHYS_BLESSING) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_MIGHTY_MR_PINCHY) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_FURIOUS_MR_PINCHY) || !sSpellMgr->GetSpellInfo(SPELL_TINY_MAGICAL_CRAWDAD) || !sSpellMgr->GetSpellInfo(SPELL_MR_PINCHYS_GIFT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_MR_PINCHYS_GIFT;
                switch (urand(1, 5))
                {
                    case 1: spellId = SPELL_MR_PINCHYS_BLESSING; break;
                    case 2: spellId = SPELL_SUMMON_MIGHTY_MR_PINCHY; break;
                    case 3: spellId = SPELL_SUMMON_FURIOUS_MR_PINCHY; break;
                    case 4: spellId = SPELL_TINY_MAGICAL_CRAWDAD; break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_make_a_wish_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_make_a_wish_SpellScript();
        }
};

// http://www.wowhead.com/item=32686 Mingo's Fortune Giblets
// 40802 Mingo's Fortune Generator
class spell_item_mingos_fortune_generator : public SpellScriptLoader
{
    public:
        spell_item_mingos_fortune_generator() : SpellScriptLoader("spell_item_mingos_fortune_generator") { }

        class spell_item_mingos_fortune_generator_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_mingos_fortune_generator_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                // Selecting one from Bloodstained Fortune item
                uint32 newitemid;
                switch (urand(1, 20))
                {
                    case 1:  newitemid = 32688; break;
                    case 2:  newitemid = 32689; break;
                    case 3:  newitemid = 32690; break;
                    case 4:  newitemid = 32691; break;
                    case 5:  newitemid = 32692; break;
                    case 6:  newitemid = 32693; break;
                    case 7:  newitemid = 32700; break;
                    case 8:  newitemid = 32701; break;
                    case 9:  newitemid = 32702; break;
                    case 10: newitemid = 32703; break;
                    case 11: newitemid = 32704; break;
                    case 12: newitemid = 32705; break;
                    case 13: newitemid = 32706; break;
                    case 14: newitemid = 32707; break;
                    case 15: newitemid = 32708; break;
                    case 16: newitemid = 32709; break;
                    case 17: newitemid = 32710; break;
                    case 18: newitemid = 32711; break;
                    case 19: newitemid = 32712; break;
                    case 20: newitemid = 32713; break;
                    default:
                        return;
                }

                CreateItem(effIndex, newitemid);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_mingos_fortune_generator_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_mingos_fortune_generator_SpellScript();
        }
};

// 71875, 71877 - Item - Black Bruise: Necrotic Touch Proc
enum NecroticTouch
{
    SPELL_ITEM_NECROTIC_TOUCH_PROC  = 71879
};

class spell_item_necrotic_touch : public SpellScriptLoader
{
    public:
        spell_item_necrotic_touch() : SpellScriptLoader("spell_item_necrotic_touch") { }

        class spell_item_necrotic_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_necrotic_touch_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ITEM_NECROTIC_TOUCH_PROC))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->IsAlive();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                int32 bp = CalculatePct(int32(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
                GetTarget()->CastCustomSpell(SPELL_ITEM_NECROTIC_TOUCH_PROC, SPELLVALUE_BASE_POINT0, bp, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_necrotic_touch_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_item_necrotic_touch_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_necrotic_touch_AuraScript();
        }
};

// http://www.wowhead.com/item=10720 Gnomish Net-o-Matic Projector
// 13120 Net-o-Matic
enum NetOMaticSpells
{
    SPELL_NET_O_MATIC_TRIGGERED1 = 16566,
    SPELL_NET_O_MATIC_TRIGGERED2 = 13119,
    SPELL_NET_O_MATIC_TRIGGERED3 = 13099,
};

class spell_item_net_o_matic : public SpellScriptLoader
{
    public:
        spell_item_net_o_matic() : SpellScriptLoader("spell_item_net_o_matic") { }

        class spell_item_net_o_matic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_net_o_matic_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_NET_O_MATIC_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    uint32 spellId = SPELL_NET_O_MATIC_TRIGGERED3;
                    uint32 roll = urand(0, 99);
                    if (roll < 2)                            // 2% for 30 sec self root (off-like chance unknown)
                        spellId = SPELL_NET_O_MATIC_TRIGGERED1;
                    else if (roll < 4)                       // 2% for 20 sec root, charge to target (off-like chance unknown)
                        spellId = SPELL_NET_O_MATIC_TRIGGERED2;

                    GetCaster()->CastSpell(target, spellId, true, NULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_net_o_matic_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_net_o_matic_SpellScript();
        }
};

// http://www.wowhead.com/item=8529 Noggenfogger Elixir
// 16589 Noggenfogger Elixir
enum NoggenfoggerElixirSpells
{
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1 = 16595,
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2 = 16593,
    SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3 = 16591,
};

class spell_item_noggenfogger_elixir : public SpellScriptLoader
{
    public:
        spell_item_noggenfogger_elixir() : SpellScriptLoader("spell_item_noggenfogger_elixir") { }

        class spell_item_noggenfogger_elixir_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_noggenfogger_elixir_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED3;
                switch (urand(1, 3))
                {
                    case 1: spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED1; break;
                    case 2: spellId = SPELL_NOGGENFOGGER_ELIXIR_TRIGGERED2; break;
                }

                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_noggenfogger_elixir_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_noggenfogger_elixir_SpellScript();
        }
};

// 17512 - Piccolo of the Flaming Fire
class spell_item_piccolo_of_the_flaming_fire : public SpellScriptLoader
{
    public:
        spell_item_piccolo_of_the_flaming_fire() : SpellScriptLoader("spell_item_piccolo_of_the_flaming_fire") { }

        class spell_item_piccolo_of_the_flaming_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_piccolo_of_the_flaming_fire_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                    target->HandleEmoteCommand(EMOTE_STATE_DANCE);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_piccolo_of_the_flaming_fire_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_piccolo_of_the_flaming_fire_SpellScript();
        }
};

// http://www.wowhead.com/item=6657 Savory Deviate Delight
// 8213 Savory Deviate Delight
enum SavoryDeviateDelight
{
    SPELL_FLIP_OUT_MALE     = 8219,
    SPELL_FLIP_OUT_FEMALE   = 8220,
    SPELL_YAAARRRR_MALE     = 8221,
    SPELL_YAAARRRR_FEMALE   = 8222,
};

class spell_item_savory_deviate_delight : public SpellScriptLoader
{
    public:
        spell_item_savory_deviate_delight() : SpellScriptLoader("spell_item_savory_deviate_delight") { }

        class spell_item_savory_deviate_delight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_savory_deviate_delight_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                for (uint32 spellId = SPELL_FLIP_OUT_MALE; spellId <= SPELL_YAAARRRR_FEMALE; ++spellId)
                    if (!sSpellMgr->GetSpellInfo(spellId))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = 0;
                switch (urand(1, 2))
                {
                    // Flip Out - ninja
                    case 1: spellId = (caster->GetGender() == GENDER_MALE ? SPELL_FLIP_OUT_MALE : SPELL_FLIP_OUT_FEMALE); break;
                    // Yaaarrrr - pirate
                    case 2: spellId = (caster->GetGender() == GENDER_MALE ? SPELL_YAAARRRR_MALE : SPELL_YAAARRRR_FEMALE); break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_savory_deviate_delight_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_savory_deviate_delight_SpellScript();
        }
};

// 48129 - Scroll of Recall
// 60320 - Scroll of Recall II
// 60321 - Scroll of Recall III
enum ScrollOfRecall
{
    SPELL_SCROLL_OF_RECALL_I                = 48129,
    SPELL_SCROLL_OF_RECALL_II               = 60320,
    SPELL_SCROLL_OF_RECALL_III              = 60321,
    SPELL_LOST                              = 60444,
    SPELL_SCROLL_OF_RECALL_FAIL_ALLIANCE_1  = 60323,
    SPELL_SCROLL_OF_RECALL_FAIL_HORDE_1     = 60328,
};

class spell_item_scroll_of_recall : public SpellScriptLoader
{
    public:
        spell_item_scroll_of_recall() : SpellScriptLoader("spell_item_scroll_of_recall") { }

        class spell_item_scroll_of_recall_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_scroll_of_recall_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                Unit* caster = GetCaster();
                uint8 maxSafeLevel = 0;
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_SCROLL_OF_RECALL_I:  // Scroll of Recall
                        maxSafeLevel = 40;
                        break;
                    case SPELL_SCROLL_OF_RECALL_II:  // Scroll of Recall II
                        maxSafeLevel = 70;
                        break;
                    case SPELL_SCROLL_OF_RECALL_III:  // Scroll of Recal III
                        maxSafeLevel = 80;
                        break;
                    default:
                        break;
                }

                if (caster->GetLevel() > maxSafeLevel)
                {
                    caster->CastSpell(caster, SPELL_LOST, true);

                    // ALLIANCE from 60323 to 60330 - HORDE from 60328 to 60335
                    uint32 spellId = SPELL_SCROLL_OF_RECALL_FAIL_ALLIANCE_1;
                    if (GetCaster()->ToPlayer()->GetTeam() == HORDE)
                        spellId = SPELL_SCROLL_OF_RECALL_FAIL_HORDE_1;

                    GetCaster()->CastSpell(GetCaster(), spellId + urand(0, 7), true);

                    PreventHitDefaultEffect(effIndex);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_scroll_of_recall_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_scroll_of_recall_SpellScript();
        }
};

// 71169 - Shadow's Fate (Shadowmourne questline)
enum ShadowsFate
{
    SPELL_SOUL_FEAST        = 71203,
    NPC_SINDRAGOSA          = 36853
};

class spell_item_unsated_craving : public SpellScriptLoader
{
    public:
        spell_item_unsated_craving() : SpellScriptLoader("spell_item_unsated_craving") { }

        class spell_item_unsated_craving_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_unsated_craving_AuraScript);

            bool CheckProc(ProcEventInfo& procInfo)
            {
                Unit* caster = procInfo.GetActor();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return false;

                Unit* target = procInfo.GetActionTarget();
                if (!target || target->GetTypeId() != TYPEID_UNIT || target->GetCreatureType() == CREATURE_TYPE_CRITTER || (target->GetEntry() != NPC_SINDRAGOSA && target->IsSummon()))
                    return false;

                return true;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_unsated_craving_AuraScript::CheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_unsated_craving_AuraScript();
        }
};

class spell_item_shadows_fate : public SpellScriptLoader
{
    public:
        spell_item_shadows_fate() : SpellScriptLoader("spell_item_shadows_fate") { }

        class spell_item_shadows_fate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_shadows_fate_AuraScript);

            void HandleProc(ProcEventInfo& procInfo)
            {
                Unit* caster = procInfo.GetActor();
                Unit* target = GetCaster();
                if (!caster || !target)
                    return;

                caster->CastSpell(target, SPELL_SOUL_FEAST, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_item_shadows_fate_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_shadows_fate_AuraScript();
        }
};

enum Shadowmourne
{
    SPELL_SHADOWMOURNE_CHAOS_BANE_DAMAGE    = 71904,
    SPELL_SHADOWMOURNE_SOUL_FRAGMENT        = 71905,
    SPELL_SHADOWMOURNE_VISUAL_LOW           = 72521,
    SPELL_SHADOWMOURNE_VISUAL_HIGH          = 72523,
    SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF      = 73422,
};

// 71903 - Item - Shadowmourne Legendary
class spell_item_shadowmourne : public SpellScriptLoader
{
    public:
        spell_item_shadowmourne() : SpellScriptLoader("spell_item_shadowmourne") { }

        class spell_item_shadowmourne_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_shadowmourne_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_CHAOS_BANE_DAMAGE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_SOUL_FRAGMENT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                if (GetTarget()->HasAura(SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF)) // cant collect shards while under effect of Chaos Bane buff
                    return false;
                return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->IsAlive();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_SHADOWMOURNE_SOUL_FRAGMENT, true, NULL, aurEff);

                // this can't be handled in AuraScript of SoulFragments because we need to know victim
                if (Aura* soulFragments = GetTarget()->GetAura(SPELL_SHADOWMOURNE_SOUL_FRAGMENT))
                {
                    if (soulFragments->GetStackAmount() >= 10)
                    {
                        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHADOWMOURNE_CHAOS_BANE_DAMAGE, true, NULL, aurEff);
                        soulFragments->Remove();
                    }
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_item_shadowmourne_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_item_shadowmourne_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_shadowmourne_AuraScript();
        }
};

// 71905 - Soul Fragment
class spell_item_shadowmourne_soul_fragment : public SpellScriptLoader
{
    public:
        spell_item_shadowmourne_soul_fragment() : SpellScriptLoader("spell_item_shadowmourne_soul_fragment") { }

        class spell_item_shadowmourne_soul_fragment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_shadowmourne_soul_fragment_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_VISUAL_LOW) || !sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_VISUAL_HIGH) || !sSpellMgr->GetSpellInfo(SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF))
                    return false;
                return true;
            }

            void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                switch (GetStackAmount())
                {
                    case 1:
                        target->CastSpell(target, SPELL_SHADOWMOURNE_VISUAL_LOW, true);
                        break;
                    case 6:
                        target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_LOW);
                        target->CastSpell(target, SPELL_SHADOWMOURNE_VISUAL_HIGH, true);
                        break;
                    case 10:
                        target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_HIGH);
                        target->CastSpell(target, SPELL_SHADOWMOURNE_CHAOS_BANE_BUFF, true);
                        break;
                    default:
                        break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_LOW);
                target->RemoveAurasDueToSpell(SPELL_SHADOWMOURNE_VISUAL_HIGH);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_item_shadowmourne_soul_fragment_AuraScript::OnStackChange, EFFECT_0, SPELL_AURA_MOD_STAT, AuraEffectHandleModes(AURA_EFFECT_HANDLE_REAL | AURA_EFFECT_HANDLE_REAPPLY));
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_shadowmourne_soul_fragment_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STAT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_shadowmourne_soul_fragment_AuraScript();
        }
};

// http://www.wowhead.com/item=7734 Six Demon Bag
// 14537 Six Demon Bag
enum SixDemonBagSpells
{
    SPELL_FROSTBOLT                 = 11538,
    SPELL_POLYMORPH                 = 14621,
    SPELL_SUMMON_FELHOUND_MINION    = 14642,
    SPELL_FIREBALL                  = 15662,
    SPELL_CHAIN_LIGHTNING           = 21179,
    SPELL_ENVELOPING_WINDS          = 25189,
};

class spell_item_six_demon_bag : public SpellScriptLoader
{
    public:
        spell_item_six_demon_bag() : SpellScriptLoader("spell_item_six_demon_bag") { }

        class spell_item_six_demon_bag_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_six_demon_bag_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FROSTBOLT) || !sSpellMgr->GetSpellInfo(SPELL_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_SUMMON_FELHOUND_MINION) || !sSpellMgr->GetSpellInfo(SPELL_FIREBALL) || !sSpellMgr->GetSpellInfo(SPELL_CHAIN_LIGHTNING) || !sSpellMgr->GetSpellInfo(SPELL_ENVELOPING_WINDS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    uint32 spellId = 0;
                    uint32 rand = urand(0, 99);
                    if (rand < 25)                      // Fireball (25% chance)
                        spellId = SPELL_FIREBALL;
                    else if (rand < 50)                 // Frostball (25% chance)
                        spellId = SPELL_FROSTBOLT;
                    else if (rand < 70)                 // Chain Lighting (20% chance)
                        spellId = SPELL_CHAIN_LIGHTNING;
                    else if (rand < 80)                 // Polymorph (10% chance)
                    {
                        spellId = SPELL_POLYMORPH;
                        if (urand(0, 100) <= 30)        // 30% chance to self-cast
                            target = caster;
                    }
                    else if (rand < 95)                 // Enveloping Winds (15% chance)
                        spellId = SPELL_ENVELOPING_WINDS;
                    else                                // Summon Felhund minion (5% chance)
                    {
                        spellId = SPELL_SUMMON_FELHOUND_MINION;
                        target = caster;
                    }

                    caster->CastSpell(target, spellId, true, GetCastItem());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_six_demon_bag_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_six_demon_bag_SpellScript();
        }
};

// 28862 - The Eye of Diminution
class spell_item_the_eye_of_diminution : public SpellScriptLoader
{
    public:
        spell_item_the_eye_of_diminution() : SpellScriptLoader("spell_item_the_eye_of_diminution") { }

        class spell_item_the_eye_of_diminution_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_the_eye_of_diminution_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                int32 diff = GetUnitOwner()->GetLevel() - 60;
                if (diff > 0)
                    amount += diff;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_the_eye_of_diminution_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_THREAT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_the_eye_of_diminution_AuraScript();
        }
};

// http://www.wowhead.com/item=44012 Underbelly Elixir
// 59640 Underbelly Elixir
enum UnderbellyElixirSpells
{
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED1 = 59645,
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED2 = 59831,
    SPELL_UNDERBELLY_ELIXIR_TRIGGERED3 = 59843,
};

class spell_item_underbelly_elixir : public SpellScriptLoader
{
    public:
        spell_item_underbelly_elixir() : SpellScriptLoader("spell_item_underbelly_elixir") { }

        class spell_item_underbelly_elixir_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_underbelly_elixir_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }
            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED1) || !sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED2) || !sSpellMgr->GetSpellInfo(SPELL_UNDERBELLY_ELIXIR_TRIGGERED3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint32 spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED3;
                switch (urand(1, 3))
                {
                    case 1: spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED1; break;
                    case 2: spellId = SPELL_UNDERBELLY_ELIXIR_TRIGGERED2; break;
                }
                caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_underbelly_elixir_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_underbelly_elixir_SpellScript();
        }
};

enum AirRifleSpells
{
    SPELL_AIR_RIFLE_HOLD_VISUAL = 65582,
    SPELL_AIR_RIFLE_SHOOT       = 67532,
    SPELL_AIR_RIFLE_SHOOT_SELF  = 65577,
};

class spell_item_red_rider_air_rifle : public SpellScriptLoader
{
    public:
        spell_item_red_rider_air_rifle() : SpellScriptLoader("spell_item_red_rider_air_rifle") { }

        class spell_item_red_rider_air_rifle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_red_rider_air_rifle_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_HOLD_VISUAL) || !sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_SHOOT) || !sSpellMgr->GetSpellInfo(SPELL_AIR_RIFLE_SHOOT_SELF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    caster->CastSpell(caster, SPELL_AIR_RIFLE_HOLD_VISUAL, true);
                    // needed because this spell shares GCD with its triggered spells (which must not be cast with triggered flag)
                    if (Player* player = caster->ToPlayer())
                        player->GetGlobalCooldownMgr().CancelGlobalCooldown(GetSpellInfo());
                    if (urand(0, 4))
                        caster->CastSpell(target, SPELL_AIR_RIFLE_SHOOT, false);
                    else
                        caster->CastSpell(caster, SPELL_AIR_RIFLE_SHOOT_SELF, false);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_red_rider_air_rifle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_red_rider_air_rifle_SpellScript();
        }
};

enum GenericData
{
    SPELL_ARCANITE_DRAGONLING           = 19804,
    SPELL_BATTLE_CHICKEN                = 13166,
    SPELL_MECHANICAL_DRAGONLING         = 4073,
    SPELL_MITHRIL_MECHANICAL_DRAGONLING = 12749,
};

enum CreateHeartCandy
{
    ITEM_HEART_CANDY_1 = 21818,
    ITEM_HEART_CANDY_2 = 21817,
    ITEM_HEART_CANDY_3 = 21821,
    ITEM_HEART_CANDY_4 = 21819,
    ITEM_HEART_CANDY_5 = 21816,
    ITEM_HEART_CANDY_6 = 21823,
    ITEM_HEART_CANDY_7 = 21822,
    ITEM_HEART_CANDY_8 = 21820,
};

class spell_item_create_heart_candy : public SpellScriptLoader
{
    public:
        spell_item_create_heart_candy() : SpellScriptLoader("spell_item_create_heart_candy") { }

        class spell_item_create_heart_candy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_create_heart_candy_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    static const uint32 items[] = {ITEM_HEART_CANDY_1, ITEM_HEART_CANDY_2, ITEM_HEART_CANDY_3, ITEM_HEART_CANDY_4, ITEM_HEART_CANDY_5, ITEM_HEART_CANDY_6, ITEM_HEART_CANDY_7, ITEM_HEART_CANDY_8};
                    target->AddItem(items[urand(0, 7)], 1);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_create_heart_candy_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_create_heart_candy_SpellScript();
        }
};

class spell_item_book_of_glyph_mastery : public SpellScriptLoader
{
    public:
        spell_item_book_of_glyph_mastery() : SpellScriptLoader("spell_item_book_of_glyph_mastery") { }

        class spell_item_book_of_glyph_mastery_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_book_of_glyph_mastery_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_LEARNED_EVERYTHING);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                uint32 spellId = GetSpellInfo()->Id;

                // learn random explicit discovery recipe (if any)
                if (uint32 discoveredSpellId = GetExplicitDiscoverySpell(spellId, caster))
                    caster->LearnSpell(discoveredSpellId, false);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_book_of_glyph_mastery_SpellScript::CheckRequirement);
                OnEffectHitTarget += SpellEffectFn(spell_item_book_of_glyph_mastery_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_book_of_glyph_mastery_SpellScript();
        }
};

enum GiftOfTheHarvester
{
    NPC_GHOUL   = 28845,
    MAX_GHOULS  = 5,
};

class spell_item_gift_of_the_harvester : public SpellScriptLoader
{
    public:
        spell_item_gift_of_the_harvester() : SpellScriptLoader("spell_item_gift_of_the_harvester") { }

        class spell_item_gift_of_the_harvester_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_gift_of_the_harvester_SpellScript);

            SpellCastResult CheckRequirement()
            {
                std::list<Creature*> ghouls;
                GetCaster()->GetAllMinionsByEntry(ghouls, NPC_GHOUL);
                if (ghouls.size() >= MAX_GHOULS)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_TOO_MANY_GHOULS);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_gift_of_the_harvester_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_gift_of_the_harvester_SpellScript();
        }
};

enum Sinkholes
{
    NPC_SOUTH_SINKHOLE      = 25664,
    NPC_NORTHEAST_SINKHOLE  = 25665,
    NPC_NORTHWEST_SINKHOLE  = 25666,
};

class spell_item_map_of_the_geyser_fields : public SpellScriptLoader
{
    public:
        spell_item_map_of_the_geyser_fields() : SpellScriptLoader("spell_item_map_of_the_geyser_fields") { }

        class spell_item_map_of_the_geyser_fields_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_map_of_the_geyser_fields_SpellScript);

            SpellCastResult CheckSinkholes()
            {
                Unit* caster = GetCaster();
                if (caster->FindNearestCreature(NPC_SOUTH_SINKHOLE, 30.0f, true) ||
                    caster->FindNearestCreature(NPC_NORTHEAST_SINKHOLE, 30.0f, true) ||
                    caster->FindNearestCreature(NPC_NORTHWEST_SINKHOLE, 30.0f, true))
                    return SPELL_CAST_OK;

                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_CLOSE_TO_SINKHOLE);
                return SPELL_FAILED_CUSTOM_ERROR;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_map_of_the_geyser_fields_SpellScript::CheckSinkholes);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_map_of_the_geyser_fields_SpellScript();
        }
};

enum VanquishedClutchesSpells
{
    SPELL_CRUSHER       = 64982,
    SPELL_CONSTRICTOR   = 64983,
    SPELL_CORRUPTOR     = 64984,
};

class spell_item_vanquished_clutches : public SpellScriptLoader
{
    public:
        spell_item_vanquished_clutches() : SpellScriptLoader("spell_item_vanquished_clutches") { }

        class spell_item_vanquished_clutches_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_vanquished_clutches_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CRUSHER) || !sSpellMgr->GetSpellInfo(SPELL_CONSTRICTOR) || !sSpellMgr->GetSpellInfo(SPELL_CORRUPTOR))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = RAND(SPELL_CRUSHER, SPELL_CONSTRICTOR, SPELL_CORRUPTOR);
                Unit* caster = GetCaster();
                caster->CastSpell(caster, spellId, true);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_vanquished_clutches_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_vanquished_clutches_SpellScript();
        }
};

enum AshbringerSounds
{
    SOUND_ASHBRINGER_1  = 8906,                             // "I was pure once"
    SOUND_ASHBRINGER_2  = 8907,                             // "Fought for righteousness"
    SOUND_ASHBRINGER_3  = 8908,                             // "I was once called Ashbringer"
    SOUND_ASHBRINGER_4  = 8920,                             // "Betrayed by my order"
    SOUND_ASHBRINGER_5  = 8921,                             // "Destroyed by Kel'Thuzad"
    SOUND_ASHBRINGER_6  = 8922,                             // "Made to serve"
    SOUND_ASHBRINGER_7  = 8923,                             // "My son watched me die"
    SOUND_ASHBRINGER_8  = 8924,                             // "Crusades fed his rage"
    SOUND_ASHBRINGER_9  = 8925,                             // "Truth is unknown to him"
    SOUND_ASHBRINGER_10 = 8926,                             // "Scarlet Crusade  is pure no longer"
    SOUND_ASHBRINGER_11 = 8927,                             // "Balnazzar's crusade corrupted my son"
    SOUND_ASHBRINGER_12 = 8928,                             // "Kill them all!"
};

class spell_item_ashbringer : public SpellScriptLoader
{
    public:
        spell_item_ashbringer() : SpellScriptLoader("spell_item_ashbringer") { }

        class spell_item_ashbringer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_ashbringer_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void OnDummyEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Player* player = GetCaster()->ToPlayer();
                uint32 sound_id = RAND( SOUND_ASHBRINGER_1, SOUND_ASHBRINGER_2, SOUND_ASHBRINGER_3, SOUND_ASHBRINGER_4, SOUND_ASHBRINGER_5, SOUND_ASHBRINGER_6,
                                SOUND_ASHBRINGER_7, SOUND_ASHBRINGER_8, SOUND_ASHBRINGER_9, SOUND_ASHBRINGER_10, SOUND_ASHBRINGER_11, SOUND_ASHBRINGER_12 );

                // Ashbringers effect (spellID 28441) retriggers every 5 seconds, with a chance of making it say one of the above 12 sounds
                if (urand(0, 60) < 1)
                    player->PlayDirectSound(sound_id, player);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_item_ashbringer_SpellScript::OnDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_ashbringer_SpellScript();
        }
};

enum MagicEater
{
    SPELL_WILD_MAGIC                             = 58891,
    SPELL_WELL_FED_1                             = 57288,
    SPELL_WELL_FED_2                             = 57139,
    SPELL_WELL_FED_3                             = 57111,
    SPELL_WELL_FED_4                             = 57286,
    SPELL_WELL_FED_5                             = 57291,
};

class spell_magic_eater_food : public SpellScriptLoader
{
    public:
        spell_magic_eater_food() : SpellScriptLoader("spell_magic_eater_food") { }

        class spell_magic_eater_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magic_eater_food_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                Unit* target = GetTarget();
                switch (urand(0, 5))
                {
                    case 0:
                        target->CastSpell(target, SPELL_WILD_MAGIC, true);
                        break;
                    case 1:
                        target->CastSpell(target, SPELL_WELL_FED_1, true);
                        break;
                    case 2:
                        target->CastSpell(target, SPELL_WELL_FED_2, true);
                        break;
                    case 3:
                        target->CastSpell(target, SPELL_WELL_FED_3, true);
                        break;
                    case 4:
                        target->CastSpell(target, SPELL_WELL_FED_4, true);
                        break;
                    case 5:
                        target->CastSpell(target, SPELL_WELL_FED_5, true);
                        break;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_magic_eater_food_AuraScript::HandleTriggerSpell, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_magic_eater_food_AuraScript();
        }
};

class spell_item_shimmering_vessel : public SpellScriptLoader
{
    public:
        spell_item_shimmering_vessel() : SpellScriptLoader("spell_item_shimmering_vessel") { }

        class spell_item_shimmering_vessel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_shimmering_vessel_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Creature* target = GetHitCreature())
                    target->setDeathState(JUST_RESPAWNED);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_shimmering_vessel_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_shimmering_vessel_SpellScript();
        }
};

enum PurifyHelboarMeat
{
    SPELL_SUMMON_PURIFIED_HELBOAR_MEAT      = 29277,
    SPELL_SUMMON_TOXIC_HELBOAR_MEAT         = 29278,
};

class spell_item_purify_helboar_meat : public SpellScriptLoader
{
    public:
        spell_item_purify_helboar_meat() : SpellScriptLoader("spell_item_purify_helboar_meat") { }

        class spell_item_purify_helboar_meat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_purify_helboar_meat_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_PURIFIED_HELBOAR_MEAT) ||  !sSpellMgr->GetSpellInfo(SPELL_SUMMON_TOXIC_HELBOAR_MEAT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(50) ? SPELL_SUMMON_PURIFIED_HELBOAR_MEAT : SPELL_SUMMON_TOXIC_HELBOAR_MEAT, true, NULL);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_purify_helboar_meat_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_purify_helboar_meat_SpellScript();
        }
};

enum CrystalPrison
{
    OBJECT_IMPRISONED_DOOMGUARD     = 179644,
};

class spell_item_crystal_prison_dummy_dnd : public SpellScriptLoader
{
    public:
        spell_item_crystal_prison_dummy_dnd() : SpellScriptLoader("spell_item_crystal_prison_dummy_dnd") { }

        class spell_item_crystal_prison_dummy_dnd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_crystal_prison_dummy_dnd_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sObjectMgr->GetGameObjectTemplate(OBJECT_IMPRISONED_DOOMGUARD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Creature* target = GetHitCreature())
                    if (target->isDead() && !target->IsPet())
                    {
                        GetCaster()->SummonGameObject(OBJECT_IMPRISONED_DOOMGUARD, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), { }, uint32(target->GetRespawnTime() - time(NULL)));
                        target->DespawnOrUnsummon();
                    }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_crystal_prison_dummy_dnd_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_crystal_prison_dummy_dnd_SpellScript();
        }
};

enum ReindeerTransformation
{
    SPELL_FLYING_REINDEER_310                   = 44827,
    SPELL_FLYING_REINDEER_280                   = 44825,
    SPELL_FLYING_REINDEER_60                    = 44824,
    SPELL_REINDEER_100                          = 25859,
    SPELL_REINDEER_60                           = 25858,
};

class spell_item_reindeer_transformation : public SpellScriptLoader
{
    public:
        spell_item_reindeer_transformation() : SpellScriptLoader("spell_item_reindeer_transformation") { }

        class spell_item_reindeer_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_reindeer_transformation_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_310) || !sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_280)
                    || !sSpellMgr->GetSpellInfo(SPELL_FLYING_REINDEER_60) || !sSpellMgr->GetSpellInfo(SPELL_REINDEER_100)
                    || !sSpellMgr->GetSpellInfo(SPELL_REINDEER_60))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (caster->HasAuraType(SPELL_AURA_MOUNTED))
                {
                    float flyspeed = caster->GetSpeedRate(MOVE_FLIGHT);
                    float speed = caster->GetSpeedRate(MOVE_RUN);

                    caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    //5 different spells used depending on mounted speed and if mount can fly or not

                    if (flyspeed >= 4.1f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_310, true); //310% flying Reindeer
                    else if (flyspeed >= 3.8f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_280, true); //280% flying Reindeer
                    else if (flyspeed >= 1.6f)
                        // Flying Reindeer
                        caster->CastSpell(caster, SPELL_FLYING_REINDEER_60, true); //60% flying Reindeer
                    else if (speed >= 2.0f)
                        // Reindeer
                        caster->CastSpell(caster, SPELL_REINDEER_100, true); //100% ground Reindeer
                    else
                        // Reindeer
                        caster->CastSpell(caster, SPELL_REINDEER_60, true); //60% ground Reindeer
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_reindeer_transformation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_reindeer_transformation_SpellScript();
    }
};

enum NighInvulnerability
{
    SPELL_NIGH_INVULNERABILITY                  = 30456,
    SPELL_COMPLETE_VULNERABILITY                = 30457,
};

class spell_item_nigh_invulnerability : public SpellScriptLoader
{
    public:
        spell_item_nigh_invulnerability() : SpellScriptLoader("spell_item_nigh_invulnerability") { }

        class spell_item_nigh_invulnerability_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_nigh_invulnerability_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NIGH_INVULNERABILITY) || !sSpellMgr->GetSpellInfo(SPELL_COMPLETE_VULNERABILITY))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (Item* castItem = GetCastItem())
                {
                    if (roll_chance_i(86))                  // Nigh-Invulnerability   - success
                        caster->CastSpell(caster, SPELL_NIGH_INVULNERABILITY, true, castItem);
                    else                                    // Complete Vulnerability - backfire in 14% casts
                        caster->CastSpell(caster, SPELL_COMPLETE_VULNERABILITY, true, castItem);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_nigh_invulnerability_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_nigh_invulnerability_SpellScript();
        }
};

enum Poultryzer
{
    SPELL_POULTRYIZER_SUCCESS    = 30501,
    SPELL_POULTRYIZER_BACKFIRE   = 30504,
};

class spell_item_poultryizer : public SpellScriptLoader
{
    public:
        spell_item_poultryizer() : SpellScriptLoader("spell_item_poultryizer") { }

        class spell_item_poultryizer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_poultryizer_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_POULTRYIZER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_POULTRYIZER_BACKFIRE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (GetCastItem() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), roll_chance_i(80) ? SPELL_POULTRYIZER_SUCCESS : SPELL_POULTRYIZER_BACKFIRE, true, GetCastItem());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_poultryizer_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_poultryizer_SpellScript();
        }
};

enum SocretharsStone
{
    SPELL_SOCRETHAR_TO_SEAT     = 35743,
    SPELL_SOCRETHAR_FROM_SEAT   = 35744,
};

class spell_item_socrethars_stone : public SpellScriptLoader
{
    public:
        spell_item_socrethars_stone() : SpellScriptLoader("spell_item_socrethars_stone") { }

        class spell_item_socrethars_stone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_socrethars_stone_SpellScript);

            bool Load() override
            {
                return (GetCaster()->GetAreaId() == 3900 || GetCaster()->GetAreaId() == 3742);
            }
            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SOCRETHAR_TO_SEAT) || !sSpellMgr->GetSpellInfo(SPELL_SOCRETHAR_FROM_SEAT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                switch (caster->GetAreaId())
                {
                    case 3900:
                        caster->CastSpell(caster, SPELL_SOCRETHAR_TO_SEAT, true);
                        break;
                    case 3742:
                        caster->CastSpell(caster, SPELL_SOCRETHAR_FROM_SEAT, true);
                        break;
                    default:
                        return;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_socrethars_stone_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_socrethars_stone_SpellScript();
        }
};

enum DemonBroiledSurprise
{
    QUEST_SUPER_HOT_STEW                    = 11379,
    SPELL_CREATE_DEMON_BROILED_SURPRISE     = 43753,
    NPC_ABYSSAL_FLAMEBRINGER                = 19973,
};

class spell_item_demon_broiled_surprise : public SpellScriptLoader
{
    public:
        spell_item_demon_broiled_surprise() : SpellScriptLoader("spell_item_demon_broiled_surprise") { }

        class spell_item_demon_broiled_surprise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_demon_broiled_surprise_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_DEMON_BROILED_SURPRISE) || !sObjectMgr->GetCreatureTemplate(NPC_ABYSSAL_FLAMEBRINGER) || !sObjectMgr->GetQuestTemplate(QUEST_SUPER_HOT_STEW))
                    return false;
                return true;
            }

            bool Load() override
            {
               return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* player = GetCaster();
                player->CastSpell(player, SPELL_CREATE_DEMON_BROILED_SURPRISE, false);
            }

            SpellCastResult CheckRequirement()
            {
                Player* player = GetCaster()->ToPlayer();
                if (player->GetQuestStatus(QUEST_SUPER_HOT_STEW) != QUEST_STATUS_INCOMPLETE)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (Creature* creature = player->FindNearestCreature(NPC_ABYSSAL_FLAMEBRINGER, 10, false))
                    if (creature->isDead())
                        return SPELL_CAST_OK;
                return SPELL_FAILED_NOT_HERE;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_demon_broiled_surprise_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_item_demon_broiled_surprise_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_demon_broiled_surprise_SpellScript();
        }
};

enum CompleteRaptorCapture
{
    SPELL_RAPTOR_CAPTURE_CREDIT     = 42337,
};

class spell_item_complete_raptor_capture : public SpellScriptLoader
{
    public:
        spell_item_complete_raptor_capture() : SpellScriptLoader("spell_item_complete_raptor_capture") { }

        class spell_item_complete_raptor_capture_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_complete_raptor_capture_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RAPTOR_CAPTURE_CREDIT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (GetHitCreature())
                {
                    GetHitCreature()->DespawnOrUnsummon();

                    //cast spell Raptor Capture Credit
                    caster->CastSpell(caster, SPELL_RAPTOR_CAPTURE_CREDIT, true, NULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_complete_raptor_capture_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_complete_raptor_capture_SpellScript();
        }
};

enum ImpaleLeviroth
{
    NPC_LEVIROTH                = 26452,
    SPELL_LEVIROTH_SELF_IMPALE  = 49882,
};

class spell_item_impale_leviroth : public SpellScriptLoader
{
    public:
        spell_item_impale_leviroth() : SpellScriptLoader("spell_item_impale_leviroth") { }

        class spell_item_impale_leviroth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_impale_leviroth_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sObjectMgr->GetCreatureTemplate(NPC_LEVIROTH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Unit* target = GetHitCreature())
                    if (target->GetEntry() == NPC_LEVIROTH && !target->HealthBelowPct(95))
                        target->CastSpell(target, SPELL_LEVIROTH_SELF_IMPALE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_impale_leviroth_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_impale_leviroth_SpellScript();
        }
};

enum BrewfestMountTransformation
{
    SPELL_MOUNT_RAM_100                         = 43900,
    SPELL_MOUNT_RAM_60                          = 43899,
    SPELL_MOUNT_KODO_100                        = 49379,
    SPELL_MOUNT_KODO_60                         = 49378,
    SPELL_BREWFEST_MOUNT_TRANSFORM              = 49357,
    SPELL_BREWFEST_MOUNT_TRANSFORM_REVERSE      = 52845,
};

class spell_item_brewfest_mount_transformation : public SpellScriptLoader
{
    public:
        spell_item_brewfest_mount_transformation() : SpellScriptLoader("spell_item_brewfest_mount_transformation") { }

        class spell_item_brewfest_mount_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_brewfest_mount_transformation_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNT_RAM_100) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_RAM_60) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_KODO_100) || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_KODO_60))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->HasAuraType(SPELL_AURA_MOUNTED))
                {
                    caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    uint32 spell_id;

                    switch (GetSpellInfo()->Id)
                    {
                        case SPELL_BREWFEST_MOUNT_TRANSFORM:
                            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                                spell_id = caster->GetTeam() == ALLIANCE ? SPELL_MOUNT_RAM_100 : SPELL_MOUNT_KODO_100;
                            else
                                spell_id = caster->GetTeam() == ALLIANCE ? SPELL_MOUNT_RAM_60 : SPELL_MOUNT_KODO_60;
                            break;
                        case SPELL_BREWFEST_MOUNT_TRANSFORM_REVERSE:
                            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                                spell_id = caster->GetTeam() == HORDE ? SPELL_MOUNT_RAM_100 : SPELL_MOUNT_KODO_100;
                            else
                                spell_id = caster->GetTeam() == HORDE ? SPELL_MOUNT_RAM_60 : SPELL_MOUNT_KODO_60;
                            break;
                        default:
                            return;
                    }
                    caster->CastSpell(caster, spell_id, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_brewfest_mount_transformation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_brewfest_mount_transformation_SpellScript();
        }
};

enum NitroBoots
{
    SPELL_NITRO_BOOTS_SUCCESS       = 54861,
    SPELL_NITRO_BOOTS_BACKFIRE      = 46014,
};

class spell_item_nitro_boots : public SpellScriptLoader
{
    public:
        spell_item_nitro_boots() : SpellScriptLoader("spell_item_nitro_boots") { }

        class spell_item_nitro_boots_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_nitro_boots_SpellScript);

            bool Load() override
            {
                if (!GetCastItem())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NITRO_BOOTS_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_NITRO_BOOTS_BACKFIRE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(95) ? SPELL_NITRO_BOOTS_SUCCESS : SPELL_NITRO_BOOTS_BACKFIRE, true, GetCastItem());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_nitro_boots_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_nitro_boots_SpellScript();
        }
};

enum RocketBoots
{
    SPELL_ROCKET_BOOTS_PROC      = 30452,
};

class spell_item_rocket_boots : public SpellScriptLoader
{
    public:
        spell_item_rocket_boots() : SpellScriptLoader("spell_item_rocket_boots") { }

        class spell_item_rocket_boots_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_rocket_boots_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ROCKET_BOOTS_PROC))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Battleground* bg = caster->GetBattleground())
                    bg->EventPlayerDroppedFlag(caster);

                caster->RemoveSpellCooldown(SPELL_ROCKET_BOOTS_PROC);
                caster->CastSpell(caster, SPELL_ROCKET_BOOTS_PROC, true, NULL);
            }

            SpellCastResult CheckCast()
            {
                if (GetCaster()->IsInWater())
                    return SPELL_FAILED_ONLY_ABOVEWATER;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_item_rocket_boots_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_item_rocket_boots_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_rocket_boots_SpellScript();
        }
};

enum PygmyOil
{
    SPELL_PYGMY_OIL_PYGMY_AURA      = 53806,
    SPELL_PYGMY_OIL_SMALLER_AURA    = 53805,
};

class spell_item_pygmy_oil : public SpellScriptLoader
{
    public:
        spell_item_pygmy_oil() : SpellScriptLoader("spell_item_pygmy_oil") { }

        class spell_item_pygmy_oil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_pygmy_oil_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PYGMY_OIL_PYGMY_AURA) || !sSpellMgr->GetSpellInfo(SPELL_PYGMY_OIL_SMALLER_AURA))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                if (Aura* aura = caster->GetAura(SPELL_PYGMY_OIL_PYGMY_AURA))
                    aura->RefreshDuration();
                else
                {
                    aura = caster->GetAura(SPELL_PYGMY_OIL_SMALLER_AURA);
                    if (!aura || aura->GetStackAmount() < 5 || !roll_chance_i(50))
                         caster->CastSpell(caster, SPELL_PYGMY_OIL_SMALLER_AURA, true);
                    else
                    {
                        aura->Remove();
                        caster->CastSpell(caster, SPELL_PYGMY_OIL_PYGMY_AURA, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_pygmy_oil_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_pygmy_oil_SpellScript();
        }
};

class spell_item_unusual_compass : public SpellScriptLoader
{
    public:
        spell_item_unusual_compass() : SpellScriptLoader("spell_item_unusual_compass") { }

        class spell_item_unusual_compass_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_unusual_compass_SpellScript);

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->SetFacingTo(frand(0.0f, 2.0f * M_PI));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_unusual_compass_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_unusual_compass_SpellScript();
        }
};

enum ChickenCover
{
    SPELL_CHICKEN_NET               = 51959,
    SPELL_CAPTURE_CHICKEN_ESCAPE    = 51037,
    QUEST_CHICKEN_PARTY             = 12702,
    QUEST_FLOWN_THE_COOP            = 12532,
};

class spell_item_chicken_cover : public SpellScriptLoader
{
    public:
        spell_item_chicken_cover() : SpellScriptLoader("spell_item_chicken_cover") { }

        class spell_item_chicken_cover_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_chicken_cover_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHICKEN_NET) || !sSpellMgr->GetSpellInfo(SPELL_CAPTURE_CHICKEN_ESCAPE) || !sObjectMgr->GetQuestTemplate(QUEST_CHICKEN_PARTY) || !sObjectMgr->GetQuestTemplate(QUEST_FLOWN_THE_COOP))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    if (!target->HasAura(SPELL_CHICKEN_NET) && (caster->GetQuestStatus(QUEST_CHICKEN_PARTY) == QUEST_STATUS_INCOMPLETE || caster->GetQuestStatus(QUEST_FLOWN_THE_COOP) == QUEST_STATUS_INCOMPLETE))
                    {
                        caster->CastSpell(caster, SPELL_CAPTURE_CHICKEN_ESCAPE, true);
                        target->Kill(target);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_chicken_cover_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_chicken_cover_SpellScript();
        }
};

class spell_item_muisek_vessel : public SpellScriptLoader
{
    public:
        spell_item_muisek_vessel() : SpellScriptLoader("spell_item_muisek_vessel") { }

        class spell_item_muisek_vessel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_muisek_vessel_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Creature* target = GetHitCreature())
                    if (target->isDead())
                        target->DespawnOrUnsummon();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_muisek_vessel_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_muisek_vessel_SpellScript();
        }
};

enum GreatmothersSoulcather
{
    SPELL_FORCE_CAST_SUMMON_GNOME_SOUL = 46486,
};
class spell_item_greatmothers_soulcatcher : public SpellScriptLoader
{
public:
    spell_item_greatmothers_soulcatcher() : SpellScriptLoader("spell_item_greatmothers_soulcatcher") { }

    class spell_item_greatmothers_soulcatcher_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_greatmothers_soulcatcher_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetHitUnit())
                GetCaster()->CastSpell(GetCaster(), SPELL_FORCE_CAST_SUMMON_GNOME_SOUL);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_greatmothers_soulcatcher_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_greatmothers_soulcatcher_SpellScript();
    }
};

class spell_item_stay_of_execution : public SpellScriptLoader
{
    public:
        spell_item_stay_of_execution() : SpellScriptLoader("spell_item_stay_of_execution") { }

        class spell_item_stay_of_execution_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_stay_of_execution_AuraScript);

            uint32 absorbPct, healPct;

            bool Load() override
            {
                withdrawn = 0;
                return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                absorbAmount = CalculatePct(dmgInfo.GetDamage(), 20);
                withdrawn += absorbAmount;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                int32 bp0 = int32(0.08f * withdrawn);
                GetCaster()->CastCustomSpell(GetCaster(), 96993, &bp0, 0, 0, true);
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_stay_of_execution_AuraScript::Absorb, EFFECT_0);
                OnEffectRemove += AuraEffectRemoveFn(spell_item_stay_of_execution_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            int32 withdrawn;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_item_stay_of_execution_AuraScript();
        }
};

class spell_item_flameseers_staff_flamebreaker : public SpellScriptLoader
{
    public:
        spell_item_flameseers_staff_flamebreaker() : SpellScriptLoader("spell_item_flameseers_staff_flamebreaker") { }

        class spell_item_flameseers_staff_flamebreaker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_flameseers_staff_flamebreaker_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetTarget()->GetEntry() == 38896)
                {
                    GetCaster()->CastSpell(GetTarget(), 74723, true);
                    GetTarget()->Kill(GetTarget());
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_item_flameseers_staff_flamebreaker_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_flameseers_staff_flamebreaker_AuraScript();
        }
};

enum WormholeGeneratorPandariaSpell
{
    SPELL_WORMHOLE_PANDARIA_ISLE_OF_RECKONING   = 126756,
    SPELL_WORMHOLE_PANDARIA_KUNLAI_UNDERWATER   = 126757,
    SPELL_WORMHOLE_PANDARIA_SRA_VESS            = 126758,
    SPELL_WORMHOLE_PANDARIA_RIKKITUN_VILLAGE    = 126759,
    SPELL_WORMHOLE_PANDARIA_ZANVESS_TREE        = 126760,
    SPELL_WORMHOLE_PANDARIA_ANGLERS_WHARF       = 126761,
    SPELL_WORMHOLE_PANDARIA_CRANE_STATUE        = 126762,
    SPELL_WORMHOLE_PANDARIA_EMPERORS_OMEN       = 126763,
    SPELL_WORMHOLE_PANDARIA_WHITEPETAL_LAKE     = 126764,
};

std::vector<uint32> const WormholeTargetLocations =
{
    SPELL_WORMHOLE_PANDARIA_ISLE_OF_RECKONING,
    SPELL_WORMHOLE_PANDARIA_KUNLAI_UNDERWATER,
    SPELL_WORMHOLE_PANDARIA_SRA_VESS,
    SPELL_WORMHOLE_PANDARIA_RIKKITUN_VILLAGE,
    SPELL_WORMHOLE_PANDARIA_ZANVESS_TREE,
    SPELL_WORMHOLE_PANDARIA_ANGLERS_WHARF,
    SPELL_WORMHOLE_PANDARIA_CRANE_STATUE,
    SPELL_WORMHOLE_PANDARIA_EMPERORS_OMEN,
    SPELL_WORMHOLE_PANDARIA_WHITEPETAL_LAKE
};

// 126755 - Wormhole: Pandaria
class spell_item_wormhole_pandaria : public SpellScript
{
    PrepareSpellScript(spell_item_wormhole_pandaria);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        for (uint32 spellId : WormholeTargetLocations)
            if (!sSpellMgr->GetSpellInfo(spellId))
                return false;
        return true;
    }

    void HandleTeleport(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        uint32 spellId = Trinity::Containers::SelectRandomContainerElement(WormholeTargetLocations);
        GetCaster()->CastSpell(GetHitUnit(), spellId, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_wormhole_pandaria::HandleTeleport, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 129339 - Flip It
class spell_item_flippable_table : public SpellScript
{
    PrepareSpellScript(spell_item_flippable_table);

    void HandleCast()
    {
        // With Ghostcrawler on the table. I dunno how it must work. Just some funny stuff.
        uint32 spellId = roll_chance_i(5) ? 129338 : 128783;
        GetCaster()->CastSpell(GetCaster(), spellId, false);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_item_flippable_table::HandleCast);
    }
};

enum Stardust2
{
    SPELL_STARDUST_NO_2               = 73667,
    QUEST_THE_LATEST_FASHION_ALLIANCE = 25157,
    QUEST_THE_LATEST_FASHION_HORDE    = 25159,
    NPC_THE_LATEST_FASHION_CREDIT     = 39237,
};

// 73667 - Stardust No. 2
class spell_item_stardust_no_2 : public AuraScript
{
    PrepareAuraScript(spell_item_stardust_no_2);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (player->GetQuestStatus(QUEST_THE_LATEST_FASHION_ALLIANCE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_LATEST_FASHION_HORDE) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_THE_LATEST_FASHION_CREDIT, ObjectGuid::Empty);
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_item_stardust_no_2::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum JewelcraftingDailyQuestsCata
{
    QUEST_NIBBLER_NO_ALLIANCE              = 25105,
    QUEST_ORGEZONIANS_IN_THE_MOOD_ALLIANCE = 25155,
    QUEST_A_PRESENT_FOR_LILA_ALLIANCE      = 25154,
    QUEST_NIBBLER_NO_HORDE                 = 25158,
    QUEST_ORGEZONIANS_IN_THE_MOOD_HORDE    = 25161,
    QUEST_A_PRESENT_FOR_LILA_HORDE         = 25160,

    SPELL_SOLID_ZEPHYRITE                  = 73227,
    SPELL_JAGGED_JASPER                    = 73274,
    SPELL_TIMELESS_NIGHTSTONE              = 73243,

    NPC_NIBBLER_NO_CREDIT                  = 39221,
    NPC_ORGEZONIANS_IN_THE_MOOD_CREDIT     = 39223,
    NPC_A_PRESENT_FOR_LILA_CREDIT          = 39222,
};

struct QuestData
{
    QuestData(uint32 hordeQuest, uint32 allianceQuest, uint32 credit) : HordeQuest(hordeQuest), AllianceQuest(allianceQuest), Credit(credit) { }

    uint32 HordeQuest;
    uint32 AllianceQuest;
    uint32 Credit;
};

std::map<uint32, QuestData> spellToQuest =
{
    { SPELL_SOLID_ZEPHYRITE,     QuestData(QUEST_NIBBLER_NO_ALLIANCE,              QUEST_NIBBLER_NO_HORDE,              NPC_NIBBLER_NO_CREDIT             ) },
    { SPELL_JAGGED_JASPER,       QuestData(QUEST_ORGEZONIANS_IN_THE_MOOD_ALLIANCE, QUEST_ORGEZONIANS_IN_THE_MOOD_HORDE, NPC_ORGEZONIANS_IN_THE_MOOD_CREDIT) },
    { SPELL_TIMELESS_NIGHTSTONE, QuestData(QUEST_A_PRESENT_FOR_LILA_ALLIANCE,      QUEST_A_PRESENT_FOR_LILA_HORDE,      NPC_A_PRESENT_FOR_LILA_CREDIT     ) },
};

class spell_jewelcrafting_daily_quests_cata : public SpellScript
{
    PrepareSpellScript(spell_jewelcrafting_daily_quests_cata);

    void HandleCast()
    {
        auto itr = spellToQuest.find(GetSpell()->GetSpellInfo()->Id);
        if (itr != spellToQuest.end())
        {
            Player* player = GetCaster()->ToPlayer();
            if (player->GetQuestStatus(itr->second.AllianceQuest) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(itr->second.HordeQuest) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(itr->second.Credit);
        }
    }

    void Register()
    {
        AfterCast += SpellCastFn(spell_jewelcrafting_daily_quests_cata::HandleCast);
    }
};

// 105617 - Alchemist's Flask
class spell_item_alchemists_flask : public SpellScript
{
    PrepareSpellScript(spell_item_alchemists_flask);

    void HandleCast()
    {
        Unit* caster = GetCaster();
        std::vector<float> stats{ caster->GetStat(STAT_STRENGTH), caster->GetStat(STAT_AGILITY), 0, caster->GetStat(STAT_INTELLECT) };
        auto it = std::max_element(stats.begin(), stats.end());
        uint32 stat = std::distance(stats.begin(), it);
        uint32 spellId = 0;
        switch (stat)
        {
            case STAT_STRENGTH:  spellId = 79638; break;
            case STAT_AGILITY:   spellId = 79639; break;
            case STAT_INTELLECT: spellId = 79640; break;
            default:
                return;
        }
        caster->CastSpell(caster, spellId, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_item_alchemists_flask::HandleCast);
    }
};

enum swarmKeepersSpell
{
    SPELL_SWARMKEEPERS_SPEED     = 128887,
    SPELL_SWARMKEEPERS_SPEED_EFF = 128882,
};

// Swarmkeeper`s Speed 128887
class spell_item_swarmkeepers_speed : public AuraScript
{
    PrepareAuraScript(spell_item_swarmkeepers_speed);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Need this cuz at unequip/equip - it doesn`t apply aura till area switch
        if (Player* player = GetOwner()->ToPlayer())
            player->CastSpell(player, SPELL_SWARMKEEPERS_SPEED_EFF, true);
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_item_swarmkeepers_speed::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Arrival of the Naaru Cosmetic 94444
class spell_item_arrival_of_the_naaru : public AuraScript
{
    PrepareAuraScript(spell_item_arrival_of_the_naaru);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* caster = GetOwner()->ToCreature())
            caster->CastSpell(caster, 94443, true);
    }

    void Register()
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_item_arrival_of_the_naaru::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Possible not full, only wowhead quite
const std::vector<WorldLocation> errorTeleLocations =
{
    WorldLocation(1, -7419.403f, 1727.004f, -35.594471f, 6.109519f), // Hive`Zora
    WorldLocation(0, -10216.424f, 187.672f, 1.724689f, 3.872366f), // Duskwood Catacombs
    WorldLocation(0, -4520.678f, -3240.069f, 179.561417f, 3.669699f), // Shattered Dam
    WorldLocation(1, -266.556f, 2494.944f, 19.763773f, 0.634602f), // Slitherblade Shore
    WorldLocation(1, -7208.254f, -653.120f, -234.697678f, 3.376429f), // Golakka Hot Springs
    WorldLocation(0, -6895.936f, -3375.137f, 202.327179f, 4.656716f), // Tomb of the Watchers
    WorldLocation(0, 2956.488f, -1409.799f, 176.101440f, 4.268025f), // Mardenholde Keep
    WorldLocation(1, -6886.273f, -4839.866f, 6.127418f, 1.941495f), // Steamwheedle Port
    WorldLocation(0, -6628.200f, -3895.360f, 378.730011f, 5.073192f), // Fuselight by the Sea
    WorldLocation(530, -1050.882f, 3046.176f, 433.985565f, 5.531922f), // Floating Rock (Hellfire Peninsula)
    WorldLocation(1, -1630.870f, -4244.494f, 4.429177f, 2.357727f), // Fray Island
    WorldLocation(1, -8008.594f, -5286.748f, 0.575901f, 1.862948f), // Lost Rigger Cove
    WorldLocation(1, 6243.650f, -1202.918f, 370.821869f, 2.177830f), // Irontree Woods
    WorldLocation(1, 7958.594f, -2492.875f, 487.918427f, 3.233107f), // Nighthaven
    WorldLocation(0, 1252.864f, -2586.790f, 92.886726f, 0.455692f), // Scholomance
    WorldLocation(0, -14808.640f, -165.362f, 5.233351f, 1.882970f), // Jaguero Isle
    WorldLocation(1, -3750.070f, 1090.165f, 131.969452f, 6.259622f), // Broken Commons
    WorldLocation(1, -686.694f, -2240.777f, 17.159506f, 0.837993f), // Cavern of Mists
};

// Teleport with Error 82674
class spell_item_teleport_with_error : public SpellScript
{
    PrepareSpellScript(spell_item_teleport_with_error);

    void HandleAfterCast()
    {
        if (Player* caster = GetCaster()->ToPlayer())
            caster->TeleportTo(Trinity::Containers::SelectRandomContainerElement(errorTeleLocations));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_item_teleport_with_error::HandleAfterCast);
    }
};

// Taunt Flag Targeting 51640
class spell_item_taunt_flag_targeting : public SpellScript
{
    PrepareSpellScript(spell_item_taunt_flag_targeting);

    enum
    {
        SPELL_TAUNT_FLAG_DEAD       = 51657,
        SPELL_TAUNT_FLAG_CORPSE     = 52605,
        GO_THE_FLAG_OF_OWNERSHIP    = 190589,
    };

    struct TextBuilder
    {
        TextBuilder(Player* source, int32 id, WorldObject* target) : _source(source), _textId(id), _target(target) { }

        size_t operator()(WorldPacket* data, LocaleConstant locale) const
        {
            std::string text = sObjectMgr->GetTrinityString(_textId, locale);
            return ChatHandler::BuildChatPacket(*data, CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, _source->GetGUID(), _target->GetGUID(), text, 0,
                _source->GetNameForLocaleIdx(locale), _target->GetNameForLocaleIdx(locale));
        }

        Player* _source;
        int32 _textId;
        WorldObject* _target;
    };

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* target) -> bool
        {
            if (target->GetTypeId() == TYPEID_CORPSE)
                return target->FindNearestGameObject(GO_THE_FLAG_OF_OWNERSHIP, 1);
            else if (Unit* unit = target->ToPlayer())
                return unit->HasAura(SPELL_TAUNT_FLAG_DEAD) || unit->IsAlive();
            return true;
        });

        if (targets.empty())
            FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
        else
        {
            targets.sort(Trinity::ObjectDistanceOrderPred{ GetCaster() });
            targets.resize(1);
        }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        WorldObject* target = GetHitUnit();
        if (!target)
            target = GetHitCorpse();
        if (target->GetTypeId() == TYPEID_CORPSE)
            GetCaster()->CastSpell(*target, SPELL_TAUNT_FLAG_CORPSE, true);
        else
            GetCaster()->CastSpell(target->ToUnit(), SPELL_TAUNT_FLAG_DEAD, true);

        if (Player* player = GetCaster()->ToPlayer())
            sCreatureTextMgr->SendChatPacket(player, TextBuilder(player, LANG_TAUNT_FLAG, target), CHAT_MSG_MONSTER_EMOTE);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_taunt_flag_targeting::SelectTarget, EFFECT_0, TARGET_CORPSE_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_item_taunt_flag_targeting::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum ChaliceMountainKingsMisc
{
    SPELL_SWORD_DANCE          = 94690, // summon invisible stalker
    SPELL_SUMMON_SWORD_DANCERS = 94691,
    NPC_SWORD_DANCER_A         = 51269,
    NPC_SWORD_DANCER_B         = 51270,
    NPC_SWORD_DANCER_C         = 51271,
    NPC_SWORD_DANCER_D         = 51272,
};

// Summon Sword Dancers 94691
class spell_item_summon_sword_dancers : public SpellScript
{
    PrepareSpellScript(spell_item_summon_sword_dancers);

    void HandleScriptEff(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetHitUnit())
        {
            faceOri = frand(0, 2 * M_PI);

            for (uint32 i = 0; i < 4; i++)
            {
                GetPositionWithDistInOrientation(caster, 3.5f, Position::NormalizeOrientation(faceOri + i * (M_PI / 2)), x, y);
                caster->SummonCreature(NPC_SWORD_DANCER_A + i, x, y, caster->GetPositionZ(), Position::NormalizeOrientation(faceOri + i * (M_PI / 2) - M_PI), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(18 * IN_MILLISECONDS));
            }
        }
    }

    private:
        float x, y, faceOri;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_summon_sword_dancers::HandleScriptEff, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum SparkOfZandalar
{
    SPELL_PROC_AURA_SPARK_OF_ZANDALAR = 138957, // same for HC, LFR
    SPELL_SPARK_OF_ZANDALAR           = 138958, // accumulate to 10
    SPELL_SPARK_OF_ZANDALAR_TRANSFORM = 138960,
};

// 138957 - Item - Proc Charges For Strength Transform
class spell_item_spark_of_zandalari_proc_aura : public AuraScript
{
    PrepareAuraScript(spell_item_spark_of_zandalari_proc_aura);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (!GetUnitOwner()->HasAura(SPELL_SPARK_OF_ZANDALAR_TRANSFORM))
        {
            Item* item = GetUnitOwner()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_SPARK_OF_ZANDALAR, true, item);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_item_spark_of_zandalari_proc_aura::HandleOnProc);
    }
};

// Spark of Zandalar 138958
class spell_item_spark_of_zandalari : public AuraScript
{
    PrepareAuraScript(spell_item_spark_of_zandalari);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() > 9)
        {
            Item* item = GetUnitOwner()->ToPlayer()->GetItemByGuid(GetAura()->GetCastItemGUID());
            GetUnitOwner()->RemoveAurasDueToSpell(SPELL_SPARK_OF_ZANDALAR);
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_SPARK_OF_ZANDALAR_TRANSFORM, true, item);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_item_spark_of_zandalari::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

enum zuldazarRelicsSpell
{
    SPELL_PROC_AURA_RELIC_OF_ZULDAZAR = 138968, // same for HC, LFR
    SPELL_ZANDALARI_WARDING           = 138925,
    SPELL_BLESSING_OF_ZULDAZAR        = 138967,
};

// Relic of Zuldazar 138925
class spell_item_relic_of_zuldazar_shield : public AuraScript
{
    PrepareAuraScript(spell_item_relic_of_zuldazar_shield);

    void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
            if (Aura* zuldazar = caster->GetAura(SPELL_BLESSING_OF_ZULDAZAR))
                amount *= zuldazar->GetStackAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_relic_of_zuldazar_shield::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// Relic of Zuldazar 138925
class spell_item_relic_of_zuldazar : public SpellScript
{
    PrepareSpellScript(spell_item_relic_of_zuldazar);

    void HandleHitEff(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_BLESSING_OF_ZULDAZAR);
    }

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->GetAura(SPELL_BLESSING_OF_ZULDAZAR) && caster->IsInCombat())
                return SPELL_CAST_OK;

        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_relic_of_zuldazar::HandleHitEff, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnCheckCast += SpellCheckCastFn(spell_item_relic_of_zuldazar::CheckCast);
    }
};

enum lightningChalicesSpell
{
    SPELL_PROC_AURA_LIGHTNING_CHALICE = 139190,
    SPELL_INFINITE_POWER              = 139189,
    SPELL_RESTORATION_OF_THE_INFINITE = 139195,
};

// Lightning Chalice Proc Aura 139190
class spell_item_lightning_chalice_proc_aura : public AuraScript
{
    PrepareAuraScript(spell_item_lightning_chalice_proc_aura);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            Item* item = owner->GetItemByGuid(GetAura()->GetCastItemGUID());
            owner->CastSpell(owner, SPELL_INFINITE_POWER, true, item);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_item_lightning_chalice_proc_aura::HandleProc);
    }
};

// Infinite Power 139189
class spell_infinite_power : public AuraScript
{
    PrepareAuraScript(spell_infinite_power);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() > 5)
        {
            if (Player* owner = GetOwner()->ToPlayer())
            {
                owner->RemoveAurasDueToSpell(SPELL_INFINITE_POWER);
                Item* item = owner->GetItemByGuid(GetAura()->GetCastItemGUID());
                owner->CastSpell(owner, SPELL_RESTORATION_OF_THE_INFINITE, true, item);
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_infinite_power::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Hardened Shell 129787
class spell_item_hardened_shell : public SpellScript
{
    PrepareSpellScript(spell_item_hardened_shell);

    void HandleHitEff(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->RemoveAurasByType(SPELL_AURA_MOUNTED);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_hardened_shell::HandleHitEff, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 138979 - Soul Barrier
class spell_item_soul_barrier : public AuraScript
{
    PrepareAuraScript(spell_item_soul_barrier);

    uint32 totalAbsorbed = 0;
    uint32 perHit = 0;

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        perHit = amount;
        amount *= 5;
    }

    void HandleAbsorb(AuraEffect* eff, DamageInfo& dmgInfo, uint32& absorbedDamage)
    {
        absorbedDamage = std::min(dmgInfo.GetDamage(), perHit);
        if (totalAbsorbed + absorbedDamage > 5 * perHit)
            absorbedDamage = 5 * perHit - totalAbsorbed;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_soul_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_soul_barrier::HandleAbsorb, EFFECT_0);
    }
};

// 138972 - Item - Proc Heal Below 35%
class spell_item_proc_ji_kuns_rising_winds : public AuraScript
{
    PrepareAuraScript(spell_item_proc_ji_kuns_rising_winds);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo()->GetDamage())
            return false;
        return GetUnitOwner()->HealthBelowPctDamaged(35, eventInfo.GetDamageInfo()->GetDamage());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_item_proc_ji_kuns_rising_winds::CheckProc);
    }
};

// 138756 - Blades of Renataki
// 138759 - Feathers of Fury
// 138786 - Wushoolay's Lightning
class spell_item_tot_stack_trinket : public AuraScript
{
    PrepareAuraScript(spell_item_tot_stack_trinket);

    uint32 GetEffectSpellId() const
    {
        switch (GetSpellInfo()->Id)
        {
            case 138756: return 138737;
            case 138786: return 138788;
            case 138759: return 138760;
        }
        return 0;
    }

    void HandlePeriodic(AuraEffect const* eff)
    {
        if (int32(eff->GetTickNumber()) > eff->GetTotalTicks()) // First at apply
            return;

        if (Aura* aura = GetUnitOwner()->GetAura(GetEffectSpellId()))
            aura->SetStackAmount(aura->GetStackAmount() + 1);
        else
        {
            Item* castItem = nullptr;
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                    castItem = player->GetItemByGuid(GetAura()->GetCastItemGUID());

            GetUnitOwner()->CastSpell(GetUnitOwner(), GetEffectSpellId(), true, castItem, eff);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_tot_stack_trinket::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 139116 - Item - Attacks Proc Highest Rating
class spell_item_rune_of_re_organization : public AuraScript
{
    PrepareAuraScript(spell_item_rune_of_re_organization);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Player* player = GetUnitOwner()->ToPlayer();
        std::array<int32, 3> ratings
        {
            int32(player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_MELEE)),
            int32(player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_MASTERY)),
            int32(player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_HASTE_MELEE))
        };
        auto it = std::max_element(ratings.begin(), ratings.end());
        uint32 spellId = 0;
        int32 bp0 = 0, bp1 = 0, bp2 = 0;
        switch (std::distance(ratings.begin(), it))
        {
            case 0:
                spellId = 139117;
                bp0 = (ratings[1] + ratings[2]) * 2;
                bp1 = -ratings[1];
                bp2 = -ratings[2];
                break;
            case 1:
                spellId = 139120;
                bp0 = (ratings[0] + ratings[2]) * 2;
                bp1 = -ratings[0];
                bp2 = -ratings[2];
                break;
            case 2:
                spellId = 139121;
                bp0 = (ratings[1] + ratings[0]) * 2;
                bp1 = -ratings[1];
                bp2 = -ratings[0];
                break;
        }
        player->CastCustomSpell(player, spellId, &bp0, &bp1, &bp2, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_item_rune_of_re_organization::HandleProc);
    }
};

// 143743 - Jard's Peculiar Energy Source
class spell_chief_engineer_jards_journal : public SpellScript
{
    PrepareSpellScript(spell_chief_engineer_jards_journal);

    void HandleCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            player->LearnSpell(139176, false); // Jard's Peculiar Energy Source
            player->LearnSpell(139192, false); // Sky Golem
            player->LearnSpell(139197, false); // Advanced Refrigeration Unit
            player->LearnSpell(139196, false); // Pierre
            player->LearnSpell(143714, false); // Rascal-Bot
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_chief_engineer_jards_journal::HandleCast);
    }
};

// 143644 - Hardened Magnificent Hide and Its Uses
// 143626 - Celestial Cloth and Its Uses
// 143646 - Balanced Trillium Ingot and Its Uses
class spell_learn_some_ingredient_and_its_uses : public SpellScript
{
    PrepareSpellScript(spell_learn_some_ingredient_and_its_uses);

    void HandleCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            uint32 spellId = 0;

            switch (GetSpellInfo()->Id)
            {
                case 143644: spellId = 142976; break;
                case 143626: spellId = 143011; break;
                case 143646: spellId = 143255; break;
                default:
                    return;
            }

            player->LearnSpell(spellId, false);

            if (uint32 recipe = GetExplicitDiscoverySpell(spellId, player))
                player->LearnSpell(recipe, false);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_learn_some_ingredient_and_its_uses::HandleCast);
    }
};

const std::vector<uint32> memorySpellsId = { 127196, 127205, 127206 };

// Memory of Mr. Smite 127207
class spell_memory_of_mr_smite : public AuraScript
{
    PrepareAuraScript(spell_memory_of_mr_smite);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, Trinity::Containers::SelectRandomContainerElement(memorySpellsId), true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_memory_of_mr_smite::OnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Potion of Illusion 80265
class spell_item_potion_of_illusion : public SpellScript
{
    PrepareSpellScript(spell_item_potion_of_illusion);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster())
            return;

        targets.remove(GetCaster());
        targets.remove_if(IllusionSourceCheck());
        if (GetCaster()->GetTypeId() == TYPEID_PLAYER)
        {
            if (Unit* selection = GetCaster()->ToPlayer()->GetSelectedUnit())
            {
                if (std::find(targets.begin(), targets.end(), selection) != targets.end())
                {
                    targets.clear();
                    targets.push_back(selection);
                    return;
                }
            }
        }
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(caster, GetEffectValue(), true, NULL, NULL, target->GetGUID());
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_potion_of_illusion::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_item_potion_of_illusion::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }

private:
    struct IllusionSourceCheck
    {
        bool operator()(WorldObject* u)
        {
            if (Unit* unit = u->ToUnit())
            {
                if (unit->GetFloatValue(OBJECT_FIELD_SCALE) <= 0)
                    return true;

                if (!unit->IsVisible())
                    return true;

                switch (unit->GetDisplayId())
                {
                    case 169:
                    case 1126:
                    case 11686:
                        return true;
                    default:
                    {
                        CreatureDisplayInfoEntry const* info = sCreatureDisplayInfoStore.LookupEntry(unit->GetDisplayId());
                        if (!info || !info->ModelID)
                            return true;

                        CreatureModelDataEntry const* model = sCreatureModelDataStore.LookupEntry(info->ModelID);
                        if (!model)
                            return true;

                        if (strstr(model->ModelName, "INVISIBLESTALKER"))
                            return true;

                        return false;
                    }
                }
            }

            return true;
        }
    };
};

class spell_item_multistrike_trinket : public AuraScript
{
    PrepareAuraScript(spell_item_multistrike_trinket);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        if (!roll_chance_f(effect->GetFloatAmount() / 10.0f))
            return;

        int32 damage = eventInfo.GetDamage() / 3;
        uint32 spellId = GetSpellId(eventInfo.GetSpellInfo());
        GetUnitOwner()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetActionTarget(), true);
    }

    virtual uint32 GetSpellId(SpellInfo const* trigger) = 0;

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_item_multistrike_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 146059 - Multistrike
class spell_item_multistrike_trinket_damage : public spell_item_multistrike_trinket
{
    uint32 GetSpellId(SpellInfo const* trigger) override
    {
        // I don't know how this must work
        if (!trigger)
            return 146061;
        if (trigger->SchoolMask == SPELL_SCHOOL_MASK_NORMAL && trigger->DmgClass == SPELL_DAMAGE_CLASS_MELEE)
            return 146061;

        switch (GetUnitOwner()->GetClass())
        {
            case CLASS_PALADIN:
                if (trigger->SchoolMask & SPELL_SCHOOL_HOLY)
                    return 146063;
                break;
            case CLASS_HUNTER:
                return 146069;
            case CLASS_PRIEST:
                if (trigger->SchoolMask & SPELL_SCHOOL_HOLY)
                    return 146063;
                return 146065;
            case CLASS_SHAMAN:
                return 146071;
            case CLASS_MAGE:
                if (trigger->SchoolMask & SPELL_SCHOOL_MASK_ARCANE)
                    return 146070;
                return 146067;
            case CLASS_WARLOCK:
                return 146065;
            case CLASS_DRUID:
                return 146064;
            case CLASS_MONK:
                return 146075;
        }
        // By default - melee
        return 146061;
    }
};

// 146176 - Multistrike
class spell_item_multistrike_trinket_heal : public spell_item_multistrike_trinket
{
    uint32 GetSpellId(SpellInfo const* trigger) override
    {
        // I don't know how this must work
        return trigger->SchoolMask & SPELL_SCHOOL_MASK_NATURE ? 146178 : 146177;
    }
};

class spell_item_cooldown_reduce_trinket : public AuraScript
{
    PrepareAuraScript(spell_item_cooldown_reduce_trinket);

    uint32 spellId = 0;

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        spellId = GetSpell();
        if (spellId)
        {
            float amount = -(1.0f - 1.0f / (1.0f + effect->GetFloatAmount() / 100.0f)) * 100.0f;
            GetUnitOwner()->CastSpell(GetUnitOwner(), spellId, true);
            if (AuraEffect* spellMod = GetUnitOwner()->GetAuraEffect(spellId, EFFECT_0))
                spellMod->ChangeAmount(amount);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (spellId)
            GetUnitOwner()->RemoveAurasDueToSpell(spellId);
    }

    virtual uint32 GetSpell() = 0;

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_item_cooldown_reduce_trinket::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_item_cooldown_reduce_trinket::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 145955 - Readiness
class spell_item_evil_eye_of_galakras : public spell_item_cooldown_reduce_trinket
{
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
        }
        return 0;
    }
};

// 146019 - Readiness
class spell_item_assurance_of_consequence : public spell_item_cooldown_reduce_trinket
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
        }
        return 0;
    }
};

// 146025 - Readiness
class spell_item_vial_of_living_corruption : public spell_item_cooldown_reduce_trinket
{
    uint32 GetSpell() override
    {
        switch (GetUnitOwner()->ToPlayer()->GetSpecialization())
        {
            case SPEC_DEATH_KNIGHT_BLOOD:   return 145958;
            case SPEC_DRUID_GUARDIAN:       return 145962;
            case SPEC_MONK_BREWMASTER:      return 145967;
            case SPEC_PALADIN_PROTECTION:   return 145976;
            case SPEC_WARRIOR_PROTECTION:   return 145992;
        }
        return 0;
    }
};

// 138728 - Change of Tactics
// 146310 - Restless Agility
// 146317 - Restless Spirit
class spell_item_restless_stat_trinket : public AuraScript
{
    PrepareAuraScript(spell_item_restless_stat_trinket);

    bool Load() override
    {
        InitStackAmount(GetAura()->GetMaxStackAmount());
        return true;
    }

    void HandleTick(AuraEffect const*)
    {
        ModStackAmount(-1);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_restless_stat_trinket::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 146285 - Cruelty
// 146184 - Wrath of the Darkspear
class spell_item_soo_stack_trinket : public AuraScript
{
    PrepareAuraScript(spell_item_soo_stack_trinket);

    uint32 GetEffectSpellId() const
    {
        switch (GetSpellInfo()->Id)
        {
            case 146285: return 146293;
            case 146184: return 146202;
        }
        return 0;
    }

    void HandlePeriodic(AuraEffect const* effect)
    {
        if (int32(effect->GetTickNumber()) > effect->GetTotalTicks()) // First at apply
            return;

        // Yes, cast every tick
        Item* castItem = nullptr;
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                castItem = player->GetItemByGuid(GetAura()->GetCastItemGUID());

        GetUnitOwner()->CastCustomSpell(GetEffectSpellId(), SPELLVALUE_BASE_POINT0, effect->GetAmount(), GetUnitOwner(), true, castItem, effect);
    }

    void HandleApply(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (GetSpellInfo()->Id == 146285) // Cruelty, no tick at apply
            HandlePeriodic(effect);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->RemoveAurasDueToSpell(GetEffectSpellId());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_soo_stack_trinket::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectRemoveFn(spell_item_soo_stack_trinket::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_item_soo_stack_trinket::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_item_soo_trinket_aoe : public AuraScript
{
    PrepareAuraScript(spell_item_soo_trinket_aoe);

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        if (!roll_chance_f(effect->GetFloatAmount() / 100.0f))
            return;

        uint32 spellId = GetSpellId(eventInfo.GetSpellInfo());
        GetUnitOwner()->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, eventInfo.GetDamage(), eventInfo.GetActionTarget(), true);
    }

    virtual uint32 GetSpellId(SpellInfo const*) = 0;

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_item_soo_trinket_aoe::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 146136 - Cleave
class spell_item_soo_trinket_aoe_damage : public spell_item_soo_trinket_aoe
{
    uint32 GetSpellId(SpellInfo const* trigger) override
    {
        // Same logic as multistrike
        if (!trigger)
            return 146137;
        if (trigger->SchoolMask == SPELL_SCHOOL_MASK_NORMAL && trigger->DmgClass == SPELL_DAMAGE_CLASS_MELEE)
            return 146137;

        switch (GetUnitOwner()->GetClass())
        {
            case CLASS_PALADIN:
                if (trigger->SchoolMask & SPELL_SCHOOL_HOLY)
                    return 146157;
                break;
            case CLASS_HUNTER:
                return 146162;
            case CLASS_PRIEST:
                if (trigger->SchoolMask & SPELL_SCHOOL_HOLY)
                    return 146157;
                return 146159;
            case CLASS_SHAMAN:
                return 146171;
            case CLASS_MAGE:
                if (trigger->SchoolMask & SPELL_SCHOOL_MASK_ARCANE)
                    return 146166;
                return 146160;
            case CLASS_WARLOCK:
                return 146159;
            case CLASS_DRUID:
                return 146158;
            case CLASS_MONK:
                return 146172;
        }
        // By default - melee
        return 146137;
    }
};

// 148233 - Cleave
class spell_item_soo_trinket_aoe_heal : public spell_item_soo_trinket_aoe
{
    uint32 GetSpellId(SpellInfo const* trigger) override
    {
        // I don't know how this must work
        return trigger->SchoolMask & SPELL_SCHOOL_MASK_NATURE ? 148235 : 148234;
    }
};

// 146137, 146157, 146158, 146159, 146160, 146162, 146166, 146171, 146172, 148234, 148235
class spell_item_soo_trinket_aoe_effect : public SpellScript
{
    PrepareSpellScript(spell_item_soo_trinket_aoe_effect);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetUnit());
    }

    void Register() override
    {
        auto const* spellInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_soo_trinket_aoe_effect::FilterTargets, EFFECT_0, spellInfo->Effects[EFFECT_0].TargetB.GetTarget());
    }
};

// 146346 - Life Steal
class spell_item_juggernauts_focusing_crystal : public AuraScript
{
    PrepareAuraScript(spell_item_juggernauts_focusing_crystal);

    enum { SPELL_LIFE_STEAL = 146347 };

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        if (int32 heal = CalculatePct(eventInfo.GetDamage(), effect->GetFloatAmount() / 100.0f))
            GetUnitOwner()->CastCustomSpell(SPELL_LIFE_STEAL, SPELLVALUE_BASE_POINT0, heal, GetUnitOwner(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_item_juggernauts_focusing_crystal::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

std::vector<std::pair<uint32, uint32>> legendaryCloakAnimations
{
    { 146195, 148957 },
    { 146197, 148954 },
    { 146199, 148956 },
    { 146193, 148958 },
};

// 146195 - Flurry of Xuen
// 146197 - Essence of Yu'lon
// 146199 - Spirit of Chi-Ji
// 146193 - Endurance of Niuzao
class spell_item_legendary_cloak : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (GetUnitOwner()->GetMap()->IsDungeon())
            return true;
        if (GetUnitOwner()->GetMap()->IsBattlegroundOrArena())
            return false;
        if (GetId() != 146199 && eventInfo.GetActionTarget() && eventInfo.GetActionTarget()->GetAffectingPlayer())
            return false;
        return true;
    }

    void CalculatePeriodic(AuraEffect const*, bool&, int32& amplitude)
    {
        amplitude = GetId() == 146195 || GetId() == 146197 ? 55000 : 50000;
    }

    void HandleTick(AuraEffect const*)
    {
        if (!GetUnitOwner()->IsInCombat())
            for (auto&& itr : legendaryCloakAnimations)
                if (itr.first == GetId())
                    GetUnitOwner()->CastSpell(GetUnitOwner(), itr.second, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_item_legendary_cloak::CheckProc);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_item_legendary_cloak::CalculatePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_legendary_cloak::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 148957 - Flurry of Xuen
// 148954 - Essence of Yu'lon
// 148956 - Spirit of Chi-Ji
// 148958 - Endurance of Niuzao
class spell_item_legendary_cloak_animation_reset : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_animation_reset);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        for (auto&& itr : legendaryCloakAnimations)
            if (GetId() == itr.second)
                if (AuraEffect* periodic = GetUnitOwner()->GetAuraEffect(itr.first, EFFECT_0))
                    periodic->ResetPeriodic();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_item_legendary_cloak_animation_reset::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 146194 - Flurry of Xuen
class spell_item_legendary_cloak_flurry_of_huen : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_flurry_of_huen);

    void HandleTriggerMelee(AuraEffect const* effect)
    {
        if (GetUnitOwner()->GetClass() == CLASS_HUNTER)
            PreventDefaultAction();
        else if (effect->GetTickNumber() % 3 == 0)
            GetUnitOwner()->SendPlaySpellVisual(33874, GetUnitOwner()->GetGUID(), 0.0f);
    }

    void HandleTriggerRanged(AuraEffect const* effect)
    {
        PreventDefaultAction();
        if (GetUnitOwner()->GetClass() == CLASS_HUNTER)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*GetUnitOwner(), GetUnitOwner()->GetTarget()))
                if (GetUnitOwner()->IsValidAttackTarget(target, sSpellMgr->GetSpellInfo(effect->GetSpellEffectInfo().TriggerSpell)))
                    GetUnitOwner()->CastSpell(target, effect->GetSpellEffectInfo().TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_legendary_cloak_flurry_of_huen::HandleTriggerMelee, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_legendary_cloak_flurry_of_huen::HandleTriggerRanged, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

enum 
{
    SPELL_ESSENCE_OF_YU_LON_DAMAGE   = 148008,
};

// 146197 - Essence of Yu'lon
class spell_item_legendary_cloak_essence_of_yu_lon : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_essence_of_yu_lon);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActionTarget())
            return false;

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_ESSENCE_OF_YU_LON_DAMAGE)
            return false;
        return true;
    }

    void HandleProc(AuraEffect const* effect, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (auto const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ESSENCE_OF_YU_LON_DAMAGE))
        {
            int32 amount = GetUnitOwner()->SpellDamageBonusDone(eventInfo.GetActionTarget(), spellInfo, EFFECT_0, spellInfo->Effects[EFFECT_0].BasePoints, DOT);
            GetUnitOwner()->CastCustomSpell(effect->GetSpellEffectInfo().TriggerSpell, SPELLVALUE_BASE_POINT0, amount / 5, eventInfo.GetActionTarget(), true);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_item_legendary_cloak_essence_of_yu_lon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_item_legendary_cloak_essence_of_yu_lon::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 146198 - Essence of Yu'lon
class spell_item_legendary_cloak_essence_of_yu_lon_periodic : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_essence_of_yu_lon_periodic);

    void HandleTick(AuraEffect const* effect)
    {
        if (Unit* caster = GetCaster())
            caster->CastCustomSpell(SPELL_ESSENCE_OF_YU_LON_DAMAGE, SPELLVALUE_BASE_POINT0, effect->GetAmount(), GetTarget(), true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_legendary_cloak_essence_of_yu_lon_periodic::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 146193 - Endurance of Niuzao
class spell_item_legendary_cloak_endurance_of_niuzao : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_endurance_of_niuzao);

    enum { SPELL_ENDURANCE_OF_NIUZAO = 148010 };

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void CalcualteAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -1;
    }

    void HandleAbsorb(AuraEffect*, DamageInfo& damageInfo, uint32& absorbed)
    {
        if (GetUnitOwner()->ToPlayer()->GetRoleForGroup(GetUnitOwner()->ToPlayer()->GetSpecialization()) != ROLES_TANK)
            return;

        if (!GetUnitOwner()->GetMap()->IsDungeon())
            if ((damageInfo.GetAttacker() && damageInfo.GetAttacker()->GetAffectingPlayer()) || GetUnitOwner()->GetMap()->IsBattlegroundOrArena())
                return;

        if (damageInfo.GetDamage() >= GetUnitOwner()->GetHealth())
        {
            if (GetUnitOwner()->HasAura(SPELL_ENDURANCE_OF_NIUZAO))
                return;
            absorbed = damageInfo.GetDamage();
            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_ENDURANCE_OF_NIUZAO, true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_legendary_cloak_endurance_of_niuzao::CalcualteAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_item_legendary_cloak_endurance_of_niuzao::HandleAbsorb, EFFECT_0);
    }
};

// 146199 - Spirit of Chi-Ji
class spell_item_legendary_cloak_spirit_of_chi_ji_proc : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_spirit_of_chi_ji_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetHealInfo()->GetOverheal() != eventInfo.GetHeal();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_item_legendary_cloak_spirit_of_chi_ji_proc::CheckProc);
    }
};

// 146200 - Spirit of Chi-Ji
class spell_item_legendary_cloak_spirit_of_chi_ji : public AuraScript
{
    PrepareAuraScript(spell_item_legendary_cloak_spirit_of_chi_ji);

    enum { SPELL_SPIRIT_OF_CHI_JI_HEAL = 148009 };

    uint32 overhealAmount = 0;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        overhealAmount += eventInfo.GetHealInfo()->GetOverheal();
    }

    void HandleTick(AuraEffect const*)
    {
        if (overhealAmount)
        {
            GetUnitOwner()->CastCustomSpell(SPELL_SPIRIT_OF_CHI_JI_HEAL, SPELLVALUE_BASE_POINT0, overhealAmount, GetUnitOwner(), true);
            overhealAmount = 0;
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_item_legendary_cloak_spirit_of_chi_ji::HandleProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_item_legendary_cloak_spirit_of_chi_ji::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 148009 - Spirit of Chi-Ji
class spell_item_spirit_of_chi_ji_heal : public spell_smart_heal
{
    PrepareSpellScript(spell_item_spirit_of_chi_ji_heal);

    uint32 targetCount = 0;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, 5);
        targetCount = targets.size();
    }

    void HandleHit()
    {
        if (targetCount)
            SetHitHeal(GetHitHeal() / targetCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_spirit_of_chi_ji_heal::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnHit += SpellHitFn(spell_item_spirit_of_chi_ji_heal::HandleHit);
    }
};

enum MedalionSpells
{
    CELESTIAL_DEFENDER_MALE   = 148369,
    CELESTIAL_DEFENDER_FEMALE = 149229
};

// Celestial Defender's Medallion - 103685
class spell_item_celestial_defender : public SpellScript
{
    PrepareSpellScript(spell_item_celestial_defender);

    void HandleDummy(SpellEffIndex /* index */)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, caster->GetGender() == GENDER_MALE ? CELESTIAL_DEFENDER_MALE : CELESTIAL_DEFENDER_FEMALE, true);
    }

    void Register()
    {
        OnEffectHit += SpellEffectFn(spell_item_celestial_defender::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spectral Grog - 104316
class spell_item_spectral_grog : public SpellScript
{
    PrepareSpellScript(spell_item_spectral_grog);

    void HandleDummy(SpellEffIndex /*index*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, caster->GetGender() == GENDER_MALE ? 148564 : 148563, true);
    }

    void Register()
    {
        OnEffectHit += SpellEffectFn(spell_item_spectral_grog::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 127284 - Symbiotic Growth
class spell_item_symbiotic_growth : public SpellScript
{
    PrepareSpellScript(spell_item_symbiotic_growth);

    void HandleCast()
    {
        if (uint32 overrideAura = GetAuraId())
            GetCaster()->CastSpell(GetCaster(), overrideAura, true);
    }

    uint32 GetAuraId()
    {
        switch (GetCaster()->GetClass())
        {
            case CLASS_DEATH_KNIGHT:
                return 129712;
            case CLASS_HUNTER:
                return 129713;
            case CLASS_MAGE:
                return 129714;
            case CLASS_MONK:
                return 129715;
            case CLASS_PALADIN:
                return 129716;
            case CLASS_PRIEST:
                return 129717;
            case CLASS_ROGUE:
                return 129718;
            case CLASS_SHAMAN:
                return 129719;
            case CLASS_WARLOCK:
                return 129720;
            case CLASS_WARRIOR:
                return 129721;
            default:
                break;
        }
        return 0;
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_item_symbiotic_growth::HandleCast);
    }
};

// 129712 - Symbiotic Growth
// 129713 - Symbiotic Growth
// 129714 - Symbiotic Growth
// 129715 - Symbiotic Growth
// 129716 - Symbiotic Growth
// 129717 - Symbiotic Growth
// 129718 - Symbiotic Growth
// 129719 - Symbiotic Growth
// 129720 - Symbiotic Growth
// 129721 - Symbiotic Growth
class spell_item_symbiotic_growth_aura : public AuraScript
{
    PrepareAuraScript(spell_item_symbiotic_growth_aura);

    uint32 spec = 0;

    bool Load() override
    {
        if (GetUnitOwner()->GetTypeId() != TYPEID_PLAYER)
            return false;

        spec = GetUnitOwner()->ToPlayer()->GetSpecialization();
        return true;
    }

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = GetSpellId();
    }

    void Update(uint32 diff)
    {
        if (GetCaster()->ToPlayer()->GetSpecialization() != spec)
            Remove();
    }

    uint32 GetSpellId()
    {
        switch (GetCaster()->ToPlayer()->GetSpecialization())
        {
            case SPEC_WARRIOR_ARMS:
            case SPEC_WARRIOR_FURY:
                return 122294;
            case SPEC_WARRIOR_PROTECTION:
                return 122286;
            case SPEC_PALADIN_HOLY:
                return 113269;
            case SPEC_PALADIN_PROTECTION:
                return 113075;
            case SPEC_PALADIN_RETRIBUTION:
                return 122287;
            case SPEC_HUNTER_BEAST_MASTERY:
            case SPEC_HUNTER_MARKSMANSHIP:
            case SPEC_HUNTER_SURVIVAL:
                return 113073;
            case SPEC_ROGUE_ASSASSINATION:
            case SPEC_ROGUE_COMBAT:
            case SPEC_ROGUE_SUBTLETY:
                return 113613;
            case SPEC_PRIEST_SHADOW:
                return 113277;
            case SPEC_PRIEST_HOLY:
                return 113275;
            case SPEC_PRIEST_DISCIPLINE:
                return 113506;
            case SPEC_DEATH_KNIGHT_FROST:
            case SPEC_DEATH_KNIGHT_UNHOLY:
                return 113516;
            case SPEC_DEATH_KNIGHT_BLOOD:
                return 113072;
            case SPEC_SHAMAN_ELEMENTAL:
            case SPEC_SHAMAN_ENHANCEMENT:
                return 113286;
            case SPEC_SHAMAN_RESTORATION:
                return 113289;
            case SPEC_MAGE_ARCANE:
            case SPEC_MAGE_FIRE:
            case SPEC_MAGE_FROST:
                return 113074;
            case SPEC_WARLOCK_AFFLICTION:
            case SPEC_WARLOCK_DEMONOLOGY:
            case SPEC_WARLOCK_DESTRUCTION:
                return 113295;
            case SPEC_MONK_BREWMASTER:
                return 113306;
            case SPEC_MONK_WINDWALKER:
                return 127361;
            case SPEC_MONK_MISTWEAVER:
                return 113275;
        }
        return 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_item_symbiotic_growth_aura::CalculateAmount, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    }
};

// 146315 - Item - Heals Proc Int
class spell_item_heals_proc_int : public AuraScript
{
    PrepareAuraScript(spell_item_heals_proc_int);

    bool CheckProc(ProcEventInfo&)
    {
        if (Player* player = GetUnitOwner()->ToPlayer())
            if (player->GetRoleForGroup(player->GetSpecialization()) == ROLES_HEALER)
                return true;
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_item_heals_proc_int::CheckProc);
    }
};

// 98681 - Chirping Box. Reward for achievement 5860 - The 'Unbeatable?' Pterodactyl: BEATEN.
class spell_item_chirping_box : public SpellScript
{
    PrepareSpellScript(spell_item_chirping_box);

    enum
    {
        ITEM_BLUE_MINI_JOUSTER = 65661,
        ITEM_GOLD_MINI_JOUSTER = 65662,
    };

    void HandleCast()
    {
        uint32 itemId = ITEM_BLUE_MINI_JOUSTER;
        ItemTemplate const* blue = sObjectMgr->GetItemTemplate(ITEM_BLUE_MINI_JOUSTER);
        if (GetCaster()->ToPlayer()->HasItemCount(blue->ItemId) || GetCaster()->ToPlayer()->HasSpell(blue->Spells[1].SpellId))
            itemId = ITEM_GOLD_MINI_JOUSTER;

        GetCaster()->ToPlayer()->StoreNewItem(Item::CreateItem(itemId, 1));
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_item_chirping_box::HandleCast);
    }
};

enum PhilosopherProcs
{
    SPELL_STRENGHT_PROC  = 60229,
    SPELL_AGILITY_PROC   = 60233,
    SPELL_INTELLECT_PROC = 60234
};

const std::vector<uint32> procAuras = { SPELL_STRENGHT_PROC, SPELL_AGILITY_PROC, SPELL_INTELLECT_PROC  };
const std::vector<Stats> procStats =  { STAT_STRENGTH,       STAT_AGILITY,       STAT_INTELLECT        };

// Zen Alchemist Stone - 105547
class spell_item_zen_alchemist_stone : public AuraScript
{
    PrepareAuraScript(spell_item_zen_alchemist_stone);

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = GetTarget()->ToPlayer();
        if (player->HasSpellCooldown(GetSpellInfo()->Id))
            return;

        Stats maxStatValue = STAT_STRENGTH;
        for (int8 i = 0; i < 3; i++)
            if (player->GetStat(procStats[i]) >= player->GetStat(procStats[maxStatValue]))
                maxStatValue = (Stats)i;

        // Can proc just on damage spell, also check for absorbed damage, because all damage can be absorbed but it's still damage spell
        if (eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage() == 0 && eventInfo.GetDamageInfo()->GetAbsorb() == 0)
            return;

        player->AddSpellCooldown(GetSpellInfo()->Id, Seconds(55));

        player->CastSpell(player, procAuras[maxStatValue], true);
        if (AuraEffect* aura = player->GetAuraEffect(procAuras[maxStatValue], EFFECT_0))
            aura->ChangeAmount(aurEff->GetAmount());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_item_zen_alchemist_stone::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// Blood Soaked Invitation 136149
class spell_item_blood_soaked_invitation : public SpellScript
{
    PrepareSpellScript(spell_item_blood_soaked_invitation);

    void HandleHordeDummy(SpellEffIndex index)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (caster->GetTeam() == HORDE)
                caster->CastSpell(caster, GetSpellInfo()->Effects[index].BasePoints);
    }

    void HandleAllianceDummy(SpellEffIndex index)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (caster->GetTeam() != HORDE)
                caster->CastSpell(caster, GetSpellInfo()->Effects[index].BasePoints);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_blood_soaked_invitation::HandleHordeDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_item_blood_soaked_invitation::HandleAllianceDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Ai-Li Skymirror 129803
class spell_item_ai_li_skymirror : public SpellScript
{
    PrepareSpellScript(spell_item_ai_li_skymirror);

    void HandleCopyImage(SpellEffIndex index)
    {
        if (Player* target = GetHitPlayer())
            target->CastSpell(GetCaster(), GetSpellInfo()->Effects[index].BasePoints, true);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_ai_li_skymirror::HandleCopyImage, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum BananaSpells
{
    SPELL_MAGIC_BANANA_EFF = 129618,
};

// 440. Summoned by 129620 - Magic Banana
class sat_magic_banana : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && !object->ToPlayer()->GetMap()->IsBattleArena();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_MAGIC_BANANA_EFF, true);
    }
};

// Party GRENADE eff 51508
class spell_item_party_grenade_eff : public SpellScript
{
    PrepareSpellScript(spell_item_party_grenade_eff);

    void HandleDance(SpellEffIndex index)
    {
        if (Player* target = GetHitPlayer())
            target->HandleEmoteStateCommand(EMOTE_STATE_DANCE);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_party_grenade_eff::HandleDance, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Party GRENADE 51510
class spell_item_party_grenade : public AuraScript
{
    PrepareAuraScript(spell_item_party_grenade);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->HandleEmoteStateCommand(EMOTE_STATE_NONE);
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->CastSpell(owner, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_item_party_grenade::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_item_party_grenade::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

enum seesawSpells
{
    SPELL_SEESAW_VEHICLE = 142096,
};

// Seesaw 144399
class spell_item_seesaw : public SpellScript
{
    PrepareSpellScript(spell_item_seesaw);

    void HandleDance(SpellEffIndex index)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SEESAW_VEHICLE, true);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_seesaw::HandleDance, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 71610, 71641 - Echoes of Light (Althor's Abacus)
class spell_item_echoes_of_light : public SpellScript
{
    PrepareSpellScript(spell_item_echoes_of_light);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() < 2)
            return;

        targets.sort(Trinity::HealthPctOrderPred());

        WorldObject* target = targets.front();
        targets.clear();
        targets.push_back(target);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_item_echoes_of_light::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 7434 - Fate Rune of Unsurpassed Vigor
enum FateRuneOfUnsurpassedVigor
{
    SPELL_UNSURPASSED_VIGOR = 25733
};

class spell_item_fate_rune_of_unsurpassed_vigor : public AuraScript
{
    PrepareAuraScript(spell_item_fate_rune_of_unsurpassed_vigor);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_UNSURPASSED_VIGOR });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_UNSURPASSED_VIGOR, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_item_fate_rune_of_unsurpassed_vigor::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 84421 - Loot-A-Rang
class spell_item_loot_a_rang : public SpellScript
{
    PrepareSpellScript(spell_item_loot_a_rang);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            std::list<Creature*> corpses;
            auto check = [=](Creature const* creature)
            {
                if (creature->IsAlive())
                    return false;
                if (!player->IsWithinDist(creature, 40.0f))
                    return false;
                return player->IsAllowedToLoot(creature);
            };
            Trinity::CreatureListSearcher<decltype(check)> searcher{ player, corpses, check };
            player->VisitNearbyGridObject(40.0f, searcher);

            // find the nearest corpse
            Creature* nearest = nullptr;
            float minDist = 40.0f;
            for (Creature* c : corpses)
            {
                float dist = player->GetExactDist(c);
                if (dist < minDist)
                {
                    minDist = dist;
                    nearest = c;
                }
            }

            if (nearest)
                player->SendLoot(nearest->GetGUID(), LOOT_CORPSE, true);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_item_loot_a_rang::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_item_spell_scripts()
{
    // 23074 Arcanite Dragonling
    new spell_item_trigger_spell("spell_item_arcanite_dragonling", SPELL_ARCANITE_DRAGONLING);
    // 23133 Gnomish Battle Chicken
    new spell_item_trigger_spell("spell_item_gnomish_battle_chicken", SPELL_BATTLE_CHICKEN);
    // 23076 Mechanical Dragonling
    new spell_item_trigger_spell("spell_item_mechanical_dragonling", SPELL_MECHANICAL_DRAGONLING);
    // 23075 Mithril Mechanical Dragonling
    new spell_item_trigger_spell("spell_item_mithril_mechanical_dragonling", SPELL_MITHRIL_MECHANICAL_DRAGONLING);

    new spell_item_aegis_of_preservation();
    new spell_item_arcane_shroud();
    new spell_item_blessing_of_ancient_kings();
    new spell_item_defibrillate("spell_item_goblin_jumper_cables", 67, SPELL_GOBLIN_JUMPER_CABLES_FAIL);
    new spell_item_defibrillate("spell_item_goblin_jumper_cables_xl", 50, SPELL_GOBLIN_JUMPER_CABLES_XL_FAIL);
    new spell_item_defibrillate("spell_item_gnomish_army_knife", 33);
    new spell_item_desperate_defense();
    new spell_item_deviate_fish();
    new spell_item_flask_of_the_north();
    new spell_item_gnomish_death_ray();
    new spell_item_make_a_wish();
    new spell_item_mingos_fortune_generator();
    new spell_item_necrotic_touch();
    new spell_item_net_o_matic();
    new spell_item_noggenfogger_elixir();
    new spell_item_piccolo_of_the_flaming_fire();
    new spell_item_savory_deviate_delight();
    new spell_item_scroll_of_recall();
    new spell_item_unsated_craving();
    new spell_item_shadows_fate();
    new spell_item_shadowmourne();
    new spell_item_shadowmourne_soul_fragment();
    new spell_item_six_demon_bag();
    new spell_item_the_eye_of_diminution();
    new spell_item_underbelly_elixir();
    new spell_item_red_rider_air_rifle();

    new spell_item_create_heart_candy();
    new spell_item_book_of_glyph_mastery();
    new spell_item_gift_of_the_harvester();
    new spell_item_map_of_the_geyser_fields();
    new spell_item_vanquished_clutches();

    new spell_item_ashbringer();
    new spell_magic_eater_food();
    new spell_item_shimmering_vessel();
    new spell_item_purify_helboar_meat();
    new spell_item_crystal_prison_dummy_dnd();
    new spell_item_reindeer_transformation();
    new spell_item_nigh_invulnerability();
    new spell_item_poultryizer();
    new spell_item_socrethars_stone();
    new spell_item_demon_broiled_surprise();
    new spell_item_complete_raptor_capture();
    new spell_item_impale_leviroth();
    new spell_item_brewfest_mount_transformation();
    new spell_item_nitro_boots();
    new spell_item_rocket_boots();
    new spell_item_pygmy_oil();
    new spell_item_unusual_compass();
    new spell_item_chicken_cover();
    new spell_item_muisek_vessel();
    new spell_item_greatmothers_soulcatcher();
    new spell_item_flameseers_staff_flamebreaker();
    new spell_script<spell_item_wormhole_pandaria>("spell_item_wormhole_pandaria");
    new spell_script<spell_item_flippable_table>("spell_item_flippable_table");
    new aura_script<spell_item_stardust_no_2>("spell_item_stardust_no_2");
    new spell_script<spell_jewelcrafting_daily_quests_cata>("spell_jewelcrafting_daily_quests_cata");
    new spell_script<spell_item_alchemists_flask>("spell_item_alchemists_flask");
    new aura_script<spell_item_swarmkeepers_speed>("spell_item_swarmkeepers_speed");
    new aura_script<spell_item_arrival_of_the_naaru>("spell_item_arrival_of_the_naaru");
    new spell_script<spell_item_teleport_with_error>("spell_item_teleport_with_error");
    new spell_script<spell_item_taunt_flag_targeting>("spell_item_taunt_flag_targeting");
    new spell_script<spell_item_summon_sword_dancers>("spell_item_summon_sword_dancers");
    new aura_script<spell_item_spark_of_zandalari_proc_aura>("spell_item_spark_of_zandalari_proc_aura");
    new aura_script<spell_item_spark_of_zandalari>("spell_item_spark_of_zandalari");
    new aura_script<spell_item_relic_of_zuldazar_shield>("spell_item_relic_of_zuldazar_shield");
    new spell_script<spell_item_relic_of_zuldazar>("spell_item_relic_of_zuldazar");
    new aura_script<spell_item_lightning_chalice_proc_aura>("spell_item_lightning_chalice_proc_aura");
    new aura_script<spell_infinite_power>("spell_infinite_power");
    new spell_script<spell_item_hardened_shell>("spell_item_hardened_shell");
    new aura_script<spell_item_soul_barrier>("spell_item_soul_barrier");
    new aura_script<spell_item_proc_ji_kuns_rising_winds>("spell_item_proc_ji_kuns_rising_winds");
    new aura_script<spell_item_tot_stack_trinket>("spell_item_tot_stack_trinket");
    new aura_script<spell_item_rune_of_re_organization>("spell_item_rune_of_re_organization");
    new spell_script<spell_chief_engineer_jards_journal>("spell_chief_engineer_jards_journal");
    new spell_script<spell_learn_some_ingredient_and_its_uses>("spell_learn_some_ingredient_and_its_uses");
    new aura_script<spell_memory_of_mr_smite>("spell_memory_of_mr_smite");
    new spell_script<spell_item_potion_of_illusion>("spell_item_potion_of_illusion");
    new aura_script<spell_item_multistrike_trinket_damage>("spell_item_multistrike_trinket_damage");
    new aura_script<spell_item_multistrike_trinket_heal>("spell_item_multistrike_trinket_heal");
    new aura_script<spell_item_evil_eye_of_galakras>("spell_item_evil_eye_of_galakras");
    new aura_script<spell_item_assurance_of_consequence>("spell_item_assurance_of_consequence");
    new aura_script<spell_item_vial_of_living_corruption>("spell_item_vial_of_living_corruption");
    new aura_script<spell_item_restless_stat_trinket>("spell_item_restless_stat_trinket");
    new aura_script<spell_item_soo_stack_trinket>("spell_item_soo_stack_trinket");
    new aura_script<spell_item_soo_trinket_aoe_damage>("spell_item_soo_trinket_aoe_damage");
    new aura_script<spell_item_soo_trinket_aoe_heal>("spell_item_soo_trinket_aoe_heal");
    new spell_script<spell_item_soo_trinket_aoe_effect>("spell_item_soo_trinket_aoe_effect");
    new aura_script<spell_item_juggernauts_focusing_crystal>("spell_item_juggernauts_focusing_crystal");
    new aura_script<spell_item_legendary_cloak>("spell_item_legendary_cloak");
    new aura_script<spell_item_legendary_cloak_animation_reset>("spell_item_legendary_cloak_animation_reset");
    new aura_script<spell_item_legendary_cloak_flurry_of_huen>("spell_item_legendary_cloak_flurry_of_huen");
    new aura_script<spell_item_legendary_cloak_essence_of_yu_lon>("spell_item_legendary_cloak_essence_of_yu_lon");
    new aura_script<spell_item_legendary_cloak_essence_of_yu_lon_periodic>("spell_item_legendary_cloak_essence_of_yu_lon_periodic");
    new aura_script<spell_item_legendary_cloak_endurance_of_niuzao>("spell_item_legendary_cloak_endurance_of_niuzao");
    new aura_script<spell_item_legendary_cloak_spirit_of_chi_ji_proc>("spell_item_legendary_cloak_spirit_of_chi_ji_proc");
    new aura_script<spell_item_legendary_cloak_spirit_of_chi_ji>("spell_item_legendary_cloak_spirit_of_chi_ji");
    new spell_script<spell_item_spirit_of_chi_ji_heal>("spell_item_spirit_of_chi_ji_heal");
    new spell_script<spell_item_celestial_defender>("spell_item_celestial_defender");
    new spell_script<spell_item_spectral_grog>("spell_item_spectral_grog");
    new spell_script<spell_item_symbiotic_growth>("spell_item_symbiotic_growth");
    new aura_script<spell_item_symbiotic_growth_aura>("spell_item_symbiotic_growth_aura");
    new aura_script<spell_item_heals_proc_int>("spell_item_heals_proc_int");
    new spell_script<spell_item_chirping_box>("spell_item_chirping_box");
    new aura_script<spell_item_zen_alchemist_stone>("spell_item_zen_alchemist_stone");
    new spell_script<spell_item_blood_soaked_invitation>("spell_item_blood_soaked_invitation");
    new spell_script<spell_item_ai_li_skymirror>("spell_item_ai_li_skymirror");
    new atrigger_script<sat_magic_banana>("sat_magic_banana");
    new spell_script<spell_item_party_grenade_eff>("spell_item_party_grenade_eff");
    new aura_script<spell_item_party_grenade>("spell_item_party_grenade");
    new spell_script<spell_item_seesaw>("spell_item_seesaw");
    //RegisterSpellScript(spell_item_echoes_of_light);
    new spell_script<spell_item_echoes_of_light>("spell_item_echoes_of_light");
    new aura_script<spell_item_fate_rune_of_unsurpassed_vigor>("spell_item_fate_rune_of_unsurpassed_vigor");
    new spell_script<spell_item_loot_a_rang>("spell_item_loot_a_rang");
}
