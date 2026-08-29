/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "DBCStores.h"
#include "GroupReference.h"
#include "MapReference.h"

#include "Item.h"
#include "PetDefines.h"
#include "PhaseMgr.h"
#include "PlayerTaxi.h"
#include "QuestDef.h"
#include "SpellMgr.h"
#include "Unit.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "SceneMgr.h"
#include "VignetteMgr.h"

#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

struct AuctionQueryContext;
struct CreatureTemplate;
struct Mail;
struct ItemExtendedCostEntry;
struct TrainerSpell;
struct VendorItem;

class BattlePetMgr;
class CinematicMgr;
class Channel;
class CharacterCreateInfo;
class Creature;
class DynamicObject;
class Group;
class Guild;
class LootLockoutMap;
class OutdoorPvP;
class Pet;
class PhaseMgr;
class PlayerMenu;
class PlayerSocial;
class PlayerAchievementMgr;
class ReputationMgr;
class SpellCastTargets;
class UpdateMask;
class PlayerAI;
class SpellHistory;
class TradeData;

typedef std::deque<Mail*> PlayerMails;

#define PLAYER_MAX_SKILLS           128
enum SkillFieldOffset
{
    SKILL_ID_OFFSET = 0,
    SKILL_STEP_OFFSET = 64,
    SKILL_RANK_OFFSET = SKILL_STEP_OFFSET + 64,
    SUBSKILL_START_RANK_OFFSET = SKILL_RANK_OFFSET + 64,
    SKILL_MAX_RANK_OFFSET = SUBSKILL_START_RANK_OFFSET + 64,
    SKILL_MODIFIER_OFFSET = SKILL_MAX_RANK_OFFSET + 64,
    SKILL_TALENT_OFFSET = SKILL_MODIFIER_OFFSET + 64
};

#define PLAYER_MAX_DAILY_QUESTS     750
#define PLAYER_EXPLORED_ZONES_SIZE  200

struct ActionButtonPACKET
{
    uint32 id;
    uint32 type;
};

// Note: SPELLMOD_* values is aura types in fact
enum SpellModType
{
    SPELLMOD_FLAT = 107,                            // SPELL_AURA_ADD_FLAT_MODIFIER
    SPELLMOD_PCT = 108                             // SPELL_AURA_ADD_PCT_MODIFIER
};

// 2^n values, Player::m_isunderwater is a bitmask. These are Trinity internal values, they are never send to any client
enum PlayerUnderwaterState
{
    UNDERWATER_NONE = 0x00,
    UNDERWATER_INWATER = 0x01,             // terrain type is water and player is afflicted by it
    UNDERWATER_INLAVA = 0x02,             // terrain type is lava and player is afflicted by it
    UNDERWATER_INSLIME = 0x04,             // terrain type is lava and player is afflicted by it
    UNDERWATER_INDARKWATER = 0x08,             // terrain type is dark water and player is afflicted by it

    UNDERWATER_EXIST_TIMERS = 0x10
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER = 2,
    ERR_BANKSLOT_OK = 3
};

enum AttackSwingResult : int32
{
    ATTACKSWING_DONT_REPORT   = -1, // Don't send to client in HandleAttackSwingOpcode
    ATTACKSWING_NOT_IN_RANGE  = 0,
    ATTACKSWING_BAD_FACING    = 1,
    ATTACKSWING_DEAD_TARGET   = 2,
    ATTACKSWING_CANNOT_ATTACK = 3
};

enum PlayerSpellState
{
    PLAYERSPELL_UNCHANGED = 0,
    PLAYERSPELL_CHANGED = 1,
    PLAYERSPELL_NEW = 2,
    PLAYERSPELL_REMOVED = 3,
    PLAYERSPELL_TEMPORARY = 4
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active : 1;                             // show in spellbook
    bool dependent : 1;                             // learned as result another spell learn, skill grow, quest reward, etc
    bool disabled : 1;                             // first rank has been learned in result talent learn but currently talent unlearned, save max learned ranks
};

struct PlayerTalent
{
    PlayerSpellState state : 8;
    uint8 spec : 8;
};

extern uint32 const MasterySpells [MAX_CLASSES];

enum TalentTree // talent tabs
{
    TALENT_TREE_WARRIOR_ARMS = 746,
    TALENT_TREE_WARRIOR_FURY = 815,
    TALENT_TREE_WARRIOR_PROTECTION = 845,
    TALENT_TREE_PALADIN_HOLY = 831,
    TALENT_TREE_PALADIN_PROTECTION = 839,
    TALENT_TREE_PALADIN_RETRIBUTION = 855,
    TALENT_TREE_HUNTER_BEAST_MASTERY = 811,
    TALENT_TREE_HUNTER_MARKSMANSHIP = 807,
    TALENT_TREE_HUNTER_SURVIVAL = 809,
    TALENT_TREE_ROGUE_ASSASSINATION = 182,
    TALENT_TREE_ROGUE_COMBAT = 181,
    TALENT_TREE_ROGUE_SUBTLETY = 183,
    TALENT_TREE_PRIEST_DISCIPLINE = 760,
    TALENT_TREE_PRIEST_HOLY = 813,
    TALENT_TREE_PRIEST_SHADOW = 795,
    TALENT_TREE_DEATH_KNIGHT_BLOOD = 398,
    TALENT_TREE_DEATH_KNIGHT_FROST = 399,
    TALENT_TREE_DEATH_KNIGHT_UNHOLY = 400,
    TALENT_TREE_SHAMAN_ELEMENTAL = 261,
    TALENT_TREE_SHAMAN_ENHANCEMENT = 263,
    TALENT_TREE_SHAMAN_RESTORATION = 262,
    TALENT_TREE_MAGE_ARCANE = 799,
    TALENT_TREE_MAGE_FIRE = 851,
    TALENT_TREE_MAGE_FROST = 823,
    TALENT_TREE_WARLOCK_AFFLICTION = 871,
    TALENT_TREE_WARLOCK_DEMONOLOGY = 867,
    TALENT_TREE_WARLOCK_DESTRUCTION = 865,
    TALENT_TREE_DRUID_BALANCE = 752,
    TALENT_TREE_DRUID_FERAL_COMBAT = 750,
    TALENT_TREE_DRUID_RESTORATION = 748,
    TALENT_TREE_MONK_BREWMASTER = 268,
    TALENT_TREE_MONK_MISTWEAVER = 270,
    TALENT_TREE_MONK_WINDWALKER = 269
};

enum class SpellModDropPhase : int32
{
    OnSpellFinish,
    OnCast,
};

// Spell modifier (used for modify other spells)
struct SpellModifier
{
    SpellModifier(Aura* aura = nullptr, AuraEffect* eff = nullptr)
        : ownerAura(aura), effect(eff) { }

    SpellModOp   op = SPELLMOD_DAMAGE;
    SpellModType type = SPELLMOD_FLAT;
    int16 charges = 0;
    float value = 0.0f;
    flag128 mask;
    uint32 spellId = 0;
    Aura* const ownerAura;
    AuraEffect* const effect;
    SpellModDropPhase dropPhase = SpellModDropPhase::OnSpellFinish;
};

enum PlayerCurrencyState
{
    PLAYERCURRENCY_UNCHANGED = 0,
    PLAYERCURRENCY_CHANGED = 1,
    PLAYERCURRENCY_NEW = 2,
    PLAYERCURRENCY_REMOVED = 3     //not removed just set count == 0
};

struct PlayerCurrency
{
    PlayerCurrencyState state;
    uint32 totalCount;
    uint32 weekCount;
    uint32 seasonCount;
    uint8 flags;
};

typedef std::unordered_map<uint32, PlayerTalent*> PlayerTalentMap;
typedef std::unordered_map<uint32, PlayerSpell*> PlayerSpellMap;
typedef std::list<SpellModifier*> SpellModList;
typedef std::unordered_map<uint32, PlayerCurrency> PlayerCurrenciesMap;

typedef std::list<uint64> WhisperListContainer;

/// Maximum number of CompactUnitFrames profiles
#define MAX_CUF_PROFILES 5

/// Bit index used in the many bool options of CompactUnitFrames
enum CUFBoolOptions
{
    CUF_KEEP_GROUPS_TOGETHER,
    CUF_DISPLAY_PETS,
    CUF_DISPLAY_MAIN_TANK_AND_ASSIST,
    CUF_DISPLAY_HEAL_PREDICTION,
    CUF_DISPLAY_AGGRO_HIGHLIGHT,
    CUF_DISPLAY_ONLY_DISPELLABLE_DEBUFFS,
    CUF_DISPLAY_POWER_BAR,
    CUF_DISPLAY_BORDER,
    CUF_USE_CLASS_COLORS,
    CUF_DISPLAY_NON_BOSS_DEBUFFS,
    CUF_DISPLAY_HORIZONTAL_GROUPS,
    CUF_LOCKED,
    CUF_SHOWN,
    CUF_AUTO_ACTIVATE_2_PLAYERS,
    CUF_AUTO_ACTIVATE_3_PLAYERS,
    CUF_AUTO_ACTIVATE_5_PLAYERS,
    CUF_AUTO_ACTIVATE_10_PLAYERS,
    CUF_AUTO_ACTIVATE_15_PLAYERS,
    CUF_AUTO_ACTIVATE_25_PLAYERS,
    CUF_AUTO_ACTIVATE_40_PLAYERS,
    CUF_AUTO_ACTIVATE_SPEC_1,
    CUF_AUTO_ACTIVATE_SPEC_2,
    CUF_AUTO_ACTIVATE_PVP,
    CUF_AUTO_ACTIVATE_PVE,
    CUF_UNK_145,
    CUF_UNK_156,
    CUF_UNK_157,

    // The unks is _LOCKED and _SHOWN and _DYNAMIC, unknown order

    CUF_BOOL_OPTIONS_COUNT,
};

/// Represents a CompactUnitFrame profile
struct CUFProfile
{
    CUFProfile() : ProfileName(), BoolOptions() // might want to change default value for options
    {
        FrameHeight = 0;
        FrameWidth = 0;
        SortBy = 0;
        HealthText = 0;
        Unk146 = 0;
        Unk147 = 0;
        Unk148 = 0;
        Unk150 = 0;
        Unk152 = 0;
        Unk154 = 0;
    }

    CUFProfile(const std::string& name, uint16 frameHeight, uint16 frameWidth, uint8 sortBy, uint8 healthText, uint32 boolOptions,
               uint8 unk146, uint8 unk147, uint8 unk148, uint16 unk150, uint16 unk152, uint16 unk154)
               : ProfileName(name), BoolOptions((int) boolOptions)
    {
        FrameHeight = frameHeight;
        FrameWidth = frameWidth;
        SortBy = sortBy;
        HealthText = healthText;
        Unk146 = unk146;
        Unk147 = unk147;
        Unk148 = unk148;
        Unk150 = unk150;
        Unk152 = unk152;
        Unk154 = unk154;
    }

    std::string ProfileName;
    uint16 FrameHeight;
    uint16 FrameWidth;
    uint8 SortBy;
    uint8 HealthText;

    // LeftAlign, TopAlight, BottomAllign (unk order)
    uint8 Unk146;
    uint8 Unk147;
    uint8 Unk148;

    // LeftOffset, TopOffset and BottomOffset (unk order)
    uint16 Unk150;
    uint16 Unk152;
    uint16 Unk154;

    std::bitset<CUF_BOOL_OPTIONS_COUNT> BoolOptions;

    // More fields can be added to BoolOptions without changing DB schema (up to 32, currently 27)
};

struct SpellCooldown
{
    TimeValue end;
    uint32 itemid;
};

typedef std::map<uint32, SpellCooldown> SpellCooldowns;
typedef std::list<uint32> SpellCooldownToRemove;
typedef std::unordered_map<uint32 /*instanceId*/, time_t/*releaseTime*/> InstanceTimeMap;

enum TrainerSpellState
{
    TRAINER_SPELL_GRAY = 0,
    TRAINER_SPELL_GREEN = 1,
    TRAINER_SPELL_RED = 2,
    TRAINER_SPELL_GREEN_DISABLED = 10                       // custom value, not send to client: formally green but learn not allowed
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED = 0,
    ACTIONBUTTON_CHANGED = 1,
    ACTIONBUTTON_NEW = 2,
    ACTIONBUTTON_DELETED = 3
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL = 0x00,
    ACTION_BUTTON_C = 0x01,                         // click?
    ACTION_BUTTON_EQSET = 0x20,
    ACTION_BUTTON_DROPDOWN = 0x30,
    ACTION_BUTTON_MACRO = 0x40,
    ACTION_BUTTON_CMACRO = ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM = 0x80
};

enum ReputationSource
{
    REPUTATION_SOURCE_KILL,
    REPUTATION_SOURCE_QUEST,
    REPUTATION_SOURCE_DAILY_QUEST,
    REPUTATION_SOURCE_WEEKLY_QUEST,
    REPUTATION_SOURCE_MONTHLY_QUEST,
    REPUTATION_SOURCE_REPEATABLE_QUEST,
    REPUTATION_SOURCE_SPELL
};

#define ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAX_ACTION_BUTTON_ACTION_VALUE (0x00FFFFFF+1)

struct ActionButton
{
    ActionButton() : packedData(0), uState(ACTIONBUTTON_NEW)
    { }

    uint32 packedData;
    ActionButtonUpdateState uState;

    // helpers
    ActionButtonType GetType() const
    {
        return ActionButtonType(ACTION_BUTTON_TYPE(packedData));
    }
    uint32 GetAction() const
    {
        return ACTION_BUTTON_ACTION(packedData);
    }
    void SetActionAndType(uint32 action, ActionButtonType type)
    {
        uint32 newData = action | (uint32(type) << 24);
        if (newData != packedData || uState == ACTIONBUTTON_DELETED)
        {
            packedData = newData;
            if (uState != ACTIONBUTTON_NEW)
                uState = ACTIONBUTTON_CHANGED;
        }
    }
};

#define  MAX_ACTION_BUTTONS 132                             //checked in 3.2.0

typedef std::map<uint8, ActionButton> ActionButtonList;

struct PvPInfo
{
    bool IsHostile = false;
    bool IsInHostileArea = false;       // Marks if player is in an area which forces PvP flag
    bool IsInNoPvPArea = false;         // Marks if player is in a sanctuary or friendly capital city
    bool IsInFFAPvPArea = false;        // Marks if player is in an FFAPvP area (such as Gurubashi Arena)
    TimeValue OffTime;                  // Time when pvp timer will disappear on the client
    TimeValue LastPvPTime;              // Time when the player had last pvp action
};

struct DuelInfo
{
    DuelInfo() : initiator(NULL), opponent(NULL), startTimer(0), startTime(0), outOfBound(0), isMounted(false)
    { }

    Player* initiator;
    Player* opponent;
    time_t startTimer;
    time_t startTime;
    time_t outOfBound;
    bool isMounted;
};

struct Areas
{
    uint32 areaID;
    uint32 areaFlag;
    float x1;
    float x2;
    float y1;
    float y2;
};

#define MAX_RUNES       6

enum RuneReadiness
{
    RUNE_READINESS_FULL = 10000,
    RUNE_READINESS_MISS = RUNE_READINESS_FULL - 1500     // cooldown applied on runes when the spell misses
};

enum RuneType
{
    RUNE_BLOOD = 0,
    RUNE_UNHOLY = 1,
    RUNE_FROST = 2,
    RUNE_DEATH = 3,
    NUM_RUNE_TYPES = 4
};

enum class RuneState
{
    Ready,
    PenaltyCooldown,
    NormalCooldown,
    FullyDepleted
};

struct RuneInfo
{
    uint8 BaseRune = 0;
    uint8 CurrentRune = 0;
    uint32 Readiness = RUNE_READINESS_FULL;
    RuneState State = RuneState::Ready;
    uint32 SpellId = 0;
    bool Permanently = false;
};

struct Runes
{
    RuneInfo runes [MAX_RUNES];
    uint8 runeState;                                        // mask of available runes
    uint8 lastUsedRuneMask;

    void SetRuneState(uint8 index, bool set = true)
    {
        if (set)
            runeState |= (1 << index);                      // usable
        else
            runeState &= ~(1 << index);                     // on cooldown
    }
};

struct EnchantDuration
{
    EnchantDuration() : item(NULL), slot(MAX_ENCHANTMENT_SLOT), leftduration(0)
    { };
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot),
        leftduration(_leftduration)
    {
        ASSERT(item);
    };

    Item* item;
    EnchantmentSlot slot;
    uint32 leftduration;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

enum DrunkenState
{
    DRUNKEN_SOBER = 0,
    DRUNKEN_TIPSY = 1,
    DRUNKEN_DRUNK = 2,
    DRUNKEN_SMASHED = 3
};

#define MAX_DRUNKEN   4

enum PlayerFlags
{
    PLAYER_FLAGS_GROUP_LEADER = 0x00000001,
    PLAYER_FLAGS_AFK = 0x00000002,
    PLAYER_FLAGS_DND = 0x00000004,
    PLAYER_FLAGS_GM = 0x00000008,
    PLAYER_FLAGS_GHOST = 0x00000010,
    PLAYER_FLAGS_RESTING = 0x00000020,
    PLAYER_FLAGS_UNK6 = 0x00000040,
    PLAYER_FLAGS_UNK7 = 0x00000080,       // pre-3.0.3 PLAYER_FLAGS_FFA_PVP flag for FFA PVP state
    PLAYER_FLAGS_CONTESTED_PVP = 0x00000100,       // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_IN_PVP = 0x00000200,
    PLAYER_FLAGS_HIDE_HELM = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK = 0x00000800,
    PLAYER_FLAGS_PLAYED_LONG_TIME = 0x00001000,       // played long time
    PLAYER_FLAGS_PLAYED_TOO_LONG = 0x00002000,       // played too long time
    PLAYER_FLAGS_IS_OUT_OF_BOUNDS = 0x00004000,
    PLAYER_FLAGS_DEVELOPER = 0x00008000,       // <Dev> prefix for something?
    PLAYER_FLAGS_UNK16 = 0x00010000,       // pre-3.0.3 PLAYER_FLAGS_SANCTUARY flag for player entered sanctuary
    PLAYER_FLAGS_TAXI_BENCHMARK = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER = 0x00040000,       // 3.0.2, pvp timer active (after you disable pvp manually)
    PLAYER_FLAGS_UBER = 0x00080000,
    PLAYER_FLAGS_UNK20 = 0x00100000,
    PLAYER_FLAGS_UNK21 = 0x00200000,
    PLAYER_FLAGS_COMMENTATOR2 = 0x00400000,
    PLAYER_FLAGS_ALLOW_ONLY_ABILITY = 0x00800000,       // used by bladestorm and killing spree, allowed only spells with SPELL_ATTR0_REQ_AMMO, SPELL_EFFECT_ATTACK, checked only for active player
    PLAYER_FLAGS_BATTLE_PET_ENABLED = 0x01000000,
    PLAYER_FLAGS_NO_XP_GAIN = 0x02000000,
    PLAYER_FLAGS_UNK26 = 0x04000000,
    PLAYER_FLAGS_AUTO_DECLINE_GUILD = 0x08000000,       // Automatically declines guild invites
    PLAYER_FLAGS_GUILD_LEVEL_ENABLED = 0x10000000,       // Lua_GetGuildLevelEnabled() - enables guild leveling related UI
    PLAYER_FLAGS_VOID_UNLOCKED = 0x20000000,       // void storage
    PLAYER_FLAGS_UNK30 = 0x40000000,
    PLAYER_FLAGS_UNK31 = 0x80000000
};

