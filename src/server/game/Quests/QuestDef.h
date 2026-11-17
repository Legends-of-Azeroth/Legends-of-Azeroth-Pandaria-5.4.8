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

#ifndef TRINITYCORE_QUEST_H
#define TRINITYCORE_QUEST_H

#include "Define.h"
#include "SharedDefines.h"
#include "WorldPacket.h"
#include "DBCEnums.h"
#include "EnumFlag.h"

#include <string>
#include <vector>
#include <bitset>

class Player;

class ObjectMgr;

#define MAX_QUEST_LOG_SIZE 25

#define QUEST_OBJECTIVES_COUNT 4
#define QUEST_ITEM_DROP_COUNT 4
#define QUEST_REWARD_CHOICES_COUNT 6
#define QUEST_REWARD_ITEM_COUNT 4
#define QUEST_DEPLINK_COUNT 10
#define QUEST_REWARD_REPUTATIONS_COUNT 5
#define QUEST_EMOTE_COUNT 4
#define QUEST_REWARD_CURRENCY_COUNT 4
#define QUEST_SOURCE_ITEM_COUNT 1

enum QuestFailedReason : uint32
{
    QUEST_ERR_NONE                        = 0,
    QUEST_ERR_FAILED_LOW_LEVEL            = 1,        // DESCRIPTION "You are not high enough level for that quest."
    QUEST_ERR_FAILED_WRONG_RACE           = 6,        // DESCRIPTION "That quest is not available to your race."
    QUEST_ERR_ALREADY_DONE                = 7,        // DESCRIPTION "You have completed that quest."
    QUEST_ERR_ONLY_ONE_TIMED              = 12,       // DESCRIPTION "You can only be on one timed quest at a time."
    QUEST_ERR_ALREADY_ON1                 = 13,       // DESCRIPTION "You are already on that quest."
    QUEST_ERR_FAILED_EXPANSION            = 16,       // DESCRIPTION "This quest requires an expansion enabled account."
    QUEST_ERR_ALREADY_ON2                 = 18,       // DESCRIPTION "You are already on that quest."
    QUEST_ERR_FAILED_MISSING_ITEMS        = 21,       // DESCRIPTION "You don't have the required items with you. Check storage."
    QUEST_ERR_FAILED_NOT_ENOUGH_MONEY     = 23,       // DESCRIPTION "You don't have enough money for that quest."
    QUEST_ERR_DAILY_QUESTS_REMAINING      = 26,       // DESCRIPTION "You have already completed 25 daily quests today."
    QUEST_ERR_FAILED_CAIS                 = 27,       // DESCRIPTION "You cannot complete quests once you have reached tired time."
    QUEST_ERR_ALREADY_DONE_DAILY          = 29        // DESCRIPTION "You have completed that daily quest today."
};

enum QuestShareMessages : uint8
{
    QUEST_PARTY_MSG_SHARING_QUEST           = 0,
    QUEST_PARTY_MSG_CANT_TAKE_QUEST         = 1,
    QUEST_PARTY_MSG_ACCEPT_QUEST            = 2,
    QUEST_PARTY_MSG_DECLINE_QUEST           = 3,
    QUEST_PARTY_MSG_BUSY                    = 4,
    QUEST_PARTY_MSG_LOG_FULL                = 5,
    QUEST_PARTY_MSG_HAVE_QUEST              = 6,
    QUEST_PARTY_MSG_FINISH_QUEST            = 7,
    QUEST_PARTY_MSG_CANT_BE_SHARED_TODAY    = 8,
    QUEST_PARTY_MSG_SHARING_TIMER_EXPIRED   = 9,
    QUEST_PARTY_MSG_NOT_IN_PARTY            = 10,
    QUEST_PARTY_MSG_NOT_ELIGIBLE_TODAY      = 11
};

