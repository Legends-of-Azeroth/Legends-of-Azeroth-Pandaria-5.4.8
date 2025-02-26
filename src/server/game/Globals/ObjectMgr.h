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

#ifndef SF_OBJECTMGR_H
#define SF_OBJECTMGR_H

#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "TemporarySummon.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "ItemPrototype.h"
#include "IteratorPair.h"
#include "NPCHandler.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "VehicleDefines.h"
#include <string>
#include <map>
#include <unordered_map>
#include <limits>
#include "ConditionMgr.h"
#include <functional>
#include "PhaseMgr.h"
#include "DB2Stores.h"
#include "Containers.h"
#include "ItemSpec.h"
#include <atomic>
#include "Hash.h"

class AreaTrigger;
class Item;
class PhaseMgr;
enum GossipOptionIcon : uint8;
struct AccessRequirement;
struct PlayerLevelInfo;
struct DeclinedName;

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct PageText
{
    std::string Text;
    uint16 NextPage;
};

/// Key for storing temp summon data in TempSummonDataContainer
struct TempSummonGroupKey
{
    TempSummonGroupKey(uint32 summonerEntry, SummonerType summonerType, uint8 group)
        : _summonerEntry(summonerEntry), _summonerType(summonerType), _summonGroup(group)
    {
    }

    bool operator<(TempSummonGroupKey const& rhs) const
    {
        // memcmp is only reliable if struct doesn't have any padding (packed)
        return memcmp(this, &rhs, sizeof(TempSummonGroupKey)) < 0;
    }

private:
    uint32 _summonerEntry;      ///< Summoner's entry
    SummonerType _summonerType; ///< Summoner's type, see SummonerType for available types
    uint8 _summonGroup;         ///< Summon's group id
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

// DB scripting commands
enum ScriptCommands
{
    SCRIPT_COMMAND_TALK                  = 0,                // source/target = Creature, target = any, datalong = talk type (0=say, 1=whisper, 2=yell, 3=emote text, 4=boss emote text), datalong2 & 1 = player talk (instead of creature), dataint = string_id
    SCRIPT_COMMAND_EMOTE                 = 1,                // source/target = Creature, datalong = emote id, datalong2 = 0: set emote state; > 0: play emote state
    SCRIPT_COMMAND_FIELD_SET             = 2,                // source/target = Creature, datalong = field id, datalog2 = value
    SCRIPT_COMMAND_MOVE_TO               = 3,                // source/target = Creature, datalong2 = time to reach, x/y/z = destination
    SCRIPT_COMMAND_FLAG_SET              = 4,                // source/target = Creature, datalong = field id, datalog2 = bitmask
    SCRIPT_COMMAND_FLAG_REMOVE           = 5,                // source/target = Creature, datalong = field id, datalog2 = bitmask
    SCRIPT_COMMAND_TELEPORT_TO           = 6,                // source/target = Creature/Player (see datalong2), datalong = map_id, datalong2 = 0: Player; 1: Creature, x/y/z = destination, o = orientation
    SCRIPT_COMMAND_QUEST_EXPLORED        = 7,                // target/source = Player, target/source = GO/Creature, datalong = quest id, datalong2 = distance or 0
    SCRIPT_COMMAND_KILL_CREDIT           = 8,                // target/source = Player, datalong = creature entry, datalong2 = 0: personal credit, 1: group credit
    SCRIPT_COMMAND_RESPAWN_GAMEOBJECT    = 9,                // source = WorldObject (summoner), datalong = GO guid, datalong2 = despawn delay
    SCRIPT_COMMAND_TEMP_SUMMON_CREATURE  = 10,               // source = WorldObject (summoner), datalong = creature entry, datalong2 = despawn delay, x/y/z = summon position, o = orientation
    SCRIPT_COMMAND_OPEN_DOOR             = 11,               // source = Unit, datalong = GO guid, datalong2 = reset delay (min 15)
    SCRIPT_COMMAND_CLOSE_DOOR            = 12,               // source = Unit, datalong = GO guid, datalong2 = reset delay (min 15)
    SCRIPT_COMMAND_ACTIVATE_OBJECT       = 13,               // source = Unit, target = GO
    SCRIPT_COMMAND_REMOVE_AURA           = 14,               // source (datalong2 != 0) or target (datalong2 == 0) = Unit, datalong = spell id
    SCRIPT_COMMAND_CAST_SPELL            = 15,               // source and/or target = Unit, datalong2 = cast direction (0: s->t 1: s->s 2: t->t 3: t->s 4: s->creature with dataint entry), dataint & 1 = triggered flag
    SCRIPT_COMMAND_PLAY_SOUND            = 16,               // source = WorldObject, target = none/Player, datalong = sound id, datalong2 (bitmask: 0/1=anyone/player, 0/2=without/with distance dependency, so 1|2 = 3 is target with distance dependency)
    SCRIPT_COMMAND_CREATE_ITEM           = 17,               // target/source = Player, datalong = item entry, datalong2 = amount
    SCRIPT_COMMAND_DESPAWN_SELF          = 18,               // target/source = Creature, datalong = despawn delay

    SCRIPT_COMMAND_LOAD_PATH             = 20,               // source = Unit, datalong = path id, datalong2 = is repeatable
    SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT    = 21,               // source = WorldObject (if present used as a search center), datalong = script id, datalong2 = unit lowguid, dataint = script table to use (see ScriptsType)
    SCRIPT_COMMAND_KILL                  = 22,               // source/target = Creature, dataint = remove corpse attribute

    // TrinityCore only
    SCRIPT_COMMAND_ORIENTATION           = 30,               // source = Unit, target (datalong > 0) = Unit, datalong = > 0 turn source to face target, o = orientation
    SCRIPT_COMMAND_EQUIP                 = 31,               // soucre = Creature, datalong = equipment id
    SCRIPT_COMMAND_MODEL                 = 32,               // source = Creature, datalong = model id
    SCRIPT_COMMAND_CLOSE_GOSSIP          = 33,               // source = Player
    SCRIPT_COMMAND_PLAYMOVIE             = 34                // source = Player, datalong = movie id
};

// Benchmarked: Faster than std::unordered_map (insert/find)
typedef std::map<uint32, PageText> PageTextContainer;

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint16, InstanceTemplate> InstanceTemplateContainer;

struct GameTele
{
    float  position_x;
    float  position_y;
    float  position_z;
    float  orientation;
    uint32 mapId;
    std::string name;
    std::wstring wnameLow;
};

typedef std::unordered_map<uint32, GameTele > GameTeleContainer;

enum ScriptsType
{
    SCRIPTS_FIRST = 1,

    SCRIPTS_SPELL = SCRIPTS_FIRST,
    SCRIPTS_EVENT,
    SCRIPTS_WAYPOINT,

    SCRIPTS_LAST
};

enum eScriptFlags
{
    // Talk Flags
    SF_TALK_USE_PLAYER          = 0x1,

    // Emote flags
    SF_EMOTE_USE_STATE          = 0x1,

    // TeleportTo flags
    SF_TELEPORT_USE_CREATURE    = 0x1,

    // KillCredit flags
    SF_KILLCREDIT_REWARD_GROUP  = 0x1,

    // RemoveAura flags
    SF_REMOVEAURA_REVERSE       = 0x1,

    // CastSpell flags
    SF_CASTSPELL_SOURCE_TO_TARGET = 0,
    SF_CASTSPELL_SOURCE_TO_SOURCE = 1,
    SF_CASTSPELL_TARGET_TO_TARGET = 2,
    SF_CASTSPELL_TARGET_TO_SOURCE = 3,
    SF_CASTSPELL_SEARCH_CREATURE  = 4,
    SF_CASTSPELL_TRIGGERED      = 0x1,

    // PlaySound flags
    SF_PLAYSOUND_TARGET_PLAYER  = 0x1,
    SF_PLAYSOUND_DISTANCE_SOUND = 0x2,

    // Orientation flags
    SF_ORIENTATION_FACE_TARGET  = 0x1
};

struct ScriptInfo
{
    ScriptsType type;
    uint32 id;
    uint32 delay;
    ScriptCommands command;

    union
    {
        struct
        {
            uint32 nData[3];
            float  fData[4];
        } Raw;

        struct                      // SCRIPT_COMMAND_TALK (0)
        {
            uint32 ChatType;        // datalong
            uint32 Flags;           // datalong2
            int32  TextID;          // dataint
        } Talk;

        struct                      // SCRIPT_COMMAND_EMOTE (1)
        {
            uint32 EmoteID;         // datalong
            uint32 Flags;           // datalong2
        } Emote;

        struct                      // SCRIPT_COMMAND_FIELD_SET (2)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FieldSet;

        struct                      // SCRIPT_COMMAND_MOVE_TO (3)
        {
            uint32 Unused1;         // datalong
            uint32 TravelTime;      // datalong2
            int32  Unused2;         // dataint

            float DestX;
            float DestY;
            float DestZ;
        } MoveTo;

        struct                      // SCRIPT_COMMAND_FLAG_SET (4)
                                    // SCRIPT_COMMAND_FLAG_REMOVE (5)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FlagToggle;

        struct                      // SCRIPT_COMMAND_TELEPORT_TO (6)
        {
            uint32 MapID;           // datalong
            uint32 Flags;           // datalong2
            int32  Unused1;         // dataint

            float DestX;
            float DestY;
            float DestZ;
            float Orientation;
        } TeleportTo;

        struct                      // SCRIPT_COMMAND_QUEST_EXPLORED (7)
        {
            uint32 QuestID;         // datalong
            uint32 Distance;        // datalong2
        } QuestExplored;

        struct                      // SCRIPT_COMMAND_KILL_CREDIT (8)
        {
            uint32 CreatureEntry;   // datalong
            uint32 Flags;           // datalong2
        } KillCredit;

        struct                      // SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (9)
        {
            uint32 GOGuid;          // datalong
            uint32 DespawnDelay;    // datalong2
        } RespawnGameobject;

        struct                      // SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (10)
        {
            uint32 CreatureEntry;   // datalong
            uint32 DespawnDelay;    // datalong2
            int32  Unused1;         // dataint

            float PosX;
            float PosY;
            float PosZ;
            float Orientation;
        } TempSummonCreature;

