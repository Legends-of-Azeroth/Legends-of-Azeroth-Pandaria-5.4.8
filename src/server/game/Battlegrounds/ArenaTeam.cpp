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

#include "ArenaTeam.h"
#include "Group.h"
#include "BattlegroundQueue.h"
#include "Player.h"

ArenaTeam::ArenaTeam(RatedPvpSlot slot, Group* group)
{
    for (auto&& member : *group)
    {
        auto arenaInfo = RatedPvpMgr::Instance()->GetOrCreateInfo(slot, member->GetGUID());
        m_rating += arenaInfo->Rating;
        m_matchmakerRating += arenaInfo->MatchmakerRating;
        m_members.push_back(arenaInfo);
    }

    if (m_members.size())
    {
        m_slot = RatedPvpSlot(m_members.front()->Slot);
        m_rating /= m_members.size();
        m_matchmakerRating /= m_members.size();
    }
}

ArenaTeam::ArenaTeam(GroupQueueInfo* group)
{
    m_slot = RatedPvpSlot(ArenaTeam::GetSlotByType(group->ArenaType));
    for (auto&& member : group->Players)
    {
        auto arenaInfo = RatedPvpMgr::Instance()->GetOrCreateInfo(m_slot, member.first);
        m_members.push_back(arenaInfo);
    }

    m_rating = group->ArenaTeamRating;
    m_matchmakerRating = group->ArenaMatchmakerRating;
}

ArenaTeam::ArenaTeam(SoloGroup* group)
{
    m_slot = PVP_SLOT_ARENA_5v5;
    for (auto&& itr : group->Current)
    {
        for (auto&& member : itr.Info->Players)
        {
            auto arenaInfo = RatedPvpMgr::Instance()->GetOrCreateInfo(m_slot, member.first);
            m_rating += arenaInfo->Rating;
            m_matchmakerRating += arenaInfo->MatchmakerRating;
            m_members.push_back(arenaInfo);
        }
    }

    if (m_members.size())
    {
        m_rating /= m_members.size();
        m_matchmakerRating /= m_members.size();
    }
}

RatedPvpInfo* ArenaTeam::GetMember(uint64 guid) const
{
    for (auto&& memberInfo : m_members)
        if (memberInfo->Guid == guid)
            return memberInfo;
    return nullptr;
}

uint8 ArenaTeam::GetSlotByType(uint32 type)
{
    switch (type)
    {
        case ARENA_TEAM_2v2: return PVP_SLOT_ARENA_2v2;
        case ARENA_TEAM_3v3: return PVP_SLOT_ARENA_3v3;
        case ARENA_TEAM_5v5: return PVP_SLOT_ARENA_5v5;
        case 0: return PVP_SLOT_RATED_BG;
        default:
            break;
    }
    TC_LOG_ERROR("bg.arena", "FATAL: Unknown arena team type {} for some arena team", type);
    return 0xFF;
}

uint8 ArenaTeam::GetTypeBySlot(uint8 slot)
{
    switch (slot)
    {
        case PVP_SLOT_ARENA_2v2: return ARENA_TEAM_2v2;
        case PVP_SLOT_ARENA_3v3: return ARENA_TEAM_3v3;
        case PVP_SLOT_ARENA_5v5: return ARENA_TEAM_5v5;
        default:
            break;
    }
    TC_LOG_ERROR("bg.arena", "FATAL: Unknown arena team slot {} for some arena team", slot);
    return 0xFF;
}

float ArenaTeam::GetChanceAgainst(uint32 ownRating, uint32 opponentRating)
{
    // Returns the chance to win against a team with the given rating, used in the rating adjustment calculation
    // ELO system
    return 1.0f / (1.0f + exp(log(10.0f) * (float)((float)opponentRating - (float)ownRating) / 400.0f));
}

