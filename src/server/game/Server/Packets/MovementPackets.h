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

#ifndef MovementPackets_h__
#define MovementPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "MovementInfo.h"
#include "MovementStructures.h"

namespace WorldPackets
{
    namespace Movement
    {
        bool ReadMovementInfo(WorldPacket& data, MovementInfo& mi, ::Movement::ExtraMovementStatusElement* extras = nullptr, uint32* mountDisplayId = nullptr, bool* hasMountDisplayId = nullptr);

        class ClientPlayerMovement final : public ClientPacket
        {
        public:
            explicit ClientPlayerMovement(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            MovementInfo Status;

            uint32 GetMountDisplayId() const { return _mountDisplayId; }
            bool HasMountDisplayId() const { return _hasMountDisplayId; }

        private:
            uint32 _mountDisplayId = 0;
            bool _hasMountDisplayId = false;
        };

        class MoveTeleportAck final : public ClientPacket
        {
        public:
            explicit MoveTeleportAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_TELEPORT_ACK, std::move(packet)) { }

            void Read() override;

            ObjectGuid MoverGUID;
            int32 MoveTime = 0;
            uint32 Flags = 0;
        };

        class SetActiveMover final : public ClientPacket
        {
        public:
            explicit SetActiveMover(WorldPacket&& packet) : ClientPacket(CMSG_SET_ACTIVE_MOVER, std::move(packet)) { }

            void Read() override;

            ObjectGuid MoverGUID;
        };

        class MoveKnockBackAck final : public ClientPacket
        {
        public:
            explicit MoveKnockBackAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_KNOCK_BACK_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Ack;
        };

        class MoveHoverAck final : public ClientPacket
        {
        public:
            explicit MoveHoverAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_HOVER_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Status;
        };

        class MoveWaterWalkAck final : public ClientPacket
        {
        public:
            explicit MoveWaterWalkAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_WATER_WALK_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Status;
        };

        class MoveSetCollisionHeightAck final : public ClientPacket
        {
        public:
            explicit MoveSetCollisionHeightAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_SET_COLLISION_HEIGHT_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Data;
            float Height = 0.0f;
            uint32 Reason = 0;
        };

        class MoveApplyMovementForceAck final : public ClientPacket
        {
        public:
            explicit MoveApplyMovementForceAck(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            MovementInfo Ack;
            float Speed = 0.0f;
            int32 UnkInt = 0;
            float DirectionX = 0.0f;
        };

        class MovementSpeedAck final : public ClientPacket
        {
        public:
            explicit MovementSpeedAck(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            MovementInfo Ack;
            float Speed = 0.0f;
        };

        class MoveSetCanTurnWhileFallingAck final : public ClientPacket
        {
        public:
            explicit MoveSetCanTurnWhileFallingAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_SET_CAN_TURN_WHILE_FALLING_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Status;
        };

        class SummonResponse final : public ClientPacket
        {
        public:
            explicit SummonResponse(WorldPacket&& packet) : ClientPacket(CMSG_SUMMON_RESPONSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid SummonerGUID;
            bool Accept = false;
        };

        class WorldPortResponse final : public ClientPacket
        {
        public:
            // MSG_MOVE_WORLDPORT_ACK is a bi-directional (OpcodeServerClient) opcode, so we can't use the validating ctor
            explicit WorldPortResponse(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override { }
        };

        class MountSpecialAnim final : public ClientPacket
        {
        public:
            explicit MountSpecialAnim(WorldPacket&& packet) : ClientPacket(CMSG_MOUNTSPECIAL_ANIM, std::move(packet)) { }

            void Read() override { }
        };

        class MoveTimeSkipped final : public ClientPacket
        {
        public:
            MoveTimeSkipped(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_TIME_SKIPPED, std::move(packet)) { }

            void Read() override;

            uint32 Time = 0;
            ObjectGuid MoverGUID;
        };

        class FeatherFallAck final : public ClientPacket
        {
        public:
            FeatherFallAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_FEATHER_FALL_ACK, std::move(packet)) { }

            void Read() override { }
        };

        class MoveUnRootAck final : public ClientPacket
        {
        public:
            MoveUnRootAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_FORCE_UNROOT_ACK, std::move(packet)) { }

            void Read() override { }
        };

        class MoveRootAck final : public ClientPacket
        {
        public:
            MoveRootAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_FORCE_ROOT_ACK, std::move(packet)) { }

            void Read() override { }
        };

        class MoveGravityAck final : public ClientPacket
        {
        public:
            // Handles both CMSG_MOVE_GRAVITY_DISABLE_ACK and CMSG_MOVE_GRAVITY_ENABLE_ACK, so we can't use the validating ctor
            MoveGravityAck(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override { }
        };

        class MoveSetCanFlyAck final : public ClientPacket
        {
        public:
            MoveSetCanFlyAck(WorldPacket&& packet) : ClientPacket(CMSG_MOVE_SET_CAN_FLY_ACK, std::move(packet)) { }

            void Read() override;

            MovementInfo Status;
        };
    }
}

#endif // MovementPackets_h__
