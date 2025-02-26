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

/** \file
    \ingroup u2w
*/

#include "WorldSocket.h"                                    // must be first to make ACE happy with ACE includes in it
#include "Config.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "AccountMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include <memory>
#include "Player.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "GameTime.h"
#include "GuildMgr.h"
#include "Group.h"
#include "Guild.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "BattlegroundMgr.h"
#include "OutdoorPvPMgr.h"
#include "MapManager.h"
#include "SocialMgr.h"
#include "zlib.h"
#include "ScriptMgr.h"
#include "Transport.h"
#include "WardenWin.h"
#include "BattlePetMgr.h"
#include "PetBattle.h"
#include "AchievementMgr.h"
#include "ServiceBoost.h"
#include "BattlePayMgr.h"
#include "QueryHolder.h"

namespace
{
    std::string const DefaultPlayerName = "<none>";
} // namespace

bool MapSessionFilter::Process(WorldPacket* packet)
{
    ClientOpcodeHandler const* opHandle = opcodeTable[static_cast<OpcodeClient>(packet->GetOpcode())];

    //let's check if our opcode can be really processed in Map::Update()
    if (opHandle->ProcessingPlace == PROCESS_INPLACE)
        return true;

    //we do not process thread-unsafe packets
    if (opHandle->ProcessingPlace == PROCESS_THREADUNSAFE)
        return false;

    Player* player = m_pSession->GetPlayer();
    if (!player)
        return false;

    //in Map::Update() we do not process packets where player is not in world!
    return player->IsInWorld();
}

//we should process ALL packets when player is not in world/logged in
//OR packet handler is not thread-safe!
bool WorldSessionFilter::Process(WorldPacket* packet)
{
    ClientOpcodeHandler const* opHandle = opcodeTable[static_cast<OpcodeClient>(packet->GetOpcode())];

    //check if packet handler is supposed to be safe
    if (opHandle->ProcessingPlace == PROCESS_INPLACE)
        return true;

    //thread-unsafe packets should be processed in World::UpdateSessions()
    if (opHandle->ProcessingPlace == PROCESS_THREADUNSAFE)
        return true;

    //no player attached? -> our client! ^^
    Player* player = m_pSession->GetPlayer();
    if (!player)
        return true;

    //lets process all packets for non-in-the-world player
    return !player->IsInWorld();
}

/// WorldSession constructor
WorldSession::WorldSession(uint32 id, std::shared_ptr<WorldSocket> sock, AccountTypes sec, uint8 expansion, time_t mute_time, LocaleConstant locale, uint32 recruiter, uint32 flags, bool isARecruiter, bool hasBoost):
    m_muteTime(mute_time),
    m_timeOutTime(0),
    AntiDOS(this),
    _player(nullptr),
    m_Socket(sock),
    _security(sec),
    _accountId(id),
    m_expansion(expansion),
    m_charBooster(new CharacterBooster(this)),
    _warden(nullptr),
    _logoutTime(0),
    m_inQueue(false),
    m_playerLoading(false),
    m_playerLogout(false),
    m_playerRecentlyLogout(false),
    m_playerSave(false),
    m_sessionDbcLocale(sWorld->GetAvailableDbcLocale(locale)),
    m_sessionDbLocaleIndex(locale),
    m_latency(0),
    m_clientTimeDelay(0),
    m_flags(flags),
    m_TutorialsChanged(false),
    _filterAddonMessages(false),
    recruiterId(recruiter),
    isRecruiter(isARecruiter),
    m_hasBoost(hasBoost),
    timeLastWhoCommand(0),
    m_currentVendorEntry(0)
{
    if (sock)
    {
        m_Address = sock->GetRemoteIpAddress().to_string();
        ResetTimeOutTime(false);
        LoginDatabase.PExecute("UPDATE account SET online = 1 WHERE id = %u;", GetAccountId());     // One-time query
    }

    // At current time it will never be removed from container, so pointer must be valid all of the session life time.

    _achievementMgr = std::make_unique<AccountAchievementMgr>();
}

/// WorldSession destructor
WorldSession::~WorldSession()
{
    ///- unload player if not unloaded
    if (_player)
        LogoutPlayer (true);

    /// - If have unclosed socket, close it
    if (m_Socket)
    {
        m_Socket->CloseSocket();
        m_Socket.reset();
    }

    delete _warden;
    delete m_charBooster;

    ///- empty incoming packet queue
    WorldPacket* packet = nullptr;
    while (_recvQueue.next(packet))
        delete packet;

    LoginDatabase.PExecute("UPDATE account SET online = 0 WHERE id = %u;", GetAccountId());     // One-time query

}

std::string const & WorldSession::GetPlayerName() const
{
    return _player != NULL ? _player->GetName() : DefaultPlayerName;
}

std::string WorldSession::GetPlayerInfo() const
{
    std::ostringstream ss;

    ss << "[Player: " << GetPlayerName()
       << " (Guid: " << (_player != NULL ? _player->GetGUID().GetCounter() : 0)
       << ", Account: " << GetAccountId() << ")]";

    return ss.str();
}

ObjectGuid WorldSession::GetGUID() const
{
    return _player ? _player->GetGUID() : ObjectGuid::Empty;
}

/// Get player guid if available. Use for logging purposes only
uint32 WorldSession::GetGuidLow() const
{
    return GetPlayer() ? GetPlayer()->GetGUID().GetCounter() : 0;
}

/// Send a packet to the client
void WorldSession::SendPacket(WorldPacket const* packet, bool forced /*= false*/)
{
    if (packet->GetOpcode() == NULL_OPCODE)
    {
        TC_LOG_ERROR("network.opcode", "Prevented sending of NULL_OPCODE to %s", GetPlayerInfo().c_str());
        return;
    }
    else if (packet->GetOpcode() == UNKNOWN_OPCODE)
    {
        TC_LOG_ERROR("network.opcode", "Prevented sending of UNKNOWN_OPCODE to %s", GetPlayerInfo().c_str());
        return;
    }

    ServerOpcodeHandler const* handler = opcodeTable[static_cast<OpcodeServer>(packet->GetOpcode())];

    if (!handler)
    {
        TC_LOG_ERROR("network.opcode", "Prevented sending of opcode %s with non existing handler to %s",
                     GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str(),
                     GetPlayerInfo().c_str());
        return;
    }

    if (!m_Socket)
    {
        TC_LOG_ERROR("network.opcode", "Prevented sending of %s to non existent socket to %s", GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet->GetOpcode())).c_str(), GetPlayerInfo().c_str());
        return;
    }

    if (!forced)
    {
        OpcodeHandler const* handler = opcodeTable[static_cast<OpcodeServer>(packet->GetOpcode())];
        if (!handler || handler->Status == STATUS_UNHANDLED)
        {
            TC_LOG_ERROR("network.opcode", "Prevented sending disabled opcode %s to %s", GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet->GetOpcode())).c_str(), GetPlayerInfo().c_str());
            return;
        }
    }

