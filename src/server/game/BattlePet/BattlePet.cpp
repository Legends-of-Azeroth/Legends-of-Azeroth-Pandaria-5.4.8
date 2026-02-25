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

#include "BattlePet.h"
#include "DB2Stores.h"
#include "DBCStore.h"
#include "BattlePetMgr.h"
#include "Player.h"
#include "AchievementMgr.h"
#include "BattlePetAbilityEffect.h"
#include "BattlePetSpawnMgr.h"
#include "PetBattle.h"

BattlePet::~BattlePet()
{
}

void BattlePet::Initialise(bool newBattlePet)
{
    // existence is checked before this, no problem should arise
    m_npc = sBattlePetSpeciesStore.LookupEntry(m_species)->NpcId;
    m_displayId = sObjectMgr->GetCreatureTemplate(m_npc)->GetModelByIdx(0)->CreatureDisplayID;

    // setup initial battle pet states
    InitialiseStates(newBattlePet);
}

void BattlePet::InitialiseStates(bool newBattlePet)
{
    // initialise states
    States.fill(0);
    m_mainStates.fill(0);

    States[BATTLE_PET_STATE_STAT_CRITICAL_CHANCE] = 5;
    States[BATTLE_PET_STATE_MOD_PET_TYPE_ID] = m_family;

    // set passive ability states
    static const uint32 familtyPassiveStates[BATTLE_PET_FAMILY_COUNT] =
    {
        BATTLE_PET_STATE_PASSIVE_HUMANOID,
        BATTLE_PET_STATE_PASSIVE_DRAGON,
        BATTLE_PET_STATE_PASSIVE_FLYING,
        BATTLE_PET_STATE_PASSIVE_UNDEAD,
        BATTLE_PET_STATE_PASSIVE_CRITTER,
        BATTLE_PET_STATE_PASSIVE_MAGIC,
        BATTLE_PET_STATE_PASSIVE_ELEMENTAL,
        BATTLE_PET_STATE_PASSIVE_BEAST,
        BATTLE_PET_STATE_PASSIVE_AQUATIC,
        BATTLE_PET_STATE_PASSIVE_MECHANICAL,
    };

    ASSERT(m_family < BATTLE_PET_FAMILY_COUNT);

    States[familtyPassiveStates[m_family]] = 1;

    // set breed specific states
    for (uint32 i = 0; i < sBattlePetBreedStateStore.GetNumRows(); i++)
    {
        auto breedStateEntry = sBattlePetBreedStateStore.LookupEntry(i);
        if (!breedStateEntry)
            continue;

        if (breedStateEntry->BreedId != m_breed)
            continue;

        // handle main stat states
        if (IsMainStatState(breedStateEntry->StateId))
            m_mainStates[breedStateEntry->StateId - BATTLE_PET_STATE_STAT_POWER] += breedStateEntry->Modifier;
        // other states
        else
            States[breedStateEntry->StateId] += breedStateEntry->Modifier;
    }

    // set species specific states
    for (uint32 i = 0; i < sBattlePetSpeciesStateStore.GetNumRows(); i++)
    {
        auto speciesStateEntry = sBattlePetSpeciesStateStore.LookupEntry(i);
        if (!speciesStateEntry)
            continue;

        if (speciesStateEntry->SpeciesId != m_species)
            continue;

        // handle main stat states
        if (IsMainStatState(speciesStateEntry->StateId))
            m_mainStates[speciesStateEntry->StateId - BATTLE_PET_STATE_STAT_POWER] += speciesStateEntry->Modifier;
        else
        // other states
            States[speciesStateEntry->StateId] += speciesStateEntry->Modifier;
    }

    // apply quality modifier
    for (uint8 i = 0; i < BATTLE_PET_MAX_MAIN_STATES; i++)
    {
        auto breedQualityEntry = sBattlePetBreedQualityStore.LookupEntry(7 + m_quality);
        ASSERT(breedQualityEntry);

        m_mainStates[i] *= breedQualityEntry->Multiplier;
    }

    // calculate main stats
    CalculateStats(newBattlePet);

    if (!m_curHealth)
        States[BATTLE_PET_STATE_IS_DEAD] = 1;
}

