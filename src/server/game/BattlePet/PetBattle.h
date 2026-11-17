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

#ifndef PET_BATTLE_H
#define PET_BATTLE_H

#include "BattlePet.h"

enum PetBattleMisc
{
    PET_BATTLE_MAX_TEAM_PETS    = 3,

    PET_BATTLE_NULL_PET_INDEX   = -1,

    PET_BATTLE_SYSTEM_UPDATE_BATTLE_TIMER   = 300,
};

// sent in SMSG_PET_BATTLE_REQUEST_FAILED
enum PetBattleRequestReason
{
    PET_BATTLE_REQUEST_FAILED                           = 0,
    PET_BATTLE_REQUEST_NOT_HERE                         = 1,
    PET_BATTLE_REQUEST_NOT_DURING_FLYING                = 2,
    PET_BATTLE_REQUEST_GROUND_NOT_ENOUGHT_SMOOTH        = 3,
    PET_BATTLE_REQUEST_AREA_NOT_CLEAR                   = 4,
    PET_BATTLE_REQUEST_ALREADY_IN_COMBAT                = 5,
    PET_BATTLE_REQUEST_DEAD                             = 6,
    PET_BATTLE_REQUEST_SEATED                           = 7,
    PET_BATTLE_REQUEST_NOT_VALID_TARGET                 = 8,
    PET_BATTLE_REQUEST_TOO_FAR                          = 9,
    PET_BATTLE_REQUEST_INVALID_TARGET                   = 10,
    PET_BATTLE_REQUEST_NEED_TO_BE_TRAINER               = 11,
    PET_BATTLE_REQUEST_DECLINED                         = 12,
    PET_BATTLE_REQUEST_ALREADY_IN_PETBATTLE             = 13,
    PET_BATTLE_REQUEST_NEED_PET_IN_SLOTS                = 14,
    PET_BATTLE_REQUEST_PET_ALL_DEAD                     = 15,
    PET_BATTLE_REQUEST_NEED_AT_LEAST_1_PET_IN_SLOT      = 16,
    PET_BATTLE_REQUEST_CODEX_LOCKED_BY_AN_ANOTHER_USER  = 17,
    PET_BATTLE_REQUEST_TARGET_IN_A_BATTLEPET            = 18
};

// names extracted from 'FrameXML/SharedPetBattleTemplates.lua'
enum PetBattleAbilityProcType
{
    PET_BATTLE_ABILITY_PROC_ON_NONE                     = -1,
    PET_BATTLE_ABILITY_PROC_ON_APPLY                    = 0,
    PET_BATTLE_ABILITY_PROC_ON_DAMAGE_TAKEN             = 1,
    PET_BATTLE_ABILITY_PROC_ON_DAMAGE_DEALT             = 2,
    PET_BATTLE_ABILITY_PROC_ON_HEAL_TAKEN               = 3,
    PET_BATTLE_ABILITY_PROC_ON_HEAL_DEALT               = 4,
    PET_BATTLE_ABILITY_PROC_ON_AURA_REMOVED             = 5,
    PET_BATTLE_ABILITY_PROC_ON_ROUND_START              = 6,
    PET_BATTLE_ABILITY_PROC_ON_ROUND_END                = 7,
    PET_BATTLE_ABILITY_PROC_ON_TURN                     = 8,
    PET_BATTLE_ABILITY_PROC_ON_ABILITY                  = 9,
    PET_BATTLE_ABILITY_PROC_ON_SWAP_IN                  = 10,
    PET_BATTLE_ABILITY_PROC_ON_SWAP_OUT                 = 11,
};

// -------------------------------------------------------------------------------

// used to track abilities that last multiple turns
struct PetBattleActiveAbility
{
    uint32 AbilityId;
    uint8 TurnsPassed;
    uint8 TurnsTotal;
};

// -------------------------------------------------------------------------------

enum PetBattleMoveType
{
    PET_BATTLE_MOVE_TYPE_REQUEST_LEAVE                  = 0,
    PET_BATTLE_MOVE_TYPE_CAST                           = 1,
    PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS                   = 2,
    PET_BATTLE_MOVE_TYPE_CATCH                          = 3,
    PET_BATTLE_MOVE_TYPE_LEAVE_PETBATTLE                = 4,
    PET_BATTLE_MOVE_MAX                                 = 5,

