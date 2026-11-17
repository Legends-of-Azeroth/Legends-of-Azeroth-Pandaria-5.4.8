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

#ifndef CreatureData_h__
#define CreatureData_h__

#include "Unit.h"
#include "UnitDefines.h"
enum class VisibilityDistanceType : uint8;

enum CreatureFlagsExtra : uint32
{
    CREATURE_FLAG_EXTRA_INSTANCE_BIND        = 0x00000001,       // creature kill bind instance with killer and killer's group
    CREATURE_FLAG_EXTRA_CIVILIAN             = 0x00000002,       // not aggro (ignore faction/reputation hostility)
    CREATURE_FLAG_EXTRA_NO_PARRY             = 0x00000004,       // creature can't parry
    CREATURE_FLAG_EXTRA_NO_PARRY_HASTEN      = 0x00000008,       // creature can't counter-attack at parry
    CREATURE_FLAG_EXTRA_NO_BLOCK             = 0x00000010,       // creature can't block
    CREATURE_FLAG_EXTRA_NO_CRUSH             = 0x00000020,       // creature can't do crush attacks
    CREATURE_FLAG_EXTRA_NO_XP_AT_KILL        = 0x00000040,       // creature kill not provide XP
    CREATURE_FLAG_EXTRA_TRIGGER              = 0x00000080,       // trigger creature
    CREATURE_FLAG_EXTRA_NO_TAUNT             = 0x00000100,       // creature is immune to taunt auras and effect attack me
    CREATURE_FLAG_EXTRA_NO_MOVE_FLAGS_UPDATE = 0x00000200,       // creature won't update movement flags
    CREATURE_FLAG_EXTRA_GHOST_VISIBILITY     = 0x00000400,       // creature will be only visible for dead players
    CREATURE_FLAG_EXTRA_USE_OFFHAND_ATTACK   = 0x00000800,       // creature will use offhand attacks
    CREATURE_FLAG_EXTRA_NO_SELL_VENDOR       = 0x00001000,       // players can't sell items to this vendor
    CREATURE_FLAG_EXTRA_NO_COMBAT            = 0x00002000,       // creature is not allowed to enter combat        
    CREATURE_FLAG_EXTRA_WORLDEVENT           = 0x00004000,       // custom flag for world event creatures (left room for merging)
    CREATURE_FLAG_EXTRA_GUARD                = 0x00008000,       // Creature is guard
    CREATURE_FLAG_EXTRA_UNUSED_16            = 0x00010000,
    CREATURE_FLAG_EXTRA_NO_CRIT              = 0x00020000,       // creature can't do critical strikes
    CREATURE_FLAG_EXTRA_NO_SKILLGAIN         = 0x00040000,       // creature won't increase weapon skills
    CREATURE_FLAG_EXTRA_TAUNT_DIMINISH       = 0x00080000,       // Taunt is a subject to diminishing returns on this creautre
    CREATURE_FLAG_EXTRA_ALL_DIMINISH         = 0x00100000,       // Creature is subject to all diminishing returns as player are
    CREATURE_FLAG_EXTRA_NO_KNOCK_BACK        = 0x00200000,       // use to immune from Thunderstorm
    CREATURE_FLAG_EXTRA_UNUSED_22            = 0x00400000,
    CREATURE_FLAG_EXTRA_UNUSED_23            = 0x00800000,
    CREATURE_FLAG_EXTRA_UNUSED_24            = 0x01000000,
    CREATURE_FLAG_EXTRA_UNUSED_25            = 0x02000000,
    CREATURE_FLAG_EXTRA_UNUSED_26            = 0x04000000,    
    CREATURE_FLAG_EXTRA_UNUSED_27            = 0x08000000,       // Creature will always be visible to the player. Only usable on creatures with entries 190000 or higher (on purpose)
    CREATURE_FLAG_EXTRA_DUNGEON_BOSS         = 0x10000000,       // creature is a dungeon boss (SET DYNAMICALLY, DO NOT ADD IN DB)
    CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING   = 0x20000000,       // creature ignore pathfinding
    CREATURE_FLAG_EXTRA_IMMUNITY_KNOCKBACK   = 0x40000000,       // creature is immune to knockback effects
    CREATURE_FLAG_EXTRA_UNUSED_31            = 0x80000000,

