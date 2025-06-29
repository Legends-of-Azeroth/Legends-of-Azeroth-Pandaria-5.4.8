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

#include "RatedPvp.h"
#include "ArenaTeam.h"
#include "Group.h"
#include "BattlegroundQueue.h"
#include "Player.h"
#include "Formulas.h"
#include "Config.h"
#include "AchievementMgr.h"
#include "Language.h"
#include "Random.h"
#include <filesystem>
#include <fstream>

void RatedPvpMgr::LoadFromDB()
{
    OnConfigLoad();

    CheckSeasonEnd();

    LoadPvpInfoStore(sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));

    for (auto&& itr : m_store)
        RecalcuateRank(itr);
}

RatedPvpInfo* RatedPvpMgr::GetOrCreateInfo(RatedPvpSlot slot, ObjectGuid guid)
{
    ASSERT(slot < PVP_SLOT_MAX);
    auto it = m_store[slot].find(guid);
    if (it != m_store[slot].end())
        return it->second.get();

    auto& info = m_store[slot][guid];
    info.reset(new RatedPvpInfo{});
    info->Guid = guid;
    info->Slot = slot;
    info->MatchmakerRating = sWorld->getIntConfig(CONFIG_ARENA_START_MATCHMAKER_RATING);
    info->Rating = sWorld->getIntConfig(CONFIG_ARENA_START_PERSONAL_RATING);
    info->Rank = m_store[slot].size();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_RATED_PVP_INFO);
    stmt->setUInt32(0, info->Guid);
    stmt->setUInt8(1, info->Slot);
    stmt->setUInt16(2, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
    stmt->setUInt16(3, info->Rating);
    stmt->setUInt16(4, info->MatchmakerRating);
    CharacterDatabase.Execute(stmt);
    return info.get();
}

void RatedPvpMgr::UpdateCap()
{
    for (auto&& store : m_store)
    {
        for (auto&& itr : store)
        {
            itr.second->LastWeekBest = itr.second->WeekBest;
            itr.second->WeekBest = 0;
            itr.second->WeekWins = 0;
            itr.second->WeekGames = 0;
        }
    }

    sWorld->setWorldState(WS_ARENA_SEASON_WEEK, sWorld->getWorldState(WS_ARENA_SEASON_WEEK) + 1);
}

void RatedPvpMgr::OnConfigLoad()
{
    m_bonusRewards.resize(PVP_SLOT_MAX);

    std::vector<std::string> const brackets{ "2v2", "3v3", "5v5", "RBG" };

    for (uint32 slot = PVP_SLOT_ARENA_2v2; slot < PVP_SLOT_MAX; ++slot)
    {
        auto& reward = m_bonusRewards[slot];

        reward.GamesCount   = sConfigMgr->GetIntDefault(("Arena.BonusReward.GamesCount." + brackets[slot]).c_str(), 0);
        reward.ItemId       = sConfigMgr->GetIntDefault(("Arena.BonusReward.Item." + brackets[slot]).c_str(), 0);
        reward.MailTitle    = sConfigMgr->GetStringDefault(("Arena.BonusReward.MailTitle." + brackets[slot]).c_str(), "");
        reward.MailText     = sConfigMgr->GetStringDefault(("Arena.BonusReward.MailText." + brackets[slot]).c_str(), "");

        if (reward.GamesCount && !sObjectMgr->GetItemTemplate(reward.ItemId))
        {
            TC_LOG_ERROR("server.loading", "RatedPvpMgr::OnConfigLoad invalid item id %u", reward.ItemId);
            reward.GamesCount = 0;
        }
    }
}

void RatedPvpMgr::RecalcuateRank(RatedPvpSlot slot)
{
    ASSERT(slot < PVP_SLOT_MAX);
    auto copy = std::make_shared<RatedPvpInfoMap>(m_store[slot]);
    // TaskMgr::Async([=]
    // {
    //     RecalcuateRank(*copy);
    // });
}

void RatedPvpMgr::SaveToDB(RatedPvpInfo const* info)
{
    uint32 i = 0;
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_RATED_PVP_INFO);
    stmt->setUInt16(i++, info->Rank);
    stmt->setUInt16(i++, info->Rating);
    stmt->setUInt16(i++, info->MatchmakerRating);
    stmt->setUInt16(i++, info->WeekBest);
    stmt->setUInt16(i++, info->WeekGames);
    stmt->setUInt16(i++, info->WeekWins);
    stmt->setUInt16(i++, info->SeasonBest);
    stmt->setUInt16(i++, info->SeasonGames);
    stmt->setUInt16(i++, info->SeasonWins);
    stmt->setUInt32(i++, info->WinStreak);
    stmt->setUInt32(i++, info->Guid.GetCounter());
    stmt->setUInt8 (i++, info->Slot);
    stmt->setUInt16(i++, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
    CharacterDatabase.Execute(stmt);
}

