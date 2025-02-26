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

#ifndef SF_ACHIEVEMENTMGR_H
#define SF_ACHIEVEMENTMGR_H

#include <map>
#include <string>

#include "Common.h"
#include "DBCEnums.h"
#include "DBCStores.h"

class Unit;
class Player;
class Guild;
class WorldSession;
class WorldPacket;

struct ModifierTreeNode
{
    ModifierTreeEntry const* Entry;
    std::vector<ModifierTreeNode const*> Nodes;
};

struct Criteria
{
    CriteriaEntry const* Entry;
    ModifierTreeNode const* Modifier = nullptr;
};

struct CriteriaTreeNode
{
    CriteriaTreeEntry const* Entry;
    ::Criteria const* Criteria = nullptr;
    AchievementEntry const* Achievement = nullptr;
    std::vector<CriteriaTreeNode const*> Nodes;
};

typedef std::vector<Criteria const*>            CriteriaList;
typedef std::vector<AchievementEntry const*>    AchievementEntryList;
typedef std::vector<CriteriaTreeNode const*>    CriteriaTreeList;
typedef std::vector<ModifierTreeNode const*>    ModifierTreeList;


typedef std::unordered_map<uint32, AchievementEntryList>         AchievementListByReferencedId;
typedef std::unordered_map<uint32, AchievementEntry const*>      AchievementEntryByCriteriaTree;
typedef std::unordered_map<uint32, ModifierTreeList>             ModifierTreeEntryByTreeId;
typedef std::unordered_map<uint32, CriteriaTreeList>             SubCriteriaTreeListById;

struct CriteriaProgress
{
    uint64 Counter;
    time_t Date;                                            // latest update time.
    ObjectGuid CompletedGUID;                               // GUID of the player that completed this criteria (guild achievements)
    bool   Changed;
};

enum AchievementCriteriaDataType
{                                                           // value1         value2        comment
    ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE                = 0, // 0              0
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_CREATURE          = 1, // creature_id    0
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE = 2, // class_id       race_id
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH= 3, // health_percent 0
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA              = 5, // spell_id       effect_idx
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AREA              = 6, // area id        0
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA              = 7, // spell_id       effect_idx
    ACHIEVEMENT_CRITERIA_DATA_TYPE_VALUE               = 8, // minvalue                     value provided with achievement update must be not less that limit
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_LEVEL             = 9, // minlevel                     minlevel of target
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_GENDER            = 10, // gender                       0=male; 1=female
    ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT              = 11, // scripted requirement
    // REUSE
    ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT    = 13, // count                        "with less than %u people in the zone"
    ACHIEVEMENT_CRITERIA_DATA_TYPE_T_TEAM              = 14, // team                         HORDE(67), ALLIANCE(469)
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_DRUNK             = 15, // drunken_state  0             (enum DrunkenState) of player
    ACHIEVEMENT_CRITERIA_DATA_TYPE_HOLIDAY             = 16, // holiday_id     0             event in holiday time
    ACHIEVEMENT_CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE  = 17, // min_score      max_score     player's team win bg and opposition team have team score in range
    ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT          = 18, // 0              0             maker instance script call for check current criteria requirements fit
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_EQUIPED_ITEM      = 19, // item_level     item_quality  for equipped item in slot to check item level and quality
    ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_ID              = 20, // map_id         0             player must be on map with id in map_id
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE = 21, // class_id       race_id
    ACHIEVEMENT_CRITERIA_DATA_TYPE_NTH_BIRTHDAY        = 22, // N                            login on day of N-th Birthday
    ACHIEVEMENT_CRITERIA_DATA_TYPE_S_KNOWN_TITLE       = 23, // title_id                     known (pvp) title, values from dbc
    ACHIEVEMENT_CRITERIA_DATA_TYPE_GAME_EVENT          = 24, // game_event_id  0
};

#define MAX_ACHIEVEMENT_CRITERIA_DATA_TYPE               25 // maximum value in AchievementCriteriaDataType enum

