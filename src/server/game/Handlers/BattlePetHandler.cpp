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

#include "PetBattle.h"
#include "BattlePetMgr.h"
#include "Common.h"
#include "DB2Enums.h"
#include "DB2Stores.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "TemporarySummon.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "BattlePetSpawnMgr.h"

void WorldSession::HandlePetBattleStartPvpMatchmaking(WorldPackets::BattlePet::PetBattleStartPvpMatchmaking& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_PET_BATTLE_START_PVP_MATCHMAKING");

    WorldPacket data(SMSG_PET_BATTLE_QUEUE_STATUS, 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 8 + 1);

    //CliRideTicket.RequesterGuid
    ObjectGuid guid = GetPlayer()->GetGUID();

    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(0); // 20 hasAverageWaitTime
    data.WriteBit(guid[4]);
    data.WriteBits(0, 22);
    data.WriteBit(guid[0]);
    data.WriteBit(0); // 48 hasClientWaitTime
    data.WriteBit(guid[3]);
    data.WriteBit(guid[5]);

    data.FlushBits();
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data << uint32(0); // 72 CliRideTicket.Time
    data.WriteByteSeq(guid[3]);
    data << uint32(1); // 24 Status // ERR_PETBATTLE_QUEUE_ status, 1 = ERR_PETBATTLE_QUEUE_QUEUED
    data.WriteByteSeq(guid[6]);
    //if (hasClientWaitTime)
    //data << uint32(0) // 44
    data.WriteByteSeq(guid[1]);
    data << uint32(0); // 68 CliRideTicket.Type
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data << uint32(0); // 64 CliRideTicket.Id
    data.WriteByteSeq(guid[0]);

    for (uint8 i = 0; i < 3; i++)
        data << uint32(1); // SlotResult

    //if (hasAverageWaitTime)
    // data << uint32(0); // 16

    SendPacket(&data);

}

void WorldSession::HandlePetBattleStopPvpMatchmaking(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_PET_BATTLE_STOP_PVP_MATCHMAKING");
    //CliRideTicket
}

void WorldSession::HandleBattlePetDelete(WorldPackets::BattlePet::BattlePetDeletePet& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_DELETE");

    ObjectGuid petEntry = packet.PetEntry;

    BattlePetMgr& battlePetMgr = GetPlayer()->GetBattlePetMgr();

    BattlePet* battlePet = battlePetMgr.GetBattlePet(petEntry);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_DELETE - Player {} tryed to release Battle Pet {} which it doesn't own!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    // This is shit. I can't reverse it in the client code for now, so just trust to the client
    /*
    if (!HasBattlePetSpeciesFlag(battlePet->GetSpecies(), BATTLE_PET_FLAG_RELEASABLE))
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_DELETE - Player {} tryed to release Battle Pet {} which isn't releasable!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }
    */

    battlePetMgr.Delete(battlePet);
}

#define BATTLE_PET_MAX_DECLINED_NAMES 5

void WorldSession::HandleBattlePetModifyName(WorldPackets::BattlePet::BattlePetModifyName& packet)
{
    // TODO: finish this...
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_MODIFY_NAME");

    ObjectGuid petEntry = packet.PetEntry;
    std::string nickname = packet.Nickname;

    BattlePetMgr& battlePetMgr = GetPlayer()->GetBattlePetMgr();

    BattlePet* battlePet = battlePetMgr.GetBattlePet(petEntry);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_MODIFY_NAME - Player {} tryed to set the name for Battle Pet {} which it doesn't own!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    if (nickname.size() > BATTLE_PET_MAX_NAME_LENGTH)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_MODIFY_NAME - Player {} tryed to set the name for Battle Pet {} with an invalid length!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    // TODO: check for invalid characters, ect...

    battlePet->SetNickname(nickname);
    battlePet->SetTimestamp((uint32)time(NULL));

    if (battlePetMgr.GetCurrentSummon())
        battlePetMgr.GetCurrentSummon()->SetUInt32Value(UNIT_FIELD_BATTLE_PET_COMPANION_NAME_TIMESTAMP, battlePet->GetTimestamp());
}

