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

#include "ObjectDefines.h"
#include "Containers.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "Group.h"
#include "LFGQueue.h"
#include "LFGMgr.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Player.h"
#include "SocialMgr.h"
#include "Config.h"

namespace lfg
{
#pragma region Queuer
bool Queuer::IsLFGGroup() const
{
    ASSERT(IsGroup());
    return sLFGMgr->IsLfgGroup(GetGUID());
}

uint32 Queuer::GetPlayersCount() const
{
    return IsGroup() ? sLFGMgr->GetPlayerCount(GetGUID()) : 1;
}

LfgGuidSet const& Queuer::GetGroupPlayers() const
{
    ASSERT(IsGroup());
    return sLFGMgr->GetPlayers(GetGUID());
}

bool Queuer::Contains(ObjectGuid playerGUID) const
{
    ASSERT(playerGUID.IsPlayer());
    if (IsPlayer())
        return GetGUID() == playerGUID;

    LfgGuidSet const players = GetGroupPlayers();
    return players.find(playerGUID) != players.end();
}

Queuer::Compatibility Queuer::CheckCompatibilityWith(Queuer const& other, DungeonQueue* queue) const
{
    // Basic size check
    if (GetPlayersCount() + other.GetPlayersCount() > queue->GetGroupSize())
        return Compatibility::TooManyPlayers;

    if (IsGroup() && other.IsGroup() && IsLFGGroup() && other.IsLFGGroup())
        return Compatibility::MultipleLfgGroups;

    // Check social ignores
    auto&& myData = queue->GetManager()->GetQueuerData(*this);
    auto&& otherData = queue->GetManager()->GetQueuerData(other);
    for (auto&& player : myData.Players)
        if (otherData.Ignores.find(player) != otherData.Ignores.end())
            return Compatibility::HasIgnores;
    for (auto&& player : otherData.Players)
        if (myData.Ignores.find(player) != myData.Ignores.end())
            return Compatibility::HasIgnores;

    return Compatibility::OK;
}
  
void Queuer::OutDebug(std::ostringstream& ss, QueueManager const* manager, Queuer const group) const
{
    if (IsGroup())
    {
        ss << "G-" << GetGUID().GetCounter() << "[";
        uint32 i = 0;
        for (auto&& guid : GetGroupPlayers())
        {
            ss << (i++ ? "||" : "");
            Queuer(guid, GetQeueueId()).OutDebug(ss, manager, *this);
        }
        ss << "]";
    }
    else
    {
        if (ObjectAccessor::FindConnectedPlayer(GetGUID()))
            ss << sWorld->GetCharacterNameData(GetGUID())->m_name;
        else
            ss << "|cFFFF0000" << sWorld->GetCharacterNameData(GetGUID())->m_name << "<OFFLINE>|r";

        if (manager)
        {
            LfgRoles role = (LfgRoles)manager->GetQueuerData(group ? group : *this).Roles.find(GetGUID())->second;
            if (role & PLAYER_ROLE_LEADER) ss << "|TInterface/LFGFrame/UI-LFG-ICON-PORTRAITROLES:0:0:0:0:64:64:0:18:1:19|t";
            if (role & PLAYER_ROLE_TANK) ss << "|TInterface/LFGFrame/UI-LFG-ICON-PORTRAITROLES:0:0:0:0:64:64:0:18:22:40|t";
            if (role & PLAYER_ROLE_HEALER) ss << "|TInterface/LFGFrame/UI-LFG-ICON-PORTRAITROLES:0:0:0:0:64:64:20:38:1:19|t";
            if (role & PLAYER_ROLE_DAMAGE) ss << "|TInterface/LFGFrame/UI-LFG-ICON-PORTRAITROLES:0:0:0:0:64:64:20:38:22:40|t";
        }
    }
}
#pragma endregion

#pragma region Bucket
Bucket::Bucket(DungeonQueue* queue) : m_queue(queue)
{
    m_tanks.reserve(GetTotalSlots(PLAYER_ROLE_TANK));
    m_healers.reserve(GetTotalSlots(PLAYER_ROLE_HEALER));
    m_damage.reserve(GetTotalSlots(PLAYER_ROLE_DAMAGE));
    m_uniqueQueuers.reserve(GetTotalSlots());
}

bool Bucket::CanAdd(Queuer const& queuer) const
{
    LfgRolesMap assignedRoles;
    return CanAdd(queuer, assignedRoles);
}

bool Bucket::CanAdd(Queuer const& queuer, LfgRolesMap& assignedRoles) const
{
    auto roles = m_queue->GetManager()->GetQueuerData(queuer).Roles;

    if (GetRemainingSlots() < roles.size())
        return false;

    for (auto&& q : m_uniqueQueuers)
    {
        if (queuer.CheckCompatibilityWith(q, m_queue) != Queuer::Compatibility::OK)
            return false;

        for (auto&& role : m_queue->GetManager()->GetQueuerData(q).Roles)
            assignedRoles.insert(role);
    }

    for (auto&& role : roles)
        assignedRoles.insert(role);

    Difficulty difficulty = sLFGMgr->GetLFGDungeon(m_queue->GetDungeonID())->difficulty;

    auto check = m_queue->IsRaid() ? LFGMgr::CheckRaidRoles : LFGMgr::CheckGroupRoles;
    if (difficulty == RAID_DIFFICULTY_1025MAN_FLEX || difficulty == SCENARIO_DIFFICULTY_NORMAL || difficulty == SCENARIO_DIFFICULTY_HEROIC)
        check = LFGMgr::CheckDpsOnly;

    if (!check(assignedRoles, m_queue->GetRoleSlots(PLAYER_ROLE_TANK), m_queue->GetRoleSlots(PLAYER_ROLE_HEALER), m_queue->GetRoleSlots(PLAYER_ROLE_DAMAGE)))
    {
        if (sWorld->getBoolConfig(CONFIG_LFG_SOLO))
            return true;
        return false;
    }

    return true;
}

void Bucket::Add(Queuer const& queuer)
{
    LfgRolesMap assignedRoles;
    ASSERT(CanAdd(queuer, assignedRoles));

    auto reassignRoles = [this, &assignedRoles](Queuer const& queuer, ObjectGuid playerGUID)
    {
        switch (m_queuerPlayerRoles[playerGUID] = (LfgRoles)(assignedRoles[playerGUID] & ~PLAYER_ROLE_LEADER))
        {
            case PLAYER_ROLE_TANK:   
                if (sWorld->getBoolConfig(CONFIG_LFG_SOLO))
                    m_damage.push_back(queuer); 
                else
                    m_tanks.push_back(queuer); 
                break;
            case PLAYER_ROLE_HEALER: 
                if (sWorld->getBoolConfig(CONFIG_LFG_SOLO))
                    m_damage.push_back(queuer); 
                else                
                    m_healers.push_back(queuer); 
                break;
            case PLAYER_ROLE_DAMAGE: 
                m_damage.push_back(queuer); 
                break;
            default: ASSERT(false);
                
        }
    };

    // Readd old queuers with new roles
    m_tanks.clear();
    m_healers.clear();
    m_damage.clear();
    for (auto&& q : m_uniqueQueuers)
    {
        if (q.IsPlayer())
            reassignRoles(q, q.GetGUID());
        else for (auto&& guid : q.GetGroupPlayers())
            reassignRoles(q, guid);
    }

    // Add new queuer
    for (auto&& role : m_queue->GetManager()->GetQueuerData(queuer).Roles)
        Add((LfgRoles)(assignedRoles[role.first] & ~PLAYER_ROLE_LEADER), queuer, role.first);
}


void Bucket::OutDebug(std::ostringstream& ss, bool client) const
{
    QueueManager* manager = client ? m_queue->GetManager() : nullptr;
    ss << "   {" << (IsDirty() ? " |cFFFF0000[DIRTY]|r" : "") << "\n";
    ss << "      " << (client ? "|cFF0000FF" : "") << "T" << (client ? "|r" : "") << ": ("; { uint32 i = 0; for (auto&& queuer : m_tanks  ) { ss << (i++ ? ", " : ""); queuer.OutDebug(ss, manager); } } ss << ")\n";
    ss << "      " << (client ? "|cFF00FF00" : "") << "H" << (client ? "|r" : "") << ": ("; { uint32 i = 0; for (auto&& queuer : m_healers) { ss << (i++ ? ", " : ""); queuer.OutDebug(ss, manager); } } ss << ")\n";
    ss << "      " << (client ? "|cFFFF0000" : "") << "D" << (client ? "|r" : "") << ": ("; { uint32 i = 0; for (auto&& queuer : m_damage ) { ss << (i++ ? ", " : ""); queuer.OutDebug(ss, manager); } } ss << ")\n";
    ss << "      " << (client ? "|cFF808080" : "") << "UniqueQueuers" << (client ? "|r" : "") << ": ("; { uint32 i = 0; for (auto&& queuer : m_uniqueQueuers) { ss << (i++ ? ", " : ""); queuer.OutDebug(ss, manager); } } ss << ")\n";
    ss << "      " << (client ? "|cFF808080" : "") << "QueuerPlayerRoles" << (client ? "|r" : "") << ": ("; { uint32 i = 0; for (auto&& role : m_queuerPlayerRoles) { ss << (i++ ? ", " : "") << sWorld->GetCharacterNameData(role.first)->m_name << "=" << (uint32)role.second; } } ss << ")\n";
    ss << "   }\n";
}

uint32 Bucket::GetTotalSlots(LfgRoles role) const
{
    return m_queue->GetRoleSlots(role);
}

uint32 Bucket::GetTotalSlots() const
{
    return m_queue->GetGroupSize();
}
#pragma endregion

#pragma region DungeonQueue
void DungeonQueue::Enqueue(Queuer const& queuer, bool inFront)
{
    ASSERT(!Contains(queuer));

    if (inFront)
    {
        if (m_buckets.empty() || !m_buckets.front().IsDirty() || !m_buckets.front().CanAdd(queuer))
        {
            m_buckets.emplace_front(this);
            m_buckets.front().SetDirty();
        }
    }

    for (auto&& bucket : m_buckets)
    {
        if (bucket.CanAdd(queuer))
        {
            bucket.Add(queuer);

            if (inFront && bucket.IsDirty() && bucket.IsFull()) // Can happen in a full group has been added to front
                bucket.SetClean();

            return;
        }
    }

    m_buckets.emplace_back(this);
    m_buckets.back().Add(queuer);
}

bool DungeonQueue::Dequeue(Queuer const& queuer)
{
    for (auto itr = m_buckets.begin(); itr != m_buckets.end(); ++itr)
    {
        Bucket& bucket = *itr;
        if (bucket.Remove(queuer))
        {
            if (bucket.IsEmpty())
                m_buckets.erase(itr);

            return true;
        }
    }

    return false;
}

void DungeonQueue::PerformBackfill()
{
    for (auto itr = m_buckets.begin(); itr != m_buckets.end(); ++itr)
    {
        Bucket& bucketDest = *itr;
        if (!bucketDest.IsDirty())      // If bucket is not marked as dirty - it didn't lose any queuers and we have no reason to backfill it - it should already be backfilled as much as possible
            continue;

        ASSERT(!bucketDest.IsFull());   // Only those buckets that are missing players should ever be marked as dirty
        ASSERT(!bucketDest.IsEmpty());  // Empty buckets should be outright removed from the list
        bucketDest.SetClean();          // Clear dirty flag now

        // Attempt to backfill current bucket with queuers from following buckets.
        // Bucket order matches queue order. First buckets represent oldest (or high-priority) queuers that should be filled first.
        // By pulling queuers from other buckets, we are marking them as dirty, and they will also be attempted to backfill later on in the loop.
        auto itr2 = itr;
        for (++itr2; itr2 != m_buckets.end();)
        {
            Bucket& bucketSource = *itr2;

            // Queuers in buckets are also stored in oldest-first layout, thus they should have preference when backfilling
            // Copy queuers, as we'll be modifying buckets, invalidating the iteration
            for (auto&& sourceQueuer : Bucket::QueuerContainer{ bucketSource.GetQueuers() })
            {
                // Check if we can add the queuer to our destination bucket
                if (bucketDest.CanAdd(sourceQueuer))
                {
                    // Move the queuer from source bucket to destination bucket
                    bucketSource.Remove(sourceQueuer); // This will mark this bucket as dirty, we might attempt to backfill it later
                    bucketDest.Add(sourceQueuer);
                }
            }

            // Remove now-empty buckets. This is faster than attempting to backfill them
            if (bucketSource.IsEmpty())
                itr2 = m_buckets.erase(itr2);
            else
                ++itr2;

            // Break backfill prematurely if destination bucket is now full - we can make a proposal out of it, no need to backfill it further
            if (bucketDest.IsFull())
                break;
        }
    }
}

DungeonQueue::WaitTime const& DungeonQueue::GetWaitTime(LfgRoles roles) const
{
    switch (roles & ~PLAYER_ROLE_LEADER)
    {
        case PLAYER_ROLE_NONE:   return m_averageWait;
        case PLAYER_ROLE_TANK:   return m_tankWait;
        case PLAYER_ROLE_HEALER: return m_healerWait;
        case PLAYER_ROLE_DAMAGE: return m_damageWait;
        case PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER: return std::min(m_tankWait, m_healerWait);
        case PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE: return std::min(m_healerWait, m_damageWait);
        case PLAYER_ROLE_TANK | PLAYER_ROLE_DAMAGE: return std::min(m_tankWait, m_damageWait);
        case PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE: return std::min(m_tankWait, std::min(m_healerWait, m_damageWait));
        default: ASSERT(false); return m_averageWait;
    }
}


void DungeonQueue::UpdateWaitTime(LfgRoles role, time_t waitTime)
{
    switch (role & ~PLAYER_ROLE_LEADER)
    {
        case PLAYER_ROLE_TANK:   m_tankWait += waitTime; break;
        case PLAYER_ROLE_HEALER: m_healerWait += waitTime; break;
        case PLAYER_ROLE_DAMAGE: m_damageWait += waitTime; break;
        default:                 m_averageWait += waitTime; break;
    }
}

void DungeonQueue::MakeProposals(ProposalList& proposals)
{
    time_t now = time(nullptr);

    for (auto&& bucket : m_buckets)
    {
        if (bucket.IsFull())
        {
            LfgProposal proposal{};

            proposal.isNew = true;
            proposal.dungeonId = GetDungeonID();
            proposal.state = LFG_PROPOSAL_INITIATING;
            proposal.cancelTime = now + LFG_TIME_PROPOSAL;
            proposal.raid = IsRaid();

            for (auto&& player : bucket.GetRoles())
                proposal.players[player.first].role = player.second;

            std::vector<ObjectGuid> potentialLeaders;
            potentialLeaders.reserve(GetGroupSize());
            for (auto&& queuer : bucket.GetQueuers())
            {
                proposal.queuers.push_back(queuer);

                auto&& data = m_manager->GetQueuerData(queuer);
                for (auto&& role : data.Roles)
                    if (role.second & PLAYER_ROLE_LEADER)
                        potentialLeaders.push_back(role.first);

                proposal.weight += now > data.JoinTime ? now - data.JoinTime : 0;

                if (queuer.IsGroup())
                {
                    for (auto&& guid : queuer.GetGroupPlayers())
                    {
                        proposal.players[guid].group = queuer.GetGUID();
                        proposal.players[guid].queueId = queuer.GetQeueueId();
                    }

                    if (queuer.IsLFGGroup())
                    {
                        ASSERT(!proposal.group);
                        proposal.group = queuer.GetGUID();
                        proposal.isNew = sLFGMgr->GetOldState(proposal.group, queuer.GetQeueueId()) != LFG_STATE_DUNGEON;

                        if (!proposal.isNew)
                            for (auto&& guid : queuer.GetGroupPlayers())
                                proposal.players[guid].accept = LFG_ANSWER_AGREE;
                    }
                }
                else
                {
                    proposal.players[queuer.GetGUID()].queueId = queuer.GetQeueueId();
                }
            }

            proposal.leader = potentialLeaders.empty()
                ? Trinity::Containers::SelectRandomContainerElement(proposal.players).first
                : Trinity::Containers::SelectRandomContainerElement(potentialLeaders);

            proposal.weight /= proposal.queuers.size();

            proposals.push_back(proposal);
        }
    }
}

void DungeonQueue::OutDebug(std::ostringstream& ss, bool client) const
{
    if (m_buckets.empty())
        return;

    LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(m_dungeonID);
    ss << "   --- DUNGEON QUEUE --- Dungeon: " << GetDungeonID() << " (" << (dungeon ? dungeon->name[DEFAULT_LOCALE] : "???") << ")" << (dungeon && dungeon->difficulty ? " (Heroic)" : "") << "\n";
    for (auto&& bucket : m_buckets)
        bucket.OutDebug(ss, client);
}
#pragma endregion

#pragma region QueueManager
QueueManager::QueuerData::QueuerData(ObjectGuid guid, uint32 queueId, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& roles)
    : GUID(guid), QueueId(queueId), JoinTime(joinTime), Dungeons(dungeons), Roles(roles)
{
    if (guid.IsPlayer())
    {
        Players.insert(guid);
        if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
            for (auto&& social : player->GetSocial()->GetSocialMap())
                if (social.second.Flags & SOCIAL_FLAG_IGNORED)
                    Ignores.insert(social.first);
    }
    else
    {
        for (auto&& member : sLFGMgr->GetPlayers(guid))
        {
            Players.insert(member);
            if (Player* player = ObjectAccessor::FindConnectedPlayer(member))
                for (auto&& social : player->GetSocial()->GetSocialMap())
                    if (social.second.Flags & SOCIAL_FLAG_IGNORED)
                        Ignores.insert(social.first);
        }
    }
}

const uint32 shortageDungeons[] = { 462, 463,  527, 528,  529, 530,  526,  610, 611, 612, 613,  716, 717, 724, 725 }; // Can add all random dungeons but little use for now

QueueManager::QueueManager()
{
    bool scenarioDebug = sConfigMgr->GetBoolDefault("DungeonFinder.ScenarioDebug", false);

    // Initialize all dungeon queues
    for (uint32 id = 0; id < sLFGDungeonStore.GetNumRows(); ++id)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(id);
        if (!dungeon)
            continue;

        MapEntry const* map = sMapStore.LookupEntry(dungeon->map);

        uint32 tank = dungeon->tankNeeded;
        uint32 heal = dungeon->healerNeeded;
        uint32 dps  = dungeon->dpsNeeded;

        bool raid = map && map->IsRaid();
        if (raid && sWorld->getBoolConfig(CONFIG_LFG_OVERRIDE_ROLES_REQUIRED))
        {
            tank = sWorld->getIntConfig(CONFIG_LFG_TANKS_NEEDED);
            heal = sWorld->getIntConfig(CONFIG_LFG_HEALERS_NEEDED);
            dps  = sWorld->getIntConfig(CONFIG_LFG_DPS_NEEDED);
        }
        if (map && map->IsScenario() && scenarioDebug)
            dps = 1;

        if (sWorld->getBoolConfig(CONFIG_LFG_SOLO))
        {
            tank = 0;
            heal = 0;
            dps = 1;
        }

        m_queues.emplace(dungeon->ID, DungeonQueue{ this, dungeon->ID, tank, heal, dps, raid });
    }