void RatedPvpMgr::SendBonusRewardIfNeed(RatedPvpInfo* info)
{
    auto& reward = Instance()->m_bonusRewards[info->Slot];

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    std::string name = "Unknown";
    sObjectMgr->GetPlayerNameByGUID(info->Guid, name);
    std::string body = Format(reward.MailText, name.c_str());

    if (reward.GamesCount && info->WeekGames % reward.GamesCount == 0)
    {
        if (Item* item = Item::CreateItem(reward.ItemId, 1))
        {
            MailDraft draft{ reward.MailTitle, body };
            item->SaveToDB(trans);
            draft.AddItem(item);
            draft.SendMailTo(trans, MailReceiver{ ObjectAccessor::FindConnectedPlayer(info->Guid), info->Guid.GetCounter() },
                MailSender{ MAIL_NORMAL, info->Guid.GetCounter(), MAIL_STATIONERY_GM });
        }
    }

    if (sWorld->getBoolConfig(CONFIG_ICORE_RICH_PVP_REWARD_ENABLED))
    {
        if (info->WeekGames % sWorld->getIntConfig(CONFIG_ICORE_RICH_PVP_REWARD_GAMES) == 0 && roll_chance_i(sWorld->getIntConfig(CONFIG_ICORE_RICH_PVP_REWARD_CHANCE)))
        {
            if (Item* item = Item::CreateItem(sWorld->getIntConfig(CONFIG_ICORE_RICH_PVP_REWARD), 1))
            {
                MailDraft draft{ sObjectMgr->GetTrinityString(LANG_BG_REWARD_TITLE_RICH, LOCALE_ruRU), body };
                item->SaveToDB(trans);
                draft.AddItem(item);
                draft.SendMailTo(trans, MailReceiver{ ObjectAccessor::FindConnectedPlayer(info->Guid), info->Guid.GetCounter() },
                    MailSender{ MAIL_NORMAL, info->Guid.GetCounter(), MAIL_STATIONERY_GM });
            }
        }
    }
    CharacterDatabase.CommitTransaction(trans);

    if (!sWorld->getBoolConfig(CONFIG_RBG_REWARDS_FOR_ARENA_ENABLED))
        return;

    Player* player = ObjectAccessor::FindConnectedPlayer(info->Guid);
    if (!player)
        return;
    //if (info->Slot == PVP_SLOT_RATED_BG)
    //    return;

    // https://ezwow.org/topic/114846-pvp-horizon/
    bool IsHorde = player->GetTeam() == HORDE;
    if (info->Rating >= 1100 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5345 : 5330));
    if (info->Rating >= 1200 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5346 : 5331));
    if (info->Rating >= 1300 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5347 : 5332));
    if (info->Rating >= 1400 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5348 : 5333));
    if (info->Rating >= 1500 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5349 : 5334));
    if (info->Rating >= 1600 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5350 : 5335));
    if (info->Rating >= 1700 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5351 : 5336));
    if (info->Rating >= 1800 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5352 : 5337));
    if (info->Rating >= 1900 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5338 : 5359));
    if (info->Rating >= 2000 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5353 : 5339));
    if (info->Rating >= 2100 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5354 : 5340));
    if (info->Rating >= 2200 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5355 : 5341));
    if (info->Rating >= 2300 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5342 : 5357));
    if (info->Rating >= 2400 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 5356 : 5343));
    if (info->Rating >= 2500 && info->Slot == PVP_SLOT_RATED_BG)
        player->CompletedAchievement(sAchievementMgr->GetAchievement(IsHorde ? 6941 : 6942));
}

struct Reward
{
    uint32 LowerPlace = 0;
    uint32 UpperPlace = 0;
    float Percent = 0.0f;
    std::vector<CharTitlesEntry const*> Titles;
    std::vector<AchievementEntry const*> Achievs;
    std::vector<ItemTemplate const*> Items;
};

static CharTitlesEntry const* GetTitle(uint32 id)
{
    CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(id);
    if (!title)
        TC_LOG_ERROR("server.loading", "Title %u not found", id);
    return title;
}

static AchievementEntry const* GetAchievement(uint32 id)
{
    AchievementEntry const* ach = sAchievementStore.LookupEntry(id);
    if (!ach)
        TC_LOG_ERROR("server.loading", "Achievement %u not found", id);
    return ach;
}

static ItemTemplate const* GetItem(uint32 id)
{
    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(id);
    if (!proto)
        TC_LOG_ERROR("server.loading", "Item template %u not found", id);
    return proto;
}

static uint32 ToInt(char const* str)
{
    return std::stoul(str, nullptr, 10);
}

