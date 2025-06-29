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
 * Scripts for the player's spells of different classes but with a same mechanic.
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellHistory.h"
#include "spell_common.h"

 // 134732 - Battle Fatigue
class spell_common_battle_fatigue : public AuraScript
{
    PrepareAuraScript(spell_common_battle_fatigue);

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetActionTarget())
            if (!target->GetMap()->IsDungeon() && target->GetCharmerOrOwnerPlayerOrPlayerItself() && target->IsValidAttackTarget(GetUnitOwner()))
                target->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_common_battle_fatigue::HandleProc);
    }
};

// TO BE REMOVED AFTER 5.4
// 134735 - Battle Fatigue
class spell_common_battle_fatigue_fix : public AuraScript
{
    PrepareAuraScript(spell_common_battle_fatigue_fix);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        amount = -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_common_battle_fatigue_fix::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_common_battle_fatigue_fix::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_ABSORPTION_PCT);
    }
};

// TO BE REMOVED AFTER 5.4
// 115043 - Player Damage Reduction
class spell_common_player_damage_reduction_fix : public AuraScript
{
    PrepareAuraScript(spell_common_player_damage_reduction_fix);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        float val = sWorld->getFloatConfig(CONFIG_PVP_RESILIENCE_BELOW_90);
        val *= 100.0f;
        amount = -val;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_common_player_damage_reduction_fix::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RESILIENCE_PCT);
    }
};

// TO BE REMOVED AFTER 5.4
// 142689 - Player Damage Reduction Level 90
class spell_common_player_damage_reduction_fix_90 : public AuraScript
{
    PrepareAuraScript(spell_common_player_damage_reduction_fix_90);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        float val = sWorld->getFloatConfig(CONFIG_PVP_RESILIENCE_90) - sWorld->getFloatConfig(CONFIG_PVP_RESILIENCE_BELOW_90);
        val *= 100.0f;
        amount = -val;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_common_player_damage_reduction_fix_90::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RESILIENCE_PCT);
    }
};

// 84839 - Paladin
// 84840 - Druid
// 93098 - Warrior
// 93099 - DK
// 120267 - Monk
class spell_common_vengeance : public AuraScript
{
    PrepareAuraScript(spell_common_vengeance);

    enum { SEPLL_VENGEANCE = 132365 };

    struct VeneganceAttackerInfo
    {
        uint32 LastDamage;
        TimeValue LastTime;
    };

    std::map<uint64, VeneganceAttackerInfo> attackers;

    void HandleProc(ProcEventInfo& eventInfo)
    {
        // Nobody know how this shit should work.
        // This fucking formula 0.015 * DamageTaken + OldVengeance * OldVengeanceSecondsRemaining / 20 doesn't work because of buff degradation.
        // How to calculate equilibrium point also unknown.
        // Fuck it.

        Unit* owner = GetUnitOwner();
        Unit* actor = eventInfo.GetActor();
        bool challenge = owner->GetMap()->GetDifficulty() == DUNGEON_DIFFICULTY_CHALLENGE;

        if (!actor || actor->GetCharmerOrOwnerPlayerOrPlayerItself())
            return;

        uint32 damage = eventInfo.GetProcTriggerContext()->UnmitigatedDamage;
        if (!damage)
            return;

        damage *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_DAMAGE_TAKEN, [](AuraEffect const* effect)
        {
            return effect->GetAmount() > 0;
        });

        float vengPct = challenge ? 0.018f : 0.015f;

        // The Nth strongest(based on pre - mitigation average auto attack DPS) mob that has hit you in the last 5 seconds
        // grants 1 / Nth of full vengeance with their attacks.N is recalculated on every hit taken.
        uint32 place = 1;
        auto now = TimeValue::Now();
        auto timecap = now - Seconds(5);
        auto& attacker = attackers[actor->GetGUID()];
        attacker.LastTime = now;
        attacker.LastDamage = damage;
        for (auto itr = attackers.begin(); itr != attackers.end();)
        {
            if (itr->second.LastTime < timecap)
            {
                attackers.erase(itr++);
                continue;
            }

            if (itr->second.LastDamage > damage)
                ++place;
            ++itr;
        }

