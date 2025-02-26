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

#ifndef BATTLE_PET_H
#define BATTLE_PET_H

#include "Common.h"
#include "BattlePetAbilityEffect.h"
#include "BattlePetAura.h"

class Player;

enum
{
    BATTLE_PET_MAX_LEVEL        = 25,
    BATTLE_PET_MAX_NAME_LENGTH  = 16,
    BATTLE_PET_MAX_ABILITIES    = 3,

    // 3-12:  Male
    // 13-22: Female
    BATTLE_PET_MAX_BREED        = 22,

    BATTLE_PET_MAX_MAIN_STATES  = 3,
};

// names from 'BattlePetState.db2'
enum BattlePetStates
{
    BATTLE_PET_STATE_IS_DEAD                            = 1,
    BATTLE_PET_STATE_STAT_POWER                         = 18,
    BATTLE_PET_STATE_STAT_STAMINA                       = 19,
    BATTLE_PET_STATE_STAT_SPEED                         = 20,
    BATTLE_PET_STATE_MECHANIC_POISONED                  = 21,
    BATTLE_PET_STATE_MECHANIC_STUNNED                   = 22,
    BATTLE_PET_STATE_MOD_DAMAGE_DEALT_PCT               = 23,
    BATTLE_PET_STATE_MOD_DAMAGE_TAKEN_PCT               = 24,
    BATTLE_PET_STATE_MOD_SPEED_PERCENT                  = 25,
    BATTLE_PET_STATE_RAMPING_DAMAGE_ID                  = 26,
    BATTLE_PET_STATE_RAMPING_DAMAGE_USES                = 27,
    BATTLE_PET_STATE_CONDITION_WAS_DAMAGED_THIS_ROUND   = 28,
    BATTLE_PET_STATE_UNTARGETABLE                       = 29,
    BATTLE_PET_STATE_UNDERGROUND                        = 30,
    BATTLE_PET_STATE_LAST_HIT_TAKEN                     = 31,
    BATTLE_PET_STATE_LAST_HIT_DEALT                     = 32,
    BATTLE_PET_STATE_MECHANIC_FLYING                    = 33,
    BATTLE_PET_STATE_MECHANIC_BURNING                   = 34,
    BATTLE_PET_STATE_TURN_LOCK                          = 35,
    BATTLE_PET_STATE_SWAP_OUT_LOCK                      = 36,
    BATTLE_PET_STATE_STAT_CRITICAL_CHANCE               = 40,
    BATTLE_PET_STATE_STAT_ACCURACY                      = 41,
    BATTLE_PET_STATE_PASSIVE_CRITTER                    = 42,
    BATTLE_PET_STATE_PASSIVE_BEAST                      = 43,
    BATTLE_PET_STATE_PASSIVE_HUMANOID                   = 44,
    BATTLE_PET_STATE_PASSIVE_FLYING                     = 45,
    BATTLE_PET_STATE_PASSIVE_DRAGON                     = 46,
    BATTLE_PET_STATE_PASSIVE_ELEMENTAL                  = 47,
    BATTLE_PET_STATE_PASSIVE_MECHANICAL                 = 48,
    BATTLE_PET_STATE_PASSIVE_MAGIC                      = 49,
    BATTLE_PET_STATE_PASSIVE_UNDEAD                     = 50,
    BATTLE_PET_STATE_PASSIVE_AQUATIC                    = 51,
    BATTLE_PET_STATE_MECHANIC_CHILLED                   = 52,
    BATTLE_PET_STATE_MECHANIC_WEBBED                    = 64,
    BATTLE_PET_STATE_MOD_HEALING_DEALT_PCT              = 65,
    BATTLE_PET_STATE_MOD_HEALING_TAKEN_PCT              = 66,
    BATTLE_PET_STATE_MECHANIC_INVISIBLE                 = 67,
    BATTLE_PET_STATE_UNKILLABLE                         = 68,
    BATTLE_PET_STATE_STAT_DODGE                         = 73,
    BATTLE_PET_STATE_MECHANIC_BLEEDING                  = 77,
    BATTLE_PET_STATE_MECHANIC_BLIND                     = 82,
    BATTLE_PET_STATE_MOD_PET_TYPE_ID                    = 89,
    BATTLE_PET_STATE_INTERNAL_CAPTURE_BOOST             = 90,
    BATTLE_PET_STATE_SWAP_IN_LOCK                       = 98,
    BATTLE_PET_STATE_CONDITION_DID_DAMAGE_THIS_ROUND    = 127,
    BATTLE_PET_STATE_MECHANIC_BOMB                      = 136,
    BATTLE_PET_STATE_RESILITANT                         = 149,