    PET_BATTLE_MOVE_TYPE_SWAP_DEAD_PET                  = 10,    // Server side only.
};

struct PendingRoundMove
{
    uint8 MoveType;
    ::BattlePet* BattlePet;
    uint32 AbilityId;
};

// -------------------------------------------------------------------------------

// TODO: check if there are anymore flags
enum PetBattleTeamInputFlags
{
    PET_BATTLE_TEAM_INPUT_FLAG_NONE                     = 0x00,
    PET_BATTLE_TEAM_INPUT_FLAG_LOCK_ABILITY_1           = 0x01,
    PET_BATTLE_TEAM_INPUT_FLAG_LOCK_ABILITY_2           = 0x02,
    PET_BATTLE_TEAM_INPUT_FLAG_LOCK_PET_SWAP            = 0x04,
    PET_BATTLE_TEAM_INPUT_FLAG_SELECT_NEW_PET           = 0x08
};

typedef std::vector<BattlePet*> BattlePetStore;
typedef std::set<BattlePet*> BattlePetSetStore;

class PetBattleTeam
{
public:
    PetBattleTeam(PetBattle* parent, PetBattleTeamIndex teamIndex)
        : m_petBattle(parent), m_teamIndex(teamIndex) { }

    PetBattle* GetParentBattle() const { return m_petBattle; }
    Player* GetOwner() const { return m_owner; }
    uint64 GetOwnerGuid() const { return m_ownerGuid; }
    Creature* GetWildBattlePet() const { return m_wildBattlePet; }
    uint8 GetTeamIndex() const { return m_teamIndex; }

    int8 ConvertToGlobalIndex(int8 localPetIndex) const;
    int8 ConvertToLocalIndex(int8 globalPetIndex) const;

    BattlePet* GetPet(uint32 index);
    BattlePet* GetActivePet() const { return m_activePet; }
    void SetActivePet(BattlePet* battlePet);

    bool IsValidBattlePet(BattlePet* battlePet) const;

    void GetAvaliablePets(BattlePetStore &avaliablePets) const;
    bool CanSwap(BattlePet* battlePet = nullptr, bool ignoreAlive = false) const;

    bool HasMultipleTurnAbility() const;

    bool IsReady() const { return m_ready; }

    void SetPendingMove(uint8 moveType, uint32 abilityId, BattlePet* newActivePet);
    PendingRoundMove GetPendingMove() { return m_pendingMove; }

    void ActivePetPrepareCast(uint32 abilityId);
    bool CanActivePetCast(uint32 abilityId) const;
    void DoCasts(int8 procType = PET_BATTLE_ABILITY_PROC_ON_NONE);

    uint8 GetTrapStatus() const;

    void AddPlayer(Player* player);
    void AddWildBattlePet(Creature* creature);

    void ResetActiveAbility();

    bool HasAuras();
    void ProcessAuras();
    void RemoveExpiredAuras();

    void SetTurn(uint32 turn) { m_turn = turn; }
    uint32 GetTurn() const { return m_turn; }
    void TurnFinished();

    uint8 GetInputStatusFlags() const;

    BattlePetStore BattlePets;
    BattlePetSetStore SeenAction;                       // contains battle pets that have contributed to the battle

    PetBattleActiveAbility ActiveAbility;

private:
    PetBattle* m_petBattle;

    Player* m_owner = nullptr;
    uint64 m_ownerGuid = 0;
    Creature* m_wildBattlePet = nullptr;
    BattlePet* m_activePet = nullptr;
    PetBattleTeamIndex m_teamIndex;
    uint32 m_turn = 0;
    bool m_ready = false;
    PendingRoundMove m_pendingMove;
};

// -------------------------------------------------------------------------------

enum PetBattleEffectTargetEx : uint8
{
    PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE     = 0,
    PET_BATTLE_EFFECT_TARGET_EX_STATE           = 1,
    PET_BATTLE_EFFECT_TARGET_EX_ABILITY_CHANGE  = 2,
    PET_BATTLE_EFFECT_TARGET_EX_PET             = 3,
    PET_BATTLE_EFFECT_TARGET_EX_NONE            = 4,
    PET_BATTLE_EFFECT_TARGET_EX_TRIGGER_ABILITY = 5,
    PET_BATTLE_EFFECT_TARGET_EX_NPC_EMOTE       = 6,
    PET_BATTLE_EFFECT_TARGET_EX_AURA            = 7,
};

