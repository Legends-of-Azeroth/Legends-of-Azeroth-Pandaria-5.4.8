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

#include "Opcodes.h"
#include "Log.h"
#include "WorldSession.h"
#include "Packets/AllPackets.h"
#include <iomanip>
#include <sstream>

template<class PacketClass, void(WorldSession::*HandlerFunction)(PacketClass&)>
class PacketHandler : public ClientOpcodeHandler
{
public:
    PacketHandler(char const* name, SessionStatus status, PacketProcessing processing) : ClientOpcodeHandler(name, status, processing) { }

    void Call(WorldSession* session, WorldPacket& packet) const override
    {
        PacketClass nicePacket(std::move(packet));
        nicePacket.Read();
        (session->*HandlerFunction)(nicePacket);
        session->LogUnprocessedTail(nicePacket.GetRawPacket());
    }
};

template<void(WorldSession::*HandlerFunction)(WorldPacket&)>
class PacketHandler<WorldPacket, HandlerFunction> : public ClientOpcodeHandler
{
public:
    PacketHandler(char const* name, SessionStatus status, PacketProcessing processing) : ClientOpcodeHandler(name, status, processing) { }

    void Call(WorldSession* session, WorldPacket& packet) const override
    {
        (session->*HandlerFunction)(packet);
        session->LogUnprocessedTail(&packet);
    }
};

OpcodeTable opcodeTable;

template<typename T>
struct get_packet_class
{
};

template<typename PacketClass>
struct get_packet_class<void(WorldSession::*)(PacketClass&)>
{
    using type = PacketClass;
};

OpcodeTable::OpcodeTable()
{
    memset(_internalTableClient, 0, sizeof(_internalTableClient));
    memset(_internalTableServer, 0, sizeof(_internalTableServer));
}

OpcodeTable::~OpcodeTable()
{
    for (uint16 i = 0; i < NUM_OPCODE_HANDLERS; ++i)
    {
        delete _internalTableClient[i];
        delete _internalTableServer[i];
    }
}

template<typename Handler, Handler HandlerFunction>
void OpcodeTable::ValidateAndSetClientOpcode(OpcodeClient opcode, char const* name, SessionStatus status, PacketProcessing processing)
{
    if (uint32(opcode) == NULL_OPCODE)
    {
        TC_LOG_ERROR("network", "Opcode %s does not have a value", name);
        return;
    }

    if (uint32(opcode) >= NUM_OPCODE_HANDLERS)
    {
        // Shaohao: temp disable spam
        //TC_LOG_ERROR("network", "Tried to set handler for an invalid opcode %d", opcode);
        return;
    }

    if (_internalTableClient[opcode] != nullptr)
    {
        TC_LOG_ERROR("network", "Tried to override client handler of %s with %s (opcode %u)", opcodeTable[opcode]->Name, name, opcode);
        return;
    }

    _internalTableClient[opcode] = new PacketHandler<typename get_packet_class<Handler>::type, HandlerFunction>(name, status, processing);
}

void OpcodeTable::ValidateAndSetServerOpcode(OpcodeServer opcode, char const* name, SessionStatus status)
{
    if (uint32(opcode) == NULL_OPCODE)
    {
        TC_LOG_ERROR("network", "Opcode %s does not have a value", name);
        return;
    }

    if (uint32(opcode) >= NUM_OPCODE_HANDLERS)
    {
        // Shaohao: temp disable spam
        //TC_LOG_ERROR("network", "Tried to set handler for an invalid opcode %d", opcode);
        return;
    }

    if (_internalTableServer[opcode] != nullptr)
    {
        TC_LOG_ERROR("network", "Tried to override server handler of %s with %s (opcode %u)", opcodeTable[opcode]->Name, name, opcode);
        return;
    }

    _internalTableServer[opcode] = new ServerOpcodeHandler(name, status);
}

