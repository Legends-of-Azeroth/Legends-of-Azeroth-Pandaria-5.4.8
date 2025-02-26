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
 * Scripts for enchantments.
 */

#include "ScriptMgr.h"
#include "SpellScript.h"

 // 118333 - Dancing Steel, 142531 - Bloody Dancing Steel 
class spell_ench_dancing_steel : public AuraScript
{
    PrepareAuraScript(spell_ench_dancing_steel);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Player* owner = GetUnitOwner()->ToPlayer();
        uint32 enchId = GetAura()->GetId() == 118333 ? 120032 : 142530;
        if (Item* castItem = owner->GetItemByGuid(GetAura()->GetCastItemGUID()))
        {
            float agi = owner->GetStat(STAT_AGILITY);
            float str = owner->GetStat(STAT_STRENGTH);
            owner->CastSpell(owner, enchId, true, castItem);
            if (Aura* aura = owner->GetAura(enchId, owner->GetGUID(), castItem->GetGUID()))
            {
                uint32 effIndex = agi > str ? EFFECT_1 : EFFECT_0;
                if (aura->HasEffect(effIndex))
                    aura->GetEffect(effIndex)->ChangeAmount(0);
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_ench_dancing_steel::HandleProc);
    }
};

// 104561 - Windsong
class spell_ench_windsong : public AuraScript
{
    PrepareAuraScript(spell_ench_windsong);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Player* owner = GetUnitOwner()->ToPlayer();
        uint32 const spells[] = { 104423, 104509, 104510 };
        if (Item* castItem = owner->GetItemByGuid(GetAura()->GetCastItemGUID()))
            owner->CastSpell(owner, spells[irand(0, 2)], true, castItem);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_ench_windsong::HandleProc);
    }
};

// 120033 - Jade Spirit (Passive)
// 142536 - Spirit of Conquest (Passive)
class spell_ench_jade_spirit_passive : public AuraScript
{
    PrepareAuraScript(spell_ench_jade_spirit_passive);

    enum
    {
        SPELL_JADE_SPIRIT_PASSIVE   = 120033,
        SPELL_JADE_SPIRIT           = 104993,
        SPELL_SPIRIT_OF_CONQUEST    = 142535,
    };

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        uint32 spell = GetId() == SPELL_JADE_SPIRIT_PASSIVE ? SPELL_JADE_SPIRIT : SPELL_SPIRIT_OF_CONQUEST;
        GetUnitOwner()->CastSpell(GetUnitOwner(), spell, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_ench_jade_spirit_passive::HandleProc);
    }
};

// 104993 - Jade Spirit
// 142535 - Spirit of Conquest
class spell_ench_jade_spirit : public AuraScript
{
    PrepareAuraScript(spell_ench_jade_spirit);

    void CaluculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (GetUnitOwner()->GetPower(POWER_MANA) > CalculatePct(GetUnitOwner()->GetMaxPower(POWER_MANA), 25))
            amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_ench_jade_spirit::CaluculateAmount, EFFECT_1, SPELL_AURA_MOD_STAT);
    }
};

// 104428 - Elemental Force(DND)
class spell_ench_elemental_force : public AuraScript
{
    PrepareAuraScript(spell_ench_elemental_force);

    bool CheckProc(ProcEventInfo& evenInfo)
    {
        // TODO: Maybe it should not proc from AOE?
        return evenInfo.GetActionTarget() != nullptr;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_ench_elemental_force::CheckProc);
    }
};

// 118314 - Mogu Shield
class spell_ench_colossus : public AuraScript
{
    PrepareAuraScript(spell_ench_colossus);

    enum { SPELL_COLOSSUS = 116631 };

    void HandleProc(ProcEventInfo&)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (Item* castItem = owner->GetItemByGuid(GetAura()->GetCastItemGUID()))
                owner->CastSpell(owner, SPELL_COLOSSUS, true, castItem);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_ench_colossus::HandleProc);
    }
};

// 82174, 126734 - Synapse Springs
class spell_ench_synapse_springs : public SpellScript
{
    PrepareSpellScript(spell_ench_synapse_springs);

    enum
    {
        SYNAPSE_SPRINGS_AGILITY  = 96228,
        SYNAPSE_SPRINGS_STRENGTH = 96229,
        SYNAPSE_SPRINGS_INTELECT = 96230,
    };

    void HandleHit()
    {
        std::vector<uint32> const spells{ SYNAPSE_SPRINGS_AGILITY, SYNAPSE_SPRINGS_STRENGTH, SYNAPSE_SPRINGS_INTELECT };

        std::vector<float> stats { GetCaster()->GetStat(STAT_AGILITY), GetCaster()->GetStat(STAT_STRENGTH), GetCaster()->GetStat(STAT_INTELLECT) };
        auto it = std::max_element(stats.begin(), stats.end());
        uint32 spell = spells[std::distance(stats.begin(), it)];
        int32 statVal = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
        GetCaster()->CastCustomSpell(spell, SPELLVALUE_BASE_POINT0, statVal, GetCaster(), true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_ench_synapse_springs::HandleHit);
    }
};

void AddSC_enchantment_spell_scripts()
{
    new aura_script<spell_ench_dancing_steel>("spell_ench_dancing_steel");
    new aura_script<spell_ench_windsong>("spell_ench_windsong");
    new aura_script<spell_ench_jade_spirit_passive>("spell_ench_jade_spirit_passive");
    new aura_script<spell_ench_jade_spirit>("spell_ench_jade_spirit");
    new aura_script<spell_ench_elemental_force>("spell_ench_elemental_force");
    new aura_script<spell_ench_colossus>("spell_ench_colossus");
    new spell_script<spell_ench_synapse_springs>("spell_ench_synapse_springs");
}
