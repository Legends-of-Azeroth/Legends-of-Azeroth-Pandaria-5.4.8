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

#ifndef SF_BATTLEGROUNDQUEUE_H
#define SF_BATTLEGROUNDQUEUE_H

#include "Common.h"
#include "DBCEnums.h"
#include "Battleground.h"
#include "EventProcessor.h"
#include <deque>

//this container can't be deque, because deque doesn't like removing the last element - if you remove it, it invalidates next iterator and crash appears
typedef std::list<Battleground*> BGFreeSlotQueueContainer;

#define COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME 10

enum class SoloQueueRole
{
    None,
    Melee,
    Ranged,
    Healer,
    Tank,
};

struct GroupQueueInfo;                                      // type predefinition
struct PlayerQueueInfo                                      // stores information for players in queue
{
    uint32  LastOnlineTime;                                 // for tracking and removing offline players from queue after 5 minutes
    GroupQueueInfo* GroupInfo;                             // pointer to the associated groupqueueinfo
    SoloQueueRole Role = SoloQueueRole::None;               // Only for solo queue
};

struct GroupQueueInfo                                       // stores information about the group in queue (also used when joined as solo!)
{
    std::map<ObjectGuid, PlayerQueueInfo*> Players;         // player queue info map
    uint32  Team;                                           // Player team (ALLIANCE/HORDE)
    BattlegroundTypeId BgTypeId;                            // battleground type id
    bool    IsRated;                                        // rated
    bool    IsRatedBG;                                      // rated bg
    uint8   ArenaType;                                      // 2v2, 3v3, 5v5 or 0 when BG
    uint32  JoinTime;                                       // time when group was added
    uint32  RemoveInviteTime;                               // time when we will remove invite for players in group
    uint32  IsInvitedToBGInstanceGUID;                      // was invited to certain BG
    uint32  ArenaTeamRating;                                // if rated match, inited to the rating of the team
    uint32  ArenaMatchmakerRating;                          // if rated match, inited to the rating of the team
    uint32  OpponentsTeamRating;                            // for rated arena matches
    uint32  OpponentsMatchmakerRating;                      // for rated arena matches
};

enum BattlegroundQueueGroupTypes
{
    BG_QUEUE_PREMADE_ALLIANCE   = 0,
    BG_QUEUE_PREMADE_HORDE      = 1,
    BG_QUEUE_NORMAL_ALLIANCE    = 2,
    BG_QUEUE_NORMAL_HORDE       = 3
};
#define BG_QUEUE_GROUP_TYPES_COUNT 4

class Battleground;
class BattlegroundQueue
{
    public:
        BattlegroundQueue();
        ~BattlegroundQueue();

        void BattlegroundQueueUpdate(uint32 diff, BattlegroundTypeId bgTypeId, BattlegroundBracketId bracket_id, uint8 arenaType = 0, bool isRated = false, uint32 minRating = 0);
        void UpdateEvents(uint32 diff);

        void FillPlayersToBG(Battleground* bg, BattlegroundBracketId bracket_id);
        bool CheckPremadeMatch(BattlegroundBracketId bracket_id, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam);
        bool CheckNormalMatch(Battleground* bg_template, BattlegroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers);
        bool CheckSkirmishForSameFaction(BattlegroundBracketId bracket_id, uint32 minPlayersPerTeam);
        GroupQueueInfo* AddGroup(Player* leader, Group* group, BattlegroundTypeId bgTypeId, PvPDifficultyEntry const*  bracketEntry, uint8 ArenaType, bool isRated, bool isPremade, uint32 ArenaRating, uint32 MatchmakerRating);
        void RemovePlayer(ObjectGuid guid, bool decreaseInvitedCount, bool leave = false);
        bool IsPlayerInvited(uint64 pl_guid, const uint32 bgInstanceGuid, const uint32 removeTime);
        bool GetPlayerGroupInfoData(uint64 guid, GroupQueueInfo* ginfo);
        void PlayerInvitedToBGUpdateAverageWaitTime(GroupQueueInfo* ginfo, BattlegroundBracketId bracket_id);
        uint32 GetAverageQueueWaitTime(GroupQueueInfo* ginfo, BattlegroundBracketId bracket_id) const;