#define KNOWN_TITLES_SIZE   4
#define MAX_TITLE_INDEX     (KNOWN_TITLES_SIZE * 64)        // 4 uint64 fields

// used in PLAYER_FIELD_BYTES values
enum PlayerFieldByteFlags
{
    PLAYER_FIELD_BYTE_TRACK_STEALTHED = 0x00000002,
    PLAYER_FIELD_BYTE_RELEASE_TIMER = 0x00000008,       // Display time till auto release spirit
    PLAYER_FIELD_BYTE_NO_RELEASE_WINDOW = 0x00000010        // Display no "release spirit" window at all
};

// used in UNIT_FIELD_BYTES_2 values
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTE2_NONE = 0x00,
    PLAYER_FIELD_BYTE2_STEALTH = 0x20,
    PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW = 0x40
};

enum MirrorTimerType
{
    FATIGUE_TIMER = 0,
    BREATH_TIMER = 1,
    FIRE_TIMER = 2 // feign death
};
#define MAX_TIMERS      3
#define DISABLED_MIRROR_TIMER   -1

// 2^n values
enum PlayerExtraFlags
{
    // gm abilities
    PLAYER_EXTRA_GM_ON = 0x0001,
    PLAYER_EXTRA_ACCEPT_WHISPERS = 0x0004,
    PLAYER_EXTRA_TAXICHEAT = 0x0008,
    PLAYER_EXTRA_GM_INVISIBLE = 0x0010,
    PLAYER_EXTRA_GM_CHAT = 0x0020,               // Show GM badge in chat messages

    // other states
    PLAYER_EXTRA_PVP_DEATH = 0x0100                // store PvP death status until corpse creating.
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE               = 0x00,
    AT_LOGIN_RENAME             = 0x01,
    AT_LOGIN_RESET_SPELLS       = 0x02,
    AT_LOGIN_RESET_TALENTS      = 0x04,
    AT_LOGIN_CUSTOMIZE          = 0x08,
    AT_LOGIN_RESET_PET_TALENTS  = 0x10,
    AT_LOGIN_FIRST              = 0x20,
    AT_LOGIN_CHANGE_FACTION     = 0x40,
    AT_LOGIN_CHANGE_RACE        = 0x80,
    AT_LOGIN_LEARN_CLASS_SPELLS = 0x100,
    AT_LOGIN_POST_AUTOTRANSFER  = 0x200,
    AT_LOGIN_RESURRECT          = 0x400,
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;
typedef std::map<uint32, uint32> QuestObjectiveStatusMap;
typedef std::set<uint32> RewardedQuestSet;

//               quest,  keep
typedef std::map<uint32, bool> QuestStatusSaveMap;

// Size (in bytes) of client completed quests bit map
#define QUESTS_COMPLETED_BITS_SIZE 2048

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET = 0,
    QUEST_STATE_OFFSET = 1,
    QUEST_COUNTS_OFFSET = 2,
    QUEST_TIME_OFFSET = 14
};

#define MAX_QUEST_COUNTS 24
#define MAX_QUEST_OFFSET 15

enum QuestSlotStateMask
{
    QUEST_STATE_NONE = 0x0000,
    QUEST_STATE_COMPLETE = 0x0001,
    QUEST_STATE_FAIL = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED = 0,
    SKILL_CHANGED = 1,
    SKILL_NEW = 2,
    SKILL_DELETED = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState)
    { }
    uint8 pos;
    SkillUpdateState uState;
};

typedef std::unordered_map<uint32, SkillStatusData> SkillStatusMap;

class Quest;
class Spell;
class Item;
class WorldSession;

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START = 0,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END = 86,
    PLAYER_SLOTS_COUNT = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

#define INVENTORY_SLOT_BAG_0    255

enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START = 0,
    EQUIPMENT_SLOT_HEAD = 0,
    EQUIPMENT_SLOT_NECK = 1,
    EQUIPMENT_SLOT_SHOULDERS = 2,
    EQUIPMENT_SLOT_BODY = 3,
    EQUIPMENT_SLOT_CHEST = 4,
    EQUIPMENT_SLOT_WAIST = 5,
    EQUIPMENT_SLOT_LEGS = 6,
    EQUIPMENT_SLOT_FEET = 7,
    EQUIPMENT_SLOT_WRISTS = 8,
    EQUIPMENT_SLOT_HANDS = 9,
    EQUIPMENT_SLOT_FINGER1 = 10,
    EQUIPMENT_SLOT_FINGER2 = 11,
    EQUIPMENT_SLOT_TRINKET1 = 12,
    EQUIPMENT_SLOT_TRINKET2 = 13,
    EQUIPMENT_SLOT_BACK = 14,
    EQUIPMENT_SLOT_MAINHAND = 15,
    EQUIPMENT_SLOT_OFFHAND = 16,
    EQUIPMENT_SLOT_RANGED  = 17,
    EQUIPMENT_SLOT_TABARD = 18,
    EQUIPMENT_SLOT_END = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_START = 19,
    INVENTORY_SLOT_BAG_END = 23
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START = 23,
    INVENTORY_SLOT_ITEM_END = 39
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START = 39,
    BANK_SLOT_ITEM_END = 67
};

enum BankBagSlots                                           // 7 slots
{
    BANK_SLOT_BAG_START = 67,
    BANK_SLOT_BAG_END = 74
};

enum BuyBackSlots                                           // 12 slots
{
    // stored in m_buybackitems
    BUYBACK_SLOT_START = 74,
    BUYBACK_SLOT_END = 86
};

enum EquipmentSetUpdateState
{
    EQUIPMENT_SET_UNCHANGED = 0,
    EQUIPMENT_SET_CHANGED = 1,
    EQUIPMENT_SET_NEW = 2,
    EQUIPMENT_SET_DELETED = 3
};

struct EquipmentSet
{
    EquipmentSet() : Guid(), IgnoreMask(0), state(EQUIPMENT_SET_NEW)
    {
        for (uint8 i = 0; i < EQUIPMENT_SLOT_END; ++i)
            Items [i] = 0;
    }

    uint64 Guid;
    std::string Name;
    std::string IconName;
    uint32 IgnoreMask;
    uint32 Items [EQUIPMENT_SLOT_END];
    EquipmentSetUpdateState state;
};

#define MAX_EQUIPMENT_SET_INDEX 10                          // client limit

typedef std::map<uint32, EquipmentSet> EquipmentSets;

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint32 _count) : pos(_pos), count(_count)
    { }
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint32 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

enum TransferAbortReason
{
    TRANSFER_ABORT_NOT_FOUND                     = 0x0,  // Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES            = 0x4,  // You have entered too many instances recently.
    TRANSFER_ABORT_ERROR                         = 0x5,  //
    TRANSFER_ABORT_ALREADY_COMPLETED_ENCOUNTER   = 0x6,  // You are ineligible to participate in at least one encounter in this instance because you are already locked to an instance in which it has been defeated.
    TRANSFER_ABORT_NOT_FOUND_2                   = 0xA,  // Transfer Aborted: instance not found
    TRANSFER_ABORT_NOT_FOUND_3                   = 0xC,  // Transfer Aborted: instance not found
    TRANSFER_ABORT_MAP_NOT_ALLOWED               = 0xD,  // Map cannot be entered at this time.
    TRANSFER_ABORT_UNIQUE_MESSAGE                = 0xE,  // Until you've escaped TLK's grasp, you cannot leave this place!
    TRANSFER_ABORT_NEED_GROUP                    = 0xF,  // Transfer Aborted: you must be in a raid group to enter this instance
    TRANSFER_ABORT_XREALM_ZONE_DOWN              = 0x11, // Transfer Aborted: cross-realm zone is down
    TRANSFER_ABORT_NOT_FOUND_4                   = 0x12, // Transfer Aborted: instance not found
    TRANSFER_ABORT_DIFFICULTY_NOT_FOUND          = 0x13, // client writes to console "Unable to resolve requested difficultyID %u to actual difficulty for map %d"
    TRANSFER_ABORT_DIFFICULTY                    = 0x14, // <Normal, Heroic, Epic> difficulty mode is not available for %s.
    TRANSFER_ABORT_LOCKED_TO_DIFFERENT_INSTANCE  = 0x16, // You are already locked to %s
    TRANSFER_ABORT_MAX_PLAYERS                   = 0x17, // Transfer Aborted: instance is full
    TRANSFER_ABORT_TOO_MANY_REALM_INSTANCES      = 0x18, // Additional instances cannot be launched, please try again later.
    TRANSFER_ABORT_INSUF_EXPAN_LVL               = 0x19, // You must have <TBC, WotLK> expansion installed to access this area.
    TRANSFER_ABORT_SOLO_PLAYER_SWITCH_DIFFICULTY = 0x1C, // This instance is already in progress. You may only switch difficulties from inside the instance.
    TRANSFER_ABORT_ZONE_IN_COMBAT                = 0x1D, // Unable to zone in while an encounter is in progress.
    TRANSFER_ABORT_REALM_ONLY                    = 0x1F, // All players in the party must be from the same realm to enter %s.
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME = 4,                    // Welcome to %s. This raid instance is scheduled to reset in %s.
    RAID_INSTANCE_EXPIRED = 5
};

class InstanceSave;

enum RestFlag
{
    REST_FLAG_NONE            = 0,
    REST_FLAG_IN_TAVERN       = 1,
    REST_FLAG_IN_CITY         = 2,
    REST_FLAG_IN_FACTION_AREA = 4
};

enum TeleportToOptions
{
    TELE_TO_GM_MODE = 0x01,
    TELE_TO_NOT_LEAVE_TRANSPORT = 0x02,
    TELE_TO_NOT_LEAVE_COMBAT = 0x04,
    TELE_TO_NOT_UNSUMMON_PET = 0x08,
    TELE_TO_SPELL = 0x10,
    TELE_TO_TRANSPORT_TELEPORT = 0x20
};

/// Type of environmental damages
enum EnviromentalDamage
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING = 1,
    DAMAGE_FALL = 2,
    DAMAGE_LAVA = 3,
    DAMAGE_SLIME = 4,
    DAMAGE_FIRE = 5,
    DAMAGE_FALL_TO_VOID = 6                                 // custom case for fall without durability loss
};

enum PlayerChatTag
{
    CHAT_TAG_NONE = 0x00,
    CHAT_TAG_AFK = 0x01,
    CHAT_TAG_DND = 0x02,
    CHAT_TAG_GM = 0x04,
    CHAT_TAG_COM = 0x08, // Commentator
    CHAT_TAG_DEV = 0x10
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL = 0,
    PLAYED_TIME_LEVEL = 1
};

#define MAX_PLAYED_TIME_INDEX 2

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOAD_FROM,
    PLAYER_LOGIN_QUERY_LOAD_BOUND_INSTANCES,
    PLAYER_LOGIN_QUERY_LOAD_AURAS,
    PLAYER_LOGIN_QUERY_LOAD_SPELLS,
    PLAYER_LOGIN_QUERY_LOAD_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_DAILY_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_REPUTATION,
    PLAYER_LOGIN_QUERY_LOAD_INVENTORY,
    PLAYER_LOGIN_QUERY_LOAD_ACTIONS,
    PLAYER_LOGIN_QUERY_LOAD_MAIL_COUNT,
    PLAYER_LOGIN_QUERY_LOAD_MAIL_DATE,
    PLAYER_LOGIN_QUERY_LOAD_SOCIAL_LIST,
    PLAYER_LOGIN_QUERY_LOAD_HOME_BIND,
    PLAYER_LOGIN_QUERY_LOAD_SPELL_COOLDOWNS,
    PLAYER_LOGIN_QUERY_LOAD_SPELL_CHARGES,
    PLAYER_LOGIN_QUERY_LOAD_DECLINED_NAMES,
    PLAYER_LOGIN_QUERY_LOAD_GUILD,
    PLAYER_LOGIN_QUERY_LOAD_ACHIEVEMENTS,
    PLAYER_LOGIN_QUERY_LOAD_CRITERIA_PROGRESS,
    PLAYER_LOGIN_QUERY_LOAD_EQUIPMENT_SETS,
    PLAYER_LOGIN_QUERY_LOAD_BG_DATA,
    PLAYER_LOGIN_QUERY_LOAD_GLYPHS,
    PLAYER_LOGIN_QUERY_LOAD_TALENTS,
    PLAYER_LOGIN_QUERY_LOAD_ACCOUNT_DATA,
    PLAYER_LOGIN_QUERY_LOAD_SKILLS,
    PLAYER_LOGIN_QUERY_LOAD_WEEKLY_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_RANDOM_BG,
    PLAYER_LOGIN_QUERY_LOAD_WEEKEND_BG,
    PLAYER_LOGIN_QUERY_LOAD_BANNED,
    PLAYER_LOGIN_QUERY_LOAD_QUEST_STATUS_REW,
    PLAYER_LOGIN_QUERY_LOAD_INSTANCE_LOCK_TIMES,
    PLAYER_LOGIN_QUERY_LOAD_SEASONAL_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_MONTHLY_QUEST_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_VOID_STORAGE,
    PLAYER_LOGIN_QUERY_LOAD_CURRENCY,
    PLAYER_LOGIN_QUERY_LOAD_CUF_PROFILES,
    PLAYER_LOGIN_QUERY_LOAD_BATTLE_PETS,
    PLAYER_LOGIN_QUERY_LOAD_BATTLE_PET_SLOTS,
    PLAYER_LOGIN_QUERY_LOAD_QUEST_OBJECTIVE_STATUS,
    PLAYER_LOGIN_QUERY_LOAD_CURRENT_PET_ID,
    PLAYER_LOGIN_QUERY_LOAD_AURA_EFFECT,
    PLAYER_LOGIN_QUERY_LOAD_RESEARCH_DIGSITES,
    PLAYER_LOGIN_QUERY_LOAD_RESEARCH_HISTORY,
    PLAYER_LOGIN_QUERY_LOAD_RESEARCH_PROJECTS,
    PLAYER_LOGIN_QUERY_LOOTLOCKOUTS,
    PLAYER_LOGIN_QUERY_LOAD_SERVICES,
    PLAYER_LOGIN_QUERY_LOAD_COMPLETED_CHALLENGES,
    PLAYER_LOGIN_QUERY_LOAD_BONUS_ROLL,
    PLAYER_LOGIN_QUERY_LOAD_DESERTER_INFO,
    PLAYER_LOGIN_QUERY_LOAD_BATTLGEROUND_STATS,
    PLAYER_LOGIN_QUERY_LOAD_CORPSE_LOCATION,
    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER = 0x01,
    DELAYED_RESURRECT_PLAYER = 0x02,
    DELAYED_SPELL_CAST_DESERTER = 0x04,
    DELAYED_BG_MOUNT_RESTORE = 0x08,                     ///< Flag to restore mount state after teleport from BG
    DELAYED_BG_TAXI_RESTORE = 0x10,                     ///< Flag to restore taxi state after teleport from BG
    DELAYED_BG_GROUP_RESTORE = 0x20,                     ///< Flag to restore group state after teleport from BG
    DELAYED_END
};

// Player summoning auto-decline time (in secs)
#define MAX_PLAYER_SUMMON_DELAY                   (2*MINUTE)
// Maximum money amount : 2^31 - 1
TC_GAME_API extern uint64 const MAX_MONEY_AMOUNT;

struct InstancePlayerBind
{
    InstanceSave* save;
    bool perm;
    /* permanent PlayerInstanceBinds are created in Raid/Heroic instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    InstancePlayerBind() : save(NULL), perm(false)
    { }
};

struct AccessRequirement
{
    uint8  levelMin;
    uint8  levelMax;
    uint32 item;
    uint32 item2;
    uint32 quest_A;
    uint32 quest_H;
    uint32 achievement;
    std::string questFailedText;
};

enum CharDeleteMethod
{
    CHAR_DELETE_REMOVE = 0,                      // Completely remove from the database
    CHAR_DELETE_UNLINK = 1                       // The character gets unlinked from the account,
    // the name gets freed up and appears as deleted ingame
};

enum ReferAFriendError
{
    ERR_REFER_A_FRIEND_NONE = 0x00,
    ERR_REFER_A_FRIEND_NOT_REFERRED_BY = 0x01,
    ERR_REFER_A_FRIEND_TARGET_TOO_HIGH = 0x02,
    ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS = 0x03,
    ERR_REFER_A_FRIEND_TOO_FAR = 0x04,
    ERR_REFER_A_FRIEND_DIFFERENT_FACTION = 0x05,
    ERR_REFER_A_FRIEND_NOT_NOW = 0x06,
    ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I = 0x07,
    ERR_REFER_A_FRIEND_NO_TARGET = 0x08,
    ERR_REFER_A_FRIEND_NOT_IN_GROUP = 0x09,
    ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I = 0x0A,
    ERR_REFER_A_FRIEND_SUMMON_COOLDOWN = 0x0B,
    ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL = 0x0C,
    ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S = 0x0D
};

enum PlayerRestState
{
    REST_STATE_RESTED = 0x01,
    REST_STATE_NOT_RAF_LINKED = 0x02,
    REST_STATE_RAF_LINKED = 0x06
};

enum PlayerCommandStates
{
    CHEAT_NONE = 0x00,
    CHEAT_GOD = 0x01,
    CHEAT_CASTTIME = 0x02,
    CHEAT_COOLDOWN = 0x04,
    CHEAT_POWER = 0x08,
    CHEAT_WATERWALK = 0x10
};

enum ToastTypes
{
    TOAST_TYPE_ITEM         = 0,
    TOAST_TYPE_UNK          = 1,
    TOAST_TYPE_CURRENCY     = 2,
    TOAST_TYPE_MONEY        = 3,
};

enum ToastDispalyTypes
{
    TOAST_DISPLAY_TYPE_ITEM     = 1, // one frame below
    TOAST_DISPLAY_TYPE_CURRENCY = 2, // two frames on top "you recived loot" and "currency name"
    TOAST_DISPLAY_TYPE_UNK      = 3, // some items from timeless isle (looks like 1st type)
};

enum TimerTypes
{
    TIMER_PVP,
    TIMER_CHALLENGE,
    TIMER_SCENARIO,
};

struct CompletedChallenge
{
    CompletedChallenge()
    {
        BestTime = 0;
        LastTime = 0;
        BestMedal = 0;
        BestMedalDate = 0;
    }

    uint32 BestTime;
    uint32 LastTime;
    uint8  BestMedal; // 0 - None, 1 - Bronze, 2 - Silver, 3 - Gold
    uint32 BestMedalDate;
};

typedef std::map<uint32 /*MapId*/, CompletedChallenge> CompletedChallengesMap;



class Player;

/// Holder for Battleground data
struct BGData
{
    BGData() : bgInstanceID(0), bgTypeID(BATTLEGROUND_TYPE_NONE), bgAfkReportedCount(0), bgAfkReportedTimer(0),
        bgTeam(0), mountSpell(0)
    {
        bgQueuesJoinedTime.clear(); ClearTaxiPath();
    }