        if (eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
            damage *= 1.5f; // Only 50% from crit

        float newAmount = damage * vengPct;
        // Add ap from current strike after the balance point calculation, just assumption
        // damage * 1 / palce
        if (!challenge)
            newAmount /= place;

        // modify according to damage type; spell damage gives 2.5x as much Vengeance
        if (eventInfo.GetSpellInfo())
            newAmount *= 2.5;

        newAmount *= sWorld->getFloatConfig(CONFIG_VENGEANCE_MULTIPLIER);

        if (AuraEffect* venegance = owner->GetAuraEffect(SEPLL_VENGEANCE, EFFECT_0))
            newAmount += venegance->GetFloatAmount() * venegance->GetBase()->GetDuration() / venegance->GetBase()->GetMaxDuration();

        float attackFrequency = 0.0f;
        if (eventInfo.GetSpellInfo())
            attackFrequency = 1.0f / 60.0f;
        else
            attackFrequency = 1.0f / (actor->GetFloatValue(UNIT_FIELD_ATTACK_ROUND_BASE_TIME) / 1000.0f);

        float equilibrium = damage * vengPct * attackFrequency * 20.0f;
        // simcraft has equilibrium / 2
        if (equilibrium * 0.66f > newAmount)
            newAmount = equilibrium * 0.66f;

        newAmount = std::min(newAmount, float(owner->GetMaxHealth()));

        owner->CastSpell(owner, SEPLL_VENGEANCE, true);
        if (Aura* venegance = owner->GetAura(SEPLL_VENGEANCE))
        {
            venegance->GetEffect(EFFECT_0)->ChangeAmount(newAmount);
            venegance->GetEffect(EFFECT_1)->ChangeAmount(newAmount);
            venegance->RefreshDuration(false);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_common_vengeance::HandleProc);
    }
};

// 2983 - Sprint
// 23451 - Speed (BG buff)
// 54861 - Nitro Boosts
// 58875 - Spirit Walk
// 64129 - Body and Soul
// 68992 - Darkflight
// 85499 - Speed of Light
// 108843 - Blazing Speed
// 111400 - Burning Rush
// 116841 - Tiger's Lust
// 137573 - Burst of Speed
class spell_common_run_speed_marker : public AuraScript
{
    PrepareAuraScript(spell_common_run_speed_marker);

    enum { SPELL_RUN_SPEED_MARKER = 96223 };

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_RUN_SPEED_MARKER, true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        static std::set<uint32> const nonStackBuffs
        {
            2983,   // Sprint
            23451,  // Speed (BG buff)
            54861,  // Nitro Boosts
            58875,  // Spirit Walk
            64129,  // Body and Soul
            68992,  // Darkflight
            85499,  // Speed of Light
            108843, // Blazing Speed
            111400, // Burning Rush
            116841, // Tiger's Lust
            137573, // Burst of Speed
        };

        for (auto&& itr : nonStackBuffs)
            if (GetUnitOwner()->HasAura(itr))
                return;

        GetUnitOwner()->RemoveAurasDueToSpell(SPELL_RUN_SPEED_MARKER);
    }

    void Register() override
    {
        OnEffectApply +=  AuraEffectApplyFn(spell_common_run_speed_marker::HandleApply,   EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_common_run_speed_marker::HandleRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 139569 - Combo Point Delayed.
// Triggered by Ruthlessness (14161) and Item - Druid T15 Feral 2P Bonus (138352)
class spell_common_combo_point_delayed : public SpellScript
{
    PrepareSpellScript(spell_common_combo_point_delayed);

    void GainCombopoint()
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            ObjectGuid targetGuid = caster->GetTarget();
            caster->m_Events.Schedule(1, [=]
            {
                if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGuid))
                    caster->CastSpell(target, 139546, true);
            });
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_common_combo_point_delayed::GainCombopoint);
    }
};

// 20167 - Seal of Insight
// 108000 - Nick of Time
// 109822 - Nick of Time
// 109825 - Nick of Time
// 113828 - Healing Touch
// 117895 - Eminence (Statue), 
// 126890 - Eminence
// 124988 - Nature's Vigil
// 138302 - Golden Apparition
// 139195 - Restoration of the Infinite
// 145153 - Dream of Cenarius
class spell_common_smart_heal_one_target : public spell_smart_heal
{
    PrepareSpellScript(spell_common_smart_heal_one_target);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_smart_heal_one_target::FilterTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_smart_heal_one_target::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 73921 - Healing Rain
// 82327 - Holy Radiance
// 88686 - Holy Word: Sanctuary
// 117640 - Spinning Crane Kick
// 119952 - Arcing Light
class spell_common_smart_heal_six_targets : public spell_smart_heal
{
    PrepareSpellScript(spell_common_smart_heal_six_targets);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        ApplySmartHeal(targets, 6);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_smart_heal_six_targets::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_smart_heal_six_targets::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 44203 - Tranquility
// 64844 - Divine Hymn
// 113278 - Tranquility (Symbiosis)
// 114942 - Healing Tide
class spell_common_smart_heal_raid_25 : public spell_smart_heal
{
    PrepareSpellScript(spell_common_smart_heal_raid_25);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 count = GetCaster()->GetMap()->Is25ManRaid() ? 12 : 5;
        targets.remove_if(Trinity::NotInRaidWithCheck{ GetCaster() });
        ApplySmartHeal(targets, count);
    }

