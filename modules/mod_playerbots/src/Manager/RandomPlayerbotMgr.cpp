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

#include "RandomPlayerbotMgr.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <random>

#include "AccountMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BotFactory.h"
#include "CharacterCache.h"
#include "CellImpl.h"
#include "ChannelMgr.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "DatabaseEnv.h"
#include "Define.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GuildMgr.h"
#include "LFGMgr.h"
#include "MapManager.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PerformanceMonitor.h"
#include "Playerbots.h"
#include "RandomItemManager.h"
#include "RandomPlayerbotBracketMgr.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "World.h"

RandomPlayerbotMgr::RandomPlayerbotMgr()
    : PlayerbotHolder(),
    _processTicks(0)
{
    _playersLevel = 1;// sPlayerbotAIConfig->randombotStartingLevel;
    _playersCheckTimer = 0;
}

RandomPlayerbotMgr::~RandomPlayerbotMgr() {}

void RandomPlayerbotMgr::Reserve(const uint32 size)
{
    _currentBots.reserve(size);
}

uint32 RandomPlayerbotMgr::GetMaxAllowedBotCount()
{
    return GetEventValue(0, "bot_count");
}

void RandomPlayerbotMgr::ScheduleRandomize(uint32 bot, uint32 time)
{
    SetEventValue(bot, "randomize", 1, time);
}

void RandomPlayerbotMgr::ScheduleTeleport(uint32 bot, uint32 time)
{
    if (!time)
        time = 60 + urand(sPlayerbotAIConfig->randomBotUpdateInterval, sPlayerbotAIConfig->randomBotUpdateInterval * 3);

    SetEventValue(bot, "teleport", 1, time);
}

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed, bool /*minimal*/)
{
    if (totalPmo)
        totalPmo->finish();

    totalPmo = sPerformanceMonitor->start(PERF_MON_TOTAL, "RandomPlayerbotMgr::FullTick");

    if (!sPlayerbotAIConfig->randomBotAutologin || !sPlayerbotAIConfig->enabled)
        return;

    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (!maxAllowedBotCount || (maxAllowedBotCount < sPlayerbotAIConfig->minRandomBots ||
        maxAllowedBotCount > sPlayerbotAIConfig->maxRandomBots))
    {
        maxAllowedBotCount = sPlayerbotAIConfig->minRandomBots + (std::rand() % (sPlayerbotAIConfig->maxRandomBots - sPlayerbotAIConfig->minRandomBots + 1));
        auto randomInterval = sPlayerbotAIConfig->randomBotCountChangeMinInterval +
            (std::rand() % (sPlayerbotAIConfig->randomBotCountChangeMaxInterval - sPlayerbotAIConfig->randomBotCountChangeMinInterval + 1));
        SetEventValue(0, "bot_count", maxAllowedBotCount, randomInterval);
    }

    GetBots();
    std::vector<uint32> availableBots = _currentBots;
    uint32 availableBotCount = _currentBots.size();
    uint32 onlineBotCount = playerBots.size();

    uint32 onlineBotFocus = 75;
    if (onlineBotCount < (uint32)(sPlayerbotAIConfig->minRandomBots * 90 / 100))
        onlineBotFocus = 25;

    // only keep updating till initializing time has completed,
    // which prevents unneeded expensive GameTime calls.
    if (_isBotInitializing)
    {
        _isBotInitializing = sWorld->GetUptime() < sPlayerbotAIConfig->maxRandomBots * (0.11 + 0.4);
    }

    uint32 updateIntervalTurboBoost = _isBotInitializing ? 1 : sPlayerbotAIConfig->randomBotUpdateInterval;
    SetNextCheckDelay(updateIntervalTurboBoost * (onlineBotFocus + 25) * 10);

    PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(
            PERF_MON_TOTAL,
            onlineBotCount < maxAllowedBotCount ? "RandomPlayerbotMgr::Login" : "RandomPlayerbotMgr::UpdateAIInternal");

    if (availableBotCount < maxAllowedBotCount)
    {
        AddRandomBots();
    }

    uint32 updateBots = sPlayerbotAIConfig->randomBotsPerInterval * onlineBotFocus / 100;
    uint32 maxNewBots = onlineBotCount < maxAllowedBotCount ? maxAllowedBotCount - onlineBotCount : 0;
    uint32 loginBots = std::min(sPlayerbotAIConfig->randomBotsPerInterval - updateBots, maxNewBots);

    if (!availableBots.empty())
    {
        // Update bots
        for (auto bot : availableBots)
        {
            if (!GetPlayerBot(bot))
                continue;

            if (ProcessBot(bot))
            {
                updateBots--;
            }

            if (!updateBots)
                break;
        }

        if (loginBots && botLoading.empty())
        {
            loginBots += updateBots;
            loginBots = std::min(loginBots, maxNewBots);

            TC_LOG_INFO("playerbots", "%d new bots", loginBots);

            // Log in bots
            for (auto bot : availableBots)
            {
                if (GetPlayerBot(bot))
                    continue;

                if (ProcessBot(bot))
                {
                    loginBots--;
                }

                if (!loginBots)
                    break;
            }
        }
    }

    if (pmo)
        pmo->finish();
}

