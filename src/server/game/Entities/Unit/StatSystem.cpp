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

#include "Unit.h"
#include "Player.h"
#include "Pet.h"
#include "Creature.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "World.h"

static void ModifyUInt32(bool apply, uint32& baseValue, int32 amount)
{
    // If amount is negative, change sign and value of apply.
    if (amount < 0)
    {
        apply = !apply;
        amount = -amount;
    }
    if (apply)
        baseValue += amount;
    else
    {
        // Make sure we do not get uint32 overflow.
        if (amount > int32(baseValue))
            amount = baseValue;
        baseValue -= amount;
    }
}

/*#######################################
########        COMMON           ########
#######################################*/

void Unit::UpdateCastingSpeed()
{
    float mult = 1.0f;
    if (TempSummon* pet = ToTempSummon())
    {
        if (Player* owner = pet->GetSpellHasteOwner())
        {
            auto positive = [](AuraEffect const* eff) { return eff->GetAmount() > 0; };
            mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, positive);
            mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_HASTE, positive);
            AddPct(mult, owner->GetRatingBonusValue(CR_HASTE_SPELL));
        }
    }

    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_SPELL_SLOW);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_HASTE);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_SPEED_SLOW_ALL);

    Player* player = ToPlayer();
    if (player)
        AddPct(mult, player->GetRatingBonusValue(CR_HASTE_SPELL));

    float val = 1.f / mult;

    SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED, val);
    SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE, val);

    if (player)
    {
        if (Pet* pet = player->GetPet())
            pet->UpdateCastingSpeed();
        for (auto&& it : m_summons)
            if (it->IsPetGuardianStuff() || it->IsTotem())
                it->UpdateCastingSpeed();
    }
}

void Unit::UpdateAttackSpeed(WeaponAttackType att)
{
    if (att == OFF_ATTACK && !CanDualWield())
        return;

    float remainingTimePct = (float)m_attackTimer[att] / (GetAttackTime(att) * m_modAttackSpeedPct[att]);

    float mult = 1.0f;

    if (IsGuardian())
    {
        Unit* owner = GetOwner();
        if (owner && owner->GetTypeId() == TYPEID_PLAYER)
        {
            auto positive = [](AuraEffect const* eff) { return eff->GetAmount() > 0; };
            mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_HASTE, positive);
            mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_MELEE_RANGED_HASTE, positive);
            mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, positive);
            if (att == RANGED_ATTACK) // Not possible for pets?
            {
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_RANGED_HASTE, positive);
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_HASTE_RANGED, positive);
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_RANGED_HASTE_2, positive);
            }
            else
            {
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_ATTACKSPEED, positive);
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_MELEE_HASTE, positive);
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_MELEE_HASTE_2, positive);
                mult *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_MELEE_HASTE_3, positive);
            }
            AddPct(mult, owner->ToPlayer()->GetRatingBonusValue(att == RANGED_ATTACK ? CR_HASTE_RANGED : CR_HASTE_MELEE));
        }
    }
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_HASTE);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_SPEED_SLOW_ALL);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_RANGED_HASTE);
    mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_RANGED_HASTE_2);

    if (att == RANGED_ATTACK)
    {
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_RANGED_HASTE);
        mult *= GetTotalAuraEffectValue(SPELL_AURA_HASTE_RANGED);
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_RANGED_HASTE_2);
    }
    else
    {
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_ATTACKSPEED);
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE);
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE_2);
        mult *= GetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE_3);
    }

    if (GetTypeId() == TYPEID_PLAYER)
        AddPct(mult, ToPlayer()->GetRatingBonusValue(att == RANGED_ATTACK ? CR_HASTE_RANGED : CR_HASTE_MELEE));

    float val = 1.0f / mult;
    m_modAttackSpeedPct[att] = val;

    float timer = BASE_ATTACK_TIME;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        SpellShapeshiftFormEntry const* entry = sSpellShapeshiftFormStore.LookupEntry(GetShapeshiftForm());
        if (entry && entry->attackSpeed)
        {
            if (att != RANGED_ATTACK)
                timer = entry->attackSpeed;
        }
        else if (Item* item = ToPlayer()->GetWeaponForAttack(att))
            timer = item->GetTemplate()->Delay;
    }
    else if (GetTypeId() == TYPEID_UNIT)
    {
        if (IsSummon())
            timer = ToTempSummon()->GetBaseAttackTimer();
        else if (att == RANGED_ATTACK)
            timer = ToCreature()->GetCreatureTemplate()->RangeAttackTime;
        else
            timer = ToCreature()->GetCreatureTemplate()->BaseAttackTime;
    }

    SetFloatValue(UNIT_FIELD_ATTACK_ROUND_BASE_TIME + att, timer * val);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (att == BASE_ATTACK)
            SetFloatValue(UNIT_FIELD_MOD_HASTE, val);
        else if (att == RANGED_ATTACK)
            SetFloatValue(UNIT_FIELD_MOD_RANGED_HASTE, val);
    }

    m_attackTimer[att] = uint32(GetAttackTime(att) * m_modAttackSpeedPct[att] * remainingTimePct);

    if (att == BASE_ATTACK)
    {
        for (auto&& it : m_summons)
        {
            if (it->IsPetGuardianStuff())
            {
                it->UpdateAttackSpeed(BASE_ATTACK);
                it->UpdateAttackSpeed(OFF_ATTACK);
            }
        }
        if (Player* player = ToPlayer())
        {
            if (Pet* pet = player->GetPet())
            {
                pet->UpdateAttackSpeed(BASE_ATTACK);
                pet->UpdateAttackSpeed(OFF_ATTACK);
            }
        }
    }
}

void Unit::UpdateHasteRegen()
{
    bool calculated = false;

    float mult = 1.0f;
    if (Unit* owner = GetOwner())
    {
        if (owner->GetTypeId() == TYPEID_PLAYER)
        {
            mult = owner->GetHasteMultiplier();
            calculated = true;
        }
    }

    if (!calculated)
        mult = GetHasteMultiplier();

    SetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN, 1.0f / mult);

    if (Player* player = ToPlayer())
        player->UpdatePowerRegeneration();

    for (auto&& itr : GetSummons())
        if (itr->IsPetGuardianStuff())
            itr->UpdateHasteRegen();
}

