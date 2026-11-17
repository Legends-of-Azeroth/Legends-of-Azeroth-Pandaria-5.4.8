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

#ifndef TalentPackets_h__
#define TalentPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Talent
    {
        class SetPrimaryTalentTree final : public ClientPacket
        {
        public:
            SetPrimaryTalentTree(WorldPacket&& packet) : ClientPacket(CMSG_SET_PRIMARY_TALENT_TREE, std::move(packet)) { }

            void Read() override;

            uint32 TabIndex = 0;
        };

        class LearnTalent final : public ClientPacket
        {
        public:
            LearnTalent(WorldPacket&& packet) : ClientPacket(CMSG_LEARN_TALENT, std::move(packet)) { }

            void Read() override;

            std::vector<uint16> Talents;
        };

        class ConfirmRespecWipe final : public ClientPacket
        {
        public:
            ConfirmRespecWipe(WorldPacket&& packet) : ClientPacket(CMSG_CONFIRM_RESPEC_WIPE, std::move(packet)) { }

            void Read() override;

            uint8 RespecType = 0;
            ObjectGuid RespecMaster;
        };

        class UnlearnSpecialization final : public ClientPacket
        {
        public:
            UnlearnSpecialization(WorldPacket&& packet) : ClientPacket(CMSG_UNLEARN_SPECIALIZATION, std::move(packet)) { }

            void Read() override;

            uint8 SpecIndex = 0;
        };

        class UnlearnSkill final : public ClientPacket
        {
        public:
            UnlearnSkill(WorldPacket&& packet) : ClientPacket(CMSG_UNLEARN_SKILL, std::move(packet)) { }

            void Read() override;

            uint32 SkillId = 0;
        };

        class RequestResearchHistory final : public ClientPacket
        {
        public:
            RequestResearchHistory(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_RESEARCH_HISTORY, std::move(packet)) { }

            void Read() override { }
        };
    }
}

#endif // TalentPackets_h__
