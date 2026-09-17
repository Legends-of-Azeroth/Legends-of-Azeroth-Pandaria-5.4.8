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

#include "PetBattle.h"
#include "BattlePetAbilityEffect.h"
#include "BattlePetMgr.h"
#include "BattlePetSpawnMgr.h"
#include "Player.h"
#include "Random.h"

void PetBattleTeam::AddPlayer(Player* player)
{
    m_owner = player;
    m_ownerGuid = player->GetGUID();

    // add player loadout battle pets to team
    int8 localIndex = 0;
    for (uint8 i = 0; i < BATTLE_PET_MAX_LOADOUT_SLOTS; i++)
    {
        uint64 battlePetId = player->GetBattlePetMgr().GetLoadoutSlot(i);
        if (!battlePetId)
            continue;

        auto battlePet = player->GetBattlePetMgr().GetBattlePet(battlePetId);
        if (!battlePet->IsAlive())
            continue;

        // bind pet battle information to battle pet
        battlePet->SetBattleInfo(m_teamIndex, ConvertToGlobalIndex(localIndex++));
        battlePet->InitialiseAbilities();

        BattlePets.push_back(battlePet);
    }

    if (m_petBattle->GetType() == PET_BATTLE_TYPE_PVE)
        m_activePet = GetPet(0);
}

void PetBattleTeam::AddWildBattlePet(Creature* creature)
{
    auto battlePet = sBattlePetSpawnMgr->GetWildBattlePet(creature);
    ASSERT(battlePet);

    battlePet->SetBattleInfo(m_teamIndex, ConvertToGlobalIndex(BattlePets.size()));

    BattlePets.push_back(battlePet);

    // only update creature if it's the first to the team
    if (!m_wildBattlePet)
    {
        m_wildBattlePet = creature;
        SetActivePet(battlePet);
    }
}

void PetBattleTeam::ActivePetPrepareCast(uint32 abilityId)
{
    if (!abilityId)
        return;

    // find the longest duration effect for ability
    uint8 duration = 0;
    for (uint32 i = 0; i < sBattlePetAbilityStore.GetNumRows(); i++)
    {
        auto abilityTurnEntry = sBattlePetAbilityTurnStore.LookupEntry(i);
        if (!abilityTurnEntry || abilityTurnEntry->AbilityId != abilityId)
            continue;

        if (abilityTurnEntry->Duration >= duration)
            duration = abilityTurnEntry->Duration;
    }

    ActiveAbility.AbilityId = abilityId;
    ActiveAbility.TurnsPassed = 1;
    ActiveAbility.TurnsTotal = duration;
}

bool PetBattleTeam::CanActivePetCast(uint32 abilityId) const
{
    if (HasMultipleTurnAbility())
        return false;

    if (!sBattlePetAbilityStore.LookupEntry(abilityId))
        return false;

    // make sure active pet has ability and it isn't on cooldown
    for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
        if (m_activePet->Abilities[i] && m_activePet->Abilities[i]->AbilityId == abilityId && !m_activePet->Abilities[i]->OnCooldown)
            return true;

    return false;
}

uint8 PetBattleTeam::GetTrapStatus() const
{
    if (m_petBattle->GetType() != PET_BATTLE_TYPE_PVE)
        return PET_BATTLE_TRAP_STATUS_NOT_CAPTURABLE;

    if (m_teamIndex == PET_BATTLE_TEAM_OPPONENT)
        return PET_BATTLE_TRAP_STATUS_DISABLED;

    auto &battlePetMgr = m_owner->GetBattlePetMgr();

    // player needs to have a trap ability
    if (!battlePetMgr.GetTrapAbility())
        return PET_BATTLE_TRAP_STATUS_DISABLED;

    // player can only catch a single pet per battle
    if (m_petBattle->GetCagedPet())
        return PET_BATTLE_TRAP_STATUS_ALREADY_TRAPPED;

    auto team = m_petBattle->GetTeam(PET_BATTLE_TEAM_OPPONENT);

    if (!team->GetActivePet()->IsAlive())
        return PET_BATTLE_TRAP_STATUS_CANT_TRAP_DEAD_PET;

    // make sure player can store more battle pets
    if (!battlePetMgr.CanStoreBattlePet(team->GetActivePet()->GetSpecies()))
        return PET_BATTLE_TRAP_STATUS_TOO_MANY_PETs;

    // check if pet is below 35% health
    if (team->GetActivePet()->GetCurrentHealth() > CalculatePct(team->GetActivePet()->GetMaxHealth(), 35))
        return PET_BATTLE_TRAP_STATUS_HEALTH_TOO_HIGH;

    return PET_BATTLE_TRAP_STATUS_ENABLED;
}

bool PetBattleTeam::CanSwap(BattlePet* battlePet, bool ignoreAlive) const
{
    if (HasMultipleTurnAbility())
        return false;

    if (!m_activePet->IsAlive() && !ignoreAlive)
        return false;

    // used for abilities such as Sticky Web (339)
    if (m_activePet->States[BATTLE_PET_STATE_SWAP_OUT_LOCK])
        return false;

    if (battlePet)
    {
        if (!battlePet->IsAlive())
            return false;

        // used for abilities such as Banished (717)
        if (battlePet->States[BATTLE_PET_STATE_SWAP_IN_LOCK])
            return false;
    }

    return true;
}

bool PetBattleTeam::HasMultipleTurnAbility() const
{
    if (ActiveAbility.AbilityId && ActiveAbility.TurnsTotal != 1 && ActiveAbility.TurnsPassed <= ActiveAbility.TurnsTotal)
        return true;

    return false;
}

void PetBattleTeam::DoCasts(int8 procType)
{
    if (!m_activePet->IsAlive())
        return;

    if (!m_activePet->CanAttack())
        return;

    bool noProc = procType == PET_BATTLE_ABILITY_PROC_ON_NONE;
    m_petBattle->Cast(m_activePet, ActiveAbility.AbilityId, noProc ? ActiveAbility.TurnsPassed : 0, procType);
}

void PetBattleTeam::GetAvaliablePets(BattlePetStore &avaliablePets) const
{
    for (auto&& battlePet : BattlePets)
    {
        if (!battlePet->IsAlive())
            continue;

        // make sure local pet can be swapped with active
        if (!CanSwap(battlePet, true))
            continue;

        avaliablePets.push_back(battlePet);
    }
}

void PetBattleTeam::SetActivePet(BattlePet* battlePet)
{
    m_activePet = battlePet;

    // alert client of active pet swap
    PetBattleEffect effect{ PET_BATTLE_EFFECT_ACTIVE_PET, battlePet->GetGlobalIndex() };
    effect.SetActivePet(battlePet->GetGlobalIndex());

    m_petBattle->AddEffect(effect);

    m_ready = true;

    SeenAction.insert(battlePet);
}