enum QuestTradeSkill
{
    QUEST_TRSKILL_NONE           = 0,
    QUEST_TRSKILL_ALCHEMY        = 1,
    QUEST_TRSKILL_BLACKSMITHING  = 2,
    QUEST_TRSKILL_COOKING        = 3,
    QUEST_TRSKILL_ENCHANTING     = 4,
    QUEST_TRSKILL_ENGINEERING    = 5,
    QUEST_TRSKILL_FIRSTAID       = 6,
    QUEST_TRSKILL_HERBALISM      = 7,
    QUEST_TRSKILL_LEATHERWORKING = 8,
    QUEST_TRSKILL_POISONS        = 9,
    QUEST_TRSKILL_TAILORING      = 10,
    QUEST_TRSKILL_MINING         = 11,
    QUEST_TRSKILL_FISHING        = 12,
    QUEST_TRSKILL_SKINNING       = 13,
    QUEST_TRSKILL_JEWELCRAFTING  = 14
};

enum QuestStatus : uint8
{
    QUEST_STATUS_NONE           = 0,
    QUEST_STATUS_COMPLETE       = 1,
    //QUEST_STATUS_UNAVAILABLE    = 2,
    QUEST_STATUS_INCOMPLETE     = 3,
    //QUEST_STATUS_AVAILABLE      = 4,
    QUEST_STATUS_FAILED         = 5,
    QUEST_STATUS_REWARDED       = 6,        // Not used in DB
    MAX_QUEST_STATUS
};

enum class QuestGiverStatus : uint32
{
    None                         = 0x0000,
    Future                       = 0x0002,
    Trivial                      = 0x0004,
    TrivialRepeatableTurnin      = 0x0008,
    TrivialDailyQuest            = 0x0010,
    Incomplete                   = 0x0020,
    RepeatableTurnin             = 0x0040,
    DailyQuest                   = 0x0080,
    Quest                        = 0x0100,
    RewardCompleteNoPOI          = 0x0200,
    RewardCompletePOI            = 0x0400,
    LegendaryQuest               = 0x0800,
    LegendaryRewardCompleteNoPOI = 0x1000,
    LegendaryRewardCompletePOI   = 0x2000
};

DEFINE_ENUM_FLAG(QuestGiverStatus);

