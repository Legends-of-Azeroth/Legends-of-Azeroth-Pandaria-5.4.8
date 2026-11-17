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

#ifndef SocialPackets_h__
#define SocialPackets_h__

#include "Packet.h"

#include <string>

namespace WorldPackets
{
    namespace Social
    {
        class SendContactList final : public ClientPacket
        {
        public:
            SendContactList(WorldPacket&& packet) : ClientPacket(CMSG_CONTACT_LIST, std::move(packet)) { }

            void Read() override;

            uint32 Unknown = 0;
        };

        class AddFriend final : public ClientPacket
        {
        public:
            AddFriend(WorldPacket&& packet) : ClientPacket(CMSG_ADD_FRIEND, std::move(packet)) { }

            void Read() override;

            std::string FriendName;
            std::string FriendNote;
        };

        class DelFriend final : public ClientPacket
        {
        public:
            DelFriend(WorldPacket&& packet) : ClientPacket(CMSG_DEL_FRIEND, std::move(packet)) { }

            void Read() override;

            ObjectGuid FriendGuid;
        };

        class AddIgnore final : public ClientPacket
        {
        public:
            AddIgnore(WorldPacket&& packet) : ClientPacket(CMSG_ADD_IGNORE, std::move(packet)) { }

            void Read() override;

            std::string IgnoreName;
        };

        class DelIgnore final : public ClientPacket
        {
        public:
            DelIgnore(WorldPacket&& packet) : ClientPacket(CMSG_DEL_IGNORE, std::move(packet)) { }

            void Read() override;

            ObjectGuid IgnoreGuid;
        };

        class SetContactNotes final : public ClientPacket
        {
        public:
            SetContactNotes(WorldPacket&& packet) : ClientPacket(CMSG_SET_CONTACT_NOTES, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            std::string Note;
        };
    }
}

#endif // SocialPackets_h__
