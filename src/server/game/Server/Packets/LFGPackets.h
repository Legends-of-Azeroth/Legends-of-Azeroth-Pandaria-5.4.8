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

#ifndef LFGPackets_h__
#define LFGPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include <vector>
#include <string>

namespace WorldPackets
{
    namespace LFG
    {
        class LFGJoin final : public ClientPacket
        {
        public:
            LFGJoin(WorldPacket&& packet) : ClientPacket(CMSG_LFG_JOIN, std::move(packet)) { }

            void Read() override;

            uint32 Roles = 0;
            std::vector<uint32> Dungeons;
            std::string Comment;
        };

        class LFGLeave final : public ClientPacket
        {
        public:
            LFGLeave(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LEAVE, std::move(packet)) { }

            void Read() override;

            uint32 QueueId = 0;
            ObjectGuid LeaveGuid;
        };

        class DFProposalResponse final : public ClientPacket
        {
        public:
            DFProposalResponse(WorldPacket&& packet) : ClientPacket(CMSG_LFG_PROPOSAL_RESULT, std::move(packet)) { }

            void Read() override;

            uint32 ProposalID = 0;
            bool Accepted = false;
        };

        class DFSetRoles final : public ClientPacket
        {
        public:
            DFSetRoles(WorldPacket&& packet) : ClientPacket(CMSG_LFG_SET_ROLES, std::move(packet)) { }

            void Read() override;

            uint32 RolesDesired = 0;
        };

        class DFBootPlayerVote final : public ClientPacket
        {
        public:
            DFBootPlayerVote(WorldPacket&& packet) : ClientPacket(CMSG_LFG_SET_BOOT_VOTE, std::move(packet)) { }

            void Read() override;

            bool Vote = false;
        };

        class DFTeleport final : public ClientPacket
        {
        public:
            DFTeleport(WorldPacket&& packet) : ClientPacket(CMSG_LFG_TELEPORT, std::move(packet)) { }

            void Read() override;

            bool TeleportOut = false;
        };

        class DFGetSystemInfo final : public ClientPacket
        {
        public:
            DFGetSystemInfo(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LOCK_INFO_REQUEST, std::move(packet)) { }

            void Read() override;

            bool Player = false;
        };

        class DFGetJoinStatus final : public ClientPacket
        {
        public:
            DFGetJoinStatus(WorldPacket&& packet) : ClientPacket(CMSG_LFG_GET_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class SetLfgBonusFactionId final : public ClientPacket
        {
        public:
            SetLfgBonusFactionId(WorldPacket&& packet) : ClientPacket(CMSG_SET_LFG_BONUS_FACTION_ID, std::move(packet)) { }

            void Read() override;

            uint32 BonusFactionId = 0;
        };
    }
}

#endif // LFGPackets_h__