uint32 RandomPlayerbotMgr::AddRandomBots()
{
    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");

    if (_currentBots.size() < maxAllowedBotCount)
    {
        maxAllowedBotCount -= _currentBots.size();
        maxAllowedBotCount = std::min(sPlayerbotAIConfig->randomBotsPerInterval, maxAllowedBotCount);

        for (std::vector<uint32>::iterator i = sPlayerbotAIConfig->randomBotAccounts.begin();
            i != sPlayerbotAIConfig->randomBotAccounts.end(); i++)
        {
            uint32 accountId = *i;

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARS_BY_ACCOUNT_ID);
            stmt->setUInt32(0, accountId);
            PreparedQueryResult result = CharacterDatabase.Query(stmt);
            if (!result)
                continue;
            std::vector<uint32> guids;
            do
            {
                Field* fields = result->Fetch();
                uint64 guid = fields[0].GetInt64();
                if (GetEventValue(guid, "add"))
                    continue;

                if (GetEventValue(guid, "logout"))
                    continue;

                if (GetPlayerBot(guid))
                    continue;

                if (std::find(_currentBots.begin(), _currentBots.end(), guid) != _currentBots.end())
                    continue;

                guids.push_back(guid);
            } while (result->NextRow());

            std::mt19937 rnd(time(0));
            std::shuffle(guids.begin(), guids.end(), rnd);

            for (uint32& guid : guids)
            {
                uint32 add_time = 31104000;

                SetEventValue(guid, "add", 1, add_time);
                SetEventValue(guid, "logout", 0, 0);
                SetEventValue(guid, "randomize", 1, add_time * 2);
                SetEventValue(guid, "teleport", 1, add_time * 2);
                _currentBots.push_back(guid);

                maxAllowedBotCount--;
                if (!maxAllowedBotCount)
                    break;
            }

            if (!maxAllowedBotCount)
                break;
        }

        if (maxAllowedBotCount)
            TC_LOG_ERROR("playerbots", "Not enough random bot accounts available. Try to increase RandomBotAccountCount in your conf file");
    }

    return _currentBots.size();
}

void RandomPlayerbotMgr::Remove(Player* bot)
{
    ObjectGuid owner = bot->GetGUID();

    PlayerbotsPreparedStatement* stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_DEL_RANDOM_BOTS_BY_OWNER);
    stmt->setUInt32(0, 0);
    stmt->setUInt32(1, owner.GetCounter());
    PlayerbotsDatabase.Execute(stmt);

    _eventCache[owner.GetCounter()].clear();

    LogoutPlayerBot(owner);
}

void RandomPlayerbotMgr::CheckPlayers()
{
}

bool RandomPlayerbotMgr::ProcessBot(uint32 bot)
{
    Player* player = GetPlayerBot(bot);
    PlayerbotAI* botAI = player ? GET_PLAYERBOT_AI(player) : nullptr;
    ObjectGuid botGUID = ObjectGuid::Create<HighGuid::Player>(bot);

    uint32 isValid = GetEventValue(bot, "add");
    if (!isValid)
    {
        if (!player || !player->GetGroup())
        {
            if (player)
            {
                auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
                TC_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->GetLevel(), player->GetName().c_str());
            }
            else
                TC_LOG_INFO("playerbots", "Bot #%u: log out", bot);

            SetEventValue(bot, "add", 0, 0);
            _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), bot), _currentBots.end());

            if (player)
                LogoutPlayerBot(botGUID);
        }

        return false;
    }

    uint32 randomTime;
    if (!player)
    {
        AddPlayerBot(botGUID, 0);
        randomTime = 1 + (std::rand() % 2);

        uint32 randomBotUpdateInterval = _isBotInitializing ? 1 : sPlayerbotAIConfig->randomBotUpdateInterval;
        int minValue = std::max(5, static_cast<int>(randomBotUpdateInterval * 0.5));
        int maxValue = std::max(12, static_cast<int>(randomBotUpdateInterval * 2));
        randomTime = minValue + (std::rand() % (maxValue - minValue + 1));
        
        SetEventValue(bot, "update", 1, randomTime);

        // do not randomize or teleport immediately after server start (prevent lagging)
        if (!GetEventValue(bot, "randomize"))
        {
            int minValue = std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7));
            int maxValue = std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4));
            randomTime = minValue + (std::rand() % (maxValue - minValue + 1));

            ScheduleRandomize(bot, randomTime);
        }
        if (!GetEventValue(bot, "teleport"))
        {
            int minValue = std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7));
            int maxValue = std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4));
            randomTime = minValue + (std::rand() % (maxValue - minValue + 1));

            ScheduleTeleport(bot, randomTime);
        }

        return true;
    }

    if (!player->IsInWorld())
        return false;

    if (player->GetGroup() || player->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    uint32 update = GetEventValue(bot, "update");
    if (!update)
    {
        if (botAI)
            botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);

        bool update = true;
        if (botAI)
        {
            botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);
            if (!sRandomPlayerbotMgr->IsRandomBot(player))
                update = false;

            if (player->GetGroup() && botAI->GetGroupMaster())
            {
                PlayerbotAI* groupMasterBotAI = GET_PLAYERBOT_AI(botAI->GetGroupMaster());
                if (!groupMasterBotAI || groupMasterBotAI->IsRealPlayer())
                {
                    update = false;
                }
            }
        }

        if (update)
            ProcessBot(player);

        randomTime = urand(60/*sPlayerbotAIConfig->minRandomBotReviveTime*/, 300/*sPlayerbotAIConfig->maxRandomBotReviveTime*/);
        SetEventValue(bot, "update", 1, randomTime);

        return true;
    }

    /*uint32 logout = GetEventValue(bot, "logout");
    if (player && !logout && !isValid)
    {
        auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
        TC_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->GetLevel(), player->GetName().c_str());
        LogoutPlayerBot(botGUID);
        for (auto it = _currentBots.begin(); it != _currentBots.end();)
        {
            if (*it == bot)
            {
                it = _currentBots.erase(it);
                break;
            }
            else
                ++it;
        }
        SetEventValue(bot, "logout", 1, urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime));
        return true;
    }*/

    return false;
}

