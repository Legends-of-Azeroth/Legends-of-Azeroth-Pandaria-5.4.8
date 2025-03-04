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

#ifndef TRINITY_DB2STRUCTURE_H
#define TRINITY_DB2STRUCTURE_H

#include "Common.h"
#include "ItemPrototype.h"

// GCC has alternative #pragma pack(N) syntax and old gcc version does not support pack(push, N), also any gcc version does not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

#define MAX_BATTLE_PET_PROPERTIES 6

// Structures used to access raw DB2 data and required packing to portability
struct BattlePetAbilityEntry
{
    uint32 Id;                                              // 0
    uint32 FamilyId;                                        // 1 - battle pet family id or -1 for aura
    //uint32 Unk1                                           // 2 - icon id?
    uint32 Cooldown;                                        // 3 - cooldown in turns
    uint32 VisualId;                                        // 4 - visual id (BattlePetVisual.db2)
    uint32 Flags;                                           // 5 - flags (see BattlePetAbilityFlags enum)
    char const* Name;                                       // 6 - name text
    char const* Description;                                // 7 - description text};
};

struct BattlePetAbilityStateEntry
{
    uint32 Id;                                              // 0
    uint32 AbilityId;                                       // 1 - battle pet ability id (BattlePetAbility.db2)
    uint32 StateId;                                         // 2 - battle pet state id (BattlePetState.db2)
    uint32 Value;                                           // 3 - value for state
};

// BattlePetAbilityEffect.db2
struct BattlePetAbilityEffectEntry
{
    uint32 Id;                                              // 0
    uint32 AbilityTurnId;                                   // 1 - ability turn id (BattlePetAbilityTurn.db2)
    uint32 VisualId;                                        // 2 - visual id (BattlePetVisual.db2)
    uint32 TriggerAbility;                                  // 3 - parent ability
    uint32 EffectProperty;                                  // 4 - effect property id (BattlePetEffectProperties.db2)
    //uint32 Unk1;                                          // 5 - effect property offset?
    int32  Properties[MAX_BATTLE_PET_PROPERTIES];           // 6 - values for effect property
};

// BattlePetAbilityTurn.db2
struct BattlePetAbilityTurnEntry
{
    uint32 Id;                                              // 0
    uint32 AbilityId;                                       // 1 - parent ability id (BattlePetAbility.db2)
    uint32 VisualId;                                        // 2 - visual id (BattlePetVisual.db2)
    uint32 Duration;                                        // 3 - amount of turns the ability lasts
    uint32 HasProcType;                                     // 4 - if set to 1, next value is positive else -1
    int32  ProcType;
};


// BattlePetBreedQuality.db2
struct BattlePetBreedQualityEntry
{
    uint32 Id;                                              // 0
    uint32 Quality;                                         // 1 - battle pet quality id (same as item quality)
    float  Multiplier;                                      // 2 - multiplier, better the quality higher the multiplier
};

// BattlePetBreedState.db2
struct BattlePetBreedStateEntry
{
    uint32 Id;                                              // 0
    uint32 BreedId;                                         // 1 - battle pet breed id
    uint32 StateId;                                         // 2 - battle pet state id (BattlePetState.db2)
    int32  Modifier;                                        // 3 - modifier value for state
};

typedef std::set<uint32> BattlePetBreedSet;

struct BattlePetSpeciesEntry
{
    uint32 SpeciesId;                                       // 0 - battle pet species id
    uint32 NpcId;                                           // 1 - npc id
    uint32 IconId;                                          // 2 - icon id
    uint32 SpellId;                                         // 3 - summon spell id (Spell.dbc)
    uint32 FamilyId;                                        // 4 - battle pet family id
    //int32 Unk1;                                           // 5
    uint32 Flags;                                           // 6 - flags (see BattlePetSpeciesFlags enum)
    char const* Description;                                     // 7 - description text, contains method to obtain and cost
    char const* Flavor;                                          // 8 - flavor text
};

struct BattlePetSpeciesStateEntry
{
    uint32 Id;                                              // 0
    uint32 SpeciesId;                                       // 1 - battle pet species id (BattlePetSpecies.db2)
    uint32 StateId;                                         // 2 - battle pet state id (BattlePetState.db2)
    int32 Modifier;                                         // 3 - modifier value for state
};

struct BattlePetSpeciesXAbilityEntry
{
    uint32 Id;                                              // 0
    uint32 SpeciesId;                                       // 1 - battle pet species id (BattlePetSpecies.db2)
    uint32 AbilityId;                                       // 2 - battle pet ability id (BattlePetAbility.db2)
    uint32 RequiredLevel;                                   // 3 - required level to use this ability
    uint32 SlotId;                                          // 4 - ability slot id (0-2)
};