#ifdef TRINITY_DEBUG
    // Code for network use statistic
    static uint64 sendPacketCount = 0;
    static uint64 sendPacketBytes = 0;

    static time_t firstTime = GameTime::GetGameTime();
    static time_t lastTime = firstTime;                     // next 60 secs start time

    static uint64 sendLastPacketCount = 0;
    static uint64 sendLastPacketBytes = 0;

    time_t cur_time = GameTime::GetGameTime();

    if ((cur_time - lastTime) < 60)
    {
        sendPacketCount += 1;
        sendPacketBytes += packet->size();

        sendLastPacketCount += 1;
        sendLastPacketBytes += packet->size();
    }
    else
    {
        uint64 minTime = uint64(cur_time - lastTime);
        uint64 fullTime = uint64(lastTime - firstTime);
        TC_LOG_DEBUG("misc", "Send all time packets count: " UI64FMTD " bytes: " UI64FMTD " avr.count/sec: %f avr.bytes/sec: %f time: %u", sendPacketCount, sendPacketBytes, float(sendPacketCount)/fullTime, float(sendPacketBytes)/fullTime, uint32(fullTime));
        TC_LOG_DEBUG("misc", "Send last min packets count: " UI64FMTD " bytes: " UI64FMTD " avr.count/sec: %f avr.bytes/sec: %f", sendLastPacketCount, sendLastPacketBytes, float(sendLastPacketCount)/minTime, float(sendLastPacketBytes)/minTime);

        lastTime = cur_time;
        sendLastPacketCount = 1;
        sendLastPacketBytes = packet->wpos();               // wpos is real written size
    }
#endif                                                      // !TRINITY_DEBUG

    sScriptMgr->OnPacketSend(this, *packet);

    TC_LOG_TRACE("network.opcode", "S->C: %s %s", GetPlayerInfo().c_str(), GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet->GetOpcode())).c_str());
    m_Socket->SendPacket(*packet);
}

/// Add an incoming packet to the queue
void WorldSession::QueuePacket(WorldPacket* new_packet)
{
    // prioritize CMSG_PLAYER_LOGIN
    // sometimes CMSG_PLAYER_LOGIN arrives after hundreds of packets that require STATUS_LOGGEDIN
    // if CMSG_PLAYER_LOGIN is not prioritized, login will never complete
    if (!_player && new_packet->GetOpcode() == CMSG_PLAYER_LOGIN)
        _recvQueue.push_front(new_packet);
    else
        _recvQueue.add(new_packet);
}

/// Logging helper for unexpected opcodes
void WorldSession::LogUnexpectedOpcode(WorldPacket* packet, const char* status, const char *reason)
{
    TC_LOG_ERROR("network.opcode", "Received unexpected opcode %s Status: %s Reason: %s from %s",
         GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str(), status, reason, GetPlayerInfo().c_str());
}

/// Logging helper for unexpected opcodes
void WorldSession::LogUnprocessedTail(WorldPacket const* packet)
{
    if (!sLog->ShouldLog("network.opcode", LOG_LEVEL_TRACE) || packet->rpos() >= packet->wpos())
        return;

    TC_LOG_TRACE("network.opcode", "Unprocessed tail data (read stop at %u from %u) Opcode %s from %s",
        uint32(packet->rpos()), uint32(packet->wpos()), GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str(), GetPlayerInfo().c_str());
    packet->print_storage();
}

struct OpcodeInfo
{
    OpcodeInfo(uint32 nb, uint32 time) : nbPkt(nb), totalTime(time) {}
    uint32 nbPkt;
    uint32 totalTime;
};

/// Update the WorldSession (triggered by World update)
bool WorldSession::Update(uint32 diff, PacketFilter& updater)
{
    /// Update Timeout timer.
    UpdateTimeOutTime(diff);
    std::unordered_map<uint16, OpcodeInfo> pktHandle;
    // Services timers
    GetBoost()->Update(diff);
    sBattlePayMgr->Update(diff);

    ///- Before we process anything:
    /// If necessary, kick the player from the character select screen
    if (IsConnectionIdle())
        m_Socket->CloseSocket();

    ///- Retrieve packets from the receive queue and call the appropriate handlers
    /// not process packets if socket already closed
    WorldPacket* packet = nullptr;
    //! Delete packet after processing by default
    bool deletePacket = true;
    std::vector<WorldPacket*> requeuePackets;
    uint32 _startMSTime = getMSTime();
    uint32 processedPackets = 0;
    time_t currentTime = GameTime::GetGameTime();

    constexpr uint32 MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE = 100;

    while (m_Socket && _recvQueue.next(packet, updater))
    {
        ClientOpcodeHandler const* opHandle = opcodeTable[static_cast<OpcodeClient>(packet->GetOpcode())];
        try
        {
            switch (opHandle->Status)
            {
                case STATUS_LOGGEDIN:
                    if (!_player)
                    {
                        // skip STATUS_LOGGEDIN opcode unexpected errors if player logout sometime ago - this can be network lag delayed packets
                        //! If player didn't log out a while ago, it means packets are being sent while the server does not recognize
                        //! the client to be in world yet. We will re-add the packets to the bottom of the queue and process them later.
                        if (!m_playerRecentlyLogout)
                        {
                            requeuePackets.push_back(packet);
                            deletePacket = false;
                            TC_LOG_DEBUG("network", "Re-enqueueing packet with opcode %s with with status STATUS_LOGGEDIN. "
                                "Player is currently not in world yet.", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str());
                        }
                    }
                    else if (_player->IsInWorld() && AntiDOS.EvaluateOpcode(*packet, currentTime))
                    {
                        auto start = TimeValue::Now();
                        sScriptMgr->OnPacketReceive(this, WorldPacket(*packet));
                        opHandle->Call(this, *packet);
                        LogUnprocessedTail(packet);
                        sWorld->RecordTimeDiffLocal(start, "WorldSession::Update %s %s", opHandle->Name, GetPlayerInfo().c_str());
                    }
                    else
                        processedPackets = MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE;   // break out of packet processing loop
                    // lag can cause STATUS_LOGGEDIN opcodes to arrive after the player started a transfer
                    break;
                case STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT:
                    if (!_player && !m_playerRecentlyLogout && !m_playerLogout) // There's a short delay between _player = null and m_playerRecentlyLogout = true during logout
                        LogUnexpectedOpcode(packet, "STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT",
                            "the player has not logged in yet and not recently logout");
                    else if (AntiDOS.EvaluateOpcode(*packet, currentTime))
                    {
                        // not expected _player or must checked in packet hanlder
                        sScriptMgr->OnPacketReceive(this, WorldPacket(*packet));
                        opHandle->Call(this, *packet);
                        LogUnprocessedTail(packet);
                    }
                    else
                        processedPackets = MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE;   // break out of packet processing loop                    
                    break;
                case STATUS_TRANSFER:
                    if (!_player)
                        LogUnexpectedOpcode(packet, "STATUS_TRANSFER", "the player has not logged in yet");
                    else if (_player->IsInWorld())
                        LogUnexpectedOpcode(packet, "STATUS_TRANSFER", "the player is still in world");
                    else if (AntiDOS.EvaluateOpcode(*packet, currentTime))
                    {
                        sScriptMgr->OnPacketReceive(this, WorldPacket(*packet));
                        opHandle->Call(this, *packet);
                        LogUnprocessedTail(packet);
                    }
                    else
                        processedPackets = MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE;   // break out of packet processing loop                    
                    break;
                case STATUS_AUTHED:
                    // prevent cheating with skip queue wait
                    if (m_inQueue)
                    {
                        LogUnexpectedOpcode(packet, "STATUS_AUTHED", "the player not pass queue yet");
                        break;
                    }

                    // some auth opcodes can be recieved before STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT opcodes
                    // however when we recieve CMSG_ENUM_CHARACTERS we are surely no longer during the logout process.
                    if (packet->GetOpcode() == CMSG_ENUM_CHARACTERS)
                        m_playerRecentlyLogout = false;

                    if (AntiDOS.EvaluateOpcode(*packet, currentTime))
                    {
                          sScriptMgr->OnPacketReceive(this, WorldPacket(*packet));
                          opHandle->Call(this, *packet);
                          LogUnprocessedTail(packet);
                    }
                    else
                        processedPackets = MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE;   // break out of packet processing loop
                    break;
                case STATUS_NEVER:
                    TC_LOG_ERROR("network.opcode", "Received not allowed opcode %s from %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str()
                        , GetPlayerInfo().c_str());
                    break;
                case STATUS_UNHANDLED:
                    TC_LOG_ERROR("network.opcode", "Received not handled opcode %s from %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet->GetOpcode())).c_str()
                        , GetPlayerInfo().c_str());
                    break;
            }
        }
        catch (ByteBufferException const& e)
        {
            TC_LOG_ERROR("network", "WorldSession::Update ByteBufferException occured while parsing a packet (opcode: %u) from client %s, accountid=%i. Skipped packet.",
                packet->GetOpcode(), GetRemoteAddress().c_str(), GetAccountId());
            packet->hexlike();
        }

        if (deletePacket)
            delete packet;

        deletePacket = true;

        processedPackets++;

        //process only a max amout of packets in 1 Update() call.
        //Any leftover will be processed in next update
        if (processedPackets > MAX_PROCESSED_PACKETS_IN_SAME_WORLDSESSION_UPDATE)
            break;

    }

    _recvQueue.readd(requeuePackets.begin(), requeuePackets.end());

    if (m_Socket && m_Socket->IsOpen() && _warden)
        _warden->Update();

    ProcessQueryCallbacks();
    uint32 sessionDiff = getMSTime();


    sessionDiff = getMSTime() - sessionDiff;
    if (sessionDiff > 20 && !pktHandle.empty())
    {
        auto itr = pktHandle.find(CMSG_ADD_FRIEND);
        if (itr != pktHandle.end())
        {
            if ((*itr).second.nbPkt > 7)
            {
                KickPlayer();
                return false;
            }
        }
    }

    //check if we are safe to proceed with logout
    //logout procedure should happen only in World::UpdateSessions() method!!!
    if (updater.ProcessUnsafe())
    {
        ///- If necessary, log the player out
        if (ShouldLogOut(currentTime) && !m_playerLoading)
            LogoutPlayer(true);

        if (m_Socket && GetPlayer() && _warden)
            _warden->Update();

        ///- Cleanup socket pointer if need
        if (m_Socket && !m_Socket->IsOpen())
        {
            m_Socket->CloseSocket();
            m_Socket.reset();
        }

        if (!m_Socket)
            return false;                                       //Will remove this session from the world session map
    }

    return true;
}