enum QuestFlags : uint32
{
    QUEST_FLAGS_NONE                        = 0x00000000,
    QUEST_FLAGS_COMPLETION_NO_DEATH         = 0x00000001,   // Not used currently
    QUEST_FLAGS_PARTY_ACCEPT                = 0x00000002,   // Not used currently. If player in party, all players that can accept this quest will receive confirmation box to accept quest CMSG_QUEST_CONFIRM_ACCEPT/SMSG_QUEST_CONFIRM_ACCEPT
    QUEST_FLAGS_COMPLETION_AREA_TRIGGER     = 0x00000004,   // Not used currently
    QUEST_FLAGS_SHARABLE                    = 0x00000008,   // Can be shared: Player::CanShareQuest()
    QUEST_FLAGS_HAS_CONDITION               = 0x00000010,   // Not used currently
    QUEST_FLAGS_HIDE_REWARD_POI             = 0x00000020,   // Not used currently: Unsure of content
    QUEST_FLAGS_RAID_GROUP_OK               = 0x00000040,   // Can be completed while in raid
    QUEST_FLAGS_EXPANSION_ONLY              = 0x00000080,   // Not used currently: Available if TBC expansion enabled only
    QUEST_FLAGS_NO_MONEY_FOR_XP             = 0x00000100,   // Not used currently: Experience is not converted to gold at max level
    QUEST_FLAGS_HIDE_REWARD                 = 0x00000200,   // Items and money rewarded only sent in SMSG_QUESTGIVER_OFFER_REWARD (not in SMSG_QUESTGIVER_QUEST_DETAILS or in client quest log(SMSG_QUEST_QUERY_RESPONSE))
    QUEST_FLAGS_TRACKING_EVENT              = 0x00000400,   // These quests are automatically rewarded on quest complete and they will never appear in quest log client side.
    QUEST_FLAGS_DEPRECATE_REPUTATION        = 0x00000800,   // Not used currently
    QUEST_FLAGS_DAILY                       = 0x00001000,   // Used to know quest is Daily one
    QUEST_FLAGS_FLAGS_PVP                   = 0x00002000,   // Having this quest in log forces PvP flag
    QUEST_FLAGS_DEPRECATED                  = 0x00004000,   // Used on quests that are not generically available
    QUEST_FLAGS_WEEKLY                      = 0x00008000,
    QUEST_FLAGS_AUTO_COMPLETE               = 0x00010000,   // auto complete
    QUEST_FLAGS_DISPLAY_ITEM_IN_TRACKER     = 0x00020000,   // Displays usable item in quest tracker
    QUEST_FLAGS_DISPLAY_COMPLETION_TEXT     = 0x00040000,   // use Objective text as Complete text
    QUEST_FLAGS_AUTO_ACCEPT                 = 0x00080000,   // The client recognizes this flag as auto-accept. However, NONE of the current quests (3.3.5a) have this flag. Maybe blizz used to use it, or will use it in the future.
    QUEST_FLAGS_AUTO_SUBMIT                 = 0x00100000,   // Quests with this flag player submit automatically by special button in player gui
    QUEST_FLAGS_AUTO_TAKE                   = 0x00200000,   // Automatically suggestion of accepting quest. Not from npc.
    QUEST_FLAGS_WELL_KNOWN                  = 0x04000000,   // Well-Known
    QUEST_FLAGS_PORTRAIT_IN_QUEST_LOG       = 0x08000000,   // Portrait from Log
    QUEST_FLAGS_SHOW_ITEM_WHEN_COMPLETED    = 0x10000000,   // Show Item When Completed
    QUEST_FLAGS_LAUNCH_GOSSIP_ACCEPT        = 0x20000000,   // Gossip on Quest Accept - Force Gossip
    QUEST_FLAGS_ITEMS_GLOW_WHEN_COMPLETE    = 0x40000000,   // Items Glow When Done
    QUEST_FLAGS_FAIL_ON_LOGOUT              = 0x80000000    // Fail on Logout
};

enum QuestFlagsEx : uint32
{
    QUEST_FLAGS_EX_NONE                             = 0x00000000,
    QUEST_FLAGS_EX_NO_ITEM_REMOVAL                  = 0x00000001,   // Keep Additional Items
    QUEST_FLAGS_EX_SUPPRESS_GOSSIP_COMPLETE         = 0x00000002,   // Gossip on Quest Completion - Suppress Gossip
    QUEST_FLAGS_EX_SUPPRESS_GOSSIP_ACCEPT           = 0x00000004,   // Gossip on Quest Accept - Suppress Gossip
    QUEST_FLAGS_EX_DENY_PLAYER_QUESTGIVER           = 0x00000008,   // Disallow Player as Questgiver (advanced)
    QUEST_FLAGS_EX_DISPLAY_CLASS_CHOICE_REWARDS     = 0x00000010,   // Choice Reward Filter - Matches Class
    QUEST_FLAGS_EX_DISPLAY_SPEC_CHOICE_REWARDS      = 0x00000020,   // Choice Reward Filter - Matches Spec
    QUEST_FLAGS_EX_REMOVE_ON_PERIODIC_RESET         = 0x00000040,   // Remove from Log on Periodic Reset
    QUEST_FLAGS_EX_ACCOUNT                          = 0x00000080,   // Account-Level Quest
    QUEST_FLAGS_EX_LEGENDARY                        = 0x00000100,   // Legendary Quest
    QUEST_FLAGS_EX_NO_GUILD_XP                      = 0x00000200,   // No Guild XP
    QUEST_FLAGS_EX_RESET_CACHE_ON_ACCEPT            = 0x00000400,   // Reset Cache on Accept (internal)
    QUEST_FLAGS_EX_NO_ABANDON_ONCE_BEGUN            = 0x00000800,   // No Abandon Once Any Objective Complete
    QUEST_FLAGS_EX_RECAST_ACCEPT_SPELL_ON_LOGIN     = 0x00001000,   // Recast accept spell on login
};