/*#######################################
########                         ########
########   PLAYERS STAT SYSTEM   ########
########                         ########
#######################################*/

bool Player::UpdateStats(Stats stat)
{
    if (stat > STAT_SPIRIT)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);

    SetStat(stat, int32(value));

    switch (stat)
    {
        case STAT_STRENGTH:
            UpdateParryPercentage();
            break;
        case STAT_AGILITY:
            UpdateArmor();
            UpdateAllCritPercentages();
            UpdateDodgePercentage();
            break;
        case STAT_STAMINA:
            UpdateMaxHealth();
            break;
        case STAT_INTELLECT:
            UpdateAllSpellCritChances();
            UpdateArmor();                                  //SPELL_AURA_MOD_RESISTANCE_OF_INTELLECT_PERCENT, only armor currently
            break;
        case STAT_SPIRIT:
            break;
        default:
            break;
    }

    if (stat == STAT_STAMINA || stat == STAT_INTELLECT || stat == STAT_STRENGTH)
        if (Pet* pet = GetPet())
            pet->UpdateStats(stat);

    if (stat == STAT_STRENGTH)
        UpdateAttackPowerAndDamage(false);
    else if (stat == STAT_AGILITY)
    {
        UpdateAttackPowerAndDamage(false);
        UpdateAttackPowerAndDamage(true);
    }

    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();

    // Update ratings in exist SPELL_AURA_MOD_RATING_FROM_STAT and only depends from stat
    uint32 mask = 0;
    AuraEffectList const& modRatingFromStat = GetAuraEffectsByType(SPELL_AURA_MOD_RATING_FROM_STAT);
    for (AuraEffectList::const_iterator i = modRatingFromStat.begin(); i != modRatingFromStat.end(); ++i)
        if (Stats((*i)->GetMiscValueB()) == stat)
            mask |= (*i)->GetMiscValue();
    if (mask)
    {
        for (uint32 rating = 0; rating < CR_MAX_COMBAT_RATING; ++rating)
            if (mask & (1 << rating))
                ApplyRatingMod(CombatRating(rating), 0, true);
    }
    return true;
}

void Player::UpdatePvpPower()
{
    float value = GetRatingBonusValue(CR_PVP_POWER);

    float pvpHealing = value;
    float pvpDamage = value;

    // http://wow.gamepedia.com/PvP_Power
    switch (GetTalentSpecialization())
    {
        // Healing specializations receive a 100% bonus to healing from PvP Power.
        case SPEC_PALADIN_HOLY:
        case SPEC_PRIEST_HOLY:
        case SPEC_PRIEST_DISCIPLINE:
        case SPEC_SHAMAN_RESTORATION:
        case SPEC_DRUID_RESTORATION:
        case SPEC_MONK_MISTWEAVER:
            pvpDamage = 0.0f;
            break;
        // Damage specializations for Druids, Monks, Paladins, Priests, and Shaman receive a 70% bonus to healing from PvP Power.
        case SPEC_PALADIN_RETRIBUTION:
        case SPEC_PRIEST_SHADOW:
        case SPEC_SHAMAN_ELEMENTAL:
        case SPEC_SHAMAN_ENHANCEMENT:
        case SPEC_DRUID_BALANCE:
        case SPEC_DRUID_FERAL:
        case SPEC_MONK_WINDWALKER:
            ApplyPct(pvpHealing, 70);
            break;
        // All other specializations and classes (including tanking) receive a 40% bonus to healing from PvP Power.
        default:
            ApplyPct(pvpHealing, 40);
            break;
    }

    SetFloatValue(PLAYER_FIELD_PVP_POWER_HEALING, pvpHealing);
    SetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE, pvpDamage);
}

void Player::ApplySpellPowerBonus(int32 amount, bool apply)
{
    ModifyUInt32(apply, m_baseSpellPower, amount);
    UpdateSpellDamageAndHealingBonus();
}

void Player::UpdateSpellDamageAndHealingBonus()
{
    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get healing bonus for all schools
    SetStatInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_ALL));
    // Get damage bonus for all schools
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        SetStatInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS+i, SpellBaseDamageBonusDone(SpellSchoolMask(1 << i)));

    if (Pet* pet = GetPet())
        pet->UpdateAttackPowerAndDamage();

    for (auto&& it : m_summons)
        if (it->IsPetGuardianStuff())
            it->UpdateAttackPowerAndDamage();

    if (GetClass() == CLASS_MONK && HasAuraType(SPELL_AURA_OVERRIDE_ATTACK_POWER_BY_SPD))    // Prevents recursive call if some jerk apply both ap -> spd and spd -> ap auras
        UpdateAttackPowerAndDamage();

    SetFloatValue(PLAYER_FIELD_OVERRIDE_SPELL_POWER_BY_AP_PERCENT, float(GetMaxPositiveAuraModifier(SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT)));
}

bool Player::UpdateAllStats()
{
    for (int8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        float value = GetTotalStatValue(Stats(i));
        SetStat(Stats(i), int32(value));
    }

    UpdateArmor();
    // calls UpdateAttackPowerAndDamage() in UpdateArmor for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    UpdateAttackPowerAndDamage(true);
    UpdateMaxHealth();

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    UpdateAllRatings();
    UpdateAllCritPercentages();
    UpdateAllSpellCritChances();
    UpdateBlockPercentage();
    UpdateParryPercentage();
    UpdateDodgePercentage();
    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();
    UpdateExpertise(BASE_ATTACK);
    UpdateExpertise(OFF_ATTACK);
    UpdateExpertise(RANGED_ATTACK);
    RecalculateRating(CR_ARMOR_PENETRATION);
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    // Killer instinct
    if (AuraEffect* killerInstinct = GetAuraEffect(108300, EFFECT_0))
        killerInstinct->RecalculateAmount();

    return true;
}

void Player::ApplySpellPenetrationBonus(int32 amount, bool apply)
{
    ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, -amount, apply);
    m_spellPenetrationItemMod += apply ? amount : -amount;
}

