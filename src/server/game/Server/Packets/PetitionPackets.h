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

#ifndef PetitionPackets_h__
#define PetitionPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"
#include <string>

namespace WorldPackets
{
    namespace Petitions
    {
        class PetitionBuy final : public ClientPacket
        {
        public:
            PetitionBuy(WorldPacket&& packet) : ClientPacket(CMSG_PETITION_BUY, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
            std::string Name;
        };

        class PetitionShowSignatures final : public ClientPacket
        {
        public:
            PetitionShowSignatures(WorldPacket&& packet) : ClientPacket(CMSG_PETITION_SHOW_SIGNATURES, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetitionGuid;
        };

        class QueryPetition final : public ClientPacket
        {
        public:
            QueryPetition(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_PETITION, std::move(packet)) { }

            void Read() override;

            uint32 GuildGuid = 0;
            ObjectGuid PetitionGuid;
        };

        class PetitionRename final : public ClientPacket
        {
        public:
            PetitionRename(WorldPacket&& packet) : ClientPacket(CMSG_PETITION_RENAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetitionGuid;
            std::string NewName;
        };

        class SignPetition final : public ClientPacket
        {
        public:
            SignPetition(WorldPacket&& packet) : ClientPacket(CMSG_SIGN_PETITION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetitionGuid;
        };

        class PetitionDecline final : public ClientPacket
        {
        public:
            PetitionDecline(WorldPacket&& packet) : ClientPacket(CMSG_DECLINE_PETITION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetitionGuid;
        };

        class OfferPetition final : public ClientPacket
        {
        public:
            OfferPetition(WorldPacket&& packet) : ClientPacket(CMSG_OFFER_PETITION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
            ObjectGuid PetitionGuid;
        };

        class TurnInPetition final : public ClientPacket
        {
        public:
            TurnInPetition(WorldPacket&& packet) : ClientPacket(CMSG_TURN_IN_PETITION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetitionGuid;
        };

        class PetitionShowList final : public ClientPacket
        {
        public:
            PetitionShowList(WorldPacket&& packet) : ClientPacket(CMSG_PETITION_SHOWLIST, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };
    }
}

#endif // PetitionPackets_h__
