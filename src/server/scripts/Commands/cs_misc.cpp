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

#include "AccountMgr.h"
#include "BattlegroundMgr.h"
#include "CellImpl.h"
#include "Chat.h"
#include "Config.h"
#include "DevTool.h"
#include "DisableMgr.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceSaveMgr.h"
#include "IPLocation.h"
#include "Language.h"
#include "LFG.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "MMapFactory.h"
#include "MovementGenerator.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Pet.h"
#include "Player.h"
#include "Realm.h"
#include "ServiceMgr.h"
#include "ScriptMgr.h"
#include "ServiceMgr.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "TargetedMovementGenerator.h"
#include "WeatherMgr.h"
#include "WordFilterMgr.h"
#include "World.h"
#include <fstream>

class misc_commandscript : public CommandScript
{
public:
    misc_commandscript() : CommandScript("misc_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> badWordCommandTable =
        {
            { "add",                SEC_ADMINISTRATOR,      true,  &HandleBadWordAddCommand, },
            { "remove",             SEC_ADMINISTRATOR,      true,  &HandleBadWordRemoveCommand, },
            { "list",               SEC_ADMINISTRATOR,      true,  &HandleBadWordListCommand, }
        };
        static std::vector<ChatCommand> wordFilterCommandTable =
        {
            { "badword",            SEC_ADMINISTRATOR,      true,  badWordCommandTable },
            { "mod",                SEC_ADMINISTRATOR,      true,  &HandleWordFilterModCommand,}
        };

        static std::vector<ChatCommand> toolCommandTable =
        {
            { "move",           SEC_GAMEMASTER,     false, &HandleToolMoveCommand,              },
            { "jump",           SEC_GAMEMASTER,     false, &HandleToolJumpCommand,              },
            { "facing",         SEC_GAMEMASTER,     false, &HandleToolFacingCommand,            },
            { "teleport",       SEC_GAMEMASTER,     false, &HandleToolTeleportCommand,          },
            { "xy",             SEC_GAMEMASTER,     false, &HandleToolXYCommand,                },
            { "xyz",            SEC_GAMEMASTER,     false, &HandleToolXYZCommand,               },
            { "distance",       SEC_GAMEMASTER,     false, &HandleToolDistanceCommand,          },
            { "angle",          SEC_GAMEMASTER,     false, &HandleToolAngleCommand,             },
            { "neargo",         SEC_GAMEMASTER,     false, &HandleToolNearGOCommand,            },
            { "spawncreature",  SEC_GAMEMASTER,     false, &HandleToolSpawnCreatureCommand,     },
            { "spawngo",        SEC_GAMEMASTER,     false, &HandleToolSpawnGOCommand,           },
            { "los",            SEC_GAMEMASTER,     false, &HandleToolLOSCommand,               },
            { "mmaps",          SEC_GAMEMASTER,     false, &HandleToolMMapsCommand,             },
        };
        static std::vector<ChatCommand> visualizeCommandTable =
        {
            { "position",       SEC_ADMINISTRATOR,      false, &HandleVisualizePositionCommand,     },
            { "combatreach",    SEC_ADMINISTRATOR,      false, &HandleVisualizeCombatReachCommand,  },
        };
        static std::vector<ChatCommand> bgCommandTable =
        {
            { "start",          SEC_ADMINISTRATOR,  false,  &HandleBattlegroundStartCommand     },
            { "regrated",       SEC_ADMINISTRATOR,  false,  &HandleBattlegroundRegRatedCommand  },
        };
        static std::vector<ChatCommand> visibilityCommandTable =
        {
            { "active",         SEC_ADMINISTRATOR,  false,  &HandleVisibilityActiveCommand,      },
            { "activego",       SEC_ADMINISTRATOR,  false,  &HandleVisibilityActiveGOCommand,    },
            { "get",            SEC_ADMINISTRATOR,  false,  &HandleVisibilityGetCommand,         },
            { "getgo",          SEC_ADMINISTRATOR,  false,  &HandleVisibilityGetGOCommand,       },
            { "set",            SEC_ADMINISTRATOR,  false,  &HandleVisibilitySetCommand,         },
            { "setgo",          SEC_ADMINISTRATOR,  false,  &HandleVisibilitySetGOCommand,       },
            { "reload",         SEC_ADMINISTRATOR,  false,  &HandleVisibilityReloadCommand,      },
            { "reloadgo",       SEC_ADMINISTRATOR,  false,  &HandleVisibilityReloadGOCommand,    },
            { "reloadall",      SEC_ADMINISTRATOR,  true,   &HandleVisibilityReloadAllCommand,   },
            { "stats",          SEC_ADMINISTRATOR,  false,  &HandleVisibilityStatsCommand,       },
            { "update",         SEC_ADMINISTRATOR,  false,  &HandleVisibilityUpdateCommand,      },
        };
        static std::vector<ChatCommand> mutelistCommandTable =
        {
            { "account",        SEC_MODERATOR,  true,  HandleMuteListAccountCommand          },
            { "character",      SEC_MODERATOR,  true,  HandleMuteListCharacterCommand        },
        };
        static std::vector<ChatCommand> replaceCommandTable =
        {
            { "skill",          SEC_ADMINISTRATOR, true, HandleReplaceCommand                },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "additem",        SEC_GAMEMASTER, false,  &HandleAddItemCommand,      },
            { "additemset",     SEC_GAMEMASTER, false,  &HandleAddItemSetCommand,   },
            { "goname",         SEC_GAMEMASTER,  false,  &HandleAppearCommand,       },
            { "appear",         SEC_GAMEMASTER,  false,  &HandleAppearCommand,       },
            { "aura",           SEC_GAMEMASTER, false,  &HandleAuraCommand,         },
            { "bank",           SEC_MODERATOR,  false,  &HandleBankCommand,         },
            { "bindsight",      SEC_GAMEMASTER, false,  &HandleBindSightCommand,    },
            { "combatstop",     SEC_GAMEMASTER, true,   &HandleCombatStopCommand,   },
            { "cometome",       SEC_GAMEMASTER,  false,  &HandleComeToMeCommand,     },
            { "commands",       SEC_PLAYER,     true,   &HandleCommandsCommand,     },
            { "cooldown",       SEC_GAMEMASTER, false,  &HandleCooldownCommand,     },
            { "damage",         SEC_GAMEMASTER, false,  &HandleDamageCommand,       },
            { "dev",            SEC_ADMINISTRATOR,  false,  &HandleDevCommand,          },
            { "die",            SEC_GAMEMASTER, false,  &HandleDieCommand,          },
            { "dismount",       SEC_PLAYER,     false,  &HandleDismountCommand,     },
            { "distance",       SEC_GAMEMASTER, false,  &HandleGetDistanceCommand,  },
            { "freeze",         SEC_GAMEMASTER,  false,  &HandleFreezeCommand,       },
            { "gps",            SEC_GAMEMASTER, false,  &HandleGPSCommand,          },
            { "guid",           SEC_GAMEMASTER, false,  &HandleGUIDCommand,         },
            { "help",           SEC_PLAYER,     true,   &HandleHelpCommand,         },
            { "hidearea",       SEC_GAMEMASTER, false,  &HandleHideAreaCommand,     },
            { "itemmove",       SEC_GAMEMASTER, false,  &HandleItemMoveCommand,     },
            { "kick",           SEC_GAMEMASTER,  true,   &HandleKickPlayerCommand,   },
            { "linkgrave",      SEC_GAMEMASTER, false,  &HandleLinkGraveCommand,    },
            { "listfreeze",     SEC_GAMEMASTER, false,  &HandleListFreezeCommand,   },
            { "maxskill",       SEC_GAMEMASTER, false,  &HandleMaxSkillCommand,     },
            { "movegens",       SEC_GAMEMASTER, false,  &HandleMovegensCommand,     },
            { "mute",           SEC_MODERATOR,  true,   &HandleMuteCommand,         },
            { "mutelist",       SEC_MODERATOR,  true,   mutelistCommandTable        },
            { "neargrave",      SEC_GAMEMASTER, false,  &HandleNearGraveCommand,    },
            { "nearareatrigger",SEC_GAMEMASTER, false,  &HandleNearAreaTriggerCommand, },
            { "pinfo",          SEC_GAMEMASTER,  true,   &HandlePInfoCommand,        },
            { "playall",        SEC_GAMEMASTER, false,  &HandlePlayAllCommand,      },
            { "possess",        SEC_GAMEMASTER, false,  &HandlePossessCommand,      },
            { "recall",         SEC_GAMEMASTER,  false,  &HandleRecallCommand,       },
            { "repairitems",    SEC_GAMEMASTER, true,   &HandleRepairitemsCommand,  },
            { "respawn",        SEC_GAMEMASTER, false,  &HandleRespawnCommand,      },
            { "revive",         SEC_GAMEMASTER, true,   &HandleReviveCommand,       },
            { "saveall",        SEC_GAMEMASTER, true,   &HandleSaveAllCommand,      },
            { "save",           SEC_PLAYER,     false,  &HandleSaveCommand,         },
            { "setskill",       SEC_GAMEMASTER, false,  &HandleSetSkillCommand,     },
            { "showarea",       SEC_GAMEMASTER, false,  &HandleShowAreaCommand,     },
            { "namego",         SEC_GAMEMASTER,  false,  &HandleSummonCommand,       },
            { "summon",         SEC_GAMEMASTER,  false,  &HandleSummonCommand,       },
            { "unaura",         SEC_GAMEMASTER, false,  &HandleUnAuraCommand,       },
            { "unbindsight",    SEC_GAMEMASTER, false,  &HandleUnbindSightCommand,  },
            { "unfreeze",       SEC_GAMEMASTER,  false,  &HandleUnFreezeCommand,     },
            { "unmute",         SEC_MODERATOR,  true,   &HandleUnmuteCommand,       },
            { "unpossess",      SEC_GAMEMASTER, false,  &HandleUnPossessCommand,    },
            { "unstuck",        SEC_GAMEMASTER, true,   &HandleUnstuckCommand,      },
            { "wchange",        SEC_GAMEMASTER, false,  &HandleChangeWeather,       },
            { "tool",           SEC_GAMEMASTER, false,  toolCommandTable            },
            { "visualize",      SEC_ADMINISTRATOR,  true,   visualizeCommandTable       },
            { "itemspecs",      SEC_ADMINISTRATOR,  true,   &HandleItemSpecsCommand,    },
            { "loot",           SEC_ADMINISTRATOR,  true,   
            {
                { "cooldown",   SEC_ADMINISTRATOR,  true,   &HandleLootCooldownCommand, },
                { "show",       SEC_ADMINISTRATOR,  true, 
                {
                    { "personal",SEC_ADMINISTRATOR, true,   &HandleLootShowPersonalCommand, },
                    { "bonus",  SEC_ADMINISTRATOR,  true,   &HandleLootShowBonusCommand,    },
                } },
                { "get",        SEC_ADMINISTRATOR,  false,
                {
                    { "personal",SEC_ADMINISTRATOR, true,   &HandleLootGetPersonalCommand,  },
                    { "bonus",  SEC_ADMINISTRATOR,  true,   &HandleLootGetBonusCommand,     },
                } },
            } },
            { "bg",             SEC_ADMINISTRATOR,  false,  bgCommandTable              },
            { "itemdelete",     SEC_GAMEMASTER, true,   &HandleItemDeleteCommand    },
            { "removeitem",     SEC_GAMEMASTER, false,  &HandleRemoveItemCommand    },
            { "visibility",     SEC_ADMINISTRATOR,  true,   visibilityCommandTable      },
            { "replace",        SEC_ADMINISTRATOR, true, replaceCommandTable        },
            { "checkladder",    SEC_ADMINISTRATOR,  true,   &HandleCheckLadderCommand   },
            { "wordfilter",         SEC_ADMINISTRATOR,      false, wordFilterCommandTable },
            { "deleteditem",    SEC_ADMINISTRATOR,  true,
            {
                { "list",      SEC_ADMINISTRATOR,   true,   &HandleDeletedItemListCommand,    },
                { "restore",   SEC_ADMINISTRATOR,   true,   &HandleDeletedItemRestoreCommand, },
            } },
        };
        return commandTable;
    }

    static bool SelectToolHelper(ChatHandler* handler, char const* args, DevToolType type)
    {
        DevToolSettings*& settings = sWorld->SetDevTool(handler->GetSession()->GetAccountId(), type);
        return (settings ? settings : (settings = new DevToolSettings()))->Setup(type, handler, args);
    }
    static bool HandleToolMoveCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Move); }
    static bool HandleToolJumpCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Jump); }
    static bool HandleToolFacingCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Facing); }
    static bool HandleToolTeleportCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Teleport); }
    static bool HandleToolXYCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::OutputXY); }
    static bool HandleToolXYZCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::OutputXYZ); }
    static bool HandleToolDistanceCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Distance); }
    static bool HandleToolAngleCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::Angle); }
    static bool HandleToolNearGOCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::FindNearestGO); }
    static bool HandleToolSpawnCreatureCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::SpawnCreature); }
    static bool HandleToolSpawnGOCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::SpawnGO); }
    static bool HandleToolLOSCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::LOS); }
    static bool HandleToolMMapsCommand(ChatHandler* handler, char const* args) { return SelectToolHelper(handler, args, DevToolType::MMaps); }

    static bool HandleBadWordAddCommand(ChatHandler* handler, char const* args)
    {
        std::string badWord = args;

        const uint8 maxWordSize = 3;
        if (badWord.size() <= maxWordSize)
        {
            handler->PSendSysMessage("The word '%s' is incorrect! The word length must be greater than %u.", badWord.c_str(), maxWordSize);
            return false;
        }

        if (!sWordFilterMgr->AddBadWord(badWord, true))
        {
            handler->PSendSysMessage("The word '%s' is exist!", badWord.c_str());
            return false;
        }

        handler->PSendSysMessage("The word '%s' is added to 'bad_word'.", badWord.c_str());
        return true;
    }

    static bool HandleBadWordRemoveCommand(ChatHandler* handler, char const* args)
    {
        std::string badWord = args;

        if (!sWordFilterMgr->RemoveBadWord(args, true))
        {
            handler->PSendSysMessage("The word '%s' is not exist!", badWord.c_str());
            return false;
        }

        handler->PSendSysMessage("The word '%s' is removed from 'bad_word'.", badWord.c_str());
        return true;
    }

    static bool HandleBadWordListCommand(ChatHandler* handler, char const* args)
    {
        WordFilterMgr::BadWordMap const& badWords = sWordFilterMgr->GetBadWords();

        if (badWords.empty())
        {
            handler->PSendSysMessage("empty.");
            return true;
        }

        std::string strBadWordList;
        uint16 addressesSize = 0;
        const uint16 maxAddressesSize = 255; // !uint8

        for (WordFilterMgr::BadWordMap::const_iterator it = badWords.begin(); it != badWords.end(); ++it)
        {
            strBadWordList.append(it->second);

            if ((*it) != (*badWords.rbegin()))
                strBadWordList.append(", ");
            else
                strBadWordList.append(".");

            // send
            if (addressesSize >= maxAddressesSize)
            {
                handler->PSendSysMessage("Bad words: %s", strBadWordList.c_str());
                strBadWordList.clear();
                addressesSize = 0;
            }

            addressesSize += it->second.size();
        }

        return true;
    }

    static bool HandleWordFilterModCommand(ChatHandler* handler, char const* args)
    {
        std::string argstr = (char*)args;
        if (argstr == "on")
        {
            sWorld->setBoolConfig(CONFIG_WORD_FILTER_ENABLE, true);
            handler->PSendSysMessage("WordFilter: mod is enabled");
            return true;
        }
        if (argstr == "off")
        {
            sWorld->setBoolConfig(CONFIG_WORD_FILTER_ENABLE, false);
            handler->PSendSysMessage("WordFilter: mod is disabled");
            return true;
        }

        std::string strModIs = sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) ? "enabled" : "disabled";
        handler->PSendSysMessage("WordFilter: mod is now %s.", strModIs.c_str());

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);

        return true;
    }

    static bool HandleDevCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            if (handler->GetSession()->GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER))
                handler->GetSession()->SendNotification(LANG_DEV_ON);
            else
                handler->GetSession()->SendNotification(LANG_DEV_OFF);
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER);
            handler->GetSession()->SendNotification(LANG_DEV_ON);
            return true;
        }

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER);
            handler->GetSession()->SendNotification(LANG_DEV_OFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleGPSCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* object = NULL;
        if (*args)
        {
            ObjectGuid guid = handler->extractGuidFromLink((char*)args);
            if (guid)
                object = (WorldObject*)ObjectAccessor::GetObjectByTypeMask(*handler->GetSession()->GetPlayer(), guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);

            if (!object)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            object = handler->getSelectedUnit();

            if (!object)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        CellCoord cellCoord = Trinity::ComputeCellCoord(object->GetPositionX(), object->GetPositionY());
        Cell cell(cellCoord);

        uint32 zoneId, areaId;
        object->GetZoneAndAreaId(zoneId, areaId);
        uint32 mapId = object->GetMapId();

        MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
        AreaTableEntry const* zoneEntry = sAreaTableStore.LookupEntry(zoneId);
        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(areaId);

        float zoneX = object->GetPositionX();
        float zoneY = object->GetPositionY();

        Map2ZoneCoordinates(zoneX, zoneY, zoneId);

        Map const* map = object->GetMap();
        float groundZ = map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), MAX_HEIGHT);
        float floorZ = map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ());

        GridCoord gridCoord = Trinity::ComputeGridCoord(object->GetPositionX(), object->GetPositionY());

        // 63? WHY?
        int gridX = 63 - gridCoord.x_coord;
        int gridY = 63 - gridCoord.y_coord;

        uint32 haveMap = Map::ExistMap(mapId, gridX, gridY) ? 1 : 0;
        uint32 haveVMap = Map::ExistVMap(mapId, gridX, gridY) ? 1 : 0;
        uint32 haveMMap = (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP_MAP, mapId, nullptr) && MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId())) ? 1 : 0;

        if (haveVMap)
        {
            if (map->IsOutdoors(object->GetPositionX(), object->GetPositionY(), object->GetPositionZ()))
                handler->PSendSysMessage("You are outdoors");
            else
                handler->PSendSysMessage("You are indoors");
        }
        else
            handler->PSendSysMessage("no VMAP available for area info");

        std::string diffStr;
        switch (map->GetDifficulty())
        {
            case REGULAR_DIFFICULTY:           diffStr = "Regular";    break;
            case DUNGEON_DIFFICULTY_NORMAL:    diffStr = "Normal";     break;
            case DUNGEON_DIFFICULTY_HEROIC:    diffStr = "Heroic";     break;
            case DUNGEON_DIFFICULTY_CHALLENGE: diffStr = "Challenge";  break;
            case RAID_DIFFICULTY_10MAN_NORMAL: diffStr = "10 Normal";  break;
            case RAID_DIFFICULTY_25MAN_NORMAL: diffStr = "25 Normal";  break;
            case RAID_DIFFICULTY_10MAN_HEROIC: diffStr = "10 Heroic";  break;
            case RAID_DIFFICULTY_25MAN_HEROIC: diffStr = "25 Heroic";  break;
            case RAID_DIFFICULTY_25MAN_LFR:    diffStr = "25 LFR";     break;
            case RAID_DIFFICULTY_1025MAN_FLEX: diffStr = "Flexible";   break;
            case RAID_DIFFICULTY_40MAN:        diffStr = "40 Normal";  break;
            case SCENARIO_DIFFICULTY_NORMAL:   diffStr = "Scenario N"; break;
            case SCENARIO_DIFFICULTY_HEROIC:   diffStr = "Scenario H"; break;
        }

        handler->PSendSysMessage(LANG_MAP_POSITION,
            mapId, (mapEntry ? mapEntry->name[handler->GetSessionDbcLocale()] : "<unknown>"),
            zoneId, (zoneEntry ? zoneEntry->area_name[handler->GetSessionDbcLocale()] : "<unknown>"),
            areaId, (areaEntry ? areaEntry->area_name[handler->GetSessionDbcLocale()] : "<unknown>"),
            object->GetPhaseMask(),
            object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), object->GetOrientation(),
            cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), object->GetInstanceId(),
            (uint32)map->GetDifficulty(), diffStr.c_str(),
            zoneX, zoneY, groundZ, floorZ, haveMap, haveVMap, haveMMap);

        LiquidData liquidStatus;
        float collisionHeight = object->GetTypeId() == TYPEID_PLAYER ? object->ToPlayer()->GetCollisionHeight() : DEFAULT_UNIT_HEIGHT;
        ZLiquidStatus status = map->GetLiquidStatus(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), MAP_ALL_LIQUIDS, &liquidStatus, collisionHeight);

        if (status)
            handler->PSendSysMessage(LANG_LIQUID_STATUS, liquidStatus.level, liquidStatus.depth_level, liquidStatus.entry, liquidStatus.type_flags, status);

        if (!object->GetPhases().empty())
        {
            std::stringstream ss;
            for (uint32 swap : object->GetPhases())
                ss << swap << " ";
            handler->PSendSysMessage("Target's active phase swaps: %s", ss.str().c_str());
        }
        if (!object->GetTerrainSwaps().empty())
        {
            std::stringstream ss;
            for (uint32 swap : object->GetTerrainSwaps())
                ss << swap << " ";
            handler->PSendSysMessage("Target's active terrain swaps: %s", ss.str().c_str());
        }
        if (!object->GetWorldMapSwaps().empty())
        {
            std::stringstream ss;
            for (uint32 swap : object->GetWorldMapSwaps())
                ss << swap << " ";
            handler->PSendSysMessage("Target's active world map area swaps: %s", ss.str().c_str());
        }

        return true;
    }

    static bool HandleAuraCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId, target->GetMap()->GetDifficulty()))
            Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, target, target);

        return true;
    }

    static bool HandleUnAuraCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string argstr = args;
        if (argstr == "all")
        {
            target->RemoveAllAuras();
            return true;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellId = handler->extractSpellIdFromLink((char*)args);
        if (!spellId)
            return false;

        target->RemoveAurasDueToSpell(spellId);

        return true;
    }
    // Teleport to Player
    static bool HandleAppearCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        Player* _player = handler->GetSession()->GetPlayer();
        if (target == _player || targetGuid == _player->GetGUID())
        {
            handler->SendSysMessage(LANG_CANT_TELEPORT_SELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
                return false;

            std::string chrNameLink = handler->playerLink(targetName);

            Map* map = target->GetMap();
            if (map->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!_player->IsGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, chrNameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                    _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                _player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
                // remember current position as entry point for return at bg end teleportation
                if (!_player->GetMap()->IsBattlegroundOrArena())
                    _player->SetBattlegroundEntryPoint();
            }
            else if (map->IsDungeon())
            {
                // we have to go to instance, and can go to player only if:
                //   1) we are in his group (either as leader or as member)
                //   2) we are not bound to any group and have GM mode on
                if (_player->GetGroup())
                {
                    // we are in group, we can go only if we are in the player group
                    if (_player->GetGroup() != target->GetGroup())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, chrNameLink.c_str());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                }
                else
                {
                    // we are not in group, let's verify our GM mode
                    if (!_player->IsGameMaster())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, chrNameLink.c_str());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                }

                // if the player or the player's group is bound to another instance
                // the player will not be bound to another one
                InstancePlayerBind* bind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty(map->IsRaid()));
                if (!bind)
                {
                    Group* group = _player->GetGroup();
                    // if no bind exists, create a solo bind
                    InstanceGroupBind* gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                    if (!gBind)
                        if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
                            _player->BindToInstance(save, !save->CanReset());
                }

                if (map->IsRaid())
                    _player->SetRaidDifficulty(target->GetRaidDifficulty());
                else
                    _player->SetDungeonDifficulty(target->GetDungeonDifficulty());
            }

            handler->PSendSysMessage(LANG_APPEARING_AT, chrNameLink.c_str());

            // stop flight if need
            if (_player->IsInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            // to point to see at target with same orientation
            float x, y, z;
            target->GetContactPoint(_player, x, y, z);

            _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);
            _player->SetPhaseMask(target->GetPhaseMask(), true);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            std::string nameLink = handler->playerLink(targetName);

            handler->PSendSysMessage(LANG_APPEARING_AT, nameLink.c_str());

            // to point where player stay (if loaded)
            float x, y, z, o;
            uint32 map;
            bool in_flight;
            if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, targetGuid))
                return false;

            // stop flight if need
            if (_player->IsInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            _player->TeleportTo(map, x, y, z, _player->GetOrientation());
        }

        return true;
    }
    // Summon Player
    static bool HandleSummonCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        Player* _player = handler->GetSession()->GetPlayer();
        if (target == _player || targetGuid == _player->GetGUID())
        {
            handler->PSendSysMessage(LANG_CANT_TELEPORT_SELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            std::string nameLink = handler->playerLink(targetName);
            // check online security
            if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
                return false;

            if (target->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, nameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            Map* map = handler->GetSession()->GetPlayer()->GetMap();

            if (map->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!_player->IsGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, nameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (target->GetBattlegroundId() && handler->GetSession()->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
                    target->LeaveBattleground(false); // Note: should be changed so target gets no Deserter debuff

                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                target->SetBattlegroundId(handler->GetSession()->GetPlayer()->GetBattlegroundId(), handler->GetSession()->GetPlayer()->GetBattlegroundTypeId());
                // remember current position as entry point for return at bg end teleportation
                if (!target->GetMap()->IsBattlegroundOrArena())
                    target->SetBattlegroundEntryPoint();
            }
            else if (map->IsDungeon())
            {
                Map* destMap = target->GetMap();

                if (destMap->Instanceable() && destMap->GetInstanceId() != map->GetInstanceId())
                    target->UnbindInstance(map->GetInstanceId(), target->GetDungeonDifficulty(), true);

                // we are in an instance, and can only summon players in our group with us as leader
                if (!handler->GetSession()->GetPlayer()->GetGroup() || !target->GetGroup() ||
                    (target->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()) ||
                    (handler->GetSession()->GetPlayer()->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()))
                    // the last check is a bit excessive, but let it be, just in case
                {
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, nameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), "");
            if (handler->needReportToTarget(target))
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_SUMMONED_BY, handler->playerLink(_player->GetName()).c_str());

            // stop flight if need
            if (target->IsInFlight())
            {
                target->GetMotionMaster()->MovementExpired();
                target->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                target->SaveRecallPosition();

            // before GM
            float x, y, z;
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
            target->TeleportTo(handler->GetSession()->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
            target->SetPhaseMask(handler->GetSession()->GetPlayer()->GetPhaseMask(), true);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            std::string nameLink = handler->playerLink(targetName);

            handler->PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), handler->GetTrinityString(LANG_OFFLINE));

            // in point where GM stay
            Player::SavePositionInDB(handler->GetSession()->GetPlayer()->GetMapId(),
                handler->GetSession()->GetPlayer()->GetPositionX(),
                handler->GetSession()->GetPlayer()->GetPositionY(),
                handler->GetSession()->GetPlayer()->GetPositionZ(),
                handler->GetSession()->GetPlayer()->GetOrientation(),
                handler->GetSession()->GetPlayer()->GetZoneId(),
                targetGuid);
        }

        return true;
    }

    static bool HandleCommandsCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->ShowHelpForCommand(handler->getCommandTable(), "");
        return true;
    }

    static bool HandleDieCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();

        if (!target || !handler->GetSession()->GetPlayer()->GetTarget())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (Player* player = target->ToPlayer())
            if (handler->HasLowerSecurity(player, ObjectGuid::Empty, false))
                return false;

        if (target->IsAlive())
        {
            if (sWorld->getBoolConfig(CONFIG_DIE_COMMAND_MODE))
                handler->GetSession()->GetPlayer()->Kill(target);
            else
                handler->GetSession()->GetPlayer()->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        return true;
    }

    static bool HandleReviveCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid))
            return false;

        if (target)
        {
            target->ResurrectPlayer(1.0f);
            target->SpawnCorpseBones();
            target->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans(nullptr);
            Player::OfflineResurrect(targetGuid, trans);
        }

        return true;
    }

    static bool HandleDismountCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // If player is not mounted, so go out :)
        if (!player->IsMounted())
        {
            handler->SendSysMessage(LANG_CHAR_NON_MOUNTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->Dismount();
        player->RemoveAurasByType(SPELL_AURA_MOUNTED);
        return true;
    }

    static bool HandleGUIDCommand(ChatHandler* handler, char const* /*args*/)
    {
        ObjectGuid guid = handler->GetSession()->GetPlayer()->GetTarget();

        if (guid == 0)
        {
            handler->SendSysMessage(LANG_NO_SELECTION);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_OBJECT_GUID, guid.GetCounter(), guid.GetHigh());
        return true;
    }

    static bool HandleHelpCommand(ChatHandler* handler, char const* args)
    {
        char const* cmd = strtok((char*)args, " ");
        if (!cmd)
        {
            handler->ShowHelpForCommand(handler->getCommandTable(), "help");
            handler->ShowHelpForCommand(handler->getCommandTable(), "");
        }
        else
        {
            if (!handler->ShowHelpForCommand(handler->getCommandTable(), cmd))
                handler->SendSysMessage(LANG_NO_HELP_CMD);
        }

        return true;
    }
    // move item to other slot
    static bool HandleItemMoveCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* param1 = strtok((char*)args, " ");
        if (!param1)
            return false;

        char const* param2 = strtok(NULL, " ");
        if (!param2)
            return false;

        uint8 srcSlot = uint8(atoi(param1));
        uint8 dstSlot = uint8(atoi(param2));

        if (srcSlot == dstSlot)
            return true;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, srcSlot, true))
            return false;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, dstSlot, false))
            return false;

        uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcSlot);
        uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstSlot);

        handler->GetSession()->GetPlayer()->SwapItem(src, dst);

        return true;
    }

    static bool HandleCooldownCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string nameLink = handler->GetNameLink(target);

        if (!*args)
        {
            target->RemoveAllSpellCooldown();
            if (Pet* pet = target->GetPet())
                pet->GetSpellHistory()->RemoveAllCooldowns();
            handler->PSendSysMessage(LANG_REMOVEALL_COOLDOWN, nameLink.c_str());
        }
        else
        {
            // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
            uint32 spellIid = handler->extractSpellIdFromLink((char*)args);
            if (!spellIid)
                return false;

            if (!sSpellMgr->GetSpellInfo(spellIid))
            {
                handler->PSendSysMessage(LANG_UNKNOWN_SPELL, target == handler->GetSession()->GetPlayer() ? handler->GetTrinityString(LANG_YOU) : nameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->RemoveSpellCooldown(spellIid, true);
            handler->PSendSysMessage(LANG_REMOVE_COOLDOWN, spellIid, target == handler->GetSession()->GetPlayer() ? handler->GetTrinityString(LANG_YOU) : nameLink.c_str());
        }
        return true;
    }

    static bool HandleGetDistanceCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* obj = NULL;

        if (*args)
        {
            ObjectGuid guid = handler->extractGuidFromLink((char*)args);
            if (guid)
                obj = (WorldObject*)ObjectAccessor::GetObjectByTypeMask(*handler->GetSession()->GetPlayer(), guid, TYPEMASK_UNIT|TYPEMASK_GAMEOBJECT);

            if (!obj)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            obj = handler->getSelectedUnit();

            if (!obj)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        handler->PSendSysMessage(LANG_DISTANCE, handler->GetSession()->GetPlayer()->GetDistance(obj), handler->GetSession()->GetPlayer()->GetDistance2d(obj), handler->GetSession()->GetPlayer()->GetExactDist(obj), handler->GetSession()->GetPlayer()->GetExactDist2d(obj));
        return true;
    }
    // Teleport player to last position
    static bool HandleRecallCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        if (target->IsBeingTeleported())
        {
            handler->PSendSysMessage(LANG_IS_TELEPORTED, handler->GetNameLink(target).c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (target->IsInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->CleanupAfterTaxiFlight();
        }

        target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
        return true;
    }

    static bool HandleSaveCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // save GM account without delay and output message
        if (handler->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
        {
            if (Player* target = handler->getSelectedPlayer())
                target->SaveToDB();
            else
                player->SaveToDB();
            handler->SendSysMessage(LANG_PLAYER_SAVED);
            return true;
        }

        // save if the player has last been saved over 20 seconds ago
        uint32 saveInterval = sWorld->getIntConfig(CONFIG_INTERVAL_SAVE);
        if (saveInterval == 0 || (saveInterval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= saveInterval - 20 * IN_MILLISECONDS))
            player->SaveToDB();

        return true;
    }

    // Save all players in the world
    static bool HandleSaveAllCommand(ChatHandler* handler, char const* /*args*/)
    {
        ObjectAccessor::SaveAllPlayers();
        handler->SendSysMessage(LANG_PLAYERS_SAVED);
        return true;
    }

    // kick player
    static bool HandleKickPlayerCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, NULL, &playerName))
            return false;

        if (handler->GetSession() && target == handler->GetSession()->GetPlayer())
        {
            handler->SendSysMessage(LANG_COMMAND_KICKSELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        if (sWorld->getBoolConfig(CONFIG_SHOW_KICK_IN_WORLD))
        {
            int32 sec = sWorld->getIntConfig(CONFIG_SHOW_INFO_KICK_PLAYER);
            if (sec > -1)
            {
                char announce[1024];

                std::string adminName = handler->GetSession() ? handler->GetSession()->GetPlayerName() : handler->GetTrinityString(LANG_CONSOLE_COMMAND);
                snprintf(announce, 1024, handler->GetTrinityString(LANG_ANNOUNCE_CHARACTER_KICKED), target->GetName().c_str(), adminName.c_str());
                handler->SecureLevelAnnounce(AccountTypes(sec), announce);
            }
        }
        else
            handler->PSendSysMessage(LANG_COMMAND_KICKMESSAGE, playerName.c_str());

        target->GetSession()->KickPlayer();

        return true;
    }

    static bool HandleUnstuckCommand(ChatHandler* handler, char const* args)
    {
        // No args required for players
        if (handler->GetSession())
        {
            // 7355: "Stuck"
            if (Player* player = handler->GetSession()->GetPlayer())
                player->CastSpell(player, 7355, false);
            return true;
        }

        if (!*args)
            return false;

        char* player_str = strtok((char*)args, " ");
        if (!player_str)
            return false;

        std::string location_str = "inn";
        if (char const* loc = strtok(NULL, " "))
            location_str = loc;

        Player* player = NULL;
        if (!handler->extractPlayerTarget(player_str, &player))
            return false;

        if (player->IsInFlight() || player->IsInCombat())
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(7355);
            if (!spellInfo)
                return false;

            if (Player* caster = handler->GetSession()->GetPlayer())
                Spell::SendCastResult(caster, spellInfo, 0, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW);

            return false;
        }

        if (location_str == "inn")
        {
            player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
            return true;
        }

        if (location_str == "graveyard")
        {
            player->RepopAtGraveyard();
            return true;
        }

        if (location_str == "startzone")
        {
            player->TeleportTo(player->GetStartPosition());
            return true;
        }

        //Not a supported argument
        return false;

    }

    static bool HandleLinkGraveCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        if (!px)
            return false;

        uint32 graveyardId = uint32(atoi(px));

        uint32 team;

        char* px2 = strtok(NULL, " ");

        if (!px2)
            team = 0;
        else if (strncmp(px2, "horde", 6) == 0)
            team = HORDE;
        else if (strncmp(px2, "alliance", 9) == 0)
            team = ALLIANCE;
        else
            return false;

        WorldSafeLocsEntry const* graveyard = sWorldSafeLocsStore.LookupEntry(graveyardId);

        if (!graveyard)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, graveyardId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        uint32 zoneId = player->GetZoneId();

        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(zoneId);
        if (!areaEntry || areaEntry->ParentAreaID !=0)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDWRONGZONE, graveyardId, zoneId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sObjectMgr->AddGraveYardLink(graveyardId, zoneId, team))
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDLINKED, graveyardId, zoneId);
        else
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDALRLINKED, graveyardId, zoneId);

        return true;
    }

    static bool HandleNearGraveCommand(ChatHandler* handler, char const* args)
    {
        uint32 team;

        size_t argStr = strlen(args);

        if (!*args)
            team = 0;
        else if (strncmp((char*)args, "horde", argStr) == 0)
            team = HORDE;
        else if (strncmp((char*)args, "alliance", argStr) == 0)
            team = ALLIANCE;
        else
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        uint32 zone_id = player->GetZoneId();

        WorldSafeLocsEntry const* graveyard = sObjectMgr->GetClosestGraveYard(*player, team, nullptr);
        if (graveyard)
        {
            uint32 graveyardId = graveyard->ID;

            GraveYardData const* data = sObjectMgr->FindGraveYardData(graveyardId, zone_id);
            if (!data)
            {
                handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDERROR, graveyardId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            team = data->team;

            std::string team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_NOTEAM);

            if (team == 0)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ANY);
            else if (team == HORDE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (team == ALLIANCE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNEAREST, graveyardId, team_name.c_str(), zone_id);
        }
        else
        {
            std::string team_name;

            if (team == HORDE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (team == ALLIANCE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            if (!team)
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAVEYARDS, zone_id);
            else
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAFACTION, zone_id, team_name.c_str());
        }

        return true;
    }

    static bool HandleNearAreaTriggerCommand(ChatHandler* handler, const char* args)
    {
        float distance = (!*args) ? 10.0f : float((atof(args)));
        std::list<AreaTriggerEntry const*> areatriggers;

        Position pos = handler->GetSession()->GetPlayer()->GetPosition();
        WorldLocation from(handler->GetSession()->GetPlayer()->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());

        uint32 numRows = sAreaTriggerStore.GetNumRows();
        for (uint32 id = 0; id < numRows; ++id)
        {
            AreaTriggerEntry const* at = sAreaTriggerStore.LookupEntry(id);
            if (!at || at->ContinentID != from.GetMapId())
                continue;

            Position pos = { at->Pos.X, at->Pos.Y, at->Pos.Z };
            if (from.GetExactDistSq(&pos) > distance * distance)
                continue;

            areatriggers.push_back(at);
        }

        areatriggers.sort([&from](AreaTriggerEntry const* a, AreaTriggerEntry const* b)
        {
            Position posA = { a->Pos.X, a->Pos.Y, a->Pos.Z };
            Position posB = { b->Pos.X, b->Pos.Y, b->Pos.Z };
            return from.GetExactDist2dSq(&posA) < from.GetExactDist2dSq(&posB);
        });

        for (auto at : areatriggers)
            handler->PSendSysMessage("|cFFFFFFFFTrigger %u, x: %.5f, y: %.5f, z: %.5f, radius: %g, box: { %g, %g, %g, %g }", at->ID, at->Pos.X, at->Pos.Y, at->Pos.Z, at->Radius, at->BoxLength, at->BoxWidth, at->BoxHeight, at->BoxYaw);

        handler->PSendSysMessage("Found near areatriggers (distance %f): %lu", distance, areatriggers.size());

        return true;
    }

    static bool HandleShowAreaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        AreaTableEntry const* area = sAreaTableStore.LookupEntry(atoi(args));
        if (!area)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (area->exploreFlag < 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 offset = area->exploreFlag / 32;
        if (offset >= PLAYER_EXPLORED_ZONES_SIZE)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 val = uint32((1 << (area->exploreFlag % 32)));
        uint32 currFields = playerTarget->GetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + offset);
        playerTarget->SetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + offset, uint32((currFields | val)));

        handler->SendSysMessage(LANG_EXPLORE_AREA);
        return true;
    }

    static bool HandleHideAreaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }


        AreaTableEntry const* area = sAreaTableStore.LookupEntry(atoi(args));
        if (!area)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 offset = area->exploreFlag / 32;
        if (offset >= PLAYER_EXPLORED_ZONES_SIZE)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 val = uint32((1 << (area->exploreFlag % 32)));
        uint32 currFields = playerTarget->GetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + offset);
        playerTarget->SetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + offset, uint32((currFields ^ val)));

        handler->SendSysMessage(LANG_UNEXPLORE_AREA);
        return true;
    }

    static bool HandleAddItemCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 itemId = 0;

        if (args[0] == '[')                                        // [name] manual form
        {
            char const* itemNameStr = strtok((char*)args, "]");

            if (itemNameStr && itemNameStr[0])
            {
                std::string itemName = itemNameStr+1;
                WorldDatabase.EscapeString(itemName);

                WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_ITEM_TEMPLATE_BY_NAME);
                stmt->setString(0, itemName);
                PreparedQueryResult result = WorldDatabase.Query(stmt);

                if (!result)
                {
                    handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, itemNameStr+1);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                itemId = result->Fetch()->GetUInt32();
            }
            else
                return false;
        }
        else                                                    // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
        {
            char const* id = handler->extractKeyFromLink((char*)args, "Hitem");
            if (!id)
                return false;
            itemId = uint32(atol(id));
        }

        char const* ccount = strtok(NULL, " ");

        int32 count = 1;

        if (ccount)
            count = strtol(ccount, NULL, 10);

        if (count == 0)
            count = 1;

        Player* player = handler->GetSession()->GetPlayer();
        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
            playerTarget = player;

        TC_LOG_DEBUG("misc", handler->GetTrinityString(LANG_ADDITEM), itemId, count);

        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemId);
        if (!itemTemplate)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Subtract
        if (count < 0)
        {
            playerTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, handler->GetNameLink(playerTarget).c_str());
            return true;
        }

        // Adding items
        uint32 noSpaceForCount = 0;

        // check space and find places
        ItemPosCountVec dest;
        InventoryResult msg = playerTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
            count -= noSpaceForCount;

        if (count == 0 || dest.empty())                         // can't add any
        {
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Item* item = playerTarget->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

        // remove binding (let GM give it to another player later)
        if (player == playerTarget)
            for (ItemPosCountVec::const_iterator itr = dest.begin(); itr != dest.end(); ++itr)
                if (Item* item1 = player->GetItemByPos(itr->pos))
                    item1->SetBinding(false);

        if (count > 0 && item)
        {
            player->SendNewItem(item, count, false, true);
            if (player != playerTarget)
                playerTarget->SendNewItem(item, count, true, false);
            sScriptMgr->OnItemPickup(playerTarget, item, ItemPickupSourceType::Command, player ? player->GetGUID().GetCounter() : 0);
        }

        if (noSpaceForCount > 0)
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);

        return true;
    }

    static bool HandleAddItemSetCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* id = handler->extractKeyFromLink((char*)args, "Hitemset"); // number or [name] Shift-click form |color|Hitemset:itemset_id|h[name]|h|r
        if (!id)
            return false;

        uint32 itemSetId = atol(id);

        // prevent generation all items with itemset field value '0'
        if (itemSetId == 0)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemSetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
            playerTarget = player;

        TC_LOG_DEBUG("misc", handler->GetTrinityString(LANG_ADDITEMSET), itemSetId);

        bool found = false;
        ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
        for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
        {
            if (itr->second.ItemSet == itemSetId)
            {
                found = true;
                ItemPosCountVec dest;
                InventoryResult msg = playerTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itr->second.ItemId, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = playerTarget->StoreNewItem(dest, itr->second.ItemId, true);

                    // remove binding (let GM give it to another player later)
                    if (player == playerTarget)
                        item->SetBinding(false);

                    player->SendNewItem(item, 1, false, true);
                    if (player != playerTarget)
                        playerTarget->SendNewItem(item, 1, true, false);

                    sScriptMgr->OnItemPickup(playerTarget, item, ItemPickupSourceType::Command, player ? player->GetGUID().GetCounter() : 0);
                }
                else
                {
                    player->SendEquipError(msg, NULL, NULL, itr->second.ItemId);
                    handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itr->second.ItemId, 1);
                }
            }
        }

        if (!found)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemSetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleBankCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->GetSession()->SendShowBank(handler->GetSession()->GetPlayer()->GetGUID());
        return true;
    }

    static bool HandleChangeWeather(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // Weather is OFF
        if (!sWorld->getBoolConfig(CONFIG_WEATHER))
        {
            handler->SendSysMessage(LANG_WEATHER_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // *Change the weather of a cell
        char const* px = strtok((char*)args, " ");
        char const* py = strtok(NULL, " ");

        if (!px || !py)
            return false;

        uint32 type = uint32(atoi(px));                         //0 to 3, 0: fine, 1: rain, 2: snow, 3: sand
        float grade = float(atof(py));                          //0 to 1, sending -1 is instand good weather

        Player* player = handler->GetSession()->GetPlayer();
        uint32 zoneid = player->GetZoneId();

        Weather* weather = player->GetMap()->GetOrGenerateZoneDefaultWeather(zoneid);
        if (!weather)
        {
            handler->SendSysMessage(LANG_NO_WEATHER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        weather->SetWeather(WeatherType(type), grade);

        return true;
    }


    static bool HandleMaxSkillCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* SelectedPlayer = handler->getSelectedPlayer();
        if (!SelectedPlayer)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // each skills that have max skill value dependent from level seted to current level max skill value
        SelectedPlayer->UpdateSkillsToMaxSkillsForLevel();
        return true;
    }

    static bool HandleSetSkillCommand(ChatHandler* handler, char const* args)
    {
        // number or [name] Shift-click form |color|Hskill:skill_id|h[name]|h|r
        char const* skillStr = handler->extractKeyFromLink((char*)args, "Hskill");
        if (!skillStr)
            return false;

        char const* levelStr = strtok(NULL, " ");
        if (!levelStr)
            return false;

        char const* maxPureSkill = strtok(NULL, " ");

        int32 skill = atoi(skillStr);
        if (skill <= 0)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 level = uint32(atol(levelStr));

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        SkillLineEntry const* skillLine = sSkillLineStore.LookupEntry(skill);
        if (!skillLine)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string tNameLink = handler->GetNameLink(target);

        if (!target->GetSkillValue(skill))
        {
            handler->PSendSysMessage(LANG_SET_SKILL_ERROR, tNameLink.c_str(), skill, skillLine->name[handler->GetSessionDbcLocale()]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint16 max = maxPureSkill ? atol (maxPureSkill) : target->GetPureMaxSkillValue(skill);

        if (level <= 0 || level > max || max <= 0)
            return false;

        target->SetSkill(skill, target->GetSkillStep(skill), level, max);
        handler->PSendSysMessage(LANG_SET_SKILL, skill, skillLine->name[handler->GetSessionDbcLocale()], tNameLink.c_str(), level, max);

        return true;
    }

    /**
    * @name Player command: .pinfo
    * @date 05/19/2013
    *
    * @brief Prints information about a character and it's linked account to the commander
    *
    * Non-applying information, e.g. a character that is not in gm mode right now or
    * that is not banned/muted, is not printed
    *
    * This can be done either by giving a name or by targeting someone, else, it'll use the commander
    *
    * @param args name   Prints information according to the given name to the commander
    *             target Prints information on the target to the commander
    *             none   No given args results in printing information on the commander
    *
    * @return Several pieces of information about the character and the account
    **/

    static bool HandlePInfoCommand(ChatHandler* handler, char const* args)
    {
        // Define ALL the player variables!
        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;

        // To make sure we get a target, we convert our guid to an omniversal...
        ObjectGuid parseGUID(HighGuid::Player, uint32(atol((char*)args)));

        // ... and make sure we get a target, somehow.
        if (sObjectMgr->GetPlayerNameByGUID(parseGUID, targetName))
        {
            target = sObjectMgr->GetPlayerByLowGUID(parseGUID);
            targetGuid = parseGUID;
        }
        // if not, then return false. Which shouldn't happen, now should it ?
        else if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        /* The variables we extract for the command. They are
         * default as "does not exist" to prevent problems
         * The output is printed in the follow manner:
         *
         * Player %s %s (guid: %u)                   - I.    LANG_PINFO_PLAYER
         * ** GM Mode active, Phase: -1              - II.   LANG_PINFO_GM_ACTIVE (if GM)
         * ** Banned: (Type, Reason, Time, By)       - III.  LANG_PINFO_BANNED (if banned)
         * ** Muted: (Time, Reason, By)              - IV.   LANG_PINFO_MUTED (if muted)
         * * Account: %s (id: %u), GM Level: %u      - V.    LANG_PINFO_ACC_ACCOUNT
         * * Last Login: %u (Failed Logins: %u)      - VI.   LANG_PINFO_ACC_LASTLOGIN
         * * Uses OS: %s - Latency: %u ms            - VII.  LANG_PINFO_ACC_OS
         * * Registration Email: %s, Bnet Acc: %s    - VIII. LANG_PINFO_ACC_REGMAILS
         * * Last IP: %u (Locked: %s)                - IX.   LANG_PINFO_ACC_IP
         * * Level: %u (%u/%u XP (%u XP left)        - X.    LANG_PINFO_CHR_LEVEL
         * * Race: %s %s, Class %s                   - XI.   LANG_PINFO_CHR_RACE
         * * Alive ?: %s                             - XII.  LANG_PINFO_CHR_ALIVE
         * * Phase: %s                               - XIII. LANG_PINFO_CHR_PHASE (if not GM)
         * * Money: %ug%us%uc                        - XIV.  LANG_PINFO_CHR_MONEY
         * * Map: %s, Area: %s                       - XV.   LANG_PINFO_CHR_MAP
         * * Guild: %s (Id: %u)                      - XVI.  LANG_PINFO_CHR_GUILD (if in guild)
         * ** Rank: %s                               - XVII. LANG_PINFO_CHR_GUILD_RANK (if in guild)
         * ** Note: %s                               - XVIII.LANG_PINFO_CHR_GUILD_NOTE (if in guild and has note)
         * ** O. Note: %s                            - XVIX. LANG_PINFO_CHR_GUILD_ONOTE (if in guild and has officer note)
         * * Played time: %s                         - XX.   LANG_PINFO_CHR_PLAYEDTIME
         * * Mails: %u Read/%u Total                 - XXI.  LANG_PINFO_CHR_MAILS (if has mails)
         * * HWID: %s                                - XXII. LANG_PINFO_CHAR_HWID
         *
         * Not all of them can be moved to the top. These should
         * place the most important ones to the head, though.
         *
         * For a cleaner overview, I segment each output in Roman numerals
         */

        // Account data print variables
        std::string userName          = handler->GetTrinityString(LANG_ERROR);
        uint32 accId                  = 0;
        uint32 lowguid                = targetGuid.GetCounter();
        std::string eMail             = handler->GetTrinityString(LANG_ERROR);
        std::string bnetAcc           = handler->GetTrinityString(LANG_ERROR);
        uint32 security               = 0;
        std::string lastIp            = handler->GetTrinityString(LANG_ERROR);
        uint8 locked                  = 0;
        std::string lastLogin         = handler->GetTrinityString(LANG_ERROR);
        uint32 failedLogins           = 0;
        uint32 latency                = 0;
        std::string OS                = "None";
        std::string HWID              = "unknown";

        // Mute data print variables
        int64 muteTime                = -1;
        std::string muteReason        = "unknown";
        std::string muteBy            = "unknown";
        uint64 onlineMuteTimer        = 0;
        uint32 activeMuteId           = 0;

        // Ban data print variables
        int64 banTime                 = -1;
        std::string banType           = "None";
        std::string banReason         = "Unknown";
        std::string bannedBy          = "Unknown";

        // Character data print variables
        uint8 raceid, classid           = 0; //RACE_NONE, CLASS_NONE
        std::string raceStr, classStr   = "None";
        uint8 gender                    = 0;
        int8 locale                     = handler->GetSessionDbcLocale();
        std::string genderStr           = handler->GetTrinityString(LANG_ERROR);
        uint32 totalPlayerTime          = 0;
        uint8 level                     = 0;
        std::string alive               = handler->GetTrinityString(LANG_ERROR);
        uint64 money                    = 0;
        uint32 xp                       = 0;
        uint32 xptotal                  = 0;

        // Position data print
        uint32 mapId;
        uint32 areaId;
        uint32 phase            = 0;
        std::string areaName    = "<unknown>";
        std::string zoneName    = "<unknown>";

        // Guild data print variables defined so that they exist, but are not necessarily used
        uint32 guildId           = 0;
        uint8 guildRankId        = 0;
        std::string guildName;
        std::string guildRank;
        std::string note;
        std::string officeNote;

        // Mail data print is only defined if you have a mail

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
                return false;

            accId             = target->GetSession()->GetAccountId();
            money             = target->GetMoney();
            totalPlayerTime   = target->GetTotalPlayedTime();
            level             = target->GetLevel();
            latency           = target->GetSession()->GetLatency();
            raceid            = target->GetRace();
            classid           = target->GetClass();
            mapId             = target->GetMapId();
            areaId            = target->GetAreaId();
            alive             = target->IsAlive() ? "Yes" : "No";
            gender            = target->GetGender();
            phase             = target->GetPhaseMask();
        }
        // get additional information from DB
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            // Query informations from the DB
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PINFO);
            stmt->setUInt32(0, lowguid);
            PreparedQueryResult result = CharacterDatabase.Query(stmt);

            if (!result)
                return false;

            Field* fields      = result->Fetch();
            totalPlayerTime    = fields[0].GetUInt32();
            level              = fields[1].GetUInt8();
            money              = fields[2].GetUInt64();
            accId              = fields[3].GetUInt32();
            raceid             = fields[4].GetUInt8();
            classid            = fields[5].GetUInt8();
            mapId              = fields[6].GetUInt16();
            areaId             = fields[7].GetUInt16();
            gender             = fields[8].GetUInt8();
            uint32 health      = fields[9].GetUInt32();
            uint32 playerFlags = fields[10].GetUInt32();

            if (!health || playerFlags & PLAYER_FLAGS_GHOST)
                alive = "No";
            else
                alive = "Yes";
        }

        // Query the prepared statement for login data
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PINFO);
        stmt->setInt32(0, int32(realm.Id.Realm));
        stmt->setUInt32(1, accId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
        {
            Field* fields = result->Fetch();
            userName      = fields[0].GetString();
            security      = fields[1].GetUInt8();

            eMail         = fields[2].GetString();
            lastIp        = fields[3].GetString();
            lastLogin     = fields[4].GetString();

            if (IpLocationRecord const* location = sIPLocation->GetLocationRecord(lastIp))
            {
                lastIp.append(" (");
                lastIp.append(location->CountryName);
                lastIp.append(")");
            }

            muteTime      = fields[5].GetUInt64();
            failedLogins  = fields[6].GetUInt32();
            locked        = fields[7].GetUInt8();
            OS            = fields[8].GetString();
            HWID          = fields[9].GetString();
            bnetAcc       = fields[10].GetString();

            onlineMuteTimer = fields[11].GetUInt64() / 1000;
            activeMuteId    = fields[12].GetUInt32();
        }

        // Creates a chat link to the character. Returns nameLink
        std::string nameLink = handler->playerLink(targetName);

        // Returns banType, banTime, bannedBy, banreason
        LoginDatabasePreparedStatement* stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PINFO_BANS);
        CharacterDatabasePreparedStatement* cstmt = nullptr;
        stmt2->setUInt32(0, accId);
        PreparedQueryResult result2 = LoginDatabase.Query(stmt2);
        if (!result2)
        {
            banType = "Character";
            cstmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_BANS);
            cstmt->setUInt32(0, lowguid);
            result2 = CharacterDatabase.Query(cstmt);
        }

        if (result2)
        {
            Field* fields = result2->Fetch();
            banTime       = int64(fields[1].GetUInt64() ? 0 : fields[0].GetUInt32());
            bannedBy      = fields[2].GetString();
            banReason     = fields[3].GetString();
        }

        // Can be used to query data from World database
        WorldDatabasePreparedStatement* stmt3 = WorldDatabase.GetPreparedStatement(WORLD_SEL_REQ_XP);
        stmt3->setUInt8(0, level);
        PreparedQueryResult result3 = WorldDatabase.Query(stmt3);

        if (result3)
        {
            Field* fields = result3->Fetch();
            xptotal       = fields[0].GetUInt32();
        }

        // Can be used to query data from Characters database
        cstmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_XP);
        cstmt->setUInt32(0, lowguid);
        PreparedQueryResult result4 = CharacterDatabase.Query(cstmt);

        if (result4)
        {
            Field* fields = result4->Fetch();
            xp            = fields[0].GetUInt32(); // Used for "current xp" output and "%u XP Left" calculation
            uint32 gguid  = fields[1].GetUInt32(); // We check if have a guild for the person, so we might not require to query it at all

            if (gguid != 0)
            {
                // Guild Data - an own query, because it may not happen.
                CharacterDatabasePreparedStatement* stmt4 = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER_EXTENDED);
                stmt4->setUInt32(0, lowguid);
                PreparedQueryResult result5 = CharacterDatabase.Query(stmt4);
                if (result5)
                {
                    Field* fields  = result5->Fetch();
                    guildId        = fields[0].GetUInt32();
                    guildName      = fields[1].GetString();
                    guildRank      = fields[2].GetString();
                    guildRankId    = fields[3].GetUInt8();
                    note           = fields[4].GetString();
                    officeNote     = fields[5].GetString();
                }
            }
        }

        // Initiate output
        // Output I. LANG_PINFO_PLAYER
        handler->PSendSysMessage(LANG_PINFO_PLAYER, target ? "" : handler->GetTrinityString(LANG_OFFLINE), nameLink.c_str(), lowguid);

        // Output II. LANG_PINFO_GM_ACTIVE if character is gamemaster
        if (target && target->IsGameMaster())
            handler->PSendSysMessage(LANG_PINFO_GM_ACTIVE);

        // Output III. LANG_PINFO_BANNED if ban exists and is applied
        if (banTime >= 0)
            handler->PSendSysMessage(LANG_PINFO_BANNED, banType.c_str(), banReason.c_str(), banTime > 0 ? secsToTimeString(banTime - time(NULL), true).c_str() : "permanently", bannedBy.c_str());

        // Output IV. LANG_PINFO_MUTED if mute is applied
        if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
        {
            if (onlineMuteTimer)
            {
                QueryResult qresult = LoginDatabase.PQuery("SELECT muted_by, mute_reason FROM account_muted WHERE id = '%u' AND acc_id = '%u' AND realmid = '%u'", activeMuteId, accId, realm.Id.Realm);
                if (qresult)
                {
                    Field* fields = qresult->Fetch();
                    std::string mutedBy = fields[0].GetString();
                    std::string muteReason = fields[1].GetString();

                    handler->PSendSysMessage(LANG_PINFO_MUTE_FULL, secsToTimeString(onlineMuteTimer, true).c_str(), mutedBy.c_str(), muteReason.c_str());
                }
                else
                    handler->PSendSysMessage(LANG_PINFO_MUTED, secsToTimeString(onlineMuteTimer, true).c_str());
            }
        }
        else if (muteTime > 0)
        {
            time_t currTime = time(NULL);
            if (muteTime > currTime)
                handler->PSendSysMessage(LANG_PINFO_MUTED, secsToTimeString(muteTime - currTime, true).c_str());
        }

        // Output V. LANG_PINFO_ACC_ACCOUNT
        handler->PSendSysMessage(LANG_PINFO_ACC_ACCOUNT, userName.c_str(), accId, security);

        // Output VI. LANG_PINFO_ACC_LASTLOGIN
        handler->PSendSysMessage(LANG_PINFO_ACC_LASTLOGIN, lastLogin.c_str(), failedLogins);

        // Output VII. LANG_PINFO_ACC_OS
        handler->PSendSysMessage(LANG_PINFO_ACC_OS, OS.c_str(), latency);

        // Output VIII. LANG_PINFO_ACC_REGMAILS
        //handler->PSendSysMessage(LANG_PINFO_ACC_REGMAILS, eMail.c_str(), bnetAcc.c_str());

        // Output IX. LANG_PINFO_ACC_IP
        handler->PSendSysMessage(LANG_PINFO_ACC_IP, lastIp.c_str(), locked ? "Yes" : "No");

        // Output X. LANG_PINFO_CHR_LEVEL
        if (level != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            handler->PSendSysMessage(LANG_PINFO_CHR_LEVEL_LOW, level, xp, xptotal, (xptotal - xp));
        else
            handler->PSendSysMessage(LANG_PINFO_CHR_LEVEL_HIGH, level);

        // Output XI. LANG_PINFO_CHR_RACE
        raceStr  = GetRaceName(raceid, locale);
        classStr = GetClassName(classid, locale);
        handler->PSendSysMessage(LANG_PINFO_CHR_RACE, (gender == 0 ? handler->GetTrinityString(LANG_CHARACTER_GENDER_MALE) : handler->GetTrinityString(LANG_CHARACTER_GENDER_FEMALE)), raceStr.c_str(), classStr.c_str());

        // Output XII. LANG_PINFO_CHR_ALIVE
        handler->PSendSysMessage(LANG_PINFO_CHR_ALIVE, alive.c_str());

        // Output XIII. LANG_PINFO_CHR_PHASE if player is not in GM mode (GM is in every phase)
        if (target && !target->IsGameMaster())                            // IsInWorld() returns false on loadingscreen, so it's more
            handler->PSendSysMessage(LANG_PINFO_CHR_PHASE, phase);        // precise than just target (safer ?).
                                                                          // However, as we usually just require a target here, we use target instead.
        // Output XIV. LANG_PINFO_CHR_MONEY
        uint32 gold                   = money / GOLD;
        uint32 silv                   = (money % GOLD) / SILVER;
        uint32 copp                   = (money % GOLD) % SILVER;
        handler->PSendSysMessage(LANG_PINFO_CHR_MONEY, gold, silv, copp);

        // Position data
        MapEntry const* map = sMapStore.LookupEntry(mapId);
        AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaId);
        if (area)
        {
            areaName = area->area_name[handler->GetSessionDbcLocale()];

            AreaTableEntry const* zone = sAreaTableStore.LookupEntry(area->ParentAreaID);
            if (zone)
                zoneName = zone->area_name[handler->GetSessionDbcLocale()];
        }

        if (target)
            handler->PSendSysMessage(LANG_PINFO_CHR_MAP, map ? map->name[handler->GetSessionDbcLocale()] : "<Unknown>", (!zoneName.empty() ? zoneName.c_str() : "<Unknown>"), (!areaName.empty() ? areaName.c_str() : "<Unknown>"));

        // Output XVII. - XVIX. if they are not empty
        if (!guildName.empty())
        {
            handler->PSendSysMessage(LANG_PINFO_CHR_GUILD, guildName.c_str(), guildId);
            handler->PSendSysMessage(LANG_PINFO_CHR_GUILD_RANK, guildRank.c_str(), uint32(guildRankId));
            if (!note.empty())
                handler->PSendSysMessage(LANG_PINFO_CHR_GUILD_NOTE, note.c_str());
            if (!officeNote.empty())
                handler->PSendSysMessage(LANG_PINFO_CHR_GUILD_ONOTE, officeNote.c_str());
        }

        // Output XX. LANG_PINFO_CHR_PLAYEDTIME
        handler->PSendSysMessage(LANG_PINFO_CHR_PLAYEDTIME, (secsToTimeString(totalPlayerTime, true, true)).c_str());

        // Mail Data - an own query, because it may or may not be useful.
        // SQL: "SELECT SUM(CASE WHEN (checked & 1) THEN 1 ELSE 0 END) AS 'readmail', COUNT(*) AS 'totalmail' FROM mail WHERE `receiver` = ?"
        CharacterDatabasePreparedStatement* stmt5 = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_MAILS);
        stmt5->setUInt32(0, lowguid);
        PreparedQueryResult result6 = CharacterDatabase.Query(stmt5);
        if (result6)
        {
            // Define the variables, so the compiler knows they exist
            uint32 rmailint = 0;

            // Fetch the fields - readmail is a SUM(x) and given out as char! Thus...
            // ... while totalmail is a COUNT(x), which is given out as INt64, which we just convert on fetch...
            Field* fields         = result6->Fetch();
            std::string readmail  = fields[0].GetString();
            uint32 totalmail      = uint32(fields[1].GetUInt64());

            // ... we have to convert it from Char to int. We can use totalmail as it is
            rmailint = atol(readmail.c_str());

            // Output XXI. LANG_INFO_CHR_MAILS if at least one mail is given
            if (totalmail >= 1)
               handler->PSendSysMessage(LANG_PINFO_CHR_MAILS, rmailint, totalmail);
        }

        return true;
    }

    static bool HandleRespawnCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // accept only explicitly selected target (not implicitly self targeting case)
        Unit* target = handler->getSelectedUnit();
        if (player->GetTarget() && target)
        {
            if (target->GetTypeId() != TYPEID_UNIT || target->IsPet())
            {
                handler->SendSysMessage(LANG_SELECT_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->isDead())
                target->ToCreature()->Respawn();
            return true;
        }

        CellCoord p(Trinity::ComputeCellCoord(player->GetPositionX(), player->GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();

        Trinity::RespawnDo u_do;
        Trinity::WorldObjectWorker<Trinity::RespawnDo> worker(player, u_do);

        TypeContainerVisitor<Trinity::WorldObjectWorker<Trinity::RespawnDo>, GridTypeMapContainer > obj_worker(worker);
        cell.Visit(p, obj_worker, *player->GetMap(), *player, player->GetGridActivationRange());

        return true;
    }

    static bool HandleMuteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        bool publicChannelsOnly = false;
        if (!strncmp(args, "public ", 7))
        {
            publicChannelsOnly = true;
            args += 7;
        }

        char* cname = NULL;
        char* cmuteTimeStr = NULL;
        char* cmuteReason = NULL;
        handler->extractOptFirstArg((char*)args, &cname, &cmuteTimeStr, &cmuteReason);
        if (!cmuteTimeStr || !cmuteReason)
            return false;

        std::string name;
        std::string muteTimeStr = cmuteTimeStr;
        std::string muteReason = cmuteReason;

        Player* target;
        ObjectGuid charId;
        WorldSession* session = nullptr;
        if (!handler->extractPlayerTarget(cname, &target, &charId, &name))
            return false;

        uint32 accId;
        if (target)
            session = target->GetSession();

        if (session)
            accId = session->GetAccountId();
        else
        {
            accId = sObjectMgr->GetPlayerAccountIdByGUID(charId);

            if (session = sWorld->FindSession(accId))
                target = session->GetPlayer();
        }

        // Don`t allow mute if sender security level <= target security level
        if (handler->HasLowerSecurity(target, charId, true))
            return false;

        // Don`t allow mute if target already has one
        if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
        {
            uint64 remainingTime = 0;
            std::string alreadyMutedBy;
            std::string alreadyReason;
            bool alreadyPublicChannelsOnly;

            if (session)
            {
                remainingTime = session->GetMute().Timer / 1000;
                alreadyMutedBy = session->GetMute().Author;
                alreadyReason = session->GetMute().Reason;
                alreadyPublicChannelsOnly = session->GetMute().PublicChannelsOnly;
            }
            else
            {
                QueryResult result = LoginDatabase.PQuery("SELECT m.mute_timer, am.muted_by, am.mute_reason, am.public_channels_only "
                    "FROM mute_active AS m, account_muted AS am "
                    "WHERE m.realmid = '%u' AND m.account = '%u' AND m.mute_id = am.id", realm.Id.Realm, accId);
                if (result)
                {
                    Field* fields = result->Fetch();
                    remainingTime = fields[0].GetUInt64() / 1000;
                    alreadyMutedBy = fields[1].GetString();
                    alreadyReason = fields[2].GetString();
                    alreadyPublicChannelsOnly = (bool)fields[3].GetUInt8();
                }
            }

            if (remainingTime)
            {
                handler->PSendSysMessage(alreadyPublicChannelsOnly ? LANG_CHAT_ALREADY_DISABLED_PUBLIC : LANG_CHAT_ALREADY_DISABLED, handler->playerLink(name).c_str(), alreadyMutedBy.c_str(), alreadyReason.c_str());
                handler->PSendSysMessage(LANG_PINFO_MUTED, secsToTimeString(remainingTime).c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        std::string mutedBy = handler->GetSession()->GetPlayer()->GetName();

        // Mute time in minutes
        uint32 muteTime = strtoul(cmuteTimeStr, nullptr, 10);
        uint32 maxMuteTime = sWorld->getIntConfig(CONFIG_GM_MAX_MUTE_TIME);
        if (muteTime == 0)
            muteTime = 1;
        else if (muteTime > maxMuteTime)
            muteTime = maxMuteTime;

        LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();

        // Write mute history
        uint32 muteId = sObjectMgr->GenerateMuteID();
        LoginDatabase.EscapeString(muteReason);
        trans->PAppend("INSERT INTO account_muted (realmid, id, acc_id, char_id, mute_acc, mute_name, mute_date, muted_by, mute_time, mute_reason, public_channels_only) "
            "SELECT %u, %u, id, %u, username, '%s', UNIX_TIMESTAMP(), '%s', %u, '%s', %u FROM account "
            "WHERE id = '%u'", realm.Id.Realm, muteId, charId.GetCounter(), name.c_str(), mutedBy.c_str(), muteTime, muteReason.c_str(), publicChannelsOnly, accId);

        if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
        {
            // Mute time in ms
            uint32 onlineMuteTimer = muteTime * 60 * 1000;

            if (session)
                session->SetMute({ onlineMuteTimer, mutedBy, muteReason, publicChannelsOnly });

            trans->PAppend("INSERT INTO mute_active (realmid, account, mute_id, mute_timer) VALUES ('%u', '%u', '%u', '%u')",
                realm.Id.Realm, accId, muteId, onlineMuteTimer);
        }
        else
        {
            // Mute time as timestamp
            time_t mutetime = time(NULL) + muteTime * 60;

            // Old "mutetime" mutes cannot store additional mute info
            if (session)
                session->m_muteTime = mutetime;

            trans->PAppend("UPDATE account SET mutetime = " UI64FMTD " WHERE id = '%u'", uint64(mutetime), accId);
        }

        LoginDatabase.CommitTransaction(trans);

        // Send messages
        if (session)
            ChatHandler(session).PSendSysMessage(publicChannelsOnly ? LANG_YOUR_CHAT_DISABLED_PUBLIC : LANG_YOUR_CHAT_DISABLED, mutedBy.c_str(), muteTime, muteReason.c_str());

        handler->PSendSysMessage(publicChannelsOnly ? LANG_YOU_DISABLE_CHAT_PUBLIC : LANG_YOU_DISABLE_CHAT, handler->playerLink(name).c_str(), muteTime, muteReason.c_str());

        int32 sec = sWorld->getIntConfig(CONFIG_SHOW_INFO_MUTE_PLAYER);
        if (sec > -1)
        {
            char announce[1024];

            snprintf(announce, 1024, handler->GetTrinityString(publicChannelsOnly ? LANG_ANNOUNCE_CHARACTER_MUTED_PUBLIC : LANG_ANNOUNCE_CHARACTER_MUTED), handler->playerLink(name).c_str(), mutedBy.c_str(), muteTime, muteReason.c_str());
            handler->SecureLevelAnnounce(AccountTypes(sec), announce);
        }

        return true;
    }

    // unmute player
    static bool HandleUnmuteCommand(ChatHandler* handler, char const* args)
    {
        std::string name;
        Player* target;
        ObjectGuid charId;
        WorldSession* session = nullptr;
        if (!handler->extractPlayerTarget((char*)args, &target, &charId, &name))
            return false;

        uint32 accId;
        if (target)
            session = target->GetSession();

        if (session)
            accId = session->GetAccountId();
        else
        {
            accId = sObjectMgr->GetPlayerAccountIdByGUID(charId);

            if (session = sWorld->FindSession(accId))
                target = session->GetPlayer();
        }

        // Don`t allow unmute if sender security level <= target security level
        if (handler->HasLowerSecurity(target, charId, true))
            return false;

        std::string unmutedBy = handler->GetSession()->GetPlayer()->GetName();

        if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
        {
            if (session)
            {
                if (!session->GetMute().Timer)
                {
                    handler->PSendSysMessage(LANG_CHAT_ALREADY_ENABLED, name.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                session->GetMute().Timer = 0;
            }

            LoginDatabase.PExecute("DELETE FROM mute_active WHERE realmid = '%u' AND account = '%u'", realm.Id.Realm, accId);
        }
        else
        {
            if (session)
            {
                if (session->m_muteTime <= time(nullptr))
                {
                    handler->PSendSysMessage(LANG_CHAT_ALREADY_ENABLED, name.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                session->m_muteTime = 0;
            }

            LoginDatabase.PExecute("UPDATE account SET mutetime = '0' WHERE id = '%u'", accId);
        }

        // Send messages
        if (session)
            ChatHandler(session).PSendSysMessage(LANG_YOUR_CHAT_ENABLED, unmutedBy.c_str());

        handler->PSendSysMessage(LANG_YOU_ENABLE_CHAT, handler->playerLink(name).c_str());

        int32 sec = sWorld->getIntConfig(CONFIG_SHOW_INFO_MUTE_PLAYER);
        if (sec > -1)
        {
            char announce[1024];

            snprintf(announce, 1024, handler->GetTrinityString(LANG_ANNOUNCE_CHARACTER_UNMUTED), handler->playerLink(name).c_str(), unmutedBy.c_str());
            handler->SecureLevelAnnounce(AccountTypes(sec), announce);
        }

        return true;
    }

    static bool HandleMuteListAccountCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cfullListStr;
        char* cacc;
        handler->extractOptFirstArg((char*)args, &cfullListStr, &cacc);

        if (!cacc)
            return false;

        uint8 limit = 10; // Limit to 10 last rows by default
        if (cfullListStr)
        {
            if (strncmp(cfullListStr, "full", strlen(cfullListStr)) != 0)
                return false;
            else
                limit = 0;
        }

        std::string acc = cacc;
        if (!AccountMgr::normalizeString(acc))
        {
            handler->PSendSysMessage(LANG_MUTELIST_ACC_NOT_FOUND, acc.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accId = sAccountMgr->GetId(acc);
        if (!accId)
        {
            handler->PSendSysMessage(LANG_MUTELIST_ACC_NOT_FOUND, acc.c_str());
            return true;
        }

        uint64 activeMuteTimer = 0;
        uint32 activeMuteId = 0;

        if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
        {
            // Get mute info
            QueryResult result = LoginDatabase.PQuery("SELECT mute_timer, mute_id FROM mute_active WHERE account = '%u' AND realmid = '%u'", accId, realm.Id.Realm);

            if (result)
            {
                if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
                {
                    activeMuteTimer = (*result)[0].GetUInt64() / 1000;
                    activeMuteId = (*result)[1].GetUInt32();
                }
            }
        }

        // Send account mute history
        MuteListSendAccHistoryHelper(handler, acc, limit, activeMuteId, activeMuteTimer, 0);

        handler->PSendSysMessage(LANG_MUTELIST_END_HISTORY);

        return true;
    }

    static bool HandleMuteListCharacterCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cfullListStr;
        char* cname;
        handler->extractOptFirstArg((char*)args, &cfullListStr, &cname);

        if (!cname)
            return false;

        uint8 limit = 10; // Limit to 10 last rows by default
        if (cfullListStr)
        {
            if (strncmp(cfullListStr, "full", strlen(cfullListStr)) != 0)
                return false;
            else
                limit = 0;
        }

        std::string name = cname;
        if (!normalizePlayerName(name))
        {
            handler->PSendSysMessage(LANG_MUTELIST_CHAR_NOT_FOUND, name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Get character and account id
        CharacterDatabase.EscapeString(name);
        QueryResult result = CharacterDatabase.PQuery("SELECT guid, account FROM characters WHERE name = '%s'", name.c_str());
    
        Field* fields;
        uint32 charId = 0;
        std::string acc;
        uint64 activeMuteTimer = 0;
        uint32 activeMuteId = 0;

        if (result)
        {
            fields = result->Fetch();
            charId = fields[0].GetUInt32();
            uint32 accId = fields[1].GetUInt32();

            sAccountMgr->GetName(accId, acc);

            // Get mute info
            result = LoginDatabase.PQuery("SELECT mute_timer, mute_id FROM mute_active WHERE account = '%u' AND realmid = '%u'", accId, realm.Id.Realm);

            if (result)
            {
                fields = result->Fetch();
                if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
                {
                    activeMuteTimer = fields[0].GetUInt64() / 1000;
                    activeMuteId = fields[1].GetUInt32();
                }
            }
        }
    
        // Send mute history by character GUID or name (for renamed or deleted characters)
        MuteListSendCharHistoryHelper(handler, name, charId, acc, limit, activeMuteId, activeMuteTimer);
    
        // Send current account mute history
        if (acc != "")
            MuteListSendAccHistoryHelper(handler, acc, limit, activeMuteId, activeMuteTimer, charId);

        handler->PSendSysMessage(LANG_MUTELIST_END_HISTORY);

        return true;
    }

    static void MuteListSendAccHistoryHelper(ChatHandler* handler, std::string acc, uint8 limit, uint32 activeMuteId, uint64 activeMuteTimer, uint32 excludeCharId)
    {
        // Get mute history
        QueryResult result;
        if (!limit)
        {
            //                                    0        1          2          3         4          5            6
            result = LoginDatabase.PQuery("SELECT char_id, mute_name, mute_date, muted_by, mute_time, mute_reason, id = '%u' FROM account_muted "
                                          "WHERE realmid = '%u' AND mute_acc = '%s' AND char_id <> '%u' ORDER BY mute_date ASC", activeMuteId, realm.Id.Realm, acc.c_str(), excludeCharId);
        }
        else
        {
            //                                                   0        1          2          3         4          5            6
            result = LoginDatabase.PQuery("SELECT * FROM (SELECT char_id, mute_name, mute_date, muted_by, mute_time, mute_reason, id = '%u' FROM account_muted "
                                          "               WHERE realmid = '%u' AND mute_acc = '%s' AND char_id <> '%u' ORDER BY mute_date DESC LIMIT %u) AS last_muted "
                                          "ORDER BY last_muted.mute_date ASC", activeMuteId, realm.Id.Realm, acc.c_str(), excludeCharId, limit);
        }

        if (limit)
            handler->PSendSysMessage(LANG_MUTELIST_ACC_LIMIT, limit, acc.c_str());
        else
            handler->PSendSysMessage(LANG_MUTELIST_ACC, acc.c_str());

        if (!result)
        {
            handler->PSendSysMessage(LANG_MUTELIST_EMPTY);
            return;
        }

        handler->PSendSysMessage("==============================");

        Field* fields;

        // Get hisrory rows
        uint32 muteCharId;

        std::string currName;
        std::string currAcc;

        std::unordered_map<uint32, std::pair<std::string, std::string>> nameAccCache;

        std::string muteName;
        std::string muteDate;
        std::string mutedBy;
        uint32 muteTime;
        std::string muteReason;
        bool muteActive;

        do
        {
            fields = result->Fetch();
            muteCharId = fields[0].GetUInt32();

            // Get current character name and account
            MuteListGetNameAndAccountHelper(ObjectGuid(HighGuid::Player, muteCharId), currName, currAcc, nameAccCache);

            muteName = fields[1].GetString();
            muteDate = TimeToTimestampStr(fields[2].GetUInt64());
            mutedBy = fields[3].GetString();
            muteTime = fields[4].GetUInt32();
            muteReason = fields[5].GetString();
            muteActive = fields[6].GetBool();

            // Send history row
            MuteListSendRowHelper(handler, currName, currAcc, muteName, acc, muteDate, mutedBy, muteTime, muteReason, false, muteActive ? activeMuteTimer : 0);
        } 
        while (result->NextRow());
    }

    static void MuteListSendCharHistoryHelper(ChatHandler* handler, std::string name, uint32 charId, std::string acc, uint8 limit, uint32 activeMuteId, uint64 activeMuteTimer)
    {
        // Get mute history
        QueryResult result;
        if (!limit)
        {
            //                                    0        1         2          3          4         5          6            7
            result = LoginDatabase.PQuery("SELECT char_id, mute_acc, mute_name, mute_date, muted_by, mute_time, mute_reason, id = '%u' FROM account_muted "
                                          "WHERE realmid = '%u' AND (char_id = '%u' OR mute_name = '%s') ORDER BY mute_date ASC", activeMuteId, realm.Id.Realm, charId, name.c_str());
        }
        else
        {
            //                                                   0        1         2          3          4         5          6            7
            result = LoginDatabase.PQuery("SELECT * FROM (SELECT char_id, mute_acc, mute_name, mute_date, muted_by, mute_time, mute_reason, id = '%u' FROM account_muted "
                                          "               WHERE realmid = %u AND (char_id = '%u' OR mute_name = '%s') ORDER BY mute_date DESC LIMIT %u) AS last_muted "
                                          "ORDER BY last_muted.mute_date ASC", activeMuteId, realm.Id.Realm, charId, name.c_str(), limit);
        }

        if (!result)
        {
            if (charId)
            {
                if (limit)
                    handler->PSendSysMessage(LANG_MUTELIST_CHAR_LIMIT, limit, name.c_str());
                else
                    handler->PSendSysMessage(LANG_MUTELIST_CHAR, name.c_str());

                handler->PSendSysMessage(LANG_MUTELIST_EMPTY);
            }
            else
                handler->PSendSysMessage(LANG_MUTELIST_CHAR_NOT_FOUND, name.c_str());

            return;
        }

        if (limit)
            handler->PSendSysMessage(LANG_MUTELIST_CHAR_LIMIT, limit, name.c_str());
        else
            handler->PSendSysMessage(LANG_MUTELIST_CHAR, name.c_str());

        handler->PSendSysMessage("==============================");

        Field* fields;

        // Get hisrory rows
        ObjectGuid muteCharId;
            
        std::string currName;
        std::string currAcc;

        std::unordered_map<uint32, std::pair<std::string, std::string>> nameAccCache;
    
        std::string muteName;
        std::string muteAcc;
        std::string muteDate;
        std::string mutedBy;
        uint32 muteTime;
        std::string muteReason;
        bool muteActive;

        do
        {
            fields = result->Fetch();
            muteCharId = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            muteAcc = fields[1].GetString();
            muteName = fields[2].GetString();
            muteDate = TimeToTimestampStr(fields[3].GetUInt64());
            mutedBy = fields[4].GetString();
            muteTime = fields[5].GetUInt32();
            muteReason = fields[6].GetString();
            muteActive = fields[7].GetBool();

            if (muteCharId == charId)
            {
                // Existing character, current name and account are known already
                currName = name;
                currAcc = acc;
            }
            else
            {
                // Get current character name and account
                MuteListGetNameAndAccountHelper(muteCharId, currName, currAcc, nameAccCache);
            }

            // Send history row
            MuteListSendRowHelper(handler, currName, currAcc, muteName, muteAcc, muteDate, mutedBy, muteTime, muteReason, true, muteActive ? activeMuteTimer : 0);
        }
        while (result->NextRow());
    }

    static void MuteListGetNameAndAccountHelper(ObjectGuid charId, std::string &name, std::string &acc, std::unordered_map<uint32, std::pair<std::string, std::string>> &nameAccCache)
    {
        std::unordered_map<uint32, std::pair<std::string, std::string>>::const_iterator iter = nameAccCache.find(charId);
        if (iter != nameAccCache.end())
        {
            name = iter->second.first;
            acc = iter->second.second;
        }
        else
        {
            // Get current character name
            if (CharacterNameData const* data = sWorld->GetCharacterNameData(charId))
            {
                name = data->m_name;

                // Get current character account
                QueryResult result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = '%u'", data->m_accountID);
                if (!result)
                {
                    // Hmmm (2), character without account?
                    acc = "";
                }
                else
                {
                    acc = result->Fetch()[0].GetString();
                }
            }
            else
            {
                // No, I don't know how this possible.
                name = "";
                acc = "";
            }
            nameAccCache[charId] = std::pair<std::string, std::string>(name, acc);
        }
    }

    static void MuteListSendRowHelper(ChatHandler* handler, std::string currName, std::string currAcc, std::string muteName, std::string muteAcc, std::string muteDate, std::string mutedBy, uint32 muteTime, std::string muteReason, bool showCurrAcc, uint64 activeMuteTimer)
    {
        // Build row title
        std::string title = muteDate + ": ";
        if (currName == "")
        {
            // Deleted character
            title += muteName + handler->GetTrinityString(LANG_MUTELIST_ROW_TITLE_DELETED);
        }
        else if (currName != muteName)
            // Renamed character
            title += muteName + handler->GetTrinityString(LANG_MUTELIST_ROW_TITLE_RENAMED) + handler->playerLink(currName);
        else
            title += handler->playerLink(currName);

        if (showCurrAcc)
        {
            if (currAcc != "")
            {
                title += handler->GetTrinityString(LANG_MUTELIST_ROW_TITLE_CURR_ACC) + currAcc;
                if (muteAcc != "" && currAcc != muteAcc)
                    // Character moved from another account
                    title += handler->GetTrinityString(LANG_MUTELIST_ROW_TITLE_MOVED_FROM) + muteAcc;
            }
        }
        else
        {
            if (currAcc != "" && currAcc != muteAcc)
                // Character moved to another account
                title += handler->GetTrinityString(LANG_MUTELIST_ROW_TITLE_MOVED_TO) + currAcc;
        }

        // Send hisrory row
        handler->PSendSysMessage(title.c_str());
        handler->PSendSysMessage(LANG_MUTELIST_ROW_INFO, muteTime, mutedBy.c_str(), muteReason.c_str());
        // Little magic for correct active mute
        if (activeMuteTimer && currName != "" && currAcc != "" && (!showCurrAcc || currAcc == muteAcc))
            handler->PSendSysMessage(LANG_MUTELIST_ROW_ACTIVE, secsToTimeString(activeMuteTimer).c_str());

        handler->PSendSysMessage("==============================");
    }

    static bool HandleMovegensCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_MOVEGENS_LIST, (unit->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), unit->GetGUID().GetCounter());

        MotionMaster* motionMaster = unit->GetMotionMaster();
        float x, y, z;
        motionMaster->GetDestination(x, y, z);

        for (uint8 i = 0; i < MAX_MOTION_SLOT; ++i)
        {
            MovementGenerator* movementGenerator = motionMaster->GetMotionSlot(i);
            if (!movementGenerator)
            {
                handler->SendSysMessage("Empty");
                continue;
            }

            switch (movementGenerator->GetMovementGeneratorType())
            {
                case IDLE_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_IDLE);
                    break;
                case RANDOM_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_RANDOM);
                    break;
                case WAYPOINT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_WAYPOINT);
                    break;
                case ANIMAL_RANDOM_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_ANIMAL_RANDOM);
                    break;
                case CONFUSED_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_CONFUSED);
                    break;
                case CHASE_MOTION_TYPE:
                {
                    Unit* target = NULL;
                    if (unit->GetTypeId() == TYPEID_PLAYER)
                        target = static_cast<ChaseMovementGenerator<Player> const*>(movementGenerator)->GetTarget();
                    else
                        target = static_cast<ChaseMovementGenerator<Creature> const*>(movementGenerator)->GetTarget();

                    if (!target)
                        handler->SendSysMessage(LANG_MOVEGENS_CHASE_NULL);
                    else if (target->GetTypeId() == TYPEID_PLAYER)
                        handler->PSendSysMessage(LANG_MOVEGENS_CHASE_PLAYER, target->GetName().c_str(), target->GetGUID().GetCounter());
                    else
                        handler->PSendSysMessage(LANG_MOVEGENS_CHASE_CREATURE, target->GetName().c_str(), target->GetGUID().GetCounter());
                    break;
                }
                case FOLLOW_MOTION_TYPE:
                {
                    Unit* target = NULL;
                    if (unit->GetTypeId() == TYPEID_PLAYER)
                        target = static_cast<FollowMovementGenerator<Player> const*>(movementGenerator)->GetTarget();
                    else
                        target = static_cast<FollowMovementGenerator<Creature> const*>(movementGenerator)->GetTarget();

                    if (!target)
                        handler->SendSysMessage(LANG_MOVEGENS_FOLLOW_NULL);
                    else if (target->GetTypeId() == TYPEID_PLAYER)
                        handler->PSendSysMessage(LANG_MOVEGENS_FOLLOW_PLAYER, target->GetName().c_str(), target->GetGUID().GetCounter());
                    else
                        handler->PSendSysMessage(LANG_MOVEGENS_FOLLOW_CREATURE, target->GetName().c_str(), target->GetGUID().GetCounter());
                    break;
                }
                case HOME_MOTION_TYPE:
                {
                    if (unit->GetTypeId() == TYPEID_UNIT)
                        handler->PSendSysMessage(LANG_MOVEGENS_HOME_CREATURE, x, y, z);
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_HOME_PLAYER);
                    break;
                }
                case FLIGHT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_FLIGHT);
                    break;
                case POINT_MOTION_TYPE:
                {
                    handler->PSendSysMessage(LANG_MOVEGENS_POINT, x, y, z);
                    break;
                }
                case FLEEING_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_FEAR);
                    break;
                case DISTRACT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_DISTRACT);
                    break;
                case EFFECT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_EFFECT);
                    break;
                default:
                    handler->PSendSysMessage(LANG_MOVEGENS_UNKNOWN, movementGenerator->GetMovementGeneratorType());
                    break;
            }
        }
        return true;
    }
    /*
    ComeToMe command REQUIRED for 3rd party scripting library to have access to PointMovementGenerator
    Without this function 3rd party scripting library will get linking errors (unresolved external)
    when attempting to use the PointMovementGenerator
    */
    static bool HandleComeToMeCommand(ChatHandler* handler, char const* args)
    {
        char const* newFlagStr = strtok((char*)args, " ");
        if (!newFlagStr)
            return false;

        Creature* caster = handler->getSelectedCreature();
        if (!caster)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        caster->GetMotionMaster()->MovePoint(0, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        return true;
    }

    static bool HandleDamageCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* str = strtok((char*)args, " ");

        if (strcmp(str, "go") == 0)
        {
            char* guidStr = strtok(NULL, " ");
            if (!guidStr)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int32 guid = atoi(guidStr);
            if (!guid)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* damageStr = strtok(NULL, " ");
            if (!damageStr)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int32 damage = atoi(damageStr);
            if (!damage)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (Player* player = handler->GetSession()->GetPlayer())
            {
                GameObject* go = NULL;

                if (GameObjectData const* goData = sObjectMgr->GetGOData(guid))
                    go = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(guid, goData->id);

                if (!go)
                {
                    handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, guid);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                if (!go->IsDestructibleBuilding())
                {
                    handler->SendSysMessage(LANG_INVALID_GAMEOBJECT_TYPE);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                go->ModifyHealth(-damage, player);
                handler->PSendSysMessage(LANG_GAMEOBJECT_DAMAGED, go->GetName().c_str(), guid, -damage, go->GetGOValue()->Building.Health);
            }

            return true;
        }

        Unit* target = handler->getSelectedUnit();
        if (!target || !handler->GetSession()->GetPlayer()->GetTarget())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (Player* player = target->ToPlayer())
            if (handler->HasLowerSecurity(player, ObjectGuid::Empty, false))
                return false;

        if (!target->IsAlive())
            return true;

        char* damageStr = strtok((char*)args, " ");
        if (!damageStr)
            return false;

        int32 damage_int = atoi((char*)damageStr);
        if (damage_int <= 0)
            return true;

        uint32 damage = damage_int;

        char* schoolStr = strtok((char*)NULL, " ");

        // flat melee damage without resistence/etc reduction
        if (!schoolStr)
        {
            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            if (target != handler->GetSession()->GetPlayer())
                handler->GetSession()->GetPlayer()->SendAttackStateUpdate (HITINFO_AFFECTS_VICTIM, target, 1, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_HIT, 0);
            return true;
        }

        uint32 school = atoi((char*)schoolStr);
        if (school >= MAX_SPELL_SCHOOL)
            return false;

        SpellSchoolMask schoolmask = SpellSchoolMask(1 << school);

        if (Unit::IsDamageReducedByArmor(schoolmask))
            damage = handler->GetSession()->GetPlayer()->CalcArmorReducedDamage(target, damage, NULL, BASE_ATTACK);

        char* spellStr = strtok((char*)NULL, " ");

        // melee damage by specific school
        if (!spellStr)
        {
            uint32 absorb = 0;
            uint32 resist = 0;

            handler->GetSession()->GetPlayer()->CalcAbsorbResist(target, schoolmask, SPELL_DIRECT_DAMAGE, damage, &absorb, &resist);

            if (damage <= absorb + resist)
                return true;

            damage -= absorb + resist;

            handler->GetSession()->GetPlayer()->DealDamageMods(target, damage, &absorb);
            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, schoolmask, NULL, false);
            handler->GetSession()->GetPlayer()->SendAttackStateUpdate (HITINFO_AFFECTS_VICTIM, target, 1, schoolmask, damage, absorb, resist, VICTIMSTATE_HIT, 0);
            return true;
        }

        // non-melee damage

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellid = handler->extractSpellIdFromLink((char*)args);
        if (!spellid || !sSpellMgr->GetSpellInfo(spellid))
            return false;

        handler->GetSession()->GetPlayer()->SpellNonMeleeDamageLog(target, spellid, damage);
        return true;
    }

    static bool HandleCombatStopCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;

        if (args && strlen(args) > 0)
        {
            target = ObjectAccessor::FindPlayerByName(args);
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (!target)
        {
            if (!handler->extractPlayerTarget((char*)args, &target))
                return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        target->CombatStop();
        target->getHostileRefManager().deleteReferences();
        return true;
    }

    static bool HandleRepairitemsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        // Repair items
        target->DurabilityRepairAll(false, 0, false);

        handler->PSendSysMessage(LANG_YOU_REPAIR_ITEMS, handler->GetNameLink(target).c_str());
        if (handler->needReportToTarget(target))
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, handler->GetNameLink().c_str());

        return true;
    }

    static bool HandleFreezeCommand(ChatHandler* handler, char const* args)
    {
        std::string name;
        Player* player;
        char const* TargetName = strtok((char*)args, " "); // get entered name
        if (!TargetName) // if no name entered use target
        {
            player = handler->getSelectedPlayer();
            if (player) //prevent crash with creature as target
            {
                name = player->GetName();
                normalizePlayerName(name);
            }
        }
        else // if name entered
        {
            name = TargetName;
            normalizePlayerName(name);
            player = ObjectAccessor::FindPlayerByName(name);
        }

        if (!player)
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
            return true;
        }

        if (player == handler->GetSession()->GetPlayer())
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_ERROR);
            return true;
        }

        // effect
        if (player && (player != handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_FREEZE, name.c_str());

            // stop combat + make player unattackable + duel stop + stop some spells
            player->SetFaction(35);
            player->CombatStop();
            if (player->IsNonMeleeSpellCasted(true))
                player->InterruptNonMeleeSpells(true);
            player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            // if player class = hunter || warlock remove pet if alive
            if ((player->GetClass() == CLASS_HUNTER) || (player->GetClass() == CLASS_WARLOCK))
                player->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);

            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(9454))
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);

            // save player
            player->SaveToDB();
        }

        return true;
    }

    static bool HandleUnFreezeCommand(ChatHandler* handler, char const*args)
    {
        std::string name;
        Player* player;
        char* targetName = strtok((char*)args, " "); // Get entered name

        if (targetName)
        {
            name = targetName;
            normalizePlayerName(name);
            player = ObjectAccessor::FindPlayerByName(name);
        }
        else // If no name was entered - use target
        {
            player = handler->getSelectedPlayer();
            if (player)
                name = player->GetName();
        }

        if (player)
        {
            handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());

            // Reset player faction + allow combat + allow duels
            player->setFactionForRace(player->GetRace());
            player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            // Remove Freeze spell (allowing movement and spells)
            player->RemoveAurasDueToSpell(9454);

            // Save player
            player->SaveToDB();
        }
        else
        {
            if (targetName)
            {
                // Check for offline players
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_GUID_BY_NAME);
                stmt->setString(0, name);
                PreparedQueryResult result = CharacterDatabase.Query(stmt);

                if (!result)
                {
                    handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                    return true;
                }

                // If player found: delete his freeze aura
                Field* fields = result->Fetch();
                uint32 lowGuid = fields[0].GetUInt32();

                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_AURA_FROZEN);
                stmt->setUInt32(0, lowGuid);
                CharacterDatabase.Execute(stmt);

                handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());
                return true;
            }
            else
            {
                handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                return true;
            }
        }

        return true;
    }

    static bool HandleListFreezeCommand(ChatHandler* handler, char const* /*args*/)
    {
        // Get names from DB
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_AURA_FROZEN);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_NO_FROZEN_PLAYERS);
            return true;
        }

        // Header of the names
        handler->PSendSysMessage(LANG_COMMAND_LIST_FREEZE);

        // Output of the results
        do
        {
            Field* fields = result->Fetch();
            std::string player = fields[0].GetString();
            handler->PSendSysMessage(LANG_COMMAND_FROZEN_PLAYERS, player.c_str());
        }
        while (result->NextRow());

        return true;
    }

    static bool HandlePlayAllCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 soundId = atoi((char*)args);

        if (!sSoundEntriesStore.LookupEntry(soundId))
        {
            handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        WorldPackets::Misc::PlaySound packet;
        packet.SourceObjectGUID = handler->GetSession()->GetPlayer()->GetGUID();
        packet.SoundKitID = soundId;

        sWorld->SendGlobalMessage(packet.Write());

        handler->PSendSysMessage(LANG_COMMAND_PLAYED_TO_ALL, soundId);
        return true;
    }

    static bool HandlePossessCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(unit, 530, true);
        return true;
    }

    static bool HandleUnPossessCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            unit = handler->GetSession()->GetPlayer();

        unit->RemoveCharmAuras();

        return true;
    }

    static bool HandleBindSightCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(unit, 6277, true);
        return true;
    }

    static bool HandleUnbindSightCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (player->isPossessing())
            return false;

        player->StopCastingBindSight();
        return true;
    }

    static void VisualizePosition(uint32 duration, WorldObject* source)
    {
        Position pos;
        pos.Relocate(source);
        for (uint32 i = 0; i < 5; ++i)
        {
            if (i)
                pos.RelocateOffset(0, 1.0f);
            if (TempSummon* waypoint = new TempSummon(nullptr, nullptr, false))
            {
                if (!waypoint->Create(source->GetMap()->GenerateLowGuid<HighGuid::Unit>(), source->GetMap(), source->GetPhaseMask(), 190012, 0, 0, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
                {
                    delete waypoint;
                    continue;
                }

                waypoint->SetHomePosition(waypoint->GetPosition());
                waypoint->InitStats(duration);
                waypoint->SetDisableGravity(true);
                waypoint->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
                if (i)
                    waypoint->SetFloatValue(OBJECT_FIELD_SCALE, 2.5f);
                source->GetMap()->AddToMap(waypoint->ToCreature());
                waypoint->InitSummon();
                waypoint->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN);
            }
        }
    }
    static void VisualizeRadius(uint32 duration, WorldObject* source, float radius)
    {
        Position pos;
        uint32 max = 8 + (uint32)radius;
        for (uint32 i = 0; i < max; ++i)
        {
            pos.Relocate(source);
            pos.RelocateOffset(2 * M_PI * i / max, radius);
            if (TempSummon* waypoint = new TempSummon(nullptr, nullptr, false))
            {
                if (!waypoint->Create(source->GetMap()->GenerateLowGuid<HighGuid::Unit>(), source->GetMap(), source->GetPhaseMask(), 190012, 0, 0, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
                {
                    delete waypoint;
                    continue;
                }

                waypoint->SetHomePosition(waypoint->GetPosition());
                waypoint->InitStats(duration);
                waypoint->SetDisableGravity(true);
                waypoint->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
                source->GetMap()->AddToMap(waypoint->ToCreature());
                waypoint->InitSummon();
                waypoint->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN);
            }
        }
    }
    static bool HandleVisualizePositionCommand(ChatHandler* handler, char const* args)
    {
        static std::set<Unit*> continuous;

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (continuous.find(target) != continuous.end())
            continuous.erase(target);
        else if (*args)
        {
            continuous.insert(target);
            target->m_Events.Repeated(10, 10, [target]() { VisualizePosition(10, target); return continuous.find(target) == continuous.end(); });
        }
        else
            VisualizePosition(1000, target);

        return true;
    }
    static bool HandleVisualizeCombatReachCommand(ChatHandler* handler, char const* args)
    {
        static std::set<Unit*> continuous;

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (continuous.find(target) != continuous.end())
            continuous.erase(target);
        else if (*args)
        {
            continuous.insert(target);
            target->m_Events.Repeated(10, 10, [target]() { VisualizeRadius(10, target, target->GetCombatReach()); return continuous.find(target) == continuous.end(); });
        }
        else
            VisualizeRadius(1000, target, target->GetCombatReach());

        return true;
    }

    static bool HandleItemSpecsCommand(ChatHandler* handler, char const* args)
    {
        uint32 id = ::strtoul(args, nullptr, 10);
        if (!id)
        {
            auto idStr = handler->extractKeyFromLink((char*)args, "Hitem");
            if (idStr)
                id = ::strtoul(idStr, nullptr, 10);
        }
        if (!id)
            return handler->SendError("Invalid argument");

        auto itemSpec = sObjectMgr->GetItemSpecInfo(id, 90);
        if (!itemSpec)
            return handler->SendError("ItemSpec coudn't be evaulated");
        handler->PSendSysMessage("Item %u:", id);

        for (uint32 i = 0; i < sChrSpecializationStore.GetNumRows(); ++i)
        {
            auto specEntry = sChrSpecializationStore.LookupEntry(i);
            auto classEntry = specEntry ? sChrClassesStore.LookupEntry(specEntry->classId) : nullptr;
            if (specEntry && classEntry)
                if (itemSpec->HasSpecializationId(specEntry->Id))
                    handler->PSendSysMessage("%s %s", classEntry->name[handler->GetSessionDbcLocale()], specEntry->Name[handler->GetSessionDbcLocale()]);
        }
        return true;
    }

    static bool HandleLootCooldownCommand(ChatHandler* handler, char const* args)
    {
        Player* player;
        if (!handler->extractPlayerTarget((char*)args, &player))
            return false;

        player->ClearLootLockouts();
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_LOOTLOCKOUTS);
        stmt->setUInt32(0, player->GetGUID().GetCounter());
        CharacterDatabase.Execute(stmt);
        handler->SendSysMessage("All loot lockouts cleared");
        return true;
    }

    static bool HandleLootGetPersonalCommand(ChatHandler* handler, char const* args)
    {
        uint32 id = ::strtoul(args, nullptr, 10);
        if (!id)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        PersonalLoot loot{ id };
        if (!loot)
            return handler->SendError("No personal loot for %u", id);
        loot.Reward(player);
        return true;
    }

    static bool HandleLootGetBonusCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        uint32 lootId = ::strtoul(tok[0], nullptr, 10);
        uint32 difficulty = ::strtoul(tok[1], nullptr, 10);
        BonusLoot loot{ lootId, Difficulty(difficulty) };
        if (loot)
            loot.Reward(handler->GetSession()->GetPlayer());
        else
            handler->SendSysMessage("No bonus loot");
        return true;
    }

    static bool HandleLootShowPersonalCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (!tok.size())
            return false;

        uint32 id = std::strtoul(tok[0], nullptr, 10);
        if (!id)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        uint32 spec = player->GetLootSpecialization();
        if (!spec)
            spec = player->GetTalentSpecialization();

        if (tok.size() > 1)
            spec = std::strtoul(tok[1], nullptr, 10);

        auto const* specEntry = sChrSpecializationStore.LookupEntry(spec);
        if (!specEntry)
            return handler->SendError("Invalid specialization id");

        PersonalLoot loot{ id };
        if (!loot)
            return handler->SendError("No personal loot for %u", id);

        handler->PSendSysMessage("Personal loot lootid: %u for spec: %s", id, specEntry->Name[handler->GetSessionDbcLocale()]);

        std::string name;
        for (auto&& itr : loot)
        {
            if (auto const* itemTemplate = sObjectMgr->GetItemTemplate(itr))
            {
                if (sObjectMgr->GetItemSpecInfo(itemTemplate->ItemId, 90)->HasSpecializationId(spec))
                {
                    if (auto const* sparse = sItemSparseStore.LookupEntry(itemTemplate->ItemId))
                        name = sparse->Name[handler->GetSessionDbcLocale()];
                    else
                        name = itemTemplate->Name1;
                    handler->PSendSysMessage("%u - |cffffffff|Hitem:%d:0:0:0:0:0:0:0:0|h[%s]|h|r", itemTemplate->ItemId, itemTemplate->ItemId, name.c_str());
                }
            }
        }

        handler->SendSysMessage("Personal loot end");
        return true;
    }

    static bool HandleLootShowBonusCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        uint32 lootId = ::strtoul(tok[0], nullptr, 10);
        uint32 difficulty = ::strtoul(tok[1], nullptr, 10);

        handler->PSendSysMessage("Bonus loot lootid: %u, difficulty: %u", lootId, difficulty);
        BonusLoot loot{ lootId, Difficulty(difficulty) };
        std::string name;
        for (auto&& it : loot)
        {
            if (auto const* sparse = sItemSparseStore.LookupEntry(it->ItemId))
                name = sparse->Name[handler->GetSessionDbcLocale()];
            else
                name = it->Name1;
            handler->PSendSysMessage("%u - |cffffffff|Hitem:%d:0:0:0:0:0:0:0:0|h[%s]|h|r", it->ItemId, it->ItemId, name.c_str());
        }
        handler->SendSysMessage("Bonus loot end");
        return true;
    }

    static bool HandleBattlegroundStartCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Battleground* bg = player->GetBattleground();
        if (!bg)
            return handler->SendError("You are not in battleground!");
        if (bg->GetStatus() != STATUS_WAIT_JOIN)
            return handler->SendError("Battleground must be in STATUS_WAIT_JOIN");
        bg->SetStartDelayTime(0);

        uint32 maxTime = bg->IsArena() ? ARENA_COUNTDOWN_MAX : BATTLEGROUND_COUNTDOWN_MAX;
        for (auto&& itr : bg->GetPlayers())
            if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                player->SendStartTimer(0, maxTime, TIMER_PVP);

        return true;
    }

    static bool HandleBattlegroundRegRatedCommand(ChatHandler* handler, char const* args)
    {
        WorldPacket data;
        handler->GetSession()->HandleBattlemasterJoinRated(data);
        return true;
    }

    static Creature* GetSelectedCreature(ChatHandler* handler)
    {
        if (Creature* target = handler->getSelectedCreature())
            return target;

        handler->SendSysMessage(LANG_SELECT_CREATURE);
        handler->SetSentErrorMessage(true);
        return nullptr;
    }
    static GameObject* GetSelectedGO(ChatHandler* handler)
    {
        if (GameObject* go = handler->GetNearbyGameObject())
            return go;

        handler->SendSysMessage(LANG_SELECT_CREATURE);
        handler->SetSentErrorMessage(true);
        return nullptr;
    }
    static bool HandleVisibilityActiveCommand(ChatHandler* handler, char const* args)
    {
        if (Creature* target = GetSelectedCreature(handler))
        {
            target->setActive(atoi(args));
            handler->PSendSysMessage("%u - %s: active %sset", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->isActiveObject() ? "" : "un");
            return true;
        }
        return false;
    }
    static bool HandleVisibilityActiveGOCommand(ChatHandler* handler, char const* args)
    {
        if (GameObject* target = GetSelectedGO(handler))
        {
            target->setActive(atoi(args));
            handler->PSendSysMessage("%u - %s: active %sset", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->isActiveObject() ? "" : "un");
            return true;
        }
        return false;
    }
    static bool HandleVisibilityGetCommand(ChatHandler* handler, char const* args)
    {
        if (Creature* target = GetSelectedCreature(handler))
        {
            if (target->HasCustomVisibility())
            {
                if (target->IsCustomVisibilityZoneOnly())
                {
                    char const* areaName = "???";
                    if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(target->GetCustomVisibilityZoneID()))
                        if (zone->area_name[handler->GetSessionDbcLocale()])
                            areaName = zone->area_name[handler->GetSessionDbcLocale()];
                    handler->PSendSysMessage("%u - %s: %.2f yards (%s)", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->GetCustomVisibilityDistance(), areaName);
                }
                else
                    handler->PSendSysMessage("%u - %s: %.2f yards", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->GetCustomVisibilityDistance());
            }
            else
                handler->PSendSysMessage("%u - %s: default", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());
            return true;
        }
        return false;
    }
    static bool HandleVisibilityGetGOCommand(ChatHandler* handler, char const* args)
    {
        if (GameObject* target = GetSelectedGO(handler))
        {
            if (target->HasCustomVisibility())
            {
                if (target->IsCustomVisibilityZoneOnly())
                {
                    char const* areaName = "???";
                    if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(target->GetCustomVisibilityZoneID()))
                        if (zone->area_name[handler->GetSessionDbcLocale()])
                            areaName = zone->area_name[handler->GetSessionDbcLocale()];
                    handler->PSendSysMessage("%u - %s: %.2f yards (%s)", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->GetCustomVisibilityDistance(), areaName);
                }
                else
                    handler->PSendSysMessage("%u - %s: %.2f yards", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), target->GetCustomVisibilityDistance());
            }
            else
                handler->PSendSysMessage("%u - %s: default", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());
            return true;
        }
        return false;
    }
    static bool HandleVisibilitySetCommand(ChatHandler* handler, char const* args)
    {
        if (Creature* target = GetSelectedCreature(handler))
        {
            float distance = atof(args);
            CustomVisibility::Type const type = CustomVisibility::Type::Creature;
            uint32 entry = target->GetEntry();
            uint32 zoneId, areaId;
            target->GetZoneAndAreaId(zoneId, areaId);

            std::stringstream ss;
            if (Map* map = target->FindMap())
                if (char const* name = map->GetEntry()->name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(zoneId))
                if (char const* name = zone->area_name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (AreaTableEntry const* area = zoneId == areaId ? nullptr : sAreaTableStore.LookupEntry(areaId))
                if (char const* name = area->area_name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (CreatureTemplate const* info = sObjectMgr->GetCreatureTemplate(entry))
                ss << info->Name;

            std::string comment = ss.str();
            size_t index = 0;
            while ((index = comment.find("\"", index)) != std::string::npos)
            {
                comment.replace(index, 1, "\"\"");
                index += 2;
            }

            if (distance > 0)
            {
                WorldDatabase.PExecute("INSERT INTO object_visibility (type, entry, distance, comment) VALUES (%u, %u, %f, \"%s\") ON DUPLICATE KEY UPDATE distance = %f", (uint32)type, entry, distance, comment.c_str(), distance);

                bool isNew = !sObjectMgr->GetCustomVisibilityInfo(type, entry);
                CustomVisibility::Info& info = sObjectMgr->GetCustomVisibilityInfoMap()[type][entry];
                info.Type = type;
                info.Entry = entry;
                info.Distance = distance;
                if (isNew)
                {
                    info.Active = false;
                    info.Importance = (CustomVisibility::Importance)0;
                    info.Comment = ss.str();
                }

                handler->PSendSysMessage("%u - %s: visibility distance set to %.2f", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), distance);
            }
            else
            {
                WorldDatabase.PExecute("DELETE FROM object_visibility WHERE type = %u AND entry = %u", (uint32)type, entry);

                sObjectMgr->GetCustomVisibilityInfoMap()[type].erase(entry);

                handler->PSendSysMessage("%u - %s: visibility distance set to default", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());
            }

            if (Map* map = target->FindMap())
                for (auto creature : map->GetCreatureBySpawnIdStore())
                    if (creature.second->GetEntry() == entry)
                        creature.second->LoadCustomVisibility();

            return true;
        }
        return false;
    }
    static bool HandleVisibilitySetGOCommand(ChatHandler* handler, char const* args)
    {
        if (GameObject* target = GetSelectedGO(handler))
        {
            float distance = atof(args);
            CustomVisibility::Type const type = CustomVisibility::Type::GameObject;
            uint32 entry = target->GetEntry();
            uint32 zoneId, areaId;
            target->GetZoneAndAreaId(zoneId, areaId);

            std::stringstream ss;
            if (Map* map = target->FindMap())
                if (char const* name = map->GetEntry()->name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(zoneId))
                if (char const* name = zone->area_name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (AreaTableEntry const* area = zoneId == areaId ? nullptr : sAreaTableStore.LookupEntry(areaId))
                if (char const* name = area->area_name[handler->GetSessionDbcLocale()])
                    ss << name << " - ";
            if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(entry))
                ss << info->name;

            std::string comment = ss.str();
            size_t index = 0;
            while ((index = comment.find("\"", index)) != std::string::npos)
            {
                comment.replace(index, 1, "\"\"");
                index += 2;
            }

            if (distance > 0)
            {
                WorldDatabase.PExecute("INSERT INTO object_visibility (type, entry, distance, comment) VALUES (%u, %u, %f, \"%s\") ON DUPLICATE KEY UPDATE distance = %f", (uint32)type, entry, distance, comment.c_str(), distance);

                bool isNew = !sObjectMgr->GetCustomVisibilityInfo(type, entry);
                CustomVisibility::Info& info = sObjectMgr->GetCustomVisibilityInfoMap()[type][entry];
                info.Type = type;
                info.Entry = entry;
                info.Distance = distance;
                if (isNew)
                {
                    info.Active = false;
                    info.Importance = (CustomVisibility::Importance)0;
                    info.Comment = ss.str();
                }

                handler->PSendSysMessage("%u - %s: visibility distance set to %.2f", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str(), distance);
            }
            else
            {
                WorldDatabase.PExecute("DELETE FROM object_visibility WHERE type = %u AND entry = %u", (uint32)type, entry);

                sObjectMgr->GetCustomVisibilityInfoMap()[type].erase(entry);

                handler->PSendSysMessage("%u - %s: visibility distance set to default", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());
            }

            if (Map* map = target->FindMap())
                for (auto creature : map->GetGameObjectBySpawnIdStore())
                    if (creature.second->GetEntry() == entry)
                        creature.second->LoadCustomVisibility();

            return true;
        }
        return false;
    }
    static bool HandleVisibilityReloadCommand(ChatHandler* handler, char const* args)
    {
        if (Creature* target = GetSelectedCreature(handler))
        {
            CustomVisibility::Type const type = CustomVisibility::Type::Creature;
            uint32 entry = target->GetEntry();

            sObjectMgr->LoadCustomVisibility(type, entry);

            handler->PSendSysMessage("%u - %s: visibility settings reloaded", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());

            uint32 count = 0;
            if (Map* map = target->FindMap())
                for (auto&& creature : map->GetCreatureBySpawnIdStore())
                {
                    if (creature.second->GetEntry() == entry)
                    {
                        creature.second->LoadCustomVisibility();
                        ++count;
                    }
                }
            handler->PSendSysMessage("Updated %u spawned Creatures", count);

            return true;
        }
        return false;
    }
    static bool HandleVisibilityReloadGOCommand(ChatHandler* handler, char const* args)
    {
        if (GameObject* target = GetSelectedGO(handler))
        {
            CustomVisibility::Type const type = CustomVisibility::Type::GameObject;
            uint32 entry = target->GetEntry();

            sObjectMgr->LoadCustomVisibility(type, entry);

            handler->PSendSysMessage("%u - %s: visibility settings reloaded", target->GetEntry(), target->GetNameForLocaleIdx((LocaleConstant)handler->GetSessionDbLocaleIndex()).c_str());

            uint32 count = 0;

            if (Map* map = target->FindMap())
                for (auto go : map->GetGameObjectBySpawnIdStore())
                    if (go.second->GetEntry() == entry)
                    {
                        go.second->LoadCustomVisibility();
                        ++count;
                    }

            handler->PSendSysMessage("Updated %u spawned GameObjects", count);

            return true;
        }
        return false;
    }
    static bool HandleVisibilityReloadAllCommand(ChatHandler* handler, char const* args)
    {
        std::map<CustomVisibility::Type, std::set<uint32>> objects;

        // Cache entries that previously had settings
        for (auto&& category : sObjectMgr->GetCustomVisibilityInfoMap())
            for (auto&& info : category.second)
                objects[category.first].insert(info.first);

        sObjectMgr->LoadCustomVisibility();

        // Cache entries that now have settings
        for (auto&& category : sObjectMgr->GetCustomVisibilityInfoMap())
            for (auto&& info : category.second)
                objects[category.first].insert(info.first);

        handler->PSendSysMessage("Reloaded visibility settings for:");
        handler->PSendSysMessage("- %lu Creatures", objects[CustomVisibility::Type::Creature].size());
        handler->PSendSysMessage("- %lu GameObjects", objects[CustomVisibility::Type::GameObject].size());
        handler->PSendSysMessage("- %lu DynamicObjects", objects[CustomVisibility::Type::DynamicObject].size());

        // Update all entries that either previously had or currently have settings
        uint32 count = 0;
        sMapMgr->DoForAllMaps([&count,&objects](Map* map)
        {
            for (auto creature : map->GetCreatureBySpawnIdStore())
            {
                if (objects[CustomVisibility::Type::Creature].find(creature.second->GetEntry()) != objects[CustomVisibility::Type::Creature].end())
                {
                    creature.second->LoadCustomVisibility();
                    ++count;
                }
            }
        });

        handler->PSendSysMessage("Updated %u spawned Creatures", count);

        count = 0;
        sMapMgr->DoForAllMaps([&count,&objects](Map* map)
        {
            for (auto go : map->GetGameObjectBySpawnIdStore())
            {
                if (objects[CustomVisibility::Type::GameObject].find(go.second->GetEntry()) != objects[CustomVisibility::Type::GameObject].end())
                {
                    go.second->LoadCustomVisibility();
                    ++count;
                }
            }
        });
        handler->PSendSysMessage("Updated %u spawned GameObjects", count);

        return true;
    }
    static bool HandleVisibilityStatsCommand(ChatHandler* handler, char const* args)
    {
        static std::map<CustomVisibility::Importance, std::string> importanceNames =
        {
            { CustomVisibility::Importance::Critical,           "Critical"          },
            { CustomVisibility::Importance::Large,              "Large"             },
            { CustomVisibility::Importance::DistantScenery,     "DistantScenery"    },
            { CustomVisibility::Importance::FlyingScenery,      "FlyingScenery"     },
            { CustomVisibility::Importance::GroundScenery,      "GroundScenery"     },
            { CustomVisibility::Importance::FlyingClutter,      "FlyingClutter"     },
            { CustomVisibility::Importance::GroundClutter,      "GroundClutter"     },
        };

        Map* map = handler->GetSession()->GetPlayer()->GetMap();
        {
            auto&& objects = map->GetCustomVisibilityObjects();
            handler->PSendSysMessage("%lu map-wide objects:", objects.size());

            std::map<CustomVisibility::Importance, uint32> counts;
            for (auto&& obj : objects)
                if (CustomVisibility::Info const* info = sObjectMgr->GetCustomVisibilityInfo(obj->GetTypeId(), obj->GetEntry()))
                    ++counts[info->Importance];

            for (auto&& count : counts)
                handler->PSendSysMessage("- %u %s", count.second, importanceNames[count.first].c_str());
        }
        {
            auto&& objects = map->GetCustomVisibilityObjects(handler->GetSession()->GetPlayer()->GetZoneId());
            handler->PSendSysMessage("%lu zone-wide objects:", objects.size());

            std::map<CustomVisibility::Importance, uint32> counts;
            for (auto&& obj : objects)
                if (CustomVisibility::Info const* info = sObjectMgr->GetCustomVisibilityInfo(obj->GetTypeId(), obj->GetEntry()))
                    ++counts[info->Importance];

            for (auto&& count : counts)
                handler->PSendSysMessage("- %u %s", count.second, importanceNames[count.first].c_str());
        }

        return true;
    }
    static bool HandleVisibilityUpdateCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 phaseMask = target->GetPhaseMask();
        target->SetPhaseMask(0, true);
        target->SetPhaseMask(phaseMask, true);
        return true;
    }

    static bool HandleReplaceCommand(ChatHandler* handler, char const* args)
    {
        // extract args
        char* temp = strtok((char*)args, " ");
        char* tempArgs[4];
        int32 i = 0;
        while (temp)
        {
            tempArgs[i++] = temp;
            temp = strtok(NULL, " ");
        }

        if (i < 4) // invalid args count
            return false;

        char* name = tempArgs[0];
        int32 oldSkill = atoi(tempArgs[1]);
        int32 newSkill = atoi(tempArgs[2]);
        int32 value = atoi(tempArgs[3]);
        Player* target = NULL;
        std::string targetName;
        ObjectGuid targetGuid;

        // extract target
        if (!handler->extractPlayerTarget(name, &target, &targetGuid, &targetName))
            return false;

        // I hope this situation will be blocked in web part...
        if (oldSkill == newSkill)
            return false;

        std::string chrNameLink = handler->playerLink(targetName);

        QueryResult result = CharacterDatabase.PQuery("SELECT guid, skill FROM character_skills WHERE guid = %u AND skill = %u", targetGuid.GetCounter(), uint32(oldSkill));
        if (!result)
        {
            if (!target || (target && !target->HasSkill(oldSkill)))
            {
                handler->PSendSysMessage(LANG_PLAYER_DOES_NOT_HAVE_SKILL, chrNameLink.c_str(), targetGuid.GetCounter(), uint32(oldSkill));
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        uint32 guid = targetGuid.GetCounter();
        ServiceEntry s{ ISERVICE_REPLACE_PROFFESION };
        s.Data1 = oldSkill;
        s.Data2 = newSkill;
        s.Data3 = value;
        sServiceMgr->AddService(target, guid, s);

        std::ostringstream oldProfStream;
        oldProfStream << "Old Prof: " << oldSkill;
        std::ostringstream newProfStream;
        newProfStream << "New Prof: " << newSkill;
        sServiceMgr->ExecutedServices(guid, SERVICE_TYPE_CHANGE_PROFESSION, oldProfStream.str(), newProfStream.str());

        handler->PSendSysMessage(target ? LANG_REPLACE_PROFESSION_OK : LANG_REPLACE_PROFESSION_NEXT_LOGIN, chrNameLink.c_str(), guid);
        return true;
    }

    struct ItemDeleteCommandRevertInfo
    {
        uint32 itemEntry;
        uint32 owner_guid;
        uint32 creatorGuid;
        uint32 giftCreatorGuid;
        uint32 count;
        uint32 duration;
        std::string charges;
        uint32 flags;
        std::string enchantments;
        int16 randomPropertyId;
        uint32 reforgeId;
        uint32 transmogrifyId;
        uint32 upgradeId;
        uint16 durability;
        uint32 playedTime;
        std::string text;
        uint32 speciesId;
        uint32 breed;
        uint32 quality;
        uint32 level;

        uint32 inventory_guid;
        uint32 inventory_bag;
        uint8 inventory_slot;
    };

    static std::map<uint64, ItemDeleteCommandRevertInfo> _itemDeleteCommandRevertInfo;

    static bool HandleItemDeleteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* a = strtok((char*)args, " ");
        char* b = strtok(NULL, " ");

        bool revert = a && *a && strstr("revert", a);

        uint32 itemGuid;
        ItemDeleteCommandRevertInfo info;

        if (revert)
        {
            itemGuid = uint32(strtoull(b, NULL, 10));
            if (!itemGuid)
                return false;

            if (_itemDeleteCommandRevertInfo.find(itemGuid) == _itemDeleteCommandRevertInfo.end())
            {
                handler->PSendSysMessage("Revert data for item with guid %u does not exist.", itemGuid);
                handler->SetSentErrorMessage(true);
                return false;
            }

            info = _itemDeleteCommandRevertInfo[itemGuid];
        }
        else
        {
            if (!a)
                return false;

            itemGuid = uint32(strtoull(a, NULL, 10));
            if (!itemGuid)
                return false;

            QueryResult result = CharacterDatabase.PQuery("SELECT itemEntry, owner_guid, creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, reforgeId, transmogrifyId, upgradeId, durability, playedTime, text, pet_species, pet_breed, pet_quality, pet_level FROM item_instance WHERE guid='%u'", itemGuid);
            if (!result)
            {
                handler->PSendSysMessage("Item with guid %u was not found in DB.", itemGuid);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Field* fields = result->Fetch();
            uint32 i = 0;
            info.itemEntry = fields[i++].GetUInt32();
            info.owner_guid = fields[i++].GetUInt32();
            info.creatorGuid = fields[i++].GetUInt32();
            info.giftCreatorGuid = fields[i++].GetUInt32();
            info.count = fields[i++].GetUInt32();
            info.duration = fields[i++].GetUInt32();
            info.charges = fields[i++].GetString();
            info.flags = fields[i++].GetUInt32();
            info.enchantments = fields[i++].GetString();
            info.randomPropertyId = fields[i++].GetInt16();
            info.reforgeId = fields[i++].GetInt32();
            info.transmogrifyId = fields[i++].GetInt32();
            info.upgradeId = fields[i++].GetInt32();
            info.durability = fields[i++].GetUInt16();
            info.playedTime = fields[i++].GetUInt32();
            info.text = fields[i++].GetString();
            info.speciesId = fields[i++].GetUInt32();
            info.breed = fields[i++].GetUInt32();
            info.quality = fields[i++].GetUInt32();
            info.level = fields[i++].GetUInt32();

            result = CharacterDatabase.PQuery("SELECT guid, bag, slot FROM character_inventory WHERE item='%u'", itemGuid);
            if (!result)
            {
                handler->PSendSysMessage("Item with guid %u was not found in players's inventory.", itemGuid);
                handler->SetSentErrorMessage(true);
                return false;
            }

            fields = result->Fetch();
            i = 0;
            info.inventory_guid = fields[i++].GetUInt32();
            info.inventory_bag = fields[i++].GetUInt32();
            info.inventory_slot = fields[i++].GetUInt8();
        }

        ObjectGuid ownerGuid(HighGuid::Player, info.owner_guid);
        if (Player* player = ObjectAccessor::FindConnectedPlayer(ownerGuid))
        {
            handler->PSendSysMessage("Item owner %s (GUID: %u) is currently online.", player->GetName().c_str(), info.owner_guid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string playerName;
        if (!sObjectMgr->GetPlayerNameByGUID(ownerGuid, playerName))
        {
            handler->PSendSysMessage("Item owner (GUID: %u) was not found.", info.owner_guid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountId = sObjectMgr->GetPlayerAccountIdByGUID(ownerGuid);
        if (!accountId)
        {
            handler->PSendSysMessage("Item owner (GUID: %u) was not found.", info.owner_guid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sWorld->FindSession(accountId))
        {
            handler->PSendSysMessage("Item owner (GUID: %u) is currently online or logged in.", info.owner_guid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(info.itemEntry);
        if (!proto)
        {
            handler->PSendSysMessage("Template of item with guid %u is missing.", itemGuid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string name = proto->Name1;
        if (handler->GetSession() && handler->GetSession()->GetPlayer())
        {
            if (ItemLocale const* locale = sObjectMgr->GetItemLocale(proto->ItemId))
                ObjectMgr::GetLocaleString(locale->Name, handler->GetSession()->GetSessionDbLocaleIndex(), name);

            std::stringstream ss;
            ss << "|c" << std::hex << ItemQualityColors[proto->Quality] << std::dec << "|Hitem:" << proto->ItemId << ":0:0:0:0:0:" << info.randomPropertyId << ":0:" << uint32(handler->GetSession()->GetPlayer()->GetLevel()) << "|h[" << name << "]|h|r";
            name = ss.str();
        }
        if (revert)
        {
            handler->PSendSysMessage("Restoring item %s (GUID: %u, Entry: %u) to player %s (GUID: %u) inventory.", name.c_str(), itemGuid, info.itemEntry, playerName.c_str(), info.owner_guid);

            uint8 index = 0;
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ITEM_INSTANCE);
            stmt->setUInt32(  index, info.itemEntry);
            stmt->setUInt32(++index, info.owner_guid);
            stmt->setUInt32(++index, info.creatorGuid);
            stmt->setUInt32(++index, info.giftCreatorGuid);
            stmt->setUInt32(++index, info.count);
            stmt->setUInt32(++index, info.duration);
            stmt->setString(++index, info.charges);
            stmt->setUInt32(++index, info.flags);
            stmt->setString(++index, info.enchantments);
            stmt->setInt16 (++index, info.randomPropertyId);
            stmt->setUInt32(++index, info.reforgeId);
            stmt->setUInt32(++index, info.transmogrifyId);
            stmt->setUInt32(++index, info.upgradeId);
            stmt->setUInt16(++index, info.durability);
            stmt->setUInt32(++index, info.playedTime);
            stmt->setString(++index, info.text);
            stmt->setUInt32(++index, itemGuid);
            CharacterDatabase.Execute(stmt);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_INVENTORY_ITEM);
            stmt->setUInt32(0, info.inventory_guid);
            stmt->setUInt32(1, info.inventory_bag);
            stmt->setUInt8 (2, info.inventory_slot);
            stmt->setUInt32(3, itemGuid);
            CharacterDatabase.Execute(stmt);

            _itemDeleteCommandRevertInfo.erase(itemGuid);
        }
        else
        {
            handler->PSendSysMessage("Deleting item %s (GUID: %u, Entry: %u) from player %s (GUID: %u) inventory. Use .itemdelete revert %u to restore the item.", name.c_str(), itemGuid, info.itemEntry, playerName.c_str(), info.owner_guid, itemGuid);

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
            stmt->setUInt32(0, itemGuid);
            CharacterDatabase.Execute(stmt);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_INVENTORY_BY_ITEM);
            stmt->setUInt32(0, itemGuid);
            CharacterDatabase.Execute(stmt);

            _itemDeleteCommandRevertInfo[itemGuid] = info;
        }

        return true;
    }

    static bool HandleRemoveItemCommand(ChatHandler* handler, const char *args)
    {
        if (!*args)
            return false;

        uint32 itemId = 0;

        if (args[0] == '[') // [name] manual form
        {
            char* citemName = strtok((char*)args, "]");

            if (citemName && citemName[0])
            {
                std::string itemName = citemName+1;
                WorldDatabase.EscapeString(itemName);
                QueryResult result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
                if (!result)
                {
                    handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, citemName+1);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                itemId = result->Fetch()->GetUInt16();
            }
            else
                return false;
        }
        else // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
        {
            char* cId = handler->extractKeyFromLink((char*)args,"Hitem");
            if (!cId)
                return false;
            itemId = atol(cId);
        }

        char* ccount = strtok(NULL, " ");

        int32 count = 1;

        if (ccount)
            count = strtol(ccount, NULL, 10);

        if (count == 0)
            count = 1;

        Player* pl = handler->GetSession()->GetPlayer();
        Player* plTarget = handler->getSelectedPlayer();
        if (!plTarget)
            plTarget = pl;

            plTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, handler->GetNameLink(plTarget).c_str());
            return true;

        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
        if (!proto)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Subtract
        if (count < 0)
        {
            plTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, handler->GetNameLink(plTarget).c_str());
            return true;
        }

        return true;
    }

    static bool HandleCheckLadderCommand(ChatHandler* handler, char const* args)
    {
        std::string dir = sConfigMgr->GetStringDefault("LogsDir", ".");
        std::ofstream out{ dir + "/laddercheck.log" };
        sRatedPvpMgr->PrintSeasonResults(out);
        handler->SendSysMessage("See log for details");
        return true;
    }

    static bool HandleDeletedItemListCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player;
        ObjectGuid guid = ObjectGuid::Empty;
        std::string name;

        if (!handler->extractPlayerTarget((char*)args, &player, &guid, &name))
            return false;

        uint8 count = 0;

        QueryResult result = CharacterDatabase.PQuery("SELECT id, old_item_guid, item_entry, item_count, delete_date, delete_type, restored FROM item_deleted WHERE owner_guid = '%u' ORDER BY delete_date DESC LIMIT 51", guid);
        if (!result)
        {
            handler->SetSentErrorMessage(true);
            handler->PSendSysMessage("No deleted items found for player %s.", name.c_str());
            return false;
        }

        handler->PSendSysMessage("List of deleted items for player %s (guid %u):", name.c_str(), guid.GetCounter());
        do
        {
            if (count > 50)
            {
                handler->PSendSysMessage("And other deleted items. Full list you can check in database.");
                break;
            }

            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            ObjectGuid oldItemGuid(fields[1].GetUInt64());
            uint32 itemEntry = fields[2].GetUInt32();
            uint32 itemCount = fields[3].GetUInt32();
            std::string deleteDate = TimeToTimestampStr(fields[4].GetUInt64());
            uint32 deleteType = fields[5].GetUInt32();
            uint32 restored = fields[6].GetUInt8();

            char const* deleteTypeStr;
            switch(deleteType)
            {
                case DELETING_TYPE_BY_PLAYER:  deleteTypeStr = "Deleted by player.";      break;
                case DELETING_TYPE_VENDOR:     deleteTypeStr = "Sold to vendor.";         break;
                case DELETING_TYPE_DISENCHANT: deleteTypeStr = "Disenchanted by player."; break;
                default:                       deleteTypeStr = "Fate unknown.";           break;
            }

            char const* restoredTypeStr;
            switch (restored)
            {
                case 0:  restoredTypeStr = "Deleted.";          break;
                case 1:  restoredTypeStr = "Restored.";         break;
                case 2:  restoredTypeStr = "Error in process."; break;
                default: restoredTypeStr = "Fate unknown.";     break;
            }

            if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(itemEntry))
            {
                std::string name = item->Name1;
                if (ItemLocale const* locale = sObjectMgr->GetItemLocale(itemEntry))
                    sObjectMgr->GetLocaleString(locale->Name, handler->GetSession()->GetSessionDbLocaleIndex(), name);

                handler->PSendSysMessage("%s - ID %u - %ux |c%x|Hitem:%u:%u:%u:%u:%u:%u:%d:%u:%u|h[%s]|h|r (guid: %u) - %s Status - %s", deleteDate.c_str(), id, itemCount, ItemQualityColors[item->Quality], itemEntry, 0, 0, 0, 0, 0, 0, 0, 0, name.c_str(), oldItemGuid.GetCounter(), deleteTypeStr, restoredTypeStr);
            }
            else
                handler->PSendSysMessage("%s - ID %u - %ux [MISSING ITEM TEMPLATE %u] (guid: %u) - %s Status - %s", deleteDate.c_str(), id, itemCount, itemEntry, oldItemGuid.GetCounter(), deleteTypeStr, restoredTypeStr);

            count++;
        } while (result->NextRow());

        return true;
    }

    static bool HandleDeletedItemRestoreCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player;
        std::string name, itemsStr;
        ObjectGuid guid = ObjectGuid::Empty;
        bool all = false;

        if (!handler->extractPlayerTarget((char*)args, &player, &guid, &name))
            return false;

        char* tail = strtok(NULL, "");
        if (!tail)
            return false;

        if (!strncmp(tail, "all", strlen(tail)))
            all = true;
        else for (auto&& itemIdStr : Tokenizer { tail, ' ' })
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT id FROM item_deleted WHERE id = '%u' AND owner_guid = '%u' AND (`restored`= 0 OR `restored`= 2)", atoi(itemIdStr), guid.GetCounter());
            if (!result)
            {
                handler->SetSentErrorMessage(true);
                handler->PSendSysMessage("Not found deleted item with id %u for player %s.", atoi(itemIdStr), name.c_str());
            }
            else
            {
                if (!itemsStr.empty())
                    itemsStr += ',';
                itemsStr += itemIdStr;
            }
        }

        if (itemsStr.empty() && !all)
        {
            handler->SetSentErrorMessage(true);
            handler->PSendSysMessage("Not found deleted items for player %s.", name.c_str());
            return false;
        }

        std::list<Item*> itemStorage;
        QueryResult result;
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        if (all)
            result = CharacterDatabase.PQuery("SELECT id, old_item_guid, item_entry, item_count FROM item_deleted WHERE owner_guid = '%u' AND `restored`=0", guid.GetCounter());
        else
            result = CharacterDatabase.PQuery("SELECT id, old_item_guid, item_entry, item_count FROM item_deleted WHERE id in (%s) AND owner_guid = '%u' AND `restored`=0", itemsStr.c_str(), guid.GetCounter());

        if (!result)
        {
            handler->SetSentErrorMessage(true);
            handler->PSendSysMessage("Not found deleted items for player %s.", name.c_str());
            return false;
        }

        std::ostringstream restoredItems;
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            uint32 oldItemGuid = fields[1].GetUInt32();
            uint32 itemEntry = fields[2].GetUInt32();
            uint32 itemCount = fields[3].GetUInt32();

            // keep in mind item will finally deleted only after logout
            if (CharacterDatabase.PQuery("SELECT 1 FROM item_instance WHERE guid = '%u' AND owner_guid = '%u' AND itemEntry = '%u'", oldItemGuid, guid.GetCounter(), itemEntry))
                Item::DeleteFromDB(trans, oldItemGuid);

            if (Item* item = Item::CreateItem(itemEntry, itemCount))
            {
                itemStorage.push_back(item);
                item->SetOwnerGUID(guid);
                item->SetBinding(true);
                item->SaveToDB(trans);                               // save for prevent lost at next mail load, if send fail then item will deleted
                restoredItems << itemEntry << " ";
            }
            else
            {
                handler->SetSentErrorMessage(true);
                handler->PSendSysMessage("Can`t create item %u, skip restoring.", itemEntry);
                continue;
            }

            CharacterDatabase.PExecute("UPDATE item_deleted SET `restored`=1, `restore_date`='%u' WHERE id = '%u' AND owner_guid = '%u'", uint32(time(NULL)), id, guid.GetCounter());
        } while (result->NextRow());

        while (!itemStorage.empty())
        {
            MailDraft draft("Тех.поддержка", "Возврат удаленных предметов.");
            for (uint8 i = 0; !itemStorage.empty() && i < MAX_MAIL_ITEMS; ++i)
            {
                draft.AddItem(itemStorage.front());
                itemStorage.pop_front();
            }
            draft.SendMailTo(trans, MailReceiver(player, guid.GetCounter()), MailSender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUID().GetCounter() : 0, MAIL_STATIONERY_GM));

            sServiceMgr->ExecutedServices(guid.GetCounter(), SERVICE_TYPE_ITEM_RESTORE, std::string("Restored Items: ") + restoredItems.str(), "");
        }
        CharacterDatabase.CommitTransaction(trans);

        handler->PSendSysMessage("Items successfully restored.");
        return true;
    }
};

std::map<uint64, misc_commandscript::ItemDeleteCommandRevertInfo> misc_commandscript::_itemDeleteCommandRevertInfo;

void AddSC_misc_commandscript()
{
    new misc_commandscript();
}