enum QuestSpecialFlags
{
    QUEST_SPECIAL_FLAGS_NONE                 = 0x000,
    // Trinity flags for set SpecialFlags in DB if required but used only at server
    QUEST_SPECIAL_FLAGS_REPEATABLE           = 0x001,   // Set by 1 in SpecialFlags from DB
    QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT = 0x002,   // Set by 2 in SpecialFlags from DB (if required area explore, spell SPELL_EFFECT_QUEST_COMPLETE casting, table `FECT_QUEST_COMPLETE casting, table `*_script` command SCRIPT_COMMAND_QUEST_EXPLORED use, set from script)
    QUEST_SPECIAL_FLAGS_AUTO_ACCEPT          = 0x004,   // Set by 4 in SpecialFlags in DB if the quest is to be auto-accepted.
    QUEST_SPECIAL_FLAGS_DF_QUEST             = 0x008,   // Set by 8 in SpecialFlags in DB if the quest is used by Dungeon Finder.
    QUEST_SPECIAL_FLAGS_MONTHLY              = 0x010,   // Set by 16 in SpecialFlags in DB if the quest is reset at the begining of the month
    QUEST_SPECIAL_FLAGS_CAST                 = 0x020,   // Set by 32 in SpecialFlags in DB if the quest requires RequiredOrNpcGo killcredit but NOT kill (a spell cast)
    // room for more custom flags

    QUEST_SPECIAL_FLAGS_DB_ALLOWED = QUEST_SPECIAL_FLAGS_REPEATABLE | QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT | QUEST_SPECIAL_FLAGS_AUTO_ACCEPT | QUEST_SPECIAL_FLAGS_DF_QUEST | QUEST_SPECIAL_FLAGS_MONTHLY | QUEST_SPECIAL_FLAGS_CAST,

    //QUEST_SPECIAL_FLAGS_DELIVER              = 0x080,   // Internal flag computed only
    //QUEST_SPECIAL_FLAGS_SPEAKTO              = 0x100,   // Internal flag computed only
    //QUEST_SPECIAL_FLAGS_KILL                 = 0x200,   // Internal flag computed only
    QUEST_SPECIAL_FLAGS_TIMED                = 0x400,   // Internal flag computed only
    //QUEST_SPECIAL_FLAGS_PLAYER_KILL          = 0x800    // Internal flag computed only
};

enum QuestObjectiveType
{
    QUEST_OBJECTIVE_MONSTER                = 0,
    QUEST_OBJECTIVE_ITEM                   = 1,
    QUEST_OBJECTIVE_GAMEOBJECT             = 2,
    QUEST_OBJECTIVE_TALKTO                 = 3,
    QUEST_OBJECTIVE_CURRENCY               = 4,
    QUEST_OBJECTIVE_LEARNSPELL             = 5,
    QUEST_OBJECTIVE_MIN_REPUTATION         = 6,
    QUEST_OBJECTIVE_MAX_REPUTATION         = 7,
    QUEST_OBJECTIVE_MONEY                  = 8,
    QUEST_OBJECTIVE_PLAYERKILLS            = 9,
    QUEST_OBJECTIVE_AREATRIGGER            = 10,
    QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC = 11,
    QUEST_OBJECTIVE_DEFEATBATTLEPET        = 12,
    QUEST_OBJECTIVE_WINPVPPETBATTLES       = 13,

    MAX_QUEST_OBJECTIVE_TYPE
};