bool RandomPlayerbotMgr::ProcessBot(Player* player)
{
    ObjectGuid bot = player->GetGUID();

    if (player->InBattleground())
        return false;

    if (player->InBattlegroundQueue())
        return false;

    

    // if death revive
    if (player->isDead())
    {
        if (!GetEventValue(bot, "dead"))
        {
            uint32 randomTime = urand(60/*sPlayerbotAIConfig->minRandomBotReviveTime*/, 300/*sPlayerbotAIConfig->maxRandomBotReviveTime*/);
            TC_LOG_DEBUG("playerbots", "Mark bot %s as dead, will be revived in %us.", player->GetName().c_str(), randomTime);
            SetEventValue(bot, "dead", 1, sPlayerbotAIConfig->maxRandomBotInWorldTime);
            SetEventValue(bot, "revive", 1, randomTime);
            return false;
        }

        if (!GetEventValue(bot, "revive"))
        {
            Revive(player);
            return true;
        }

        return false;
    }

    // only randomize and teleport idle bots
    bool idleBot = false;
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
    if (botAI)
    {
        /*if (TravelTarget* target = botAI->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get())
        {
            if (target->getTravelState() == TravelState::TRAVEL_STATE_IDLE)
            {
                idleBot = true;
            }
        }
        else*/
        //{
            idleBot = true;
        //}
    }
    if (idleBot)
    {
        if (player->GetInstanceId() || player->GetGroup() || player->InArena() || player->inRandomLfgDungeon() || player->IsInFlight() || player->IsInCombat() || !player->IsAlive())
            return false;

        // randomize
        uint32 randomize = GetEventValue(bot, "randomize");
        if (!randomize)
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "Randomize");
            Randomize(player);
            TC_LOG_DEBUG("playerbots", "Bot #%u %s:%u <%s>: randomized", bot.GetCounter(), player->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", player->GetLevel(), player->GetName().c_str());
            if (pmo)
                pmo->finish();
            return true;
        }

        uint32 teleport = GetEventValue(bot, "teleport");
        if (!teleport)
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "RandomTeleportByLocations");
            TC_LOG_DEBUG("playerbots", "Bot #%u <%s>: teleport for level and refresh", bot.GetCounter(), player->GetName().c_str());
            Refresh(player);
            RandomTeleportForLevel(player);
            uint32 time = urand(sPlayerbotAIConfig->minRandomBotTeleportInterval, sPlayerbotAIConfig->maxRandomBotTeleportInterval);
            ScheduleTeleport(bot, time);
            if (pmo)
                pmo->finish();
            return true;
        }
    }

    return false;
}

void RandomPlayerbotMgr::TagForRandomize(Player* bot, uint32 level)
{
    ObjectGuid::LowType guid = bot->GetGUID().GetCounter();

    SetValue(bot, "level", level);
    ScheduleRandomize(guid, 0);
}

void RandomPlayerbotMgr::Revive(Player* player)
{
    uint64 bot = player->GetGUID();

    SetEventValue(bot, "dead", 0, 0);
    SetEventValue(bot, "revive", 0, 0);

    Refresh(player);
}

