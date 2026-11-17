/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "LFG.h"
#include "LFGGroupData.h"

namespace lfg
{

void LfgGroupData::AddPlayer(ObjectGuid guid)
{
    m_players.insert(guid);
}

uint8 LfgGroupData::RemovePlayer(ObjectGuid guid)
{
    LfgGuidSet::iterator it = m_players.find(guid);
    if (it != m_players.end())
        m_players.erase(it);
    return uint8(m_players.size());
}

void LfgGroupData::RemoveAllPlayers()
{
    m_players.clear();
}

void LfgGroupData::SetLeader(ObjectGuid guid)
{
    m_leader = guid;
}

void LfgGroupData::SetDungeon(uint32 dungeon)
{
    m_dungeon = dungeon;
}

void LfgGroupData::SetKicksLeft(uint8 kicksLeft)
{
    m_kicksLeft = kicksLeft;
}

void LfgGroupData::DecreaseKicksLeft()
{
    if (m_kicksLeft)
      --m_kicksLeft;
}

LfgGuidSet const& LfgGroupData::GetPlayers() const
{
    return m_players;
}

uint8 LfgGroupData::GetPlayerCount() const
{
    return m_players.size();
}

ObjectGuid LfgGroupData::GetLeader() const
{
    return m_leader;
}

uint32 LfgGroupData::GetDungeon(bool asId /* = true */) const
{
    if (asId)
        return (m_dungeon & 0x00FFFFFF);
    else
        return m_dungeon;
}

uint8 LfgGroupData::GetKicksLeft() const
{
    return m_kicksLeft;
}

void LfgGroupData::SetSoloJoinedPlayersCount(uint8 count)
{
    m_soloJoinedPlayersCount = count;
}

uint8 LfgGroupData::GetSoloJoinedPlayersCount() const
{
    return m_soloJoinedPlayersCount;
}

void LfgGroupData::AddQueue(uint32 queueId)
{
    auto itr = m_queues.find(queueId);
    ASSERT(itr == m_queues.end());
    m_queues.emplace(queueId, GroupQueueData{});
}

} // namespace lfg