void WorldSession::HandleBattlePetQueryName(WorldPackets::BattlePet::QueryBattlePetName& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_QUERY_BATTLE_PET_NAME");

    ObjectGuid petEntry = packet.PetEntry;
    ObjectGuid petguid = packet.PetGuid;

    Unit* tempUnit = ObjectAccessor::GetUnit(*GetPlayer(), petguid);
    if (!tempUnit)
    {
        TC_LOG_DEBUG("network", "CMSG_QUERY_BATTLE_PET_NAME - Player {} queried the name of Battle Pet {} which doesnt't exist in world!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    Unit* ownerUnit = tempUnit->ToTempSummon()->GetSummoner();
    if (!ownerUnit)
        return;

    BattlePetMgr& battlePetMgr = ownerUnit->ToPlayer()->GetBattlePetMgr();

    BattlePet* battlePet = battlePetMgr.GetBattlePet(battlePetMgr.GetCurrentSummonId());
    if (!battlePet)
        return;

    BattlePetSpeciesEntry const* speciesEntry = sBattlePetSpeciesStore.LookupEntry(battlePet->GetSpecies());

    WorldPacket data(SMSG_QUERY_BATTLE_PET_NAME_RESPONSE, 8 + 4 + 4 + 5 + battlePet->GetNickname().size());
    data << uint64(petEntry);
    data << uint32(battlePet->GetTimestamp());
    data << uint32(speciesEntry->NpcId);
    data.WriteBit(1);               // has names
    data.WriteBits(battlePet->GetNickname().size(), 8);

    // TODO: finish declined names
    for (uint8 i = 0; i < BATTLE_PET_MAX_DECLINED_NAMES; i++)
        data.WriteBits(0, 7);

    data.WriteBit(0);               // allowed?
    data.FlushBits();

    data.WriteString(battlePet->GetNickname());

    SendPacket(&data);
}

void WorldSession::HandleBattlePetRequestJournal(WorldPackets::BattlePet::BattlePetRequestJournal& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_REQUEST_JOURNAL");

    GetPlayer()->GetBattlePetMgr().SendBattlePetJournal();
}

void WorldSession::HandleBattlePetSetBattleSlot(WorldPackets::BattlePet::BattlePetSetBattleSlot& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_SET_BATTLE_SLOT");

    uint8 slot = packet.Slot;
    ObjectGuid petEntry = packet.PetEntry;

    BattlePetMgr& battlePetMgr = GetPlayer()->GetBattlePetMgr();

    BattlePet* battlePet = battlePetMgr.GetBattlePet(petEntry);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_SET_BATTLE_SLOT - Player {} tryed to add Battle Pet {} to loadout which it doesn't own!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    if (!battlePetMgr.HasLoadoutSlot(slot))
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_SET_BATTLE_SLOT - Player {} tryed to add Battle Pet {} into slot {} which is locked!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry, slot);
        return;
    }

    // this check is also done clientside
    if (HasBattlePetSpeciesFlag(battlePet->GetSpecies(), BATTLE_PET_FLAG_COMPANION))
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_SET_BATTLE_SLOT - Player {} tryed to add a compainion Battle Pet {} into slot {}!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry, slot);
        return;
    }

    // sever handles slot swapping, find source slot and replace it with the destination slot
    uint8 srcSlot = battlePetMgr.GetLoadoutSlotForBattlePet(petEntry);
    if (srcSlot != BATTLE_PET_LOADOUT_SLOT_NONE)
        battlePetMgr.SetLoadoutSlot(srcSlot, battlePetMgr.GetLoadoutSlot(slot), true);

    battlePetMgr.SetLoadoutSlot(slot, petEntry, true);
}

enum BattlePetFlagModes
{
    BATTLE_PET_FLAG_MODE_UNSET       = 0,
    BATTLE_PET_FLAG_MODE_SET         = 3
};

void WorldSession::HandleBattlePetSetFlags(WorldPackets::BattlePet::BattlePetSetFlags& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_SET_FLAGS");

    ObjectGuid petEntry = packet.PetEntry;
    uint32 flag = packet.Flag;
    uint8 mode = packet.Mode;

    BattlePet* battlePet = GetPlayer()->GetBattlePetMgr().GetBattlePet(petEntry);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_SET_FLAGS - Player {} tryed to set the flags for Battle Pet {} which it doesn't own!",
            GetPlayer()->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    // list of flags the client can currently change
    if (flag != BATTLE_PET_JOURNAL_FLAG_FAVORITES
        && flag != BATTLE_PET_JOURNAL_FLAG_ABILITY_1
        && flag != BATTLE_PET_JOURNAL_FLAG_ABILITY_2
        && flag != BATTLE_PET_JOURNAL_FLAG_ABILITY_3)
    {
        TC_LOG_DEBUG("network", "CMSG_BATTLE_PET_SET_FLAGS - Player {} tryed to set an invalid Battle Pet flag {}!",
            GetPlayer()->GetGUID().GetCounter(), flag);
        return;
    }

    // TODO: check if Battle Pet is correct level for ability

    switch (mode)
    {
        case BATTLE_PET_FLAG_MODE_SET:
            battlePet->SetFlag(flag);
            break;
        case BATTLE_PET_FLAG_MODE_UNSET:
            battlePet->UnSetFlag(flag);
            break;
    }
}

