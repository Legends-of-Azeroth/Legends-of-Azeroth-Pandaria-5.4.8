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

/* ScriptData
Name: server_commandscript
%Complete: 100
Comment: All server related commands
Category: commandscripts
EndScriptData */

#include "Chat.h"
#include "Config.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SystemConfig.h"
#include "MapManager.h"
#include "MapInstanced.h"
#include "Group.h"

class server_commandscript : public CommandScript
{
public:
    server_commandscript() : CommandScript("server_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> serverIdleRestartCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,   &HandleServerShutDownCancelCommand, },
            { "",               SEC_ADMINISTRATOR,  true,   &HandleServerIdleRestartCommand,    },
        };

        static std::vector<ChatCommand> serverIdleShutdownCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,   &HandleServerShutDownCancelCommand, },
            { "",               SEC_ADMINISTRATOR,  true,   &HandleServerIdleShutDownCommand,   },
        };

        static std::vector<ChatCommand> serverRestartCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,   &HandleServerShutDownCancelCommand, },
            { "",               SEC_ADMINISTRATOR,  true,   &HandleServerRestartCommand,        },
        };

        static std::vector<ChatCommand> serverShutdownCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,   &HandleServerShutDownCancelCommand, },
            { "",               SEC_ADMINISTRATOR,  true,   &HandleServerShutDownCommand,       },
            
        };

        static std::vector<ChatCommand> serverSetCommandTable =
        {
            { "difftime",       SEC_ADMINISTRATOR,  true,   &HandleServerSetDiffTimeCommand,    },
            { "loglevel",       SEC_ADMINISTRATOR,  true,   &HandleServerSetLogLevelCommand,    },
            { "motd",           SEC_ADMINISTRATOR,  true,   &HandleServerSetMotdCommand,        },
            { "closed",         SEC_ADMINISTRATOR,  true,   &HandleServerSetClosedCommand,      },
        };

        static std::vector<ChatCommand> serverStatsCommandTable =
        {
            { "mapupdate",      SEC_ADMINISTRATOR,      true,   &HandleServerStatsMapUpdateCommand, },
        };

        static std::vector<ChatCommand> serverCommandTable =
        {
            { "corpses",        SEC_ADMINISTRATOR,  true,   &HandleServerCorpsesCommand,        },
            { "exit",           SEC_ADMINISTRATOR,  true,   &HandleServerExitCommand,           },
            { "idlerestart",    SEC_ADMINISTRATOR,  true,   serverIdleRestartCommandTable       },
            { "idleshutdown",   SEC_ADMINISTRATOR,  true,   serverIdleShutdownCommandTable      },
            { "info",           SEC_PLAYER,         true,   &HandleServerInfoCommand,           },
            { "motd",           SEC_PLAYER,         true,   &HandleServerMotdCommand,           },
            { "plimit",         SEC_ADMINISTRATOR,  true,   &HandleServerPLimitCommand,         },
            { "restart",        SEC_ADMINISTRATOR,  true,   serverRestartCommandTable           },
            { "shutdown",       SEC_ADMINISTRATOR,  true,   serverShutdownCommandTable          },
            { "set",            SEC_ADMINISTRATOR,  true,   serverSetCommandTable               },
            { "stats",          SEC_ADMINISTRATOR,      true,   serverStatsCommandTable             },
        };

         static std::vector<ChatCommand> commandTable =
        {
            { "server",         SEC_PLAYER,         true,   serverCommandTable                  },
            
        };
        return commandTable;
    }

    // Triggering corpses expire check in world
    static bool HandleServerCorpsesCommand(ChatHandler* /*handler*/, char const* /*args*/)
    {
        sWorld->RemoveOldCorpses();
        return true;
    }

    static bool HandleServerInfoCommand(ChatHandler* handler, char const* /*args*/)
    {
        uint32 playersNum           = sWorld->GetPlayerCount();
        //uint32 maxPlayersNum        = sWorld->GetMaxPlayerCount();
        //uint32 activeClientsNum     = sWorld->GetActiveSessionCount();
        uint32 queuedClientsNum     = sWorld->GetQueuedSessionCount();
        uint32 maxActiveClientsNum  = sWorld->GetMaxActiveSessionCount();
        uint32 maxQueuedClientsNum  = sWorld->GetMaxQueuedSessionCount();
        std::string uptime          = secsToTimeString(sWorld->GetUptime());
        uint32 updateTime           = sWorld->GetUpdateTime();

        if (playersNum > sWorld->getIntConfig(CONFIG_MAX_ICORE))
            playersNum = (playersNum * sWorld->getRate(RATE_MAX_MODW));
        if (maxActiveClientsNum > sWorld->getIntConfig(CONFIG_MAX_ICORE))
            maxActiveClientsNum = (maxActiveClientsNum * sWorld->getRate(RATE_MAX_MODW));

        handler->PSendSysMessage("-=Project=- " REV_DATE " |cffff0000 Update Pack 3 (11.06.2019)");
        handler->PSendSysMessage(LANG_CONNECTED_USERS, playersNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
        handler->PSendSysMessage(LANG_UPTIME, uptime.c_str());

        // Can't use sWorld->ShutdownMsg here in case of console command
        if (sWorld->IsShuttingDown())
            handler->PSendSysMessage(LANG_SHUTDOWN_TIMELEFT, secsToTimeString(sWorld->GetShutDownTimeLeft()).c_str());

        if (updateTime > 500)
            handler->PSendSysMessage(LANG_DIFF_POOR, updateTime);
        else if (updateTime > 200)
            handler->PSendSysMessage(LANG_DIFF_NORMAL, updateTime);
        else
            handler->PSendSysMessage(LANG_DIFF_GOOD, updateTime);

        if (handler->GetSession())
            for (auto&& bonusRate : sWorld->GetBonusRates())
                if (bonusRate.second.IsActive())
                    if (!bonusRate.second.GetActiveAnnouncement().empty())
                        handler->SendSysMessage(bonusRate.second.GetActiveAnnouncement().c_str());

        return true;
    }

    // Display the 'Message of the day' for the realm
    static bool HandleServerMotdCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage(LANG_MOTD_CURRENT, sWorld->GetMotd());
        return true;
    }

    static bool HandleServerPLimitCommand(ChatHandler* handler, char const* args)
    {
        if (*args)
        {
            char* paramStr = strtok((char*)args, " ");
            if (!paramStr)
                return false;

            int32 limit = strlen(paramStr);

            if (strncmp(paramStr, "player", limit) == 0)
                sWorld->SetPlayerSecurityLimit(SEC_PLAYER);
            else if (strncmp(paramStr, "moderator", limit) == 0)
                sWorld->SetPlayerSecurityLimit(SEC_MODERATOR);
            else if (strncmp(paramStr, "gamemaster", limit) == 0)
                sWorld->SetPlayerSecurityLimit(SEC_GAMEMASTER);
            else if (strncmp(paramStr, "administrator", limit) == 0)
                sWorld->SetPlayerSecurityLimit(SEC_ADMINISTRATOR);
            else if (strncmp(paramStr, "reset", limit) == 0)
            {
                sWorld->SetPlayerAmountLimit(sConfigMgr->GetIntDefault("PlayerLimit", 100));
                sWorld->LoadDBAllowedSecurityLevel();
            }
            else
            {
                int32 value = atoi(paramStr);
                if (value < 0)
                    sWorld->SetPlayerSecurityLimit(AccountTypes(-value));
                else
                    sWorld->SetPlayerAmountLimit(uint32(value));
            }
        }

        uint32 playerAmountLimit = sWorld->GetPlayerAmountLimit();
        AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
        char const* secName = "";
        switch (allowedAccountType)
        {
            case SEC_PLAYER:
                secName = "Player";
                break;
            case SEC_MODERATOR:
                secName = "Moderator";
                break;
            case SEC_GAMEMASTER:
                secName = "Gamemaster";
                break;
            case SEC_ADMINISTRATOR:
                secName = "Administrator";
                break;
            default:
                secName = "<unknown>";
                break;
        }
        handler->PSendSysMessage("Player limits: amount %u, min. security level %s.", playerAmountLimit, secName);

        return true;
    }

    static bool HandleServerShutDownCancelCommand(ChatHandler* /*handler*/, char const* /*args*/)
    {
        sWorld->ShutdownCancel();

        return true;
    }

    static bool HandleServerShutDownCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* timeStr = strtok((char*) args, " ");
        char* exitCodeStr = strtok(NULL, "");

        int32 time = atoi(timeStr);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (timeStr[0] != '0' || timeStr[1] != '\0')) || time < 0)
            return false;

        if (exitCodeStr)
        {
            int32 exitCode = atoi(exitCodeStr);

            // Handle atoi() errors
            if (exitCode == 0 && (exitCodeStr[0] != '0' || exitCodeStr[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitCode < 0 || exitCode > 125)
                return false;

            sWorld->ShutdownServ(time, 0, exitCode);
        }
        else
            sWorld->ShutdownServ(time, 0, SHUTDOWN_EXIT_CODE);

        return true;
    }

    static bool HandleServerRestartCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* timeStr = strtok((char*) args, " ");
        char* exitCodeStr = strtok(NULL, "");

        int32 time = atoi(timeStr);

        //  Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (timeStr[0] != '0' || timeStr[1] != '\0')) || time < 0)
            return false;

        if (exitCodeStr)
        {
            int32 exitCode = atoi(exitCodeStr);

            // Handle atoi() errors
            if (exitCode == 0 && (exitCodeStr[0] != '0' || exitCodeStr[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitCode < 0 || exitCode > 125)
                return false;

            sWorld->ShutdownServ(time, SHUTDOWN_MASK_RESTART, exitCode);
        }
        else
            sWorld->ShutdownServ(time, SHUTDOWN_MASK_RESTART, RESTART_EXIT_CODE);

        return true;
    }

    static bool HandleServerIdleRestartCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* timeStr = strtok((char*) args, " ");
        char* exitCodeStr = strtok(NULL, "");

        int32 time = atoi(timeStr);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (timeStr[0] != '0' || timeStr[1] != '\0')) || time < 0)
            return false;

        if (exitCodeStr)
        {
            int32 exitCode = atoi(exitCodeStr);

            // Handle atoi() errors
            if (exitCode == 0 && (exitCodeStr[0] != '0' || exitCodeStr[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitCode < 0 || exitCode > 125)
                return false;

            sWorld->ShutdownServ(time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, exitCode);
        }
        else
            sWorld->ShutdownServ(time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, RESTART_EXIT_CODE);
        return true;
    }

    static bool HandleServerIdleShutDownCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* timeStr = strtok((char*) args, " ");
        char* exitCodeStr = strtok(NULL, "");

        int32 time = atoi(timeStr);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (timeStr[0] != '0' || timeStr[1] != '\0')) || time < 0)
            return false;

        if (exitCodeStr)
        {
            int32 exitCode = atoi(exitCodeStr);

            // Handle atoi() errors
            if (exitCode == 0 && (exitCodeStr[0] != '0' || exitCodeStr[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitCode < 0 || exitCode > 125)
                return false;

            sWorld->ShutdownServ(time, SHUTDOWN_MASK_IDLE, exitCode);
        }
        else
            sWorld->ShutdownServ(time, SHUTDOWN_MASK_IDLE, SHUTDOWN_EXIT_CODE);

        return true;
    }

    // Exit the realm
    static bool HandleServerExitCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(LANG_COMMAND_EXIT);
        World::StopNow(SHUTDOWN_EXIT_CODE);
        return true;
    }

    // Define the 'Message of the day' for the realm
    static bool HandleServerSetMotdCommand(ChatHandler* handler, char const* args)
    {
        sWorld->SetMotd(args);
        handler->PSendSysMessage(LANG_MOTD_NEW, args);
        return true;
    }

    // Set whether we accept new clients
    static bool HandleServerSetClosedCommand(ChatHandler* handler, char const* args)
    {
        if (strncmp(args, "on", 3) == 0)
        {
            handler->SendSysMessage(LANG_WORLD_CLOSED);
            sWorld->SetClosed(true);
            return true;
        }
        else if (strncmp(args, "off", 4) == 0)
        {
            handler->SendSysMessage(LANG_WORLD_OPENED);
            sWorld->SetClosed(false);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    // Set the level of logging
    static bool HandleServerSetLogLevelCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* type = strtok((char*)args, " ");
        char* name = strtok(NULL, " ");
        char* level = strtok(NULL, " ");

        if (!type || !name || !level || *name == '\0' || *level == '\0' || (*type != 'a' && *type != 'l'))
            return false;

        sLog->SetLogLevel(name, atoi(level), *type == 'l');
        return true;
    }

    // set diff time record interval
    static bool HandleServerSetDiffTimeCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        char* newTimeStr = strtok((char*)args, " ");
        if (!newTimeStr)
            return false;

        int32 newTime = atoi(newTimeStr);
        if (newTime < 0)
            return false;

        sWorld->SetRecordDiffInterval(newTime);
        printf("Record diff every %u ms\n", newTime);

        return true;
    }