// checks if a battle pet belongs to a pet battle team
bool PetBattleTeam::IsValidBattlePet(BattlePet* battlePet) const
{
    bool isValid = false;
    for (auto battlePetTeam : BattlePets)
        if (battlePetTeam == battlePet)
            isValid = true;

    return isValid;
}

uint8 PetBattleTeam::GetInputStatusFlags() const
{
    uint8 flags = PET_BATTLE_TEAM_INPUT_FLAG_NONE;

    // TODO: more checks probably required
    if (HasMultipleTurnAbility())
        flags |= (PET_BATTLE_TEAM_INPUT_FLAG_LOCK_ABILITY_1 | PET_BATTLE_TEAM_INPUT_FLAG_LOCK_ABILITY_2);

    if (!CanSwap())
        flags |= PET_BATTLE_TEAM_INPUT_FLAG_LOCK_PET_SWAP;

    if (!m_activePet->IsAlive())
    {
        BattlePetStore avaliablePets;
        GetAvaliablePets(avaliablePets);

        if (avaliablePets.size())
            flags |= PET_BATTLE_TEAM_INPUT_FLAG_SELECT_NEW_PET;
        else
            flags |= PET_BATTLE_TEAM_INPUT_FLAG_LOCK_PET_SWAP;
    }

    return flags;
}

void PetBattleTeam::ResetActiveAbility()
{
    ActiveAbility.AbilityId = 0;
    ActiveAbility.TurnsPassed = 0;
    ActiveAbility.TurnsTotal = 0;
}

bool PetBattleTeam::HasAuras()
{
    for (auto&& battlePet : BattlePets)
        if (!battlePet->Auras.empty())
            return true;
    return false;
}

void PetBattleTeam::ProcessAuras()
{
    // process any active auras
    for (auto&& battlePet : BattlePets)
    {
        for (auto&& aura : battlePet->Auras)
        {
            if (!aura->HasExpired())
                aura->Process();

            // 543 - Prowl
            if (aura->GetAbility() == 543
                && aura->GetTurn() > 2
                && m_activePet->States[BATTLE_PET_STATE_CONDITION_DID_DAMAGE_THIS_ROUND])
                aura->Expire();

            // remove next attack auras (Attack Reduction)
            if (aura->GetDuration() == -1
                && aura->GetTurn() > 2
                && m_activePet->States[BATTLE_PET_STATE_CONDITION_DID_DAMAGE_THIS_ROUND])
                aura->Expire();
        }
    }
}

typedef std::vector<BattlePetAura*> AuraRemovalStore;

void PetBattleTeam::RemoveExpiredAuras()
{
    for (auto&& battlePet : BattlePets)
    {
        for (auto itr = battlePet->Auras.begin(); itr != battlePet->Auras.end();)
        {
            if ((*itr)->HasExpired())
                battlePet->Auras.erase(itr++);
            else
                ++itr;
        }
    }
}

void PetBattleTeam::TurnFinished()
{
    RemoveExpiredAuras();

    // reduce ability cooldowns
    for (auto&& battlePet : BattlePets)
        for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
            if (battlePet->Abilities[i] && battlePet->Abilities[i]->Cooldown)
                battlePet->Abilities[i]->Cooldown--;

    if (!HasMultipleTurnAbility())
        m_ready = false;

    // Do next turn for PvE team
    if (!m_ready && !m_owner)
    {
        std::vector<uint32> avaliableAbilities;
        for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
            if (m_activePet->Abilities[i])
                if (CanActivePetCast(m_activePet->Abilities[i]->AbilityId))
                    avaliableAbilities.push_back(m_activePet->Abilities[i]->AbilityId);

        // cast ability
        if (avaliableAbilities.size())
            SetPendingMove(PET_BATTLE_MOVE_TYPE_CAST, avaliableAbilities[rand() % avaliableAbilities.size()], nullptr);
        // skip turn
        else
            SetPendingMove(PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS, 0, m_activePet);
    }
}

void PetBattleTeam::SetPendingMove(uint8 moveType, uint32 abilityId, BattlePet* newActivePet)
{
    m_pendingMove.MoveType = moveType;
    m_pendingMove.AbilityId = abilityId;
    m_pendingMove.BattlePet = newActivePet;
    m_ready = true;
}

// -------------------------------------------------------------------------------

int8 PetBattleTeam::ConvertToGlobalIndex(int8 localPetIndex) const
{
    ASSERT(localPetIndex >= 0);
    ASSERT(localPetIndex < PET_BATTLE_MAX_TEAM_PETS);
    return localPetIndex + (m_teamIndex == PET_BATTLE_TEAM_CHALLANGER ? 0 : PET_BATTLE_MAX_TEAM_PETS);
}

int8 PetBattleTeam::ConvertToLocalIndex(int8 globalPetIndex) const
{
    ASSERT(globalPetIndex >= 0);
    ASSERT(globalPetIndex < (PET_BATTLE_MAX_TEAM_PETS * PET_BATTLE_MAX_TEAMS));
    return globalPetIndex - (m_teamIndex == PET_BATTLE_TEAM_CHALLANGER ? 0 : PET_BATTLE_MAX_TEAM_PETS);
}

BattlePet* PetBattleTeam::GetPet(uint32 index)
{
    if (index >= PET_BATTLE_MAX_TEAM_PETS)
    {
        TC_LOG_ERROR("battlepets", "PetBattleTeam::GetPet invalid pet index {}", index);
        return nullptr;
    }
    return BattlePets[index];
}

// -------------------------------------------------------------------------------

PetBattle::PetBattle(uint32 battleId, PetBattleRequest const& request)
    : m_battleId(battleId), m_type(request.Type), m_roundResult(PET_BATTLE_ROUND_RESULT_NONE)
{
    // FIXME
    auto challengerTeam = new PetBattleTeam(this, PET_BATTLE_TEAM_CHALLANGER);
    challengerTeam->AddPlayer(request.Challenger);
    challengerTeam->ResetActiveAbility();

    m_teams[PET_BATTLE_TEAM_CHALLANGER] = challengerTeam;

    auto opponentTeam = new PetBattleTeam(this, PET_BATTLE_TEAM_OPPONENT);
    opponentTeam->ResetActiveAbility();

    if (m_type == PET_BATTLE_TYPE_PVE)
    {
        opponentTeam->AddWildBattlePet(request.Opponent->ToCreature());

        // TODO: nearby wild battle pets should join the pet battle as well
        // ...
    }
    else
        opponentTeam->AddPlayer(request.Opponent->ToPlayer());
    
    m_teams[PET_BATTLE_TEAM_OPPONENT] = opponentTeam;

    SendFinalizeLocation(request);
}

PetBattle::~PetBattle()
{
    // clean up teams
    for (uint8 i = 0; i < PET_BATTLE_MAX_TEAMS; i++)
        delete m_teams[i];
}

void PetBattle::StartBattle()
{
    for (auto&& team : m_teams)
    {
        if (Player* player = team->GetOwner())
        {
            player->GetBattlePetMgr().UnSummonCurrentBattlePet(true);
            SendFirstRound(player);
        }
    }

    m_effects.clear();
    m_state = PetBattleState::InProgress;
    m_round++;

    for (auto&& team : m_teams)
        team->TurnFinished();
}

