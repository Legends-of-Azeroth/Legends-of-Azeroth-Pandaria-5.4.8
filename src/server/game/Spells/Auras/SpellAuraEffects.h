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

#ifndef TRINITY_SPELLAURAEFFECTS_H
#define TRINITY_SPELLAURAEFFECTS_H

class Unit;
class AuraEffect;
class IAreaTrigger;
class Aura;

#include "SpellAuras.h"
#include "SpellMgr.h"

typedef void(AuraEffect::*pAuraEffectHandler)(AuraApplication const* aurApp, uint8 mode, bool apply) const;

class AuraEffect
{
    friend void Aura::_InitEffects(uint32 effMask, Unit* caster, int32 *baseAmount);
    friend Aura* Unit::_TryStackingOrRefreshingExistingAura(SpellInfo const* newAura, uint32 effMask, Unit* caster, int32* baseAmount, Item* castItem, ObjectGuid casterGUID);
    friend Aura::~Aura();
    friend Aura::Aura(SpellInfo const*, WorldObject*, Unit*, Item*, ObjectGuid, uint32, int32*);
    friend void Aura::RemoveEffect(uint8 effIndex);

    private:
        ~AuraEffect();
        explicit AuraEffect(Aura* base, uint8 effIndex, int32 *baseAmount, Unit* caster);
        void InitAmount(Unit* caster);

    public:
        Unit* GetCaster() const { return GetBase()->GetCaster(); }
        ObjectGuid GetCasterGUID() const { return GetBase()->GetCasterGUID(); }
        Aura* GetBase() const { return m_base; }
        void GetTargetList(std::list<Unit*> & targetList) const;
        void GetApplicationList(std::list<AuraApplication*> & applicationList) const;
        SpellModifier* GetSpellModifier() const { return m_spellmod; }

        SpellInfo const* GetSpellInfo() const { return m_spellInfo; }
        SpellEffectInfo const& GetSpellEffectInfo() const { return m_spellInfo->Effects[m_effIndex]; }
        uint32 GetId() const { return m_spellInfo->Id; }
        uint32 GetEffIndex() const { return m_effIndex; }
        int32 GetBaseAmount() const { return m_baseAmount; }
        int32 GetAmplitude() const { return m_amplitude; }
        void SetAmplitude(int32 newAmplitude) { m_amplitude = newAmplitude; }

        int32 GetMiscValueB() const { return m_spellInfo->Effects[m_effIndex].MiscValueB; }
        int32 GetMiscValue() const { return m_spellInfo->Effects[m_effIndex].MiscValue; }
        AuraType GetAuraType() const { return (AuraType)m_spellInfo->Effects[m_effIndex].ApplyAuraName; }
        int32 GetAmount() const { return m_amount; }
        float GetFloatAmount() const { return m_floatAmount; }
        void SetAmount(int32 amount);
        void SetFloatAmount(float amount);

        template <class T>
        T GetUserData() const { return m_userData.Get<T>(); }
        template <class T>
        void SetUserData(T data) { m_userData.Set(data); }

        int32 GetPeriodicTimer() const { return m_periodicTimer; }
        void SetPeriodicTimer(int32 periodicTimer) { m_periodicTimer = periodicTimer; }

        float CalculateAmount(Unit* caster, bool recalculate = false);
        void CalculatePeriodic(Unit* caster, bool resetPeriodicTimer = true, bool load = false);
        void CalculateSpellMod();
        void RecalculateSpellMod();
        void ChangeAmount(float newAmount, bool mark = true, bool onStackOrReapply = false);
        void RecalculateAmount() { if (!CanBeRecalculated()) return; ChangeAmount(CalculateAmount(GetCaster()), false); }
        void RecalculateAmount(Unit* caster) { if (!CanBeRecalculated()) return; ChangeAmount(CalculateAmount(caster), false); }
        bool CanBeRecalculated() const { return m_canBeRecalculated; }
        void SetCanBeRecalculated(bool val) { m_canBeRecalculated = val; }
        void HandleEffect(AuraApplication * aurApp, uint8 mode, bool apply);
        void HandleEffect(Unit* target, uint8 mode, bool apply);
        void ApplySpellMod(Unit* target, bool apply);

        void Update(uint32 diff, Unit* caster);
        void UpdatePeriodic(Unit* caster);

        uint32 GetTickNumber() const { return m_tickNumber; }
        void SetTickNumber(uint32 tick) { m_tickNumber = tick; }
        int32 GetTotalTicks() const { return m_amplitude ? (GetBase()->GetMaxDuration() / m_amplitude) : 1;}
        void ResetPeriodic(bool resetPeriodicTimer = false) { if (resetPeriodicTimer) m_periodicTimer = m_amplitude; m_tickNumber = 0;}

