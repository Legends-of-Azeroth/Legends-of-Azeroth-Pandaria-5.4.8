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

#ifndef CalendarPackets_h__
#define CalendarPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

#include <string>
#include <vector>

namespace WorldPackets
{
    namespace Calendar
    {
        struct CalendarInviteData
        {
            uint8 Status = 0;
            uint8 ModerationRank = 0;
        };

        class CalendarGetCalendar final : public ClientPacket
        {
        public:
            CalendarGetCalendar(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_GET_CALENDAR, std::move(packet)) { }

            void Read() override { }
        };

        class CalendarGetEvent final : public ClientPacket
        {
        public:
            CalendarGetEvent(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_GET_EVENT, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
        };

        class CalendarRemoveEvent final : public ClientPacket
        {
        public:
            CalendarRemoveEvent(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_REMOVE_EVENT, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
        };

        class CalendarCopyEvent final : public ClientPacket
        {
        public:
            CalendarCopyEvent(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_COPY_EVENT, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
            uint64 InviteId = 0;
            uint32 Time = 0;
        };

        class CalendarInvite final : public ClientPacket
        {
        public:
            CalendarInvite(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_INVITE, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
            uint64 InviteId = 0;
            bool IsPreInvite = false;
            bool IsGuildEvent = false;
            std::string Name;
        };

        class CalendarRemoveInvite final : public ClientPacket
        {
        public:
            CalendarRemoveInvite(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_REMOVE_INVITE, std::move(packet)) { }

            void Read() override;

            uint64 InviteId = 0;
            uint64 SenderId = 0;
            uint64 EventId = 0;
            ObjectGuid Invitee;
        };

        class CalendarRSVP final : public ClientPacket
        {
        public:
            CalendarRSVP(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_RSVP, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
            uint64 InviteId = 0;
            uint8 Status = 0;
        };

        class CalendarEventSignUp final : public ClientPacket
        {
        public:
            CalendarEventSignUp(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_SIGNUP, std::move(packet)) { }

            void Read() override;

            uint64 EventId = 0;
            bool Tentative = false;
        };

        class CalendarStatus final : public ClientPacket
        {
        public:
            CalendarStatus(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_STATUS, std::move(packet)) { }

            void Read() override;

            uint64 SenderId = 0;
            uint64 EventId = 0;
            uint64 InviteId = 0;
            uint8 Status = 0;
            ObjectGuid Invitee;
        };

        class CalendarModeratorStatusQuery final : public ClientPacket
        {
        public:
            CalendarModeratorStatusQuery(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_EVENT_MODERATOR_STATUS, std::move(packet)) { }

            void Read() override;

            uint8 Rank = 0;
            uint64 EventId = 0;
            uint64 SenderId = 0;
            uint64 InviteId = 0;
            ObjectGuid Invitee;
        };

        class CalendarGetNumPending final : public ClientPacket
        {
        public:
            CalendarGetNumPending(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_GET_NUM_PENDING, std::move(packet)) { }

            void Read() override { }
        };

        class CalendarComplain final : public ClientPacket
        {
        public:
            CalendarComplain(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_COMPLAIN, std::move(packet)) { }

            void Read() override;

            uint64 InviteId = 0;
            uint64 EventId = 0;
            ObjectGuid ComplainGuid;
        };

        class CalendarGuildFilter final : public ClientPacket
        {
        public:
            CalendarGuildFilter(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_GUILD_FILTER, std::move(packet)) { }

            void Read() override;

            uint8 MaxLevel = 0;
            uint8 MinRank = 0;
            uint8 MinLevel = 0;
        };

        class CalendarAddEvent final : public ClientPacket
        {
        public:
            CalendarAddEvent(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_ADD_EVENT, std::move(packet)) { }

            void Read() override;

            uint32 MaxInvites = 0;
            uint32 Flags = 0;
            int32 DungeonId = 0;
            uint32 EventPackedTime = 0;
            uint8 Type = 0;
            std::vector<CalendarInviteData> Invites;
            std::string Title;
            std::string Description;
        };

        class CalendarUpdateEvent final : public ClientPacket
        {
        public:
            CalendarUpdateEvent(WorldPacket&& packet) : ClientPacket(CMSG_CALENDAR_UPDATE_EVENT, std::move(packet)) { }

            void Read() override;

            uint32 MaxInvites = 0;
            int32 DungeonId = 0;
            uint32 EventPackedTime = 0;
            uint32 Flags = 0;
            uint8 Type = 0;
            ObjectGuid EventId;
            ObjectGuid InviteId;
            std::string Title;
            std::string Description;
        };
    }
}

#endif // CalendarPackets_h__