void RandomPlayerbotMgr::Clear(Player* bot)
{
    //PlayerbotFactory factory(bot, bot->GetLevel());
    //factory.ClearEverything();
}

void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{
    uint32 maxLevel = sPlayerbotAIConfig->randomBotMaxLevel;
    maxLevel = std::min(maxLevel, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

    // if lvl sync is enabled, max level is limited by online players lvl
    /*if (sPlayerbotAIConfig->syncLevelWithPlayers)
        maxLevel = std::max(sPlayerbotAIConfig->randomBotMinLevel,
            std::min(playersLevel, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)));*/

    //PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "RandomizeFirst");

    
    uint32 level = urand(sPlayerbotAIConfig->randomBotMinLevel, maxLevel);
    uint32 chance = std::clamp<uint32>(sPlayerbotAIConfig->randomBotMaxLevelChance * 100, 0, 100);
    if (urand(1, 100) <= chance)
        level = maxLevel;

    if (bot->GetClass() == CLASS_DEATH_KNIGHT)
        level = urand(std::max(sPlayerbotAIConfig->randomBotMinLevel, sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)), std::max(sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL), maxLevel));

    if (sPlayerbotAIConfig->disableRandomLevels)
    {
        level = bot->GetClass() == CLASS_DEATH_KNIGHT ? std::max(uint32(55), sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : uint32(55);
    }

    SetValue(bot, "level", level);
    BotFactory factory(bot, level);
    factory.Randomize(false);

    uint32 randomTime   = urand(sPlayerbotAIConfig->minRandomBotRandomizeTime, sPlayerbotAIConfig->maxRandomBotRandomizeTime);
    uint32 inworldTime  = urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime);

    PlayerbotsPreparedStatement* stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_UPD_RANDOM_BOTS);
    stmt->setUInt32(0, randomTime);
    stmt->setString(1, "bot_delete");
    stmt->setUInt32(2, bot->GetGUID().GetCounter());
    PlayerbotsDatabase.Execute(stmt);

    stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_UPD_RANDOM_BOTS);
    stmt->setUInt32(0, inworldTime);
    stmt->setString(1, "logout");
    stmt->setUInt32(2, bot->GetGUID().GetCounter());
    PlayerbotsDatabase.Execute(stmt);

    // teleport to a random inn for bot level
    if (GET_PLAYERBOT_AI(bot))
        GET_PLAYERBOT_AI(bot)->Reset(true);

    if (bot->GetGroup())
        bot->RemoveFromGroup();

    //if (pmo)
        //pmo->finish();

    //for (int i = 1; i < MAX_INVTYPE; ++i)
    //{
        //sRandomItemMgr->FindBestItemForLevelAndEquip(bot, InventoryType(i));
    //}

    RandomTeleportForLevel(bot);
}

void RandomPlayerbotMgr::Randomize(Player* bot)
{
    if (GET_PLAYERBOT_AI(bot))
            GET_PLAYERBOT_AI(bot)->Reset(true);

    uint8 level = GetValue(bot, "level");
    BotFactory factory(bot, level);
    factory.Randomize(false);

    // rez / clean cd / etc
    Refresh(bot);
    RandomTeleportForLevel(bot);

    // -- safe
    ScheduleRandomize(bot->GetGUID().GetCounter(), 62208000);
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return;

    if (bot->isDead())
    {
        bot->ResurrectPlayer(1.0f);
        bot->SpawnCorpseBones();
        botAI->ResetStrategies();
    }

    // if (sPlayerbotAIConfig->disableRandomLevels)
    //     return;

    if (bot->InBattleground())
        return;

    TC_LOG_INFO("playerbots", "Refreshing bot #%u <%s>", bot->GetGUID().GetCounter(), bot->GetName().c_str());
    PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "Refresh");

    botAI->Reset();
    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SetFullHealth();
    //bot->SetPvP(true);
    BotFactory factory(bot, bot->GetLevel());
    factory.Refresh();

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));

    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 500 * sqrt(urand(1, bot->GetLevel() * 5)));

    //if (bot->GetGroup())
        //bot->RemoveFromGroup();

    if (pmo)
        pmo->finish();
}

bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
    if (bot && GET_PLAYERBOT_AI(bot))
    {
        if (GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        {
            return false;
        }
    }
    if (bot)
    {
        return IsRandomBot(bot->GetGUID().GetCounter());
    }

    return false;
}

bool RandomPlayerbotMgr::IsRandomBot(ObjectGuid::LowType bot)
{
    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(bot);
    if (!sPlayerbotAIConfig->IsInRandomAccountList(sCharacterCache->GetCharacterAccountIdByGuid(guid)))
        return false;
    if (std::find(_currentBots.begin(), _currentBots.end(), bot) != _currentBots.end())
        return true;
    return false;
}