        typedef std::map<uint64, PlayerQueueInfo> QueuedPlayersMap;
        QueuedPlayersMap m_QueuedPlayers;

        //we need constant add to begin and constant remove / add from the end, therefore deque suits our problem well
        typedef std::list<GroupQueueInfo*> GroupsQueueType;

        /*
        This two dimensional array is used to store All queued groups
        First dimension specifies the bgTypeId
        Second dimension specifies the player's group types -
             BG_QUEUE_PREMADE_ALLIANCE  is used for premade alliance groups and alliance rated arena teams
             BG_QUEUE_PREMADE_HORDE     is used for premade horde groups and horde rated arena teams
             BG_QUEUE_NORMAL_ALLIANCE   is used for normal (or small) alliance groups or non-rated arena matches
             BG_QUEUE_NORMAL_HORDE      is used for normal (or small) horde groups or non-rated arena matches
        */
        GroupsQueueType m_QueuedGroups[MAX_BATTLEGROUND_BRACKETS][BG_QUEUE_GROUP_TYPES_COUNT];

        // class to select and invite groups to bg
        class SelectionPool
        {
        public:
            SelectionPool(): PlayerCount(0) { };
            void Init();
            bool AddGroup(GroupQueueInfo* ginfo, uint32 desiredCount);
            bool KickGroup(uint32 size);
            uint32 GetPlayerCount() const {return PlayerCount;}
        public:
            GroupsQueueType SelectedGroups;
        private:
            uint32 PlayerCount;
        };

        //one selection pool for horde, other one for alliance
        SelectionPool m_SelectionPools[BG_TEAMS_COUNT];
        uint32 GetPlayersInQueue(TeamId id);

    protected:

        bool InviteGroupToBG(GroupQueueInfo* ginfo, Battleground* bg, uint32 side);
        uint32 m_WaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS][COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME];
        uint32 m_WaitTimeLastPlayer[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];
        uint32 m_SumOfWaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];

        // Event handler
        EventProcessor m_events;

        bool m_isSolo = false;
        uint32 m_damagersCount = 0;
        uint32 m_healersCount = 0;
};

struct SoloGroup;

struct SoloPlayer
{
    SoloPlayer(Player* player);

    Player* Target;
    GroupQueueInfo* Info = nullptr;
    SoloGroup* OriginalGroup = nullptr;
    uint8 Class;
    Specializations Specialization;
    SoloQueueRole Role;
};

struct SoloGroup
{
    bool Create(GroupQueueInfo* ginfo);
    void Reset();
    void AddPlayer(SoloPlayer const& p);

    bool InTeam = false;
    uint32 Team;
    std::vector<SoloPlayer> Members;
    std::vector<SoloPlayer> Current;

    uint32 Healer = 0;
    uint32 Melee = 0;
    uint32 Ranged = 0;
    uint32 Tank = 0;
    uint32 Rating = 0;
};

struct SoloTeamSelectionRules : public std::map<uint64, time_t>
{
    void flush()
    {
        time_t now = time(nullptr);
        for (auto it = begin(); it != end();)
        {
            if (it->second < now)
                erase(it++);
            else
                ++it;
        }
    }

    bool has(uint64 guid) const
    {
        auto it = find(guid);
        if (it == end())
            return false;
        return it->second > time(nullptr);
    }
};

class SoloQueue : public BattlegroundQueue
{
public:
    struct PlayerClass
    {
        uint8 Class = 0;
        uint8 Spec = 0;

        bool operator==(PlayerClass const& other) const
        {
            return Class == other.Class && Spec == other.Spec;
        }

