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

#ifndef ChallengeModePackets_h__
#define ChallengeModePackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace ChallengeMode
    {
        class GetChallengeModeRewards final : public ClientPacket
        {
        public:
            GetChallengeModeRewards(WorldPacket&& packet) : ClientPacket(CMSG_GET_CHALLENGE_MODE_REWARDS, std::move(packet)) { }

            void Read() override;
        };

        class ChallengeModeRequestLeaders final : public ClientPacket
        {
        public:
            ChallengeModeRequestLeaders(WorldPacket&& packet) : ClientPacket(CMSG_CHALLENGE_MODE_REQUEST_LEADERS, std::move(packet)) { }

            void Read() override;

            uint32 MapId = 0;
        };

        class ChallengeModeRequestMapStats final : public ClientPacket
        {
        public:
            ChallengeModeRequestMapStats(WorldPacket&& packet) : ClientPacket(CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS, std::move(packet)) { }

            void Read() override;
        };

        class ResetChallengeMode final : public ClientPacket
        {
        public:
            ResetChallengeMode(WorldPacket&& packet) : ClientPacket(CMSG_RESET_CHALLENGE_MODE, std::move(packet)) { }

            void Read() override;
        };
    }
}

#endif // ChallengeModePackets_h__