    void Register() override
    {
        SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);
        Targets targetType = spell->Effects[EFFECT_0].TargetB.GetTarget();
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_smart_heal_raid_25::FilterTargets, EFFECT_ALL, targetType);
    }
};

// 475 - Remove Curse
// 527 - Purify
// 2782 - Remove Corruption
// 4987 - Cleanse
// 51886 - Cleanse Spirit
// 77130 - Purify Spirit
// 88423 - Nature's Cure
// 115450 - Detox
class spell_common_dispel_reset_cooldown : public SpellScript
{
    PrepareSpellScript(spell_common_dispel_reset_cooldown);

    void HandleHit()
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (!GetDispelCount())
                caster->RemoveSpellCooldown(GetSpellInfo()->Id, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_common_dispel_reset_cooldown::HandleHit);
    }
};

// 2825 - Bloodlust
// 32182 - Heroism
// 80353 - Time Warp
// 90355 - Ancient Hysteria
// 146555 - Drums of Rage
class spell_common_raid_haste_30 : public SpellScript
{
    PrepareSpellScript(spell_common_raid_haste_30);

    enum
    {
        SPELL_SHAMAN_BLOODLUST              = 2825,
        SPELL_SATED                         = 57724,
        SPELL_SHAMAN_HEROISM                = 32182,
        SPELL_EXHAUSTED                     = 57723,
        SPELL_MAGE_TIME_WARP                = 80353,
        SPELL_TEMPORAL_DISPLACEMENT         = 80354,
        SPELL_HUNTER_ANCIENT_HYSTERIA       = 90355,
        SPELL_INSANITY                      = 95809,
        SPELL_DRUMS_OF_RAGE                 = 146555,
    };

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_INSANITY));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_EXHAUSTED));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SATED));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_TEMPORAL_DISPLACEMENT));
    }

    void HandleHit()
    {
        uint32 spellId = 0;
        switch (GetSpellInfo()->Id)
        {
            case SPELL_SHAMAN_BLOODLUST: spellId = SPELL_SATED; break;
            case SPELL_SHAMAN_HEROISM: spellId = SPELL_EXHAUSTED; break;
            case SPELL_MAGE_TIME_WARP: spellId = SPELL_TEMPORAL_DISPLACEMENT; break;
            case SPELL_HUNTER_ANCIENT_HYSTERIA: spellId = SPELL_INSANITY; break;
            case SPELL_DRUMS_OF_RAGE: spellId = SPELL_EXHAUSTED; break;
        }
        GetHitUnit()->CastSpell(GetHitUnit(), spellId, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_raid_haste_30::FilterTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_common_raid_haste_30::HandleHit);
    }
};

// 107574 - Avatar
// 114029 - Safeguard
class spell_common_remove_snare_root : public SpellScript
{
    PrepareSpellScript(spell_common_remove_snare_root);

    void HandleCast()
    {
        GetCaster()->RemoveMovementImpairingAuras();
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_common_remove_snare_root::HandleCast);
    }
};

// 58838 - Inherit Master's Threat List
// Currently not implemented, but used with Mirror Image and Spectral Guise
class spell_common_inherit_masters_threat_list : public SpellScript
{
    PrepareSpellScript(spell_common_inherit_masters_threat_list);

    Unit* master = nullptr;

    bool Load() override
    {
        master = GetCaster()->GetOwner();
        return master;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([this](WorldObject* obj)
        {
            if (Unit* target = obj->ToUnit())
                if (target->IsInCombat() && target->CanHaveThreatList() && target->GetThreatManager().getThreat(master) > 0)
                    return false;

            return true;
        });
    }

    void HandleHit()
    {
        float threat = GetHitUnit()->GetThreatManager().getThreat(master);
        GetHitUnit()->GetThreatManager().addThreat(GetCaster(), threat);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_common_inherit_masters_threat_list::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENTRY);
        OnHit += SpellHitFn(spell_common_inherit_masters_threat_list::HandleHit);
    }
};

