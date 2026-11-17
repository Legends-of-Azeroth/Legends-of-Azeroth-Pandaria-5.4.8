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

#ifndef EquipmentSetPackets_h__
#define EquipmentSetPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

#include <string>
#include <vector>

namespace WorldPackets
{
    namespace EquipmentSet
    {
        class DeleteEquipmentSet final : public ClientPacket
        {
        public:
            DeleteEquipmentSet(WorldPacket&& packet) : ClientPacket(CMSG_DELETE_EQUIPMENT_SET, std::move(packet)) { }

            void Read() override;

            ObjectGuid SetGuid;
        };

        class SaveEquipmentSet final : public ClientPacket
        {
        public:
            SaveEquipmentSet(WorldPacket&& packet) : ClientPacket(CMSG_SAVE_EQUIPMENT_SET, std::move(packet)) { }

            void Read() override;

            uint32 Index = 0;
            std::vector<ObjectGuid> ItemGuids;
            ObjectGuid SetGuid;
            std::string IconName;
            std::string Name;
        };

        class UseEquipmentSet final : public ClientPacket
        {
        public:
            UseEquipmentSet(WorldPacket&& packet) : ClientPacket(CMSG_USE_EQUIPMENT_SET, std::move(packet)) { }

            void Read() override;

            std::vector<ObjectGuid> ItemGuids;
        };
    }
}

#endif // EquipmentSetPackets_h__
