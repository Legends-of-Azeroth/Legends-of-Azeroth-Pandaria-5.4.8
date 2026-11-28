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

#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Vehicle.h"
#include "Player.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "MovementStructures.h"

void WorldSession::HandleDismissControlledVehicle(WorldPackets::Vehicle::MoveDismissVehicle& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_DISMISS_CONTROLLED_VEHICLE");

    uint64 vehicleGUID = _player->GetCharmGUID();

    if (!vehicleGUID)                                       // something wrong here...
    {
        return;
    }

    MovementInfo mi = packet.Mi;

    _player->m_movementInfo = mi;


    _player->ExitVehicle();
}

void WorldSession::HandleChangeSeatsOnControlledVehicle(WorldPackets::Vehicle::ChangeSeatsOnControlledVehicle& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");

    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
        return;

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        TC_LOG_ERROR("network", "HandleChangeSeatsOnControlledVehicle, Opcode: {}, Player {} tried to switch seats but current seatflags {} don't permit that.",
            CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE, GetPlayer()->GetGUID().GetCounter(), seat->m_flags);
        return;
    }

    vehicle_base->m_movementInfo = packet.MovementInfo;

    ObjectGuid accessory = packet.AccessoryGuid;
    int8 seatId = packet.SeatId;

    if (vehicle_base->GetGUID() != packet.MovementInfo.guid)
        return;

    if (!accessory)
        GetPlayer()->ChangeSeat(-1, seatId > 0);
    else if (Unit* vehUnit = Unit::GetUnit(*GetPlayer(), accessory))
    {
        if (Vehicle* vehicle = vehUnit->GetVehicleKit())
            if (vehicle->HasEmptySeat(seatId))
                vehUnit->HandleSpellClick(GetPlayer(), seatId);
    }
}

void WorldSession::HandleRequestVehicleNextSeat(WorldPackets::Vehicle::RequestVehicleNextSeat& /*packet*/)
{
    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
        return;

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        TC_LOG_ERROR("network", "HandleRequestVehicleNextSeat, Player {} tried to switch seats but current seatflags {} don't permit that.",
            GetPlayer()->GetGUID().GetCounter(), seat->m_flags);
        return;
    }

    GetPlayer()->ChangeSeat(-1, true);
}

void WorldSession::HandleRequestVehiclePrevSeat(WorldPackets::Vehicle::RequestVehiclePrevSeat& /*packet*/)
{
    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
        return;

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        TC_LOG_ERROR("network", "HandleRequestVehiclePrevSeat, Player {} tried to switch seats but current seatflags {} don't permit that.",
            GetPlayer()->GetGUID().GetCounter(), seat->m_flags);
        return;
    }

    GetPlayer()->ChangeSeat(-1, false);
}

void WorldSession::HandleRequestVehicleSwitchSeat(WorldPackets::Vehicle::RequestVehicleSwitchSeat& packet)
{
    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
        return;

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        TC_LOG_ERROR("network", "HandleRequestVehicleSwitchSeat, Player {} tried to switch seats but current seatflags {} don't permit that.",
            GetPlayer()->GetGUID().GetCounter(), seat->m_flags);
        return;
    }

    if (vehicle_base->GetGUID() == packet.VehicleGuid)
        GetPlayer()->ChangeSeat(packet.SeatId);
    else if (Unit* vehUnit = Unit::GetUnit(*GetPlayer(), packet.VehicleGuid))
        if (Vehicle* vehicle = vehUnit->GetVehicleKit())
            if (vehicle->HasEmptySeat(packet.SeatId))
                vehUnit->HandleSpellClick(GetPlayer(), packet.SeatId);
}