void WorldSession::HandleBattlePetSummonCompanion(WorldPackets::BattlePet::BattlePetSummon& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BATTLE_PET_SUMMON_COMPANION");

    ObjectGuid petEntry = packet.PetEntry;

    Player* player = GetPlayer();
    BattlePetMgr& battlePetMgr = player->GetBattlePetMgr();

    BattlePet* battlePet = battlePetMgr.GetBattlePet(petEntry);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "CMSG_SUMMON_BATTLE_PET_COMPANION - Player {} tryed to summon battle pet companion {} which it doesn't own!",
            player->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    if (!battlePet->GetCurrentHealth())
    {
        TC_LOG_DEBUG("network", "CMSG_SUMMON_BATTLE_PET_COMPANION - Player {} tryed to summon battle pet companion {} which is dead!",
            player->GetGUID().GetCounter(), (uint64)petEntry);
        return;
    }

    if (battlePetMgr.GetCurrentSummonId() == petEntry)
        battlePetMgr.UnSummonCurrentBattlePet(false);
    else
    {
        if (uint32 summonSpell = db2::GetBattlePetSummonSpell(battlePet->GetSpecies()))
        {
            battlePetMgr.UnSummonCurrentBattlePet(false);
            battlePetMgr.SetCurrentSummonId(petEntry);
            player->CastSpell(player, summonSpell, true);
        }
    }
}

void WorldSession::HandleBattlePetCage(WorldPackets::BattlePet::CageBattlePet& packet)
{
    ObjectGuid petEntry = packet.PetEntry;

    GetPlayer()->GetBattlePetMgr().CageBattlePet(petEntry);
}

void WorldSession::HandleBattlePetLearn(WorldPackets::BattlePet::BattlePetLearn& packet)
{
    ObjectGuid itemGuid = packet.ItemGuid;

    Item* item = GetPlayer()->GetItemByGuid(itemGuid);
    if (!item)
    {
        TC_LOG_ERROR("network", "WorldSession::HandleBattlePetLearn - Player {} tryed to learn pet from non existing item: " UI64FMTD, GetPlayer()->GetGUID().GetCounter(), (uint64)itemGuid);
        return;
    }

    uint32 learnSpell = item->GetTemplate()->Spells[1].SpellId;
    if (!learnSpell)
    {
        TC_LOG_ERROR("network", "WorldSession::HandleBattlePetLearn - Player {} tryed to learn pet from wrong item: {}", GetPlayer()->GetGUID().GetCounter(), item->GetEntry());
        return;
    }

    uint32 species = db2::GetBattlePetSpeciesFromSpell(learnSpell);
    if (!species)
        return;

    if (!GetPlayer()->GetBattlePetMgr().CanStoreBattlePet(species))
        return;

    GetPlayer()->GetBattlePetMgr().Create(species);
    GetPlayer()->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
}

void WorldSession::HandlePetBattleInput(WorldPackets::BattlePet::PetBattleInput& packet)
{
    bool const hasAbilityId = packet.HasAbilityId;
    bool const hasNewFrontPet = packet.HasNewFrontPet;
    uint32 const abilityId = packet.AbilityId;
    uint8 const newFrontPet = packet.NewFrontPet;
    uint8 const moveType = packet.MoveType;

    // make sure player is in a pet battle
    auto petBattle = sPetBattleSystem->GetPlayerPetBattle(_player->GetGUID());
    if (!petBattle)
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried to make a pet battle move while not in battle!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());
        return;
    }

    // make sure the pet battle is in progress
    if (petBattle->GetState() != PetBattleState::InProgress)
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried to make a move in a pet battle that isn't in progress!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());
        return;
    }

    if (hasAbilityId)
    {
        // make sure ability is valid
        if (!sBattlePetAbilityStore.LookupEntry(abilityId))
        {
            TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried use an invalid battle pet ability {}!",
                GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), abilityId);
            return;
        }
    }

    if (hasNewFrontPet)
    {
        // make sure active pet is valid
        if (newFrontPet >= PET_BATTLE_MAX_TEAM_PETS)
        {
            TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried to switch to invalid front pet {}!",
                GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), newFrontPet);
            return;
        }
    }

    auto team = petBattle->GetTeam(_player->GetGUID());
    ASSERT(team);

    // make sure players team hasn't already made a move this round
    if (team->IsReady())
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried use an invalid battle pet ability {}!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), abilityId);
        return;
    }

    // make sure new active pet is valid
    BattlePet* battlePet = nullptr;
    if (hasNewFrontPet)
    {
        battlePet = team->GetPet(team->ConvertToLocalIndex(newFrontPet));
        if (!battlePet)
        {
            TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried to swap to invalid team battle pet {}!",
                GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), newFrontPet);
            return;
        }
    }

    // make sure move type is valid
    if (moveType >= PET_BATTLE_MOVE_MAX)
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_INPUT - Player {}({}) tried to do invalid move {}!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), moveType);
        return;
    }

    if (moveType == PET_BATTLE_MOVE_TYPE_REQUEST_LEAVE)
    {
        sPetBattleSystem->ForfietBattle(petBattle, team);
        return;
    }

    team->SetPendingMove(moveType, abilityId, battlePet);
}

