#include "ChatCommand.h"
#include "Chat.h"
#include "ScriptMgr.h"
#include "DatabaseEnv.h"
#include "AccountMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Log.h"
#include "Language.h"

#include <cctype>
#include <cstring>

namespace
{
    std::vector<std::string> SplitBySpace(std::string_view str)
    {
        std::vector<std::string> tokens;
        size_t start = 0;
        while (start < str.size())
        {
            while (start < str.size() && str[start] == ' ')
                ++start;
            if (start == str.size())
                break;
            size_t end = start;
            while (end < str.size() && str[end] != ' ')
                ++end;
            tokens.emplace_back(str.substr(start, end - start));
            start = end;
        }
        return tokens;
    }

    std::string_view SkipSpaces(std::string_view str)
    {
        size_t i = 0;
        while (i < str.size() && str[i] == ' ')
            ++i;
        return str.substr(i);
    }

    bool StringStartsWithI(std::string_view str, std::string_view prefix)
    {
        if (prefix.size() > str.size())
            return false;
        for (size_t i = 0; i < prefix.size(); ++i)
            if (std::tolower(static_cast<unsigned char>(str[i])) != std::tolower(static_cast<unsigned char>(prefix[i])))
                return false;
        return true;
    }

    void AppendCommandToList(ChatHandler const& who, std::string& list, std::string_view name, bool hasSubCommands)
    {
        list += who.IsConsole() ? "\n\r    " : "\n    ";
        list += name;
        if (hasSubCommands)
            list += " ...";
    }
}

namespace Trinity
{
namespace Impl
{
namespace ChatCommands
{
    using Trinity::ChatCommands::ChatCommandBuilder;
    using Trinity::ChatCommands::ChatCommandTable;
    using Trinity::ChatCommands::CommandInvoker;
    using Trinity::ChatCommands::CommandPermissions;
    using Trinity::ChatCommands::Console;

    static void LoadCommandMapFromDatabase(CommandMap& map)
    {
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_COMMANDS);
        PreparedQueryResult result = WorldDatabase.Query(stmt);
        if (!result)
            return;

        do
        {
            Field* fields = result->Fetch();
            std::string name = fields[0].GetString();
            std::string help = fields[2].GetString();

            std::vector<std::string> tokens = SplitBySpace(name);
            if (tokens.empty())
                continue;

            CommandMap* current = &map;
            bool found = true;
            for (size_t i = 0; i < tokens.size(); ++i)
            {
                auto it = current->find(tokens[i]);
                if (it == current->end())
                {
                    TC_LOG_ERROR("sql.sql", "Table `command` have not existed command '%s', skip.", name.c_str());
                    found = false;
                    break;
                }

                if (i == tokens.size() - 1)
                {
                    it->second.SetHelpFromDatabase(help);
                }

                current = &it->second.GetSubCommands();
            }

            (void)found;
        } while (result->NextRow());
    }

    CommandMap& GetCommandMap()
    {
        static CommandMap commandMap;

        if (ChatHandler::LoadCommandTable())
        {
            commandMap.clear();
            ChatCommandNode::LoadCommandMap(commandMap, sScriptMgr->GetChatCommands());
            LoadCommandMapFromDatabase(commandMap);
            ChatHandler::SetLoadCommandTable(false);
        }

        return commandMap;
    }

    CommandMap const& GetTopLevelMap()
    {
        return GetCommandMap();
    }

    void ChatCommandNode::LoadCommandMap(CommandMap& map, ChatCommandBuilder const& builder)
    {
        std::vector<std::string> tokens = SplitBySpace(builder._name);
        if (tokens.empty())
            return;

        CommandMap* current = &map;
        std::string currentName;
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (!currentName.empty())
                currentName += ' ';
            currentName += tokens[i];

            auto& node = (*current)[tokens[i]];
            if (node._name.empty())
                node._name = currentName;

            if (i == tokens.size() - 1)
                LoadFromBuilder(node, builder);

            current = &node._subCommands;
        }
    }