void WorldSession::HandleEnterPlayerVehicle(WorldPackets::Vehicle::PlayerVehicleEnter& packet)
{
    ObjectGuid Guid = packet.Guid;

    if (Player* player = ObjectAccessor::FindPlayer(Guid))
    {
        if (!player->GetVehicleKit())
            return;
        if (!player->IsInRaidWith(_player))
            return;
        if (!player->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        _player->EnterVehicle(player);
    }
}

void WorldSession::HandleEjectPassenger(WorldPackets::Vehicle::EjectPassenger& packet)
{
    Vehicle* vehicle = _player->GetVehicleKit();
    if (!vehicle)
    {
        TC_LOG_ERROR("network", "HandleEjectPassenger: Player {} is not in a vehicle!", GetPlayer()->GetGUID().GetCounter());
        return;
    }

    ObjectGuid guid = packet.Guid;

    if (guid.IsPlayer())
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
        {
            TC_LOG_ERROR("network", "Player {} tried to eject player {} from vehicle, but the latter was not found in world!", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
            return;
        }

        if (!player->IsOnVehicle(vehicle->GetBase()))
        {
            TC_LOG_ERROR("network", "Player {} tried to eject player {}, but they are not in the same vehicle", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(player);
        ASSERT(seat);
        if (seat->IsEjectable())
        {

            player->ExitVehicle();
        }
        else
            TC_LOG_ERROR("network", "Player {} attempted to eject player {} from non-ejectable seat.", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
    }

    else if (guid.IsCreature())
    {
        Unit* unit = ObjectAccessor::GetUnit(*_player, guid);
        if (!unit) // creatures can be ejected too from player mounts
        {
            TC_LOG_ERROR("network", "Player {} tried to eject creature guid {} from vehicle, but the latter was not found in world!", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
            return;
        }

        if (!unit->IsOnVehicle(vehicle->GetBase()))
        {
            TC_LOG_ERROR("network", "Player {} tried to eject unit {}, but they are not in the same vehicle", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(unit);
        ASSERT(seat);
            if (seat->IsEjectable())
            {
                ASSERT(GetPlayer() == vehicle->GetBase());

                unit->ExitVehicle();
            }
        else
            TC_LOG_ERROR("network", "Player {} attempted to eject creature GUID {} from non-ejectable seat.", GetPlayer()->GetGUID().GetCounter(), guid.GetCounter());
    }
    else
        TC_LOG_ERROR("network", "HandleEjectPassenger: Player {} tried to eject invalid GUID " UI64FMTD, GetPlayer()->GetGUID().GetCounter(), uint64(guid));
}

void WorldSession::HandleRequestVehicleExit(WorldPackets::Vehicle::RequestVehicleExit& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_REQUEST_VEHICLE_EXIT");

    if (Vehicle* vehicle = GetPlayer()->GetVehicle())
    {
        if (VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(GetPlayer()))
        {
            if (seat->CanEnterOrExit())
            {

                GetPlayer()->ExitVehicle();
            }
            else
                TC_LOG_ERROR("network", "Player {} tried to exit vehicle, but seatflags {} (ID: {}) don't permit that.",
                GetPlayer()->GetGUID().GetCounter(), seat->m_ID, seat->m_flags);
        }
    }
}

void WorldSession::HandleMoveSetVehicleRecAck(WorldPackets::Vehicle::MoveSetVehicleRecIdAck& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_SET_VEHICLE_REC_ID_ACK");

    uint32 vehicleId = packet.VehicleId;

    ObjectGuid vehicleGuid = GetPlayer()->GetGUID();
    WorldPacket data(SMSG_SET_VEHICLE_REC_ID, 8 + 4);
    data.WriteBit(vehicleGuid[5]);
    data.WriteBit(vehicleGuid[7]);
    data.WriteBit(vehicleGuid[2]);
    data.WriteBit(vehicleGuid[1]);
    data.WriteBit(vehicleGuid[4]);
    data.WriteBit(vehicleGuid[0]);
    data.WriteBit(vehicleGuid[3]);
    data.WriteBit(vehicleGuid[6]);
    data.WriteByteSeq(vehicleGuid[5]);
    data.WriteByteSeq(vehicleGuid[7]);
    data.WriteByteSeq(vehicleGuid[4]);
    data.WriteByteSeq(vehicleGuid[6]);
    data.WriteByteSeq(vehicleGuid[2]);
    data.WriteByteSeq(vehicleGuid[1]);
    data.WriteByteSeq(vehicleGuid[3]);
    data.WriteByteSeq(vehicleGuid[0]);
    data << uint32(vehicleId);
    GetPlayer()->SendMessageToSet(&data, true);
}