static Reward LoadReward(uint32 rank, ArenaType type)
{
    Reward r;
    std::string name = "Arena.SeasonReward.Rank" + std::to_string(rank) + ".Titles";
    std::string config = sConfigMgr->GetStringDefault(name.c_str(), "");
    if (config.empty())
        TC_LOG_ERROR("server.loading", "Achievement %s not specified in config", name.c_str());

    std::size_t sp = config.find(";");
    if (sp != config.npos)
    {
        Tokenizer tok{ config, ';' };
        if (tok.size() != 3)
            TC_LOG_ERROR("server.loading", "Invalid config format for %s", name.c_str());
        switch (type)
        {
        case ARENA_TEAM_2v2: config = tok[0]; break;
        case ARENA_TEAM_3v3: config = tok[1]; break;
        case ARENA_TEAM_5v5: config = tok[2]; break;
        default: break;
        }
        TrimString(config);
    }

    for (auto&& it : Tokenizer{ config, ' ' })
        r.Titles.push_back(GetTitle(ToInt(it)));

    name = "Arena.SeasonReward.Rank" + std::to_string(rank) + ".Achievements";
    config = sConfigMgr->GetStringDefault(name.c_str(), "");
    if (config.empty())
        TC_LOG_ERROR("server.loading", "Invalid config format for %s", name.c_str());

    sp = config.find(";");
    if (sp != config.npos)
    {
        Tokenizer tok{ config, ';' };
        if (tok.size() != 3)
            TC_LOG_ERROR("server.loading", "Invalid config format for %s", name.c_str());
        switch (type)
        {
        case ARENA_TEAM_2v2: config = tok[0]; break;
        case ARENA_TEAM_3v3: config = tok[1]; break;
        case ARENA_TEAM_5v5: config = tok[2]; break;
        default: break;
        }
        TrimString(config);
    }

    for (auto&& it : Tokenizer{ config, ' ' })
        r.Achievs.push_back(GetAchievement(ToInt(it)));

    name = "Arena.SeasonReward.Rank" + std::to_string(rank) + ".Items";
    config = sConfigMgr->GetStringDefault(name.c_str(), "");

    sp = config.find(";");
    if (sp != config.npos)
    {
        Tokenizer tok{ config, ';' };
        if (tok.size() != 3)
            TC_LOG_ERROR("server.loading", "Invalid config format for %s", name.c_str());
        switch (type)
        {
        case ARENA_TEAM_2v2: config = tok[0]; break;
        case ARENA_TEAM_3v3: config = tok[1]; break;
        case ARENA_TEAM_5v5: config = tok[2]; break;
        default: break;
        }
        TrimString(config);
    }

    for (auto&& it : Tokenizer{ config, ' ' })
        r.Items.push_back(GetItem(ToInt(it)));

    return r;
}

static std::vector<Reward> GetRewards(ArenaType type)
{
    std::vector<Reward> rewards;
    Reward r = LoadReward(1, type);
    r.LowerPlace = 1;
    r.UpperPlace = type == ARENA_TYPE_5v5 ? 1 : 2;
    rewards.push_back(r);

    r = LoadReward(2, type);
    r.LowerPlace = type == ARENA_TYPE_5v5 ? 2 : 3;
    r.UpperPlace = type == ARENA_TYPE_5v5 ? 2 : 4;
    rewards.push_back(r);

    r = LoadReward(3, type);
    r.LowerPlace = type == ARENA_TYPE_5v5 ? 3 : 5;
    r.UpperPlace = type == ARENA_TYPE_5v5 ? 3 : 6;
    rewards.push_back(r);

    r = LoadReward(4, type);
    r.Percent = 10.0f;
    rewards.push_back(r);

    r = LoadReward(5, type);
    r.Percent = 15.0f;
    rewards.push_back(r);

    r = LoadReward(6, type);
    r.Percent = 20.0f;
    rewards.push_back(r);

    return rewards;
}

static std::ostream& operator<<(std::ostream& stream, Reward const& reward)
{
    if (reward.LowerPlace)
    {
        if (reward.LowerPlace == reward.UpperPlace)
            stream << reward.LowerPlace << " place";
        else
            stream << reward.LowerPlace << "-" << reward.UpperPlace << " places";
    }
    else
    {
        stream << reward.Percent << "% of total";
    }
    stream << std::endl << "Titles:";
    for (auto&& it : reward.Titles)
    {
        std::string str = it->name;
        size_t sp = str.find('%');
        stream << " " << str.substr(0, sp) << "(" << it->ID << ")";
    }
    stream << std::endl << "Achievements:";
    for (auto&& it : reward.Achievs)
        stream << " " << it->Name << "(" << it->ID << ")";

    stream << std::endl << "Items:";
    for (auto&& it : reward.Items)
        stream << " " << it->ItemId;

    stream << std::endl;
    return stream;
}

