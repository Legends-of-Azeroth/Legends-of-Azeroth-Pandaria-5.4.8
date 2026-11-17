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

#ifndef _RBAC_H
#define _RBAC_H

#include "Define.h"
#include "DatabaseEnvFwd.h"
#include <string>
#include <set>
#include <map>

namespace rbac
{
    enum RBACPermissions
    {
        RBAC_PERM_INSTANT_LOGOUT                                 = 1,
        RBAC_PERM_SKIP_QUEUE                                     = 2,
        RBAC_PERM_JOIN_NORMAL_BG                                 = 3,
        RBAC_PERM_JOIN_RANDOM_BG                                 = 4,
        RBAC_PERM_JOIN_ARENAS                                    = 5,
        RBAC_PERM_JOIN_DUNGEON_FINDER                            = 6,
        RBAC_PERM_IGNORE_IDLE_CONNECTION                         = 7,
        RBAC_PERM_CANNOT_EARN_ACHIEVEMENTS                       = 8,
        RBAC_PERM_CANNOT_EARN_REALM_FIRST_ACHIEVEMENTS           = 9,
        RBAC_PERM_USE_CHARACTER_TEMPLATES                        = 10, // not on 3.3.5a
        RBAC_PERM_LOG_GM_TRADE                                   = 11,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_DEMON_HUNTER     = 12, // not on 3.3.5a
        RBAC_PERM_SKIP_CHECK_INSTANCE_REQUIRED_BOSSES            = 13,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_TEAMMASK         = 14,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_CLASSMASK        = 15,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_RACEMASK         = 16,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_RESERVEDNAME     = 17,
        RBAC_PERM_SKIP_CHECK_CHARACTER_CREATION_DEATH_KNIGHT     = 18,
        RBAC_PERM_SKIP_CHECK_CHAT_CHANNEL_REQ                    = 19,
        RBAC_PERM_SKIP_CHECK_DISABLE_MAP                         = 20,
        RBAC_PERM_SKIP_CHECK_MORE_TALENTS_THAN_ALLOWED           = 21,
        RBAC_PERM_SKIP_CHECK_CHAT_SPAM                           = 22,
        RBAC_PERM_SKIP_CHECK_OVERSPEED_PING                      = 23,
        RBAC_PERM_TWO_SIDE_CHARACTER_CREATION                    = 24,
        RBAC_PERM_TWO_SIDE_INTERACTION_CHAT                      = 25,
        RBAC_PERM_TWO_SIDE_INTERACTION_CHANNEL                   = 26,
        RBAC_PERM_TWO_SIDE_INTERACTION_MAIL                      = 27,
        RBAC_PERM_TWO_SIDE_WHO_LIST                              = 28,
        RBAC_PERM_TWO_SIDE_ADD_FRIEND                            = 29,
        RBAC_PERM_COMMANDS_SAVE_WITHOUT_DELAY                    = 30,
        RBAC_PERM_COMMANDS_USE_UNSTUCK_WITH_ARGS                 = 31,
        RBAC_PERM_COMMANDS_BE_ASSIGNED_TICKET                    = 32,
        RBAC_PERM_COMMANDS_NOTIFY_COMMAND_NOT_FOUND_ERROR        = 33,
        RBAC_PERM_COMMANDS_APPEAR_IN_GM_LIST                     = 34,
        RBAC_PERM_WHO_SEE_ALL_SEC_LEVELS                         = 35,
        RBAC_PERM_CAN_FILTER_WHISPERS                            = 36,
        RBAC_PERM_CHAT_USE_STAFF_BADGE                           = 37,
        RBAC_PERM_RESURRECT_WITH_FULL_HPS                        = 38,
        RBAC_PERM_RESTORE_SAVED_GM_STATE                         = 39,
        RBAC_PERM_ALLOW_GM_FRIEND                                = 40,
        RBAC_PERM_USE_START_GM_LEVEL                             = 41,
        RBAC_PERM_OPCODE_WORLD_TELEPORT                          = 42,
        RBAC_PERM_OPCODE_WHOIS                                   = 43,
        RBAC_PERM_RECEIVE_GLOBAL_GM_TEXTMESSAGE                  = 44,
        RBAC_PERM_SILENTLY_JOIN_CHANNEL                          = 45,
        RBAC_PERM_CHANGE_CHANNEL_NOT_MODERATOR                   = 46,
        RBAC_PERM_CAN_IGNORE_LOWER_SECURITY_CHECK                = 47,
        RBAC_PERM_COMMANDS_PINFO_CHECK_PERSONAL_DATA             = 48,
        RBAC_PERM_EMAIL_CONFIRM_FOR_PASS_CHANGE                  = 49,
        RBAC_PERM_MAY_CHECK_OWN_EMAIL                            = 50,
        RBAC_PERM_ALLOW_TWO_SIDE_TRADE                           = 51,
        RBAC_PERM_NO_BATTLEGROUND_DESERTER_DEBUFF                = 52,
        RBAC_PERM_CAN_AFK_ON_BATTLEGROUND                        = 53,
        // Free space for core permissions (till 149)
        // Roles (Permissions with delegated permissions) use 199 and descending
        RBAC_ROLE_ADMINISTRATOR                                  = 196,
        RBAC_ROLE_GAMEMASTER                                     = 197,
        RBAC_ROLE_MODERATOR                                      = 198,
        RBAC_ROLE_PLAYER                                         = 199,
        // 200 previously used, do not reuse
        // 201 previously used, do not reuse
        RBAC_PERM_COMMAND_RBAC_ACC_PERM_LIST                     = 202,
        RBAC_PERM_COMMAND_RBAC_ACC_PERM_GRANT                    = 203,
        RBAC_PERM_COMMAND_RBAC_ACC_PERM_DENY                     = 204,
        RBAC_PERM_COMMAND_RBAC_ACC_PERM_REVOKE                   = 205,
        RBAC_PERM_COMMAND_RBAC_LIST                              = 206,
        RBAC_PERM_COMMAND_BNET_ACCOUNT                           = 207, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_CREATE                    = 208, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_LOCK_COUNTRY              = 209, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_LOCK_IP                   = 210, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_PASSWORD                  = 211, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_SET                       = 212, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_SET_PASSWORD              = 213, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_LINK                      = 214, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_UNLINK                    = 215, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_CREATE_GAME               = 216, // not on 3.3.5a
        RBAC_PERM_COMMAND_ACCOUNT                                = 217,
        RBAC_PERM_COMMAND_ACCOUNT_ADDON                          = 218,
        RBAC_PERM_COMMAND_ACCOUNT_CREATE                         = 219,
        RBAC_PERM_COMMAND_ACCOUNT_DELETE                         = 220,
        RBAC_PERM_COMMAND_ACCOUNT_LOCK                           = 221,
        RBAC_PERM_COMMAND_ACCOUNT_LOCK_COUNTRY                   = 222,
        RBAC_PERM_COMMAND_ACCOUNT_LOCK_IP                        = 223,
        RBAC_PERM_COMMAND_ACCOUNT_ONLINE_LIST                    = 224,
        RBAC_PERM_COMMAND_ACCOUNT_PASSWORD                       = 225,
        RBAC_PERM_COMMAND_ACCOUNT_SET                            = 226,
        RBAC_PERM_COMMAND_ACCOUNT_SET_ADDON                      = 227,
        RBAC_PERM_COMMAND_ACCOUNT_SET_SECLEVEL                   = 228,
        RBAC_PERM_COMMAND_ACCOUNT_SET_PASSWORD                   = 229,
        // 230 previously used, do not reuse
        RBAC_PERM_COMMAND_ACHIEVEMENT_ADD                        = 231,
        // 232 previously used, do not reuse
        RBAC_PERM_COMMAND_ARENA_CAPTAIN                          = 233,
        RBAC_PERM_COMMAND_ARENA_CREATE                           = 234,
        RBAC_PERM_COMMAND_ARENA_DISBAND                          = 235,
        RBAC_PERM_COMMAND_ARENA_INFO                             = 236,
        RBAC_PERM_COMMAND_ARENA_LOOKUP                           = 237,
        RBAC_PERM_COMMAND_ARENA_RENAME                           = 238,
        // 239 previously used, do not reuse
        RBAC_PERM_COMMAND_BAN_ACCOUNT                            = 240,
        RBAC_PERM_COMMAND_BAN_CHARACTER                          = 241,
        RBAC_PERM_COMMAND_BAN_IP                                 = 242,
        RBAC_PERM_COMMAND_BAN_PLAYERACCOUNT                      = 243,
        // 244 previously used, do not reuse
        RBAC_PERM_COMMAND_BANINFO_ACCOUNT                        = 245,
        RBAC_PERM_COMMAND_BANINFO_CHARACTER                      = 246,
        RBAC_PERM_COMMAND_BANINFO_IP                             = 247,
        // 248 previously used, do not reuse
        RBAC_PERM_COMMAND_BANLIST_ACCOUNT                        = 249,
        RBAC_PERM_COMMAND_BANLIST_CHARACTER                      = 250,
        RBAC_PERM_COMMAND_BANLIST_IP                             = 251,
        // 252 previously used, do not reuse
        RBAC_PERM_COMMAND_UNBAN_ACCOUNT                          = 253,
        RBAC_PERM_COMMAND_UNBAN_CHARACTER                        = 254,
        RBAC_PERM_COMMAND_UNBAN_IP                               = 255,
        RBAC_PERM_COMMAND_UNBAN_PLAYERACCOUNT                    = 256,
        // 257 previously used, do not reuse
        RBAC_PERM_COMMAND_BF_START                               = 258,
        RBAC_PERM_COMMAND_BF_STOP                                = 259,
        RBAC_PERM_COMMAND_BF_SWITCH                              = 260,
        RBAC_PERM_COMMAND_BF_TIMER                               = 261,
        RBAC_PERM_COMMAND_BF_ENABLE                              = 262,
        RBAC_PERM_COMMAND_ACCOUNT_EMAIL                          = 263,
        // 264 previously used, do not reuse
        RBAC_PERM_COMMAND_ACCOUNT_SET_SEC_EMAIL                  = 265,
        RBAC_PERM_COMMAND_ACCOUNT_SET_SEC_REGMAIL                = 266,
        RBAC_PERM_COMMAND_CAST                                   = 267,
        RBAC_PERM_COMMAND_CAST_BACK                              = 268,
        RBAC_PERM_COMMAND_CAST_DIST                              = 269,
        RBAC_PERM_COMMAND_CAST_SELF                              = 270,
        RBAC_PERM_COMMAND_CAST_TARGET                            = 271,
        RBAC_PERM_COMMAND_CAST_DEST                              = 272,
        // 273 previously used, do not reuse
        RBAC_PERM_COMMAND_CHARACTER_CUSTOMIZE                    = 274,
        RBAC_PERM_COMMAND_CHARACTER_CHANGEFACTION                = 275,
        RBAC_PERM_COMMAND_CHARACTER_CHANGERACE                   = 276,
        // 277 previously used, do not reuse
        RBAC_PERM_COMMAND_CHARACTER_DELETED_DELETE               = 278,
        RBAC_PERM_COMMAND_CHARACTER_DELETED_LIST                 = 279,
        RBAC_PERM_COMMAND_CHARACTER_DELETED_RESTORE              = 280,
        RBAC_PERM_COMMAND_CHARACTER_DELETED_OLD                  = 281,
        RBAC_PERM_COMMAND_CHARACTER_ERASE                        = 282,
        RBAC_PERM_COMMAND_CHARACTER_LEVEL                        = 283,
        RBAC_PERM_COMMAND_CHARACTER_RENAME                       = 284,
        RBAC_PERM_COMMAND_CHARACTER_REPUTATION                   = 285,
        RBAC_PERM_COMMAND_CHARACTER_TITLES                       = 286,
        RBAC_PERM_COMMAND_LEVELUP                                = 287,
        // 288 previously used, do not reuse
        RBAC_PERM_COMMAND_PDUMP_LOAD                             = 289,
        RBAC_PERM_COMMAND_PDUMP_WRITE                            = 290,
        // 291 previously used, do not reuse
        RBAC_PERM_COMMAND_CHEAT_CASTTIME                         = 292,
        RBAC_PERM_COMMAND_CHEAT_COOLDOWN                         = 293,
        RBAC_PERM_COMMAND_CHEAT_EXPLORE                          = 294,
        RBAC_PERM_COMMAND_CHEAT_GOD                              = 295,
        RBAC_PERM_COMMAND_CHEAT_POWER                            = 296,
        RBAC_PERM_COMMAND_CHEAT_STATUS                           = 297,
        RBAC_PERM_COMMAND_CHEAT_TAXI                             = 298,
        RBAC_PERM_COMMAND_CHEAT_WATERWALK                        = 299,
        RBAC_PERM_COMMAND_DEBUG                                  = 300,
        // 301-342 previously used, do not reuse
        RBAC_PERM_COMMAND_DESERTER_BG_ADD                        = 343,
        RBAC_PERM_COMMAND_DESERTER_BG_REMOVE                     = 344,
        // 345 previously used, do not reuse
        RBAC_PERM_COMMAND_DESERTER_INSTANCE_ADD                  = 346,
        RBAC_PERM_COMMAND_DESERTER_INSTANCE_REMOVE               = 347,
        // 348-349 previously used, do not reuse
        RBAC_PERM_COMMAND_DISABLE_ADD_ACHIEVEMENT_CRITERIA       = 350,
        RBAC_PERM_COMMAND_DISABLE_ADD_BATTLEGROUND               = 351,
        RBAC_PERM_COMMAND_DISABLE_ADD_MAP                        = 352,
        RBAC_PERM_COMMAND_DISABLE_ADD_MMAP                       = 353,
        RBAC_PERM_COMMAND_DISABLE_ADD_OUTDOORPVP                 = 354,
        RBAC_PERM_COMMAND_DISABLE_ADD_QUEST                      = 355,
        RBAC_PERM_COMMAND_DISABLE_ADD_SPELL                      = 356,
        RBAC_PERM_COMMAND_DISABLE_ADD_VMAP                       = 357,
        // 358 previously used, do not reuse
        RBAC_PERM_COMMAND_DISABLE_REMOVE_ACHIEVEMENT_CRITERIA    = 359,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_BATTLEGROUND            = 360,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_MAP                     = 361,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_MMAP                    = 362,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_OUTDOORPVP              = 363,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_QUEST                   = 364,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_SPELL                   = 365,
        RBAC_PERM_COMMAND_DISABLE_REMOVE_VMAP                    = 366,
        RBAC_PERM_COMMAND_EVENT_INFO                             = 367,
        RBAC_PERM_COMMAND_EVENT_ACTIVELIST                       = 368,
        RBAC_PERM_COMMAND_EVENT_START                            = 369,
        RBAC_PERM_COMMAND_EVENT_STOP                             = 370,
        RBAC_PERM_COMMAND_GM                                     = 371,
        RBAC_PERM_COMMAND_GM_CHAT                                = 372,
        RBAC_PERM_COMMAND_GM_FLY                                 = 373,
        RBAC_PERM_COMMAND_GM_INGAME                              = 374,
        RBAC_PERM_COMMAND_GM_LIST                                = 375,
        RBAC_PERM_COMMAND_GM_VISIBLE                             = 376,
        RBAC_PERM_COMMAND_GO                                     = 377,
        RBAC_PERM_COMMAND_ACCOUNT_2FA                            = 378,
        RBAC_PERM_COMMAND_ACCOUNT_2FA_SETUP                      = 379,
        RBAC_PERM_COMMAND_ACCOUNT_2FA_REMOVE                     = 380,
        RBAC_PERM_COMMAND_ACCOUNT_SET_2FA                        = 381,
        // unused 382-386
        // 387 previously used, do not reuse
        RBAC_PERM_COMMAND_GOBJECT_ACTIVATE                       = 388,
        RBAC_PERM_COMMAND_GOBJECT_ADD                            = 389,
        RBAC_PERM_COMMAND_GOBJECT_ADD_TEMP                       = 390,
        RBAC_PERM_COMMAND_GOBJECT_DELETE                         = 391,
        RBAC_PERM_COMMAND_GOBJECT_INFO                           = 392,
        RBAC_PERM_COMMAND_GOBJECT_MOVE                           = 393,
        RBAC_PERM_COMMAND_GOBJECT_NEAR                           = 394,
        // 395 previously used, do not reuse
        RBAC_PERM_COMMAND_GOBJECT_SET_PHASE                      = 396,
        RBAC_PERM_COMMAND_GOBJECT_SET_STATE                      = 397,
        RBAC_PERM_COMMAND_GOBJECT_TARGET                         = 398,
        RBAC_PERM_COMMAND_GOBJECT_TURN                           = 399,
        // 400 previously used, do not reuse
        RBAC_PERM_COMMAND_GUILD                                  = 401,
        RBAC_PERM_COMMAND_GUILD_CREATE                           = 402,
        RBAC_PERM_COMMAND_GUILD_DELETE                           = 403,
        RBAC_PERM_COMMAND_GUILD_INVITE                           = 404,
        RBAC_PERM_COMMAND_GUILD_UNINVITE                         = 405,
        RBAC_PERM_COMMAND_GUILD_RANK                             = 406,
        RBAC_PERM_COMMAND_GUILD_RENAME                           = 407,
        // 408 previously used, do not reuse
        RBAC_PERM_COMMAND_HONOR_ADD                              = 409,
        RBAC_PERM_COMMAND_HONOR_ADD_KILL                         = 410,
        RBAC_PERM_COMMAND_HONOR_UPDATE                           = 411,
        // 412 previously used, do not reuse
        RBAC_PERM_COMMAND_INSTANCE_LISTBINDS                     = 413,
        RBAC_PERM_COMMAND_INSTANCE_UNBIND                        = 414,
        RBAC_PERM_COMMAND_INSTANCE_STATS                         = 415,
        RBAC_PERM_COMMAND_INSTANCE_SAVEDATA                      = 416,
        RBAC_PERM_COMMAND_LEARN                                  = 417,
        // 418 previously used, do not reuse
        RBAC_PERM_COMMAND_LEARN_ALL_MY                           = 419,
        RBAC_PERM_COMMAND_LEARN_ALL_MY_CLASS                     = 420,
        RBAC_PERM_COMMAND_LEARN_MY_PETTALENTS                    = 421,
        RBAC_PERM_COMMAND_LEARN_ALL_MY_SPELLS                    = 422,
        RBAC_PERM_COMMAND_LEARN_ALL_TALENTS                      = 423,
        RBAC_PERM_COMMAND_LEARN_ALL_GM                           = 424,
        RBAC_PERM_COMMAND_LEARN_ALL_CRAFTS                       = 425,
        RBAC_PERM_COMMAND_LEARN_ALL_DEFAULT                      = 426,
        RBAC_PERM_COMMAND_LEARN_ALL_LANG                         = 427,
        RBAC_PERM_COMMAND_LEARN_ALL_RECIPES                      = 428,
        RBAC_PERM_COMMAND_UNLEARN                                = 429,
        // 430 previously used, do not reuse
        RBAC_PERM_COMMAND_LFG_PLAYER                             = 431,
        RBAC_PERM_COMMAND_LFG_GROUP                              = 432,
        RBAC_PERM_COMMAND_LFG_QUEUE                              = 433,
        RBAC_PERM_COMMAND_LFG_CLEAN                              = 434,
        RBAC_PERM_COMMAND_LFG_OPTIONS                            = 435,
        // 436 previously used, do not reuse
        RBAC_PERM_COMMAND_LIST_CREATURE                          = 437,
        RBAC_PERM_COMMAND_LIST_ITEM                              = 438,
        RBAC_PERM_COMMAND_LIST_OBJECT                            = 439,
        RBAC_PERM_COMMAND_LIST_AURAS                             = 440,
        RBAC_PERM_COMMAND_LIST_MAIL                              = 441,
        RBAC_PERM_COMMAND_LOOKUP                                 = 442,
        RBAC_PERM_COMMAND_LOOKUP_AREA                            = 443,
        RBAC_PERM_COMMAND_LOOKUP_CREATURE                        = 444,
        RBAC_PERM_COMMAND_LOOKUP_EVENT                           = 445,
        RBAC_PERM_COMMAND_LOOKUP_FACTION                         = 446,
        RBAC_PERM_COMMAND_LOOKUP_ITEM                            = 447,
        RBAC_PERM_COMMAND_LOOKUP_ITEMSET                         = 448,
        RBAC_PERM_COMMAND_LOOKUP_OBJECT                          = 449,
        RBAC_PERM_COMMAND_LOOKUP_QUEST                           = 450,
        RBAC_PERM_COMMAND_LOOKUP_PLAYER                          = 451,
        RBAC_PERM_COMMAND_LOOKUP_PLAYER_IP                       = 452,
        RBAC_PERM_COMMAND_LOOKUP_PLAYER_ACCOUNT                  = 453,
        RBAC_PERM_COMMAND_LOOKUP_PLAYER_EMAIL                    = 454,
        RBAC_PERM_COMMAND_LOOKUP_SKILL                           = 455,
        RBAC_PERM_COMMAND_LOOKUP_SPELL                           = 456,
        RBAC_PERM_COMMAND_LOOKUP_SPELL_ID                        = 457,
        RBAC_PERM_COMMAND_LOOKUP_TAXINODE                        = 458,
        RBAC_PERM_COMMAND_LOOKUP_TELE                            = 459,
        RBAC_PERM_COMMAND_LOOKUP_TITLE                           = 460,
        RBAC_PERM_COMMAND_LOOKUP_MAP                             = 461,
        RBAC_PERM_COMMAND_ANNOUNCE                               = 462,
        RBAC_PERM_COMMAND_CHANNEL                                = 463,
        RBAC_PERM_COMMAND_CHANNEL_SET                            = 464,
        RBAC_PERM_COMMAND_CHANNEL_SET_OWNERSHIP                  = 465,
        RBAC_PERM_COMMAND_GMANNOUNCE                             = 466,
        RBAC_PERM_COMMAND_GMNAMEANNOUNCE                         = 467,
        RBAC_PERM_COMMAND_GMNOTIFY                               = 468,
        RBAC_PERM_COMMAND_NAMEANNOUNCE                           = 469,
        RBAC_PERM_COMMAND_NOTIFY                                 = 470,
        RBAC_PERM_COMMAND_WHISPERS                               = 471,
        RBAC_PERM_COMMAND_GROUP                                  = 472,
        RBAC_PERM_COMMAND_GROUP_LEADER                           = 473,
        RBAC_PERM_COMMAND_GROUP_DISBAND                          = 474,
        RBAC_PERM_COMMAND_GROUP_REMOVE                           = 475,
        RBAC_PERM_COMMAND_GROUP_JOIN                             = 476,
        RBAC_PERM_COMMAND_GROUP_LIST                             = 477,
        RBAC_PERM_COMMAND_GROUP_SUMMON                           = 478,
        RBAC_PERM_COMMAND_PET                                    = 479,
        RBAC_PERM_COMMAND_PET_CREATE                             = 480,
        RBAC_PERM_COMMAND_PET_LEARN                              = 481,
        RBAC_PERM_COMMAND_PET_UNLEARN                            = 482,
        RBAC_PERM_COMMAND_SEND                                   = 483,
        RBAC_PERM_COMMAND_SEND_ITEMS                             = 484,
        RBAC_PERM_COMMAND_SEND_MAIL                              = 485,
        RBAC_PERM_COMMAND_SEND_MESSAGE                           = 486,
        RBAC_PERM_COMMAND_SEND_MONEY                             = 487,
        RBAC_PERM_COMMAND_ADDITEM                                = 488,
        RBAC_PERM_COMMAND_ADDITEMSET                             = 489,
        RBAC_PERM_COMMAND_APPEAR                                 = 490,
        RBAC_PERM_COMMAND_AURA                                   = 491,
        RBAC_PERM_COMMAND_BANK                                   = 492,
        RBAC_PERM_COMMAND_BINDSIGHT                              = 493,
        RBAC_PERM_COMMAND_COMBATSTOP                             = 494,
        RBAC_PERM_COMMAND_COMETOME                               = 495,
        RBAC_PERM_COMMAND_COMMANDS                               = 496,
        RBAC_PERM_COMMAND_COOLDOWN                               = 497,
        RBAC_PERM_COMMAND_DAMAGE                                 = 498,
        RBAC_PERM_COMMAND_DEV                                    = 499,
        RBAC_PERM_COMMAND_DIE                                    = 500,
        RBAC_PERM_COMMAND_DISMOUNT                               = 501,
        RBAC_PERM_COMMAND_DISTANCE                               = 502,
        RBAC_PERM_COMMAND_FLUSHARENAPOINTS                       = 503,
        RBAC_PERM_COMMAND_FREEZE                                 = 504,
        RBAC_PERM_COMMAND_GPS                                    = 505,
        RBAC_PERM_COMMAND_GUID                                   = 506,
        RBAC_PERM_COMMAND_HELP                                   = 507,
        RBAC_PERM_COMMAND_HIDEAREA                               = 508,
        RBAC_PERM_COMMAND_ITEMMOVE                               = 509,
        RBAC_PERM_COMMAND_KICK                                   = 510,
        RBAC_PERM_COMMAND_LINKGRAVE                              = 511,
        RBAC_PERM_COMMAND_LISTFREEZE                             = 512,
        RBAC_PERM_COMMAND_MAXSKILL                               = 513,
        RBAC_PERM_COMMAND_MOVEGENS                               = 514,
        RBAC_PERM_COMMAND_MUTE                                   = 515,
        RBAC_PERM_COMMAND_NEARGRAVE                              = 516,
        RBAC_PERM_COMMAND_PINFO                                  = 517,
        RBAC_PERM_COMMAND_PLAYALL                                = 518,
        RBAC_PERM_COMMAND_POSSESS                                = 519,
        RBAC_PERM_COMMAND_RECALL                                 = 520,
        RBAC_PERM_COMMAND_REPAIRITEMS                            = 521,
        RBAC_PERM_COMMAND_RESPAWN                                = 522,
        RBAC_PERM_COMMAND_REVIVE                                 = 523,
        RBAC_PERM_COMMAND_SAVEALL                                = 524,
        RBAC_PERM_COMMAND_SAVE                                   = 525,
        RBAC_PERM_COMMAND_SETSKILL                               = 526,
        RBAC_PERM_COMMAND_SHOWAREA                               = 527,
        RBAC_PERM_COMMAND_SUMMON                                 = 528,
        RBAC_PERM_COMMAND_UNAURA                                 = 529,
        RBAC_PERM_COMMAND_UNBINDSIGHT                            = 530,
        RBAC_PERM_COMMAND_UNFREEZE                               = 531,
        RBAC_PERM_COMMAND_UNMUTE                                 = 532,
        RBAC_PERM_COMMAND_UNPOSSESS                              = 533,
        RBAC_PERM_COMMAND_UNSTUCK                                = 534,
        RBAC_PERM_COMMAND_WCHANGE                                = 535,
        RBAC_PERM_COMMAND_MMAP                                   = 536,
        RBAC_PERM_COMMAND_MMAP_LOADEDTILES                       = 537,
        RBAC_PERM_COMMAND_MMAP_LOC                               = 538,
        RBAC_PERM_COMMAND_MMAP_PATH                              = 539,
        RBAC_PERM_COMMAND_MMAP_STATS                             = 540,
        RBAC_PERM_COMMAND_MMAP_TESTAREA                          = 541,
        RBAC_PERM_COMMAND_MORPH                                  = 542,
        RBAC_PERM_COMMAND_DEMORPH                                = 543,
        RBAC_PERM_COMMAND_MODIFY                                 = 544,
        RBAC_PERM_COMMAND_MODIFY_ARENAPOINTS                     = 545,
        RBAC_PERM_COMMAND_MODIFY_BIT                             = 546,
        RBAC_PERM_COMMAND_MODIFY_DRUNK                           = 547,
        RBAC_PERM_COMMAND_MODIFY_ENERGY                          = 548,
        RBAC_PERM_COMMAND_MODIFY_FACTION                         = 549,
        RBAC_PERM_COMMAND_MODIFY_GENDER                          = 550,
        RBAC_PERM_COMMAND_MODIFY_HONOR                           = 551,
        RBAC_PERM_COMMAND_MODIFY_HP                              = 552,
        RBAC_PERM_COMMAND_MODIFY_MANA                            = 553,
        RBAC_PERM_COMMAND_MODIFY_MONEY                           = 554,
        RBAC_PERM_COMMAND_MODIFY_MOUNT                           = 555,
        RBAC_PERM_COMMAND_MODIFY_PHASE                           = 556,
        RBAC_PERM_COMMAND_MODIFY_RAGE                            = 557,
        RBAC_PERM_COMMAND_MODIFY_REPUTATION                      = 558,
        RBAC_PERM_COMMAND_MODIFY_RUNICPOWER                      = 559,
        RBAC_PERM_COMMAND_MODIFY_SCALE                           = 560,
        RBAC_PERM_COMMAND_MODIFY_SPEED                           = 561,
        RBAC_PERM_COMMAND_MODIFY_SPEED_ALL                       = 562,
        RBAC_PERM_COMMAND_MODIFY_SPEED_BACKWALK                  = 563,
        RBAC_PERM_COMMAND_MODIFY_SPEED_FLY                       = 564,
        RBAC_PERM_COMMAND_MODIFY_SPEED_WALK                      = 565,
        RBAC_PERM_COMMAND_MODIFY_SPEED_SWIM                      = 566,
        RBAC_PERM_COMMAND_MODIFY_SPELL                           = 567,
        RBAC_PERM_COMMAND_MODIFY_STANDSTATE                      = 568,
        RBAC_PERM_COMMAND_MODIFY_TALENTPOINTS                    = 569,
        // 570 previously used, do not reuse
        RBAC_PERM_COMMAND_NPC_ADD                                = 571,
        RBAC_PERM_COMMAND_NPC_ADD_FORMATION                      = 572,
        RBAC_PERM_COMMAND_NPC_ADD_ITEM                           = 573,
        RBAC_PERM_COMMAND_NPC_ADD_MOVE                           = 574,
        RBAC_PERM_COMMAND_NPC_ADD_TEMP                           = 575,
        RBAC_PERM_COMMAND_NPC_DELETE                             = 576,
        RBAC_PERM_COMMAND_NPC_DELETE_ITEM                        = 577,
        RBAC_PERM_COMMAND_NPC_FOLLOW                             = 578,
        RBAC_PERM_COMMAND_NPC_FOLLOW_STOP                        = 579,
        RBAC_PERM_COMMAND_NPC_SET                                = 580,
        RBAC_PERM_COMMAND_NPC_SET_ALLOWMOVE                      = 581,
        RBAC_PERM_COMMAND_NPC_SET_ENTRY                          = 582,
        RBAC_PERM_COMMAND_NPC_SET_FACTIONID                      = 583,
        RBAC_PERM_COMMAND_NPC_SET_FLAG                           = 584,
        RBAC_PERM_COMMAND_NPC_SET_LEVEL                          = 585,
        RBAC_PERM_COMMAND_NPC_SET_LINK                           = 586,
        RBAC_PERM_COMMAND_NPC_SET_MODEL                          = 587,
        RBAC_PERM_COMMAND_NPC_SET_MOVETYPE                       = 588,
        RBAC_PERM_COMMAND_NPC_SET_PHASE                          = 589,
        RBAC_PERM_COMMAND_NPC_SET_SPAWNDIST                      = 590,
        RBAC_PERM_COMMAND_NPC_SET_SPAWNTIME                      = 591,
        RBAC_PERM_COMMAND_NPC_SET_DATA                           = 592,
        RBAC_PERM_COMMAND_NPC_INFO                               = 593,
        RBAC_PERM_COMMAND_NPC_NEAR                               = 594,
        RBAC_PERM_COMMAND_NPC_MOVE                               = 595,
        RBAC_PERM_COMMAND_NPC_PLAYEMOTE                          = 596,
        RBAC_PERM_COMMAND_NPC_SAY                                = 597,
        RBAC_PERM_COMMAND_NPC_TEXTEMOTE                          = 598,
        RBAC_PERM_COMMAND_NPC_WHISPER                            = 599,
        RBAC_PERM_COMMAND_NPC_YELL                               = 600,
        RBAC_PERM_COMMAND_NPC_TAME                               = 601,
        RBAC_PERM_COMMAND_QUEST                                  = 602,
        RBAC_PERM_COMMAND_QUEST_ADD                              = 603,
        RBAC_PERM_COMMAND_QUEST_COMPLETE                         = 604,
        RBAC_PERM_COMMAND_QUEST_REMOVE                           = 605,
        RBAC_PERM_COMMAND_QUEST_REWARD                           = 606,
        RBAC_PERM_COMMAND_RELOAD                                 = 607,
        RBAC_PERM_COMMAND_RELOAD_ACCESS_REQUIREMENT              = 608,
        RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_CRITERIA_DATA       = 609,
        RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD              = 610,
        RBAC_PERM_COMMAND_RELOAD_ALL                             = 611,
        RBAC_PERM_COMMAND_RELOAD_ALL_ACHIEVEMENT                 = 612,
        RBAC_PERM_COMMAND_RELOAD_ALL_AREA                        = 613,
        RBAC_PERM_COMMAND_RELOAD_BROADCAST_TEXT                  = 614,
        RBAC_PERM_COMMAND_RELOAD_ALL_GOSSIP                      = 615,
        RBAC_PERM_COMMAND_RELOAD_ALL_ITEM                        = 616,
        RBAC_PERM_COMMAND_RELOAD_ALL_LOCALES                     = 617,
        RBAC_PERM_COMMAND_RELOAD_ALL_LOOT                        = 618,
        RBAC_PERM_COMMAND_RELOAD_ALL_NPC                         = 619,
        RBAC_PERM_COMMAND_RELOAD_ALL_QUEST                       = 620,
        RBAC_PERM_COMMAND_RELOAD_ALL_SCRIPTS                     = 621,
        RBAC_PERM_COMMAND_RELOAD_ALL_SPELL                       = 622,
        RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_INVOLVEDRELATION    = 623,
        RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TAVERN              = 624,
        RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TELEPORT            = 625,
        RBAC_PERM_COMMAND_RELOAD_AUCTIONS                        = 626,
        RBAC_PERM_COMMAND_RELOAD_AUTOBROADCAST                   = 627,
        // 628 previously used, do not reuse
        RBAC_PERM_COMMAND_RELOAD_CONDITIONS                      = 629,
        RBAC_PERM_COMMAND_RELOAD_CONFIG                          = 630,
        RBAC_PERM_COMMAND_RELOAD_BATTLEGROUND_TEMPLATE           = 631,
        RBAC_PERM_COMMAND_MUTEHISTORY                            = 632,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_LINKED_RESPAWN         = 633,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_LOOT_TEMPLATE          = 634,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_ONKILL_REPUTATION      = 635,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTENDER             = 636,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTSTARTER           = 637,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_SUMMON_GROUPS          = 638,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_TEMPLATE               = 639,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_TEXT                   = 640,
        RBAC_PERM_COMMAND_RELOAD_DISABLES                        = 641,
        RBAC_PERM_COMMAND_RELOAD_DISENCHANT_LOOT_TEMPLATE        = 642,
        RBAC_PERM_COMMAND_RELOAD_EVENT_SCRIPTS                   = 643,
        RBAC_PERM_COMMAND_RELOAD_FISHING_LOOT_TEMPLATE           = 644,
        RBAC_PERM_COMMAND_RELOAD_GRAVEYARD_ZONE                  = 645,
        RBAC_PERM_COMMAND_RELOAD_GAME_TELE                       = 646,
        RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTENDER           = 647,
        RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUEST_LOOT_TEMPLATE  = 648,
        RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTSTARTER         = 649,
        RBAC_PERM_COMMAND_RELOAD_GM_TICKETS                      = 650,
        RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU                     = 651,
        RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION              = 652,
        RBAC_PERM_COMMAND_RELOAD_ITEM_ENCHANTMENT_TEMPLATE       = 653,
        RBAC_PERM_COMMAND_RELOAD_ITEM_LOOT_TEMPLATE              = 654,
        RBAC_PERM_COMMAND_RELOAD_ITEM_SET_NAMES                  = 655,
        RBAC_PERM_COMMAND_RELOAD_LFG_DUNGEON_REWARDS             = 656,
        RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD_LOCALE       = 657,
        RBAC_PERM_COMMAND_RELOAD_CRETURE_TEMPLATE_LOCALE         = 658,
        RBAC_PERM_COMMAND_RELOAD_CRETURE_TEXT_LOCALE             = 659,
        RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_TEMPLATE_LOCALE      = 660,
        RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION_LOCALE       = 661,
        RBAC_PERM_COMMAND_RELOAD_ITEM_TEMPLATE_LOCALE            = 662,
        RBAC_PERM_COMMAND_RELOAD_ITEM_SET_NAME_LOCALE            = 663,
        RBAC_PERM_COMMAND_RELOAD_NPC_TEXT_LOCALE                 = 664,
        RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT_LOCALE                = 665,
        RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST_LOCALE       = 666,
        RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE_LOCALE           = 667,
        RBAC_PERM_COMMAND_RELOAD_MAIL_LEVEL_REWARD               = 668,
        RBAC_PERM_COMMAND_RELOAD_MAIL_LOOT_TEMPLATE              = 669,
        RBAC_PERM_COMMAND_RELOAD_MILLING_LOOT_TEMPLATE           = 670,
        RBAC_PERM_COMMAND_RELOAD_NPC_SPELLCLICK_SPELLS           = 671,
        RBAC_PERM_COMMAND_RELOAD_TRAINER                         = 672,
        RBAC_PERM_COMMAND_RELOAD_NPC_VENDOR                      = 673,
        RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT                       = 674,
        RBAC_PERM_COMMAND_RELOAD_PICKPOCKETING_LOOT_TEMPLATE     = 675,
        RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST              = 676,
        RBAC_PERM_COMMAND_RELOAD_PROSPECTING_LOOT_TEMPLATE       = 677,
        RBAC_PERM_COMMAND_RELOAD_QUEST_POI                       = 678,
        RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE                  = 679,
        RBAC_PERM_COMMAND_RELOAD_RBAC                            = 680,
        RBAC_PERM_COMMAND_RELOAD_REFERENCE_LOOT_TEMPLATE         = 681,
        RBAC_PERM_COMMAND_RELOAD_RESERVED_NAME                   = 682,
        RBAC_PERM_COMMAND_RELOAD_REPUTATION_REWARD_RATE          = 683,
        RBAC_PERM_COMMAND_RELOAD_SPILLOVER_TEMPLATE              = 684,
        RBAC_PERM_COMMAND_RELOAD_SKILL_DISCOVERY_TEMPLATE        = 685,
        RBAC_PERM_COMMAND_RELOAD_SKILL_EXTRA_ITEM_TEMPLATE       = 686,
        RBAC_PERM_COMMAND_RELOAD_SKILL_FISHING_BASE_LEVEL        = 687,
        RBAC_PERM_COMMAND_RELOAD_SKINNING_LOOT_TEMPLATE          = 688,
        RBAC_PERM_COMMAND_RELOAD_SMART_SCRIPTS                   = 689,
        RBAC_PERM_COMMAND_RELOAD_SPELL_REQUIRED                  = 690,
        RBAC_PERM_COMMAND_RELOAD_SPELL_AREA                      = 691,
        RBAC_PERM_COMMAND_RELOAD_SPELL_BONUS_DATA                = 692,
        RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP                     = 693,
        RBAC_PERM_COMMAND_RELOAD_SPELL_LEARN_SPELL               = 694,
        RBAC_PERM_COMMAND_RELOAD_SPELL_LOOT_TEMPLATE             = 695,
        RBAC_PERM_COMMAND_RELOAD_SPELL_LINKED_SPELL              = 696,
        RBAC_PERM_COMMAND_RELOAD_SPELL_PET_AURAS                 = 697,
        RBAC_PERM_COMMAND_CHARACTER_CHANGEACCOUNT                = 698,
        RBAC_PERM_COMMAND_RELOAD_SPELL_PROC                      = 699,
        // 700 previously used, do not reuse
        RBAC_PERM_COMMAND_RELOAD_SPELL_TARGET_POSITION           = 701,
        RBAC_PERM_COMMAND_RELOAD_SPELL_THREATS                   = 702,
        RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP_STACK_RULES         = 703,
        RBAC_PERM_COMMAND_RELOAD_TRINITY_STRING                  = 704,
        // 705 previously used, do not reuse
        RBAC_PERM_COMMAND_RELOAD_WAYPOINT_SCRIPTS                = 706,
        RBAC_PERM_COMMAND_RELOAD_WAYPOINT_DATA                   = 707,
        RBAC_PERM_COMMAND_RELOAD_VEHICLE_ACCESORY                = 708,
        RBAC_PERM_COMMAND_RELOAD_VEHICLE_TEMPLATE_ACCESSORY      = 709,
        RBAC_PERM_COMMAND_RESET                                  = 710,
        RBAC_PERM_COMMAND_RESET_ACHIEVEMENTS                     = 711,
        RBAC_PERM_COMMAND_RESET_HONOR                            = 712,
        RBAC_PERM_COMMAND_RESET_LEVEL                            = 713,
        RBAC_PERM_COMMAND_RESET_SPELLS                           = 714,
        RBAC_PERM_COMMAND_RESET_STATS                            = 715,
        RBAC_PERM_COMMAND_RESET_TALENTS                          = 716,
        RBAC_PERM_COMMAND_RESET_ALL                              = 717,
        RBAC_PERM_COMMAND_SERVER                                 = 718,
        RBAC_PERM_COMMAND_SERVER_CORPSES                         = 719,
        RBAC_PERM_COMMAND_SERVER_EXIT                            = 720,
        RBAC_PERM_COMMAND_SERVER_IDLERESTART                     = 721,
        RBAC_PERM_COMMAND_SERVER_IDLERESTART_CANCEL              = 722,
        RBAC_PERM_COMMAND_SERVER_IDLESHUTDOWN                    = 723,
        RBAC_PERM_COMMAND_SERVER_IDLESHUTDOWN_CANCEL             = 724,
        RBAC_PERM_COMMAND_SERVER_INFO                            = 725,
        RBAC_PERM_COMMAND_SERVER_PLIMIT                          = 726,
        RBAC_PERM_COMMAND_SERVER_RESTART                         = 727,
        RBAC_PERM_COMMAND_SERVER_RESTART_CANCEL                  = 728,
        RBAC_PERM_COMMAND_SERVER_SET                             = 729,
        RBAC_PERM_COMMAND_SERVER_SET_CLOSED                      = 730,
        RBAC_PERM_COMMAND_SERVER_SET_DIFFTIME                    = 731, // reserved
        RBAC_PERM_COMMAND_SERVER_SET_LOGLEVEL                    = 732,
        RBAC_PERM_COMMAND_SERVER_SET_MOTD                        = 733,
        RBAC_PERM_COMMAND_SERVER_SHUTDOWN                        = 734,
        RBAC_PERM_COMMAND_SERVER_SHUTDOWN_CANCEL                 = 735,
        RBAC_PERM_COMMAND_SERVER_MOTD                            = 736,
        RBAC_PERM_COMMAND_TELE                                   = 737,
        RBAC_PERM_COMMAND_TELE_ADD                               = 738,
        RBAC_PERM_COMMAND_TELE_DEL                               = 739,
        RBAC_PERM_COMMAND_TELE_NAME                              = 740,
        RBAC_PERM_COMMAND_TELE_GROUP                             = 741,
        RBAC_PERM_COMMAND_TICKET                                 = 742,
        RBAC_PERM_COMMAND_TICKET_ASSIGN                          = 743,
        RBAC_PERM_COMMAND_TICKET_CLOSE                           = 744,
        RBAC_PERM_COMMAND_TICKET_CLOSEDLIST                      = 745,
        RBAC_PERM_COMMAND_TICKET_COMMENT                         = 746,
        RBAC_PERM_COMMAND_TICKET_COMPLETE                        = 747,
        RBAC_PERM_COMMAND_TICKET_DELETE                          = 748,
        RBAC_PERM_COMMAND_TICKET_ESCALATE                        = 749,
        RBAC_PERM_COMMAND_TICKET_ESCALATEDLIST                   = 750,
        RBAC_PERM_COMMAND_TICKET_LIST                            = 751,
        RBAC_PERM_COMMAND_TICKET_ONLINELIST                      = 752,
        RBAC_PERM_COMMAND_TICKET_RESET                           = 753,
        RBAC_PERM_COMMAND_TICKET_RESPONSE                        = 754,
        RBAC_PERM_COMMAND_TICKET_RESPONSE_APPEND                 = 755,
        RBAC_PERM_COMMAND_TICKET_RESPONSE_APPENDLN               = 756,
        RBAC_PERM_COMMAND_TICKET_TOGGLESYSTEM                    = 757,
        RBAC_PERM_COMMAND_TICKET_UNASSIGN                        = 758,
        RBAC_PERM_COMMAND_TICKET_VIEWID                          = 759,
        RBAC_PERM_COMMAND_TICKET_VIEWNAME                        = 760,
        // 761 previously used, do not reuse
        RBAC_PERM_COMMAND_TITLES_ADD                             = 762,
        RBAC_PERM_COMMAND_TITLES_CURRENT                         = 763,
        RBAC_PERM_COMMAND_TITLES_REMOVE                          = 764,
        // 765 previously used, do not reuse
        RBAC_PERM_COMMAND_TITLES_SET_MASK                        = 766,
        RBAC_PERM_COMMAND_WP                                     = 767,
        RBAC_PERM_COMMAND_WP_ADD                                 = 768,
        RBAC_PERM_COMMAND_WP_EVENT                               = 769,
        RBAC_PERM_COMMAND_WP_LOAD                                = 770,
        RBAC_PERM_COMMAND_WP_MODIFY                              = 771,
        RBAC_PERM_COMMAND_WP_UNLOAD                              = 772,
        RBAC_PERM_COMMAND_WP_RELOAD                              = 773,
        RBAC_PERM_COMMAND_WP_SHOW                                = 774,
        RBAC_PERM_COMMAND_MODIFY_CURRENCY                        = 775, // not on 3.3.5a
        RBAC_PERM_COMMAND_DEBUG_PHASE                            = 776, // not on 3.3.5a
        RBAC_PERM_COMMAND_MAILBOX                                = 777,
        // 778 previously used, do not reuse
        RBAC_PERM_COMMAND_AHBOT_ITEMS                            = 779,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_GRAY                       = 780,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_WHITE                      = 781,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_GREEN                      = 782,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_BLUE                       = 783,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_PURPLE                     = 784,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_ORANGE                     = 785,
        RBAC_PERM_COMMAND_AHBOT_ITEMS_YELLOW                     = 786,
        RBAC_PERM_COMMAND_AHBOT_RATIO                            = 787,
        RBAC_PERM_COMMAND_AHBOT_RATIO_ALLIANCE                   = 788,
        RBAC_PERM_COMMAND_AHBOT_RATIO_HORDE                      = 789,
        RBAC_PERM_COMMAND_AHBOT_RATIO_NEUTRAL                    = 790,
        RBAC_PERM_COMMAND_AHBOT_REBUILD                          = 791,
        RBAC_PERM_COMMAND_AHBOT_RELOAD                           = 792,
        RBAC_PERM_COMMAND_AHBOT_STATUS                           = 793,
        RBAC_PERM_COMMAND_GUILD_INFO                             = 794,
        RBAC_PERM_COMMAND_INSTANCE_SET_BOSS_STATE                = 795,
        RBAC_PERM_COMMAND_INSTANCE_GET_BOSS_STATE                = 796,
        RBAC_PERM_COMMAND_PVPSTATS                               = 797,
        RBAC_PERM_COMMAND_MODIFY_XP                              = 798,
        RBAC_PERM_COMMAND_GO_BUG_TICKET                          = 799, // not on 3.3.5a
        RBAC_PERM_COMMAND_GO_COMPLAINT_TICKET                    = 800, // not on 3.3.5a
        RBAC_PERM_COMMAND_GO_SUGGESTION_TICKET                   = 801, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG                             = 802, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT                       = 803, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION                      = 804, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_ASSIGN                      = 805, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_CLOSE                       = 806, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_CLOSEDLIST                  = 807, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_COMMENT                     = 808, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_DELETE                      = 809, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_LIST                        = 810, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_UNASSIGN                    = 811, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_BUG_VIEW                        = 812, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_ASSIGN                = 813, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_CLOSE                 = 814, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_CLOSEDLIST            = 815, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_COMMENT               = 816, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_DELETE                = 817, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_LIST                  = 818, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_UNASSIGN              = 819, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_COMPLAINT_VIEW                  = 820, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_ASSIGN               = 821, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_CLOSE                = 822, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_CLOSEDLIST           = 823, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_COMMENT              = 824, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_DELETE               = 825, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_LIST                 = 826, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_UNASSIGN             = 827, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_SUGGESTION_VIEW                 = 828, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_RESET_ALL                       = 829, // not on 3.3.5a
        RBAC_PERM_COMMAND_BNET_ACCOUNT_LIST_GAME_ACCOUTNS        = 830, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_RESET_BUG                       = 831, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_RESET_COMPLAINT                 = 832, // not on 3.3.5a
        RBAC_PERM_COMMAND_TICKET_RESET_SUGGESTION                = 833, // not on 3.3.5a
        RBAC_PERM_COMMAND_GO_QUEST                               = 834, // not on 3.3.5a
        // 835-836 previously used, do not reuse
        RBAC_PERM_COMMAND_NPC_EVADE                              = 837,
        RBAC_PERM_COMMAND_PET_LEVEL                              = 838,
        RBAC_PERM_COMMAND_SERVER_SHUTDOWN_FORCE                  = 839,
        RBAC_PERM_COMMAND_SERVER_RESTART_FORCE                   = 840,
        RBAC_PERM_COMMAND_NEARGRAVEYARD                          = 841,
        RBAC_PERM_COMMAND_RELOAD_CHARACTER_TEMPLATE              = 842, // not on 3.3.5a
        RBAC_PERM_COMMAND_RELOAD_QUEST_GREETING                  = 843,
        RBAC_PERM_COMMAND_SCENE                                  = 844, // not on 3.3.5a
        RBAC_PERM_COMMAND_SCENE_DEBUG                            = 845, // not on 3.3.5a
        RBAC_PERM_COMMAND_SCENE_PLAY                             = 846, // not on 3.3.5a
        RBAC_PERM_COMMAND_SCENE_PLAY_PACKAGE                     = 847, // not on 3.3.5a
        RBAC_PERM_COMMAND_SCENE_CANCEL                           = 848, // not on 3.3.5a
        RBAC_PERM_COMMAND_LIST_SCENES                            = 849, // not on 3.3.5a
        RBAC_PERM_COMMAND_RELOAD_SCENE_TEMPLATE                  = 850, // not on 3.3.5a
        RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TEMPLATE            = 851, // not on 3.3.5a
        // 852 previously used, do not reuse
        RBAC_PERM_COMMAND_RELOAD_CONVERSATION_TEMPLATE           = 853, // not on 3.3.5a
        RBAC_PERM_COMMAND_DEBUG_CONVERSATION                     = 854, // not on 3.3.5a
        // 855 previously used, do not reuse
        RBAC_PERM_COMMAND_NPC_SPAWNGROUP                         = 856,
        RBAC_PERM_COMMAND_NPC_DESPAWNGROUP                       = 857,
        RBAC_PERM_COMMAND_GOBJECT_SPAWNGROUP                     = 858,
        RBAC_PERM_COMMAND_GOBJECT_DESPAWNGROUP                   = 859,
        RBAC_PERM_COMMAND_LIST_RESPAWNS                          = 860,
        RBAC_PERM_COMMAND_GROUP_SET                              = 861,
        RBAC_PERM_COMMAND_GROUP_ASSISTANT                        = 862,
        RBAC_PERM_COMMAND_GROUP_MAINTANK                         = 863,
        RBAC_PERM_COMMAND_GROUP_MAINASSIST                       = 864,
        RBAC_PERM_COMMAND_NPC_SHOWLOOT                           = 865,
        RBAC_PERM_COMMAND_LIST_SPAWNPOINTS                       = 866,
        RBAC_PERM_COMMAND_RELOAD_QUEST_GREETING_LOCALE           = 867,
        RBAC_PERM_COMMAND_MODIFY_POWER                           = 868, // reserved
        RBAC_PERM_COMMAND_DEBUG_SEND_PLAYER_CHOICE               = 869, // reserved
        // 870-871 previously used, do not reuse
        RBAC_PERM_COMMAND_SERVER_DEBUG                           = 872,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_MOVEMENT_OVERRIDE      = 873,
        // 874 previously used, do not reuse
        RBAC_PERM_COMMAND_LOOKUP_MAP_ID                          = 875,
        RBAC_PERM_COMMAND_LOOKUP_ITEM_ID                         = 876,
        RBAC_PERM_COMMAND_LOOKUP_QUEST_ID                        = 877,
        // 878-879 previously used, do not reuse
        RBAC_PERM_COMMAND_PDUMP_COPY                             = 880,
        RBAC_PERM_COMMAND_RELOAD_VEHICLE_TEMPLATE                = 881,
        RBAC_PERM_COMMAND_BG_START                               = 884,
        RBAC_PERM_COMMAND_BG_STOP                                = 885,
        RBAC_PERM_COMMAND_ACCOUNT_SET_SEC                        = 1045,
        RBAC_PERM_COMMAND_ACCOUNT_SET_GMLEVEL                    = 1046,
        RBAC_PERM_COMMAND_ACCOUNT_ONLINELIST                     = 1047,
        RBAC_PERM_COMMAND_ACCOUNT_BOOST                          = 1048,
        RBAC_PERM_COMMAND_ACHIEVEMENT_INFO_CRITERIA              = 1049,
        RBAC_PERM_COMMAND_ACHIEVEMENT_INFO_MODIFIER              = 1050,
        RBAC_PERM_COMMAND_ACHIEVEMENT_INFO                       = 1051,
        RBAC_PERM_COMMAND_ACHIEVEMENT_REMOVE                     = 1052,
        RBAC_PERM_COMMAND_ACHIEVEMENT                            = 1053,
        RBAC_PERM_COMMAND_AHBOT                                  = 1054,
        RBAC_PERM_COMMAND_UNBAN_SOLO                             = 1055,
        RBAC_PERM_COMMAND_BAN_SOLO                               = 1056,
        RBAC_PERM_COMMAND_BAN                                    = 1057,
        RBAC_PERM_COMMAND_BANINFO                                = 1058,
        RBAC_PERM_COMMAND_BANLIST                                = 1059,
        RBAC_PERM_COMMAND_UNBAN                                  = 1060,
        RBAC_PERM_COMMAND_BNETACCOUNT_GAMEACCOUNTCREATE          = 1061,
        RBAC_PERM_COMMAND_BNETACCOUNT_PASSWORD                   = 1062,
        RBAC_PERM_COMMAND_BNETACCOUNT_LINK                       = 1063,
        RBAC_PERM_COMMAND_BNETACCOUNT_UNLINK                     = 1064,
        RBAC_PERM_COMMAND_CHARACTER_ANTIERROR                    = 1065,
        RBAC_PERM_COMMAND_CHARACTER_DELETED                      = 1067,
        RBAC_PERM_COMMAND_CHARACTER_CHANGECLASS                  = 1068,
        RBAC_PERM_COMMAND_CHARACTER_BOOST                        = 1069,
        RBAC_PERM_COMMAND_CHARACTER                              = 1070,
        RBAC_PERM_COMMAND_PDUMP                                  = 1071,
        RBAC_PERM_COMMAND_CHEAT                                  = 1072,
        RBAC_PERM_COMMAND_DEBUG_PLAY_CINEMATIC                   = 1073,
        RBAC_PERM_COMMAND_DEBUG_PLAY_MOVIE                       = 1074,
        RBAC_PERM_COMMAND_DEBUG_PLAY_SOUND                       = 1075,
        RBAC_PERM_COMMAND_DEBUG_PLAY_VISUAL                      = 1076,
        RBAC_PERM_COMMAND_DEBUG_PLAY_VISUALKIT                   = 1077,
        RBAC_PERM_COMMAND_DEBUG_SEND_BUYERROR                    = 1078,
        RBAC_PERM_COMMAND_DEBUG_SEND_CHANNELNOTIFY               = 1079,
        RBAC_PERM_COMMAND_DEBUG_SEND_CHATMESSAGE                 = 1080,
        RBAC_PERM_COMMAND_DEBUG_SEND_EQUIPERROR                  = 1081,
        RBAC_PERM_COMMAND_DEBUG_SEND_LARGEPACKET                 = 1082,
        RBAC_PERM_COMMAND_DEBUG_SEND_OPCODE                      = 1083,
        RBAC_PERM_COMMAND_DEBUG_SEND_QPARTYMSG                   = 1084,
        RBAC_PERM_COMMAND_DEBUG_SEND_QINVALIDMSG                 = 1085,
        RBAC_PERM_COMMAND_DEBUG_SEND_SELLERROR                   = 1086,
        RBAC_PERM_COMMAND_DEBUG_SEND_SETPHASESHIFT               = 1087,
        RBAC_PERM_COMMAND_DEBUG_SEND_SPELLFAIL                   = 1088,
        RBAC_PERM_COMMAND_DEBUG_SETBIT                           = 1089,
        RBAC_PERM_COMMAND_DEBUG_THREAT                           = 1090,
        RBAC_PERM_COMMAND_DEBUG_HOSTIL                           = 1091,
        RBAC_PERM_COMMAND_DEBUG_ANIM                             = 1092,
        RBAC_PERM_COMMAND_DEBUG_ARENA                            = 1093,
        RBAC_PERM_COMMAND_DEBUG_BG                               = 1094,
        RBAC_PERM_COMMAND_DEBUG_RATEDBG                          = 1095,
        RBAC_PERM_COMMAND_DEBUG_GETITEMSTATE                     = 1096,
        RBAC_PERM_COMMAND_DEBUG_LOOTRECIPIENT                    = 1097,
        RBAC_PERM_COMMAND_DEBUG_GETVALUE                         = 1098,
        RBAC_PERM_COMMAND_DEBUG_GETITEMVALUE                     = 1099,
        RBAC_PERM_COMMAND_DEBUG_MOD32VALUE                       = 1100,
        RBAC_PERM_COMMAND_DEBUG_PLAY                             = 1101,
        RBAC_PERM_COMMAND_DEBUG_SEND                             = 1102,
        RBAC_PERM_COMMAND_DEBUG_SETAURASTATE                     = 1103,
        RBAC_PERM_COMMAND_DEBUG_SETITEMVALUE                     = 1104,
        RBAC_PERM_COMMAND_DEBUG_SETVALUE                         = 1105,
        RBAC_PERM_COMMAND_DEBUG_SPAWNVEHICLE                     = 1106,
        RBAC_PERM_COMMAND_DEBUG_SETVID                           = 1107,
        RBAC_PERM_COMMAND_DEBUG_ENTERVEHICLE                     = 1108,
        RBAC_PERM_COMMAND_DEBUG_UWS                              = 1109,
        RBAC_PERM_COMMAND_DEBUG_UPDATE                           = 1110,
        RBAC_PERM_COMMAND_DEBUG_ITEMEXPIRE                       = 1111,
        RBAC_PERM_COMMAND_DEBUG_AREATRIGGERS                     = 1112,
        RBAC_PERM_COMMAND_DEBUG_LOS                              = 1113,
        RBAC_PERM_COMMAND_DEBUG_MOVEFLAGS                        = 1114,
        RBAC_PERM_COMMAND_DEBUG_TRANSPORT                        = 1115,
        RBAC_PERM_COMMAND_DEBUG_CASTERROR                        = 1116,
        RBAC_PERM_COMMAND_DEBUG_SET                              = 1117,
        RBAC_PERM_COMMAND_DEBUG_SET_AP                           = 1118,
        RBAC_PERM_COMMAND_DEBUG_SET_SP                           = 1119,
        RBAC_PERM_COMMAND_DEBUG_ARCHAEOLOGY                      = 1120,
        RBAC_PERM_COMMAND_DEBUG_ARCHAEOLOGY_FIND                 = 1121,
        RBAC_PERM_COMMAND_DEBUG_ARCHAEOLOGY_DIGSITE              = 1122,
        RBAC_PERM_COMMAND_DEBUG_VIGNETTE                         = 1123,
        RBAC_PERM_COMMAND_DEBUG_VALUE                            = 1124,
        RBAC_PERM_COMMAND_WPGPS                                  = 1125,
        RBAC_PERM_COMMAND_DESERTER_INSTANCE                      = 1126,
        RBAC_PERM_COMMAND_DESERTER_BG                            = 1127,
        RBAC_PERM_COMMAND_DESERTER                               = 1128,
        RBAC_PERM_COMMAND_DISABLE_ADD                            = 1129,
        RBAC_PERM_COMMAND_DISABLE_REMOVE                         = 1130,
        RBAC_PERM_COMMAND_DISABLE                                = 1131,
        RBAC_PERM_COMMAND_EVENT                                  = 1132,
        RBAC_PERM_COMMAND_GO_CREATURE                            = 1133,
        RBAC_PERM_COMMAND_GO_GRAVEYARD                           = 1134,
        RBAC_PERM_COMMAND_GO_GRID                                = 1135,
        RBAC_PERM_COMMAND_GO_OBJECT                              = 1136,
        RBAC_PERM_COMMAND_GO_TAXINODE                            = 1137,
        RBAC_PERM_COMMAND_GO_TRIGGER                             = 1138,
        RBAC_PERM_COMMAND_GO_ZONEXY                              = 1139,
        RBAC_PERM_COMMAND_GO_XYZ                                 = 1140,
        RBAC_PERM_COMMAND_GO_TICKET                              = 1141,
        RBAC_PERM_COMMAND_GOBJECT_SET_PHASEID                    = 1142,
        RBAC_PERM_COMMAND_GOBJECT_SET                            = 1143,
        RBAC_PERM_COMMAND_GOBJECT                                = 1144,
        RBAC_PERM_COMMAND_GUILD_LEVEL                            = 1145,
        RBAC_PERM_COMMAND_INSTANCE                               = 1146,
        RBAC_PERM_COMMAND_LEARN_ALL_MY_PETTALENTS                = 1147,
        RBAC_PERM_COMMAND_LEARN_ALL_MY_TALENTS                   = 1148,
        RBAC_PERM_COMMAND_LEARN_ALL_MY_GLYPHS                    = 1149,
        RBAC_PERM_COMMAND_LEARN_ALL                              = 1150,
        RBAC_PERM_COMMAND_LFG_PROFILING                          = 1151,
        RBAC_PERM_COMMAND_LFG_BUG                                = 1152,
        RBAC_PERM_COMMAND_LFG_FLEX                               = 1153,
        RBAC_PERM_COMMAND_LFG_FLEX_REG                           = 1154,
        RBAC_PERM_COMMAND_LFG_FLEX_SCALE                         = 1155,
        RBAC_PERM_COMMAND_LFG                                    = 1156,
        RBAC_PERM_COMMAND_LIST_AGGRO                             = 1157,
        RBAC_PERM_COMMAND_LIST_HOSTILES                          = 1158,
        RBAC_PERM_COMMAND_LIST_THREAT                            = 1159,
        RBAC_PERM_COMMAND_LIST                                   = 1160,
        RBAC_PERM_COMMAND_LOOKUP_PLAYER_HWID                     = 1161,
        RBAC_PERM_COMMAND_WORDFILTER_BADWORD_ADD                 = 1162,
        RBAC_PERM_COMMAND_WORDFILTER_BADWORD_REMOVE              = 1163,
        RBAC_PERM_COMMAND_WORDFILTER_BADWORD_LIST                = 1164,
        RBAC_PERM_COMMAND_WORDFILTER_BADWORD                     = 1165,
        RBAC_PERM_COMMAND_WORDFILTER_MOD                         = 1166,
        RBAC_PERM_COMMAND_TOOL_MOVE                              = 1167,
        RBAC_PERM_COMMAND_TOOL_JUMP                              = 1168,
        RBAC_PERM_COMMAND_TOOL_FACING                            = 1169,
        RBAC_PERM_COMMAND_TOOL_TELEPORT                          = 1170,
        RBAC_PERM_COMMAND_TOOL_XY                                = 1171,
        RBAC_PERM_COMMAND_TOOL_XYZ                               = 1172,
        RBAC_PERM_COMMAND_TOOL_DISTANCE                          = 1173,
        RBAC_PERM_COMMAND_TOOL_ANGLE                             = 1174,
        RBAC_PERM_COMMAND_TOOL_NEARGO                            = 1175,
        RBAC_PERM_COMMAND_TOOL_SPAWNCREATURE                     = 1176,
        RBAC_PERM_COMMAND_TOOL_SPAWNGO                           = 1177,
        RBAC_PERM_COMMAND_TOOL_LOS                               = 1178,
        RBAC_PERM_COMMAND_TOOL_MMAPS                             = 1179,
        RBAC_PERM_COMMAND_VISUALIZE_POSITION                     = 1180,
        RBAC_PERM_COMMAND_VISUALIZE_COMBATREACH                  = 1181,
        RBAC_PERM_COMMAND_BG_REGRATED                            = 1182,
        RBAC_PERM_COMMAND_VISIBILITY_ACTIVE                      = 1183,
        RBAC_PERM_COMMAND_VISIBILITY_ACTIVEGO                    = 1184,
        RBAC_PERM_COMMAND_VISIBILITY_GET                         = 1185,
        RBAC_PERM_COMMAND_VISIBILITY_GETGO                       = 1186,
        RBAC_PERM_COMMAND_VISIBILITY_SET                         = 1187,
        RBAC_PERM_COMMAND_VISIBILITY_SETGO                       = 1188,
        RBAC_PERM_COMMAND_VISIBILITY_RELOAD                      = 1189,
        RBAC_PERM_COMMAND_VISIBILITY_RELOADGO                    = 1190,
        RBAC_PERM_COMMAND_VISIBILITY_RELOADALL                   = 1191,
        RBAC_PERM_COMMAND_VISIBILITY_STATS                       = 1192,
        RBAC_PERM_COMMAND_VISIBILITY_UPDATE                      = 1193,
        RBAC_PERM_COMMAND_MUTELIST_ACCOUNT                       = 1194,
        RBAC_PERM_COMMAND_MUTELIST_CHARACTER                     = 1195,
        RBAC_PERM_COMMAND_REPLACE_SKILL                          = 1196,
        RBAC_PERM_COMMAND_GONAME                                 = 1197,
        RBAC_PERM_COMMAND_MUTELIST                               = 1198,
        RBAC_PERM_COMMAND_NEARAREATRIGGER                        = 1199,
        RBAC_PERM_COMMAND_NAMEGO                                 = 1200,
        RBAC_PERM_COMMAND_TOOL                                   = 1201,
        RBAC_PERM_COMMAND_VISUALIZE                              = 1202,
        RBAC_PERM_COMMAND_ITEMSPECS                              = 1203,
        RBAC_PERM_COMMAND_LOOT                                   = 1204,
        RBAC_PERM_COMMAND_LOOT_COOLDOWN                          = 1205,
        RBAC_PERM_COMMAND_LOOT_SHOW                              = 1206,
        RBAC_PERM_COMMAND_LOOT_SHOW_PERSONAL                     = 1207,
        RBAC_PERM_COMMAND_LOOT_SHOW_BONUS                        = 1208,
        RBAC_PERM_COMMAND_LOOT_GET                               = 1209,
        RBAC_PERM_COMMAND_LOOT_GET_PERSONAL                      = 1210,
        RBAC_PERM_COMMAND_LOOT_GET_BONUS                         = 1211,
        RBAC_PERM_COMMAND_BG                                     = 1212,
        RBAC_PERM_COMMAND_ITEMDELETE                             = 1213,
        RBAC_PERM_COMMAND_REMOVEITEM                             = 1214,
        RBAC_PERM_COMMAND_VISIBILITY                             = 1215,
        RBAC_PERM_COMMAND_REPLACE                                = 1216,
        RBAC_PERM_COMMAND_CHECKLADDER                            = 1217,
        RBAC_PERM_COMMAND_WORDFILTER                             = 1218,
        RBAC_PERM_COMMAND_DELETEDITEM                            = 1219,
        RBAC_PERM_COMMAND_DELETEDITEM_LIST                       = 1220,
        RBAC_PERM_COMMAND_DELETEDITEM_RESTORE                    = 1221,
        RBAC_PERM_COMMAND_MMAP_ENABLE_MAP                        = 1222,
        RBAC_PERM_COMMAND_MMAP_ENABLE_ZONE                       = 1223,
        RBAC_PERM_COMMAND_MMAP_ENABLE_AREA                       = 1224,
        RBAC_PERM_COMMAND_MMAP_ENABLE_CREATURE                   = 1225,
        RBAC_PERM_COMMAND_MMAP_ENABLE                            = 1226,
        RBAC_PERM_COMMAND_MMAP_DISABLE_MAP                       = 1227,
        RBAC_PERM_COMMAND_MMAP_DISABLE_ZONE                      = 1228,
        RBAC_PERM_COMMAND_MMAP_DISABLE_AREA                      = 1229,
        RBAC_PERM_COMMAND_MMAP_DISABLE_CREATURE                  = 1230,
        RBAC_PERM_COMMAND_MMAP_DISABLE                           = 1231,
        RBAC_PERM_COMMAND_MMAP_VISUALIZE                         = 1233,
        RBAC_PERM_COMMAND_MODIFY_PHASEID                         = 1234,
        RBAC_PERM_COMMAND_NPC_SET_PHASEID                        = 1235,
        RBAC_PERM_COMMAND_NPC_SET_PHASEGROUP                     = 1236,
        RBAC_PERM_COMMAND_NPC_SET_WANDERDISTANCE                 = 1237,
        RBAC_PERM_COMMAND_NPC_SCALE                              = 1238,
        RBAC_PERM_COMMAND_NPC                                    = 1239,
        RBAC_PERM_COMMAND_RELOAD_ALL_GOSSIPS                     = 1240,
        RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_QUESTSTARTER        = 1241,
        RBAC_PERM_COMMAND_RELOAD_COMMAND                         = 1242,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_TEXT_LOCALE            = 1243,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_SCALING                = 1244,
        RBAC_PERM_COMMAND_RELOAD_GAME_GRAVEYARD_ZONE             = 1245,
        RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_LOOT_TEMPLATE        = 1246,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_TEMPLATE_LOCALE        = 1247,
        RBAC_PERM_COMMAND_RELOAD_NPC_TRAINER                     = 1248,
        RBAC_PERM_COMMAND_RELOAD_PROMOTION_AURAS                 = 1249,
        RBAC_PERM_COMMAND_RELOAD_REPUTATION_SPILLOVER_TEMPLATE   = 1250,
        RBAC_PERM_COMMAND_RELOAD_SPELL_PROC_EVENT                = 1251,
        RBAC_PERM_COMMAND_RELOAD_SPELL_SCRIPTS                   = 1252,
        RBAC_PERM_COMMAND_RELOAD_VEHICLE_ACCESSORY               = 1253,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_DIFFICULTY             = 1254,
        RBAC_PERM_COMMAND_RELOAD_CREATURE_LOOT_CURRENCY          = 1255,
        RBAC_PERM_COMMAND_RELOAD_GOCOLLISION                     = 1256,
        RBAC_PERM_COMMAND_RELOAD_BATTLEPAY                       = 1257,
        RBAC_PERM_COMMAND_RELOAD_BAD_WORD                        = 1258,
        RBAC_PERM_COMMAND_RESET_PVPSTAT                          = 1259,
        RBAC_PERM_COMMAND_ARENA                                  = 1260,
        RBAC_PERM_COMMAND_SERVER_STATS_MAPUPDATE                 = 1266,
        RBAC_PERM_COMMAND_SERVER_STATS                           = 1267,
        RBAC_PERM_COMMAND_SUPPORT                                = 1268,
        RBAC_PERM_COMMAND_SUPPORT_ADD                            = 1269,
        RBAC_PERM_COMMAND_SUPPORT_ADD_SPELL                      = 1270,
        RBAC_PERM_COMMAND_SUPPORT_ADD_SKILL                      = 1271,
        RBAC_PERM_COMMAND_SUPPORT_ADD_ITEM                       = 1272,
        RBAC_PERM_COMMAND_SUPPORT_ADD_TITLE                      = 1273,
        RBAC_PERM_COMMAND_SUPPORT_ADD_ACHIEVEMENT                = 1274,
        RBAC_PERM_COMMAND_SUPPORT_ADD_ACHIEVEMENT_PLAYER         = 1275,
        RBAC_PERM_COMMAND_SUPPORT_ADD_ACHIEVEMENT_ACCOUNT        = 1276,
        RBAC_PERM_COMMAND_SUPPORT_ADD_ACHIEVEMENT_GUILD          = 1277,
        RBAC_PERM_COMMAND_SUPPORT_ADD_QUEST                      = 1278,
        RBAC_PERM_COMMAND_SUPPORT_ADD_QUEST_NEW                  = 1279,
        RBAC_PERM_COMMAND_SUPPORT_ADD_QUEST_COMPLETE             = 1280,
        RBAC_PERM_COMMAND_SUPPORT_ADD_QUEST_REWARD               = 1281,
        RBAC_PERM_COMMAND_SUPPORT_CHECK                          = 1282,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_SPELL                    = 1283,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_SKILL                    = 1284,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ITEM                     = 1285,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ITEMACC                  = 1286,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_TITLE                    = 1287,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ACHIEVEMENT              = 1288,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ACHIEVEMENT_PLAYER       = 1289,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ACHIEVEMENT_ACCOUNT      = 1290,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_ACHIEVEMENT_GUILD        = 1291,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_MAIL                     = 1292,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_MAILACC                  = 1293,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_PROFESSIONS              = 1294,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_AURA                     = 1295,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_AURAS                    = 1296,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_BOOST                    = 1297,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_SERVICES                 = 1298,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_SERVICES_ACTIVE          = 1299,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_SERVICES_EXECUTED        = 1300,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_QUEST                    = 1301,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_QUEST_STATUS             = 1302,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_QUEST_ACTIVE             = 1303,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_QUEST_REWARDED           = 1304,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_MONEY                    = 1305,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_MONEYACC                 = 1306,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_MONEYGUILD               = 1307,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_REPUTATION               = 1308,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_REPUTATIONS              = 1309,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_PVPRATING                = 1310,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_CURRENCY                 = 1311,
        RBAC_PERM_COMMAND_SUPPORT_CHECK_CURRENCIES               = 1312,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE                         = 1313,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_SPELL                   = 1314,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_SKILL                   = 1315,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_ITEM                    = 1316,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_TITLE                   = 1317,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_ACHIEVEMENT             = 1318,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_ACHIEVEMENT_PLAYER      = 1319,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_ACHIEVEMENT_ACCOUNT     = 1320,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_ACHIEVEMENT_GUILD       = 1321,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_AURA                    = 1322,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_QUEST                   = 1323,
        RBAC_PERM_COMMAND_SUPPORT_REMOVE_CACHE                   = 1324,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE                         = 1325,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE_MONEY                   = 1326,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE_REPUTATION              = 1327,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE_PVPRATING               = 1328,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE_PVPMMRRATING            = 1329,
        RBAC_PERM_COMMAND_SUPPORT_CHANGE_CURRENCY                = 1330,
        RBAC_PERM_COMMAND_TICKET_BUG_RESET                       = 1331,
        RBAC_PERM_COMMAND_WARDEN_INFO_ENDSCENE                   = 1332,
        RBAC_PERM_COMMAND_WARDEN_RELOAD                          = 1333,
        RBAC_PERM_COMMAND_WARDEN_ENABLE                          = 1334,
        RBAC_PERM_COMMAND_WARDEN_DISABLE                         = 1335,
        RBAC_PERM_COMMAND_WARDEN_TOGGLE                          = 1336,
        RBAC_PERM_COMMAND_WARDEN_CHECK                           = 1337,
        RBAC_PERM_COMMAND_WARDEN_GROUP                           = 1338,
        RBAC_PERM_COMMAND_WARDEN_DIRECT                          = 1339,
        RBAC_PERM_COMMAND_WARDEN_LIST                            = 1340,
        RBAC_PERM_COMMAND_WARDEN_INFO                            = 1341,
        RBAC_PERM_COMMAND_WARDEN                                 = 1342,
        RBAC_PERM_COMMAND_HELLO                                  = 1343,
        RBAC_PERM_COMMAND_NPCBOT                                  = 1344,
        RBAC_PERM_COMMAND_PMON                                    = 1345,
        //
        // IF YOU ADD NEW PERMISSIONS, ADD THEM IN MASTER BRANCH AS WELL!
        //
        // custom permissions 1000+
        RBAC_PERM_MAX
    };