enum class QuestCompleteSpellType : uint32
{
    LegacyBehavior  = 0,
    Follower        = 1,
    Tradeskill      = 2,
    Ability         = 3,
    Aura            = 4,
    Spell           = 5,
    Unlock          = 6,
    Companion       = 7,
    Max
};

struct QuestGreeting
{
    uint16 EmoteType;
    uint32 EmoteDelay;
    std::string Text;

    QuestGreeting() : EmoteType(0), EmoteDelay(0) { }
    QuestGreeting(uint16 emoteType, uint32 emoteDelay, std::string text)
        : EmoteType(emoteType), EmoteDelay(emoteDelay), Text(std::move(text)) { }
};

struct QuestGreetingLocale
{
    std::vector<std::string> Greeting;
};

struct QuestTemplateLocale
{
    std::vector<std::string> LogTitle;
    std::vector<std::string> LogDescription;
    std::vector<std::string> QuestDescription;
    std::vector<std::string> AreaDescription;
    std::vector<std::string> PortraitGiverText;
    std::vector<std::string> PortraitGiverName;
    std::vector<std::string> PortraitTurnInText;
    std::vector<std::string> PortraitTurnInName;
    std::vector<std::string> QuestCompletionLog;
};

struct QuestRequestItemsLocale
{
    std::vector<std::string> CompletionText;
};

struct QuestObjectivesLocale
{
    std::vector<std::string> Description;
};

struct QuestOfferRewardLocale
{
    std::vector<std::string> RewardText;
};

struct QuestObjective
{
    uint32 ID         = 0;
    uint32 QuestID    = 0;
    uint8 Type        = 0;
    int8 StorageIndex = 0;
    uint32 ObjectID   = 0;
    int32 Amount      = 0;
    uint32 Flags      = 0;
    std::string Description;
    std::vector<uint32> VisualEffects;
};

using QuestObjectives = std::vector<QuestObjective>;

struct QuestRewardDisplaySpell
{
    QuestRewardDisplaySpell() : SpellId(0), PlayerConditionId(0), Type(QuestCompleteSpellType::LegacyBehavior) { }
    QuestRewardDisplaySpell(uint32 spellId, uint32 playerConditionId, QuestCompleteSpellType type) : SpellId(spellId), PlayerConditionId(playerConditionId), Type(type) { }

    uint32 SpellId;
    uint32 PlayerConditionId;
    QuestCompleteSpellType Type;
};

// This Quest class provides a convenient way to access a few pretotaled (cached) quest details,
// all base quest information, and any utility functions such as generating the amount of
// xp to give
class Quest
{
    friend class ObjectMgr;
    public:
        // Loading data. All queries are in ObjectMgr::LoadQuests()
        explicit Quest(Field* questRecord);
        void LoadRewardDisplaySpell(Field* fields);
        void LoadQuestDetails(Field* fields);
        void LoadQuestRequestItems(Field* fields);
        void LoadQuestOfferReward(Field* fields);
        void LoadQuestTemplateAddon(Field* fields);
        void LoadQuestObjective(Field* fields);
        void LoadQuestObjectiveVisualEffect(Field* fields);

        uint32 XPValue(Player const* player) const;

        bool HasFlag(QuestFlags flag) const { return (_flags & uint32(flag)) != 0; }
        bool HasFlagEx(QuestFlagsEx flag) const { return (_flagsEx & uint32(flag)) != 0; }

        bool HasSpecialFlag(QuestSpecialFlags flag) const { return (_specialFlags & flag) != 0; }
        void SetSpecialFlag(QuestSpecialFlags flag) { _specialFlags |= flag; }
        bool HasQuestObjectiveType(QuestObjectiveType type) const { return _usedQuestObjectiveTypes[type]; }

        // Possibly deprecated flag
        bool IsUnavailable() const { return HasFlag(QUEST_FLAGS_DEPRECATED); }

        // whether the quest is globally enabled (spawned by pool, game event active etc.)
        static bool IsTakingQuestEnabled(uint32 questId);