struct AchievementCriteriaData
{
    AchievementCriteriaDataType dataType;
    union
    {
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE              = 0 (no data)
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_CREATURE        = 1
        struct
        {
            uint32 id;
        } creature;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE = 2
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE = 21
        struct
        {
            uint32 class_id;
            uint32 race_id;
        } classRace;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH = 3
        struct
        {
            uint32 percent;
        } health;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AURA            = 5
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_AURA            = 7
        struct
        {
            uint32 spell_id;
            uint32 effect_idx;
        } aura;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_S_AREA            = 6
        struct
        {
            uint32 id;
        } area;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_VALUE             = 8
        struct
        {
            uint32 value;
            uint32 compType;
        } value;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_LEVEL           = 9
        struct
        {
            uint32 minlevel;
        } level;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_GENDER          = 10
        struct
        {
            uint32 gender;
        } gender;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_SCRIPT            = 11 (no data)
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT  = 13
        struct
        {
            uint32 maxcount;
        } map_players;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_T_TEAM            = 14
        struct
        {
            uint32 team;
        } team;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_S_DRUNK           = 15
        struct
        {
            uint32 state;
        } drunk;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_HOLIDAY           = 16
        struct
        {
            uint32 id;
        } holiday;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE= 17
        struct
        {
            uint32 min_score;
            uint32 max_score;
        } bg_loss_team_score;
        // ACHIEVEMENT_CRITERIA_DATA_INSTANCE_SCRIPT        = 18 (no data)
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_S_EQUIPED_ITEM    = 19
        struct
        {
            uint32 item_level;
            uint32 item_quality;
        } equipped_item;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_MAP_ID            = 20
        struct
        {
            uint32 mapId;
        } map_id;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_NTH_BIRTHDAY      = 21
        struct
        {
            uint32 nth_birthday;
        } birthday_login;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_KNOWN_TITLE       = 22
        struct
        {
            uint32 title_id;
        } known_title;
        // ACHIEVEMENT_CRITERIA_DATA_TYPE_GAME_EVENT        = 24
        struct
        {
            uint32 id;
        } game_event;
        // raw
        struct
        {
            uint32 value1;
            uint32 value2;
        } raw;
    };
    uint32 ScriptId;

    AchievementCriteriaData() : dataType(ACHIEVEMENT_CRITERIA_DATA_TYPE_NONE)
    {
        raw.value1 = 0;
        raw.value2 = 0;
        ScriptId = 0;
    }

    AchievementCriteriaData(uint32 _dataType, uint32 _value1, uint32 _value2, uint32 _scriptId) : dataType(AchievementCriteriaDataType(_dataType))
    {
        raw.value1 = _value1;
        raw.value2 = _value2;
        ScriptId = _scriptId;
    }

    bool IsValid(CriteriaEntry const* criteria);
    bool Meets(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscValue1 = 0) const;
};

struct AchievementCriteriaDataSet
{
        AchievementCriteriaDataSet() : criteria_id(0) { }
        typedef std::vector<AchievementCriteriaData> Storage;
        void Add(AchievementCriteriaData const& data) { storage.push_back(data); }
        bool Meets(Player const* source, Unit const* target, uint32 miscValue = 0) const;
        void SetCriteriaId(uint32 id) {criteria_id = id;}
    private:
        uint32 criteria_id;
        Storage storage;
};

typedef std::map<uint32, AchievementCriteriaDataSet> AchievementCriteriaDataMap;

struct AchievementReward
{
    uint32 TitleId[2];
    uint32 ItemId;
    int32  SpellId;
    uint32 Sender;
    std::string Subject;
    std::string Text;
};

typedef std::unordered_map<uint32, AchievementReward> AchievementRewards;

struct AchievementRewardLocale
{
    std::vector<std::string> Subject;
    std::vector<std::string> Text;
};

typedef std::unordered_map<uint32, AchievementRewardLocale> AchievementRewardLocales;

struct CompletedAchievementData
{
    time_t Date;
    uint32 CompletedBy;
    std::set<ObjectGuid> Guids;
    bool Changed;
};

typedef std::unordered_map<uint32, CriteriaProgress> CriteriaProgressMap;
typedef std::unordered_map<uint32, CompletedAchievementData> CompletedAchievementMap;

enum ProgressType
{
    PROGRESS_SET,
    PROGRESS_ACCUMULATE,
    PROGRESS_HIGHEST
};

enum class AchievementType
{
    Player,
    Account,
    Guild,
    Max
};

class AchievementMgr
{
    protected:
        AchievementMgr(AchievementType type)
            : m_type(type), m_achievementPoints(0) { }
        ~AchievementMgr() { }

    public:

        template<typename Func>
        static void WalkCriteriaTree(CriteriaTreeNode const* tree, Func const& func)
        {
            for (CriteriaTreeNode const* node : tree->Nodes)
                WalkCriteriaTree(node, func);

            func(tree);
        }

