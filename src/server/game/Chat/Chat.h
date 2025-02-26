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

#ifndef TRINITYCORE_CHAT_H
#define TRINITYCORE_CHAT_H

#include "SharedDefines.h"
#include "WorldSession.h"
#include "Language.h"

#include <vector>

class ChatHandler;
class Creature;
class Group;
class Player;
class Unit;
class WorldSession;
class WorldObject;

struct GameTele;

class ChatCommand
{
        typedef bool(*pHandler)(ChatHandler*, char const*);
    public:
        ChatCommand(char const* name, AccountTypes security, bool allowConsole, pHandler handler, std::string help = "", std::vector<ChatCommand> childCommands = std::vector<ChatCommand>())
            : Name(name), SecurityLevel(security), AllowConsole(allowConsole), Handler(handler), Help(std::move(help)), ChildCommands(std::move(childCommands)) { }

        ChatCommand(char const* name, AccountTypes security, bool allowConsole, std::vector<ChatCommand> childCommands)
            : Name(name), SecurityLevel(security), AllowConsole(allowConsole), Handler(nullptr), ChildCommands(std::move(childCommands)) { }

        char const*        Name;
        uint32             SecurityLevel;                   // function pointer required correct align (use uint32)
        bool               AllowConsole;
        pHandler           Handler;
        std::string        Help;
        std::vector<ChatCommand> ChildCommands;
};

struct ChatCommandHolder;
typedef std::shared_ptr<ChatCommandHolder> CommandHolder;

class ChatHandler
{
    public:
        WorldSession* GetSession() { return m_session; }
        explicit ChatHandler(WorldSession* session) : m_session(session), sentErrorMessage(false) { }
        explicit ChatHandler(Player* player);
        virtual ~ChatHandler() { }

        // Builds chat packet and returns receiver guid position in the packet to substitute in whisper builders
        static size_t BuildChatPacket(WorldPacket& data, ChatMsg chatType, Language language, ObjectGuid senderGUID, ObjectGuid receiverGUID, std::string const& message, uint8 chatTag,
        std::string const& senderName = "", std::string const& receiverName = "",
        uint32 achievementId = 0, bool gmMessage = false, std::string const& channelName = "", std::string const& addonPrefix = "");

        // Builds chat packet and returns receiver guid position in the packet to substitute in whisper builders
        static size_t BuildChatPacket(WorldPacket& data, ChatMsg chatType, Language language, WorldObject const* sender, WorldObject const* receiver, std::string const& message, uint32 achievementId = 0, std::string const& channelName = "", LocaleConstant locale = DEFAULT_LOCALE, std::string const& addonPrefix = "");