// 1784 - Stealth
// 5215 - Prowl
// 115191 - Stealth (Subterfuge)
// 113289 - Prowl (Shaman)
// 119032 - Spectral Guise
class spell_common_stealth_proc : public AuraScript
{
    PrepareAuraScript(spell_common_stealth_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK)
        {
            bool heal = eventInfo.GetTypeMask() & (PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS) || eventInfo.GetHitMask() & PROC_EX_INTERNAL_HOT;
            if (!heal && eventInfo.GetDamageInfo()->GetDamage()) // Has damage - break anyway
                return true;

            if (!eventInfo.GetActor())                  // Why not?
                return true;
            // Has no damage - only for enemies
            if (!eventInfo.GetActor()->IsValidAttackTarget(GetUnitOwner(), eventInfo.GetSpellInfo(), nullptr, true))
                return false;

            if (eventInfo.GetSpellInfo())
            {
                if (eventInfo.GetHitMask() & PROC_EX_ABSORB)
                {
                    if ((eventInfo.GetHitMask() & PROC_EX_INTERNAL_DOT) || eventInfo.GetSpellInfo()->IsAoESpell())  // DoT or AoE in shield - don't break
                        return false;
                }
                else // Damage already checked. PROC_EX_ABSORB with 0 damage can't be damaging spell
                {
                    if (eventInfo.GetSpellInfo()->Id == 64803) // Entrapment
                        return false;

                    // AoE from Ice Trap...
                    if (eventInfo.GetSpellInfo()->Id == 63487)
                        if (auto targetInfo = eventInfo.TriggeredBySpell()->GetTargetInfo(GetUnitOwner()->GetGUID()))
                            if (!(targetInfo->effectMask & (1 << EFFECT_0)))
                                return false;

                    for (auto&& eff : eventInfo.GetSpellInfo()->Effects)
                    {
                        if (eff.Effect == SPELL_EFFECT_KNOCK_BACK)
                            return true;

                        if (eff.Effect == SPELL_EFFECT_DISPEL ||
                            eff.Effect == SPELL_EFFECT_DISTRACT ||
                            eff.ApplyAuraName == SPELL_AURA_MOD_DECREASE_SPEED ||
                            eff.ApplyAuraName == SPELL_AURA_INTERFERE_TARGETTING)
                            return false;
                    }
                }
            }
        }
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_common_stealth_proc::CheckProc);
    }
};

// 5760 - Mind-numbing Poison
// 31589 - Slow
// 58604 - Lava Breath
// 73975 - Necrotic Strike
// 109466 - Curse of Enfeeblement
// 109468 - Curse of Enfeeblement
class spell_common_spell_slow_players : public AuraScript
{
    PrepareAuraScript(spell_common_spell_slow_players);

    void CalculateAmount(AuraEffect const*, float& amount, bool&)
    {
        if (GetOwner()->GetTypeId() == TYPEID_PLAYER)
            amount = -10.0f;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_common_spell_slow_players::CalculateAmount, EFFECT_ALL, SPELL_AURA_SPELL_SLOW);
    }
};

// 88747  - Wild Mushroom (Balance)
// 108921 - Psyfiend
// 114192 - Mocking Banner
// 114203 - Demoralizing Banner
// 147349 - Wild Mushroom (Restoration with glyph)
class spell_common_check_destination_availability : public SpellScript
{
    PrepareSpellScript(spell_common_check_destination_availability);