static uint32 const GROUPS_SIZE = 6;

static void CalculateRanks(std::size_t size, std::vector<uint32>& groups, std::vector<Reward> const& rewards)
{
    ASSERT(rewards.size() == GROUPS_SIZE);
    groups.resize(GROUPS_SIZE);
    groups[0] = rewards[0].UpperPlace;
    groups[1] = rewards[1].UpperPlace;
    groups[2] = rewards[2].UpperPlace;
    for (int32 i = 3; i < GROUPS_SIZE; ++i)
    {
        uint32 n = uint32(std::ceil(size * rewards[i].Percent / 100.0f));
        // I not sure that this is normal, but web-part has such algorithm.
        uint32 prev = groups[i - 1];
        n = std::max(prev + 1, n);
        groups[i] = n < size ? n : 0;
    }
}

static void PrintResults(std::ostream& out, std::vector<RatedPvpInfo*>& teams, ArenaType type)
{
    std::string str = "Arena.SeasonReward.RatingThreshold." + std::to_string(type) + "x" + std::to_string(type);
    int32 threshold = sConfigMgr->GetIntDefault(str.c_str(), -1);
    out << "============== " << type << "x" << type << "==============" << std::endl;

    if (threshold < 0)
    {
        out << "DISABLED." << std::endl;
        return;
    }

    uint32 slot = ArenaTeam::GetSlotByType(type);
    QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM rated_pvp_info WHERE rating > %i AND season = %u AND slot = %u", threshold, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID), slot);
    size_t size = result ? (*result)[0].GetUInt32() : 0;
    if (teams.empty() || !size)
    {
        out << "Neither team satisfies the conditions of " << threshold << " rating" << std::endl;
        return;
    }

    std::vector<Reward> rewards;

    rewards = GetRewards(type);

    ASSERT(rewards.size() == GROUPS_SIZE);

    std::vector<uint32> groups;
    CalculateRanks(size, groups, rewards);

    std::string error;
    auto it = teams.begin();
    for (uint32 i = 0; i < GROUPS_SIZE; ++i)
    {
        out << rewards[i] << std::endl;
        while (it != teams.end() && (*it)->Rank <= groups[i])
        {
            RatedPvpInfo* team = *it;
            ++it;
            CharacterNameData const* data = sWorld->GetCharacterNameData(team->Guid);
            out << team->Rank << ") " << (data ? data->m_name : std::string("???")) << " (" << team->Guid << ")" << std::endl;
        }

        out << std::endl;
    }
}

void RatedPvpMgr::PrintSeasonResults(std::ostream& out)
{
    std::vector<RatedPvpInfo*> teams2v2;
    std::vector<RatedPvpInfo*> teams3v3;
    std::vector<RatedPvpInfo*> teams5v5;
    GetSeasonPrizewinners(teams2v2, teams3v3, teams5v5);

    PrintResults(out, teams2v2, ARENA_TYPE_2v2);
    PrintResults(out, teams3v3, ARENA_TYPE_3v3);
    PrintResults(out, teams5v5, ARENA_TYPE_5v5);
}

static bool IsHigherRank(RatedPvpInfo const* lhs, RatedPvpInfo const* rhs)
{
    if (lhs->Rating > rhs->Rating)
        return true;
    if (lhs->Rating < rhs->Rating)
        return false;
    if (lhs->SeasonGames < rhs->SeasonGames)
        return true;
    if (lhs->SeasonGames > rhs->SeasonGames)
        return false;
    if (lhs->SeasonGames && rhs->SeasonGames)
    {
        if (lhs->SeasonWins * 100 / lhs->SeasonGames > rhs->SeasonWins * 100 / rhs->SeasonGames)
            return true;
        if (lhs->SeasonWins * 100 / lhs->SeasonGames < rhs->SeasonWins * 100 / rhs->SeasonGames)
            return false;
    }
    if (lhs->MatchmakerRating > rhs->MatchmakerRating)
        return true;
    if (lhs->MatchmakerRating < rhs->MatchmakerRating)
        return false;
    if (lhs->SeasonBest > rhs->SeasonBest)
        return true;
    if (lhs->SeasonBest < rhs->SeasonBest)
        return false;
    return lhs->Guid > rhs->Guid; // derp
}

