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

#ifndef TRINITYCORE_GROUP_H
#define TRINITYCORE_GROUP_H

#include "DBCEnums.h"
#include "GroupReference.h"
#include "GroupRefManager.h"
#include "LootMgr.h"
#include "QueryResult.h"
#include "SharedDefines.h"
#include "World.h"

class Battlefield;
class Battleground;
class Creature;
class GroupReference;
class InstanceSave;
class Map;
class Player;
class Guild;
class Unit;
class WorldObject;
class WorldPacket;
class WorldSession;

struct MapEntry;

enum class GroupSlot : uint8;

#define MAXGROUPSIZE 5
#define MAXRAIDSIZE 40
#define MAX_RAID_SUBGROUPS MAXRAIDSIZE/MAXGROUPSIZE
#define TARGETICONCOUNT 8

enum RollVote
{
    PASS              = 0,
    NEED              = 1,
    GREED             = 2,
    DISENCHANT        = 3,
    NOT_EMITED_YET    = 4,
    NOT_VALID         = 5
};

enum GroupMemberOnlineStatus
{
    MEMBER_STATUS_OFFLINE   = 0x0000,
    MEMBER_STATUS_ONLINE    = 0x0001,                       // Lua_UnitIsConnected
    MEMBER_STATUS_PVP       = 0x0002,                       // Lua_UnitIsPVP
    MEMBER_STATUS_DEAD      = 0x0004,                       // Lua_UnitIsDead
    MEMBER_STATUS_GHOST     = 0x0008,                       // Lua_UnitIsGhost
    MEMBER_STATUS_PVP_FFA   = 0x0010,                       // Lua_UnitIsPVPFreeForAll
    MEMBER_STATUS_UNK3      = 0x0020,                       // used in calls from Lua_GetPlayerMapPosition/Lua_GetBattlefieldFlagPosition
    MEMBER_STATUS_AFK       = 0x0040,                       // Lua_UnitIsAFK
    MEMBER_STATUS_DND       = 0x0080                        // Lua_UnitIsDND
};

enum GroupMemberFlags
{
    MEMBER_FLAG_ASSISTANT   = 0x01,
    MEMBER_FLAG_MAINTANK    = 0x02,
    MEMBER_FLAG_MAINASSIST  = 0x04
};

enum GroupMemberAssignment
{
    GROUP_ASSIGN_MAINTANK   = 0,
    GROUP_ASSIGN_MAINASSIST = 1
};

enum GroupType
{
    GROUPTYPE_NORMAL                = 0x00,
    GROUPTYPE_BG                    = 0x01,
    GROUPTYPE_RAID                  = 0x02,
    GROUPTYPE_BGRAID = GROUPTYPE_BG | GROUPTYPE_RAID,       // mask
    GROUPTYPE_UNK1                  = 0x04,
    GROUPTYPE_LFG                   = 0x08,
    GROUPTYPE_EVERYONE_IS_ASSISTANT = 0x40
    // 0x10, leave/change group?, I saw this flag when leaving group and after leaving BG while in group
};

