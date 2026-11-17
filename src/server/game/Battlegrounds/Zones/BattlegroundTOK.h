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

#ifndef BATTLEGROUND_KT_H
#define BATTLEGROUND_KT_H

#include "Battleground.h"
#include "TaskScheduler.h"

enum BG_TOK_NPC
{
    BG_SM_NPC_POWERBALL         = 29265
};

enum BG_TOK_Misc
{
    BG_TOK_MAX_TEAM_SCORE       = 1600,
    BG_TOK_ORB_POINTS_MAX       = 1600,
    BG_TOK_MAX_ORBS             = 4,
    BG_TOK_MAX_BUFFS            = 2,
};

enum BG_TOK_Objects
{
    BG_TOK_OBJECT_A_DOOR        = 0,
    BG_TOK_OBJECT_H_DOOR,
    BG_TOK_OBJECT_A_COLLISION,
    BG_TOK_OBJECT_H_COLLISION,
    BG_TOK_OBJECT_ORB_1,
    BG_TOK_OBJECT_ORB_2,
    BG_TOK_OBJECT_ORB_3,
    BG_TOK_OBJECT_ORB_4,
    BG_TOK_OBJECT_BUFF_NORTH,
    BG_TOK_OBJECT_BUFF_SOUTH,
    BG_TOK_OBJECT_MAX
};

enum BG_TOK_Creatures
{
    BG_TOK_CREATURE_ORB_AURA_1  = 0,
    BG_TOK_CREATURE_ORB_AURA_2  = 1,
    BG_TOK_CREATURE_ORB_AURA_3  = 2,
    BG_TOK_CREATURE_ORB_AURA_4  = 3,

    BG_TOK_CREATURE_SPIRIT_1    = 4,
    BG_TOK_CREATURE_SPIRIT_2    = 5,

    BG_TOK_CREATURE_MAX         = 6
};

enum BG_TOK_Objets_Entry
{
    BG_TOK_OBJECT_DOOR_ENTRY    = 213172,
    BG_TOK_OBJECT_COL_ENTRY     = 215787,

    BG_TOK_OBJECT_ORB_1_ENTRY   = 212091,
    BG_TOK_OBJECT_ORB_2_ENTRY   = 212092,
    BG_TOK_OBJECT_ORB_3_ENTRY   = 212093,
    BG_TOK_OBJECT_ORB_4_ENTRY   = 212094
};

enum BG_TOK_Sound
{
    BG_TOK_SOUND_ORB_PLACED      = 8232,
    BG_TOK_SOUND_A_ORB_PICKED_UP = 8174,
    BG_TOK_SOUND_H_ORB_PICKED_UP = 8174,
    BG_TOK_SOUND_ORB_RESPAWNED   = 8232
};

enum BG_TOK_SpellId
{
    BG_TOK_SPELL_ORB_PICKED_UP_1    = 121164,   // Blue
    BG_TOK_SPELL_ORB_PICKED_UP_2    = 121175,   // Purple
    BG_TOK_SPELL_ORB_PICKED_UP_3    = 121176,   // Green
    BG_TOK_SPELL_ORB_PICKED_UP_4    = 121177,   // Orange

    BG_TOK_SPELL_ORB_AURA_1         = 121217,   // Blue (but called "yellow")
    BG_TOK_SPELL_ORB_AURA_2         = 121219,   // Purple
    BG_TOK_SPELL_ORB_AURA_3         = 121220,   // Green
    BG_TOK_SPELL_ORB_AURA_4         = 121221,   // Orange

    BG_TOK_ALLIANCE_INSIGNIA        = 131527,
    BG_TOK_HORDE_INSIGNIA           = 131528,
    BG_TOK_10_VICTORY_POINTS        = 112910,
};

enum BG_TOK_WorldStates
{
    BG_TOK_ICON_A                   = 6308,
    BG_TOK_ICON_H                   = 6307,
    BG_TOK_ORB_POINTS_A             = 6303,
    BG_TOK_ORB_POINTS_H             = 6304,
    BG_TOK_ORB_STATE                = 6309,
    BG_TOK_BLUE_ORB_POS             = 6714,
    BG_TOK_PURPLE_ORB_POS           = 6715,
    BG_TOK_GREEN_ORB_POS            = 6716,
    BG_TOK_ORANGE_ORB_POS           = 6717,
    BG_TOK_TIME_ENABLED             = 4247,
    BG_TOK_TIME_REMAINING           = 4248,
};

enum BG_TOK_Graveyards
{
    BG_TOK_GRAVEYARD_RECTANGLEA1    = 3552,
    BG_TOK_GRAVEYARD_RECTANGLEA2    = 4058,
    GB_TOK_GRAVEYARD_RECTANGLEH1    = 3553,
    BG_TOK_GRAVEYARD_RECTANGLEH2    = 4057
};

enum BG_TOK_Zone
{
    BG_TOK_ZONE_OUT     = 0,
    BG_TOK_ZONE_IN      = 1,
    BG_TOK_ZONE_MILLDE  = 2,
};

const static uint32 OrbsWorldStates[4] =
{
    BG_TOK_BLUE_ORB_POS,
    BG_TOK_PURPLE_ORB_POS,
    BG_TOK_GREEN_ORB_POS,
    BG_TOK_ORANGE_ORB_POS,
};