/// Correspondence between opcodes and their names
void OpcodeTable::Initialize()
{
#define DEFINE_HANDLER(opcode, status, processing, handler) \
    ValidateAndSetClientOpcode<decltype(handler), handler>(opcode, #opcode, status, processing)
    
    DEFINE_HANDLER(CMSG_ACCEPT_LEVEL_GRANT,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAcceptGrantLevel);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ACCEPT_TRADE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAcceptTradeOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ACTIVATE_TAXI,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleActivateTaxiOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ACTIVATE_TAXI_EXPRESS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleActivateTaxiExpressOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ADDON_REGISTERED_PREFIXES,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonRegisteredPrefixesOpcode);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ADD_FRIEND,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddFriendOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ADD_IGNORE,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddIgnoreOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ALTER_APPEARANCE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAlterAppearance);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AREATRIGGER,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAreaTriggerOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUERY,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAreaSpiritHealerQueryOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUEUE,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAreaSpiritHealerQueueOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ATTACKSTOP,                                                      STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleAttackStopOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ATTACKSWING,                                                     STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleAttackSwingOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_HELLO,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionHelloOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_LIST_BIDDER_ITEMS,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionListBidderItems);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_LIST_ITEMS,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionListItems);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_LIST_OWNER_ITEMS,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionListOwnerItems);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_LIST_PENDING_SALES,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionListPendingSales);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_PLACE_BID,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionPlaceBid);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_REMOVE_ITEM,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionRemoveItem);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_REPLICATE_ITEMS,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReplicateItems);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUCTION_SELL_ITEM,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAuctionSellItem);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTH_CONTINUED_SESSION,                                          STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTH_SESSION,                                                    STATUS_NEVER,                        PROCESS_THREADUNSAFE,       &WorldSession::Handle_EarlyProccess);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTOBANK_ITEM,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoBankItemOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTOEQUIP_ITEM,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoEquipItemOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTOEQUIP_ITEM_SLOT,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoEquipItemSlotOpcode);
    DEFINE_HANDLER(CMSG_AUTOSTORE_BAG_ITEM,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoStoreBagItemOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTOSTORE_BANK_ITEM,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoStoreBankItemOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTOSTORE_LOOT_ITEM,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutostoreLootItemOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_AUTO_DECLINE_GUILD_INVITES,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAutoDeclineGuildInvites);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BANKER_ACTIVATE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBankerActivateOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_LEAVE,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlefieldLeaveOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_LIST,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlefieldListOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE,                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBfEntryInviteResponse);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_MGR_EXIT_REQUEST,                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBfExitRequest);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE,                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBfQueueInviteResponse);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_REQUEST,                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBfQueueRequest);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_PORT,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattleFieldPortOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEFIELD_STATUS,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlefieldStatusOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEMASTER_JOIN,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlemasterJoinOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlemasterJoinArena);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLEMASTER_JOIN_RATED,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlemasterJoinRated);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PAY_CHAR_BOOST,                                           STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePayCharBoost);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE,                            STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePayConfirmPurchaseResponse); // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PAY_GET_PRODUCT_LIST,                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePayGetProductList);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PAY_GET_PURCHASE_LIST,                                    STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePayGetPurchaseList);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PAY_START_PURCHASE,                                       STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePayStartPurchase);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_START_PVP_MATCHMAKING,                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetBattleStartPvpMatchmaking);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_STOP_PVP_MATCHMAKING,                                 STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::HandlePetBattleStopPvpMatchmaking);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_DELETE_PET,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetDelete);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_LEARN,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetLearn);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_MODIFY_NAME,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetModifyName);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_QUERY_NAME,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetQueryName);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_REQUEST_JOURNAL,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetRequestJournal);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_REQUEST_JOURNAL_LOCK,                                 STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_SET_BATTLE_SLOT,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetSetBattleSlot);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_SET_FLAGS,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetSetFlags);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_SUMMON_COMPANION,                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetSummonCompanion);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BATTLE_PET_UPDATE_NOTIFY,                                        STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BEGIN_TRADE,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBeginTradeOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BINDER_ACTIVATE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBinderActivateOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BLACK_MARKET_BID_ON_ITEM,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBlackMarketBidOnItem);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BLACK_MARKET_OPEN,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBlackMarketHelloOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BLACK_MARKET_REQUEST_ITEMS,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBlackMarketRequestItemOpcode);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BUYBACK_ITEM,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBuybackItem);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BUY_BANK_SLOT,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBuyBankSlotOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_BUY_ITEM,                                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBuyItemOpcode);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CAGE_BATTLE_PET,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleBattlePetCage);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_ADD_EVENT,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarAddEvent);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_COMPLAIN,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarComplain);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_COPY_EVENT,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarCopyEvent);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_INVITE,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventInvite);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_MODERATOR_STATUS,                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventModeratorStatus);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_REMOVE_INVITE,                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventRemoveInvite);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_RSVP,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventRsvp);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_SIGNUP,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventSignup);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_EVENT_STATUS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarEventStatus);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_GET_CALENDAR,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarGetCalendar);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_GET_EVENT,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarGetEvent);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_GET_NUM_PENDING,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarGetNumPending);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_GUILD_FILTER,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarGuildFilter);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_REMOVE_EVENT,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarRemoveEvent);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CALENDAR_UPDATE_EVENT,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCalendarUpdateEvent);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_AURA,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelAuraOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_AUTO_REPEAT_SPELL,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelAutoRepeatSpellOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_CAST,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleCancelCastOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_CHANNELLING,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelChanneling);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_MOD_SPEED_NO_CONTROL_AURAS,                               STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleCancelModSpeedNoControlAuras);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_MOUNT_AURA,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelMountAuraOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_TEMP_ENCHANTMENT,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelTempEnchantmentOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CANCEL_TRADE,                                                    STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT, PROCESS_THREADUNSAFE,       &WorldSession::HandleCancelTradeOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CAST_SPELL,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleCastSpellOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_LEADERS,                                  STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleChallengeModeRequestLeaders);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS,                                STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleChallengeModeRequestMapStats);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANGE_PLAYER_DIFFICULTY,                                        STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE,                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChangeSeatsOnControlledVehicle);   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_ANNOUNCEMENTS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelAnnouncements);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_BAN,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelBan);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_INVITE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelInvite);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_KICK,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelKick);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_LIST,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelList);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_MODERATOR,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelModerator);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_MUTE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelMute);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_OWNER,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelOwner);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_PASSWORD,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelPassword);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_SET_OWNER,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelSetOwner);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_UNBAN,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelUnban);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_UNMODERATOR,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelUnmoderator);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHANNEL_UNMUTE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChannelUnmute);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAR_CREATE,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharCreateOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAR_CUSTOMIZE,                                                  STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharCustomize);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAR_DELETE,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharDeleteOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ENUM_CHARACTERS,                                                 STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharEnumOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAR_FACTION_OR_RACE_CHANGE,                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharFactionOrRaceChange);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAR_RENAME,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleCharRenameOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHAT_IGNORED,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChatIgnoredOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CHOICE_RESPONSE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePlayerChoiceResponse);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CLEAR_RAID_MARKER,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleClearRaidMarkerOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CLEAR_TRADE_ITEM,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleClearTradeItemOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_COMPLETE_CINEMATIC,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCompleteCinematic);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_COMPLETE_MOVIE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCompleteMovie);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CONFIRM_RESPEC_WIPE,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRespecWipeConfirmOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CONTACT_LIST,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleContactListOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CORPSE_MAP_POSITION_QUERY,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCorpseMapPositionQuery);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CREATURE_QUERY,                                                  STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleCreatureQueryOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DEL_FRIEND,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDelFriendOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DEL_IGNORE,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDelIgnoreOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DESTROY_ITEM,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDestroyItemOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DISCARDED_TIME_SYNC_ACKS,                                        STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT, PROCESS_THREADUNSAFE,       &WorldSession::HandleDiscardedTimeSyncAcks);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DISMISS_CONTROLLED_VEHICLE,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDismissControlledVehicle);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DISMISS_CRITTER,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDismissCritter);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DUEL_PROPOSED,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDuelProposedOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_DUEL_RESPONSE,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleDuelResponseOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_EJECT_PASSENGER,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEjectPassenger);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_EMOTE,                                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEmoteOpcode);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ENABLE_NAGLE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ENABLE_TAXI,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleEnableTaxi);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_EQUIPMENT_SET_DELETE,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEquipmentSetDelete);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_EQUIPMENT_SET_SAVE,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEquipmentSetSave);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_EQUIPMENT_SET_USE,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEquipmentSetUse);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_FAR_SIGHT,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleFarSightOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_FORCE_MOVE_ROOT_ACK,                                             STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveRootAck);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_FORCE_MOVE_UNROOT_ACK,                                           STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveUnRootAck);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GAMEOBJECT_QUERY,                                                STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleGameObjectQueryOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GAMEOBJ_REPORT_USE,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGameobjectReportUse);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GAMEOBJ_USE,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGameObjectUseOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GET_CHALLENGE_MODE_REWARDS,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGetChallengeModeRewards);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GET_MAIL_LIST,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGetMailList);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GET_MIRROR_IMAGE_DATA,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMirrorImageDataRequest);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_CREATE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketCreateOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_DELETE_TICKET,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketDeleteOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_GET_TICKET,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketGetTicketOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_SYSTEM_STATUS,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketSystemStatusOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_CASE_STATUS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketCaseStatusOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GM_TICKET_UPDATE_TEXT,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGMTicketUpdateOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GOSSIP_HELLO,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGossipHelloOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GOSSIP_SELECT_OPTION,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGossipSelectOptionOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GRANT_LEVEL,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGrantLevel);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_ASSISTANT_LEADER,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupAssistantLeaderOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_CHANGE_SUB_GROUP,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupChangeSubGroupOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_DISBAND,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupDisbandOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_INVITE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupInviteOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_INVITE_RESPONSE,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupInviteResponseOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_RAID_CONVERT,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupRaidConvertOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_REQUEST_JOIN_UPDATES,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupRequestJoinUpdates);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_SET_LEADER,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupSetLeaderOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_SET_ROLES,                                                 STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleGroupSetRolesOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GROUP_UNINVITE_GUID,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupUninviteGuidOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ACCEPT,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildAcceptOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ACHIEVEMENT_MEMBERS,                                       STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ACHIEVEMENT_PROGRESS_QUERY,                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildAchievementProgressQuery);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ADD_RANK,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildAddRankOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ASSIGN_MEMBER_RANK,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildAssignRankOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANKER_ACTIVATE,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankerActivate);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_BUY_TAB,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankBuyTab);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_DEPOSIT_MONEY,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankDepositMoney);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_LOG_QUERY,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankLogQuery);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_MONEY_WITHDRAWN_QUERY,                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankMoneyWithdrawn);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_NOTE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankTabNote);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_QUERY_TAB,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankQueryTab);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_QUERY_TEXT,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryGuildBankTabText);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankSwapItems);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_UPDATE_TAB,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankUpdateTab);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_BANK_WITHDRAW_MONEY,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildBankWithdrawMoney);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_DECLINE,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildDeclineOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_DEL_RANK,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildDelRankOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_DEMOTE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildDemoteOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_DISBAND,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildDisbandOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_EVENT_LOG_QUERY,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildEventLogQueryOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_INFO_TEXT,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildChangeInfoTextOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_INVITE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildInviteOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_LEAVE,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildLeaveOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_MOTD,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildMOTDOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_NEWS_UPDATE_STICKY,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildNewsUpdateStickyOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_PERMISSIONS,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildPermissions);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_PROMOTE,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildPromoteOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_QUERY,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildQueryOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_QUERY_NEWS,                                                STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleGuildQueryNewsOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_QUERY_RANKS,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildQueryRanksOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_REMOVE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildRemoveOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_REPLACE_GUILD_MASTER,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildReplaceGuildMaster);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_REQUEST_CHALLENGE_UPDATE,                                  STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleGuildRequestChallengeUpdate);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_REQUEST_PARTY_STATE,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildRequestPartyState);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_ROSTER,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildRosterOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_SET_ACHIEVEMENT_TRACKING,                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildSetAchievementTracking);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_SET_GUILD_MASTER,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildSetGuildMaster);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_SET_NOTE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildSetNoteOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_SET_RANK_PERMISSIONS,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildSetRankPermissionsOpcode);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_GUILD_SWITCH_RANK,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildSwitchRank);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_IGNORE_TRADE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleIgnoreTradeOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_INITIATE_TRADE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleInitiateTradeOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_INSPECT,                                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleInspectOpcode);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_INSPECT_HONOR_STATS,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleInspectHonorStatsOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_INSTANCE_LOCK_WARNING_RESPONSE,                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleInstanceLockResponse);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ITEM_REFUND,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleItemRefund);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ITEM_REFUND_INFO,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleItemRefundInfoRequest);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ITEM_TEXT_QUERY,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleItemTextQuery);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_JOIN_CHANNEL,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleJoinChannel);                      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_KEEP_ALIVE,                                                      STATUS_NEVER,                        PROCESS_THREADUNSAFE,       &WorldSession::Handle_EarlyProccess);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_KEYBOUND_OVERRIDE,                                               STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LEARN_TALENT,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLearnTalentOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LEARN_PET_SPECIALIZATION_GROUP,                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLearnPetSpecialization);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LEAVE_CHANNEL,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLeaveChannel);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_GET_STATUS,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleLfgGetStatus);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_JOIN,                                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgJoinOpcode);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_LEAVE,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgLeaveOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_LOCK_INFO_REQUEST,                                           STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleLfgGetLockInfoOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_PROPOSAL_RESULT,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgProposalResultOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_SET_BOOT_VOTE,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgSetBootVoteOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_SET_ROLES,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgSetRolesOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LFG_TELEPORT,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLfgTeleportOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_ADD_APPLICATION,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderAddApplication);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_BROWSE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderBrowse);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_DECLINE_RECRUIT,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderDeclineRecruit);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_GET_APPLICATIONS,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderGetApplications);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_GET_RECRUITS,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderGetRecruits);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_POST_REQUEST,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderPostRequest);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_REMOVE_APPLICATION,                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderRemoveApplication);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LF_GUILD_SET_GUILD_POST,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildFinderSetGuildPost);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LIST_INVENTORY,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleListInventoryOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LIST_PETS,                                                       STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleListPets);
    DEFINE_HANDLER(CMSG_LOAD_SCREEN,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleLoadScreenOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOGOUT_CANCEL,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLogoutCancelOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOGOUT_REQUEST,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLogoutRequestOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOG_DISCONNECT,                                                  STATUS_NEVER,                        PROCESS_INPLACE,            &WorldSession::Handle_EarlyProccess);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOG_STREAMING_ERROR,                                             STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleLogStreamingError);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT,                                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootOpcode);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT_MASTER_GIVE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootMasterGiveOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT_METHOD,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootMethodOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT_MONEY,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootMoneyOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT_RELEASE,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootReleaseOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_LOOT_ROLL,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleLootRoll);                         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_CREATE_TEXT_ITEM,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailCreateTextItem);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_DELETE,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailDelete);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_MARK_AS_READ,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailMarkAsRead);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_QUERY_NEXT_TIME,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryNextMailTime);
    DEFINE_HANDLER(CMSG_MAIL_RETURN_TO_SENDER,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailReturnToSender);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_TAKE_ITEM,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailTakeItem);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MAIL_TAKE_MONEY,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMailTakeMoney);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_GUILD,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_INSTANCE_CHAT,                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_OFFICER,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_PARTY,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_RAID,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_ADDON_WHISPER,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleAddonMessagechatOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_AFK,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_CHANNEL,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_DND,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_EMOTE,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_GUILD,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_INSTANCE,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_OFFICER,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_PARTY,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_RAID,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_RAID_WARNING,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_SAY,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_WHISPER,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MESSAGECHAT_YELL,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMessagechatOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MINIMAP_PING,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMinimapPingOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MISSILE_TRAJECTORY_COLLISION,                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMissileTrajectoryCollision);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOUNTSPECIAL_ANIM,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMountSpecialAnimOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK,                                   STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMovementForceAck);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_CHNG_TRANSPORT,                                             STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMovementOpcodes);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FALL_RESET,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMovementOpcodes);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FEATHER_FALL_ACK,                                           STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleFeatherFallAck);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK,                         STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK,                              STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK,                                STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK,                            STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK,                                 STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK,                           STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK,                                STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK,                                 STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK,                                STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleForceSpeedChangeAck);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_GRAVITY_DISABLE_ACK,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMoveGravityAck);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_GRAVITY_ENABLE_ACK,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMoveGravityAck);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_HOVER_ACK,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMoveHoverAck);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_KNOCK_BACK_ACK,                                             STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveKnockBackAck);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_REMOVE_MOVEMENT_FORCE_ACK,                                  STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMovementForceAck);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SET_CAN_FLY_ACK,                                            STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveSetCanFlyAckOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SET_CAN_TURN_WHILE_FALLING_ACK,                             STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveSetCanTurnWhileFallingAck);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK,                STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SET_COLLISION_HEIGHT_ACK,                                   STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleSetCollisionHeightAck);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SET_FLY,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMovementOpcodes);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_SPLINE_DONE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveSplineDoneOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_TELEPORT_ACK,                                               STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveTeleportAck);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_TIME_SKIPPED,                                               STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleMoveTimeSkippedOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_MOVE_WATER_WALK_ACK,                                             STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleMoveWaterWalkAck);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_NAME_QUERY,                                                      STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleNameQueryOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REALM_NAME_QUERY,                                                STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleRealmNameQueryOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_NEXT_CINEMATIC_CAMERA,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleNextCinematicCamera);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_NPC_TEXT_QUERY,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleNpcTextQueryOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_OBJECT_UPDATE_FAILED,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleObjectUpdateFailedOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_OFFER_PETITION,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleOfferPetitionOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_OPENING_CINEMATIC,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleOpeningCinematic);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_OPEN_ITEM,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleOpenItemOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_OPT_OUT_OF_LOOT,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleOptOutOfLootOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PAGE_TEXT_QUERY,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePageTextQueryOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_BUY,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionBuyOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_DECLINE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionDeclineOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_RENAME,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionRenameOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_QUERY,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionQueryOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_SHOWLIST,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionShowListOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_SHOW_SIGNATURES,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionShowSignOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PETITION_SIGN,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetitionSignOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_ABANDON,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetAbandon);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_ACTION,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetAction);                        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_FINAL_NOTIFY,                                         STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_INPUT,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetBattleInput);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH_RESULT,                           STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_QUIT_NOTIFY,                                          STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_REQUEST_PVP,                                          STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_REQUEST_UPDATE,                                       STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_REQUEST_WILD,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetBattleRequestWild);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_BATTLE_SET_FRONT_PET,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetBattleSetFrontPet);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_CANCEL_AURA,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetCancelAuraOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_CAST_SPELL,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetCastSpellOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_NAME_QUERY,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetNameQuery);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_RENAME,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetRename);                        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_SET_ACTION,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetSetAction);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_SPELL_AUTOCAST,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetSpellAutocastOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PET_STOP_ATTACK,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePetStopAttack);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PING,                                                            STATUS_NEVER,                        PROCESS_INPLACE,            &WorldSession::Handle_EarlyProccess);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PLAYED_TIME,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePlayedTime);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PLAYER_LOGIN,                                                    STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandlePlayerLoginOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PLAYER_VEHICLE_ENTER,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleEnterPlayerVehicle);
    DEFINE_HANDLER(CMSG_PUSHQUESTTOPARTY,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePushQuestToParty);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_PVP_LOG_DATA,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandlePVPLogDataOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_COUNTDOWN_TIMER,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryCountdownTimer);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_GUILD_MEMBERS_FOR_RECIPE,                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryGuildMembersForRecipe);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_GUILD_MEMBER_RECIPES,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryGuildMemberRecipes);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_GUILD_RECIPES,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryGuildRecipes);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_GUILD_REWARDS,                                             STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleGuildRewardsQueryOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_GUILD_XP,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGuildQueryXPOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_INSPECT_ACHIEVEMENTS,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryInspectAchievements);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_SCENARIO_POI,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryScenarioPOI);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUERY_TIME,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQueryTimeOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_ACCEPT_QUEST,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverAcceptQuestOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_CHOOSE_REWARD,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverChooseRewardOpcode);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_COMPLETE_QUEST,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverCompleteQuest);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_HELLO,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverHelloOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_QUERY_QUEST,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverQueryQuestOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_REQUEST_REWARD,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverRequestRewardOpcode);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY,                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestgiverStatusMultipleQuery);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTGIVER_STATUS_QUERY,                                         STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleQuestgiverStatusQueryOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUESTLOG_REMOVE_QUEST,                                           STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestLogRemoveQuest);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEST_CONFIRM_ACCEPT,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestConfirmAccept);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEST_NPC_QUERY,                                                 STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleQuestNPCQuery);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEST_POI_QUERY,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestPOIQuery);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEST_PUSH_RESULT,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestPushResult);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEST_QUERY,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleQuestQueryOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_QUEUED_MESSAGES_END,                                             STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RAID_READY_CHECK,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRaidReadyCheckOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RAID_READY_CHECK_CONFIRM,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRaidReadyCheckConfirmOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RAID_TARGET_UPDATE,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRaidTargetUpdateOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RANDOM_ROLL,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRandomRollOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RANDOMIZE_CHAR_NAME,                                             STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleRandomizeCharNameOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_READY_FOR_ACCOUNT_DATA_TIMES,                                    STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleReadyForAccountDataTimes);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_READ_ITEM,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReadItem);                         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RECLAIM_CORPSE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReclaimCorpseOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REFORGE_ITEM,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReforgeItemOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REORDER_CHARACTERS,                                              STATUS_AUTHED,                       PROCESS_INPLACE,            &WorldSession::HandleReorderCharacters);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REPAIR_ITEM,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRepairItemOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REPOP_REQUEST,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRepopRequestOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REPORT_PVP_AFK,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReportPvPAFK);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_ACCOUNT_DATA,                                            STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestAccountData);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_CATEGORY_COOLDOWNS,                                      STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestCategoryCooldowns);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_CEMETERY_LIST,                                           STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestCemeteryList);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS,                              STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestConquestFormulaConstants);  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_FORCED_REACTIONS,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestForcedReactionsOpcode);     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_HOTFIX,                                                  STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestHotfix);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_INSPECT_RATED_BG_STATS,                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleInspectRatedBGStatsOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_PARTY_MEMBER_STATS,                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestPartyMemberStatsOpcode);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_PET_INFO,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestPetInfoOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_PVP_OPTIONS_ENABLED,                                     STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestPvpOptions);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_PVP_REWARDS,                                             STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestPvpReward);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_RAID_INFO,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestRaidInfoOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_RATED_BG_STATS,                                          STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleRequestRatedBgStats);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_RESEARCH_HISTORY,                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleArcheologyRequestHistory);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_VEHICLE_EXIT,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleRequestVehicleExit);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_VEHICLE_NEXT_SEAT,                                       STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::HandleChangeSeatsOnControlledVehicle);   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_VEHICLE_PREV_SEAT,                                       STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::HandleChangeSeatsOnControlledVehicle);   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_REQUEST_VEHICLE_SWITCH_SEAT,                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleChangeSeatsOnControlledVehicle);   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RESET_CHALLENGE_MODE,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleResetChallengeMode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RESET_INSTANCES,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleResetInstancesOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RESURRECT_RESPONSE,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleResurrectResponseOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_RETURN_TO_GRAVEYARD,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleReturnToGraveyard);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_ROLE_POLL_BEGIN,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupInitiatePollRole);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SAVE_CUF_PROFILES,                                               STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSaveCUFProfiles);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SAVE_GUILD_EMBLEM,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSaveGuildEmblemOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SCENE_PLAYBACK_CANCELED,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleScenePlaybackCanceled);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SCENE_PLAYBACK_COMPLETE,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleScenePlaybackComplete);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SCENE_TRIGGER_EVENT,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSceneTriggerEvent);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SELF_RES,                                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSelfResOpcode);
    DEFINE_HANDLER(CMSG_SELL_ITEM,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSellItemOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SELECT_FACTION,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSelectFactionOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SEND_MAIL,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSendMail);                         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SETSHEATHED,                                                     STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSetSheathedOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_ACHIEVEMENTS_HIDDEN,                                         STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_ACTIONBAR_TOGGLES,                                           STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleSetActionBarToggles);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_ACTION_BUTTON,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetActionButtonOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_ACTIVE_MOVER,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetActiveMoverOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_ADVANCED_COMBAT_LOGGING,                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetAdvancedCombatLogging);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_CONTACT_NOTES,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetContactNotesOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_CURRENCY_FLAGS,                                              STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleChangeCurrencyFlags);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_DUNGEON_DIFFICULTY,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetDungeonDifficultyOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_EVERYONE_IS_ASSISTANT,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleGroupEveryoneIsAssistantOpcode);   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_FACTION_ATWAR,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetFactionAtWar);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_FACTION_INACTIVE,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetFactionInactiveOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_FACTION_NOTATWAR,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetFactionNotAtWar);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_LFG_BONUS_FACTION_ID,                                        STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSetLfgBonusFactionId);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_LOOT_SPECIALIZATION,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetLootSpecialization);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_PET_SLOT,                                                    STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSetPetSlot);
    DEFINE_HANDLER(CMSG_SET_PLAYER_DECLINED_NAMES,                                       STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleSetPlayerDeclinedNames);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_PRIMARY_TALENT_TREE,                                         STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandeSetTalentSpecialization);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_PVP,                                                         STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSetPvP);                           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_RAID_DIFFICULTY,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetRaidDifficultyOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_SAVED_INSTANCE_EXTEND,                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetSavedInstanceExtend);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_SELECTION,                                                   STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSetSelectionOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_TAXI_BENCHMARK_MODE,                                         STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetTaxiBenchmarkOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_TITLE,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetTitleOpcode);
    DEFINE_HANDLER(CMSG_SET_TRADE_CURRENCY,                                              STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);
    DEFINE_HANDLER(CMSG_SET_TRADE_GOLD,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetTradeGoldOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_TRADE_ITEM,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetTradeItemOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_VEHICLE_REC_ID_ACK,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleMoveSetVehicleRecAck);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SET_WATCHED_FACTION,                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSetWatchedFactionOpcode);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SHOWING_CLOAK,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleShowingCloakOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SHOWING_HELM,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleShowingHelmOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SHOW_TRADE_SKILL,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleShowTradeSkill);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SOCKET_GEMS,                                                     STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSocketOpcode);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SPELLCLICK,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSpellClick);                       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SPIRIT_HEALER_ACTIVATE,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSpiritHealerActivateOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SPLIT_ITEM,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSplitItemOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_STANDSTATECHANGE,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleStandStateChangeOpcode);           // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SUBMIT_BUG,                                                      STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleSubmitBugOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SUBMIT_COMPLAIN,                                                 STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);
    DEFINE_HANDLER(CMSG_SUGGESTION_SUBMIT,                                               STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SUMMON_RESPONSE,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSummonResponseOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SUSPEND_TOKEN_RESPONSE,                                          STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SWAP_INV_ITEM,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSwapInvItemOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_SWAP_ITEM,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleSwapItem);                         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TABARD_VENDOR_ACTIVATE,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTabardVendorActivateOpcode);       // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TAXI_NODE_STATUS_QUERY,                                          STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleTaxiNodeStatusQueryOpcode);        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TAXI_QUERY_AVAILABLE_NODES,                                      STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleTaxiQueryAvailableNodes);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TEXT_EMOTE,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTextEmoteOpcode);                  // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TIME_SYNC_RESP,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADSAFE,         &WorldSession::HandleTimeSyncResp);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TIME_SYNC_RESPONSE_DROPPED,                                      STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TOGGLE_PVP,                                                      STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTogglePvP);                        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TOTEM_DESTROYED,                                                 STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTotemDestroyed);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TRAINER_BUY_SPELL,                                               STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTrainerBuySpellOpcode);            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TRAINER_LIST,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTrainerListOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TRANSMOGRIFY_ITEMS,                                              STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTransmogrifyItems);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TURN_IN_PETITION,                                                STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTurnInPetitionOpcode);             // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TUTORIAL_CLEAR,                                                  STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::HandleTutorialClear);
    DEFINE_HANDLER(CMSG_TUTORIAL_FLAG,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTutorialFlag);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_TUTORIAL_RESET,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleTutorialReset);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UNACCEPT_TRADE,                                                  STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleUnacceptTradeOpcode);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UNLEARN_SKILL,                                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleUnlearnSkillOpcode);               // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UNLEARN_SPECIALIZATION,                                          STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleUnlearnSpecializationOpcode);      // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UNREGISTER_ALL_ADDON_PREFIXES,                                   STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleUnregisterAddonPrefixesOpcode);    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UPDATE_ACCOUNT_DATA,                                             STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleUpdateAccountData);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UPDATE_CLIENT_SETTINGS,                                          STATUS_UNHANDLED,                    PROCESS_THREADUNSAFE,       &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_UPGRADE_ITEM,                                                    STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleUpgradeItemOpcode);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_USED_FOLLOW,                                                     STATUS_UNHANDLED,                    PROCESS_INPLACE,            &WorldSession::Handle_NULL);                            // 5.4.8 18414
    DEFINE_HANDLER(CMSG_USE_ITEM,                                                        STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleUseItemOpcode);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VIOLENCE_LEVEL,                                                  STATUS_AUTHED,                       PROCESS_INPLACE,            &WorldSession::HandleViolenceLevel);                    // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VOICE_SESSION_ENABLE,                                            STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleVoiceSessionEnableOpcode);         // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VOID_STORAGE_QUERY,                                              STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleVoidStorageQuery);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VOID_STORAGE_TRANSFER,                                           STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleVoidStorageTransfer);              // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VOID_STORAGE_UNLOCK,                                             STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleVoidStorageUnlock);                // 5.4.8 18414
    DEFINE_HANDLER(CMSG_VOID_SWAP_ITEM,                                                  STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleVoidSwapItem);                     // 5.4.8 18414
    DEFINE_HANDLER(CMSG_WARDEN_DATA,                                                     STATUS_AUTHED,                       PROCESS_THREADUNSAFE,       &WorldSession::HandleWardenDataOpcode);                 // 5.4.8 18414
    DEFINE_HANDLER(CMSG_WHO,                                                             STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleWhoOpcode);                        // 5.4.8 18414
    DEFINE_HANDLER(CMSG_WORLD_STATE_UI_TIMER_UPDATE,                                     STATUS_LOGGEDIN,                     PROCESS_INPLACE,            &WorldSession::HandleWorldStateUITimerUpdate);          // 5.4.8 18414
    DEFINE_HANDLER(CMSG_WRAP_ITEM,                                                       STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleWrapItemOpcode);                   // 5.4.8 18414
    DEFINE_HANDLER(CMSG_CORPSE_QUERY,                                                    STATUS_LOGGEDIN,                     PROCESS_THREADUNSAFE,       &WorldSession::HandleCorpseQueryOpcode);                // 5.4.8 18414