        // table data accessors:
        uint32 GetQuestId() const { return _id; }
        uint32 GetQuestType() const { return _type; }
        int32 GetQuestLevel() const { return _questLevel; }
        int32 GetMinLevel() const { return _minLevel; }
        uint32 GetQuestPackageID() const { return _packageID; }
        int32  GetZoneOrSort() const { return _questSortID; }
        uint32 GetMaxLevel() const { return _maxLevel; }
        uint32 GetQuestInfoID() const { return _questInfoID; }
        uint32 GetAllowableClasses() const { return _allowableClasses; }
        uint32 GetAllowableRaces() const { return _allowableRaces; }
        uint32 GetRequiredSkill() const { return _requiredSkillId; }
        uint32 GetRequiredSkillValue() const { return _requiredSkillPoints; }
        uint32 GetRequiredMinRepFaction() const { return _requiredMinRepFaction; }
        int32  GetRequiredMinRepValue() const { return _requiredMinRepValue; }
        uint32 GetRequiredMaxRepFaction() const { return _requiredMaxRepFaction; }
        int32  GetRequiredMaxRepValue() const { return _requiredMaxRepValue; }
        uint32 GetSuggestedPlayers() const { return _suggestedPlayers; }
        int64 GetLimitTime() const { return _limitTime; }
        int32  GetPrevQuestId() const { return _prevQuestID; }
        uint32 GetNextQuestId() const { return _nextQuestID; }
        int32  GetExclusiveGroup() const { return _exclusiveGroup; }
        uint32 GetNextQuestInChain() const { return _nextQuestInChain; }
        int32  GetRewArenaPoints() const {return _rewardArenaPoints; }
        uint32 GetXPDifficulty() const { return _rewardXPDifficulty; }
        uint32 GetSrcItemId() const { return _sourceItemId; }
        uint32 GetSrcItemCount() const { return _sourceItemIdCount; }
        uint32 GetSrcSpell() const { return _sourceSpellID; }
        std::string const& GetLogTitle() const { return _logTitle; }
        std::string const& GetLogDescription() const { return _logDescription; }
        std::string const& GetQuestDescription() const { return _questDescription; }
        std::string const& GetAreaDescription() const { return _areaDescription; }
        std::string const& GetOfferRewardText() const { return _offerRewardText; }
        std::string const& GetRequestItemsText() const { return _requestItemsText; }
        std::string const& GetQuestCompletionLog() const { return _questCompletionLog; }
        std::string const& GetPortraitGiverText() const { return _portraitGiverText; }
        std::string const& GetPortraitGiverName() const { return _portraitGiverName; }
        std::string const& GetPortraitTurnInText() const { return _portraitTurnInText; }
        std::string const& GetPortraitTurnInName() const { return _portraitTurnInName; }
        QuestObjectives const& GetObjectives() const { return Objectives; }
        int32 GetRewOrReqMoney(Player const* player) const;
        uint32 GetRewHonor() const { return _rewardHonor; }
        uint32 GetRewKillHonor() const { return _rewardKillHonor; }
        uint32 GetRewMoneyMaxLevel() const; // use in XP calculation at client
        uint32 GetRewSpell() const { return _rewardSpell; }
        uint32 GetRewDisplaySpell() const { return _rewardDisplaySpell; }
        uint32 GetRewMailTemplateId() const { return _rewardMailTemplateId; }
        uint32 GetRewMailDelaySecs() const { return _rewardMailDelay; }
        uint32 GetRewTitle() const { return _rewardTitleId; }
        uint32 GetPOIContinent() const { return _poiContinent; }
        float GetPOIx() const { return _poix; }
        float GetPOIy() const { return _poiy; }
        uint32 GetPOIPriority() const { return _poiPriority; }
        uint32 GetSoundAccept() const { return _soundAccept; }
        uint32 GetSoundTurnIn() const { return _soundTurnIn; }
        uint32 GetIncompleteEmote() const { return _emoteOnIncomplete; }
        uint32 GetCompleteEmote() const { return _emoteOnComplete; }
        bool IsRepeatable() const { return _specialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE; }
        bool IsAutoAccept() const;
        bool IsTurnIn() const;
        uint32 GetFlags() const { return _flags; }
        uint32 GetFlagsEx() const { return _flagsEx; }
        uint32 GetSpecialFlags() const { return _specialFlags; }
        uint32 GetScriptId() const { return _scriptId; }
        uint32 GetAreaGroupID() const { return _areaGroupID; }
        uint32 GetRewardSkillId() const { return _rewardSkillId; }
        uint32 GetRewardSkillPoints() const { return _rewardSkillPoints; }
        uint32 GetRewardReputationMask() const { return _rewardReputationMask; }
        uint32 GetQuestGiverPortrait() const { return _questGiverPortrait; }
        uint32 GetQuestTurnInPortrait() const { return _questTurnInPortrait; }
        bool IsDaily() const { return (_flags & QUEST_FLAGS_DAILY) != 0; }
        bool IsWeekly() const { return (_flags & QUEST_FLAGS_WEEKLY) != 0; }
        bool IsMonthly() const { return (_specialFlags & QUEST_SPECIAL_FLAGS_MONTHLY) != 0; }
        bool IsSeasonal() const { return (_questSortID == -QUEST_SORT_SEASONAL || _questSortID == -QUEST_SORT_SPECIAL || _questSortID == -QUEST_SORT_LUNAR_FESTIVAL || _questSortID == -QUEST_SORT_MIDSUMMER || _questSortID == -QUEST_SORT_BREWFEST || _questSortID == -QUEST_SORT_LOVE_IS_IN_THE_AIR || _questSortID == -QUEST_SORT_NOBLEGARDEN) && !IsRepeatable(); }
        bool IsDailyOrWeekly() const { return (_flags & (QUEST_FLAGS_DAILY | QUEST_FLAGS_WEEKLY)) != 0; }
        bool IsRaidQuest(Difficulty difficulty) const;
        bool IsAllowedInRaid(Difficulty difficulty) const;
        bool IsDFQuest() const { return (_specialFlags & QUEST_SPECIAL_FLAGS_DF_QUEST) != 0; }
        uint32 CalculateHonorGain(uint8 level) const;