/// %Log the player out
void WorldSession::LogoutPlayer(bool save)
{
    // finish pending transfers before starting the logout
    while (_player && _player->IsBeingTeleportedFar())
        HandleMoveWorldportAck();

    // Wait until all async auction queries are processed.
    while (_player)
    {
        if (_player->m_activeAuctionQueries.empty())
            break;
    }

    m_playerLogout = true;
    m_playerSave = save;

    if (_player)
    {
        if (uint64 lguid = _player->GetLootGUID())
            DoLootReleaseAll();

        ///- If the player just died before logging out, make him appear as a ghost
        //FIXME: logout must be delayed in case lost connection with client in time of combat
        if (_player->GetDeathTimer())
        {
            _player->getHostileRefManager().deleteReferences();
            _player->BuildPlayerRepop();
            _player->RepopAtGraveyard();
        }
        else if (!_player->getAttackers().empty() || _player->IsCharmed())
        {
            // build set of player who attack _player or who have pet attacking of _player
            std::set<Player*> attackers;
            for (auto&& itr : _player->getAttackers())
            {
                Unit* owner = itr->GetOwner();           // including player controlled case
                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    attackers.insert(owner->ToPlayer());
                else if (itr->GetTypeId() == TYPEID_PLAYER)
                    attackers.insert(itr->ToPlayer());
            }

            _player->CombatStop();
            _player->RemoveAllAurasOnDeath();
            _player->SetPvPDeath(!attackers.empty());
            _player->KillPlayer();
            _player->BuildPlayerRepop();
            _player->RepopAtGraveyard();

            // give honor to all attackers from set like group case
            for (auto&& itr : attackers)
                itr->RewardHonor(_player, attackers.size());

            // give bg rewards and update counters like kill by first from attackers
            // this can't be called for all attackers.
            if (!attackers.empty())
                if (Battleground *bg = _player->GetBattleground())
                    bg->HandleKillPlayer(_player, *attackers.begin());
        }
        else if (_player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        {
            // this will kill character by SPELL_AURA_SPIRIT_OF_REDEMPTION
            _player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
            _player->KillPlayer();
            _player->BuildPlayerRepop();
            _player->RepopAtGraveyard();
        }
        else if (_player->HasPendingBind())
        {
            _player->RepopAtGraveyard();
            _player->SetPendingBind(0, 0);
        }

        //drop a flag if player is carrying it
        if (Battleground* bg = _player->GetBattleground())
            bg->EventPlayerLoggedOut(_player);

        ///- Teleport to home if the player is in an invalid instance
        if (!_player->m_InstanceValid && !_player->IsGameMaster())
            _player->TeleportTo(_player->m_homebindMapId, _player->m_homebindX, _player->m_homebindY, _player->m_homebindZ, _player->GetOrientation());

        sOutdoorPvPMgr->HandlePlayerLeaveZone(_player, _player->GetZoneId());

        for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
        {
            if (BattlegroundQueueTypeId bgQueueTypeId = _player->GetBattlegroundQueueTypeId(i))
            {
                _player->RemoveBattlegroundQueueId(bgQueueTypeId);
                BattlegroundQueue& queue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
                queue.RemovePlayer(_player->GetGUID(), true);
            }
        }

        // Repop at GraveYard or other player far teleport will prevent saving player because of not present map
        // Teleport player immediately for correct player save
        while (_player->IsBeingTeleportedFar())
            HandleMoveWorldportAck();

        ///- If the player is in a guild, update the guild roster and broadcast a logout message to other guild members
        if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
            guild->HandleMemberLogout(this);

        ///- Remove pet
        _player->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RETURN_REAGENT);

        ///- Clear whisper whitelist
        _player->ClearWhisperWhiteList();

        ///- empty buyback items and save the player in the database
        // some save parts only correctly work in case player present in map/player_lists (pets, etc)
        if (save)
        {
            uint32 eslot;
            for (int j = BUYBACK_SLOT_START; j < BUYBACK_SLOT_END; ++j)
            {
                eslot = j - BUYBACK_SLOT_START;
                _player->SetGuidValue(PLAYER_FIELD_INV_SLOTS + (j * 2), ObjectGuid::Empty);
                _player->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE + eslot, 0);

                _player->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP + eslot, 0);
            }
            _player->SaveToDB();
        }

        ///- Leave all channels before player delete...
        _player->CleanupChannels();

        // if player is leader of a group and is holding a ready check, complete it early
        _player->ReadyCheckComplete();

        ///- If the player is in a group (or invited), remove him. If the group if then only 1 person, disband the group.
        _player->UninviteFromGroup();

        //! Send update to group and reset stored max enchanting level
        if (_player->GetGroup())
        {
            _player->GetGroup()->SendUpdate();
            _player->GetGroup()->ResetMaxEnchantingLevel();
        }

        //! Broadcast a logout message to the player's friends
        sSocialMgr->SendFriendStatus(_player, FRIEND_OFFLINE, _player->GetGUID(), true);
        sSocialMgr->RemovePlayerSocial(_player->GetGUID());

        // forfiet any pet battles in progress
        sPetBattleSystem->ForfietBattle(_player->GetGUID());

        //! Call script hook before deletion
        sScriptMgr->OnPlayerLogout(_player);

        //! Remove the player from the world
        // the player may not be in the world when logging out
        // e.g if he got disconnected during a transfer to another map
        // calls to GetMap in this case may cause crashes
        _player->SetDestroyedObject(true);
        _player->CleanupsBeforeDelete();
        TC_LOG_INFO("entities.player.character", "Account: %d (IP: %s) Logout Character:[%s] (GUID: %u) Level: %d",
            GetAccountId(), GetRemoteAddress().c_str(), _player->GetName().c_str(), _player->GetGUID().GetCounter(), _player->GetLevel());
        if (Map* _map = _player->FindMap())
            _map->RemovePlayerFromMap(_player, true);

        SetPlayer(NULL); //! Pointer already deleted during RemovePlayerFromMap

        //! Send the 'logout complete' packet to the client
        //! Client will respond by sending 3x CMSG_CANCEL_TRADE, which we currently dont handle
        WorldPacket data(SMSG_LOGOUT_COMPLETE);
        ObjectGuid guid = ObjectGuid::Empty; // Autolog guid - 0 for logout

        data.WriteBit(true);

        data.WriteBit(guid[3]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[0]);

        data.FlushBits();

        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[5]);
        SendPacket(&data);

        TC_LOG_DEBUG("network", "SESSION: Sent SMSG_LOGOUT_COMPLETE Message");

        //! Since each account can only have one online character at any given time, ensure all characters for active account are marked as offline
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ACCOUNT_ONLINE);
        stmt->setUInt32(0, GetAccountId());
        CharacterDatabase.Execute(stmt);
    }

    m_playerLogout = false;
    m_playerSave = false;
    m_playerRecentlyLogout = true;
    AntiDOS.AllowOpcode(CMSG_ENUM_CHARACTERS, true);
    SetLogoutStartTime(0);
}