void RandomPlayerbotMgr::GetBots()
{
    if (!_currentBots.empty())
        return;

    PlayerbotsPreparedStatement* stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_SEL_RANDOM_BOTS_BY_OWNER_AND_EVENT);
    stmt->setUInt32(0, 0);
    stmt->setString(1, "add");
    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (PreparedQueryResult result = PlayerbotsDatabase.Query(stmt))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 bot = fields[0].GetUInt32();
            if (GetEventValue(bot, "add"))
                _currentBots.push_back(bot);

            if (_currentBots.size() >= maxAllowedBotCount)
                break;
        } while (result->NextRow());
    }
}

uint32 RandomPlayerbotMgr::GetEventValue(uint32 bot, std::string const event)
{
    // load all events at once on first event load
    if (_eventCache[bot].empty())
    {
        PlayerbotsPreparedStatement* stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_SEL_RANDOM_BOTS_BY_OWNER_AND_BOT);
        stmt->setUInt32(0, 0);
        stmt->setUInt64(1, bot);
        if (PreparedQueryResult result = PlayerbotsDatabase.Query(stmt))
        {
            do
            {
                Field* fields = result->Fetch();
                std::string const eventName = fields[0].GetString();

                CachedEvent e;
                e.value = fields[1].GetUInt32();
                e.lastChangeTime = fields[2].GetUInt32();
                e.validIn = fields[3].GetUInt32();
                e.data = fields[4].GetString();
                _eventCache[bot][eventName] = std::move(e);
            } while (result->NextRow());
        }
    }

    CachedEvent& e = _eventCache[bot][event];
    if ((time(0) - e.lastChangeTime) >= e.validIn && event != "specNo" && event != "specLink")
        e.value = 0;

    return e.value;
}

std::string const RandomPlayerbotMgr::GetEventData(uint32 bot, std::string const event)
{
    std::string data = "";
    if (GetEventValue(bot, event))
    {
        CachedEvent e = _eventCache[bot][event];
        data = e.data;
    }

    return data;
}

uint32 RandomPlayerbotMgr::SetEventValue(uint32 bot, std::string const event, uint32 value, uint32 validIn,
    std::string const data)
{
    PlayerbotsDatabaseTransaction trans = PlayerbotsDatabase.BeginTransaction();

    PlayerbotsPreparedStatement* stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_DEL_RANDOM_BOTS_BY_OWNER_AND_EVENT);
    stmt->setUInt32(0, 0);
    stmt->setUInt64(1, bot);
    stmt->setString(2, event);
    trans->Append(stmt);

    if (value)
    {
        stmt = PlayerbotsDatabase.GetPreparedStatement(PLAYERBOTS_INS_RANDOM_BOTS);
        stmt->setUInt32(0, 0);
        stmt->setUInt64(1, bot);
        auto time = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        stmt->setInt64(2, time);
        stmt->setUInt32(3, validIn);
        stmt->setString(4, event);
        stmt->setUInt32(5, value);
        if (data != "")
        {
            stmt->setString(6, data);
        }
        else
        {
            stmt->setString(6, "");
        }
        trans->Append(stmt);
    }

    PlayerbotsDatabase.CommitTransaction(trans);

    CachedEvent e(value, (uint32)time(nullptr), validIn, data);
    _eventCache[bot][event] = std::move(e);
    return value;
}

uint32 RandomPlayerbotMgr::GetValue(uint32 bot, std::string const type) { return GetEventValue(bot, type); }

uint32 RandomPlayerbotMgr::GetValue(Player* bot, std::string const type)
{
    return GetValue(bot->GetGUID().GetCounter(), type);
}

std::string const RandomPlayerbotMgr::GetData(uint32 bot, std::string const type) { return GetEventData(bot, type); }

void RandomPlayerbotMgr::SetValue(uint32 bot, std::string const type, uint32 value, std::string const data)
{
    SetEventValue(bot, type, value, sPlayerbotAIConfig->maxRandomBotInWorldTime, data);
}

void RandomPlayerbotMgr::SetValue(Player* bot, std::string const type, uint32 value, std::string const data)
{
    SetValue(bot->GetGUID().GetCounter(), type, value, data);
}

void RandomPlayerbotMgr::OnPlayerLogout(Player* player)
{
    if (!player)
        return;

    DisablePlayerBot(player->GetGUID());

    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI && player == botAI->GetMaster())
        {
            botAI->SetMaster(nullptr);
            if (!bot->InBattleground())
            {
                //botAI->ResetStrategies();
            }
        }
    }

    std::vector<Player*>::iterator i = std::find(_players.begin(), _players.end(), player);
    if (i != _players.end())
        _players.erase(i);
}

