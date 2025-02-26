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

#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Group.h"
#include "Player.h"
#include "Config.h"

static void GetPlayerInfo(ChatHandler* handler, Player* player)
{
    if (!player)
        return;

    ObjectGuid guid = player->GetGUID();
    if (auto queues = sLFGMgr->GetPlayerQueues(guid))
    {
        for (auto&& itr : *queues)
        {
            std::string state = lfg::GetStateString(itr.second.State);
            std::string dungeons = lfg::ConcatenateDungeons(itr.second.Dungeons);
            std::string roles = lfg::GetRolesString(itr.second.Roles);
            handler->PSendSysMessage(LANG_LFG_PLAYER_INFO, player->GetName().c_str(), itr.first,
                state.c_str(), uint8(itr.second.Dungeons.size()), dungeons.c_str(), roles.c_str());
        }
    }
    else
    {
        handler->PSendSysMessage(LANG_LFG_PLAYER_INFO, player->GetName().c_str(), 0,
            "None", 0, "None", "None");
    }
}

class lfg_commandscript : public CommandScript
{
public:
    lfg_commandscript() : CommandScript("lfg_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> lfgCommandTable =
        {
            { "player",     SEC_ADMINISTRATOR,  false,  &HandleLfgPlayerInfoCommand,    },
            { "group",      SEC_ADMINISTRATOR,  false,  &HandleLfgGroupInfoCommand,     },
            { "queue",      SEC_ADMINISTRATOR,  false,  &HandleLfgQueueInfoCommand,     },
            { "clean",      SEC_ADMINISTRATOR,  false,  &HandleLfgCleanCommand,         },
            { "options",    SEC_ADMINISTRATOR,  false,  &HandleLfgOptionsCommand,       },
            { "profiling",  SEC_ADMINISTRATOR,  true,   &HandleLfgProfilingCommand      },
            { "bug",        SEC_ADMINISTRATOR,  false,  &HandleLfgBugCommand            },
            { "flex",       SEC_ADMINISTRATOR,  false,
            {
                { "reg",    SEC_ADMINISTRATOR,  false,  &HandleLfgFlexRegCommand,       },
                { "scale",  SEC_ADMINISTRATOR,  false,  &HandleLfgFlexScaleCommand,     },
            } },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "lfg",        SEC_ADMINISTRATOR,  false,  lfgCommandTable                 },
        };
        return commandTable;
    }

    static bool HandleLfgPlayerInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, NULL, &playerName))
            return false;

        GetPlayerInfo(handler, target);
        return true;
    }

    static bool HandleLfgGroupInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, NULL, &playerName))
            return false;

        if (!target)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            return true;
        }

        Group* grp = target->GetGroup();
        if (!grp)
        {
            handler->PSendSysMessage(LANG_LFG_NOT_IN_GROUP, playerName.c_str());
            return true;
        }

        ObjectGuid guid = grp->GetGUID();
        std::string const& state = lfg::GetStateString(sLFGMgr->GetActiveState(guid));
        handler->PSendSysMessage(LANG_LFG_GROUP_INFO, grp->isLFGGroup(),
            state.c_str(), sLFGMgr->GetDungeon(guid));

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
            GetPlayerInfo(handler, itr->GetSource());

        return true;
    }

    static bool HandleLfgOptionsCommand(ChatHandler* handler, char const* args)
    {
        int32 options = -1;
        if (char* str = strtok((char*)args, " "))
        {
            int32 tmp = atoi(str);
            if (tmp > -1)
                options = tmp;
        }

        if (options != -1)
        {
            sLFGMgr->SetOptions(options);
            handler->PSendSysMessage(LANG_LFG_OPTIONS_CHANGED);
        }
        handler->PSendSysMessage(LANG_LFG_OPTIONS, sLFGMgr->GetOptions());
        return true;
    }

    static bool HandleLfgQueueInfoCommand(ChatHandler* handler, char const* args)
    {
        uint32 dungeonID = std::strtoul(args, nullptr, 10);

        handler->SendSysMessage(sLFGMgr->DumpQueueInfo(dungeonID, handler->GetSession()).c_str());
        return true;
    }

    static bool HandleLfgCleanCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage(LANG_LFG_CLEAN);
        sLFGMgr->Clean();
        return true;
    }

    static bool HandleLfgProfilingCommand(ChatHandler* handler, char const* /*args*/)
    {
        for (auto&& manager : sLFGMgr->GetQueueManagers())
        {
            handler->PSendSysMessage("Queue %u:", manager.first);
            static struct ProfilingInfo
            {
                char const* Name;
                lfg::QueueManager::ProfilingType Type;
            } infos[] =
            {
                { "AddToQueue",         lfg::QueueManager::ProfilingType::AddToQueue },
                { "RemoveFromQueue",    lfg::QueueManager::ProfilingType::RemoveFromQueue },
                { "MakeProposals",      lfg::QueueManager::ProfilingType::MakeProposals },
                { "UpdateQueueTimers",  lfg::QueueManager::ProfilingType::UpdateQueueTimers },
            };
            for (auto&& info : infos)
            {
                lfg::QueueManager::ProfilingData const& data = manager.second.GetProfilingData(info.Type);
                if (data.TotalProfiles)
                {
                    float total = data.TotalMicroseconds / 1000.0f;
                    handler->PSendSysMessage("- %s: Avg: %.6f ms (%.6f ms/queuer), %.3f%% of uptime, Total: %.3f ms", info.Name, total / data.TotalProfiles, data.TotalQueuers ? (total / data.TotalQueuers) : 0, 100.0f * total / getMSTime(), total);
                }
            }
        }
        return true;
    }

    static bool HandleLfgBugCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        time_t now = time(nullptr);

        static std::map<uint32, time_t> cooldowns;
        time_t& cooldown = cooldowns[player->GetGUID().GetCounter()];
        if (now <= cooldown)
        {
            handler->PSendSysMessage("Вы уже использовали эту команду за последнюю минуту");
            handler->SetSentErrorMessage(true);
            return false;
        }
        cooldown = now + MINUTE;

        return true;
    }

    static bool HandleLfgFlexRegCommand(ChatHandler* handler, char const* args)
    {
        uint32 quarter = std::strtoul(args, nullptr, 10);
        if (!quarter || quarter > 4)
        {
            handler->PSendSysMessage("Incorrect quarter.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!handler->GetSession()->GetPlayer()->IsGameMaster())
        {
            handler->PSendSysMessage("You must be in gm mode to get access in flex.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        lfg::LfgDungeonSet flexRaid;
        switch (quarter)
        {
            case 1: flexRaid.insert(726); break;
            case 2: flexRaid.insert(728); break;
            case 3: flexRaid.insert(729); break;
            case 4: flexRaid.insert(730); break;
        }

        if (!flexRaid.empty())
            sLFGMgr->JoinLfg(handler->GetSession()->GetPlayer(), lfg::PLAYER_ROLE_DAMAGE, flexRaid, "");

        return true;
    }

    static bool HandleLfgFlexScaleCommand(ChatHandler* handler, char const* args)
    {
        uint32 count = std::strtoul(args, nullptr, 10);
        if (!count || count < 10)
        {
            handler->PSendSysMessage("Incorrect value.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* map = handler->GetSession()->GetPlayer()->GetMap();
        if (!map->IsRaid() && map->GetDifficulty() != RAID_DIFFICULTY_1025MAN_FLEX)
        {
            handler->PSendSysMessage("You must be in dungeon with flex difficulty.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        map->SetDebugFlexPlayersCount(count);
        if (InstanceScript* instance = ((InstanceMap*)map)->GetInstanceScript())
            instance->UpdateDynamicHealth();
        handler->PSendSysMessage("Instance scalled to value %d.", count);
        return true;
    }
};

void AddSC_lfg_commandscript()
{
    new lfg_commandscript();
}