#define GOOD_DIFF       50
#define BAD_DIFF        100
#define GOOD_DIFF_I     10
#define BAD_DIFF_I      50
#define GOOD_COLOR      "|cFF00FF00"
#define NORMAL_COLOR    "|cFFFFFF00"
#define BAD_COLOR       "|cFFFF0000"

    static bool HandleServerStatsMapUpdateCommand(ChatHandler* handler, const char* args)
    {
        static const char* difficulty[MAX_DIFFICULTY] = { "REGULAR", "5N", "5H", "10N", "25N", "10H", "25H", "LFR", "CHALLENGE", "40", "UNUSED", "SCENARIO_H", "SCENARIO_N", "UNUSED2", "FLEX" };
        struct InstanceInfo
        {
            uint32 InstanceID;
            uint32 UpdateTime;
            uint32 PlayerCount;
            Difficulty InstanceDifficulty;
            std::string GroupLeaderName;
        };
        struct MapInfo
        {
            uint32 MapID;
            char const* MapName;
            bool IsRaid;
            uint32 UpdateTime;
            uint32 PlayerCount;
            uint32 InstanceCount;
            std::list<InstanceInfo> Instances;
        };

        char const* detailsStr = "details";
        bool details = args && strlen(args) && strstr(detailsStr, args) == detailsStr;

        std::list<MapInfo> maps;
        for (uint32 mapId = 0; mapId < sMapStore.GetNumRows(); ++mapId)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
            if (!mapEntry)
                continue;

            Map* map = sMapMgr->FindBaseMap(mapId);
            if (!map)
                continue;

            MapInfo mapInfo = { mapId, map->GetMapName(), map->IsRaid(), 0, 0, 0, {}};
            if (map->Instanceable() && map->ToMapInstanced())
            {
                for (auto& instance : map->ToMapInstanced()->GetInstancedMaps())
                {
                    uint32 updateTime = instance.second->GetUpdateTime();
                    uint32 playerCount = instance.second->GetPlayersCountExceptGMs();

                    ++mapInfo.InstanceCount;
                    mapInfo.PlayerCount += playerCount;

                    if (!updateTime) // Did not contribute to server performance
                        continue;

                    InstanceInfo info = { instance.first, updateTime, playerCount, instance.second->GetDifficulty(), "" };

                    Map::PlayerList const& players = instance.second->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            info.GroupLeaderName = player->GetGroup() ? player->GetGroup()->GetLeaderName() : player->GetName();

                    mapInfo.UpdateTime += info.UpdateTime;
                    if (details)
                        mapInfo.Instances.push_back(info);
                }
            }
            else
            {
                mapInfo.PlayerCount = map->GetPlayersCountExceptGMs();
                mapInfo.UpdateTime = map->GetUpdateTime();
            }

            if (!mapInfo.UpdateTime) // Did not contribute to server performance
                continue;

            if (details && mapInfo.InstanceCount)
                mapInfo.Instances.sort([](InstanceInfo const& a, InstanceInfo const& b) { return a.UpdateTime < b.UpdateTime; });

            maps.push_back(mapInfo);
        }

        if (maps.empty())
        {
            handler->PSendSysMessage("Am I even alive?");
            return true;
        }

        handler->PSendSysMessage("Listing maps and the time it took to update them:");

        maps.sort([](MapInfo const& a, MapInfo const& b) { return a.UpdateTime < b.UpdateTime; });
        uint32 mapDiff = 0;
        for (auto& map : maps)
        {
            mapDiff += map.UpdateTime;
            if (map.InstanceCount)
            {
                handler->PSendSysMessage("|cFFE0E0E0- Map |cFFFFFFFF%u|cFFE0E0E0 - |cFFFFFFFF%s|cFFE0E0E0: |cFFFFFFFF%u|cFFE0E0E0i |cFFFFFFFF%u|cFFE0E0E0p %s[%ums]|r", map.MapID, map.MapName, map.InstanceCount, map.PlayerCount, map.UpdateTime <= GOOD_DIFF ? GOOD_COLOR : map.UpdateTime > BAD_DIFF ? BAD_COLOR : NORMAL_COLOR, map.UpdateTime);
                if (details)
                    for (auto& instance : map.Instances)
                        handler->PSendSysMessage("|cFFC0C0C0  - Instance |cFFFFFFFF%u|cFFE0E0E0 - |cFFFFFFFF%s|cFFC0C0C0: |cFFFFFFFF%u|cFFC0C0C0p %s %s[%ums]|r", instance.InstanceID, difficulty[instance.InstanceDifficulty], instance.PlayerCount, instance.GroupLeaderName.size() ? handler->playerLink(instance.GroupLeaderName).c_str() : "", instance.UpdateTime <= GOOD_DIFF_I ? GOOD_COLOR : instance.UpdateTime > BAD_DIFF_I ? BAD_COLOR : NORMAL_COLOR, instance.UpdateTime);
            }
            else
                handler->PSendSysMessage("|cFFE0E0E0- Map |cFFFFFFFF%u|cFFE0E0E0 - |cFFFFFFFF%s|cFFE0E0E0: |cFFFFFFFF%u|cFFE0E0E0p %s[%ums]|r", map.MapID, map.MapName, map.PlayerCount, map.UpdateTime <= GOOD_DIFF ? GOOD_COLOR : map.UpdateTime > BAD_DIFF ? BAD_COLOR : NORMAL_COLOR, map.UpdateTime);
        }
        uint32 worldDiff = sWorld->GetUpdateTime();
        handler->PSendSysMessage("Total time spent on map updates: %ums", mapDiff);
        handler->PSendSysMessage("Time spent on other updates: %ums", worldDiff >= mapDiff ? worldDiff - mapDiff : 0);

        return true;
    }
};

void AddSC_server_commandscript()
{
    new server_commandscript();
}