void Player::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        float value  = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + school));
        SetResistance(SpellSchools(school), int32(value));

        Pet* pet = GetPet();
        if (pet)
            pet->UpdateResistances(school);
    }
    else
        UpdateArmor();
}

void Player::UpdateArmor()
{
    UnitMods unitMod = UNIT_MOD_ARMOR;

    float value = GetModifierValue(unitMod, BASE_VALUE);    // base armor (from items)
    value *= GetModifierValue(unitMod, BASE_PCT);           // armor percent from items
    value += GetModifierValue(unitMod, TOTAL_VALUE);

    //add dynamic flat mods
    AuraEffectList const& mResbyIntellect = GetAuraEffectsByType(SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT);
    for (AuraEffectList::const_iterator i = mResbyIntellect.begin(); i != mResbyIntellect.end(); ++i)
    {
        if ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            value += CalculatePct(GetStat(Stats((*i)->GetMiscValueB())), (*i)->GetAmount());
    }

    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetArmor(int32(value));

    Pet* pet = GetPet();
    if (pet)
        pet->UpdateArmor();

    UpdateAttackPowerAndDamage();                           // armor dependent auras update for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
}

float Player::GetHealthBonusFromStamina()
{
    // Taken from PaperDollFrame.lua - 4.3.4.15595
    float ratio = 10.0f;
    if (gtOCTHpPerStaminaEntry const* hpBase = sGtOCTHpPerStaminaStore.LookupEntry(GetLevel()))
        ratio = hpBase->ratio;

    float stamina = GetStat(STAT_STAMINA);
    float baseStam = std::min(20.0f, stamina);
    float moreStam = stamina - baseStam;

    return baseStam + moreStam * ratio;
}

float Player::GetManaBonusFromIntellect()
{
    return 0;
}

void Player::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + GetHealthBonusFromStamina();
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxHealth((uint32)value);
}

void Player::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float bonusPower = (power == POWER_MANA && GetCreatePowers(power) > 0) ? GetManaBonusFromIntellect() : 0;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreatePowers(power);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) +  bonusPower;
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxPower(power, uint32(value));
}

void Player::UpdateAttackPowerAndDamage(bool ranged)
{
    float val2 = 0.0f;
    float level = float(GetLevel());

    ChrClassesEntry const* entry = sChrClassesStore.LookupEntry(GetClass());
    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 indexMult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        indexMult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
        val2 = (level + std::max(GetStat(STAT_AGILITY) - 10.0f, 0.0f)) * (float)entry->RAPPerAgility;
    }
    else
    {
        float apPerLevel = entry->APPerStrenth == 2 ? 3 : 2;
        float strengthValue = std::max((GetStat(STAT_STRENGTH) - 10.0f) * (float)entry->APPerStrenth, 0.0f);
        float agilityValue = std::max((GetStat(STAT_AGILITY) - 10.0f) * (float)entry->APPerAgility, 0.0f);

        if (GetShapeshiftForm() == FORM_CAT || GetShapeshiftForm() == FORM_BEAR)
        {
            agilityValue += std::max((GetStat(STAT_AGILITY) - 10.0f) * 2, 0.0f);
            apPerLevel = 3;
        }

        val2 = strengthValue + agilityValue + level * apPerLevel;
    }

    SetModifierValue(unitMod, BASE_VALUE, val2);

    float attackPower = 0.f;
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;
    if (!GetAuraEffectsByType(SPELL_AURA_OVERRIDE_ATTACK_POWER_BY_SPD).empty())
        attackPower = CalculatePct<float>((float)SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_ALL), GetMaxPositiveAuraModifier(SPELL_AURA_OVERRIDE_ATTACK_POWER_BY_SPD));
    else
    {
        float baseAttackPower = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
        float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);

        //add dynamic flat mods
        if (!ranged)
        {
            AuraEffectList const& mAPbyArmor = GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR);
            for (AuraEffectList::const_iterator iter = mAPbyArmor.begin(); iter != mAPbyArmor.end(); ++iter)
                // always: ((*i)->GetModifier()->m_miscvalue == 1 == SPELL_SCHOOL_MASK_NORMAL)
                attPowerMod += int32(GetArmor() / (*iter)->GetAmount());
        }
        attackPower = baseAttackPower + attPowerMod;
    }

    SetInt32Value(index, uint32(attackPower));            //UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetFloatValue(indexMult, attPowerMultiplier);

    Pet* pet = GetPet();                                //update pet's AP
    Guardian* guardian = GetGuardianPet();
    //automatically update weapon damage after attack power modification
    if (ranged)
    {
        UpdateDamagePhysical(RANGED_ATTACK);
        if (pet && pet->IsHunterPet()) // At ranged attack change for hunter pet
            pet->UpdateAttackPowerAndDamage();
    }
    else
    {
        UpdateDamagePhysical(BASE_ATTACK);
        if (CanDualWield() && HasOffhandWeapon())           //allow update offhand damage only if player knows DualWield Spec and has equipped offhand weapon
            UpdateDamagePhysical(OFF_ATTACK);
        if (GetClass() == CLASS_SHAMAN || GetClass() == CLASS_PALADIN)                      // mental quickness
            UpdateSpellDamageAndHealingBonus();

        if (pet && pet->IsPetGhoul()) // At melee attack power change for DK pet
            pet->UpdateAttackPowerAndDamage();

        if (guardian && guardian->IsSpiritWolf()) // At melee attack power change for Shaman feral spirit
            guardian->UpdateAttackPowerAndDamage();
    }

    SetFloatValue(PLAYER_FIELD_OVERRIDE_AP_BY_SPELL_POWER_PERCENT, float(GetMaxPositiveAuraModifier(SPELL_AURA_OVERRIDE_ATTACK_POWER_BY_SPD)));
}

