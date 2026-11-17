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

#include "ScriptMgr.h"
#include "Vehicle.h"
#include "PassiveAI.h"
#include "Player.h"
#include "Language.h"

enum WildTurkey
{
    SPELL_TURKEY_TRACKER = 62014
};

struct npc_wild_turkey : public CritterAI
{
    npc_wild_turkey(Creature* creature) : CritterAI(creature) { }

    void JustDied(Unit* killer) override
    {
        if (killer && killer->GetTypeId() == TYPEID_PLAYER)
            killer->CastSpell(killer, SPELL_TURKEY_TRACKER, true);
    }
};

// Item: Turkey Caller
enum LonelyTurkey
{
    SPELL_COSMETIC_HEARTBROKEN = 62013,
};

struct npc_lonely_turkey : public ScriptedAI
{
    npc_lonely_turkey(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (me->IsSummon())
            if (Unit* owner = me->ToTempSummon()->GetSummoner())
                me->GetMotionMaster()->MovePoint(0, owner->GetPositionX() + 25 * cos(owner->GetOrientation()), owner->GetPositionY() + 25 * cos(owner->GetOrientation()), owner->GetPositionZ());

        timer = 3.5 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 diff) override
    {
        if (timer <= diff)
        {
            DoCast(SPELL_COSMETIC_HEARTBROKEN);
            me->SetFaction(31);
        }
        timer -= diff;
    }
private:
    uint32 timer;
};

enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL       = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX   = 62021
};

class spell_gen_turkey_tracker : public SpellScript
{
    PrepareSpellScript(spell_gen_turkey_tracker);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
            return false;
        return true;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->ToPlayer()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
            return;

        Unit* caster = GetCaster();

        if (!caster || !caster->ToPlayer())
            return;

        if (Aura* aura = caster->GetAura(GetSpellInfo()->Id))
        {
            switch (aura->GetStackAmount())
            {
                case 10:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_HUNTER, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 20:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_DOMINATION, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 30:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_SLAUGHTER, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 40:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_TRIUMPH, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL_MAX, true); // Achievement Credit
                    caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                    break;
                default:
                    break;
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class spell_gen_feast_on : public SpellScript
{
    PrepareSpellScript(spell_gen_feast_on);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

        Unit* caster = GetCaster();
        if (caster->IsVehicle())
            if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                caster->CastSpell(player, bp0, true, nullptr, nullptr, player->ToPlayer()->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum WellFedPilgrimsBounty
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY           = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES      = 61804,
    SPELL_A_SERVING_OF_STUFFING         = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES   = 61808,
    SPELL_A_SERVING_OF_PIE              = 61805,

    // Well Fed
    SPELL_WELL_FED_AP                   = 65414,
    SPELL_WELL_FED_ZM                   = 65412,
    SPELL_WELL_FED_HIT                  = 65416,
    SPELL_WELL_FED_HASTE                = 65410,
    SPELL_WELL_FED_SPIRIT               = 65415,

    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING         = 61849,
};

class spell_gen_well_fed_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bounty_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_well_fed_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBounty
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,

    SPELL_RIDE_VEHICLE_CHAIRS       = 65403,
};

// 66250 - Pass The Turkey
// 66259 - Pass The Stuffing
// 66260 - Pass The Pie
// 66261 - Pass The Cranberries
// 66262 - Pass The Sweet Potatoes
class spell_gen_on_plate_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    if (!GetHitUnit()->HasAura(SPELL_RIDE_VEHICLE_CHAIRS))
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, nullptr, nullptr, player->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bounty_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_on_plate_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK          = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD           = 66478,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT    = 66622,
};

class spell_gen_bountiful_feast : public SpellScript
{
    PrepareSpellScript(spell_gen_bountiful_feast);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415,
};

class spell_pilgrims_bounty_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bounty_buff_food(const char* name, uint32 triggeredSpellId = 0) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bounty_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bounty_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load() override
            {
                _handled = false;
                return true;
            }

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                {
                    amount = caster->CountPctFromMaxHealth(4 * 5);
                    canBeRecalculated = false;
                }
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                _handled = true;
                caster->CastSpell(caster, _triggeredSpellId, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pilgrims_bounty_buff_food_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_REGEN);
                if (_triggeredSpellId)
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bounty_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pilgrims_bounty_buff_food_AuraScript(_triggeredSpellId);
        }
};

class spell_pilgrims_bounty_buff_drink : public AuraScript
{
    PrepareAuraScript(spell_pilgrims_bounty_buff_drink);

    void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
        {
            amount = CalculatePct(caster->GetMaxPower(POWER_MANA), 4 * 5);
            canBeRecalculated = false;
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pilgrims_bounty_buff_drink::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN);
    }
};

void AddSC_event_pilgrims_bounty()
{
    new creature_script<npc_wild_turkey>("npc_wild_turkey");
    new creature_script<npc_lonely_turkey>("npc_lonely_turkey");
    new spell_script<spell_gen_turkey_tracker>("spell_gen_turkey_tracker");
    new spell_script<spell_gen_feast_on>("spell_gen_feast_on");
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_script<spell_gen_bountiful_feast>("spell_gen_bountiful_feast");
    new spell_pilgrims_bounty_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_pilgrims_bounty_buff_food");
    new aura_script<spell_pilgrims_bounty_buff_drink>("spell_pilgrims_bounty_buff_drink");
}