void PetBattle::EndBattle(PetBattleTeam* lostTeam, bool forfeit)
{
    if (std::this_thread::get_id() != sWorld->GetThreadId())
    {
        m_state = PetBattleState::Interrupted;
        return;
    }

    // if no winning team is specified, battle was forcefully ended
    m_winningTeam = nullptr;
    if (lostTeam)
        m_winningTeam = m_teams[!lostTeam->GetTeamIndex()];

    for (auto team : m_teams)
    {
        for (auto battlePet : team->BattlePets)
        {
            // remove auras
            for (auto&& aura : battlePet->Auras)
                aura->OnExpire();

            battlePet->Auras.clear();

            // remove any remaining mechanic states not cleaned up on aura removal
            battlePet->ResetMechanicStates();
        }

        if (auto player = team->GetOwner())
        {
            for (auto&& battlePet : team->BattlePets)
            {
                // make sure battle wasn't forcefully ended
                if (m_winningTeam)
                {
                    // remove 10% health on forfeit
                    if (team != m_winningTeam && forfeit)
                    {
                        uint16 reduction = CalculatePct(battlePet->GetCurrentHealth(), 10);
                        battlePet->SetCurrentHealth(battlePet->GetCurrentHealth() - reduction);
                    }

                    // award XP to battle pets that actively participated in the battle
                    if (team->SeenAction.find(battlePet) != team->SeenAction.end()
                        && GetType() == PET_BATTLE_TYPE_PVE
                        && m_winningTeam == team
                        && battlePet->GetLevel() != BATTLE_PET_MAX_LEVEL
                        && battlePet->IsAlive())
                    {
                        // XP gain formular from: http://www.wowwiki.com/Pet_Battle_System#Calculating_experience
                        uint16 xp = 0;
                        for (auto&& opponentBattlePet : m_teams[!team->GetTeamIndex()]->BattlePets)
                        {
                            int levelDifference = opponentBattlePet->GetLevel() - battlePet->GetLevel();

                            // level difference roof capped at +2
                            if (levelDifference > 2)
                                levelDifference = 2;
                            // level difference floor capped at -4
                            else if (levelDifference < -4)
                                levelDifference = -4;

                            xp += ((opponentBattlePet->GetLevel() + 9) * (levelDifference + 5)) / team->SeenAction.size();
                        }

                        battlePet->SetXP(xp);
                    }

                    // update battle pet clientside
                    player->GetBattlePetMgr().SendBattlePetUpdate(battlePet, false);
                }
            }

            // save captured pets
            if (m_winningTeam == team && GetType() == PET_BATTLE_TYPE_PVE)
            {
                if (auto battlePet = GetCagedPet())
                {
                    auto &battlePetMgr = player->GetBattlePetMgr();
                    if (!battlePetMgr.CanStoreBattlePet(battlePet->GetSpecies()))
                        continue;

                    uint8 level = battlePet->GetLevel();

                    // level 16-20 pets lose 1 level when caught
                    if (level >= 16 && level <= 20)
                        level--;
                    // level 21-25 pets lose 2 levels when caught
                    else if (level >= 21 && level <= 25)
                        level -= 2;

                    battlePetMgr.Create(battlePet->GetSpecies(), level, battlePet->GetBreed(), battlePet->GetQuality());
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET, battlePet->GetSpecies(), uint32(1 << battlePet->GetFamilty()), 0, player);
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAPTURE_BATTLE_PET2, 1, 0, 0, player);
                }
            }

            if (team == m_winningTeam)
            {
                // Idk how this supposed to work. Comments on wowhead say what: only frist pet, non-pvp, don't swap pet.
                // At current time it's uselss because in pve battles opponenet is a single pet. But nevertheless...
                uint32 familyMask = 0;
                if (GetType() == PET_BATTLE_TYPE_PVE)
                    for (auto&& pet : Opponent()->BattlePets)
                        familyMask |= (1 << pet->GetFamilty());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_PET_BATTLE, 1, familyMask, 0, player);
            }
            else
                player->ResetCriterias(CRITERIA_RESET_TYPE_LOSE_PET_BATTLE, 0);

            player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC);
            player->SetControlled(false, UNIT_STATE_ROOT);

            // alert client of pet battle end
            if (!m_winningTeam)
                // instant battle end
                SendFinished(player);
            else
                // delayed battle end and statistics are displayed
                SendFinalRound(player);
        }

        if (auto creature = team->GetWildBattlePet())
            sBattlePetSpawnMgr->LeftBattle(creature, team == lostTeam);
    }

    m_state = PetBattleState::Finished;
}

