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

#ifndef _UPDATEFIELDS_H
#define _UPDATEFIELDS_H

// Auto generated for version 5, 4, 8, 18414

enum ObjectFields
{
    OBJECT_FIELD_GUID                                      = 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    OBJECT_FIELD_DATA                                      = 0x002, // Size: 2, Flags: UF_FLAG_PUBLIC
    OBJECT_FIELD_TYPE                                      = 0x004, // Size: 1, Flags: UF_FLAG_PUBLIC
    OBJECT_FIELD_ENTRY_ID                                  = 0x005, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT
    OBJECT_FIELD_DYNAMIC_FLAGS                             = 0x006, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT, UF_FLAG_URGENT
    OBJECT_FIELD_SCALE                                     = 0x007, // Size: 1, Flags: UF_FLAG_PUBLIC
    OBJECT_END                                             = 0x008,
};

enum ObjectDynamicFields
{
    OBJECT_DYNAMIC_END                                     = 0x000,
};

enum ItemFields
{
    ITEM_FIELD_OWNER                                       = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_CONTAINED_IN                                = OBJECT_END + 0x002, // Size: 2, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_CREATOR                                     = OBJECT_END + 0x004, // Size: 2, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_GIFT_CREATOR                                = OBJECT_END + 0x006, // Size: 2, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_STACK_COUNT                                 = OBJECT_END + 0x008, // Size: 1, Flags: UF_FLAG_OWNER
    ITEM_FIELD_EXPIRATION                                  = OBJECT_END + 0x009, // Size: 1, Flags: UF_FLAG_OWNER
    ITEM_FIELD_SPELL_CHARGES                               = OBJECT_END + 0x00A, // Size: 5, Flags: UF_FLAG_OWNER
    ITEM_FIELD_DYNAMIC_FLAGS                               = OBJECT_END + 0x00F, // Size: 1, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_ENCHANTMENT                                 = OBJECT_END + 0x010, // Size: 39, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_PROPERTY_SEED                               = OBJECT_END + 0x037, // Size: 1, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_RANDOM_PROPERTIES_ID                        = OBJECT_END + 0x038, // Size: 1, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_DURABILITY                                  = OBJECT_END + 0x039, // Size: 1, Flags: UF_FLAG_OWNER
    ITEM_FIELD_MAX_DURABILITY                              = OBJECT_END + 0x03A, // Size: 1, Flags: UF_FLAG_OWNER
    ITEM_FIELD_CREATE_PLAYED_TIME                          = OBJECT_END + 0x03B, // Size: 1, Flags: UF_FLAG_PUBLIC
    ITEM_FIELD_MODIFIERS_MASK                              = OBJECT_END + 0x03C, // Size: 1, Flags: UF_FLAG_OWNER
    ITEM_END                                               = OBJECT_END + 0x03D,
};

enum ItemDynamicFields
{
    ITEM_DYNAMIC_MODIFIERS                                 = OBJECT_DYNAMIC_END + 0x000, // Flags: UF_FLAG_OWNER
    ITEM_DYNAMIC_END                                       = OBJECT_DYNAMIC_END + 0x001,
};

enum ContainerFields
{
    CONTAINER_FIELD_SLOTS                                  = ITEM_END + 0x000, // Size: 72, Flags: UF_FLAG_PUBLIC
    CONTAINER_FIELD_NUM_SLOTS                              = ITEM_END + 0x048, // Size: 1, Flags: UF_FLAG_PUBLIC
    CONTAINER_END                                          = ITEM_END + 0x049,
};

enum ContainerDynamicFields
{
    CONTAINER_DYNAMIC_END                                  = ITEM_DYNAMIC_END + 0x000,
};