    uint32 bgInstanceID;                    ///< This variable is set to bg->m_InstanceID,
    ///  when player is teleported to BG - (it is battleground's GUID)
    BattlegroundTypeId bgTypeID;

    std::map<uint32, uint32> bgQueuesJoinedTime;

    uint8              bgAfkReportedCount;
    time_t             bgAfkReportedTimer;

    uint32 bgTeam;                          ///< What side the player will be added to

    uint32 mountSpell;
    uint32 taxiPath[2];
    uint32 taxiLastNode = 0;

    WorldLocation joinPos;                  ///< From where player entered BG

    void ClearTaxiPath()
    {
        taxiPath [0] = taxiPath [1] = 0;
        taxiLastNode = 0;
    }
    bool HasTaxiPath() const
    {
        return taxiPath [0] && taxiPath [1];
    }
};

struct VoidStorageItem
{
    VoidStorageItem()
    {
        ItemId = 0;
        ItemEntry = 0;
        CreatorGuid = ObjectGuid::Empty;
        ItemRandomPropertyId = 0;
        ItemSuffixFactor = 0;
        ItemUpgradeId = 0;
    }

    VoidStorageItem(uint64 id, uint32 entry, ObjectGuid creator, uint32 randomPropertyId, uint32 suffixFactor, uint32 upgradeId)
    {
        ItemId = id;
        ItemEntry = entry;
        CreatorGuid = creator;
        ItemRandomPropertyId = randomPropertyId;
        ItemSuffixFactor = suffixFactor;
        ItemUpgradeId = upgradeId;
    }

    uint64 ItemId;
    uint32 ItemEntry;
    ObjectGuid CreatorGuid;
    uint32 ItemRandomPropertyId;
    uint32 ItemSuffixFactor;
    uint32 ItemUpgradeId;
};

struct ResurrectionData
{
    ObjectGuid GUID;
    WorldLocation Location;
    uint32 Health;
    uint32 Mana;
    uint32 Aura;
};



struct PlayerTalentInfo
{
    PlayerTalentInfo() : UsedTalentCount(0), ResetTalentsCost(0), ResetTalentsTime(0),
        ResetSpecializationCost(0), ActiveSpec(0), SpecsCount(1), ResetSpecializationTime(0)
    {
        for (uint8 i = 0; i < MAX_TALENT_SPECS; ++i)
        {
            SpecInfo [i].Talents = new PlayerTalentMap();
            memset(SpecInfo [i].Glyphs, 0, MAX_GLYPH_SLOT_INDEX * sizeof(uint32));
            SpecInfo [i].TalentTree = SPEC_NONE;
        }
    }

    ~PlayerTalentInfo()
    {
        for (uint8 i = 0; i < MAX_TALENT_SPECS; ++i)
        {
            for (PlayerTalentMap::const_iterator itr = SpecInfo [i].Talents->begin(); itr != SpecInfo [i].Talents->end(); ++itr)
                delete itr->second;
            delete SpecInfo [i].Talents;
        }
    }

    struct TalentSpecInfo
    {
        PlayerTalentMap* Talents;
        uint32 Glyphs [MAX_GLYPH_SLOT_INDEX];
        Specializations TalentTree;
    } SpecInfo [MAX_TALENT_SPECS];

    uint32 UsedTalentCount;
    uint32 ResetTalentsCost;
    time_t ResetTalentsTime;
    uint32 ResetSpecializationCost;
    time_t ResetSpecializationTime;
    uint8 ActiveSpec;
    uint8 SpecsCount;

private:
    PlayerTalentInfo(PlayerTalentInfo const&) = delete;
};

#define RESEARCH_CONTINENT_COUNT    5
#define RESEARCH_BRANCH_COUNT       13
#define MAX_DIGSITES_PER_CONTINENT  4
#define MAX_FINDS_PER_DIGSITE       6

const uint32 ResearchContinents [RESEARCH_CONTINENT_COUNT] = { 0, 1, 530, 571, 870 }; // Eastern Kingdoms, Kalimdor, Outland, Northrend, Pandaria

struct ResearchDigsite
{
    ResearchDigsite(ResearchDigsiteInfo const* digsiteInfo, uint8 remainingFindCount) : _digsiteInfo(digsiteInfo), _archaeologyFind(NULL), _remainingFindCount(remainingFindCount)
    { }

    void SelectNewArchaeologyFind(bool onInit);
    void ChangeArchaeologyFind(ArchaeologyFindInfo const* find)
    {
        _archaeologyFind = find;
    }
    ArchaeologyFindInfo const* GetArchaeologyFind()
    {
        return _archaeologyFind;
    }
    bool IsEmptyDigsite()
    {
        return !_remainingFindCount;
    }

    uint32 GetDigsiteId()
    {
        return _digsiteInfo->digsiteId;
    }
    ResearchDigsiteInfo const* GetDigsiteInfo()
    {
        return _digsiteInfo;
    }
    uint8 GetRemainingFindCount()
    {
        return _remainingFindCount;
    }

    private:
    ResearchDigsiteInfo const* _digsiteInfo;
    ArchaeologyFindInfo const* _archaeologyFind;
    uint8 _remainingFindCount;
};

struct ResearchProjectHistory
{
    uint32 researchCount;
    uint32 firstResearchTimestamp;
};

typedef std::unordered_map<uint32 /*projectId*/, ResearchProjectHistory> ResearchHistoryMap;
typedef std::unordered_map<uint32 /*branchId*/, uint32 /*projectId*/> ResearchProjectMap;

struct LastTargetInfo
{
    uint32 Timer = 0;
    uint32 Priority = 0;
};

enum class GroupSlot : uint8
{
    Original,       // LE_PARTY_CATEGORY_HOME       (home i.e. home realm only)
    Instance,       // LE_PARTY_CATEGORY_INSTANCE
    Max,
};

#define SPELL_VALOR_OF_THE_ANCIENTS 130609

enum class LootLockoutType
{
    PersonalLoot        = 0,    // World bosses + Lfr + flex 
    BonusLoot           = 1,    // Per boss per week per difficult. Actually normal/heroic 10/25 must be one, but since we have all 4 difficulties as separate lockouts for normal loot...
    MoguSeals           = 2,    // Per boss per week. First quest for legendary cloak
    SecretOfTheEmpire   = 3,    // Per boss per week. Second quest for legendary cloak
    TitanRunestone      = 4,    // Per boss per week. Third quest for legendary cloak
    HeirloomWeapon      = 5,    // One boss (Garrosh) per week per difficulty
    Max
};

class TC_GAME_API Player : public Unit, public GridObject<Player>
{
    friend class WorldSession;
    friend class CinematicMgr;
    friend void Item::AddToUpdateQueueOf(Player* player);
    friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
    explicit Player(WorldSession* session);
    ~Player();

    PlayerAI* AI() const { return reinterpret_cast<PlayerAI*>(i_AI); }

    void CleanupsBeforeDelete(bool finalCleanup = true);

    void AddToWorld();
    void RemoveFromWorld();

    void SetObjectScale(float scale)
    {
        Unit::SetObjectScale(scale);
        SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, scale * DEFAULT_PLAYER_BOUNDING_RADIUS);
        SetFloatValue(UNIT_FIELD_COMBAT_REACH, scale * DEFAULT_PLAYER_COMBAT_REACH);
    }

    bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation, uint32 options = 0);
    bool TeleportTo(WorldLocation const &loc, uint32 options = 0);
    bool TeleportToBGEntryPoint();

    void SetSummonPoint(uint32 mapid, float x, float y, float z);
    void SummonIfPossible(bool agree);

    bool Create(uint32 guidlow, CharacterCreateInfo* createInfo);

    void Update(uint32 time);

    static bool BuildEnumData(PreparedQueryResult result, ByteBuffer* dataBuffer, ByteBuffer* bitBuffer, bool boosted = false);

    void SetInWater(bool apply);

    bool IsInAreaTrigger(const AreaTriggerEntry* areaTrigger) const;

    void SendInitialPacketsBeforeAddToMap();
    void SendInitialPacketsAfterAddToMap();
    void SendTransferAborted(uint32 mapid, TransferAbortReason reason, uint8 arg = 0);
    void SendInstanceResetWarning(uint32 mapid, Difficulty difficulty, uint32 time);
    void SendGameError(GameError error, uint32 arg1 = 0, uint32 arg2 = 0);
    void SendAreaTriggerDenied(uint32 trigger, bool entered);

    bool CanInteractWithQuestGiver(Object* questGiver);
    Creature* GetNPCIfCanInteractWith(ObjectGuid guid, uint32 npcflagmask);
    Creature* GetNPCIfCanInteractWithFlag2(ObjectGuid guid, uint32 npcflagmask);
    GameObject* GetGameObjectIfCanInteractWith(ObjectGuid guid, GameobjectTypes type) const;

    void ToggleAFK();
    void ToggleDND();
    bool isAFK() const
    {
        return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_AFK);
    }
    bool isDND() const
    {
        return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_DND);
    }
    uint8 GetChatTag() const;
    std::string autoReplyMsg;

    uint32 GetBarberShopCost(uint8 newhairstyle, uint8 newhaircolor, uint8 newfacialhair, BarberShopStyleEntry const* newSkin = NULL);

    PlayerSocial *GetSocial()
    {
        return m_social;
    }

    PlayerTaxi m_taxi;
    void InitTaxiNodesForLevel()
    {
        m_taxi.InitTaxiNodesForLevel(GetRace(), GetClass(), GetLevel());
    }
    bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, Creature* npc = NULL, uint32 spellid = 0);
    bool ActivateTaxiPathTo(uint32 taxi_path_id, uint32 spellid = 0);
    void CleanupAfterTaxiFlight();
    void ContinueTaxiFlight();
    // flags
    bool HasPlayerFlag(PlayerFlags flags) const
    {
        return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, flags) != 0;
    }
    void SetPlayerFlag(PlayerFlags flags)
    {
        SetFlag(PLAYER_FIELD_PLAYER_FLAGS, flags);
    }
    void RemovePlayerFlag(PlayerFlags flags)
    {
        RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, flags);
    }
    void ReplaceAllPlayerFlags(PlayerFlags flags)
    {
        SetUInt32Value(PLAYER_FIELD_PLAYER_FLAGS, flags);
    }
    // mount_id can be used in scripting calls
    bool isAcceptWhispers() const
    {
        return m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS;
    }
    void SetAcceptWhispers(bool on)
    {
        if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS; else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS;
    }
    bool IsGameMaster() const
    {
        return m_ExtraFlags & PLAYER_EXTRA_GM_ON;
    }
    void SetGameMaster(bool on);
    bool isGMChat() const
    {
        return m_ExtraFlags & PLAYER_EXTRA_GM_CHAT;
    }
    void SetGMChat(bool on)
    {
        if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_CHAT; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_CHAT;
    }
    bool isTaxiCheater() const
    {
        return m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT;
    }
    void SetTaxiCheater(bool on)
    {
        if (on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT; else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT;
    }
    bool isGMVisible() const
    {
        return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE);
    }
    void SetGMVisible(bool on);
    void SetPvPDeath(bool on)
    {
        if (on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH; else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH;
    }

    void GiveXP(uint32 xp, Unit* victim, float group_rate = 1.0f);
    void GiveLevel(uint8 level);
    bool IsMaxLevel() const;

    void InitStatsForLevel(bool reapplyMods = false);
    void RemoveSpecializationSpells();

    // .cheat command related
    bool GetCommandStatus(uint32 command) const
    {
        return _activeCheats & command;
    }
    void SetCommandStatusOn(uint32 command)
    {
        _activeCheats |= command;
    }
    void SetCommandStatusOff(uint32 command)
    {
        _activeCheats &= ~command;
    }

     // PlayedTimeReward
    uint32 ptr_Interval;

    // Played Time Stuff
    time_t m_logintime;
    time_t m_Last_tick;
    uint32 m_Played_time [MAX_PLAYED_TIME_INDEX];
    uint32 GetTotalPlayedTime() const
    {
        return m_Played_time [PLAYED_TIME_TOTAL];
    }
    uint32 GetLevelPlayedTime() const
    {
        return m_Played_time [PLAYED_TIME_LEVEL];
    }

    void setDeathState(DeathState s);                   // overwrite Unit::setDeathState

    float GetRestBonus() const { return m_rest_bonus; }
    void SetRestBonus(float rest_bonus_new);

    bool HasRestFlag(RestFlag restFlag) const { return (_restFlagMask & restFlag) != 0; }
    void SetRestFlag(RestFlag restFlag, uint32 triggerId = 0);
    void RemoveRestFlag(RestFlag restFlag);

    uint32 GetXPRestBonus(uint32 xp);
    uint32 GetInnTriggerId() const { return inn_triggerId; }

    Pet* GetPet() const;
    Pet* SummonPet(uint32 entry, float x, float y, float z, float ang, uint32 despwtime);
    void RemovePet(PetRemoveMode mode, int32 flags = PET_REMOVE_FLAG_NONE);

    PhaseMgr& GetPhaseMgr()
    {
        return phaseMgr;
    }

    /// Handles said message in regular chat based on declared language and in config pre-defined Range.
    void Say(std::string const& text, Language language, WorldObject const* = nullptr) override;
    void Say(uint32 textId, WorldObject const* target = nullptr) override;
    /// Handles yelled message in regular chat based on declared language and in config pre-defined Range.
    void Yell(std::string const& text, Language language, WorldObject const* = nullptr) override;
    void Yell(uint32 textId, WorldObject const* target = nullptr) override;
    /// Outputs an universal text which is supposed to be an action.
    void TextEmote(std::string const& text, WorldObject const* = nullptr, bool = false) override;
    void TextEmote(uint32 textId, WorldObject const* target = nullptr, bool isBossEmote = false) override;
    /// Handles whispers from Addons and players based on sender, receiver's guid and language.
    void Whisper(std::string const& text, Language language, Player* receiver, bool = false) override;
    void Whisper(uint32 textId, Player* target, bool isBossWhisper = false) override;
    void WhisperAddon(std::string const& text, std::string const& prefix, Player* receiver);

    void SendPersonalMessage(std::string const& text, ChatMsg type, Language lang);
    void SendPersonalMessage(uint32 text, ChatMsg type, Language lang)
    {
        SendPersonalMessage(GetSession()->GetTrinityString(text), type, lang);
    };

    /*********************************************************/
    /***                    STORAGE SYSTEM                 ***/
    /*********************************************************/

    void SetVirtualItemSlot(uint8 i, Item* item);
    void SetSheath(SheathState sheathed);             // overwrite Unit version
    uint8 FindEquipSlot(ItemTemplate const* proto, uint32 slot, bool swap) const;
    uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item* skipItem = NULL) const;
    uint32 GetItemCountWithLimitCategory(uint32 limitCategory, Item* skipItem = NULL) const;
    Item* GetItemByGuid(ObjectGuid guid) const;
    Item* GetItemByEntry(uint32 entry) const;
    Item* GetItemByPos(uint16 pos) const;
    Item* GetItemByPos(uint8 bag, uint8 slot) const;
    Item* GetUseableItemByPos(uint8 bag, uint8 slot) const;
    Bag*  GetBagByPos(uint8 slot) const;
    Item* GetWeaponForAttack(WeaponAttackType attackType, bool useable = false) const;
    Item* GetShield(bool useable = false) const;
    static uint8 GetAttackBySlot(uint8 slot);        // MAX_ATTACK if not weapon slot
    std::vector<Item*> &GetItemUpdateQueue()
    {
        return m_itemUpdateQueue;
    }
    static bool IsInventoryPos(uint16 pos)
    {
        return IsInventoryPos(pos >> 8, pos & 255);
    }
    static bool IsInventoryPos(uint8 bag, uint8 slot);
    static bool IsEquipmentPos(uint16 pos)
    {
        return IsEquipmentPos(pos >> 8, pos & 255);
    }
    static bool IsEquipmentPos(uint8 bag, uint8 slot);
    static bool IsBagPos(uint16 pos);
    static bool IsBankPos(uint16 pos)
    {
        return IsBankPos(pos >> 8, pos & 255);
    }
    static bool IsBankPos(uint8 bag, uint8 slot);
    bool IsValidPos(uint16 pos, bool explicit_pos)
    {
        return IsValidPos(pos >> 8, pos & 255, explicit_pos);
    }
    bool IsValidPos(uint8 bag, uint8 slot, bool explicit_pos);
    uint8 GetBankBagSlotCount() const
    {
        return GetByteValue(PLAYER_BYTES_2, 2);
    }
    void SetBankBagSlotCount(uint8 count)
    {
        SetByteValue(PLAYER_BYTES_2, 2, count);
    }
    //BattlePay
    uint64 GetDonateTokens() const;
    void DestroyDonateTokenCount(uint64 count);
    void AddDonateTokenCount(uint64 count);
    bool HasItemCount(uint32 item, uint32 count = 1, bool inBankAlso = false) const;
    bool HasItemFitToSpellRequirements(SpellInfo const* spellInfo, Item const* ignoreItem = NULL) const;
    bool CanNoReagentCast(SpellInfo const* spellInfo) const;
    bool HasItemOrGemWithIdEquipped(uint32 item, uint32 count, uint8 except_slot = NULL_SLOT) const;
    bool HasItemOrGemWithLimitCategoryEquipped(uint32 limitCategory, uint32 count, uint8 except_slot = NULL_SLOT) const;
    InventoryResult CanTakeMoreSimilarItems(Item* pItem) const
    {
        return CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem);
    }
    InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count) const
    {
        return CanTakeMoreSimilarItems(entry, count, NULL);
    }
    InventoryResult CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = NULL) const;
    InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const;
    InventoryResult CanStoreItems(Item** pItem, int count) const;
    InventoryResult CanEquipNewItem(uint8 slot, uint16& dest, uint32 item, bool swap) const;
    InventoryResult CanEquipItem(uint8 slot, uint16& dest, Item* pItem, bool swap, bool not_loading = true) const;

    InventoryResult CanEquipUniqueItem(Item* pItem, uint8 except_slot = NULL_SLOT, uint32 limit_count = 1) const;
    InventoryResult CanEquipUniqueItem(ItemTemplate const* itemProto, uint8 except_slot = NULL_SLOT, uint32 limit_count = 1) const;
    InventoryResult CanUnequipItems(uint32 item, uint32 count) const;
    InventoryResult CanUnequipItem(uint16 src, bool swap) const;
    InventoryResult CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap, bool not_loading = true) const;
    InventoryResult CanUseItem(Item* pItem, bool not_loading = true) const;
    InventoryResult CanUseItem(ItemTemplate const* pItem) const;
    InventoryResult CanRollForItemInLFG(ItemTemplate const* item, WorldObject const* lootedObject) const;
    Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update, int32 randomPropertyId = 0, AllowedLooterSet const& allowedLooters = AllowedLooterSet());
    void  StoreNewItem(Item* item);
    Item* StoreItem(ItemPosCountVec const& pos, Item* pItem, bool update);
    Item* EquipNewItem(uint16 pos, uint32 item, bool update);
    Item* EquipItem(uint16 pos, Item* pItem, bool update);
    uint32 GetItemLevel(Item* item) const;
    void UpdateItemLevel(Item* item);
    void AutoUnequipOffhandIfNeed(bool force = false);
    bool StoreNewItemInBestSlots(uint32 item_id, uint32 item_count);
    void AutoStoreLoot(uint8 bag, uint8 slot, uint32 loot_id, LootStore const& store, ItemPickupSourceType sourceType, uint32 sourceId, bool broadcast = false, uint32 guildPeckChance = 0);
    void AutoStoreLoot(uint32 loot_id, LootStore const& store, ItemPickupSourceType sourceType, uint32 sourceId, bool broadcast = false, uint32 guildPeckChance = 0)
    {
        AutoStoreLoot(NULL_BAG, NULL_SLOT, loot_id, store, sourceType, sourceId, broadcast, guildPeckChance);
    }
    void StoreLootItem(uint8 lootSlot, Loot* loot, Object* src = nullptr, AELootResult* lootResult = nullptr);

    InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item* pItem, uint32* no_space_count = NULL) const;
    InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item* pItem = NULL, bool swap = false, uint32* no_space_count = NULL) const;

    void AddRefundReference(ObjectGuid it);
    void DeleteRefundReference(ObjectGuid it);

    /// send initialization of new currency for client
    void SendNewCurrency(uint32 id) const;
    /// send full data about all currencies to client
    void SendCurrencies() const;
    /// send conquest currency points and their cap week/arena
    void SendPvpRewards() const;
    /// return count of currency witch has plr
    uint32 GetCurrency(uint32 id, bool usePrecision) const;
    /// return count of currency gaind on current week
    uint32 GetCurrencyOnWeek(uint32 id, bool usePrecision) const;
    /// return week cap by currency id
    uint32 GetCurrencyWeekCap(uint32 id, bool usePrecision) const;
    /// return count of currency gaind on current season
    uint32 GetCurrencyOnSeason(uint32 id, bool usePrecision) const;
    /// return presence related currency
    bool HasCurrency(uint32 id, uint32 count) const;
    /// initialize currency count for custom initialization at create character
    void SetCurrency(uint32 id, uint32 count, bool printLog = true);
    void ResetCurrencyWeekCap();
    void ModifyCurrencyFlag(uint32 id, uint8 flag);

    /**
      * @name   ModifyCurrency
      * @brief  Change specific currency and send result to client

      * @param  id currency entry from CurrencyTypes.dbc
      * @param  count integer value for adding/removing curent currency
      * @param  printLog used on SMSG_UPDATE_CURRENCY
      * @param  ignore gain multipliers
      */
    void ModifyCurrency(uint32 id, int32 count, bool printLog = true, bool ignoreMultipliers = false, bool ignoreLimit = false);

    void ApplyEquipCooldown(Item* pItem);
    void QuickEquipItem(uint16 pos, Item* pItem);
    void VisualizeItem(uint8 slot, Item* pItem);
    void SetVisibleItemSlot(uint8 slot, Item* pItem);
    Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update);
    void RemoveItem(uint8 bag, uint8 slot, bool update);
    void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
    // in trade, auction, guild bank, mail....
    void MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
    // in trade, guild bank, mail....
    void RemoveItemDependentAurasAndCasts(Item* pItem);
    void DestroyItem(uint8 bag, uint8 slot, bool update);
    void DestroyItemCount(uint32 item, uint32 count, bool update, bool unequip_check = false);
    void DestroyItemCount(Item* item, uint32& count, bool update);
    void DestroyConjuredItems(bool update);
    void DestroyZoneLimitedItem(bool update, uint32 new_zone);
    void SplitItem(uint16 src, uint16 dst, uint32 count);
    void SwapItem(uint16 src, uint16 dst);
    void AddItemToBuyBackSlot(Item* pItem);
    Item* GetItemFromBuyBackSlot(uint32 slot);
    void RemoveItemFromBuyBackSlot(uint32 slot, bool del);
    void SendEquipError(InventoryResult msg, Item* pItem, Item* pItem2 = NULL, uint32 itemid = 0);
    void SendBuyError(BuyResult msg, Creature* creature, uint32 item, uint32 param);
    void SendSellError(SellResult msg, Creature* creature, ObjectGuid guid);
    void AddWeaponProficiency(uint32 newflag)
    {
        m_WeaponProficiency |= newflag;
    }
    void AddArmorProficiency(uint32 newflag)
    {
        m_ArmorProficiency |= newflag;
    }
    uint32 GetWeaponProficiency() const
    {
        return m_WeaponProficiency;
    }
    uint32 GetArmorProficiency() const
    {
        return m_ArmorProficiency;
    }
    bool IsUseEquipedWeapon(bool mainhand) const;
    bool IsTwoHandUsed() const;
    void SendNewItem(Item* item, uint32 count, bool received, bool created, bool broadcast = false);
    bool BuyItemFromVendorSlot(ObjectGuid vendorguid, uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot);
    bool BuyCurrencyFromVendorSlot(ObjectGuid vendorGuid, uint32 vendorSlot, uint32 currency, uint32 count);
    bool _StoreOrEquipNewItem(uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot, int32 price, ItemTemplate const* pProto, Creature* pVendor, VendorItem const* crItem, bool bStore);

    float GetReputationPriceDiscount(Creature const* creature) const;

    Player* GetTrader() const;
    TradeData* GetTradeData() const { return m_trade; }
    void TradeCancel(bool sendback);

    CinematicMgr* GetCinematicMgr() const { return _cinematicMgr.get(); }

    void UpdateEnchantTime(uint32 time);
    void UpdateSoulboundTradeItems();
    void UpdateItemDuration(uint32 time, bool realtimeonly = false);
    void AddEnchantmentDurations(Item* item);
    void RemoveEnchantmentDurations(Item* item);
    void RemoveArenaEnchantments(EnchantmentSlot slot);
    void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
    void ApplyEnchantment(Item* item, EnchantmentSlot slot, bool apply, bool apply_dur = true, bool ignore_condition = false);
    void ApplyEnchantment(Item* item, bool apply);
    void ApplyReforgeEnchantment(Item* item, bool apply, uint32 ilvl = 0);
    void UpdateSkillEnchantments(uint16 skill_id, uint16 curr_value, uint16 new_value);
    void SendEnchantmentDurations();
    void BuildEnchantmentsInfoData(WorldPacket* data);
    void AddItemDurations(Item* item);
    void RemoveItemDurations(Item* item);
    void SendItemDurations();
    void LoadCorpse(PreparedQueryResult result);
    void LoadPet();

    bool AddItem(uint32 itemId, uint32 count);

    uint32 m_stableSlots;

    /*********************************************************/
    /***                    GOSSIP SYSTEM                  ***/
    /*********************************************************/

    void PrepareGossipMenu(WorldObject* source, uint32 menuId = 0, bool showQuests = false);
    void SendPreparedGossip(WorldObject* source);
    void OnGossipSelect(WorldObject* source, uint32 gossipListId, uint32 menuId);

    uint32 GetGossipTextId(uint32 menuId, WorldObject* source);
    uint32 GetGossipTextId(WorldObject* source);
    static uint32 GetDefaultGossipMenuForSource(WorldObject* source);

    /*********************************************************/
    /***                    QUEST SYSTEM                   ***/
    /*********************************************************/

    int32 GetQuestLevel(Quest const* quest) const
    {
        return quest && (quest->GetQuestLevel() > 0) ? quest->GetQuestLevel() : GetLevel();
    }

    void PrepareQuestMenu(ObjectGuid guid);
    void SendPreparedQuest(WorldObject* source);
    bool IsActiveQuest(uint32 quest_id) const;
    Quest const* GetNextQuest(Object const* questGiver, Quest const* quest) const;
    bool CanSeeStartQuest(Quest const* quest);
    bool CanTakeQuest(Quest const* quest, bool msg);
    bool CanAddQuest(Quest const* quest, bool msg);
    bool CanCompleteQuest(uint32 quest_id);
    bool CanCompleteRepeatableQuest(Quest const* quest);
    bool CanRewardQuest(Quest const* quest, bool msg);
    bool CanRewardQuest(Quest const* quest, uint32 rewardId, bool msg);
    void AddQuestAndCheckCompletion(Quest const* quest, Object* questGiver);
    void AddQuest(Quest const* quest, Object* questGiver);
    void CompleteQuest(uint32 quest_id, bool completely = false, bool fromCommand = false);
    void IncompleteQuest(uint32 quest_id);
    bool CanSelectQuestPackageItem(QuestPackageItemEntry const* questPackageItem) const;
    void RewardQuest(Quest const* quest, uint32 reward, Object* questGiver, bool announce = true);
    void FailQuest(uint32 quest_id);
    bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
    bool SatisfyQuestLevel(Quest const* qInfo, bool msg);
    bool SatisfyQuestLog(bool msg);
    bool SatisfyQuestDependentQuests(Quest const* qInfo, bool msg) const;
    bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg) const;
    bool SatisfyQuestDependentPreviousQuests(Quest const* qInfo, bool msg) const;
    bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
    bool SatisfyQuestRace(Quest const* qInfo, bool msg);
    bool SatisfyQuestReputation(Quest const* qInfo, bool msg);
    bool SatisfyQuestStatus(Quest const* qInfo, bool msg);
    bool SatisfyQuestConditions(Quest const* qInfo, bool msg);
    bool SatisfyQuestTimed(Quest const* qInfo, bool msg);
    bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg);
    bool SatisfyQuestDay(Quest const* qInfo, bool msg);
    bool SatisfyQuestWeek(Quest const* qInfo, bool msg);
    bool SatisfyQuestMonth(Quest const* qInfo, bool msg);
    bool SatisfyQuestSeasonal(Quest const* qInfo, bool msg);
    bool GiveQuestSourceItem(Quest const* quest);
    bool TakeQuestSourceItem(uint32 questId, bool msg);
    bool GetQuestRewardStatus(uint32 quest_id) const;
    QuestStatus GetQuestStatus(uint32 quest_id) const;
    void SetQuestStatus(uint32 quest_id, QuestStatus status, bool update = true);
    QuestGiverStatus GetQuestDialogStatus(Object const* questgiver);
    void RemoveActiveQuest(uint32 quest_id, bool update = true, bool removeFromQuestLog = false);
    void RemoveRewardedQuest(uint32 quest_id, bool update = true);
    void SendQuestUpdate(uint32 questId);
    void DestroyQuestItems(uint32 questId);

    void SetDailyQuestStatus(uint32 quest_id);
    bool IsDailyQuestDone(uint32 quest_id) const;
    void SetWeeklyQuestStatus(uint32 quest_id);
    bool IsWeeklyQuestDone(uint32 quest_id) const;
    void SetMonthlyQuestStatus(uint32 quest_id);
    bool IsMonthlyQuestDone(uint32 quest_id) const;
    void SetSeasonalQuestStatus(uint32 quest_id);
    void ResetDailyQuestStatus();
    void ResetWeeklyQuestStatus();
    void ResetMonthlyQuestStatus();
    void ResetSeasonalQuestStatus(uint16 event_id);

    uint16 FindQuestSlot(uint32 quest_id) const;
    uint32 GetQuestSlotQuestId(uint16 slot) const;
    uint32 GetQuestSlotState(uint16 slot) const;
    uint16 GetQuestSlotCounter(uint16 slot, uint8 counter) const;
    uint32 GetQuestSlotTime(uint16 slot) const;
    void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0);
    void SetQuestSlotCounter(uint16 slot, uint8 counter, uint16 count);
    void SetQuestSlotState(uint16 slot, uint32 state);
    void RemoveQuestSlotState(uint16 slot, uint32 state);
    void SetQuestSlotTimer(uint16 slot, uint32 timer);
    void SwapQuestSlot(uint16 slot1, uint16 slot2);

    uint16 GetReqKillOrCastCurrentCount(uint32 quest_id, int32 entry);
    void AreaExploredOrEventHappens(uint32 questId);
    void GroupEventHappens(uint32 questId, WorldObject const* pEventObject);
    void ItemAddedQuestCheck(uint32 entry, uint32 count);
    void ItemRemovedQuestCheck(uint32 entry, uint32 count);
    void KilledMonster(CreatureTemplate const* cInfo, ObjectGuid guid);
    void KilledMonsterCredit(uint32 entry, ObjectGuid guid = ObjectGuid::Empty, uint32 count = 1);
    void KilledPlayerCredit();
    void KillCreditGO(uint32 entry, ObjectGuid guid = ObjectGuid::Empty);
    void TalkedToCreature(uint32 entry, ObjectGuid guid);
    void MoneyChanged(uint32 value);
    void ReputationChanged(FactionEntry const* factionEntry);
    void ReputationChangedQuestCheck(FactionEntry const* factionEntry);
    bool HasQuestForItem(uint32 itemId) const;
    bool HasQuestForGO(int32 goId) const;
    void UpdateForQuestWorldObjects();
    bool CanShareQuest(uint32 questId) const;
    void QuestObjectiveSatisfy(uint32 objectId, uint32 amount, uint8 type = 0u, uint64 guid = 0u);

    bool IsQuestObjectiveComplete(Quest const* quest, QuestObjective const& objective) const;

    void SendQuestComplete(Quest const* quest);
    void SendQuestComplete(uint32 questId) const;
    void SendQuestReward(Quest const* quest, uint32 XP);
    void SendQuestFailed(uint32 questId, InventoryResult reason = EQUIP_ERR_OK);
    void SendQuestTimerFailed(uint32 questId);
    void SendCanTakeQuestResponse(QuestFailedReason msg) const;
    void SendQuestConfirmAccept(Quest const* quest, Player* pReceiver);
    void SendPushToPartyResponse(Player* player, uint8 msg);
    void SendQuestUpdateAddCredit(Quest const* quest, QuestObjective const* objective, ObjectGuid guid, uint16 oldCount, uint16 addCount);
    void SendQuestUpdateAddCreditSimple(Quest const* quest, QuestObjective const* objective);
    void SendQuestUpdateAddPlayer(Quest const* quest, QuestObjective const* objective, uint16 oldCount, uint16 addCount);
    void SendQuestGiverStatusMultiple();

    ObjectGuid GetDivider() const
    {
        return m_divider;
    }
    void SetDivider(ObjectGuid guid)
    {
        m_divider = guid;
    }

    uint32 GetInGameTime() const
    {
        return m_ingametime;
    }
    void SetInGameTime(uint32 time)
    {
        m_ingametime = time;
    }

    void AddTimedQuest(uint32 questId)
    {
        m_timedquests.insert(questId);
    }
    void RemoveTimedQuest(uint32 questId)
    {
        m_timedquests.erase(questId);
    }

    void SaveCUFProfile(uint8 id, CUFProfile* profile)
    {
        delete _CUFProfiles [id]; _CUFProfiles [id] = profile;
    } ///> Replaces a CUF profile at position 0-4
    CUFProfile* GetCUFProfile(uint8 id) const
    {
        return _CUFProfiles [id];
    } ///> Retrieves a CUF profile at position 0-4
    uint8 GetCUFProfilesCount() const
    {
        uint8 count = 0;
        for (uint8 i = 0; i < MAX_CUF_PROFILES; ++i)
            if (_CUFProfiles [i])
                ++count;
        return count;
    }

    bool HasPvPForcingQuest() const;

    /*********************************************************/
    /***                   LOAD SYSTEM                     ***/
    /*********************************************************/

    bool LoadFromDB(ObjectGuid guid, CharacterDatabaseQueryHolder const& holder);
    bool isBeingLoaded() const;

    void Initialize(ObjectGuid::LowType guid);
    static uint32 GetUInt32ValueFromArray(Tokenizer const& data, uint16 index);
    static float  GetFloatValueFromArray(Tokenizer const& data, uint16 index);
    static uint32 GetZoneIdFromDB(ObjectGuid guid);
    static uint32 GetLevelFromDB(ObjectGuid guid);
    static bool   LoadPositionFromDB(uint32& mapid, float& x, float& y, float& z, float& o, bool& in_flight, ObjectGuid guid);

    static bool IsValidGender(uint8 Gender)
    {
        return Gender <= GENDER_FEMALE;
    }
    static bool IsValidClass(uint8 Class)
    {
        return (1 << (Class - 1)) & CLASSMASK_ALL_PLAYABLE;
    }
    static bool IsValidRace(uint8 Race)
    {
        return (1 << (Race - 1)) & RACEMASK_ALL_PLAYABLE;
    }

    /*********************************************************/
    /***                   SAVE SYSTEM                     ***/
    /*********************************************************/

    void SaveToDB(bool create = false);
    void SaveInventoryAndGoldToDB(CharacterDatabaseTransaction trans);                    // fast save function for item/money cheating preventing
    void SaveGoldToDB(CharacterDatabaseTransaction trans);

    static void SetUInt32ValueInArray(Tokenizer& data, uint16 index, uint32 value);
    static void Customize(ObjectGuid guid, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair);
    static void SavePositionInDB(uint32 mapid, float x, float y, float z, float o, uint32 zone, ObjectGuid guid);

    static void DeleteFromDB(ObjectGuid playerguid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
    static void DeleteOldCharacters(bool startup = false);
    static void DeleteOldCharacters(uint32 keepDays, bool startup);

    bool m_mailsLoaded;
    bool m_mailsUpdated;

    void SetBindPoint(ObjectGuid guid);
    void SendTalentWipeConfirm(ObjectGuid guid, bool resetType);
    void ResetPetTalents();
    void RegenerateAll();
    void Regenerate(Powers power);
    uint32 GetHealthRegenFromItems() const { return m_baseHealthRegen; }
    void setRegenTimerCount(uint32 time)
    {
        m_regenTimerCount = time;
    }
    void setWeaponChangeTimer(uint32 time)
    {
        m_weaponChangeTimer = time;
    }

    uint64 GetMoney() const { return GetUInt64Value(PLAYER_FIELD_COINAGE); }
    bool ModifyMoney(int64 amount, bool sendError = true);
    bool HasEnoughMoney(uint64 amount) const { return (GetMoney() >= amount); }
    bool HasEnoughMoney(int64 amount) const;
    void SetMoney(uint64 value);

    RewardedQuestSet const& getRewardedQuests() const
    {
        return m_RewardedQuests;
    }
    QuestStatusMap& getQuestStatusMap()
    {
        return m_QuestStatus;
    }

    size_t GetRewardedQuestCount() const
    {
        return m_RewardedQuests.size();
    }
    bool IsQuestRewarded(uint32 quest_id) const;

    Unit* GetSelectedUnit() const;
    Player* GetSelectedPlayer() const;

    void SetTarget(ObjectGuid /*guid*/) override { } /// Used for serverside target changes, does not apply to players
        void SetSelection(ObjectGuid guid)
    {
        SetGuidValue(UNIT_FIELD_TARGET, guid);
    }

    uint8 GetComboPoints() const
    {
        return m_comboPoints;
    }
    Unit* GetComboTarget() const
    {
        return m_comboTarget;
    }
    ObjectGuid GetComboTargetGUID() const
    {
        if (m_comboTarget)
            return m_comboTarget->GetGUID();
        return ObjectGuid::Empty;
    }

    void AddComboPoints(Unit* target, int8 count, Spell* spell = NULL);
    void ClearComboPoints();
    void SendComboPoints();

    void SendMailResult(uint32 mailId, MailResponseType mailAction, MailResponseResult mailError, uint32 equipError = 0, uint32 item_guid = 0, uint32 item_count = 0);
    void SendNewMail();
    void UpdateNextMailTimeAndUnreads();
    void AddNewMailDeliverTime(time_t deliver_time);
    bool IsMailsLoaded() const
    {
        return m_mailsLoaded;
    }

    void RemoveMail(uint32 id);

    void AddMail(Mail* mail)
    {
        m_mail.push_front(mail);
    }// for call from WorldSession::SendMailTo
    uint32 GetMailSize()
    {
        return m_mail.size();
    }
    Mail* GetMail(uint32 id);

    PlayerMails::iterator GetMailBegin()
    {
        return m_mail.begin();
    }
    PlayerMails::iterator GetMailEnd()
    {
        return m_mail.end();
    }
    PlayerMails GetMail()
    {
        return m_mail;
    }

    /*********************************************************/
    /*** MAILED ITEMS SYSTEM ***/
    /*********************************************************/

    uint8 unReadMails;
    time_t m_nextMailDelivereTime;

    typedef std::unordered_map<uint32, Item*> ItemMap;

    ItemMap mMitems;                                    //template defined in objectmgr.cpp

    Item* GetMItem(uint32 id);
    void AddMItem(Item* it);
    bool RemoveMItem(uint32 id);

    void SendOnCancelExpectedVehicleRideAura();
    void PetSpellInitialize();
    void CharmSpellInitialize();
    void PossessSpellInitialize();
    void VehicleSpellInitialize();
    void SendRemoveControlBar();
    bool HasSpell(uint32 spell) const;
    bool HasActiveSpell(uint32 spell) const;            // show in spellbook
    TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell) const;
    bool IsSpellFitByClassAndRace(uint32 spell_id) const;
    bool IsNeedCastPassiveSpellAtLearn(SpellInfo const* spellInfo) const;
    bool IsCurrentSpecMasterySpell(SpellInfo const* spellInfo) const;

    void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask);
    void SendInitialSpells();
    void SendUnlearnSpells();
    void SendInitialSetup();
    bool AddSpell(uint32 spellId, bool active, bool learning, bool dependent, bool disabled, bool loading = false);
    void LearnSpell(uint32 spell_id, bool dependent);
    void RemoveSpell(uint32 spell_id, bool disabled = false, bool learn_low_rank = true);
    void ResetSpells(bool myClassOnly = false);
    void LearnDefaultSkills();
    void LearnDefaultSkill(SkillRaceClassInfoEntry const* entry);
    void LearnSpecializationSpells();
    void learnQuestRewardedSpells();
    void learnQuestRewardedSpells(Quest const* quest);
    void learnSpellHighRank(uint32 spellid);
    void AddTemporarySpell(uint32 spellId);
    void RemoveTemporarySpell(uint32 spellId);
    void SetReputation(uint32 factionentry, uint32 value);
    uint32 GetReputation(uint32 factionentry) const;
    std::string GetGuildName();

    // Talents
    uint32 GetUsedTalentCount() const { return _talentMgr->UsedTalentCount; }
    void SetUsedTalentCount(uint32 talents) { _talentMgr->UsedTalentCount = talents; }
    uint32 GetTalentResetCost() const { return _talentMgr->ResetTalentsCost; }
    void SetTalentResetCost(uint32 cost) { _talentMgr->ResetTalentsCost = cost; }
    uint32 GetSpecializationResetCost() const { return _talentMgr->ResetSpecializationCost; }
    void SetSpecializationResetCost(uint32 cost) { _talentMgr->ResetSpecializationCost = cost; }
    uint32 GetSpecializationResetTime() const { return _talentMgr->ResetSpecializationTime; }
    void SetSpecializationResetTime(time_t time_) { _talentMgr->ResetSpecializationTime = time_; }
    uint32 GetTalentResetTime() const { return _talentMgr->ResetTalentsTime; }
    void SetTalentResetTime(time_t time_) { _talentMgr->ResetTalentsTime = time_; }
    // TODO: DEPRECATED
    uint32 GetTalentSpecialization() const { return _talentMgr->SpecInfo[GetActiveSpec()].TalentTree; }
    uint32 GetTalentSpecialization(uint8 spec) const { return _talentMgr->SpecInfo [spec].TalentTree; }

    Specializations GetSpecialization() const { return _talentMgr->SpecInfo[GetActiveSpec()].TalentTree; }
    Specializations GetSpecialization(uint8 spec) const { return _talentMgr->SpecInfo[spec].TalentTree; }
    void SetTalentSpecialization(uint8 spec, Specializations tree) {  _talentMgr->SpecInfo [spec].TalentTree = tree; }
    uint8 GetActiveSpec() const { return _talentMgr->ActiveSpec; }
    void SetActiveSpec(uint8 spec) { _talentMgr->ActiveSpec = spec; }
    uint8 GetSpecsCount() const {  return _talentMgr->SpecsCount; }
    void SetSpecsCount(uint8 count) { _talentMgr->SpecsCount = count; }
    static Roles GetRoleForGroup(uint32 specializationId);
    Roles GetRoleForGroup() const { return GetRoleForGroup(GetSpecialization()); }

    void SendInspectResult(Player const* player);

    bool ResetTalents(bool noCost = false, bool resetTalents = true, bool resetSpecialization = true);
    bool RemoveTalent(uint32 talentId);

    uint32 GetNextResetTalentsCost() const;
    uint32 GetNextResetSpecializationCost() const;
    void InitTalentForLevel();
    void BuildPlayerTalentsInfoData(WorldPacket* data);
    void SendTalentsInfoData();
    bool LearnTalent(uint16 talentId);
    bool AddTalent(uint32 spellId, uint8 spec, bool learning);
    bool HasTalent(uint32 spell_id, uint8 spec) const;
    uint32 CalculateTalentsPoints() const;

    // Dual Spec
    void UpdateSpecCount(uint8 count);
    void ActivateSpec(uint8 spec);

    void InitGlyphsForLevel();
    void SetGlyphSlot(uint8 slot, uint32 slottype)
    {
        SetUInt32Value(PLAYER_FIELD_GLYPH_SLOTS + slot, slottype);
    }

    uint32 GetGlyphSlot(uint8 slot) const
    {
        return GetUInt32Value(PLAYER_FIELD_GLYPH_SLOTS + slot);
    }
    void SetGlyph(uint8 slot, uint32 glyph);
    uint32 GetGlyph(uint8 spec, uint8 slot) const
    {
        return _talentMgr->SpecInfo [spec].Glyphs [slot];
    }

    void ApplyGlyph(uint8 slot, uint32 glyph);
    void UnapplyGlyph(uint8 slot);

    PlayerTalentMap const* GetTalentMap(uint8 spec) const
    {
        return _talentMgr->SpecInfo [spec].Talents;
    }
    PlayerTalentMap* GetTalentMap(uint8 spec)
    {
        return _talentMgr->SpecInfo [spec].Talents;
    }
    ActionButtonList const& GetActionButtons() const
    {
        return m_actionButtons;
    }

    uint32 GetFreePrimaryProfessionPoints() const
    {
        return GetUInt32Value(PLAYER_FIELD_CHARACTER_POINTS);
    }
    void SetFreePrimaryProfessions(uint16 profs)
    {
        SetUInt32Value(PLAYER_FIELD_CHARACTER_POINTS, profs);
    }
    void InitPrimaryProfessions();

    PlayerSpellMap const& GetSpellMap() const
    {
        return m_spells;
    }
    PlayerSpellMap      & GetSpellMap()
    {
        return m_spells;
    }

    void AddSpellMod(SpellModifier* mod, bool apply);
    bool IsAffectedBySpellmod(SpellInfo const* spellInfo, SpellModifier* mod, Spell* spell = NULL);
    template <class T> void ApplySpellMod(uint32 spellId, SpellModOp op, T &basevalue, Spell* spell = NULL);
    void ApplyCostSpellMod(uint32 spellId, int32& baseValue);
    void RemoveSpellMods(Spell* spell);
    void RestoreSpellMods(Spell* spell, uint32 ownerAuraId = 0, Aura* aura = NULL);
    void RestoreAllSpellMods(uint32 ownerAuraId = 0, Aura* aura = NULL);
    void DropModCharge(SpellModifier* mod, Spell* spell);
    void SetSpellModTakingSpell(Spell* spell, bool apply);

    bool HasSpellCooldown(uint32 spell_id) const;
    uint32 GetSpellCooldownDelay(uint32 spell_id) const;
    void AddSpellCooldown(uint32 spellId, uint32 itemId, uint32 mscooldown);
    void AddSpellCooldown(uint32 spellId, TimeValue const& cooldown);
    void AddCustomCooldown(uint32 spellId, TimeValue const& cooldown);
    void ModifySpellCooldown(uint32 spellId, int32 cooldown);
    void SendCooldownEvent(SpellInfo const* spellInfo, uint32 itemId = 0, Spell* spell = nullptr);
    void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs);
    void RemoveSpellCooldown(uint32 spell_id, bool update = false);
    void SendClearCooldown(uint32 spell_id, Unit* target);

    GlobalCooldownMgr& GetGlobalCooldownMgr()
    {
        return m_GlobalCooldownMgr;
    }

    void RemoveArenaSpellCooldowns(bool removeActivePetCooldowns = false);
    void RemoveAllSpellCooldown();
    void SetLastPotionId(uint32 item_id)
    {
        m_lastPotionId = item_id;
    }
    void UpdatePotionCooldown(Spell* spell = NULL);

    void SetResurrectRequestData(Unit* caster, uint32 health, uint32 mana, uint32 appliedAura);
    void ClearResurrectRequestData()
    {
        delete _resurrectionData;
        _resurrectionData = NULL;
    }

    bool IsRessurectRequestedBy(uint64 guid) const
    {
        if (!IsRessurectRequested())
            return false;

        return _resurrectionData->GUID == guid;
    }

    bool IsRessurectRequested() const
    {
        return _resurrectionData != NULL;
    }
    void ResurrectUsingRequestData();

    uint8 getCinematic() const { return m_cinematic; }
    void setCinematic(uint8 cine) { m_cinematic = cine; }

    uint32 GetMovie() const { return m_movie; }
    void SetMovie(uint32 movie) { m_movie = movie; }

    ActionButton* AddActionButton(uint8 button, uint32 action, uint8 type);
    void RemoveActionButton(uint8 button);
    ActionButton const* GetActionButton(uint8 button);
    void SendInitialActionButtons() const
    {
        SendActionButtons(0);
    }
    void SendActionButtons(uint32 state) const;
    bool IsActionButtonDataValid(uint8 button, uint32 action, uint8 type);

    PvPInfo pvpInfo;
    void UpdatePvPState(bool onlyFFA = false);