void PetBattle::HandleRound()
{
    for (auto&& team : m_teams)
    {
        auto pendingMove = team->GetPendingMove();
        switch (pendingMove.MoveType)
        {
            case PET_BATTLE_MOVE_TYPE_REQUEST_LEAVE:
            case PET_BATTLE_MOVE_TYPE_LEAVE_PETBATTLE:
                break;
            case PET_BATTLE_MOVE_TYPE_CAST:
            {
                if (team->CanActivePetCast(pendingMove.AbilityId))
                    team->ActivePetPrepareCast(pendingMove.AbilityId);
                break;
            }
            case PET_BATTLE_MOVE_TYPE_CATCH:
            {
                if (team->GetTrapStatus() == PET_BATTLE_TRAP_STATUS_ENABLED)
                    team->ActivePetPrepareCast(team->GetOwner()->GetBattlePetMgr().GetTrapAbility());
                break;
            }
            case PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS:
            {
                if (team->CanSwap(pendingMove.BattlePet))
                    SwapActivePet(pendingMove.BattlePet);
                break;
            }
            case PET_BATTLE_MOVE_TYPE_SWAP_DEAD_PET:
            {
                if (team->CanSwap(pendingMove.BattlePet, true))
                    SwapActivePet(pendingMove.BattlePet, true);
                break;
            }
            default:
                std::abort();
                break;
        }
    }

    uint32 oldStates[PET_BATTLE_MAX_TEAMS][BATTLE_PET_MAX_STATES] = { };

    // reset state conditions
    for (auto&& team : m_teams)
    {
        for (auto&& battlePet : team->BattlePets)
        {
            battlePet->States[BATTLE_PET_STATE_CONDITION_WAS_DAMAGED_THIS_ROUND] = 0;
            battlePet->States[BATTLE_PET_STATE_CONDITION_DID_DAMAGE_THIS_ROUND] = 0;
        }

        // store main stat states before they are modified during the round
        oldStates[team->GetTeamIndex()][BATTLE_PET_STATE_STAT_SPEED] = team->GetActivePet()->GetSpeed();
        oldStates[team->GetTeamIndex()][BATTLE_PET_STATE_STAT_POWER] = team->GetActivePet()->GetPower();
    }

    auto firstTeam = GetTeam(GetFirstAttackingTeam());
    auto secondTeam = firstTeam == Challenger() ? Opponent() : Challenger();

    firstTeam->SetTurn(1);
    secondTeam->SetTurn(2);

    // cast abilities that have round start proc type (example: Deflection(490))
    firstTeam->DoCasts(PET_BATTLE_ABILITY_PROC_ON_ROUND_START);
    secondTeam->DoCasts(PET_BATTLE_ABILITY_PROC_ON_ROUND_START);

    // cast non proc abilities
    firstTeam->DoCasts();
    secondTeam->DoCasts();

    // -------------------------------------------------------------------------------

    // cast abilities that have round end proc type
    firstTeam->DoCasts(PET_BATTLE_ABILITY_PROC_ON_ROUND_END);
    secondTeam->DoCasts(PET_BATTLE_ABILITY_PROC_ON_ROUND_END);

    bool hasAuras = firstTeam->HasAuras() || secondTeam->HasAuras();
    if (hasAuras)
    {
        PetBattleEffect effect{ PET_BATTLE_EFFECT_AURA_PROCESSING_BEGIN };
        effect.SetNoTarget();
        m_effects.push_back(effect);
    }

    // process active battle pet auras
    firstTeam->ProcessAuras();
    secondTeam->ProcessAuras();

    if (hasAuras)
    {
        PetBattleEffect effect{ PET_BATTLE_EFFECT_AURA_PROCESSING_END };
        effect.SetNoTarget();
        m_effects.push_back(effect);
    }

    // send stat updates if they changed this round
    for (auto&& team : m_teams)
    {
        auto activePet = team->GetActivePet();
        if (oldStates[team->GetTeamIndex()][BATTLE_PET_STATE_STAT_SPEED] != activePet->GetSpeed())
        {
            // alert client of speed update
            PetBattleEffect speedEffect{ PET_BATTLE_EFFECT_SET_SPEED };
            speedEffect.SetTurn(team->GetTurn() + 2, 1);
            speedEffect.UpdateStat(activePet->GetGlobalIndex(), activePet->GetSpeed());

            m_effects.push_back(speedEffect);
        }

        if (oldStates[team->GetTeamIndex()][BATTLE_PET_STATE_STAT_POWER] != activePet->GetPower())
        {
            // alert client of power update
            PetBattleEffect powerEffect{ PET_BATTLE_EFFECT_SET_POWER };
            powerEffect.UpdateStat(activePet->GetGlobalIndex(), activePet->GetPower());

            m_effects.push_back(powerEffect);
        }
    }

    // must be incremented before sending round result
    for (auto&& team : m_teams)
        team->ActiveAbility.TurnsPassed++;

    // send round result to players
    if (m_roundResult == PET_BATTLE_ROUND_RESULT_NONE)
        m_roundResult = PET_BATTLE_ROUND_RESULT_NORMAL;

    for (auto&& team : m_teams)
        if (Player* player = team->GetOwner())
            SendRoundResult(player);

    // Remove auras, decrease cooldowns and prepare for next turn
    for (auto&& team : m_teams)
        team->TurnFinished();

    m_round++;
    m_roundResult = PET_BATTLE_ROUND_RESULT_NONE;

    m_effects.clear();

    for (auto&& team : m_teams)
    {
        uint32 aliveBattlePets = 0;
        for (auto&& battlePet : team->BattlePets)
            if (battlePet->IsAlive())
                ++aliveBattlePets;

        if (!aliveBattlePets)
        {
            EndBattle(team);
            break;
        }
    }
}

void PetBattle::Update(uint32 diff)
{
    switch (m_state)
    {
        // Immediatley after battle creation. We must init teams and wait while client chose (or confirm) the front pet
        case PetBattleState::Created:
            for (auto&& team : m_teams)
                if (Player* player = team->GetOwner())
                    SendInitialUpdate(player);
            m_state = PetBattleState::WaitingForFrontPets;
            break;
        // Client answered with CMSG_PET_BATTLE_SET_FRONT_PET
        // When both teams are ready we can start with the first round.
        // First round is special. We must send the front pets. PvE pets may perform emotions.
        case PetBattleState::WaitingForFrontPets:
            if (Challenger()->IsReady() && Opponent()->IsReady())
                StartBattle();
            break;
        case PetBattleState::InProgress:
            // TODO: Turn timer for pvp
            if (Challenger()->IsReady() && Opponent()->IsReady())
                HandleRound();
            break;
        case PetBattleState::Interrupted:
            EndBattle(nullptr);
            break;
        case PetBattleState::Finished:
            break;
        default:
            std::abort();
            break;
    }
}

bool PetBattle::Cast(BattlePet* caster, uint32 abilityId, uint8 turn, int8 procType)
{
    // make sure the ability exists
    auto abilityEntry = sBattlePetAbilityStore.LookupEntry(abilityId);
    if (!abilityEntry)
        return false;

    // prevent states being updated every tick for multiple turn abilities
    if (!turn)
    {
        // update any states the ability modifies
        for (uint32 i = 0; i != sBattlePetAbilityStateStore.GetNumRows(); i++)
        {
            auto abilityStateEntry = sBattlePetAbilityStateStore.LookupEntry(i);
            if (!abilityStateEntry)
                continue;

            if (abilityStateEntry->AbilityId != abilityId)
                continue;

            // update battle pet state
            UpdatePetState(caster, caster, 0, abilityStateEntry->StateId, caster->States[abilityStateEntry->StateId] + abilityStateEntry->Value);
        }
    }

    // handle ability effects
    for (uint32 i = 0; i < sBattlePetAbilityTurnStore.GetNumRows(); i++)
    {
        auto abilityTurnEntry = sBattlePetAbilityTurnStore.LookupEntry(i);
        if (!abilityTurnEntry)
            continue;

        if (abilityTurnEntry->AbilityId != abilityId)
            continue;

        // make sure multiple turn ability has done it's full duration
        if (abilityTurnEntry->Duration > 1 && abilityTurnEntry->Duration != turn)
            continue;

        if (abilityTurnEntry->ProcType != procType)
            continue;

        for (uint32 j = 0; j < sBattlePetAbilityEffectStore.GetNumRows(); j++)
        {
            auto abilityEffectEntry = sBattlePetAbilityEffectStore.LookupEntry(j);
            if (!abilityEffectEntry)
                continue;

            if (abilityEffectEntry->AbilityTurnId != abilityTurnEntry->Id)
                continue;

            // initialise ability effect
            BattlePetAbilityEffect abilityEffect;
            abilityEffect.SetAbilityInfo(abilityId, abilityEffectEntry, abilityEntry->FamilyId);
            abilityEffect.SetCaster(caster);
            abilityEffect.SetParentBattle(this);

            abilityEffect.AddTargets();
            abilityEffect.Execute();
        }
    }

    // update ability cooldown
    for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
    {
        if (caster->Abilities[i] && caster->Abilities[i]->AbilityId == abilityEntry->Id)
        {
            // make sure ability has a cooldown
            if (uint32 cooldown = abilityEntry->Cooldown)
            {
                caster->Abilities[i]->Cooldown = cooldown;
                caster->Abilities[i]->OnCooldown = true;
            }
        }
    }

    return true;
}