enum UnitFields
{
    UNIT_FIELD_CHARM                                       = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_SUMMON                                      = OBJECT_END + 0x002, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_CRITTER                                     = OBJECT_END + 0x004, // Size: 2, Flags: UF_FLAG_PRIVATE
    UNIT_FIELD_CHARMED_BY                                  = OBJECT_END + 0x006, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_SUMMONED_BY                                 = OBJECT_END + 0x008, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_CREATED_BY                                  = OBJECT_END + 0x00A, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_DEMON_CREATOR                               = OBJECT_END + 0x00C, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_TARGET                                      = OBJECT_END + 0x00E, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_BATTLE_PET_COMPANION_GUID                   = OBJECT_END + 0x010, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_CHANNEL_OBJECT                              = OBJECT_END + 0x012, // Size: 2, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_FIELD_CHANNEL_SPELL                               = OBJECT_END + 0x014, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_FIELD_SUMMONED_BY_HOME_REALM                      = OBJECT_END + 0x015, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_BYTES_0                                     = OBJECT_END + 0x016, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_DISPLAY_POWER                               = OBJECT_END + 0x017, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID                   = OBJECT_END + 0x018, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_HEALTH                                      = OBJECT_END + 0x019, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_POWER                                       = OBJECT_END + 0x01A, // Size: 5, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MAX_HEALTH                                  = OBJECT_END + 0x01F, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MAX_POWER                                   = OBJECT_END + 0x020, // Size: 5, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER                   = OBJECT_END + 0x025, // Size: 5, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_UNIT_ALL
    UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER       = OBJECT_END + 0x02A, // Size: 5, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_UNIT_ALL
    UNIT_FIELD_LEVEL                                       = OBJECT_END + 0x02F, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_EFFECTIVE_LEVEL                             = OBJECT_END + 0x030, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_FACTION_TEMPLATE                            = OBJECT_END + 0x031, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_VIRTUAL_ITEM_ID                             = OBJECT_END + 0x032, // Size: 3, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_FLAGS                                       = OBJECT_END + 0x035, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_FLAGS_2                                     = OBJECT_END + 0x036, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_AURA_STATE                                  = OBJECT_END + 0x037, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_ATTACK_ROUND_BASE_TIME                      = OBJECT_END + 0x038, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_RANGED_ATTACK_ROUND_BASE_TIME               = OBJECT_END + 0x03A, // Size: 1, Flags: UF_FLAG_PRIVATE
    UNIT_FIELD_BOUNDING_RADIUS                             = OBJECT_END + 0x03B, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_COMBAT_REACH                                = OBJECT_END + 0x03C, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_DISPLAY_ID                                  = OBJECT_END + 0x03D, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT, UF_FLAG_URGENT
    UNIT_FIELD_NATIVE_DISPLAY_ID                           = OBJECT_END + 0x03E, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_FIELD_MOUNT_DISPLAY_ID                            = OBJECT_END + 0x03F, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_FIELD_MIN_DAMAGE                                  = OBJECT_END + 0x040, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_SPECIAL_INFO
    UNIT_FIELD_MAX_DAMAGE                                  = OBJECT_END + 0x041, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_SPECIAL_INFO
    UNIT_FIELD_MIN_OFF_HAND_DAMAGE                         = OBJECT_END + 0x042, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_SPECIAL_INFO
    UNIT_FIELD_MAX_OFF_HAND_DAMAGE                         = OBJECT_END + 0x043, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_SPECIAL_INFO
    UNIT_FIELD_BYTES_1                                     = OBJECT_END + 0x044, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_PET_NUMBER                                  = OBJECT_END + 0x045, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_PET_NAME_TIMESTAMP                          = OBJECT_END + 0x046, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_PET_EXPERIENCE                              = OBJECT_END + 0x047, // Size: 1, Flags: UF_FLAG_OWNER
    UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE                   = OBJECT_END + 0x048, // Size: 1, Flags: UF_FLAG_OWNER
    UNIT_FIELD_MOD_CASTING_SPEED                           = OBJECT_END + 0x049, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MOD_SPELL_HASTE                             = OBJECT_END + 0x04A, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MOD_HASTE                                   = OBJECT_END + 0x04B, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MOD_RANGED_HASTE                            = OBJECT_END + 0x04C, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MOD_HASTE_REGEN                             = OBJECT_END + 0x04D, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_CREATED_BY_SPELL                            = OBJECT_END + 0x04E, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_NPC_FLAGS                                   = OBJECT_END + 0x04F, // Size: 2, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_NPC_EMOTESTATE                              = OBJECT_END + 0x051, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_STATS                                       = OBJECT_END + 0x052, // Size: 5, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_STAT_POS_BUFF                               = OBJECT_END + 0x057, // Size: 5, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_STAT_NEG_BUFF                               = OBJECT_END + 0x05C, // Size: 5, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RESISTANCES                                 = OBJECT_END + 0x061, // Size: 7, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER, UF_FLAG_SPECIAL_INFO
    UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE               = OBJECT_END + 0x068, // Size: 7, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE               = OBJECT_END + 0x06F, // Size: 7, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_BASE_MANA                                   = OBJECT_END + 0x076, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_BASE_HEALTH                                 = OBJECT_END + 0x077, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_BYTES_2                                     = OBJECT_END + 0x078, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_ATTACK_POWER                                = OBJECT_END + 0x079, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_ATTACK_POWER_MOD_POS                        = OBJECT_END + 0x07A, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_ATTACK_POWER_MOD_NEG                        = OBJECT_END + 0x07B, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_ATTACK_POWER_MULTIPLIER                     = OBJECT_END + 0x07C, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER                         = OBJECT_END + 0x07D, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS                 = OBJECT_END + 0x07E, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG                 = OBJECT_END + 0x07F, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER              = OBJECT_END + 0x080, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_MIN_RANGED_DAMAGE                           = OBJECT_END + 0x081, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_MAX_RANGED_DAMAGE                           = OBJECT_END + 0x082, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_POWER_COST_MODIFIER                         = OBJECT_END + 0x083, // Size: 7, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_POWER_COST_MULTIPLIER                       = OBJECT_END + 0x08A, // Size: 7, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_MAX_HEALTH_MODIFIER                         = OBJECT_END + 0x091, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_OWNER
    UNIT_FIELD_HOVER_HEIGHT                                = OBJECT_END + 0x092, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MIN_ITEM_LEVEL                              = OBJECT_END + 0x093, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_MAX_ITEM_LEVEL                              = OBJECT_END + 0x094, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_WILD_BATTLE_PET_LEVEL                       = OBJECT_END + 0x095, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_BATTLE_PET_COMPANION_NAME_TIMESTAMP         = OBJECT_END + 0x096, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_FIELD_INTERACT_SPELL_ID                           = OBJECT_END + 0x097, // Size: 1, Flags: UF_FLAG_PUBLIC
    UNIT_END                                               = OBJECT_END + 0x098,
};