void RatedPvpMgr::LoadPvpInfoStore(uint32 season)
{
    for (auto&& store : m_store)
        store.clear();

    uint32 count = 0;
    uint32 start = getMSTime();
    //                                                       0       1       2        3           4                    5            6             7              8              9              10            11              12
    QueryResult result = CharacterDatabase.PQuery("SELECT r.guid, r.slot, r.rank, r.rating, r.matchmaker_rating, r.week_best, r.week_games, r.week_wins, r.season_best, r.season_games, r.season_wins, r.last_week_best, r.win_streak FROM rated_pvp_info r JOIN characters c ON r.guid = c.guid WHERE r.season = %u AND c.`deleteDate` IS NULL ORDER BY r.slot", season);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint8 slot = fields[1].GetInt8();
            ObjectGuid guid(HighGuid::Player, fields[0].GetUInt32());
            auto& info = m_store[slot][guid];
            info.reset(new RatedPvpInfo{});
            info->Guid              = guid;
            info->Slot              = slot;
            info->Rank              = fields[2].GetUInt32();
            info->Rating            = fields[3].GetUInt32();
            info->MatchmakerRating  = fields[4].GetUInt32();
            info->WeekBest          = fields[5].GetUInt32();
            info->WeekGames         = fields[6].GetUInt32();
            info->WeekWins          = fields[7].GetUInt32();
            info->SeasonBest        = fields[8].GetUInt32();
            info->SeasonGames       = fields[9].GetUInt32();
            info->SeasonWins        = fields[10].GetUInt32();
            info->LastWeekBest      = fields[11].GetUInt32();
            info->WinStreak         = fields[12].GetUInt32();
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", "Loaded %u rated PvP info in %u ms", count, GetMSTimeDiffToNow(start));
}

void RatedPvpMgr::RecalcuateRank(RatedPvpInfoMap const& map)
{
    for (auto&& itr1 : map)
    {
        RatedPvpInfo* info1 = itr1.second.get();
        uint32 rank = 1;
        for (auto&& itr2 : map)
        {
            RatedPvpInfo* info2 = itr2.second.get();
            if (info1 == info2)
                continue;
            if (IsHigherRank(info2, info1))
                ++rank;
        }

        if (rank != info1->Rank)
        {
            auto task = [=]
            {
                info1->Rank = rank;
                RatedPvpMgr::SaveToDB(info1); // RatedPvpInfo never be deleted, so we can just use pointer
            };

            // if (std::this_thread::get_id() != sWorld->GetThreadId())
            //     TaskMgr::Default()->ScheduleInvocation(task);
            // else
                task();
        }
    }
}

void RatedPvpMgr::GetSeasonPrizewinners(std::vector<RatedPvpInfo*>& two, std::vector<RatedPvpInfo*>& three, std::vector<RatedPvpInfo*>& five)
{
    int32 threshold2x2 = sConfigMgr->GetIntDefault("Arena.SeasonReward.RatingThreshold.2x2", -1);
    int32 threshold3x3 = sConfigMgr->GetIntDefault("Arena.SeasonReward.RatingThreshold.3x3", -1);
    int32 threshold5x5 = sConfigMgr->GetIntDefault("Arena.SeasonReward.RatingThreshold.5x5", -1);

    for (auto&& itr : m_store[PVP_SLOT_ARENA_2v2])
        if (threshold2x2 >= 0 && itr.second->Rating > uint32(threshold2x2))
            two.push_back(itr.second.get());

    for (auto&& itr : m_store[PVP_SLOT_ARENA_3v3])
        if (threshold3x3 >= 0 && itr.second->Rating > uint32(threshold3x3))
            three.push_back(itr.second.get());

    for (auto&& itr : m_store[PVP_SLOT_ARENA_5v5])
        if (threshold5x5 >= 0 && itr.second->Rating > uint32(threshold5x5))
            five.push_back(itr.second.get());

    auto sortfn = [](RatedPvpInfo const* lhs, RatedPvpInfo const* rhs) -> bool
    {
        return lhs->Rank < rhs->Rank;
    };

    std::sort(two.begin(),   two.end(),   sortfn);
    std::sort(three.begin(), three.end(), sortfn);
    std::sort(five.begin(),  five.end(),  sortfn);
}