    // Masks
    CREATURE_FLAG_EXTRA_UNUSED               = (CREATURE_FLAG_EXTRA_UNUSED_16 | CREATURE_FLAG_EXTRA_UNUSED_22 |
                                                CREATURE_FLAG_EXTRA_UNUSED_23 | CREATURE_FLAG_EXTRA_UNUSED_24 | CREATURE_FLAG_EXTRA_UNUSED_25 |
                                                CREATURE_FLAG_EXTRA_UNUSED_26 | CREATURE_FLAG_EXTRA_UNUSED_27 | CREATURE_FLAG_EXTRA_UNUSED_31),

    CREATURE_FLAG_EXTRA_DB_ALLOWED           = (0xFFFFFFFF & ~(CREATURE_FLAG_EXTRA_UNUSED | CREATURE_FLAG_EXTRA_DUNGEON_BOSS))

};

enum class CreatureGroundMovementType : uint8
{
    None,
    Run,
    Hover,

    Max
};

enum class CreatureFlightMovementType : uint8
{
    None,
    DisableGravity,
    CanFly,

    Max
};

enum class CreatureChaseMovementType : uint8
{
    Run,
    CanWalk,
    AlwaysWalk,

    Max
};

enum class CreatureRandomMovementType : uint8
{
    Walk,
    CanRun,
    AlwaysRun,

    Max
};

struct TC_GAME_API CreatureMovementData
{
    CreatureMovementData();

    CreatureGroundMovementType Ground;
    CreatureFlightMovementType Flight;
    bool Swim;
    bool Rooted;
    CreatureChaseMovementType Chase;
    CreatureRandomMovementType Random;
    uint32 InteractionPauseTimer;

    bool IsGroundAllowed() const { return Ground != CreatureGroundMovementType::None; }
    bool IsSwimAllowed() const { return Swim; }
    bool IsFlightAllowed() const { return Flight != CreatureFlightMovementType::None; }
    bool IsRooted() const { return Rooted; }

    CreatureChaseMovementType GetChase() const { return Chase; }
    CreatureRandomMovementType GetRandom() const { return Random; }

    uint32 GetInteractionPauseTimer() const { return InteractionPauseTimer; }

    std::string ToString() const;
};

static const uint32 CREATURE_REGEN_INTERVAL = 2 * IN_MILLISECONDS;
static const uint32 PET_FOCUS_REGEN_INTERVAL = 4 * IN_MILLISECONDS;
static const uint32 CREATURE_NOPATH_EVADE_TIME = 5 * IN_MILLISECONDS;

static const uint8 MAX_KILL_CREDIT = 2;
static const uint32 MAX_CREATURE_MODELS = 4;
static const uint32 MAX_CREATURE_QUEST_ITEMS = 6;
static const uint32 MAX_CREATURE_SPELLS = 8;

struct CreatureModel
{
    static CreatureModel const DefaultInvisibleModel;
    static CreatureModel const DefaultVisibleModel;

    CreatureModel() :
        CreatureDisplayID(0), DisplayScale(0.0f), Probability(0.0f) { }

    CreatureModel(uint32 creatureDisplayID, float displayScale, float probability) :
        CreatureDisplayID(creatureDisplayID), DisplayScale(displayScale), Probability(probability) { }

    uint32 CreatureDisplayID;
    float DisplayScale;
    float Probability;
};