struct BattlePetStateEntry
{
    uint32 StateId;                                         // 0 - battle pet state id
    //uint32 Unk1;                                          // 1 - only states 21 and 34 (linked states?)
    char const* Name;                                       // 2 - name text
    uint32 Flags;                                           // 3 - flags
};

typedef std::map<uint32, uint16> BattlePetItemXSpeciesStore;

struct ItemToBattlePetEntry
{
    uint32 ItemId;                                          // 0 - item id
    uint32 SpeciesId;                                       // 1 - battle pet species id (BattlePetSpecies.db2)
};

struct BroadcastTextEntry
{
    uint32   ID;                                             // 0
    //uint32   Unk0;                                         // 1
    char const*   Text_0;                                    // 2
    char const*   Text_1;                                    // 3
    //uint32   Unk1;                                         // 4
    //uint32   Unk2;                                         // 5
    //uint32   Unk3;                                         // 6
    //uint32   Unk4;                                         // 7
    //uint32   Unk5;                                         // 8
    //uint32   Unk6;                                         // 9
    uint32   SoundID;                                        // 10
    //uint32   Unk7;                                         // 11
    //uint32   Unk8;                                         // 12
};

struct ItemEntry
{
    uint32   ID;                                             // 0
    uint32   Class;                                          // 1
    uint32   SubClass;                                       // 2
    int32    SoundOverrideSubclass;                          // 3
    int32    Material;                                       // 4
    uint32   DisplayId;                                      // 5
    uint32   InventoryType;                                  // 6
    uint32   Sheath;                                         // 7
};

struct ItemCurrencyCostEntry
{
    uint32  ID;
    uint32  ItemId;
};

struct ItemSparseEntry
{
    uint32     ID;                                           // 0
    uint32     Quality;                                      // 1
    uint32     Flags;                                        // 2
    uint32     Flags2;                                       // 3
    uint32     Flags3;                                       // 4
    float      Unk430_1;                                     // 5
    float      Unk430_2;                                     // 6
    uint32     BuyCount;                                     // 7
    uint32     BuyPrice;                                     // 8
    uint32     SellPrice;                                    // 9
    uint32     InventoryType;                                // 10
    int32      AllowableClass;                               // 11
    int32      AllowableRace;                                // 12
    uint32     ItemLevel;                                    // 13
    int32      RequiredLevel;                                // 14
    uint32     RequiredSkill;                                // 15
    uint32     RequiredSkillRank;                            // 16
    uint32     RequiredSpell;                                // 17
    uint32     RequiredHonorRank;                            // 18
    uint32     RequiredCityRank;                             // 19
    uint32     RequiredReputationFaction;                    // 20
    uint32     RequiredReputationRank;                       // 21
    uint32     MaxCount;                                     // 22
    uint32     Stackable;                                    // 23
    uint32     ContainerSlots;                               // 24
    int32      ItemStatType[MAX_ITEM_PROTO_STATS];           // 25 - 34
    uint32     ItemStatValue[MAX_ITEM_PROTO_STATS];          // 35 - 44
    int32      ItemStatAllocation[MAX_ITEM_PROTO_STATS];     // 45 - 54
    float      ItemStatSocketCostMultiplier[MAX_ITEM_PROTO_STATS];// 55 - 64
    uint32     ScalingStatDistribution;                      // 65
    uint32     DamageType;                                   // 66
    uint32     Delay;                                        // 67
    float      RangedModRange;                               // 68
    int32      SpellId[MAX_ITEM_PROTO_SPELLS];               // 69 - 73
    int32      SpellTrigger[MAX_ITEM_PROTO_SPELLS];          // 74 - 78
    int32      SpellCharges[MAX_ITEM_PROTO_SPELLS];          // 79 - 83
    int32      SpellCooldown[MAX_ITEM_PROTO_SPELLS];         // 84 - 88
    int32      SpellCategory[MAX_ITEM_PROTO_SPELLS];         // 89 - 89
    int32      SpellCategoryCooldown[MAX_ITEM_PROTO_SPELLS]; // 94 - 98
    uint32     Bonding;                                      // 99
    char const*     Name;                                    // 100
    char const*     Name2;                                   // 101
    char const*     Name3;                                   // 102
    char const*     Name4;                                   // 103
    char const*     Description;                             // 104
    uint32     PageText;                                     // 105
    uint32     LanguageID;                                   // 106
    uint32     PageMaterial;                                 // 107
    uint32     StartQuest;                                   // 108
    uint32     LockID;                                       // 109
    int32      Material;                                     // 110
    uint32     Sheath;                                       // 111
    uint32     RandomProperty;                               // 112
    uint32     RandomSuffix;                                 // 113
    uint32     ItemSet;                                      // 114
    uint32     Area;                                         // 115
    uint32     Map;                                          // 116
    uint32     BagFamily;                                    // 117
    uint32     TotemCategory;                                // 118
    uint32     Color[MAX_ITEM_PROTO_SOCKETS];                // 119 - 121
    uint32     Content[MAX_ITEM_PROTO_SOCKETS];              // 122 - 124
    int32      SocketBonus;                                  // 125
    uint32     GemProperties;                                // 126
    float      ArmorDamageModifier;                          // 127
    uint32     Duration;                                     // 128
    uint32     ItemLimitCategory;                            // 129
    uint32     HolidayId;                                    // 130
    float      StatScalingFactor;                            // 131
    int32      CurrencySubstitutionId;                       // 132
    int32      CurrencySubstitutionCount;                    // 133
};