    enum RBACCommandResult
    {
        RBAC_OK,
        RBAC_CANT_ADD_ALREADY_ADDED,
        RBAC_CANT_REVOKE_NOT_IN_LIST,
        RBAC_IN_GRANTED_LIST,
        RBAC_IN_DENIED_LIST,
        RBAC_ID_DOES_NOT_EXISTS
    };

    typedef std::set<uint32> RBACPermissionContainer;

    class TC_GAME_API RBACPermission
    {
        public:
            RBACPermission(uint32 id = 0, std::string const& name = ""):
                _id(id), _name(name), _perms() { }

            std::string const& GetName() const { return _name; }
            uint32 GetId() const { return _id; }

            RBACPermissionContainer const& GetLinkedPermissions() const { return _perms; }
            void AddLinkedPermission(uint32 id) { _perms.insert(id); }
            void RemoveLinkedPermission(uint32 id) { _perms.erase(id); }

        private:
            uint32 _id;
            std::string _name;
            RBACPermissionContainer _perms;
    };

    class TC_GAME_API RBACData
    {
        public:
            RBACData(uint32 id, std::string const& name, int32 realmId, uint8 secLevel = 255):
                _id(id), _name(name), _realmId(realmId), _secLevel(secLevel),
                _grantedPerms(), _deniedPerms(), _globalPerms() { }