    for (auto&& itr : shortageDungeons)
        m_shortageData.emplace(itr, LfgShortageData{});
}

void QueueManager::MakeProposals()
{
    if (!IsDirty())
        return;

    SetClean();

#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    DungeonQueue::ProposalList rejectedProposals;
    // Removing queuers from other queuers when forming a proposal might lead to other buckets getting filled up, hence we repeat the process until we are no longer able to form a single proposal
    while (true)
    {
        DungeonQueue::ProposalList proposals;

        // Shuffle the queues randomly so that if the same set of players queued for the same set of dungeons (i.e. random heroic etc) a random one will be picked
        std::vector<DungeonQueue*> shuffledQueues{ m_queues.size() };
        std::transform(m_queues.begin(), m_queues.end(), shuffledQueues.begin(), [](DungeonQueueMap::reference& queue) { return &queue.second; });
        std::shuffle(shuffledQueues.begin(), shuffledQueues.end(), m_random);

        // Iterate over queues and collect created proposals
        for (auto&& queue : shuffledQueues)
            queue->MakeProposals(proposals);

        // Search for proposals until none were found
        if (proposals.empty())
            break;

        // Sort proposals by queued time average, so we prioritize players that have been queued for the longest time
        proposals.sort([](LfgProposal const& a, LfgProposal const& b) { return a.weight > b.weight; });

        // Remove queuers from other queues and following proposals
        for (auto itr = proposals.begin(); itr != proposals.end(); ++itr)
        {
            LfgProposal const& proposal = *itr; // *itr++; do not postincrement here, as the iterator it would now be pointing to could be erased inside the following loop

                                                // Erase following proposals if they contain any queuers from the successful one
            auto itr2 = itr;
            for (++itr2; itr2 != proposals.end();)
            {
                LfgProposal const& otherProposal = *itr2;
                if (std::any_of(proposal.queuers.begin(), proposal.queuers.end(), [&otherProposal](Queuer const& queuer) { return std::find(otherProposal.queuers.begin(), otherProposal.queuers.end(), queuer) != otherProposal.queuers.end(); }))
                    itr2 = proposals.erase(itr2);
                else
                    ++itr2;
            }

            // Dequeue the successful proposal queuers from all other queues
            for (auto&& queue : m_queues)
            {
                bool removed = false;
                for (auto&& queuer : proposal.queuers)
                    removed |= queue.second.Dequeue(queuer);

                if (removed)
                    queue.second.PerformBackfill();
            }
        }

        // Send proposals to LFGMgr
        for (auto&& proposal : proposals)
            if (!sLFGMgr->AddProposal(proposal))
                rejectedProposals.push_back(proposal);
    }

    for (auto&& itr : rejectedProposals)
        AddToQueue(itr.queuers, false);

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::MakeProposals, sw.GetMicroSec(), 1);
#endif
}

