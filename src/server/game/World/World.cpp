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

/** \file
    \ingroup world
*/
#include "Common.h"
#include "Memory.h"
#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Config.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Vehicle.h"
#include "SkillExtraItems.h"
#include "SkillDiscovery.h"
#include "World.h"
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AuctionHouseBot.h"
#include "AuctionHouseMgr.h"
#include "BlackMarketMgr.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "GuildFinderMgr.h"
#include "TicketMgr.h"
#include "SpellMgr.h"
#include "GroupMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "DB2Stores.h"
#include "LootMgr.h"
#include "ItemEnchantmentMgr.h"
#include "IPLocation.h"
#include "MapManager.h"
#include "CreatureAIRegistry.h"
#include "BattlegroundMgr.h"
#include "OutdoorPvPMgr.h"
#include "WaypointMovementGenerator.h"
#include "M2Stores.h"
#include "MMapFactory.h"
#include "GameEventMgr.h"
#include "GameTime.h"
#include "GitRevision.h"
#include "GridNotifiersImpl.h"
#include "PoolMgr.h"
#include "QuestPools.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "Util.h"
#include "Language.h"
#include "CreatureGroups.h"
#include "Transport.h"
#include "ScriptMgr.h"
#include "AddonMgr.h"
#include "LFGMgr.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "CharacterDatabaseCleaner.h"
#include "WeatherMgr.h"
#include "CreatureTextMgr.h"
#include "SmartAI.h"
#include "Channel.h"
#include "WardenCheckMgr.h"
#include "Warden.h"
#include "CalendarMgr.h"
#include "BattlefieldMgr.h"
#include "TransportMgr.h"
#include "BattlePayMgr.h"
#include "BattlePetMgr.h"
#include "BattlePetSpawnMgr.h"
#include "PetBattle.h"
#include "DevTool.h"
#include "AnticheatMgr.h"
#include "ServiceBoost.h"
#include "ServiceMgr.h"
#include "WordFilterMgr.h"
#include "Realm.h"
#include "VMapFactory.h"
#include "VMapManager2.h"
#ifdef ELUNA
#include "LuaEngine.h"
#endif

uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;

TC_GAME_API std::atomic<bool> World::m_stopEvent(false);

TC_GAME_API std::atomic<uint32> World::m_worldLoopCounter(0);

float World::m_MaxVisibleDistanceOnContinents = DEFAULT_VISIBILITY_DISTANCE;
float World::m_MaxVisibleDistanceInInstances  = DEFAULT_VISIBILITY_INSTANCE;
float World::m_MaxVisibleDistanceInBGArenas   = DEFAULT_VISIBILITY_BGARENAS;

int32 World::m_visibility_notify_periodOnContinents = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
int32 World::m_visibility_notify_periodInInstances  = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
int32 World::m_visibility_notify_periodInBGArenas   = DEFAULT_VISIBILITY_NOTIFY_PERIOD;

float World::Visibility_RelocationLowerLimit = 10.0f;
uint32 World::Visibility_AINotifyDelay = 1000;

#ifdef ELUNA
extern void StartEluna(bool restart);
#endif
/// World constructor
World::World()
{
    m_playerLimit = 0;
    m_allowedSecurityLevel = SEC_PLAYER;
    m_allowMovement = true;
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_gameTime = time(NULL);
    m_startTime = m_gameTime;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;
    m_PlayerCount = 0;
    m_MaxPlayerCount = 0;
    m_NextDailyQuestReset = 0;
    m_NextWeeklyQuestReset = 0;
    m_nextCurrencyReset = 0;

    m_defaultDbcLocale = LOCALE_enUS;
    m_availableDbcLocaleMask = 0;

    m_updateTimeSum = 0;

    m_isClosed = false;

    m_CleaningFlags = 0;
}

/// World destructor
World::~World()
{
    ///- Empty the kicked session set
    while (!m_sessions.empty())
    {
        // not remove from queue, prevent loading new sessions
        delete m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
    }

    CliCommandHolder* command = NULL;
    while (cliCmdQueue.next(command))
        delete command;

    VMAP::VMapFactory::clear();
    MMAP::MMapFactory::clear();

    /// @todo free addSessQueue

    for (auto&& setting : m_devToolSettings)
        delete setting.second.second;
}

World* World::instance()
{
    static World instance;
    return &instance;
}

/// Find a player in a specified zone
Player* World::FindPlayerInZone(uint32 zone)
{
    ///- circle through active sessions and return the first player found in the zone
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;

        Player* player = itr->second->GetPlayer();
        if (!player)
            continue;

        if (player->IsInWorld() && player->GetZoneId() == zone)
        {
            // Used by the weather system. We return the player to broadcast the change weather message to him and all players in the zone.
            return player;
        }
    }
    return NULL;
}

bool World::IsClosed() const
{
    return m_isClosed;
}

void World::SetClosed(bool val)
{
    m_isClosed = val;

    // Invert the value, for simplicity for scripters.
    sScriptMgr->OnOpenStateChange(!val);
}

void World::SetMotd(const std::string& motd)
{
    m_motd = motd;

    sScriptMgr->OnMotdChange(m_motd);
}

const char* World::GetMotd() const
{
    return m_motd.c_str();
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;                                 // also can return NULL for kicked session
    else
        return NULL;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;

        itr->second->KickPlayer();
    }

    return true;
}

void World::AddSession(WorldSession* s)
{
    addSessQueue.add(s);
}

void World::AddSession_(WorldSession* s)
{
    ASSERT(s);
    //NOTE - Still there is race condition in WorldSession* being used in the Sockets

    ///- kick already loaded player with same account (if any) and remove session
    ///- if player is in loading and want to load again, return
    if (!RemoveSession (s->GetAccountId()))
    {
        s->KickPlayer();
        delete s;                                           // session not added yet in session list, so not listed in queue
        return;
    }

    // decrease session counts only at not reconnection case
    bool decrease_session = true;

    // if session already exist, prepare to it deleting at next world update
    // NOTE - KickPlayer() should be called on "old" in RemoveSession()
    {
        SessionMap::const_iterator old = m_sessions.find(s->GetAccountId());

        if (old != m_sessions.end())
        {
            // prevent decrease sessions count if session queued
            if (RemoveQueuedPlayer(old->second))
                decrease_session = false;
            // not remove replaced session form queue if listed
            delete old->second;
        }
    }

    m_sessions[s->GetAccountId()] = s;

    uint32 Sessions = GetActiveAndQueuedSessionCount();
    uint32 pLimit = GetPlayerAmountLimit();
    uint32 QueueSize = GetQueuedSessionCount(); //number of players in the queue

    //so we don't count the user trying to
    //login as a session and queue the socket that we are using
    if (decrease_session)
        --Sessions;

    if (pLimit > 0 && Sessions >= pLimit && !s->GetSecurity() == SEC_PLAYER && !HasRecentlyDisconnected(s))
    {
        AddQueuedPlayer(s);
        UpdateMaxSessionCounters();
        TC_LOG_INFO("misc", "PlayerQueue: Account id %u is in Queue Position (%u).", s->GetAccountId(), ++QueueSize);
        return;
    }

    s->InitializeSession();
    s->SendDanceStudioCreateResult();
    s->SendFeatureSystemStatusGlueScreen();
    sBattlePayMgr->SendBattlePayDistributionList(s);
    s->SendDispalyPromotionOpcode();
    s->SendTimezoneInformation();

    UpdateMaxSessionCounters();

    // Updates the population
    if (pLimit > 0)
    {
        float popu = (float)GetActiveSessionCount();              // updated number of users on the server
        popu /= pLimit;
        popu *= 2;
        TC_LOG_INFO("misc", "Server Population (%f).", popu);
    }
}

