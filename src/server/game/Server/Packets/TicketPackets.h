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

#ifndef TicketPackets_h__
#define TicketPackets_h__

#include "Packet.h"

#include <list>
#include <string>

namespace WorldPackets
{
    namespace Ticket
    {
        class GMTicketGetCaseStatus final : public ClientPacket
        {
        public:
            GMTicketGetCaseStatus(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_CASE_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class GMTicketGetSystemStatus final : public ClientPacket
        {
        public:
            GMTicketGetSystemStatus(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_SYSTEM_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class GMTicketUpdateText final : public ClientPacket
        {
        public:
            GMTicketUpdateText(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_UPDATE_TEXT, std::move(packet)) { }

            void Read() override;

            std::string Message;
        };

        class GMTicketGetTicket final : public ClientPacket
        {
        public:
            GMTicketGetTicket(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_GET_TICKET, std::move(packet)) { }

            void Read() override { }
        };

        class GMTicketDeleteTicket final : public ClientPacket
        {
        public:
            GMTicketDeleteTicket(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_DELETE_TICKET, std::move(packet)) { }

            void Read() override { }
        };

        class GMTicketCreate final : public ClientPacket
        {
        public:
            GMTicketCreate(WorldPacket&& packet) : ClientPacket(CMSG_GM_TICKET_CREATE, std::move(packet)) { }

            void Read() override;

            uint32 MapId = 0;
            float X = 0.0f;
            float Y = 0.0f;
            float Z = 0.0f;
            std::list<uint32> Times;
            std::string ChatLog;
            bool DecompressOk = true;
            bool NeedResponse = false;
            bool HaveTicket = false;
            std::string Message;
        };
    }
}

#endif // TicketPackets_h__
