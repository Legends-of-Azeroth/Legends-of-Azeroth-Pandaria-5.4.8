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

#ifndef SF_LFG_H
#define SF_LFG_H

#include "Common.h"

enum
{
    QUEST_LFG_SHORTAGE = 32097,
    QUEST_LFR_SHORTAGE = 33233,
};

namespace lfg
{
enum LfgRoles
{
    PLAYER_ROLE_NONE                             = 0x00,
    PLAYER_ROLE_LEADER                           = 0x01,
    PLAYER_ROLE_TANK                             = 0x02,
    PLAYER_ROLE_HEALER                           = 0x04,
    PLAYER_ROLE_DAMAGE                           = 0x08
};

enum LfgUpdateType : uint32
{
    LFG_UPDATETYPE_DEFAULT                       = 0,      // Internal Use
    LFG_UPDATETYPE_LEADER_UNK1                   = 1,      // FIXME: At group leave
    LFG_UPDATETYPE_ROLECHECK_ABORTED             = 4,
    LFG_UPDATETYPE_JOIN_QUEUE                    = 6,
    LFG_UPDATETYPE_ROLECHECK_FAILED              = 7,
    LFG_UPDATETYPE_REMOVED_FROM_QUEUE            = 8,
    LFG_UPDATETYPE_PROPOSAL_FAILED               = 9,
    LFG_UPDATETYPE_PROPOSAL_DECLINED             = 10,
    LFG_UPDATETYPE_GROUP_FOUND                   = 11,
    LFG_UPDATETYPE_ADDED_TO_QUEUE                = 13,
    LFG_UPDATETYPE_PROPOSAL_BEGIN                = 14,
    LFG_UPDATETYPE_UPDATE_STATUS                 = 15,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE          = 16,
    LFG_UPDATETYPE_GROUP_DISBAND_UNK16           = 17,     // FIXME: Sometimes at group disband
    LFG_UPDATETYPE_JOIN_QUEUE_INITIAL            = 24,
    LFG_UPDATETYPE_DUNGEON_FINISHED              = 25,
    LFG_UPDATETYPE_PARTY_ROLE_NOT_AVAILABLE      = 43,
    LFG_UPDATETYPE_JOIN_LFG_OBJECT_FAILED        = 45,
    LFG_UPDATETYPE_UNK_52                        = 52,
};

enum LfgState
{
    LFG_STATE_NONE,                                        // Not using LFG / LFR
    LFG_STATE_ROLECHECK,                                   // Rolecheck active
    LFG_STATE_QUEUED,                                      // Queued
    LFG_STATE_PROPOSAL,                                    // Proposal active
    LFG_STATE_BOOT,                                        // Vote kick active
    LFG_STATE_DUNGEON,                                     // In LFG Group, in a Dungeon
    LFG_STATE_FINISHED_DUNGEON,                            // In LFG Group, in a finished Dungeon
};

/// Instance lock types
enum LfgLockStatusType
{
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION        = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL                 = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL                = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE            = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE           = 5,
    LFG_LOCKSTATUS_RAID_LOCKED                   = 6,
    LFG_LOCKSTATUS_INCOMPATIBLE_FACTION          = 10,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL      = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL     = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED           = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM                  = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON                 = 1031,
    LFG_LOCKSTATUS_MISSING_ACHIEVEMENT           = 1034
};

/// Answer state (Also used to check compatibilites)
enum LfgAnswer
{
    LFG_ANSWER_PENDING                           = -1,
    LFG_ANSWER_DENY                              = 0,
    LFG_ANSWER_AGREE                             = 1
};

/// Role Shortage Index (Seems to only use LFG_ROLE_SHORTAGE_RARE on live servers)
enum LfgShortageIndex
{
    LFG_ROLE_SHORTAGE_RARE                       = 0, // Gold icon
    LFG_ROLE_SHORTAGE_UNCOMMON                   = 1, // Silver icon
    LFG_ROLE_SHORTAGE_PLENTIFUL                  = 2, // Bronze icon
    LFG_ROLE_SHORTAGE_MAX
};

enum LfgCategory
{
    LFG_CATEGORY_NONE                            = 0,
    LFG_CATEGORY_DUNGEON                         = 1,
    LFG_CATEGORY_LFR                             = 2,
    LFG_CATEGORY_SCENARIO                        = 3,
    LFG_CATEGORY_FLEX                            = 4,
};

struct LfgLockInfoData
{
    LfgLockInfoData(uint32 _lockStatus = 0, uint16 _requiredItemLevel = 0, float _currentItemLevel = 0) :
        lockStatus(_lockStatus), requiredItemLevel(_requiredItemLevel), currentItemLevel(_currentItemLevel) { }

    uint32 lockStatus;
    uint16 requiredItemLevel;
    float currentItemLevel;
};

typedef std::set<uint32> LfgDungeonSet;
typedef std::map<uint32, LfgLockInfoData> LfgLockMap;
typedef std::map<ObjectGuid, LfgLockMap> LfgLockPartyMap;
typedef std::set<ObjectGuid> LfgGuidSet;
typedef std::list<ObjectGuid> LfgGuidList;
typedef std::map<ObjectGuid, uint8> LfgRolesMap;
typedef std::map<uint64, uint64> LfgGroupsMap;

std::string ConcatenateDungeons(LfgDungeonSet const& dungeons);
std::string GetRolesString(uint8 roles);
std::string GetStateString(LfgState state);
uint32 GetGroupSize(bool raid);
float GetShortagePercent();

// allow implicit enum to int conversions for formatting
inline int32 format_as(LfgUpdateType e) { return e; }
inline uint8 format_as(LfgState e) { return e; }
inline uint8 format_as(LfgRoles e) { return e; }

} // namespace lfg

#endif
