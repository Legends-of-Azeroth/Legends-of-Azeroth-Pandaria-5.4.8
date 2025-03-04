/*
* This file is part of the Legends of Azeroth Pandria Project. See THANKS file for Copyright information
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
Name: achievement_commandscript
%Complete: 100
Comment: All achievement related commands
Category: commandscripts
EndScriptData */

#include "AchievementMgr.h"
#include "Guild.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"

class achievement_commandscript : public CommandScript
{
public:
    achievement_commandscript() : CommandScript("achievement_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> infoCommandTable =
        {
            { "criteria",       SEC_ADMINISTRATOR,      true,  &HandleAchievementCriteriaInfoCommand,   },
            { "modifier",       SEC_ADMINISTRATOR,      true,  &HandleAchievementModifierInfoCommand,   },
            { "",               SEC_ADMINISTRATOR,      true,  &HandleAchievementInfoCommand,           },
        };

        static std::vector<ChatCommand> achievementCommandTable =
        {
            { "add",            SEC_ADMINISTRATOR,  false, &HandleAchievementAddCommand,            },
            { "remove",         SEC_ADMINISTRATOR,  false, &HandleAchievementRemoveCommand,         },
            { "info",           SEC_ADMINISTRATOR,      true,  infoCommandTable                         },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "achievement",    SEC_ADMINISTRATOR,  false, achievementCommandTable                  },
            
        };
        return commandTable;
    }

    static AchievementEntry const* ExtractAchievement(ChatHandler* handler, std::string achstr)
    {
        uint32 achievementId = ::strtoul(achstr.c_str(), nullptr, 10);
        if (!achievementId)
            if (char* id = handler->extractKeyFromLink((char*)achstr.c_str(), "Hachievement"))
                achievementId = atoi(id);
        return achievementId ? sAchievementMgr->GetAchievement(achievementId) : nullptr;
    }

    static bool HandleAchievementAddCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        auto achievementEntry = ExtractAchievement(handler, args);
        if (!achievementEntry)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
            return handler->SendError(LANG_NO_CHAR_SELECTED);

        if (achievementEntry->Flags & ACHIEVEMENT_FLAG_GUILD)
        {
            if (Guild* guild = target->GetGuild())
                guild->GetAchievementMgr().CompletedAchievement(achievementEntry, target);
        }
        else
            target->CompletedAchievement(achievementEntry);

        return true;
    }

    static bool HandleAchievementRemoveCommand(ChatHandler* handler, char const* args)
    {
        auto achievement = ExtractAchievement(handler, args);
        if (!achievement)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
            return handler->SendError(LANG_NO_CHAR_SELECTED);

        if (achievement->Flags & ACHIEVEMENT_FLAG_GUILD)
        {
            if (Guild* guild = target->GetGuild())
                guild->GetAchievementMgr().RemoveAchievement(achievement);
        }
        else
            target->RemoveAchievement(achievement);
        return true;
    }

    static void PrintCriteriaTree(ChatHandler* handler, CriteriaTreeNode const* tree)
    {
        std::function<void(CriteriaTreeNode const*)> walk = [&](CriteriaTreeNode const* tree)
        {
            auto treeEntry = tree->Entry;
            handler->PSendSysMessage("|cffffffffNode %u: Parent: %u, RequieredCount: %u, Operator: %u|r", treeEntry->ID, treeEntry->Parent, treeEntry->RequieredCount, treeEntry->Operator);
            if (tree->Criteria)
            {
                auto entry = tree->Criteria->Entry;
                handler->PSendSysMessage("|cff40FB40Criteria: %u, Type: %u (%s), Raw: %u, Modifier: %u|r", entry->ID, entry->Type, AchievementGlobalMgr::GetCriteriaTypeString(entry->Type), entry->Asset.Raw, entry->ModifierTree);
            }
            for (auto&& node : tree->Nodes)
                walk(node);
        };
        walk(tree);
    }

    static bool HandleAchievementInfoCommand(ChatHandler* handler, char const* args)
    {
        auto achievement = ExtractAchievement(handler, args);
        if (!achievement)
            return handler->SendError("Achievement not found!");

        handler->SendSysMessage(achievement->Name[handler->GetSessionDbcLocale()]);
        handler->PSendSysMessage("Achievement ID: %u", achievement->ID);
        if (achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT)
            handler->SendSysMessage("Account wide");
        else if (achievement->Flags & ACHIEVEMENT_FLAG_GUILD)
            handler->SendSysMessage("Guild achievement");
        handler->SendSysMessage("CriteriaTree:");
        if (achievement->Flags & ACHIEVEMENT_FLAG_HIDDEN)
            handler->SendSysMessage("Hidden");
        if (achievement->Flags & ACHIEVEMENT_FLAG_COUNTER)
            handler->SendSysMessage("Counter");
        handler->PSendSysMessage("CriteriaTree ID: %u", achievement->CriteriaTree);

        if (auto tree = sAchievementMgr->GetCriteriaTree(achievement->CriteriaTree))
            PrintCriteriaTree(handler, tree);
        return true;
    }

    static bool HandleAchievementCriteriaInfoCommand(ChatHandler* handler, char const* args)
    {
        uint32 id = ::strtoul(args, nullptr, 10);
        if (!id)
            return handler->SendError("Criteria not found");

        if (auto list = sAchievementMgr->GetCriteriaTreesByCriteria(id))
        {
            handler->PSendSysMessage("Criterid %u is contained in %u trees", id, uint32(list->size()));
            for (auto&& tree : *list)
            {
                uint32 achievement = tree->Achievement ? tree->Achievement->ID : 0;
                char const* name = tree->Achievement ? tree->Achievement->Name : "not found";
                handler->PSendSysMessage("CriteriaTree: %u (Achievement: %s (%u))", tree->Entry->ID, name, achievement);
                PrintCriteriaTree(handler, tree);
            }
        }
        return true;
    }

    static bool HandleAchievementModifierInfoCommand(ChatHandler* handler, char const* args)
    {

        uint32 id = ::strtoul(args, nullptr, 10);

        auto tree = sAchievementMgr->GetCriteriaModifierTree(id);
        if (!tree)
            return handler->SendError("ModifierTreeNode not found");

        handler->PSendSysMessage("Looking for modifier %u", id);

        auto root = tree;
        while (root->Entry->Parent)
            root = sAchievementMgr->GetCriteriaModifierTree(root->Entry->Parent);

        if (auto const* criteria = sAchievementMgr->GetCriteriaByModifierTree(root->Entry->ID))
            handler->PSendSysMessage("Root node: %u, linked to criteria: %u", root->Entry->ID, criteria->Entry->ID);
        else
            handler->PSendSysMessage("Root node: %u, linked to criteria: not found", root->Entry->ID);

        std::function<void(ModifierTreeNode const* node)> walk = [&](ModifierTreeNode const* node)
        {
            auto entry = node->Entry;
            handler->PSendSysMessage("ID: %u, Parent: %u, Condition: %u, Condition Value: %u, Operator: %u", entry->ID, entry->Parent, entry->ConditionType, entry->ConditionValue[0], entry->Operator);

            for (auto&& it : node->Nodes)
                walk(it);
        };
        walk(root);
        return true;
    }
};

void AddSC_achievement_commandscript()
{
    new achievement_commandscript();
}