int32 ArenaTeam::GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;
    float mod = won_mod - chance;

    // Work in progress:
    /*
    // This is a simulation, as there is not much info on how it really works
    float confidence_mod = min(1.0f - fabs(mod), 0.5f);

    // Apply confidence factor to the mod:
    mod *= confidence_factor

    // And only after that update the new confidence factor
    confidence_factor -= ((confidence_factor - 1.0f) * confidence_mod) / confidence_factor;
    */

    // Real rating modification
    mod *= 32.0f;

    return (int32)ceil(mod);
}

int32 ArenaTeam::GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float confidence_factor*/)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;

    // Calculate the rating modification
    float mod;

    /// @todo Replace this hack with using the confidence factor (limiting the factor to 2.0f)
    if (won && ownRating < 1300)
    {
        if (ownRating < 1000)
            mod = 96.0f * (won_mod - chance);
        else
            mod = (32.0f + (24.0f * (1300.0f - float(ownRating)) / 300.0f)) * (won_mod - chance);
    }
    else
        mod = 32.0f * (won_mod - chance);

    return (int32)ceil(mod);
}

int32 ArenaTeam::WonAgainst(uint32 ownMMRating, uint32 opponentMMRating, int32& ratingChange)
{
    // Called when the team has won
    // Change in Matchmaker rating
    int32 mod = GetMatchmakerRatingMod(ownMMRating, opponentMMRating, true);

    // Change in Team Rating
    ratingChange = GetRatingMod(m_rating, opponentMMRating, true);
    m_matchmakerRating += mod;

    // Return the rating change, used to display it on the results screen
    return mod;
}

int32 ArenaTeam::LostAgainst(uint32 ownMMRating, uint32 opponentMMRating, int32& ratingChange)
{
    // Called when the team has lost
    // Change in Matchmaker Rating
    int32 mod = GetMatchmakerRatingMod(ownMMRating, opponentMMRating, false);

    // Change in Team Rating
    ratingChange = GetRatingMod(m_rating, opponentMMRating, false);
    m_matchmakerRating += mod;

    // return the rating change, used to display it on the results screen
    return mod;
}

void ArenaTeam::MemberWon(Player* player, uint32 againstMatchmakerRating, int32 matchmakerRatingChange)
{
    auto memberInfo = GetMember(player->GetGUID());

    int32 mod = GetRatingMod(memberInfo->Rating, againstMatchmakerRating, true);

    if (int32(memberInfo->Rating) + mod < 0)
        mod = -int32(memberInfo->Rating);

    if (sWorld->getBoolConfig(CONFIG_ARENA_WIN_STREAK_MOD_ENABLED) && memberInfo->WinStreak)
    {
        mod *= (1.0f + ((float)memberInfo->WinStreak / 10.0f));
        matchmakerRatingChange *= (1.0f + ((float)memberInfo->WinStreak / 10.0f));

        int32 limit = sWorld->getIntConfig(CONFIG_ARENA_WIN_STREAK_MOD_LIMIT);
        if (mod > limit)
            mod = limit;
        if (matchmakerRatingChange > limit)
            matchmakerRatingChange = limit;
    }

    memberInfo->Rating += mod;

    if (player && GetSlot() != PVP_SLOT_RATED_BG)
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, memberInfo->Rating, GetTypeBySlot(memberInfo->Slot));

    if (memberInfo->Rating > memberInfo->SeasonBest)
        memberInfo->SeasonBest = memberInfo->Rating;

    if (memberInfo->Rating > memberInfo->WeekBest)
        memberInfo->WeekBest = memberInfo->Rating;

    if (int32(memberInfo->MatchmakerRating) + matchmakerRatingChange < 0)
        memberInfo->MatchmakerRating = 0;
    else
        memberInfo->MatchmakerRating += matchmakerRatingChange;

    memberInfo->WeekGames += 1;
    memberInfo->SeasonGames += 1;
    memberInfo->SeasonWins += 1;
    memberInfo->WeekWins += 1;
    if (memberInfo->WinStreak < 10)
        memberInfo->WinStreak += 1;

    m_ratingChanges[memberInfo->Guid] = mod;

    RatedPvpMgr::SendBonusRewardIfNeed(memberInfo);
    RatedPvpMgr::SaveToDB(memberInfo);
}