bool World::HasRecentlyDisconnected(WorldSession* session)
{
    if (!session)
        return false;

    if (uint32 tolerance = getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
    {
        for (DisconnectMap::iterator i = m_disconnects.begin(); i != m_disconnects.end();)
        {
            if (difftime(i->second, time(NULL)) < tolerance)
            {
                if (i->first == session->GetAccountId())
                    return true;
                ++i;
            }
            else
                m_disconnects.erase(i++);
        }
    }
    return false;
}

int32 World::GetQueuePos(WorldSession* sess)
{
    uint32 position = 1;

    for (Queue::const_iterator iter = m_QueuedPlayer.begin(); iter != m_QueuedPlayer.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void World::AddQueuedPlayer(WorldSession* sess)
{
    sess->SetInQueue(true);
    m_QueuedPlayer.push_back(sess);

    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    sess->SendAuthResponse(AUTH_OK, true, GetQueuePos(sess));
}

bool World::RemoveQueuedPlayer(WorldSession* sess)
{
    // sessions count including queued to remove (if removed_session set)
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    Queue::iterator iter = m_QueuedPlayer.begin();

    // search to remove and count skipped positions
    bool found = false;

    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            sess->ResetTimeOutTime(false);
            iter = m_QueuedPlayer.erase(iter);
            found = true;                                   // removing queued session
            break;
        }
    }

    // iter point to next socked after removed or end()
    // position store position of removed socket and then new position next socket after removed

    // if session not queued then we need decrease sessions count
    if (!found && sessions)
        --sessions;

    // accept first in queue
    if ((!m_playerLimit || sessions < m_playerLimit) && !m_QueuedPlayer.empty())
    {
        WorldSession* pop_sess = m_QueuedPlayer.front();
        pop_sess->InitializeSession();
        pop_sess->SetInQueue(false);
        pop_sess->ResetTimeOutTime(false);
        pop_sess->SendAccountDataTimes(GLOBAL_CACHE_MASK);
        pop_sess->SendTimezoneInformation();

        m_QueuedPlayer.pop_front();

        // update iter to point first queued socket or end() if queue is empty now
        iter = m_QueuedPlayer.begin();
        position = 1;
    }

    // update position from iter to end()
    // iter point to first not updated socket, position store new position
    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
        (*iter)->SendAuthWaitQue(position);

    return found;
}

/// Initialize config values
void World::LoadConfigSettings(bool reload)
{
    if (reload)
    {
        std::vector<std::string> configErrors;
        if (!sConfigMgr->Reload(configErrors))
        {
            for (std::string const& configError : configErrors)
                TC_LOG_ERROR("misc", "World settings reload fail: %s.", configError.c_str());

            return;
        }
    }

    if (reload)
        sLog->LoadFromConfig();

    m_defaultDbcLocale = LocaleConstant(sConfigMgr->GetIntDefault("DBC.Locale", 0));

    if (m_defaultDbcLocale >= TOTAL_LOCALES)
    {
        TC_LOG_ERROR("server.loading", "Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)", TOTAL_LOCALES);
        m_defaultDbcLocale = LOCALE_enUS;
    }

    TC_LOG_INFO("server.loading", "Using %s DBC Locale", localeNames[m_defaultDbcLocale]);

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerAmountLimit(sConfigMgr->GetIntDefault("PlayerLimit", 100));
    SetMotd(sConfigMgr->GetStringDefault("Motd", "Welcome to a Trinity Core Server."));

    ///- Read ticket system setting from the config file
    m_bool_configs[CONFIG_TICKETS_FEEDBACK_SYSTEM_ENABLED] = sConfigMgr->GetBoolDefault("TicketSystem.FeedBackTickets", true);
    m_bool_configs[CONFIG_TICKETS_GM_ENABLED]    = sConfigMgr->GetBoolDefault("TicketSystem.GMTickets", true);
    if (reload)
    {
        sTicketMgr->SetFeedBackSystemStatus(m_bool_configs[CONFIG_TICKETS_FEEDBACK_SYSTEM_ENABLED]);
        sTicketMgr->SetGmTicketSystemStatus(m_bool_configs[CONFIG_TICKETS_GM_ENABLED]);
    }

    ///- Get string for new logins (newly created characters)
    SetNewCharString(sConfigMgr->GetStringDefault("PlayerStart.String", ""));

    ///- Send server info on login?
    m_int_configs[CONFIG_ENABLE_SINFO_LOGIN] = sConfigMgr->GetIntDefault("Server.LoginInfo", 0);

    m_bool_configs[CONFIG_BONUS_RATES_ENABLED] = sConfigMgr->GetBoolDefault("BonusRates.Enabled", false);

    LoadRates(true);

    ///- Read other configuration items from the config file

    m_bool_configs[CONFIG_DURABILITY_LOSS_IN_PVP] = sConfigMgr->GetBoolDefault("DurabilityLoss.InPvP", false);

    m_int_configs[CONFIG_COMPRESSION] = sConfigMgr->GetIntDefault("Compression", 1);
    if (m_int_configs[CONFIG_COMPRESSION] < 1 || m_int_configs[CONFIG_COMPRESSION] > 9)
    {
        TC_LOG_ERROR("server.loading", "Compression level (%i) must be in range 1..9. Using default compression level (1).", m_int_configs[CONFIG_COMPRESSION]);
        m_int_configs[CONFIG_COMPRESSION] = 1;
    }
    m_bool_configs[CONFIG_ADDON_CHANNEL] = sConfigMgr->GetBoolDefault("AddonChannel", true);
    m_bool_configs[CONFIG_CLEAN_CHARACTER_DB] = sConfigMgr->GetBoolDefault("CleanCharacterDB", false);
    m_int_configs[CONFIG_PERSISTENT_CHARACTER_CLEAN_FLAGS] = sConfigMgr->GetIntDefault("PersistentCharacterCleanFlags", 0);
    m_int_configs[CONFIG_AUCTION_GETALL_DELAY] = sConfigMgr->GetIntDefault("Auction.GetAllScanDelay", 900);
    m_int_configs[CONFIG_AUCTION_SEARCH_DELAY] = sConfigMgr->GetIntDefault("Auction.SearchDelay", 300);
    if (m_int_configs[CONFIG_AUCTION_SEARCH_DELAY] < 100 || m_int_configs[CONFIG_AUCTION_SEARCH_DELAY] > 10000)
    {
        TC_LOG_ERROR("server.loading", "Auction.SearchDelay (%i) must be between 100 and 10000. Using default of 300ms", m_int_configs[CONFIG_AUCTION_SEARCH_DELAY]);
        m_int_configs[CONFIG_AUCTION_SEARCH_DELAY] = 300;
    }
    m_int_configs[CONFIG_CHAT_CHANNEL_LEVEL_REQ] = sConfigMgr->GetIntDefault("ChatLevelReq.Channel", 1);
    m_int_configs[CONFIG_CHAT_WHISPER_LEVEL_REQ] = sConfigMgr->GetIntDefault("ChatLevelReq.Whisper", 1);
    m_int_configs[CONFIG_CHAT_EMOTE_LEVEL_REQ] = sConfigMgr->GetIntDefault("ChatLevelReq.Emote", 1);
    m_int_configs[CONFIG_CHAT_SAY_LEVEL_REQ] = sConfigMgr->GetIntDefault("ChatLevelReq.Say", 1);
    m_int_configs[CONFIG_CHAT_YELL_LEVEL_REQ] = sConfigMgr->GetIntDefault("ChatLevelReq.Yell", 1);
    m_int_configs[CONFIG_TRADE_LEVEL_REQ] = sConfigMgr->GetIntDefault("LevelReq.Trade", 1);
    m_int_configs[CONFIG_TICKET_LEVEL_REQ] = sConfigMgr->GetIntDefault("LevelReq.Ticket", 1);
    m_int_configs[CONFIG_AUCTION_LEVEL_REQ] = sConfigMgr->GetIntDefault("LevelReq.Auction", 1);
    m_int_configs[CONFIG_MAIL_LEVEL_REQ] = sConfigMgr->GetIntDefault("LevelReq.Mail", 1);
    m_bool_configs[CONFIG_PRESERVE_CUSTOM_CHANNELS] = sConfigMgr->GetBoolDefault("PreserveCustomChannels", false);
    m_int_configs[CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION] = sConfigMgr->GetIntDefault("PreserveCustomChannelDuration", 14);
    m_bool_configs[CONFIG_GRID_UNLOAD] = sConfigMgr->GetBoolDefault("GridUnload", true);
    m_int_configs[CONFIG_INTERVAL_SAVE] = sConfigMgr->GetIntDefault("PlayerSaveInterval", 15 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_INTERVAL_DISCONNECT_TOLERANCE] = sConfigMgr->GetIntDefault("DisconnectToleranceInterval", 0);
    m_bool_configs[CONFIG_STATS_SAVE_ONLY_ON_LOGOUT] = sConfigMgr->GetBoolDefault("PlayerSave.Stats.SaveOnlyOnLogout", true);

    m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = sConfigMgr->GetIntDefault("PlayerSave.Stats.MinLevel", 0);
    if (m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] > MAX_LEVEL)
    {
        TC_LOG_ERROR("server.loading", "PlayerSave.Stats.MinLevel (%i) must be in range 0..80. Using default, do not save character stats (0).", m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE]);
        m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = 0;
    }

    m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] = sConfigMgr->GetIntDefault("GridCleanUpDelay", 5 * MINUTE * IN_MILLISECONDS);
    if (m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] < MIN_GRID_DELAY)
    {
        TC_LOG_ERROR("server.loading", "GridCleanUpDelay (%i) must be greater %u. Use this minimal value.", m_int_configs[CONFIG_INTERVAL_GRIDCLEAN], MIN_GRID_DELAY);
        m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] = MIN_GRID_DELAY;
    }
    if (reload)
        sMapMgr->SetGridCleanUpDelay(m_int_configs[CONFIG_INTERVAL_GRIDCLEAN]);

    m_int_configs[CONFIG_INTERVAL_MAPUPDATE] = 200; // Don't mess around with mapupdate cause of the issues with the mmaps.

    m_int_configs[CONFIG_INTERVAL_CHANGEWEATHER] = sConfigMgr->GetIntDefault("ChangeWeatherInterval", 10 * MINUTE * IN_MILLISECONDS);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("WorldServerPort", 8085);
        if (val != m_int_configs[CONFIG_PORT_WORLD])
            TC_LOG_ERROR("server.loading", "WorldServerPort option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_PORT_WORLD]);
    }
    else
        m_int_configs[CONFIG_PORT_WORLD] = sConfigMgr->GetIntDefault("WorldServerPort", 8085);

    // Config values are in "milliseconds" but we handle SocketTimeOut only as "seconds" so divide by 1000
    m_int_configs[CONFIG_SOCKET_TIMEOUTTIME] = sConfigMgr->GetIntDefault("SocketTimeOutTime", 900000) / 1000;
    m_int_configs[CONFIG_SOCKET_TIMEOUTTIME_ACTIVE] = sConfigMgr->GetIntDefault("SocketTimeOutTimeActive", 60000) / 1000;

    m_int_configs[CONFIG_SESSION_ADD_DELAY] = sConfigMgr->GetIntDefault("SessionAddDelay", 10000);

    m_float_configs[CONFIG_GROUP_XP_DISTANCE] = sConfigMgr->GetFloatDefault("MaxGroupXPDistance", 74.0f);
    m_float_configs[CONFIG_MAX_RECRUIT_A_FRIEND_DISTANCE] = sConfigMgr->GetFloatDefault("MaxRecruitAFriendBonusDistance", 100.0f);

    /// @todo Add MonsterSight and GuarderSight (with meaning) in worldserver.conf or put them as define
    m_float_configs[CONFIG_SIGHT_MONSTER] = sConfigMgr->GetFloatDefault("MonsterSight", 50);
    m_float_configs[CONFIG_SIGHT_GUARDER] = sConfigMgr->GetFloatDefault("GuarderSight", 50);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("GameType", 0);
        if (val != m_int_configs[CONFIG_GAME_TYPE])
            TC_LOG_ERROR("server.loading", "GameType option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_GAME_TYPE]);
    }
    else
        m_int_configs[CONFIG_GAME_TYPE] = sConfigMgr->GetIntDefault("GameType", 0);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("RealmZone", REALM_ZONE_DEVELOPMENT);
        if (val != m_int_configs[CONFIG_REALM_ZONE])
            TC_LOG_ERROR("server.loading", "RealmZone option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_REALM_ZONE]);
    }
    else
        m_int_configs[CONFIG_REALM_ZONE] = sConfigMgr->GetIntDefault("RealmZone", REALM_ZONE_DEVELOPMENT);

    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CALENDAR]= sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Calendar", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL] = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Channel", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP]   = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Group", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD]   = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Guild", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION] = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Auction", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_TRADE]               = sConfigMgr->GetBoolDefault("AllowTwoSide.Trade", false);
    m_int_configs[CONFIG_STRICT_PLAYER_NAMES]                 = sConfigMgr->GetIntDefault ("StrictPlayerNames",  0);
    m_int_configs[CONFIG_STRICT_CHARTER_NAMES]                = sConfigMgr->GetIntDefault ("StrictCharterNames", 0);
    m_int_configs[CONFIG_STRICT_PET_NAMES]                    = sConfigMgr->GetIntDefault ("StrictPetNames",     0);

    m_int_configs[CONFIG_MIN_PLAYER_NAME]                     = sConfigMgr->GetIntDefault ("MinPlayerName",  2);
    if (m_int_configs[CONFIG_MIN_PLAYER_NAME] < 1 || m_int_configs[CONFIG_MIN_PLAYER_NAME] > MAX_PLAYER_NAME)
    {
        TC_LOG_ERROR("server.loading", "MinPlayerName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PLAYER_NAME], MAX_PLAYER_NAME);
        m_int_configs[CONFIG_MIN_PLAYER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_CHARTER_NAME]                    = sConfigMgr->GetIntDefault ("MinCharterName", 2);
    if (m_int_configs[CONFIG_MIN_CHARTER_NAME] < 1 || m_int_configs[CONFIG_MIN_CHARTER_NAME] > MAX_CHARTER_NAME)
    {
        TC_LOG_ERROR("server.loading", "MinCharterName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_CHARTER_NAME], MAX_CHARTER_NAME);
        m_int_configs[CONFIG_MIN_CHARTER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_PET_NAME]                        = sConfigMgr->GetIntDefault ("MinPetName",     2);
    if (m_int_configs[CONFIG_MIN_PET_NAME] < 1 || m_int_configs[CONFIG_MIN_PET_NAME] > MAX_PET_NAME)
    {
        TC_LOG_ERROR("server.loading", "MinPetName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PET_NAME], MAX_PET_NAME);
        m_int_configs[CONFIG_MIN_PET_NAME] = 2;
    }

    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED] = sConfigMgr->GetIntDefault("CharacterCreating.Disabled", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_RACEMASK] = sConfigMgr->GetIntDefault("CharacterCreating.Disabled.RaceMask", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_CLASSMASK] = sConfigMgr->GetIntDefault("CharacterCreating.Disabled.ClassMask", 0);

    m_int_configs[CONFIG_CHARACTERS_PER_REALM] = sConfigMgr->GetIntDefault("CharactersPerRealm", 11);
    if (m_int_configs[CONFIG_CHARACTERS_PER_REALM] < 1 || m_int_configs[CONFIG_CHARACTERS_PER_REALM] > 11)
    {
        TC_LOG_ERROR("server.loading", "CharactersPerRealm (%i) must be in range 1..11. Set to 11.", m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_REALM] = 11;
    }

    // must be after CONFIG_CHARACTERS_PER_REALM
    m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = sConfigMgr->GetIntDefault("CharactersPerAccount", 50);
    if (m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] < m_int_configs[CONFIG_CHARACTERS_PER_REALM])
    {
        TC_LOG_ERROR("server.loading", "CharactersPerAccount (%i) can't be less than CharactersPerRealm (%i).", m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT], m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = m_int_configs[CONFIG_CHARACTERS_PER_REALM];
    }

    m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = sConfigMgr->GetIntDefault("HeroicCharactersPerRealm", 1);
    if (int32(m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]) < 0 || m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] > 10)
    {
        TC_LOG_ERROR("server.loading", "HeroicCharactersPerRealm (%i) must be in range 0..10. Set to 1.", m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = 1;
    }

    m_int_configs[CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER] = sConfigMgr->GetIntDefault("CharacterCreating.MinLevelForHeroicCharacter", 55);

    m_int_configs[CONFIG_SKIP_CINEMATICS] = sConfigMgr->GetIntDefault("SkipCinematics", 0);
    if (int32(m_int_configs[CONFIG_SKIP_CINEMATICS]) < 0 || m_int_configs[CONFIG_SKIP_CINEMATICS] > 2)
    {
        TC_LOG_ERROR("server.loading", "SkipCinematics (%i) must be in range 0..2. Set to 0.", m_int_configs[CONFIG_SKIP_CINEMATICS]);
        m_int_configs[CONFIG_SKIP_CINEMATICS] = 0;
    }

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("MaxPlayerLevel", DEFAULT_MAX_LEVEL);
        if (val != m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
            TC_LOG_ERROR("server.loading", "MaxPlayerLevel option can't be changed at config reload, using current value (%u).", m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
    }
    else
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("MaxPlayerLevel", DEFAULT_MAX_LEVEL);

    if (m_int_configs[CONFIG_MAX_PLAYER_LEVEL] > MAX_LEVEL)
    {
        TC_LOG_ERROR("server.loading", "MaxPlayerLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_MAX_PLAYER_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = MAX_LEVEL;
    }

    m_int_configs[CONFIG_MIN_DUALSPEC_LEVEL] = sConfigMgr->GetIntDefault("MinDualSpecLevel", 40);

    m_int_configs[CONFIG_START_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("StartPlayerLevel", 1);
    if (m_int_configs[CONFIG_START_PLAYER_LEVEL] < 1)
    {
        TC_LOG_ERROR("server.loading", "StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 1.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = 1;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        TC_LOG_ERROR("server.loading", "StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("StartHeroicPlayerLevel", 55);
    if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] < 1)
    {
        TC_LOG_ERROR("server.loading", "StartHeroicPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 55.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = 55;
    }
    else if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        TC_LOG_ERROR("server.loading", "StartHeroicPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_PLAYER_MONEY] = sConfigMgr->GetIntDefault("StartPlayerMoney", 0);
    if (int32(m_int_configs[CONFIG_START_PLAYER_MONEY]) < 0)
    {
        TC_LOG_ERROR("server.loading", "StartPlayerMoney (%i) must be in range 0.." UI64FMTD ". Set to %u.", m_int_configs[CONFIG_START_PLAYER_MONEY], uint64(MAX_MONEY_AMOUNT), 0);
        m_int_configs[CONFIG_START_PLAYER_MONEY] = 0;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_MONEY] > 0x7FFFFFFF-1) // TODO: (See MAX_MONEY_AMOUNT)
    {
        TC_LOG_ERROR("server.loading", "StartPlayerMoney (%i) must be in range 0..%u. Set to %u.",
            m_int_configs[CONFIG_START_PLAYER_MONEY], 0x7FFFFFFF-1, 0x7FFFFFFF-1);
        m_int_configs[CONFIG_START_PLAYER_MONEY] = 0x7FFFFFFF-1;
    }

    m_int_configs[CONFIG_CURRENCY_RESET_HOUR] = sConfigMgr->GetIntDefault("Currency.ResetHour", 3);
    if (m_int_configs[CONFIG_CURRENCY_RESET_HOUR] > 23)
    {
        TC_LOG_ERROR("server.loading", "Currency.ResetHour (%i) can't be load. Set to 6.", m_int_configs[CONFIG_CURRENCY_RESET_HOUR]);
        m_int_configs[CONFIG_CURRENCY_RESET_HOUR] = 3;
    }
    m_int_configs[CONFIG_CURRENCY_RESET_DAY] = sConfigMgr->GetIntDefault("Currency.ResetDay", 3);
    if (m_int_configs[CONFIG_CURRENCY_RESET_DAY] > 7 || m_int_configs[CONFIG_CURRENCY_RESET_DAY] < 1)
    {
        TC_LOG_ERROR("server.loading", "Currency.ResetDay (%i) can't be load. Set to 3.", m_int_configs[CONFIG_CURRENCY_RESET_DAY]);
        m_int_configs[CONFIG_CURRENCY_RESET_DAY] = 3;
    }
    m_int_configs[CONFIG_CURRENCY_RESET_INTERVAL] = sConfigMgr->GetIntDefault("Currency.ResetInterval", 7);
    if (int32(m_int_configs[CONFIG_CURRENCY_RESET_INTERVAL]) <= 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.ResetInterval (%i) must be > 0, set to default 7.", m_int_configs[CONFIG_CURRENCY_RESET_INTERVAL]);
        m_int_configs[CONFIG_CURRENCY_RESET_INTERVAL] = 7;
    }

    m_int_configs[CONFIG_CURRENCY_START_HONOR_POINTS] = sConfigMgr->GetIntDefault("Currency.StartHonorPoints", 0);
    if (int32(m_int_configs[CONFIG_CURRENCY_START_HONOR_POINTS]) < 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.StartHonorPoints (%i) must be >= 0, set to default 0.", m_int_configs[CONFIG_CURRENCY_START_HONOR_POINTS]);
        m_int_configs[CONFIG_CURRENCY_START_HONOR_POINTS] = 0;
    }
    m_int_configs[CONFIG_CURRENCY_MAX_HONOR_POINTS] = sConfigMgr->GetIntDefault("Currency.MaxHonorPoints", 4000);
    if (int32(m_int_configs[CONFIG_CURRENCY_MAX_HONOR_POINTS]) < 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.MaxHonorPoints (%i) can't be negative. Set to default 4000.", m_int_configs[CONFIG_CURRENCY_MAX_HONOR_POINTS]);
        m_int_configs[CONFIG_CURRENCY_MAX_HONOR_POINTS] = 4000;
    }
    m_int_configs[CONFIG_CURRENCY_MAX_HONOR_POINTS] *= 100;     //precision mod

    m_int_configs[CONFIG_CURRENCY_START_JUSTICE_POINTS] = sConfigMgr->GetIntDefault("Currency.StartJusticePoints", 0);
    if (int32(m_int_configs[CONFIG_CURRENCY_START_JUSTICE_POINTS]) < 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.StartJusticePoints (%i) must be >= 0, set to default 0.", m_int_configs[CONFIG_CURRENCY_START_JUSTICE_POINTS]);
        m_int_configs[CONFIG_CURRENCY_START_JUSTICE_POINTS] = 0;
    }
    m_int_configs[CONFIG_CURRENCY_MAX_JUSTICE_POINTS] = sConfigMgr->GetIntDefault("Currency.MaxJusticePoints", 4000);
    if (int32(m_int_configs[CONFIG_CURRENCY_MAX_JUSTICE_POINTS]) < 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.MaxJusticePoints (%i) can't be negative. Set to default 4000.", m_int_configs[CONFIG_CURRENCY_MAX_JUSTICE_POINTS]);
        m_int_configs[CONFIG_CURRENCY_MAX_JUSTICE_POINTS] = 4000;
    }
    m_int_configs[CONFIG_CURRENCY_MAX_JUSTICE_POINTS] *= 100;     //precision mod

    m_int_configs[CONFIG_CURRENCY_START_CONQUEST_POINTS] = sConfigMgr->GetIntDefault("Currency.StartConquestPoints", 0);
    if (int32(m_int_configs[CONFIG_CURRENCY_START_CONQUEST_POINTS]) < 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.StartConquestPoints (%i) must be >= 0, set to default 0.", m_int_configs[CONFIG_CURRENCY_START_CONQUEST_POINTS]);
        m_int_configs[CONFIG_CURRENCY_START_CONQUEST_POINTS] = 0;
    }
    m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_WEEK_CAP] = sConfigMgr->GetIntDefault("Currency.ConquestPointsWeekCap", 1650);
    if (int32(m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_WEEK_CAP]) <= 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.ConquestPointsWeekCap (%i) must be > 0, set to default 1650.", m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_WEEK_CAP]);
        m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_WEEK_CAP] = 1650;
    }
    m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_WEEK_CAP] *= 100;     //precision mod

    int32 arenaReward   = sConfigMgr->GetIntDefault("Currency.ConquestPointsArenaReward", 180);
    int32 ratedBgReward = sConfigMgr->GetIntDefault("Currency.ConquestPointsRatedBgReward", 400);

    if (arenaReward <= 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.ConquestPointsArenaReward (%i) must be > 0, set to default 180.", arenaReward);
        m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_ARENA_REWARD] = 180;
    }
    if (ratedBgReward <= 0)
    {
        TC_LOG_ERROR("server.loading", "Currency.ConquestPointsRatedBgReward (%i) must be > 0, set to default 400.", ratedBgReward);
        m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_ARENA_REWARD] = 400;
    }

    m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_ARENA_REWARD]     = arenaReward   * 100;     //precision mod
    m_int_configs[CONFIG_CURRENCY_CONQUEST_POINTS_RATED_BG_REWARD]  = ratedBgReward * 100;

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("RecruitAFriend.MaxLevel", 60);
    if (m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        TC_LOG_ERROR("server.loading", "RecruitAFriend.MaxLevel (%i) must be in the range 0..MaxLevel(%u). Set to %u.",
            m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], 60);
        m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = 60;
    }

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL_DIFFERENCE] = sConfigMgr->GetIntDefault("RecruitAFriend.MaxDifference", 4);
    m_bool_configs[CONFIG_ALL_TAXI_PATHS] = sConfigMgr->GetBoolDefault("AllFlightPaths", false);
    m_bool_configs[CONFIG_INSTANT_TAXI] = sConfigMgr->GetBoolDefault("InstantFlightPaths", false);

    m_bool_configs[CONFIG_INSTANCE_IGNORE_LEVEL] = sConfigMgr->GetBoolDefault("Instance.IgnoreLevel", false);
    m_bool_configs[CONFIG_INSTANCE_IGNORE_RAID]  = sConfigMgr->GetBoolDefault("Instance.IgnoreRaid", false);

    m_bool_configs[CONFIG_CAST_UNSTUCK] = sConfigMgr->GetBoolDefault("CastUnstuck", true);
    int32 instanceResetDay = sConfigMgr->GetIntDefault("Instance.ResetTimeDay", 3);
    if (instanceResetDay < 1 || instanceResetDay > 7)
    {
        TC_LOG_ERROR("server.loading", "Instance.ResetTimeDay (%i) must be in the range 1..7. Set to 3.", instanceResetDay);
        instanceResetDay = 3;
    }
    m_int_configs[CONFIG_INSTANCE_RESET_TIME_DAY] = instanceResetDay;

    int32 instanceResetHour = sConfigMgr->GetIntDefault("Instance.ResetTimeHour", 6);
    if (instanceResetHour < 0 || instanceResetHour > 23)
    {
        TC_LOG_ERROR("server.loading", "Instance.ResetTimeHour (%i) must be in the range 0..23. Set to 6.", instanceResetHour);
        instanceResetHour = 6;
    }
    m_int_configs[CONFIG_INSTANCE_RESET_TIME_HOUR] = instanceResetHour;

    m_int_configs[CONFIG_INSTANCE_UNLOAD_DELAY] = sConfigMgr->GetIntDefault("Instance.UnloadDelay", 30 * MINUTE * IN_MILLISECONDS);

    m_int_configs[CONFIG_MAX_PRIMARY_TRADE_SKILL] = sConfigMgr->GetIntDefault("MaxPrimaryTradeSkill", 2);
    m_int_configs[CONFIG_MIN_PETITION_SIGNS] = sConfigMgr->GetIntDefault("MinPetitionSigns", 9);
    if (m_int_configs[CONFIG_MIN_PETITION_SIGNS] > 9)
    {
        TC_LOG_ERROR("server.loading", "MinPetitionSigns (%i) must be in range 0..9. Set to 9.", m_int_configs[CONFIG_MIN_PETITION_SIGNS]);
        m_int_configs[CONFIG_MIN_PETITION_SIGNS] = 9;
    }

    m_int_configs[CONFIG_GM_LOGIN_STATE]        = sConfigMgr->GetIntDefault("GM.LoginState", 2);
    m_int_configs[CONFIG_GM_VISIBLE_STATE]      = sConfigMgr->GetIntDefault("GM.Visible", 2);
    m_int_configs[CONFIG_GM_CHAT]               = sConfigMgr->GetIntDefault("GM.Chat", 2);
    m_int_configs[CONFIG_GM_WHISPERING_TO]      = sConfigMgr->GetIntDefault("GM.WhisperingTo", 2);

    m_int_configs[CONFIG_GM_LEVEL_IN_GM_LIST]   = sConfigMgr->GetIntDefault("GM.InGMList.Level", SEC_ADMINISTRATOR);
    m_int_configs[CONFIG_GM_LEVEL_IN_WHO_LIST]  = sConfigMgr->GetIntDefault("GM.InWhoList.Level", SEC_ADMINISTRATOR);
    m_int_configs[CONFIG_START_GM_LEVEL]        = sConfigMgr->GetIntDefault("GM.StartLevel", 1);
    if (m_int_configs[CONFIG_START_GM_LEVEL] < m_int_configs[CONFIG_START_PLAYER_LEVEL])
    {
        TC_LOG_ERROR("server.loading", "GM.StartLevel (%i) must be in range StartPlayerLevel(%u)..%u. Set to %u.",
            m_int_configs[CONFIG_START_GM_LEVEL], m_int_configs[CONFIG_START_PLAYER_LEVEL], MAX_LEVEL, m_int_configs[CONFIG_START_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_GM_LEVEL] = m_int_configs[CONFIG_START_PLAYER_LEVEL];
    }
    else if (m_int_configs[CONFIG_START_GM_LEVEL] > MAX_LEVEL)
    {
        TC_LOG_ERROR("server.loading", "GM.StartLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_START_GM_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_START_GM_LEVEL] = MAX_LEVEL;
    }
    m_bool_configs[CONFIG_ALLOW_GM_GROUP]       = sConfigMgr->GetBoolDefault("GM.AllowInvite", false);
    m_bool_configs[CONFIG_GM_LOWER_SECURITY] = sConfigMgr->GetBoolDefault("GM.LowerSecurity", false);
    m_float_configs[CONFIG_CHANCE_OF_GM_SURVEY] = sConfigMgr->GetFloatDefault("GM.TicketSystem.ChanceOfGMSurvey", 50.0f);

    m_bool_configs[CONFIG_GM_USE_ONLINE_MUTES] = sConfigMgr->GetBoolDefault("GM.UseOnlineMutes", false);
    m_int_configs[CONFIG_GM_MAX_MUTE_TIME] = sConfigMgr->GetIntDefault("GM.MaxMuteTime", 1440);
    if (m_int_configs[CONFIG_GM_MAX_MUTE_TIME] < 1)
    {
        TC_LOG_ERROR("server.loading", "GM.MaxMuteTime (%i) must be > 0, set to default 1440", m_int_configs[CONFIG_GM_MAX_MUTE_TIME]);
        m_int_configs[CONFIG_GM_MAX_MUTE_TIME] = 1440;
    }

    m_int_configs[CONFIG_GROUP_VISIBILITY] = sConfigMgr->GetIntDefault("Visibility.GroupMode", 1);

    m_int_configs[CONFIG_MAIL_DELIVERY_DELAY] = sConfigMgr->GetIntDefault("MailDeliveryDelay", HOUR);

    m_int_configs[CONFIG_UPTIME_UPDATE] = sConfigMgr->GetIntDefault("UpdateUptimeInterval", 10);
    if (int32(m_int_configs[CONFIG_UPTIME_UPDATE]) <= 0)
    {
        TC_LOG_ERROR("server.loading", "UpdateUptimeInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_UPTIME_UPDATE]);
        m_int_configs[CONFIG_UPTIME_UPDATE] = 10;
    }
    if (reload)
    {
        m_timers[WUPDATE_UPTIME].SetInterval(m_int_configs[CONFIG_UPTIME_UPDATE]*MINUTE*IN_MILLISECONDS);
        m_timers[WUPDATE_UPTIME].Reset();
    }

    // log db cleanup interval
    m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = sConfigMgr->GetIntDefault("LogDB.Opt.ClearInterval", 10);
    if (int32(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]) <= 0)
    {
        TC_LOG_ERROR("server.loading", "LogDB.Opt.ClearInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]);
        m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = 10;
    }
    if (reload)
    {
        m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] * MINUTE * IN_MILLISECONDS);
        m_timers[WUPDATE_CLEANDB].Reset();
    }

    m_int_configs[CONFIG_LOGDB_CLEARTIME] = sConfigMgr->GetIntDefault("LogDB.Opt.ClearTime", 1209600); // 14 days default
    TC_LOG_INFO("server.loading", "Will clear `logs` table of entries older than %i seconds every %u minutes.",
        m_int_configs[CONFIG_LOGDB_CLEARTIME], m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]);

    m_int_configs[CONFIG_SKILL_CHANCE_ORANGE] = sConfigMgr->GetIntDefault("SkillChance.Orange", 100);
    m_int_configs[CONFIG_SKILL_CHANCE_YELLOW] = sConfigMgr->GetIntDefault("SkillChance.Yellow", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_GREEN]  = sConfigMgr->GetIntDefault("SkillChance.Green", 25);
    m_int_configs[CONFIG_SKILL_CHANCE_GREY]   = sConfigMgr->GetIntDefault("SkillChance.Grey", 0);

    m_int_configs[CONFIG_SKILL_CHANCE_MINING_STEPS]  = sConfigMgr->GetIntDefault("SkillChance.MiningSteps", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_SKINNING_STEPS]   = sConfigMgr->GetIntDefault("SkillChance.SkinningSteps", 75);

    m_bool_configs[CONFIG_SKILL_PROSPECTING] = sConfigMgr->GetBoolDefault("SkillChance.Prospecting", false);
    m_bool_configs[CONFIG_SKILL_MILLING] = sConfigMgr->GetBoolDefault("SkillChance.Milling", false);

    m_int_configs[CONFIG_SKILL_GAIN_CRAFTING]  = sConfigMgr->GetIntDefault("SkillGain.Crafting", 1);

    m_int_configs[CONFIG_SKILL_GAIN_GATHERING]  = sConfigMgr->GetIntDefault("SkillGain.Gathering", 1);

    m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = sConfigMgr->GetIntDefault("MaxOverspeedPings", 2);

    if (m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] != 0 && m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] < 2)
    {
        TC_LOG_ERROR("server.loading", "MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check). Set to 2.", m_int_configs[CONFIG_MAX_OVERSPEED_PINGS]);
        m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = 2;
    }

    m_bool_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY] = sConfigMgr->GetBoolDefault("SaveRespawnTimeImmediately", true);
    m_bool_configs[CONFIG_WEATHER] = sConfigMgr->GetBoolDefault("ActivateWeather", true);

    m_int_configs[CONFIG_DISABLE_BREATHING] = sConfigMgr->GetIntDefault("DisableWaterBreath", SEC_CONSOLE);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("Expansion", 1);
        if (val != m_int_configs[CONFIG_EXPANSION])
            TC_LOG_ERROR("server.loading", "Expansion option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_EXPANSION]);
    }
    else
        m_int_configs[CONFIG_EXPANSION] = sConfigMgr->GetIntDefault("Expansion", 1);

    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_COUNT] = sConfigMgr->GetIntDefault("ChatFlood.MessageCount", 10);
    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_DELAY] = sConfigMgr->GetIntDefault("ChatFlood.MessageDelay", 1);
    m_int_configs[CONFIG_CHATFLOOD_MUTE_TIME]     = sConfigMgr->GetIntDefault("ChatFlood.MuteTime", 10);

    m_int_configs[CONFIG_WORD_FILTER_MUTE_DURATION] = sConfigMgr->GetIntDefault("WordFilter.MuteDuration", 30000);
    m_bool_configs[CONFIG_WORD_FILTER_ENABLE]       = sConfigMgr->GetBoolDefault("WordFilter.Enable", true);

    m_bool_configs[CONFIG_EVENT_ANNOUNCE] = sConfigMgr->GetIntDefault("Event.Announce", false);

    m_float_configs[CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS] = sConfigMgr->GetFloatDefault("CreatureFamilyFleeAssistanceRadius", 30.0f);
    m_float_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS] = sConfigMgr->GetFloatDefault("CreatureFamilyAssistanceRadius", 10.0f);
    m_int_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY]  = sConfigMgr->GetIntDefault("CreatureFamilyAssistanceDelay", 1500);
    m_int_configs[CONFIG_CREATURE_FAMILY_FLEE_DELAY]        = sConfigMgr->GetIntDefault("CreatureFamilyFleeDelay", 7000);

    m_int_configs[CONFIG_WORLD_BOSS_LEVEL_DIFF] = sConfigMgr->GetIntDefault("WorldBossLevelDiff", 3);

    // note: disable value (-1) will assigned as 0xFFFFFFF, to prevent overflow at calculations limit it to max possible player level MAX_LEVEL(100)
    m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = sConfigMgr->GetIntDefault("Quests.LowLevelHideDiff", 4);
    if (m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = sConfigMgr->GetIntDefault("Quests.HighLevelHideDiff", 7);
    if (m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_bool_configs[CONFIG_QUEST_IGNORE_RAID] = sConfigMgr->GetBoolDefault("Quests.IgnoreRaid", false);
    m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_ACCEPT] = sConfigMgr->GetBoolDefault("Quests.IgnoreAutoAccept", false);
    m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_COMPLETE] = sConfigMgr->GetBoolDefault("Quests.IgnoreAutoComplete", false);

    m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] = sConfigMgr->GetIntDefault("Battleground.Random.ResetHour", 6);
    if (m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] > 23)
    {
        TC_LOG_ERROR("server.loading", "Battleground.Random.ResetHour (%i) can't be load. Set to 6.", m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR]);
        m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] = 6;
    }

    m_int_configs[CONFIG_GUILD_RESET_HOUR] = sConfigMgr->GetIntDefault("Guild.ResetHour", 6);
    if (m_int_configs[CONFIG_GUILD_RESET_HOUR] > 23)
    {
        TC_LOG_ERROR("misc", "Guild.ResetHour (%i) can't be load. Set to 6.", m_int_configs[CONFIG_GUILD_RESET_HOUR]);
        m_int_configs[CONFIG_GUILD_RESET_HOUR] = 6;
    }

    m_bool_configs[CONFIG_DETECT_POS_COLLISION] = sConfigMgr->GetBoolDefault("DetectPosCollision", true);

    m_int_configs[CONFIG_CHANNEL_CONSTANT_LOCALE]      = sConfigMgr->GetIntDefault("Channel.ConstantLocale", 0);
    m_bool_configs[CONFIG_RESTRICTED_LFG_CHANNEL]      = sConfigMgr->GetBoolDefault("Channel.RestrictedLfg", true);
    m_bool_configs[CONFIG_TALENTS_INSPECTING]           = sConfigMgr->GetBoolDefault("TalentsInspecting", true);
    m_bool_configs[CONFIG_CHAT_FAKE_MESSAGE_PREVENTING] = sConfigMgr->GetBoolDefault("ChatFakeMessagePreventing", false);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY] = sConfigMgr->GetIntDefault("ChatStrictLinkChecking.Severity", 0);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_KICK] = sConfigMgr->GetIntDefault("ChatStrictLinkChecking.Kick", 0);

    m_int_configs[CONFIG_CORPSE_DECAY_NORMAL]    = sConfigMgr->GetIntDefault("Corpse.Decay.NORMAL", 60);
    m_int_configs[CONFIG_CORPSE_DECAY_RARE]      = sConfigMgr->GetIntDefault("Corpse.Decay.RARE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_ELITE]     = sConfigMgr->GetIntDefault("Corpse.Decay.ELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_RAREELITE] = sConfigMgr->GetIntDefault("Corpse.Decay.RAREELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_WORLDBOSS] = sConfigMgr->GetIntDefault("Corpse.Decay.WORLDBOSS", 3600);

    m_int_configs[CONFIG_DEATH_SICKNESS_LEVEL]           = sConfigMgr->GetIntDefault ("Death.SicknessLevel", 11);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP] = sConfigMgr->GetBoolDefault("Death.CorpseReclaimDelay.PvP", true);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE] = sConfigMgr->GetBoolDefault("Death.CorpseReclaimDelay.PvE", true);
    m_bool_configs[CONFIG_DEATH_BONES_WORLD]              = sConfigMgr->GetBoolDefault("Death.Bones.World", true);
    m_bool_configs[CONFIG_DEATH_BONES_BG_OR_ARENA]        = sConfigMgr->GetBoolDefault("Death.Bones.BattlegroundOrArena", true);

    m_bool_configs[CONFIG_DIE_COMMAND_MODE] = sConfigMgr->GetBoolDefault("Die.Command.Mode", true);

    m_float_configs[CONFIG_THREAT_RADIUS] = sConfigMgr->GetFloatDefault("ThreatRadius", 60.0f);

    // always use declined names in the russian client
    m_bool_configs[CONFIG_DECLINED_NAMES_USED] =

        (m_int_configs[CONFIG_REALM_ZONE] == REALM_ZONE_RUSSIAN) ? true : sConfigMgr->GetBoolDefault("DeclinedNames", false);

    m_float_configs[CONFIG_LISTEN_RANGE_SAY]       = sConfigMgr->GetFloatDefault("ListenRange.Say", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_TEXTEMOTE] = sConfigMgr->GetFloatDefault("ListenRange.TextEmote", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_YELL]      = sConfigMgr->GetFloatDefault("ListenRange.Yell", 300.0f);

    m_bool_configs[CONFIG_BATTLEGROUND_CAST_DESERTER]                = sConfigMgr->GetBoolDefault("Battleground.CastDeserter", true);
    m_bool_configs[CONFIG_BATTLEGROUND_IGNORE_FACTION]               = sConfigMgr->GetBoolDefault("Battleground.IgnoreFaction", false);
    m_bool_configs[CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE]       = sConfigMgr->GetBoolDefault("Battleground.QueueAnnouncer.Enable", false);
    m_bool_configs[CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY]   = sConfigMgr->GetBoolDefault("Battleground.QueueAnnouncer.PlayerOnly", false);
    m_int_configs[CONFIG_BATTLEGROUND_INVITATION_TYPE]               = sConfigMgr->GetIntDefault ("Battleground.InvitationType", 0);
    m_int_configs[CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER]        = sConfigMgr->GetIntDefault ("Battleground.PrematureFinishTimer", 5 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH]  = sConfigMgr->GetIntDefault ("Battleground.PremadeGroupWaitForMatch", 30 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_BATTLEGROUND_HUGE_BG_START_TIME]            = sConfigMgr->GetIntDefault ("Battleground.HugeBgStartTime", 20);
    m_int_configs[CONFIG_BATTLEGROUND_HUGE_BG_END_TIME]              = sConfigMgr->GetIntDefault ("Battleground.HugeBgEndTime", 22);

    m_bool_configs[CONFIG_BATTLEGROUND_DT_BG_ENABLED]                = sConfigMgr->GetBoolDefault("Battleground.DtBgEnabled", false);
    m_int_configs[CONFIG_BATTLEGROUND_DT_BG_START_TIME]              = sConfigMgr->GetIntDefault("Battleground.DtBgStartTime", 10);
    m_int_configs[CONFIG_BATTLEGROUND_DT_BG_END_TIME]                = sConfigMgr->GetIntDefault("Battleground.DtBgEndTime", 23);

    m_bool_configs[CONFIG_BG_XP_FOR_KILL]                            = sConfigMgr->GetBoolDefault("Battleground.GiveXPForKills", false);
    m_int_configs[CONFIG_ARENA_MAX_RATING_DIFFERENCE]                = sConfigMgr->GetIntDefault ("Arena.MaxRatingDifference", 150);
    m_int_configs[CONFIG_ARENA_RATING_DISCARD_TIMER]                 = sConfigMgr->GetIntDefault ("Arena.RatingDiscardTimer", 10 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_ARENA_RATED_UPDATE_TIMER]                   = sConfigMgr->GetIntDefault ("Arena.RatedUpdateTimer", 5 * IN_MILLISECONDS);
    m_bool_configs[CONFIG_ARENA_QUEUE_ANNOUNCER_ENABLE]              = sConfigMgr->GetBoolDefault("Arena.QueueAnnouncer.Enable", false);
    m_bool_configs[CONFIG_ARENA_QUEUE_ANNOUNCER_PLAYERONLY]          = sConfigMgr->GetBoolDefault("Arena.QueueAnnouncer.PlayerOnly", false);
    m_int_configs[CONFIG_ARENA_SEASON_ID]                            = sConfigMgr->GetIntDefault ("Arena.ArenaSeason.ID", 1);
    m_int_configs[CONFIG_ARENA_START_RATING]                         = sConfigMgr->GetIntDefault ("Arena.ArenaStartRating", 0);
    m_int_configs[CONFIG_ARENA_START_PERSONAL_RATING]                = sConfigMgr->GetIntDefault ("Arena.ArenaStartPersonalRating", 1000);
    m_int_configs[CONFIG_ARENA_START_MATCHMAKER_RATING]              = sConfigMgr->GetIntDefault ("Arena.ArenaStartMatchmakerRating", 1500);
    m_int_configs[CONFIG_ARENA_PROGRESSIVE_MMR_TIMER]                = sConfigMgr->GetIntDefault ("Arena.ProgressiveMMRTimer", 30000);
    m_int_configs[CONFIG_ARENA_PROGRESSIVE_MMR_STEPSIZE]             = sConfigMgr->GetIntDefault ("Arena.ProgressiveMMRStepSize", 50);
    m_bool_configs[CONFIG_ARENA_SEASON_IN_PROGRESS]                  = sConfigMgr->GetBoolDefault("Arena.ArenaSeason.InProgress", true);
    m_bool_configs[CONFIG_ARENA_LOG_EXTENDED_INFO]                   = sConfigMgr->GetBoolDefault("ArenaLog.ExtendedInfo", false);

    m_bool_configs[CONFIG_OFFHAND_CHECK_AT_SPELL_UNLEARN]            = sConfigMgr->GetBoolDefault("OffhandCheckAtSpellUnlearn", true);

    if (int32 clientCacheId = sConfigMgr->GetIntDefault("ClientCacheVersion", 0))
    {
        // overwrite DB/old value
        if (clientCacheId > 0)
        {
            m_int_configs[CONFIG_CLIENTCACHE_VERSION] = clientCacheId;
            TC_LOG_INFO("server.loading", "Client cache version set to: %u", clientCacheId);
        }
        else
            TC_LOG_ERROR("server.loading", "ClientCacheVersion can't be negative %d, ignored.", clientCacheId);
    }

    m_int_configs[CONFIG_INSTANT_LOGOUT] = sConfigMgr->GetIntDefault("InstantLogout", SEC_MODERATOR);

    m_int_configs[CONFIG_GUILD_NEWS_LOG_COUNT] = sConfigMgr->GetIntDefault("Guild.NewsLogRecordsCount", GUILD_NEWSLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_NEWS_LOG_COUNT] > GUILD_NEWSLOG_MAX_RECORDS)
        m_int_configs[CONFIG_GUILD_NEWS_LOG_COUNT] = GUILD_NEWSLOG_MAX_RECORDS;
    m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = sConfigMgr->GetIntDefault("Guild.EventLogRecordsCount", GUILD_EVENTLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] > GUILD_EVENTLOG_MAX_RECORDS)
        m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = GUILD_EVENTLOG_MAX_RECORDS;
    m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = sConfigMgr->GetIntDefault("Guild.BankEventLogRecordsCount", GUILD_BANKLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] > GUILD_BANKLOG_MAX_RECORDS)
        m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = GUILD_BANKLOG_MAX_RECORDS;

    // battle pet
    m_int_configs[CONFIG_BATTLE_PET_LOADOUT_UNLOCK_COUNT] = sConfigMgr->GetIntDefault("BattlePet.LoadoutUnlockCount", 1);
    if (m_int_configs[CONFIG_BATTLE_PET_LOADOUT_UNLOCK_COUNT] > BATTLE_PET_MAX_LOADOUT_SLOTS)
    {
        TC_LOG_ERROR("server.loading", "BattlePet.LoadoutUnlockCount (%i) can't be loaded. Set to 1.", m_int_configs[CONFIG_BATTLE_PET_LOADOUT_UNLOCK_COUNT]);
        m_int_configs[CONFIG_BATTLE_PET_LOADOUT_UNLOCK_COUNT] = 1;
    }

    m_int_configs[CONFIG_BATTLE_PET_INITIAL_LEVEL] = sConfigMgr->GetIntDefault("BattlePet.InitialLevel", 1);
    if (m_int_configs[CONFIG_BATTLE_PET_INITIAL_LEVEL] > BATTLE_PET_MAX_LEVEL)
    {
        TC_LOG_ERROR("server.loading", "BattlePet.InitialLevel (%i) can't be loaded. Set to 1.", m_int_configs[CONFIG_BATTLE_PET_INITIAL_LEVEL]);
        m_int_configs[CONFIG_BATTLE_PET_INITIAL_LEVEL] = 1;
    }

    // blackmarket
    m_bool_configs[CONFIG_BLACK_MARKET_OPEN] = sConfigMgr->GetBoolDefault("BlackMarket.Open", false);
    m_int_configs[CONFIG_BLACK_MARKET_MAX_AUCTIONS] = sConfigMgr->GetIntDefault("BlackMarket.MaxAuctions", 10);
    if (m_int_configs[CONFIG_BLACK_MARKET_MAX_AUCTIONS] > CONFIG_BLACK_MARKET_MAX_AUCTIONS)
        m_int_configs[CONFIG_BLACK_MARKET_MAX_AUCTIONS] = CONFIG_BLACK_MARKET_MAX_AUCTIONS;
    m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY] = sConfigMgr->GetIntDefault("BlackMarket.AuctionDelay", 120);
    if (m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY] > CONFIG_BLACK_MARKET_AUCTION_DELAY)
        m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY] = CONFIG_BLACK_MARKET_AUCTION_DELAY;
    m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD] = sConfigMgr->GetIntDefault("BlackMarket.AuctionDelayMod", 60);
    if (m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD] > CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD)
        m_int_configs[CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD] = CONFIG_BLACK_MARKET_AUCTION_DELAY_MOD;

    m_int_configs[CONFIG_RESPAWN_GUIDWARNLEVEL] = sConfigMgr->GetIntDefault("Respawn.GuidWarnLevel", 12000000);
    if (m_int_configs[CONFIG_RESPAWN_GUIDWARNLEVEL] > 16777215)
    {
        TC_LOG_ERROR("server.loading", "Respawn.GuidWarnLevel (%u) cannot be greater than maximum GUID (16777215). Set to 12000000.", m_int_configs[CONFIG_RESPAWN_GUIDWARNLEVEL]);
        m_int_configs[CONFIG_RESPAWN_GUIDWARNLEVEL] = 12000000;
    }
    m_int_configs[CONFIG_RESPAWN_GUIDALERTLEVEL] = sConfigMgr->GetIntDefault("Respawn.GuidAlertLevel", 16000000);
    if (m_int_configs[CONFIG_RESPAWN_GUIDALERTLEVEL] > 16777215)
    {
        TC_LOG_ERROR("server.loading", "Respawn.GuidWarnLevel (%u) cannot be greater than maximum GUID (16777215). Set to 16000000.", m_int_configs[CONFIG_RESPAWN_GUIDALERTLEVEL]);
        m_int_configs[CONFIG_RESPAWN_GUIDALERTLEVEL] = 16000000;
    }

    // battle pay
    sBattlePayMgr->SetEnableState(sConfigMgr->GetBoolDefault("BattlePay.StoreEnabled", false));
    sBattlePayMgr->SetStoreCurrency(sConfigMgr->GetIntDefault("BattlePay.Currency", BATTLE_PAY_CURRENCY_BETA));

    //visibility on continents
    m_MaxVisibleDistanceOnContinents = sConfigMgr->GetFloatDefault("Visibility.Distance.Continents", DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceOnContinents < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.Continents can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceOnContinents = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceOnContinents > MAX_VISIBILITY_DISTANCE)
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.Continents can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceOnContinents = MAX_VISIBILITY_DISTANCE;
    }

    Visibility_RelocationLowerLimit = sConfigMgr->GetFloatDefault("Visibility.RelocationLowerLimit", 10.f);
    Visibility_AINotifyDelay = sConfigMgr->GetFloatDefault("Visibility.AINotifyDelay", 1000);

    //visibility in instances
    m_MaxVisibleDistanceInInstances = sConfigMgr->GetFloatDefault("Visibility.Distance.Instances", DEFAULT_VISIBILITY_INSTANCE);
    if (m_MaxVisibleDistanceInInstances < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.Instances can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInInstances = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInInstances > MAX_VISIBILITY_DISTANCE)
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.Instances can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInInstances = MAX_VISIBILITY_DISTANCE;
    }

    //visibility in BG/Arenas
    m_MaxVisibleDistanceInBGArenas = sConfigMgr->GetFloatDefault("Visibility.Distance.BGArenas", DEFAULT_VISIBILITY_BGARENAS);
    if (m_MaxVisibleDistanceInBGArenas < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.BGArenas can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInBGArenas = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInBGArenas > MAX_VISIBILITY_DISTANCE)
    {
        TC_LOG_ERROR("server.loading", "Visibility.Distance.BGArenas can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInBGArenas = MAX_VISIBILITY_DISTANCE;
    }

    m_visibility_notify_periodOnContinents = sConfigMgr->GetIntDefault("Visibility.Notify.Period.OnContinents", DEFAULT_VISIBILITY_NOTIFY_PERIOD);
    m_visibility_notify_periodInInstances = sConfigMgr->GetIntDefault("Visibility.Notify.Period.InInstances",   DEFAULT_VISIBILITY_NOTIFY_PERIOD);
    m_visibility_notify_periodInBGArenas = sConfigMgr->GetIntDefault("Visibility.Notify.Period.InBGArenas",    DEFAULT_VISIBILITY_NOTIFY_PERIOD);

    ///- Load the CharDelete related config options
    m_int_configs[CONFIG_CHARDELETE_METHOD] = sConfigMgr->GetIntDefault("CharDelete.Method", 0);
    m_int_configs[CONFIG_CHARDELETE_MIN_LEVEL] = sConfigMgr->GetIntDefault("CharDelete.MinLevel", 0);
    m_int_configs[CONFIG_CHARDELETE_HEROIC_MIN_LEVEL] = sConfigMgr->GetIntDefault("CharDelete.Heroic.MinLevel", 0);
    m_int_configs[CONFIG_CHARDELETE_KEEP_DAYS] = sConfigMgr->GetIntDefault("CharDelete.KeepDays", 30);

    m_int_configs[CONFIG_CUSTOM_VISIBILITY_MAP_THRESHOLD] = sConfigMgr->GetIntDefault("CustomVisibility.Threshold.Map", (uint32)CustomVisibility::Importance::DistantScenery);
    m_int_configs[CONFIG_CUSTOM_VISIBILITY_ZONE_THRESHOLD] = sConfigMgr->GetIntDefault("CustomVisibility.Threshold.Zone", (uint32)CustomVisibility::Importance::GroundClutter);

    m_bool_configs[CONFIG_EXECUTED_SERVICES_LOG] = sConfigMgr->GetBoolDefault("ExecutedServices.Log.Enabled", true);

    // custom balance system
    m_float_configs[CONFIGSPECMAGEARCANE] = sConfigMgr->GetFloatDefault("CONFIGSPECMAGEARCANE", 100.0f);
    m_float_configs[CONFIGSPECMAGEFIRE] = sConfigMgr->GetFloatDefault("CONFIGSPECMAGEFIRE", 100.0f);
    m_float_configs[CONFIGSPECMAGEFROST] = sConfigMgr->GetFloatDefault("CONFIGSPECMAGEFROST", 100.0f);
    m_float_configs[CONFIGSPECPALADINHOLY] = sConfigMgr->GetFloatDefault("CONFIGSPECPALADINHOLY", 100.0f);
    m_float_configs[CONFIGSPECPALADINPROTECTION] = sConfigMgr->GetFloatDefault("CONFIGSPECPALADINPROTECTION", 100.0f);
    m_float_configs[CONFIGSPECPALADINRETRIBUTION] = sConfigMgr->GetFloatDefault("CONFIGSPECPALADINRETRIBUTION", 100.0f);
    m_float_configs[CONFIGSPECWARRIORARMS] = sConfigMgr->GetFloatDefault("CONFIGSPECWARRIORARMS", 100.0f);
    m_float_configs[CONFIGSPECWARRIORFURY] = sConfigMgr->GetFloatDefault("CONFIGSPECWARRIORFURY", 100.0f);
    m_float_configs[CONFIGSPECWARRIORPROTECTION] = sConfigMgr->GetFloatDefault("CONFIGSPECWARRIORPROTECTION", 100.0f);
    m_float_configs[CONFIGSPECDRUIDBALANCE] = sConfigMgr->GetFloatDefault("CONFIGSPECDRUIDBALANCE", 100.0f);
    m_float_configs[CONFIGSPECDRUIDFERAL] = sConfigMgr->GetFloatDefault("CONFIGSPECDRUIDFERAL", 100.0f);
    m_float_configs[CONFIGSPECDRUIDGUARDIAN] = sConfigMgr->GetFloatDefault("CONFIGSPECDRUIDGUARDIAN", 100.0f);
    m_float_configs[CONFIGSPECDRUIDRESTORATION] = sConfigMgr->GetFloatDefault("CONFIGSPECDRUIDRESTORATION", 100.0f);
    m_float_configs[CONFIGSPECDEATHKNIGHTBLOOD] = sConfigMgr->GetFloatDefault("CONFIGSPECDEATHKNIGHTBLOOD", 100.0f);
    m_float_configs[CONFIGSPECDEATHKNIGHTFROST] = sConfigMgr->GetFloatDefault("CONFIGSPECDEATHKNIGHTFROST", 100.0f);
    m_float_configs[CONFIGSPECDEATHKNIGHTUNHOLY] = sConfigMgr->GetFloatDefault("CONFIGSPECDEATHKNIGHTUNHOLY", 100.0f);
    m_float_configs[CONFIGSPECHUNTERBEASTMASTERY] = sConfigMgr->GetFloatDefault("CONFIGSPECHUNTERBEASTMASTERY", 100.0f);
    m_float_configs[CONFIGSPECHUNTERMARKSMANSHIP] = sConfigMgr->GetFloatDefault("CONFIGSPECHUNTERMARKSMANSHIP", 100.0f);
    m_float_configs[CONFIGSPECHUNTERSURVIVAL] = sConfigMgr->GetFloatDefault("CONFIGSPECHUNTERSURVIVAL", 100.0f);
    m_float_configs[CONFIGSPECPRIESTDISCIPLINE] = sConfigMgr->GetFloatDefault("CONFIGSPECPRIESTDISCIPLINE", 100.0f);
    m_float_configs[CONFIGSPECPRIESTHOLY] = sConfigMgr->GetFloatDefault("CONFIGSPECPRIESTHOLY", 100.0f);
    m_float_configs[CONFIGSPECPRIESTSHADOW] = sConfigMgr->GetFloatDefault("CONFIGSPECPRIESTSHADOW", 100.0f);
    m_float_configs[CONFIGSPECROGUEASSASSINATION] = sConfigMgr->GetFloatDefault("CONFIGSPECROGUEASSASSINATION", 100.0f);
    m_float_configs[CONFIGSPECROGUECOMBAT] = sConfigMgr->GetFloatDefault("CONFIGSPECROGUECOMBAT", 100.0f);
    m_float_configs[CONFIGSPECROGUESUBTLETY] = sConfigMgr->GetFloatDefault("CONFIGSPECROGUESUBTLETY", 100.0f);
    m_float_configs[CONFIGSPECSHAMANELEMENTAL] = sConfigMgr->GetFloatDefault("CONFIGSPECSHAMANELEMENTAL", 100.0f);
    m_float_configs[CONFIGSPECSHAMANENHANCEMENT] = sConfigMgr->GetFloatDefault("CONFIGSPECSHAMANENHANCEMENT", 100.0f);
    m_float_configs[CONFIGSPECSHAMANRESTORATION] = sConfigMgr->GetFloatDefault("CONFIGSPECSHAMANRESTORATION", 100.0f);
    m_float_configs[CONFIGSPECWARLOCKAFFLICTION] = sConfigMgr->GetFloatDefault("CONFIGSPECWARLOCKAFFLICTION", 100.0f);
    m_float_configs[CONFIGSPECWARLOCKDEMONOLOGY] = sConfigMgr->GetFloatDefault("CONFIGSPECWARLOCKDEMONOLOGY", 100.0f);
    m_float_configs[CONFIGSPECWARLOCKDESTRUCTION] = sConfigMgr->GetFloatDefault("CONFIGSPECWARLOCKDESTRUCTION", 100.0f);
    m_float_configs[CONFIGSPECMONKBREWMASTER] = sConfigMgr->GetFloatDefault("CONFIGSPECMONKBREWMASTER", 100.0f);
    m_float_configs[CONFIGSPECMONKWINDWALKER] = sConfigMgr->GetFloatDefault("CONFIGSPECMONKWINDWALKER", 100.0f);
    m_float_configs[CONFIGSPECMONKMISTWEAVER] = sConfigMgr->GetFloatDefault("CONFIGSPECMONKMISTWEAVER", 100.0f);
    // healing custom config balance system
    m_float_configs[HEALCONFIGSPECMAGEARCANE] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMAGEARCANE", 100.0f);
    m_float_configs[HEALCONFIGSPECMAGEFIRE] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMAGEFIRE", 100.0f);
    m_float_configs[HEALCONFIGSPECMAGEFROST] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMAGEFROST", 100.0f);
    m_float_configs[HEALCONFIGSPECPALADINHOLY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPALADINHOLY", 100.0f);
    m_float_configs[HEALCONFIGSPECPALADINPROTECTION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPALADINPROTECTION", 100.0f);
    m_float_configs[HEALCONFIGSPECPALADINRETRIBUTION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPALADINRETRIBUTION", 100.0f);
    m_float_configs[HEALCONFIGSPECWARRIORARMS] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARRIORARMS", 100.0f);
    m_float_configs[HEALCONFIGSPECWARRIORFURY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARRIORFURY", 100.0f);
    m_float_configs[HEALCONFIGSPECWARRIORPROTECTION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARRIORPROTECTION", 100.0f);
    m_float_configs[HEALCONFIGSPECDRUIDBALANCE] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDRUIDBALANCE", 100.0f);
    m_float_configs[HEALCONFIGSPECDRUIDFERAL] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDRUIDFERAL", 100.0f);
    m_float_configs[HEALCONFIGSPECDRUIDGUARDIAN] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDRUIDGUARDIAN", 100.0f);
    m_float_configs[HEALCONFIGSPECDRUIDRESTORATION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDRUIDRESTORATION", 100.0f);
    m_float_configs[HEALCONFIGSPECDEATHKNIGHTBLOOD] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDEATHKNIGHTBLOOD", 100.0f);
    m_float_configs[HEALCONFIGSPECDEATHKNIGHTFROST] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDEATHKNIGHTFROST", 100.0f);
    m_float_configs[HEALCONFIGSPECDEATHKNIGHTUNHOLY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECDEATHKNIGHTUNHOLY", 100.0f);
    m_float_configs[HEALCONFIGSPECHUNTERBEASTMASTERY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECHUNTERBEASTMASTERY", 100.0f);
    m_float_configs[HEALCONFIGSPECHUNTERMARKSMANSHIP] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECHUNTERMARKSMANSHIP", 100.0f);
    m_float_configs[HEALCONFIGSPECHUNTERSURVIVAL] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECHUNTERSURVIVAL", 100.0f);
    m_float_configs[HEALCONFIGSPECPRIESTDISCIPLINE] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPRIESTDISCIPLINE", 100.0f);
    m_float_configs[HEALCONFIGSPECPRIESTHOLY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPRIESTHOLY", 100.0f);
    m_float_configs[HEALCONFIGSPECPRIESTSHADOW] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECPRIESTSHADOW", 100.0f);
    m_float_configs[HEALCONFIGSPECROGUEASSASSINATION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECROGUEASSASSINATION", 100.0f);
    m_float_configs[HEALCONFIGSPECROGUECOMBAT] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECROGUECOMBAT", 100.0f);
    m_float_configs[HEALCONFIGSPECROGUESUBTLETY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECROGUESUBTLETY", 100.0f);
    m_float_configs[HEALCONFIGSPECSHAMANELEMENTAL] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECSHAMANELEMENTAL", 100.0f);
    m_float_configs[HEALCONFIGSPECSHAMANENHANCEMENT] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECSHAMANENHANCEMENT", 100.0f);
    m_float_configs[HEALCONFIGSPECSHAMANRESTORATION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECSHAMANRESTORATION", 100.0f);
    m_float_configs[HEALCONFIGSPECWARLOCKAFFLICTION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARLOCKAFFLICTION", 100.0f);
    m_float_configs[HEALCONFIGSPECWARLOCKDEMONOLOGY] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARLOCKDEMONOLOGY", 100.0f);
    m_float_configs[HEALCONFIGSPECWARLOCKDESTRUCTION] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECWARLOCKDESTRUCTION", 100.0f);
    m_float_configs[HEALCONFIGSPECMONKBREWMASTER] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMONKBREWMASTER", 100.0f);
    m_float_configs[HEALCONFIGSPECMONKWINDWALKER] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMONKWINDWALKER", 100.0f);
    m_float_configs[HEALCONFIGSPECMONKMISTWEAVER] = sConfigMgr->GetFloatDefault("HEALCONFIGSPECMONKMISTWEAVER", 100.0f);

    // PETS CUSTOM BALANCE SYSTEM
    m_float_configs[CONFIG_SPEC_PET_FEROCITY] = sConfigMgr->GetFloatDefault("CONFIG_SPEC_PET_FEROCITY", 100.0f);
    m_float_configs[CONFIG_SPEC_PET_TENACITY] = sConfigMgr->GetFloatDefault("CONFIG_SPEC_PET_TENACITY", 100.0f);
    m_float_configs[CONFIG_SPEC_PET_CUNNING] = sConfigMgr->GetFloatDefault("CONFIG_SPEC_PET_CUNNING", 100.0f);

    // THIS IS USED FOR PETS THAT HAVE NO SPECS, SO MAYBE SOME MAGE PETS OR WARLOCK PETS?
    m_float_configs[CONFIG_PET_NO_SPEC] = sConfigMgr->GetFloatDefault("CONFIG_PET_NO_SPEC", 100.0f);

    ///- Read the "Data" directory from the config file
    std::string dataPath = sConfigMgr->GetStringDefault("DataDir", "./");
    if (dataPath.empty() || (dataPath.at(dataPath.length()-1) != '/' && dataPath.at(dataPath.length()-1) != '\\'))
        dataPath.push_back('/');

#if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
    if (dataPath[0] == '~')
    {
        const char* home = getenv("HOME");
        if (home)
            dataPath.replace(0, 1, home);
    }
#endif

    if (reload)
    {
        if (dataPath != m_dataPath)
            TC_LOG_ERROR("server.loading", "DataDir option can't be changed at worldserver.conf reload, using current value (%s).", m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
        TC_LOG_INFO("server.loading", "Using DataDir %s", m_dataPath.c_str());
    }

    m_bool_configs[CONFIG_ENABLE_MMAPS] = sConfigMgr->GetBoolDefault("mmap.enablePathFinding", false);

    TC_LOG_INFO("server.loading", "WORLD: MMap data directory is: %smmaps", m_dataPath.c_str());

    m_bool_configs[CONFIG_VMAP_INDOOR_CHECK] = sConfigMgr->GetBoolDefault("vmap.enableIndoorCheck", true);
    bool enableIndoor = sConfigMgr->GetBoolDefault("vmap.enableIndoorCheck", true);
    bool enableLOS = sConfigMgr->GetBoolDefault("vmap.enableLOS", true);
    bool enableHeight = sConfigMgr->GetBoolDefault("vmap.enableHeight", true);

    if (!enableHeight)
        TC_LOG_ERROR("server.loading", "VMap height checking disabled! Creatures movements and other various things WILL be broken! Expect no support.");

    VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(enableLOS);
    VMAP::VMapFactory::createOrGetVMapManager()->setEnableHeightCalc(enableHeight);
    TC_LOG_INFO("server.loading", "VMap support included. LineOfSight: %i, getHeight: %i, indoorCheck: %i", enableLOS, enableHeight, enableIndoor);
    TC_LOG_INFO("server.loading", "VMap data directory is: %svmaps", m_dataPath.c_str());

    m_int_configs[CONFIG_MAX_WHO] = 49;
    m_int_configs[CONFIG_MAX_ICORE] = 49;
    m_bool_configs[CONFIG_START_ALL_SPELLS] = sConfigMgr->GetBoolDefault("PlayerStart.AllSpells", false);
    if (m_bool_configs[CONFIG_START_ALL_SPELLS])
        TC_LOG_WARN("server.loading", "PlayerStart.AllSpells enabled - may not function as intended!");
    m_int_configs[CONFIG_HONOR_AFTER_DUEL] = sConfigMgr->GetIntDefault("HonorPointsAfterDuel", 0);
    m_bool_configs[CONFIG_START_ALL_EXPLORED] = sConfigMgr->GetBoolDefault("PlayerStart.MapsExplored", false);
    m_bool_configs[CONFIG_START_ALL_REP] = sConfigMgr->GetBoolDefault("PlayerStart.AllReputation", false);
    m_bool_configs[CONFIG_ALWAYS_MAXSKILL] = sConfigMgr->GetBoolDefault("AlwaysMaxWeaponSkill", false);
    m_bool_configs[CONFIG_PVP_TOKEN_ENABLE] = sConfigMgr->GetBoolDefault("PvPToken.Enable", false);
    m_int_configs[CONFIG_PVP_TOKEN_MAP_TYPE] = sConfigMgr->GetIntDefault("PvPToken.MapAllowType", 4);
    m_int_configs[CONFIG_PVP_TOKEN_ID] = sConfigMgr->GetIntDefault("PvPToken.ItemID", 29434);
    m_int_configs[CONFIG_PVP_TOKEN_COUNT] = sConfigMgr->GetIntDefault("PvPToken.ItemCount", 1);
    if (m_int_configs[CONFIG_PVP_TOKEN_COUNT] < 1)
        m_int_configs[CONFIG_PVP_TOKEN_COUNT] = 1;

    m_bool_configs[CONFIG_NO_RESET_TALENT_COST] = sConfigMgr->GetBoolDefault("NoResetTalentsCost", false);
    m_bool_configs[CONFIG_SHOW_KICK_IN_WORLD] = sConfigMgr->GetBoolDefault("ShowKickInWorld", false);
    m_int_configs[CONFIG_INTERVAL_LOG_UPDATE] = sConfigMgr->GetIntDefault("RecordUpdateTimeDiffInterval", 60000);
    m_int_configs[CONFIG_MIN_LOG_UPDATE] = sConfigMgr->GetIntDefault("MinRecordUpdateTimeDiff", 100);
    m_int_configs[CONFIG_NUMTHREADS] = sConfigMgr->GetIntDefault("MapUpdate.Threads", 1);
    m_int_configs[CONFIG_MAX_RESULTS_LOOKUP_COMMANDS] = sConfigMgr->GetIntDefault("Command.LookupMaxResults", 0);

    // chat logging
    m_bool_configs[CONFIG_CHATLOG_CHANNEL] = sConfigMgr->GetBoolDefault("ChatLogs.Channel", false);
    m_bool_configs[CONFIG_CHATLOG_WHISPER] = sConfigMgr->GetBoolDefault("ChatLogs.Whisper", false);
    m_bool_configs[CONFIG_CHATLOG_SYSCHAN] = sConfigMgr->GetBoolDefault("ChatLogs.SysChan", false);
    m_bool_configs[CONFIG_CHATLOG_PARTY] = sConfigMgr->GetBoolDefault("ChatLogs.Party", false);
    m_bool_configs[CONFIG_CHATLOG_RAID] = sConfigMgr->GetBoolDefault("ChatLogs.Raid", false);
    m_bool_configs[CONFIG_CHATLOG_GUILD] = sConfigMgr->GetBoolDefault("ChatLogs.Guild", false);
    m_bool_configs[CONFIG_CHATLOG_PUBLIC] = sConfigMgr->GetBoolDefault("ChatLogs.Public", false);
    m_bool_configs[CONFIG_CHATLOG_ADDON] = sConfigMgr->GetBoolDefault("ChatLogs.Addon", false);
    m_bool_configs[CONFIG_CHATLOG_BGROUND] = sConfigMgr->GetBoolDefault("ChatLogs.BattleGround", false);

    // Warden
    m_bool_configs[CONFIG_WARDEN_ENABLED]             =  true;
    m_int_configs[CONFIG_WARDEN_NUM_MEM_CHECKS]        = sConfigMgr->GetIntDefault("Warden.NumMemChecks", 3);
    m_int_configs[CONFIG_WARDEN_NUM_OTHER_CHECKS]      = sConfigMgr->GetIntDefault("Warden.NumOtherChecks", 7);
    m_int_configs[CONFIG_WARDEN_CLIENT_BAN_DURATION]   = sConfigMgr->GetIntDefault("Warden.BanDuration", 86400);
    m_int_configs[CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF]  = sConfigMgr->GetIntDefault("Warden.ClientCheckHoldOff", 30);
    m_int_configs[CONFIG_WARDEN_CLIENT_FAIL_ACTION]    = sConfigMgr->GetIntDefault("Warden.ClientCheckFailAction", 0);
    m_int_configs[CONFIG_WARDEN_CLIENT_RESPONSE_DELAY] = sConfigMgr->GetIntDefault("Warden.ClientResponseDelay", 600);

    // Dungeon finder
    m_int_configs[CONFIG_LFG_OPTIONSMASK]               = sConfigMgr->GetIntDefault("DungeonFinder.OptionsMask", 1);
    m_bool_configs[CONFIG_LFG_SOLO]                     = sConfigMgr->GetBoolDefault("LFGSolo.Enabled", false);
    m_bool_configs[CONFIG_LFG_CASTDESERTER]             = sConfigMgr->GetBoolDefault("DungeonFinder.CastDeserter", false);
    m_bool_configs[CONFIG_LFG_OVERRIDE_ROLES_REQUIRED]  = sConfigMgr->GetBoolDefault("DungeonFinder.OverrideRolesRequired", false);
    m_bool_configs[CONFIG_LFG_MULTIQUEUE_ENABLED]       = sConfigMgr->GetBoolDefault("DungeonFinder.MultiqueueEnabled", false);
    m_bool_configs[CONFIG_LFG_KEEP_QUEUES_IN_DUNGEON]   = sConfigMgr->GetBoolDefault("DungeonFinder.KeepQueuesInDungeon", false);
    m_int_configs[CONFIG_LFG_TANKS_NEEDED]              = sConfigMgr->GetIntDefault("DungeonFinder.TanksNeeded", 1);
    m_int_configs[CONFIG_LFG_HEALERS_NEEDED]            = sConfigMgr->GetIntDefault("DungeonFinder.HealersNeeded", 1);
    m_int_configs[CONFIG_LFG_DPS_NEEDED]                = sConfigMgr->GetIntDefault("DungeonFinder.DPSNeeded", 3);
    m_int_configs[CONFIG_LFG_SHORTAGE_CHECK_INTERVAL]   = sConfigMgr->GetIntDefault("DungeonFinder.ShortageCheckInterval", 5);
    m_int_configs[CONFIG_LFG_SHORTAGE_PERCENT]          = sConfigMgr->GetIntDefault("DungeonFinder.ShortagePercent", 50);
    m_int_configs[CONFIG_LFG_MAX_LFR_QUEUES]            = sConfigMgr->GetIntDefault("DungeonFinder.MaxLfrQueues", 3);

    // DBC_ItemAttributes
    m_bool_configs[CONFIG_DBC_ENFORCE_ITEM_ATTRIBUTES] = sConfigMgr->GetBoolDefault("DBC.EnforceItemAttributes", true);

    // Random Battleground Rewards
    m_int_configs[CONFIG_BG_REWARD_WINNER_HONOR_FIRST]    = sConfigMgr->GetIntDefault("Battleground.RewardWinnerHonorFirst",    27000);
    m_int_configs[CONFIG_BG_REWARD_WINNER_HONOR_LAST]     = sConfigMgr->GetIntDefault("Battleground.RewardWinnerHonorLast",     13500);
    m_int_configs[CONFIG_BG_REWARD_WINNER_CONQUEST_FIRST] = sConfigMgr->GetIntDefault("Battleground.RewardWinnerConquestFirst", 15000);
    m_int_configs[CONFIG_BG_REWARD_WINNER_CONQUEST_LAST]  = sConfigMgr->GetIntDefault("Battleground.RewardWinnerConquestLast",  7500);
    m_int_configs[CONFIG_BG_REWARD_LOSER_HONOR_FIRST]     = sConfigMgr->GetIntDefault("Battleground.RewardLoserHonorFirst",     4500);
    m_int_configs[CONFIG_BG_REWARD_LOSER_HONOR_LAST]      = sConfigMgr->GetIntDefault("Battleground.RewardLoserHonorLast",      3500);

    // Max instances per hour
    m_int_configs[CONFIG_MAX_INSTANCES_PER_HOUR] = sConfigMgr->GetIntDefault("AccountInstancesPerHour", 5);
	
   //Server restart in hour
	m_int_configs[CONFIG_AUTO_SERVER_RESTART_HOUR] = sConfigMgr->GetIntDefault("Server.Auto.RestartHour", 4);
	if (m_int_configs[CONFIG_AUTO_SERVER_RESTART_HOUR] > 23)
	{
		m_int_configs[CONFIG_AUTO_SERVER_RESTART_HOUR] = 4;
	}

    // Anounce reset of instance to whole party
    m_bool_configs[CONFIG_INSTANCES_RESET_ANNOUNCE] = sConfigMgr->GetBoolDefault("InstancesResetAnnounce", false);

    // AutoBroadcast
    m_bool_configs[CONFIG_AUTOBROADCAST] = sConfigMgr->GetBoolDefault("AutoBroadcast.On", false);
    m_int_configs[CONFIG_AUTOBROADCAST_CENTER] = sConfigMgr->GetIntDefault("AutoBroadcast.Center", 0);
    m_int_configs[CONFIG_AUTOBROADCAST_INTERVAL] = sConfigMgr->GetIntDefault("AutoBroadcast.Timer", 60000);
    if (reload)
    {
        m_timers[WUPDATE_AUTOBROADCAST].SetInterval(getIntConfig(WorldIntConfigs::CONFIG_AUTOBROADCAST_INTERVAL));
        m_timers[WUPDATE_AUTOBROADCAST].Reset();
    }

    // MySQL ping time interval
    m_int_configs[CONFIG_DB_PING_INTERVAL] = sConfigMgr->GetIntDefault("MaxPingTime", 30);

    // Guild save interval
    m_bool_configs[CONFIG_GUILD_LEVELING_ENABLED] = sConfigMgr->GetBoolDefault("Guild.LevelingEnabled", true);
    m_int_configs[CONFIG_GUILD_SAVE_INTERVAL] = sConfigMgr->GetIntDefault("Guild.SaveInterval", 15);
    m_int_configs[CONFIG_GUILD_MAX_LEVEL] = sConfigMgr->GetIntDefault("Guild.MaxLevel", 25);
    m_int_configs[CONFIG_GUILD_UNDELETABLE_LEVEL] = sConfigMgr->GetIntDefault("Guild.UndeletableLevel", 4);
    rate_values[RATE_XP_GUILD_MODIFIER] = sConfigMgr->GetFloatDefault("Guild.XPModifier", 0.25f);
    m_int_configs[CONFIG_GUILD_DAILY_XP_CAP] = sConfigMgr->GetIntDefault("Guild.DailyXPCap", 7807500);
    m_int_configs[CONFIG_GUILD_WEEKLY_REP_CAP] = sConfigMgr->GetIntDefault("Guild.WeeklyReputationCap", 4375);

    // misc
    m_bool_configs[CONFIG_PDUMP_NO_PATHS] = sConfigMgr->GetBoolDefault("PlayerDump.DisallowPaths", true);
    m_bool_configs[CONFIG_PDUMP_NO_OVERWRITE] = sConfigMgr->GetBoolDefault("PlayerDump.DisallowOverwrite", true);
    m_bool_configs[CONFIG_UI_QUESTLEVELS_IN_DIALOGS] = sConfigMgr->GetBoolDefault("UI.ShowQuestLevelsInDialogs", false);

    // Wintergrasp battlefield
    m_bool_configs[CONFIG_WINTERGRASP_ENABLE] = sConfigMgr->GetBoolDefault("Wintergrasp.Enable", false);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MAX] = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMax", 100);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MIN] = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMin", 0);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MIN_LVL] = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMinLvl", 77);
    m_int_configs[CONFIG_WINTERGRASP_BATTLETIME] = sConfigMgr->GetIntDefault("Wintergrasp.BattleTimer", 30);
    m_int_configs[CONFIG_WINTERGRASP_NOBATTLETIME] = sConfigMgr->GetIntDefault("Wintergrasp.NoBattleTimer", 150);
    m_int_configs[CONFIG_WINTERGRASP_RESTART_AFTER_CRASH] = sConfigMgr->GetIntDefault("Wintergrasp.CrashRestartTimer", 10);

    // Stats limits
    m_bool_configs[CONFIG_STATS_LIMITS_ENABLE] = sConfigMgr->GetBoolDefault("Stats.Limits.Enable", false);
    m_float_configs[CONFIG_STATS_LIMITS_DODGE] = sConfigMgr->GetFloatDefault("Stats.Limits.Dodge", 95.0f);
    m_float_configs[CONFIG_STATS_LIMITS_PARRY] = sConfigMgr->GetFloatDefault("Stats.Limits.Parry", 95.0f);
    m_float_configs[CONFIG_STATS_LIMITS_BLOCK] = sConfigMgr->GetFloatDefault("Stats.Limits.Block", 95.0f);
    m_float_configs[CONFIG_STATS_LIMITS_CRIT] = sConfigMgr->GetFloatDefault("Stats.Limits.Crit", 95.0f);
	
    //Restart 
	m_bool_configs[CONFIG_DISABLE_RESTART] = sConfigMgr->GetBoolDefault("DisableRestart", false);

    // Wow Token
    m_bool_configs[CONFIG_WOW_TOKEN] = sConfigMgr->GetBoolDefault("Wow.Token", false);

    // Bonus played time reward
    m_bool_configs[CONFIG_BONUS_TIME_REWARD] = sConfigMgr->GetBoolDefault("PlayedTimeReward.Enabled", false);
    m_int_configs[CONFIG_PLAYED_TIME_REWARD] = sConfigMgr->GetIntDefault("PlayedTimeReward.Interval", 1200);

    //packet spoof punishment
    m_int_configs[CONFIG_PACKET_SPOOF_POLICY] = sConfigMgr->GetIntDefault("PacketSpoof.Policy", (uint32)WorldSession::DosProtection::POLICY_KICK);
    m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] = sConfigMgr->GetIntDefault("PacketSpoof.BanMode", (uint32)BAN_ACCOUNT);
    if (m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] == BAN_CHARACTER || m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] > BAN_IP)
        m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] = BAN_ACCOUNT;

    m_int_configs[CONFIG_PACKET_SPOOF_BANDURATION] = sConfigMgr->GetIntDefault("PacketSpoof.BanDuration", 86400);

    // AHBot
    m_int_configs[CONFIG_AHBOT_UPDATE_INTERVAL] = sConfigMgr->GetIntDefault("AuctionHouseBot.Update.Interval", 20);

    m_bool_configs[CONFIG_CALCULATE_CREATURE_ZONE_AREA_DATA] = sConfigMgr->GetBoolDefault("Calculate.Creature.Zone.Area.Data", false);
    m_bool_configs[CONFIG_CALCULATE_GAMEOBJECT_ZONE_AREA_DATA] = sConfigMgr->GetBoolDefault("Calculate.Gameoject.Zone.Area.Data", false);

    m_int_configs[CONFIG_BIRTHDAY_TIME] = sConfigMgr->GetIntDefault("BirthdayTime", 1222964635);

    m_bool_configs[CONFIG_GM_LOG_TRADE]                     = sConfigMgr->GetBoolDefault("GM.LogTrade", false);
    m_bool_configs[CONFIG_ALLOW_GM_FRIEND]                  = sConfigMgr->GetBoolDefault("GM.AllowFriend", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_WHO_LIST]          = sConfigMgr->GetBoolDefault("AllowTwoSide.WhoList", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_ACCOUNTS]          = sConfigMgr->GetBoolDefault("AllowTwoSide.Accounts", true);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND]        = sConfigMgr->GetBoolDefault("AllowTwoSide.AddFriend", false);
    m_bool_configs[CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL]      = sConfigMgr->GetBoolDefault("Channel.SilentlyGMJoin", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_MAIL]  = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Mail", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT]  = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.Chat", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_LFG]   = sConfigMgr->GetBoolDefault("AllowTwoSide.Interaction.DungeonFinder", false);


    m_bool_configs[CONFIG_TRANSPORT_DISABLE_MAPOBJ_PRESPAWN] = sConfigMgr->GetBoolDefault("Transport.DisableMapObjPrespawn", false);
    m_bool_configs[CONFIG_TRANSPORT_DISABLE_LOCAL_PRESPAWN] = sConfigMgr->GetBoolDefault("Transport.DisableLocalPrespawn", false);
    m_bool_configs[CONFIG_TRANSPORT_PREFER_SERVER_WORLD_POSITION] = sConfigMgr->GetBoolDefault("Transport.PreferServerWorldPosition", true);
    m_bool_configs[CONFIG_TRANSPORT_LOAD_GRIDS] = sConfigMgr->GetBoolDefault("Transport.LoadGrids", true);

    m_bool_configs[CONFIG_ANTICHEAT_ENABLE] = true;
    m_int_configs[CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION] = 70;
    m_int_configs[CONFIG_ANTICHEAT_DETECTIONS_ENABLED] = 31;
    m_int_configs[CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT] = 70;

    m_float_configs[CONFIG_LFR_LOOT_CHANCE] = 50.0f;
    m_float_configs[CONFIG_BONUS_ROLL_LOOT_CHANCE] = 50.0f;
    m_float_configs[CONFIG_BONUS_ROLL_BONUS_CHANCE] = 50.0f;
    m_int_configs[CONFIG_BONUS_ROLL_MONEY_MIN] = sConfigMgr->GetIntDefault("BonusRoll.MoneyMin", 285000);
    m_int_configs[CONFIG_BONUS_ROLL_MONEY_MAX] = sConfigMgr->GetIntDefault("BonusRoll.MoneyMax", 1285000);

    m_int_configs[CONFIG_SHOW_INFO_BAN_PLAYER] = sConfigMgr->GetIntDefault("ShowInfoPlayer.ban", 0);
    m_int_configs[CONFIG_SHOW_INFO_KICK_PLAYER] = sConfigMgr->GetIntDefault("ShowInfoPlayer.kick", 0);
    m_int_configs[CONFIG_SHOW_INFO_MUTE_PLAYER] = sConfigMgr->GetIntDefault("ShowInfoPlayer.mute", 0);

    m_bool_configs[CONFIG_TRANSFER_MAIL_ENABLED]  = sConfigMgr->GetBoolDefault("TransferMail.Enabled", true);

    m_int_configs[CONFIG_XP_KILL_LEVEL_DIFFERENCE] = sConfigMgr->GetIntDefault("XP.Kill.Difference", 10);
    m_int_configs[CONFIG_XP_GATHER_LEVEL_DIFFERENCE] = sConfigMgr->GetIntDefault("XP.Gather.Difference", 10);


    m_float_configs[CONFIG_PVP_RESILIENCE_90] = sConfigMgr->GetFloatDefault("PvP.Resilience.Value", 44.0f);
    m_float_configs[CONFIG_PVP_RESILIENCE_BELOW_90] = 40.0f;
    m_float_configs[CONFIG_BATTLE_FATIGUE] = 50.0f;

    m_bool_configs[CONFIG_ENABLE_ILVL_SCALING_PVE] = sConfigMgr->GetBoolDefault("Ilvl.Scaling.PvE.Enable", true);
    m_bool_configs[CONFIG_ENABLE_ILVL_SCALING_PVP] = sConfigMgr->GetBoolDefault("Ilvl.Scaling.PvP.Enable", true);
    m_int_configs[CONFIG_ILVL_SCALING_PVE] = sConfigMgr->GetIntDefault("Ilvl.Scaling.PvE", 463);
    m_int_configs[CONFIG_ILVL_SCALING_PVP] = sConfigMgr->GetIntDefault("Ilvl.Scaling.PvP", 463);
    m_bool_configs[CONFIG_PET_BATTLES_ENABLED] = sConfigMgr->GetIntDefault("PetBattles.Endabled", true);

    m_int_configs[CONFIG_ICORE_ARENA_HIGH_LATENCY_THRESHOLD] = sConfigMgr->GetIntDefault("ICore.Arena.HighLatencyThreshold", 200);
    m_arenaPrecastSpells.clear();
    for (auto&& id : Tokenizer{ sConfigMgr->GetStringDefault("ICore.Arena.PrecastSpells", ""), ' ' })
        m_arenaPrecastSpells.insert(atoi(id));

    m_int_configs[CONFIG_ISLE_OF_THUNDER_STAGE] = sConfigMgr->GetIntDefault("IsleofThunder.Stage", 1);
    m_int_configs[CONFIG_ISLE_OF_THUNDER_PROGRESSION_HORDE] = sConfigMgr->GetIntDefault("IsleofThunder.Progression.Horde", 0);
    m_int_configs[CONFIG_ISLE_OF_THUNDER_PROGRESSION_ALLIANCE] = sConfigMgr->GetIntDefault("IsleofThunder.Progression.Alliance", 0);

    m_bool_configs[CONFIG_DELETING_ITEM_LOG] = sConfigMgr->GetBoolDefault("DeletingItem.Log.Enabled", true);
    m_int_configs[CONFIG_DELETING_ITEM_KEEP_DAYS] = sConfigMgr->GetIntDefault("DeletingItem.KeepDays", 30);
    m_int_configs[CONFIG_DELETING_ITEM_MIN_ITEM_LEVEL_EQUIP] = sConfigMgr->GetIntDefault("DeletingItem.MinEquipItemLevel", 410);
    m_int_configs[CONFIG_DELETING_ITEM_MIN_ITEM_LEVEL_OTHER] = sConfigMgr->GetIntDefault("DeletingItem.MinOtherItemLevel", 85);
    m_int_configs[CONFIG_DELETING_ITEM_MIN_QUALITY] = sConfigMgr->GetIntDefault("DeletingItem.MinQuality", ITEM_QUALITY_RARE);
    m_int_configs[CONFIG_DELETING_ITEM_MAX_QUALITY] = sConfigMgr->GetIntDefault("DeletingItem.MaxQuality", ITEM_QUALITY_LEGENDARY);

    m_int_configs[CONFIG_CREATURE_PICKPOCKET_REFILL] = sConfigMgr->GetIntDefault("Creature.PickPocketRefillDelay", 10 * MINUTE);
    m_int_configs[CONFIG_CREATURE_STOP_FOR_PLAYER] = sConfigMgr->GetIntDefault("Creature.MovingStopTimeForPlayer", 3 * MINUTE * IN_MILLISECONDS);

    m_bool_configs[CONFIG_AUCTIONHOUSE_ALLOW_SORTING]        = sConfigMgr->GetBoolDefault("AuctionHouse.AllowSorting", true);
    m_bool_configs[CONFIG_AUCTIONHOUSE_FORCE_MAIN_THREAD]    = sConfigMgr->GetBoolDefault("AuctionHouse.ForceMainThread", false);
    m_int_configs[CONFIG_AUCTIONHOUSE_MIN_DIFF_FOR_LOG]      = sConfigMgr->GetIntDefault("AuctionHouse.MinDiffForLog", 25);
    m_int_configs[CONFIG_AUCTIONHOUSE_MIN_DIFF_FOR_THROTTLE] = sConfigMgr->GetIntDefault("AuctionHouse.MinDiffForThrottle", 25);

    m_bool_configs[CONFIG_BATTLEGROUND_REWARDS_ENABLED] = sConfigMgr->GetBoolDefault("Battleground.Rewards.Enabled", true);
    m_int_configs[CONFIG_BATTLEGROUND_REWARD] = sConfigMgr->GetIntDefault("Battleground.Rewards.Item", 32546);
    m_int_configs[CONFIG_BATTLEGROUND_GAMES] = sConfigMgr->GetIntDefault("Battleground.Rewards.Games", 15);

    m_bool_configs[CONFIG_ICORE_RICH_PVP_REWARD_ENABLED] = sConfigMgr->GetBoolDefault("ICore.RichPvPRewards.Enabled", true);
    m_int_configs[CONFIG_ICORE_RICH_PVP_REWARD] = sConfigMgr->GetIntDefault("ICore.RichPvPRewards.Item", 32547);
    m_int_configs[CONFIG_ICORE_RICH_PVP_REWARD_CHANCE] = sConfigMgr->GetIntDefault("ICore.Rewards.RichPvPRewards.Chance", 30);
    m_int_configs[CONFIG_ICORE_RICH_PVP_REWARD_GAMES] = sConfigMgr->GetIntDefault("ICore.Rewards.RichPvPRewards.Games", 30);

    m_bool_configs[CONFIG_RBG_REWARDS_FOR_ARENA_ENABLED] = sConfigMgr->GetBoolDefault("Arena.RbgRewards.Enabled", true);

    m_bool_configs[CONFIG_ARENA_WIN_STREAK_MOD_ENABLED] = sConfigMgr->GetBoolDefault("Arena.WinStreakMod.Enabled", true);
    m_int_configs[CONFIG_ARENA_WIN_STREAK_MOD_LIMIT] = sConfigMgr->GetIntDefault("Arena.WinStreakMod.Limit", 96);

    m_bool_configs[CONFIG_ICORE_ROYALE_EVENT_ENABLED] = sConfigMgr->GetBoolDefault("ICore.RoyaleEvent.Enabled", true);

    m_float_configs[CONFIG_VENGEANCE_MULTIPLIER] = sConfigMgr->GetFloatDefault("VengeanceMultipier", 1.0f);

    m_bool_configs[CONFIG_BOOST_PROMOTION] = sConfigMgr->GetBoolDefault("BoostPromotion.Enabled", false);
#ifdef ELUNA
	//eluna
	m_bool_configs[CONFIG_BOOL_ELUNA_ENABLED] = sConfigMgr->GetBoolDefault("Eluna.Enabled", true);
#endif    
    // Loading of Locales
    m_bool_configs[CONFIG_LOAD_LOCALES] = sConfigMgr->GetBoolDefault("Load.Locales", true);
    // call ScriptMgr if we're reloading the configuration
    if (reload)
    {
        sScriptMgr->OnConfigLoad(reload);
        sRatedPvpMgr->OnConfigLoad();
    }
}

void World::LoadRates(bool loading)
{
    struct RateConfig
    {
        Rates       Rate;
        char const* ConfigName;
        float       DefaultValue;
        bool        PositiveOnly;
    };

    static const RateConfig configs[] =
    {
        { RATE_HEALTH,                                  "Rate.Health",                                  1.0f,   true  },
        { RATE_POWER_MANA,                              "Rate.Mana",                                    1.0f,   true  },
        { RATE_POWER_RAGE_INCOME,                       "Rate.Rage.Income",                             1.0f,   false },
        { RATE_POWER_RAGE_LOSS,                         "Rate.Rage.Loss",                               1.0f,   true  },
        { RATE_POWER_RUNICPOWER_INCOME,                 "Rate.RunicPower.Income",                       1.0f,   false },
        { RATE_POWER_RUNICPOWER_LOSS,                   "Rate.RunicPower.Loss",                         1.0f,   true  },
        { RATE_POWER_FOCUS,                             "Rate.Focus",                                   1.0f,   false },
        { RATE_POWER_ENERGY,                            "Rate.Energy",                                  1.0f,   false },
        { RATE_POWER_CHI,                               "Rate.Chi",                                     1.0f,   false },
        { RATE_SKILL_DISCOVERY,                         "Rate.Skill.Discovery",                         1.0f,   false },
        { RATE_DROP_ITEM_POOR,                          "Rate.Drop.Item.Poor",                          1.0f,   false },
        { RATE_DROP_ITEM_NORMAL,                        "Rate.Drop.Item.Normal",                        1.0f,   false },
        { RATE_DROP_ITEM_UNCOMMON,                      "Rate.Drop.Item.Uncommon",                      1.0f,   false },
        { RATE_DROP_ITEM_RARE,                          "Rate.Drop.Item.Rare",                          1.0f,   false },
        { RATE_DROP_ITEM_EPIC,                          "Rate.Drop.Item.Epic",                          1.0f,   false },
        { RATE_DROP_ITEM_LEGENDARY,                     "Rate.Drop.Item.Legendary",                     1.0f,   false },
        { RATE_DROP_ITEM_ARTIFACT,                      "Rate.Drop.Item.Artifact",                      1.0f,   false },
        { RATE_DROP_ITEM_REFERENCED,                    "Rate.Drop.Item.Referenced",                    1.0f,   false },
        { RATE_DROP_ITEM_REFERENCED_AMOUNT,             "Rate.Drop.Item.ReferencedAmount",              1.0f,   false },
        { RATE_DROP_MONEY,                              "Rate.Drop.Money",                              1.0f,   false },
        { RATE_XP_KILL,                                 "Rate.XP.Kill",                                 1.0f,   false },
        { RATE_XP_KILL_PREMIUM,                         "Rate.XP.Kill.Premium",                         1.0f,   false },
        { RATE_XP_QUEST,                                "Rate.XP.Quest",                                1.0f,   false },
        { RATE_XP_QUEST_PREMIUM,                        "Rate.XP.Quest.Premium",                        1.0f,   false },
        { RATE_XP_EXPLORE,                              "Rate.XP.Explore",                              1.0f,   false },
        { RATE_XP_EXPLORE_PREMIUM,                      "Rate.XP.Explore.Premium",                      1.0f,   false },
        { RATE_XP_GATHER,                               "Rate.XP.Gather",                               1.0f,   false },
        { RATE_REPAIRCOST,                              "Rate.RepairCost",                              1.0f,   true  },
        { RATE_REPUTATION_GAIN,                         "Rate.Reputation.Gain",                         1.0f,   false },
        { RATE_REPUTATION_GAIN_PREMIUM,                 "Rate.Reputation.Premium",                      1.0f,   false },
        { RATE_REPUTATION_LOWLEVEL_KILL,                "Rate.Reputation.LowLevel.Kill",                1.0f,   false },
        { RATE_REPUTATION_LOWLEVEL_QUEST,               "Rate.Reputation.LowLevel.Quest",               1.0f,   false },
        { RATE_REPUTATION_RECRUIT_A_FRIEND_BONUS,       "Rate.Reputation.RecruitAFriendBonus",          0.1f,   false },
        { RATE_CREATURE_NORMAL_DAMAGE,                  "Rate.Creature.Normal.Damage",                  1.0f,   false },
        { RATE_CREATURE_ELITE_ELITE_DAMAGE,             "Rate.Creature.Elite.Elite.Damage",             1.0f,   false },
        { RATE_CREATURE_ELITE_RAREELITE_DAMAGE,         "Rate.Creature.Elite.RAREELITE.Damage",         1.0f,   false },
        { RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,         "Rate.Creature.Elite.WORLDBOSS.Damage",         1.0f,   false },
        { RATE_CREATURE_ELITE_RARE_DAMAGE,              "Rate.Creature.Elite.RARE.Damage",              1.0f,   false },
        { RATE_CREATURE_NORMAL_HP,                      "Rate.Creature.Normal.HP",                      1.0f,   false },
        { RATE_CREATURE_ELITE_ELITE_HP,                 "Rate.Creature.Elite.Elite.HP",                 1.0f,   false },
        { RATE_CREATURE_ELITE_RAREELITE_HP,             "Rate.Creature.Elite.RAREELITE.HP",             1.0f,   false },
        { RATE_CREATURE_ELITE_WORLDBOSS_HP,             "Rate.Creature.Elite.WORLDBOSS.HP",             1.0f,   false },
        { RATE_CREATURE_ELITE_RARE_HP,                  "Rate.Creature.Elite.RARE.HP",                  1.0f,   false },
        { RATE_CREATURE_NORMAL_SPELLDAMAGE,             "Rate.Creature.Normal.SpellDamage",             1.0f,   false },
        { RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,        "Rate.Creature.Elite.Elite.SpellDamage",        1.0f,   false },
        { RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE,    "Rate.Creature.Elite.RAREELITE.SpellDamage",    1.0f,   false },
        { RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE,    "Rate.Creature.Elite.WORLDBOSS.SpellDamage",    1.0f,   false },
        { RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,         "Rate.Creature.Elite.RARE.SpellDamage",         1.0f,   false },
        { RATE_CREATURE_AGGRO,                          "Rate.Creature.Aggro",                          1.0f,   false },
        { RATE_REST_INGAME,                             "Rate.Rest.InGame",                             1.0f,   false },
        { RATE_MAX_MODW,                                "Rate.Modw.InGame",                             1.0f,   false },
        { RATE_REST_OFFLINE_IN_TAVERN_OR_CITY,          "Rate.Rest.Offline.InTavernOrCity",             1.0f,   false },
        { RATE_REST_OFFLINE_IN_WILDERNESS,              "Rate.Rest.Offline.InWilderness",               1.0f,   false },
        { RATE_DAMAGE_FALL,                             "Rate.Damage.Fall",                             1.0f,   false },
        { RATE_AUCTION_TIME,                            "Rate.Auction.Time",                            1.0f,   false },
        { RATE_AUCTION_DEPOSIT,                         "Rate.Auction.Deposit",                         1.0f,   false },
        { RATE_AUCTION_CUT,                             "Rate.Auction.Cut",                             1.0f,   false },
        { RATE_HONOR,                                   "Rate.Honor",                                   1.0f,   false },
        { RATE_HONOR_PREMIUM,                           "Rate.Honor.Premium",                           1.0f,   false },
        { RATE_INSTANCE_RESET_TIME,                     "Rate.InstanceResetTime",                       1.0f,   false },
        { RATE_MOVESPEED,                               "Rate.MoveSpeed",                               1.0f,   true  },
        { RATE_CORPSE_DECAY_LOOTED,                     "Rate.Corpse.Decay.Looted",                     0.5f,   false },
        { RATE_TARGET_POS_RECALCULATION_RANGE,          "TargetPosRecalculateRange",                    1.5f,   true  },
        { RATE_DURABILITY_LOSS_ON_DEATH,                "DurabilityLoss.OnDeath",                       10.0f,  true  },
        { RATE_DURABILITY_LOSS_DAMAGE,                  "DurabilityLossChance.Damage",                  0.5f,   true  },
        { RATE_DURABILITY_LOSS_ABSORB,                  "DurabilityLossChance.Absorb",                  0.5f,   true  },
        { RATE_DURABILITY_LOSS_PARRY,                   "DurabilityLossChance.Parry",                   0.05f,  true  },
        { RATE_DURABILITY_LOSS_BLOCK,                   "DurabilityLossChance.Block",                   0.05f,  true  },
    };

    if (loading)
    {
        // Loading new and reloading existing entries
        std::set<uint32> loadedIDs;
        if (getBoolConfig(CONFIG_BONUS_RATES_ENABLED))
        {
            QueryResult result = LoginDatabase.PQuery("SELECT id, schedule, multiplier, rates, start_announcement, end_announcement, active_announcement FROM bonus_rates WHERE (realmid=-1 OR realmid='%u') AND active", sConfigMgr->GetIntDefault("RealmID", 0));
            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    uint32 id = fields->GetUInt32();
                    m_bonusRates[id].Load(fields);
                    loadedIDs.insert(id);
                }
                while (result->NextRow());
            }
        }

        // Deleting no longer existing entries
        for (auto itr = m_bonusRates.begin(); itr != m_bonusRates.end();)
        {
            if (loadedIDs.find(itr->first) == loadedIDs.end())
            {
                if (itr->second.IsActive())
                    itr->second.SetActive(false); // Announce deactivation

                m_bonusRates.erase(itr++);
            }
            else
                ++itr;
        }

        UpdateBonusRatesState();
    }

    for (RateConfig const& config : configs)
    {
        rate_values[config.Rate] = sConfigMgr->GetFloatDefault(config.ConfigName, config.DefaultValue);
        if (config.PositiveOnly && rate_values[config.Rate] <= 0.0f)
        {
            TC_LOG_ERROR("server.loading", "%s (%f) must be >= 0. Using %f instead.", config.ConfigName, rate_values[config.Rate], config.DefaultValue);
            rate_values[config.Rate] = config.DefaultValue;
        }

        for (auto&& bonusRate : m_bonusRates)
            if (bonusRate.second.IsActive() && bonusRate.second.IsAffectingRate(config.ConfigName))
                rate_values[config.Rate] *= bonusRate.second.GetMultiplier();

        switch (config.Rate)
        {
            case RATE_MOVESPEED:
                for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                    playerBaseMoveSpeed[i] = baseMoveSpeed[i] * rate_values[RATE_MOVESPEED];
                break;
            case RATE_TARGET_POS_RECALCULATION_RANGE:
                if (rate_values[config.Rate] < CONTACT_DISTANCE)
                {
                    TC_LOG_ERROR("server.loading", "TargetPosRecalculateRange (%f) must be >= %f. Using %f instead.", rate_values[config.Rate], CONTACT_DISTANCE, CONTACT_DISTANCE);
                    rate_values[config.Rate] = CONTACT_DISTANCE;
                }
                else if (rate_values[config.Rate] > NOMINAL_MELEE_RANGE)
                {
                    TC_LOG_ERROR("server.loading", "TargetPosRecalculateRange (%f) must be <= %f. Using %f instead.", rate_values[config.Rate], NOMINAL_MELEE_RANGE, NOMINAL_MELEE_RANGE);
                    rate_values[config.Rate] = NOMINAL_MELEE_RANGE;
                }
                break;
            case RATE_DURABILITY_LOSS_ON_DEATH:
                if (rate_values[config.Rate] > 100.0f)
                {
                    TC_LOG_ERROR("server.loading", "DurabilityLoss.OnDeath (%f) must be <= 100. Using 100.0 instead.", rate_values[config.Rate]);
                    rate_values[config.Rate] = 0.0f;
                }
                rate_values[config.Rate] = rate_values[config.Rate] / 100.0f;
                break;
            default:
                break;
        }
    }
}

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Server startup begin
    uint32 startupBegin = getMSTime();

    ///- Initialize the random number generator
    srand((unsigned int)time(NULL));

    ///- Initialize detour memory management
    dtAllocSetCustom(dtCustomAlloc, dtCustomFree);

    ///- Initialize VMapManager function pointers (to untangle game/collision circular deps)
    VMAP::VMapManager2* vmmgr2 = VMAP::VMapFactory::createOrGetVMapManager();
    vmmgr2->GetLiquidFlagsPtr = &GetLiquidFlags;
    vmmgr2->IsVMAPDisabledForPtr = &DisableMgr::IsVMAPDisabledFor;

    ///- Initialize config settings
    LoadConfigSettings();

    ///- Initialize Allowed Security Level
    LoadDBAllowedSecurityLevel();

    ///- Init highest guids before any table loading to prevent using not initialized guids in some code.
    sObjectMgr->SetHighestGuids();

    ///- Check the existence of the map files for all races' startup areas.
    if (!MapManager::ExistMapAndVMap(0, -6240.32f, 331.033f)
        || !MapManager::ExistMapAndVMap(0, -8949.95f, -132.493f)
        || !MapManager::ExistMapAndVMap(1, -618.518f, -4251.67f)
        || !MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f)
        || !MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f)
        || !MapManager::ExistMapAndVMap(1, -2917.58f, -257.98f)
        || (m_int_configs[CONFIG_EXPANSION] && (
           !MapManager::ExistMapAndVMap(530, 10349.6f, -6357.29f)
        || !MapManager::ExistMapAndVMap(530, -3961.64f, -13931.2f)
        || !MapManager::ExistMapAndVMap(648, -8423.81f, 1361.3f)
        || !MapManager::ExistMapAndVMap(654, -1451.53f, 1403.35f)
        || !MapManager::ExistMapAndVMap(609, 2356.21f, -5662.21f)
        || !MapManager::ExistMapAndVMap(860, 1471.67f, 3466.25f))))
    {
        TC_LOG_ERROR("server.loading", "Correct *.map files not found in path '%smaps' or *.vmtree/*.vmtile files in '%svmaps'. Please place *.map/*.vmtree/*.vmtile files in appropriate directories or correct the DataDir value in the worldserver.conf file.", m_dataPath.c_str(), m_dataPath.c_str());
        exit(1);
    }

    ///- Initialize pool manager
    sPoolMgr->Initialize();

    ///- Initialize game event manager
    sGameEventMgr->Initialize();

    ///- Loading strings. Getting no records means core load has to be canceled because no error message can be output.

    TC_LOG_INFO("server.loading", "Loading Trinity strings...");
    if (!sObjectMgr->LoadTrinityStrings())
        exit(1);                                            // Error message displayed in function already

    ///- Update the realm entry in the database with the realm type from the config file
    //No SQL injection as values are treated as integers

    // not send custom type REALM_FFA_PVP to realm list
    uint32 server_type = IsFFAPvPRealm() ? uint32(REALM_TYPE_PVP) : getIntConfig(CONFIG_GAME_TYPE);
    uint32 realm_zone = getIntConfig(CONFIG_REALM_ZONE);

    LoginDatabase.PExecute("UPDATE realmlist SET icon = %u, timezone = %u WHERE id = '%d'", server_type, realm_zone, realm.Id.Realm);      // One-time query

    ///- Load the DBC files
    TC_LOG_INFO("server.loading", "Initialize data stores...");
    LoadDBCStores(m_dataPath, m_availableDbcLocaleMask);
    LoadDB2Stores(m_dataPath, m_availableDbcLocaleMask);

    // Load cinematic cameras
    LoadM2Cameras(m_dataPath);

    // Load IP Location Database
    sIPLocation->Load();

    std::vector<uint32> mapIds;
    for (uint32 mapId = 0; mapId < sMapStore.GetNumRows(); mapId++)
        if (sMapStore.LookupEntry(mapId))
            mapIds.push_back(mapId);

    vmmgr2->InitializeThreadUnsafe(mapIds);

    MMAP::MMapManager* mmmgr = MMAP::MMapFactory::createOrGetMMapManager();
    mmmgr->InitializeThreadUnsafe(mapIds);    

    TC_LOG_INFO("server.loading", "Loading SpellInfo store...");
    sSpellMgr->LoadSpellInfoStore();

    TC_LOG_INFO("server.loading", "Loading SpellInfo corrections...");
    sSpellMgr->LoadSpellInfoCorrections();

    TC_LOG_INFO("server.loading", "Loading SkillLineAbilityMultiMap Data...");
    sSpellMgr->LoadSkillLineAbilityMap();

    TC_LOG_INFO("server.loading", "Loading SpellInfo custom attributes...");
    sSpellMgr->LoadSpellInfoCustomAttributes();

    TC_LOG_INFO("server.loading", "Loading GameObject models...");
    LoadGameObjectModelList(m_dataPath);

    TC_LOG_INFO("server.loading", "Loading Script Names...");
    sObjectMgr->LoadScriptNames();

    TC_LOG_INFO("server.loading", "Loading Instance Template...");
    sObjectMgr->LoadInstanceTemplate();

    // Must be called before `creature_respawn`/`gameobject_respawn` tables
    TC_LOG_INFO("server.loading", "Loading instances...");
    sInstanceSaveMgr->LoadInstances();

    TC_LOG_INFO("server.loading", "Loading Broadcast texts...");
    sObjectMgr->LoadBroadcastTexts();

    if (m_bool_configs[CONFIG_LOAD_LOCALES])
        sObjectMgr->LoadBroadcastTextLocales();

    TC_LOG_INFO("server.loading", "Loading Localization strings...");
    uint32 oldMSTime = getMSTime();
    if (m_bool_configs[CONFIG_LOAD_LOCALES])
    {
        sObjectMgr->LoadCreatureLocales();
        sObjectMgr->LoadGameObjectLocales();
        sObjectMgr->LoadItemLocales();
        sObjectMgr->LoadQuestTemplateLocale();
        sObjectMgr->LoadQuestObjectivesLocale();
        sObjectMgr->LoadQuestOfferRewardLocale();
        sObjectMgr->LoadQuestRequestItemsLocale();
        sObjectMgr->LoadNpcTextLocales();
        sObjectMgr->LoadPageTextLocales();
        sObjectMgr->LoadGossipMenuItemsLocales();
        sObjectMgr->LoadPointOfInterestLocales();
    }

    sObjectMgr->SetDBCLocaleIndex(GetDefaultDbcLocale());        // Get once for all the locale index of DBC language (console/broadcasts)
    TC_LOG_INFO("server.loading", ">> Localization strings loaded in %u ms", GetMSTimeDiffToNow(oldMSTime));

    TC_LOG_INFO("server.loading", "Loading Letter Analogs...");
    sWordFilterMgr->LoadLetterAnalogs();

    TC_LOG_INFO("server.loading", "Loading Bad Words...");
    sWordFilterMgr->LoadBadWords();

    TC_LOG_INFO("server.loading", "Loading Page Texts...");
    sObjectMgr->LoadPageTexts();

    TC_LOG_INFO("server.loading", "Loading Game Object Templates...");         // must be after LoadPageTexts
    sObjectMgr->LoadGameObjectTemplate();

    TC_LOG_INFO("server.loading", "Loading Game Object template addons...");
    sObjectMgr->LoadGameObjectTemplateAddons();

    TC_LOG_INFO("server.loading", "Loading Transport templates...");
    sTransportMgr->LoadTransportTemplates();

    TC_LOG_INFO("server.loading", "Loading Spell Rank Data...");
    sSpellMgr->LoadSpellRanks();

    TC_LOG_INFO("server.loading", "Loading Spell Required Data...");
    sSpellMgr->LoadSpellRequired();

    TC_LOG_INFO("server.loading", "Loading Spell Group types...");
    sSpellMgr->LoadSpellGroups();

    TC_LOG_INFO("server.loading", "Loading Spell Learn Skills...");
    sSpellMgr->LoadSpellLearnSkills();                           // must be after LoadSpellRanks

    TC_LOG_INFO("server.loading", "Loading Spell Learn Spells...");
    sSpellMgr->LoadSpellLearnSpells();

    TC_LOG_INFO("server.loading", "Loading Spell Proc Event conditions...");
    sSpellMgr->LoadSpellProcEvents();

    TC_LOG_INFO("server.loading", "Loading Spell Proc conditions and data...");
    sSpellMgr->LoadSpellProcs();

    TC_LOG_INFO("server.loading", "Loading Spell Bonus Data...");
    sSpellMgr->LoadSpellBonusess();

    TC_LOG_INFO("server.loading", "Loading Aggro Spells Definitions...");
    sSpellMgr->LoadSpellThreats();

    TC_LOG_INFO("server.loading", "Loading Spell Group Stack Rules...");
    sSpellMgr->LoadSpellGroupStackRules();

    TC_LOG_INFO("server.loading", "Loading Spell Phase Dbc Info...");
    sObjectMgr->LoadSpellPhaseInfo();

    TC_LOG_INFO("server.loading", "Loading Spell AreaTrigger templates...");
    sObjectMgr->LoadSpellAreaTriggerTemplates();

    TC_LOG_INFO("server.loading", "Loading NPC Texts...");
    sObjectMgr->LoadGossipText();

    TC_LOG_INFO("server.loading", "Loading Enchant Spells Proc datas...");
    sSpellMgr->LoadSpellEnchantProcData();

    TC_LOG_INFO("server.loading", "Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();

    TC_LOG_INFO("server.loading", "Loading Disables");                         // must be before loading quests and items
    DisableMgr::LoadDisables();

    TC_LOG_INFO("server.loading", "Loading Items...");                         // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr->LoadItemTemplates();

    TC_LOG_INFO("server.loading", "Loading Item set names...");                // must be after LoadItemPrototypes
    sObjectMgr->LoadItemTemplateAddon();

    TC_LOG_INFO("misc", "Loading Item Scripts...");                 // must be after LoadItemPrototypes
    sObjectMgr->LoadItemScriptNames();

    TC_LOG_INFO("server.loading", "Loading Creature Model Based Info Data...");
    sObjectMgr->LoadCreatureModelInfo();

    TC_LOG_INFO("server.loading", "Loading Creature templates...");
    sObjectMgr->LoadCreatureTemplates();

    TC_LOG_INFO("server.loading", "Loading Equipment templates...");           // must be after LoadCreatureTemplates
    sObjectMgr->LoadEquipmentTemplates();

    TC_LOG_INFO("server.loading", "Loading creature difficulty moidifiers...");
    sObjectMgr->LoadCreatureDifficultyModifiers();

    TC_LOG_INFO("server.loading", "Loading Creature template addons...");
    sObjectMgr->LoadCreatureTemplateAddons();

    TC_LOG_INFO("server.loading", "Loading Pet scaling...");
    sObjectMgr->LoadPetScaling();

    TC_LOG_INFO("server.loading", "Loading Reputation Reward Rates...");
    sObjectMgr->LoadReputationRewardRate();

    TC_LOG_INFO("server.loading", "Loading Creature Reputation OnKill Data...");
    sObjectMgr->LoadReputationOnKill();

    TC_LOG_INFO("server.loading", "Loading Reputation Spillover Data...");
    sObjectMgr->LoadReputationSpilloverTemplate();

    TC_LOG_INFO("server.loading", "Loading Points Of Interest Data...");
    sObjectMgr->LoadPointsOfInterest();

    TC_LOG_INFO("server.loading", "Loading Creature Base Stats...");
    sObjectMgr->LoadCreatureClassLevelStats();

    TC_LOG_INFO("server.loading", "Loading Creature Data...");
    sObjectMgr->LoadCreatures();

    TC_LOG_INFO("server.loading", "Loading Temporary Summon Data...");
    sObjectMgr->LoadTempSummons();                               // must be after LoadCreatureTemplates() and LoadGameObjectTemplates()

    TC_LOG_INFO("server.loading", "Loading pet levelup spells...");
    sSpellMgr->LoadPetSpellMap();

    TC_LOG_INFO("server.loading", "Loading Creature Addon Data...");
    sObjectMgr->LoadCreatureAddons();                            // must be after LoadCreatureTemplates() and LoadCreatures()

    TC_LOG_INFO("server.loading", "Loading Creature Movement Overrides...");
    sObjectMgr->LoadCreatureMovementOverrides();                 // must be after LoadCreatures()

    TC_LOG_INFO("server.loading", "Loading Gameobject Data...");
    sObjectMgr->LoadGameobjects();

    TC_LOG_INFO("server.loading", "Loading GameObject Addon Data...");
    sObjectMgr->LoadGameObjectAddons();                          // must be after LoadGameObjectTemplate() and LoadGameobjects()

    TC_LOG_INFO("server.loading", "Loading Creature Sparring Data...");
    sObjectMgr->LoadCreatureSparringTemplate();

    TC_LOG_INFO("server.loading", "Loading Creature Linked Respawn...");
    sObjectMgr->LoadLinkedRespawn();                             // must be after LoadCreatures(), LoadGameObjects()

    TC_LOG_INFO("server.loading", "Loading Custom Object Visibility...");
    sObjectMgr->LoadCustomVisibility();

    TC_LOG_INFO("server.loading", "Loading Weather Data...");
    WeatherMgr::LoadWeatherData();

    TC_LOG_INFO("server.loading", "Loading Quests...");
    sObjectMgr->LoadQuests();                                    // must be loaded after DBCs, creature_template, item_template, gameobject tables

    TC_LOG_INFO("server.loading", "Checking Quest Disables");
    DisableMgr::CheckQuestDisables();                           // must be after loading quests

    if (m_bool_configs[CONFIG_LOAD_LOCALES])
    {
        TC_LOG_INFO("server.loading", "Loading Quest Objective Locales...");
        sObjectMgr->LoadQuestObjectivesLocale();
    }

    TC_LOG_INFO("server.loading", "Loading Quest Objective Visual Effects...");
    sObjectMgr->LoadQuestObjectiveVisualEffects();

    TC_LOG_INFO("server.loading", "Loading Quest POI");
    sObjectMgr->LoadQuestPOI();

    TC_LOG_INFO("server.loading", "Loading Quests Starters and Enders...");
    sObjectMgr->LoadQuestStartersAndEnders();                    // must be after quest load

    TC_LOG_INFO("server.loading", "Loading Objects Pooling Data...");
    sPoolMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Quest Pooling Data...");
    sQuestPoolMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Game Event Data...");               // must be after loading pools fully
    sGameEventMgr->LoadHolidayDates();                           // Must be after loading DBC
    sGameEventMgr->LoadFromDB();                                 // Must be after loading holiday dates

    TC_LOG_INFO("server.loading", "Loading UNIT_NPC_FLAG_SPELLCLICK Data..."); // must be after LoadQuests
    sObjectMgr->LoadNPCSpellClickSpells();

    TC_LOG_INFO("server.loading", "Loading Vehicle Template Accessories...");
    sObjectMgr->LoadVehicleTemplateAccessories();                // must be after LoadCreatureTemplates() and LoadNPCSpellClickSpells()

    TC_LOG_INFO("server.loading", "Loading Vehicle Accessories...");
    sObjectMgr->LoadVehicleAccessories();                       // must be after LoadCreatureTemplates() and LoadNPCSpellClickSpells()

    TC_LOG_INFO("server.loading", "Loading SpellArea Data...");                // must be after quest load
    sSpellMgr->LoadSpellAreas();

    TC_LOG_INFO("server.loading", "Loading AreaTrigger definitions...");
    sObjectMgr->LoadAreaTriggerTeleports();

    TC_LOG_INFO("server.loading", "Loading Access Requirements...");
    sObjectMgr->LoadAccessRequirements();                        // must be after item template load

    TC_LOG_INFO("server.loading", "Loading Quest Giver Area Triggers...");
    sObjectMgr->LoadQuestGiverAreaTriggers();

    TC_LOG_INFO("server.loading", "Loading Quest Area Triggers...");
    sObjectMgr->LoadQuestAreaTriggers();                         // must be after LoadQuests

    TC_LOG_INFO("server.loading", "Loading Tavern Area Triggers...");
    sObjectMgr->LoadTavernAreaTriggers();

    TC_LOG_INFO("server.loading", "Loading AreaTrigger script names...");
    sObjectMgr->LoadAreaTriggerScripts();

    TC_LOG_INFO("server.loading", "Loading LFG entrance positions..."); // Must be after areatriggers
    sLFGMgr->LoadLFGDungeons();

    TC_LOG_INFO("server.loading", "Loading Dungeon boss data...");
    sObjectMgr->LoadInstanceEncounters();

    TC_LOG_INFO("server.loading", "Loading LFG rewards...");
    sLFGMgr->LoadRewards();

    TC_LOG_INFO("server.loading", "Loading Graveyard-zone links...");
    sObjectMgr->LoadGraveyardZones();

    TC_LOG_INFO("server.loading", "Loading Graveyard Orientations...");
    sObjectMgr->LoadGraveyardOrientations();

    TC_LOG_INFO("server.loading", "Loading spell pet auras...");
    sSpellMgr->LoadSpellPetAuras();

    TC_LOG_INFO("server.loading", "Loading Spell target coordinates...");
    sSpellMgr->LoadSpellTargetPositions();

    TC_LOG_INFO("server.loading", "Loading enchant custom attributes...");
    sSpellMgr->LoadEnchantCustomAttr();

    TC_LOG_INFO("server.loading", "Loading linked spells...");
    sSpellMgr->LoadSpellLinked();

    TC_LOG_INFO("server.loading", "Loading Player Create Data...");
    sObjectMgr->LoadPlayerInfo();

    TC_LOG_INFO("server.loading", "Loading Exploration BaseXP Data...");
    sObjectMgr->LoadExplorationBaseXP();

    TC_LOG_INFO("server.loading", "Loading Pet Name Parts...");
    sObjectMgr->LoadPetNames();

    CharacterDatabaseCleaner::CleanDatabase();

    TC_LOG_INFO("server.loading", "Loading the max pet number...");
    sObjectMgr->LoadPetNumber();

    TC_LOG_INFO("server.loading", "Loading pet level stats...");
    sObjectMgr->LoadPetLevelInfo();

    TC_LOG_INFO("server.loading", "Loading Player level dependent mail rewards...");
    sObjectMgr->LoadMailLevelRewards();

    TC_LOG_INFO("server.loading", "Loading Scenes Templates...");
    sObjectMgr->LoadSceneTemplates();

    // Loot tables
    sLootMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();

    TC_LOG_INFO("server.loading", "Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();

    TC_LOG_INFO("server.loading", "Loading Skill Fishing base level requirements...");
    sObjectMgr->LoadFishingBaseSkillLevel();

    TC_LOG_INFO("server.loading", "Loading Achievements...");
    sAchievementMgr->LoadAchievementReferenceList();
    TC_LOG_INFO("server.loading", "Loading Achievement Criteria Lists...");
    sAchievementMgr->LoadAchievementCriteriaList();
    TC_LOG_INFO("server.loading", "Loading Achievement Criteria Data...");
    sAchievementMgr->LoadAchievementCriteriaData();
    TC_LOG_INFO("server.loading", "Loading Achievement Rewards...");
    sAchievementMgr->LoadRewards();
    if (m_bool_configs[CONFIG_LOAD_LOCALES])
    {
        TC_LOG_INFO("server.loading", "Loading Achievement Reward Locales...");
        sAchievementMgr->LoadRewardLocales();
    }
    TC_LOG_INFO("server.loading", "Loading Completed Achievements...");
    sAchievementMgr->LoadCompletedAchievements();

    ///- Load dynamic data tables from the database
    TC_LOG_INFO("server.loading", "Loading Item Auctions...");
    sAuctionMgr->LoadAuctionItems();

    TC_LOG_INFO("server.loading", "Loading Auctions...");
    sAuctionMgr->LoadAuctions();

    TC_LOG_INFO("server.loading", "Loading Guild XP for level...");
    sGuildMgr->LoadGuildXpForLevel();

    TC_LOG_INFO("server.loading", "Loading Guild rewards...");
    sGuildMgr->LoadGuildRewards();

    TC_LOG_INFO("server.loading", "Loading Guilds...");
    sGuildMgr->LoadGuilds();

    TC_LOG_INFO("server.loading", "Loading Groups...");
    sGroupMgr->LoadGroups();

    TC_LOG_INFO("server.loading", "Loading ReservedNames...");
    sObjectMgr->LoadReservedPlayersNames();

    TC_LOG_INFO("server.loading", "Loading GameObjects for quests...");
    sObjectMgr->LoadGameObjectForQuests();

    TC_LOG_INFO("server.loading", "Loading BattleMasters...");
    sBattlegroundMgr->LoadBattleMastersEntry();

    TC_LOG_INFO("server.loading", "Loading GameTeleports...");
    sObjectMgr->LoadGameTele();

    TC_LOG_INFO("server.loading", "Loading Gossip menu...");
    sObjectMgr->LoadGossipMenu();

    TC_LOG_INFO("server.loading", "Loading Gossip menu options...");
    sObjectMgr->LoadGossipMenuItems();

    TC_LOG_INFO("server.loading", "Loading Vendors...");
    sObjectMgr->LoadVendors();                                   // must be after load CreatureTemplate and ItemTemplate

    TC_LOG_INFO("server.loading", "Loading Trainers...");
    sObjectMgr->LoadTrainerSpell();                              // must be after load CreatureTemplate

    TC_LOG_INFO("server.loading", "Loading Waypoints...");
    sWaypointMgr->Load();

    TC_LOG_INFO("server.loading", "Loading SmartAI Waypoints...");
    sSmartWaypointMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Creature Formations...");
    sFormationMgr->LoadCreatureFormations();

    TC_LOG_INFO("server.loading", "Loading World States...");              // must be loaded before battleground, outdoor PvP and conditions
    LoadWorldStates();

    TC_LOG_INFO("server.loading", "Loading Phase definitions...");
    sObjectMgr->LoadPhaseDefinitions();

    TC_LOG_INFO("server.loading", "Loading Terrain Phase definitions...");
    sObjectMgr->LoadTerrainPhaseInfo();

    TC_LOG_INFO("server.loading", "Loading Terrain Swap Default definitions...");
    sObjectMgr->LoadTerrainSwapDefaults();

    TC_LOG_INFO("server.loading", "Loading Terrain World Map definitions...");
    sObjectMgr->LoadTerrainWorldMaps();

    TC_LOG_INFO("server.loading", "Loading Phase Area definitions...");
    sObjectMgr->LoadAreaPhases();

    TC_LOG_INFO("server.loading", "Loading Conditions...");
    sConditionMgr->LoadConditions();

    TC_LOG_INFO("server.loading", "Loading faction change achievement pairs...");
    sObjectMgr->LoadFactionChangeAchievements();

    TC_LOG_INFO("server.loading", "Loading faction change spell pairs...");
    sObjectMgr->LoadFactionChangeSpells();

    TC_LOG_INFO("server.loading", "Loading faction change item pairs...");
    sObjectMgr->LoadFactionChangeItems();

    TC_LOG_INFO("server.loading", "Loading faction change reputation pairs...");
    sObjectMgr->LoadFactionChangeReputations();

    TC_LOG_INFO("server.loading", "Loading faction change title pairs...");
    sObjectMgr->LoadFactionChangeTitles();

    TC_LOG_INFO("server.loading", "Loading GM tickets...");
    sTicketMgr->LoadGmTickets();

    TC_LOG_INFO("server.loading", "Loading GM bugs...");
    sTicketMgr->LoadBugTickets();

    TC_LOG_INFO("server.loading", "Loading client addons...");
    AddonMgr::LoadFromDB();

    ///- Handle outdated emails (delete/return)
    TC_LOG_INFO("server.loading", "Returning old mails...");
    sObjectMgr->ReturnOrDeleteOldMails(false);

    TC_LOG_INFO("server.loading", "Loading Autobroadcasts...");
    LoadAutobroadcasts();

    ///- Load and initialize scripts
    sObjectMgr->LoadSpellScripts();                              // must be after load Creature/Gameobject(Template/Data)
    sObjectMgr->LoadEventScripts();                              // must be after load Creature/Gameobject(Template/Data)
    sObjectMgr->LoadWaypointScripts();

    TC_LOG_INFO("server.loading", "Loading spell script names...");
    sObjectMgr->LoadSpellScriptNames();

    TC_LOG_INFO("server.loading", "Loading Creature Texts...");
    sCreatureTextMgr->LoadCreatureTexts();

    if (m_bool_configs[CONFIG_LOAD_LOCALES])
    {
        TC_LOG_INFO("server.loading", "Loading Creature Text Locales...");
        sCreatureTextMgr->LoadCreatureTextLocales();
    }

    TC_LOG_INFO("server.loading", "Initializing Scripts...");
    sScriptMgr->Initialize();
    sScriptMgr->OnConfigLoad(false);                                // must be done after the ScriptMgr has been properly initialized

    TC_LOG_INFO("server.loading", "Validating spell scripts...");
    sObjectMgr->ValidateSpellScripts();

    TC_LOG_INFO("server.loading", "Loading SmartAI scripts...");
    sSmartScriptMgr->LoadSmartAIFromDB();

    TC_LOG_INFO("server.loading", "Loading Calendar data...");
    sCalendarMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Research Digsite info...");
    sObjectMgr->LoadResearchDigsiteInfo();

    TC_LOG_INFO("server.loading", "Loading Archaeology Find info...");
    sObjectMgr->LoadArchaeologyFindInfo();

    TC_LOG_INFO("server.loading", "Loading Research Project requirements...");
    sObjectMgr->LoadResearchProjectRequirements();

    TC_LOG_INFO("server.loading", "Loading Battle Pet breed data...");
    sObjectMgr->LoadBattlePetBreedData();

    TC_LOG_INFO("server.loading", "Loading Battle Pet quality data...");
    sObjectMgr->LoadBattlePetQualityData();

    TC_LOG_INFO("server.loading", "Loading Battle Pet item to species data...");
    sObjectMgr->LoadBattlePetItemToSpeciesData();

    TC_LOG_INFO("server.loading", "Loading Battle Pet spawn pool data...");
    sBattlePetSpawnMgr->Initialise();

    TC_LOG_INFO("server.loading", "Loading Battle Pay store data...");
    sBattlePayMgr->LoadFromDb();

    TC_LOG_INFO("server.loading", "Loading Battle Pay boost items data...");
    LoadBoostItems();

    TC_LOG_INFO("server.loading", "Loading Broken Quests data...");
    sObjectMgr->LoadBrokenQuests();

    TC_LOG_INFO("server.loading", "Loading object visibility state data...");
    sObjectMgr->LoadObjectVisibilityState();

    ///- Initialize game time and timers
    TC_LOG_INFO("server.loading", "Initialize game time and timers");
    m_gameTime = time(NULL);
    m_startTime = m_gameTime;

    LoginDatabase.PExecute("INSERT INTO uptime (realmid, starttime, uptime, revision) VALUES(%u, %u, 0, '%s')",
                            realm.Id.Realm, uint32(m_startTime), GitRevision::GetFullVersion());       // One-time query

    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE*IN_MILLISECONDS);
    m_timers[WUPDATE_AUCTIONS_PENDING].SetInterval(250);
    m_timers[WUPDATE_BLACK_MARKET].SetInterval(MINUTE*IN_MILLISECONDS);
    m_timers[WUPDATE_UPTIME].SetInterval(m_int_configs[CONFIG_UPTIME_UPDATE]*MINUTE*IN_MILLISECONDS);
                                                            //Update "uptime" table based on configuration entry in minutes.
    m_timers[WUPDATE_CORPSES].SetInterval(20 * MINUTE * IN_MILLISECONDS);
                                                            //erase corpses every 20 minutes
    m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]*MINUTE*IN_MILLISECONDS);
                                                            // clean logs table every 14 days by default
    m_timers[WUPDATE_AUTOBROADCAST].SetInterval(getIntConfig(CONFIG_AUTOBROADCAST_INTERVAL));

    m_timers[WUPDATE_DELETECHARS].SetInterval(DAY*IN_MILLISECONDS); // check for chars to delete every day

    // for AhBot
    m_timers[WUPDATE_AHBOT].SetInterval(getIntConfig(CONFIG_AHBOT_UPDATE_INTERVAL) * IN_MILLISECONDS); // every 20 sec

    m_timers[WUPDATE_PINGDB].SetInterval(getIntConfig(CONFIG_DB_PING_INTERVAL)*MINUTE*IN_MILLISECONDS);    // Mysql ping time in minutes

    m_timers[WUPDATE_GUILDSAVE].SetInterval(getIntConfig(CONFIG_GUILD_SAVE_INTERVAL) * MINUTE * IN_MILLISECONDS);

    m_timers[WUPDATE_DIFFSTAT].SetInterval(MINUTE * IN_MILLISECONDS);

    m_timers[WUPDATE_BONUS_RATES].SetInterval(30 * IN_MILLISECONDS);

    //to set mailtimer to return mails every day between 4 and 5 am
    //mailtimer is increased when updating auctions
    //one second is 1000 -(tested on win system)
    /// @todo Get rid of magic numbers
    tm localTm;
    localtime_r(&m_gameTime, &localTm);
    mail_timer = ((((localTm.tm_hour + 20) % 24)* HOUR * IN_MILLISECONDS) / m_timers[WUPDATE_AUCTIONS].GetInterval());
                                                            //1440
    mail_timer_expires = ((DAY * IN_MILLISECONDS) / (m_timers[WUPDATE_AUCTIONS].GetInterval()));
    TC_LOG_INFO("server.loading", "Mail timer set to: " UI64FMTD ", mail return is called every " UI64FMTD " minutes", uint64(mail_timer), uint64(mail_timer_expires));

    ///- Initilize static helper structures
    AIRegistry::Initialize();

    ///- Initialize MapManager
    TC_LOG_INFO("server.loading", "Starting Map System");
    sMapMgr->Initialize();

    TC_LOG_INFO("server.loading", "Starting Game Event system...");
    uint32 nextGameEvent = sGameEventMgr->StartSystem();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);    //depend on next event

    // Delete all characters which have been deleted X days before
    Player::DeleteOldCharacters(true);

    TC_LOG_INFO("server.loading", "Initialize AuctionHouseBot...");
    sAuctionBot->Initialize();

    // Delete all custom channels which haven't been used for PreserveCustomChannelDuration days.
    Channel::CleanOldChannelsInDB();

    TC_LOG_INFO("server.loading", "Starting Arena Season...");
    sGameEventMgr->StartArenaSeason();

    sTicketMgr->Initialize();

    ///- Initialize Battlegrounds
    TC_LOG_INFO("server.loading", "Starting Battleground System");
    sBattlegroundMgr->CreateInitialBattlegrounds();

    ///- Initialize outdoor pvp
    TC_LOG_INFO("server.loading", "Starting Outdoor PvP System");
    sOutdoorPvPMgr->InitOutdoorPvP();

    ///- Initialize Battlefield
    TC_LOG_INFO("server.loading", "Starting Battlefield System");
    sBattlefieldMgr->InitBattlefield();

    TC_LOG_INFO("server.loading", "Loading Transports...");
    sTransportMgr->SpawnContinentTransports();

    ///- Initialize Warden
    TC_LOG_INFO("server.loading", "Loading Warden Checks...");
    sWardenCheckMgr->LoadWardenChecks();

    TC_LOG_INFO("server.loading", "Deleting expired bans...");
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");      // One-time query

    TC_LOG_INFO("server.loading", "Calculate next daily quest reset time...");
    InitDailyQuestResetTime();

    TC_LOG_INFO("server.loading", "Calculate next weekly quest reset time...");
    InitWeeklyQuestResetTime();

    TC_LOG_INFO("server.loading", "Calculate next monthly quest reset time...");
    InitMonthlyQuestResetTime();

    TC_LOG_INFO("server.loading", "Calculate random battleground reset time...");
    InitRandomBGResetTime();

    TC_LOG_INFO("server.loading", "Calculate guild limitation(s) reset time...");
    InitGuildResetTime();

    TC_LOG_INFO("server.loading", "Calculate next currency reset time...");
    InitCurrencyResetTime();

    LoadCharacterNameData();
    LoadAccountCacheData();

    // After name data
    sGuildFinderMgr->LoadFromDB();

    TC_LOG_INFO("server.loading", "Loading Rated PVP cache...");    // After world states and name data
    sRatedPvpMgr->LoadFromDB();

    TC_LOG_INFO("misc", "Initializing Opcodes...");
    opcodeTable.Initialize();

    TC_LOG_INFO("misc", "Loading hotfix info...");
    sObjectMgr->LoadHotfixData();

    TC_LOG_INFO("server.loading", "Loading black market templates...");
    sBlackMarketMgr->LoadTemplates();

    TC_LOG_INFO("server.loading", "Loading black market auctions...");
    sBlackMarketMgr->LoadAuctions();

    TC_LOG_INFO("server.loading", "Loading missing KeyChains...");
    sObjectMgr->LoadMissingKeyChains();

    TC_LOG_INFO("server.loading", "Loading realm completed challenges...");
    sObjectMgr->LoadRealmCompletedChallenges();

    TC_LOG_INFO("server.loading", "Loading challenge mode rewards...");
    sObjectMgr->LoadChallengeRewards();

    TC_LOG_INFO("server.loading", "Loading creature scalling data...");
    sObjectMgr->LoadCreatureScaling();

    TC_LOG_INFO("server.loading", "Loading promotion auras data...");
    sObjectMgr->LoadPromotionAuras();

    sObjectMgr->LoadGuildChallengeRewardInfo();

    sServiceMgr->LoadFromDB();

    uint32 startupDuration = GetMSTimeDiffToNow(startupBegin);

