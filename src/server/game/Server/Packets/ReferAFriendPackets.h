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

#ifndef ReferAFriendPackets_h__
#define ReferAFriendPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace ReferAFriend
    {
        class GrantLevel final : public ClientPacket
        {
        public:
            GrantLevel(WorldPacket&& packet) : ClientPacket(CMSG_GRANT_LEVEL, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class AcceptGrantLevel final : public ClientPacket
        {
        public:
            AcceptGrantLevel(WorldPacket&& packet) : ClientPacket(CMSG_ACCEPT_LEVEL_GRANT, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };
    }
}

#endif // ReferAFriendPackets_h__