struct PetBattleEffectTarget
{
    PetBattleEffectTargetEx Type;
    int8 Target;

    union
    {
        uint32 Health;
        uint32 StatValue;

        struct
        {
            uint32 AuraInstanceId;
            uint32 AbilityId;
            int32  RoundsRemaing;
            uint32 CurrentRound;
        } Aura;

        struct
        {
            uint32 StateId;
            uint32 Value;
        } State;
    } Data;
};

// -------------------------------------------------------------------------------

typedef std::vector<PetBattleEffectTarget> PetBattleEffectTargetStore;

enum PetBattleEffectType : uint8
{
    PET_BATTLE_EFFECT_SET_HEALTH            = 0,
    PET_BATTLE_EFFECT_AURA_APPLY            = 1,
    PET_BATTLE_EFFECT_AURA_REMOVE           = 2,
    PET_BATTLE_EFFECT_AURA_CHANGE           = 3,
    PET_BATTLE_EFFECT_ACTIVE_PET            = 4,
    PET_BATTLE_EFFECT_CATCH                 = 5,
    PET_BATTLE_EFFECT_SET_STATE             = 6,
    PET_BATTLE_EFFECT_SET_MAX_HEALTH        = 7,
    PET_BATTLE_EFFECT_SET_SPEED             = 8,
    PET_BATTLE_EFFECT_SET_POWER             = 9,
    PET_BATTLE_EFFECT_TRIGGER_ABILITY       = 10,
    PET_BATTLE_EFFECT_ABILITY_CHANGE        = 11,
    PET_BATTLE_EFFECT_NPC_EMOTE             = 12,
    PET_BATTLE_EFFECT_AURA_PROCESSING_BEGIN = 13,
    PET_BATTLE_EFFECT_AURA_PROCESSING_END   = 14
};

enum PetBattleEffectFlags : uint32
{
    PET_BATTLE_EFFECT_FLAG_NONE             = 0x00000,
    PET_BATTLE_EFFECT_FLAG_SKIP_TURN        = 0x00001,
    PET_BATTLE_EFFECT_FLAG_MISS             = 0x00002,
    PET_BATTLE_EFFECT_FLAG_CRITICAL         = 0x00004,
    PET_BATTLE_EFFECT_FLAG_BLOCKED          = 0x00008,
    PET_BATTLE_EFFECT_FLAG_DODGE            = 0x00010,
    PET_BATTLE_EFFECT_FLAG_HEAL             = 0x00020,
    PET_BATTLE_EFFECT_FLAG_REFLECT          = 0x00080,
    PET_BATTLE_EFFECT_FLAG_ABSORB           = 0x00100,
    PET_BATTLE_EFFECT_FLAG_IMMUNE           = 0x00200,
    PET_BATTLE_EFFECT_FLAG_STRONG           = 0x00400,
    PET_BATTLE_EFFECT_FLAG_WEAK             = 0x00800,
    PET_BATTLE_EFFECT_FLAG_HIT              = 0x01000,  // probably
    PET_BATTLE_EFFECT_FLAG_PERIODIC         = 0x10000,  // custom flag (better way of doing this?)

    PET_BATTLE_EFFECT_FAIL_MAKS = PET_BATTLE_EFFECT_FLAG_MISS | PET_BATTLE_EFFECT_FLAG_BLOCKED | PET_BATTLE_EFFECT_FLAG_DODGE | PET_BATTLE_EFFECT_FLAG_REFLECT | PET_BATTLE_EFFECT_FLAG_IMMUNE,
};

inline void operator|=(PetBattleEffectFlags& lhs, PetBattleEffectFlags rhs) { lhs = PetBattleEffectFlags(lhs | rhs); }

class PetBattleEffect
{
public:
    PetBattleEffect(PetBattleEffectType type, int8 source = PET_BATTLE_NULL_PET_INDEX, PetBattleEffectFlags flags = PET_BATTLE_EFFECT_FLAG_NONE, uint32 abilityEffect = 0)
        : m_type(type), m_source(source), m_flags(flags), m_abilityEffect(abilityEffect) { }

