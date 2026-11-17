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

#ifndef VoidStoragePackets_h__
#define VoidStoragePackets_h__

#include "Packet.h"

#include <vector>

namespace WorldPackets
{
    namespace VoidStorage
    {
        class VoidStorageUnlock final : public ClientPacket
        {
        public:
            VoidStorageUnlock(WorldPacket&& packet) : ClientPacket(CMSG_UNLOCK_VOID_STORAGE, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
        };

        class VoidStorageQuery final : public ClientPacket
        {
        public:
            VoidStorageQuery(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_VOID_STORAGE, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
        };

        class VoidStorageTransfer final : public ClientPacket
        {
        public:
            VoidStorageTransfer(WorldPacket&& packet) : ClientPacket(CMSG_VOID_STORAGE_TRANSFER, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
            uint32 CountDeposit = 0;
            std::vector<ObjectGuid> ItemGuids;
            uint32 CountWithdraw = 0;
            std::vector<ObjectGuid> ItemIds;
        };

        class VoidSwapItem final : public ClientPacket
        {
        public:
            VoidSwapItem(WorldPacket&& packet) : ClientPacket(CMSG_SWAP_VOID_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 NewSlot = 0;
            ObjectGuid NpcGuid;
            ObjectGuid ItemId;
        };
    }
}

#endif // VoidStoragePackets_h__