#include "Chat.h"

void WorldSession::HandlePetBattleRequestWild(WorldPackets::BattlePet::PetBattleRequestWild& packet)
{
    if (!sWorld->getBoolConfig(CONFIG_PET_BATTLES_ENABLED))
    {
        ChatHandler(this).SendSysMessage("Pet battles are disabled");
        return;
    }

    PetBattleRequest petBattleRequest;

    for (uint8 i = 0; i < PET_BATTLE_MAX_TEAMS; i++)
    {
        petBattleRequest.TeamPositions[i].x = packet.TeamPositions[i][0];
        petBattleRequest.TeamPositions[i].y = packet.TeamPositions[i][1];
        petBattleRequest.TeamPositions[i].z = packet.TeamPositions[i][2];
    }

    petBattleRequest.BattleOrigin.x = packet.BattleOrigin[0];
    petBattleRequest.BattleOrigin.y = packet.BattleOrigin[1];
    petBattleRequest.BattleOrigin.z = packet.BattleOrigin[2];

    petBattleRequest.OpponentGuid = packet.OpponentGuid;
    petBattleRequest.BattleFacing = packet.BattleFacing;
    petBattleRequest.LocationResult = packet.LocationResult;

    // check if player is dead
    if (!GetPlayer()->IsAlive())
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_REQUEST_WILD - Player {}({}) tried to initiate a wild pet battle while dead!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());

        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_DEAD);
        return;
    }

    // check if player is in combat
    if (GetPlayer()->IsInCombat())
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_REQUEST_WILD - Player {}({}) tried to initiate a wild pet battle while in combat!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());

        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_ALREADY_IN_COMBAT);
        return;
    }

    // check if player isn't already in a battle
    if (sPetBattleSystem->GetPlayerPetBattle(GetPlayer()->GetGUID()))
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_REQUEST_WILD - Player {}({}) tried to initiate a new wild pet battle while still in an old pet battle!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());

        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_ALREADY_IN_PETBATTLE);
        return;
    }

    Creature* wildBattlePet = ObjectAccessor::GetCreatureOrPetOrVehicle(*GetPlayer(), petBattleRequest.OpponentGuid);
    if (!wildBattlePet)
    {
        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_INVALID_TARGET);
        return;
    }

    // player must be able to interact with the creature
    if (!wildBattlePet->IsWithinDistInMap(GetPlayer(), PETBATTLE_INTERACTION_DIST))
    {
        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_TOO_FAR);
        return;
    }

    if (!GetPlayer()->GetNPCIfCanInteractWith(petBattleRequest.OpponentGuid, UNIT_NPC_FLAG_WILDPET_CAPTURABLE))
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_REQUEST_WILD - Player {}({}) tried to initiate a wild pet battle but can't interact with opponent {}!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), wildBattlePet->GetGUID().GetCounter());

        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_NOT_VALID_TARGET);
        return;
    }

    // check if creature is a wild battle pet
    if (!sBattlePetSpawnMgr->GetWildBattlePet(wildBattlePet))
    {
        TC_LOG_DEBUG("network", "CMSG_PET_BATTLE_REQUEST_WILD - Player {}({}) tried to initiate a wild pet battle but creature {} isn't a wild battle pet!",
            GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str(), wildBattlePet->GetGUID().GetCounter());

        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_INVALID_TARGET);
        return;
    }

    auto &battlePetMgr = GetPlayer()->GetBattlePetMgr();

    // player needs to have at least one battle pet loadout slot populated
    if (!battlePetMgr.GetLoadoutSlot(0))
    {
        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_NEED_AT_LEAST_1_PET_IN_SLOT);
        return;
    }

    // make sure all of the players loadout pets aren't dead
    bool allDead = true;
    for (uint8 i = 0; i < BATTLE_PET_MAX_LOADOUT_SLOTS; i++)
        if (auto battlePet = battlePetMgr.GetBattlePet(battlePetMgr.GetLoadoutSlot(i)))
            if (battlePet->IsAlive())
            {
                allDead = false;
                break;
            }

    if (allDead)
    {
        SendPetBattleRequestFailed(PET_BATTLE_REQUEST_PET_ALL_DEAD);
        return;
    }

    // make sure there is enough room for the pet battle
    for (uint8 i = 0; i < PET_BATTLE_MAX_TEAMS; i++)
    {
        G3D::Vector3& pos = petBattleRequest.TeamPositions[i];
        G3D::Vector3& origin = petBattleRequest.BattleOrigin;
        if (_player->GetMap()->getObjectHitPos(_player->GetPhaseMask(), origin.x, origin.y, origin.z, pos.x, pos.y, pos.z, pos.x, pos.y, pos.z, 0.0f))
        {
            SendPetBattleRequestFailed(PET_BATTLE_REQUEST_GROUND_NOT_ENOUGHT_SMOOTH);
            return;
        }
    }

    // setup player
    _player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC);
    _player->SetTarget(wildBattlePet->GetGUID());
    _player->SetFacingTo(_player->GetAngle(petBattleRequest.TeamPositions[PET_BATTLE_TEAM_OPPONENT].x, petBattleRequest.TeamPositions[PET_BATTLE_TEAM_OPPONENT].y));
    _player->SetRooted(true);

    // setup wild battle pet
    wildBattlePet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
    wildBattlePet->SetTarget(GetPlayer()->GetGUID());
    wildBattlePet->SetControlled(true, UNIT_STATE_ROOT);

    sBattlePetSpawnMgr->EnteredBattle(wildBattlePet);

    petBattleRequest.Type = PET_BATTLE_TYPE_PVE;
    petBattleRequest.Challenger = GetPlayer();
    petBattleRequest.Opponent = wildBattlePet;

    // create and start pet battle
    sPetBattleSystem->Create(petBattleRequest);
}