    PetBattleEffectType GetType() const { return m_type; }
    PetBattleEffectFlags GetFlags() const { return m_flags; }
    int8 GetSource() const { return m_source; }
    uint32 GetAbilityEffect() const { return m_abilityEffect; }
    uint16 GetTurn() const { return m_turn; }
    uint8 GetStackDepth() const { return m_stackDepth; }

    void SetTurn(uint8 turn, uint8 stackDepth) { m_turn = turn; m_stackDepth = stackDepth; }
    void SetAbilityEffect(uint32 abilityEffect) { m_abilityEffect = abilityEffect; }
    void SetNoTarget();
    void SetActivePet(int8 target);
    void UpdateHealth(int8 target, uint32 health);
    void UpdateState(int8 target, uint32 state, uint32 value);
    void UpdateAura(int8 targetPet, uint32 auraInstance, uint32 ability, int32 duration, uint32 turn);
    void UpdateStat(int8 targetPet, uint32 value);

    PetBattleEffectTargetStore Targets;

private:
    PetBattleEffectType m_type;
    int8 m_source;
    PetBattleEffectFlags m_flags;
    uint32 m_abilityEffect;
    uint16 m_turn = 0;
    uint8  m_stackDepth = 0;
};


// Client states for pet battle
//
// LE_PET_BATTLE_STATE_CREATED                  1
// LE_PET_BATTLE_STATE_WAITING_PRE_BATTLE       2
// LE_PET_BATTLE_STATE_ROUND_IN_PROGRESS        3
// LE_PET_BATTLE_STATE_WAITING_FOR_FRONT_PETS   4
// LE_PET_BATTLE_STATE_CREATED_FAILED           5
// LE_PET_BATTLE_STATE_FINAL_ROUND              6
// LE_PET_BATTLE_STATE_FINISHED                 7

// Our internal values, from current view of problem
enum class PetBattleState
{
    Created,
    WaitingForFrontPets,
    InProgress,
    Finished,
    Interrupted,        // Happens when another player attacks you while pet battle is active (action from map upadte, can't end battle right now)
};

enum PetBattleType
{
    PET_BATTLE_TYPE_PVE                     = 0,
    PET_BATTLE_TYPE_PVP_DUEL                = 1,
    PET_BATTLE_TYPE_PVP_MATCHMAKING         = 2
};

enum PetBattleRoundResult
{
    PET_BATTLE_ROUND_RESULT_NONE            = 0,
    PET_BATTLE_ROUND_RESULT_INITIALISE      = 1,
    PET_BATTLE_ROUND_RESULT_NORMAL          = 2,
    PET_BATTLE_ROUND_RESULT_CATCH_OR_KILL   = 3
};

enum PetBattleTrapStatus
{
    PET_BATTLE_TRAP_STATUS_DISABLED             = 0,
    PET_BATTLE_TRAP_STATUS_ENABLED              = 1,
    PET_BATTLE_TRAP_STATUS_CANT_TRAP_DEAD_PET   = 3,
    PET_BATTLE_TRAP_STATUS_HEALTH_TOO_HIGH      = 4,
    PET_BATTLE_TRAP_STATUS_TOO_MANY_PETs        = 5,
    PET_BATTLE_TRAP_STATUS_NOT_CAPTURABLE       = 6,
    PET_BATTLE_TRAP_STATUS_CANT_TRAP_NPC_PET    = 7,
    PET_BATTLE_TRAP_STATUS_ALREADY_TRAPPED      = 8
};

struct PetBattleRequest
{
    // Data to read from packet
    ObjectGuid OpponentGuid;
    uint32 LocationResult;                              // name dumped from client, use unknown

    G3D::Vector3 BattleOrigin;
    G3D::Vector3 TeamPositions[PET_BATTLE_MAX_TEAMS];
    float BattleFacing;

    // Data for creation of pet battle
    PetBattleType Type;
    Player* Challenger;
    Unit* Opponent;
};

typedef std::vector<PetBattleEffect> PetBattleEffectStore;
typedef std::vector<PetBattleTeam*> PetBattleTeamStore;

class PetBattle
{
public:
    PetBattle(uint32 battleId, PetBattleRequest const& request);

    ~PetBattle();

