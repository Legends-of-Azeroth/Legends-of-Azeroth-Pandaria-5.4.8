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

#ifndef VehiclePackets_h__
#define VehiclePackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "MovementInfo.h"

namespace WorldPackets
{
    namespace Vehicle
    {
        class RequestVehicleExit final : public ClientPacket
        {
        public:
            RequestVehicleExit(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_VEHICLE_EXIT, std::move(packet)) { }

            void Read() override { }
        };

        class RequestVehicleNextSeat final : public ClientPacket
        {
        public:
            RequestVehicleNextSeat(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_VEHICLE_NEXT_SEAT, std::move(packet)) { }

            void Read() override { }
        };

        class RequestVehiclePrevSeat final : public ClientPacket
        {
        public:
            RequestVehiclePrevSeat(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_VEHICLE_PREV_SEAT, std::move(packet)) { }

            void Read() override { }
        };

        class RequestVehicleSwitchSeat final : public ClientPacket
        {
        public:
            RequestVehicleSwitchSeat(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_VEHICLE_SWITCH_SEAT, std::move(packet)) { }

            void Read() override;

            int8 SeatId = 0;
            ObjectGuid VehicleGuid;
        };

        class ChangeSeatsOnControlledVehicle final : public ClientPacket
        {
        public:
            ChangeSeatsOnControlledVehicle(WorldPacket&& packet) : ClientPacket(CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE, std::move(packet)) { }

            void Read() override;

            MovementInfo MovementInfo;
            ObjectGuid AccessoryGuid;
            int8 SeatId = 0;
        };

        class MoveSetVehicleRecIdAck final : public ClientPacket
        {
        public:
            MoveSetVehicleRecIdAck(WorldPacket&& packet) : ClientPacket(CMSG_SET_VEHICLE_REC_ID_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo MovementInfo;
            uint32 VehicleId = 0;
        };

        class MoveDismissVehicle final : public ClientPacket
        {
        public:
            MoveDismissVehicle(WorldPacket&& packet) : ClientPacket(CMSG_DISMISS_CONTROLLED_VEHICLE, std::move(packet)) { }

            void Read() override;

            MovementInfo Mi;
        };

        class EjectPassenger final : public ClientPacket
        {
        public:
            EjectPassenger(WorldPacket&& packet) : ClientPacket(CMSG_EJECT_PASSENGER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class PlayerVehicleEnter final : public ClientPacket
        {
        public:
            PlayerVehicleEnter(WorldPacket&& packet) : ClientPacket(CMSG_PLAYER_VEHICLE_ENTER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };
    }
}

#endif // VehiclePackets_h__