void RandomPlayerbotMgr::OnBotLoginInternal(Player* const bot)
{
    auto maxAllowed = sRandomPlayerbotMgr->GetMaxAllowedBotCount();
    TC_LOG_INFO("playerbots", "%u/%u Bot %s logged in - Active spec tab: %u Spec: %u", playerBots.size(), maxAllowed, bot->GetName().c_str(), (uint32)bot->GetActiveSpec(), (uint32)bot->GetSpecialization());

    // If this player has been created recently and is not assign horde / alliance as pandaren
    if (bot->GetRace() == RACE_PANDAREN_NEUTRAL)
    {
        // Allow player to get fully logged
        bot->AddDelayedEvent(5000, [bot]()
        {
            static const uint32 JOIN_THE_ALLIANCE = 1;
            static const uint32 JOIN_THE_HORDE = 0;

            WorldPacket packet(CMSG_SELECT_FACTION);
            packet << (std::rand() % 2 ? JOIN_THE_HORDE : JOIN_THE_ALLIANCE);

            WorldSession* session = bot->GetSession();
            session->HandleSelectFactionOpcode(packet);

            TC_LOG_INFO("playerbots", "%s Assigned to faction: %s", bot->GetName().c_str(), (bot->GetTeamId() ? "Alliance" : "Horde"));
        });        
    }
    if (bot->GetRace() == RACE_GOBLIN && bot->GetLevel() == 1)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(25265);
            if (it == bot->getRewardedQuests().end())
            {
                bot->CompleteQuest(25265, true, true);
                bot->CompleteQuest(25266, true, true);
                bot->CompleteQuest(25267, true, true);
                bot->CompleteQuest(25268, true, true);
            }
        });
    }
    if (bot->GetRace() == RACE_WORGEN && bot->GetLevel() == 1)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(26706);
            if (it == bot->getRewardedQuests().end())
            {
                bot->CompleteQuest(26706, true, true);
                bot->CompleteQuest(14434, true, true);
            }
        });
    }

    if (bot->GetClass() == CLASS_DEATH_KNIGHT && bot->GetLevel() == 55)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(13188);
            if (it == bot->getRewardedQuests().end() && bot->GetTeam() == Team::ALLIANCE)
                bot->CompleteQuest(13188, true, true);
            it = bot->getRewardedQuests().find(13189);
            if (it == bot->getRewardedQuests().end() && bot->GetTeam() == Team::HORDE)
                bot->CompleteQuest(13189, true, true);
        });
    }

    /*if (sPlayerbotAIConfig->randomBotFixedLevel)
    {
        bot->SetPlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }
    else
    {
        bot->RemovePlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }*/
}

void RandomPlayerbotMgr::OnPlayerLogin(Player* player)
{
    uint32 botsNearby = 0;

    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        if (player == bot /* || GET_PLAYERBOT_AI(player)*/)  // TEST
            continue;

        Cell playerCell(player->GetPositionX(), player->GetPositionY());
        Cell botCell(bot->GetPositionX(), bot->GetPositionY());

        // if (playerCell == botCell)
        // botsNearby++;

        Group* group = bot->GetGroup();
        if (!group)
            continue;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
            if (botAI && member == player && (!botAI->GetMaster() || GET_PLAYERBOT_AI(botAI->GetMaster())))
            {
                if (!bot->InBattleground())
                {
                    botAI->SetMaster(player);
                    botAI->ResetStrategies();
                    botAI->TellMaster("Hello");
                }

                break;
            }
        }
    }

    if (botsNearby > 100 && false)
    {
        WorldPosition botPos(player);

        if (!player->GetFactionTemplateEntry())
        {
            botPos.GetReachableRandomPointOnGround(player, sPlayerbotAIConfig->reactDistance * 2, true);
            player->TeleportTo(botPos);
        }
        else
        {
            /*std::vector<TravelDestination*> dests = sTravelMgr->getRpgTravelDestinations(player, true, true, 200000.0f);

            do
            {
                RpgTravelDestination* dest = (RpgTravelDestination*)dests[urand(0, dests.size() - 1)];
                CreatureTemplate const* cInfo = dest->GetCreatureTemplate();
                if (!cInfo)
                    continue;

                FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->faction);
                ReputationRank reaction = Unit::GetFactionReactionTo(player->GetFactionTemplateEntry(), factionEntry);

                if (reaction > REP_NEUTRAL && dest->nearestPoint(&botPos)->m_mapId == player->GetMapId())
                {
                    botPos = *dest->nearestPoint(&botPos);
                    break;
                }
            } while (true);*/
        }

        //player->TeleportTo(botPos);
    }

    if (!IsRandomBot(player))
    {
        _players.push_back(player);
        TC_LOG_DEBUG("playerbots", "Including non-random bot player %s into random bot update", player->GetName().c_str());
    }
}

void RandomPlayerbotMgr::OnPlayerLoginError(uint32 bot)
{
    SetEventValue(bot, "add", 0, 0);
    _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), bot), _currentBots.end());
}