    void StartBattle();
    void EndBattle(PetBattleTeam* team, bool forfeit = false);
    void HandleRound();
    void Update(uint32 diff);

    bool Cast(BattlePet* caster, uint32 abilityId, uint8 turn, int8 procType);

    void SwapActivePet(BattlePet* battlePet, bool ignoreAlive = false);
    void UpdatePetState(BattlePet* source, BattlePet* target, uint32 abilityEffect, uint32 state, int32 value, PetBattleEffectFlags flags = PET_BATTLE_EFFECT_FLAG_NONE);
    void AddAura(BattlePet* source, BattlePet* target, uint32 ability, uint32 abilityEffect, int32 duration, PetBattleEffectFlags flags = PET_BATTLE_EFFECT_FLAG_NONE, uint8 maxAllowed = 0);
    void Kill(BattlePet* killer, BattlePet* victim, uint32 abilityEffect, PetBattleEffectFlags flags);
    void Catch(BattlePet* source, BattlePet* target, uint32 abilityEffect);

    PetBattleTeamIndex GetFirstAttackingTeam();
    PetBattleTeam* GetWinningTeam() { return m_winningTeam; }

    uint32 GetId() const { return m_battleId; }
    uint8 GetType() const { return m_type; }
    uint32 GetRound() const { return m_round; }
    PetBattleState GetState() const { return m_state; }
    uint8 GetRoundResult() const { return m_roundResult; }
    BattlePet* GetCagedPet() const { return m_cagedPet; }

    PetBattleTeam* GetTeam(uint64 guid) const;
    PetBattleTeam* GetTeam(PetBattleTeamIndex team) const;
    PetBattleTeam* Challenger() const { return m_teams[PET_BATTLE_TEAM_CHALLANGER]; }
    PetBattleTeam* Opponent() const { return m_teams[PET_BATTLE_TEAM_OPPONENT]; }

    uint8 GetTeamIndex(uint8 globalPetId) { return globalPetId < PET_BATTLE_MAX_TEAM_PETS ? PET_BATTLE_TEAM_CHALLANGER : PET_BATTLE_TEAM_OPPONENT; }

    // TODO: This is shit
    void AddEffect(PetBattleEffect const& effect) { m_effects.push_back(effect); }

private:
    void SendFinalizeLocation(PetBattleRequest const& request);
    void SendInitialUpdate(Player* player);
    void SendFirstRound(Player* player);
    void SendRoundResult(Player* player);
    void SendFinalRound(Player* player);
    void SendFinished(Player* player);

private:
    uint32 m_battleId;                                  // global id of the pet battle

    PetBattleTeam* m_teams[PET_BATTLE_MAX_TEAMS];       // teams participating in the battle

    uint8 m_type;
    PetBattleState m_state = PetBattleState::Created;
    uint32 m_round = 0;
    uint8 m_roundResult = 0;
    PetBattleEffectStore m_effects;                     // current round effects

    BattlePet* m_cagedPet = nullptr;
    PetBattleTeam* m_winningTeam = nullptr;
};

//          petBattleId
typedef std::map<uint32, PetBattle*> PetBattleStore;
//           playerGuid  petBattleId
typedef std::map<uint64, uint32> PlayerPetBattleStore;

typedef std::set<PetBattle*> PetBattleRemoveStore;

class PetBattleSystem
{
public:
    static PetBattleSystem* instance()
    {
        static PetBattleSystem _instance;
        return &_instance;
    }

    PetBattleSystem()
        : m_globalPetBattleId(0), m_updatePetBattlesTimer(0) { }

    void Update(uint32 diff);

    void Create(PetBattleRequest const& request);
    void Remove(PetBattle* petBattle);

    void ForfietBattle(uint64 guid);
    void ForfietBattle(PetBattle* petBattle, PetBattleTeam* team);

    PetBattle* GetPlayerPetBattle(uint64 guid);

private:
    uint32 m_globalPetBattleId;
    PetBattleStore m_petBattles;                        // contains battle ids to pet battle instances
    PlayerPetBattleStore m_playerPetBattles;            // contains player GUIDs to battle ids

    uint32 m_updatePetBattlesTimer;
    PetBattleRemoveStore m_petBattlesToRemove;
};

#define sPetBattleSystem PetBattleSystem::instance()

#endif // !PET_BATTLE_H