        void UpdateAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, uint64 miscValue3 = 0, Unit const* unit = nullptr, Player* referencePlayer = nullptr);
        virtual void CompletedAchievement(AchievementEntry const* entry, Player* referencePlayer);
        void CheckAllAchievementCriteria(Player* referencePlayer);
        bool HasAchieved(uint32 achievementId) const;
        virtual ObjectGuid GetGUID() const { return ObjectGuid::Empty; };

        virtual uint32 GetAchievementPoints() const { return m_achievementPoints; }

        uint32 GetCriteriaProgress(uint32 id);
        bool TryToCompleteAchievement(uint32 achievementId, Player* player);

    protected:
        virtual void SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const = 0;
        void SendAchievementEarned(AchievementEntry const* achievement, Player* referencePlayer) const;
        CriteriaProgress* GetCriteriaProgress(CriteriaEntry const* entry);
    public:
        void SetCriteriaProgress(Criteria const* entry, uint64 changeValue, Player* referencePlayer, ProgressType ptype = PROGRESS_SET);
    protected:
        void CompletedCriteriaTree(CriteriaTreeNode const* tree, Player* referencePlayer);
        void CompletedCriteriaFor(AchievementEntry const* achievement, Player* referencePlayer);
        bool IsCompletedCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer);
        bool CanCompleteCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer);
        bool IsCompletedCriteria(Criteria const* criteria, uint64 requiredAmount);
        bool IsCompletedAchievement(AchievementEntry const* entry, Player const* referencePlayer);
        bool IsFitTypeCondition(AchievementEntry const* entry);
        bool CanUpdateCriteriaTree(CriteriaTreeNode const* tree, Player const* referencePlayer);
        bool CanUpdateCriteria(Criteria const* criteria, CriteriaTreeList const* trees, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer);
        virtual void SendPacket(WorldPacket* data) const = 0;

        bool ConditionsSatisfied(CriteriaEntry const* criteria, Player* referencePlayer) const;
        bool RequirementsSatisfied(CriteriaEntry const* criteria, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer) const;
        bool ModifierTreeRequirementsSatisfied(ModifierTreeNode const* tree, uint64 miscValue1, uint64 miscValue2, Unit const* unit, Player* referencePlayer) const;
        bool AdditionalRequirementsSatisfied(ModifierTreeNode const* tree, uint64 miscValue1, uint64 miscValue2, Unit const* unit, Player* referencePlayer) const;

        AchievementType m_type;
        CriteriaProgressMap m_criteriaProgress;
        CompletedAchievementMap m_completedAchievements;
        uint32 m_achievementPoints;

        // Basically needed only for PlayerAchievementMgr but I don't want create another virtual methods and other shit
        struct AdditionalCriteriaProgerss
        {
            uint32 Timer;
            CriteriaEntry const* Criteria;
            CriteriaTreeNode const* Root;
        };

        std::map<uint32, AdditionalCriteriaProgerss> m_additionalCriteriaProgress;
};

class AccountAchievementMgr;

class PlayerAchievementMgrBase : public AchievementMgr
{
    public:
        PlayerAchievementMgrBase(AchievementType type)
            : AchievementMgr(type) { }

        Player* GetOwner() const { return m_owner; }
        ObjectGuid GetGUID() const override;
        void SendPacket(WorldPacket* data) const override;
        void RemoveCriteriaProgress(CriteriaEntry const* entry);
        // TODO: DEPRECATED
        void ResetAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false);
        void ResetCriterias(CriteriaResetTypes type, uint32 asset);
        void ResetCriterias(CriteriaStartTypes type, uint32 asset);
        void UpdateTimedCriteria(uint32 timeDiff);
        void StartCriteria(CriteriaStartTypes type, uint32 entry, uint32 timeLost);
        void RemoveCriteriaTimer(CriteriaStartTypes type, uint32 entry);   // used for quest and scripted timed achievements

    protected:
        Player* m_owner = nullptr;
};

class PlayerAchievementMgr final : public PlayerAchievementMgrBase
{
    friend class AccountAchievementMgr;
    public:
        PlayerAchievementMgr(Player* player)
            : PlayerAchievementMgrBase(AchievementType::Player) { m_owner = player; }

        uint32 GetAchievementPoints() const override;
        void Reset();
        static void DeleteFromDB(ObjectGuid::LowType lowguid);
        void LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult);
        void SaveToDB(CharacterDatabaseTransaction trans);
        void SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;

        // ONLY FOR COMMAND AND TESTS. DO NOT USE!
        void RemoveAchievement(AchievementEntry const* entry);
        void SendAllAchievementData() const;
        void SendAchievementInfo(Player* receiver) const;
};

