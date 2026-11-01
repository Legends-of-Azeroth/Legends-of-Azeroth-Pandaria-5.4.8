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

#ifndef PartyPackets_h__
#define PartyPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "Group.h"

namespace WorldPackets
{
    namespace Party
    {
        class SendRaidTargetUpdateAll final : public ServerPacket
        {
        public:
            SendRaidTargetUpdateAll() : ServerPacket(SMSG_RAID_TARGET_UPDATE_ALL, 1 + TARGETICONCOUNT * 9) { }

            WorldPacket const* Write() override;

            int8 PartyIndex = 0;
            std::map<uint8, ObjectGuid> TargetIcons;
        };

        class PartyInviteClient final : public ClientPacket
        {
        public:
            PartyInviteClient(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_INVITE, std::move(packet)) { }

            void Read() override;

            ObjectGuid CrossRealmGuid;
            std::string RealmName;
            std::string MemberName;
        };

        class GroupInviteResponse final : public ClientPacket
        {
        public:
            GroupInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            bool Unknown = false;
            bool Accept = false;
        };

        class GroupUninviteGuid final : public ClientPacket
        {
        public:
            GroupUninviteGuid(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_UNINVITE_GUID, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            std::string Reason;
        };

        class GroupSetLeader final : public ClientPacket
        {
        public:
            GroupSetLeader(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_SET_LEADER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class GroupSetRoles final : public ClientPacket
        {
        public:
            GroupSetRoles(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_SET_ROLES, std::move(packet)) { }

            void Read() override;

            uint32 NewRole = 0;
            ObjectGuid TargetGuid;
        };

        class GroupDisband final : public ClientPacket
        {
        public:
            GroupDisband(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_DISBAND, std::move(packet)) { }

            void Read() override;
        };

        class LootMethod final : public ClientPacket
        {
        public:
            LootMethod(WorldPacket&& packet) : ClientPacket(CMSG_LOOT_METHOD, std::move(packet)) { }

            void Read() override;

            uint8 Method = 0;
            uint32 LootThreshold = 0;
            ObjectGuid LootMaster;
        };

        class MinimapPingClient final : public ClientPacket
        {
        public:
            MinimapPingClient(WorldPacket&& packet) : ClientPacket(CMSG_MINIMAP_PING, std::move(packet)) { }

            void Read() override;

            float Y = 0.0f;
            float X = 0.0f;
        };

        class RandomRollClient final : public ClientPacket
        {
        public:
            RandomRollClient(WorldPacket&& packet) : ClientPacket(CMSG_RANDOM_ROLL, std::move(packet)) { }

            void Read() override;

            uint32 Maximum = 0;
            uint32 Minimum = 0;
        };

        class GroupRaidConvert final : public ClientPacket
        {
        public:
            GroupRaidConvert(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_RAID_CONVERT, std::move(packet)) { }

            void Read() override;

            bool ToRaid = false;
        };

        class GroupChangeSubGroup final : public ClientPacket
        {
        public:
            GroupChangeSubGroup(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_CHANGE_SUB_GROUP, std::move(packet)) { }

            void Read() override;

            uint8 GroupNr = 0;
            uint8 GroupIdx = 0;
            ObjectGuid Guid;
        };

        class GroupAssistantLeader final : public ClientPacket
        {
        public:
            GroupAssistantLeader(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_ASSISTANT_LEADER, std::move(packet)) { }

            void Read() override;

            uint8 Unk = 0;
            ObjectGuid Guid;
            bool Apply = false;
        };

        class SetEveryoneIsAssistant final : public ClientPacket
        {
        public:
            SetEveryoneIsAssistant(WorldPacket&& packet) : ClientPacket(CMSG_SET_EVERYONE_IS_ASSISTANT, std::move(packet)) { }

            void Read() override;

            bool Apply = false;
        };

        class RaidReadyCheck final : public ClientPacket
        {
        public:
            RaidReadyCheck(WorldPacket&& packet) : ClientPacket(CMSG_RAID_READY_CHECK, std::move(packet)) { }

            void Read() override;

            uint8 PartyIndex = 0;
        };

        class RaidReadyCheckConfirm final : public ClientPacket
        {
        public:
            RaidReadyCheckConfirm(WorldPacket&& packet) : ClientPacket(CMSG_RAID_READY_CHECK_CONFIRM, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            bool Status = false;
        };

        class RequestPartyMemberStats final : public ClientPacket
        {
        public:
            RequestPartyMemberStats(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_PARTY_MEMBER_STATS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class RequestRaidInfo final : public ClientPacket
        {
        public:
            RequestRaidInfo(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_RAID_INFO, std::move(packet)) { }

            void Read() override { }
        };

        class OptOutOfLoot final : public ClientPacket
        {
        public:
            OptOutOfLoot(WorldPacket&& packet) : ClientPacket(CMSG_OPT_OUT_OF_LOOT, std::move(packet)) { }

            void Read() override;

            bool PassOnLoot = false;
        };

        class RolePollBegin final : public ClientPacket
        {
        public:
            RolePollBegin(WorldPacket&& packet) : ClientPacket(CMSG_ROLE_POLL_BEGIN, std::move(packet)) { }

            void Read() override;

            uint8 Index = 0;
        };

        class GroupRequestJoinUpdates final : public ClientPacket
        {
        public:
            GroupRequestJoinUpdates(WorldPacket&& packet) : ClientPacket(CMSG_GROUP_REQUEST_JOIN_UPDATES, std::move(packet)) { }

            void Read() override;

            uint8 PartyIndex = 0;
        };

        class ClearRaidMarker final : public ClientPacket
        {
        public:
            ClearRaidMarker(WorldPacket&& packet) : ClientPacket(CMSG_CLEAR_RAID_MARKER, std::move(packet)) { }

            void Read() override;

            int8 MarkerId = 0;
        };
    }
}

#endif // PartyPackets_h__
