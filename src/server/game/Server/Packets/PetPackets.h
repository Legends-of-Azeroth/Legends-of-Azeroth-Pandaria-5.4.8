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

#ifndef PetPackets_h__
#define PetPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "Position.h"

namespace WorldPackets
{
    namespace Pet
    {
        class RequestPetInfo final : public ClientPacket
        {
        public:
            RequestPetInfo(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_PET_INFO, std::move(packet)) { }

            void Read() override { }
        };

        class DismissCritter final : public ClientPacket
        {
        public:
            DismissCritter(WorldPacket&& packet) : ClientPacket(CMSG_DISMISS_CRITTER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class PetNameQuery final : public ClientPacket
        {
        public:
            PetNameQuery(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_PET_NAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
            ObjectGuid PetNumber;
        };

        class PetCastSpell final : public ClientPacket
        {
        public:
            PetCastSpell(WorldPacket&& packet) : ClientPacket(CMSG_PET_CAST_SPELL, std::move(packet)) { }

            void Read() override;

            uint8 CastCount = 0;
            uint32 SpellId = 0;
            uint8 CastFlags = 0;
            uint32 TargetFlags = 0;
            ObjectGuid TargetGuid;
            ObjectGuid ItemGuid;
            ObjectGuid TransportDstGuid;
            ObjectGuid TransportSrcGuid;
            ObjectGuid PetGuid;
            WorldLocation DstLoc;
            WorldLocation SrcLoc;
            float MissileSpeed = 0.0f;
            float Elevation = 0.0f;
        };

        class LearnPetSpecializationGroup final : public ClientPacket
        {
        public:
            LearnPetSpecializationGroup(WorldPacket&& packet) : ClientPacket(CMSG_LEARN_PET_SPECIALIZATION_GROUP, std::move(packet)) { }

            void Read() override;

            uint32 Index = 0;
            ObjectGuid Guid;
        };

        class PetAbandon final : public ClientPacket
        {
        public:
            PetAbandon(WorldPacket&& packet) : ClientPacket(CMSG_PET_ABANDON, std::move(packet)) { }

            void Read() override;

            ObjectGuid Pet;
        };

        class PetStopAttack final : public ClientPacket
        {
        public:
            PetStopAttack(WorldPacket&& packet) : ClientPacket(CMSG_PET_STOP_ATTACK, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGUID;
        };

        class PetSpellAutocast final : public ClientPacket
        {
        public:
            PetSpellAutocast(WorldPacket&& packet) : ClientPacket(CMSG_PET_SPELL_AUTOCAST, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGUID;
            uint32 SpellID = 0;
            bool AutocastEnabled = false;
        };

        class PetAction final : public ClientPacket
        {
        public:
            PetAction(WorldPacket&& packet) : ClientPacket(CMSG_PET_ACTION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGUID;
            ObjectGuid TargetGUID;
            uint32 Action = 0;
            float X = 0;
            float Y = 0;
            float Z = 0;
        };

        class PetSetAction final : public ClientPacket
        {
        public:
            PetSetAction(WorldPacket&& packet) : ClientPacket(CMSG_PET_SET_ACTION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGUID;
            uint32 Index = 0;
            uint32 Action = 0;
        };

        class PetRename final : public ClientPacket
        {
        public:
            PetRename(WorldPacket&& packet) : ClientPacket(CMSG_PET_RENAME, std::move(packet)) { }

            void Read() override;

            uint32 PetNumber = 0;
            bool HasName = false;
            bool IsDeclined = false;
            std::string Name;
            std::vector<std::string> DeclinedNames;
        };
    }
}

#endif // PetPackets_h__