void Player::CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& min_damage, float& max_damage)
{
    UnitMods unitMod;

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float att_speed = GetAPMultiplier(attType, normalized);

    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + GetTotalAttackPowerValue(attType) / 14.0f * att_speed;
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = addTotalPct ? GetModifierValue(unitMod, TOTAL_PCT) : 1.0f;

    if (addTotalPct)
    {
        total_pct *= GetTotalAuraEffectValue(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        total_pct *= GetScallingDamageMod();
    }

    if (!normalized)
        total_pct *= GetTotalAuraMultiplier(SPELL_AURA_MOD_AUTOATTACK_DAMAGE);

    float weapon_mindamage = GetWeaponDamageRange(attType, MINDAMAGE);
    float weapon_maxdamage = GetWeaponDamageRange(attType, MAXDAMAGE);

    if (IsInFeralForm())                                    //check if player is druid and in cat or bear forms
    {
        float weaponSpeed = BASE_ATTACK_TIME / 1000.f;
        Item* weapon = GetWeaponForAttack(BASE_ATTACK, false); // usable = false because in feral form
        if (weapon && !weapon->IsBroken())
            weaponSpeed = weapon->GetTemplate()->Delay / 1000.f;

        weapon_mindamage = weapon_mindamage / weaponSpeed;
        weapon_maxdamage = weapon_maxdamage / weaponSpeed;

        // I dunno how exactly this must work, but it seems so
        if (SpellShapeshiftFormEntry const* entry = sSpellShapeshiftFormStore.LookupEntry(GetShapeshiftForm()))
        {
            weapon_mindamage *= entry->attackSpeed / 1000.0f;
            weapon_maxdamage *= entry->attackSpeed / 1000.0f;
        }
    }
    else if (!CanUseAttackType(attType))      //check if player not in form but still can't use (disarm case)
    {
        //cannot use ranged/off attack, set values to 0
        if (attType != BASE_ATTACK)
        {
            min_damage = 0;
            max_damage = 0;
            return;
        }
        weapon_mindamage = BASE_MINDAMAGE;
        weapon_maxdamage = BASE_MAXDAMAGE;
    }
    /*
    TODO: Is this still needed after ammo has been removed?
    else if (attType == RANGED_ATTACK)                       //add ammo DPS to ranged damage
    {
        weapon_mindamage += ammo * att_speed;
        weapon_maxdamage += ammo * att_speed;
    }*/

    min_damage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    max_damage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;
}

void Player::UpdateDamagePhysical(WeaponAttackType attType)
{
    float mindamage;
    float maxdamage;

    CalculateMinMaxDamage(attType, false, true, mindamage, maxdamage);

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, maxdamage);
            break;
        case OFF_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, maxdamage);
            break;
        case RANGED_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, maxdamage);
            break;
    }
}

void Player::UpdateBlockPercentage()
{
    // No block
    float value = 0.0f;
    if (CanBlock())
    {
        // Base value
        value = 5.0f;
        // Increase from SPELL_AURA_MOD_BLOCK_PERCENT aura
        value += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
        // Increase from rating
        value += GetRatingBonusValue(CR_BLOCK);

        if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
             value = value > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_BLOCK) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_BLOCK) : value;

        value = value < 0.0f ? 0.0f : value;
    }
    SetStatFloatValue(PLAYER_FIELD_BLOCK_PERCENTAGE, value);
}

void Player::UpdateBlockValue()
{
    float val = 30.0f;
    val += GetTotalAuraModifier(SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT);
    RoundToInterval(val, 0.0f, 100.0f);
    SetUInt32Value(PLAYER_FIELD_SHIELD_BLOCK, val);
}

void Player::UpdateCritPercentage(WeaponAttackType attType)
{
    uint16 index;
    CombatRating cr;

    switch (attType)
    {
        case OFF_ATTACK:
            index = PLAYER_FIELD_OFFHAND_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
        case RANGED_ATTACK:
            index = PLAYER_FIELD_RANGED_CRIT_PERCENTAGE;
            cr = CR_CRIT_RANGED;
            break;
        case BASE_ATTACK:
        default:
            index = PLAYER_FIELD_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
    }

    int32 auraMod = GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT); 
    Item* weapon = GetWeaponForAttack(attType);
    if (weapon)
        auraMod += GetTotalAuraModifier(SPELL_AURA_MOD_WEAPON_CRIT_PERCENT, [=](AuraEffect const* eff)
    {
        return weapon->IsFitToSpellRequirements(eff->GetSpellInfo());
    });

    float value = GetMeleeCritFromAgility() + GetRatingBonusValue(cr) + auraMod;

    if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
         value = value > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_CRIT) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_CRIT) : value;

    value = value < 0.0f ? 0.0f : value;
    SetStatFloatValue(index, value);
}

void Player::UpdateAllCritPercentages()
{
    UpdateCritPercentage(BASE_ATTACK);
    UpdateCritPercentage(OFF_ATTACK);
    UpdateCritPercentage(RANGED_ATTACK);
}

void Player::UpdateMastery()
{
    float mastery = float(GetTotalAuraModifier(SPELL_AURA_MASTERY)) + GetRatingBonusValue(CR_MASTERY);
    SetFloatValue(PLAYER_FIELD_MASTERY, mastery);

    for (auto&& it : GetAppliedAuras())
    {
        Aura* aura = it.second->GetBase();
        if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR8_MASTERY_SPECIALIZATION))
            aura->RecalculateAmountOfEffects();
    }
}

static const float DodgeOrParryScalingFactor[MAX_CLASSES] =
{
    0.9560f,  // Warrior
    0.8860f,  // Paladin
    0.9880f,  // Hunter
    0.9880f,  // Rogue
    0.9830f,  // Priest
    0.9560f,  // DK
    0.9880f,  // Shaman
    0.9830f,  // Mage
    0.9830f,  // Warlock
    1.4220f,  // Monk
    1.2220f   // Druid
};