class AccountAchievementMgr final : public PlayerAchievementMgrBase
{
    friend class PlayerAchievementMgr;
    public:
        AccountAchievementMgr()
            : PlayerAchievementMgrBase(AchievementType::Account) { }

        void SetCurrentPlayer(Player* player) { m_owner = player; }
        uint32 GetAchievementPoints() const override;
        void LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult);
        void SaveToDB(CharacterDatabaseTransaction trans);
        void SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;

        void RemoveAchievement(AchievementEntry const* entry);
        void SendAllAchievementData() const;
};

class GuildAchievementMgr final : public AchievementMgr
{
    public:
        explicit GuildAchievementMgr(Guild* guild)
            : AchievementMgr(AchievementType::Guild), m_owner(guild) { }

        void SendPacket(WorldPacket* data) const override;
        void RemoveCriteriaProgress(CriteriaEntry const* entry);
        void Reset();
        Guild* GetOwner() const { return m_owner; }
        ObjectGuid GetGUID() const override;

        static void DeleteFromDB(uint32 lowguid);
        void LoadFromDB(PreparedQueryResult achievementResult, PreparedQueryResult criteriaResult);
        void SaveToDB(CharacterDatabaseTransaction trans);
        void SendAchievementEarned(AchievementEntry const* achievement) const;
        void SendCriteriaUpdate(CriteriaEntry const* entry, CriteriaProgress const* progress, uint32 timeElapsed, bool timedCompleted) const override;
        void SendAllTrackedCriterias(Player* receiver, std::set<uint32> const& trackedCriterias) const;

        void CompletedAchievement(AchievementEntry const* achievement, Player* referencePlayer) override;
        void RemoveAchievement(AchievementEntry const* entry);
        void SendAllAchievementData(Player* receiver) const;
        void SendAchievementInfo(Player* receiver, uint32 achievementId = 0) const;

    private:
        Guild* m_owner;
};

class AchievementGlobalMgr
{
        AchievementGlobalMgr() = default;
        ~AchievementGlobalMgr() = default;

    public:
        static AchievementGlobalMgr* instance()
        {
            static AchievementGlobalMgr _instance;
            return &_instance;
        }

        static char const* GetCriteriaTypeString(AchievementCriteriaTypes type);
        static char const* GetCriteriaTypeString(uint32 type);

        CriteriaList const& GetAchievementCriteriaByType(AchievementCriteriaTypes criteriaType, AchievementType achievementType) const
        {
            return m_criteriasByType[size_t(achievementType)][criteriaType];
        }

        CriteriaList const& GetTimedCriteriaByType(CriteriaStartTypes type) const
        {
            return m_criteriasByTimedType[type];
        }

        CriteriaList const& GetCriteriaByStartType(CriteriaStartTypes type) const
        {
            return m_criteriasByStartType[type];
        }

        CriteriaList const& GetCriteriaByResetType(CriteriaResetTypes type) const
        {
            return m_criteriasByResetType[type];
        }

        CriteriaTreeList const* GetCriteriaTreesByCriteria(uint32 criteriaId, AchievementType type) const
        {
            auto it = m_criteriaTreeByCriteriaTyped[size_t(type)].find(criteriaId);
            return it != m_criteriaTreeByCriteriaTyped[size_t(type)].end() ? &it->second : nullptr;
        }

        CriteriaTreeList const* GetCriteriaTreesByCriteria(uint32 criteriaId) const
        {
            auto it = m_criteriaTreeByCriteria.find(criteriaId);
            return it != m_criteriaTreeByCriteria.end() ? &it->second : nullptr;
        }


        AchievementEntryList const* GetAchievementByReferencedId(uint32 id) const
        {
            auto itr = m_achievementListByReferencedId.find(id);
            return itr != m_achievementListByReferencedId.end() ? &itr->second : nullptr;
        }

        ModifierTreeNode const* GetCriteriaModifierTree(uint32 id) const
        {
            auto it = m_criteriaModifiers.find(id);
            return it != m_criteriaModifiers.end() ? it->second.get() : nullptr;
        }

        Criteria const* GetCriteriaByModifierTree(uint32 modifierTreeId)
        {
            auto it = m_criteriasByModifier.find(modifierTreeId);
            return it != m_criteriasByModifier.end() ? it->second : nullptr;
        }

        AchievementReward const* GetAchievementReward(AchievementEntry const* achievement) const
        {
            auto iter = m_achievementRewards.find(achievement->ID);
            return iter != m_achievementRewards.end() ? &iter->second : nullptr;
        }