        // multiple values
        // multiple values
        std::vector<QuestRewardDisplaySpell> RewardDisplaySpell;
        std::array<uint32, QUEST_REWARD_ITEM_COUNT> RewardItemId = { };
        std::array<uint32, QUEST_REWARD_ITEM_COUNT> RewardItemCount = { };
        std::array<uint32, QUEST_ITEM_DROP_COUNT> ItemDrop = { };
        std::array<uint32, QUEST_ITEM_DROP_COUNT> ItemDropQuantity = { };
        std::array<uint32, QUEST_REWARD_CHOICES_COUNT> RewardChoiceItemId = { };
        std::array<uint32, QUEST_REWARD_CHOICES_COUNT> RewardChoiceItemCount = { };
        std::array<uint32, QUEST_REWARD_REPUTATIONS_COUNT> RewardFactionId = { };
        std::array<int32, QUEST_REWARD_REPUTATIONS_COUNT>  RewardFactionValue = { };
        std::array<int32, QUEST_REWARD_REPUTATIONS_COUNT>  RewardFactionOverride = { };
        std::array<uint32, QUEST_REWARD_CURRENCY_COUNT> RewardCurrencyId = { };
        std::array<uint32, QUEST_REWARD_CURRENCY_COUNT> RewardCurrencyCount = { };
        QuestObjectives Objectives;
        std::array<uint32, QUEST_EMOTE_COUNT> DetailsEmote = { };
        std::array<uint32, QUEST_EMOTE_COUNT> DetailsEmoteDelay = { };
        std::array<int32, QUEST_EMOTE_COUNT> OfferRewardEmote = { };
        std::array<uint32, QUEST_EMOTE_COUNT> OfferRewardEmoteDelay = { };