        bool IsPeriodic() const { return m_isPeriodic; }
        void SetPeriodic(bool isPeriodic) { m_isPeriodic = isPeriodic; }
        bool IsAffectingSpell(SpellInfo const* spell) const;
        bool HasSpellClassMask() const { return m_spellInfo->Effects[m_effIndex].SpellClassMask; }

        void SendTickImmune(Unit* target, Unit* caster) const;
        void PeriodicTick(AuraApplication * aurApp, Unit* caster) const;

        void HandleProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);

        void CleanupTriggeredSpells(Unit* target);

        // add/remove SPELL_AURA_MOD_SHAPESHIFT (36) linked auras
        void HandleShapeshiftBoosts(Unit* target, bool apply) const;

        static std::set<AuraType> const& WeaponDependingEffects();

        float GetPeriodicCrit() const { return m_periodicCrit; }
        void SetPeriodicCrit(float val) { m_periodicCrit = val; }

        struct FixedPeriodic
        {
            void SetCriticalChance(float value) { bCrit = true; fx_crit_chance = value; }
            float GetCriticalChance() const { return fx_crit_chance; }
            bool HasCritChance() const { return bCrit; }

            void SetFixedDamage(int32 value) { bDamage = true; fx_fixed_damage = value; }
            void SetFixedTotalDamage(int32 value) { fx_fixed_total_damage = value; }
            int32 GetFixedDamage() const { return fx_fixed_damage; }
            int32 GetFixedTotalDamage() const { return fx_fixed_total_damage; }
            bool HasDamage() const { return bDamage; }

        private:
            float fx_crit_chance = 0.0f;
            int32 fx_fixed_damage = 0;
            int32 fx_fixed_total_damage = 0;
            bool bCrit = false;
            bool bDamage = false;
        };

        bool HasFixedDamageInfo() { return hasFixedPeriodic; }
        FixedPeriodic& GetFixedDamageInfo() { return m_fixed_periodic; }

        FixedPeriodic m_fixed_periodic;
        bool hasFixedPeriodic;

    private:
        Aura* const m_base;

        SpellInfo const* const m_spellInfo;
        int32 const m_baseAmount;

        int32 m_amount = 0;
        float m_floatAmount = 0.0f;

        SpellModifier* m_spellmod;

        int32 m_periodicTimer;
        int32 m_amplitude;
        uint32 m_tickNumber;

        AnyValue m_userData;

        uint8 const m_effIndex;
        bool m_canBeRecalculated;
        bool m_isPeriodic;
        bool m_isFixedPeriodic = false;
        float m_periodicCrit = 0.0f;

    private:
        bool IsPeriodicTickCrit(Unit* target, Unit const* caster) const;

    public:
        // aura effect apply/remove handlers
        void HandleNULL(AuraApplication const* /*aurApp*/, uint8 /*mode*/, bool /*apply*/) const
        {
            // not implemented
        }
        void HandleUnused(AuraApplication const* /*aurApp*/, uint8 /*mode*/, bool /*apply*/) const
        {
            // useless
        }
        void HandleNoImmediateEffect(AuraApplication const* /*aurApp*/, uint8 /*mode*/, bool /*apply*/) const
        {
            // aura type not have immediate effect at add/remove and handled by ID in other code place
        }
        //  visibility & phases
        void HandleModInvisibilityDetect(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModInvisibility(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModStealth(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModStealthLevel(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModStealthDetect(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleSpiritOfRedemption(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraGhost(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandlePhase(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandlePhaseGroup(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  unit model
        void HandleAuraModShapeshift(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraTransform(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModScale(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraCloneCaster(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  fight
        void HandleFeignDeath(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModUnattackable(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModDisarm(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModSilence(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModPacify(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModPacifyAndSilence(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraAllowOnlyAbility(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModResiliencePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  tracking
        void HandleAuraTrackResources(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraTrackCreatures(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraTrackStealthed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModStalked(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraUntrackable(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  skills & talents
        void HandleAuraModPetTalentsPoints(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModSkill(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  movement
        void HandleAuraMounted(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraAllowFlight(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraWaterWalk(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraFeatherFall(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraHover(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleWaterBreathing(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleForceMoveForward(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleTurnWhileFalling(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  threat
        void HandleModThreat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModTotalThreat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModTaunt(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  control
        void HandleModConfuse(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModFear(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModStun(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModRoot(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandlePreventFleeing(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleStrangulate(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  charm
        void HandleModPossess(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModFixateTarget(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModCharm(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleCharmConvert(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraControlVehicle(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  modify speed
        void HandleAuraModIncreaseSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseMountedSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseFlightSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseSwimSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModDecreaseSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModUseNormalSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  immunity
        void HandleModStateImmunityMask(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModMechanicImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModEffectImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModStateImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModSchoolImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModDmgImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModDispelImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  modify stats
        //   resistance
        void HandleAuraModResistanceExclusive(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModBaseResistancePCT(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModResistancePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModBaseResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModTargetResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //    stat
        void HandleAuraModStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModPercentStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellDamagePercentFromStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellHealingPercentFromStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellDamagePercentFromAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellHealingPercentFromAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModHealingDone(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleOverrideSpellPowerByAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleOverrideAttackPowerBySpellPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModTotalPercentStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModResistenceOfStatPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModExpertise(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModSpellPowerPercent(AuraApplication const * aurApp, uint8 mode, bool apply) const;
        void HandleAuraModHasteRatingPct(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModPetStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   heal and energize
        void HandleModPowerRegen(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModPowerRegenPCT(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModManaRegen(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseHealth(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseHealth2(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseMaxHealth(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseMaxPowerFlat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseEnergyPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModIncreaseHealthPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraIncreaseBaseHealthPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   fight
        void HandleAuraModParryPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModDodgePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModBlockPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModRegenInterrupt(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModWeaponCritPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModHitChance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellHitChance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellCritChance(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModSpellCritChanceShool(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModCritPct(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   attack speed
        void HandleModCastingSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModMeleeRangedSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModCombatSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModAttackSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModMeleeSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModRangedHaste(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleSpellSlow(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   combat rating
        void HandleModRating(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModRatingFromStat(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   attack power
        void HandleAuraModAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModRangedAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModAttackPowerPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModRangedAttackPowerPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModAttackPowerOfArmor(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //   damage bonus
        void HandleModDamageDone(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModDamagePercentDone(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModOffhandDamagePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleShieldBlockValue(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModAutoAttackDamage(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  power cost
        void HandleModPowerCostPCT(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleModPowerCost(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleArenaPreparation(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleNoReagentUseAura(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraRetainComboPoints(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        //  others
        void HandleAuraDummy(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleChannelDeathItem(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleBindSight(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleForceReaction(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraEmpathy(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModFaction(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleComprehendLanguage(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraConvertRune(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraLinked(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraOpenStable(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraModFakeInebriation(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraOverrideSpells(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraSetVehicle(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandlePreventResurrection(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleMastery(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraForceWeather(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleEnableAltPower(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAuraOverrideAutoattackWithSpell(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandlePlayScene(AuraApplication const* aurApp, uint8 mode, bool apply) const;
        void HandleAreaTrigger(AuraApplication const* aurApp, uint8 mode, bool apply) const;

        // aura effect periodic tick handlers
        void HandlePeriodicDummyAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicTriggerSpellAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicTriggerSpellWithValueAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicDamageAurasTick(Unit* target, Unit* caster) const;
        void HandlePeriodicHealthLeechAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicHealthFunnelAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicHealAurasTick(Unit* target, Unit* caster) const;
        void HandlePeriodicManaLeechAuraTick(Unit* target, Unit* caster) const;
        void HandleObsModPowerAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicEnergizeAuraTick(Unit* target, Unit* caster) const;
        void HandlePeriodicPowerBurnAuraTick(Unit* target, Unit* caster) const;

        // aura effect proc handlers
        void HandleProcTriggerSpellAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);
        void HandleProcTriggerSpellWithValueAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);
        void HandleProcTriggerDamageAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);
        void HandleRaidProcFromChargeAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);
        void HandleRaidProcFromChargeWithValueAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo);
};

namespace Trinity
{
    // Binary predicate for sorting the priority of absorption aura effects
    class AbsorbAuraOrderPred
    {
        public:
            AbsorbAuraOrderPred() { }
            bool operator() (AuraEffect* aurEffA, AuraEffect* aurEffB) const
            {
                SpellInfo const* spellProtoA = aurEffA->GetSpellInfo();
                SpellInfo const* spellProtoB = aurEffB->GetSpellInfo();

                // Wards
                if ((spellProtoA->SpellFamilyName == SPELLFAMILY_MAGE) ||
                    (spellProtoA->SpellFamilyName == SPELLFAMILY_WARLOCK))
                    if (spellProtoA->GetCategory() == 56)
                        return true;
                if ((spellProtoB->SpellFamilyName == SPELLFAMILY_MAGE) ||
                    (spellProtoB->SpellFamilyName == SPELLFAMILY_WARLOCK))
                    if (spellProtoB->GetCategory() == 56)
                        return false;

                // Endurance of Niuzao (Legendary cloak)
                if (spellProtoA->Id == 146193)
                    return true;
                if (spellProtoB->Id == 146193)
                    return false;

                for (auto&& itr : { spellProtoA, spellProtoB })
                {
                    switch (itr->Id)
                    {
                        case 20711:  // Spirit of Redemption
                        case 31230:  // Cheat Death
                        case 31850:  // Ardent Defender
                        case 47788:  // Guardian Spirit
                        case 86949:  // Cauterize
                        case 114556: // Purgatory
                            return itr == spellProtoB;
                    }
                }

                // Maddening Shout
                if (spellProtoA->Id == 117708)
                    return true;
                if (spellProtoB->Id == 117708)
                    return false;

                // Stagger
                if (spellProtoA->Id == 115069)
                    return true;
                if (spellProtoB->Id == 115069)
                    return false;

                // Incanter's Ward
                if (spellProtoA->Id == 1463)
                    return true;
                if (spellProtoB->Id == 1463)
                    return false;

                // Sacred Shield
                if (spellProtoA->Id == 58597)
                    return true;
                if (spellProtoB->Id == 58597)
                    return false;

                // Fel Blossom
                if (spellProtoA->Id == 28527)
                    return true;
                if (spellProtoB->Id == 28527)
                    return false;

                // Divine Aegis
                if (spellProtoA->Id == 47753)
                    return true;
                if (spellProtoB->Id == 47753)
                    return false;

                // Ice Barrier
                if (spellProtoA->GetCategory() == 471)
                    return true;
                if (spellProtoB->GetCategory() == 471)
                    return false;

                // Sacrifice
                if ((spellProtoA->SpellFamilyName == SPELLFAMILY_WARLOCK) &&
                    (spellProtoA->SpellIconID == 693))
                    return true;
                if ((spellProtoB->SpellFamilyName == SPELLFAMILY_WARLOCK) &&
                    (spellProtoB->SpellIconID == 693))
                    return false;

                return false;
            }
    };
}

struct MiscMask
{
    MiscMask(int32 mask) : _mask(mask) { }
    bool operator()(AuraEffect const* eff) const { return eff->GetMiscValue() & _mask; }
private:
    int32 _mask;
};

struct MiscMaskB 
{
    MiscMaskB(int32 mask) : _mask(mask) { }
    bool operator()(AuraEffect const* eff) const { return eff->GetMiscValueB() & _mask; }
private:
    int32 _mask;
};

struct PetStatMod
{
    PetStatMod(Unit* pet, uint32 stat) : m_stat(stat), m_entry(pet->GetEntry()) { }

    bool operator()(AuraEffect const* eff) const
    {
        if (eff->GetMiscValueB() && eff->GetMiscValueB() != m_entry)
            return false;
        return m_stat == eff->GetMiscValue();
    }

private:
    uint32 m_stat, m_entry;
};

template <class Cond>
float Unit::GetTotalAuraMultiplier(AuraType auraType, Cond cond) const
{
    std::map<SpellGroup, int32> sameEffectSpellGroup;
    float multiplier = 1.0f;

    for (auto&& aurEff : GetAuraEffectsByType(auraType))
    {
        if (cond(aurEff))
        {
            // Check if the Aura Effect has a the Same Effect Stack Rule and if so, use the highest amount of that SpellGroup
            // If the Aura Effect does not have this Stack Rule, it returns false so we can add to the multiplier as usual
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(aurEff->GetSpellInfo(), aurEff->GetAmount(), sameEffectSpellGroup))
                AddPct(multiplier, aurEff->GetAmount());
        }
    }
    // Add the highest of the Same Effect Stack Rule SpellGroups to the multiplier
    for (auto&& it : sameEffectSpellGroup)
        AddPct(multiplier, it.second);

    return multiplier;
}

template <class Cond>
int32 Unit::GetTotalAuraModifier(AuraType auraType, Cond cond) const
{
    std::map<SpellGroup, int32> sameEffectSpellGroup;
    float modifier = 0.0f;

    for (auto&& aurEff : GetAuraEffectsByType(auraType))
    {
        if (cond(aurEff))
        {
            // Check if the Aura Effect has a the Same Effect Stack Rule and if so, use the highest amount of that SpellGroup
            // If the Aura Effect does not have this Stack Rule, it returns false so we can add to the multiplier as usual
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(aurEff->GetSpellInfo(), aurEff->GetAmount(), sameEffectSpellGroup))
                modifier += aurEff->GetAmount();
        }
    }
    // Add the highest of the Same Effect Stack Rule SpellGroups to the multiplier
    for (auto&& it : sameEffectSpellGroup)
        modifier += it.second;

    return modifier;
}


#endif