    SpellCastResult CheckCast()
    {
        if (GetExplTargetDest())
        {
            PathGenerator path{ GetCaster() };
            Position pos = GetExplTargetDest()->GetPosition();
            path.CalculatePath(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
            if (path.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
                return SPELL_FAILED_NOPATH;
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_common_check_destination_availability::CheckCast);
    }
};

// 113873, 127649, 127650 - Remove Talent
class spell_common_unlearn_talent : public SpellScript
{
    PrepareSpellScript(spell_common_unlearn_talent);

    void HandleCast()
    {
        Player* player = GetCaster()->ToPlayer();
        TalentEntry const* talent = sTalentStore.LookupEntry(GetSpell()->m_glyphIndex);
        if (player && talent)
        {
            switch (talent->SpellID)
            {
                case 137619: // Marked for Death
                    if (player->GetComboTarget())
                    {
                        player->GetComboTarget()->RemoveAurasDueToSpell(talent->SpellID);
                        player->ClearComboPoints();
                    }
                    break;
                case 115399: // Chi Brew
                    player->RemoveAurasDueToSpell(115867); // Mana Tea
                    player->RemoveAurasDueToSpell(128939); // Elusive Brew
                    player->RemoveAurasDueToSpell(125195); // Tigereye Brew
                    break;
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_common_unlearn_talent::HandleCast);
    }
};

template <int spellId>
class spell_common_remove_periodic_damage : public SpellScript
{
    PrepareSpellScript(spell_common_remove_periodic_damage<spellId>);

    void HandleHit()
    {
        if (GetCaster()->GetCharmerOrOwnerOrSelf()->HasAura(spellId))
        {
            Unit* target = GetHitUnit();
            target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, 0, target->GetAura(32409)); // SW:D shall not be removed.
            target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
            while (true)
            {
                Aura* aura = target->GetAura(131894); // A Murder of Crows
                if (!aura)
                    break;
                // I hope this won't create bugs...
                if (Unit* caster = aura->GetCaster())
                    caster->m_Events.KillCustomEvents([](BasicEvent* event)
                {
                    auto spellEvent = dynamic_cast<SpellEvent*>(event);
                    return spellEvent && spellEvent->GetSpell()->GetSpellInfo()->Id == 131900;
                });
                target->RemoveAura(aura);
            }
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_common_remove_periodic_damage<spellId>::HandleHit);
    }
};

// 115078 - Paralysis
typedef spell_common_remove_periodic_damage<125755> spell_monk_glyph_of_paralysis;
// 3355 - Freezing Trap
// 19503 - Scatter Shot
typedef spell_common_remove_periodic_damage<119407> spell_hunt_glyph_of_solace;

// 118 - Polymorph
// 28271 - Polymorph (turtle)
// 28272 - Polymorph (pig, not available?)
// 61025 - Polymorph (snake, not available?)
// 61305 - Polymorph (cat)
// 61721 - Polymorph (rabbit)
// 61780 - Polymorph (turkey, not available?)
typedef spell_common_remove_periodic_damage<56375>  spell_mage_glyph_of_polymorph;

// 44521 - Preparation
class spell_preparation : public AuraScript
{
    PrepareAuraScript(spell_preparation);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->ModifyAuraState(AURA_STATE_PREPARATION, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->ModifyAuraState(AURA_STATE_PREPARATION, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_preparation::OnApply, EFFECT_0, SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_preparation::OnRemove, EFFECT_0, SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_common_spells_script()
{
    new aura_script<spell_common_battle_fatigue>("spell_common_battle_fatigue");
    new aura_script<spell_common_battle_fatigue_fix>("spell_common_battle_fatigue_fix");
    new aura_script<spell_common_player_damage_reduction_fix>("spell_common_player_damage_reduction_fix");
    new aura_script<spell_common_player_damage_reduction_fix_90>("spell_common_player_damage_reduction_fix_90");
    new aura_script<spell_common_vengeance>("spell_common_vengeance");
    new aura_script<spell_common_run_speed_marker>("spell_common_run_speed_marker");
    new spell_script<spell_common_combo_point_delayed>("spell_common_combo_point_delayed");
    new spell_script<spell_common_smart_heal_one_target>("spell_common_smart_heal_one_target");
    new spell_script<spell_common_smart_heal_six_targets>("spell_common_smart_heal_six_targets");
    new spell_script<spell_common_smart_heal_raid_25>("spell_common_smart_heal_raid_25");
    new spell_script<spell_common_dispel_reset_cooldown>("spell_common_dispel_reset_cooldown");
    new spell_script<spell_common_raid_haste_30>("spell_common_raid_haste_30");
    new spell_script<spell_common_remove_snare_root>("spell_common_remove_snare_root");
    new spell_script<spell_common_inherit_masters_threat_list>("spell_common_inherit_masters_threat_list");
    new aura_script<spell_common_stealth_proc>("spell_common_stealth_proc");
    new aura_script<spell_common_spell_slow_players>("spell_common_spell_slow_players");
    new spell_script<spell_common_check_destination_availability>("spell_common_check_destination_availability");
    new spell_script<spell_common_unlearn_talent>("spell_common_unlearn_talent");
    new spell_script<spell_monk_glyph_of_paralysis>("spell_monk_glyph_of_paralysis");
    new spell_script<spell_hunt_glyph_of_solace>("spell_hunt_glyph_of_solace");
    new spell_script<spell_mage_glyph_of_polymorph>("spell_mage_glyph_of_polymorph");
    new aura_script<spell_preparation>("spell_preparation");
}