#undef DEFINE_HANDLER

#define DEFINE_SERVER_OPCODE_HANDLER(opcode, status) \
    ValidateAndSetServerOpcode(opcode, #opcode, status)

    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACCOUNT_CRITERIA_UPDATE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACCOUNT_CRITERIA_UPDATE_ALL,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACCOUNT_DATA_TIMES,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACHIEVEMENT_DELETED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACHIEVEMENT_EARNED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACTION_BUTTONS,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ACTIVATE_TAXI_REPLY,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ADDON_INFO,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ADD_LOSS_OF_CONTROL,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ADD_RUNE_POWER,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AE_LOOT_TARGETS,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AI_REACTION,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ALL_ACHIEVEMENT_DATA,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ARCHAEOLOGY_SURVERY_CAST,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AREA_SPIRIT_HEALER_TIME,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AREA_TRIGGER_DENIED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ARENA_ERROR,                                             STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ARENA_PREP_OPPONENT_SPECIALIZATIONS,                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ARENA_UNIT_DESTROYED,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ATTACKERSTATEUPDATE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ATTACKSTART,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ATTACKSTOP,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ATTACK_SWING_ERROR,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ATTACK_SWING_LANDED_LOG,                                 STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_LIST_RESULT,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_NOTIFICATION,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_COMMAND_RESULT,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_HELLO,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_LIST_PENDING_SALES,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_LIST_RESULT,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_OWNER_LIST_RESULT,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUCTION_OWNER_NOTIFICATION,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AURA_UPDATE,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUTH_CHALLENGE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_AUTH_RESPONSE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BARBER_SHOP_RESULT,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_LIST,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECTED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTERED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE,                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_RATED_INFO,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_QUEUED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_ACTIVE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NEEDCONFIRMATION,                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_FAILED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_JOINED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_LEFT,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_QUEUE_STATUS,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_POSITIONS,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_ACK_FAILED,                                   STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_CONFIRM_PURCHASE,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_DELIVERY_ENDED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_DELIVERY_STARTED,                             STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_DISTRIBUTION_UPDATE,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE,               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE,                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE,                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_PURCHASE_UPDATE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_DISTRIBUTION_ASSIGN_TO_TARGET_RESPONSE, STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE,                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_DELETED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED,                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_DENINED,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_QUERY_NAME_RESPONSE,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_SLOT_UPDATE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BATTLE_PET_UPDATE,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BINDER_CONFIRM,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BINDPOINTUPDATE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BLACK_MARKET_BID_ON_ITEM_RESULT,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BLACK_MARKET_OPEN_RESULT,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BLACK_MARKET_OUTBID,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT,                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BLACK_MARKET_WON,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BREAK_TARGET,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BUY_FAILED,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_BUY_ITEM,                                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_CLEAR_PENDING_ACTION,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_COMMAND_RESULT,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INITIAL_INVITE,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_ALERT,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT,                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT,                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_MODERATOR_STATUS,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_REMOVED_ALERT,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_UPDATED_ALERT,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_ADDED,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_SEND_CALENDAR,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_SEND_EVENT,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CALENDAR_SEND_NUM_PENDING,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CANCEL_AUTO_REPEAT,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CANCEL_COMBAT,                                           STATUS_NEVER);     // 5.4.8 18414  0x0E8B, skyfire is 0x0534
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CANCEL_SCENE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CAST_FAILED,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_ALL_MAP_STATS,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_COMPLETE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD,                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT,                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REWARDS,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHANNEL_LIST,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHARACTER_UPGRADE_COMPLETE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHARACTER_UPGRADE_STARTED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_CREATE,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_DELETE,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_ENUM,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_FACTION_CHANGE_RESULT,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAR_RENAME,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAT_PLAYER_AMBIGUOUS,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAT_PLAYER_NOT_FOUND,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CHAT_RESTRICTED,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_ALL_SPELL_CHARGES,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWN,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWNS,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_QUEST_COMPLETED_BIT,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_QUEST_COMPLETED_BITS,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_SPELL_CHARGES,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_LOSS_OF_CONTROL,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLEAR_TARGET,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLIENTCACHE_VERSION,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CLIENT_CONTROL_UPDATE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CONNECT_TO,                                              STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CONQUEST_FORMULA_CONSTANTS,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CONTACT_LIST,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CONVERT_RUNE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_COOLDOWN_CHEAT,                                          STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_COOLDOWN_EVENT,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE,                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CORPSE_NOT_IN_INSTANCE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CORPSE_QUERY,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CORPSE_RECLAIM_DELAY,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CREATURE_QUERY_RESPONSE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CRITERIA_DELETED,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CRITERIA_UPDATE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CROSSED_INEBRIATION_THRESHOLD,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_CUSTOM_LOAD_SCREEN,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DANCE_STUDIO_CREATE_RESULT,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DB_REPLY,                                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DEATH_RELEASE_LOC,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DEBUG_RUNE_REGEN,                                        STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DEFENSE_MESSAGE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DESTROY_OBJECT,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISMOUNT,                                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISPEL_FAILED,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISPLAY_GAME_ERROR,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISPLAY_PLAYER_CHOICE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISPLAY_PROMOTION,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DISPLAY_TOAST,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_COMPLETE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_COUNTDOWN,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_INBOUNDS,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_OUTOFBOUNDS,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_REQUESTED,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DUEL_WINNER,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_DURABILITY_DAMAGE_DEATH,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_EMOTE,                                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ENABLE_BARBER_SHOP,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ENCHANTMENT_LOG,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ENVIRONMENTALDAMAGELOG,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_ID,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_EXPLORATION_EXPERIENCE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS_GLUE_SCREEN,                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FEIGN_DEATH_RESISTED,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FISH_ESCAPED,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FISH_NOT_HOOKED,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FLIGHT_SPLINE_SYNC,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_FRIEND_STATUS,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GAMEOBJECT_CUSTOM_ANIM,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GAMEOBJECT_DESPAWN_ANIM,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GAMEOBJECT_PAGETEXT,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GAMEOBJECT_QUERY_RESPONSE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT,                           STATUS_NEVER);     // 5.4.8 18414 not in skyfire
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GM_TICKET_RESPONSE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GM_TICKET_UPDATE,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GM_TICKET_GET_TICKET,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GM_TICKET_SYSTEM_STATUS,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GM_TICKET_CASE_STATUS,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GOSSIP_COMPLETE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GOSSIP_MESSAGE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GOSSIP_POI,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_DECLINE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_DESTROYED,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_INVITE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_LIST,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_ROLE_POLL_INFORM,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_SET_LEADER,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_SET_ROLE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GROUP_UNINVITE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DATA,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DELETED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_EARNED,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_MEMBERS,                               STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_BANK_LIST,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_BANK_LOG_QUERY_RESULT,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_BANK_MONEY_WITHDRAWN,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_TEXT_RESULT,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_COMPLETED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_UPDATED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_COMMAND_RESULT,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DATA,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DELETED,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_MONEY_CHANGED,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_CONTENTS_CHANGED,                       STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_TAB_ADDED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_TAB_DELETED,                            STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_TAB_TEXT_CHANGED,                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_TAB_MODIFIED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_DISBANDED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_LOG_QUERY_RESULT,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_MOTD,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_NEW_LEADER,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_JOINED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_LEFT,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_PRESENCE_CHANGE,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANK_CHANGED,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANKS_UPDATED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_FLAGGED_FOR_RENAME,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_INVITE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_INVITE_CANCEL,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_INVITE_DECLINED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_MAX_DAILY_XP,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_MEMBERS_FOR_RECIPE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_MEMBER_DAILY_RESET,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_MEMBER_RECIPES,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_MEMBER_UPDATE_NOTE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_NEWS_DELETED,                                      STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_NEWS_UPDATE,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_PARTY_STATE_RESPONSE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_QUERY_RESPONSE,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_RANK,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_RANKS_UPDATE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_RECIPES,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_RENAMED,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_REPUTATION_WEEKLY_CAP,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_REWARDS_LIST,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_ROSTER,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_XP,                                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_GUILD_XP_GAIN,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_HEALTH_UPDATE,                                           STATUS_NEVER);     // 5.4.8 18414 from skyfire
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_HIGHEST_THREAT_UPDATE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_HOTFIX_INFO,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INITIALIZE_FACTIONS,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INITIAL_SETUP,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INITIAL_SPELLS,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INIT_CURRENCY,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INIT_WORLD_STATES,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSPECT_HONOR_STATS,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSPECT_RATED_BG_STATS,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSPECT_RESULTS,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSTANCE_LOCK_WARNING_QUERY,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSTANCE_RESET,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSTANCE_RESET_FAILED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INSTANCE_SAVE_CREATED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INVALIDATE_PLAYER,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_INVENTORY_CHANGE_FAILURE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_COOLDOWN,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_ENCHANT_TIME_UPDATE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_EXPIRE_PURCHASE_REFUND,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_PURCHASE_REFUND_RESULT,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_PUSH_RESULT,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_TEXT_QUERY_RESPONSE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_TIME_UPDATE,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ITEM_UPGRADE_RESULT,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LEARNED_SPELL,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LEVELUP_INFO,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_BOOT_PROPOSAL_UPDATE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_DISABLED,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_JOIN_RESULT,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_OFFER_CONTINUE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_PARTY_INFO,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_PLAYER_INFO,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_PLAYER_REWARD,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_PROPOSAL_UPDATE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_QUEUE_STATUS,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_ROLE_CHECK_UPDATE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_ROLE_CHOSEN,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_SLOT_INVALID,                                        STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_TELEPORT_DENIED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LFG_UPDATE_STATUS,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICANT_LIST_UPDATED,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICATIONS_LIST_CHANGED,                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_BROWSE_UPDATED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_COMMAND_RESULT,                                 STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_MEMBERSHIP_LIST_UPDATED,                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_POST_UPDATED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LF_GUILD_RECRUIT_LIST_UPDATED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LIST_INVENTORY,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOAD_CUF_PROFILES,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOAD_EQUIPMENT_SET,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOGIN_SETTIMESPEED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOGIN_VERIFY_WORLD,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOGOUT_CANCEL_ACK,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOGOUT_COMPLETE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOGOUT_RESPONSE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOG_XPGAIN,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_ALL_PASSED,                                         STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_CLEAR_MONEY,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_CONTENTS,                                           STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_ITEM_NOTIFY,                                        STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_LIST,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_MASTER_LIST,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_MONEY_NOTIFY,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_RELEASE_ALL,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_RELEASE_RESPONSE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_REMOVED,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_RESPONSE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_ROLL,                                               STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_ROLL_WON,                                           STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_ROLLS_COMPLETE,                                     STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOOT_START_ROLL,                                         STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_LOSS_OF_CONTROL_AURA_UPDATE,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MAIL_LIST_RESULT,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MAIL_QUERY_NEXT_TIME_RESULT,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MAP_OBJ_EVENTS,                                          STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MESSAGECHAT,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MINIMAP_PING,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_COMPONENTED_DATA,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_CREATURE_DATA,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MISSILE_CANCEL,                                          STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MODIFY_COOLDOWN,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MONEY_NOTIFY,                                            STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MONSTER_MOVE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOTD,                                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOUNTSPECIAL_ANIM,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_APPLY_MOVEMENT_FORCE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_COLLISION_DISABLE,                                  STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_COLLISION_ENABLE,                                   STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_FEATHER_FALL,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_GRAVITY_DISABLE,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_GRAVITY_ENABLE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_KNOCK_BACK,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_LAND_WALK,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_NORMAL_FALL,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_REMOVE_MOVEMENT_FORCE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_ROOT,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_ACTIVE_MOVER,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_FLY,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TURN_WHILE_FALLING,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,            STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_COLLISION_HEIGHT,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_COMPOUND_STATE,                                 STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_BACK_SPEED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_SPEED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_HOVER,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_PITCH_RATE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_BACK_SPEED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_SPEED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_BACK_SPEED,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_SPEED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_TURN_RATE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_VEHICLE_REC_ID,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_SET_WALK_SPEED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_TELEPORT,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UNROOT,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_FLY,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TURN_WHILE_FALLING,                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,          STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UNSET_HOVER,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_APPLY_MOVEMENT_FORCE,                        STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_COLLISION_HEIGHT,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_SPEED,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_KNOCK_BACK,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_PITCH_RATE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_REMOVE_MOVEMENT_FORCE,                       STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_BACK_SPEED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_SPEED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_BACK_SPEED,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_SPEED,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TELEPORT,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TURN_RATE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_UPDATE_WALK_SPEED,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MOVE_WATER_WALK,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NAME_QUERY_RESPONSE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REALM_NAME_QUERY_RESPONSE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NEW_TAXI_PATH,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NEW_WORLD,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NEW_WORLD_ABORT,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NOTIFICATION,                                            STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NOTIFY_MISSILE_TRAJECTORY_COLLISION,                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_NPC_TEXT_UPDATE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA,                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_OPEN_REFORGE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_OVERRIDE_LIGHT,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PAGE_TEXT_QUERY_RESPONSE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PARTYKILLLOG,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PARTY_COMMAND_RESULT,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATS,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_ALREADY_SIGNED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_QUERY_RESPONSE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_RENAME_RESULT,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_SHOWLIST,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_SHOW_SIGNATURES,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PETITION_SIGN_RESULTS,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_ACTION_FEEDBACK,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_ACTION_SOUND,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_INITIAL_UPDATE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_FINALIZE_LOCATION,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_FINAL_ROUND,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_FINISHED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_FIRST_ROUND,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_REQUEST_FAILED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_BATTLE_ROUND_RESULT,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_CAST_FAILED,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_LEARNED_SPELL,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_NAME_INVALID,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_NAME_QUERY_RESPONSE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_REMOVED_SPELL,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_SPELLS_MESSAGE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_STABLE_LIST,                                         STATUS_NEVER);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_STABLE_RESULT,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_TAME_FAILURE,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PET_UPDATE_COMBO_POINTS,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAYED_TIME,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAYERBOUND,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAYER_DIFFICULTY_CHANGE,                                STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAYER_MOVE,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_MUSIC,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_OBJECT_SOUND,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_ONE_SHOT_ANIM_KIT,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_SCENE,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_SOUND,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL_KIT,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PONG,                                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_POWER_UPDATE,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PRE_RESURRECT,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PROC_RESIST,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PROPOSE_LEVEL_GRANT,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PVP_CREDIT,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PVP_LOG_DATA,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PVP_OPTIONS_ENABLED,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_PVP_SEASON,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUERY_TIME_RESPONSE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_OFFER_REWARD,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_COMPLETE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_DETAILS,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_FAILED,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_INVALID,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_LIST,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_REQUEST_ITEMS,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_STATUS,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTGIVER_STATUS_MULTIPLE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTLOG_FULL,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTUPDATE_ADD_CREDIT,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTUPDATE_ADD_CREDIT_SIMPLE,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTUPDATE_ADD_PVP_KILL,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_UPDATE_COMPLETE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTUPDATE_FAILED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUESTUPDATE_FAILEDTIMER,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_CONFIRM_ACCEPT,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_NPC_QUERY_RESPONSE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_POI_QUERY_RESPONSE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_PUSH_RESULT,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_QUEST_QUERY_RESPONSE,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_GROUP_ONLY,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_INSTANCE_INFO,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_INSTANCE_MESSAGE,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_MARKERS_CHANGED,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_READY_CHECK,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_READY_CHECK_COMPLETED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_READY_CHECK_CONFIRM,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_TARGET_UPDATE_ALL,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RAID_TARGET_UPDATE_SINGLE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RANDOM_ROLL,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RANDOMIZE_CHAR_NAME,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_READ_ITEM_RESULT_OK,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_READ_ITEM_RESULT_FAILED,                                 STATUS_NEVER);     // 5.4.8 18414 skyfire version
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REALM_SPLIT,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RECEIVED_MAIL,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_EXPIRED,                                  STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_FAILURE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REFORGE_RESULT,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REMOVE_LOSS_OF_CONTROL,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REMOVED_SPELL,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REQUEST_CEMETERY_LIST_RESPONSE,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_REQUEST_PVP_REWARDS_RESPONSE,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESEARCH_COMPLETE,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESEARCH_SETUP_HISTORY,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESET_FAILED_NOTIFY,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESPEC_WIPE_CONFIRM,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESPOND_INSPECT_ACHIEVEMENTS,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESUME_CAST_BAR,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESUME_COMMS,                                            STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESUME_TOKEN,                                            STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESURRECT_REQUEST,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_RESYNC_RUNES,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SAVE_GUILD_EMBLEM,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SCENARIO_POIS,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SCENARIO_PROGRESS_UPDATE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SCENARIO_STATE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SCRIPT_CAST,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SELL_ITEM,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SEND_MAIL_RESULT,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SEND_SPELL_CHARGES,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SEND_SPELL_HISTORY,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SEND_UNLEARN_SPELLS,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SERVER_FIRST_ACHIEVEMENT,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SERVER_MESSAGE,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_AI_ANIM_KIT,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_DUNGEON_DIFFICULTY,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_FACTION_ATWAR,                                       STATUS_UNHANDLED);
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_FACTION_STANDING,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_FACTION_VISIBLE,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_FLAT_SPELL_MODIFIER,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_FORCED_REACTIONS,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_ITEM_PURCHASE_DATA,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_MELEE_ANIM_KIT,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_MOVEMENT_ANIM_KIT,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PCT_SPELL_MODIFIER,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PET_SPECIALIZATION,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PHASE_SHIFT,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT,                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PLAY_HOVER_ANIM,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_PROFICIENCY,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_QUEST_COMPLETED_BIT,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_RAID_DIFFICULTY,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_TIMEZONE_INFORMATION,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SET_VEHICLE_REC_ID,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SHOW_TAXI_NODES,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SHOW_BANK,                                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SHOW_MAILBOX,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SHOW_NEURTRAL_PLAYER_FACTION_SELECT_UI,                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SHOW_TRADE_SKILL_RESPONSE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SOCKET_GEMS_RESULT,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLDISPELLOG,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLENERGIZELOG,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLHEALLOG,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLINSTAKILLLOG,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLINTERRUPTLOG,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLLOGMISS,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELLNONMELEEDAMAGELOG,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_CATEGORY_COOLDOWN,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_START,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_UPDATE,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_COOLDOWN,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_DAMAGE_SHIELD,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_DELAYED,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_EXECUTE_LOG,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_FAILED_OTHER,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_FAILURE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_GO,                                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_OR_DAMAGE_IMMUNE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_PERIODIC_AURA_LOG,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPELL_START,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPIRIT_HEALER_CONFIRM,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_DISABLE,                           STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_ENABLE,                            STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_DISABLE,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_ENABLE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_ROOT,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FEATHER_FALL,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED,                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED,                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLYING,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_HOVER,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_LAND_WALK,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_NORMAL_FALL,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_PITCH_RATE,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_MODE,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_SPEED,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED,                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_SPEED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_TURN_RATE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_MODE,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_SPEED,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WATER_WALK,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_START_SWIM,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_STOP_SWIM,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNROOT,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_FLYING,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_HOVER,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_STANDSTATE_UPDATE,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMER,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMERS,                                    STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_START_MIRROR_TIMER,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_START_TIMER,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_STOP_ELAPSED_TIMER,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_STOP_MIRROR_TIMER,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_STREAMING_MOVIES,                                        STATUS_NEVER); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SUMMON_REQUEST,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SUPERCEDED_SPELL,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_SUSPEND_TOKEN,                                           STATUS_UNHANDLED); // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TABARD_VENDOR_ACTIVATE,                                  STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TALENTS_INFO,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TAXI_NODE_STATUS,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TEXT_EMOTE,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_THREAT_CLEAR,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_THREAT_REMOVE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_THREAT_UPDATE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TIME_SYNC_REQ,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TITLE_EARNED,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TITLE_LOST,                                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TOTEM_CREATED,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRADE_STATUS,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRADE_STATUS_EXTENDED,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRAINER_BUY_FAILED,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRAINER_LIST,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRANSFER_ABORTED,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRANSFER_PENDING,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRIGGER_CINEMATIC,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TRIGGER_MOVIE,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TURN_IN_PETITION_RESULTS,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_TUTORIAL_FLAGS,                                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_ACCOUNT_DATA,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_COMBO_POINTS,                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY_WEEK_LIMIT,                              STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT,                          STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_OWNERSHIP,                               STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_LAST_INSTANCE,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_OBJECT,                                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_UPDATE_WORLD_STATE,                                      STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_USE_EQUIPMENT_SET_RESULT,                                STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_USERLIST_ADD,                                            STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_USERLIST_REMOVE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_USERLIST_UPDATE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_VOID_ITEM_SWAP_RESPONSE,                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_VOID_STORAGE_CONTENTS,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_VOID_STORAGE_TRANSFER_CHANGES,                           STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_VOID_TRANSFER_RESULT,                                    STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_VIGNETTE_UPDATE,                                         STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WARDEN_DATA,                                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WEATHER,                                                 STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WEEKLY_RESET_CURRENCY,                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WHO,                                                     STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WHOIS,                                                   STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WORLD_SERVER_INFO,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WORLD_STATE_UI_TIMER_UPDATE,                             STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_ZONE_UNDER_ATTACK,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_MULTIPLE_PACKETS,                                        STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_COMPRESSED_PACKET,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WAIT_QUEUE_FINISH,                                       STATUS_NEVER);     // 5.4.8 18414
    DEFINE_SERVER_OPCODE_HANDLER(SMSG_WAIT_QUEUE_UPDATE,                                       STATUS_NEVER);     // 5.4.8 18414