void PetBattle::SwapActivePet(BattlePet* battlePet, bool ignoreAlive)
{
    auto team = m_teams[battlePet->GetTeamIndex()];
    if (!team->IsValidBattlePet(battlePet))
        return;

    if (!team->CanSwap(battlePet, ignoreAlive))
        return;

    // update team information
    team->ResetActiveAbility();
    team->SetActivePet(battlePet);

    // alert client of active pet swap
    PetBattleEffect effect{ PET_BATTLE_EFFECT_ACTIVE_PET, battlePet->GetGlobalIndex() };
    effect.SetActivePet(battlePet->GetGlobalIndex());

    m_effects.push_back(effect);
}

void PetBattle::UpdatePetState(BattlePet* source, BattlePet* target, uint32 abilityEffect, uint32 state, int32 value, PetBattleEffectFlags flags)
{
    if (!sBattlePetStateStore.LookupEntry(state))
        return;

    uint8 modifyType = BattlePetStateModifyType[state];
    if (!modifyType)
        return;

    // make sure value is within allowed bounds
    if (modifyType == BATTLE_PET_STATE_MODIFY_TYPE_BOOL)
        value = value >= 1 ? 1 : 0;

    if (target->States[state] == value)
        return;

    target->States[state] = value;

    // notify client of state change
    PetBattleEffect effect{ PET_BATTLE_EFFECT_SET_STATE, source->GetGlobalIndex(), flags, abilityEffect };
    effect.SetTurn(GetTeam(source->GetTeamIndex())->GetTurn(), 1);
    effect.UpdateState(target->GetGlobalIndex(), state, value);

    m_effects.push_back(effect);
}

void PetBattle::AddAura(BattlePet* source, BattlePet* target, uint32 ability, uint32 abilityEffect, int32 duration, PetBattleEffectFlags flags, uint8 maxAllowed)
{
    // count current auras for ability
    uint32 id = 0;
    uint32 auraCount = 0;

    for (auto&& aura : target->Auras)
    {
        if (aura->GetId() > id)
            id = aura->GetId();

        if (!aura->HasExpired() && aura->GetAbility() == ability)
            auraCount++;
    }

    id++;

    if (!duration && duration != -1)
        duration++;

    // notify client of aura update
    PetBattleEffect effect{ PET_BATTLE_EFFECT_AURA_APPLY, source->GetGlobalIndex(), flags, abilityEffect };
    effect.SetTurn(GetTeam(source->GetTeamIndex())->GetTurn(), 1);
    effect.UpdateAura(target->GetGlobalIndex(), id, ability, duration, 0);

    m_effects.push_back(effect);

    if (flags)
        return;

    // expire auras above the allowed count
    if (maxAllowed && auraCount >= maxAllowed)
    {
        uint8 removeCount = 1 + auraCount - maxAllowed;
        for (auto&& aura : target->Auras)
        {
            if (!removeCount)
                continue;

            if (!aura->HasExpired() && aura->GetAbility() == ability)
            {
                aura->Expire();
                removeCount--;
            }
        }
    }

    // create and apply aura
    auto aura = new BattlePetAura(this, id, ability, abilityEffect, source, target, duration);
    target->Auras.emplace_back(aura);

    aura->OnApply();
}

void PetBattle::Kill(BattlePet* killer, BattlePet* victim, uint32 abilityEffect, PetBattleEffectFlags flags)
{
    auto victimTeam = m_teams[victim->GetTeamIndex()];
    if (victimTeam->GetActivePet() == victim)
        victimTeam->ResetActiveAbility();

    // remove any auras the victim has
    for (auto&& aura : victim->Auras)
        aura->Expire();

    UpdatePetState(killer, victim, abilityEffect, BATTLE_PET_STATE_IS_DEAD, 1, flags);

    m_roundResult = PET_BATTLE_ROUND_RESULT_CATCH_OR_KILL;
}

void PetBattle::Catch(BattlePet* source, BattlePet* target, uint32 abilityEffect)
{
    Kill(source, target, abilityEffect, PET_BATTLE_EFFECT_FLAG_NONE);
    m_cagedPet = target;

    m_roundResult = PET_BATTLE_ROUND_RESULT_CATCH_OR_KILL;
}

typedef std::vector<BattlePet*> ActivePetStore;

PetBattleTeamIndex PetBattle::GetFirstAttackingTeam()
{
    auto challangerPet = Challenger()->GetActivePet();
    auto opponentPet   = Opponent()->GetActivePet();

    // return random team if active pet speed is the same
    if (challangerPet->GetSpeed() == opponentPet->GetSpeed())
        return roll_chance_i(50) ? PET_BATTLE_TEAM_CHALLANGER : PET_BATTLE_TEAM_OPPONENT;

    return challangerPet->GetSpeed() < opponentPet->GetSpeed() ? PET_BATTLE_TEAM_OPPONENT : PET_BATTLE_TEAM_CHALLANGER;
}

PetBattleTeam* PetBattle::GetTeam(uint64 guid) const
{
    for (auto team : m_teams)
    {
        // opponent team in PvE should always be a wild battle pet
        if (team->GetTeamIndex() == PET_BATTLE_TEAM_OPPONENT
            && GetType() == PET_BATTLE_TYPE_PVE)
            return nullptr;

        if (team->GetOwner()->GetGUID() == guid)
            return team;
    }

    return nullptr;
}

PetBattleTeam* PetBattle::GetTeam(PetBattleTeamIndex team) const
{
    ASSERT(team < PET_BATTLE_MAX_TEAMS);
    return m_teams[team];
}

void PetBattle::SendFinalizeLocation(PetBattleRequest const& request)
{
    WorldPacket data(SMSG_PET_BATTLE_FINALIZE_LOCATION, 100);
    data << request.BattleOrigin.x << request.BattleOrigin.y;
    for (uint8 i = 0; i < PET_BATTLE_MAX_TEAMS; i++)
        data << request.TeamPositions[i].y << request.TeamPositions[i].x << request.TeamPositions[i].z;
    data << request.BattleOrigin.z;

    data.WriteBit(!request.BattleFacing);
    data.WriteBit(!request.LocationResult);

    if (request.LocationResult)
        data << uint32(request.LocationResult);
    if (request.BattleFacing)
        data << float(request.BattleFacing);

    // TODO: PvP
    request.Challenger->SendDirectMessage(&data);
}