enum GroupUpdateFlags
{
    GROUP_UPDATE_FLAG_NONE           = 0x00000000,      // nothing
    GROUP_UPDATE_FLAG_STATUS         = 0x00000001,      // uint16 (GroupMemberStatusFlag)
    GROUP_UPDATE_FLAG_MOP_UNK        = 0x00000002,      // for (2) uint8 unk
    GROUP_UPDATE_FLAG_CUR_HP         = 0x00000004,      // uint32 (HP)
    GROUP_UPDATE_FLAG_MAX_HP         = 0x00000008,      // uint32 (HP)
    GROUP_UPDATE_FLAG_POWER_TYPE     = 0x00000010,      // uint8 (PowerType)
    GROUP_UPDATE_FLAG_UNK20          = 0x00000020,      // uint16
    GROUP_UPDATE_FLAG_CUR_POWER      = 0x00000040,      // int16 (power value)
    GROUP_UPDATE_FLAG_MAX_POWER      = 0x00000080,      // int16 (power value)
    GROUP_UPDATE_FLAG_LEVEL          = 0x00000100,      // uint16 (level value)
    GROUP_UPDATE_FLAG_AREA           = 0x00000200,      // uint16 (area id)
    GROUP_UPDATE_FLAG_UNK400         = 0x00000400,      // int16 (unk)
    GROUP_UPDATE_FLAG_POSITION       = 0x00000800,      // uint16 (x), uint16 (y), uint16 (z)
    GROUP_UPDATE_FLAG_AURAS          = 0x00001000,      // uint8 (unk), uint64 (mask), uint32 (count), for each bit set: uint32 (spell id) + uint16 (AuraFlags)  (if has flags Scalable -> 3x int32 (bps))
    GROUP_UPDATE_FLAG_PET_GUID       = 0x00002000,      // uint64 (pet guid)
    GROUP_UPDATE_FLAG_PET_NAME       = 0x00004000,      // cstring (name, NULL terminated string)
    GROUP_UPDATE_FLAG_PET_MODEL_ID   = 0x00008000,      // uint16 (model id)
    GROUP_UPDATE_FLAG_PET_CUR_HP     = 0x00010000,      // uint32 (HP)
    GROUP_UPDATE_FLAG_PET_MAX_HP     = 0x00020000,      // uint32 (HP)
    GROUP_UPDATE_FLAG_PET_POWER_TYPE = 0x00040000,      // uint8 (PowerType)
    GROUP_UPDATE_FLAG_PET_CUR_POWER  = 0x00080000,      // uint16 (power value)
    GROUP_UPDATE_FLAG_PET_MAX_POWER  = 0x00100000,      // uint16 (power value)
    GROUP_UPDATE_FLAG_MOP_UNK_2      = 0x00200000,      // uint16 unk
    GROUP_UPDATE_FLAG_PET_AURAS      = 0x00400000,      // [see GROUP_UPDATE_FLAG_AURAS]
    GROUP_UPDATE_FLAG_VEHICLE_SEAT   = 0x00800000,      // int32 (vehicle seat id)
    GROUP_UPDATE_FLAG_PHASE          = 0x01000000,      // int32 (unk), uint32 (phase count), for (count) uint16(phaseId)
    GROUP_UPDATE_FLAG_UNK_2000000    = 0x2000000,       // byte ?
    GROUP_UPDATE_FLAG_UNK_4000000    = 0x4000000,       // ?
    GROUP_UPDATE_FLAG_UNK_8000000    = 0x8000000,       // byte ?
    GROUP_UPDATE_FLAG_UNK_10000000   = 0x10000000,      // byte ?
    GROUP_UPDATE_FLAG_UNK_20000000   = 0x20000000,      // count of bytes + bytes
    GROUP_UPDATE_FLAG_UNK_40000000   = 0x40000000,      // byte, maybe with dead state
    GROUP_UPDATE_FLAG_UNK_80000000   = 0x80000000,      // ?

    // Send all in one packet ?
    GROUP_UPDATE_FLAG_UNK_ALL = GROUP_UPDATE_FLAG_UNK_2000000 | GROUP_UPDATE_FLAG_UNK_4000000 | GROUP_UPDATE_FLAG_UNK_8000000 |
                                GROUP_UPDATE_FLAG_UNK_10000000 | GROUP_UPDATE_FLAG_UNK_20000000 | GROUP_UPDATE_FLAG_UNK_40000000,

    GROUP_UPDATE_PLAYER_BASE =  GROUP_UPDATE_FLAG_CUR_HP | GROUP_UPDATE_FLAG_MAX_HP | GROUP_UPDATE_FLAG_POWER_TYPE |
                                GROUP_UPDATE_FLAG_MAX_POWER,

    GROUP_UPDATE_PLAYER =       GROUP_UPDATE_FLAG_MOP_UNK | GROUP_UPDATE_FLAG_CUR_HP | GROUP_UPDATE_FLAG_MAX_HP |
                                GROUP_UPDATE_FLAG_POWER_TYPE | GROUP_UPDATE_FLAG_MAX_POWER | GROUP_UPDATE_FLAG_LEVEL |
                                GROUP_UPDATE_FLAG_AREA | GROUP_UPDATE_FLAG_UNK400 | GROUP_UPDATE_FLAG_POSITION |
                                GROUP_UPDATE_FLAG_AURAS | GROUP_UPDATE_FLAG_PHASE,       // all player flags