static const float DodgeOrParryPerStat[90] =
{
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.194192f,
    0.187000f,
    0.187000f,
    0.180321f,
    0.180321f,
    0.168300f,
    0.168300f,
    0.162871f,
    0.157781f,
    0.157781f,
    0.140250f,
    0.140250f,
    0.136459f,
    0.132868f,
    0.132868f,
    0.126225f,
    0.126225f,
    0.123146f,
    0.120213f,
    0.120214f,
    0.109761f,
    0.107426f,
    0.107426f,
    0.105188f,
    0.103041f,
    0.100980f,
    0.099000f,
    0.097096f,
    0.097096f,
    0.095264f,
    0.088579f,
    0.087052f,
    0.087052f,
    0.085576f,
    0.084150f,
    0.081435f,
    0.080143f,
    0.080143f,
    0.078891f,
    0.077677f,
    0.073081f,
    0.072129f,
    0.071051f,
    0.070084f,
    0.070047f,
    0.068230f,
    0.067257f,
    0.066400f,
    0.065508f,
    0.064664f,
    0.061495f,
    0.059691f,
    0.058929f,
    0.056911f,
    0.055821f,
    0.054698f,
    0.053754f,
    0.052888f,
    0.051482f,
    0.050669f,
    0.049898f,
    0.046399f,
    0.043136f,
    0.040091f,
    0.037265f,
    0.034674f,
    0.032183f,
    0.029918f,
    0.027816f,
    0.025848f,
    0.024015f,
    0.016866f,
    0.011848f,
    0.008322f,
    0.005844f,
    0.004103f,
    0.003131f,
    0.002381f,
    0.001813f,
    0.001381f,
    0.001051f,
};

void Player::UpdateParryPercentage()
{
    const float parryCap[MAX_CLASSES] =
    {
        237.186f,       // Warrior
        237.186f,       // Paladin
        0.0f,           // Hunter
        145.560408f,    // Rogue
        0.0f,           // Priest
        237.186f,       // DK
        145.560408f,    // Shaman
        0.0f,           // Mage
        0.0f,           // Warlock
        90.6424f,       // Monk
        0.0f            // Druid
    };

    // No parry
    float value = 0.0f;
    uint32 pclass = GetClass()-1;
    if (CanParry() && parryCap[pclass] > 0.0f)
    {
        float b = GetCreateStat(STAT_STRENGTH);
        float q = DodgeOrParryPerStat[GetLevel() - 1];
        float c = parryCap[GetClass() - 1];
        float k = DodgeOrParryScalingFactor[GetClass() - 1];
        float t = GetStat(STAT_STRENGTH);
        float r = GetRatingBonusValue(CR_PARRY);

        value = b * q + std::pow((1.0f / c + k / ((t - b) * q + r)), -1);
        value += (3.0f + GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT));

        if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
             value = value > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_PARRY) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_PARRY) : value;

        value = value < 0.0f ? 0.0f : value;
    }
    SetStatFloatValue(PLAYER_FIELD_PARRY_PERCENTAGE, value);
}

void Player::UpdateDodgePercentage()
{
    const float dodgeCap[MAX_CLASSES] =
    {
        90.6425f,       // Warrior
        66.5674f,       // Paladin
        145.560408f,    // Hunter
        145.560408f,    // Rogue
        66.5674f,       // Priest
        90.6425f,       // DK
        145.560408f,    // Shaman
        150.375940f,    // Mage
        150.375940f,    // Warlock
        501.25f,        // Monk
        150.375940f     // Druid
    };

    float b = GetCreateStat(STAT_AGILITY);
    float q = DodgeOrParryPerStat[GetLevel() - 1];
    float c = dodgeCap[GetClass() - 1];
    float k = DodgeOrParryScalingFactor[GetClass() - 1];
    float t = GetStat(STAT_AGILITY);
    float r = GetRatingBonusValue(CR_DODGE);

    float value = b * q + std::pow((1.0f / c + k / ((t - b) * q + r)), -1);
    value += (3.0f + GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT));

    if (sWorld->getBoolConfig(CONFIG_STATS_LIMITS_ENABLE))
         value = value > sWorld->getFloatConfig(CONFIG_STATS_LIMITS_DODGE) ? sWorld->getFloatConfig(CONFIG_STATS_LIMITS_DODGE) : value;

    value = value < 0.0f ? 0.0f : value;
    SetStatFloatValue(PLAYER_FIELD_DODGE_PERCENTAGE, value);
}

void Player::UpdateSpellCritChance(uint32 school)
{
    // For normal school set zero crit chance
    if (school == SPELL_SCHOOL_NORMAL)
    {
        SetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE, 0.0f);
        return;
    }
    // For others recalculate it from:
    float crit = 0.0f;
    // Crit from Intellect
    crit += GetSpellCritFromIntellect();
    // Increase crit from SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    crit += GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    // Increase crit from SPELL_AURA_MOD_CRIT_PCT
    crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
    // Increase crit from spell crit ratings
    crit += GetRatingBonusValue(CR_CRIT_SPELL);

    // Store crit value
    SetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + school, crit);
}

void Player::UpdateArmorPenetration(int32 amount)
{
    // Store Rating Value
    SetUInt32Value((uint16)PLAYER_FIELD_COMBAT_RATINGS + (uint16)CR_ARMOR_PENETRATION, amount);
}

void Player::UpdateMeleeHitChances()
{
    m_modMeleeHitChance = GetFloatValue(PLAYER_FIELD_UI_HIT_MODIFIER);
    m_modMeleeHitChance += GetRatingBonusValue(CR_HIT_MELEE);
}

void Player::UpdateRangedHitChances()
{
    m_modRangedHitChance = GetFloatValue(PLAYER_FIELD_UI_HIT_MODIFIER);
    m_modRangedHitChance += GetRatingBonusValue(CR_HIT_RANGED);
}

void Player::UpdateSpellHitChances()
{
    // Client just sum them
    float expertise = GetFloatValue(PLAYER_FIELD_COMBAT_RATING_EXPERTISE);
    expertise += std::max(GetFloatValue(PLAYER_FIELD_MAINHAND_EXPERTISE), GetFloatValue(PLAYER_FIELD_OFFHAND_EXPERTISE));
    expertise += GetFloatValue(PLAYER_FIELD_RANGED_EXPERTISE);

    m_modSpellHitChance = expertise;
    m_modSpellHitChance += GetFloatValue(PLAYER_FIELD_UI_SPELL_HIT_MODIFIER);
    m_modSpellHitChance += GetRatingBonusValue(CR_HIT_SPELL);
}

void Player::UpdateAllSpellCritChances()
{
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateSpellCritChance(i);
}

