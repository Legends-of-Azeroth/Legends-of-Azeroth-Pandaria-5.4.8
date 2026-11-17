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

#ifndef ChannelPackets_h__
#define ChannelPackets_h__

#include "Packet.h"
#include <string>

namespace WorldPackets
{
    namespace Channel
    {
        class JoinChannel final : public ClientPacket
        {
        public:
            JoinChannel(WorldPacket&& packet) : ClientPacket(CMSG_JOIN_CHANNEL, std::move(packet)) { }

            void Read() override;

            uint32 ChannelId = 0;
            std::string Password;
            std::string ChannelName;
        };

        class LeaveChannel final : public ClientPacket
        {
        public:
            LeaveChannel(WorldPacket&& packet) : ClientPacket(CMSG_LEAVE_CHANNEL, std::move(packet)) { }

            void Read() override;

            uint32 Unk = 0;
            std::string ChannelName;
        };

        class ChannelAnnouncements final : public ClientPacket
        {
        public:
            ChannelAnnouncements(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_ANNOUNCEMENTS, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
        };

        class ChannelBan final : public ClientPacket
        {
        public:
            ChannelBan(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_BAN, std::move(packet)) { }

            void Read() override;

            std::string TargetName;
            std::string ChannelName;
        };

        class ChannelDisplayList final : public ClientPacket
        {
        public:
            ChannelDisplayList(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
        };

        class ChannelInvite final : public ClientPacket
        {
        public:
            ChannelInvite(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_INVITE, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
            std::string TargetName;
        };

        class ChannelKick final : public ClientPacket
        {
        public:
            ChannelKick(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_KICK, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
            std::string TargetName;
        };

        class ChannelList final : public ClientPacket
        {
        public:
            ChannelList(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_LIST, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
        };

        class ChannelModerator final : public ClientPacket
        {
        public:
            ChannelModerator(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_MODERATOR, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
            std::string TargetName;
        };

        class ChannelMute final : public ClientPacket
        {
        public:
            ChannelMute(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_MUTE, std::move(packet)) { }

            void Read() override;

            std::string TargetName;
            std::string ChannelName;
        };

        class ChannelOwner final : public ClientPacket
        {
        public:
            ChannelOwner(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_OWNER, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
        };

        class ChannelPassword final : public ClientPacket
        {
        public:
            ChannelPassword(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_PASSWORD, std::move(packet)) { }

            void Read() override;

            std::string Password;
            std::string ChannelName;
        };

        class ChannelSetOwner final : public ClientPacket
        {
        public:
            ChannelSetOwner(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_SET_OWNER, std::move(packet)) { }

            void Read() override;

            std::string ChannelName;
            std::string TargetName;
        };

        class ChannelUnban final : public ClientPacket
        {
        public:
            ChannelUnban(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_UNBAN, std::move(packet)) { }

            void Read() override;

            std::string TargetName;
            std::string ChannelName;
        };

        class ChannelUnmoderator final : public ClientPacket
        {
        public:
            ChannelUnmoderator(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_UNMODERATOR, std::move(packet)) { }

            void Read() override;

            std::string TargetName;
            std::string ChannelName;
        };

        class ChannelUnmute final : public ClientPacket
        {
        public:
            ChannelUnmute(WorldPacket&& packet) : ClientPacket(CMSG_CHANNEL_UNMUTE, std::move(packet)) { }

            void Read() override;

            std::string TargetName;
            std::string ChannelName;
        };

        class ChannelVoiceOn final : public ClientPacket
        {
        public:
            ChannelVoiceOn(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;
        };
    }
}

#endif // ChannelPackets_h__