#undef DEFINE_SERVER_OPCODE_HANDLER

#define DEFINE_MSG_OPCODE_HANDLER(opcode, status, processing, handler) \
    ValidateAndSetServerOpcode(OpcodeServer(opcode), #opcode, status); \
    ValidateAndSetClientOpcode<decltype(handler), handler>(OpcodeClient(opcode), #opcode, status, processing)

    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_FALL_LAND,                                        STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_HEARTBEAT,                                        STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_JUMP,                                             STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_SET_FACING,                                       STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_SET_PITCH,                                        STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_SET_RUN_MODE,                                     STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_SET_WALK_MODE,                                    STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_ASCEND,                                     STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_BACKWARD,                                   STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_DESCEND,                                    STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_FORWARD,                                    STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_PITCH_DOWN,                                 STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_PITCH_UP,                                   STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_STRAFE_LEFT,                                STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_STRAFE_RIGHT,                               STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_SWIM,                                       STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_TURN_LEFT,                                  STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_START_TURN_RIGHT,                                 STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP,                                             STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP_ASCEND,                                      STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP_PITCH,                                       STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP_STRAFE,                                      STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP_SWIM,                                        STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_STOP_TURN,                                        STATUS_LOGGEDIN,  PROCESS_THREADSAFE,    &WorldSession::HandleMovementOpcodes);        // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(MSG_MOVE_WORLDPORT_ACK,                                    STATUS_TRANSFER,  PROCESS_THREADUNSAFE,  &WorldSession::HandleMoveWorldportAckOpcode); // 5.4.8 18414
    DEFINE_MSG_OPCODE_HANDLER(CMSG_GMRESPONSE_RESOLVE,                                   STATUS_UNHANDLED, PROCESS_THREADUNSAFE,  &WorldSession::HandleGMResponseResolve);      // 5.4.8 18414 Skyfire
    DEFINE_MSG_OPCODE_HANDLER(MSG_VERIFY_CONNECTIVITY,                                   STATUS_UNHANDLED, PROCESS_INPLACE,       &WorldSession::Handle_EarlyProccess);         // Not an opcode

#undef DEFINE_MSG_OPCODE_HANDLER
}

template<typename T>
inline std::string GetOpcodeNameForLoggingImpl(T id)
{
    uint16 opcode = uint16(id);
    std::ostringstream ss;
    ss << '[';

    if (static_cast<uint32>(id) < NUM_OPCODE_HANDLERS)
    {
        if (OpcodeHandler const* handler = opcodeTable[id])
        {
            ss << handler->Name;
            if (opcode & COMPRESSED_OPCODE_MASK)
                ss << "_COMPRESSED";
        }
        else
            ss << "UNKNOWN OPCODE";
    }
    else
        ss << "INVALID OPCODE";

    ss << " 0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << opcode << std::nouppercase << std::dec << " (" << opcode << ")]";
    return ss.str();
}

std::string GetOpcodeNameForLogging(OpcodeClient opcode)
{
    return GetOpcodeNameForLoggingImpl(opcode);
}

std::string GetOpcodeNameForLogging(OpcodeServer opcode)
{
    return GetOpcodeNameForLoggingImpl(opcode);
}
