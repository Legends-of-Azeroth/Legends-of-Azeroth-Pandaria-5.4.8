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

#ifndef BattlegroundPackets_h__
#define BattlegroundPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Battleground
    {
        class PVPLogDataRequest final : public ClientPacket
        {
        public:
            PVPLogDataRequest(WorldPacket&& packet) : ClientPacket(CMSG_PVP_LOG_DATA, std::move(packet)) { }

            void Read() override { }
        };

        class BattlefieldListRequest final : public ClientPacket
        {
        public:
            BattlefieldListRequest(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_LIST, std::move(packet)) { }

            void Read() override;

            uint32 BgTypeId = 0;
        };

        class BattlefieldPort final : public ClientPacket
        {
        public:
            BattlefieldPort(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_PORT, std::move(packet)) { }

            void Read() override;

            uint8 Action = 0;
            uint32 QueueSlot = 0;
            uint32 Id = 0;
            uint32 Time = 0;
            ObjectGuid Guid;
        };

        class BattlefieldLeave final : public ClientPacket
        {
        public:
            BattlefieldLeave(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_LEAVE, std::move(packet)) { }

            void Read() override { }
        };

        class RequestBattlefieldStatus final : public ClientPacket
        {
        public:
            RequestBattlefieldStatus(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class BattlemasterJoinArena final : public ClientPacket
        {
        public:
            BattlemasterJoinArena(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_ARENA, std::move(packet)) { }

            void Read() override;

            uint8 ArenaSlot = 0;
        };

        class BattlemasterJoinRated final : public ClientPacket
        {
        public:
            BattlemasterJoinRated(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_RATED, std::move(packet)) { }

            void Read() override { }
        };

        class ReportPvPPlayerAFK final : public ClientPacket
        {
        public:
            ReportPvPPlayerAFK(WorldPacket&& packet) : ClientPacket(CMSG_REPORT_PVP_AFK, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
        };

        class RequestPVPRewards final : public ClientPacket
        {
        public:
            RequestPVPRewards(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_PVP_REWARDS, std::move(packet)) { }

            void Read() override { }
        };

        class BattlefieldMgrQueueInviteResponse final : public ClientPacket
        {
        public:
            BattlefieldMgrQueueInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            uint8 Accepted = 0;
        };

        class BattlefieldMgrEntryInviteResponse final : public ClientPacket
        {
        public:
            BattlefieldMgrEntryInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            uint8 Accepted = 0;
        };

        class BattlefieldMgrQueueRequest final : public ClientPacket
        {
        public:
            BattlefieldMgrQueueRequest(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_QUEUE_REQUEST, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class BattlefieldMgrExitRequest final : public ClientPacket
        {
        public:
            BattlefieldMgrExitRequest(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_EXIT_REQUEST, std::move(packet)) { }

            void Read() override { }
        };

        class AreaSpiritHealerQuery final : public ClientPacket
        {
        public:
            AreaSpiritHealerQuery(WorldPacket&& packet) : ClientPacket(CMSG_AREA_SPIRIT_HEALER_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class AreaSpiritHealerQueue final : public ClientPacket
        {
        public:
            AreaSpiritHealerQueue(WorldPacket&& packet) : ClientPacket(CMSG_AREA_SPIRIT_HEALER_QUEUE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class BattlemasterJoin final : public ClientPacket
        {
        public:
            BattlemasterJoin(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN, std::move(packet)) { }

            void Read() override;

            bool AsGroup = false;
            bool HasRoleMask = false;
            ObjectGuid Guid;
            uint8 RoleMask = 0;
        };

        class RequestPvpOptionsEnabled final : public ClientPacket
        {
        public:
            RequestPvpOptionsEnabled(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_PVP_OPTIONS_ENABLED, std::move(packet)) { }

            void Read() override { }
        };

        class RequestRatedBgStats final : public ClientPacket
        {
        public:
            RequestRatedBgStats(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_RATED_BG_STATS, std::move(packet)) { }

            void Read() override { }
        };

        class RequestConquestFormulaConstants final : public ClientPacket
        {
        public:
            RequestConquestFormulaConstants(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS, std::move(packet)) { }

            void Read() override { }
        };
    }
}

#endif // BattlegroundPackets_h__