/// Kick a player out of the World
void WorldSession::KickPlayer()
{
    if (m_Socket)
        m_Socket->CloseSocket();
}

void WorldSession::KickPlayerOP(std::string const& reason)
{
    if (m_Socket)
    {
        m_Socket->CloseSocket();
        forceExit = true;
    }
}

void WorldSession::SendNotification(const char *format, ...)
{
    if (format)
    {
        va_list ap;
        char szStr[1024];
        szStr[0] = '\0';
        va_start(ap, format);
        vsnprintf(szStr, 1024, format, ap);
        va_end(ap);

        size_t len = strlen(szStr);
        WorldPacket data(SMSG_NOTIFICATION, 2 + len);
        data.WriteBits(len, 12);
        data.FlushBits();
        data.append(szStr, len);
        SendPacket(&data);
    }
}

void WorldSession::SendNotification(uint32 string_id, ...)
{
    char const* format = GetTrinityString(string_id);
    if (format)
    {
        va_list ap;
        char szStr[1024];
        szStr[0] = '\0';
        va_start(ap, string_id);
        vsnprintf(szStr, 1024, format, ap);
        va_end(ap);

        size_t len = strlen(szStr);
        WorldPacket data(SMSG_NOTIFICATION, 2 + len);
        data.WriteBits(len, 12);
        data.FlushBits();
        data.append(szStr, len);
        SendPacket(&data);
    }
}

const char *WorldSession::GetTrinityString(int32 entry) const
{
    return sObjectMgr->GetTrinityString(entry, GetSessionDbLocaleIndex());
}

void WorldSession::ResetTimeOutTime(bool onlyActive)
{
    if (GetPlayer())
        m_timeOutTime = GameTime::GetGameTime() + time_t(sWorld->getIntConfig(CONFIG_SOCKET_TIMEOUTTIME_ACTIVE));
    else if (!onlyActive)
        m_timeOutTime = GameTime::GetGameTime() + time_t(sWorld->getIntConfig(CONFIG_SOCKET_TIMEOUTTIME));
}

void WorldSession::AddFlag(AccountFlags flag)
{
    m_flags |= flag;
    LoginDatabase.PExecute("UPDATE account SET flags = %u WHERE id = %u", m_flags, _accountId);
}

void WorldSession::RemoveFlag(AccountFlags flag)
{
    m_flags &= ~flag;
    LoginDatabase.PExecute("UPDATE account SET flags = %u WHERE id = %u", m_flags, _accountId);
}

void WorldSession::Handle_NULL(WorldPacket& null)
{
    TC_LOG_ERROR("network.opcode", "Received unhandled opcode %s from %s"
    , GetOpcodeNameForLogging(static_cast<OpcodeClient>(null.GetOpcode())).c_str(), GetPlayerInfo().c_str());
}

void WorldSession::Handle_EarlyProccess(WorldPacket& recvPacket)
{
    TC_LOG_ERROR("network.opcode", "Received opcode %s that must be processed in WorldSocket::OnRead from %s"
    , GetOpcodeNameForLogging(static_cast<OpcodeClient>(recvPacket.GetOpcode())).c_str(), GetPlayerInfo().c_str());
}

void WorldSession::Handle_Deprecated(WorldPacket& recvPacket)
{
    TC_LOG_ERROR("network.opcode", "Received deprecated opcode %s from %s"
    , GetOpcodeNameForLogging(static_cast<OpcodeClient>(recvPacket.GetOpcode())).c_str(), GetPlayerInfo().c_str());
}

void WorldSession::SendAuthWaitQue(uint32 position)
{
    if (position == 0)
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet.WriteBit(0); // has account info
        packet.WriteBit(0); // has queue info
        packet.FlushBits();
        packet << uint8(AUTH_OK);
        SendPacket(&packet);
    }
    else
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 6);
        packet.WriteBit(0); // has account info
        packet.WriteBit(1); // has queue info
        packet.WriteBit(0); // unk queue bool
        packet.FlushBits();
        packet << uint32(position);
        packet << uint8(AUTH_WAIT_QUEUE);
        SendPacket(&packet);
    }
}

void WorldSession::LoadGlobalAccountData()
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_DATA);
    stmt->setUInt32(0, GetAccountId());
    LoadAccountData(CharacterDatabase.Query(stmt), GLOBAL_CACHE_MASK);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_ACHIEVEMENT);
    stmt->setUInt32(0, GetAccountId());

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_ACHIEVEMENT_PROGRESS);
    stmt->setUInt32(0, GetAccountId());

    GetAchievementMgr().LoadFromDB(result, CharacterDatabase.Query(stmt));
}

void WorldSession::LoadAccountData(PreparedQueryResult result, uint32 mask)
{
    for (uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
        if (mask & (1 << i))
            m_accountData[i] = AccountData();

    if (!m_accountData)
        return;

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 type = fields[0].GetUInt8();
        if (type >= NUM_ACCOUNT_DATA_TYPES)
        {
            TC_LOG_ERROR("misc", "Table `%s` have invalid account data type (%u), ignore.",
                mask == GLOBAL_CACHE_MASK ? "account_data" : "character_account_data", type);
            continue;
        }

        if ((mask & (1 << type)) == 0)
        {
            TC_LOG_ERROR("misc", "Table `%s` have non appropriate for table  account data type (%u), ignore.",
                mask == GLOBAL_CACHE_MASK ? "account_data" : "character_account_data", type);
            continue;
        }

        m_accountData[type].Time = time_t(fields[1].GetUInt32());
        m_accountData[type].Data = fields[2].GetString();
    }
    while (result->NextRow());
}