QueueManager::QueuerData const& QueueManager::GetQueuerData(Queuer const& queuer) const
{
    bool found = false;
    auto bounds = m_queuerData.equal_range(queuer.GetGUID());
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
        if (itr->second.QueueId == queuer.GetQeueueId())
            return itr->second;
    std::abort();
}

void QueueManager::AddQueueData(Queuer const& queuer, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& rolesMap, bool isContinue)
{
    m_queuerData.emplace(queuer.GetGUID(), QueuerData{ queuer.GetGUID(), queuer.GetQeueueId(), joinTime, dungeons, rolesMap });

    AddToQueue(queuer, isContinue);
}

void QueueManager::RemoveQueueData(Queuer const& queuer)
{
    bool removed = false;
    auto bounds = m_queuerData.equal_range(queuer.GetGUID());
    for (auto itr = bounds.first; itr != bounds.second;)
    {
        if (itr->second.QueueId == queuer.GetQeueueId())
        {
            itr = m_queuerData.erase(itr);
            removed = true;
        }
        else
            ++itr;
    }
}

void QueueManager::UpdateWaitTime(LfgRoles role, time_t waitTime, uint32 dungeonID)
{
    auto itr = m_queues.find(dungeonID);
    ASSERT(itr != m_queues.end());
    itr->second.UpdateWaitTime(role, waitTime);
}

