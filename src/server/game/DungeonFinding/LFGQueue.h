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

#ifndef SF_LFGQUEUE_H
#define SF_LFGQUEUE_H

#include "LFG.h"
#include "Util.h"
#include <random>
#include <numeric>

#define LFG_QUEUE_PROFILING

namespace lfg
{

class Bucket;
class DungeonQueue;
class QueueManager;
struct LfgProposal;

class Queuer
{
public:
    enum class Compatibility : uint8
    {
        OK,
        TooManyPlayers,
        MultipleLfgGroups,
        HasIgnores,
    };

    Queuer() : m_queueId(0) { }
    Queuer(ObjectGuid guid, uint32 queueId) : m_guid(guid), m_queueId(queueId) { }

    ObjectGuid GetGUID() const { return m_guid; }
    uint32 GetQeueueId() const { return m_queueId; }

    bool IsGroup() const { return GetGUID().IsGroup(); }
    bool IsPlayer() const { return !IsGroup(); }
    bool IsLFGGroup() const;
    uint32 GetPlayersCount() const;

    LfgGuidSet const& GetGroupPlayers() const;
    bool Contains(ObjectGuid playerGUID) const;

    Compatibility CheckCompatibilityWith(Queuer const& other, DungeonQueue* queue) const;

    void OutDebug(std::ostringstream& ss, QueueManager const* manager, Queuer const group = {}) const;

    operator bool() const { return m_guid != 0; }
    bool operator==(Queuer const& other) const { return m_guid == other.m_guid && m_queueId == other.m_queueId; }

private:
    ObjectGuid m_guid;
    uint32 m_queueId;
};

class Bucket
{
public:
    using RoleContainer = std::vector<Queuer>;
    using QueuerContainer = std::vector<Queuer>; // Not a set because we want to preserve order
    using QueuerRoleMap = std::map<ObjectGuid, LfgRoles>;

    Bucket(DungeonQueue* queue);

    RoleContainer const& GetContainer(LfgRoles role) const
    {
        if (role == PLAYER_ROLE_TANK) return m_tanks;
        if (role == PLAYER_ROLE_HEALER) return m_healers;
        if (role == PLAYER_ROLE_DAMAGE) return m_damage;
        ASSERT(false);
        return m_damage;
    }
    uint32 GetTotalSlots(LfgRoles role) const;
    uint32 GetTotalSlots() const;
    uint32 GetUsedSlots(LfgRoles role) const { return GetContainer(role).size(); }
    uint32 GetRemainingSlots(LfgRoles role) const { return GetTotalSlots(role) - GetUsedSlots(role); }
    uint32 GetRemainingSlots() const { return GetRemainingSlots(PLAYER_ROLE_TANK) + GetRemainingSlots(PLAYER_ROLE_HEALER) + GetRemainingSlots(PLAYER_ROLE_DAMAGE); }

    QueuerContainer const& GetQueuers() const { return m_uniqueQueuers; }
    QueuerRoleMap const& GetRoles() const { return m_queuerPlayerRoles; }

    bool IsEmpty() const { return m_uniqueQueuers.empty(); }
    bool IsFull() const { return GetRemainingSlots() == 0; }

    bool IsDirty() const { return m_dirty; }
    void SetDirty() { m_dirty = true; }
    void SetClean() { m_dirty = false; }

    bool Contains(Queuer const& queuer) const { return std::find(m_uniqueQueuers.begin(), m_uniqueQueuers.end(), queuer) != m_uniqueQueuers.end(); }
    bool CanAdd(Queuer const& queuer) const;

    void Add(Queuer const& queuer);
    bool Remove(Queuer const& queuer)
    {
        if (!Contains(queuer))
            return false;

        m_uniqueQueuers.erase(std::remove(m_uniqueQueuers.begin(), m_uniqueQueuers.end(), queuer), m_uniqueQueuers.end());
        if (queuer.IsPlayer())
            m_queuerPlayerRoles.erase(queuer.GetGUID());
        else for (auto&& guid : queuer.GetGroupPlayers())
            m_queuerPlayerRoles.erase(guid);

        m_tanks  .erase(std::remove(m_tanks  .begin(), m_tanks  .end(), queuer), m_tanks  .end());
        m_healers.erase(std::remove(m_healers.begin(), m_healers.end(), queuer), m_healers.end());
        m_damage .erase(std::remove(m_damage .begin(), m_damage .end(), queuer), m_damage .end());

        SetDirty();
        return true;
    }

    void OutDebug(std::ostringstream& ss, bool client) const;

private:
    RoleContainer m_tanks, m_healers, m_damage;
    QueuerContainer m_uniqueQueuers;
    QueuerRoleMap m_queuerPlayerRoles;
    bool m_dirty = false;
    DungeonQueue* m_queue;