void ArenaTeam::MemberLost(Player* player, uint32 againstMatchmakerRating, int32 matchmakerRatingChange)
{
    MemberLost(player->GetGUID(), againstMatchmakerRating, matchmakerRatingChange);
}

void ArenaTeam::MemberLost(uint64 guid, uint32 againstMatchmakerRating, int32 matchmakerRatingChange, bool gamePlayed)
{
    auto memberInfo = GetMember(guid);

    int32 mod = GetRatingMod(memberInfo->Rating, againstMatchmakerRating, false);

    if (int32(memberInfo->Rating) + mod < 0)
        mod = -int32(memberInfo->Rating);
    memberInfo->Rating += mod;

    if (int32(memberInfo->MatchmakerRating) + matchmakerRatingChange < 0)
        memberInfo->MatchmakerRating = 0;
    else
        memberInfo->MatchmakerRating += matchmakerRatingChange;

    if (gamePlayed)
    {
        memberInfo->WeekGames += 1;
        memberInfo->SeasonGames += 1;
        memberInfo->WinStreak = 0;
    }

    m_ratingChanges[memberInfo->Guid] = mod;

    RatedPvpMgr::SendBonusRewardIfNeed(memberInfo);
    RatedPvpMgr::SaveToDB(memberInfo);
}

void ArenaTeam::WinSolo(uint64 guid, int32 against)
{
    auto memberInfo = GetMember(guid);
    int32 ratingChange = ArenaTeam::GetRatingMod(memberInfo->Rating, against, true);
    int32 mmrChange = GetMatchmakerRatingMod(memberInfo->MatchmakerRating, against, true);
    FinishSolo(memberInfo->Guid, ratingChange, mmrChange, true);
}

void ArenaTeam::LoseSolo(uint64 guid, int32 againts)
{
    auto memberInfo = GetMember(guid);
    int32 ratingChange = ArenaTeam::GetRatingMod(memberInfo->Rating, againts, false);
    int32 mmrChange = GetMatchmakerRatingMod(memberInfo->MatchmakerRating, againts, false);
    FinishSolo(memberInfo->Guid, ratingChange, mmrChange, false);
}

void ArenaTeam::FinishSolo(uint64 guid, int32 ratingChange, int32 mmrChange, bool win)
{
    if (IsPlayerHandled(guid))
        return;

    auto memberInfo = GetMember(guid);

    if (win)
    {
        memberInfo->WeekWins += 1;
        memberInfo->SeasonWins += 1;
    }

    if (int32(memberInfo->Rating) + ratingChange < 0)
        ratingChange = -int32(memberInfo->Rating);

    if (sWorld->getBoolConfig(CONFIG_ARENA_WIN_STREAK_MOD_ENABLED) && win && memberInfo->WinStreak)
    {
        ratingChange *= (1.0f + ((float)memberInfo->WinStreak / 10.0f));
        mmrChange *= (1.0f + ((float)memberInfo->WinStreak / 10.0f));

        int32 limit = sWorld->getIntConfig(CONFIG_ARENA_WIN_STREAK_MOD_LIMIT);
        if (ratingChange > limit)
            ratingChange = limit;
        if (mmrChange > limit)
            mmrChange = limit;
    }

    if (int32(memberInfo->MatchmakerRating) + mmrChange < 0)
        mmrChange = -int32(memberInfo->MatchmakerRating);

    memberInfo->WeekGames += 1;
    memberInfo->SeasonGames += 1;
    memberInfo->Rating += ratingChange;
    memberInfo->MatchmakerRating += mmrChange;
    if (win)
    {
        if (memberInfo->WinStreak < 10)
            memberInfo->WinStreak += 1;
    }
    else
        memberInfo->WinStreak = 0;

    if (memberInfo->Rating > memberInfo->WeekBest)
        memberInfo->WeekBest = memberInfo->Rating;

    SetPlayerHandled(guid);

    m_ratingChanges[guid] = ratingChange;

    RatedPvpMgr::SendBonusRewardIfNeed(memberInfo);
    RatedPvpMgr::SaveToDB(memberInfo);
}