Player* RandomPlayerbotMgr::GetRandomPlayer()
{
    if (_players.empty())
        return nullptr;

    uint32 index = std::rand() % _players.size();
    return _players[index];
}

void RandomPlayerbotMgr::PrepareAddclassCache()
{
    if (sPlayerbotAIConfig->randomBotAccounts.empty())
    {
        TC_LOG_WARN("playerbots", "Unable to prepare add class cache as accounts are empty");
        return;
    }
    int32 maxAccountId = sPlayerbotAIConfig->randomBotAccounts.back();
    int32 minIdx = 0;
    int32 minAccountId = sPlayerbotAIConfig->randomBotAccounts[minIdx];
    if (minAccountId < 0)
    {
        TC_LOG_ERROR("playerbots", "No available account for add class!");
    }
    int32 collected = 0;
    for (uint8 claz = CLASS_WARRIOR; claz <= CLASS_DRUID; claz++)
    {
        if (claz == 10)
            continue;
        QueryResult results = CharacterDatabase.PQuery(
            "SELECT guid, race FROM characters "
            "WHERE account >= %u AND account <= %u AND class = '%u' AND online = 0 AND "
            "guid NOT IN ( SELECT guid FROM guild_member ) "
            "ORDER BY account DESC",
            minAccountId, maxAccountId, claz);
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                ObjectGuid guid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
                uint32 race = fields[1].GetUInt32();
                bool isAlliance = race == 1 || race == 3 || race == 4 || race == 7 || race == 11;
                AddclassCache()[GetTeamClassIdx(isAlliance, claz)].push_back(guid);
                collected++;
            } while (results->NextRow());
        }
    }
    TC_LOG_INFO("playerbots", ">> %u characters collected for addclass command.", collected);
}

void RandomPlayerbotMgr::PrepareTeleportCache()
{
    uint32 zone_count = 0, farm_spot_count = 0, city_count = 0;
    QueryResult results = PlayerbotsDatabase.PQuery("SELECT ZoneId, ZoneTyp, MinLevel, MaxLevel, TeamsDisabled, MapId, MinPlayers, MaxPlayers FROM playerbot_farming_zone");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 zoneId = fields[0].GetUInt32();
            uint32 zoneType = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            uint32 mapid = fields[5].GetUInt32();
            uint32 minPlayer = fields[6].GetUInt32();
            uint32 maxPlayer = fields[7].GetUInt32();

            farm_zone new_zone
            {
                zoneId,     /* ZoneId */
                zoneType,   /* ZoneType */
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                disabled,   /* TeamDisabled */
                mapid,      /* MapId */
                minPlayer,  /* MinPlayer */
                maxPlayer   /* MaxPlayer */
            };

            _farm_cache_data.emplace_back(std::move(new_zone));
            zone_count++;
        } while (results->NextRow());
    }

    results = PlayerbotsDatabase.PQuery("SELECT ZoneId, Id, MinLevel, MaxLevel, TeamsDisabled, X, Y, Z, Radius FROM playerbot_farming_spot");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 zoneid = fields[0].GetUInt32();
            uint32 id = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            float x = fields[5].GetFloat();
            float y = fields[6].GetFloat();
            float z = fields[7].GetFloat();
            uint32 radius = fields[8].GetUInt32();

            farm_spot new_spot
            {
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                0,          /* MapId */
                x,          /* X */
                y,          /* Y */
                z           /* Z */
            };

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == zoneid)
                {
                    new_spot.map_id = zone_data.map_id;
                    zone_data.spots.emplace_back(std::move(new_spot));
                    break;
                }
            }

            farm_spot_count++;
        } while (results->NextRow());
    }

    results = PlayerbotsDatabase.PQuery("SELECT ZoneId, CityId, MinLevel, MaxLevel, TeamsDisabled, X, Y, Z, Radius FROM playerbot_city");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 ZoneId = fields[0].GetUInt32();
            uint32 city_id = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            float x = fields[5].GetFloat();
            float y = fields[6].GetFloat();
            float z = fields[7].GetFloat();
            uint32 radius = fields[8].GetUInt32();
            uint32 map_id = UINT32_MAX;

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == ZoneId)
                {
                    map_id = zone_data.map_id;
                    break;
                }
            }

            if (map_id == UINT32_MAX)
            {
                TC_LOG_ERROR("server.loading", "City {%u} in ZoneId {%u} is not found in zone cache", city_id, ZoneId);
                continue;
            }

            city new_city
            {
                ZoneId,     /* ZoneId */
                city_id,    /* CityId */
                map_id,     /* map_id */
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                disabled,   /* TeamDisabled */
                x,          /* X */
                y,          /* Y */
                z,          /* Z */
            };

            _city_cache_data.emplace_back(std::move(new_city));
            city_count++;
        } while (results->NextRow());
    }
    results = PlayerbotsDatabase.PQuery("SELECT Zone, PathId, PointId, X, Y, Z FROM playerbot_zone_path");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 ZoneId = fields[0].GetUInt32();
            uint32 PathId = fields[1].GetUInt32();
            uint32 PointId = fields[2].GetUInt32();
            float x = fields[3].GetFloat();
            float y = fields[4].GetFloat();
            float z = fields[5].GetFloat();

            farm_spot new_path
            {
                0,          /* MinLevel */
                0,          /* MaxLevel */
                0,          /* MapId */
                x,          /* X */
                y,          /* Y */
                z           /* Z */
            };

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == ZoneId)
                {
                    new_path.map_id = zone_data.map_id;
                    new_path.min_level = zone_data.min_level;
                    new_path.max_level = zone_data.max_level;
                    zone_data.spots.emplace_back(std::move(new_path));
                    break;
                }
            }
        } while (results->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u cache zone for %u farm spot", zone_count, farm_spot_count);
    TC_LOG_INFO("server.loading", ">> Loaded %u city zone", city_count);
}