        struct                      // SCRIPT_COMMAND_CLOSE_DOOR (12)
                                    // SCRIPT_COMMAND_OPEN_DOOR (11)
        {
            uint32 GOGuid;          // datalong
            uint32 ResetDelay;      // datalong2
        } ToggleDoor;

                                    // SCRIPT_COMMAND_ACTIVATE_OBJECT (13)

        struct                      // SCRIPT_COMMAND_REMOVE_AURA (14)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
        } RemoveAura;

        struct                      // SCRIPT_COMMAND_CAST_SPELL (15)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
            int32  CreatureEntry;   // dataint

            float SearchRadius;
        } CastSpell;

        struct                      // SCRIPT_COMMAND_PLAY_SOUND (16)
        {
            uint32 SoundID;         // datalong
            uint32 Flags;           // datalong2
        } PlaySound;

        struct                      // SCRIPT_COMMAND_CREATE_ITEM (17)
        {
            uint32 ItemEntry;       // datalong
            uint32 Amount;          // datalong2
        } CreateItem;

        struct                      // SCRIPT_COMMAND_DESPAWN_SELF (18)
        {
            uint32 DespawnDelay;    // datalong
        } DespawnSelf;

        struct                      // SCRIPT_COMMAND_LOAD_PATH (20)
        {
            uint32 PathID;          // datalong
            uint32 IsRepeatable;    // datalong2
        } LoadPath;

        struct                      // SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT (21)
        {
            uint32 CreatureEntry;   // datalong
            uint32 ScriptID;        // datalong2
            uint32 ScriptType;      // dataint
        } CallScript;

        struct                      // SCRIPT_COMMAND_KILL (22)
        {
            uint32 Unused1;         // datalong
            uint32 Unused2;         // datalong2
            int32  RemoveCorpse;    // dataint
        } Kill;

        struct                      // SCRIPT_COMMAND_ORIENTATION (30)
        {
            uint32 Flags;           // datalong
            uint32 Unused1;         // datalong2
            int32  Unused2;         // dataint

            float Unused3;
            float Unused4;
            float Unused5;
            float Orientation;
        } Orientation;

        struct                      // SCRIPT_COMMAND_EQUIP (31)
        {
            uint32 EquipmentID;     // datalong
        } Equip;

        struct                      // SCRIPT_COMMAND_MODEL (32)
        {
            uint32 ModelID;         // datalong
        } Model;

                                    // SCRIPT_COMMAND_CLOSE_GOSSIP (33)

        struct                      // SCRIPT_COMMAND_PLAYMOVIE (34)
        {
            uint32 MovieID;         // datalong
        } PlayMovie;
    };

    std::string GetDebugInfo() const;
};

typedef std::multimap<uint32, ScriptInfo> ScriptMap;
typedef std::map<uint32, ScriptMap > ScriptMapMap;
typedef std::multimap<uint32, uint32> SpellScriptsContainer;
typedef std::pair<SpellScriptsContainer::iterator, SpellScriptsContainer::iterator> SpellScriptsBounds;
extern ScriptMapMap sSpellScripts;
extern ScriptMapMap sEventScripts;
extern ScriptMapMap sWaypointScripts;

std::string GetScriptsTableNameByType(ScriptsType type);
ScriptMapMap* GetScriptsMapByType(ScriptsType type);
std::string GetScriptCommandName(ScriptCommands command);

struct SpellClickInfo
{
    uint32 spellId;
    uint8 castFlags;
    SpellClickUserTypes userType;

    // helpers
    bool IsFitToRequirements(Unit const* clicker, Unit const* clickee) const;
};

typedef std::multimap<uint32, SpellClickInfo> SpellClickInfoContainer;
typedef std::pair<SpellClickInfoContainer::const_iterator, SpellClickInfoContainer::const_iterator> SpellClickInfoMapBounds;

struct AreaTriggerStruct
{
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

struct BroadcastText
{
    BroadcastText() : Id(0), LanguageID(0), EmoteId1(0), EmoteId2(0), EmoteId3(0),
        EmoteDelay1(0), EmoteDelay2(0), EmoteDelay3(0), SoundEntriesID(0), EmotesID(0), Flags(0)
    {
        Text.resize(DEFAULT_LOCALE + 1);
        Text1.resize(DEFAULT_LOCALE + 1);
    }

    uint32 Id;
    uint32 LanguageID;
    std::vector<std::string> Text;
    std::vector<std::string> Text1;
    uint32 EmoteId1;
    uint32 EmoteId2;
    uint32 EmoteId3;
    uint32 EmoteDelay1;
    uint32 EmoteDelay2;
    uint32 EmoteDelay3;
    uint32 SoundEntriesID;
    uint32 EmotesID;
    uint32 Flags;
    // uint32 VerifiedBuild;

    std::string const& GetText(LocaleConstant locale = DEFAULT_LOCALE, uint8 gender = GENDER_MALE, bool forceGender = false) const
    {
        if ((gender == GENDER_FEMALE || gender == GENDER_NONE) && (forceGender || !Text1[DEFAULT_LOCALE].empty()))
        {
            if (Text1.size() > size_t(locale) && !Text1[locale].empty())
                return Text1[locale];
            return Text1[DEFAULT_LOCALE];
        }
        // else if (gender == GENDER_MALE)
        {
            if (Text.size() > size_t(locale) && !Text[locale].empty())
                return Text[locale];
            return Text[DEFAULT_LOCALE];
        }
    }
};

typedef std::unordered_map<uint32, BroadcastText> BroadcastTextContainer;

typedef std::set<uint32> CellGuidSet;
struct CellObjectGuids
{
    CellGuidSet creatures;
    CellGuidSet gameobjects;
};
typedef std::unordered_map<uint32/*cell_id*/, CellObjectGuids> CellObjectGuidsMap;
typedef std::unordered_map<uint32/*(mapid, spawnMode) pair*/, CellObjectGuidsMap> MapObjectGuids;

// Trinity string ranges
#define MIN_TRINITY_STRING_ID           1                    // 'trinity_string'
#define MAX_TRINITY_STRING_ID           2000000000
#define MIN_CREATURE_AI_TEXT_STRING_ID (-1)                 // 'creature_ai_texts'
#define MAX_CREATURE_AI_TEXT_STRING_ID (-1000000)

// Trinity Trainer Reference start range
#define TRINITY_TRAINER_START_REF      200000

struct TrinityStringLocale
{
    std::vector<std::string> Content;
};

typedef std::map<ObjectGuid, ObjectGuid> LinkedRespawnContainer;
typedef std::unordered_map<uint32, CreatureData> CreatureDataContainer;
typedef std::unordered_map<uint32, GameObjectData> GameObjectDataContainer;
typedef std::map<TempSummonGroupKey, std::vector<TempSummonData> > TempSummonDataContainer;
typedef std::unordered_map<uint32, CreatureLocale> CreatureLocaleContainer;
typedef std::unordered_map<uint32, GameObjectLocale> GameObjectLocaleContainer;
typedef std::unordered_map<uint32, ItemLocale> ItemLocaleContainer;
typedef std::unordered_map<uint32, QuestTemplateLocale> QuestTemplateLocaleContainer;
typedef std::unordered_map<uint32, QuestObjectivesLocale> QuestObjectivesLocaleContainer;
typedef std::unordered_map<uint32, QuestOfferRewardLocale> QuestOfferRewardLocaleContainer;
typedef std::unordered_map<uint32, QuestRequestItemsLocale> QuestRequestItemsLocaleContainer;
typedef std::unordered_map<uint32, NpcTextLocale> NpcTextLocaleContainer;
typedef std::unordered_map<uint32, PageTextLocale> PageTextLocaleContainer;
typedef std::unordered_map<int32, TrinityStringLocale> TrinityStringLocaleContainer;
//typedef std::unordered_map<uint32, GossipMenuItemsLocale> GossipMenuItemsLocaleContainer;

struct GossipMenuItemsLocale
{
    std::vector<std::string> OptionText;
    std::vector<std::string> BoxText;
};

typedef std::unordered_map<std::pair<uint32, uint32>, GossipMenuItemsLocale> GossipMenuItemsLocaleContainer;
typedef std::unordered_map<uint32, PointOfInterestLocale> PointOfInterestLocaleContainer;
typedef std::unordered_map<uint32, QuestObjectivesLocale> QuestObjectivesLocaleContainer;

struct PetLevelInfo
{
    PetLevelInfo() { for (uint8 i=0; i < MAX_STATS; ++i) stats[i] = 0; }

    uint16 stats[MAX_STATS];
    uint16 health = 0;
    uint16 mana = 0;
    uint16 armor = 0;
};

struct MailLevelReward
{
    MailLevelReward() : raceMask(0), mailTemplateId(0), senderEntry(0) { }
    MailLevelReward(uint32 _raceMask, uint32 _mailTemplateId, uint32 _senderEntry) : raceMask(_raceMask), mailTemplateId(_mailTemplateId), senderEntry(_senderEntry) { }