private:
    // Only sets pvp flag in Unit
    void SetPvP(bool state);
    void StartPvPOffTimer();
    void StopPvPOffTimer();
public:
    //Message BattlePay
    void SendBattlePayMessage(std::string const& n);
    void SendBattlePayMessage(std::string const& n, std::ostringstream const& data);
    // Actaul method to (completely) disables PvP
    void DisablePvP();
    // After something changed, the player hit/healed another player with enabled pvp, or toggled button in UI, or zone changed
    void UpdatePvP(bool state);
    void UpdateZone(uint32 newZone, uint32 newArea);
    void UpdateArea(uint32 newArea);
    void SetNeedsZoneUpdate(bool needsUpdate) { m_needsZoneUpdate = needsUpdate; }

    void UpdateZoneDependentAuras(uint32 zone_id);    // zones
    void UpdateAreaDependentAuras(uint32 area_id);    // subzones

    void UpdateAfkReport(time_t currTime);
    void UpdatePvPFlag(time_t currTime);
    void UpdateContestedPvP(uint32 currTime);
    void SetContestedPvPTimer(uint32 newTime)
    {
        m_contestedPvPTimer = newTime;
    }
    void ResetContestedPvP();

    /** todo: -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.. **/
    DuelInfo* duel;
    void UpdateDuelFlag(time_t currTime);
    void CheckDuelDistance(time_t currTime);
    void DuelComplete(DuelCompleteType type);
    void SendDuelCountdown(uint32 counter);

    bool IsGroupVisibleFor(Player const* p) const;
    bool IsInSameGroupWith(Player const* p) const;
    bool IsInSameRaidWith(Player const* p) const;
    void UninviteFromGroup();
    static void RemoveFromGroup(Group* group, ObjectGuid guid, RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT, ObjectGuid kicker = ObjectGuid::Empty, const char* reason = NULL);
    void RemoveFromGroup(RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT)
    {
        RemoveFromGroup(GetGroup(), GetGUID(), method);
    }
    void SendUpdateToOutOfRangeGroupMembers();

    void SetInGuild(ObjectGuid::LowType guildId);
    void SetRank(uint8 rankId)
    {
        SetUInt32Value(PLAYER_FIELD_GUILD_RANK_ID, rankId);
    }
    uint8 GetRank() const
    {
        return uint8(GetUInt32Value(PLAYER_FIELD_GUILD_RANK_ID));
    }
    void SetGuildLevel(uint32 level)
    {
        SetUInt32Value(PLAYER_FIELD_GUILD_LEVEL, level);
    }
    uint32 GetGuildLevel()
    {
        return GetUInt32Value(PLAYER_FIELD_GUILD_LEVEL);
    }
    void SetGuildIdInvited(uint32 GuildId)
    {
        m_GuildIdInvited = GuildId;
    }
    ObjectGuid::LowType GetGuildId() const
    {
        return GetUInt64Value(OBJECT_FIELD_DATA); /* return only lower part */
    }
    Guild* GetGuild();
    static ObjectGuid::LowType GetGuildIdFromDB(ObjectGuid guid);
    static uint8 GetRankFromDB(ObjectGuid guid);
    int GetGuildIdInvited() { return m_GuildIdInvited; }
    void SetLastGuildInviterGUID(ObjectGuid guid) { m_lastGuildInviterGUID = guid; }
    ObjectGuid GetLastGuildInviterGUID() const { return m_lastGuildInviterGUID; }
    void SendDeclineGuildInvitation(std::string declinerName, bool autoDecline = false);
    static void RemovePetitionsAndSigns(ObjectGuid guid, uint32 type);

    uint32 GetArenaPersonalRating(uint8 slot) const;
    uint32 GetMaxArenaRating() const;
    void SetArenaTeamIdInvited(uint32 ArenaTeamId)
    {
        m_ArenaTeamIdInvited = ArenaTeamId;
    }
    uint32 GetArenaTeamIdInvited()
    {
        return m_ArenaTeamIdInvited;
    }
    Difficulty GetDifficulty(bool isRaid) const
    {
        return isRaid ? m_raidDifficulty : m_dungeonDifficulty;
    }
    Difficulty GetDungeonDifficulty() const
    {
        return m_dungeonDifficulty;
    }
    Difficulty GetRaidDifficulty() const
    {
        return m_raidDifficulty;
    }
    Difficulty GetStoredRaidDifficulty() const
    {
        return m_raidMapDifficulty;
    } // only for use in difficulty packet after exiting to raid map
    void SetDungeonDifficulty(Difficulty dungeon_difficulty)
    {
        m_dungeonDifficulty = dungeon_difficulty;
    }
    void SetRaidDifficulty(Difficulty raid_difficulty)
    {
        m_raidDifficulty = raid_difficulty;
    }
    void StoreRaidMapDifficulty()
    {
        m_raidMapDifficulty = GetMap()->GetDifficulty();
    }

    bool UpdateSkill(uint32 skill_id, uint32 step);
    bool UpdateSkillPro(uint16 skillId, int32 chance, uint32 step);

    bool UpdateCraftSkill(uint32 spellid);
    bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1, WorldObject const* lootObject = nullptr);
    bool UpdateFishingSkill();

    float GetHealthBonusFromStamina();
    float GetManaBonusFromIntellect();

    bool UpdateStats(Stats stat);
    bool UpdateAllStats();
    void ApplySpellPenetrationBonus(int32 amount, bool apply);
    void UpdateResistances(uint32 school);
    void UpdateArmor();
    void UpdateMaxHealth();
    void UpdateMaxPower(Powers power);
    void UpdateAttackPowerAndDamage(bool ranged = false);
    void UpdateDamagePhysical(WeaponAttackType attType);
    void ApplySpellPowerBonus(int32 amount, bool apply);
    void UpdateSpellDamageAndHealingBonus();
    void ApplyRatingMod(CombatRating cr, int32 value, bool apply);
    void UpdateRating(CombatRating cr);
    void UpdateAllRatings();
    void UpdatePowerRegeneration();
    void UpdateMastery();
    void UpdatePvpPower();
    bool CanUseMastery() const;

    void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& min_damage, float& max_damage);

    inline void RecalculateRating(CombatRating cr)
    {
        ApplyRatingMod(cr, 0, true);
    }
    float GetMeleeCritFromAgility();
    float GetSpellCritFromIntellect();
    float OCTRegenMPPerSpirit();
    float GetRatingMultiplier(CombatRating cr) const;
    float GetRatingBonusValue(CombatRating cr) const;

    /// Returns base spellpower bonus from spellpower stat on items, without spellpower from intellect stat
    uint32 GetBaseSpellPowerBonus() const
    {
        return m_baseSpellPower;
    }
    int32 GetSpellPenetrationItemMod() const
    {
        return m_spellPenetrationItemMod;
    }

    float GetExpertiseDodgeOrParryReduction(WeaponAttackType attType) const;
    void UpdateBlockPercentage();
    void UpdateBlockValue();
    void UpdateCritPercentage(WeaponAttackType attType);
    void UpdateAllCritPercentages();
    void UpdateParryPercentage();
    void UpdateDodgePercentage();
    void UpdateMeleeHitChances();
    void UpdateRangedHitChances();
    void UpdateSpellHitChances();

    void UpdateAllSpellCritChances();
    void UpdateSpellCritChance(uint32 school);
    void UpdateArmorPenetration(int32 amount);
    void UpdateExpertise(WeaponAttackType attType);
    void ApplyManaRegenBonus(int32 amount, bool apply);
    void ApplyHealthRegenBonus(int32 amount, bool apply);
    void UpdateManaRegen();
    void UpdateRuneRegen();

    ObjectGuid GetLootGUID() const { return m_lootGuid; }
    void SetLootGUID(ObjectGuid guid) { m_lootGuid = guid; }

    bool HasLfrLockout(uint32 bossEntry) const { return HasLootLockout(LootLockoutType::PersonalLoot, bossEntry, RAID_DIFFICULTY_25MAN_LFR); }
    bool HasLootLockout(LootLockoutType type, uint32 lootedObjectEntry, Difficulty difficulty, bool checkPending = false) const;
    void AddLootLockout(LootLockoutType type, uint32 lootedObjectEntry, Difficulty difficulty, bool pending = true);
    void ClearLootLockouts();

    void RemovedInsignia(Player* looterPlr);

    WorldSession* GetSession() const
    {
        return m_session;
    }

    void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const;
    void DestroyForPlayer(Player* target, bool onDeath = false) const;
    void SendLogXPGain(uint32 GivenXP, Unit* victim, uint32 BonusXP, bool recruitAFriend = false, float group_rate = 1.0f);

    // notifiers
    void SendAttackSwingResult(AttackSwingResult result) const;
    void SendAttackSwingCancelAttack();
    void SendAutoRepeatCancel(Unit* target);
    void SendExplorationExperience(uint32 Area, uint32 Experience);

    void SendDungeonDifficulty(uint32 difficulty);
    void SendRaidDifficulty(uint32 difficulty);
    void ResetInstances(uint8 method, bool isRaid);
    void SendResetInstanceSuccess(uint32 MapId);
    void SendResetInstanceFailed(uint32 reason, uint32 MapId);
    void SendResetFailedNotify(uint32 mapid);

    bool UpdatePosition(float x, float y, float z, float orientation, bool teleport = false) override;
    bool UpdatePosition(const Position &pos, bool teleport = false) override { return UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), teleport); }
    void ProcessTerrainStatusUpdate(ZLiquidStatus oldLiquidStatus, Optional<LiquidData> const& newLiquidData) override;

    void SendMessageToSet(WorldPacket const* data, bool self) const override { SendMessageToSetInRange(data, GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, self); }
    void SendMessageToSetInRange(WorldPacket const* data, float dist, bool self) const override;
    void SendMessageToSetInRange(WorldPacket const* data, float dist, bool self, bool own_team_only, bool required3dDist = false) const;
    void SendMessageToSet(WorldPacket const* data, Player const* skipped_rcvr) const override;

    Corpse* GetCorpse() const;
    void SpawnCorpseBones(bool triggerSave = true);
    Corpse* CreateCorpse();
    void KillPlayer();
    static void OfflineResurrect(ObjectGuid const& guid, CharacterDatabaseTransaction trans);
    bool HasCorpse() const { return _corpseLocation.GetMapId() != MAPID_INVALID; }
    WorldLocation GetCorpseLocation() const { return _corpseLocation; }
    uint32 GetResurrectionSpellId();
    void ResurrectPlayer(float restore_percent, bool applySickness = false);
    void BuildPlayerRepop();
    void RepopAtGraveyard();

    void DurabilityLossAll(double percent, bool inventory);
    void DurabilityLoss(Item* item, double percent);
    void DurabilityPointsLossAll(int32 points, bool inventory);
    void DurabilityPointsLoss(Item* item, int32 points);
    void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
    uint32 DurabilityRepairAll(bool cost, float discountMod, bool guildBank);
    uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod, bool guildBank);

    void UpdateMirrorTimers();
    void StopMirrorTimers();
    bool IsMirrorTimerActive(MirrorTimerType type);

    bool CanJoinConstantChannelInZone(ChatChannelsEntry const* channel, AreaTableEntry const* zone);

    void JoinedChannel(Channel* c);
    void LeftChannel(Channel* c);
    void CleanupChannels();
    void UpdateLocalChannels(uint32 newZone);
    void LeaveLFGChannel();

    void SetSkill(uint16 id, uint16 step, uint16 currVal, uint16 maxVal);
    uint16 GetMaxSkillValue(uint32 skill) const;        // max + perm. bonus + temp bonus
    uint16 GetPureMaxSkillValue(uint32 skill) const;    // max
    uint16 GetSkillValue(uint32 skill) const;           // skill value + perm. bonus + temp bonus
    uint16 GetBaseSkillValue(uint32 skill) const;       // skill value + perm. bonus
    uint16 GetPureSkillValue(uint32 skill) const;       // skill value
    int16 GetSkillPermBonusValue(uint32 skill) const;
    int16 GetSkillTempBonusValue(uint32 skill) const;
    uint16 GetSkillStep(uint16 skill) const;            // 0...6
    bool HasSkill(uint32 skill) const;
    void LearnSkillRewardedSpells(uint32 id, uint32 value);

    WorldLocation& GetTeleportDest()
    {
        return m_teleport_dest;
    }
    bool IsBeingTeleported() const
    {
        return mSemaphoreTeleport_Near || mSemaphoreTeleport_Far;
    }
    bool IsBeingTeleportedNear() const
    {
        return mSemaphoreTeleport_Near;
    }
    bool IsBeingTeleportedFar() const
    {
        return mSemaphoreTeleport_Far;
    }
    void SetSemaphoreTeleportNear(bool semphsetting)
    {
        mSemaphoreTeleport_Near = semphsetting;
    }
    void SetSemaphoreTeleportFar(bool semphsetting)
    {
        mSemaphoreTeleport_Far = semphsetting;
    }
    bool IsForcedTeleportFar() { return m_forcedTeleportFar; }
    void SetForcedTeleportFar(bool forced) { m_forcedTeleportFar = forced; if (forced) SetrSemaphoreTeleportForcedFar(true); }
    bool IsBeingForcedTeleportFar() const { return m_forcedTeleportFarSemaphore; }
    void SetrSemaphoreTeleportForcedFar(bool val) { m_forcedTeleportFarSemaphore = val; }

    void ProcessDelayedOperations();

    void CheckAreaExploreAndOutdoor(void);

    static uint32 TeamForRace(uint8 race);
    uint32 GetTeam() const
    {
        return m_team;
    }
    TeamId GetTeamId() const
    {
        return m_team == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE;
    }
    void setFactionForRace(uint8 race);

    void InitDisplayIds();

    bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
    bool IsAtRecruitAFriendDistance(WorldObject const* pOther) const;
    void RewardPersonalLootAndCurrency(Unit* victim, uint32 lootId = 0);
    void RewardPlayerAndGroupAtKill(Unit* victim, bool isBattleGround);
    void RewardPlayerAndGroupAtEvent(uint32 creature_id, WorldObject* pRewardSource, bool noGuid = false);
    // This function ain't about literally giving xp or honor, it's about procs and such stuff
    bool IsHonorOrXPTarget(Unit const* victim);

    bool GetsRecruitAFriendBonus(bool forXP);
    uint8 GetGrantableLevels()
    {
        return m_grantableLevels;
    }
    void SetGrantableLevels(uint8 val)
    {
        m_grantableLevels = val;
    }

    ReputationMgr&       GetReputationMgr()
    {
        return *m_reputationMgr;
    }
    ReputationMgr const& GetReputationMgr() const
    {
        return *m_reputationMgr;
    }
    ReputationRank GetReputationRank(uint32 faction_id) const;
    void RewardReputation(Unit* victim, float rate);
    void RewardReputation(Quest const* quest);
    void RewardReputationOnChampioning(Unit* victim);

    float CalculateReputationGain(ReputationSource source, uint32 creatureOrQuestLevel, int32 rep, int32 faction, bool noQuestBonus = false);

    void UpdateSkillsForLevel();
    void UpdateSkillsToMaxSkillsForLevel();             // for .levelup
    void ModifySkillBonus(uint32 skillid, int32 val, bool talent);

    BattlePetMgr& GetBattlePetMgr()
    {
        return *m_battlePetMgr;
    }

    // challenge mode
    void _LoadCompletedChallenges(PreparedQueryResult result);
    bool HasChallengeCompleted(uint32 mapId) const
    {
        if (m_CompletedChallenges.find(mapId) == m_CompletedChallenges.end())
            return false;
        return true;
    }
    CompletedChallenge* GetCompletedChallenge(uint32 mapId)
    {
        if (m_CompletedChallenges.find(mapId) == m_CompletedChallenges.end())
            return nullptr;
        return &m_CompletedChallenges[mapId];
    }
    void AddCompletedChallenge(uint32 mapId, CompletedChallenge challenge);
    CompletedChallengesMap GetCompletedChallenges() { return m_CompletedChallenges; }

    // Vignette
    Vignette::Manager& GetVignetteMgr() { return m_VignetteMgr; }

    /*********************************************************/
    /***                  PVP SYSTEM                       ***/
    /*********************************************************/
    // TODO: Properly implement correncies as of Cataclysm
    void UpdateHonorFields();
    bool RewardHonor(Unit* victim, uint32 groupsize, int32 honor = -1, bool pvptoken = false);
    uint32 GetMaxPersonalRatingRequirement(uint32 minarenaslot) const;

    //End of PvP System

    void SetDrunkValue(uint8 newDrunkValue, uint32 itemId = 0);
    uint8 GetDrunkValue() const
    {
        return GetByteValue(PLAYER_BYTES_3, 1);
    }
    static DrunkenState GetDrunkenstateByValue(uint8 value);

    uint32 GetDeathTimer() const { return m_deathTimer; }
    uint32 GetCorpseReclaimDelay(bool pvp) const;
    void UpdateCorpseReclaimDelay();
    int32 CalculateCorpseReclaimDelay(bool load = false) const;
    void SendCorpseReclaimDelay(uint32 delay);

    uint32 GetBlockPercent() const
    {
        return GetUInt32Value(PLAYER_FIELD_SHIELD_BLOCK);
    }
    bool CanParry() const
    {
        return m_canParry;
    }
    void SetCanParry(bool value);
    bool CanBlock() const
    {
        return m_canBlock;
    }
    void SetCanBlock(bool value);
    bool CanTitanGrip() const
    {
        return m_canTitanGrip;
    }
    void SetCanTitanGrip(bool value)
    {
        m_canTitanGrip = value;
    }
    bool CanTameExoticPets() const
    {
        return IsGameMaster() || HasAuraType(SPELL_AURA_ALLOW_TAME_PET_TYPE);
    }

    void SetRegularAttackTime();
    void _ApplyAllStatBonuses();
    void _RemoveAllStatBonuses();

    void ResetAllPowers();

    void _ApplyWeaponDependentAuraMods(Item* item, WeaponAttackType attackType, bool apply);
    void _ApplyWeaponDependentAuraDamageMod(Item* item, WeaponAttackType attackType, AuraEffect const* aura, bool apply);

    void _ApplyItemMods(Item* item, uint8 slot, bool apply);
    void _RemoveAllItemMods();
    void _ApplyAllItemMods();
    void _ApplyAllLevelScaleItemMods(bool apply);
    void _ApplyItemBonuses(Item const* item, uint8 slot, bool apply, bool only_level_scale = false);
    void GetItemDamage(float& minDamage, float& maxDamage, Item const* item) const;
    void ApplyWeaponDamage(uint8 slot, Item const* item, bool apply);
    bool EnchantmentFitsRequirements(uint32 enchantmentcondition, int8 slot);
    void ToggleMetaGemsActive(uint8 exceptslot, bool apply);
    void CorrectMetaGemEnchants(uint8 slot, bool apply);
    void InitDataForForm(bool reapplyMods = false);
    void UpdateAllowedItemLevel();
    void ReapplyItemsBonuses();

    void ApplyItemEquipSpell(Item* item, bool apply, bool form_change = false);
    void ApplyEquipSpell(SpellInfo const* spellInfo, Item* item, bool apply, bool formChange = false, bool itemSet = false);
    void UpdateEquipSpellsAtFormChange();
    void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procAttacker, uint32 procVictim, uint32 procEx, SpellInfo const* procSpell);
    void CastItemUseSpell(Item* item, SpellCastTargets const& targets, uint8 cast_count, uint32 glyphIndex);
    void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procAttacker, uint32 procVictim, uint32 procEx, Item* item, ItemTemplate const* proto, SpellInfo const* procSpell);

    void SendEquipmentSetList();
    void SetEquipmentSet(uint32 index, EquipmentSet eqset);
    void DeleteEquipmentSet(uint64 setGuid);

    void SendInitWorldStates(uint32 zone, uint32 area);
    void SendUpdateWorldState(uint32 Field, uint32 Value);
    void SendDirectMessage(WorldPacket const* data) const;
    void SendBGWeekendWorldStates();
    void SendBattlefieldWorldStates();

    void SendAurasForTarget(Unit* target);

    std::unique_ptr<PlayerMenu> PlayerTalkClass;
    std::vector<ItemSetEffect*> ItemSetEff;

    std::unordered_map<ObjectGuid, ObjectGuid> const& GetLootView() const { return m_lootView; }
    void RemoveLootedObject(ObjectGuid guid);
    void SendLoot(ObjectGuid guid, LootType lootType, bool isAoE = false);
    void SendLootRelease(ObjectGuid guid);
    void SendLootReleaseAll();
    void SendNotifyLootItemRemoved(uint8 lootSlot, ObjectGuid guid);
    void SendNotifyLootMoneyRemoved(ObjectGuid guid);
    void SendDisplayToast(Item* item, uint32 entry, uint32 count, ToastTypes type, ToastDispalyTypes displayType, bool bonusRoll = false);

    /*********************************************************/
    /***               BATTLEGROUND SYSTEM                 ***/
    /*********************************************************/

    bool InBattleground()       const
    {
        return m_bgData.bgInstanceID != 0;
    }
    bool InArena()              const;
    uint32 GetBattlegroundId()  const
    {
        return m_bgData.bgInstanceID;
    }
    BattlegroundTypeId GetBattlegroundTypeId() const
    {
        return m_bgData.bgTypeID;
    }
    Battleground* GetBattleground() const;

    uint32 GetBattlegroundQueueJoinTime(uint32 bgTypeId) const
    {
        return m_bgData.bgQueuesJoinedTime.find(bgTypeId)->second;
    }
    void AddBattlegroundQueueJoinTime(uint32 bgTypeId, uint32 joinTime)
    {
        m_bgData.bgQueuesJoinedTime [bgTypeId] = joinTime;
    }
    void RemoveBattlegroundQueueJoinTime(uint32 bgTypeId)
    {
        m_bgData.bgQueuesJoinedTime.erase(m_bgData.bgQueuesJoinedTime.find(bgTypeId)->second);
    }
    void SetJoinPos(WorldLocation pos)
    {
        m_bgData.joinPos = pos;
    }

    bool InBattlegroundQueue() const;

    BattlegroundQueueTypeId GetBattlegroundQueueTypeId(uint32 index) const;
    uint32 GetBattlegroundQueueIndex(BattlegroundQueueTypeId bgQueueTypeId) const;
    bool IsInvitedForBattlegroundQueueType(BattlegroundQueueTypeId bgQueueTypeId) const;
    bool InBattlegroundQueueForBattlegroundQueueType(BattlegroundQueueTypeId bgQueueTypeId) const;

    void SetBattlegroundId(uint32 val, BattlegroundTypeId bgTypeId);
    uint32 AddBattlegroundQueueId(BattlegroundQueueTypeId val);
    bool HasFreeBattlegroundQueueId();
    void RemoveBattlegroundQueueId(BattlegroundQueueTypeId val);
    void SetInviteForBattlegroundQueueType(BattlegroundQueueTypeId bgQueueTypeId, uint32 instanceId);
    bool IsInvitedForBattlegroundInstance(uint32 instanceId) const;
    WorldLocation const& GetBattlegroundEntryPoint() const
    {
        return m_bgData.joinPos;
    }
    void SetBattlegroundEntryPoint();

    void SetBGTeam(uint32 team);
    Team GetBGTeam() const;
    TeamId GetBGTeamId() const { return (m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam()) == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE; }

    void LeaveBattleground(bool teleportToEntryPoint = true);
    bool CanJoinToBattleground(Battleground const* bg) const;
    bool CanReportAfkDueToLimit();

    bool GetBGAccessByLevel(BattlegroundTypeId bgTypeId) const;
    bool isTotalImmunity();
    bool CanUseBattlegroundObject(GameObject* gameobject);
    bool isTotalImmune();
    bool CanCaptureTowerPoint();

    bool GetRandomWinner() { return m_IsBGRandomWinner; }
    void SetRandomWinner(bool isWinner);
    bool GetBgWeekendWinner() { return m_IsBGWeekendWinner; }
    void SetBgWeekendWinner(bool isWinner);

    /*********************************************************/
    /***               OUTDOOR PVP SYSTEM                  ***/
    /*********************************************************/

    OutdoorPvP* GetOutdoorPvP() const;
    // returns true if the player is in active state for outdoor pvp objective capturing, false otherwise
    bool IsOutdoorPvPActive();

    /*********************************************************/
    /***                    REST SYSTEM                    ***/
    /*********************************************************/

    bool isRested() const
    {
        return GetRestTime() >= 10 * IN_MILLISECONDS;
    }
    uint32 GetRestTime() const
    {
        return _restTime;
    }
    void SetRestTime(uint32 v)
    {
        _restTime = v;
    }

    /*********************************************************/
    /***              ENVIROMENTAL SYSTEM                  ***/
    /*********************************************************/

    bool IsImmuneToEnvironmentalDamage();
    uint32 EnvironmentalDamage(EnviromentalDamage type, uint32 damage);

    /*********************************************************/
    /***               FLOOD FILTER SYSTEM                 ***/
    /*********************************************************/

    void UpdateSpeakTime();
    bool CanSpeak() const;

    /*********************************************************/
    /***                 VARIOUS SYSTEMS                   ***/
    /*********************************************************/
    void UpdateFallInformationIfNeed(MovementInfo const& minfo, uint16 opcode);
    Unit* m_mover;
    WorldObject* m_seer;
    void SetFallInformation(uint32 time, float z);
    void HandleFall(MovementInfo const& movementInfo);

    bool IsKnowHowFlyIn(uint32 mapid, uint32 zone) const;

    void SetClientControl(Unit* target, bool allowMove);

    void SetMover(Unit* target);

    uint64 GetClientMoverGuid() const { return m_clientMoverGuid; }

    void SetSeer(WorldObject* target)
    {
        m_seer = target;
    }
    void SetViewpoint(WorldObject* target, bool apply);
    WorldObject* GetViewpoint() const;
    void StopCastingCharm();
    void StopCastingBindSight();

    uint32 GetSaveTimer() const
    {
        return m_nextSave;
    }
    void   SetSaveTimer(uint32 timer)
    {
        m_nextSave = timer;
    }

    // Recall position
    uint32 m_recallMap;
    float  m_recallX;
    float  m_recallY;
    float  m_recallZ;
    float  m_recallO;
    void   SaveRecallPosition();

    void SetHomebind(WorldLocation const& loc, uint32 areaId);
    void SendBindPointUpdate();

    // Homebind coordinates
    uint32 m_homebindMapId;
    uint16 m_homebindAreaId;
    float m_homebindX;
    float m_homebindY;
    float m_homebindZ;

    WorldLocation GetStartPosition() const;

    // currently visible objects at player client
    typedef std::unordered_set<ObjectGuid> ClientGUIDs;
    ClientGUIDs m_clientGUIDs;

    bool HaveAtClient(WorldObject const* u) const
    {
        return u == this || m_clientGUIDs.find(u->GetGUID()) != m_clientGUIDs.end();
    }
    bool IsNeverVisibleFor(WorldObject const* seer, bool allowServersideObjects = false) const override;

    bool IsVisibleGloballyFor(Player const* player) const;

    void SendInitialVisiblePackets(Unit* target);
    void UpdateVisibilityForPlayer();
    void UpdateVisibilityOf(WorldObject* target);
    void UpdateTriggerVisibility();

    template<class T>
    void UpdateVisibilityOf(T* target, UpdateData& data, std::set<Unit*>& visibleNow);
    void UpdatePhasing();
    void PlayerSendSetPhaseShift(std::set<uint32> const& phaseIds);

    uint8 m_forced_speed_changes [MAX_MOVE_TYPE];

    bool HasAtLoginFlag(AtLoginFlags f) const
    {
        return m_atLoginFlags & f;
    }
    void SetAtLoginFlag(AtLoginFlags f)
    {
        m_atLoginFlags |= f;
    }
    void RemoveAtLoginFlag(AtLoginFlags flags, bool persist = false);

    bool IsUsingLfg(bool inProgressOnly = false);
    bool inRandomLfgDungeon();

    typedef std::set<uint32> DFQuestsDoneList;

    // Temporarily removed pet cache
    uint32 GetTemporaryUnsummonedPetNumber() const
    {
        return m_temporaryUnsummonedPetNumber;
    }
    void SetTemporaryUnsummonedPetNumber(uint32 petnumber)
    {
        m_temporaryUnsummonedPetNumber = petnumber;
    }
    void UnsummonPetTemporaryIfAny();
    void ResummonPetTemporaryUnSummonedIfAny();
    bool IsPetNeedBeTemporaryUnsummoned() const;

    void SendCinematicStart(uint32 CinematicSequenceId);
    void SendMovieStart(uint32 MovieId);

    /*********************************************************/
    /***                 INSTANCE SYSTEM                   ***/
    /*********************************************************/

    typedef std::unordered_map< uint32 /*mapId*/, InstancePlayerBind > BoundInstancesMap;

    void UpdateHomebindTime(uint32 time);

    uint32 m_HomebindTimer;
    bool m_InstanceValid;
    // permanent binds and solo binds by difficulty
    BoundInstancesMap m_boundInstances [MAX_DIFFICULTY];
    InstancePlayerBind* GetBoundInstance(uint32 mapid, Difficulty difficulty);
    BoundInstancesMap& GetBoundInstances(Difficulty difficulty)
    {
        return m_boundInstances [difficulty];
    }
    InstanceSave* GetInstanceSave(uint32 mapid, bool raid);
    void UnbindInstance(uint32 mapid, Difficulty difficulty, bool unload = false);
    void UnbindInstance(BoundInstancesMap::iterator &itr, Difficulty difficulty, bool unload = false);
    InstancePlayerBind* BindToInstance(InstanceSave* save, bool permanent, bool load = false);
    void BindToInstance();
    void SetPendingBind(uint32 instanceId, uint32 bindTimer);
    bool HasPendingBind() const
    {
        return _pendingBindId > 0;
    }
    void SendRaidInfo();
    void SendSavedInstances();
    static void ConvertInstancesToGroup(Player* player, Group* group, bool switchLeader);
    bool Satisfy(AccessRequirement const* ar, uint32 target_map, bool report = false);
    bool CheckInstanceLoginValid();
    bool CheckInstanceCount(uint32 instanceId) const;
    void AddInstanceEnterTime(uint32 instanceId, time_t enterTime);

    /*********************************************************/
    /***                   GROUP SYSTEM                    ***/
    /*********************************************************/

    Group* GetGroupInvite()
    {
        return m_groupInvite;
    }
    void SetGroupInvite(Group* group)
    {
        m_groupInvite = group;
    }
    Group* GetGroup()
    {
        if (Group* group = m_group[uint32(GroupSlot::Instance)].getTarget())
            return group;
        return m_group[uint32(GroupSlot::Original)].getTarget();
    }
    Group const* GetGroup() const
    {
        if (Group const* group = m_group[uint32(GroupSlot::Instance)].getTarget())
            return group;
        return m_group[uint32(GroupSlot::Original)].getTarget();
    }
    Group* GetGroup(GroupSlot slot)
    {
        ASSERT(slot < GroupSlot::Max);
        return m_group[uint32(slot)].getTarget();
    }

    bool IsInGroup(ObjectGuid groupGuid) const;
    void SetGroup(GroupSlot slot, Group* group, int8 subgroup = -1);
    void SetSubGroup(GroupSlot slot, uint8 subgroup)
    {
        ASSERT(slot < GroupSlot::Max);
        return m_group[uint32(slot)].setSubGroup(subgroup);
    }
    uint8 GetSubGroup() const
    {
        if (m_group[uint32(GroupSlot::Instance)].getTarget())
            return m_group[uint32(GroupSlot::Instance)].getSubGroup();
        return m_group[uint32(GroupSlot::Original)].getSubGroup();
    }

    uint32 GetGroupUpdateFlag() const
    {
        return m_groupUpdateMask;
    }
    void SetGroupUpdateFlag(uint32 flag)
    {
        m_groupUpdateMask |= flag;
    }
    uint64 GetAuraUpdateMaskForRaid() const
    {
        return m_auraRaidUpdateMask;
    }
    void SetAuraUpdateMaskForRaid(uint8 slot)
    {
        m_auraRaidUpdateMask |= (uint64(1) << slot);
    }
    Player* GetNextRandomRaidMember(float radius);
    PartyResult CanUninviteFromGroup(std::string const* membername, ObjectGuid memberguid, uint32& val) const;
    void SetReadyCheckTimer(uint32 timer)
    {
        _readyCheckTimer = timer;
    }

    Group* GetOriginalGroup()
    {
        return m_group[uint32(GroupSlot::Original)].getTarget();
    }
    Group const* GetOriginalGroup() const 
    {
        return m_group[uint32(GroupSlot::Original)].getTarget();
    }
    uint8 GetOriginalSubGroup() const
    {
        return m_group[uint32(GroupSlot::Original)].getSubGroup();
    }

    void SetPassOnGroupLoot(bool bPassOnGroupLoot)
    {
        m_bPassOnGroupLoot = bPassOnGroupLoot;
    }
    bool GetPassOnGroupLoot() const
    {
        return m_bPassOnGroupLoot;
    }

    MapReference &GetMapRef()
    {
        return m_mapRef;
    }

    // Set map to player and add reference
    void SetMap(Map* map);
    void ResetMap();

    bool IsAllowedToLoot(Creature const* creature);

    // For internal use only
    bool CanRollForLootIn(WorldObject const* obj) const;

    DeclinedName const* GetDeclinedNames() const
    {
        return m_declinedname;
    }

    uint8 GetRunesState() const { return m_runes->runeState; }
    RuneType GetBaseRune(uint8 index) const { return RuneType(m_runes->runes[index].BaseRune); }
    RuneType GetCurrentRune(uint8 index) const { return RuneType(m_runes->runes[index].CurrentRune); }
    RuneInfo const& GetRune(uint8 index) const { ASSERT(index < MAX_RUNES); return m_runes->runes[index]; }
    uint32 GetRuneConvertSpell(uint8 index) const { return m_runes->runes[index].SpellId; }
    bool HasRuneCooldown(uint8 index) const { return GetRune(index).State != RuneState::Ready; }
    bool IsBaseRuneSlotsOnCooldown(RuneType runeType) const;
    bool IsRunePermanentlyConverted(uint8 index) { return m_runes->runes[index].Permanently; }
    void SetBaseRune(uint8 index, RuneType baseRune) { m_runes->runes[index].BaseRune = baseRune; }
    void SetCurrentRune(uint8 index, RuneType currentRune) { m_runes->runes[index].CurrentRune = currentRune; }
    void ConsumeRune(uint8 index);
    void ActivateRune(uint8 index);
    void SetRuneConvertSpell(uint8 index, uint32 spellId);
    void SetRuneConvertType(uint8 index, bool permanently) { m_runes->runes[index].Permanently = permanently; }
    void AddRuneBySpell(uint8 index, RuneType newType, uint32 spell_id) { SetRuneConvertSpell(index, spell_id); ConvertRune(index, newType); }
    uint8 GetLastUsedRuneMask() { return m_runes->lastUsedRuneMask; }
    void ClearLastUsedRuneMask() { m_runes->lastUsedRuneMask = 0; }
    void SetLastUsedRuneIndex(uint8 index) { m_runes->lastUsedRuneMask |= (1 << index); }
    void RemoveRunesBySpell(uint32 spellid);
    void RestoreBaseRune(uint8 index);
    void ConvertRune(uint8 index, RuneType newType);
    void ResyncRunes(uint8 count);
    void AddRunePower(uint8 index);
    void InitRunes();

    void SendRespondInspectAchievements(Player* player) const;
    uint32 GetAchievementPoints() const;
    bool HasAchieved(uint32 achievementId, bool playerOnly = false) const;
    void ResetAchievements();
    void ResetAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false) ATTR_DEPRECATED;
    void ResetCriterias(CriteriaResetTypes type, uint32 asset);
    void ResetCriterias(CriteriaStartTypes type, uint32 asset);
    void UpdateAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, uint64 miscValue3 = 0, Unit* unit = NULL);
    void StartCriteria(CriteriaStartTypes type, uint32 entry, uint32 timeLost = 0);
    void RemoveTimedAchievement(CriteriaStartTypes type, uint32 entry);
    void CompletedAchievement(AchievementEntry const* entry);
    void RemoveAchievement(AchievementEntry const* entry);

    bool HasTitle(uint32 bitIndex) const;
    bool HasTitle(CharTitlesEntry const* title) const
    {
        return HasTitle(title->bit_index);
    }
    void SetTitle(CharTitlesEntry const* title, bool lost = false);

    //bool isActiveObject() const { return true; }
    bool CanSeeSpellClickOn(Creature const* creature) const;

    uint32 GetChampioningFaction() const
    {
        return m_championingFaction;
    }
    void SetChampioningFaction(uint32 faction, Expansions expansion)
    {
        m_championingFaction = faction;
        m_championingExpansion = expansion;
    }

    Spell* m_spellModTakingSpell;

    float GetAverageItemLevel();
    bool isDebugAreaTriggers;

    void ClearWhisperWhiteList()
    {
        WhisperList.clear();
    }
    void AddWhisperWhiteList(uint64 guid)
    {
        WhisperList.push_back(guid);
    }
    bool IsInWhisperWhiteList(uint64 guid);
    void RemoveFromWhisperWhiteList(uint64 guid)
    {
        WhisperList.remove(guid);
    }

    void ReadMovementInfo(WorldPacket& data, MovementInfo* mi, Movement::ExtraMovementStatusElement* extras = NULL, bool beforeAnticheat = false);
    void SanitizeMovementInfo(MovementInfo* mi, bool afterAnticheat);

    /*! These methods send different packets to the client in apply and unapply case.
        These methods are only sent to the current unit.
        */
    void SendMovementSetCollisionHeight(float height);
    void SendApplyMovementForce(bool apply, Position const &source, float force = 0.0f);

    bool HasForcedMovement() const { return hasForcedMovement_; }

    bool CanFly() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY);}
    bool CanEnterWater() const override { return true; }

    std::string GetMapAreaAndZoneString();
    std::string GetCoordsMapAreaAndZoneString();

    // Void Storage
    bool IsVoidStorageUnlocked() const
    {
        return HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED);
    }
    void UnlockVoidStorage()
    {
        SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED);
    }
    void LockVoidStorage()
    {
        RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_VOID_UNLOCKED);
    }
    uint8 GetNextVoidStorageFreeSlot() const;
    uint8 GetNumOfVoidStorageFreeSlots() const;
    uint8 AddVoidStorageItem(const VoidStorageItem& item);
    void AddVoidStorageItemAtSlot(uint8 slot, const VoidStorageItem& item);
    void DeleteVoidStorageItem(uint8 slot);
    bool SwapVoidStorageItem(uint8 oldSlot, uint8 newSlot);
    VoidStorageItem* GetVoidStorageItem(uint8 slot) const;
    VoidStorageItem* GetVoidStorageItem(uint64 id, uint8& slot) const;

    void ReadyCheckComplete();

    void ShowNeutralPlayerFactionSelectUI();

    uint32 GetQuestObjectiveCounter(uint32 objectiveId) const;

    // Archaeology
    void SaveResearchDigsiteToDB(ResearchDigsite* digsite);
    void DeleteResearchDigsite(ResearchDigsite* digsite);
    void UpdateResearchDigsites();
    void RandomizeDigsites(uint32 map);
    // DO NOT USE, DEBUG ONLY
    bool SpawnDigsite(uint32 digsiteId);
    bool IsWithinResearchDigsite(ResearchDigsite* digsite);
    ResearchDigsite* GetCurrentResearchDigsite();
    ResearchDigsite* TryToSpawnResearchDigsiteOnContinent(uint32 mapId);
    ResearchDigsiteInfo const* GetRandomResearchDigsiteForContinent(uint32 mapId);
    bool IsResearchDigsiteAvailable(ResearchDigsiteInfo const* digsiteInfo);
    void SendResearchHistory();
    void SolveResearchProject(Spell* spell);
    bool HasCompletedResearchProject(uint32 projectId) { return _researchHistory.end() != _researchHistory.find(projectId); }
    bool HasCompletedAllRareProjectsForRace(uint32 researchBranchId);
    bool HasCompletedAllCommonProjectsForRace(uint32 researchBranchId, bool onlyAvailable);
    uint32 GetRandomResearchProjectForRace(uint32 researchBranchId);
    void UpdateResearchProjects();
    void SendSurveryCastInfo(ResearchDigsite* digsite, bool success);

    Creature* GetTransSpirit() { return transcendence_spirit; }
    void SetTransSpirit(Creature* creature) { transcendence_spirit = creature; }

    void setCurrentCustomVendorEntry(uint32 entry) { m_currentCustomVendorEntry = entry; }
    uint32 getCurrentCustomVendorEntry() { return m_currentCustomVendorEntry; }
    void setCurrentEnchanterEntry(uint32 entry) { m_currentEnchanterEntry = entry; }
    uint32 getCurrentEnchanterEntry() { return m_currentEnchanterEntry; }
    uint32 getEnchanterSlot() const { return m_currentEnchanterEntrySlot; }
    void setEnchanterSlot(uint32 slot) { m_currentEnchanterEntrySlot = slot; }

    SceneMgr& GetSceneMgr() { return m_sceneMgr; }
    PlayerAchievementMgr& GetAchievementMgr() { return *m_achievementMgr; }

    // last used pet number (for BG's)
    uint32 GetLastPetNumber(bool withDismissed = false) const;
    void SetLastPetNumber(uint32 petnumber) { m_lastpetnumber = petnumber; }

    uint32 GetCurrentPetId() const { return m_currentPetId; }
    void SetCurrentPetId(uint32 newPetId, CharacterDatabaseTransaction trans = nullptr);
    void AddNewPet(int8 slot, Pet* pet);
    void SwapPets(int8 destSlot, int8 srcSlot);
    uint32 GetPetIdBySlot(uint8 slot) const;
    int8 GetSlotByPetId(uint32 petId) const;
    int8 GetCurrentPetSlot() const { return GetSlotByPetId(GetCurrentPetId()); }
    int8 GetSlotForNewPet() const;
    void LoadPetList();

    void SetLootSpecialization(uint32 specialization);
    uint32 GetLootSpecialization() const { return GetUInt32Value(PLAYER_FIELD_LOOT_SPEC_ID); }
    uint32 GetLootSpecOrClassSpec() const;

    void ClearInCombat() override;

    std::vector<uint8> MakeTradeSkillRecipesData(uint32 skillId) const;

    void ResetEmoteStateIfNeed()
    {
        if (GetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE))
            SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);
    }

    uint32 GetResurrectHealthBonus() const;

    template<class Func>
    bool DoForEachItem(Func func, bool inventory = true, bool bags = true, bool equipment = true, bool bank = true, bool bankBags = true) const;

    bool HasTwoHandWeaponInOneHand() const;
    bool HasGlyph(uint32 spellId) const;
    bool HasItemApplyingAura(uint32 spellId) const;

    bool IsItemFitToSpecialization(ItemTemplate const* proto) const;

    void SendStartTimer(uint32 time, uint32 maxTime, uint8 type);

    void AddToAsisstList(ObjectGuid guid);
    std::map<ObjectGuid, LastTargetInfo> const& GetAssistList() const { return m_lastTargets; }

    void SendTransferMail();

    float GetBonusRollExtraChance() const { return m_bonusRollBonusChance; }
    void SetBonusRollExtraChance(float value);

    std::unordered_map<uint32, std::string> overrideTexts;
    void AddOverrideText(uint32 id, std::string text)
    {
        overrideTexts.emplace(id, text);
    };
    std::string GetOverrideText(uint32 id)
    { 
        auto itr = overrideTexts.find(id);
        return itr != overrideTexts.end() ? itr->second : "";
    };

    void RestoreCombatWithInstance();
    void RestoreCombatWithPlayer(Player* player);

    void SetCanTurnWhileFalling(bool on);

    // For services
    void ChangeClass(uint32 oldClass, uint32 newClass);
    void CleanNotForMyClass(bool talents = true, bool common = false);
    void ResetBothSpec(bool disable = true, bool allTalents = false);
    std::list<Item*> GetAllItems();

    uint32 m_lastTeleportOrChargeTime = 0;
    uint32 m_lastFallAbsoluteTime = 0;

    std::unordered_set<AuctionQueryContext*> m_activeAuctionQueries;

    void LeaveFromSoloQueueIfNeed();
    void UpdateKnockbackTime() { m_lastKnockbackTime = TimeValue::Now(); }
    bool IsImmueToPvpKnockback() const { return TimeValue::Now() < m_lastKnockbackTime + Seconds(10); }

    void UpdateValorOfTheAncients();
    void UpdatePromotionAuras();

    void ApplyDeserter(bool reset = false);
    uint8 GetDeserterMod() { return deserterMod; }
    time_t GetLastDeserterTime() { return lastDeserterTime; }

    void AddTotalBGCount(bool win);
    uint32 GetTotalBGCount() { return totalBGCount; }
    uint32 GetTotalBGWins() { return totalBGWins; }

