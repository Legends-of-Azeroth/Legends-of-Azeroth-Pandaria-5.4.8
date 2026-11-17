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

#ifndef SpellPackets_h__
#define SpellPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"
#include "SharedDefines.h"
#include "Spell.h"
#include <string>
#include <vector>

namespace WorldPackets
{
    namespace Spells
    {
        
        class ClearTarget final : public ServerPacket
        {
        public:
            ClearTarget() : ServerPacket(SMSG_CLEAR_TARGET, 8) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class UseItem final : public ClientPacket
        {
        public:
            UseItem(WorldPacket&& packet) : ClientPacket(CMSG_USE_ITEM, std::move(packet)) { }

            void Read() override;

            uint8 Slot = 0;
            uint8 BagIndex = 0;
            uint8 CastCount = 0;
            uint8 CastFlags = 0;
            uint32 SpellId = 0;
            uint32 GlyphIndex = 0;
            uint32 TargetMask = 0;
            float Elevation = 0.0f;
            float MissileSpeed = 0.0f;
            ObjectGuid ItemGuid;
            ObjectGuid TargetGuid;
            ObjectGuid ItemTargetGuid;
            ObjectGuid TransportDstGuid;
            ObjectGuid TransportSrcGuid;
            Position SrcLoc;
            Position DstLoc;
            std::string TargetString;
            bool HasSrcLocation = false;
            bool HasDestLocation = false;
        };

        class CastSpell final : public ClientPacket
        {
        public:
            CastSpell(WorldPacket&& packet) : ClientPacket(CMSG_CAST_SPELL, std::move(packet)) { }

            void Read() override;

            uint8 CastCount = 0;
            uint8 CastFlags = 0;
            uint32 SpellId = 0;
            uint32 GlyphIndex = 0;
            uint32 TargetMask = 0;
            float Elevation = 0.0f;
            float MissileSpeed = 0.0f;
            ObjectGuid TargetGuid;
            ObjectGuid ItemTargetGuid;
            ObjectGuid TransportDstGuid;
            ObjectGuid TransportSrcGuid;
            Position SrcLoc;
            Position DstLoc;
            std::string TargetString;
            std::vector<SpellResearchInfo> ResearchInfoVector;
            bool HasSrcLocation = false;
            bool HasDestLocation = false;
        };

        class PetCancelAura final : public ClientPacket
        {
        public:
            PetCancelAura(WorldPacket&& packet) : ClientPacket(CMSG_PET_CANCEL_AURA, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
            ObjectGuid PetGuid;
        };

        class SpellClick final : public ClientPacket
        {
        public:
            SpellClick(WorldPacket&& packet) : ClientPacket(CMSG_SPELLCLICK, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class GetMirrorImageData final : public ClientPacket
        {
        public:
            GetMirrorImageData(WorldPacket&& packet) : ClientPacket(CMSG_GET_MIRROR_IMAGE_DATA, std::move(packet)) { }

            void Read() override;

            uint32 DisplayId = 0;
            ObjectGuid Guid;
        };

        class MissileTrajectoryCollision final : public ClientPacket
        {
        public:
            MissileTrajectoryCollision(WorldPacket&& packet) : ClientPacket(CMSG_MISSILE_TRAJECTORY_COLLISION, std::move(packet)) { }

            void Read() override;

            float Z = 0.0f;
            uint8 CastCount = 0;
            float Y = 0.0f;
            float X = 0.0f;
            uint32 SpellId = 0;
            ObjectGuid CasterGuid;
        };

        class RequestCategoryCooldowns final : public ClientPacket
        {
        public:
            RequestCategoryCooldowns(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_CATEGORY_COOLDOWNS, std::move(packet)) { }

            void Read() override { }
        };

        class ChoiceResponse final : public ClientPacket
        {
        public:
            ChoiceResponse(WorldPacket&& packet) : ClientPacket(CMSG_CHOICE_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint32 ResponseId = 0;
            uint32 ChoiceId = 0;
        };

    }
}

#endif // SpellPackets_h__