void Player::UpdateExpertise(WeaponAttackType attack)
{
    SetFloatValue(PLAYER_FIELD_COMBAT_RATING_EXPERTISE, GetRatingBonusValue(CR_EXPERTISE));

    float expertise = 0.0f;

    if (Item* weapon = GetWeaponForAttack(attack, true))
    {
        if (weapon->IsRangedWeapon() ? attack == RANGED_ATTACK : attack != RANGED_ATTACK)
        {
            for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_EXPERTISE))
            {
                it->RecalculateAmount();    // Might be redundant, but current situation makes so.
                if (it->GetSpellInfo()->EquippedItemClass == -1)
                    expertise += it->GetAmount();
                // item dependent spell
                else if (weapon && weapon->IsFitToSpellRequirements(it->GetSpellInfo()))
                    expertise += it->GetAmount();
            }
        }
    }

    if (expertise < 0.0f)
        expertise = 0.0f;

    switch (attack)
    {
        case BASE_ATTACK:   SetFloatValue(PLAYER_FIELD_MAINHAND_EXPERTISE, expertise);  break;
        case OFF_ATTACK:    SetFloatValue(PLAYER_FIELD_OFFHAND_EXPERTISE, expertise);   break;
        case RANGED_ATTACK: SetFloatValue(PLAYER_FIELD_RANGED_EXPERTISE, expertise);    break;
        default: break;
    }

    UpdateSpellHitChances();
}

void Player::ApplyManaRegenBonus(int32 amount, bool apply)
{
    ModifyUInt32(apply, m_baseManaRegen, amount);
    UpdateManaRegen();
}

void Player::ApplyHealthRegenBonus(int32 amount, bool apply)
{
    ModifyUInt32(apply, m_baseHealthRegen, amount);
}

void Player::UpdateManaRegen()
{
    if (GetPowerIndex(POWER_MANA) == MAX_POWERS)
        return;

    // Mana regen from spirit
    float spiritregen = OCTRegenMPPerSpirit();

    // 5% of base mana per 5 seconds for mage and warlock. 2% for others.
    float basepct;
    switch (GetClass())
    {
        case CLASS_WARLOCK:     // Mage has +400% and -60% auras. Idk wtf is this.
            basepct = 0.05f;
            break;
        default:
            basepct = 0.02f;
            break;
    }

    float baseregen = (GetCreateMana() * basepct) / 5.0f;   // Per second.
    baseregen *= GetTotalAuraMultiplier(SPELL_AURA_MOD_MANA_REGEN_PERCENT);
    if (HasAuraType(SPELL_AURA_MOD_MANA_REGEN_BY_HASTE))
        baseregen /= GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN); // Equivalent to *(1 + haste/100)

    baseregen += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) / 5.0f;

    baseregen *= GetTotalAuraMultiplier(SPELL_AURA_MOD_POWER_REGEN_PERCENT, [](AuraEffect const* effect) { return effect->GetMiscValue() == POWER_MANA; });

    // Set regen rate in cast state apply only on spirit based regen
    int32 modManaRegenInterrupt = GetTotalAuraModifier(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);

    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, baseregen + CalculatePct(spiritregen, modManaRegenInterrupt));
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, baseregen + spiritregen);
}

void Player::UpdateRuneRegen()
{
    for (uint8 i = 0; i < NUM_RUNE_TYPES; ++i)
    {
        float rate = GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);
        rate = 1 / rate;
        rate *= GetTotalAuraMultiplier(SPELL_AURA_MOD_POWER_REGEN_PERCENT,
            [=](AuraEffect const* effect)
        {
            return effect->GetMiscValue() == POWER_RUNES && effect->GetMiscValueB() == i;
        });
        rate = 1 / rate;
        float val = float(1 * IN_MILLISECONDS) / (float)RUNE_READINESS_FULL / rate;
        SetFloatValue(PLAYER_FIELD_RUNE_REGEN + i, val);
    }
}

void Player::_ApplyAllStatBonuses()
{
    SetCanModifyStats(false);

    _ApplyAllAuraStatMods();
    _ApplyAllItemMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

void Player::_RemoveAllStatBonuses()
{
    SetCanModifyStats(false);

    _RemoveAllItemMods();
    _RemoveAllAuraStatMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

/*#######################################
########                         ########
########    MOBS STAT SYSTEM     ########
########                         ########
#######################################*/

bool Creature::UpdateStats(Stats /*stat*/)
{
    return true;
}

bool Creature::UpdateAllStats()
{
    UpdateMaxHealth();
    UpdateAttackPowerAndDamage();
    UpdateAttackPowerAndDamage(true);

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    for (int8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Creature::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        float value  = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + school));
        SetResistance(SpellSchools(school), int32(value));
    }
    else
        UpdateArmor();
}

void Creature::UpdateArmor()
{
    float value = GetTotalAuraModValue(UNIT_MOD_ARMOR);
    SetArmor(int32(value));
}

void Creature::UpdateMaxHealth()
{
    float value = GetTotalAuraModValue(UNIT_MOD_HEALTH);
    SetMaxHealth((uint32)value);
}

void Creature::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float value  = GetTotalAuraModValue(unitMod);
    SetMaxPower(power, uint32(value));
}

void Creature::UpdateAttackPowerAndDamage(bool ranged)
{
    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
    }

    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    SetInt32Value(index, (uint32)base_attPower);            //UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetFloatValue(index_mult, attPowerMultiplier);          //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field

    //automatically update weapon damage after attack power modification
    if (ranged)
        UpdateDamagePhysical(RANGED_ATTACK);
    else
    {
        UpdateDamagePhysical(BASE_ATTACK);
        UpdateDamagePhysical(OFF_ATTACK);
    }
}