protected:
    void MarkQuestObjectiveToSave(uint32 questId, uint32 objectiveId)
    {
        m_questObjectiveStatusSave[objectiveId] = true;
        m_QuestStatusSave[questId] = true;
    }

    void ModifyCurrencyMetaConquest(CurrencyTypesEntry const* currency, int32 count, bool printLog);

    // Gamemaster whisper whitelist
    WhisperListContainer WhisperList;
    uint32 m_regenTimerCount;
    int32 m_alternateRegenTimerCount = 0;
    float m_powerFraction [MAX_POWERS_PER_CLASS];
    uint32 m_contestedPvPTimer;

    /*********************************************************/
    /***               BATTLEGROUND SYSTEM                 ***/
    /*********************************************************/

    /*
    this is an array of BG queues (BgTypeIDs) in which is player
    */
    struct BgBattlegroundQueueID_Rec
    {
        BattlegroundQueueTypeId bgQueueTypeId;
        uint32 invitedToInstance;
    };

    BgBattlegroundQueueID_Rec m_bgBattlegroundQueueID [PLAYER_MAX_BATTLEGROUND_QUEUES];
    BGData                    m_bgData;

    bool m_IsBGRandomWinner;
    bool m_IsBGWeekendWinner;

    /*********************************************************/
    /***                    QUEST SYSTEM                   ***/
    /*********************************************************/

    //We allow only one timed quest active at the same time. Below can then be simple value instead of set.
    typedef std::set<uint32> QuestSet;
    typedef std::set<uint32> SeasonalQuestSet;
    typedef std::unordered_map<uint32, SeasonalQuestSet> SeasonalEventQuestMap;
    QuestSet m_timedquests;
    QuestSet m_dailyquests;
    QuestSet m_weeklyquests;
    QuestSet m_monthlyquests;
    SeasonalEventQuestMap m_seasonalquests;

    void SendClearQuestCompletedBits(QuestSet quests);

    ObjectGuid m_divider;
    uint32 m_ingametime;

    /*********************************************************/
    /***                   LOAD SYSTEM                     ***/
    /*********************************************************/

    void _LoadActions(PreparedQueryResult result);
    void _LoadAuras(PreparedQueryResult result, PreparedQueryResult effectResult, uint32 timediff);
    void _LoadGlyphAuras();
    void _LoadBoundInstances(PreparedQueryResult result);
    void _LoadInventory(PreparedQueryResult result, uint32 timeDiff);
    void _LoadVoidStorage(PreparedQueryResult result);
    void _LoadMailInit(PreparedQueryResult resultUnread, PreparedQueryResult resultDelivery);
    void _LoadMail();
    void _LoadMailedItems(Mail* mail);
    void _LoadQuestStatus(PreparedQueryResult result);
    void _LoadQuestObjectiveStatus(PreparedQueryResult result);
    void _LoadQuestStatusRewarded(PreparedQueryResult result);
    void _LoadDailyQuestStatus(PreparedQueryResult result);
    void _LoadWeeklyQuestStatus(PreparedQueryResult result);
    void _LoadMonthlyQuestStatus(PreparedQueryResult result);
    void _LoadSeasonalQuestStatus(PreparedQueryResult result);
    void _LoadRandomBGStatus(PreparedQueryResult result);
    void _LoadWeekendBGStatus(PreparedQueryResult result);
    void _LoadSkills(PreparedQueryResult result);
    void _LoadSpells(PreparedQueryResult result);
    bool _LoadHomeBind(PreparedQueryResult result);
    void _LoadDeclinedNames(PreparedQueryResult result);
    void _LoadEquipmentSets(PreparedQueryResult result);
    void _LoadBGData(PreparedQueryResult result);
    void _LoadGlyphs(PreparedQueryResult result);
    void _LoadTalents(PreparedQueryResult result);
    void _LoadInstanceTimeRestrictions(PreparedQueryResult result);
    void _LoadCurrency(PreparedQueryResult result);
    void _LoadCUFProfiles(PreparedQueryResult result);
    void _LoadResearchHistory(PreparedQueryResult result);
    void _LoadResearchProjects(PreparedQueryResult result);
    void _LoadResearchDigsites(PreparedQueryResult result);
    void _LoadGuildInfo(PreparedQueryResult result);
    void _LoadDeserterInfo(PreparedQueryResult result);
    void _LoadBattlegroundStats(PreparedQueryResult result);

    /*********************************************************/
    /***                   SAVE SYSTEM                     ***/
    /*********************************************************/

    void _SaveActions(CharacterDatabaseTransaction trans);
    void _SaveAuras(CharacterDatabaseTransaction trans);
    void _SaveInventory(CharacterDatabaseTransaction trans);
    void _SaveVoidStorage(CharacterDatabaseTransaction trans);
    void _SaveMail(CharacterDatabaseTransaction trans);
    void _SaveQuestStatus(CharacterDatabaseTransaction trans);
    void _SaveQuestObjectiveStatus(CharacterDatabaseTransaction trans);
    void _SaveDailyQuestStatus(CharacterDatabaseTransaction trans);
    void _SaveWeeklyQuestStatus(CharacterDatabaseTransaction trans);
    void _SaveMonthlyQuestStatus(CharacterDatabaseTransaction trans);
    void _SaveSeasonalQuestStatus(CharacterDatabaseTransaction trans);
    void _SaveSkills(CharacterDatabaseTransaction trans);
    void _SaveSpells(CharacterDatabaseTransaction trans);
    void _SaveEquipmentSets(CharacterDatabaseTransaction trans);
    void _SaveBGData(CharacterDatabaseTransaction trans);
    void _SaveGlyphs(CharacterDatabaseTransaction trans);
    void _SaveTalents(CharacterDatabaseTransaction trans);
    void _SaveStats(CharacterDatabaseTransaction trans);
    void _SaveInstanceTimeRestrictions(CharacterDatabaseTransaction trans);
    void _SaveCurrency(CharacterDatabaseTransaction trans);
    void _SaveCUFProfiles(CharacterDatabaseTransaction trans);
    void _SaveResearchHistory(CharacterDatabaseTransaction trans);
    void _SaveResearchProjects(CharacterDatabaseTransaction trans);
    void _SaveDeserterInfo(CharacterDatabaseTransaction trans);
    void _SaveBattlegroundStats(CharacterDatabaseTransaction trans);

    /*********************************************************/
    /***              ENVIRONMENTAL SYSTEM                 ***/
    /*********************************************************/
    void HandleSobering();
    void SendMirrorTimer(MirrorTimerType Type, uint32 MaxValue, uint32 CurrentValue, int32 Regen);
    void StopMirrorTimer(MirrorTimerType Type);
    void HandleDrowning(uint32 time_diff);
    int32 getMaxTimer(MirrorTimerType timer) const;

    /*********************************************************/
    /***                  HONOR SYSTEM                     ***/
    /*********************************************************/
    time_t m_lastHonorUpdateTime;

    void outDebugValues() const;
    ObjectGuid m_lootGuid;
    std::unordered_map<ObjectGuid, ObjectGuid> m_lootView;

    uint32 m_team;
    uint32 m_nextSave;
    time_t m_speakTime;
    uint32 m_speakCount;
    Difficulty m_dungeonDifficulty;
    Difficulty m_raidDifficulty;
    Difficulty m_raidMapDifficulty;

    uint32 m_atLoginFlags;

    Item* m_items [PLAYER_SLOTS_COUNT];
    uint32 m_currentBuybackSlot;

    PlayerCurrenciesMap _currencyStorage;

    uint32 GetCurrencyWeekCap(CurrencyTypesEntry const* currency) const;
    uint32 GetCurrencyTotalCap(CurrencyTypesEntry const* currency) const;

    /// Updates weekly conquest point cap (dynamic cap)
    void UpdateConquestCurrencyCap(uint32 currency);

    VoidStorageItem* _voidStorageItems [VOID_STORAGE_MAX_SLOT];

    std::vector<Item*> m_itemUpdateQueue;
    bool m_itemUpdateQueueBlocked;

    uint32 m_ExtraFlags;

    Unit* m_comboTarget;
    int8 m_comboPoints;

    QuestStatusMap m_QuestStatus;
    QuestObjectiveStatusMap m_questObjectiveStatus;
    QuestStatusSaveMap m_questObjectiveStatusSave;
    QuestStatusSaveMap m_QuestStatusSave;

    RewardedQuestSet m_RewardedQuests;
    QuestStatusSaveMap m_RewardedQuestsSave;

    boost::dynamic_bitset<uint8> _completedQuestBits;

    SkillStatusMap mSkillStatus;

    uint32 m_GuildIdInvited;
    uint32 m_ArenaTeamIdInvited;

    PlayerMails m_mail;
    PlayerSpellMap m_spells;
    uint32 m_lastPotionId;                              // last used health/mana potion in combat, that block next potion use

    GlobalCooldownMgr m_GlobalCooldownMgr;

    PlayerTalentInfo* _talentMgr;

    ActionButtonList m_actionButtons;

    int32 m_baseRatingValue [CR_MAX_COMBAT_RATING];
    uint32 m_baseSpellPower;
    uint32 m_baseManaRegen;
    uint32 m_baseHealthRegen;
    int32 m_spellPenetrationItemMod;

    SpellModList m_spellMods [MAX_SPELLMOD];

    EnchantDurationList m_enchantDuration;
    ItemDurationList m_itemDuration;
    time_t m_lastSoulboundTradeItemsUpdateTime = 0;

    void ResetTimeSync();
    void SendTimeSync();

    ResurrectionData* _resurrectionData;

    WorldSession* m_session;

    typedef std::list<Channel*> JoinedChannelsList;
    JoinedChannelsList m_channels;

    uint8 m_cinematic;

    uint32 m_movie;

    TradeData* m_trade;

    bool   m_DailyQuestChanged;
    bool   m_WeeklyQuestChanged;
    bool   m_MonthlyQuestChanged;
    bool   m_SeasonalQuestChanged;
    time_t m_lastDailyQuestTime;

    uint32 m_drunkTimer;
    uint32 m_weaponChangeTimer;
    uint32 _readyCheckTimer;

    uint32 m_zoneUpdateId;
    uint32 m_zoneUpdateTimer;
    uint32 m_areaUpdateId;

    uint32 m_deathTimer;
    time_t m_deathExpireTime;

    uint32 m_WeaponProficiency;
    uint32 m_ArmorProficiency;
    bool m_canParry;
    bool m_canBlock;
    bool m_canTitanGrip;
    uint8 m_swingErrorMsg;

    ////////////////////Rest System/////////////////////
    time_t _restTime;
    uint32 inn_triggerId;
    float m_rest_bonus;
    uint32 _restFlagMask;
    ////////////////////Rest System/////////////////////

    // Social
    PlayerSocial *m_social;

    // Groups
    GroupReference m_group[(uint32)GroupSlot::Max];
    Group* m_groupInvite;
    uint32 m_groupUpdateMask;
    uint64 m_auraRaidUpdateMask;
    bool m_bPassOnGroupLoot;

    // last used pet number (for BG's)
    uint32 m_lastpetnumber;

    // Player summoning
    time_t m_summon_expire;
    uint32 m_summon_mapid;
    float  m_summon_x;
    float  m_summon_y;
    float  m_summon_z;

    DeclinedName *m_declinedname;
    Runes *m_runes;
    EquipmentSets m_EquipmentSets;

    bool CanAlwaysSee(WorldObject const* obj) const;

    bool IsAlwaysDetectableFor(WorldObject const* seer) const;

    uint8 m_grantableLevels;

    bool m_needsZoneUpdate;

    CUFProfile* _CUFProfiles [MAX_CUF_PROFILES];

    // Archaeology
    ResearchDigsite* _researchDigsites [RESEARCH_CONTINENT_COUNT] [MAX_DIGSITES_PER_CONTINENT];
    ResearchProjectMap _researchProjects;
    ResearchHistoryMap _researchHistory;

    //WorldFilter
    std::string m_sentMsgCache;

    private:
    // internal common parts for CanStore/StoreItem functions
    InventoryResult CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool swap, Item* pSrcItem) const;
    InventoryResult CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, bool non_specialized, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
    InventoryResult CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
    Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);
    Item* _LoadItem(CharacterDatabaseTransaction trans, uint32 zoneId, uint32 timeDiff, Field* fields);

    std::unique_ptr<CinematicMgr> _cinematicMgr;

    std::set<uint32> m_refundableItems;
    void SendRefundInfo(Item* item);
    void RefundItem(Item* item);
    void SendItemRefundResult(Item* item, ItemExtendedCostEntry const* iece, uint8 error);

    // know currencies are not removed at any point (0 displayed)
    void AddKnownCurrency(uint32 itemId);

    void AdjustQuestReqItemCount(Quest const* quest, QuestStatusData& questStatusData);

    bool IsCanDelayTeleport() const
    {
        return m_bCanDelayTeleport;
    }
    void SetCanDelayTeleport(bool setting)
    {
        m_bCanDelayTeleport = setting;
    }
    bool IsHasDelayedTeleport() const
    {
        return m_bHasDelayedTeleport;
    }
    void SetDelayedTeleportFlag(bool setting)
    {
        m_bHasDelayedTeleport = setting;
    }
    void ScheduleDelayedOperation(uint32 operation)
    {
        if (operation < DELAYED_END) m_DelayedOperations |= operation;
    }

    MapReference m_mapRef;

    uint32 m_lastFallTime;
    float  m_lastFallZ;

    int32 m_MirrorTimer [MAX_TIMERS];
    uint8 m_MirrorTimerFlags;
    uint8 m_MirrorTimerFlagsLast;
    bool m_isInWater;

    // Guid of player who invited player to guild as last
    ObjectGuid m_lastGuildInviterGUID;

    // Current teleport data
    WorldLocation m_teleport_dest;
    uint32 m_teleport_options;
    bool mSemaphoreTeleport_Near;
    bool mSemaphoreTeleport_Far;

    uint32 m_DelayedOperations;
    bool m_bCanDelayTeleport;
    bool m_bHasDelayedTeleport;

    bool m_forcedTeleportFar;
    bool m_forcedTeleportFarSemaphore = false;

    // Temporary removed pet cache
    uint32 m_temporaryUnsummonedPetNumber;

    std::unique_ptr<PlayerAchievementMgr> m_achievementMgr;
    ReputationMgr*  m_reputationMgr;

    uint32 m_championingFaction = 0;
    Expansions m_championingExpansion = EXPANSION_CLASSIC;

    std::queue<uint32> m_timeSyncQueue;

    uint32 m_timeSyncTimer;
    uint32 m_timeSyncClient;
    uint32 m_timeSyncServer;


    InstanceTimeMap _instanceResetTimes;
    uint32 _pendingBindId;
    uint32 _pendingBindTimer;

    uint32 _activeCheats;
    uint32 _maxPersonalArenaRate;

    PhaseMgr phaseMgr;

    BattlePetMgr* m_battlePetMgr;

    Creature* transcendence_spirit;

    uint32 m_currentCustomVendorEntry;
    uint32 m_currentEnchanterEntrySlot;
    uint32 m_currentEnchanterEntry;

    bool hasForcedMovement_;

    SceneMgr m_sceneMgr;
    std::map<int8, PetData> m_petList;
    uint32 m_currentPetId;

    std::unique_ptr<LootLockoutMap> m_lootLockouts;

    CompletedChallengesMap m_CompletedChallenges;

    // Assist stuff
    std::map<ObjectGuid, LastTargetInfo> m_lastTargets;
    uint32 m_assistTimer = 0;

    float m_bonusRollBonusChance = 0.0f;
    uint64 m_clientMoverGuid = 0;
    TimeValue m_lastKnockbackTime;

    MountCapabilityEntry const* m_mountCapability = nullptr;

    // Vignette
    Vignette::Manager m_VignetteMgr;

    uint8 deserterMod = 0;
    time_t lastDeserterTime = 0;

    uint32 totalBGCount = 0;
    uint32 totalBGWins = 0;

    WorldLocation _corpseLocation;
};