// from `creature_template` table
struct TC_GAME_API CreatureTemplate
{
    uint32  Entry;
    uint32  DifficultyEntry[MAX_TEMPLATE_DIFFICULTY - 1];
    uint32  KillCredit[MAX_KILL_CREDIT];
    std::vector<CreatureModel> Models;
    std::string  Name;
    std::string  FemaleName;
    std::string  SubName;
    std::string  IconName;
    uint32  GossipMenuId;
    uint8   minlevel;
    uint8   maxlevel;
    uint32  expansion;
    uint32  expansionUnknown;                               // either 0 or 3, sent to the client / wdb
    uint32  faction;
    uint32  npcflag;
    uint32  npcflag2;
    float   speed_walk;
    float   speed_run;
    float   scale;
    uint32  rank;
    uint32  dmgschool;
    float   dmg_multiplier;                                 // runtime-only (default 1.0, not loaded from DB); used by world events
    uint32  BaseAttackTime;
    uint32  RangeAttackTime;
    uint32  unit_class;                                     // enum Classes. Note only 4 classes are known for creatures.
    uint32  unit_flags;                                     // enum UnitFlags mask values
    uint32  unit_flags2;                                    // enum UnitFlags2 mask values
    uint32  dynamicflags;
    uint32  family;                                         // enum CreatureFamily values (optional)
    uint32  trainer_type;
    uint32  trainer_class;
    uint32  trainer_race;
    uint32  type;                                           // enum CreatureType values
    uint32  type_flags;                                     // enum CreatureTypeFlags mask values
    uint32  type_flags2;                                    // unknown enum, only set for 4 creatures (with value 1)
    uint32  lootid;
    uint32  pickpocketLootId;
    uint32  SkinLootId;
    int32   resistance[MAX_SPELL_SCHOOL];
    uint32  spells[CREATURE_MAX_SPELLS];
    uint32  PetSpellDataId;
    uint32  VehicleId;
    uint32  mingold;
    uint32  maxgold;
    std::string AIName;
    uint32  MovementType;
    CreatureMovementData Movement;
    float   HoverHeight;
    float   ModHealth;
    float   ModMana;
    float   ModManaExtra;                                   // Added in 4.x, this value is usually 2 for a small group of creatures with double mana
    float   ModArmor;
    bool    RacialLeader;
    uint32  questItems[MAX_CREATURE_QUEST_ITEMS];
    uint32  movementId;
    bool    RegenHealth;
    uint32  VignetteID;
    uint32  TrackingQuestID;
    uint32  MechanicImmuneMask;
    uint32  flags_extra;
    uint32  ScriptID;
    CreatureModel const* GetModelByIdx(uint32 idx) const;
    CreatureModel const* GetRandomValidModel() const;
    CreatureModel const* GetFirstValidModel() const;
    CreatureModel const* GetModelWithDisplayId(uint32 displayId) const;
    CreatureModel const* GetFirstInvisibleModel() const;
    CreatureModel const* GetFirstVisibleModel() const;

    // helpers
    SkillType GetRequiredLootSkill() const
    {
        if (type_flags & CREATURE_TYPEFLAGS_HERBLOOT)
            return SKILL_HERBALISM;
        else if (type_flags & CREATURE_TYPEFLAGS_MININGLOOT)
            return SKILL_MINING;
        else if (type_flags & CREATURE_TYPEFLAGS_ENGINEERLOOT)
            return SKILL_ENGINEERING;
        else
            return SKILL_SKINNING;                          // normal case
    }

    bool IsExotic() const
    {
        return (type_flags & CREATURE_TYPEFLAGS_EXOTIC) != 0;
    }

    bool IsTameable(bool canTameExotic) const
    {
        if (type != CREATURE_TYPE_BEAST || family == 0 || (type_flags & CREATURE_TYPEFLAGS_TAMEABLE) == 0)
            return false;

        // if can tame exotic then can tame any tameable
        return canTameExotic || !IsExotic();
    }
};

#pragma pack(push, 1)

// Defines base stats for creatures (used to calculate HP/mana/armor).
struct CreatureBaseStats
{
    uint32 BaseHealth[MAX_CREATURE_BASE_HP];
    uint32 BaseMana;
    uint32 BaseArmor;
    uint32 AttackPower;
    uint32 RangedAttackPower;
    float BaseDamage[MAX_CREATURE_BASE_DAMAGE];

    // Helpers

    uint32 GenerateHealth(CreatureTemplate const* info) const
    {
        return uint32(ceil(BaseHealth[info->expansion] * info->ModHealth));
    }

    uint32 GenerateMana(CreatureTemplate const* info) const
    {
        // Mana can be 0.
        if (!BaseMana)
            return 0;

        return uint32(ceil(BaseMana * info->ModMana * info->ModManaExtra));
    }

    uint32 GenerateArmor(CreatureTemplate const* info) const
    {
        return uint32(ceil(BaseArmor * info->ModArmor));
    }

    float GenerateBaseDamage(CreatureTemplate const* info) const
    {
        return BaseDamage[info->expansion];
    }

    static CreatureBaseStats const* GetBaseStats(uint8 level, uint8 unitClass);
};

struct CreatureLocale
{
    std::vector<std::string> Name;
    std::vector<std::string> FemaleName;
    std::vector<std::string> Title;
};

struct CreatureModelInfo
{
    float bounding_radius;
    float combat_reach;
    uint8 gender;
    uint32 modelid_other_gender;
    bool is_trigger;    
};

#pragma pack(pop)

// `creature_addon` table
struct CreatureAddon
{
    uint32 path_id;
    uint32 mount;
    uint8 standState;
    uint8 animTier;
    uint8 sheathState;
    uint8 pvpFlags;
    uint8 visFlags;
    uint32 emote;
    uint16 aiAnimKit;
    uint16 movementAnimKit;
    uint16 meleeAnimKit;
    std::vector<uint32> auras;
    VisibilityDistanceType visibilityDistanceType;
};

#endif // CreatureData_h__