#ifdef ELUNA
	if (sWorld->getBoolConfig(CONFIG_BOOL_ELUNA_ENABLED))
	{
		///- Initialize Lua Engine
		TC_LOG_INFO("server.loading", ">> Initialize Eluna Lua Engine...");
		StartEluna(false);
	}

#endif

    TC_LOG_INFO("server.worldserver", "World initialized in %u minutes %u seconds", (startupDuration / 60000), ((startupDuration % 60000) / 1000));

    if (uint32 realmId = sConfigMgr->GetIntDefault("RealmID", 0)) // 0 reserved for auth
        sLog->SetRealmId(realmId);
}

void World::RecordTimeDiff(const char *text, ...)
{
    if (!text)
    {
        m_currentTime = getMSTime();
        return;
    }

    uint32 thisTime = getMSTime();
    uint32 diff = getMSTimeDiff(m_currentTime, thisTime);

    if (diff > m_int_configs[CONFIG_MIN_LOG_UPDATE])
    {
        va_list ap;
        char str[8192];
        va_start(ap, text);
        vsnprintf(str, sizeof(str), text, ap);
        va_end(ap);
        TC_LOG_INFO("diff", "Difftime %s: %u.", str, diff);
    }

    m_currentTime = thisTime;
}

void World::RecordTimeDiff(uint32 diff, char const* msg, ...)
{
    char str[8192];
    va_list ap;
    va_start(ap, msg);
    vsnprintf(str, sizeof(str), msg, ap);
    va_end(ap);
    TC_LOG_INFO("diff", "Difftime %s: %u.", str, diff);
}

