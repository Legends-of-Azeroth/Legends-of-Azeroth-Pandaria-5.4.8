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
Name: gm_commandscript
%Complete: 100
Comment: All gm related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "Language.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Realm.h"

class gm_commandscript : public CommandScript
{
public:
    gm_commandscript() : CommandScript("gm_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> gmCommandTable =
        {
            { "chat",       SEC_MODERATOR,  false,  &HandleGMChatCommand,       },
            { "fly",        SEC_GAMEMASTER,  false,  &HandleGMFlyCommand,        },
            { "ingame",     SEC_PLAYER,     true,   &HandleGMListIngameCommand, },
            { "list",       SEC_PLAYER,     true,   &HandleGMListFullCommand,   },
            { "visible",    SEC_GAMEMASTER,  false,  &HandleGMVisibleCommand,    },
            { "",           SEC_GAMEMASTER,  false,  &HandleGMCommand,           },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "gm",         SEC_GAMEMASTER,  false,  gmCommandTable              },
        };
        return commandTable;
    }

    // Enables or disables hiding of the staff badge
    static bool HandleGMChatCommand(ChatHandler* handler, char const* args)
    {
        if (WorldSession* session = handler->GetSession())
        {
            if (!*args)
            {
                if (session->GetPlayer()->isGMChat())
                    session->SendNotification(LANG_GM_CHAT_ON);
                else
                    session->SendNotification(LANG_GM_CHAT_OFF);
                return true;
            }

            std::string param = (char*)args;

            if (param == "on")
            {
                session->GetPlayer()->SetGMChat(true);
                session->SendNotification(LANG_GM_CHAT_ON);
                return true;
            }

            if (param == "off")
            {
                session->GetPlayer()->SetGMChat(false);
                session->SendNotification(LANG_GM_CHAT_OFF);
                return true;
            }
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleGMFlyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target =  handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        WorldPacket data;
        if (strncmp(args, "on", 3) == 0)
            target->SetCanFly(true);
        else if (strncmp(args, "off", 4) == 0)
            target->SetCanFly(false);
        else
        {
            handler->SendSysMessage(LANG_USE_BOL);
            return false;
        }
        handler->PSendSysMessage(LANG_COMMAND_FLYMODE_STATUS, handler->GetNameLink(target).c_str(), args);
        return true;
    }

    static void SendGMInGame(ChatHandler* handler, AccountTypes sec, std::string const& name)
    {
        static std::vector<int32> const stringID =
        {
            -1, -1, -1,
            LANG_ICORE_ACC_MODERATOR,
            LANG_ICORE_ACC_BANMASTER,
            LANG_ICORE_ACC_EVENTMASTER,
            LANG_ICORE_ACC_GAMEMASTER,
            LANG_ICORE_ACC_DEVELOPER,
            LANG_ICORE_ACC_ADMINISTRATOR,
        };

        if (sec < SEC_MODERATOR || sec > SEC_ADMINISTRATOR)
            return;

        std::string type = handler->GetTrinityString(stringID[sec]);
        std::string nameLink = handler->playerLink(name);
        handler->SendSysMessage((type + nameLink).c_str());
    }

    static bool HandleGMListIngameCommand(ChatHandler* handler, char const* /*args*/)
    {
        bool first = true;

        std::shared_lock<std::shared_mutex> lock(*HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const&m = ObjectAccessor::GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
            if ((itr->second->IsGameMaster() || (itr_sec > SEC_MODERATOR && itr_sec <= AccountTypes(sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_GM_LIST)))))
            {
                if (handler->GetSession() && !itr->second->IsVisibleGloballyFor(handler->GetSession()->GetPlayer()))
                    continue;

                if (!handler->GetSession() && !itr->second->IsVisible() && itr_sec > handler->GetSession()->GetSecurity())
                    continue;

                if (first)
                {
                    handler->SendSysMessage(LANG_GMS_ON_SRV);
                    first = false;
                }

                SendGMInGame(handler, itr_sec, itr->second->GetName());
            }
        }

        if (first)
            handler->SendSysMessage(LANG_GMS_NOT_LOGGED);

        return true;
    }

    /// Display the list of GMs
    static bool HandleGMListFullCommand(ChatHandler* handler, char const* /*args*/)
    {
        ///- Get the accounts with GM Level >0
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_GM_ACCOUNTS);
        stmt->setUInt8(0, uint8(SEC_MODERATOR));
        stmt->setInt32(1, int32(realm.Id.Realm));
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
        {
            handler->SendSysMessage(LANG_GMLIST);
            handler->SendSysMessage("========================");
            ///- Cycle through them. Display username and GM level
            do
            {
                Field* fields = result->Fetch();
                char const* name = fields[0].GetCString();
                uint8 security = fields[1].GetUInt8();
                uint8 max = (16 - strlen(name)) / 2;
                uint8 max2 = max;
                if ((max + max2 + strlen(name)) == 16)
                    max2 = max - 1;
                if (handler->GetSession())
                    handler->PSendSysMessage("|    %s GMLevel %u", name, security);
                else
                    handler->PSendSysMessage("|%*s%s%*s|   %u  |", max, " ", name, max2, " ", security);
            } while (result->NextRow());
            handler->SendSysMessage("========================");
        }
        else
            handler->PSendSysMessage(LANG_GMLIST_EMPTY);
        return true;
    }

    //Enable\Disable Invisible mode
    static bool HandleGMVisibleCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            handler->PSendSysMessage(LANG_YOU_ARE, handler->GetSession()->GetPlayer()->isGMVisible() ? handler->GetTrinityString(LANG_VISIBLE) : handler->GetTrinityString(LANG_INVISIBLE));
            return true;
        }

        const uint32 VISUAL_AURA = 37800;
        std::string param = (char*)args;
        Player* player = handler->GetSession()->GetPlayer();

        if (param == "on")
        {
            if (player->HasAura(VISUAL_AURA, 0))
                player->RemoveAurasDueToSpell(VISUAL_AURA);

            player->SetGMVisible(true);
            handler->GetSession()->SendNotification(LANG_INVISIBLE_VISIBLE);
            return true;
        }

        if (param == "off")
        {
            handler->GetSession()->SendNotification(LANG_INVISIBLE_INVISIBLE);
            player->SetGMVisible(false);

            player->AddAura(VISUAL_AURA, player);

            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    //Enable\Disable GM Mode
    static bool HandleGMCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            if (handler->GetSession()->GetPlayer()->IsGameMaster())
                handler->GetSession()->SendNotification(LANG_GM_ON);
            else
                handler->GetSession()->SendNotification(LANG_GM_OFF);
            return true;
        }

        std::string param = (char*)args;

        if (param == "on")
        {
            handler->GetSession()->GetPlayer()->SetGameMaster(true);
            handler->GetSession()->SendNotification(LANG_GM_ON);
            handler->GetSession()->GetPlayer()->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("stoplog");
#endif
            return true;
        }

        if (param == "off")
        {
            handler->GetSession()->GetPlayer()->SetGameMaster(false);
            handler->GetSession()->SendNotification(LANG_GM_OFF);
            handler->GetSession()->GetPlayer()->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("startlog");
#endif
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_gm_commandscript()
{
    new gm_commandscript();
}
