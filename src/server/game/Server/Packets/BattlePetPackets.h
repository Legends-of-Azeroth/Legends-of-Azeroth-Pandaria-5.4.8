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

#ifndef BattlePetPackets_h__
#define BattlePetPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace BattlePet
    {
        class PetBattleStartPvpMatchmaking final : public ClientPacket
        {
        public:
            PetBattleStartPvpMatchmaking(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_START_PVP_MATCHMAKING, std::move(packet)) { }

            void Read() override { }
        };

        class CageBattlePet final : public ClientPacket
        {
        public:
            CageBattlePet(WorldPacket&& packet) : ClientPacket(CMSG_CAGE_BATTLE_PET, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetEntry;
        };

        class PetBattleInput final : public ClientPacket
        {
        public:
            PetBattleInput(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_INPUT, std::move(packet)) { }

            void Read() override;

            bool HasAbilityId = false;
            bool HasNewFrontPet = false;
            uint32 AbilityId = 0;
            uint8 NewFrontPet = 0;
            uint8 MoveType = 0;
        };

        class PetBattleRequestWild final : public ClientPacket
        {
        public:
            PetBattleRequestWild(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_REQUEST_WILD, std::move(packet)) { }

            void Read() override;

            float TeamPositions[2][3];   // [team][x, y, z]
            float BattleOrigin[3];       // x, y, z
            ObjectGuid OpponentGuid;
            float BattleFacing = 0;
            uint32 LocationResult = 0;
        };

        class PetBattleSetFrontPet final : public ClientPacket
        {
        public:
            PetBattleSetFrontPet(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_SET_FRONT_PET, std::move(packet)) { }

            void Read() override;

            uint8 PetNum = 0;
        };

        class BattlePetRequestJournal final : public ClientPacket
        {
        public:
            BattlePetRequestJournal(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_REQUEST_JOURNAL, std::move(packet)) { }

            void Read() override { }
        };

        class BattlePetSetBattleSlot final : public ClientPacket
        {
        public:
            BattlePetSetBattleSlot(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SET_BATTLE_SLOT, std::move(packet)) { }

            void Read() override;

            uint8 Slot = 0;
            ObjectGuid PetEntry;
        };

        class BattlePetSetFlags final : public ClientPacket
        {
        public:
            BattlePetSetFlags(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SET_FLAGS, std::move(packet)) { }

            void Read() override;

            uint32 Flag = 0;
            uint8 Mode = 0;
            ObjectGuid PetEntry;
        };

        class BattlePetSummon final : public ClientPacket
        {
        public:
            BattlePetSummon(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SUMMON_COMPANION, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetEntry;
        };

        class QueryBattlePetName final : public ClientPacket
        {
        public:
            QueryBattlePetName(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_BATTLE_PET_NAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetEntry;
            ObjectGuid PetGuid;
        };

        class BattlePetDeletePet final : public ClientPacket
        {
        public:
            BattlePetDeletePet(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_DELETE_PET, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetEntry;
        };

        class BattlePetLearn final : public ClientPacket
        {
        public:
            BattlePetLearn(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_LEARN, std::move(packet)) { }

            void Read() override;

            ObjectGuid ItemGuid;
        };

        class BattlePetModifyName final : public ClientPacket
        {
        public:
            BattlePetModifyName(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_MODIFY_NAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetEntry;
            uint8 NicknameLen = 0;
            bool HasDeclinedNames = false;
            std::vector<uint8> DeclinedNameLens;
            std::string Nickname;
            std::vector<std::string> DeclinedNames;
        };
    }
}

#endif // BattlePetPackets_h__