enum UnitDynamicFields
{
    UNIT_DYNAMIC_FIELD_PASSIVE_SPELLS                      = OBJECT_DYNAMIC_END + 0x000, // Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_DYNAMIC_FIELD_WORLD_EFFECTS                       = OBJECT_DYNAMIC_END + 0x001, // Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    UNIT_DYNAMIC_END                                       = OBJECT_DYNAMIC_END + 0x002,
};

enum PlayerFields
{
    PLAYER_FIELD_DUEL_ARBITER                              = UNIT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_PLAYER_FLAGS                              = UNIT_END + 0x002, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_GUILD_RANK_ID                             = UNIT_END + 0x003, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_GUILD_DELETE_DATE                         = UNIT_END + 0x004, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_GUILD_LEVEL                               = UNIT_END + 0x005, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_BYTES                                           = UNIT_END + 0x006, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_BYTES_2                                         = UNIT_END + 0x007, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_BYTES_3                                         = UNIT_END + 0x008, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_DUEL_TEAM                                 = UNIT_END + 0x009, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_GUILD_TIME_STAMP                          = UNIT_END + 0x00A, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_QUEST_LOG                                 = UNIT_END + 0x00B, // Size: 750, Flags: UF_FLAG_PARTY_MEMBER
    PLAYER_FIELD_VISIBLE_ITEMS                             = UNIT_END + 0x2F9, // Size: 38, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_PLAYER_TITLE                              = UNIT_END + 0x31F, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_FAKE_INEBRIATION                          = UNIT_END + 0x320, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_VIRTUAL_PLAYER_REALM                      = UNIT_END + 0x321, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_CURRENT_SPEC_ID                           = UNIT_END + 0x322, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_TAXI_MOUNT_ANIM_KIT_ID                    = UNIT_END + 0x323, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_CURRENT_BATTLE_PET_BREED_QUALITY          = UNIT_END + 0x324, // Size: 1, Flags: UF_FLAG_PUBLIC
    PLAYER_FIELD_INV_SLOTS                                 = UNIT_END + 0x325, // Size: 172, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_END_NOT_SELF                              = UNIT_END + 0x325,
    PLAYER_FIELD_FARSIGHT_OBJECT                           = UNIT_END + 0x3D1, // Size: 2, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_KNOWN_TITLES                              = UNIT_END + 0x3D3, // Size: 10, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_COINAGE                                   = UNIT_END + 0x3DD, // Size: 2, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_XP                                        = UNIT_END + 0x3DF, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_NEXT_LEVEL_XP                             = UNIT_END + 0x3E0, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SKILL                                     = UNIT_END + 0x3E1, // Size: 448, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_CHARACTER_POINTS                          = UNIT_END + 0x5A1, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MAX_TALENT_TIERS                          = UNIT_END + 0x5A2, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_TRACK_CREATURE_MASK                       = UNIT_END + 0x5A3, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_TRACK_RESOURCE_MASK                       = UNIT_END + 0x5A4, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MAINHAND_EXPERTISE                        = UNIT_END + 0x5A5, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_OFFHAND_EXPERTISE                         = UNIT_END + 0x5A6, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_RANGED_EXPERTISE                          = UNIT_END + 0x5A7, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_COMBAT_RATING_EXPERTISE                   = UNIT_END + 0x5A8, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_BLOCK_PERCENTAGE                          = UNIT_END + 0x5A9, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_DODGE_PERCENTAGE                          = UNIT_END + 0x5AA, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PARRY_PERCENTAGE                          = UNIT_END + 0x5AB, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_CRIT_PERCENTAGE                           = UNIT_END + 0x5AC, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_RANGED_CRIT_PERCENTAGE                    = UNIT_END + 0x5AD, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_OFFHAND_CRIT_PERCENTAGE                   = UNIT_END + 0x5AE, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SPELL_CRIT_PERCENTAGE                     = UNIT_END + 0x5AF, // Size: 7, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SHIELD_BLOCK                              = UNIT_END + 0x5B6, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SHIELD_BLOCK_CRIT_PERCENTAGE              = UNIT_END + 0x5B7, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MASTERY                                   = UNIT_END + 0x5B8, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PVP_POWER_DAMAGE                          = UNIT_END + 0x5B9, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PVP_POWER_HEALING                         = UNIT_END + 0x5BA, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_EXPLORED_ZONES                            = UNIT_END + 0x5BB, // Size: 200, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_REST_STATE_BONUS_POOL                     = UNIT_END + 0x683, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_POS                       = UNIT_END + 0x684, // Size: 7, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_NEG                       = UNIT_END + 0x68B, // Size: 7, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_PERCENT                   = UNIT_END + 0x692, // Size: 7, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_POS                      = UNIT_END + 0x699, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_HEALING_PERCENT                       = UNIT_END + 0x69A, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_PERCENT                  = UNIT_END + 0x69B, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_PERIODIC_HEALING_DONE_PERCENT         = UNIT_END + 0x69C, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_WEAPON_DMG_MULTIPLIERS                    = UNIT_END + 0x69D, // Size: 3, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_SPELL_POWER_PERCENT                   = UNIT_END + 0x6A0, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_RESILIENCE_PERCENT                    = UNIT_END + 0x6A1, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_OVERRIDE_SPELL_POWER_BY_AP_PERCENT        = UNIT_END + 0x6A2, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_OVERRIDE_AP_BY_SPELL_POWER_PERCENT        = UNIT_END + 0x6A3, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_TARGET_RESISTANCE                     = UNIT_END + 0x6A4, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE            = UNIT_END + 0x6A5, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_BYTES                                     = UNIT_END + 0x6A6, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SELF_RES_SPELL                            = UNIT_END + 0x6A7, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PVP_MEDALS                                = UNIT_END + 0x6A8, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_BUYBACK_PRICE                             = UNIT_END + 0x6A9, // Size: 12, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_BUYBACK_TIMESTAMP                         = UNIT_END + 0x6B5, // Size: 12, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS                 = UNIT_END + 0x6C1, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_LIFETIME_HONORABLE_KILLS                  = UNIT_END + 0x6C2, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_WATCHED_FACTION_INDEX                     = UNIT_END + 0x6C3, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_COMBAT_RATINGS                            = UNIT_END + 0x6C4, // Size: 27, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PVP_INFO                                  = UNIT_END + 0x6DF, // Size: 24, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MAX_LEVEL                                 = UNIT_END + 0x6F7, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_RUNE_REGEN                                = UNIT_END + 0x6F8, // Size: 4, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_NO_REAGENT_COST_MASK                      = UNIT_END + 0x6FC, // Size: 4, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_GLYPH_SLOTS                               = UNIT_END + 0x700, // Size: 6, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_GLYPHS                                    = UNIT_END + 0x706, // Size: 6, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_GLYPH_SLOTS_ENABLED                       = UNIT_END + 0x70C, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PET_SPELL_POWER                           = UNIT_END + 0x70D, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_RESEARCHING                               = UNIT_END + 0x70E, // Size: 8, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_PROFESSION_SKILL_LINE                     = UNIT_END + 0x716, // Size: 2, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_UI_HIT_MODIFIER                           = UNIT_END + 0x718, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_UI_SPELL_HIT_MODIFIER                     = UNIT_END + 0x719, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_HOME_REALM_TIME_OFFSET                    = UNIT_END + 0x71A, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_MOD_PET_HASTE                             = UNIT_END + 0x71B, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_SUMMONED_BATTLE_PET_GUID                  = UNIT_END + 0x71C, // Size: 2, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_BYTES2                                    = UNIT_END + 0x71E, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_URGENT_SELF_ONLY
    PLAYER_FIELD_LFG_BONUS_FACTION_ID                      = UNIT_END + 0x71F, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_LOOT_SPEC_ID                              = UNIT_END + 0x720, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_FIELD_OVERRIDE_ZONE_PVP_TYPE                    = UNIT_END + 0x721, // Size: 1, Flags: UF_FLAG_PRIVATE, UF_FLAG_URGENT_SELF_ONLY
    PLAYER_FIELD_ITEM_LEVEL_DELTA                          = UNIT_END + 0x722, // Size: 1, Flags: UF_FLAG_PRIVATE
    PLAYER_END                                             = UNIT_END + 0x723,
};