void WorldSession::SetAccountData(AccountDataType type, time_t tm, std::string const& data)
{
    uint32 id = 0;
    CharacterDatabasePreparedStatement* stmt = nullptr;
    if ((1 << type) & GLOBAL_CACHE_MASK)
    {
        id = GetAccountId();
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ACCOUNT_DATA);
    }
    else
    {
        // _player can be NULL and packet received after logout but m_GUID still store correct guid
        if (!m_GUIDLow)
            return;

        id = m_GUIDLow;
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_PLAYER_ACCOUNT_DATA);
    }

    stmt->setUInt32(0, id);
    stmt->setUInt8 (1, type);
    stmt->setUInt32(2, uint32(tm));
    stmt->setString(3, data);
    CharacterDatabase.Execute(stmt);

    m_accountData[type].Time = tm;
    m_accountData[type].Data = data;
}

void WorldSession::SendAccountDataTimes(uint32 mask)
{
    WorldPacket data(SMSG_ACCOUNT_DATA_TIMES, 4 + 1 + 4 + NUM_ACCOUNT_DATA_TYPES * 4);

    data.WriteBit(1);
    data.FlushBits();

    for (uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
        data << uint32(GetAccountData(AccountDataType(i))->Time); // also unix time

    data << uint32(mask);
    data << uint32(time(NULL)); // Server time

    SendPacket(&data);
}

void WorldSession::LoadTutorialsData(PreparedQueryResult result)
{
    memset(m_Tutorials, 0, sizeof(uint32) * MAX_ACCOUNT_TUTORIAL_VALUES);

    if (result)
    {
        for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
            m_Tutorials[i] = (*result)[i].GetUInt32();
        m_TutorialsChanged |= TUTORIALS_FLAG_LOADED_FROM_DB;
    }

    m_TutorialsChanged &= ~TUTORIALS_FLAG_CHANGED;
}

void WorldSession::SendTutorialsData()
{
    WorldPacket data(SMSG_TUTORIAL_FLAGS, 4 * MAX_ACCOUNT_TUTORIAL_VALUES);
    for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
        data << m_Tutorials[i];
    SendPacket(&data);
}

void WorldSession::SaveTutorialsData(CharacterDatabaseTransaction trans)
{
    if (!m_TutorialsChanged)
        return;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_HAS_TUTORIALS);
    stmt->setUInt32(0, GetAccountId());
    bool hasTutorials = CharacterDatabase.Query(stmt) != nullptr;
    // Modify data in DB
    stmt = CharacterDatabase.GetPreparedStatement(hasTutorials ? CHAR_UPD_TUTORIALS : CHAR_INS_TUTORIALS);
    for (uint8 i = 0; i < MAX_ACCOUNT_TUTORIAL_VALUES; ++i)
        stmt->setUInt32(i, m_Tutorials[i]);
    stmt->setUInt32(MAX_ACCOUNT_TUTORIAL_VALUES, GetAccountId());
    trans->Append(stmt);

    m_TutorialsChanged = false;
}

void WorldSession::ReadAddonsInfo(WorldPacket &data)
{
    if (data.rpos() + 4 > data.size())
        return;

    uint32 size;
    data >> size;

    if (!size)
        return;

    if (size > 0xFFFFF)
    {
        TC_LOG_ERROR("misc", "WorldSession::ReadAddonsInfo addon info too big, size %u", size);
        return;
    }

    uLongf uSize = size;

    uint32 pos = data.rpos();

    ByteBuffer addonInfo;
    addonInfo.resize(size);

    if (uncompress(addonInfo.contents(), &uSize, data.contents() + pos, data.size() - pos) == Z_OK)
    {
        uint32 addonsCount;
        addonInfo >> addonsCount;                         // addons count

        for (uint32 i = 0; i < addonsCount; ++i)
        {
            std::string addonName;
            uint8 usingPubKey;
            uint32 crc, urlFile;

            // check next addon data format correctness
            if (addonInfo.rpos() + 1 > addonInfo.size())
                return;

            addonInfo >> addonName;

            addonInfo >> usingPubKey >> crc >> urlFile;

            TC_LOG_INFO("misc", "ADDON: Name: %s, UsePubKey: 0x%x, CRC: 0x%x, UrlFile: %i", addonName.c_str(), usingPubKey, crc, urlFile);

            AddonInfo addon(addonName, true, crc, 2, usingPubKey);

            SavedAddon const* savedAddon = AddonMgr::GetAddonInfo(addonName);
            if (savedAddon)
            {
                if (addon.CRC != savedAddon->CRC)
                    TC_LOG_INFO("misc", "ADDON: %s was known, but didn't match known CRC (0x%x)!", addon.Name.c_str(), savedAddon->CRC);
                else
                    TC_LOG_INFO("misc", "ADDON: %s was known, CRC is correct (0x%x)", addon.Name.c_str(), savedAddon->CRC);
            }
            else
            {
                AddonMgr::SaveAddon(addon);

                TC_LOG_INFO("misc", "ADDON: %s (0x%x) was not known, saving...", addon.Name.c_str(), addon.CRC);
            }

            /// @todo Find out when to not use CRC/pubkey, and other possible states.
            m_addonsList.push_back(addon);
        }

        uint32 currentTime;
        addonInfo >> currentTime;
        TC_LOG_DEBUG("network", "ADDON: CurrentTime: %u", currentTime);
    }
    else
        TC_LOG_ERROR("misc", "Addon packet uncompress error!");
}