    GROUP_UPDATE_PET =          GROUP_UPDATE_FLAG_PET_GUID | GROUP_UPDATE_FLAG_PET_NAME | GROUP_UPDATE_FLAG_PET_MODEL_ID |
                                GROUP_UPDATE_FLAG_PET_CUR_HP | GROUP_UPDATE_FLAG_PET_MAX_HP | GROUP_UPDATE_FLAG_PET_POWER_TYPE |
                                GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER | GROUP_UPDATE_FLAG_PET_AURAS, // all pet flags

    GROUP_UPDATE_FULL =         GROUP_UPDATE_PLAYER | GROUP_UPDATE_PET // all known flags, except UNK100 and VEHICLE_SEAT
};

#define GROUP_UPDATE_FLAGS_COUNT          20

enum GroupRaidMarkersFlags
{
    RAID_MARKER_NONE    = 0x00,
    RAID_MARKER_BLUE    = 0x01,
    RAID_MARKER_GREEN   = 0x02,
    RAID_MARKER_PURPLE  = 0x04,
    RAID_MARKER_RED     = 0x08,
    RAID_MARKER_YELLOW  = 0x10,

    RAID_MARKER_ALL     = 0x20
};

#define MAX_RAID_MARKERS 5

class Roll : public LootValidatorRef
{
    public:
        Roll(ObjectGuid lootGuid, LootItem const& li);
        ~Roll();
        void setLoot(Loot* pLoot);
        Loot* getLoot();
        void targetObjectBuildLink();

        uint32 itemid;
        int32  itemRandomPropId;
        uint32 itemRandomSuffix;
        uint8 itemCount;
        ObjectGuid lootGUID;
        typedef std::map<ObjectGuid, RollVote> PlayerVote;
        PlayerVote playerVote;                              //vote position correspond with player position (in group)
        uint8 totalPlayersRolling;
        uint8 totalNeed;
        uint8 totalGreed;
        uint8 totalPass;
        uint8 itemSlot;
        uint8 rollVoteMask;
};

struct InstanceGroupBind
{
    InstanceSave* save;
    bool perm;
    /* permanent InstanceGroupBinds exist if the leader has a permanent
       PlayerInstanceBind for the same instance. */
    InstanceGroupBind() : save(NULL), perm(false) { }
};

/** request member stats checken **/
/// @todo uninvite people that not accepted invite
class Group
{
    public:
        struct MemberSlot
        {
            ObjectGuid  guid;
            std::string name;
            uint8       group;
            uint8       flags;
            uint8       roles;
            bool        readyCheckHasResponded;
        };
        typedef std::list<MemberSlot> MemberSlotList;
        typedef MemberSlotList::const_iterator member_citerator;

        typedef std::unordered_map< uint32 /*mapId*/, InstanceGroupBind> BoundInstancesMap;

        struct RaidMarker
        {
            float  posX;
            float  posY;
            float  posZ;
            uint32 mapId;
            uint32 mask;
            ObjectGuid guid;
            uint32 spellId;
        };

        typedef std::list<RaidMarker> RaidMarkerList;

        struct iterator
        {
            iterator(GroupReference const* ref) : _ref(ref) { }
            Player* operator->() const { return _ref->GetSource(); }
            Player* operator*() const { return _ref->GetSource(); }
            iterator operator++() { if (_ref) _ref = _ref->next(); return *this; }
            iterator operator++(int) { iterator ret = *this; ++(*this); return ret; }
            bool operator==(iterator const& other) { return _ref == other._ref; }
            bool operator!=(iterator const& other) { return _ref != other._ref; }

        private:
            GroupReference const* _ref;
        };

        iterator begin() const { return{ m_memberMgr.getFirst() }; }
        iterator end() const { return{ nullptr }; }

    protected:
        typedef MemberSlotList::iterator member_witerator;
        typedef std::set<Player*> InvitesList;

        typedef std::vector<Roll*> Rolls;

    public:
        Group();
        ~Group();

