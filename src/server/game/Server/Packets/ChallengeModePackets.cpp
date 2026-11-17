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

#include "ChallengeModePackets.h"

void WorldPackets::ChallengeMode::GetChallengeModeRewards::Read()
{
}

void WorldPackets::ChallengeMode::ChallengeModeRequestLeaders::Read()
{
    MapId = _worldPacket.read<uint32>();
    _worldPacket.read_skip<uint32>(); // Last Guild update
    _worldPacket.read_skip<uint32>(); // Last Realm update
}

void WorldPackets::ChallengeMode::ChallengeModeRequestMapStats::Read()
{
}

void WorldPackets::ChallengeMode::ResetChallengeMode::Read()
{
}