void WorldSession::SendAddonsInfo()
{
    uint8 addonPublicKey[256] =
    {
        0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D, 0x54,
        0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45, 0x75,
        0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59, 0x34,
        0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28, 0xA8,
        0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C, 0xC8,
        0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F, 0x5A,
        0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2, 0xC3,
        0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46, 0xB9,
        0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD, 0x22,
        0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92, 0x7E,
        0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB, 0xFB,
        0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8, 0xF7,
        0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36, 0xF0,
        0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0, 0xA6,
        0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3, 0x9A,
        0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89, 0xD2
    };

    uint8 pubKeyOrder[256] =
    {
        0x05, 0xB0, 0x94, 0x2B, 0x1C, 0x87, 0x40, 0x08, 0xA0, 0x91, 0xE2, 0x77, 0xB5, 0xC0, 0xF0, 0x48,
        0xF3, 0xD4, 0xD1, 0xAC, 0x15, 0xED, 0x55, 0x0A, 0x4B, 0x75, 0xF4, 0x52, 0x18, 0x14, 0x12, 0x4C,
        0x43, 0x39, 0x9D, 0x3B, 0xC6, 0x5A, 0x16, 0x06, 0x31, 0x0C, 0x5F, 0xC1, 0x76, 0x5E, 0x28, 0x62,
        0xFF, 0xA9, 0xD6, 0x53, 0x80, 0xDB, 0x49, 0xF7, 0x84, 0xCA, 0xDA, 0x9A, 0x70, 0x83, 0xB1, 0x6F,
        0x90, 0x38, 0x27, 0x98, 0x30, 0x3F, 0x19, 0x72, 0x26, 0x54, 0x63, 0xA5, 0x7E, 0x22, 0x45, 0xB7,
        0xB9, 0x34, 0x67, 0x24, 0xE9, 0x03, 0x2F, 0x8D, 0xA2, 0xE8, 0xC2, 0xFD, 0x74, 0x1B, 0x50, 0x2E,
        0x59, 0x6B, 0xBD, 0x0E, 0xE1, 0xA7, 0x8C, 0xFA, 0xBC, 0x11, 0x1D, 0x89, 0x85, 0x4A, 0xB2, 0x3E,
        0xEC, 0x1F, 0x65, 0x09, 0xA4, 0xC8, 0x88, 0x9F, 0xC5, 0xD8, 0xF6, 0x86, 0x00, 0x61, 0xEA, 0xA6,
        0xCC, 0x41, 0x3C, 0xDF, 0x7A, 0x02, 0x04, 0xEF, 0xF9, 0x1E, 0xFC, 0xD3, 0x7C, 0x1A, 0x17, 0xA1,
        0x5C, 0x8A, 0x25, 0xE3, 0x78, 0x99, 0x73, 0x97, 0xFE, 0xAD, 0xAF, 0x6C, 0x82, 0xFB, 0xAA, 0x9E,
        0x0B, 0xF5, 0xBE, 0x68, 0xD9, 0x07, 0x4E, 0xE7, 0x9B, 0xAB, 0x37, 0x51, 0x8F, 0xCE, 0x46, 0x9C,
        0x58, 0x2D, 0xC9, 0xB6, 0xB4, 0x10, 0xD7, 0xE6, 0x32, 0x95, 0xCB, 0xA8, 0xDC, 0xBB, 0x29, 0x3D,
        0xEE, 0xD0, 0xE0, 0x6A, 0xCD, 0xDE, 0x2A, 0x44, 0x7F, 0xD2, 0x4D, 0x81, 0xD5, 0x0F, 0x66, 0x92,
        0x36, 0x23, 0x5B, 0x13, 0xC7, 0x20, 0x8B, 0x96, 0xC4, 0x7D, 0x35, 0x64, 0x71, 0x6E, 0x47, 0xBF,
        0x3A, 0xF2, 0xF8, 0x0D, 0xB8, 0xA3, 0x93, 0x4F, 0x5D, 0xE5, 0xE4, 0xBA, 0xCF, 0x01, 0x42, 0x21,
        0x79, 0x60, 0x7B, 0xB3, 0xEB, 0xF1, 0x6D, 0x8E, 0x2C, 0x56, 0xC3, 0xAE, 0x57, 0x69, 0x33, 0xDD,
    };

    WorldPacket data(SMSG_ADDON_INFO, 1000);

    AddonMgr::BannedAddonList const* bannedAddons = AddonMgr::GetBannedAddons();
    data.WriteBits((uint32)bannedAddons->size(), 18);
    data.WriteBits((uint32)m_addonsList.size(), 23);

    for (AddonsList::iterator itr = m_addonsList.begin(); itr != m_addonsList.end(); ++itr)
    {
        data.WriteBit(0); // Has URL
        data.WriteBit(itr->Enabled);
        data.WriteBit(!itr->UsePublicKeyOrCRC); // If client doesnt have it, send it
    }

    data.FlushBits();

    for (AddonsList::iterator itr = m_addonsList.begin(); itr != m_addonsList.end(); ++itr)
    {
        if (!itr->UsePublicKeyOrCRC)
        {
            for (int i = 0; i < 256; i++)
                data << uint8(addonPublicKey[pubKeyOrder[i]]);
        }

        if (itr->Enabled)
        {
            data << uint8(itr->Enabled);
            data << uint32(0);
        }

        data << uint8(itr->State);
    }

    m_addonsList.clear();

    for (AddonMgr::BannedAddonList::const_iterator itr = bannedAddons->begin(); itr != bannedAddons->end(); ++itr)
    {
        data << uint32(itr->Id);
        data << uint32(1);  // IsBanned

        for (int32 i = 0; i < 8; i++)
            data << uint32(0);

        // Those 3 might be in wrong order
        data << uint32(itr->Timestamp);
    }

    SendPacket(&data);
}

void WorldSession::SendTimezoneInformation()
{
    char timezoneString[256];

    // TIME_ZONE_INFORMATION timeZoneInfo;
    // GetTimeZoneInformation(&timeZoneInfo);
    // wcstombs(timezoneString, timeZoneInfo.StandardName, sizeof(timezoneString));

    sprintf(timezoneString, "Etc/UTC"); // The method above cannot be used, because of non-english OS translations, so we send const data (possible strings are hardcoded in the client because of the same reason)

    WorldPacket data(SMSG_SET_TIMEZONE_INFORMATION, 2 + strlen(timezoneString) * 2);
    data.WriteBits(strlen(timezoneString), 7);
    data.WriteBits(strlen(timezoneString), 7);
    data.FlushBits();
    data.WriteString(std::string_view(timezoneString));
    data.WriteString(std::string_view(timezoneString));
    SendPacket(&data);
}

bool WorldSession::IsAddonRegistered(const std::string& prefix) const
{
    if (!_filterAddonMessages) // if we have hit the softcap (64) nothing should be filtered
        return true;

    if (_registeredAddonPrefixes.empty())
        return false;

    std::vector<std::string>::const_iterator itr = std::find(_registeredAddonPrefixes.begin(), _registeredAddonPrefixes.end(), prefix);
    return itr != _registeredAddonPrefixes.end();
}

void WorldSession::HandleUnregisterAddonPrefixesOpcode(WorldPacket& /*recvPacket*/) // empty packet
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_UNREGISTER_ALL_ADDON_PREFIXES");

    _registeredAddonPrefixes.clear();
}

void WorldSession::HandleAddonRegisteredPrefixesOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_ADDON_REGISTERED_PREFIXES");

    // This is always sent after CMSG_UNREGISTER_ALL_ADDON_PREFIXES

    uint32 count = recvPacket.ReadBits(24);

    if (count > REGISTERED_ADDON_PREFIX_SOFTCAP)
    {
        // if we have hit the softcap (64) nothing should be filtered
        _filterAddonMessages = false;
        recvPacket.rfinish();
        return;
    }

    std::vector<uint8> lengths(count);
    for (uint32 i = 0; i < count; ++i)
        lengths[i] = recvPacket.ReadBits(5);

    for (uint32 i = 0; i < count; ++i)
        _registeredAddonPrefixes.push_back(recvPacket.ReadString(lengths[i]));

    if (_registeredAddonPrefixes.size() > REGISTERED_ADDON_PREFIX_SOFTCAP) // shouldn't happen
    {
        _filterAddonMessages = false;
        return;
    }

    _filterAddonMessages = true;
}

void WorldSession::SetPlayer(Player* player)
{
    _player = player;

    // set m_GUID that can be used while player loggined and later until m_playerRecentlyLogout not reset
    if (_player)
        m_GUIDLow = _player->GetGUID().GetCounter();
    GetAchievementMgr().SetCurrentPlayer(player);
}

void WorldSession::ProcessQueryCallbacks()
{
    _queryProcessor.ProcessReadyCallbacks();
    _transactionCallbacks.ProcessReadyCallbacks();
    _queryHolderProcessor.ProcessReadyCallbacks();
}

TransactionCallback& WorldSession::AddTransactionCallback(TransactionCallback&& callback)
{
    return _transactionCallbacks.AddCallback(std::move(callback));
}

SQLQueryHolderCallback& WorldSession::AddQueryHolderCallback(SQLQueryHolderCallback&& callback)
{
    return _queryHolderProcessor.AddCallback(std::move(callback));
}

void WorldSession::InitWarden(BigNumber* k, std::string const& os)
{
    if (os == "Win" || os == "Wn64")
    {
        _warden = new WardenWin();
        _warden->Init(this, k);
    }
    else if (os == "OSX")
    {
        // Disabled as it is causing the client to crash
        // _warden = new WardenMac();
        // _warden->Init(this, k);
    }
}

class AccountInfoQueryHolderPerRealm : public CharacterDatabaseQueryHolder
{
public:
    enum
    {
        GLOBAL_ACCOUNT_DATA = 0,
        TUTORIALS,

        MAX_QUERIES
    };

    AccountInfoQueryHolderPerRealm() { SetSize(MAX_QUERIES); }

    bool Initialize(uint32 accountId)
    {
        bool ok = true;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_DATA);
        stmt->setUInt32(0, accountId);
        ok = SetPreparedQuery(GLOBAL_ACCOUNT_DATA, stmt) && ok;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_TUTORIALS);
        stmt->setUInt32(0, accountId);
        ok = SetPreparedQuery(TUTORIALS, stmt) && ok;

        return ok;
    }
};