        // group manipulation methods
        bool   Create(Player* leader);
        void   LoadGroupFromDB(Field* field);
        void   LoadMemberFromDB(ObjectGuid::LowType guidLow, uint8 memberFlags, uint8 subgroup, uint8 roles);
        void   SaveRolesToDB();
        bool   AddInvite(Player* player);
        void   RemoveInvite(Player* player);
        void   RemoveAllInvites();
        bool   AddLeaderInvite(Player* player);
        bool   AddMember(Player* player);
        bool   RemoveMember(ObjectGuid guid, const RemoveMethod &method = GROUP_REMOVEMETHOD_DEFAULT, ObjectGuid kicker = ObjectGuid::Empty, const char* reason = NULL);
        void   ChangeLeader(ObjectGuid guid);
        void   FindNewLeader(ObjectGuid exceptGuid = ObjectGuid::Empty);
        void   SetLootMethod(LootMethod method);
        void   SetLooterGuid(ObjectGuid guid);
        void   SetMasterLooterGuid(ObjectGuid guid);
        void   UpdateLooterGuid(WorldObject* pLootedObject, bool ifneed = false);
        void   SetLootThreshold(ItemQualities threshold);
        void   Disband(bool hideDestroy = false);
        uint8  GetLfgRoles(ObjectGuid guid);
        void   SetLfgRoles(ObjectGuid guid, const uint8 roles);

        // properties accessories
        bool IsFull() const;
        bool isLFGGroup()  const;
        bool isRaidGroup() const;
        bool isBGGroup()   const;
        bool isBFGroup()   const;
        bool IsCreated()   const;
        bool IsFlex() const { return m_flex; }
        ObjectGuid GetLeaderGUID() const;
        ObjectGuid GetGUID() const;
        ObjectGuid::LowType GetLowGUID() const;
        const char * GetLeaderName() const;
        LootMethod GetLootMethod() const;
        ObjectGuid GetLooterGuid() const;
        ItemQualities GetLootThreshold() const;

        uint32 GetDbStoreId() const { return m_dbStoreId; };