        bool operator!=(PlayerClass const& other) const
        {
            return !this->operator==(other);
        }
    };

    struct TeamSetup : public std::array<PlayerClass, 3>
    {
        void sort(size_t size)
        {
            std::sort(begin(), begin() + size, [](SoloQueue::PlayerClass const& lhs, SoloQueue::PlayerClass const& rhs)
            {
                if (lhs.Class == rhs.Class)
                    return lhs.Spec < rhs.Spec;
                return lhs.Class < rhs.Class;
            });
        }
    };

    typedef std::vector<SoloGroup*>::iterator iterator;

public:
    SoloQueue();

    bool IsCompatible(SoloGroup const* lhs, SoloGroup const* rhs, bool strong);
    bool MakeTeams(SoloGroup* group, SoloGroup* opponent, bool strong);
    bool Search(SoloGroup* group, uint32 desired, bool strong);

    void DoMatchmaking(GroupQueueInfo* ginfo, uint32 extraDiff, bool strong);
    void DynamicUpdate();
    void GetQueuedPlayersCount(uint32& dd, uint32& healers);
    void SetQueuedPlayersCount(uint32 dd, uint32 healers);
    void SetBlacklist(std::vector<TeamSetup> const& blist);
    SoloTeamSelectionRules& GetRuleFor(uint64 guid)
    {
        std::lock_guard<std::mutex> lock(m_rulesMutex);
        return m_rules[guid];
    }
    static uint32 TeamSize();

    static SoloQueueRole GetRole(Specializations spec);
    static BattlegroundBracketId GetSoloQueueBracketId();

private:
    std::ostringstream m_buff;
    uint32 m_dynamicUpdateCount = 0;
    std::vector<SoloGroup>  m_all;
    std::vector<SoloGroup*> m_selection;
    std::vector<size_t> m_opponents;
    uint32 m_teamDiff;

    std::vector<TeamSetup> m_disabled;

    std::mutex m_rulesMutex;
    std::unordered_map<uint32, SoloTeamSelectionRules> m_rules;
};

/*
    This class is used to invite player to BG again, when minute lasts from his first invitation
    it is capable to solve all possibilities
*/
class BGQueueInviteEvent : public BasicEvent
{
    public:
        BGQueueInviteEvent(ObjectGuid pl_guid, uint32 BgInstanceGUID, BattlegroundTypeId BgTypeId, uint8 arenaType, uint32 removeTime) :
          m_PlayerGuid(pl_guid), m_BgInstanceGUID(BgInstanceGUID), m_BgTypeId(BgTypeId), m_ArenaType(arenaType), m_RemoveTime(removeTime)
          { }
        virtual ~BGQueueInviteEvent() { }

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        ObjectGuid m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        BattlegroundTypeId m_BgTypeId;
        uint8  m_ArenaType;
        uint32 m_RemoveTime;
};

/*
    This class is used to remove player from BG queue after 1 minute 20 seconds from first invitation
    We must store removeInvite time in case player left queue and joined and is invited again
    We must store bgQueueTypeId, because battleground can be deleted already, when player entered it
*/
class BGQueueRemoveEvent : public BasicEvent
{
    public:
        BGQueueRemoveEvent(ObjectGuid pl_guid, uint32 bgInstanceGUID, BattlegroundTypeId BgTypeId, BattlegroundQueueTypeId bgQueueTypeId, uint32 removeTime)
            : m_PlayerGuid(pl_guid), m_BgInstanceGUID(bgInstanceGUID), m_RemoveTime(removeTime), m_BgTypeId(BgTypeId), m_BgQueueTypeId(bgQueueTypeId)
        { }

        virtual ~BGQueueRemoveEvent() { }

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        ObjectGuid m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        uint32 m_RemoveTime;
        BattlegroundTypeId m_BgTypeId;
        BattlegroundQueueTypeId m_BgQueueTypeId;
};

#endif