        uint32 GetRewChoiceItemsCount() const { return _rewChoiceItemsCount; }
        uint32 GetRewItemsCount() const { return _rewItemsCount; }
        uint32 GetRewCurrencyCount() const { return _rewCurrencyCount; }

        std::vector<uint32> DependentPreviousQuests;

    private:
        uint32 _rewChoiceItemsCount;
        uint32 _rewItemsCount;
        uint32 _rewCurrencyCount;

        // wdb data (quest query response)
        uint32 _id = 0;
        uint32 _type = 0;
        int32 _questLevel = 0;
        int32 _minLevel = 0;
        uint32 _packageID = 0;
        int32 _questSortID = 0;
        uint32 _questInfoID = 0;
        uint32 _suggestedPlayers = 0;
        uint32 _nextQuestInChain = 0;
        uint32 _rewardXPDifficulty = 0;
        int32 _rewardMoney = 0;
        uint32 _rewardBonusMoney = 0;
        uint32 _rewardDisplaySpell = 0;
        uint32 _rewardSpell = 0;
        uint32 _rewardHonor = 0;
        uint32 _rewardKillHonor = 0;
        uint32 _sourceItemId = 0;
        uint32 _flags = 0;
        uint32 _flagsEx = 0;
        uint32 _poiContinent = 0;
        float _poix = 0.f;
        float _poiy = 0.f;
        uint32 _poiPriority = 0;
        uint32 _rewardTitleId = 0;
        int32 _rewardArenaPoints = 0;
        uint32 _rewardSkillId = 0;
        uint32 _rewardSkillPoints = 0;
        uint32 _questGiverPortrait = 0;
        uint32 _questTurnInPortrait = 0;
        uint32 _rewardReputationMask;
        uint32 _soundAccept = 0;
        uint32 _soundTurnIn = 0;
        uint32 _areaGroupID = 0;
        int64 _limitTime = 0;
        uint32 _allowableRaces;
        std::string _logTitle;
        std::string _logDescription;
        std::string _questDescription;
        std::string _areaDescription;
        std::string _portraitGiverText;
        std::string _portraitGiverName;
        std::string _portraitTurnInText;
        std::string _portraitTurnInName;
        std::string _questCompletionLog;

        // quest_request_items table
        uint32 _emoteOnComplete = 0;
        uint32 _emoteOnIncomplete = 0;
        std::string _requestItemsText;

        // quest_offer_reward table
        std::string _offerRewardText;

        // quest_template_addon table (custom data)
        uint32 _maxLevel = 0;
        uint32 _allowableClasses = 0;
        uint32 _sourceSpellID = 0;
        int32 _prevQuestID = 0;
        uint32 _nextQuestID = 0;
        int32 _exclusiveGroup = 0;
        uint32 _rewardMailTemplateId = 0;
        uint32 _rewardMailDelay = 0;
        uint32 _requiredSkillId = 0;
        uint32 _requiredSkillPoints = 0;
        uint32 _requiredMinRepFaction = 0;
        int32 _requiredMinRepValue = 0;
        uint32 _requiredMaxRepFaction = 0;
        int32 _requiredMaxRepValue = 0;
        uint32 _sourceItemIdCount = 0;
        uint32 _specialFlags = 0; // custom flags, not sniffed/WDB
        std::bitset<MAX_QUEST_OBJECTIVE_TYPE> _usedQuestObjectiveTypes;
        uint32 _scriptId = 0;
};

struct QuestStatusData
{
    QuestStatusData() : Status(QUEST_STATUS_NONE), Timer(0), Explored(false) { }

    QuestStatus Status;
    uint32 Timer;
    bool Explored;
};

#endif