void BattlePet::InitialiseAbilities(bool wild)
{
    auto abilities = db2::GetAbilitiesForPetSpecies(GetSpecies());
    if (!abilities)
        return;

    for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; ++i)
    {
        auto& abilityPair = (*abilities)[i];
        if (GetLevel() < abilityPair.FirstTier->RequiredLevel)
            continue;

        auto abilityEntry = abilityPair.FirstTier;
        if (GetLevel() >= abilityPair.SecondTier->RequiredLevel)
        {
            switch (i)
            {
                case 0:
                    if (HasFlag(BATTLE_PET_JOURNAL_FLAG_ABILITY_1))
                        abilityEntry = abilityPair.SecondTier;
                    break;
                case 1:
                    if (HasFlag(BATTLE_PET_JOURNAL_FLAG_ABILITY_2))
                        abilityEntry = abilityPair.SecondTier;
                    break;
                case 2:
                    if (HasFlag(BATTLE_PET_JOURNAL_FLAG_ABILITY_3))
                        abilityEntry = abilityPair.SecondTier;
                    break;
                default:
                    std::abort();
                    break;
            }
            if (wild && roll_chance_i(50))
                abilityEntry = abilityPair.SecondTier;
        }

        auto battlePetAbility = new BattlePetAbility();
        battlePetAbility->AbilityId = abilityEntry->AbilityId;
        battlePetAbility->OnCooldown = false;
        battlePetAbility->Cooldown = 0;
        battlePetAbility->Lockdown = 0;
        Abilities[i].reset(battlePetAbility);
    }
}

void BattlePet::SetBattleInfo(PetBattleTeamIndex teamIndex, int8 globalPetId)
{
    ASSERT(globalPetId >= 0);
    m_teamIndex = teamIndex;
    m_globalIndex = globalPetId;
    m_oldLevel = m_level;

    for (auto&& ability : Abilities)
    {
        if (!ability)
            continue;
        ability->OnCooldown = false;
        ability->Cooldown = 0;
        ability->Lockdown = 0;
    }
}

void BattlePet::CalculateStats(bool currentHealth)
{
    // initial values
    for (uint8 i = 0; i < BATTLE_PET_MAX_MAIN_STATES; i++)
        States[BATTLE_PET_STATE_STAT_POWER + i] = m_mainStates[i];

    // apply level modifier
    States[BATTLE_PET_STATE_STAT_POWER] *= m_level;
    States[BATTLE_PET_STATE_STAT_SPEED] *= m_level;

    // calculate health
    m_maxHealth = (10000 + (States[BATTLE_PET_STATE_STAT_STAMINA] * 5) * m_level) / 100;

    if (currentHealth)
        m_curHealth = m_maxHealth;

    // flag battle pet for save
    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::SetFlag(uint16 flag)
{
    if (HasFlag(flag))
        return;

    m_flags |= flag;
    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::UnSetFlag(uint16 flag)
{
    if (!HasFlag(flag))
        return;

    m_flags &= ~flag;
    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::SetCurrentHealth(uint32 health)
{
    if (health > m_maxHealth)
        health = m_maxHealth;

    if (!IsAlive() && health)
        States[BATTLE_PET_STATE_IS_DEAD] = 0;

    m_curHealth = health;
    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::SetNickname(std::string nickname)
{
    m_nickname = nickname;
    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::SetXP(uint16 xpGain)
{
    if (!xpGain)
        return;

    if (m_level == BATTLE_PET_MAX_LEVEL)
        return;

    uint16 baseValue = sGtBattlePetXpStore.LookupEntry(100 + (m_level - 1))->Value;
    uint16 multiplier = sGtBattlePetXpStore.LookupEntry(m_level - 1)->Value;

    uint16 maxXpForLevel = baseValue * multiplier;

    // battle pet has leveled
    if (m_xp + xpGain >= maxXpForLevel)
    {
        m_xp = (m_xp + xpGain) - maxXpForLevel;
        SetLevel(m_level + 1);
    }
    else
        m_xp += xpGain;

    m_dbState = BATTLE_PET_DB_STATE_SAVE;
}

void BattlePet::SetLevel(uint8 level)
{
    // make sure level is valid
    if (!level || level > BATTLE_PET_MAX_LEVEL || level == m_level)
        return;

    if (m_level == BATTLE_PET_MAX_LEVEL || level < m_level)
        m_xp = 0;

    m_level = level;

    if (m_level >= 3 && !m_owner->GetBattlePetMgr().HasLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_2))
        m_owner->GetBattlePetMgr().UnlockLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_2);

    if (m_level >= 5 && !m_owner->GetBattlePetMgr().HasLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_3))
        m_owner->GetBattlePetMgr().UnlockLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_3);

    CalculateStats(true);

    // update world object if battle pet is currently summoned
    if (m_owner->GetBattlePetMgr().GetCurrentSummonId() == m_id)
    {
        if (auto worldPet = m_owner->GetBattlePetMgr().GetCurrentSummon())
        {
            worldPet->SetUInt32Value(UNIT_FIELD_WILD_BATTLE_PET_LEVEL, m_level);
            worldPet->SetHealth(m_curHealth);
            worldPet->SetMaxHealth(m_maxHealth);
        }
    }

    GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_REACH_BATTLE_PET_LEVEL, level, uint32(1 << GetFamilty()));
}

