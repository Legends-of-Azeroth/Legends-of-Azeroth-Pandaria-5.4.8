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

/* ScriptData
Name: debug_commandscript
%Complete: 100
Comment: All debug related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Chat.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GossipDef.h"
#include "Transport.h"
#include "Language.h"

#include <fstream>

class debug_commandscript : public CommandScript
{
public:
    debug_commandscript() : CommandScript("debug_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> debugPlayCommandTable =
        {
            { "cinematic",      SEC_ADMINISTRATOR,  false,  &HandleDebugPlayCinematicCommand,       },
            { "movie",          SEC_ADMINISTRATOR,  false,  &HandleDebugPlayMovieCommand,           },
            { "sound",          SEC_ADMINISTRATOR,  false,  &HandleDebugPlaySoundCommand,           },
            { "visual",         SEC_ADMINISTRATOR,  false,  &HandleDebugPlayVisualCommand,          },
            { "visualkit",      SEC_ADMINISTRATOR,  false,  &HandleDebugPlayVisualKitCommand        },
        };
        static std::vector<ChatCommand> debugSendCommandTable =
        {
            { "buyerror",       SEC_ADMINISTRATOR,  false,  &HandleDebugSendBuyErrorCommand,        },
            { "channelnotify",  SEC_ADMINISTRATOR,  false,  &HandleDebugSendChannelNotifyCommand,   },
            { "chatmessage",    SEC_ADMINISTRATOR,  false,  &HandleDebugSendChatMsgCommand,         },
            { "equiperror",     SEC_ADMINISTRATOR,  false,  &HandleDebugSendEquipErrorCommand,      },
            { "largepacket",    SEC_ADMINISTRATOR,  false,  &HandleDebugSendLargePacketCommand,     },
            { "opcode",         SEC_ADMINISTRATOR,  false,  &HandleDebugSendOpcodeCommand,          },
            { "qpartymsg",      SEC_ADMINISTRATOR,  false,  &HandleDebugSendQuestPartyMsgCommand,   },
            { "qinvalidmsg",    SEC_ADMINISTRATOR,  false,  &HandleDebugSendQuestInvalidMsgCommand, },
            { "sellerror",      SEC_ADMINISTRATOR,  false,  &HandleDebugSendSellErrorCommand,       },
            { "setphaseshift",  SEC_ADMINISTRATOR,  false,  &HandleDebugSendSetPhaseShiftCommand,   },
            { "spellfail",      SEC_ADMINISTRATOR,  false,  &HandleDebugSendSpellFailCommand,       },
        };
        static std::vector<ChatCommand> debugCommandTable =
        {
            { "setbit",         SEC_ADMINISTRATOR,  false,  &HandleDebugSet32BitCommand,            },
            { "threat",         SEC_ADMINISTRATOR,  false,  &HandleDebugThreatListCommand,          },
            { "hostil",         SEC_ADMINISTRATOR,  false,  &HandleDebugHostileRefListCommand,      },
            { "anim",           SEC_ADMINISTRATOR,  false,  &HandleDebugAnimCommand,                },
            { "arena",          SEC_ADMINISTRATOR,  false,  &HandleDebugArenaCommand,               },
            { "bg",             SEC_ADMINISTRATOR,  false,  &HandleDebugBattlegroundCommand,        },
            { "ratedbg",        SEC_ADMINISTRATOR,  false,  &HandleDebugRatedBgCommand              },
            { "getitemstate",   SEC_ADMINISTRATOR,  false,  &HandleDebugGetItemStateCommand,        },
            { "lootrecipient",  SEC_ADMINISTRATOR,  false,  &HandleDebugGetLootRecipientCommand,    },
            { "getvalue",       SEC_ADMINISTRATOR,  false,  &HandleDebugGetValueCommand,            },
            { "getitemvalue",   SEC_ADMINISTRATOR,  false,  &HandleDebugGetItemValueCommand,        },
            { "Mod32Value",     SEC_ADMINISTRATOR,  false,  &HandleDebugMod32ValueCommand,          },
            { "play",           SEC_ADMINISTRATOR,  false,  debugPlayCommandTable                   },
            { "send",           SEC_ADMINISTRATOR,  false,  debugSendCommandTable                   },
            { "setaurastate",   SEC_ADMINISTRATOR,  false,  &HandleDebugSetAuraStateCommand,        },
            { "setitemvalue",   SEC_ADMINISTRATOR,  false,  &HandleDebugSetItemValueCommand,        },
            { "setvalue",       SEC_ADMINISTRATOR,  false,  &HandleDebugSetValueCommand,            },
            { "spawnvehicle",   SEC_ADMINISTRATOR,  false,  &HandleDebugSpawnVehicleCommand,        },
            { "setvid",         SEC_ADMINISTRATOR,  false,  &HandleDebugSetVehicleIdCommand,        },
            { "entervehicle",   SEC_ADMINISTRATOR,  false,  &HandleDebugEnterVehicleCommand,        },
            { "uws",            SEC_ADMINISTRATOR,  false,  &HandleDebugUpdateWorldStateCommand,    },
            { "update",         SEC_ADMINISTRATOR,  false,  &HandleDebugUpdateCommand,              },
            { "itemexpire",     SEC_ADMINISTRATOR,  false,  &HandleDebugItemExpireCommand,          },
            { "areatriggers",   SEC_ADMINISTRATOR,  false,  &HandleDebugAreaTriggersCommand,        },
            { "los",            SEC_ADMINISTRATOR,  false,  &HandleDebugLoSCommand,                 },
            { "moveflags",      SEC_ADMINISTRATOR,  false,  &HandleDebugMoveflagsCommand,           },
            { "transport",      SEC_ADMINISTRATOR,  false,  &HandleDebugTransportCommand,           },
            { "phase",          SEC_ADMINISTRATOR,  false,  &HandleDebugPhaseCommand,               },
            { "casterror",      SEC_ADMINISTRATOR,  false,  &HandleDebugCastErrorCommand,           },
            { "set",            SEC_ADMINISTRATOR,  false,  {
                { "ap",         SEC_ADMINISTRATOR,  false,  &HandleDebugSetAttackPower              },
                { "sp",         SEC_ADMINISTRATOR,  false,  &HandleDebugSetSpellPower               },
            } },
            { "archaeology",    SEC_ADMINISTRATOR,  false,  {
                { "find",       SEC_ADMINISTRATOR,  false,  &HandleDebugArchaeologyFind             },
                { "digsite",    SEC_ADMINISTRATOR,  false,  &HandleDebugArchaeologyDigsite          },
            } },
            { "vignette",       SEC_ADMINISTRATOR,  false,  &HandleDebugVignette,                   },
            { "value",          SEC_ADMINISTRATOR,  true,   &HandleDebugValueCommand,               },
            { "",               SEC_ADMINISTRATOR,  true,   &HandleDebugValueCommand,               },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "debug",          SEC_ADMINISTRATOR,  true,   debugCommandTable                       },
            { "wpgps",          SEC_ADMINISTRATOR,  false,  &HandleWPGPSCommand,                    },
        };
        return commandTable;
    }

    static bool HandleDebugPlayCinematicCommand(ChatHandler* handler, char const* args)
    {
        // USAGE: .debug play cinematic #cinematicid
        // #cinematicid - ID decimal number from CinemaicSequences.dbc (1st column)
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 id = atoi((char*)args);

        if (!sCinematicSequencesStore.LookupEntry(id))
        {
            handler->PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->GetPlayer()->SendCinematicStart(id);
        return true;
    }

    static bool HandleDebugPlayMovieCommand(ChatHandler* handler, char const* args)
    {
        // USAGE: .debug play movie #movieid
        // #movieid - ID decimal number from Movie.dbc (1st column)
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 id = atoi((char*)args);

        if (!sMovieStore.LookupEntry(id))
        {
            handler->PSendSysMessage(LANG_MOVIE_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->GetPlayer()->SendMovieStart(id);
        return true;
    }

    //Play sound
    static bool HandleDebugPlaySoundCommand(ChatHandler* handler, char const* args)
    {
        // USAGE: .debug playsound #soundid
        // #soundid - ID decimal number from SoundEntries.dbc (1st column)
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 soundId = atoi((char*)args);

        if (!sSoundEntriesStore.LookupEntry(soundId))
        {
            handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (handler->GetSession()->GetPlayer()->GetTarget())
            unit->PlayDistanceSound(soundId, handler->GetSession()->GetPlayer());
        else
            unit->PlayDirectSound(soundId, handler->GetSession()->GetPlayer());

        handler->PSendSysMessage(LANG_YOU_HEAR_SOUND, soundId);
        return true;
    }

    static bool HandleDebugPlayVisualCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (tok.size() < 1)
            return false;

        Unit* target = handler->getSelectedUnit();
        uint32 id;
        float speed = 1.0f;
        bool dest = ::strncmp(tok[0], "dest", ::strlen(tok[0])) == 0;
        if (dest)
        {
            if (tok.size() < 2)
                return false;
            id = ::strtoul(tok[1], nullptr, 10);
            if (tok.size() > 2)
                speed = ::strtof(tok[2], nullptr);
        }
        else
        {
            id = ::strtoul(tok[0], nullptr, 10);
            if (tok.size() > 1)
                speed = ::strtof(tok[1], nullptr);
        }

        if (!id)
            return false;

        if (dest)
            handler->GetSession()->GetPlayer()->SendPlaySpellVisual(id, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), speed);
        else
            handler->GetSession()->GetPlayer()->SendPlaySpellVisual(id, target->GetGUID(), speed);
        return true;

    }

    static bool HandleDebugPlayVisualKitCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (tok.size() < 1)
            return false;

        uint32 kit = std::strtoul(tok[0], 0, 10);
        if (!kit)
            return false;

        uint32 duration = 0;
        if (tok.size() > 1)
            duration = std::strtoul(tok[1], 0, 10);

        int32 type = 0;
        if (tok.size() > 2)
            type = std::strtol(tok[2], 0, 10);

        Unit* unit = handler->getSelectedUnit();
        unit->SendPlaySpellVisualKit(kit, duration, type);
        return true;
    }

    static bool HandleDebugSendSpellFailCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* result = strtok((char*)args, " ");
        if (!result)
            return false;

        uint8 failNum = (uint8)atoi(result);
        if (failNum == 0 && *result != '0')
            return false;

        char* fail1 = strtok(NULL, " ");
        uint8 failArg1 = fail1 ? (uint8)atoi(fail1) : 0;

        char* fail2 = strtok(NULL, " ");
        uint8 failArg2 = fail2 ? (uint8)atoi(fail2) : 0;

        WorldPacket data(SMSG_CAST_FAILED);
        data << uint32(133);
        data << uint32(failNum);
        data << uint8(0);
        data.WriteBit(failArg1 != 0);
        data.WriteBit(failArg2 != 0);
        if (fail1 || fail2)
            data << uint32(failArg1);
        if (fail2)
            data << uint32(failArg2);

        handler->GetSession()->SendPacket(&data);

        return true;
    }

    static bool HandleDebugSendEquipErrorCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        InventoryResult msg = InventoryResult(atoi(args));
        handler->GetSession()->GetPlayer()->SendEquipError(msg, NULL, NULL);
        return true;
    }

    static bool HandleDebugSendSellErrorCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        SellResult msg = SellResult(atoi(args));
        handler->GetSession()->GetPlayer()->SendSellError(msg, 0, ObjectGuid::Empty);
        return true;
    }

    static bool HandleDebugSendBuyErrorCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        BuyResult msg = BuyResult(atoi(args));
        handler->GetSession()->GetPlayer()->SendBuyError(msg, 0, 0, 0);
        return true;
    }

    static bool HandleDebugSendOpcodeCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        Player* player = NULL;
        if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
            player = handler->GetSession()->GetPlayer();
        else
            player = unit->ToPlayer();

        if (!unit)
            unit = player;

        std::ifstream ifs("opcode.txt");
        if (ifs.fail())
            return false;

        // remove comments from file
        std::stringstream parsedStream;
        while (!ifs.eof())
        {
            char commentToken[2];
            ifs.get(commentToken[0]);
            if (commentToken[0] == '/' && !ifs.eof())
            {
                ifs.get(commentToken[1]);
                // /* comment
                if (commentToken[1] == '*')
                {
                    while (!ifs.eof())
                    {
                        ifs.get(commentToken[0]);
                        if (commentToken[0] == '*' && !ifs.eof())
                        {
                            ifs.get(commentToken[1]);
                            if (commentToken[1] == '/')
                                break;
                            else
                                ifs.putback(commentToken[1]);
                        }
                    }
                    continue;
                }
                // line comment
                else if (commentToken[1] == '/')
                {
                    std::string str;
                    getline(ifs, str);
                    continue;
                }
                // regular data
                else
                    ifs.putback(commentToken[1]);
            }
            parsedStream.put(commentToken[0]);
        }
        ifs.close();

        uint32 opcode;
        parsedStream >> opcode;

        WorldPacket data(opcode, 0);

        while (!parsedStream.eof())
        {
            std::string type;
            parsedStream >> type;

            if (type == "")
                break;

            if (type == "uint8")
            {
                uint16 val1;
                parsedStream >> val1;
                data << uint8(val1);
            }
            else if (type == "uint16")
            {
                uint16 val2;
                parsedStream >> val2;
                data << val2;
            }
            else if (type == "uint32")
            {
                uint32 val3;
                parsedStream >> val3;
                data << val3;
            }
            else if (type == "uint64")
            {
                uint64 val4;
                parsedStream >> val4;
                data << val4;
            }
            else if (type == "float")
            {
                float val5;
                parsedStream >> val5;
                data << val5;
            }
            else if (type == "string")
            {
                std::string val6;
                parsedStream >> val6;
                data << val6;
            }
            else if (type == "appitsguid")
            {
                data << unit->GetPackGUID();
            }
            else if (type == "appmyguid")
            {
                data << player->GetPackGUID();
            }
            else if (type == "appgoguid")
            {
                GameObject* obj = handler->GetNearbyGameObject();
                if (!obj)
                {
                    handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, 0);
                    handler->SetSentErrorMessage(true);
                    ifs.close();
                    return false;
                }
                data << obj->GetPackGUID();
            }
            else if (type == "goguid")
            {
                GameObject* obj = handler->GetNearbyGameObject();
                if (!obj)
                {
                    handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, 0);
                    handler->SetSentErrorMessage(true);
                    ifs.close();
                    return false;
                }
                data << uint64(obj->GetGUID());
            }
            else if (type == "myguid")
            {
                data << uint64(player->GetGUID());
            }
            else if (type == "itsguid")
            {
                data << uint64(unit->GetGUID());
            }
            else if (type == "itspos")
            {
                data << unit->GetPositionX();
                data << unit->GetPositionY();
                data << unit->GetPositionZ();
            }
            else if (type == "mypos")
            {
                data << player->GetPositionX();
                data << player->GetPositionY();
                data << player->GetPositionZ();
            }
            else
            {
                TC_LOG_ERROR("misc", "Sending opcode that has unknown type '%s'", type.c_str());
                break;
            }
        }
        TC_LOG_DEBUG("network", "Sending opcode %u", data.GetOpcode());
        data.hexlike();
        player->GetSession()->SendPacket(&data, true);
        handler->PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName().c_str());
        return true;
    }

    static bool HandleDebugUpdateWorldStateCommand(ChatHandler* handler, char const* args)
    {
        char* w = strtok((char*)args, " ");
        char* s = strtok(NULL, " ");

        if (!w || !s)
            return false;

        uint32 world = (uint32)atoi(w);
        uint32 state = (uint32)atoi(s);
        handler->GetSession()->GetPlayer()->SendUpdateWorldState(world, state);
        return true;
    }

    static bool HandleDebugAreaTriggersCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player->isDebugAreaTriggers)
        {
            handler->PSendSysMessage(LANG_DEBUG_AREATRIGGER_ON);
            player->isDebugAreaTriggers = true;
        }
        else
        {
            handler->PSendSysMessage(LANG_DEBUG_AREATRIGGER_OFF);
            player->isDebugAreaTriggers = false;
        }
        return true;
    }

    //Send notification in channel
    static bool HandleDebugSendChannelNotifyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* name = "test";
        uint8 code = atoi(args);

        WorldPacket data(SMSG_CHANNEL_NOTIFY, (1+10));
        data << code;                                           // notify type
        data << name;                                           // channel name
        data << uint32(0);
        data << uint32(0);
        handler->GetSession()->SendPacket(&data);
        return true;
    }

    //Send notification in chat
    static bool HandleDebugSendChatMsgCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* msg = "testtest";
        uint8 type = atoi(args);
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, ChatMsg(type), LANG_UNIVERSAL, handler->GetSession()->GetPlayer(), handler->GetSession()->GetPlayer(), msg, 0, "chan");
        handler->GetSession()->SendPacket(&data);
        return true;
    }

    static bool HandleDebugSendQuestPartyMsgCommand(ChatHandler* handler, char const* args)
    {
        uint32 msg = atol((char*)args);
        handler->GetSession()->GetPlayer()->SendPushToPartyResponse(handler->GetSession()->GetPlayer(), msg);
        return true;
    }

    static bool HandleDebugGetLootRecipientCommand(ChatHandler* handler, char const* /*args*/)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
            return false;

        handler->PSendSysMessage("Loot recipient for creature %s (GUID %u, DB GUID %u) is %s",
            target->GetName().c_str(), target->GetGUID().GetCounter(), target->GetDBTableGUIDLow(),
            target->HasLootRecipient() ? (target->GetLootRecipient() ? target->GetLootRecipient()->GetName().c_str() : "offline") : "no loot recipient");
        return true;
    }

    static bool HandleDebugSendQuestInvalidMsgCommand(ChatHandler* handler, char const* args)
    {
        QuestFailedReason msg = static_cast<QuestFailedReason>(atol((char*)args));
        handler->GetSession()->GetPlayer()->SendCanTakeQuestResponse(msg);
        return true;
    }

    static bool HandleDebugGetItemStateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string itemState = args;

        ItemUpdateState state = ITEM_UNCHANGED;
        bool listQueue = false;
        bool checkAll = false;

        if (itemState == "unchanged")
            state = ITEM_UNCHANGED;
        else if (itemState == "changed")
            state = ITEM_CHANGED;
        else if (itemState == "new")
            state = ITEM_NEW;
        else if (itemState == "removed")
            state = ITEM_REMOVED;
        else if (itemState == "queue")
            listQueue = true;
        else if (itemState == "check_all")
            checkAll = true;
        else
            return false;

        Player* player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        if (!listQueue && !checkAll)
        {
            itemState = "The player has the following " + itemState + " items: ";
            handler->SendSysMessage(itemState.c_str());
            for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
            {
                if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                    continue;

                if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    if (Bag* bag = item->ToBag())
                    {
                        for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                            if (Item* item2 = bag->GetItemByPos(j))
                                if (item2->GetState() == state)
                                    handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item2->GetSlot(), item2->GetGUID().GetCounter(), item2->GetOwnerGUID().GetCounter());
                    }
                    else if (item->GetState() == state)
                        handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUID().GetCounter(), item->GetOwnerGUID().GetCounter());
                }
            }
        }

        if (listQueue)
        {
            std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
            for (size_t i = 0; i < updateQueue.size(); ++i)
            {
                Item* item = updateQueue[i];
                if (!item)
                    continue;

                Bag* container = item->GetContainer();
                uint8 bagSlot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

                std::string st;
                switch (item->GetState())
                {
                    case ITEM_UNCHANGED:
                        st = "unchanged";
                        break;
                    case ITEM_CHANGED:
                        st = "changed";
                        break;
                    case ITEM_NEW:
                        st = "new";
                        break;
                    case ITEM_REMOVED:
                        st = "removed";
                        break;
                }

                handler->PSendSysMessage("bag: %d slot: %d guid: %d - state: %s", bagSlot, item->GetSlot(), item->GetGUID().GetCounter(), st.c_str());
            }
            if (updateQueue.empty())
                handler->PSendSysMessage("The player's updatequeue is empty");
        }

        if (checkAll)
        {
            bool error = false;
            std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
            for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
            {
                if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                    continue;

                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                if (!item)
                    continue;

                if (item->GetSlot() != i)
                {
                    handler->PSendSysMessage("Item with slot %d and guid %d has an incorrect slot value: %d", i, item->GetGUID().GetCounter(), item->GetSlot());
                    error = true;
                    continue;
                }

                if (item->GetOwnerGUID() != player->GetGUID())
                {
                    handler->PSendSysMessage("The item with slot %d and itemguid %d does have non-matching owner guid (%d) and player guid (%d) !", item->GetSlot(), item->GetGUID().GetCounter(), item->GetOwnerGUID().GetCounter(), player->GetGUID().GetCounter());
                    error = true;
                    continue;
                }

                if (Bag* container = item->GetContainer())
                {
                    handler->PSendSysMessage("The item with slot %d and guid %d has a container (slot: %d, guid: %d) but shouldn't!", item->GetSlot(), item->GetGUID().GetCounter(), container->GetSlot(), container->GetGUID().GetCounter());
                    error = true;
                    continue;
                }

                if (item->IsInUpdateQueue())
                {
                    uint16 qp = item->GetQueuePos();
                    if (qp > updateQueue.size())
                    {
                        handler->PSendSysMessage("The item with slot %d and guid %d has its queuepos (%d) larger than the update queue size! ", item->GetSlot(), item->GetGUID().GetCounter(), qp);
                        error = true;
                        continue;
                    }

                    if (updateQueue[qp] == NULL)
                    {
                        handler->PSendSysMessage("The item with slot %d and guid %d has its queuepos (%d) pointing to NULL in the queue!", item->GetSlot(), item->GetGUID().GetCounter(), qp);
                        error = true;
                        continue;
                    }

                    if (updateQueue[qp] != item)
                    {
                        handler->PSendSysMessage("The item with slot %d and guid %d has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", item->GetSlot(), item->GetGUID().GetCounter(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUID().GetCounter());
                        error = true;
                        continue;
                    }
                }
                else if (item->GetState() != ITEM_UNCHANGED)
                {
                    handler->PSendSysMessage("The item with slot %d and guid %d is not in queue but should be (state: %d)!", item->GetSlot(), item->GetGUID().GetCounter(), item->GetState());
                    error = true;
                    continue;
                }

                if (Bag* bag = item->ToBag())
                {
                    for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                    {
                        Item* item2 = bag->GetItemByPos(j);
                        if (!item2)
                            continue;

                        if (item2->GetSlot() != j)
                        {
                            handler->PSendSysMessage("The item in bag %d and slot %d (guid: %d) has an incorrect slot value: %d", bag->GetSlot(), j, item2->GetGUID().GetCounter(), item2->GetSlot());
                            error = true;
                            continue;
                        }

                        if (item2->GetOwnerGUID() != player->GetGUID())
                        {
                            handler->PSendSysMessage("The item in bag %d at slot %d and with itemguid %d, the owner's guid (%d) and the player's guid (%d) don't match!", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), item2->GetOwnerGUID().GetCounter(), player->GetGUID().GetCounter());
                            error = true;
                            continue;
                        }

                        Bag* container = item2->GetContainer();
                        if (!container)
                        {
                            handler->PSendSysMessage("The item in bag %d at slot %d with guid %d has no container!", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter());
                            error = true;
                            continue;
                        }

                        if (container != bag)
                        {
                            handler->PSendSysMessage("The item in bag %d at slot %d with guid %d has a different container(slot %d guid %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), container->GetSlot(), container->GetGUID().GetCounter());
                            error = true;
                            continue;
                        }

                        if (item2->IsInUpdateQueue())
                        {
                            uint16 qp = item2->GetQueuePos();
                            if (qp > updateQueue.size())
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) larger than the update queue size! ", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), qp);
                                error = true;
                                continue;
                            }

                            if (updateQueue[qp] == NULL)
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) that points to NULL in the queue!", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), qp);
                                error = true;
                                continue;
                            }

                            if (updateQueue[qp] != item2)
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUID().GetCounter());
                                error = true;
                                continue;
                            }
                        }
                        else if (item2->GetState() != ITEM_UNCHANGED)
                        {
                            handler->PSendSysMessage("The item in bag %d at slot %d having guid %d is not in queue but should be (state: %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUID().GetCounter(), item2->GetState());
                            error = true;
                            continue;
                        }
                    }
                }
            }

            for (size_t i = 0; i < updateQueue.size(); ++i)
            {
                Item* item = updateQueue[i];
                if (!item)
                    continue;

                if (item->GetOwnerGUID() != player->GetGUID())
                {
                    handler->PSendSysMessage("queue(%lu): For the item with guid %d, the owner's guid (%d) and the player's guid (%d) don't match!", i, item->GetGUID().GetCounter(), item->GetOwnerGUID().GetCounter(), player->GetGUID().GetCounter());
                    error = true;
                    continue;
                }

                if (item->GetQueuePos() != i)
                {
                    handler->PSendSysMessage("queue(%lu): For the item with guid %d, the queuepos doesn't match it's position in the queue!", i, item->GetGUID().GetCounter());
                    error = true;
                    continue;
                }

                if (item->GetState() == ITEM_REMOVED)
                    continue;

                Item* test = player->GetItemByPos(item->GetBagSlot(), item->GetSlot());

                if (test == NULL)
                {
                    handler->PSendSysMessage("queue(%lu): The bag(%d) and slot(%d) values for the item with guid %d are incorrect, the player doesn't have any item at that position!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUID().GetCounter());
                    error = true;
                    continue;
                }

                if (test != item)
                {
                    handler->PSendSysMessage("queue(%lu): The bag(%d) and slot(%d) values for the item with guid %d are incorrect, an item which guid is %d is there instead!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUID().GetCounter(), test->GetGUID().GetCounter());
                    error = true;
                    continue;
                }
            }
            if (!error)
                handler->SendSysMessage("All OK!");
        }

        return true;
    }

    static bool HandleDebugBattlegroundCommand(ChatHandler* /*handler*/, char const* /*args*/)
    {
        sBattlegroundMgr->ToggleTesting();
        return true;
    }

    static bool HandleDebugArenaCommand(ChatHandler* /*handler*/, char const* /*args*/)
    {
        sBattlegroundMgr->ToggleArenaTesting();
        return true;
    }

    static bool HandleDebugRatedBgCommand(ChatHandler*, char const*)
    {
        sBattlegroundMgr->ToggleRatedBgTesting();
        return true;
    }

    static bool HandleDebugThreatListCommand(ChatHandler* handler, char const* /*args*/)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target || target->IsTotem() || target->IsPet())
            return false;

        ThreatContainer::StorageType const &threatList = target->GetThreatManager().getThreatList();
        ThreatContainer::StorageType::const_iterator itr;
        uint32 count = 0;
        handler->PSendSysMessage("Threat list of %s (guid %u)", target->GetName().c_str(), target->GetGUID().GetCounter());
        for (itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            Unit* unit = (*itr)->getTarget();
            if (!unit)
                continue;
            ++count;
            handler->PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", count, unit->GetName().c_str(), unit->GetGUID().GetCounter(), (*itr)->getThreat());
        }
        handler->SendSysMessage("End of threat list.");
        return true;
    }

    static bool HandleDebugHostileRefListCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();
        uint32 count = 0;
        handler->PSendSysMessage("Hostil reference list of %s (guid %u)", target->GetName().c_str(), target->GetGUID().GetCounter());
        for (auto const& threatEntry : target->GetThreatManager().GetThreatenedByMeList())
        {
            if (Unit* unit = threatEntry.second->GetOwner())
            {
                ++count;
                handler->PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", count, unit->GetName().c_str(), unit->GetGUID().GetCounter(), threatEntry.second->GetThreat());
            }
        }
        handler->SendSysMessage("End of hostil reference list.");
        return true;
    }

    static bool HandleDebugSetVehicleIdCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target || target->IsVehicle())
            return false;

        if (!args)
            return false;

        char* i = strtok((char*)args, " ");
        if (!i)
            return false;

        uint32 id = (uint32)atoi(i);
        //target->SetVehicleId(id);
        handler->PSendSysMessage("Vehicle id set to %u", id);
        return true;
    }

    static bool HandleDebugEnterVehicleCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target || !target->IsVehicle())
            return false;

        if (!args)
            return false;

        char* i = strtok((char*)args, " ");
        if (!i)
            return false;

        char* j = strtok(NULL, " ");

        uint32 entry = (uint32)atoi(i);
        int8 seatId = j ? (int8)atoi(j) : -1;

        if (!entry)
            handler->GetSession()->GetPlayer()->EnterVehicle(target, seatId);
        else
        {
            Creature* passenger = NULL;
            Trinity::AllCreaturesOfEntryInRange check(handler->GetSession()->GetPlayer(), entry, 20.0f);
            Trinity::CreatureSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(handler->GetSession()->GetPlayer(), passenger, check);
            handler->GetSession()->GetPlayer()->VisitNearbyObject(30.0f, searcher);
            if (!passenger || passenger == target)
                return false;
            passenger->EnterVehicle(target, seatId);
        }

        handler->PSendSysMessage("Unit %u entered vehicle %d", entry, (int32)seatId);
        return true;
    }

    static bool HandleDebugSpawnVehicleCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* e = strtok((char*)args, " ");
        char* i = strtok(NULL, " ");

        if (!e)
            return false;

        uint32 entry = (uint32)atoi(e);

        float x, y, z, o = handler->GetSession()->GetPlayer()->GetOrientation();
        handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, handler->GetSession()->GetPlayer()->GetObjectSize());

        if (!i)
            return handler->GetSession()->GetPlayer()->SummonCreature(entry, x, y, z, o);

        uint32 id = (uint32)atoi(i);

        CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(entry);

        if (!ci)
            return false;

        VehicleEntry const* ve = sVehicleStore.LookupEntry(id);

        if (!ve)
            return false;

        Creature* v = new Creature;

        Map* map = handler->GetSession()->GetPlayer()->GetMap();

        if (!v->Create(map->GenerateLowGuid<HighGuid::Vehicle>(), map, handler->GetSession()->GetPlayer()->GetPhaseMask(), entry, id, handler->GetSession()->GetPlayer()->GetTeam(), x, y, z, o))
        {
            delete v;
            return false;
        }

        for (auto phase : handler->GetSession()->GetPlayer()->GetPhases())
            v->SetPhased(phase, false, true);

        map->AddToMap(v->ToCreature());

        return true;
    }

    static bool HandleDebugSendLargePacketCommand(ChatHandler* handler, char const* /*args*/)
    {
        const char* stuffingString = "This is a dummy string to push the packet's size beyond 128000 bytes. ";
        std::ostringstream ss;
        while (ss.str().size() < 128000)
            ss << stuffingString;
        handler->SendSysMessage(ss.str().c_str());
        return true;
    }

    static bool HandleDebugSendSetPhaseShiftCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* t = strtok((char*)args, " ");
        char* p = strtok(NULL, " ");

        if (!t)
            return false;

        std::set<uint32> terrainswap;
        std::set<uint32> phaseId;
        std::set<uint32> worldMapArea;

        terrainswap.insert((uint32)atoi(t));

        if (p)
            phaseId.insert((uint32)atoi(p));

        handler->GetSession()->SendSetPhaseShift(phaseId, terrainswap, worldMapArea);
        return true;
    }

    static bool HandleDebugGetItemValueCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* e = strtok((char*)args, " ");
        char* f = strtok(NULL, " ");

        if (!e || !f)
            return false;

        uint32 guid = (uint32)atoi(e);
        uint32 index = (uint32)atoi(f);

        Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(ObjectGuid(HighGuid::Item, guid));

        if (!i)
            return false;

        if (index >= i->GetValuesCount())
            return false;

        uint32 value = i->GetUInt32Value(index);

        handler->PSendSysMessage("Item %u: value at %u is %u", guid, index, value);

        return true;
    }

    static bool HandleDebugSetItemValueCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* e = strtok((char*)args, " ");
        char* f = strtok(NULL, " ");
        char* g = strtok(NULL, " ");

        if (!e || !f || !g)
            return false;

        uint32 guid = (uint32)atoi(e);
        uint32 index = (uint32)atoi(f);
        uint32 value = (uint32)atoi(g);

        Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(ObjectGuid(HighGuid::Item, guid));

        if (!i)
            return false;

        if (index >= i->GetValuesCount())
            return false;

        i->SetUInt32Value(index, value);

        return true;
    }

    static bool HandleDebugItemExpireCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* e = strtok((char*)args, " ");
        if (!e)
            return false;

        uint32 guid = (uint32)atoi(e);

        Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(ObjectGuid(HighGuid::Item, guid));

        if (!i)
            return false;

        handler->GetSession()->GetPlayer()->DestroyItem(i->GetBagSlot(), i->GetSlot(), true);
        sScriptMgr->OnItemExpire(handler->GetSession()->GetPlayer(), i->GetTemplate());

        return true;
    }

    //show animation
    static bool HandleDebugAnimCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 animId = atoi((char*)args);
        handler->GetSession()->GetPlayer()->HandleEmoteCommand(animId);
        return true;
    }

    static bool HandleDebugLoSCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (Unit* unit = handler->getSelectedUnit())
            handler->PSendSysMessage("Unit %s (GuidLow: %u) is %sin LoS", unit->GetName().c_str(), unit->GetGUID().GetCounter(), handler->GetSession()->GetPlayer()->IsWithinLOSInMap(unit) ? "" : "not ");
        return true;
    }

    static bool HandleDebugSetAuraStateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 state = atoi((char*)args);
        if (!state)
        {
            // reset all states
            for (int i = 1; i <= 32; ++i)
                unit->ModifyAuraState(AuraStateType(i), false);
            return true;
        }

        unit->ModifyAuraState(AuraStateType(abs(state)), state > 0);
        return true;
    }

    static bool HandleDebugSetValueCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* x = strtok((char*)args, " ");
        char* y = strtok(NULL, " ");
        char* z = strtok(NULL, " ");

        if (!x || !y)
            return false;

        WorldObject* target = handler->getSelectedObject();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ObjectGuid guid = target->GetGUID();

        uint32 opcode = (uint32)atoi(x);
        if (opcode >= target->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, guid.GetCounter(), target->GetValuesCount());
            return false;
        }

        bool isInt32 = true;
        if (z)
            isInt32 = (bool)atoi(z);

        if (isInt32)
        {
            uint32 value = (uint32)atoi(y);
            target->SetUInt32Value(opcode, value);
            handler->PSendSysMessage(LANG_SET_UINT_FIELD, guid.GetCounter(), opcode, value);
        }
        else
        {
            float value = (float)atof(y);
            target->SetFloatValue(opcode, value);
            handler->PSendSysMessage(LANG_SET_FLOAT_FIELD, guid.GetCounter(), opcode, value);
        }

        return true;
    }

    static bool HandleDebugGetValueCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* x = strtok((char*)args, " ");
        char* z = strtok(NULL, " ");

        if (!x)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ObjectGuid guid = target->GetGUID();

        uint32 opcode = (uint32)atoi(x);
        if (opcode >= target->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, guid.GetCounter(), target->GetValuesCount());
            return false;
        }

        bool isInt32 = true;
        if (z)
            isInt32 = (bool)atoi(z);

        if (isInt32)
        {
            uint32 value = target->GetUInt32Value(opcode);
            handler->PSendSysMessage(LANG_GET_UINT_FIELD, guid.GetCounter(), opcode, value);
        }
        else
        {
            float value = target->GetFloatValue(opcode);
            handler->PSendSysMessage(LANG_GET_FLOAT_FIELD, guid.GetCounter(), opcode, value);
        }

        return true;
    }

    static bool HandleDebugMod32ValueCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* x = strtok((char*)args, " ");
        char* y = strtok(NULL, " ");

        if (!x || !y)
            return false;

        uint32 opcode = (uint32)atoi(x);
        int value = atoi(y);

        if (opcode >= handler->GetSession()->GetPlayer()->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, handler->GetSession()->GetPlayer()->GetGUID().GetCounter(), handler->GetSession()->GetPlayer()->GetValuesCount());
            return false;
        }

        int currentValue = (int)handler->GetSession()->GetPlayer()->GetUInt32Value(opcode);

        currentValue += value;
        handler->GetSession()->GetPlayer()->SetUInt32Value(opcode, (uint32)currentValue);

        handler->PSendSysMessage(LANG_CHANGE_32BIT_FIELD, opcode, currentValue);

        return true;
    }

    static bool HandleDebugUpdateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 updateIndex;
        uint32 value;

        char* index = strtok((char*)args, " ");

        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!index)
            return true;

        updateIndex = atoi(index);
        //check updateIndex
        if (unit->GetTypeId() == TYPEID_PLAYER)
        {
            if (updateIndex >= PLAYER_END)
                return true;
        }
        else if (updateIndex >= UNIT_END)
            return true;

        char* val = strtok(NULL, " ");
        if (!val)
        {
            value = unit->GetUInt32Value(updateIndex);

            handler->PSendSysMessage(LANG_UPDATE, unit->GetGUID().GetCounter(), updateIndex, value);
            return true;
        }

        value = atoi(val);

        handler->PSendSysMessage(LANG_UPDATE_CHANGE, unit->GetGUID().GetCounter(), updateIndex, value);

        unit->SetUInt32Value(updateIndex, value);

        return true;
    }

    static bool HandleDebugSet32BitCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        WorldObject* target = handler->getSelectedObject();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* x = strtok((char*)args, " ");
        char* y = strtok(NULL, " ");

        if (!x || !y)
            return false;

        uint32 opcode = (uint32)atoi(x);
        uint32 val = (uint32)atoi(y);
        if (val > 32)                                         //uint32 = 32 bits
            return false;

        uint32 value = val ? 1 << (val - 1) : 0;
        target->SetUInt32Value(opcode,  value);

        handler->PSendSysMessage(LANG_SET_32BIT_FIELD, opcode, value);
        return true;
    }

    static bool HandleDebugMoveflagsCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        if (!*args)
        {
            //! Display case
            handler->PSendSysMessage(LANG_MOVEFLAGS_GET, target->GetUnitMovementFlags(), target->GetExtraUnitMovementFlags());
        }
        else
        {
            char* mask1 = strtok((char*)args, " ");
            if (!mask1)
                return false;

            char* mask2 = strtok(NULL, " \n");

            uint32 moveFlags = (uint32)atoi(mask1);
            target->SetUnitMovementFlags(moveFlags);

            /// @fixme: port master's HandleDebugMoveflagsCommand; flags need different handling

            if (mask2)
            {
                uint32 moveFlagsExtra = uint32(atoi(mask2));
                target->SetExtraUnitMovementFlags(moveFlagsExtra);
            }

            if (target->GetTypeId() != TYPEID_PLAYER)
                target->DestroyForNearbyPlayers();  // Force new SMSG_UPDATE_OBJECT:CreateObject
            else
            {
                WorldPacket data(SMSG_PLAYER_MOVE);
                target->WriteMovementInfo(data);
                target->SendMessageToSet(&data, true);
            }

            handler->PSendSysMessage(LANG_MOVEFLAGS_SET, target->GetUnitMovementFlags(), target->GetExtraUnitMovementFlags());
        }

        return true;
    }

    static bool HandleWPGPSCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        TC_LOG_INFO("sql.dev", "(@PATH, XX, %.3f, %.3f, %.5f, 0, 0, 0, 100, 0),", player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        handler->PSendSysMessage("Waypoint SQL written to SQL Developer log");
        return true;
    }

    static bool HandleDebugTransportCommand(ChatHandler* handler, char const* args)
    {
        Transport* transport = handler->GetSession()->GetPlayer()->GetTransport();
        if (!transport)
            return false;

        bool start = false;
        if (!stricmp(args, "stop"))
            transport->EnableMovement(false);
        else if (!stricmp(args, "start"))
        {
            transport->EnableMovement(true);
            start = true;
        }
        else
        {
            handler->PSendSysMessage("Transport %s is %s", transport->GetName().c_str(), transport->GetGoState() == GO_STATE_READY ? "stopped" : "moving");
            return true;
        }

        handler->PSendSysMessage("Transport %s %s", transport->GetName().c_str(), start ? "started" : "stopped");
        return true;
    }

    static bool HandleDebugPhaseCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        Player* player = handler->GetSession()->GetPlayer();
        if (unit && unit->GetTypeId() == TYPEID_PLAYER)
            player = unit->ToPlayer();

        player->GetPhaseMgr().SendDebugReportToPlayer(handler->GetSession()->GetPlayer());
        return true;
    }

    static bool HandleDebugCastErrorCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer tok{ args, ' ' };
        if (tok.size() < 2)
            return false;
        uint32 spellId = std::strtoul(tok[0], nullptr, 10);
        uint32 error = std::strtoul(tok[1], nullptr, 10);
        uint32 custom = 0;
        if (tok.size() > 2)
            custom = std::strtoul(tok[2], nullptr, 10);
        auto spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            return handler->SendError("Spell doesn't exist!");

        Player* player = handler->GetSession()->GetPlayer();
        Spell::SendCastResult(player, spellInfo, 0, SpellCastResult(error), SpellCustomErrors(custom));
        return true;
    }

    static bool HandleDebugSetAttackPower(ChatHandler* handler, char const* args)
    {
        uint32 const attackPowerAura = 9136;

        Player* player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        float desired = strtol(args, nullptr, 10);
        if (desired <= 0)
            return handler->SendError("Invalid value");

        player->RemoveAurasDueToSpell(attackPowerAura);
        float base = player->GetTotalAttackPowerValue(BASE_ATTACK);
        float ranged = player->GetTotalAttackPowerValue(RANGED_ATTACK);
        CustomSpellValues ctx;
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, (desired - base) / player->GetTotalAuraMultiplier(SPELL_AURA_MOD_ATTACK_POWER_PCT));
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT1, (desired - ranged) / player->GetTotalAuraMultiplier(SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT));
        player->CastCustomSpell(attackPowerAura, ctx, player, TRIGGERED_FULL_MASK);
        return true;
    }

    static bool HandleDebugSetSpellPower(ChatHandler* handler, char const* args)
    {
        uint32 const spellPowerAura = 9342;

        Player* player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        float desired = strtol(args, nullptr, 10);
        if (desired <= 0)
            return handler->SendError("Invalid value");

        player->RemoveAurasDueToSpell(spellPowerAura);
        float base = player->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + 1);
        CustomSpellValues ctx;
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT0, (desired - base) / player->GetTotalAuraMultiplier(SPELL_AURA_MOD_SPELL_POWER_PCT));
        ctx.AddSpellMod(SPELLVALUE_BASE_POINT1, (desired - base) / player->GetTotalAuraMultiplier(SPELL_AURA_MOD_SPELL_POWER_PCT));
        player->CastCustomSpell(spellPowerAura, ctx, player, TRIGGERED_FULL_MASK);
        return true;
    }

    static bool HandleDebugArchaeologyFind(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        auto sites = sObjectMgr->GetResearchDigsitesForContinent(player->GetMapId());
        if (!sites)
            return handler->SendError("No digsites found at this map!");

        float radius = 20.0f;
        if (*args)
            radius = std::strtof(args, nullptr);

        for (auto&& itr : *sites)
        {
            if (auto list = sObjectMgr->GetArcheologyFindListForDigsite(itr.digsiteId))
            {
                std::list<GameObject*> gameobjects;
                Trinity::GameObjectInRangeCheck check{ player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), radius };
                Trinity::GameObjectListSearcher<Trinity::GameObjectInRangeCheck> searcher{ player, gameobjects, check };
                player->VisitNearbyObject(radius * 2, searcher, true);

                for (auto&& find : *list)
                {
                    if (player->IsWithinDist2d(find.x, find.y, radius))
                    {
                        bool found = false;
                        for (auto&& itr : gameobjects)
                        {
                            if (itr->GetPositionX() == find.x && itr->GetPositionY() == find.y && itr->GetPositionZ() == find.z)
                            {
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                            player->SummonGameObject(find.goEntry, find.x, find.y, find.z, 0, { }, 60);

                        player->SendPlaySpellVisual(7906, find.x, find.y, find.z, 1.0f);
                    }
                }
            }
        }
        return true;
    }

    static bool HandleDebugArchaeologyDigsite(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint32 digsiteId = std::strtoul(args, nullptr, 10);
        auto const* digsiteEntry = sResearchSiteStore.LookupEntry(digsiteId);
        if (!digsiteEntry)
            return handler->SendError("Invalid digsite id: %u", digsiteId);

        if (!player->SpawnDigsite(digsiteId))
            return handler->SendError("You already have digsite %u (%s)", digsiteEntry->Id, digsiteEntry->SiteName[handler->GetSessionDbcLocale()]);
        handler->PSendSysMessage("Digsite %u (%s) was spawned", digsiteEntry->Id, digsiteEntry->SiteName[handler->GetSessionDbcLocale()]);
        return true;
    }

    static bool HandleDebugVignette(ChatHandler* handler, char const* args)
    {
        char* vignetteIDStr = strtok((char*)args, " ");
        if (!vignetteIDStr)
            return false;

        Unit* selectedUnit = handler->GetSession()->GetPlayer()->GetSelectedUnit();
        if (!selectedUnit)
            return false;

        uint32 vignetteID = atoi(vignetteIDStr);
        ObjectGuid vignetteGUID(HighGuid::Vignette, vignetteID, selectedUnit->GetGUID().GetCounter());

        ByteBuffer playersAddBuffer, objectAddBuffer;
        WorldPacket data(SMSG_VIGNETTE_UPDATE);

        data.WriteBits(0, 24);                                // RemovedCount
        data.WriteBit(true);                                  // ForceUpdate
        data.WriteBits(0, 24);                                // UpdateCount
        //for (auto&& vignetteGuid : m_UpdatedVignette)
        //{ }

        // for (auto&& vignetteGuid : m_RemovedVignette)
        // { }

        data.WriteBits(1, 24);                                // Added count
        {
            data.WriteBit(vignetteGUID[1]);
            data.WriteBit(vignetteGUID[4]);
            data.WriteBit(vignetteGUID[3]);
            data.WriteBit(vignetteGUID[6]);
            data.WriteBit(vignetteGUID[2]);
            data.WriteBit(vignetteGUID[0]);
            data.WriteBit(vignetteGUID[7]);
            data.WriteBit(vignetteGUID[5]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[7]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[1]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[0]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[6]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[2]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[3]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[4]);
            playersAddBuffer.WriteByteSeq(vignetteGUID[5]);
        }

        data.WriteBits(1, 20);
        {
            data.WriteBit(vignetteGUID[5]);
            data.WriteBit(vignetteGUID[3]);
            data.WriteBit(vignetteGUID[7]);
            data.WriteBit(vignetteGUID[4]);
            data.WriteBit(vignetteGUID[2]);
            data.WriteBit(vignetteGUID[0]);
            data.WriteBit(vignetteGUID[6]);
            data.WriteBit(vignetteGUID[1]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[2]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[5]);
            objectAddBuffer << float(selectedUnit->GetPositionZ());
            objectAddBuffer << uint32(vignetteID);
            objectAddBuffer << float(selectedUnit->GetPositionY());
            objectAddBuffer.WriteByteSeq(vignetteGUID[1]);
            objectAddBuffer << float(selectedUnit->GetPositionX());
            objectAddBuffer.WriteByteSeq(vignetteGUID[6]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[7]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[4]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[3]);
            objectAddBuffer.WriteByteSeq(vignetteGUID[0]);
        }

        data.WriteBits(0, 20);                                // UpdateDataCount
        // for (auto&& vignetteGuid : m_UpdatedVignette)
        // { }

        data.FlushBits();

        if (objectAddBuffer.size())
            data.append(objectAddBuffer);
        if (playersAddBuffer.size())
            data.append(playersAddBuffer);

        handler->GetSession()->SendPacket(&data);
        return true;
    }

    static bool HandleDebugValueCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !strlen(args))
            return false;

        char* idStr = strtok((char*)args, " ");
        char* value = strtok(NULL, " ");

        if (!idStr)
            return false;

        uint32 id = atoi(idStr);

        World::DebugValue& dv = sWorld->GetDebugValue(id);
        if (value && strlen(value))
        {
            if (value[strlen(value) - 1] == 'f')
            {
                dv.Float = atof(value);
                handler->PSendSysMessage("DebugValue[%u] = %f", id, dv.Float);
            }
            else
            {
                dv.UInt32 = atof(value);
                handler->PSendSysMessage("DebugValue[%u] = %u", id, dv.UInt32);
            }
        }
        else
            handler->PSendSysMessage("DebugValue[%u] == %u (float: %f)", id, dv.UInt32, dv.Float);

        return true;
    }
};

void AddSC_debug_commandscript()
{
    new debug_commandscript();
}