struct BattlegroundTOKScore : public BattlegroundScore
{
    uint32 OrbHandles = 0;
    uint32 Score = 0;
};

class BattlegroundTOK : public Battleground
{
    uint32 const m_orbAuras[BG_TOK_MAX_ORBS]
    {
        BG_TOK_SPELL_ORB_AURA_1,
        BG_TOK_SPELL_ORB_AURA_2,
        BG_TOK_SPELL_ORB_AURA_3,
        BG_TOK_SPELL_ORB_AURA_4
    };

    uint32 const m_orbSpells[BG_TOK_MAX_ORBS]
    {
        BG_TOK_SPELL_ORB_PICKED_UP_1,
        BG_TOK_SPELL_ORB_PICKED_UP_2,
        BG_TOK_SPELL_ORB_PICKED_UP_3,
        BG_TOK_SPELL_ORB_PICKED_UP_4
    };

    float const m_orbPositions[BG_TOK_MAX_ORBS][4]
    {
        { 1716.83f, 1249.93f, 13.5706f, 4.71397f }, // Blue
        { 1850.26f, 1416.77f, 13.5709f, 1.56061f }, // Purple
        { 1716.78f, 1416.64f, 13.5709f, 1.57239f }, // Green
        { 1850.29f, 1250.31f, 13.5708f, 4.70848f }, // Orange
    };

    float const m_doorPositions[2][4]
    {
        { 1783.84f, 1100.66f, 20.60f, 1.625020f },
        { 1780.15f, 1570.22f, 24.59f, 4.711630f }
    };

    float const m_spiritPositions[BG_TOK_MAX_ORBS][4]
    {
        { 1892.61f, 1151.69f, 14.7160f, 2.523528f },
        { 1672.40f, 1524.10f, 16.7387f, 6.032206f },
    };

    float const m_buffPositions[BG_TOK_MAX_BUFFS][4]
    {
        { 1856.007935f, 1333.637392f, 10.554197f, 3.147876f },    // Berserk
        { 1710.443604f, 1333.375000f, 10.554073f, 0.002354f },    // Berserk
    };

public:
    BattlegroundTOK();

    void Reset() override { m_scheduler = std::make_shared<TaskScheduler>(); }
    void PostUpdateImpl(uint32 diff) override;
    void AddPlayer(Player* player) override;
    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;

    void EventPlayerClickedOnFlag(Player* source, GameObject* targetObj) override;

    void RemovePlayer(Player*, ObjectGuid, uint32) override;
    void HandleAreaTrigger(Player* source, uint32 trigger, bool entered) override;
    void HandleKillPlayer(Player* player, Player* killer) override;
    bool SetupBattleground() override;
    WorldSafeLocsEntry const* GetClosestGraveYard(Player* player) override;
    void EndBattleground(uint32 winner);

    void UpdateOrbState(Team team, uint32 value);
    void UpdateTeamScore(Team team);
    void UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor = true) override;
    void FillInitialWorldStates(WorldStateBuilder& builder) override;

    bool HasOrb(uint64 guid)
    {
        return GetOrbIndex(guid) != BG_TOK_MAX_ORBS;
    }

    uint8 GetOrbIndex(uint64 guid)
    {
        for (uint8 i = 0; i < BG_TOK_MAX_ORBS; ++i)
            if (m_orbKeepers[i].Guid == guid)
                return i;
        return BG_TOK_MAX_ORBS;
    }

    std::vector<ObjectGuid> GetOrbPickersGUID() const
    {
        std::vector<ObjectGuid> guids;
        for (auto&& itr : m_orbKeepers)
            guids.push_back(itr.Guid);
        return guids;
    }

    void TakeOrb(Player* player, uint8 index);
    void ReleaseOrb(Player* player);

    /* Scorekeeping */
    uint32 GetTeamScore(Team team) const            { return m_TeamScores[GetTeamIndexByTeamId(team)]; }
    void AddPoint(Team team, uint32 Points = 1)     { m_TeamScores[GetTeamIndexByTeamId(team)] += Points; }
    void SetTeamPoint(Team team, uint32 Points = 0) { m_TeamScores[GetTeamIndexByTeamId(team)] = Points; }
    void RemovePoint(Team team, uint32 Points = 1)  { m_TeamScores[GetTeamIndexByTeamId(team)] -= Points; }

    void UpdateScore(Player* player);
    void AddScore(Player* player, uint32 value);

    uint32 GetRBGLoserReward(uint32 team) const override;

private:
    struct OrbKeeperInfo
    {
        ObjectGuid Guid = ObjectGuid::Empty;
        TimeValue AchievementTimer;
    };

    OrbKeeperInfo m_orbKeepers[BG_TOK_MAX_ORBS];

    std::map<ObjectGuid, BG_TOK_Zone> m_playersZone;

    uint32 m_reputationCapture;
    uint32 m_honorWinKills;
    uint32 m_honorEndKills;
    Team   m_lastCapturedOrbTeam{ TEAM_OTHER };
    std::shared_ptr<TaskScheduler> m_scheduler;
    uint32 m_minutesRemain{ 25 };
};

#endif