void World::RecordTimeDiffLocal(TimeValue const& start, char const* msg, ...)
{
    uint32 diff = (TimeValue::Now() - start).ToMilliseconds();
    if (diff > sWorld->getIntConfig(CONFIG_MIN_LOG_UPDATE))
    {
        char str[8192];
        va_list ap;
        va_start(ap, msg);
        vsnprintf(str, sizeof(str), msg, ap);
        va_end(ap);
        TC_LOG_INFO("diff", "Difftime %s: %u.", str, diff);
    }
}

void World::LoadAutobroadcasts()
{
    uint32 oldMSTime = getMSTime();

    m_autobroadcasts.clear();
    m_autobroadcastsWeights.clear();

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
    stmt->setInt32(0, realm.Id.Realm);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 autobroadcasts definitions. DB table `autobroadcast` is empty for this realm!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint8 id = fields[0].GetUInt8();

        m_autobroadcasts[id] = fields[2].GetString();
        m_autobroadcastsWeights[id] = fields[1].GetUInt8();

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u autobroadcast definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

/// Update the World !
void World::Update(uint32 diff)
{
    m_updateTime = diff;

    ///- Update the different timers
    for (int i = 0; i < WUPDATE_COUNT; ++i)
    {
        if (m_timers[i].GetCurrent() >= 0)
            m_timers[i].Update(diff);
        else
            m_timers[i].SetCurrent(0);
    }

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    /// Handle daily quests reset time
    if (m_gameTime > m_NextDailyQuestReset)
    {
        ResetDailyQuests();
        m_NextDailyQuestReset += DAY;
    }

    /// Handle weekly quests reset time
    if (m_gameTime > m_NextWeeklyQuestReset)
        ResetWeeklyQuests();

    /// Handle monthly quests reset time
    if (m_gameTime > m_NextMonthlyQuestReset)
        ResetMonthlyQuests();

    if (m_gameTime > m_NextRandomBGReset)
        ResetRandomBG();

    if (m_gameTime > m_NextGuildReset)
        ResetGuildCap();

    if (m_gameTime > m_nextCurrencyReset)
    {
        ResetCurrencyWeekCap();
        ResetLootLockouts();
        sGuildMgr->ResetGuildChallenges();
    }

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_BLACK_MARKET].Passed())
    {
        m_timers[WUPDATE_BLACK_MARKET].Reset();

        ///- Update mails (return old mails with item, or delete them)
        //(tested... works on win)
        if (++mail_timer > mail_timer_expires)
        {
            mail_timer = 0;
            sObjectMgr->ReturnOrDeleteOldMails(true);
        }

        ///- Handle expired Blackmarket auctions
        sBlackMarketMgr->Update();
    }

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_AUCTIONS].Passed())
    {
        m_timers[WUPDATE_AUCTIONS].Reset();

        ///- Update mails (return old mails with item, or delete them)
        //(tested... works on win)
        if (++mail_timer > mail_timer_expires)
        {
            mail_timer = 0;
            sObjectMgr->ReturnOrDeleteOldMails(true);
        }

        ///- Handle expired auctions
        RecordTimeDiff(nullptr);
        sAuctionMgr->Update();
        RecordTimeDiff("AuctionMgr::Update");
    }

    if (m_timers[WUPDATE_AUCTIONS_PENDING].Passed())
    {
        m_timers[WUPDATE_AUCTIONS_PENDING].Reset();

        sAuctionMgr->UpdatePendingAuctions();
    }

    /// <li> Handle AHBot operations
    if (m_timers[WUPDATE_AHBOT].Passed())
    {
        sAuctionBot->Update();
        m_timers[WUPDATE_AHBOT].Reset();
    }

    /// <li> Handle session updates when the timer has passed

    RecordTimeDiff(nullptr);
    UpdateSessions(diff);
    RecordTimeDiff("UpdateSessions");

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff = uint32(m_gameTime - m_startTime);
        uint32 maxOnlinePlayers = GetMaxPlayerCount();

        m_timers[WUPDATE_UPTIME].Reset();

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_UPTIME_PLAYERS);

        stmt->setUInt32(0, tmpDiff);
        stmt->setUInt16(1, uint16(maxOnlinePlayers));
        stmt->setUInt32(2, realm.Id.Realm);
        stmt->setUInt32(3, uint32(m_startTime));

        LoginDatabase.Execute(stmt);
    }

    /// <li> Clean logs table
    if (sWorld->getIntConfig(CONFIG_LOGDB_CLEARTIME) > 0) // if not enabled, ignore the timer
    {
        if (m_timers[WUPDATE_CLEANDB].Passed())
        {
            m_timers[WUPDATE_CLEANDB].Reset();

            DBCleanup();
        }
    }

    /// <li> Handle all other objects
    ///- Update objects when the timer has passed (maps, transport, creatures, ...)
    sMapMgr->Update(diff);

    if (sWorld->getBoolConfig(CONFIG_AUTOBROADCAST))
    {
        if (m_timers[WUPDATE_AUTOBROADCAST].Passed())
        {
            m_timers[WUPDATE_AUTOBROADCAST].Reset();
            SendAutoBroadcast();
        }
    }

    RecordTimeDiff(nullptr);
    sBattlegroundMgr->Update(diff);
    RecordTimeDiff("UpdateBattlegroundMgr");

    sOutdoorPvPMgr->Update(diff);
    RecordTimeDiff("UpdateOutdoorPvPMgr");

    sBattlefieldMgr->Update(diff);
    RecordTimeDiff("BattlefieldMgr");

    ///- Delete all characters which have been deleted X days before
    if (m_timers[WUPDATE_DELETECHARS].Passed())
    {
        m_timers[WUPDATE_DELETECHARS].Reset();
        Player::DeleteOldCharacters();
        RecordTimeDiff("Player::DeleteOldCharacters");
    }

    sLFGMgr->Update(diff);
    RecordTimeDiff("UpdateLFGMgr");

    // execute callbacks from sql queries that were queued recently
    ProcessQueryCallbacks();
    RecordTimeDiff("ProcessQueryCallbacks");

    ///- Erase corpses once every 20 minutes
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();
        sMapMgr->DoForAllMaps([](Map* map)
        {
            map->RemoveOldCorpses();
        });
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
        RecordTimeDiff(nullptr);
        uint32 nextGameEvent = sGameEventMgr->Update();
        RecordTimeDiff("GameEventMgr::Update");
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
    }

    ///- Ping to keep MySQL connections alive
    if (m_timers[WUPDATE_PINGDB].Passed())
    {
        m_timers[WUPDATE_PINGDB].Reset();
        TC_LOG_DEBUG("misc", "Ping MySQL to keep connection alive");
        CharacterDatabase.KeepAlive();
        LoginDatabase.KeepAlive();
        WorldDatabase.KeepAlive();
    }

    if (m_timers[WUPDATE_GUILDSAVE].Passed())
    {
        m_timers[WUPDATE_GUILDSAVE].Reset();
        RecordTimeDiff(nullptr);
        sGuildMgr->SaveGuilds();
        RecordTimeDiff("GuildMgr::SaveGuilds");
    }

    if (m_minDiff > diff && diff > 0)
        m_minDiff = diff;
    if (m_maxDiff < diff)
        m_maxDiff = diff;

    ++m_statDiffCounter;

    if (m_timers[WUPDATE_DIFFSTAT].Passed())
    {
        // minute elapsed - write to DB
        LoginDatabasePreparedStatement *stmt = LoginDatabase.GetPreparedStatement(LOGIN_DIFF_STAT);
        int32 index = -1;
        stmt->setUInt8(++index, realm.Id.Realm);
        stmt->setUInt16(++index, m_timers[WUPDATE_DIFFSTAT].GetCurrent() / m_statDiffCounter);
        stmt->setUInt16(++index, m_minDiff);
        stmt->setUInt16(++index, m_maxDiff);
        stmt->setInt32(++index, time(nullptr));
        LoginDatabase.Execute(stmt);
        m_statDiffCounter = 0;
        m_minDiff = ~0;
        m_maxDiff = 0;
        m_timers[WUPDATE_DIFFSTAT].Reset();
    }

    if (m_timers[WUPDATE_BONUS_RATES].Passed())
    {
        UpdateBonusRatesState();
        m_timers[WUPDATE_BONUS_RATES].Reset();
    }

    RecordTimeDiff(nullptr);
    // update the instance reset times
    sInstanceSaveMgr->Update();
    RecordTimeDiff("InstanceSaveMgr::Update");
    sBattlePetSpawnMgr->Update(diff);
    RecordTimeDiff("BattlePetSpawnMgr::Update");
    sPetBattleSystem->Update(diff);
    RecordTimeDiff("PetBattleSystem::Update");

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();
    RecordTimeDiff("ProcessCliCommands");

    sScriptMgr->OnWorldUpdate(diff);
}

