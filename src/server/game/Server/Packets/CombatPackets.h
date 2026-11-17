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

#ifndef CombatPackets_h__
#define CombatPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Combat
    {
        class AttackSwing final : public ClientPacket
        {
        public:
            AttackSwing(WorldPacket&& packet) : ClientPacket(CMSG_ATTACKSWING, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGUID;
        };

        class AttackStop final : public ClientPacket
        {
        public:
            AttackStop(WorldPacket&& packet) : ClientPacket(CMSG_ATTACKSTOP, std::move(packet)) { }

            void Read() override { }
        };

        class SetSheathed final : public ClientPacket
        {
        public:
            SetSheathed(WorldPacket&& packet) : ClientPacket(CMSG_SETSHEATHED, std::move(packet)) { }

            void Read() override;

            uint32 Sheathed = 0;
            bool HasData = false;
        };

        class CancelAura final : public ClientPacket
        {
        public:
            CancelAura(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_AURA, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
            ObjectGuid TargetGUID;
        };

        class CancelCast final : public ClientPacket
        {
        public:
            CancelCast(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_CAST, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
            uint8 Counter = 0;
        };

        class CancelChannelling final : public ClientPacket
        {
        public:
            CancelChannelling(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_CHANNELLING, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
        };

        class CancelAutoRepeatSpell final : public ClientPacket
        {
        public:
            CancelAutoRepeatSpell(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_AUTO_REPEAT_SPELL, std::move(packet)) { }

            void Read() override { }
        };

        class CancelTempEnchantment final : public ClientPacket
        {
        public:
            CancelTempEnchantment(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_TEMP_ENCHANTMENT, std::move(packet)) { }

            void Read() override;

            uint32 Slot = 0;
        };

        class CancelMountAura final : public ClientPacket
        {
        public:
            CancelMountAura(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_MOUNT_AURA, std::move(packet)) { }

            void Read() override { }
        };

        class CancelModSpeedNoControlAuras final : public ClientPacket
        {
        public:
            CancelModSpeedNoControlAuras(WorldPacket&& packet) : ClientPacket(CMSG_CANCEL_MOD_SPEED_NO_CONTROL_AURAS, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGUID;
        };

        class SelfRes final : public ClientPacket
        {
        public:
            SelfRes(WorldPacket&& packet) : ClientPacket(CMSG_SELF_RES, std::move(packet)) { }

            void Read() override { }
        };

        class ResurrectResponse final : public ClientPacket
        {
        public:
            ResurrectResponse(WorldPacket&& packet) : ClientPacket(CMSG_RESURRECT_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint32 Status = 0;
            ObjectGuid ResurrectingPlayer;
        };

        class ReclaimCorpse final : public ClientPacket
        {
        public:
            ReclaimCorpse(WorldPacket&& packet) : ClientPacket(CMSG_RECLAIM_CORPSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGUID;
        };

        class PortGraveyard final : public ClientPacket
        {
        public:
            PortGraveyard(WorldPacket&& packet) : ClientPacket(CMSG_RETURN_TO_GRAVEYARD, std::move(packet)) { }

            void Read() override { }
        };

        class RepopRequest final : public ClientPacket
        {
        public:
            RepopRequest(WorldPacket&& packet) : ClientPacket(CMSG_REPOP_REQUEST, std::move(packet)) { }

            void Read() override;

            uint8 Unk = 0;
        };

        class SetAdvancedCombatLogging final : public ClientPacket
        {
        public:
            SetAdvancedCombatLogging(WorldPacket&& packet) : ClientPacket(CMSG_SET_ADVANCED_COMBAT_LOGGING, std::move(packet)) { }

            void Read() override;
        };
    }
}

#endif // CombatPackets_h__