void QueueManager::UpdateQueueTimers(time_t time)
{
#ifdef LFG_QUEUE_PROFILING
    Stopwatch sw;
#endif

    static auto selectTime = [](DungeonQueue::WaitTime const& container, int32& accumulator)
    {
        int32 value = container();
        if (value && value != -1)
            accumulator = accumulator == -1 ? value : std::min(accumulator, value); // Select lowest wait time among all the dungeons player selected. Can easily by changed to max/avg if need be
    };

    for (auto&& data : m_queuerData)
    {
        uint32 queuerDungeonID = 0;
        uint32 needTanks = 0;
        uint32 needHealers = 0;
        uint32 needDamage = 0;
        uint32 bestBucketScore = 0;
        uint32 queuedTime = uint32(time - data.second.JoinTime);
        int32 waitQueuer = -1;
        int32 waitTank = -1;
        int32 waitHealer = -1;
        int32 waitDamage = -1;
        int32 waitAverage = -1;

        LfgRoles queuerRoles = PLAYER_ROLE_NONE;
        for (auto&& role : data.second.Roles)
            queuerRoles = (LfgRoles)(queuerRoles | (role.second & ~PLAYER_ROLE_LEADER));

        // We can just pick the first player in the group as the source for selected dungeon, since all members adopt the group leader's choice when queuing
        auto& dungeons = sLFGMgr->GetSelectedDungeons(data.second.Roles.begin()->first, data.second.QueueId);
        for (auto&& dungeonID : dungeons)
        {
            if (!queuerDungeonID)
                queuerDungeonID = dungeonID;

            DungeonQueue const& queue = GetQueue(dungeonID);
            selectTime(queue.GetWaitTime(queuerRoles), waitQueuer);
            selectTime(queue.GetWaitTime(PLAYER_ROLE_TANK), waitTank);
            selectTime(queue.GetWaitTime(PLAYER_ROLE_HEALER), waitHealer);
            selectTime(queue.GetWaitTime(PLAYER_ROLE_DAMAGE), waitDamage);
            selectTime(queue.GetWaitTime(PLAYER_ROLE_NONE), waitAverage);
        }

        for (auto&& dungeonID : data.second.Dungeons)
        {
            for (auto&& bucket : GetQueue(dungeonID).GetBuckets())
            {
                if (bucket.Contains(Queuer{ data.first, data.second.QueueId }))
                {
                    uint32 score = bucket.GetUsedSlots(PLAYER_ROLE_DAMAGE) * (100 / bucket.GetTotalSlots(PLAYER_ROLE_DAMAGE));
                    if (bucket.GetTotalSlots(PLAYER_ROLE_TANK))
                        score += bucket.GetUsedSlots(PLAYER_ROLE_TANK) * (100 / bucket.GetTotalSlots(PLAYER_ROLE_TANK));
                    if (bucket.GetTotalSlots(PLAYER_ROLE_HEALER))
                        score += bucket.GetUsedSlots(PLAYER_ROLE_HEALER) * (100 / bucket.GetTotalSlots(PLAYER_ROLE_HEALER));

                    if (score > bestBucketScore)
                    {
                        bestBucketScore = score;
                        needTanks = bucket.GetRemainingSlots(PLAYER_ROLE_TANK);
                        needHealers = bucket.GetRemainingSlots(PLAYER_ROLE_HEALER);
                        needDamage = bucket.GetRemainingSlots(PLAYER_ROLE_DAMAGE);
                    }
                    break;
                }
            }
        }

        LfgQueueStatusData queueData(data.second.QueueId, queuerDungeonID, data.second.JoinTime, waitQueuer, waitAverage, waitTank, waitHealer, waitDamage, queuedTime, needTanks, needHealers, needDamage);
        for (auto&& role : data.second.Roles)
            LFGMgr::SendLfgQueueStatus(role.first, queueData);
    }

#ifdef LFG_QUEUE_PROFILING
    RecordProfilingData(ProfilingType::UpdateQueueTimers, sw.GetMicroSec(), m_queuerData.size());
#endif
}