bool RatedPvpMgr::RewardTeams(std::ostream& log, RewardMap& map, std::vector<RatedPvpInfo*> const& teams, ArenaType type, uint32 season)
{
    std::string str = "Arena.SeasonReward.RatingThreshold." + std::to_string(type) + "x" + std::to_string(type);
    int32 threshold = sConfigMgr->GetIntDefault(str.c_str(), -1);
    log << "============== " << type << "x" << type << "==============" << std::endl;
    if (threshold < 0)
    {
        log << "DISABLED." << std::endl;
        return true;
    }

    uint32 slot = ArenaTeam::GetSlotByType(type);
    QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM rated_pvp_info WHERE rating > %i AND season = %u AND slot = %u", threshold, season, slot);
    size_t size = result ? (*result)[0].GetUInt32() : 0;

    if (teams.empty() || !size)
    {
        log << "Neither team satisfies the conditions of " << threshold << " rating" << std::endl;
        return true;
    }

    std::vector<Reward> rewards;

    rewards = GetRewards(type);

    ASSERT(rewards.size() == GROUPS_SIZE);

    std::vector<uint32> groups;
    CalculateRanks(size, groups, rewards);

    std::string error;

    auto itr = teams.begin();
    for (uint32 i = 0; i < GROUPS_SIZE; ++i)
    {
        log << std::endl << rewards[i] << std::endl;
        while (itr != teams.end() && (*itr)->Rank <= groups[i])
        {
            RatedPvpInfo* team = *itr;
            ++itr;
            CharacterNameData const* data = sWorld->GetCharacterNameData(team->Guid);
            log << team->Rank << ") " << (data ? data->m_name : std::string("???")) << " (" << team->Guid << ")" << std::endl;
            auto& playerRewards = map[team->Guid];
            for (auto&& it : rewards[i].Titles)
                playerRewards.Titles.insert(it);
            for (auto&& it : rewards[i].Achievs)
                playerRewards.Achievs.insert(it);
            for (auto&& it : rewards[i].Items)
                playerRewards.Items.push_back(it);
        }
        log << std::endl;
    }
    return true;
}

bool RatedPvpMgr::SendRewards(std::ostream& log, RewardMap& map, CharacterDatabaseTransaction trans)
{
    std::string str;
    std::string error;
    std::string name;
    std::ostringstream strbuf;
    std::vector<std::unique_ptr<Item>> items;

    for (auto&& playerIter : map)
    {
        uint32 guid = playerIter.first;
        QueryResult result = CharacterDatabase.PQuery("SELECT knownTitles, name FROM characters WHERE guid = %u", guid);
        if (!result)
        {
            log << guid << " not found in DB" << std::endl;
            continue;
        }

        str = (*result)[0].GetString();
        name = (*result)[1].GetString();
        Tokenizer t(str, ' ');
        int32 const size = KNOWN_TITLES_SIZE * 2;
        if (t.size() != size)
        {
            log << "Invalid array size for player " << guid << std::endl;
            return false;
        }

        uint32 playerTitles[size];
        for (int32 n = 0; n < size; ++n)
            playerTitles[n] = strtoul(t[n], nullptr, 10);

        for (auto&& it : playerIter.second.Titles)
        {
            uint32 pos = it->bit_index / 32;
            uint32 flag = 1 << (it->bit_index % 32);
            if (pos >= t.size())
            {
                log << "Shit happens! Guid: " << guid << std::endl;
                return false;
            }
            playerTitles[pos] |= flag;
        }

        strbuf.str("");
        for (auto&& it : playerTitles)
            strbuf << it << ' ';
        trans->PAppend("UPDATE characters SET knownTitles = '%s' WHERE guid = %u", strbuf.str().c_str(), guid);

        if (!playerIter.second.Achievs.empty())
        {
            strbuf.str("");
            strbuf << "INSERT IGNORE INTO character_achievement (guid, achievement, date) VALUES ";
            for (auto&& it : playerIter.second.Achievs)
                strbuf << "(" << guid << "," << it->ID << "," << time(nullptr) << "),";
            str = strbuf.str();
            str.pop_back();
            trans->PAppend(str.c_str());
        }

        MailDraft mail("PvP награда за завершение сезона арены",
            Format("Уважаемый %s, поздравляем вас завершением сезона арены, с вашими успехами на PvP поприще и вручаем заслуженную награду.", name.c_str()));
        for (auto&& it : playerIter.second.Items)
        {
            Item* item = Item::CreateItem(it->ItemId, 1);
            if (!item)
            {
                log << "Can't create item: " << it->ItemId << std::endl;
                return false;
            }
            items.emplace_back(item);
            item->SaveToDB(trans);
            mail.AddItem(item);
        }

        mail.SendMailTo(trans, MailReceiver(guid), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));

        for (auto&& item : items)
            item.release();
        items.clear();
    }
    return true;
}