        AchievementRewardLocale const* GetAchievementRewardLocale(AchievementEntry const* achievement) const
        {
            auto iter = m_achievementRewardLocales.find(achievement->ID);
            return iter != m_achievementRewardLocales.end() ? &iter->second : nullptr;
        }

        AchievementCriteriaDataSet const* GetCriteriaDataSet(CriteriaEntry const* achievementCriteria) const
        {
            auto iter = m_criteriaDataMap.find(achievementCriteria->ID);
            return iter != m_criteriaDataMap.end() ? &iter->second : nullptr;
        }

        AchievementEntry const* GetAchievementEntryByCriteriaTree(CriteriaTreeEntry const* criteria) const
        {
            while (true)
            {
                if (!criteria->Parent || criteria->Parent == criteria->ID)
                {
                    auto iter = m_achievementEntryByCriteriaTree.find(criteria->ID);
                    return iter != m_achievementEntryByCriteriaTree.end() ? iter->second : nullptr;
                }
                criteria = sCriteriaTreeStore.LookupEntry(criteria->Parent);
            }
            return nullptr;
        }

        CriteriaTreeNode const* GetCriteriaTree(uint32 treeId) const
        {
            auto it = m_criteriaTrees.find(treeId);
            return it != m_criteriaTrees.end() ? it->second.get() : nullptr;
        }

        Criteria const* GetCriteria(uint32 criteriaId)
        {
            auto it = m_criterias.find(criteriaId);
            return it != m_criterias.end() ? it->second.get() : nullptr;
        }

        bool IsRealmCompleted(AchievementEntry const* achievement) const
        {
            return m_allCompletedAchievements.find(achievement->ID) != m_allCompletedAchievements.end();
        }

        void SetRealmCompleted(AchievementEntry const* achievement)
        {
            m_allCompletedAchievements.insert(achievement->ID);
        }

        static bool IsGroupCriteriaType(AchievementCriteriaTypes type)
        {
            switch (type)
            {
                case ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE:
                case ACHIEVEMENT_CRITERIA_TYPE_WIN_BG:
                case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET:
                case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA:
                case ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2:
                case ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
                case ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON:
                case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SCENARIO:
                case ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_SPECIFIC_SCENARIO:
                    return true;
                default:
                    break;
            }

            return false;
        }

        void LoadAchievementCriteriaList();
        void LoadAchievementCriteriaData();
        void LoadAchievementReferenceList();
        void LoadCompletedAchievements();
        void LoadRewards();
        void LoadRewardLocales();
        AchievementEntry const* GetAchievement(uint32 achievementId) const;
        CriteriaEntry const* GetAchievementCriteria(uint32 achievementId) const;

        std::set<uint32> const& GetAchievementsWithTitle(uint32 team)
        {
            return m_achievementsWithTitle[team == ALLIANCE ? 0 : 1];
        }
    private:
        AchievementCriteriaDataMap m_criteriaDataMap;

        // store achievement criterias by type to speed up lookup
        CriteriaList m_criteriasByType[size_t(AchievementType::Max)][ACHIEVEMENT_CRITERIA_TYPE_TOTAL];
        CriteriaList m_criteriasByStartType[CRITERIA_START_TYPE_MAX];
        CriteriaList m_criteriasByResetType[CRITERIA_RESET_TYPE_MAX];
        CriteriaList m_criteriasByTimedType[CRITERIA_START_TYPE_MAX];

        // store achievements by referenced achievement id to speed up lookup
        AchievementListByReferencedId m_achievementListByReferencedId;

        std::unordered_map<uint32, CriteriaTreeList> m_criteriaTreeByCriteriaTyped[size_t(AchievementType::Max)];
        std::unordered_map<uint32, CriteriaTreeList> m_criteriaTreeByCriteria;
        AchievementEntryByCriteriaTree m_achievementEntryByCriteriaTree;
        typedef std::set<uint32> AllCompletedAchievements;
        AllCompletedAchievements m_allCompletedAchievements;

        AchievementRewards m_achievementRewards;
        AchievementRewardLocales m_achievementRewardLocales;
        std::set<uint32> m_achievementsWithTitle[2];
        std::unordered_map<uint32, std::unique_ptr<CriteriaTreeNode>> m_criteriaTrees;
        std::unordered_map<uint32, std::unique_ptr<ModifierTreeNode>> m_criteriaModifiers;
        std::unordered_map<uint32, Criteria*> m_criteriasByModifier;
        std::unordered_map<uint32, std::unique_ptr<Criteria>> m_criterias;
};

#define sAchievementMgr AchievementGlobalMgr::instance()

#endif