    BATTLE_PET_MAX_STATES                               = 163,
};

enum BattlePetStateModifyTypes
{
    BATTLE_PET_STATE_MODIFY_TYPE_NONE   = 0,
    BATTLE_PET_STATE_MODIFY_TYPE_BOOL   = 1, // state value can only be 0 or 1
    BATTLE_PET_STATE_MODIFY_TYPE_INT    = 2, // state value can be any int value
};

static uint8 BattlePetStateModifyType[BATTLE_PET_MAX_STATES] =
{
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State001  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State002  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State003  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State004  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State017  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State018  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State019  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State020  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State021  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State022  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State023  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State024  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State025  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State026  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State027  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State028  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State029  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State030  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State031  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State032  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State033  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State034  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State035  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State036  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State040  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State041  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State042  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State043  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State044  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State045  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State046  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State047  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State048  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State049  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State050  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State051  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State052  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State053  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State054  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State055  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State056  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State057  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State058  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State059  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State060  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State061  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State062  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State063  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State064  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State065  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State066  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State067  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State068  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State069  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State070  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State071  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State072  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State073  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State074  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State075  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State077  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State078  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State082  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State084  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State085  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State087  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State088  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State089  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State090  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State091  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State093  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State098  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State099  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State100  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State101  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State102  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State103  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State104  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State105  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State106  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State107  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State108  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State113  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State114  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State117  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State118  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State119  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State120  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State121  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*  State122  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State123  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State124  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State126  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State127  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State128  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State129  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State136  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State141  */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State144  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*  State145  */ BATTLE_PET_STATE_MODIFY_TYPE_INT,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State149  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State153  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State158  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*   UNUSED   */ BATTLE_PET_STATE_MODIFY_TYPE_NONE,
    /*  State162  */ BATTLE_PET_STATE_MODIFY_TYPE_BOOL,
};

enum BattlePetFamily
{
    BATTLE_PET_FAMILY_HUMANOID         = 0,
    BATTLE_PET_FAMILY_DRAGONKIN        = 1,
    BATTLE_PET_FAMILY_FLYING           = 2,
    BATTLE_PET_FAMILY_UNDEAD           = 3,
    BATTLE_PET_FAMILY_CRITTER          = 4,
    BATTLE_PET_FAMILY_MAGIC            = 5,
    BATTLE_PET_FAMILY_ELEMENTAL        = 6,
    BATTLE_PET_FAMILY_BEAST            = 7,
    BATTLE_PET_FAMILY_AQUATIC          = 8,
    BATTLE_PET_FAMILY_MECHANICAL       = 9,
    BATTLE_PET_FAMILY_COUNT
};

enum BattlePetDbState
{
    BATTLE_PET_DB_STATE_NONE            = 0,
    BATTLE_PET_DB_STATE_DELETE          = 1,
    BATTLE_PET_DB_STATE_SAVE            = 2
};

enum PetBattleTeamIndex
{
    PET_BATTLE_TEAM_CHALLANGER          = 0,
    PET_BATTLE_TEAM_OPPONENT            = 1,
    PET_BATTLE_MAX_TEAMS                = 2,
};

struct BattlePetAbility
{
    uint32 AbilityId;
    bool OnCooldown;
    uint16 Cooldown;
    uint16 Lockdown;                                    // NYI
};

typedef std::list<std::unique_ptr<BattlePetAura>> BattlePetAuraStore;
typedef std::vector<uint32> BattlePetStateStore;

class BattlePet
{
public:
    // new battle pet
    BattlePet(uint64 id, uint32 speciesId, uint8 family, uint8 level, uint8 quality, uint8 breedId, Player* owner = nullptr)
        : m_owner(owner), m_id(id), m_timestamp(0), m_species(speciesId), m_family(family), m_level(level), m_xp(0), m_curHealth(0),
        m_quality(quality), m_breed(breedId), m_flags(0),
        m_dbState(BATTLE_PET_DB_STATE_SAVE) { Initialise(true); }

