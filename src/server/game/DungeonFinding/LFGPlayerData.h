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

#ifndef SF_LFGPLAYERDATA_H
#define SF_LFGPLAYERDATA_H

#include "LFGDataOwner.h"

namespace lfg
{

struct PlayerQueueData
{
    LfgDungeonSet Dungeons;
    ObjectGuid OriginalGroup = ObjectGuid::Empty;
    ObjectGuid LfgGroup = ObjectGuid::Empty;
    time_t JoinTime = 0;
    uint32 ClientQueueId = 0;
    LfgState State = LFG_STATE_NONE;
    LfgState OldState = LFG_STATE_NONE;
    LfgRoles Roles = PLAYER_ROLE_NONE;
    LfgRoles CTARoles = PLAYER_ROLE_NONE;
    LfgCategory Category = LFG_CATEGORY_NONE;
    uint32 RandomDungeon = 0;
};

typedef std::map<uint32, PlayerQueueData> PlayerQueueDataMap;

/**
    Stores all lfg data needed about the player.
*/
class LfgPlayerData : public LfgDataOwner<PlayerQueueData>
{
    public:
        // General
        void SetLockedDungeons(LfgLockMap const& lock);
        void SetTeam(uint8 team);
        void SetGroup(uint32 queueId, ObjectGuid group);

        // Queue
        void SetRoles(uint32 queuId, LfgRoles roles);
        void SetSelectedDungeons(uint32 queueId, const LfgDungeonSet& dungeons);

        // General
        LfgLockMap const& GetLockedDungeons() const;
        uint8 GetTeam() const;
        ObjectGuid GetGroup(uint32 queueId) const;
        void SetJoinTime(uint32 queueId, time_t time);
        uint32 GetRandomDungeon(uint32 queueId) const;
        void SetRandomDungeon(uint32 queueId, uint32 dungeon);
        void SetCTARoles(uint32 queueId, LfgRoles roles);
        LfgRoles GetCTARoles(uint32 queueId) const;

        // Queue
        uint8 GetRoles(uint32 queueId) const;
        LfgDungeonSet const& GetSelectedDungeons(uint32 queueId) const;

        // Boot
        bool CanInitBoot(uint32& timeLeft) const;
        bool CanBeBooted(uint32& timeLeft) const;
        void IncrementBootCounter();
        void MarkLastBootTime();

        void AddQueue(uint32 queueId, ObjectGuid originalGroup);
        void ReformQueue(uint32 oldQueueId, uint32 newQueueId);

    private:
        // Player
        LfgLockMap m_lockedDungeons;                       ///< Dungeons player can't do and reason
        uint8 m_team = 0;                                  ///< Player team - determines the queue to join

        // Boot
        uint32 m_bootCounter = 0;
        uint32 m_bootCounterResetTime = 0;
        uint32 m_bootCooldownResetTime = 0;
        uint32 m_lastBootAttemptTime = 0;
};

} // namespace lfg

#endif