    uint32 raceMask;
    uint32 mailTemplateId;
    uint32 senderEntry;
};

typedef std::list<MailLevelReward> MailLevelRewardList;
typedef std::unordered_map<uint8, MailLevelRewardList> MailLevelRewardContainer;

// We assume the rate is in general the same for all three types below, but chose to keep three for scalability and customization
struct RepRewardRate
{
    float questRate;            // We allow rate = 0.0 in database. For this case, it means that
    float questDailyRate;
    float questWeeklyRate;
    float questMonthlyRate;
    float questRepeatableRate;
    float creatureRate;         // no reputation are given at all for this faction/rate type.
    float spellRate;
};

struct ReputationOnKillEntry
{
    uint32 RepFaction1;
    uint32 RepFaction2;
    uint32 ReputationMaxCap1;
    int32 RepValue1;
    uint32 ReputationMaxCap2;
    int32 RepValue2;
    bool IsTeamAward1;
    bool IsTeamAward2;
    bool TeamDependent;
};

struct RepSpilloverTemplate
{
    uint32 faction[MAX_SPILLOVER_FACTIONS];
    float faction_rate[MAX_SPILLOVER_FACTIONS];
    uint32 faction_rank[MAX_SPILLOVER_FACTIONS];
};

struct PointOfInterest
{
    uint32 ID;
    float PositionX;
    float PositionY;
    uint32 Icon;
    uint32 Flags;
    uint32 Importance;
    std::string Name;
};

struct GossipMenuItems
{
    uint32              MenuID;
    uint32              OptionID;
    GossipOptionIcon    OptionIcon;
    std::string         OptionText;
    uint32              OptionBroadcastTextID;
    uint32              OptionType;
    uint32              OptionNpcFlag;
    uint32              ActionMenuID;
    uint32              ActionPoiID;
    bool                BoxCoded;
    uint32              BoxMoney;
    std::string         BoxText;
    uint32              BoxBroadcastTextID;
    ConditionContainer  Conditions;
};

struct GossipMenus
{
    uint32              MenuID;
    uint32              TextID;
    ConditionContainer  Conditions;
};

typedef std::multimap<uint32, GossipMenus> GossipMenusContainer;
typedef std::pair<GossipMenusContainer::const_iterator, GossipMenusContainer::const_iterator> GossipMenusMapBounds;
typedef std::pair<GossipMenusContainer::iterator, GossipMenusContainer::iterator> GossipMenusMapBoundsNonConst;
typedef std::multimap<uint32, GossipMenuItems> GossipMenuItemsContainer;
typedef std::pair<GossipMenuItemsContainer::const_iterator, GossipMenuItemsContainer::const_iterator> GossipMenuItemsMapBounds;
typedef std::pair<GossipMenuItemsContainer::iterator, GossipMenuItemsContainer::iterator> GossipMenuItemsMapBoundsNonConst;

struct QuestPOIBlobPoint
{
    int32 X;
    int32 Y;

    QuestPOIBlobPoint() : X(0), Y(0) { }
    QuestPOIBlobPoint(int32 x, int32 y) : X(x), Y(y) { }
};

struct QuestPOIBlobData
{
    uint32 Idx1;
    int32 ObjectiveIndex;
    uint32 QuestObjectiveId;
    uint32 MapId;
    uint32 WorldMapAreaId;
    uint32 Floor;
    uint32 Priority;
    uint32 Flags;
    std::vector<QuestPOIBlobPoint> Points;

    QuestPOIBlobData() : Idx1(0), ObjectiveIndex(0), QuestObjectiveId(0), MapId(0), WorldMapAreaId(0), Floor(0), Priority(0), Flags(0) { }
    QuestPOIBlobData(uint32 idx1, int32 objectiveIndex, uint32 questObjectiveId, uint32 mapId, uint32 worldMapAreaId, uint32 floor, uint32 priority, uint32 flags, std::vector<QuestPOIBlobPoint> points)
        : Idx1(idx1), ObjectiveIndex(objectiveIndex), QuestObjectiveId(questObjectiveId), MapId(mapId), WorldMapAreaId(worldMapAreaId), Floor(floor), Priority(priority), Flags(flags), Points(std::move(points)) { }
};

struct QuestPOIData
{
    uint32 QuestID;
    std::vector<QuestPOIBlobData> Blobs;
};

typedef std::unordered_map<uint32, QuestPOIData> QuestPOIContainer;

typedef std::multimap<uint32, uint32> QuestRelations; // unit/go -> quest
typedef std::multimap<uint32, uint32> QuestRelationsReverse; // quest -> unit/go

struct QuestRelationResult
{
public:
    struct Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = QuestRelations::mapped_type;
        using pointer = value_type const*;
        using reference = value_type const&;
        using difference_type = void;

        Iterator(QuestRelations::const_iterator it, QuestRelations::const_iterator end, bool onlyActive)
                : _it(it), _end(end), _onlyActive(onlyActive)
        {
            skip();
        }

        bool operator==(Iterator const& other) const { return _it == other._it; }
        bool operator!=(Iterator const& other) const { return _it != other._it; }

        Iterator& operator++() { ++_it; skip(); return *this; }
        Iterator operator++(int) { Iterator t = *this; ++*this; return t; }

        value_type operator*() const { return _it->second; }

    private:
        void skip() { if (_onlyActive) _skip(); }
        void _skip();

        QuestRelations::const_iterator _it, _end;
        bool _onlyActive;
    };

    QuestRelationResult() : _onlyActive(false) {}
    QuestRelationResult(std::pair<QuestRelations::const_iterator, QuestRelations::const_iterator> range, bool onlyActive)
            : _begin(range.first), _end(range.second), _onlyActive(onlyActive) {}

    Iterator begin() const { return { _begin, _end, _onlyActive }; }
    Iterator end() const { return { _end, _end, _onlyActive }; }

    bool HasQuest(uint32 questId) const;

private:
    QuestRelations::const_iterator _begin, _end;
    bool _onlyActive;
};

typedef std::multimap<int32, uint32> ExclusiveQuestGroups; // exclusiveGroupId -> quest
typedef std::pair<ExclusiveQuestGroups::const_iterator, ExclusiveQuestGroups::const_iterator> ExclusiveQuestGroupsBounds;

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount)
    { }

    uint32 item_id;
    uint32 item_amount;
};

typedef std::list<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerLevelInfo
{
    PlayerLevelInfo()
    {
        for (uint8 i = 0; i < MAX_STATS; ++i) stats [i] = 0;
    }

    uint8 stats [MAX_STATS];
};

struct PlayerCreateInfoSkill
{
    uint16 SkillId;
    uint16 Rank;
};

typedef std::vector<PlayerCreateInfoSkill> PlayerCreateInfoSkills;

typedef std::list<uint32> PlayerCreateSkillRaceClassList;

typedef std::vector<uint32> PlayerCreateInfoSpells;

struct PlayerCreateInfoAction
{
    PlayerCreateInfoAction() : button(0), type(0), action(0)
    { }
    PlayerCreateInfoAction(uint8 _button, uint32 _action, uint8 _type) : button(_button), type(_type), action(_action)
    { }

    uint8 button;
    uint8 type;
    uint32 action;
};

typedef std::list<PlayerCreateInfoAction> PlayerCreateInfoActions;

struct PlayerInfo
{
    // existence checked by displayId != 0
    PlayerInfo() : displayId_m(0), displayId_f(0), levelInfo(NULL)
    { }

    uint32 mapId;
    uint32 areaId;
    float positionX;
    float positionY;
    float positionZ;
    float orientation;
    uint16 displayId_m;
    uint16 displayId_f;
    PlayerCreateInfoItems item;
    PlayerCreateInfoSpells castSpells;
    //PlayerCreateInfoSkills skills;      // Not skill id - index from SkillRaceClassInfo.dbc 
    PlayerCreateSkillRaceClassList skills;
    PlayerCreateInfoActions action;

    PlayerLevelInfo* levelInfo;                             //[level-1] 0..MaxPlayerLevel-1
};

struct GraveYardData
{
    uint32 safeLocId;
    uint32 team;
};

typedef std::multimap<uint32, GraveYardData> GraveYardContainer;
typedef std::unordered_map<uint32 /* graveyard Id */, float /* orientation */> GraveyardOrientationContainer;
typedef std::pair<GraveYardContainer::const_iterator, GraveYardContainer::const_iterator> GraveYardMapBounds;
typedef std::pair<GraveYardContainer::iterator, GraveYardContainer::iterator> GraveYardMapBoundsNonConst;

typedef std::unordered_map<uint32, VendorItemData> CacheVendorItemContainer;
typedef std::unordered_map<uint32, TrainerSpellData> CacheTrainerSpellContainer;

enum SkillRangeType
{
    SKILL_RANGE_LANGUAGE,                                   // 300..300
    SKILL_RANGE_LEVEL,                                      // 1..max skill for level
    SKILL_RANGE_MONO,                                       // 1..1, grey monolite bar
    SKILL_RANGE_RANK,                                       // 1..skill for known rank
    SKILL_RANGE_NONE                                        // 0..0 always
};

SkillRangeType GetSkillRangeType(SkillLineEntry const* pSkill, bool racial);

#define MAX_PLAYER_NAME          12                         // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length (> MAX_PLAYER_NAME for support declined names)
#define MAX_PET_NAME             12                         // max allowed by client name length
#define MAX_CHARTER_NAME         24                         // max allowed by client name length

bool normalizePlayerName(std::string& name);

struct LanguageDesc
{
    Language lang_id;
    uint32   spell_id;
    uint32   skill_id;
};

extern LanguageDesc lang_description[LANGUAGES_COUNT];
LanguageDesc const* GetLanguageDescByID(uint32 lang);

enum EncounterCreditType
{
    ENCOUNTER_CREDIT_KILL_CREATURE  = 0,
    ENCOUNTER_CREDIT_CAST_SPELL     = 1
};

struct DungeonEncounter
{
    DungeonEncounter(DungeonEncounterEntry const* _dbcEntry, EncounterCreditType _creditType, uint32 _creditEntry, uint32 _lastEncounterDungeon)
        : dbcEntry(_dbcEntry), creditType(_creditType), creditEntry(_creditEntry), lastEncounterDungeon(_lastEncounterDungeon) { }

    DungeonEncounterEntry const* dbcEntry;
    EncounterCreditType creditType;
    uint32 creditEntry;
    uint32 lastEncounterDungeon;
};

typedef std::list<DungeonEncounter const*> DungeonEncounterList;
typedef std::unordered_map<uint32, DungeonEncounterList> DungeonEncounterContainer;

struct HotfixInfo
{
    uint32 Type;
    uint32 Timestamp;
    uint32 Entry;
};

typedef std::vector<HotfixInfo> HotfixData;

struct ResearchDigsiteInfo
{
    uint32 digsiteId;
    uint32 branchId;
    uint32 requiredSkillValue;
    uint32 requiredLevel;
};

typedef std::list<ResearchDigsiteInfo> ResearchDigsiteList;
typedef std::unordered_map<uint32 /*mapId*/, ResearchDigsiteList> ResearchDigsiteContainer;

struct ArchaeologyFindInfo
{
    uint32 guid;
    uint32 goEntry;
    float x;
    float y;
    float z;
};

typedef std::list<ArchaeologyFindInfo> ArchaeologyFindList;
typedef std::unordered_map<uint32 /*digsiteId*/, ArchaeologyFindList> ArchaeologyFindContainer;