void RatedPvpMgr::SeasonCleanup(CharacterDatabaseTransaction trans)
{
    if (!sConfigMgr->GetBoolDefault("Arena.SeasonReward.Cleanup.Enabled", true))
        return;

    std::set<uint32> gladiators;
    std::ostringstream sstr;
    std::string str = sConfigMgr->GetStringDefault("Arena.SeasonReward.Cleanup.KeepWithAchievements", "2091");
    if (!str.empty())
    {
        for (auto&& it : Tokenizer{ str, ' ' })
            sstr << GetAchievement(ToInt(it))->ID << ',';     // Ensures what achievement is real
        str = sstr.str();
        str.pop_back();
        QueryResult result = CharacterDatabase.PQuery("SELECT guid FROM character_achievement WHERE achievement IN (%s)", str.c_str());
        if (result)
        {
            do
            {
                gladiators.insert((*result)[0].GetUInt32());
            } while (result->NextRow());
        }
    }

    sstr.str("");

    str = sConfigMgr->GetStringDefault("Arena.SeasonReward.Cleanup.Achievements", "2093 2092 2090");
    if (!str.empty())
    {
        for (auto&& it : Tokenizer{ str, ' ' })
            sstr << GetAchievement(ToInt(it))->ID << ",";   // Ensures what achievement is real

        str = sstr.str();
        str.pop_back();
        sstr.str("");

        sstr << "DELETE FROM character_achievement WHERE achievement IN (" << str << ") ";

        if (!gladiators.empty())
        {
            sstr << "AND guid NOT IN (";
            for (auto&& it : gladiators)
                sstr << it << ",";
            sstr.seekp(-1, sstr.cur);
            sstr << ")";
        }

        trans->Append(sstr.str().c_str());
    }

    std::vector<CharTitlesEntry const*> titles;

    str = sConfigMgr->GetStringDefault("Arena.SeasonReward.Cleanup.Titles", "44 43 45");
    if (!str.empty())
    {
        sstr.str("");
        for (auto&& it : Tokenizer{ str, ' ' })
            titles.push_back(GetTitle(ToInt(it)));
    }
    QueryResult result = CharacterDatabase.PQuery("SELECT guid, knownTitles FROM characters");
    int32 count = 0;
    if (!titles.empty() && result)
    {
        do
        {
            std::string str = (*result)[1].GetString();
            uint32 guid = (*result)[0].GetUInt32();
            if (gladiators.find(guid) != gladiators.end())
                continue;

            Tokenizer t(str, ' ');
            int32 const size = KNOWN_TITLES_SIZE * 2;
            if (t.size() != size)   // Players who never was logged in
                continue;

            uint32 playerTitles[size];
            for (int32 i = 0; i < size; ++i)
                playerTitles[i] = strtoul(t[i], nullptr, 10);

            int32 old = count;
            for (auto&& title : titles)
            {
                uint32 pos = title->bit_index / 32;
                uint32 flag = 1 << (title->bit_index % 32);
                if (pos >= t.size())
                    continue;

                if (flag & playerTitles[pos])
                {
                    ++count;
                    playerTitles[pos] &= ~flag;
                    TC_LOG_INFO("server", "Title %u was removed from player: %u", title->ID, guid);
                }
            }

            if (count != old)
            {
                sstr.str("");
                for (auto&& it : playerTitles)
                    sstr << it << ' ';
                trans->PAppend("UPDATE characters SET knownTitles = '%s' WHERE guid = %u", sstr.str().c_str(), guid);
            }
        } while (result->NextRow());
    }
}

void RatedPvpMgr::CheckSeasonEnd()
{
    uint32 curseason = sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID);
    if (!curseason)
        return;

    if (sWorld->getWorldState(WS_ARENA_SEASON_ID) >= sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID))
        return;

    RewardSeason();
}