    void ChatCommandNode::LoadCommandMap(CommandMap& map, ChatCommandTable const& commands)
    {
        for (auto const& command : commands)
            LoadCommandMap(map, command);
    }

    void ChatCommandNode::LoadFromBuilder(ChatCommandNode& node, ChatCommandBuilder const& builder)
    {
        if (builder._invoker && !node._invoker)
        {
            node._invoker = builder._invoker;
            node._permissions = builder._permissions;
        }
        else if (!node._invoker)
            node._permissions = builder._permissions;

        if (std::holds_alternative<std::monostate>(node._help) && !std::holds_alternative<std::monostate>(builder._help))
            node._help = builder._help;

        for (auto const& child : builder._childCommands)
        {
            if (child._name.empty())
                MergeEmptyChild(node, child);
            else
                LoadCommandMap(node._subCommands, child);
        }
    }

    void ChatCommandNode::MergeEmptyChild(ChatCommandNode& node, ChatCommandBuilder const& child)
    {
        if (child._invoker && !node._invoker)
        {
            node._invoker = child._invoker;
            node._permissions = child._permissions;
        }
        else if (!node._invoker)
            node._permissions = child._permissions;

        if (std::holds_alternative<std::monostate>(node._help) && !std::holds_alternative<std::monostate>(child._help))
            node._help = child._help;

        for (auto const& sub : child._childCommands)
        {
            if (sub._name.empty())
                MergeEmptyChild(node, sub);
            else
                LoadCommandMap(node._subCommands, sub);
        }
    }

    bool ChatCommandNode::IsInvokerVisible(ChatCommandNode const& node, ChatHandler const& who)
    {
        if (!node._invoker)
            return false;

        if (who.IsConsole() && node._permissions.AllowConsole == Console::No)
            return false;

        return who.HasPermission(static_cast<uint32>(node._permissions.RequiredPermission));
    }

    bool ChatCommandNode::IsNodeVisible(ChatCommandNode const& node, ChatHandler const& who)
    {
        if (IsInvokerVisible(node, who))
            return true;

        for (auto const& pair : node._subCommands)
            if (IsNodeVisible(pair.second, who))
                return true;

        return false;
    }

    ChatCommandNode const* ChatCommandNode::FindExact(CommandMap const& map, std::string_view token)
    {
        auto it = map.find(std::string(token));
        return it != map.end() ? &it->second : nullptr;
    }

    std::vector<ChatCommandNode const*> ChatCommandNode::FindVisibleMatches(CommandMap const& map, std::string_view token, ChatHandler const& who)
    {
        std::vector<ChatCommandNode const*> matches;
        if (token.empty())
            return matches;

        ChatCommandNode const* exact = FindExact(map, token);
        if (exact && IsNodeVisible(*exact, who))
            matches.push_back(exact);
        else
        {
            for (auto const& pair : map)
                if (StringStartsWithI(pair.first, token) && IsNodeVisible(pair.second, who))
                    matches.push_back(&pair.second);
        }

        return matches;
    }

    bool ChatCommandNode::ExecuteNode(ChatCommandNode const& node, ChatHandler& who, std::string_view args, std::string_view fullCommand)
    {
        who.SetSentErrorMessage(false);
        bool success = node._invoker(who, args);

        if (success)
            LogCommand(who, fullCommand);
        else if (!who.HasSentErrorMessage())
        {
            if (!SendHelpString(who, node))
                who.SendSysMessage(LANG_CMD_SYNTAX);
        }

        return true;
    }