enum PlayerDynamicFields
{
    PLAYER_DYNAMIC_FIELD_RESERACH_SITE                     = UNIT_DYNAMIC_END + 0x000, // Flags: UF_FLAG_PRIVATE
    PLAYER_DYNAMIC_FIELD_RESEARCH_SITE_PROGRESS            = UNIT_DYNAMIC_END + 0x001, // Flags: UF_FLAG_PRIVATE
    PLAYER_DYNAMIC_FIELD_DAILY_QUESTS                      = UNIT_DYNAMIC_END + 0x002, // Flags: UF_FLAG_PRIVATE
    PLAYER_DYNAMIC_END                                     = UNIT_DYNAMIC_END + 0x003,
};

enum GameObjectFields
{
    GAMEOBJECT_FIELD_CREATED_BY                            = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    GAMEOBJECT_FIELD_DISPLAY_ID                            = OBJECT_END + 0x002, // Size: 1, Flags: UF_FLAG_PUBLIC
    GAMEOBJECT_FIELD_FLAGS                                 = OBJECT_END + 0x003, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    GAMEOBJECT_FIELD_PARENT_ROTATION                       = OBJECT_END + 0x004, // Size: 4, Flags: UF_FLAG_PUBLIC
    GAMEOBJECT_FIELD_FACTION_TEMPLATE                      = OBJECT_END + 0x008, // Size: 1, Flags: UF_FLAG_PUBLIC
    GAMEOBJECT_FIELD_LEVEL                                 = OBJECT_END + 0x009, // Size: 1, Flags: UF_FLAG_PUBLIC
    GAMEOBJECT_BYTES_1                                     = OBJECT_END + 0x00A, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID                 = OBJECT_END + 0x00B, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    GAMEOBJECT_END                                         = OBJECT_END + 0x00C,
};