struct PhaseInfoStruct
{
    uint32 id;
    ConditionContainer Conditions;
};

typedef std::unordered_map<uint32, std::vector<uint32 /*id*/>> TerrainPhaseInfo;
typedef std::unordered_map<uint32, std::vector<uint32>> TerrainUIPhaseInfo;
typedef std::unordered_map<uint32, std::vector<PhaseInfoStruct>> PhaseAreaInfo;

struct ResearchProjectRequirements
{
    uint32 requiredSkillValue;
    float chance;
};

struct CreatureDifficultyInfo
{
    uint8 LevelMin;
    uint8 LevelMax;
    float DamageMod;
    float HealthMod;
    float MinDamage;
    float MaxDamage;
    float MinRangeDamage;
    float MaxRangeDamage;
    uint32 AttackPower;
    uint32 RangedAttackPower;
};

typedef std::unordered_map<uint32, ResearchProjectRequirements> ResearchProjectRequirementContainer;

struct SceneTemplate
{
    uint32 SceneId;
    uint32 PlaybackFlags;
    uint32 ScenePackageId;
    uint32 ScriptId;
};

typedef std::unordered_map<uint32, SceneTemplate> SceneTemplateContainer;

struct GuildChallengeReward
{
    uint32 ChallengeCount;
    uint32 Experience;
    uint32 Gold;
    uint32 MaxLevelGold;
};

struct RealmCompletedChallenge
{
    struct ChallengeMember
    {
        ChallengeMember() :
                Guid(ObjectGuid::Empty),
                SpecId(0) {}

        ObjectGuid Guid{};
        uint32 SpecId{};
    };

    RealmCompletedChallenge() = default;

    uint32 GuildId{0};
    uint32 AttemptId{0};
    uint32 CompletionTime{0};
    uint32 CompletionDate{0};
    uint8 MedalEarned{0};
    uint8 MembersCount{0};

    ChallengeMember Members[5];
};

struct ChallengeReward
{
    ChallengeReward()
    {
        memset(this, 0, sizeof(ChallengeReward));
    }

    uint32 MapId{0};
    uint32 MoneyReward[4]{0};
    uint32 ValorReward[4]{0};
    uint32 TitleID{0};
    uint32 AchievementID{0};
};

typedef std::vector<GuildChallengeReward> GuildChallengeRewardData;
typedef std::map<uint32, RealmCompletedChallenge> GroupsCompletedChallengesMap;
typedef std::map<uint32, RealmCompletedChallenge> GuildsCompletedChallengesMap;
typedef std::map<uint32, ChallengeReward> ChallengeRewardsMap;

class PlayerDumpReader;

struct PetScaling
{
    float ApPerSpd;
    float ApPerAp;
    float SpdPerSpd;
    float SpdPerAp;
    float HpPerHp;
    float BaseDamageCoeff;
};

struct AreaTriggerTemplate
{
    uint32 Entry;
    uint32 Flags;
    uint32 CollisionType;
    float Radius;
    float ScaleX;
    float ScaleY;
    uint32 ScriptId;
};

typedef std::unordered_map<uint32, AreaTriggerTemplate> AreaTriggerTemplateContainer;

struct CreatureScalingInfo
{
    float Health;
    float DamageMod;
};

struct PromotionAurasInfo
{
    uint32 Entry;
    uint32 StartDate;
    uint32 Lenght;
    bool   Active;
};

namespace ObjectVisibilityState
{
    enum class objectType : uint8
    {
        GameObject = 1,
        Creature,
    };

    struct ObjectVisibilityStateInfo
    {
        ObjectVisibilityState::objectType type;
        uint32 questId;
        uint8 questState;
    };
};

class ObjectMgr
{
    friend class PlayerDumpReader;

    private:
        ObjectMgr();
        ~ObjectMgr();

    public:

        static ObjectMgr* instance();

        typedef std::unordered_map<uint32, Item*> ItemMap;

        typedef std::unordered_map<uint32, Quest*> QuestMap;
        typedef std::unordered_map<uint32 /*questObjectiveId*/, QuestObjective const*> QuestObjectivesByIdContainer;

        typedef std::unordered_map<uint32, AreaTriggerStruct> AreaTriggerContainer;

        typedef std::unordered_map<uint32, uint32> AreaTriggerScriptContainer;

        typedef std::unordered_map<uint32, AccessRequirement*> AccessRequirementContainer;

        typedef std::unordered_map<uint32, RepRewardRate > RepRewardRateContainer;
        typedef std::unordered_map<uint32, ReputationOnKillEntry> RepOnKillContainer;
        typedef std::unordered_map<uint32, RepSpilloverTemplate> RepSpilloverTemplateContainer;

        typedef std::unordered_map<uint32, PointOfInterest> PointOfInterestContainer;

        typedef std::vector<std::string> ScriptNameContainer;

        typedef std::map<uint32, uint32> CharacterConversionMap;

        Player* GetPlayerByLowGUID(uint32 lowguid) const;

        GameObjectTemplate const* GetGameObjectTemplate(uint32 entry);
        GameObjectTemplateContainer const* GetGameObjectTemplates() const { return &_gameObjectTemplateStore; }
        int LoadReferenceVendor(int32 vendor, int32 item, uint8 type, std::set<uint32> *skip_vendors);

        void LoadGameObjectTemplate();
        void LoadGameObjectTemplateAddons();
        void AddGameobjectInfo(GameObjectTemplate* goinfo);

        CreatureTemplate const* GetCreatureTemplate(uint32 entry) const;
        CreatureTemplateContainer const* GetCreatureTemplates() const { return &_creatureTemplateStore; }
        CreatureModelInfo const* GetCreatureModelInfo(uint32 modelId) const;
        CreatureModelInfo const* GetCreatureModelRandomGender(CreatureModel* model, CreatureTemplate const* creatureTemplate) const;
        static CreatureModel const* ChooseDisplayId(CreatureTemplate const* cinfo, CreatureData const* data = nullptr);
        static void ChooseCreatureFlags(CreatureTemplate const* cinfo, uint32& npcflag, uint32& unit_flags, uint32& dynamicflags, CreatureData const* data = NULL);
        EquipmentInfo const* GetEquipmentInfo(uint32 entry, int8& id);
        CreatureAddon const* GetCreatureAddon(uint32 lowguid);
        GameObjectAddon const* GetGameObjectAddon(uint32 lowguid);
        GameObjectTemplateAddon const* GetGameObjectTemplateAddon(uint32 entry) const;
        CreatureAddon const* GetCreatureTemplateAddon(uint32 entry) const;
        CreatureMovementData const* GetCreatureMovementOverride(ObjectGuid::LowType spawnId) const;
        ItemTemplate const* GetItemTemplate(uint32 entry);
        ItemTemplateContainer const* GetItemTemplateStore() const { return &_itemTemplateStore; }
        AreaTriggerTemplate const* GetAreaTriggerTemplate(uint32 entry) const;

        InstanceTemplate const* GetInstanceTemplate(uint32 mapId);

        PetLevelInfo const* GetPetLevelInfo(uint32 creature_id, uint8 level) const;

        void GetPlayerClassLevelInfo(uint32 class_, uint8 level, uint32& baseHP, uint32& baseMana) const;

        PlayerInfo const* GetPlayerInfo(uint32 race, uint32 class_) const;

        void GetPlayerLevelInfo(uint32 race, uint32 class_, uint8 level, PlayerLevelInfo* info) const;

        ObjectGuid GetPlayerGUIDByName(std::string const& name) const;

        /**
        * Retrieves the player name by guid.
        *
        * If the player is online, the name is retrieved immediately otherwise
        * a database query is done.
        *
        * @remark Use sWorld->GetCharacterNameData because it doesn't require a database query when player is offline
        *
        * @param guid player full guid
        * @param name returned name
        *
        * @return true if player was found, false otherwise
        */
        bool GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const;
        uint32 GetPlayerTeamByGUID(ObjectGuid guid) const;
        uint32 GetPlayerAccountIdByGUID(ObjectGuid guid) const;
        uint32 GetPlayerAccountIdByPlayerName(std::string const& name) const;

        uint32 GetNearestTaxiNode(float x, float y, float z, uint32 mapid, uint32 team);
        void GetTaxiPath(uint32 source, uint32 destination, uint32 &path, uint32 &cost);
        uint32 GetTaxiMountDisplayId(uint32 id, uint32 team, bool allowed_alt_team = false);

        Quest const* GetQuestTemplate(uint32 quest_id) const;

        QuestMap const& GetQuestTemplates() const { return _questTemplates; }

        QuestObjective const* GetQuestObjective(uint32 questObjectiveId) const
        {
            auto itr = _questObjectives.find(questObjectiveId);
            return itr != _questObjectives.end() ? itr->second : nullptr;
        }

        std::unordered_set<uint32> const* GetQuestGiverForAreaTrigger(uint32 Trigger_ID) const
        {
            auto itr = _questGiverAreaTriggerStore.find(Trigger_ID);
            if (itr != _questGiverAreaTriggerStore.end())
                return &itr->second;
            return nullptr;
        }

        std::unordered_set<uint32> const* GetQuestsForAreaTrigger(uint32 Trigger_ID) const
        {
            auto itr = _questAreaTriggerStore.find(Trigger_ID);
            if (itr != _questAreaTriggerStore.end())
                return &itr->second;
            return nullptr;
        }

        bool IsTavernAreaTrigger(uint32 Trigger_ID) const
        {
            return _tavernAreaTriggerStore.find(Trigger_ID) != _tavernAreaTriggerStore.end();
        }

        bool IsGameObjectForQuests(uint32 entry) const
        {
            return _gameObjectForQuestStore.find(entry) != _gameObjectForQuestStore.end();
        }

        GossipText const* GetGossipText(uint32 Text_ID) const;

