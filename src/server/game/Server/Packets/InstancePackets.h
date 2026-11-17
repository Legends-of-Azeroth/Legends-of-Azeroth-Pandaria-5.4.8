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

#ifndef InstancePackets_h__
#define InstancePackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace Instance
    {
        class InstanceLockResponse final : public ClientPacket
        {
        public:
            InstanceLockResponse(WorldPacket&& packet) : ClientPacket(CMSG_INSTANCE_LOCK_WARNING_RESPONSE, std::move(packet)) { }

            void Read() override;

            bool Accept = false;
        };

        class ResetInstances final : public ClientPacket
        {
        public:
            ResetInstances(WorldPacket&& packet) : ClientPacket(CMSG_RESET_INSTANCES, std::move(packet)) { }

            void Read() override { }
        };

        class SetSavedInstanceExtend final : public ClientPacket
        {
        public:
            SetSavedInstanceExtend(WorldPacket&& packet) : ClientPacket(CMSG_SET_SAVED_INSTANCE_EXTEND, std::move(packet)) { }

            void Read() override;

            uint32 MapId = 0;
            uint32 Difficulty = 0;
            bool ToggleExtend = false;
        };

        class SetDungeonDifficulty final : public ClientPacket
        {
        public:
            SetDungeonDifficulty(WorldPacket&& packet) : ClientPacket(CMSG_SET_DUNGEON_DIFFICULTY, std::move(packet)) { }

            void Read() override;

            uint32 Mode = 0;
        };

        class SetRaidDifficulty final : public ClientPacket
        {
        public:
            SetRaidDifficulty(WorldPacket&& packet) : ClientPacket(CMSG_SET_RAID_DIFFICULTY, std::move(packet)) { }

            void Read() override;

            uint32 Mode = 0;
        };
    }
}

#endif // InstancePackets_h__
