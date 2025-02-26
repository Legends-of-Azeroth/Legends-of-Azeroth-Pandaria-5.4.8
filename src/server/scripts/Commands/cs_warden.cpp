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
#include "WardenCheckMgr.h"
#include "Warden.h"

class warden_commandscript : public CommandScript
{
public:
    warden_commandscript() : CommandScript("warden_commandscript") { }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> wardenInfoCommandTable =
        {
            { "endscene",                   SEC_GAMEMASTER,      false, &HandleWardenInfoEndScene,                       },
        };
        static std::vector<ChatCommand> wardenCommandTable =
        {
            { "reload",                     SEC_ADMINISTRATOR,      true,   &HandleWardenReloadCommand,                     },
            { "enable",                     SEC_GAMEMASTER,      false,  &HandleWardenEnableCommand,                     },
            { "disable",                    SEC_GAMEMASTER,      false,  &HandleWardenDisableCommand,                    },
            { "toggle",                     SEC_GAMEMASTER,      false,  &HandleWardenToggleCommand,                     },
            { "check",                      SEC_GAMEMASTER,      false,  &HandleWardenCheckCommand,                      },
            { "group",                      SEC_GAMEMASTER,      false,  &HandleWardenGroupCommand,                      },
            { "direct",                     SEC_ADMINISTRATOR,      false,  &HandleWardenDirectCommand,                     },
            { "list",                       SEC_GAMEMASTER,      true,   &HandleWardenListCommand,                       },
            { "info",                       SEC_GAMEMASTER,      false,  wardenInfoCommandTable                          },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "warden",                     SEC_GAMEMASTER,      true,   wardenCommandTable                              },
        };
        return commandTable;
    }

    static bool HandleWardenReloadCommand(ChatHandler* handler, char const* /*args*/)
    {
        sWardenCheckMgr->LoadWardenChecks(true);
        handler->PSendSysMessage("Warden checks successfully reloaded");
        return true;
    }

    static Warden* GetWardenHelper(ChatHandler* handler, char const* /*args*/, bool checkBusy = true)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
            return NULL;

        WorldSession* session = target->GetSession();
        if (!session)
        {
            handler->PSendSysMessage("Targeted player is missing session O_O. Call reality check police.");
            handler->SetSentErrorMessage(true);
            return NULL;
        }

        Warden* warden = session->GetWarden();
        if (!warden)
        {
            handler->PSendSysMessage("Warden is not enabled for the targeted player");
            handler->SetSentErrorMessage(true);
            return NULL;
        }

        if (checkBusy && warden->IsBusy())
        {
            handler->PSendSysMessage("Warden is currently awaiting response for sent request from the targeted player, please wait until making more requests. If no manual request was made, this can be a sign of a hack that forces client to ignore incoming warden requests");
            handler->SetSentErrorMessage(true);
            return NULL;
        }

        return warden;
    }

    static bool HandleWardenEnableCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args, false))
        {
            if (warden->IsEnabled())
            {
                handler->PSendSysMessage("Warden is already enabled for this player");
                handler->SetSentErrorMessage(true);
                return false;
            }

            warden->Enable();
            handler->PSendSysMessage("Enabled warden for player %s", handler->getSelectedPlayer()->GetName().c_str());
            return true;
        }
        return false;
    }

    static bool HandleWardenDisableCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args, false))
        {
            if (!warden->IsEnabled())
            {
                handler->PSendSysMessage("Warden is already disabled for this player");
                handler->SetSentErrorMessage(true);
                return false;
            }

            warden->Disable();
            handler->PSendSysMessage("Disabled warden for player %s", handler->getSelectedPlayer()->GetName().c_str());
            return true;
        }
        return false;
    }

    static bool HandleWardenToggleCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args, false))
        {
            if (warden->IsEnabled())
            {
                warden->Disable();
                handler->PSendSysMessage("Enabled warden for player %s", handler->getSelectedPlayer()->GetName().c_str());
            }
            else
            {
                warden->Enable();
                handler->PSendSysMessage("Disabled warden for player %s", handler->getSelectedPlayer()->GetName().c_str());
            }
            return true;
        }
        return false;
    }

    static bool HandleWardenCheckCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args))
        {
            char* idStr = strtok((char*)args, " ");
            if (!idStr)
            {
                handler->PSendSysMessage("No check ID was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint16 id = atoi(idStr);

            WardenRequestContext context;
            context.ResponseReceiverGUID = handler->GetSession()->GetPlayer()->GetGUID();
            context.CheckCount = 1;

            WardenCheck* check = sWardenCheckMgr->GetWardenDataById(id);
            if (!check)
            {
                handler->PSendSysMessage("Check with ID %u not found", uint32(id));
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (check->Locale != TOTAL_LOCALES && check->Locale != handler->getSelectedPlayer()->GetSession()->GetSessionDbLocaleIndex())
            {
                handler->PSendSysMessage("Locale mismatch: check requires locale %s and targeted client uses %s", localeNames[check->Locale], localeNames[handler->GetSessionDbLocaleIndex()]);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (check->Type == MEM_CHECK || check->Type == MODULE_CHECK)
                context.MemCheckIDs.push_back(id);
            else
                context.OtherCheckIDs.push_back(id);

            warden->RequestData(&context);

            handler->PSendSysMessage("Sent warden check request");
            return true;
        }
        return false;
    }

    static bool HandleWardenGroupCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args))
        {
            char* groupStr = strtok((char*)args, " ");
            if (!groupStr)
            {
                handler->PSendSysMessage("No check group was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* countStr = strtok(NULL, " ");

            WardenRequestContext context;
            context.ResponseReceiverGUID = handler->GetSession()->GetPlayer()->GetGUID();

            WardenCheckMgr::CheckContainer const& checks = sWardenCheckMgr->GetWardenData();
            for (uint16 id = 0; id < checks.size(); ++id)
            {
                if (WardenCheck* check = checks[id])
                {
                    if (check->Locale != TOTAL_LOCALES && check->Locale != handler->getSelectedPlayer()->GetSession()->GetSessionDbLocaleIndex())
                        continue;

                    if (!check->Group.compare(groupStr))
                    {
                        ++context.CheckCount;
                        if (check->Type == MEM_CHECK || check->Type == MODULE_CHECK)
                            context.MemCheckIDs.push_back(id);
                        else
                            context.OtherCheckIDs.push_back(id);
                    }
                }
            }

            if (countStr)
            {
                if (uint32 count = atoi(countStr))
                    if (context.CheckCount > count)
                        context.CheckCount = count;
            }
            else if (context.CheckCount > 5)
                context.CheckCount = 5;

            warden->RequestData(&context);

            handler->PSendSysMessage("Sent warden group check request");
            return true;
        }
        return false;
    }

    static bool HandleWardenDirectCommand(ChatHandler* handler, char const* args)
    {
        if (Warden* warden = GetWardenHelper(handler, args))
        {
            char* typeStr = strtok((char*)args, " ");
            if (!typeStr)
            {
                handler->PSendSysMessage("No check type was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* tailStr = strtok(NULL, "");
            if (!tailStr)
            {
                handler->PSendSysMessage("Provided data is malformed");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* dataStr = handler->extractQuotedArg(tailStr);
            if (!dataStr)
            {
                handler->PSendSysMessage("No check data was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            tailStr = strtok(NULL, "");
            if (!tailStr)
            {
                handler->PSendSysMessage("Provided str is malformed");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* strStr = handler->extractQuotedArg(tailStr);
            if (!strStr)
            {
                handler->PSendSysMessage("No check str was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* addressStr = strtok(NULL, " ");
            if (!addressStr)
            {
                handler->PSendSysMessage("No check address was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* lengthStr = strtok(NULL, " ");
            if (!lengthStr)
            {
                handler->PSendSysMessage("No check length was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            tailStr = strtok(NULL, "");
            if (!tailStr)
            {
                handler->PSendSysMessage("Provided result is malformed");
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* resultStr = handler->extractQuotedArg(tailStr);
            if (!resultStr)
            {
                handler->PSendSysMessage("No check result was provided");
                handler->SetSentErrorMessage(true);
                return false;
            }

            WardenRequestContext context;
            context.ResponseReceiverGUID = handler->GetSession()->GetPlayer()->GetGUID();

            WardenCheck check;
            check.Type = atoi(typeStr);
            check.Data.SetHexStr(dataStr);
            check.Address = (uint32)atol(addressStr);
            check.Length = (uint32)atol(lengthStr);
            check.Str = strStr;
            check.CheckId = 0;
            check.Action = WARDEN_ACTION_LOG_GM;
            check.Locale = (LocaleConstant)TOTAL_LOCALES;
            check.Negative = false;

            WardenCheckResult result;
            result.Result.SetHexStr(resultStr);

            if (check.Type == MEM_CHECK || check.Type == MODULE_CHECK)
            {
                context.CustomMemCheck = &check;
                context.CustomMemResult = &result;
            }
            else
            {
                context.CustomOtherCheck = &check;
                context.CustomOtherResult = &result;
            }

            warden->RequestData(&context);

            handler->PSendSysMessage("Sent warden check request");
            return true;
        }
        return false;
    }

    static bool HandleWardenListCommand(ChatHandler* handler, char const* /*args*/)
    {
        std::map<std::string, std::list<std::string>> output;
        for (auto&& session : sWorld->GetAllSessions())
            if (Player* player = session.second->GetPlayer())
                if (Warden* warden = session.second->GetWarden())
                    for (auto&& group : warden->GetRecordedFailedCheckGroups())
                    {
                        std::stringstream ss;
                        ss << handler->playerLink(player->GetName()) << " x " << group.second.Count << " @ " << TimeToTimestampStr(group.second.LastFailTime) << " (";
                        bool first = true;
                        for (auto&& comment : group.second.Comments)
                        {
                            ss << (first ? "" : ", ") << comment;
                            first = false;
                        }
                        ss << ")";
                        output[group.first].push_back(ss.str());
                    }

        if (output.empty())
        {
            handler->PSendSysMessage("No offenders");
            return true;
        }

        for (auto&& group : output)
        {
            handler->PSendSysMessage("[%s]:", group.first.c_str());
            for (auto&& player : group.second)
                handler->SendSysMessage(player.c_str());
        }

        return true;
    }

    static bool HandleWardenInfoEndScene(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (handler->extractPlayerTarget((char*)args, &target))
        {
            if (Warden* warden = target->GetSession()->GetWarden())
            {
                int32 addr;
                if (warden->GetEndSceneAddress(addr))
                {
                    handler->PSendSysMessage("EndScene address: %i", addr);
                    return true;
                }

                handler->SetSentErrorMessage(false);
                handler->SendSysMessage("EndScene address is not captured or target has Mac OS!");
            }
        }
        return false;
    }
};

void AddSC_warden_commandscript()
{
    new warden_commandscript();
}