void World::ForceGameEventUpdate()
{
    m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
    uint32 nextGameEvent = sGameEventMgr->Update();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
    m_timers[WUPDATE_EVENTS].Reset();
}

/// Send a packet to all players (except self if mentioned)
void World::SendGlobalMessage(WorldPacket const* packet, WorldSession* self, uint32 team)
{
    SendGlobalMessage(packet, SEC_PLAYER, self, team);
}

void World::SendGlobalMessage(WorldPacket const* packet, AccountTypes security, WorldSession* self, uint32 team)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second != self &&
            itr->second->GetSecurity() >= security &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

/// Send a packet to all GMs (except self if mentioned)
void World::SendGlobalGMMessage(WorldPacket* packet, WorldSession* self, uint32 team)
{
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        // check if session and can receive global GM Messages and its not self
        WorldSession* session = itr->second;
        Player* player = session->GetPlayer();

        if (session == self || session->GetSecurity() < SEC_GAMEMASTER)
            continue;

        // Player should be in world
        if (!player || !player->IsInWorld())
            continue;

        // Send only to same team, if team is given
        if (!team || player->GetTeam() == team)
            session->SendPacket(packet);
    }
}

namespace Trinity
{
    class WorldWorldTextBuilder
    {
        public:
            typedef std::vector<WorldPacket*> WorldPacketList;
            explicit WorldWorldTextBuilder(int32 textId, va_list* args = NULL) : i_textId(textId), i_args(args) { }
            void operator()(WorldPacketList& data_list, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);