void QueueManager::UpdateShortageData()
{
    auto now = time(nullptr);

    for (auto&& randomDungeonId : shortageDungeons)
    {
        LFGDungeonData const* dungeon = sLFGMgr->GetLFGDungeon(randomDungeonId);
        if (!dungeon)
            continue;

        LfgDungeonSet const* randomDungeons = nullptr;
        if (dungeon->group)
            randomDungeons = &sLFGMgr->GetDungeonsByRandom(randomDungeonId);

        m_shortageData[randomDungeonId].Reset();    // We should do it after MakeNewGroup, but whatever

        bool callToArms = false;

        // Collect all queued players' roles for shortage calculation
        uint32 tanksCount = 0, healersCount = 0, dpsCount = 0;
        for (auto&& itr : m_queuerData)
        {
            ObjectGuid guid = itr.first;
            uint32 queueId = itr.second.QueueId;
            if (sLFGMgr->GetState(guid, queueId) != LFG_STATE_QUEUED)
                continue;

            if (itr.second.Dungeons.empty())
                continue;

            if (itr.second.JoinTime + 90 < now)
                callToArms = true;

            if (!randomDungeons)
            {
                if (itr.second.Dungeons.size() == 1 && *itr.second.Dungeons.begin() == randomDungeonId)
                    continue;
            }
            else if (itr.second.Dungeons.size() != 1 || *itr.second.Dungeons.begin() != randomDungeonId)
            {
                auto first1 = randomDungeons->begin();
                auto last1 = randomDungeons->end();
                auto first2 = itr.second.Dungeons.begin();
                auto last2 = itr.second.Dungeons.end();

                bool found = false;

                while (first1 != last1 && first2 != last2)
                {
                    if (*first1 < *first2)
                        ++first1;
                    else if (*first1 > *first2)
                        ++first2;
                    else
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    continue;
            }

            if (guid.IsGroup())
            {
                LfgGuidSet const& members = sLFGMgr->GetPlayers(guid);
                for (LfgGuidSet::const_iterator mitr = members.begin(); mitr != members.end(); ++mitr)
                {
                    uint8 roles = sLFGMgr->GetRoles(*mitr, queueId);
                    if (roles & PLAYER_ROLE_TANK)
                        ++tanksCount;
                    if (roles & PLAYER_ROLE_HEALER)
                        ++healersCount;
                    if (roles & PLAYER_ROLE_DAMAGE)
                        ++dpsCount;
                }
            }
            else
            {
                uint8 roles = sLFGMgr->GetRoles(guid, queueId);
                if (roles & PLAYER_ROLE_TANK)
                    ++tanksCount;
                if (roles & PLAYER_ROLE_HEALER)
                    ++healersCount;
                if (roles & PLAYER_ROLE_DAMAGE)
                    ++dpsCount;
            }
        }

        bool raid = false;
        if (auto lfg_dungeon = sLFGDungeonStore.LookupEntry(randomDungeonId))
            raid = lfg_dungeon->difficulty == RAID_DIFFICULTY_25MAN_LFR;

        // Queue exists for more than 90 seconds and we have "enough" people in queue
        uint32 total = tanksCount + healersCount + dpsCount;
        if (callToArms && total >= uint32(raid ? 15 : 3))
            m_shortageData[randomDungeonId].Update(tanksCount, healersCount, dpsCount, raid);
    }
}

LfgRoles QueueManager::GetShortageRoles(uint32 dungeonId) const
{
    auto it = m_shortageData.find(dungeonId);
    if (it == m_shortageData.end())
        return PLAYER_ROLE_NONE;
    return LfgRoles(it->second.GetShortageRoles());
}

LfgRoles QueueManager::GetRolesForCTAReward(uint32 dungeonId, LfgRoles roles)
{
    auto it = m_shortageData.find(dungeonId);
    if (it == m_shortageData.end())
        return PLAYER_ROLE_NONE;
    uint8 shortageRoles = it->second.GetShortageRoles();
    if (!shortageRoles)
        return PLAYER_ROLE_NONE;
    return LfgRoles(shortageRoles & roles);
}

void QueueManager::OutDebug(std::ostringstream& ss, uint32 dungeonID, bool client) const
{
    ss << "=== QUEUE MANAGER ===\n";
    for (auto&& queue : m_queues)
        if (!dungeonID || queue.second.GetDungeonID() == dungeonID)
            queue.second.OutDebug(ss, client);
    if (dungeonID)
        return;
    ss << "=== QUEUE MANAGER QUEUER DATAS ===\n";
    for (auto&& data : m_queuerData)
    {
        Queuer(data.second.GUID, data.second.QueueId).OutDebug(ss, nullptr);
        ss << ": { JoinTime: " << data.second.JoinTime << " Roles: {";
        uint32 i = 0;
        for (auto&& role : data.second.Roles)
            ss << (i++ ? ", " : "") << sWorld->GetCharacterNameData(role.first)->m_name << "=" << (uint32)role.second;
        ss << "} Dungeons: [";
        i = 0;
        for (auto&& dungeon : data.second.Dungeons)
            ss << (i++ ? "," : "") << dungeon;
        ss << "] }\n";
    }
}
#pragma endregion

} // namespace lfg