void AddItemsSetItem(Player*player, Item* item);
void RemoveItemsSetItem(Player*player, ItemTemplate const* proto);

// "the bodies of template functions must be made available in a header file"
template <class T> void Player::ApplySpellMod(uint32 spellId, SpellModOp op, T& basevalue, Spell* spell)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return;
    float totalmul = 1.0f;
    int32 totalflat = 0;

    // Drop charges for triggering spells instead of triggered ones
    if (m_spellModTakingSpell)
        spell = m_spellModTakingSpell;

    for (SpellModList::iterator itr = m_spellMods [op].begin(); itr != m_spellMods [op].end(); ++itr)
    {
        SpellModifier* mod = *itr;

        // Charges can be set only for mods with auras
        if (!mod->ownerAura)
            ASSERT(mod->charges == 0);

        if (!IsAffectedBySpellmod(spellInfo, mod, spell))
            continue;

        if (mod->type == SPELLMOD_FLAT)
            totalflat += mod->value;
        else if (mod->type == SPELLMOD_PCT)
        {
            // skip percent mods for null basevalue (most important for spell mods with charges)
            if (basevalue == T(0))
                continue;

            // special case (skip > 10sec spell casts for instant cast setting)
            if (mod->op == SPELLMOD_CASTING_TIME && basevalue >= T(10000) && mod->value <= -100)
                continue;

            AddPct(totalmul, mod->value);
        }

        DropModCharge(mod, spell);
    }
    basevalue = T((basevalue + totalflat) * totalmul);
}

template<class Func>
bool Player::DoForEachItem(Func func, bool inventory /*= true*/, bool bags /*= true*/, bool equipment /*= true*/, bool bank /*= true*/, bool bankBags /*= true*/) const
{
    if (inventory)
        for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
            if (Item* item = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                if (func(item))
                    return true;

    if (bags)
        for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Bag* bag = (Bag*)GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                for (uint32 j = 0; j < bag->GetBagSize(); j++)
                    if (Item* item = bag->GetItemByPos(j))
                        if (func(item))
                            return true;

    if (equipment)
        for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_BAG_END; ++i)
            if (Item* item = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                if (func(item))
                    return true;

    if (bank)
        for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; ++i)
            if (Item* item = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                if (func(item))
                    return true;

    if (bankBags)
        for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
            if (Bag* bag = (Bag*)GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                for (uint32 j = 0; j < bag->GetBagSize(); j++)
                    if (Item* item = bag->GetItemByPos(j))
                        if (func(item))
                            return true;

    return false;
}

#endif