void RandomPlayerbotMgr::RandomTeleportForLevel(Player* bot)
{
    if (!bot) return;

    // Teleport to city
    if (bot->GetLevel() >= 10 && urand(0, 100) < 0.25 * 100)
    {
        if (const auto city_data = GetCityForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(city_data->map_id, 0);

            bot->TeleportTo(city_data->map_id, city_data->x, city_data->y, city_data->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to City: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), city_data->map_id, city_data->x, city_data->y, city_data->z);
        }
        else if (const auto farm_spot = GetFarmZoneForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(farm_spot->map_id, 0);

            bot->TeleportTo(farm_spot->map_id, farm_spot->x, farm_spot->y, farm_spot->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to FarmSpot: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), farm_spot->map_id, farm_spot->x, farm_spot->y, farm_spot->z);
        }
    }
    else
    {
        if (const auto farm_zone = GetFarmZoneForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(farm_zone->map_id, 0);

            bot->TeleportTo(farm_zone->map_id, farm_zone->x, farm_zone->y, farm_zone->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to FarmZone: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), farm_zone->map_id, farm_zone->x, farm_zone->y, farm_zone->z);
        }
    }
}

const RandomPlayerbotMgr::city* RandomPlayerbotMgr::GetCityForPlayer(Player* player)
{
    if (_city_cache_data.empty()) return nullptr;

    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(_city_cache_data.begin(), _city_cache_data.end(), gen);

    for (const auto& city_data : _city_cache_data)
    {
        if (player->GetLevel() < city_data.min_level || player->GetLevel() > city_data.max_level || city_data.team_disabled == player->GetTeam())
            continue;
        return &city_data;
    }

    return nullptr;
}

const RandomPlayerbotMgr::farm_spot* RandomPlayerbotMgr::GetFarmZoneForPlayer(Player* player)
{
    if (_farm_cache_data.empty()) return nullptr;

    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(_farm_cache_data.begin(), _farm_cache_data.end(), gen);

    for (auto& zone : _farm_cache_data)
    {
        if (player->GetLevel() < zone.min_level || player->GetLevel() > zone.max_level || zone.team_disabled == player->GetTeam())
            continue;

        if (zone.max_player != 0)
        {
            uint32 playercount = 0;
            for (const auto& _internal_player : _players)
            {
                if (_internal_player && _internal_player->IsInWorld() && !_internal_player->IsBeingTeleported() && _internal_player->GetZoneId() == zone.zone_id && _internal_player != player)
                    ++playercount;
            }

            for (PlayerBotMap::const_iterator itr = GetPlayerBotsBegin(); itr != GetPlayerBotsEnd(); ++itr)
            {
                Player* const bot = itr->second;
                if (!bot || bot == player || !bot->IsInWorld() || bot->IsBeingTeleported()
                    || bot->GetZoneId() != zone.zone_id) continue;

                ++playercount;
            }

            if (zone.max_player <= playercount)
            {
                TC_LOG_DEBUG("playerbots", "Zone {%u} has too many players for: %s {%u} : %u/%u", zone.zone_id, player->GetName().c_str(), player->GetLevel(), playercount, zone.max_player);

                continue;
            }
        }

        auto& spots = zone.spots;
        std::shuffle(spots.begin(), spots.end(), gen);
        for (auto& spot : spots)
        {
            if (player->GetLevel() >= spot.min_level && player->GetLevel() <= spot.max_level)
            {
                // -- 
                TC_LOG_DEBUG("playerbots", "Farm spot found for : %s - level: %u", player->GetName().c_str(), player->GetLevel());
                return &spot;
            }
        }
    }

    TC_LOG_WARN("playerbots", "No valid zone farm found for %s level: %u", player->GetName().c_str(), player->GetLevel());
    return nullptr;
}