    void ChatCommandNode::LogCommand(ChatHandler& who, std::string_view fullCommand)
    {
        WorldSession* session = who.GetSession();
        if (!session || AccountMgr::IsPlayerAccount(session->GetSecurity()))
            return;

        Player* player = session->GetPlayer();
        if (!player)
            return;

        ObjectGuid guid = player->GetTarget();
        uint32 areaId = player->GetAreaId();
        std::string areaName = "Unknown";
        std::string zoneName = "Unknown";
        if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaId))
        {
            areaName = area->area_name[sObjectMgr->GetDBCLocaleIndex()];
            if (AreaTableEntry const* zone = sAreaTableStore.LookupEntry(area->ParentAreaID))
                zoneName = zone->area_name[sObjectMgr->GetDBCLocaleIndex()];
        }

        std::string fullCmd(fullCommand);
        sLog->OutCommand(session->GetAccountId(), "Command: {} [Player: {} (Guid: {}) (Account: {}) X: {} Y: {} Z: {} Map: {} ({}) Area: {} ({}) Zone: {} Selected {}: {} (GUID: {})]",
            fullCmd.c_str(), player->GetName().c_str(), player->GetGUID().GetCounter(),
            session->GetAccountId(), player->GetPositionX(), player->GetPositionY(),
            player->GetPositionZ(), player->GetMapId(),
            player->GetMap() ? player->GetMap()->GetMapName() : "Unknown",
            areaId, areaName.c_str(), zoneName.c_str(), guid.GetTypeName(),
            (player->GetSelectedUnit()) ? player->GetSelectedUnit()->GetName().c_str() : "",
            guid.GetCounter());
    }

    bool ChatCommandNode::SendHelpString(ChatHandler& who, ChatCommandNode const& node)
    {
        if (std::holds_alternative<std::string>(node._help))
        {
            std::string const& help = std::get<std::string>(node._help);
            if (!help.empty())
            {
                who.SendSysMessage(help.c_str());
                return true;
            }
        }
        else if (std::holds_alternative<TrinityStrings>(node._help))
        {
            who.SendSysMessage(static_cast<int32>(std::get<TrinityStrings>(node._help)));
            return true;
        }

        return false;
    }

    bool ChatCommandNode::SendTopLevelHelp(ChatHandler& who, std::string_view filter)
    {
        std::string list;
        CommandMap const& map = GetTopLevelMap();
        for (auto const& pair : map)
            if (IsNodeVisible(pair.second, who) && (filter.empty() || StringStartsWithI(pair.first, filter)))
                AppendCommandToList(who, list, pair.first, !pair.second._subCommands.empty());

        if (list.empty())
            return false;

        who.PSendSysMessage(LANG_AVIABLE_CMD);
        who.PSendSysMessage("%s", list.c_str());
        return true;
    }

    bool ChatCommandNode::SendSubCommandHelp(ChatHandler& who, ChatCommandNode const& node, std::string_view filter)
    {
        std::string list;
        for (auto const& pair : node._subCommands)
            if (IsNodeVisible(pair.second, who) && (filter.empty() || StringStartsWithI(pair.first, filter)))
                AppendCommandToList(who, list, pair.first, !pair.second._subCommands.empty());

        if (list.empty())
            return false;

        who.PSendSysMessage(LANG_SUBCMDS_LIST, node.GetName().c_str(), list.c_str());
        return true;
    }

    void ChatCommandNode::SendNoSubCommand(ChatHandler& who, ChatCommandNode const& node, bool hasArgs)
    {
        who.SendSysMessage(hasArgs ? LANG_NO_SUBCMD : LANG_CMD_SYNTAX);
        SendSubCommandHelp(who, node, "");
    }

    void ChatCommandNode::SendAmbiguous(ChatHandler& who, std::string_view token, std::vector<ChatCommandNode const*> const& matches)
    {
        std::string tokenStr(token);
        who.PSendSysMessage("Ambiguous command: %s", tokenStr.c_str());

        std::string list;
        for (auto const* node : matches)
        {
            list += who.IsConsole() ? "\n\r    " : "\n    ";
            list += node->GetName();
        }

        who.PSendSysMessage("%s", list.c_str());
    }

    bool ChatCommandNode::TryExecuteCommand(ChatHandler& who, std::string_view command, std::string_view fullCommand)
    {
        std::string_view rest = SkipSpaces(command);
        if (rest.empty())
            return false;

        size_t space = rest.find(' ');
        std::string_view token = rest.substr(0, space);
        std::string_view after = (space == std::string_view::npos) ? std::string_view() : rest.substr(space + 1);

        CommandMap const& top = GetTopLevelMap();
        std::vector<ChatCommandNode const*> topMatches = FindVisibleMatches(top, token, who);
        if (topMatches.empty())
            return false;

        if (topMatches.size() > 1)
        {
            SendAmbiguous(who, token, topMatches);
            return true;
        }

        ChatCommandNode const* node = topMatches[0];
        rest = after;

        while (true)
        {
            rest = SkipSpaces(rest);
            if (rest.empty())
            {
                if (IsInvokerVisible(*node, who))
                    return ExecuteNode(*node, who, "", fullCommand);

                if (!node->_subCommands.empty())
                {
                    SendNoSubCommand(who, *node, false);
                    return true;
                }

                return false;
            }

            space = rest.find(' ');
            token = rest.substr(0, space);
            after = (space == std::string_view::npos) ? std::string_view() : rest.substr(space + 1);

            std::vector<ChatCommandNode const*> matches = FindVisibleMatches(node->_subCommands, token, who);
            if (!matches.empty())
            {
                if (matches.size() > 1)
                {
                    SendAmbiguous(who, token, matches);
                    return true;
                }

                node = matches[0];
                rest = after;
                continue;
            }

            if (IsInvokerVisible(*node, who))
                return ExecuteNode(*node, who, rest, fullCommand);

            if (!node->_subCommands.empty())
            {
                SendNoSubCommand(who, *node, true);
                return true;
            }

            return false;
        }
    }

    bool ChatCommandNode::SendCommandHelpFor(ChatHandler& who, std::string_view name)
    {
        if (name.empty())
            return SendTopLevelHelp(who, "");

        std::vector<std::string> tokens = SplitBySpace(name);
        if (tokens.empty())
            return SendTopLevelHelp(who, "");

        CommandMap const* map = &GetTopLevelMap();
        ChatCommandNode const* parent = nullptr;

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            ChatCommandNode const* node = FindExact(*map, tokens[i]);
            if (!node)
            {
                if (parent)
                    return SendSubCommandHelp(who, *parent, tokens[i]);
                return false;
            }

            if (i == tokens.size() - 1)
            {
                bool sent = SendHelpString(who, *node);
                if (!node->_subCommands.empty())
                    sent = SendSubCommandHelp(who, *node, "") || sent;
                return sent;
            }

            parent = node;
            map = &node->_subCommands;
        }

        return false;
    }

    std::vector<std::string> ChatCommandNode::GetAutoCompletionsFor(ChatHandler const& who, std::string_view text)
    {
        std::vector<std::string> completions;
        size_t lastSpace = text.find_last_of(' ');
        std::string_view previous;
        std::string_view partial;

        if (lastSpace == std::string_view::npos)
        {
            previous = std::string_view();
            partial = text;
        }
        else
        {
            previous = text.substr(0, lastSpace);
            partial = text.substr(lastSpace + 1);
        }

        CommandMap const* map = &GetTopLevelMap();
        std::string prefix;
        std::vector<std::string> previousTokens = SplitBySpace(previous);

        for (auto const& token : previousTokens)
        {
            ChatCommandNode const* node = FindExact(*map, token);
            if (!node)
                return completions;

            if (!prefix.empty())
                prefix += ' ';
            prefix += node->GetName();
            prefix += ' ';
            map = &node->GetSubCommands();
        }

        for (auto const& pair : *map)
            if (IsNodeVisible(pair.second, who) && (partial.empty() || StringStartsWithI(pair.first, partial)))
                completions.push_back(prefix + pair.first);

        return completions;
    }
}
}
}