void WorldSession::InitializeSession()
{
    std::shared_ptr<AccountInfoQueryHolderPerRealm> realmHolder = std::make_shared<AccountInfoQueryHolderPerRealm>();
    if (!realmHolder->Initialize(GetAccountId()))
    {
        SendAuthResponse(AUTH_SYSTEM_ERROR, false);
        return;
    }

    AddQueryHolderCallback(CharacterDatabase.DelayQueryHolder(realmHolder)).AfterComplete([this](SQLQueryHolderBase const& holder)
    {
            InitializeSessionCallback(static_cast<AccountInfoQueryHolderPerRealm const&>(holder));
    });
}

void WorldSession::InitializeSessionCallback(CharacterDatabaseQueryHolder const& realmHolder)
{
    LoadAccountData(realmHolder.GetPreparedResult(AccountInfoQueryHolderPerRealm::GLOBAL_ACCOUNT_DATA), GLOBAL_CACHE_MASK);
    LoadTutorialsData(realmHolder.GetPreparedResult(AccountInfoQueryHolderPerRealm::TUTORIALS));

    if (!m_inQueue)
        SendAuthResponse(AUTH_OK, false);
    else
        SendAuthWaitQue(0);

    SetInQueue(false);
    ResetTimeOutTime(false);

    SendAddonsInfo();
    SendClientCacheVersion(sWorld->getIntConfig(CONFIG_CLIENTCACHE_VERSION));
    SendTutorialsData();
}


bool WorldSession::DosProtection::EvaluateOpcode(WorldPacket& p, time_t time) const
{
    uint32 maxPacketCounterAllowed = GetMaxPacketCounterAllowed(p.GetOpcode());

    // Return true if there no limit for the opcode
    if (!maxPacketCounterAllowed)
         return true;

    PacketCounter& packetCounter = _PacketThrottlingMap[p.GetOpcode()];
    if (packetCounter.lastReceiveTime != time)
    {
        packetCounter.lastReceiveTime = time;
        packetCounter.amountCounter = 0;
    }

    // Check if player is flooding some packets
    if (++packetCounter.amountCounter <= maxPacketCounterAllowed)
        return true;


    switch (_policy)
    {
        case POLICY_LOG:
            return true;
        case POLICY_KICK:
            TC_LOG_INFO("network", "AntiDOS: Player kicked!");
            Session->KickPlayerOP("WorldSession::DosProtection::EvaluateOpcode AntiDOS");
            return false;
        case POLICY_BAN:
        {
            BanMode bm = (BanMode)sWorld->getIntConfig(CONFIG_PACKET_SPOOF_BANMODE);
            uint32 duration = sWorld->getIntConfig(CONFIG_PACKET_SPOOF_BANDURATION); // in seconds
            std::string nameOrIp = "";
            switch (bm)
            {
                case BAN_CHARACTER: // not supported, ban account
                case BAN_ACCOUNT: (void)sAccountMgr->GetName(Session->GetAccountId(), nameOrIp); break;
                case BAN_IP: nameOrIp = Session->GetRemoteAddress(); break;
            }
            sWorld->BanAccount(bm, nameOrIp, duration, "DOS (Packet Flooding/Spoofing", "Server: AutoDOS");
            TC_LOG_INFO("network", "AntiDOS: Player automatically banned for %u seconds.", duration);

            return false;
        }
        case BAN_SOLO:
        default: // invalid policy
            return true;
    }
}