enum GameObjectDynamicFields
{
    GAMEOBJECT_DYNAMIC_END                                 = OBJECT_DYNAMIC_END + 0x000,
};

enum DynamicObjectFields
{
    DYNAMICOBJECT_FIELD_CASTER                             = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    DYNAMICOBJECT_BYTES                                    = OBJECT_END + 0x002, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT
    DYNAMICOBJECT_FIELD_SPELL_ID                           = OBJECT_END + 0x003, // Size: 1, Flags: UF_FLAG_PUBLIC
    DYNAMICOBJECT_FIELD_RADIUS                             = OBJECT_END + 0x004, // Size: 1, Flags: UF_FLAG_PUBLIC
    DYNAMICOBJECT_FIELD_CAST_TIME                          = OBJECT_END + 0x005, // Size: 1, Flags: UF_FLAG_PUBLIC
    DYNAMICOBJECT_END                                      = OBJECT_END + 0x006,
};

enum DynamicObjectDynamicFields
{
    DYNAMICOBJECT_DYNAMIC_END                              = OBJECT_DYNAMIC_END + 0x000,
};

enum CorpseFields
{
    CORPSE_FIELD_OWNER                                     = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_PARTY_GUID                                = OBJECT_END + 0x002, // Size: 2, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_DISPLAY_ID                                = OBJECT_END + 0x004, // Size: 1, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_ITEMS                                     = OBJECT_END + 0x005, // Size: 19, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_BYTES_1                                   = OBJECT_END + 0x018, // Size: 1, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_BYTES_2                                   = OBJECT_END + 0x019, // Size: 1, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_FLAGS                                     = OBJECT_END + 0x01A, // Size: 1, Flags: UF_FLAG_PUBLIC
    CORPSE_FIELD_DYNAMIC_FLAGS                             = OBJECT_END + 0x01B, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT
    CORPSE_END                                             = OBJECT_END + 0x01C,
};