                if (i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap, *i_args);

                    char str[2048];
                    vsnprintf(str, 2048, text, ap);
                    va_end(ap);

                    do_helper(data_list, &str[0]);
                }
                else
                    do_helper(data_list, (char*)text);
            }
        private:
            char* lineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = NULL; return start; }
            void do_helper(WorldPacketList& data_list, char* text)
            {
                char* pos = text;

                while (char* line = lineFromMessage(pos))
                {
                    WorldPacket* data = new WorldPacket();

                    uint32 lineLength = strlen(line) + 1;

                    ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, NULL, line);
                    data_list.push_back(data);
                }
            }

            int32 i_textId;
            va_list* i_args;
    };
}                                                           // namespace Trinity

/// Send a System Message to all players (except self if mentioned)
void World::SendWorldText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    bool pvpPlayersOnly = getBoolConfig(CONFIG_ARENA_QUEUE_ANNOUNCER_PLAYERONLY);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        if (pvpPlayersOnly)
        {
            if (string_id == LANG_ARENA_QUEUE_ANNOUNCE_WORLD_JOIN && !itr->second->GetPlayer()->GetArenaPersonalRating(PVP_SLOT_ARENA_2v2))
                continue;
            if (string_id == LANG_ARENA_QUEUE_ANNOUNCE_WORLD_SOLO && !itr->second->GetPlayer()->GetArenaPersonalRating(PVP_SLOT_ARENA_5v5))
                continue;
        }

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

