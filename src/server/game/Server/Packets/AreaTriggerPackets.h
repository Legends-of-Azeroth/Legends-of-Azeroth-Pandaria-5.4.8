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

#ifndef AreaTriggerPackets_h__
#define AreaTriggerPackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace AreaTrigger
    {
        class AreaTrigger final : public ClientPacket
        {
        public:
            explicit AreaTrigger(WorldPacket&& packet) : ClientPacket(CMSG_AREATRIGGER, std::move(packet)) { }

            void Read() override;

            uint32 AreaTriggerID = 0;
            bool FromClient = false;
            bool Entered = false;
        };
    }
}

#endif // AreaTriggerPackets_h__