    bool CanAdd(Queuer const& queuer, LfgRolesMap& assignedRoles) const;
    void Add(LfgRoles role, Queuer const& queuer, ObjectGuid playerGUID)
    {
        if (role == PLAYER_ROLE_TANK) m_tanks.push_back(queuer);
        else if (role == PLAYER_ROLE_HEALER) m_healers.push_back(queuer);
        else if (role == PLAYER_ROLE_DAMAGE) m_damage.push_back(queuer);
        else ASSERT(false && "Bucket::Add: Invalid role");

        if (!Contains(queuer))
            m_uniqueQueuers.push_back(queuer);

        m_queuerPlayerRoles[playerGUID] = role;
    }
};

class LfgShortageData
{
public:
    void Reset() { m_shortageRoles = 0; }

    void Update(uint32 tanks, uint32 healers, uint32 dps, bool raid)
    {
        m_shortageRoles = PLAYER_ROLE_NONE;

        const float idealTanksRatio = raid ? (2.0f / 25) : (1.0f / 5);
        const float idealHealersRatio = raid ? (6.0f / 25) : (1.0f / 5);
        const float idealDPSRatio = raid ? (17.0f / 25) : (3.0f / 5);

        uint32 total = tanks + healers + dps;

        float tanksRatio = (float)tanks / total;
        float healersRatio = (float)healers / total;
        float dpsRatio = (float)dps / total;

        if (tanksRatio < idealTanksRatio * GetShortagePercent())
            m_shortageRoles |= PLAYER_ROLE_TANK;
        if (healersRatio < idealHealersRatio * GetShortagePercent())
            m_shortageRoles |= PLAYER_ROLE_HEALER;
        if (dpsRatio < idealDPSRatio * GetShortagePercent())
            m_shortageRoles |= PLAYER_ROLE_DAMAGE;
    }

    LfgRoles GetShortageRoles() const
    {
        return LfgRoles(m_shortageRoles);
    }

private:
    uint8 m_shortageRoles = 0;
};

class DungeonQueue
{
public:
    using BucketList = std::list<Bucket>;
    using ProposalList = std::list<LfgProposal>;
    struct WaitTime
    {
        time_t Time = 0;    // Total wait time in seconds
        uint32 Players = 0; // Total players spent time waiting

        time_t operator()() const { return Time / std::max(Players, 1u); }
        bool operator<(WaitTime const& other) const { return (*this)() < other(); }
        void operator+=(time_t time) { Time += time; ++Players; }
    };

    DungeonQueue(QueueManager* manager, uint32 dungeonID, uint32 tanksNeeded, uint32 healersNeeded, uint32 damageNeeded, bool raid) : m_manager(manager), m_dungeonID(dungeonID), m_tankSlots(tanksNeeded), m_healerSlots(healersNeeded), m_damageSlots(damageNeeded), m_raid(raid) { }

    QueueManager* GetManager() const { return m_manager; }

    uint32 GetDungeonID() const { return m_dungeonID; }

    uint32 GetRoleSlots(LfgRoles role) const
    {
        if (role == PLAYER_ROLE_TANK) return m_tankSlots;
        if (role == PLAYER_ROLE_HEALER) return m_healerSlots;
        if (role == PLAYER_ROLE_DAMAGE) return m_damageSlots;
        ASSERT(false);
        return 0;
    }
    uint32 GetGroupSize() const { return m_tankSlots + m_healerSlots + m_damageSlots; }

    WaitTime const& GetWaitTime(LfgRoles roles) const;
    void UpdateWaitTime(LfgRoles role, time_t waitTime);

    BucketList const& GetBuckets() const { return m_buckets; }

    bool Contains(Queuer const& queuer) const { return std::any_of(m_buckets.begin(), m_buckets.end(), [&queuer](Bucket const& bucket) { return bucket.Contains(queuer); }); }
    uint32 GetTotalPlayers(LfgRoles role) const { return std::accumulate(m_buckets.begin(), m_buckets.end(), 0u, [role](uint32 sum, Bucket const& bucket) { return sum + bucket.GetUsedSlots(role); }); }

    void Enqueue(Queuer const& queuer, bool inFront);
    bool Dequeue(Queuer const& queuer);

    void PerformBackfill();

    void MakeProposals(ProposalList& proposals);

    void OutDebug(std::ostringstream& ss, bool client) const;

    bool IsRaid() const { return m_raid; }

private:
    QueueManager* m_manager;
    uint32 m_dungeonID;
    uint32 m_tankSlots, m_healerSlots, m_damageSlots;
    WaitTime m_tankWait, m_healerWait, m_damageWait, m_averageWait;
    BucketList m_buckets;
    bool m_raid;
};

class QueueManager
{
public:
    using DungeonQueueMap = std::map<uint32, DungeonQueue>;
    struct QueuerData
    {
        ObjectGuid GUID;
        uint32 QueueId;
        time_t JoinTime;
        LfgDungeonSet Dungeons;
        LfgRolesMap Roles;
        LfgGuidSet Players;
        LfgGuidSet Ignores;