    // load from database
    BattlePet(uint64 id, uint32 speciesId, uint8 family, std::string nickname, uint32 timestamp,
        uint8 level, uint16 xp, uint16 curHealth, uint8 quality, uint8 breedId, uint16 flags, Player* owner)
        : m_owner(owner), m_id(id), m_nickname(nickname), m_timestamp(timestamp), m_species(speciesId), m_family(family), m_level(level), m_xp(xp),
         m_curHealth(curHealth), m_quality(quality), m_breed(breedId), m_flags(flags),
         m_dbState(BATTLE_PET_DB_STATE_NONE) { Initialise(false); }

    ~BattlePet();

    void Initialise(bool newBattlePet);
    void InitialiseStates(bool newBattlePet);

    Player* GetOwner() const { return m_owner; }
    ObjectGuid GetId() const { return m_id; }
    uint32 GetSpecies() const { return m_species; }
    uint8 GetFamilty() const { return m_family; }
    std::string GetNickname() const { return m_nickname; }
    uint32 GetTimestamp() const { return m_timestamp; }
    uint8 GetLevel() const { return m_level; }
    uint16 GetXp() const { return m_xp; }
    uint16 GetCurrentHealth() const { return m_curHealth; }
    uint16 GetMaxHealth() const { return m_maxHealth; }
    uint16 GetPower() const;
    uint16 GetSpeed() const;
    uint8 GetQuality() const { return m_quality; }
    uint8 GetBreed() const { return m_breed; }

    uint32 GetNpc() const { return m_npc; }
    uint32 GetDisplayId() const { return m_displayId; }

    void SetCurrentHealth(uint32 health);
    void SetNickname(std::string nickname);
    void SetTimestamp(uint32 timestamp) { m_timestamp = timestamp; }
    void SetXP(uint16 xpGain);
    void SetLevel(uint8 level);

    uint8 GetDbState() const { return m_dbState; }
    void SetDbState(uint8 state) { m_dbState = state; }

    uint8 GetFlags() const { return m_flags; }
    bool HasFlag(uint16 flag) const { return (m_flags & flag) != 0; }
    void SetFlag(uint16 flag);
    void UnSetFlag(uint16 flag);

// -------------------------------------------------------------------------------

    void InitialiseAbilities(bool wild = false);

    void SetBattleInfo(PetBattleTeamIndex teamIndex, int8 globalPetId);

    int8 GetGlobalIndex() { return m_globalIndex; }
    PetBattleTeamIndex GetTeamIndex() { return m_teamIndex; }
    uint16 GetOldLevel() { return m_oldLevel; }
    BattlePetAura* GetAura(uint32 abilityId);

    void CalculateStats(bool currentHealth = false);
    bool IsMainStatState(uint32 state) { return state == BATTLE_PET_STATE_STAT_POWER || state == BATTLE_PET_STATE_STAT_STAMINA || state == BATTLE_PET_STATE_STAT_SPEED; }

    bool IsAlive() { return States[BATTLE_PET_STATE_IS_DEAD] == 0; }
    bool CanAttack() { return !States[BATTLE_PET_STATE_TURN_LOCK] && !States[BATTLE_PET_STATE_MECHANIC_STUNNED] && !States[BATTLE_PET_STATE_MECHANIC_WEBBED]; }
    bool IsTargetable() { return States[BATTLE_PET_STATE_UNTARGETABLE] == 0; }

    void ResetMechanicStates();

    typedef std::unique_ptr<BattlePetAbility> Ability;
    std::array<Ability, BATTLE_PET_MAX_ABILITIES> Abilities;

    BattlePetAuraStore Auras;

    std::array<int32, BATTLE_PET_MAX_STATES> States;

// -------------------------------------------------------------------------------

private:
    Player* m_owner;
    ObjectGuid m_id;
    std::string m_nickname;
    uint32 m_timestamp;
    uint32 m_species;
    uint8  m_family;
    uint8  m_level;
    uint16 m_xp = 0;
    uint16 m_curHealth = 0;
    uint16 m_maxHealth = 0;
    uint16 m_power = 0;
    uint16 m_speed = 0;
    uint8  m_quality;
    uint8  m_breed;
    uint16 m_flags;
    uint8  m_dbState;

    int8  m_globalIndex = 0;
    PetBattleTeamIndex m_teamIndex = PET_BATTLE_TEAM_CHALLANGER;

    uint32 m_npc = 0;
    uint32 m_displayId = 0;
    uint16 m_oldLevel = 0;

    std::array<int32, BATTLE_PET_MAX_MAIN_STATES> m_mainStates;     // used to calculate stats on levelup
};

#endif