#define MAX_ITEM_EXT_COST_ITEMS         5
#define MAX_ITEM_EXT_COST_CURRENCIES    5

struct ItemExtendedCostEntry
{
    uint32      ID;                                              // 0 extended-cost entry id
    uint32      RequiredHonorPoints;                             // 1 required honor points
    uint32      RequiredArenaPoints;                             // 2 required arena points
    uint32      RequiredArenaSlot;                               // 3 arena slot restrictions (min slot value)
    uint32      RequiredItem[MAX_ITEM_EXT_COST_ITEMS];           // 4-8 required item id
    uint32      RequiredItemCount[MAX_ITEM_EXT_COST_ITEMS];      // 9-13 required count of 1st item
    uint32      RequiredPersonalArenaRating;                     // 14 required personal arena rating
    uint32      ItemPurchaseGroup;                               // 15
    uint32      RequiredCurrency[MAX_ITEM_EXT_COST_CURRENCIES];  // 16-20 required curency id
    uint32      RequiredCurrencyCount[MAX_ITEM_EXT_COST_CURRENCIES];// 21-25 required curency count
    uint32      RequiredFactionId;
    uint32      RequiredFactionStanding;
    uint32      RequirementFlags;
    uint32      RequiredGuildLevel;
    uint32      RequiredAchievement;
};

struct ItemUpgradeEntry
{
    uint32 ID;                                                      // 0
    uint32 ItemUpgradePathID;                                       // 1
    uint32 ItemLevelBonus;                                          // 2
    uint32 PrevItemUpgradeID;                                       // 3
    uint32 CurrencyID;                                              // 4
    uint32 CurrencyCost;                                            // 5
};

struct ItemToMountSpellEntry
{
    uint32 ItemId;                                        // 0
    uint32 SpellId;                                       // 1
};

#define KEYCHAIN_SIZE   32

struct KeyChainEntry
{
    uint32      Id;
    uint8       Key[KEYCHAIN_SIZE];
};

struct MapChallengeModeEntry
{
    uint32 ID;                  // 0
    uint32 MapID;               // 1
    //uint32 Unk2;              // 2
    //uint32 Unk3;              // 3
    //uint32 Unk4;              // 4
    uint32 BronzeTime;          // 5
    uint32 SilverTime;          // 6
    uint32 GoldTime;            // 7
    //uint32 Unk8;              // 8
    //uint32 Unk9;              // 9
};

// QuestPackageItem.db2
struct QuestPackageItemEntry
{
    uint32      ID;                                         // 0
    uint32      PackageID;                                  // 1
    uint32      ItemID;                                     // 2
    uint32      ItemQuantity;                               // 3
    uint32      DisplayType;                                // 4
};

struct RulesetItemUpgradeEntry
{
    uint32 ID;                                                      // 0
    uint32 RulesetID;                                               // 1
    uint32 ItemUpgradeID;                                           // 2
    uint32 ItemID;                                                  // 3
};

// SceneScript.db2
struct SceneScriptEntry
{
    uint32 ID;                                             // 0         m_ID
    char const* Name;                                      // 1         m_name
    char const* Script;                                    // 2         m_script
    uint32 PrevScriptPartID;                               // 3         m_prevScriptPartID - Prev Script Part Id From Chain
    uint32 NextScriptPartID;                               // 4         m_nextScriptPartID - Next Script Part Id From Chain
};

// SceneScriptPackage.db2
struct SceneScriptPackageEntry
{
    uint32 ID;
    char const* Name;
};

#define MAX_SPELL_REAGENTS 8
#define MAX_SPELL_REAGENTS2 10

// SpellReagents.db2
struct SpellReagentsEntry
{
    //uint32    Id;                                          // 0         m_ID
    int32     Reagent[MAX_SPELL_REAGENTS];                   // 1  - 10   m_reagent
    uint32    ReagentCount[MAX_SPELL_REAGENTS2];             // 11 - 20   m_reagentCount
};

struct VignetteEntry
{
    uint32 Id;                                                      // 0
    char const* Name;                                               // 1
    uint32 QuestFeedbackEffectId;                                   // 2
    uint32 Flags;                                                   // 3
    float  X;                                                       // 4
    float  Y;                                                       // 5
};

// GCC has alternative #pragma pack(N) syntax and old gcc version does not support pack(push, N), also any gcc version does not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif