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
Name: honor_commandscript
%Complete: 100
Comment: All honor related commands
Category: commandscripts
EndScriptData */

#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"

class honor_commandscript : public CommandScript
{
public:
    honor_commandscript() : CommandScript("honor_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> honorAddCommandTable =
        {
            { "kill",   &HandleHonorAddKillCommand,  rbac::RBAC_PERM_COMMAND_HONOR_ADD_KILL,  Trinity::ChatCommands::Console::No },
            { "",       &HandleHonorAddCommand,      rbac::RBAC_PERM_COMMAND_HONOR_ADD,       Trinity::ChatCommands::Console::No },
        };

        static std::vector<ChatCommand> honorCommandTable =
        {
            { "add",    honorAddCommandTable,        rbac::RBAC_PERM_COMMAND_HONOR_ADD,       Trinity::ChatCommands::Console::No },
            { "update", &HandleHonorUpdateCommand,   rbac::RBAC_PERM_COMMAND_HONOR_UPDATE,    Trinity::ChatCommands::Console::No },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "honor",  honorCommandTable,           rbac::RBAC_PERM_COMMAND_HONOR_ADD,       Trinity::ChatCommands::Console::No },
        };
        return commandTable;
    }

    static bool HandleHonorAddCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        uint32 amount = (uint32)atoi(args);
        target->RewardHonor(NULL, 1, amount);
        return true;
    }

    static bool HandleHonorAddKillCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (Player* player = target->ToPlayer())
            if (handler->HasLowerSecurity(player, ObjectGuid::Empty))
                return false;

        handler->GetSession()->GetPlayer()->RewardHonor(target, 1);
        return true;
    }

    static bool HandleHonorUpdateCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        target->UpdateHonorFields();
        return true;
    }
};

void AddSC_honor_commandscript()
{
    new honor_commandscript();
}