        static char* LineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = NULL; return start; }

        // function with different implementation for chat/console
        virtual const char *GetTrinityString(int32 entry) const;
        virtual void SendSysMessage(const char *str);

        void SendSysMessage(int32     entry);
        void PSendSysMessage(const char *format, ...) ATTR_PRINTF(2, 3);
        void PSendSysMessage(int32     entry, ...);
        // Alwayls return false
        bool SendError(int32 entry, ...);
        // Alwayls return false
        bool SendError(char const* str, ...);

        std::string PGetParseString(int32 entry, ...) const;
        bool ParseCommands(const char* text);

        static std::vector<ChatCommand> const& getCommandTable();

        bool isValidChatMessage(const char* msg);
        void SendGlobalSysMessage(const char *str);

        bool hasStringAbbr(const char* name, const char* part);

        // function with different implementation for chat/console
        virtual bool isAvailable(ChatCommand const& cmd) const;
        virtual std::string GetNameLink() const { return GetNameLink(m_session->GetPlayer()); }
        virtual bool needReportToTarget(Player* chr) const;
        virtual LocaleConstant GetSessionDbcLocale() const;
        virtual int GetSessionDbLocaleIndex() const;

        bool HasLowerSecurity(Player* target, ObjectGuid guid, bool strong = false);
        bool HasLowerSecurityAccount(WorldSession* target, uint32 account, bool strong = false);

        void SendGlobalGMSysMessage(const char *str);
        Player*   getSelectedPlayer();
        Creature* getSelectedCreature();
        Unit*     getSelectedUnit();
        WorldObject* getSelectedObject();

        char*     extractKeyFromLink(char* text, char const* linkType, char** something1 = NULL);
        char*     extractKeyFromLink(char* text, char const* const* linkTypes, int* found_idx, char** something1 = NULL);

        // if args have single value then it return in arg2 and arg1 == NULL
        void      extractOptFirstArg(char* args, char** arg1, char** arg2);
        // If args have two values then it return in arg2 and arg3, arg1 will be NULL
        void      extractOptFirstArg(char* args, char** arg1, char** arg2, char** arg3);
        char*     extractQuotedArg(char* args);

        uint32    extractSpellIdFromLink(char* text);
        ObjectGuid extractGuidFromLink(char* text);
        GameTele const* extractGameTeleFromLink(char* text);
        bool GetPlayerGroupAndGUIDByName(const char* cname, Player* &player, Group* &group, ObjectGuid &guid, bool offline = false);
        std::string extractPlayerNameFromLink(char* text);
        // select by arg (name/link) or in-game selection online/offline player
        bool extractPlayerTarget(char* args, Player** player, ObjectGuid* player_guid = nullptr, std::string* player_name = NULL);

        std::string playerLink(std::string const& name) const { return m_session ? "|cffffffff|Hplayer:"+name+"|h["+name+"]|h|r" : name; }
        std::string GetNameLink(Player* chr) const;

        GameObject* GetNearbyGameObject();
        GameObject* GetObjectGlobalyWithGuidOrNearWithDbGuid(uint32 lowguid, uint32 entry);
        bool HasSentErrorMessage() const { return sentErrorMessage; }
        void SetSentErrorMessage(bool val){ sentErrorMessage = val; }
        static bool LoadCommandTable() { return load_command_table; }
        static void SetLoadCommandTable(bool val) { load_command_table = val; }

        bool ShowHelpForCommand(std::vector<ChatCommand> const& table, const char* cmd);
        void SecureLevelAnnounce(AccountTypes sec, const char* args);

        static bool ValidatePipeSequence(std::string const& text);

        // virtual CommandHolder CreateCommandHolder(TaskBase* task);

    protected:
        explicit ChatHandler() : m_session(NULL), sentErrorMessage(false) { }     // for CLI subclass
        static bool SetDataForCommandInTable(std::vector<ChatCommand>& table, const char* text, uint32 security, std::string const& help, std::string const& fullcommand);
        bool ExecuteCommandInTable(std::vector<ChatCommand> const& table, const char* text, std::string const& fullcmd);
        bool ShowHelpForSubCommands(std::vector<ChatCommand> const& table, char const* cmd, char const* subcmd);

    private:
        WorldSession* m_session;                           // != NULL for chat command call and NULL for CLI command

        // common global flag
        static bool load_command_table;
        bool sentErrorMessage = false;
};

//! Okay, class for some asynchronus command handling.
struct ChatCommandHolder : public std::enable_shared_from_this<ChatCommandHolder>
{
    explicit ChatCommandHolder(ChatHandler* handler) : m_handler(handler) { }

    ChatHandler& GetHandler() { return *m_handler; }
    virtual void FinishCommand(bool /*success*/) { }

protected:
    ChatCommandHolder() = default;

    std::unique_ptr<ChatHandler> m_handler;
};


//! Storage class for commands issued for delayed execution
struct CliCommandHolder : public ChatCommandHolder
{
    typedef void Print(void*, const char*);
    typedef void CommandFinished(void*, bool success);

    void* m_callbackArg;
    std::string m_command;
    Print* m_print;
    bool m_shared;

    CommandFinished* m_commandFinished;

    CliCommandHolder(void* callbackArg, char const* command, Print* zprint, CommandFinished* commandFinished)
        : m_callbackArg(callbackArg), m_command(command), m_print(zprint), m_shared(false), m_commandFinished(commandFinished)
    {
    }

    void Delay(ChatHandler* handler)
    {
        m_shared = true;
        m_handler.reset(handler);
    }

    void FinishCommand(bool success) override;
};

class CliHandler : public ChatHandler
{
    public:
        typedef void Print(void*, char const*);
        explicit CliHandler(CliCommandHolder* holder) :
            m_callbackArg(holder->m_callbackArg), m_print(holder->m_print), m_hptr((ChatCommandHolder*)holder), m_holder(holder) { }

        // overwrite functions
        const char *GetTrinityString(int32 entry) const;
        bool isAvailable(ChatCommand const& cmd) const;
        bool HasPermission(uint32 /*permission*/) const { return true; }
        void SendSysMessage(const char *str);
        std::string GetNameLink() const;
        bool needReportToTarget(Player* chr) const;
        LocaleConstant GetSessionDbcLocale() const;
        int GetSessionDbLocaleIndex() const;
        // CommandHolder CreateCommandHolder(TaskBase* task);
    private:
        void* m_callbackArg;
        Print* m_print;
        CommandHolder m_hptr;           // This is for storage
        CliCommandHolder* m_holder;     // and this for usage, fucking standard shared pointers.
        bool m_resp = false;
};

#endif