void WorldSession::HandlePetBattleSetFrontPet(WorldPackets::BattlePet::PetBattleSetFrontPet& packet)
{
    // This packets is sended in to cases:
    // 1) Right after initial setup to set active pet even if this is pve and front pet can't be chosen
    // 2) Whn active pet is dead, to make replacement

    uint8 const petNum = packet.PetNum;

    PetBattle* battle = sPetBattleSystem->GetPlayerPetBattle(_player->GetGUID());
    if (!battle)
    {
        TC_LOG_ERROR("network", "CMSG_PET_BATTLE_SET_FRONT_PET Player {} doesn't in a pet battle", GetPlayer()->GetGUID().GetCounter());
        return;
    }

    PetBattleTeam* team = battle->GetTeam(GetPlayer()->GetGUID());
    ASSERT(team);

    BattlePet* pet = team->GetPet(petNum);
    if (!pet)
    {
        TC_LOG_ERROR("network", "CMSG_PET_BATTLE_SET_FRONT_PET Player {} sent invalid pet number {}", GetPlayer()->GetGUID().GetCounter(), petNum);
        return;
    }

    if (battle->GetState() == PetBattleState::WaitingForFrontPets)
    {
        if (team->GetActivePet() == pet)
        {
            // To produce data for the first round
            team->SetActivePet(pet);
            return;
        }
    }

    if (team->GetActivePet()->IsAlive())
    {
        TC_LOG_ERROR("network", "CMSG_PET_BATTLE_SET_FRONT_PET Player {} want to swap dead pet but it is alive", GetPlayer()->GetGUID().GetCounter());
        return;
    }

    if (!pet->IsAlive())
    {
        TC_LOG_ERROR("network", "CMSG_PET_BATTLE_SET_FRONT_PET Player {} want to swap dead pet but a replacement is dead too", GetPlayer()->GetGUID().GetCounter());
        return;
    }

    team->SetPendingMove(PET_BATTLE_MOVE_TYPE_SWAP_DEAD_PET, 0, pet);
}

void WorldSession::SendPetBattleRequestFailed(uint8 reason)
{
    WorldPacket data(SMSG_PET_BATTLE_REQUEST_FAILED, 2);
    data.WriteBit(0);
    data << uint8(reason);
    SendPacket(&data);
}