        // member manipulation methods
        bool IsMember(ObjectGuid guid) const;
        bool IsLeader(ObjectGuid guid) const;
        ObjectGuid GetMemberGUID(const std::string& name) const;
        bool IsAssistant(ObjectGuid guid) const;
        bool IsGuildGroup(ObjectGuid::LowType guildId, Player* refPlayer, float* outXpMultiplier = nullptr, uint32* outCurrentCount = nullptr, uint32* outRequiredCount = nullptr) const;
        void UpdateGuildAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1, uint32 miscValue2, uint32 miscValue3, Unit* unit, WorldObject* rewardSource);
        Guild* GetGroupGuild(WorldObject* rewardSource, Player** participant = nullptr) const;
        bool ReadyCheckInProgress() const { return !m_readyCheckGuid.IsEmpty(); }
        ObjectGuid ReadyCheckInitiator() const { return m_readyCheckGuid; }
        bool ReadyCheckAllResponded() const;

        Player* GetInvited(uint64 guid) const;
        Player* GetInvited(const std::string& name) const;

        bool SameSubGroup(uint64 guid1, uint64 guid2) const;
        bool SameSubGroup(uint64 guid1, MemberSlot const* slot2) const;
        bool SameSubGroup(Player const* member1, Player const* member2) const;
        bool HasFreeSlotSubGroup(uint8 subgroup) const;

        MemberSlotList const& GetMemberSlots() const { return m_memberSlots; }
        GroupReference* GetFirstMember() { return m_memberMgr.getFirst(); }
        GroupReference const* GetFirstMember() const { return m_memberMgr.getFirst(); }
        uint32 GetMembersCount() const { return m_memberSlots.size(); }

        RaidMarkerList const& GetRaidMarkers() const { return m_raidMarkers; }
        RaidMarkerList& GetRaidMarkers() { return m_raidMarkers; }
        uint8 GetRaidMarkerByGuid(ObjectGuid objectGuid);
        RaidMarker GetRaidMarkerById(uint8 markerId);
        bool HasRaidMarker(ObjectGuid objectGuid);
        bool HasRaidMarker(uint8 markerId);
        void SendRaidMarkersUpdate();
        void AddRaidMarker(ObjectGuid objectGuid, uint32 spellId, uint32 mapId, float x, float y, float z);
        void RemoveRaidMarker(uint8 markerId);
        void RemoveAllRaidMarkers();

        uint8 GetMemberGroup(ObjectGuid guid) const;

        void ChangeFlagEveryoneAssistant(bool apply);
        void ConvertToLFG(bool flex);
        void ConvertToRaid();
        void ConvertToGroup();

        void SetBattlegroundGroup(Battleground* bg);
        void SetBattlefieldGroup(Battlefield* bf);
        GroupJoinBattlegroundResult CanJoinBattlegroundQueue(Battleground const* bgOrTemplate, BattlegroundQueueTypeId bgQueueTypeId, uint32 MinPlayerCount, uint32 MaxPlayerCount, bool isRated, uint32 arenaSlot);

        void ChangeMembersGroup(ObjectGuid guid, uint8 group);
        void ChangeMembersGroup(Player* player, uint8 group);
        void SetTargetIcon(uint8 symbol, ObjectGuid whoGuid, ObjectGuid targetGuid, uint8 partyIndex);
        void SetGroupMemberFlag(ObjectGuid guid, bool apply, GroupMemberFlags flag);
        void RemoveUniqueGroupMemberFlag(GroupMemberFlags flag);

        void SetMemberRole(ObjectGuid guid, uint32 role);
        uint32 GetMemberRole(ObjectGuid guid) const;

        ObjectGuid const GetTargetIcon(uint8 id) const { return m_targetIcons[id]; }

        Difficulty GetDifficulty(bool isRaid) const;
        Difficulty GetDungeonDifficulty() const;
        Difficulty GetRaidDifficulty() const;
        void SetDungeonDifficulty(Difficulty difficulty);
        void SetRaidDifficulty(Difficulty difficulty);
        uint16 InInstance();
        bool InCombatToInstance(uint32 instanceId);
        void ResetInstances(uint8 method, bool isRaid, Player* SendMsgTo);

        void ReadyCheck(ObjectGuid playerGuid) { m_readyCheckGuid = playerGuid; }
        void ReadyCheckMemberHasResponded(uint64 guid);
        void ReadyCheckResetResponded();

        // -no description-
        //void SendInit(WorldSession* session);
        void SendTargetIconList(WorldSession* session, int8 partyIndex = 0);
        void SendUpdate();
        void SendUpdateToPlayer(ObjectGuid playerGUID);
        void SendGroupRemoved(ObjectGuid playerGUID);
        void UpdatePlayerOutOfRange(Player* player);
        void SendReadyCheckCompleted();

        template<class Worker>
        void BroadcastWorker(Worker& worker)
        {
            for (GroupReference* itr = GetFirstMember(); itr != nullptr; itr = itr->next())
                worker(itr->GetSource());
        }

        template<class Worker>
        void BroadcastWorker(Worker const& worker) const
        {
            for (GroupReference const* itr = GetFirstMember(); itr != nullptr; itr = itr->next())
                worker(itr->GetSource());
        }

        void BroadcastPacket(WorldPacket* packet, bool ignorePlayersInBGRaid, int group = -1, ObjectGuid ignore = ObjectGuid::Empty);
        void BroadcastChat(WorldPacket const* packet, ObjectGuid sender, int32 group = -1) const;
        void BroadcastAddonMessagePacket(WorldPacket* packet, const std::string& prefix, bool ignorePlayersInBGRaid, int group = -1, uint64 ignore = 0);
        void BroadcastReadyCheck(WorldPacket* packet);
        void OfflineReadyCheck();

        /*********************************************************/
        /***                   LOOT SYSTEM                     ***/
        /*********************************************************/

        bool isRollLootActive() const;
        void SendLootStartRoll(uint32 CountDown, uint32 mapid, const Roll &r);
        void SendLootStartRollToPlayer(uint32 countDown, uint32 mapId, Player* p, bool canNeed, Roll const& r);
        void SendLootRoll(ObjectGuid targetGuid, int32 rollNumber, uint8 rollType, const Roll &r);
        void SendLootRollWon(ObjectGuid targetGuid, int32 rollNumber, uint8 rollType, const Roll &r);
        void SendLootAllPassed(Roll const& roll);
        void SendLooter(Creature* creature, Player* pLooter);
        void GroupLoot(Loot* loot, WorldObject* pLootedObject);
        void NeedBeforeGreed(Loot* loot, WorldObject* pLootedObject);
        void MasterLoot(Loot* loot, WorldObject* pLootedObject);
        Rolls::iterator GetRoll(ObjectGuid lootGuid, uint8 slot);
        void CountTheRoll(Rolls::iterator roll);
        void CountRollVote(ObjectGuid lootGuid, ObjectGuid playerGUID, uint8 slot, uint8 choise);
        void EndRoll(Loot* loot);

        // related to disenchant rolls
        void ResetMaxEnchantingLevel();

        void LinkMember(GroupReference* pRef);
        void DelinkMember(uint64 guid);

        InstanceGroupBind* BindToInstance(InstanceSave* save, bool permanent, bool load = false);
        void UnbindInstance(uint32 mapid, uint8 difficulty, bool unload = false);
        InstanceGroupBind* GetBoundInstance(Player* player);
        InstanceGroupBind* GetBoundInstance(Map* aMap);
        InstanceGroupBind* GetBoundInstance(MapEntry const* mapEntry);
        InstanceGroupBind* GetBoundInstance(Difficulty difficulty, uint32 mapId);
        BoundInstancesMap& GetBoundInstances(Difficulty difficulty);

        void AddLootLockout(WorldObject* lootedObject, bool pending = true);

        // FG: evil hacks
        void BroadcastGroupUpdate(void);

        uint8 GetGroupType() const { return m_groupType; }
        GroupSlot GetGroupSlot() const { return m_slot; }

        uint32 GetCounter() const { return m_counter; }

        void ResumeLoggingIfNeeded();

        char const* GetPlayerName(ObjectGuid guid) const;
        std::string FormatPlayer(ObjectGuid guid);
        std::string FormatLeader();
        static char const* GetMapName(uint32 id);
        static char const* GetItemName(uint32 id);
        static char const* Format(Difficulty difficulty);
        static char const* Format(LootMethod method);
        static char const* Format(ItemQualities quality);
        static char const* Format(RollVote vote);
        static std::string Format(Roll const* roll);
        static std::string Format(Item const* item);
        static std::string Format(Player const* player);
        static std::string FormatMoney(uint32 money);
        static std::string GetPlayerTalentString(Player const* player);

    protected:
        void SendUpdateToPlayer(ObjectGuid playerGUID, MemberSlot const* slot);

        bool _setMembersGroup(ObjectGuid guid, uint8 group);
        void HomebindIfInstance(Player* player);

        void _initRaidSubGroupsCounter();
        member_citerator _getMemberCSlot(uint64 Guid) const;
        member_witerator _getMemberWSlot(uint64 Guid);
        void SubGroupCounterIncrease(uint8 subgroup);
        void SubGroupCounterDecrease(uint8 subgroup);
        void ToggleGroupMemberFlag(member_witerator slot, uint8 flag, bool apply);

        MemberSlotList      m_memberSlots;
        RaidMarkerList      m_raidMarkers;
        GroupRefManager     m_memberMgr;
        InvitesList         m_invitees;
        ObjectGuid          m_leaderGuid;
        std::string         m_leaderName;
        GroupType           m_groupType;
        Difficulty          m_dungeonDifficulty;
        Difficulty          m_raidDifficulty;
        Battleground*       m_bgGroup;
        Battlefield*        m_bfGroup;
        ObjectGuid          m_targetIcons[TARGETICONCOUNT];
        LootMethod          m_lootMethod;
        ItemQualities       m_lootThreshold;
        ObjectGuid          m_looterGuid;
        Rolls               RollId;
        BoundInstancesMap   m_boundInstances[MAX_DIFFICULTY];
        uint8*              m_subGroupsCounts;
        ObjectGuid          m_guid;
        uint32              m_counter;                      // used only in SMSG_GROUP_LIST
        uint32              m_maxEnchantingLevel;
        uint32              m_enchantGuildPerkChance = 0;
        uint32              m_dbStoreId;                    // Represents the ID used in database (Can be reused by other groups if group was disbanded)
        ObjectGuid          m_readyCheckGuid = ObjectGuid::Empty;
        bool                m_logResumeOnLogin = false;
        GroupSlot           m_slot;
        bool                m_flex = false;
};

#endif
