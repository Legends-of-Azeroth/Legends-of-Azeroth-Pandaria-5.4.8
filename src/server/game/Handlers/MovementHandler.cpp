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

#include "Common.h"
#include "AnticheatMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Transport.h"
#include "Battleground.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"
#include "ObjectMgr.h"
#include "MovementStructures.h"
#include "BattlePetMgr.h"
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#define MOVEMENT_PACKET_TIME_DELAY 0

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAck();
}

void WorldSession::HandleMoveWorldportAck()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    // get the teleport destination
    WorldLocation const& loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        LogoutPlayer(false);
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.GetMapId());
    InstanceTemplate const* mInstance = sObjectMgr->GetInstanceTemplate(loc.GetMapId());

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    Map* oldMap = GetPlayer()->GetMap();

    Map* newMap = sMapMgr->CreateMap(loc.GetMapId(), GetPlayer());

    if (GetPlayer()->IsInWorld())
    {
        TC_LOG_ERROR("network", "Player %s (GUID: %u) is still in world when teleported from map %s (%u) to new map %s (%u)", GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter(), oldMap->GetMapName(), oldMap->GetId(), newMap ? newMap->GetMapName() : "Unknown", loc.GetMapId());
        oldMap->RemovePlayerFromMap(GetPlayer(), false);
    }

    // relocate the player to the teleport destination
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || !newMap->CanEnter(GetPlayer()))
    {
        GetPlayer()->SetrSemaphoreTeleportForcedFar(false);
        TC_LOG_ERROR("network", "Map %d (%s) could not be created for player %d (%s), porting player to homebind", loc.GetMapId(), newMap ? newMap->GetMapName() : "Unknown", GetPlayer()->GetGUID().GetCounter(), GetPlayer()->GetName().c_str());
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }
    else
        GetPlayer()->Relocate(&loc);

    GetPlayer()->SetrSemaphoreTeleportForcedFar(false);

    GetPlayer()->ResetMap();
    GetPlayer()->SetMap(newMap);

    bool invalidZ = false;
    if (GetPlayer()->GetPositionZ() > 500000.0f || GetPlayer()->GetPositionZ() <= -200000.0f)
    {
        TC_LOG_ERROR("shitlog", "WorldSession::HandleMoveWorldportAckOpcode z coordinate fucked: %s, map %u, z %f\n", GetPlayerInfo().c_str(), newMap->GetId(), GetPlayer()->GetPositionZ());
        invalidZ = true;
    }

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    if (invalidZ || !GetPlayer()->GetMap()->AddPlayerToMap(GetPlayer()))
    {
        TC_LOG_ERROR("network", "WORLD: failed to teleport player %s (%d) to map %d (%s) because of unknown reason!",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter(), loc.GetMapId(), newMap ? newMap->GetMapName() : "Unknown");
        GetPlayer()->ResetMap();
        GetPlayer()->SetMap(oldMap);
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (_player->InBattleground())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattlegroundOrArena())
        {
            // We're not in BG
            _player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
            // reset destination bg team
            _player->SetBGTeam(0);
        }
        // join to bg case
        else if (Battleground* bg = _player->GetBattleground())
        {
            if (_player->IsInvitedForBattlegroundInstance(_player->GetBattlegroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (uint32 sourceNode = GetPlayer()->m_taxi.GetTaxiSource())
    {
        if (!_player->InBattleground())
        {
            GetPlayer()->ContinueTaxiFlight();
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    if (mEntry->IsDungeon() && !GetPlayer()->IsAlive())
        if (GetPlayer()->GetCorpseLocation().GetMapId() == mEntry->MapID)
    {
        GetPlayer()->ResurrectPlayer(0.5f, false);
        GetPlayer()->SpawnCorpseBones();
    }

    bool allowMount = !mEntry->IsDungeon() || mEntry->IsBattlegroundOrArena();
    if (mInstance)
    {
        Difficulty diff = GetPlayer()->GetDifficulty(mEntry->IsRaid());
        if (MapDifficulty const* mapDiff = GetMapDifficultyData(mEntry->MapID, diff))
        {
            if (mapDiff->resetTime)
            {
                if (time_t timeReset = sInstanceSaveMgr->GetResetTimeFor(mEntry->MapID, diff))
                {
                    uint32 timeleft = uint32(timeReset - time(NULL));
                    GetPlayer()->SendInstanceResetWarning(mEntry->MapID, diff, timeleft);
                }
            }
        }
        allowMount = mInstance->AllowMount;
    }

    // mount allow check
    if (!allowMount)
        _player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // update zone immediately, otherwise leave channel will cause crash in mtmap
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea);
    GetPlayer()->UpdateZone(newzone, newarea);

    // honorless target
    if (GetPlayer()->pvpInfo.IsHostile)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // in friendly area
    else if (GetPlayer()->IsPvP() && !GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
        GetPlayer()->UpdatePvP(false);

    // Resummon pet, 2ms just to be sure what player's visibility update (it scheduled with 1ms) will be before pet
    GetPlayer()->m_Events.Schedule(2, [=]()
    {
        GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();
    });

    // resummon battle pet
    GetPlayer()->GetBattlePetMgr().ResummonLastBattlePet();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_TELEPORT_ACK");

    ObjectGuid guid;
    uint32 flags, time;
    recvPacket >> time >> flags;

    guid[0] = recvPacket.ReadBit();
    guid[7] = recvPacket.ReadBit();
    guid[3] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[6]);
    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[0]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[3]);

    TC_LOG_DEBUG("network", "Guid " UI64FMTD, uint64(guid));
    TC_LOG_DEBUG("network", "Flags %u, time %u", flags, time/IN_MILLISECONDS);

    Player* plMover = _player->m_mover->ToPlayer();

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetGUID())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->UpdatePosition(dest, true);

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (plMover->pvpInfo.IsHostile)
            plMover->CastSpell(plMover, 2479, true);

        // in friendly area
        else if (plMover->IsPvP() && !plMover->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
            plMover->UpdatePvP(false);
    }

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    // resummon battle pet
    GetPlayer()->GetBattlePetMgr().ResummonLastBattlePet();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket& recvPacket)
{
    uint16 opcode = recvPacket.GetOpcode();

    Unit* mover = _player->m_mover;

    ASSERT(mover != NULL);                      // there must always be a mover

    Player* plrMover = mover->ToPlayer();

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (plrMover && plrMover->IsBeingTeleported())
    {
        recvPacket.rfinish();                     // prevent warnings spam
        return;
    }

    // Discard movement updates that were sent while the player uses a movement generator. Fixes movement motion being overriden by position update on client
    MovementGeneratorType moveGenType = mover->GetMotionMaster()->GetCurrentMovementGeneratorType();
    if (moveGenType != NULL_MOTION_TYPE && moveGenType != IDLE_MOTION_TYPE)
    {
        recvPacket.rfinish();
        return;
    }

    /* extract packet */
    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvPacket, &movementInfo, nullptr, true);

    // prevent tampered movement data
    if (movementInfo.guid != mover->GetGUID())
    {
        TC_LOG_ERROR("network", "HandleMovementOpcodes: guid error");
        return;
    }
    if (!movementInfo.pos.IsPositionValid())
    {
        TC_LOG_ERROR("network", "HandleMovementOpcodes: Invalid Position");
        return;
    }

    /* handle special cases */
    if (movementInfo.transport.guid)
    {
        // transports size limited
        // (also received at zeppelin leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (std::fabs(movementInfo.transport.pos.GetPositionX()) > 250.0f ||
            std::fabs(movementInfo.transport.pos.GetPositionY()) > 250.0f ||
            std::fabs(movementInfo.transport.pos.GetPositionZ()) > 250.0f)
        {
            recvPacket.rfinish();                 // prevent warnings spam
            return;
        }

        if (!Trinity::IsValidMapCoord(movementInfo.pos.GetPositionX() + movementInfo.transport.pos.GetPositionX(), movementInfo.pos.GetPositionY() + movementInfo.transport.pos.GetPositionY(),
            movementInfo.pos.GetPositionZ() + movementInfo.transport.pos.GetPositionZ(), movementInfo.pos.GetOrientation() + movementInfo.transport.pos.GetOrientation()))
        {
            recvPacket.rfinish();                 // prevent warnings spam
            return;
        }

        // if we boarded a transport, add us to it
        if (plrMover)
        {
            if (!plrMover->GetTransport())
            {
                if (Transport* transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid))
                {
                    plrMover->m_transport = transport;
                    transport->AddPassenger(plrMover);
                }
            }
            else if (plrMover->GetTransport()->GetGUID() != movementInfo.transport.guid)
            {
                bool foundNewTransport = false;
                plrMover->m_transport->RemovePassenger(plrMover);
                if (Transport* transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid))
                {
                    foundNewTransport = true;
                    plrMover->m_transport = transport;
                    transport->AddPassenger(plrMover);
                }

                if (!foundNewTransport)
                {
                    plrMover->m_transport = NULL;
                    movementInfo.ResetTransport();
                }
            }
        }

        if (!mover->GetTransport() && !mover->GetVehicle())
            movementInfo.transport.guid.Clear();
    }
    else if (plrMover && plrMover->GetTransport())                // if we were on a transport, leave
    {
        plrMover->m_transport->RemovePassenger(plrMover);
        plrMover->m_transport = NULL;
        movementInfo.ResetTransport();
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->IsInFlight())
        plrMover->HandleFall(movementInfo);

    if (plrMover && ((movementInfo.flags & MOVEMENTFLAG_SWIMMING) != 0) != plrMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plrMover->SetInWater(!plrMover->IsInWater() || plrMover->GetBaseMap()->IsUnderWater(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY(), movementInfo.pos.GetPositionZ(), _player->GetCollisionHeight()));
    }

    // Client strips this flag for whatever reason when controlling rooted vehicles
    if (mover->HasUnitState(UNIT_STATE_ROOT))
        movementInfo.flags |= MOVEMENTFLAG_ROOT;

    if (plrMover)
    {
        sAnticheatMgr->StartHackDetection(plrMover, movementInfo, opcode);
        plrMover->SanitizeMovementInfo(&movementInfo, true);
    }

    uint32 mstime = getMSTime();
    if(m_clientTimeDelay == 0)
        m_clientTimeDelay = mstime - movementInfo.time;

    /* process position-change */
    movementInfo.time = movementInfo.time + m_clientTimeDelay + MOVEMENT_PACKET_TIME_DELAY;
    movementInfo.guid = mover->GetGUID();
    mover->m_movementInfo = movementInfo;

    /*----------------------*/
    /* process position-change */
    // this is almost never true (not sure why it is sometimes, but it is), normally use mover->IsVehicle()
    if (mover->GetVehicle())
        return;

    if (Transport* transport = mover->GetTransport())
    {
        // Let server be the authority on mover's world-space position when they're on a transport.
        // Prevents NPCs flickering in and out of visibility range if transport's position is too desynched on client's side.
        // Prevents NPCs shifting in place whenever the player moves due to slight position desyncs.
        if (sWorld->getBoolConfig(CONFIG_TRANSPORT_PREFER_SERVER_WORLD_POSITION))
        {
            movementInfo.pos.Relocate(movementInfo.transport.pos);
            float m_orientation = movementInfo.pos.GetOrientation();
            transport->CalculatePassengerPosition(movementInfo.pos.m_positionX, movementInfo.pos.m_positionY, movementInfo.pos.m_positionZ, &m_orientation);
        }
    }
    mover->UpdatePosition(movementInfo.pos);

    WorldPacket data(SMSG_PLAYER_MOVE, recvPacket.size());
    mover->WriteMovementInfo(data);
    mover->SendMessageToSet(&data, _player);

    if (plrMover)                                            // nothing is charmed, or player charmed
    {
        plrMover->ResetEmoteStateIfNeed();
        plrMover->UpdateFallInformationIfNeed(movementInfo, opcode);

        if (movementInfo.pos.GetPositionZ() < plrMover->GetMap()->GetMinHeight(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY()))
        {
            if (!(plrMover->GetBattleground() && plrMover->GetBattleground()->HandlePlayerUnderMap(_player)))
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                /// @todo discard movement packets after the player is rooted
                if (plrMover->IsAlive())
                {
                    plrMover->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
                    plrMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
                    // player can be alive if GM/etc
                    // change the death state to CORPSE to prevent the death timer from
                    // starting in the next player update
                    if (!plrMover->IsAlive())
                        plrMover->KillPlayer();
                }
            }
        }
    }
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recvData)
{
    uint32 opcode = recvData.GetOpcode();

    /* extract packet */
    MovementInfo movementInfo;
    static MovementStatusElements const speedElement = MSEExtraFloat;
    Movement::ExtraMovementStatusElement extras(&speedElement);
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo, &extras);

    // now can skip not our packet
    if (_player->GetGUID() != movementInfo.guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    float newspeed = extras.Data.floatData;
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;

    static char const* const move_type_name[MAX_MOVE_TYPE] =
    {
        "Walk",
        "Run",
        "RunBack",
        "Swim",
        "SwimBack",
        "TurnRate",
        "Flight",
        "FlightBack",
        "PitchRate"
    };

    switch (opcode)
    {
        case CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK:        move_type = MOVE_WALK;        break;
        case CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK:         move_type = MOVE_RUN;         break;
        case CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK:    move_type = MOVE_RUN_BACK;    break;
        case CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK:        move_type = MOVE_SWIM;        break;
        case CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_SWIM_BACK;   break;
        case CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK:         move_type = MOVE_TURN_RATE;   break;
        case CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK:      move_type = MOVE_FLIGHT;      break;
        case CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK: move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK:        move_type = MOVE_PITCH_RATE;  break;
        default:
            TC_LOG_ERROR("network", "WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped. m_forced_speed_changes[MOVE_RUN] store both.
    if (_player->m_forced_speed_changes[move_type] > 0)
    {
        --_player->m_forced_speed_changes[move_type];
        if (_player->m_forced_speed_changes[move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            TC_LOG_ERROR("network", "%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName().c_str(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeed(move_type, _player->GetSpeedRate(move_type), true);
        }
        else                                                // must be lesser - cheating
        {
            TC_LOG_DEBUG("misc", "Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName().c_str(), _player->GetSession()->GetAccountId(), _player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    ObjectGuid guid;

    recvPacket.ReadBit();

    guid[3] = recvPacket.ReadBit();
    guid[0] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();
    guid[7] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[0]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[6]);

    if (GetPlayer()->IsInWorld())
    {
        if (guid != _player->GetClientMoverGuid())
            TC_LOG_ERROR("network", "HandleSetActiveMoverOpcode: incorrect mover guid: mover is " UI64FMTD " (%s - Entry: %u) and should be " UI64FMTD, guid.GetRawValue(), guid.GetTypeName(), guid.GetEntry(), _player->GetClientMoverGuid());
    }
    // Unit* newActivelyMovedUnit = ObjectAccessor::GetUnit(*_player, guid); // todo
    // _player->SetMover(newActivelyMovedUnit);
}

void WorldSession::HandleMoveNotActiveMover(WorldPacket &recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");

    MovementInfo mi;
    GetPlayer()->ReadMovementInfo(recvData, &mi);
    _player->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvData*/)
{
    ObjectGuid guid = GetPlayer()->GetGUID();

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 1 + 8);

    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[3]);

    GetPlayer()->SendMessageToSet(&data, false);
}


void WorldSession::HandleMoveKnockBackAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_KNOCK_BACK_ACK");

    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo);

    if (_player->m_mover->GetGUID() != movementInfo.guid)
        return;

    _player->m_movementInfo = movementInfo;

    WorldPacket data(SMSG_MOVE_UPDATE_KNOCK_BACK, 66);
    _player->WriteMovementInfo(data);
    _player->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_HOVER_ACK");

    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo);
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_WATER_WALK_ACK");

    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvData, &movementInfo);
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recvData)
{
    if (!_player->IsAlive() || _player->IsInCombat())
        return;

    ObjectGuid SummonerGUID;

    SummonerGUID[1] = recvData.ReadBit();
    SummonerGUID[3] = recvData.ReadBit();
    SummonerGUID[5] = recvData.ReadBit();
    SummonerGUID[2] = recvData.ReadBit();
    bool Accept = recvData.ReadBit();
    SummonerGUID[7] = recvData.ReadBit();
    SummonerGUID[0] = recvData.ReadBit();
    SummonerGUID[4] = recvData.ReadBit();
    SummonerGUID[6] = recvData.ReadBit();

    recvData.ReadByteSeq(SummonerGUID[0]);
    recvData.ReadByteSeq(SummonerGUID[1]);
    recvData.ReadByteSeq(SummonerGUID[6]);
    recvData.ReadByteSeq(SummonerGUID[3]);
    recvData.ReadByteSeq(SummonerGUID[5]);
    recvData.ReadByteSeq(SummonerGUID[4]);
    recvData.ReadByteSeq(SummonerGUID[2]);
    recvData.ReadByteSeq(SummonerGUID[7]);

    _player->SummonIfPossible(Accept);
}