void PetBattle::SendInitialUpdate(Player * player)
{
    bool hasWatingForFrontPetsMaxSecs = true;
    bool hasPvPMaxRoundTime = true;
    bool hasForfietPenalty = true;
    bool hasCreatureId = false;
    bool hasDisplayId = false;

    // only set wild battle pet guid if you are in a pve match
    ObjectGuid wildBattlePetGuid = ObjectGuid::Empty;
    if (GetType() == PET_BATTLE_TYPE_PVE)
        wildBattlePetGuid = m_teams[PET_BATTLE_TEAM_OPPONENT]->GetWildBattlePet()->GetGUID();

    WorldPacket data(SMSG_PET_BATTLE_INITIAL_UPDATE, 1000);

    // enviroment update (bits)
    for (uint8 i = 0; i < 3; i++)
    {
        data.WriteBits(0, 21);                      // AuraCount
        data.WriteBits(0, 21);                      // StateCount
    }

    for (auto&& team : m_teams)
    {
        bool hasRoundTimeSec = false;
        bool hasFrontPet = team->GetActivePet() != nullptr;
        bool trapStatus = true;

        ObjectGuid characterGuid = ObjectGuid::Empty;
        if ((GetType() != PET_BATTLE_TYPE_PVE || team->GetTeamIndex() == PET_BATTLE_TEAM_CHALLANGER))
            characterGuid = team->GetOwner()->GetGUID();

        data.WriteBit(!trapStatus);
        data.WriteBits(team->BattlePets.size(), 2);
        data.WriteBit(characterGuid[2]);

        for (auto&& pet : team->BattlePets)
        {
            ObjectGuid petEntry = pet->GetId();

            data.WriteBits(0, 21);      // aura count
            data.WriteBit(petEntry[3]);
            data.WriteBits(0, 21);

            //

            uint8 abilityCount = 0;
            for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
                if (pet->Abilities[i])
                    abilityCount++;

            data.WriteBit(petEntry[0]);
            data.WriteBit(!pet->GetFlags());
            data.WriteBit(petEntry[5]);
            data.WriteBit(petEntry[1]);

            data.WriteBits(abilityCount, 20);
            data.WriteBit(0);
            data.WriteBits(pet->GetNickname().size(), 7);
            data.WriteBit(petEntry[2]);
            data.WriteBit(petEntry[4]);

            for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
                if (pet->Abilities[i])
                    data.WriteBit(0);

            data.WriteBit(petEntry[6]);
            data.WriteBit(petEntry[7]);
        }

        data.WriteBit(!hasFrontPet);
        data.WriteBit(!hasRoundTimeSec);
        data.WriteBit(characterGuid[5]);
        data.WriteBit(characterGuid[3]);
        data.WriteBit(characterGuid[4]);
        data.WriteBit(characterGuid[6]);
        data.WriteBit(characterGuid[7]);
        data.WriteBit(characterGuid[0]);
        data.WriteBit(characterGuid[1]);
    }

    data.WriteBit(!hasForfietPenalty);

    data.WriteBit(0); // CanAwardXP?
    data.WriteBit(0); // IsPvP

    data.WriteBit(!hasDisplayId);
    data.WriteBit(!hasCreatureId);
    data.WriteBit(!hasWatingForFrontPetsMaxSecs);
    data.WriteBit(!wildBattlePetGuid);

    data.WriteBit(wildBattlePetGuid[2]);
    data.WriteBit(wildBattlePetGuid[4]);
    data.WriteBit(wildBattlePetGuid[5]);
    data.WriteBit(wildBattlePetGuid[1]);
    data.WriteBit(wildBattlePetGuid[3]);
    data.WriteBit(wildBattlePetGuid[6]);
    data.WriteBit(wildBattlePetGuid[7]);
    data.WriteBit(wildBattlePetGuid[0]);

    data.WriteBit(!hasPvPMaxRoundTime);
    data.WriteBit(0); // CurPetBattleState

    for (auto&& team : m_teams)
    {
        ObjectGuid characterGuid = ObjectGuid::Empty;
        if ((GetType() != PET_BATTLE_TYPE_PVE || team->GetTeamIndex() == PET_BATTLE_TEAM_CHALLANGER))
            characterGuid = team->GetOwner()->GetGUID();

        bool hasRoundTimeSec = false;
        bool hasFrontPet = true;
        bool trapStatus = true;

        uint8 battlePetTeamIndex = 0;
        for (auto&& pet : team->BattlePets)
        {
            data << battlePetTeamIndex;
            battlePetTeamIndex++;
            ObjectGuid petEntry = pet->GetId();

            for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
            {
                if (!pet->Abilities[i])
                    continue;

                data << uint32(pet->Abilities[i]->AbilityId);
                data << uint8(i);
                if (true)
                    data << uint8(pet->GetGlobalIndex());
                data << uint16(pet->Abilities[i]->Cooldown);
                data << uint16(pet->Abilities[i]->Lockdown);
            }

            data << uint32(0);
            data.WriteByteSeq(petEntry[4]);
            data << uint16(pet->GetLevel());
            data.WriteByteSeq(petEntry[7]);
            data << uint16(pet->GetQuality());
            data.WriteByteSeq(petEntry[6]);
            data << uint32(pet->GetPower());
            data.WriteByteSeq(petEntry[0]);
            data << uint32(pet->GetMaxHealth());
            data.WriteByteSeq(petEntry[5]);
            data.WriteByteSeq(petEntry[2]);
            data << uint32(pet->GetSpeed());
            data << uint32(pet->GetCurrentHealth());
            data.WriteByteSeq(petEntry[3]);
            data << uint32(0);
            data.WriteByteSeq(petEntry[1]);
            data << uint32(0);
            data << uint16(pet->GetXp());
            if (uint16 flags = pet->GetFlags())
                data << flags;
            data.WriteString(pet->GetNickname());
            data << uint32(pet->GetSpecies());
        }

        if (trapStatus)
            data << uint32(team->GetTrapStatus());

        data << uint32(team->GetOwner() ? team->GetOwner()->GetBattlePetMgr().GetTrapAbility() : 0);
        data.WriteByteSeq(characterGuid[5]);
        data.WriteByteSeq(characterGuid[7]);
        data.WriteByteSeq(characterGuid[6]);
        data.WriteByteSeq(characterGuid[1]);
        data.WriteByteSeq(characterGuid[4]);
        data.WriteByteSeq(characterGuid[0]);

        if (hasFrontPet)
            data << uint8(team->ConvertToLocalIndex(team->GetActivePet()->GetGlobalIndex()));

        data << uint8(6);

        if (hasRoundTimeSec)
            data << uint16(0);

        data.WriteByteSeq(characterGuid[3]);
        data.WriteByteSeq(characterGuid[2]);
    }

    if (hasForfietPenalty)
        data << uint8(10);

    if (true)
        data << uint8(PET_BATTLE_ROUND_RESULT_INITIALISE);

    data.WriteByteSeq(wildBattlePetGuid[5]);
    data.WriteByteSeq(wildBattlePetGuid[4]);
    data.WriteByteSeq(wildBattlePetGuid[3]);
    data.WriteByteSeq(wildBattlePetGuid[2]);
    data.WriteByteSeq(wildBattlePetGuid[7]);
    data.WriteByteSeq(wildBattlePetGuid[0]);
    data.WriteByteSeq(wildBattlePetGuid[1]);
    data.WriteByteSeq(wildBattlePetGuid[6]);

    if (hasDisplayId)
        data << uint32(0);

    if (hasPvPMaxRoundTime)
        data << uint16(30);

    data << uint32(GetRound());

    if (hasWatingForFrontPetsMaxSecs)
        data << uint16(30);

    if (hasCreatureId)
        data << uint32(0);

    player->SendDirectMessage(&data);
}