        QueuerData(ObjectGuid guid, uint32 queueId, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& roles);
    };
    using QueuerDataMap = std::multimap<ObjectGuid, QueuerData>;
    using RandomSeed = std::random_device;
    using RandomGenerator = std::mt19937;
    enum class ProfilingType
    {
        AddToQueue,
        RemoveFromQueue,
        MakeProposals,
        UpdateQueueTimers,
        Max,
    };
    struct ProfilingData
    {
        uint64 TotalMicroseconds = 0;
        uint32 TotalQueuers = 0;
        uint32 TotalProfiles = 0;
    };
    using ProfilingDataContainer = std::vector<ProfilingData>;

    QueueManager();

    DungeonQueue const& GetQueue(uint32 dungeonID) const
    {
        auto itr = m_queues.find(dungeonID);
        ASSERT(itr != m_queues.end());
        return itr->second;
    }
    bool Contains(Queuer const& queuer) const { return std::any_of(m_queues.begin(), m_queues.end(), [&queuer](DungeonQueueMap::const_reference queue) { return queue.second.Contains(queuer); }); }
    template<typename Container> void AddToQueue(Container const& queuers, bool inFront);
    template<typename Container> void RemoveFromQueue(Container const& queuers);

    QueuerDataMap const& GetQueuerData() const { return m_queuerData; }
    QueuerData const& GetQueuerData(Queuer const& queuer) const;
    void AddQueueData(Queuer const& queuer, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& rolesMap, bool isContinue);
    void RemoveQueueData(Queuer const& queuer);

    bool IsDirty() const { return m_dirty; }
    void SetDirty() { m_dirty = true; }
    void SetClean() { m_dirty = false; }

    void MakeProposals();

    void UpdateWaitTime(LfgRoles role, time_t waitTime, uint32 dungeonID);
    void UpdateQueueTimers(time_t time);

    void UpdateShortageData();
    LfgRoles GetShortageRoles(uint32 dungeonId) const;
    LfgRoles GetRolesForCTAReward(uint32 dungeonId, LfgRoles roles);

    void OutDebug(std::ostringstream& ss, uint32 dungeonID, bool client) const;

    ProfilingData const& GetProfilingData(ProfilingType type) const { return m_profiling[(uint32)type]; }
    void RecordProfilingData(ProfilingType type, uint32 us, uint32 queuers)
    {
        ProfilingData& data = m_profiling[(uint32)type];
        data.TotalMicroseconds += us;
        data.TotalQueuers += queuers;
        ++data.TotalProfiles;
    }

    void SavePrevState()
    {
        m_queuesPrev = m_queues;
        m_queuerDataPrev = m_queuerData;
    }

private:
    DungeonQueueMap m_queues;
    QueuerDataMap m_queuerData;
    RandomSeed m_device;
    RandomGenerator m_random{ m_device() };
    bool m_dirty = false;
    ProfilingDataContainer m_profiling{ (uint32)ProfilingType::Max };
    std::map<uint32, LfgShortageData> m_shortageData;

    DungeonQueueMap m_queuesPrev;
    QueuerDataMap m_queuerDataPrev;
};

template<typename Container>
inline void QueueManager::AddToQueue(Container const& queuers, bool inFront)
{
#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    std::set<DungeonQueue*> affectedQueues;
    for (auto&& queuer : queuers)
    {
        for (auto&& dungeon : GetQueuerData(queuer).Dungeons)
        {
            auto itr = m_queues.find(dungeon);
            ASSERT(itr != m_queues.end());
            itr->second.Enqueue(queuer, inFront);
            if (inFront)
                affectedQueues.insert(&itr->second);
        }
    }

    for (auto&& queue : affectedQueues)
        queue->PerformBackfill();

    SetDirty();

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::AddToQueue, sw.GetMicroSec(), queuers.size());
#endif
}

template<>
inline void QueueManager::AddToQueue(Queuer const& queuer, bool inFront)
{
#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    for (auto&& dungeon : GetQueuerData(queuer).Dungeons)
    {
        auto itr = m_queues.find(dungeon);
        ASSERT(itr != m_queues.end());
        itr->second.Enqueue(queuer, inFront);
        if (inFront)
            itr->second.PerformBackfill();
    }

    SetDirty();

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::AddToQueue, sw.GetMicroSec(), 1);
#endif
}

template<typename Container>
inline void QueueManager::RemoveFromQueue(Container const& queuers)
{
#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    std::set<DungeonQueue*> affectedQueues;
    for (auto&& queuer : queuers)
    {
        for (auto&& queue : m_queues)
            if (queue.second.Dequeue(queuer))
                affectedQueues.insert(&queue.second);

        RemoveQueueData(queuer);
    }

    for (auto&& queue : affectedQueues)
        queue->PerformBackfill();

    SetDirty();

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::RemoveFromQueue, sw.GetMicroSec(), queuers.size());
#endif
}

template<>
inline void QueueManager::RemoveFromQueue(Queuer const& queuer)
{
#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    for (auto&& queue : m_queues)
        if (queue.second.Dequeue(queuer))
            queue.second.PerformBackfill();

    RemoveQueueData(queuer);

    SetDirty();

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::RemoveFromQueue, sw.GetMicroSec(), 1);
#endif
}

} // namespace lfg

#endif