enum CorpseDynamicFields
{
    CORPSE_DYNAMIC_END                                     = OBJECT_DYNAMIC_END + 0x000,
};

enum AreaTriggerFields
{
    AREATRIGGER_FIELD_CASTER                               = OBJECT_END + 0x000, // Size: 2, Flags: UF_FLAG_PUBLIC
    AREATRIGGER_FIELD_DURATION                             = OBJECT_END + 0x002, // Size: 1, Flags: UF_FLAG_PUBLIC
    AREATRIGGER_FIELD_SPELL_ID                             = OBJECT_END + 0x003, // Size: 1, Flags: UF_FLAG_PUBLIC
    AREATRIGGER_FIELD_SPELL_VISUAL_ID                      = OBJECT_END + 0x004, // Size: 1, Flags: UF_FLAG_VIEWER_DEPENDENT
    AREATRIGGER_FIELD_EXPLICIT_SCALE                       = OBJECT_END + 0x005, // Size: 1, Flags: UF_FLAG_PUBLIC, UF_FLAG_URGENT
    AREATRIGGER_END                                        = OBJECT_END + 0x006,
};

enum AreaTriggerDynamicFields
{
    AREATRIGGER_DYNAMIC_END                                = OBJECT_DYNAMIC_END + 0x000,
};

enum SceneObjectFields
{
    SCENEOBJECT_FIELD_SCRIPT_PACKAGE_ID                    = OBJECT_END + 0x000, // Size: 1, Flags: UF_FLAG_PUBLIC
    SCENEOBJECT_FIELD_RND_SEED_VAL                         = OBJECT_END + 0x001, // Size: 1, Flags: UF_FLAG_PUBLIC
    SCENEOBJECT_FIELD_CREATED_BY                           = OBJECT_END + 0x002, // Size: 2, Flags: UF_FLAG_PUBLIC
    SCENEOBJECT_FIELD_SCENE_TYPE                           = OBJECT_END + 0x004, // Size: 1, Flags: UF_FLAG_PUBLIC
    SCENEOBJECT_END                                        = OBJECT_END + 0x005,
};

enum SceneObjectDynamicFields
{
    SCENEOBJECT_DYNAMIC_END                                = OBJECT_DYNAMIC_END + 0x000,
};

#endif // _UPDATEFIELDS_H