void Creature::UpdateDamagePhysical(WeaponAttackType attType)
{
    UnitMods unitMod;
    switch (attType)
    {
        case BASE_ATTACK:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    //float att_speed = float(GetAttackTime(attType))/1000.0f;

    float weapon_mindamage = GetWeaponDamageRange(attType, MINDAMAGE);
    float weapon_maxdamage = GetWeaponDamageRange(attType, MAXDAMAGE);

    /* difference in AP between current attack power and base value from DB */
    float dmg_multiplier = GetCreatureTemplate()->dmg_multiplier;

    if (GetMap()->GetDifficulty() > REGULAR_DIFFICULTY || GetMap()->IsBattleground())
        if (auto mod = sObjectMgr->SelectDifficultyInfo(GetMap(), GetEntry()))
            dmg_multiplier = mod->DamageMod;

    float att_pwr_change = GetTotalAttackPowerValue(attType);

    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + (att_pwr_change * GetAPMultiplier(attType, false) / 14.0f);
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = GetModifierValue(unitMod, TOTAL_PCT);

    total_pct *= GetTotalAuraEffectValue(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    total_pct *= GetScallingDamageMod();

    if (!CanUseAttackType(attType))
    {
        weapon_mindamage = 0;
        weapon_maxdamage = 0;
    }

    float mindamage = ((base_value + weapon_mindamage) * dmg_multiplier * base_pct + total_value) * total_pct;
    float maxdamage = ((base_value + weapon_maxdamage) * dmg_multiplier * base_pct + total_value) * total_pct;

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, maxdamage);
            break;
        case OFF_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, maxdamage);
            break;
        case RANGED_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, maxdamage);
            break;
    }
}

/*#######################################
########                         ########
########    PETS STAT SYSTEM     ########
########                         ########
#######################################*/

#define ENTRY_IMP               416
#define ENTRY_VOIDWALKER        1860
#define ENTRY_SUCCUBUS          1863
#define ENTRY_FELHUNTER         417
#define ENTRY_FELGUARD          17252
#define ENTRY_WATER_ELEMENTAL   510
#define ENTRY_TREANT            1964
#define ENTRY_FIRE_ELEMENTAL    15438
#define ENTRY_GHOUL             26125
#define ENTRY_BLOODWORM         28017

Player* TempSummon::GetSpellHasteOwner() const
{
    if (IsPet())
        return ToPet()->GetOwner();
    if (GetEntry() == 59190)
        return nullptr;
    // TODO: I think most summons are scaled from the owner
    if (IsTotem() && GetEntry() != 61245)   // Capacitor Totem
        if (Unit* summoner = GetSummoner())
            return summoner->ToPlayer();
    if (IsGuardian())
        if (Unit* owner = GetOwner())
            return owner->ToPlayer();
    return nullptr;
}

bool Guardian::UpdateStats(Stats stat)
{
    if (stat >= MAX_STATS)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);
    ApplyStatBuffMod(stat, m_statFromOwner[stat], false);
    float ownersBonus = 0.0f;

    Unit* owner = GetOwner();
    // Handle Death Knight Glyphs and Talents
    float mod = 0.75f;
    switch (stat)
    {
        case STAT_STAMINA:
        {
            mod = 0.3f;

            if (IsPetGhoul() || GetEntry() == 27829)
                mod = 0.45f;
            else if (owner->GetClass() == CLASS_WARLOCK && IsPet())
                mod = 0.75f;
            else
                mod = 0.7f;

            ownersBonus = float(owner->GetStat(stat)) * mod;
            ownersBonus *= GetModifierValue(UNIT_MOD_STAT_STAMINA, TOTAL_PCT);
            value += ownersBonus;
            break;
        }
        case STAT_STRENGTH:
        {
            mod = 0.7f;

            ownersBonus = owner->GetStat(stat) * mod;
            value += ownersBonus;
            break;
        }
        case STAT_INTELLECT:
        {
            if (owner->GetClass() == CLASS_WARLOCK || owner->GetClass() == CLASS_MAGE)
            {
                mod = 0.3f;
                ownersBonus = owner->GetStat(stat) * mod;
            }
            else if (owner->GetClass() == CLASS_DEATH_KNIGHT && GetEntry() == 31216)
            {
                mod = 0.3f;
                ownersBonus = owner->GetStat(stat) * mod;
            }

            value += ownersBonus;
            break;
        }
        default:
            break;
    }

    SetStat(stat, int32(value));
    m_statFromOwner[stat] = ownersBonus;
    ApplyStatBuffMod(stat, m_statFromOwner[stat], true);

    switch (stat)
    {
        case STAT_STRENGTH:         UpdateAttackPowerAndDamage();        break;
        case STAT_AGILITY:          UpdateArmor();                       break;
        case STAT_STAMINA:          UpdateMaxHealth();                   break;
        case STAT_INTELLECT:        UpdateMaxPower(POWER_MANA);          break;
        case STAT_SPIRIT:
        default:
            break;
    }

    return true;
}

bool Guardian::UpdateAllStats()
{
    for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
        UpdateStats(Stats(i));

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    for (uint8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Guardian::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        float value  = GetTotalAuraModValue(UnitMods(UNIT_MOD_RESISTANCE_START + school));

        // hunter and warlock pets gain 40% of owner's resistance
        if (IsPet())
            value += float(CalculatePct(m_owner->GetResistance(SpellSchools(school)), 40));

        SetResistance(SpellSchools(school), int32(value));
    }
    else
        UpdateArmor();
}

void Guardian::UpdateArmor()
{
    float value = 0.0f;
    float bonusArmor = 0.0f;
    UnitMods unitMod = UNIT_MOD_ARMOR;

    // hunter pets gain 35% of owner's armor value, warlock pets gain 100% of owner's armor

    if (IsPet())
    {
        if (IsHunterPet())
            bonusArmor = float(CalculatePct(m_owner->GetArmor(), 170));
        else if (ToPet()->GetOwner()->GetClass() == CLASS_WARLOCK)
        {
            if (GetEntry() == 1860 || GetEntry() == 58960)
                bonusArmor = float(CalculatePct(m_owner->GetArmor(), 400));
            else
                bonusArmor = float(CalculatePct(m_owner->GetArmor(), 300));
        }
        else
            bonusArmor = m_owner->GetArmor();
    }

    value  = GetModifierValue(unitMod, BASE_VALUE);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + bonusArmor;
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    if (IsPet())
        value *= m_owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_PET_STAT, PetStatMod{ this, 13 });

    SetArmor(int32(value));
}

void Guardian::UpdateMaxHealth()
{
    Unit* owner = GetOwner();
    float stamina = GetStat(STAT_STAMINA) - GetCreateStat(STAT_STAMINA);
    float baseValue = stamina * 10.0f;
    float createHealth = GetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE) + GetCreateHealth();

    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
    {
        float ownerPct = 0.0f;
        if (auto petScaling = sObjectMgr->GetPetScaling(GetEntry()))
            ownerPct = petScaling->HpPerHp;
        if (owner->GetClass() == CLASS_HUNTER && IsPet())
            ownerPct = 0.7f;
        if (ownerPct)
        {
            baseValue = owner->GetMaxHealth() * ownerPct;
            createHealth = 0.0f;
        }
    }

    float value = createHealth;
    value *= GetModifierValue(UNIT_MOD_HEALTH, BASE_PCT);
    value += GetModifierValue(UNIT_MOD_HEALTH, TOTAL_VALUE) + baseValue;
    value *= GetModifierValue(UNIT_MOD_HEALTH, TOTAL_PCT);

    if (IsPet())
        value *= owner->GetTotalAuraMultiplier(SPELL_AURA_MOD_PET_STAT, PetStatMod{ this, 1 });

    SetMaxHealth((uint32)value);
}

void Guardian::UpdateMaxPower(Powers power)
{
    if (GetPowerIndex(power) == MAX_POWERS)
        return;

    UnitMods unitMod = UnitMods((uint16)UNIT_MOD_POWER_START + (uint16)power);

    float addValue = (power == POWER_MANA) ? GetStat(STAT_INTELLECT) - GetCreateStat(STAT_INTELLECT) : 0.0f;
    float multiplicator = 15.0f;

    float value  = GetModifierValue(unitMod, BASE_VALUE) + (float)GetCreatePowers(power);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + addValue * multiplicator;
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxPower(power, int32(value));
}

void Guardian::UpdateAttackPowerAndDamage(bool ranged)
{
    if (ranged)
        return;

    UnitMods unitMod = UNIT_MOD_ATTACK_POWER;

    float bonusAP = 2 * GetStat(STAT_STRENGTH) - 20.0f;

    Unit* owner = GetOwner();
    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
    {
        int32 spd = 0;
        for (int32 i = SPELL_SCHOOL_HOLY; i <= SPELL_SCHOOL_ARCANE; ++i)
            if (int32 val = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) + owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i))
                if (val > spd)
                    spd = val;

        float ap = owner->GetTotalAttackPowerValue(owner->GetClass() == CLASS_HUNTER ? RANGED_ATTACK : BASE_ATTACK);
        float apPerSpd = 0.0f;
        float apPerAp = 0.0f;
        float spdPerSpd = 1.0f;
        float spdPerAp = 0.0f;

        if (IsPet() && owner->GetClass() == CLASS_HUNTER)
        {
            apPerAp = 1.0f;
            spdPerAp = 0.5f;
        }

        if (auto petScaling = sObjectMgr->GetPetScaling(GetEntry()))
        {
            apPerSpd  = petScaling->ApPerSpd;
            apPerAp   = petScaling->ApPerAp;
            spdPerSpd = petScaling->SpdPerSpd;
            spdPerAp  = petScaling->SpdPerAp;
        }

        SetBonusDamage(int32(spdPerSpd * (float)spd + spdPerAp * ap));
        bonusAP = apPerSpd * (float)spd + apPerAp * ap;
    }

    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, bonusAP);

    //in BASE_VALUE of UNIT_MOD_ATTACK_POWER for creatures we store data of meleeattackpower field in DB
    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    //UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(UNIT_FIELD_ATTACK_POWER, (int32)base_attPower);
    //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
    SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, attPowerMultiplier);

    //automatically update weapon damage after attack power modification
    UpdateDamagePhysical(BASE_ATTACK);
    UpdateDamagePhysical(OFF_ATTACK);
}

void Guardian::UpdateDamagePhysical(WeaponAttackType attType)
{
    if (attType > OFF_ATTACK)
        return;

    UnitMods unitMod = attType == BASE_ATTACK ? UNIT_MOD_DAMAGE_MAINHAND : UNIT_MOD_DAMAGE_OFFHAND;

    float att_speed = float(GetAttackTime(attType))/1000.0f;

    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + GetTotalAttackPowerValue(attType)/ 14.0f * att_speed;
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = GetModifierValue(unitMod, TOTAL_PCT);

    total_pct *= GetTotalAuraEffectValue(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    total_pct *= GetScallingDamageMod();

    float weapon_mindamage = GetWeaponDamageRange(attType, MINDAMAGE);
    float weapon_maxdamage = GetWeaponDamageRange(attType, MAXDAMAGE);

    float mindamage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    float maxdamage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;

    uint32 fieldMin = attType == BASE_ATTACK ? UNIT_FIELD_MIN_DAMAGE : UNIT_FIELD_MIN_OFF_HAND_DAMAGE;
    uint32 fieldMax = attType == BASE_ATTACK ? UNIT_FIELD_MAX_DAMAGE : UNIT_FIELD_MAX_OFF_HAND_DAMAGE;

    SetStatFloatValue(fieldMin, mindamage);
    SetStatFloatValue(fieldMax, maxdamage);
}

void Guardian::SetBonusDamage(int32 damage)
{
    m_bonusSpellDamage = damage;
    if (GetOwner()->GetTypeId() == TYPEID_PLAYER)
        GetOwner()->SetUInt32Value(PLAYER_FIELD_PET_SPELL_POWER, damage);
}

void Pet::UpdateMaxPower(Powers power)
{
    if (GetPowerIndex(power) == MAX_POWERS)
        return;

    if (GetOwner()->GetClass() == CLASS_MAGE)
    {
        float pct = GetMaxPower(POWER_MANA) ? 100.0f * (float)GetPower(POWER_MANA) / (float)GetMaxPower(POWER_MANA) : 0.0f;
        SetCreateMana(GetOwner()->GetMaxPower(POWER_MANA));
        SetMaxPower(POWER_MANA, (int32)GetCreateMana());
        SetPower(POWER_MANA, CalculatePct(GetMaxPower(POWER_MANA), pct));
    }
    else
        Guardian::UpdateMaxPower(power);
}