        WorldSafeLocsEntry const* GetDefaultGraveYard(uint32 team);
        WorldSafeLocsEntry const* GetClosestGraveYard(WorldLocation const& location, uint32 team, WorldObject* conditionObject);
        bool AddGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist = true);
        void RemoveGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist = false);
        void LoadGraveyardZones();
        GraveYardData const* FindGraveYardData(uint32 id, uint32 zone);

        AreaTriggerStruct const* GetAreaTrigger(uint32 trigger) const;

        AccessRequirement const* GetAccessRequirement(uint32 mapid, Difficulty difficulty) const;

        AreaTriggerStruct const* GetGoBackTrigger(uint32 Map) const;
        AreaTriggerStruct const* GetMapEntranceTrigger(uint32 Map) const;

        uint32 GetAreaTriggerScriptId(uint32 trigger_id);
        SpellScriptsBounds GetSpellScriptsBounds(uint32 spellId);

        RepRewardRate const* GetRepRewardRate(uint32 factionId) const
        {
            RepRewardRateContainer::const_iterator itr = _repRewardRateStore.find(factionId);
            if (itr != _repRewardRateStore.end())
                return &itr->second;

            return nullptr;
        }

        ReputationOnKillEntry const* GetReputationOnKilEntry(uint32 id) const
        {
            RepOnKillContainer::const_iterator itr = _repOnKillStore.find(id);
            if (itr != _repOnKillStore.end())
                return &itr->second;
            return nullptr;
        }

        int32 GetBaseReputationOf(FactionEntry const* factionEntry, uint8 race, uint8 playerClass);

        RepSpilloverTemplate const* GetRepSpilloverTemplate(uint32 factionId) const
        {
            RepSpilloverTemplateContainer::const_iterator itr = _repSpilloverTemplateStore.find(factionId);
            if (itr != _repSpilloverTemplateStore.end())
                return &itr->second;

            return NULL;
        }

        PointOfInterest const* GetPointOfInterest(uint32 id) const
        {
            PointOfInterestContainer::const_iterator itr = _pointsOfInterestStore.find(id);
            if (itr != _pointsOfInterestStore.end())
                return &itr->second;
            return NULL;
        }

        QuestPOIData const* GetQuestPOIData(uint32 questId);

        VehicleAccessoryList const* GetVehicleAccessoryList(Vehicle* veh) const;

        DungeonEncounterList const* GetDungeonEncounterList(uint32 mapId, Difficulty difficulty)
        {
            std::unordered_map<uint32, DungeonEncounterList>::const_iterator itr = _dungeonEncounterStore.find(MAKE_PAIR32(mapId, difficulty));
            if (itr != _dungeonEncounterStore.end())
                return &itr->second;
            return NULL;
        }

        void LoadQuests();
        void LoadQuestObjectiveVisualEffects();
        void LoadQuestStartersAndEnders();
        void LoadGameobjectQuestStarters();
        void LoadGameobjectQuestEnders();
        void LoadCreatureQuestStarters();
        void LoadCreatureQuestEnders();

        QuestRelations* GetGOQuestRelationMapHACK() { return &_goQuestRelations; }
        QuestRelationResult GetGOQuestRelations(uint32 entry) const { return GetQuestRelationsFrom(_goQuestRelations, entry, true); }
        QuestRelationResult GetGOQuestInvolvedRelations(uint32 entry) const { return GetQuestRelationsFrom(_goQuestInvolvedRelations, entry, false); }
        Trinity::IteratorPair<QuestRelationsReverse::const_iterator> GetGOQuestInvolvedRelationReverseBounds(uint32 questId) const { return _goQuestInvolvedRelationsReverse.equal_range(questId); }
        QuestRelations* GetCreatureQuestRelationMapHACK() { return &_creatureQuestRelations; }
        QuestRelationResult GetCreatureQuestRelations(uint32 entry) const { return GetQuestRelationsFrom(_creatureQuestRelations, entry, true); }
        QuestRelationResult GetCreatureQuestInvolvedRelations(uint32 entry) const { return GetQuestRelationsFrom(_creatureQuestInvolvedRelations, entry, false); }
        Trinity::IteratorPair<QuestRelationsReverse::const_iterator> GetCreatureQuestInvolvedRelationReverseBounds(uint32 questId) const { return _creatureQuestInvolvedRelationsReverse.equal_range(questId); }

        ExclusiveQuestGroupsBounds GetExclusiveQuestGroupBounds(int32 exclusiveGroupId) const
        {
            return _exclusiveQuestGroups.equal_range(exclusiveGroupId);
        }

        void LoadEventScripts();
        void LoadSpellScripts();
        void LoadWaypointScripts();

        void LoadSpellScriptNames();
        void ValidateSpellScripts();

        bool LoadTrinityStrings(char const* table, int32 min_value, int32 max_value);
        bool LoadTrinityStrings() { return LoadTrinityStrings("trinity_string", MIN_TRINITY_STRING_ID, MAX_TRINITY_STRING_ID); }
        void LoadBroadcastTexts();
        void LoadBroadcastTextLocales();        
        void LoadCreatureClassLevelStats();
        void LoadCreatureLocales();
        void LoadGraveyardOrientations();
        void LoadCreatureTemplates();
        void LoadCreatureTemplateAddons();
        void LoadCreatureSparringTemplate();
        void LoadCreatureTemplate(Field* fields);
        void LoadCreatureTemplateModels();        
        void LoadCreatureDifficultyModifiers();
        void CheckCreatureTemplate(CreatureTemplate const* cInfo);
        void CheckCreatureMovement(char const* table, uint64 id, CreatureMovementData& creatureMovement);
        void LoadTempSummons();
        void LoadCreatures();
        void LoadLinkedRespawn();
        bool SetCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid);
        void LoadCreatureAddons();
        void LoadGameObjectAddons();
        void LoadCreatureModelInfo();
        void LoadEquipmentTemplates();
        void LoadCreatureMovementOverrides();
        void LoadGameObjectLocales();
        void LoadGameobjects();
        void LoadItemTemplates();
        void LoadItemTemplateAddon();
        void LoadItemScriptNames();
        void LoadItemLocales();
        void LoadQuestTemplateLocale();
        void LoadQuestObjectivesLocale();
        void LoadQuestGreetingLocales();
        void LoadQuestOfferRewardLocale();
        void LoadQuestRequestItemsLocale();         
        void LoadNpcTextLocales();
        void LoadPageTextLocales();
        void LoadGossipMenuItemsLocales();
        void LoadPointOfInterestLocales();
        void LoadInstanceTemplate();
        void LoadInstanceEncounters();
        void LoadMailLevelRewards();
        void LoadVehicleTemplateAccessories();
        void LoadVehicleAccessories();
        void LoadSpellAreaTriggerTemplates();

        void LoadGossipText();

        void LoadAreaTriggerTeleports();
        void LoadAccessRequirements();
        void LoadQuestGiverAreaTriggers();
        void LoadQuestAreaTriggers();
        void LoadAreaTriggerScripts();
        void LoadTavernAreaTriggers();
        void LoadGameObjectForQuests();

        void LoadPageTexts();
        PageText const* GetPageText(uint32 pageEntry);

        void LoadPlayerInfo();
        void LoadPetLevelInfo();
        void LoadExplorationBaseXP();
        void LoadPetNames();
        void LoadPetNumber();
        void LoadFishingBaseSkillLevel();

        void LoadReputationRewardRate();
        void LoadReputationOnKill();
        void LoadReputationSpilloverTemplate();

        void LoadPointsOfInterest();
        void LoadQuestPOI();

        void LoadNPCSpellClickSpells();

        void LoadGameTele();

        void LoadGossipMenu();
        void LoadGossipMenuItems();

        void LoadVendors();
        void LoadTrainerSpell();
        void AddSpellToTrainer(uint32 entry, uint32 spell, uint32 spellCost, uint32 reqSkill, uint32 reqSkillValue, uint32 reqLevel);

        void LoadPhaseDefinitions();
        void LoadTerrainPhaseInfo();
        void LoadTerrainSwapDefaults();
        void LoadTerrainWorldMaps();
        void LoadAreaPhases();
        void LoadSpellPhaseInfo();

        PhaseDefinitionStore const* GetPhaseDefinitionStore() { return &_PhaseDefinitionStore; }
        SpellPhaseStore const* GetSpellPhaseStore() { return &_SpellPhaseStore; }

        std::vector<uint32> const* GetPhaseTerrainSwaps(uint32 phaseid) const
        {
            auto itr = _terrainPhaseInfoStore.find(phaseid);
            return itr != _terrainPhaseInfoStore.end() ? &itr->second : nullptr;
        }
        std::vector<uint32> const* GetDefaultTerrainSwaps(uint32 mapid) const
        {
            auto itr = _terrainMapDefaultStore.find(mapid);
            return itr != _terrainMapDefaultStore.end() ? &itr->second : nullptr;
        }
        std::vector<uint32> const* GetTerrainWorldMaps(uint32 terrainId) const
        {
            auto itr = _terrainWorldMapStore.find(terrainId);
            return itr != _terrainWorldMapStore.end() ? &itr->second : nullptr;
        }
        std::vector<PhaseInfoStruct> const* GetPhasesForArea(uint32 area) const
        {
            auto itr = _areaPhases.find(area);
            return itr != _areaPhases.end() ? &itr->second : nullptr;
        }
        TerrainPhaseInfo const& GetDefaultTerrainSwapStore() const { return _terrainMapDefaultStore; }
        PhaseAreaInfo const& GetAreaAndZonePhases() const { return _areaPhases; }
        // condition loading helpers
        std::vector<PhaseInfoStruct>* GetPhasesForAreaOrZoneForLoading(uint32 areaOrZone)
        {
            auto itr = _areaPhases.find(areaOrZone);
            return itr != _areaPhases.end() ? &itr->second : nullptr;
        }
        PhaseAreaInfo& GetAreaAndZonePhasesForLoading() { return _areaPhases; }

        void LoadBattlePetBreedData();
        void LoadBattlePetQualityData();
        void LoadBattlePetItemToSpeciesData();

        uint8 BattlePetGetRandomBreed(uint32 speciesId) const;
        uint8 BattlePetGetRandomQuality(uint32 speciesId) const;
        uint16 BattlePetGetSpeciesFromItem(uint32 itemId) const;

        void LoadSceneTemplates();

        SceneTemplate const* GetSceneTemplate(uint32 sceneId) const
        {
            auto itr = _sceneTemplateStore.find(sceneId);
            if (itr != _sceneTemplateStore.end())
                return &itr->second;
            return nullptr;
        }

        void LoadGuildChallengeRewardInfo();
        GuildChallengeRewardData const& GetGuildChallengeRewardData() const { return _challengeRewardData; }

        void LoadPetScaling();
        PetScaling const* GetPetScaling(uint32 entry)
        {
            auto it = _petScaling.find(entry);
            return it != _petScaling.end() ? &it->second : nullptr;
        }

        void LoadRealmCompletedChallenges();
        void LoadChallengeRewards();

        void AddGroupCompletedChallenge(uint32 mapId, RealmCompletedChallenge challengers);
        void AddGuildCompletedChallenge(uint32 mapId, RealmCompletedChallenge challengers);
        RealmCompletedChallenge* GetGroupCompletedChallengeForMap(uint32 mapId)
        {
            if (m_GroupsCompletedChallenges.find(mapId) == m_GroupsCompletedChallenges.end())
                return nullptr;
            return &m_GroupsCompletedChallenges[mapId];
        }
        RealmCompletedChallenge* GetGuildCompletedChallengeForMap(uint32 mapId)
        {
            if (m_GuildsCompletedChallenges.find(mapId) == m_GuildsCompletedChallenges.end())
                return nullptr;
            return &m_GuildsCompletedChallenges[mapId];
        }
        ChallengeReward* GetChallengeRewardsForMap(uint32 mapId)
        {
            if (m_ChallengeRewardsMap.find(mapId) == m_ChallengeRewardsMap.end())
                return nullptr;
            return &m_ChallengeRewardsMap[mapId];
        }
        ChallengeRewardsMap GetChallengeRewards() const
        {
            return m_ChallengeRewardsMap;
        }

        std::string GeneratePetName(uint32 entry);
        uint32 GetBaseXP(uint8 level);
        uint32 GetXPForLevel(uint8 level) const;

        int32 GetFishingBaseSkillLevel(uint32 entry) const
        {
            FishingBaseSkillContainer::const_iterator itr = _fishingBaseForAreaStore.find(entry);
            return itr != _fishingBaseForAreaStore.end() ? itr->second : 0;
        }

        void ReturnOrDeleteOldMails(bool serverUp);

        CreatureBaseStats const* GetCreatureBaseStats(uint8 level, uint8 unitClass);

        void SetHighestGuids();

        template<HighGuid type>
        inline ObjectGuidGeneratorBase& GetGenerator()
        {
            static_assert(ObjectGuidTraits<type>::Global, "Only global guid can be generated in ObjectMgr context");
            return GetGuidSequenceGenerator<type>();
        }

        uint32 GenerateAuctionID();
        uint64 GenerateEquipmentSetGuid();
        uint32 GenerateMailID();
        uint32 GeneratePetNumber();
        uint32 GenerateMuteID();
        uint64 GenerateBattlePetId();
        uint64 GenerateVoidStorageItemId();
        uint64 GenerateCreatureSpawnId();
        uint64 GenerateGameObjectSpawnId();

        MailLevelReward const* GetMailLevelReward(uint32 level, uint32 raceMask)
        {
            MailLevelRewardContainer::const_iterator map_itr = _mailLevelRewardStore.find(level);
            if (map_itr == _mailLevelRewardStore.end())
                return NULL;

            for (MailLevelRewardList::const_iterator set_itr = map_itr->second.begin(); set_itr != map_itr->second.end(); ++set_itr)
                if (set_itr->raceMask & raceMask)
                    return &*set_itr;

            return NULL;
        }

        CellObjectGuids const& GetCellObjectGuids(uint16 mapid, uint8 spawnMode, uint32 cell_id)
        {
            return _mapObjectGuidsStore[MAKE_PAIR32(mapid, spawnMode)][cell_id];
        }

        CellObjectGuidsMap const& GetMapObjectGuids(uint16 mapid, uint8 spawnMode)
        {
            return _mapObjectGuidsStore[MAKE_PAIR32(mapid, spawnMode)];
        }

        /**
         * Gets temp summon data for all creatures of specified group.
         *
         * @param summonerId   Summoner's entry.
         * @param summonerType Summoner's type, see SummonerType for available types.
         * @param group        Id of required group.
         *
         * @return null if group was not found, otherwise reference to the creature group data
         */
        std::vector<TempSummonData> const* GetSummonGroup(uint32 summonerId, SummonerType summonerType, uint8 group) const
        {
            TempSummonDataContainer::const_iterator itr = _tempSummonDataStore.find(TempSummonGroupKey(summonerId, summonerType, group));
            if (itr != _tempSummonDataStore.end())
                return &itr->second;

            return NULL;
        }

        BroadcastText const* GetBroadcastText(uint32 id) const
        {
            BroadcastTextContainer::const_iterator itr = _broadcastTextStore.find(id);
            if (itr != _broadcastTextStore.end())
                return &itr->second;
            return nullptr;
        }

        CreatureData const* GetCreatureData(uint32 guid) const
        {
            CreatureDataContainer::const_iterator itr = _creatureDataStore.find(guid);
            if (itr == _creatureDataStore.end()) return NULL;
            return &itr->second;
        }
        CreatureData& NewOrExistCreatureData(uint32 guid) { return _creatureDataStore[guid]; }
        void DeleteCreatureData(uint32 guid);
        ObjectGuid GetLinkedRespawnGuid(ObjectGuid guid) const
        {
            LinkedRespawnContainer::const_iterator itr = _linkedRespawnStore.find(guid);
            if (itr == _linkedRespawnStore.end()) return ObjectGuid::Empty;
            return itr->second;
        }
        CreatureLocale const* GetCreatureLocale(uint32 entry) const
        {
            CreatureLocaleContainer::const_iterator itr = _creatureLocaleStore.find(entry);
            if (itr == _creatureLocaleStore.end()) return NULL;
            return &itr->second;
        }
        GameObjectLocale const* GetGameObjectLocale(uint32 entry) const
        {
            GameObjectLocaleContainer::const_iterator itr = _gameObjectLocaleStore.find(entry);
            if (itr == _gameObjectLocaleStore.end()) return NULL;
            return &itr->second;
        }
        ItemLocale const* GetItemLocale(uint32 entry) const
        {
            ItemLocaleContainer::const_iterator itr = _itemLocaleStore.find(entry);
            if (itr == _itemLocaleStore.end()) return NULL;
            return &itr->second;
        }
        QuestTemplateLocale const* GetQuestLocale(uint32 entry) const
        {
            QuestTemplateLocaleContainer::const_iterator itr = _questTemplateLocaleStore.find(entry);
            if (itr == _questTemplateLocaleStore.end()) return nullptr;
            return &itr->second;
        }        
        QuestOfferRewardLocale const* GetQuestOfferRewardLocale(uint32 entry) const
        {
            auto itr = _questOfferRewardLocaleStore.find(entry);
            if (itr == _questOfferRewardLocaleStore.end()) return nullptr;
            return &itr->second;
        }
        QuestRequestItemsLocale const* GetQuestRequestItemsLocale(uint32 entry) const
        {
            auto itr = _questRequestItemsLocaleStore.find(entry);
            if (itr == _questRequestItemsLocaleStore.end()) return nullptr;
            return &itr->second;
        }
        QuestObjectivesLocale const* GetQuestObjectivesLocale(uint32 entry) const
        {
            QuestObjectivesLocaleContainer::const_iterator itr = _questObjectivesLocaleStore.find(entry);
            if (itr == _questObjectivesLocaleStore.end()) return nullptr;
            return &itr->second;
        }        
        NpcTextLocale const* GetNpcTextLocale(uint32 entry) const
        {
            NpcTextLocaleContainer::const_iterator itr = _npcTextLocaleStore.find(entry);
            if (itr == _npcTextLocaleStore.end()) return NULL;
            return &itr->second;
        }
        PageTextLocale const* GetPageTextLocale(uint32 entry) const
        {
            PageTextLocaleContainer::const_iterator itr = _pageTextLocaleStore.find(entry);
            if (itr == _pageTextLocaleStore.end()) return NULL;
            return &itr->second;
        }
        GossipMenuItemsLocale const* GetGossipMenuItemsLocale(uint32 menuId, uint32 optionIndex) const
        {
            GossipMenuItemsLocaleContainer::const_iterator itr = _gossipMenuItemsLocaleStore.find(std::make_pair(menuId, optionIndex));
            if (itr == _gossipMenuItemsLocaleStore.end()) return nullptr;
            return &itr->second;
        }
        PointOfInterestLocale const* GetPointOfInterestLocale(uint32 poi_id) const
        {
            PointOfInterestLocaleContainer::const_iterator itr = _pointOfInterestLocaleStore.find(poi_id);
            if (itr == _pointOfInterestLocaleStore.end()) return NULL;
            return &itr->second;
        }

        GameObjectData const* GetGOData(uint32 guid) const
        {
            GameObjectDataContainer::const_iterator itr = _gameObjectDataStore.find(guid);
            if (itr == _gameObjectDataStore.end()) return NULL;
            return &itr->second;
        }
        GameObjectData& NewGOData(uint32 guid) { return _gameObjectDataStore[guid]; }

        void DeleteGOData(uint32 guid);

        float GetSparringHealthLimitFor(uint32 entry) const
        {
            auto itr = _creatureSparringTemplateStore.find(entry);

            if (itr != _creatureSparringTemplateStore.end())
            {
                return itr->second;
            }
            return 0.0f;
        }

        TrinityStringLocale const* GetTrinityStringLocale(int32 entry) const
        {
            TrinityStringLocaleContainer::const_iterator itr = _trinityStringLocaleStore.find(entry);
            if (itr == _trinityStringLocaleStore.end()) return NULL;
            return &itr->second;
        }
        const char *GetTrinityString(int32 entry, LocaleConstant locale_idx) const;
        const char *GetTrinityStringForDBCLocale(int32 entry) const { return GetTrinityString(entry, DBCLocaleIndex); }
        LocaleConstant GetDBCLocaleIndex() const { return DBCLocaleIndex; }
        void SetDBCLocaleIndex(LocaleConstant locale) { DBCLocaleIndex = locale; }

        // grid objects
        void AddCreatureToGrid(uint32 guid, CreatureData const* data);
        void RemoveCreatureFromGrid(uint32 guid, CreatureData const* data);
        void AddGameobjectToGrid(uint32 guid, GameObjectData const* data);
        void RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data);
        uint32 AddGOData(uint32 entry, uint32 map, Position const& position, uint32 spawntimedelay = 0, G3D::Quat const& rotation = G3D::Quat{});
        uint32 AddCreData(uint32 entry, uint32 team, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0);
        bool MoveCreData(uint32 guid, uint32 map, const Position& pos);

        // reserved names
        void LoadReservedPlayersNames();
        bool IsReservedName(std::string const& name) const;

        // name with valid structure and symbols
        static uint8 CheckPlayerName(std::string const& name, bool create = false);
        static PetNameInvalidReason CheckPetName(std::string const& name);
        static bool IsValidCharterName(std::string const& name);

        static bool CheckDeclinedName(std::string originalName, std::string declinedName);
        static bool CheckDeclinedNames(std::string originalName, DeclinedName const& names);

        GameTele const* GetGameTele(uint32 id) const
        {
            GameTeleContainer::const_iterator itr = _gameTeleStore.find(id);
            if (itr == _gameTeleStore.end()) return NULL;
            return &itr->second;
        }
        GameTele const* GetGameTele(std::string const& name) const;
        GameTele const* GetGameTeleExactName(std::string const& name) const;
        GameTeleContainer const& GetGameTeleMap() const { return _gameTeleStore; }
        bool AddGameTele(GameTele& data);
        bool DeleteGameTele(std::string const& name);

        TrainerSpellData const* GetNpcTrainerSpells(uint32 entry) const
        {
            CacheTrainerSpellContainer::const_iterator  iter = _cacheTrainerSpellStore.find(entry);
            if (iter == _cacheTrainerSpellStore.end())
                return NULL;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorItemList(uint32 entry) const
        {
            CacheVendorItemContainer::const_iterator iter = _cacheVendorItemStore.find(entry);
            if (iter == _cacheVendorItemStore.end())
                return NULL;

            return &iter->second;
        }

        float const* GetGraveyardOrientation(uint32 id) const
        {
            GraveyardOrientationContainer::const_iterator iter = _graveyardOrientations.find(id);
            if (iter != _graveyardOrientations.end())
                return &iter->second;

            return NULL;
        }

        void AddVendorItem(uint32 entry, uint32 item, int32 maxcount, uint32 incrtime, uint32 extendedCost, uint8 type, bool persist = true); // for event
        bool RemoveVendorItem(uint32 entry, uint32 item, uint8 type, bool persist = true); // for event
        bool IsVendorItemValid(uint32 vendor_entry, uint32 id, int32 maxcount, uint32 ptime, uint32 ExtendedCost, uint8 type, Player* player = NULL, std::set<uint32>* skip_vendors = NULL, uint32 ORnpcflag = 0) const;

        void LoadScriptNames();
        ScriptNameContainer const& GetAllScriptNames() const;
        std::string const& GetScriptName(uint32 id) const;        
        uint32 GetScriptId(std::string const& name);

        SpellClickInfoMapBounds GetSpellClickInfoMapBounds(uint32 creature_id) const
        {
            return _spellClickInfoStore.equal_range(creature_id);
        }

        GossipMenusMapBounds GetGossipMenusMapBounds(uint32 uiMenuId) const
        {
            return _gossipMenusStore.equal_range(uiMenuId);
        }

        GossipMenusMapBoundsNonConst GetGossipMenusMapBoundsNonConst(uint32 uiMenuId)
        {
            return _gossipMenusStore.equal_range(uiMenuId);
        }

        GossipMenuItemsMapBounds GetGossipMenuItemsMapBounds(uint32 uiMenuId) const
        {
            return _gossipMenuItemsStore.equal_range(uiMenuId);
        }
        GossipMenuItemsMapBoundsNonConst GetGossipMenuItemsMapBoundsNonConst(uint32 uiMenuId)
        {
            return _gossipMenuItemsStore.equal_range(uiMenuId);
        }

        // for wintergrasp only
        GraveYardContainer GraveYardStore;

        static void AddLocaleString(std::string const& s, LocaleConstant locale, StringVector& data);
        static inline void GetLocaleStringOld(const StringVector& data, int loc_idx, std::string& value) // SkyFire version
        {
            if (data.size() > size_t(loc_idx) && !data[loc_idx].empty())
                value = data[loc_idx];
        }        
        static inline void GetLocaleString(std::vector<std::string> const& data, LocaleConstant localeConstant, std::string& value) //TC version
        {
            if (data.size() > size_t(localeConstant) && !data[localeConstant].empty())
                value = data[localeConstant];
        }

        CharacterConversionMap FactionChangeAchievements;
        CharacterConversionMap FactionChangeItems;
        CharacterConversionMap FactionChangeQuests;
        CharacterConversionMap FactionChangeReputation;
        CharacterConversionMap FactionChangeSpells;
        CharacterConversionMap FactionChangeTitles;

        void LoadFactionChangeAchievements();
        void LoadFactionChangeItems();
        void LoadFactionChangeQuests();
        void LoadFactionChangeReputations();
        void LoadFactionChangeSpells();
        void LoadFactionChangeTitles();

        void LoadHotfixData();
        HotfixData const& GetHotfixData() const { return _hotfixData; }
        time_t GetHotfixDate(uint32 entry, uint32 type) const;

        void LoadMissingKeyChains();

        void LoadResearchDigsiteInfo();
        void LoadArchaeologyFindInfo();
        void LoadResearchProjectRequirements();

        ResearchDigsiteInfo const* GetResearchDigsiteInfo(uint32 digsiteId) const;
        ResearchDigsiteList const* GetResearchDigsitesForContinent(uint32 mapId) const;

        ArchaeologyFindInfo const* GetArchaeologyFindInfo(uint32 findGUID, uint32 digsiteId);
        ArchaeologyFindInfo const* GetRandomArchaeologyFindForDigsite(uint32 digsiteId);
        ArchaeologyFindList const* GetArcheologyFindListForDigsite(uint32 digsiteId);

        ResearchProjectRequirements const* GetResearchProjectRequirements(uint32 projectId) const;
        
        CreatureDifficultyInfo const* GetCreatureDifficultyInfo(Difficulty difficulty, uint32 id) const;
        CreatureDifficultyInfo const* SelectDifficultyInfo(Map const* map, uint32 entry) const;

        ItemSpecInfo const* GetItemSpecInfo(uint32 id, uint32 level);

        void LoadBrokenQuests();

        bool IsBrokenQuest(uint32 questId) const
        {
            return _brokenQuests.find(questId) != _brokenQuests.end();
        }

        typedef std::map<CustomVisibility::Type, std::map<uint32, CustomVisibility::Info>> CustomVisibilityInfoMap;
        void LoadCustomVisibility();
        bool LoadCustomVisibility(CustomVisibility::Type type, uint32 entry);
        CustomVisibility::Info const* GetCustomVisibilityInfo(TypeID type, uint32 entry)
        {
            switch (type)
            {
                case TYPEID_UNIT:
                    return GetCustomVisibilityInfo(CustomVisibility::Type::Creature, entry);
                case TYPEID_GAMEOBJECT:
                    return GetCustomVisibilityInfo(CustomVisibility::Type::GameObject, entry);
                case TYPEID_DYNAMICOBJECT:
                    return GetCustomVisibilityInfo(CustomVisibility::Type::DynamicObject, entry);
                default:
                    return nullptr;
            }
        }
        CustomVisibility::Info const* GetCustomVisibilityInfo(CustomVisibility::Type type, uint32 entry)
        {
            auto itr = m_customVisibilityInfo[type].find(entry);
            return itr == m_customVisibilityInfo[type].end() ? nullptr : &itr->second;
        }
        CustomVisibilityInfoMap& GetCustomVisibilityInfoMap() { return m_customVisibilityInfo; }

        bool CanBeTransmogrified(ItemTemplate const* proto) const;
        bool CanTransmogrify(ItemTemplate const* proto) const;
        bool SubClassesCompatible(ItemTemplate const* transmogrified, ItemTemplate const* transmogrifier) const;
        bool CanTransmogrifyItemWithItem(ItemTemplate const* transmogrified, ItemTemplate const* transmogrifier);
        bool HasStats(ItemTemplate const* proto) const;

        void LoadCreatureScaling();
        CreatureScalingInfo const* GetCreatureScalingData(uint32 entry, uint32 groupSize) const
        {
            CreatureScalingContainer::const_iterator itr = _creatureScalingStore.find(MAKE_PAIR32(entry, groupSize));
            return itr != _creatureScalingStore.end() ? &itr->second : nullptr;
        }

        typedef std::vector<PromotionAurasInfo> PromotionAurasMap;
        PromotionAurasMap const& GetPromotionAurasData() const { return _promotionAuras; }
        void LoadPromotionAuras();

        void LoadObjectVisibilityState();
        ObjectVisibilityState::ObjectVisibilityStateInfo const* GetObjectVisibilityStateData(int32 entryorguid) const
        {
            ObjectVisibilityStateContainer::const_iterator itr = _objectVisibilityStateStore.find(entryorguid);
            return itr != _objectVisibilityStateStore.end() ? &itr->second : nullptr;
        }

    private:
        // first free id for selected id type
        std::atomic<uint32> _auctionId{ 1 };
        std::atomic<uint64> _equipmentSetGuid{ 1 };
        std::atomic<uint32> _itemTextId{ 1 };
        std::atomic<uint32> _mailId{ 1 };
        std::atomic<uint32> _muteId{ 1 };
        std::atomic<uint32> _hiPetNumber{ 1 };
        std::atomic<uint64> _voidItemId{ 1 };
        std::atomic<uint32> _battlePetId{ 1 };
        std::atomic<uint64> _creatureSpawnId { 1 };
        std::atomic<uint64> _gameObjectSpawnId { 1 };

        // first free low guid for selected guid type
        template<HighGuid high>
        inline ObjectGuidGeneratorBase& GetGuidSequenceGenerator()
        {
            auto itr = _guidGenerators.find(high);
            if (itr == _guidGenerators.end())
                itr = _guidGenerators.insert(std::make_pair(high, std::unique_ptr<ObjectGuidGenerator<high>>(new ObjectGuidGenerator<high>()))).first;

            return *itr->second;
        }

        std::map<HighGuid, std::unique_ptr<ObjectGuidGeneratorBase>> _guidGenerators;

        QuestMap _questTemplates;
        QuestObjectivesByIdContainer _questObjectives;

        typedef std::unordered_map<uint32, GossipText> GossipTextContainer;
        typedef std::unordered_map<uint32, std::unordered_set<uint32>> QuestAreaTriggerContainer;
        typedef std::set<uint32> TavernAreaTriggerContainer;
        typedef std::set<uint32> GameObjectForQuestContainer;

        QuestAreaTriggerContainer _questGiverAreaTriggerStore;
        QuestAreaTriggerContainer _questAreaTriggerStore;
        TavernAreaTriggerContainer _tavernAreaTriggerStore;
        GameObjectForQuestContainer _gameObjectForQuestStore;
        GossipTextContainer _gossipTextStore;
        AreaTriggerContainer _areaTriggerStore;
        AreaTriggerScriptContainer _areaTriggerScriptStore;
        AccessRequirementContainer _accessRequirementStore;
        DungeonEncounterContainer _dungeonEncounterStore;

        RepRewardRateContainer _repRewardRateStore;
        RepOnKillContainer _repOnKillStore;
        RepSpilloverTemplateContainer _repSpilloverTemplateStore;

        GossipMenusContainer _gossipMenusStore;
        GossipMenuItemsContainer _gossipMenuItemsStore;
        PointOfInterestContainer _pointsOfInterestStore;

        QuestPOIContainer _questPOIStore;

        QuestRelations _goQuestRelations;
        QuestRelations _goQuestInvolvedRelations;
        QuestRelationsReverse _goQuestInvolvedRelationsReverse;
        QuestRelations _creatureQuestRelations;
        QuestRelations _creatureQuestInvolvedRelations;
        QuestRelationsReverse _creatureQuestInvolvedRelationsReverse;

        ExclusiveQuestGroups _exclusiveQuestGroups;

        //character reserved names
        typedef std::set<std::wstring> ReservedNamesContainer;
        ReservedNamesContainer _reservedNamesStore;

        GameTeleContainer _gameTeleStore;

        ScriptNameContainer _scriptNamesStore;

        SpellClickInfoContainer _spellClickInfoStore;

        SpellScriptsContainer _spellScriptsStore;

        VehicleAccessoryContainer _vehicleTemplateAccessoryStore;
        VehicleAccessoryContainer _vehicleAccessoryStore;

        LocaleConstant DBCLocaleIndex;

        PageTextContainer _pageTextStore;
        InstanceTemplateContainer _instanceTemplateStore;

        PhaseDefinitionStore _PhaseDefinitionStore;
        TerrainPhaseInfo _terrainPhaseInfoStore;
        TerrainPhaseInfo _terrainMapDefaultStore;
        TerrainUIPhaseInfo _terrainWorldMapStore;
        PhaseAreaInfo _areaPhases;
        SpellPhaseStore _SpellPhaseStore;

        typedef std::set<uint8> BattleBetBreedSet;
        typedef std::unordered_map<uint16, BattleBetBreedSet> BattlePetBreedXSpeciesMap;
        typedef std::set<uint8> BattlePetQualitySet;
        typedef std::unordered_map<uint16, BattlePetQualitySet> BattlePetQualityXSpeciesMap;
        typedef std::unordered_map<uint32, uint16> BattlePetItemToSpeciesMap;

        BattlePetBreedXSpeciesMap _battlePetBreedXSpeciesStore;
        BattlePetQualityXSpeciesMap _battlePetQualityXSpeciesStore;
        BattlePetItemToSpeciesMap _battlePetItemToSpeciesStore;

    private:
        void LoadScripts(ScriptsType type);
        void LoadQuestRelationsHelper(QuestRelations& map, QuestRelationsReverse* reverseMap, std::string const& table);
        QuestRelationResult GetQuestRelationsFrom(QuestRelations const& map, uint32 key, bool onlyActive) const { return { map.equal_range(key), onlyActive }; }
        void PlayerCreateInfoAddItemHelper(uint32 race_, uint32 class_, uint32 itemId, int32 count);

        MailLevelRewardContainer _mailLevelRewardStore;

        CreatureBaseStatsContainer _creatureBaseStatsStore;

        typedef std::map<uint32, PetLevelInfo*> PetLevelInfoContainer;
        // PetLevelInfoContainer[creature_id][level]
        PetLevelInfoContainer _petInfoStore;                            // [creature_id][level]

        void BuildPlayerLevelInfo(uint8 race, uint8 class_, uint8 level, PlayerLevelInfo* plinfo) const;

        PlayerInfo* _playerInfo[MAX_RACES][MAX_CLASSES];

        typedef std::vector<uint32> PlayerXPperLevel;       // [level]
        PlayerXPperLevel _playerXPperLevel;

        typedef std::map<uint32, uint32> BaseXPContainer;          // [area level][base xp]
        BaseXPContainer _baseXPTable;

        typedef std::map<uint32, int32> FishingBaseSkillContainer; // [areaId][base skill level]
        FishingBaseSkillContainer _fishingBaseForAreaStore;

        typedef std::unordered_map<int32, ObjectVisibilityState::ObjectVisibilityStateInfo> ObjectVisibilityStateContainer;
        ObjectVisibilityStateContainer _objectVisibilityStateStore;

        typedef std::map<uint32, StringVector> HalfNameContainer;
        HalfNameContainer _petHalfName0;
        HalfNameContainer _petHalfName1;

        MapObjectGuids _mapObjectGuidsStore;
        CreatureDataContainer _creatureDataStore;
        CreatureTemplateContainer _creatureTemplateStore;
        CreatureModelContainer _creatureModelStore;
        CreatureAddonContainer _creatureAddonStore;
        GameObjectAddonContainer _gameObjectAddonStore;
        CreatureAddonContainer _creatureTemplateAddonStore;
        std::unordered_map<ObjectGuid::LowType, CreatureMovementData> _creatureMovementOverrides;
        CreatureSparringTemplateMap _creatureSparringTemplateStore;
        EquipmentInfoContainer _equipmentInfoStore;
        LinkedRespawnContainer _linkedRespawnStore;
        CreatureLocaleContainer _creatureLocaleStore;
        GameObjectDataContainer _gameObjectDataStore;
        GameObjectLocaleContainer _gameObjectLocaleStore;
        GameObjectTemplateContainer _gameObjectTemplateStore;
        GameObjectTemplateAddonContainer _gameObjectTemplateAddonStore;
        /// Stores temp summon data grouped by summoner's entry, summoner's type and group id
        TempSummonDataContainer _tempSummonDataStore;

        BroadcastTextContainer _broadcastTextStore;
        ItemTemplateContainer _itemTemplateStore;
        ItemLocaleContainer _itemLocaleStore;
        QuestTemplateLocaleContainer _questTemplateLocaleStore;
        QuestObjectivesLocaleContainer _questObjectivesLocaleStore;
        QuestOfferRewardLocaleContainer _questOfferRewardLocaleStore;
        QuestRequestItemsLocaleContainer _questRequestItemsLocaleStore;        
        NpcTextLocaleContainer _npcTextLocaleStore;
        PageTextLocaleContainer _pageTextLocaleStore;
        TrinityStringLocaleContainer _trinityStringLocaleStore;
        GossipMenuItemsLocaleContainer _gossipMenuItemsLocaleStore;
        PointOfInterestLocaleContainer _pointOfInterestLocaleStore;
        AreaTriggerTemplateContainer _areaTriggerTemplateStore;

        CacheVendorItemContainer _cacheVendorItemStore;
        CacheTrainerSpellContainer _cacheTrainerSpellStore;

        GraveyardOrientationContainer _graveyardOrientations;

        std::set<uint32> _difficultyEntries[MAX_TEMPLATE_DIFFICULTY - 1]; // already loaded difficulty 1 value in creatures, used in CheckCreatureTemplate
        std::set<uint32> _hasDifficultyEntries[MAX_TEMPLATE_DIFFICULTY - 1]; // already loaded creatures with difficulty 1 values, used in CheckCreatureTemplate

        enum CreatureLinkedRespawnType
        {
            CREATURE_TO_CREATURE,
            CREATURE_TO_GO,         // Creature is dependant on GO
            GO_TO_GO,
            GO_TO_CREATURE          // GO is dependant on creature
        };

        std::set<uint32> _transportMaps; // Helper container storing map ids that are for transports only, loaded from gameobject_template

        HotfixData _hotfixData;

        ResearchDigsiteContainer _researchDigsiteStore;
        ArchaeologyFindContainer _archaeologyFindStore;
        ResearchProjectRequirementContainer _researchProjectRequirementStore;

        typedef std::unordered_map<uint32, CreatureDifficultyInfo> CreatureDifficultyContainer;
        std::vector<CreatureDifficultyContainer> _creatureDifficulty;

        SceneTemplateContainer _sceneTemplateStore;

        GuildChallengeRewardData _challengeRewardData;

        std::unordered_map<uint32, std::unique_ptr<ItemSpecInfo>> _itemsSpecCache;
        std::mutex _itemSpecCacheLock;

        std::unordered_map<uint32, PetScaling> _petScaling;

        GroupsCompletedChallengesMap m_GroupsCompletedChallenges;
        GuildsCompletedChallengesMap m_GuildsCompletedChallenges;
        ChallengeRewardsMap m_ChallengeRewardsMap;

        std::set<uint32> _brokenQuests;

        CustomVisibilityInfoMap m_customVisibilityInfo;

        typedef std::unordered_map<uint32, CreatureScalingInfo> CreatureScalingContainer;
        CreatureScalingContainer _creatureScalingStore;

        PromotionAurasMap _promotionAuras;
};

#define sObjectMgr ObjectMgr::instance()

// scripting access functions
bool LoadTrinityStrings(char const* table, int32 start_value = MAX_CREATURE_AI_TEXT_STRING_ID, int32 end_value = std::numeric_limits<int32>::min());

#endif
