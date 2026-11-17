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

#include "LFGPlayerData.h"

namespace lfg
{

void LfgPlayerData::SetLockedDungeons(LfgLockMap const& lockStatus)
{
    m_lockedDungeons = lockStatus;
}

void LfgPlayerData::SetTeam(uint8 team)
{
    m_team = team;
}

void LfgPlayerData::SetGroup(uint32 queueId, ObjectGuid group)
{
    auto& queueData = GetQueueData(queueId);
    queueData.LfgGroup = group;
}

void LfgPlayerData::SetRoles(uint32 queueId, LfgRoles roles)
{
    auto& queueData = GetQueueData(queueId);
    // Roles can be overridden if we queue lfg group to continue dungeon
    queueData.Roles = roles;
}

void LfgPlayerData::SetSelectedDungeons(uint32 queueId, LfgDungeonSet const& dungeons)
{
    auto& queueData = GetQueueData(queueId);
    ASSERT(queueData.Dungeons.empty());
    queueData.Dungeons = dungeons;
    queueData.Category = LfgCategory(sLFGDungeonStore.LookupEntry(*dungeons.begin())->category);
}

const LfgLockMap& LfgPlayerData::GetLockedDungeons() const
{
    return m_lockedDungeons;
}

uint8 LfgPlayerData::GetTeam() const
{
    return m_team;
}

ObjectGuid LfgPlayerData::GetGroup(uint32 queueId) const
{
    return GetQueueData(queueId).LfgGroup;
}

void LfgPlayerData::SetJoinTime(uint32 queueId, time_t time)
{
    auto& queueData = GetQueueData(queueId);
    ASSERT(!queueData.JoinTime);
    queueData.JoinTime = time;
}

uint32 LfgPlayerData::GetRandomDungeon(uint32 queueId) const
{
    return GetQueueData(queueId).RandomDungeon;
}

void LfgPlayerData::SetRandomDungeon(uint32 queueId, uint32 dungeon)
{
    auto& queueData = GetQueueData(queueId);
    ASSERT(!queueData.RandomDungeon);
    queueData.RandomDungeon = dungeon;
}

void LfgPlayerData::SetCTARoles(uint32 queueId, LfgRoles roles)
{
    auto& queueData = GetQueueData(queueId);
    ASSERT(queueData.CTARoles == PLAYER_ROLE_NONE);
    queueData.CTARoles = roles;
}

LfgRoles LfgPlayerData::GetCTARoles(uint32 queueId) const
{
    return GetQueueData(queueId).CTARoles;
}

uint8 LfgPlayerData::GetRoles(uint32 queueId) const
{
    return GetQueueData(queueId).Roles;
}

LfgDungeonSet const& LfgPlayerData::GetSelectedDungeons(uint32 queueId) const
{
    return GetQueueData(queueId).Dungeons;
}

bool LfgPlayerData::CanInitBoot(uint32& timeLeft) const
{
    uint32 now = getMSTime();

    if (!m_bootCooldownResetTime || now > m_bootCooldownResetTime)
        return true;

    timeLeft = m_bootCooldownResetTime - now;
    return false;
}

bool LfgPlayerData::CanBeBooted(uint32& timeLeft) const
{
    uint32 now = getMSTime();
    uint32 allowedTime = m_lastBootAttemptTime + 5 * 60 * IN_MILLISECONDS; // Allow next boot attempt of this player in 5 minutes

    if (!m_lastBootAttemptTime || now > allowedTime)
        return true;

    timeLeft = allowedTime - now;
    return false;
}

void LfgPlayerData::IncrementBootCounter()
{
    uint32 now = getMSTime();

    if (now < m_bootCooldownResetTime)
        return;

    if (now > m_bootCounterResetTime) // If reset time has passed - reset the boot counter
        m_bootCounter = 0;

    m_bootCounterResetTime = now + 10 * 60 * IN_MILLISECONDS; // Reset in 10 minutes

    if (++m_bootCounter >= 5) // If more than 5 boots were initiated within 10 minutes of each other
        m_bootCooldownResetTime = now + 2 * 60 * 60 * IN_MILLISECONDS; // Disallow boot for 2 hours
}

void LfgPlayerData::MarkLastBootTime()
{
    m_lastBootAttemptTime = getMSTime();
}

void LfgPlayerData::AddQueue(uint32 queueId, ObjectGuid originalGroup)
{
    auto itr = m_queues.find(queueId);
    ASSERT(itr == m_queues.end());
    auto& queueData = m_queues[queueId];
    queueData.ClientQueueId = queueId;
    queueData.OriginalGroup = std::move(originalGroup);
}

void LfgPlayerData::ReformQueue(uint32 oldQueueId, uint32 newQueueId)
{
    // Copy
    PlayerQueueData queueData = GetQueueData(oldQueueId);
    m_queues.erase(oldQueueId);
    m_queues.emplace(newQueueId, queueData);

    OnStateChanged();
}

} // namespace lfg