            std::string const& GetName() const { return _name; }
            uint32 GetId() const { return _id; }

            bool HasPermission(uint32 permission) const
            {
                return _globalPerms.find(permission) != _globalPerms.end();
            }

            RBACPermissionContainer const& GetPermissions() const { return _globalPerms; }
            RBACPermissionContainer const& GetGrantedPermissions() const { return _grantedPerms; }
            RBACPermissionContainer const& GetDeniedPermissions() const { return _deniedPerms; }

            RBACCommandResult GrantPermission(uint32 permissionId, int32 realmId = 0);
            RBACCommandResult DenyPermission(uint32 permissionId, int32 realmId = 0);
            RBACCommandResult RevokePermission(uint32 permissionId, int32 realmId = 0);

            void LoadFromDB();
            QueryCallback LoadFromDBAsync();
            void LoadFromDBCallback(PreparedQueryResult result);

            void SetSecurityLevel(uint8 id)
            {
                _secLevel = id;
                LoadFromDB();
            }

            uint8 GetSecurityLevel() const { return _secLevel; }

        private:
            void SavePermission(uint32 role, bool granted, int32 realm);
            void ClearData();
            void CalculateNewPermissions();

            int32 GetRealmId() const { return _realmId; }

            bool HasGrantedPermission(uint32 permissionId) const
            {
                return _grantedPerms.find(permissionId) != _grantedPerms.end();
            }

            bool HasDeniedPermission(uint32 permissionId) const
            {
                return _deniedPerms.find(permissionId) != _deniedPerms.end();
            }

            void AddGrantedPermission(uint32 permissionId)
            {
                _grantedPerms.insert(permissionId);
            }

            void RemoveGrantedPermission(uint32 permissionId)
            {
                _grantedPerms.erase(permissionId);
            }

            void AddDeniedPermission(uint32 permissionId)
            {
                _deniedPerms.insert(permissionId);
            }

            void RemoveDeniedPermission(uint32 permissionId)
            {
                _deniedPerms.erase(permissionId);
            }

            void AddPermissions(RBACPermissionContainer const& permsFrom, RBACPermissionContainer& permsTo);
            void RemovePermissions(RBACPermissionContainer& permsFrom, RBACPermissionContainer const& permsToRemove);
            void ExpandPermissions(RBACPermissionContainer& permissions);

            uint32 _id;
            std::string _name;
            int32 _realmId;
            uint8 _secLevel;
            RBACPermissionContainer _grantedPerms;
            RBACPermissionContainer _deniedPerms;
            RBACPermissionContainer _globalPerms;
    };
}

#endif