void RatedPvpMgr::RewardSeason()
{
    uint32 curseason = sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID);
    uint32 oldseason = sWorld->getWorldState(WS_ARENA_SEASON_ID);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    SeasonCleanup(trans);
    LoadPvpInfoStore(oldseason);

    std::vector<RatedPvpInfo*> teams2v2;
    std::vector<RatedPvpInfo*> teams3v3;
    std::vector<RatedPvpInfo*> teams5v5;
    GetSeasonPrizewinners(teams2v2, teams3v3, teams5v5);

    std::string dir = sConfigMgr->GetStringDefault("LogsDir", ".") + "/arena";
    std::filesystem::path path{ dir };
    std::error_code c;
    if (!std::filesystem::create_directories(path, c) && c.value() != 0)
    {
        TC_LOG_ERROR("server", "RatedPvpMgr::CheckSeasonEnd Couldn't create directory %s, errno %u", dir.c_str(), c.value());
        return;
    }

    std::string p = dir + "/rewarding_season_" + std::to_string(oldseason) + ".log";

    path = std::filesystem::path{ p };
    std::fstream out(path.c_str(), std::ios::out | std::ios::trunc);
    if (!out)
    {
        TC_LOG_ERROR("server", "Coudn't open file, errno %u", errno);
        return;
    }

    p = dir + "/rewarding_season_" + std::to_string(oldseason) + "_misc.log";
    path = std::filesystem::path{ p };
    std::ofstream misclog{ path.c_str() };
    if (!misclog.is_open())
    {
        TC_LOG_ERROR("server", "RatedPvpMgr::CheckSeasonEnd Couldn't create file %s, errno %u", p.c_str(), c.value());
        return;
    }

    RewardMap map;

    if (!RewardTeams(out, map, teams2v2, ARENA_TYPE_2v2, oldseason) ||
        !RewardTeams(out, map, teams3v3, ARENA_TYPE_3v3, oldseason) ||
        !RewardTeams(out, map, teams5v5, ARENA_TYPE_5v5, oldseason) ||
        !SendRewards(out, map, trans))
    {
        TC_LOG_ERROR("server", "Error occured. Watch to the logfile for details.");
        return;
    }

    /*
    QueryResult result = CharacterDatabase.PQuery("SELECT `guid`, `currency`, `total_count` FROM `character_currency` WHERE `currency` IN(%u,%u) ORDER BY `guid` ASC, `currency` DESC", CURRENCY_TYPE_CONQUEST_POINTS, CURRENCY_TYPE_HONOR_POINTS);
    if (result)
    {
        struct Currency
        {
            uint32 Honor = 0;
            uint32 Conquest = 0;
        };

        std::map<uint32, Currency> currencyMap;
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            auto& currency = currencyMap[guid];
            CurrencyTypes type = CurrencyTypes(fields[1].GetUInt32());
            if (type == CURRENCY_TYPE_HONOR_POINTS)
                currency.Honor = fields[2].GetUInt32();
            else
                currency.Conquest = fields[2].GetUInt32();
        } while (result->NextRow());

        for (auto&& itr : currencyMap)
        {
            uint32 honor = 0;
            uint32 money = 0;

            if (itr.second.Conquest > 0)
            {
                if (itr.second.Honor + itr.second.Conquest <= 4000 * CURRENCY_PRECISION)
                {
                    honor = itr.second.Conquest;
                }
                else
                {
                    honor = (4000 * CURRENCY_PRECISION) - itr.second.Honor;
                    money = (itr.second.Conquest - honor) / CURRENCY_PRECISION * 3500;
                }
            }
            misclog << "Player " << itr.first << ":" << "conquest (" << itr.second.Conquest << "), honor (" << itr.second.Honor << ") -> honor (" << itr.second.Honor + honor << "), money (" << money << ")" << std::endl;
            if (honor)
                trans->PAppend("UPDATE character_currency SET total_count = %u WHERE guid = %u AND currency = %u", itr.second.Honor + honor, itr.first, CURRENCY_TYPE_HONOR_POINTS);
            if (money)
                trans->PAppend("UPDATE characters SET money = money + %u WHERE guid = %u", money, itr.first);
        }
    }
    */

    QueryResult result = CharacterDatabase.PQuery("SELECT itemEntry, guid, owner_guid FROM item_instance WHERE flags & %u", ITEM_FLAG_REFUNDABLE);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            if (auto const* itemTemplate = sObjectMgr->GetItemTemplate(fields[0].GetUInt32()))
            {
                if (itemTemplate->IsPvPItem())
                {
                    misclog << "Removing ITEM_FLAG_REFUNDABLE for item " << fields[1].GetUInt32() << " of player " << fields[2].GetUInt32() << std::endl;
                    trans->PAppend("UPDATE item_instance SET flags = flags & ~%u WHERE guid = %u", ITEM_FLAG_REFUNDABLE, fields[1].GetUInt32());
                }
            }
        } while (result->NextRow());
    }

    trans->PAppend("UPDATE character_currency SET season_count = 0, week_count = 0 WHERE currency = %u", CURRENCY_TYPE_HONOR_POINTS);
    trans->PAppend("UPDATE character_currency SET season_count = 0, week_count = 0 WHERE currency = %u", CURRENCY_TYPE_CONQUEST_POINTS);
    trans->PAppend("UPDATE character_currency SET season_count = 0, week_count = 0 WHERE currency = %u", CURRENCY_TYPE_CONQUEST_META_RANDOG_BG);
    trans->PAppend("UPDATE character_currency SET season_count = 0, week_count = 0 WHERE currency = %u", CURRENCY_TYPE_CONQUEST_META_RATED_BG);
    trans->PAppend("UPDATE character_currency SET season_count = 0, week_count = 0 WHERE currency = %u", CURRENCY_TYPE_CONQUEST_META_ARENA);
    sWorld->setWorldState(WS_ARENA_SEASON_ID, curseason);
    sWorld->setWorldState(WS_ARENA_SEASON_WEEK, 0);
    CharacterDatabase.CommitTransaction(trans);
    // CharacterDatabase.Async(trans)->ContinueWith([](bool res)
    // {
    //     if (!res)
    //         TC_LOG_ERROR("server", "RatedPvpMgr::RewardSeason - transaction failed!");
    // });;
}
