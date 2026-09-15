#ifndef TRINITY_CHATCOMMAND_H
#define TRINITY_CHATCOMMAND_H

#include "Common.h"
#include "Define.h"
#include "RBAC.h"
#include "Language.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class ChatHandler;

namespace Trinity
{
namespace Impl
{
namespace ChatCommands
{
class ChatCommandNode;
}
}
}

namespace Trinity
{
namespace ChatCommands
{
    enum class Console
    {
        No,
        Yes
    };

    class CommandInvoker
    {
        std::function<bool(ChatHandler&, std::string_view)> _invoker;

    public:
        CommandInvoker() = default;
        CommandInvoker(CommandInvoker&&) = default;
        CommandInvoker(CommandInvoker const&) = default;
        CommandInvoker& operator=(CommandInvoker&&) = default;
        CommandInvoker& operator=(CommandInvoker const&) = default;

        template<typename F>
        CommandInvoker(F&& f) : _invoker(std::forward<F>(f)) { }

        bool operator()(ChatHandler& who, std::string_view args) const
        {
            return _invoker ? _invoker(who, args) : false;
        }

        explicit operator bool() const { return static_cast<bool>(_invoker); }
    };

    struct CommandPermissions
    {
        rbac::RBACPermissions RequiredPermission = static_cast<rbac::RBACPermissions>(0);
        Console AllowConsole = Console::No;
    };

    class ChatCommandBuilder;
    using ChatCommandTable = std::vector<ChatCommandBuilder>;

    class ChatCommandBuilder
    {
        friend class Trinity::Impl::ChatCommands::ChatCommandNode;

        std::string _name;
        CommandPermissions _permissions;
        CommandInvoker _invoker;
        std::variant<std::monostate, TrinityStrings, std::string> _help;
        ChatCommandTable _childCommands;

        void SetRbacPermission(rbac::RBACPermissions permission, Console allowConsole)
        {
            _permissions.RequiredPermission = permission;
            _permissions.AllowConsole = allowConsole;
        }

        void SetInvokerFromLegacyHandler(bool (*handler)(ChatHandler*, char const*))
        {
            if (!handler)
                return;

            _invoker = [handler](ChatHandler& who, std::string_view args)
            {
                std::string argsCopy(args);
                return handler(&who, argsCopy.c_str());
            };
        }

        void SetHelp(std::string const& help)
        {
            if (!help.empty())
                _help = help;
        }

        void SetHelp(TrinityStrings help)
        {
            _help = help;
        }

    public:
        ChatCommandBuilder() = default;
        ChatCommandBuilder(ChatCommandBuilder&&) = default;
        ChatCommandBuilder(ChatCommandBuilder const&) = default;
        ChatCommandBuilder& operator=(ChatCommandBuilder&&) = default;
        ChatCommandBuilder& operator=(ChatCommandBuilder const&) = default;

        ChatCommandBuilder(char const* name, bool (*handler)(ChatHandler*, char const*), rbac::RBACPermissions permission, Console allowConsole)
        {
            _name = name ? name : "";
            SetRbacPermission(permission, allowConsole);
            SetInvokerFromLegacyHandler(handler);
        }

        ChatCommandBuilder(char const* name, bool (*handler)(ChatHandler*, char const*), rbac::RBACPermissions permission, Console allowConsole, std::string const& help)
        {
            _name = name ? name : "";
            SetRbacPermission(permission, allowConsole);
            SetInvokerFromLegacyHandler(handler);
            SetHelp(help);
        }

        ChatCommandBuilder(char const* name, bool (*handler)(ChatHandler*, char const*), rbac::RBACPermissions permission, Console allowConsole, TrinityStrings help)
        {
            _name = name ? name : "";
            SetRbacPermission(permission, allowConsole);
            SetInvokerFromLegacyHandler(handler);
            SetHelp(help);
        }

        ChatCommandBuilder(char const* name, ChatCommandTable const& childCommands, rbac::RBACPermissions permission, Console allowConsole)
        {
            _name = name ? name : "";
            SetRbacPermission(permission, allowConsole);
            _childCommands = childCommands;
        }

        ChatCommandBuilder(char const* name, ChatCommandTable const& childCommands, rbac::RBACPermissions permission, Console allowConsole, std::string const& help)
        {
            _name = name ? name : "";
            SetRbacPermission(permission, allowConsole);
            SetHelp(help);
            _childCommands = childCommands;
        }

        std::string const& GetName() const { return _name; }
    };
}
}

