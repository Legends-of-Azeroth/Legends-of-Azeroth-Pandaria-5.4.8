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

#ifndef ChatPackets_h__
#define ChatPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Chat
    {
        class EmoteClient final : public ClientPacket
        {
        public:
            EmoteClient(WorldPacket&& packet) : ClientPacket(CMSG_EMOTE, std::move(packet)) { }

            void Read() override;

            uint32 EmoteID = 0;
        };

        class CTextEmote final : public ClientPacket
        {
        public:
            CTextEmote(WorldPacket&& packet) : ClientPacket(CMSG_TEXT_EMOTE, std::move(packet)) { }

            void Read() override;

            uint32 EmoteID = 0;
            uint32 SoundIndex = 0;
            ObjectGuid Target;
        };

        class ChatReportIgnored final : public ClientPacket
        {
        public:
            ChatReportIgnored(WorldPacket&& packet) : ClientPacket(CMSG_CHAT_IGNORED, std::move(packet)) { }

            void Read() override;

            uint8 Reason = 0;
            ObjectGuid IgnoredGUID;
        };
    }
}

#endif // ChatPackets_h__
