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

#ifndef GuildFinderPackets_h__
#define GuildFinderPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace GuildFinder
    {
        class LFGuildAddApplication final : public ClientPacket
        {
        public:
            LFGuildAddApplication(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_ADD_APPLICATION, std::move(packet)) { }

            void Read() override;

            uint32 ClassRoles = 0;
            uint32 Availability = 0;
            uint32 GuildInterests = 0;
            ObjectGuid GuildGuid;
            uint16 CommentLength = 0;
            std::string Comment;
        };

        class LFGuildBrowse final : public ClientPacket
        {
        public:
            LFGuildBrowse(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_BROWSE, std::move(packet)) { }

            void Read() override;

            uint32 PlayerLevel = 0;
            uint32 Availability = 0;
            uint32 ClassRoles = 0;
            uint32 GuildInterests = 0;
        };

        class LFGuildDeclineRecruit final : public ClientPacket
        {
        public:
            LFGuildDeclineRecruit(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_DECLINE_RECRUIT, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
        };

        class LFGuildGetApplications final : public ClientPacket
        {
        public:
            LFGuildGetApplications(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_GET_APPLICATIONS, std::move(packet)) { }

            void Read() override;
        };

        class LFGuildGetRecruits final : public ClientPacket
        {
        public:
            LFGuildGetRecruits(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_GET_RECRUITS, std::move(packet)) { }

            void Read() override;

            uint32 UnkTime = 0;
        };

        class LFGuildPostRequest final : public ClientPacket
        {
        public:
            LFGuildPostRequest(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_POST_REQUEST, std::move(packet)) { }

            void Read() override;
        };

        class LFGuildRemoveApplication final : public ClientPacket
        {
        public:
            LFGuildRemoveApplication(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_REMOVE_APPLICATION, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class LFGuildSetGuildPost final : public ClientPacket
        {
        public:
            LFGuildSetGuildPost(WorldPacket&& packet) : ClientPacket(CMSG_LF_GUILD_SET_GUILD_POST, std::move(packet)) { }

            void Read() override;

            uint32 Level = 0;
            uint32 Availability = 0;
            uint32 ClassRoles = 0;
            uint32 GuildInterests = 0;
            uint16 Length = 0;
            bool Listed = false;
            std::string Comment;
        };
    }
}

#endif