using ChatCommand = Trinity::ChatCommands::ChatCommandBuilder;
using ChatCommandTable = Trinity::ChatCommands::ChatCommandTable;

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

    struct StringCompareLessI
    {
        bool operator()(std::string const& lhs, std::string const& rhs) const
        {
            size_t len = std::min(lhs.size(), rhs.size());
            for (size_t i = 0; i < len; ++i)
            {
                int l = std::tolower(static_cast<unsigned char>(lhs[i]));
                int r = std::tolower(static_cast<unsigned char>(rhs[i]));
                if (l != r)
                    return l < r;
            }
            return lhs.size() < rhs.size();
        }
    };

    class ChatCommandNode;
    using CommandMap = std::map<std::string, ChatCommandNode, StringCompareLessI>;

    class ChatCommandNode
    {
        friend class Trinity::ChatCommands::ChatCommandBuilder;

        std::string _name;
        CommandPermissions _permissions;
        CommandInvoker _invoker;
        std::variant<std::monostate, TrinityStrings, std::string> _help;
        CommandMap _subCommands;

        static void LoadFromBuilder(ChatCommandNode& node, ChatCommandBuilder const& builder);
        static void MergeEmptyChild(ChatCommandNode& node, ChatCommandBuilder const& child);
        static bool IsInvokerVisible(ChatCommandNode const& node, ChatHandler const& who);
        static bool IsNodeVisible(ChatCommandNode const& node, ChatHandler const& who);
        static ChatCommandNode const* FindExact(CommandMap const& map, std::string_view token);
        static std::vector<ChatCommandNode const*> FindVisibleMatches(CommandMap const& map, std::string_view token, ChatHandler const& who);
        static bool ExecuteNode(ChatCommandNode const& node, ChatHandler& who, std::string_view args, std::string_view fullCommand);
        static void LogCommand(ChatHandler& who, std::string_view fullCommand);
        static bool SendHelpString(ChatHandler& who, ChatCommandNode const& node);
        static bool SendTopLevelHelp(ChatHandler& who, std::string_view filter);
        static bool SendSubCommandHelp(ChatHandler& who, ChatCommandNode const& node, std::string_view filter);
        static void SendNoSubCommand(ChatHandler& who, ChatCommandNode const& node, bool hasArgs);
        static void SendAmbiguous(ChatHandler& who, std::string_view token, std::vector<ChatCommandNode const*> const& matches);

    public:
        ChatCommandNode() = default;
        ChatCommandNode(ChatCommandNode&&) = default;
        ChatCommandNode(ChatCommandNode const&) = default;
        ChatCommandNode& operator=(ChatCommandNode&&) = default;
        ChatCommandNode& operator=(ChatCommandNode const&) = default;

        std::string const& GetName() const { return _name; }
        CommandMap const& GetSubCommands() const { return _subCommands; }
        CommandMap& GetSubCommands() { return _subCommands; }

        void SetHelpFromDatabase(std::string const& help)
        {
            if (std::holds_alternative<std::monostate>(_help) || std::holds_alternative<std::string>(_help))
                _help = help;
        }

        static void LoadCommandMap(CommandMap& map, ChatCommandBuilder const& builder);
        static void LoadCommandMap(CommandMap& map, ChatCommandTable const& commands);
        static bool TryExecuteCommand(ChatHandler& who, std::string_view command, std::string_view fullCommand);
        static bool SendCommandHelpFor(ChatHandler& who, std::string_view name);
        static std::vector<std::string> GetAutoCompletionsFor(ChatHandler const& who, std::string_view text);
    };

    CommandMap& GetCommandMap();
    CommandMap const& GetTopLevelMap();
}
}
}

namespace Trinity
{
namespace ChatCommands
{
    inline bool TryExecuteCommand(ChatHandler& who, std::string_view command)
    {
        return Impl::ChatCommands::ChatCommandNode::TryExecuteCommand(who, command, command);
    }

    inline bool TryExecuteCommand(ChatHandler& who, std::string_view command, std::string_view fullCommand)
    {
        return Impl::ChatCommands::ChatCommandNode::TryExecuteCommand(who, command, fullCommand);
    }

    inline bool SendCommandHelpFor(ChatHandler& who, std::string_view name)
    {
        return Impl::ChatCommands::ChatCommandNode::SendCommandHelpFor(who, name);
    }

    inline std::vector<std::string> GetAutoCompletionsFor(ChatHandler const& who, std::string_view text)
    {
        return Impl::ChatCommands::ChatCommandNode::GetAutoCompletionsFor(who, text);
    }
}
}

#endif