void WorldSession::HandleSetCollisionHeightAck(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_SET_COLLISION_HEIGHT_ACK");

    static MovementStatusElements const heightElements[] = { MSEExtraFloat, MSEExtra2Bits };
    Movement::ExtraMovementStatusElement extra(heightElements);
    MovementInfo movementInfo;
    GetPlayer()->ReadMovementInfo(recvPacket, &movementInfo, &extra);
}

void WorldSession::HandleMovementForceAck(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "%s", recvPacket.GetOpcode() == CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK ? "CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK" : "CMSG_MOVE_REMOVE_MOVEMENT_FORCE_ACK");

    if (recvPacket.GetOpcode() == CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK)
    {
        static MovementStatusElements const unkElements[] = { MSEExtraFloat, MSEExtraInt32, MSEExtraFloat };
        Movement::ExtraMovementStatusElement extra(unkElements);
        MovementInfo movementInfo;
        GetPlayer()->ReadMovementInfo(recvPacket, &movementInfo, &extra);
    }
    else
    {
        MovementInfo movementInfo;
        GetPlayer()->ReadMovementInfo(recvPacket, &movementInfo);
    }
}

void WorldSession::HandleMoveSetCanTurnWhileFallingAck(WorldPacket& recvData)
{
    // TODO: movement info
    recvData.rfinish();
}