void PetBattle::SendFirstRound(Player* player)
{
    // This packet makes front pets take their positions and client shows "Battle!" message on the screen.
    WorldPacket data(SMSG_PET_BATTLE_FIRST_ROUND, 100);
    for (auto&& team : m_teams)
    {
        data << uint16(0);
        data << uint8(team->GetTrapStatus());
        data << uint8(team->GetInputStatusFlags());
    }

    data << GetRound();

    data.WriteBit(0); // NextPetBattleState
    data.WriteBits(0, 3);
    data.WriteBits(0, 20);
    data.WriteBits(m_effects.size(), 22);

    for (auto&& effect : m_effects)
    {
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(effect.GetSource() == PET_BATTLE_NULL_PET_INDEX);

        data.WriteBits(effect.Targets.size(), 25);
        for (auto&& target : effect.Targets)
        {
            data.WriteBit(0);
            data.WriteBits(target.Type, 3);
        }

        data.WriteBit(1);
        data.WriteBit(!effect.GetType());
    }

    for (auto&& effect : m_effects)
    {
        if (effect.GetSource() != PET_BATTLE_NULL_PET_INDEX)
            data << uint8(effect.GetSource());

        for (auto&& target : effect.Targets)
        {
            data << uint8(target.Target);
        }

        if (uint8 type = effect.GetType())
            data << uint8(type);
    }

    if (true)
        data << uint8(PET_BATTLE_ROUND_RESULT_NORMAL);

    player->SendDirectMessage(&data);
}

void PetBattle::SendRoundResult(Player* player)
{
    auto team = m_teams[PET_BATTLE_TEAM_CHALLANGER];
    if (GetType() != PET_BATTLE_TYPE_PVE && m_teams[PET_BATTLE_TEAM_OPPONENT]->GetOwner() == player)
        team = m_teams[PET_BATTLE_TEAM_OPPONENT];

    WorldPacket data(SMSG_PET_BATTLE_ROUND_RESULT, 300);

    for (auto&& team : m_teams)
    {
        data << uint8(team->GetTrapStatus());
        data << uint8(team->GetInputStatusFlags());
        data << uint16(0);                          // RoundTimeSecs
    }

    data << uint32(GetRound());
    data.WriteBits(m_effects.size(), 22);

    for (auto&& effect : m_effects)
    {
        bool unk6 = false;

        data.WriteBit(!effect.GetAbilityEffect());
        data.WriteBit(!effect.GetType());
        data.WriteBits(effect.Targets.size(), 25);
        data.WriteBit(!unk6);

        for (auto&& target : effect.Targets)
        {
            auto& targetData = target.Data;
            data.WriteBits(target.Type, 3);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE)
                data.WriteBit(!targetData.StatValue);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_AURA)
            {
                data.WriteBit(!targetData.Aura.AuraInstanceId);
                data.WriteBit(!targetData.Aura.RoundsRemaing);
                data.WriteBit(!targetData.Aura.AbilityId);
                data.WriteBit(!targetData.Aura.CurrentRound);
            }
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_NPC_EMOTE)
                data.WriteBit(1);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_ABILITY_CHANGE)
            {
                data.WriteBit(1);
                data.WriteBit(1);
                data.WriteBit(1);
            }
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_TRIGGER_ABILITY)
                data.WriteBit(1);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_STATE)
            {
                data.WriteBit(!targetData.State.Value);
                data.WriteBit(!targetData.State.StateId);
            }
            data.WriteBit(!(target.Target != PET_BATTLE_NULL_PET_INDEX));
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_PET)
                data.WriteBit(!targetData.Health);
        }

        data.WriteBit(effect.GetSource() == PET_BATTLE_NULL_PET_INDEX);
        data.WriteBit(!effect.GetTurn());
        data.WriteBit(!effect.GetStackDepth());
        data.WriteBit(!effect.GetFlags());
    }

    data.WriteBit(!GetRoundResult());
    data.WriteBits(0, 3);
    size_t cooldownPos = data.bitwpos();
    data.WriteBits(0, 20);

    struct CooldownInfo
    {
        uint8 Index;
        BattlePetAbility* Ability;
        int8 BattlePetGlobalIndex;
    };

    std::vector<CooldownInfo> cooldowns;
    for (auto&& pet : team->BattlePets)
    {
        for (uint8 i = 0; i < BATTLE_PET_MAX_ABILITIES; i++)
        {
            BattlePetAbility* ability = pet->Abilities[i].get();
            if (!ability)
                continue;

            if (!ability->OnCooldown)
                continue;

            if (!ability->Cooldown)
                ability->OnCooldown = false;

            data.WriteBit(0);
            cooldowns.push_back({ i, ability, pet->GetGlobalIndex() });
        }
    }

    data.FlushBits();
    data.PutBits(cooldownPos, cooldowns.size(), 20);

    for (auto&& it : cooldowns)
    {
        data << uint16(it.Ability->Cooldown);
        data << uint16(it.Ability->Lockdown);
        data << uint32(it.Ability->AbilityId);
        data << uint8(it.Index);
        if (true)
            data << it.BattlePetGlobalIndex;
    }

    for (auto&& effect : m_effects)
    {
        bool unk6 = false;

        if (uint16 flags = effect.GetFlags())
            data << uint16(flags);

        if (unk6)
            data << uint16(0);

        if (uint32 abilityEffect = effect.GetAbilityEffect())
            data << uint32(abilityEffect);

        for (auto&& target : effect.Targets)
        {
            auto& targetData = target.Data;
            if (target.Target != PET_BATTLE_NULL_PET_INDEX)
                data << uint8(target.Target);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_AURA)
            {
                if (targetData.Aura.AuraInstanceId)
                    data << uint32(targetData.Aura.AuraInstanceId);
                if (targetData.Aura.RoundsRemaing)
                    data << uint32(targetData.Aura.RoundsRemaing);
                if (targetData.Aura.CurrentRound)
                    data << uint32(targetData.Aura.CurrentRound);
                if (targetData.Aura.AbilityId)
                    data << uint32(targetData.Aura.AbilityId);
            }
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE && targetData.StatValue)
                data << uint32(targetData.StatValue);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_STATE)
            {
                if (targetData.State.StateId)
                    data << uint32(targetData.State.StateId);
                if (targetData.State.Value)
                    data << uint32(targetData.State.Value);
            }
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_ABILITY_CHANGE)
            {
                data << uint32(0);
                data << uint32(0);
                data << uint32(0);
            }
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_PET && targetData.Health)
                data << uint32(targetData.Health);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_TRIGGER_ABILITY)
                data << uint32(0);
            if (target.Type == PET_BATTLE_EFFECT_TARGET_EX_NPC_EMOTE)
                data << uint32(0);
        }

        if (uint8 type = effect.GetType())
            data << uint8(type);
        if (uint16 turn = effect.GetTurn())
            data << uint16(turn);
        if (effect.GetSource() != PET_BATTLE_NULL_PET_INDEX)
            data << uint8(effect.GetSource());
        if (uint8 stackDepth = effect.GetStackDepth())
            data << uint8(stackDepth);
    }

    if (uint8 roundResult = GetRoundResult())
        data << uint8(roundResult);

    player->SendDirectMessage(&data);
}