/// Send a System Message to all GMs (except self if mentioned)
void World::SendGMText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        // Session should have permissions to receive global gm messages
        WorldSession* session = itr->second;
        if (session->GetSecurity() < SEC_GAMEMASTER)
            continue;

        // Player should be in world
        Player* player = session->GetPlayer();
        if (!player || !player->IsInWorld())
            continue;

        wt_do(player);
    }

    va_end(ap);
}

/// DEPRECATED, only for debug purpose. Send a System Message to all players (except self if mentioned)
void World::SendGlobalText(const char* text, WorldSession* self)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, NULL, line);
        SendGlobalMessage(&data, self);
    }

    free(buf);
}

/// Send a packet to all players (or players selected team) in the zone (except self if mentioned)
bool World::SendZoneMessage(uint32 zone, WorldPacket const* packet, WorldSession* self, uint32 team)
{
    bool foundPlayerToSend = false;
    SessionMap::const_iterator itr;

    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second->GetPlayer()->GetZoneId() == zone &&
            itr->second != self &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
            foundPlayerToSend = true;
        }
    }

    return foundPlayerToSend;
}

/// Send a System Message to all players in the zone (except self if mentioned)
void World::SendZoneText(uint32 zone, const char* text, WorldSession* self, uint32 team)
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, NULL, text);
    SendZoneMessage(zone, &data, self, team);
}

/// Kick (and save) all players
void World::KickAll()
{
    m_QueuedPlayer.clear();                                 // prevent send queue update packet and login queued sessions

    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        itr->second->KickPlayer();
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllLess(AccountTypes sec)
{
    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetSecurity() < sec)
            itr->second->KickPlayer();
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string const& nameOrIP, std::string const& duration, std::string const& reason, std::string const& author)
{
    uint32 duration_secs = TimeStringToSecs(duration);
    return BanAccount(mode, nameOrIP, duration_secs, reason, author);
}

/// Ban an account or ban an IP address, duration is in seconds if positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string const& nameOrIP, uint32 duration_secs, std::string const& reason, std::string const& author)
{
    PreparedQueryResult resultAccounts = PreparedQueryResult(NULL); //used for kicking
    LoginDatabasePreparedStatement* stmt = nullptr;
    CharacterDatabasePreparedStatement* cstmt = nullptr;
    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_IP);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_IP_BANNED);
            stmt->setString(0, nameOrIP);
            stmt->setUInt32(1, duration_secs);
            stmt->setString(2, author);
            stmt->setString(3, reason);
            LoginDatabase.Execute(stmt);
            break;
        case BAN_ACCOUNT:
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            break;
        case BAN_SOLO:
            if (duration_secs > WEEK)
                return BAN_SYNTAX_ERROR;
        case BAN_CHARACTER:
            // No SQL injection with prepared statements
            cstmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_NAME);
            cstmt->setString(0, nameOrIP);
            resultAccounts = CharacterDatabase.Query(cstmt);
            break;
        default:
            return BAN_SYNTAX_ERROR;
    }

    if (!resultAccounts)
    {
        if (mode == BAN_IP)
            return BAN_SUCCESS;                             // ip correctly banned but nobody affected (yet)
        else
            return BAN_NOTFOUND;                            // Nobody to ban
    }

    ///- Disconnect all affected players (for IP it can be several)
    LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();
    do
    {
        Field* fieldsAccount = resultAccounts->Fetch();
        uint32 account = fieldsAccount[0].GetUInt32();

        if (mode != BAN_IP)
        {
            // make sure there is only one active ban
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
            stmt->setUInt32(0, account);
            trans->Append(stmt);
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_BANNED);
            stmt->setUInt32(0, account);
            stmt->setUInt32(1, realm.Id.Realm);
            stmt->setUInt32(2, duration_secs);
            stmt->setString(3, author);
            stmt->setString(4, reason);
            trans->Append(stmt);
        }

        if (WorldSession* sess = FindSession(account))
            if (std::string(sess->GetPlayerName()) != author)
                sess->KickPlayer();
    } while (resultAccounts->NextRow());

    LoginDatabase.CommitTransaction(trans);

    return BAN_SUCCESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, std::string const& nameOrIP)
{
    LoginDatabasePreparedStatement* stmt = NULL;
    if (mode == BAN_IP)
    {
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_IP_NOT_BANNED);
        stmt->setString(0, nameOrIP);
        LoginDatabase.Execute(stmt);
    }
    else
    {
        uint32 account = 0;
        if (mode == BAN_ACCOUNT)
            account = AccountMgr::GetId(nameOrIP);
        else if (mode == BAN_CHARACTER)
            account = sObjectMgr->GetPlayerAccountIdByPlayerName(nameOrIP);

        if (!account)
            return false;

        //NO SQL injection as account is uint32
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
        stmt->setUInt32(0, account);
        LoginDatabase.Execute(stmt);
    }
    return true;
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanCharacter(std::string const& name, std::string const& duration, std::string const& reason, std::string const& author)
{
    Player* pBanned = ObjectAccessor::FindPlayerByName(name);
    ObjectGuid guid;

    uint32 duration_secs = TimeStringToSecs(duration);

    /// Pick a player to ban if not online
    if (!pBanned)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);
        stmt->setString(0, name);
        PreparedQueryResult resultCharacter = CharacterDatabase.Query(stmt);

        if (!resultCharacter)
            return BAN_NOTFOUND;                                    // Nobody to ban

        guid = ObjectGuid(HighGuid::Player, (*resultCharacter)[0].GetUInt32());
    }
    else
        guid = pBanned->GetGUID();

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    // make sure there is only one active ban
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid.GetCounter());
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_BAN);
    stmt->setUInt32(0, guid.GetCounter());
    stmt->setUInt32(1, duration_secs);
    stmt->setString(2, author);
    stmt->setString(3, reason);
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);

    if (pBanned)
        pBanned->GetSession()->KickPlayer();

    return BAN_SUCCESS;
}

/// Remove a ban from a character
bool World::RemoveBanCharacter(std::string const& name)
{
    Player* pBanned = ObjectAccessor::FindPlayerByName(name);
    ObjectGuid guid;

    /// Pick a player to ban if not online
    if (!pBanned)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);
        stmt->setString(0, name);
        PreparedQueryResult resultCharacter = CharacterDatabase.Query(stmt);

        if (!resultCharacter)
            return false;

        guid = ObjectGuid(HighGuid::Player, (*resultCharacter)[0].GetUInt32());
    }
    else
        guid = pBanned->GetGUID();

    if (!guid)
        return false;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid.GetCounter());
    CharacterDatabase.Execute(stmt);
    LoginDatabase.PExecute("DELETE FROM account_muted WHERE char_id = %u AND realmid  = %u", guid, realm.Id.Realm);
    CharacterDatabase.PExecute("DELETE FROM rated_pvp_info WHERE guid = %u", guid);
    return true;
}

void World::OnStartup()
{
    m_threadId = std::this_thread::get_id();
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    GameTime::UpdateGameTimers();
    time_t thisTime = time(NULL);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;

    ///- if there is a shutdown timer
    if (!IsStopped() && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
                m_stopEvent = true;                         // exist code already set
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode)
{
    // ignore if server shutdown at next tick
    if (IsStopped())
        return;

    m_ShutdownMask = options;
    m_ExitCode = exitcode;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if (time == 0)
    {
        if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
            m_stopEvent = true;                             // exist code already set
        else
            m_ShutdownTimer = 1;                            //So that the session count is re-evaluated at next world tick
    }
    ///- Else set the shutdown timer and warn users
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }

    sScriptMgr->OnShutdownInitiate(ShutdownExitCode(exitcode), ShutdownMask(options));
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show, Player* player)
{
    // not show messages for idle shutdown mode
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    ///- Display a message every 12 hours, hours, 5 minutes, minute, 5 seconds and finally seconds
    if (show ||
        (m_ShutdownTimer < 10) || // < 30 sec; every 5 sec
        (m_ShutdownTimer < 30 && (m_ShutdownTimer % 5) == 0) || // < 5 min ; every 1 min
        (m_ShutdownTimer < 5 * MINUTE && (m_ShutdownTimer % MINUTE) == 0) || // < 30 min ; every 5 min
        (m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) || // < 12 h ; every 1 h
        (m_ShutdownTimer < 12 * HOUR && (m_ShutdownTimer % HOUR) == 0) || // > 12 h ; every 12 h
        (m_ShutdownTimer > 12 * HOUR && (m_ShutdownTimer % (12 * HOUR)) == 0))
    {
        std::string str = secsToTimeString(m_ShutdownTimer);

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid, str.c_str(), player);
        TC_LOG_DEBUG("misc", "Server is %s in %s", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"), str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    // nothing cancel or too later
    if (!m_ShutdownTimer || m_stopEvent)
        return;

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_ExitCode = SHUTDOWN_EXIT_CODE;                       // to default value
    SendServerMessage(msgid);

    TC_LOG_DEBUG("misc", "Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"));

    sScriptMgr->OnShutdownCancel();
}

/// Send a server message to the user(s)
void World::SendServerMessage(ServerMessageType type, const char *text, Player* player)
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50);              // guess size
    data << uint32(type);
    if (type <= SERVER_MSG_STRING)
        data << text;

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        SendGlobalMessage(&data);
}

void World::UpdateSessions(uint32 diff)
{
    ///- Add new sessions
    WorldSession* sess = nullptr;
    while (addSessQueue.next(sess))
        AddSession_(sess);

    ///- Then send an update signal to remaining ones
    for (SessionMap::iterator itr = m_sessions.begin(), next; itr != m_sessions.end(); itr = next)
    {
        next = itr;
        ++next;

        ///- and remove not active sessions from the list
        WorldSession* pSession = itr->second;
        WorldSessionFilter updater(pSession);

        if (!pSession->Update(diff, updater))    // As interval = 0
        {
            if (!RemoveQueuedPlayer(itr->second) && itr->second && getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
                m_disconnects[itr->second->GetAccountId()] = time(NULL);
            RemoveQueuedPlayer(pSession);
            m_sessions.erase(itr);
            delete pSession;

        }
    }
}

// This handles the issued and queued CLI commands
void World::ProcessCliCommands()
{
    CliCommandHolder::Print* zprint = NULL;
    void* callbackArg = NULL;
    CliCommandHolder* command = NULL;
    while (cliCmdQueue.next(command))
    {
        TC_LOG_INFO("misc", "CLI command under processing...");
        CliHandler handler(command);
        handler.ParseCommands(command->m_command.c_str());
        if (!command->m_shared)
        {
            if (command->m_commandFinished)
                command->m_commandFinished(command->m_callbackArg, !handler.HasSentErrorMessage());
        }
    }
}

void World::SendAutoBroadcast()
{
    if (m_autobroadcasts.empty())
        return;

    uint32 weight = 0;
    AutobroadcastsWeightMap selectionWeights;
    std::string msg;

    for (AutobroadcastsWeightMap::const_iterator it = m_autobroadcastsWeights.begin(); it != m_autobroadcastsWeights.end(); ++it)
    {
        if (it->second)
        {
            weight += it->second;
            selectionWeights[it->first] = it->second;
        }
    }

    if (weight)
    {
        uint32 selectedWeight = urand(0, weight - 1);
        weight = 0;
        for (AutobroadcastsWeightMap::const_iterator it = selectionWeights.begin(); it != selectionWeights.end(); ++it)
        {
            weight += it->second;
            if (selectedWeight < weight)
            {
                msg = m_autobroadcasts[it->first];
                break;
            }
        }
    }
    else
        msg = m_autobroadcasts[urand(0, m_autobroadcasts.size())];

    uint32 abcenter = sWorld->getIntConfig(WorldIntConfigs::CONFIG_AUTOBROADCAST_CENTER);

    if (abcenter == 0)
        sWorld->SendWorldText(LANG_AUTO_BROADCAST, msg.c_str());
    else if (abcenter == 1)
    {
        WorldPacket data(SMSG_NOTIFICATION, 2 + msg.length());
        data.WriteBits(msg.length(), 12);
        data.FlushBits();
        data.WriteString(msg);
        sWorld->SendGlobalMessage(&data);
    }
    else if (abcenter == 2)
    {
        sWorld->SendWorldText(LANG_AUTO_BROADCAST, msg.c_str());

        WorldPacket data(SMSG_NOTIFICATION, 2 + msg.length());
        data.WriteBits(msg.length(), 12);
        data.FlushBits();
        data.WriteString(msg);
        sWorld->SendGlobalMessage(&data);
    }

    TC_LOG_DEBUG("misc", "AutoBroadcast: '%s'", msg.c_str());
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_COUNT);
    stmt->setUInt32(0, accountId);
    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&World::_UpdateRealmCharCount, this, std::placeholders::_1)));
}

void World::_UpdateRealmCharCount(PreparedQueryResult resultCharCount)
{
    if (resultCharCount)
    {
        Field* fields = resultCharCount->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        uint8 charCount = uint8(fields[1].GetUInt64());

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHARACTERS_BY_REALM);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realm.Id.Realm);
        LoginDatabase.Execute(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_REALM_CHARACTERS);
        stmt->setUInt8(0, charCount);
        stmt->setUInt32(1, accountId);
        stmt->setUInt32(2, realm.Id.Realm);
        LoginDatabase.Execute(stmt);
    }
}

void World::InitWeeklyQuestResetTime()
{
    time_t wstime = uint64(sWorld->getWorldState(WS_WEEKLY_QUEST_RESET_TIME));
    time_t curtime = time(NULL);
    m_NextWeeklyQuestReset = wstime < curtime ? curtime : time_t(wstime);
}

void World::InitDailyQuestResetTime()
{
    time_t mostRecentQuestTime;

    QueryResult result = CharacterDatabase.Query("SELECT MAX(time) FROM character_queststatus_daily");
    if (result)
    {
        Field* fields = result->Fetch();
        mostRecentQuestTime = time_t(fields[0].GetUInt32());
    }
    else
        mostRecentQuestTime = 0;

    // client built-in time for reset is 6:00 AM
    // FIX ME: client not show day start time
    time_t curTime = time(NULL);
    tm localTm;
    localtime_r(&curTime, &localTm);
    localTm.tm_hour = 6;
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;

    // current day reset time
    time_t curDayResetTime = mktime(&localTm);

    // last reset time before current moment
    time_t resetTime = (curTime < curDayResetTime) ? curDayResetTime - DAY : curDayResetTime;

    // need reset (if we have quest time before last reset time (not processed by some reason)
    if (mostRecentQuestTime && mostRecentQuestTime <= resetTime)
        m_NextDailyQuestReset = mostRecentQuestTime;
    else // plan next reset time
        m_NextDailyQuestReset = (curTime >= curDayResetTime) ? curDayResetTime + DAY : curDayResetTime;
}

void World::InitMonthlyQuestResetTime()
{
    time_t wstime = uint64(sWorld->getWorldState(WS_MONTHLY_QUEST_RESET_TIME));
    time_t curtime = time(NULL);
    m_NextMonthlyQuestReset = wstime < curtime ? curtime : time_t(wstime);
}

void World::InitRandomBGResetTime()
{
    time_t bgtime = uint64(sWorld->getWorldState(WS_BG_DAILY_RESET_TIME));
    if (!bgtime)
        m_NextRandomBGReset = time_t(time(NULL));         // game time not yet init

    // generate time by config
    time_t curTime = time(NULL);
    tm localTm;
    localtime_r(&curTime, &localTm);
    localTm.tm_hour = getIntConfig(CONFIG_RANDOM_BG_RESET_HOUR);
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    // current day reset time
    time_t nextDayResetTime = mktime(&localTm);

    // next reset time before current moment
    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    // normalize reset time
    m_NextRandomBGReset = bgtime < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!bgtime)
        sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::InitGuildResetTime()
{
    time_t gtime = uint64(getWorldState(WS_GUILD_DAILY_RESET_TIME));
    if (!gtime)
        m_NextGuildReset = time_t(time(NULL));         // game time not yet init

    // generate time by config
    time_t curTime = time(NULL);
    tm localTm;
    localtime_r(&curTime, &localTm);
    localTm.tm_hour = getIntConfig(CONFIG_GUILD_RESET_HOUR);
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    // current day reset time
    time_t nextDayResetTime = mktime(&localTm);

    // next reset time before current moment
    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    // normalize reset time
    m_NextGuildReset = gtime < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!gtime)
        sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));
}