uint32 WorldSession::DosProtection::GetMaxPacketCounterAllowed(uint16 opcode) const
{
    uint32 maxPacketCounterAllowed;
    switch (opcode)
    {
        // CPU usage sending 2000 packets/second on a 3.70 GHz 4 cores on Win x64
        //                                              [% CPU mysqld]   [%CPU worldserver RelWithDebInfo]
    case CMSG_PLAYER_LOGIN:                         //   0               0.5
    case CMSG_QUERY_TIME:                           //   0               1
    case CMSG_MOVE_TIME_SKIPPED:                    //   0               1
    case CMSG_LOGOUT_REQUEST:                       //   0               1
    case CMSG_PET_RENAME:                           //   0               1
    case CMSG_COMPLETE_CINEMATIC:                   //   0               1
    case CMSG_BANKER_ACTIVATE:                      //   0               1
    case CMSG_BUY_BANK_SLOT:                        //   0               1
    case CMSG_OPT_OUT_OF_LOOT:                      //   0               1
    case CMSG_DUEL_RESPONSE:                        //   0               1
    case CMSG_CALENDAR_COMPLAIN:                    //   0               1
    case CMSG_TAXI_NODE_STATUS_QUERY:               //   0               1.5
    case CMSG_TAXI_QUERY_AVAILABLE_NODES:           //   0               1.5
    case CMSG_REQUEST_PARTY_MEMBER_STATS:           //   0               1.5
    case CMSG_SET_ACTION_BUTTON:                    //   0               1.5
    case CMSG_RESET_INSTANCES:                      //   0               1.5
    //case CMSG_HEARTH_AND_RESURRECT:                 //   0               1.5
    case CMSG_TOGGLE_PVP:                           //   0               1.5
    case CMSG_PET_ABANDON:                          //   0               1.5
    case CMSG_ACTIVATE_TAXI:                        //   0               1.5
    case CMSG_SELF_RES:                             //   0               1.5
    case CMSG_UNLEARN_SKILL:                        //   0               1.5
    case CMSG_DISMISS_CRITTER:                      //   0               1.5
    case CMSG_REPOP_REQUEST:                        //   0               1.5
    case CMSG_BATTLEMASTER_JOIN_ARENA:              //   0               1.5
    case CMSG_BATTLEFIELD_LEAVE:                    //   0               1.5
    case CMSG_GUILD_BANK_LOG_QUERY:                 //   0               2
    case CMSG_LOGOUT_CANCEL:                        //   0               2
    case CMSG_ALTER_APPEARANCE:                     //   0               2
    case CMSG_QUEST_CONFIRM_ACCEPT:                 //   0               2
    case CMSG_GUILD_EVENT_LOG_QUERY:                //   0               2.5
    case CMSG_BEGIN_TRADE:                          //   0               2.5
    case CMSG_INITIATE_TRADE:                       //   0               3
    case CMSG_AREA_SPIRIT_HEALER_QUERY:             // not profiled                  // not profiled
    case CMSG_RANDOM_ROLL:                          // not profiled               // not profiled
    case CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK:      // not profiled
    case MSG_MOVE_HEARTBEAT:                        // Should not have limit to make player movement feel less laggy
    case MSG_MOVE_JUMP:                             // Same as above
    case MSG_MOVE_WORLDPORT_ACK:                    // Run as soon as we get it no cpu usage.
    case CMSG_GUILD_BANK_QUERY_TAB:                 //   0               3.5       medium upload bandwidth usage (non important uses too much bandwith)
    {
        // "0" is a magic number meaning there's no limit for the opcode.
        // All the opcodes above must cause little CPU usage and no sync/async database queries at all
        maxPacketCounterAllowed = 0;
        break;
    }

    case CMSG_QUESTGIVER_ACCEPT_QUEST:             //   0               4
    case CMSG_QUESTLOG_REMOVE_QUEST:               //   0               4
    case CMSG_QUESTGIVER_CHOOSE_REWARD:            //   0               4
    case CMSG_AUTOBANK_ITEM:                        //   0               6
    case CMSG_AUTOSTORE_BANK_ITEM:                  //   0               6    
    case CMSG_WHO:                                  //  0                0
    {
        maxPacketCounterAllowed = 0;
        break;
    }

    case CMSG_CALENDAR_EVENT_INVITE:
    case CMSG_CALENDAR_EVENT_STATUS:
    case CMSG_CALENDAR_GET_CALENDAR:
    case CMSG_GAMEOBJ_REPORT_USE:                  // not profiled
    case CMSG_GAMEOBJ_USE:                         // not profiled
    case CMSG_PETITION_DECLINE:                     // not profiled
    {
        maxPacketCounterAllowed = 50;
        break;
    }

    case CMSG_QUEST_POI_QUERY:                      //   0              25         very high upload bandwidth usage
    {
        maxPacketCounterAllowed = MAX_QUEST_LOG_SIZE;
        break;
    }

    case CMSG_SPELLCLICK:                          // not profiled
    case CMSG_DISMISS_CONTROLLED_VEHICLE:                 // not profiled
    {
        maxPacketCounterAllowed = 20;
        break;
    }

    case CMSG_PETITION_SIGN:                        //   9               4         2 sync 1 async db queries
    case CMSG_TURN_IN_PETITION:                     //   8               5.5       2 sync db query
    case CMSG_GROUP_CHANGE_SUB_GROUP:
    //case CMSG_GROUP_SWAP_SUB_GROUP:
    case CMSG_PETITION_QUERY:                       //   4               3.5       1 sync db query
    case CMSG_CHAR_CUSTOMIZE:                       //   5               5         1 sync db query
    case CMSG_CHAR_FACTION_OR_RACE_CHANGE:          //   5               5         1 sync db query
    case CMSG_CHAR_DELETE:                          //   4               4         1 sync db query
    case CMSG_DEL_FRIEND:                           //   7               5         1 async db query
    case CMSG_ADD_FRIEND:                           //   6               4         1 async db query
    //case CMSG_GUILD_CHANGE_NAME_REQUEST:             //   5               3         1 async db query
    case CMSG_SUBMIT_BUG:                           //   1               1         1 async db query
    case CMSG_GROUP_SET_LEADER:                     //   1               2         1 async db query
    case CMSG_GROUP_RAID_CONVERT:                         //   1               5         1 async db query
    case CMSG_GROUP_ASSISTANT_LEADER:                 //   1               2         1 async db query
    case CMSG_CALENDAR_ADD_EVENT:                   //  21              10         2 async db query
    case CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE:            // not profiled
    case CMSG_PETITION_BUY:                         // not profiled                1 sync 1 async db queries
    case CMSG_REQUEST_VEHICLE_PREV_SEAT:            // not profiled
    case CMSG_REQUEST_VEHICLE_NEXT_SEAT:            // not profiled
    case CMSG_REQUEST_VEHICLE_SWITCH_SEAT:          // not profiled
    case CMSG_REQUEST_VEHICLE_EXIT:                 // not profiled
    case CMSG_EJECT_PASSENGER:                      // not profiled
    case CMSG_ITEM_REFUND:                 // not profiled
    case CMSG_SOCKET_GEMS:                          // not profiled
    case CMSG_WRAP_ITEM:                            // not profiled
    case CMSG_REPORT_PVP_AFK:                // not profiled
    case CMSG_QUERY_INSPECT_ACHIEVEMENTS:           //   0              13         high upload bandwidth usage (non important uses too much bandwith)
    case SMSG_GUILD_MEMBER_UPDATE_NOTE:                //   1               2         1 async db query (non important uses too much bandwith)
    case CMSG_SET_CONTACT_NOTES:                    //   1               2.5       1 async db query    (non important uses too much bandwith)
    case CMSG_INSPECT:                              //   0               3.5 (non important uses too much bandwith)
    case CMSG_CONTACT_LIST:                         //   0               5
    {
        maxPacketCounterAllowed = 50;
        break;
    }

    case CMSG_CHAR_CREATE:                     //   7               5         3 async db queries
    case CMSG_ENUM_CHARACTERS:                      //  22               3         2 async db queries
    case CMSG_SUGGESTION_SUBMIT:     // not profiled                1 async db query
    case CMSG_SUBMIT_COMPLAIN:      // not profiled                1 async db query
    case CMSG_CALENDAR_UPDATE_EVENT:                // not profiled
    case CMSG_CALENDAR_REMOVE_EVENT:                // not profiled
    case CMSG_CALENDAR_COPY_EVENT:                  // not profiled
    case CMSG_CALENDAR_EVENT_SIGNUP:               // not profiled
    case CMSG_CALENDAR_EVENT_RSVP:                  // not profiled
    case CMSG_CALENDAR_EVENT_MODERATOR_STATUS:      // not profiled
    case CMSG_CALENDAR_EVENT_REMOVE_INVITE:               // not profiled
    case CMSG_LOOT_METHOD:                      // not profiled
    case CMSG_GUILD_INVITE:                 // not profiled
    case CMSG_GUILD_ACCEPT:                  // not profiled
    case CMSG_GUILD_DECLINE:             // not profiled
    case CMSG_GUILD_LEAVE:                          // not profiled
    case CMSG_GUILD_DISBAND:                         // not profiled
    case CMSG_GUILD_SET_GUILD_MASTER:               // not profiled
    case CMSG_GUILD_MOTD:               // not profiled
    case CMSG_GUILD_SET_RANK_PERMISSIONS:           // not profiled
    case CMSG_GUILD_ADD_RANK:                       // not profiled
    case CMSG_GUILD_DEL_RANK:                    // not profiled
    case CMSG_GUILD_INFO_TEXT:               // not profiled
    case CMSG_GUILD_BANK_DEPOSIT_MONEY:             // not profiled
    case CMSG_GUILD_BANK_WITHDRAW_MONEY:            // not profiled
    case CMSG_GUILD_BANK_BUY_TAB:                   // not profiled
    case CMSG_GUILD_BANK_UPDATE_TAB:                // not profiled
    case CMSG_GUILD_BANK_QUERY_TEXT:              // not profiled
    case CMSG_SAVE_GUILD_EMBLEM:                    // not profiled
    case CMSG_PETITION_RENAME:                // not profiled
    case CMSG_CONFIRM_RESPEC_WIPE:                  // not profiled
    case CMSG_SET_DUNGEON_DIFFICULTY:               // not profiled
    case CMSG_SET_RAID_DIFFICULTY:                  // not profiled
    //case MSG_PARTY_ASSIGNMENT:                 // not profiled
    case CMSG_RAID_READY_CHECK:                       // not profiled
    case CMSG_JOIN_CHANNEL:
    case CMSG_LEAVE_CHANNEL:
    case CMSG_CHANNEL_LIST:
    case CMSG_CHANNEL_INVITE:
    case CMSG_GUILD_ACHIEVEMENT_PROGRESS_QUERY:
    //case CMSG_PLAYER_LOGOUT:
    case SMSG_AUTH_RESPONSE:
    case SMSG_AUTH_CHALLENGE:
    case CMSG_PING:
    case CMSG_AUTH_SESSION:
    case CMSG_AUTH_CONTINUED_SESSION:
    case CMSG_KEEP_ALIVE:
    case CMSG_LOG_DISCONNECT:
    //case CMSG_ENABLE_NAGLE:
    //case CMSG_CONNECT_TO_FAILED:
    {
        maxPacketCounterAllowed = 25;
        break;
    }

    case CMSG_REQUEST_HOTFIX:                       // not profiled
    {
        maxPacketCounterAllowed = 1;
        break;
    }
    default:
    {
        maxPacketCounterAllowed = 200; // Increase the number of packets , because with more than 100 players can cause issues.
        break;
    }
    }

    return maxPacketCounterAllowed;
}