uint16 BattlePet::GetSpeed() const
{
    int32 modPercent = States[BATTLE_PET_STATE_MOD_SPEED_PERCENT];

    // Flying (239)
    // Flying creatures gain 50% extra speed while above 50% health
    if (States[BATTLE_PET_STATE_PASSIVE_FLYING])
        if ((GetCurrentHealth() * 100 / GetMaxHealth()) > 50)
            modPercent += 50;

    return (States[BATTLE_PET_STATE_STAT_SPEED] + CalculatePct(States[BATTLE_PET_STATE_STAT_SPEED], modPercent)) / 100;
}

uint16 BattlePet::GetPower() const
{
    return (States[BATTLE_PET_STATE_STAT_POWER] + CalculatePct(States[BATTLE_PET_STATE_STAT_POWER], States[BATTLE_PET_STATE_MOD_DAMAGE_DEALT_PCT])) / 100;
}

void BattlePet::ResetMechanicStates()
{
    States[BATTLE_PET_STATE_MECHANIC_POISONED]  = 0;
    States[BATTLE_PET_STATE_MECHANIC_STUNNED]   = 0;
    States[BATTLE_PET_STATE_MOD_SPEED_PERCENT]  = 0;
    States[BATTLE_PET_STATE_UNTARGETABLE]       = 0;
    States[BATTLE_PET_STATE_UNDERGROUND]        = 0;
    States[BATTLE_PET_STATE_MECHANIC_FLYING]    = 0;
    States[BATTLE_PET_STATE_MECHANIC_BURNING]   = 0;
    States[BATTLE_PET_STATE_TURN_LOCK]          = 0;
    States[BATTLE_PET_STATE_SWAP_OUT_LOCK]      = 0;
    States[BATTLE_PET_STATE_MECHANIC_CHILLED]   = 0;
    States[BATTLE_PET_STATE_MECHANIC_WEBBED]    = 0;
    States[BATTLE_PET_STATE_MECHANIC_INVISIBLE] = 0;
    States[BATTLE_PET_STATE_UNKILLABLE]         = 0;
    States[BATTLE_PET_STATE_MECHANIC_BLEEDING]  = 0;
    States[BATTLE_PET_STATE_MECHANIC_BLIND]     = 0;
    States[BATTLE_PET_STATE_SWAP_IN_LOCK]       = 0;
    States[BATTLE_PET_STATE_MECHANIC_BOMB]      = 0;
    States[BATTLE_PET_STATE_RESILITANT]         = 0;
}

BattlePetAura* BattlePet::GetAura(uint32 abilityId)
{
    for (auto&& aura : Auras)
        if (aura->GetAbility() == abilityId && !aura->HasExpired())
            return aura.get();

    return nullptr;
}