void PetBattle::SendFinalRound(Player* player)
{
    WorldPacket data(SMSG_PET_BATTLE_FINAL_ROUND, 100);
    data.WriteBit(0);

    for (auto team : m_teams)
        data.WriteBit(GetWinningTeam() == team);

    data.WriteBits(m_teams[0]->BattlePets.size() + m_teams[1]->BattlePets.size(), 20);

    for (auto&& team : m_teams)
    {
        for (auto&& pet : team->BattlePets)
        {
            bool caged = GetCagedPet() == pet;

            data.WriteBit(0); // !has level
            data.WriteBit(caged); // Caged
            data.WriteBit(team->SeenAction.find(pet) != team->SeenAction.end());      // SeenAction
            data.WriteBit(0); // !has xp
            data.WriteBit(caged && GetWinningTeam() != team);                    // Captured
            data.WriteBit(0); // !has old level
            data.WriteBit(team->SeenAction.find(pet) != team->SeenAction.end());      // AwardedXP
        }
    }

    data.WriteBit(1);                            // Abandoned (hackfix! client gets stuck in battle if this is false)

    for (auto&& team : m_teams)
    {
        for (auto&& pet : team->BattlePets)
        {
            data << uint32(pet->GetCurrentHealth());
            if (true)
                data << uint16(pet->GetOldLevel());
            if (true)
                data << uint16(pet->GetXp());
            data << uint8(pet->GetGlobalIndex());
            if (true)
                data << uint16(pet->GetLevel());
            data << uint32(pet->GetMaxHealth());
        }
    }

    for (auto team : m_teams)
        data << uint32(0);                          // NpcCreatureID

    player->SendDirectMessage(&data);
}

void PetBattle::SendFinished(Player* player)
{
    WorldPacket data(SMSG_PET_BATTLE_FINISHED, 0);
    player->SendDirectMessage(&data);
}

// -------------------------------------------------------------------------------

void PetBattleEffect::SetNoTarget()
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_NONE;
    target.Target = PET_BATTLE_NULL_PET_INDEX;

    Targets.push_back(target);
}

void PetBattleEffect::SetActivePet(int8 targetPet)
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_NONE;
    target.Target = targetPet;

    Targets.push_back(target);
}

void PetBattleEffect::UpdateHealth(int8 targetPet, uint32 health)
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_PET;
    target.Target = targetPet;
    target.Data.Health = health;

    Targets.push_back(target);
}

void PetBattleEffect::UpdateState(int8 targetPet, uint32 state, uint32 value)
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_STATE;
    target.Target = targetPet;
    target.Data.State.StateId = state;
    target.Data.State.Value = value;

    Targets.push_back(target);
}

void PetBattleEffect::UpdateAura(int8 targetPet, uint32 auraInstance, uint32 ability, int32 duration, uint32 turn)
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_AURA;
    target.Target = targetPet;
    target.Data.Aura.AuraInstanceId = auraInstance;
    target.Data.Aura.AbilityId = ability;
    target.Data.Aura.CurrentRound = turn;
    target.Data.Aura.RoundsRemaing = duration;

    Targets.push_back(target);
}

void PetBattleEffect::UpdateStat(int8 targetPet, uint32 value)
{
    PetBattleEffectTarget target;
    target.Type = PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE;
    target.Target = targetPet;
    target.Data.StatValue = value;

    Targets.push_back(target);
}

// -------------------------------------------------------------------------------

void PetBattleSystem::Update(uint32 diff)
{
    m_updatePetBattlesTimer += diff;

    // update all pet battles
    if (m_updatePetBattlesTimer >= PET_BATTLE_SYSTEM_UPDATE_BATTLE_TIMER)
    {
        for (auto&& petBattleSet : m_petBattles)
        {
            auto petBattle = petBattleSet.second;
            petBattle->Update(diff + m_updatePetBattlesTimer);
            if (petBattle->GetState() == PetBattleState::Finished)
                m_petBattlesToRemove.insert(petBattle);
        }

        m_updatePetBattlesTimer = 0;

        for (auto& petBattleToRemove : m_petBattlesToRemove)
            Remove(petBattleToRemove);

        m_petBattlesToRemove.clear();
    }
}

void PetBattleSystem::Create(PetBattleRequest const& request)
{
    uint32 battleId = m_globalPetBattleId++;
    auto petBattle = new PetBattle{ battleId, request };

    m_playerPetBattles[request.Challenger->GetGUID()] = battleId;
    if (request.Type != PET_BATTLE_TYPE_PVE)
        m_playerPetBattles[request.Opponent->GetGUID()] = battleId;

    m_petBattles[battleId] = petBattle;
}

void PetBattleSystem::Remove(PetBattle* petBattle)
{
    if (!petBattle)
        return;

    if (uint64 guid = petBattle->GetTeam(PET_BATTLE_TEAM_CHALLANGER)->GetOwnerGuid())
        m_playerPetBattles.erase(guid);
    if (uint64 guid = petBattle->GetTeam(PET_BATTLE_TEAM_OPPONENT)->GetOwnerGuid())
        m_playerPetBattles.erase(guid);

    m_petBattles.erase(petBattle->GetId());
    delete petBattle;
}

void PetBattleSystem::ForfietBattle(uint64 guid)
{
    // make sure player is currently in a pet battle
    auto petBattle = GetPlayerPetBattle(guid);
    if (!petBattle)
        return;

    auto team = petBattle->GetTeam(guid);
    ASSERT(team);

    ForfietBattle(petBattle, team);
}

void PetBattleSystem::ForfietBattle(PetBattle* petBattle, PetBattleTeam* team)
{
    if (!petBattle || !team)
        return;

    petBattle->EndBattle(team, true);
}

PetBattle* PetBattleSystem::GetPlayerPetBattle(uint64 guid)
{
    // find players pet battle id
    auto playerPetBattle = m_playerPetBattles.find(guid);
    if (playerPetBattle == m_playerPetBattles.end())
        return nullptr;

    // find players pet battle instance
    auto petBattle = m_petBattles.find(playerPetBattle->second);
    if (petBattle == m_petBattles.end())
        return nullptr;

    return petBattle->second;
}