void World::InitCurrencyResetTime()
{
    time_t currencytime = uint64(sWorld->getWorldState(WS_CURRENCY_RESET_TIME));
    time_t now = time(nullptr);

    if (currencytime)   // Normal case (event if server was offline)
    {
        m_nextCurrencyReset = currencytime;
        TC_LOG_INFO("currency", "World::InitCurrencyResetTime next reset time loaded: %u", uint32(m_nextCurrencyReset));
    }
    else                // Reschedule
    {
        m_nextCurrencyReset = time(nullptr);

        uint32 day = getIntConfig(CONFIG_CURRENCY_RESET_DAY);
        if (day == 7)   // Week starts from sunday
            day = 0;
        uint32 hour = getIntConfig(CONFIG_CURRENCY_RESET_HOUR);

        tm tmNow = *localtime(&now);
        time_t next;

        if (day > uint32(tmNow.tm_wday) || (day == uint32(tmNow.tm_wday) && hour > uint32(tmNow.tm_hour)))
        {
            next = now + (day - tmNow.tm_wday) * DAY;
        }
        else
        {
            next = now - (tmNow.tm_wday - day) * DAY;
            next += 7 * DAY;
        }

        tm tmNext = *localtime(&next);
        tmNext.tm_hour = hour;
        tmNext.tm_min = 0;
        tmNext.tm_sec = 0;
        m_nextCurrencyReset = mktime(&tmNext);

        sWorld->setWorldState(WS_CURRENCY_RESET_TIME, uint64(m_nextCurrencyReset));
        TC_LOG_INFO("currency", "World::InitCurrencyResetTime next reset time initialized: %u", uint32(m_nextCurrencyReset));
    }
}

void World::ResetDailyQuests()
{
    TC_LOG_INFO("misc", "Daily quests reset for all characters.");

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_DAILY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (Player* player = itr->second->GetPlayer())
        {
            player->ResetDailyQuestStatus();
        }
    }

    // change available dailies
    sQuestPoolMgr->ChangeDailyQuests();

    sAnticheatMgr->ResetDailyReportStates();
}

void World::ResetCurrencyWeekCap()
{
    TC_LOG_INFO("currency", "World::ResetCurrencyWeekCap");

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    trans->Append(CharacterDatabase.GetPreparedStatement(CHAR_UPD_ALL_CURRENCY_WEEKLY));
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ALL_RATED_PVP_INFO_LAST_WEEK);
    stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
    trans->Append(stmt);
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ALL_RATED_PVP_INFO_WEEKLY);
    stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);

    for (auto&& itr : m_sessions)
    {
        if (Player* player = itr.second->GetPlayer())
        {
            player->ResetCurrencyWeekCap();

            player->GetSession()->RemoveFlag(ACC_FLAG_VALOR_CAP_REACHED);
            player->RemoveAura(SPELL_VALOR_OF_THE_ANCIENTS);
        }
    }
    LoginDatabase.PExecute("UPDATE account SET flags = flags&~(%u)", ACC_FLAG_VALOR_CAP_REACHED);

    sRatedPvpMgr->UpdateCap();

    m_nextCurrencyReset = time_t(m_nextCurrencyReset + DAY * getIntConfig(CONFIG_CURRENCY_RESET_INTERVAL));
    sWorld->setWorldState(WS_CURRENCY_RESET_TIME, uint64(m_nextCurrencyReset));
}

void World::ResetLootLockouts()
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ALL_LOOTLOCKOUTS);
    CharacterDatabase.Execute(stmt);

    for (auto&& it : m_sessions)
        if (Player* player = it.second->GetPlayer())
            player->ClearLootLockouts();
}

void World::DBCleanup()
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_OLD_LOGS);

    stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_LOGDB_CLEARTIME));
    stmt->setUInt32(1, uint32(time(0)));

    LoginDatabase.Execute(stmt);

}

void World::LoadDBAllowedSecurityLevel()
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALMLIST_SECURITY_LEVEL);
    stmt->setInt32(0, int32(realm.Id.Realm));
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (result)
        SetPlayerSecurityLimit(AccountTypes(result->Fetch()->GetUInt8()));
}

void World::SetPlayerSecurityLimit(AccountTypes _sec)
{
    AccountTypes sec = _sec < SEC_CONSOLE ? _sec : SEC_PLAYER;
    bool update = sec > m_allowedSecurityLevel;
    m_allowedSecurityLevel = sec;
    if (update)
        KickAllLess(m_allowedSecurityLevel);
}

void World::ResetWeeklyQuests()
{
    TC_LOG_INFO("misc", "Weekly quests reset for all characters.");

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_WEEKLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetWeeklyQuestStatus();

    m_NextWeeklyQuestReset = time_t(m_NextWeeklyQuestReset + WEEK);
    sWorld->setWorldState(WS_WEEKLY_QUEST_RESET_TIME, uint64(m_NextWeeklyQuestReset));

    // change available weeklies
    sQuestPoolMgr->ChangeWeeklyQuests();
}

void World::ResetMonthlyQuests()
{
    TC_LOG_INFO("misc", "Monthly quests reset for all characters.");

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_MONTHLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetMonthlyQuestStatus();

    sQuestPoolMgr->ChangeMonthlyQuests();

    // generate time
    time_t curTime = time(NULL);
    tm localTm;
    localtime_r(&curTime, &localTm);

    int month   = localTm.tm_mon;
    int year    = localTm.tm_year;

    ++month;

    // month 11 is december, next is january (0)
    if (month > 11)
    {
        month = 0;
        year += 1;
    }

    // reset time for next month
    localTm.tm_year     = year;
    localTm.tm_mon      = month;
    localTm.tm_mday     = 1;        // don't know if we really need config option for day / hour
    localTm.tm_hour     = 0;
    localTm.tm_min      = 0;
    localTm.tm_sec      = 0;

    time_t nextMonthResetTime = mktime(&localTm);

    // plan next reset time
    m_NextMonthlyQuestReset = (curTime >= nextMonthResetTime) ? nextMonthResetTime + MONTH : nextMonthResetTime;

    sWorld->setWorldState(WS_MONTHLY_QUEST_RESET_TIME, uint64(m_NextMonthlyQuestReset));
}

void World::ResetEventSeasonalQuests(uint16 event_id)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_SEASONAL);
    stmt->setUInt16(0, event_id);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetSeasonalQuestStatus(event_id);
}

void World::ResetRandomBG()
{
    TC_LOG_INFO("misc", "Random BG status reset for all characters.");

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BATTLEGROUND_RANDOM);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BATTLEGROUND_WEEKEND);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second->GetPlayer())
        {
            itr->second->GetPlayer()->SetRandomWinner(false);
            itr->second->GetPlayer()->SetBgWeekendWinner(false);
        }
    }

    m_NextRandomBGReset = time_t(m_NextRandomBGReset + DAY);
    sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::ResetGuildCap()
{
    m_NextGuildReset = time_t(m_NextGuildReset + DAY);
    sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));
    uint32 week = getWorldState(WS_GUILD_WEEKLY_RESET_TIME);
    week = week < 7 ? week + 1 : 1;

    TC_LOG_INFO("misc", "Guild Daily Cap reset. Week: %u", week == 1);
    sWorld->setWorldState(WS_GUILD_WEEKLY_RESET_TIME, week);
    sGuildMgr->ResetTimes(week == 1);
}

void World::UpdateMaxSessionCounters()
{
    m_maxActiveSessionCount = std::max(m_maxActiveSessionCount, uint32(m_sessions.size()-m_QueuedPlayer.size()));
    m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount, uint32(m_QueuedPlayer.size()));
}

void World::LoadDBVersion()
{
    QueryResult result = WorldDatabase.Query("SELECT db_version, cache_id FROM version LIMIT 1");
    if (result)
    {
        Field* fields = result->Fetch();

        m_DBVersion = fields[0].GetString();
        // will be overwrite by config values if different and non-0
        m_int_configs[CONFIG_CLIENTCACHE_VERSION] = fields[1].GetUInt32();
    }

    if (m_DBVersion.empty())
        m_DBVersion = "Unknown world database.";
}

void World::ProcessStartEvent()
{
    isEventKillStart = true;
}

void World::ProcessStopEvent()
{
    isEventKillStart = false;
}

void World::UpdateAreaDependentAuras()
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
        {
            itr->second->GetPlayer()->UpdateAreaDependentAuras(itr->second->GetPlayer()->GetAreaId());
            itr->second->GetPlayer()->UpdateZoneDependentAuras(itr->second->GetPlayer()->GetZoneId());
        }
}

void World::LoadWorldStates()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT entry, value FROM worldstates");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 world states. DB table `worldstates` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        m_worldstates[fields[0].GetUInt32()] = fields[1].GetUInt32();
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u world states in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

}

// Setting a worldstate will save it to DB
void World::setWorldState(uint32 index, uint64 value)
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    if (it != m_worldstates.end())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_WORLDSTATE);

        stmt->setUInt32(0, uint32(value));
        stmt->setUInt32(1, index);

        CharacterDatabase.Execute(stmt);
    }
    else
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_WORLDSTATE);

        stmt->setUInt32(0, index);
        stmt->setUInt32(1, uint32(value));

        CharacterDatabase.Execute(stmt);
    }
    m_worldstates[index] = value;
}

uint64 World::getWorldState(uint32 index) const
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    return it != m_worldstates.end() ? it->second : 0;
}

void World::ProcessQueryCallbacks()
{
    _queryProcessor.ProcessReadyCallbacks();
    // PreparedQueryResult result;

    // while (!m_realmCharCallbacks.is_empty())
    // {
    //     ACE_Future<PreparedQueryResult> lResult;
    //     ACE_Time_Value timeout = ACE_Time_Value::zero;
    //     if (m_realmCharCallbacks.next_readable(lResult, &timeout) != 1)
    //         break;

    //     if (lResult.ready())
    //     {
    //         lResult.get(result);
    //         _UpdateRealmCharCount(result);
    //         lResult.cancel();
    //     }
    // }
}

/**
* @brief Loads several pieces of information on server startup with the low GUID
* There is no further database query necessary.
* These are a number of methods that work into the calling function.
*
* @param guid Requires a lowGUID to call
* @return Name, Gender, Race, Class and Level of player character
* Example Usage:
* @code
*    CharacterNameData const* nameData = sWorld->GetCharacterNameData(lowGUID);
*    if (!nameData)
*        return;
*
* std::string playerName = nameData->m_name;
* uint8 playerGender = nameData->m_gender;
* uint8 playerRace = nameData->m_race;
* uint8 playerClass = nameData->m_class;
* uint8 playerLevel = nameData->m_level;
* @endcode
**/

void World::LoadCharacterNameData()
{
    TC_LOG_INFO("server.loading", "Loading character name data");

    QueryResult result = CharacterDatabase.Query("SELECT c.guid, name, race, gender, class, level, account, genitive, dative, accusative, instrumental, prepositional FROM characters c LEFT JOIN character_declinedname cdn ON cdn.guid=c.guid WHERE deleteDate IS NULL");
    if (!result)
    {
        TC_LOG_INFO("server.loading", "No character name data loaded, empty query");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        DeclinedName* declinedName = NULL;
        if (!fields[7].IsNull() && !fields[7].GetString().empty())
        {
            declinedName = new DeclinedName();
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                declinedName->name[i] = fields[7 + i].GetString();
        }
        AddCharacterNameData(ObjectGuid(HighGuid::Player, fields[0].GetUInt32()), fields[1].GetString(),
            fields[3].GetUInt8(), fields[2].GetUInt8(), fields[4].GetUInt8(), fields[5].GetUInt8());
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", "Loaded name data for %u characters", count);
}

void World::AddCharacterNameData(ObjectGuid guid, std::string const& name, uint8 gender, uint8 race, uint8 playerClass, uint8 level)
{
    CharacterNameData& data = _characterNameDataMap[guid];
    data.m_name = name;
    data.m_race = race;
    data.m_gender = gender;
    data.m_class = playerClass;
    data.m_level = level;
}

void World::UpdateCharacterNameData(ObjectGuid guid, std::string const& name, uint8 gender /*= GENDER_NONE*/, uint8 race /*= RACE_NONE*/, DeclinedName const* declinedName)
{
    std::map<ObjectGuid, CharacterNameData>::iterator itr = _characterNameDataMap.find(guid);
    if (itr == _characterNameDataMap.end())
        return;

    itr->second.m_name = name;

    if (gender != GENDER_NONE)
        itr->second.m_gender = gender;

    if (race != RACE_NONE)
        itr->second.m_race = race;

    if (declinedName)
    {
        delete itr->second.m_declinedName;
        itr->second.m_declinedName = declinedName;
    }

    WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);

    SendGlobalMessage(&data);
}

void World::UpdateCharacterNameDataLevel(ObjectGuid guid, uint8 level)
{
    std::map<ObjectGuid, CharacterNameData>::iterator itr = _characterNameDataMap.find(guid);
    if (itr == _characterNameDataMap.end())
        return;

    itr->second.m_level = level;
}

void World::UpdateCharacterNameDataClass(ObjectGuid guid, uint8 classID)
{
    std::map<ObjectGuid, CharacterNameData>::iterator iter = _characterNameDataMap.find(guid);
    if (iter == _characterNameDataMap.end())
        return;

    iter->second.m_class = classID;
}

void World::UpdateCharacterNameDataAccount(ObjectGuid guid, ObjectGuid account)
{
    std::map<ObjectGuid, CharacterNameData>::iterator iter = _characterNameDataMap.find(guid);
    if (iter == _characterNameDataMap.end())
        return;

    iter->second.m_accountID = account;
}

CharacterNameData const* World::GetCharacterNameData(ObjectGuid guid) const
{
    std::map<ObjectGuid, CharacterNameData>::const_iterator itr = _characterNameDataMap.find(guid);
    if (itr != _characterNameDataMap.end())
        return &itr->second;
    else
        return NULL;
}

void World::LoadAccountCacheData()
{
    TC_LOG_INFO("server.loading", "Loading account cache");

    QueryResult result = LoginDatabase.Query("SELECT id, project_member_id FROM account");
    if (!result)
    {
        TC_LOG_INFO("server.loading", "No account cache data loaded, empty query.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid accountId(HighGuid::WowAccount, fields[0].GetUInt32());

        AccountCacheData& data = GetAccountCacheData(accountId);
        data.MemberID = fields[1].GetUInt32();

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", "Loaded account cache data for %u accounts", count);
}

AccountCacheData& World::GetAccountCacheData(ObjectGuid accountId)
{
    auto itr = _accountCacheData.find(accountId);
    if (itr != _accountCacheData.end())
        return itr->second;

    return _accountCacheData[accountId];
}

void World::UpdatePhaseDefinitions()
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
            itr->second->GetPlayer()->GetPhaseMgr().NotifyStoresReloaded();
}

#if _MSC_VER && (_MSC_VER >= 1900)
#define timezone _timezone
#endif


// New AutoBroadcast scheduling system
void AutoBroadcastEntry::ScheduleData::Part::Set(std::string dbData)
{
    Valid = true;

    auto denomIndex = dbData.find('/');
    if (denomIndex != std::string::npos)
    {
        int32 denom = atoi(dbData.substr(denomIndex + 1).c_str());
        if (!ValidateValue(denom))
            return;
        Denominator = denom;
        dbData.erase(denomIndex);
    }

    auto rangeIndex = dbData.find('-');
    if (rangeIndex != std::string::npos)
    {
        AcceptedRange.Value = std::make_pair(ParseToken(dbData.substr(0, rangeIndex)), ParseToken(dbData.substr(rangeIndex + 1)));
        if (!Valid)
            return;
        AcceptedRange.HasValue = true;
    }

    auto setIndex = dbData.find(',');
    if (setIndex != std::string::npos)
    {
        Tokenizer set(dbData, ',');
        for (auto&& token : set)
            AcceptedValues.Value.insert(ParseToken(token));
        if (!Valid)
            return;
        AcceptedValues.HasValue = true;
    }

    auto anyIndex = dbData.find('*');
    if (anyIndex != std::string::npos)
        Any = true;

    uint32 totalValueTypes = (Any ? 1 : 0) + (AcceptedRange.HasValue ? 1 : 0) + (AcceptedValues.HasValue ? 1 : 0);
    if (totalValueTypes > 1)
    {
        Valid = false;
        return;
    }

    if (!totalValueTypes)
    {
        AcceptedValue.Value = ParseToken(dbData);
        if (!Valid)
            return;
        AcceptedValue.HasValue = true;
    }
}

bool AutoBroadcastEntry::ScheduleData::Part::Satisfies(uint8 value) const
{
    return SatisfiesValue(value) && SatisfiesDenominator(value);
}

bool AutoBroadcastEntry::ScheduleData::Part::SatisfiesValue(uint8 const& value) const
{
    if (Any)
        return true;

    if (AcceptedValue.HasValue && AcceptedValue.Value == value)
        return true;

    if (AcceptedValues.HasValue && AcceptedValues.Value.find(value) != AcceptedValues.Value.end())
        return true;

    if (AcceptedRange.HasValue && AcceptedRange.Value.first <= value && value <= AcceptedRange.Value.second)
        return true;

    return false;
}

bool AutoBroadcastEntry::ScheduleData::Part::SatisfiesDenominator(uint8 const& value) const
{
    if (!Denominator)
        return true;

    if (AcceptedRange.HasValue)
        return !((value - AcceptedRange.Value.first) % Denominator);

    return !(value % Denominator);
}

bool AutoBroadcastEntry::ScheduleData::Part::ValidateValue(uint32 value)
{
    if (value >= std::numeric_limits<uint8>::min() && value <= std::numeric_limits<uint8>::max())
        return true;

    Valid = false;
    return false;
}

uint8 AutoBroadcastEntry::ScheduleData::Part::ParseToken(std::string token)
{
    if (!token.size())
    {
        Valid = false;
        return 0;
    }

    if (token[0] >= '0' && token[0] <= '9')
    {
        uint32 value = atoi(token.c_str());
        if (!ValidateValue(value))
            return 0;

        return value;
    }

    std::transform(token.begin(), token.end(), token.begin(), tolower);
    if (                  token == "sun") return 0;
    if (token == "jan" || token == "mon") return 1;
    if (token == "feb" || token == "tue") return 2;
    if (token == "mar" || token == "wed") return 3;
    if (token == "apr" || token == "thu") return 4;
    if (token == "may" || token == "fri") return 5;
    if (token == "jun" || token == "sat") return 6;
    if (token == "jul") return 7;
    if (token == "aug") return 8;
    if (token == "sep") return 9;
    if (token == "oct") return 10;
    if (token == "nov") return 11;
    if (token == "dec") return 12;

    Valid = false;
    return 0;
}

AutoBroadcastEntry::ScheduleData::ScheduleData(std::string const& dbData)
{
    Tokenizer tokens(dbData, ' ', 5);
    if (tokens.size() != 5)
        return;

    Minute.Set(tokens[0]);
    Hour.Set(tokens[1]);
    Day.Set(tokens[2]);
    Month.Set(tokens[3]);
    Weekday.Set(tokens[4]);

    if (!Minute.Valid || !Hour.Valid || !Day.Valid || !Month.Valid || !Weekday.Valid)
        return;

    Valid = true;
}

bool AutoBroadcastEntry::ScheduleData::Satisfies(tm const& lt) const
{
    return Minute.Satisfies(lt.tm_min)
        && Hour.Satisfies(lt.tm_hour)
        && Day.Satisfies(lt.tm_mday)
        && Month.Satisfies(lt.tm_mon + 1)
        && Weekday.Satisfies(lt.tm_wday);
}

void AutoBroadcastEntry::Update(time_t const& now, tm const& lt)
{
    if (!Schedule.Valid)
    {
        TC_LOG_ERROR("misc", "AutoBroadcastEntry::Update: Schedule data for autobroadcast with id %d is malformed", ID);
        return;
    }

    if (now < CooldownUntil)
        return;

    if (Schedule.Satisfies(lt))
    {
        CooldownUntil = now + 90; // 1.5 minutes to make sure. 45 seconds could probably do the job as well, but whatever.

        switch (sWorld->getIntConfig(CONFIG_AUTOBROADCAST_CENTER))
        {
            case 0:
                sWorld->SendWorldText(LANG_AUTO_BROADCAST, Text.c_str());
                break;
            case 1:
            {
                WorldPacket data(SMSG_NOTIFICATION, Text.size() + 1);
                data.WriteBits(Text.size(), 12);
                data.FlushBits();
                data.append(Text.c_str(), Text.size());
                sWorld->SendGlobalMessage(&data);
                break;
            }
            case 2:
            {
                sWorld->SendWorldText(LANG_AUTO_BROADCAST, Text.c_str());

                WorldPacket data(SMSG_NOTIFICATION, Text.size() + 1);
                data.WriteBits(Text.size(), 12);
                data.FlushBits();
                data.append(Text.c_str(), Text.size());
                sWorld->SendGlobalMessage(&data);
                break;
            }
            default:
                break;
        }

        TC_LOG_INFO("misc", "AutoBroadcast: '%s'", Text.c_str());
    }
}

void World::UpdateBonusRatesState()
{
    time_t curTime = time(NULL);
    tm localTm;
    localtime_r(&curTime, &localTm);

    for (auto&& bonusRates : m_bonusRates)
        bonusRates.second.Update(curTime, localTm);
}

void BonusRatesEntry::Load(Field* fields)
{
    uint32 index = 0;
    m_id = fields[index++].GetUInt32();
    m_schedules.clear();
    for (auto&& schedule : Tokenizer(fields[index++].GetString(), ';'))
    {
        m_schedules.push_back(AutoBroadcastEntry::ScheduleData(schedule));
        if (!m_schedules.back().Valid)
            TC_LOG_ERROR("server.loading", "BonusRatesEntry::Update: Schedule data for bonus rate %u is malformed", m_id);
    }
    m_multiplier = fields[index++].GetFloat();
    m_rates.clear();
    for (auto&& rate : Tokenizer(fields[index++].GetString(), ' '))
        m_rates.insert(rate);
    m_startAnnouncement = fields[index++].GetString();
    m_endAnnouncement = fields[index++].GetString();
    m_activeAnnouncement = fields[index++].GetString();
}

void BonusRatesEntry::Update(time_t const& now, tm const& lt)
{
    bool shouldBeActive = false;
    for (auto&& schedule : m_schedules)
    {
        if (!schedule.Valid)
        {
            TC_LOG_ERROR("server.loading", "BonusRatesEntry::Update: Schedule data for bonus rate %u is malformed", m_id);
            return;
        }

        if (shouldBeActive == schedule.Satisfies(lt))
            break;
    }

    if (m_activatedBySchedule == shouldBeActive)
        return;
    m_activatedBySchedule = shouldBeActive;

    if (m_active != shouldBeActive)
    {
        SetActive(shouldBeActive);
        sWorld->LoadRates();
    }
}

void BonusRatesEntry::SetActive(bool active)
{
    m_active = active;

    if (m_active)
    {
        if (!m_startAnnouncement.empty())
            sWorld->SendServerMessage(SERVER_MSG_STRING, m_startAnnouncement.c_str());
    }
    else
    {
        if (!m_endAnnouncement.empty())
            sWorld->SendServerMessage(SERVER_MSG_STRING, m_endAnnouncement.c_str());
    }
}

bool BonusRatesEntry::IsAffectingRate(char const* rateConfigName) const
{
    return m_rates.find(std::string(rateConfigName)) != m_rates.end();
}

float World::getRate(Rates rate, Player* player) const
{
    return getRate(rate);
}

float World::getRate(Rates rate, WorldSession* session) const
{
    return getRate(rate);
}

void World::InitServerAutoRestartTime()
{
	time_t serverRestartTime = uint64(sWorld->getWorldState(WS_AUTO_SERVER_RESTART_TIME));
	if (!serverRestartTime)
		m_NextServerRestart = time_t(time(nullptr));         // game time not yet init

	// generate time by config
	time_t curTime = time(nullptr);
	tm localTm;
	localtime_r(&curTime, &localTm);
	localTm.tm_hour = getIntConfig(CONFIG_AUTO_SERVER_RESTART_HOUR);
	localTm.tm_min = 0;
	localTm.tm_sec = 0;

	// current day reset time
	time_t nextDayRestartTime = mktime(&localTm);

	// next reset time before current moment
	if (curTime >= nextDayRestartTime)
		nextDayRestartTime += DAY;

	// normalize reset time
	m_NextServerRestart = serverRestartTime < curTime ? nextDayRestartTime - DAY : nextDayRestartTime;

	if (sWorld->getBoolConfig(CONFIG_DISABLE_RESTART))
		m_NextServerRestart = time_t(m_NextServerRestart + DAY);

	if (!serverRestartTime)
		sWorld->setWorldState(WS_AUTO_SERVER_RESTART_TIME, uint64(m_NextServerRestart));
}

void World::RemoveOldCorpses()
{
    m_timers[WUPDATE_CORPSES].SetCurrent(m_timers[WUPDATE_CORPSES].GetInterval());
}

Realm realm;

