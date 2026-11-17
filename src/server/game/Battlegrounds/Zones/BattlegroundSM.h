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

#ifndef __BATTLEGROUNDSM_H
#define __BATTLEGROUNDSM_H

#include "Language.h"
#include "Battleground.h"

enum BG_SM_WorldStates
{
    SM_MINE_CARTS_DISPLAY            = 6436,
    SM_ALLIANCE_RESOURCES            = 6437,
    SM_HORDE_RESOURCES               = 6438,
    SM_MINE_CART_1                   = 6439,
    SM_MINE_CART_2                   = 6440,
    SM_DISPLAY_ALLIANCE_RESSOURCES   = 6441,
    SM_MINE_CART_3                   = 6442,
    SM_DISPLAY_HORDE_RESSOURCES      = 6443,
    SM_DISPLAY_PROGRESS_BAR          = 6875, // 0 = false, 1 = true
    SM_PROGRESS_BAR_STATUS           = 6876, // 0 = Horde max, 50 = Neutral, 100 = Alliance max
    SM_UNK                           = 6877
};

enum BG_SM_MineCarts
{
    BG_SM_MINE_CART_1 = 0,
    BG_SM_MINE_CART_2 = 1,
    BG_SM_MINE_CART_3 = 2,
};

enum BG_SM_ProgressBarConsts
{
    BG_SM_PROGRESS_BAR_DONT_SHOW  = 0,
    BG_SM_PROGRESS_BAR_SHOW       = 1,
    BG_SM_PROGRESS_BAR_NEUTRAL    = 50
};

enum BG_SM_Sounds
{
    BG_SM_SOUND_MINE_CART_CAPTURED_HORDE     = 8213,
    BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE  = 8173,
    BG_SM_SOUND_NEAR_VICTORY                 = 8456,
    BG_SM_SOUND_MINE_CART_SPAWNED            = 8174
};

enum BG_SM_Spells
{
    BG_SM_CONTROL_VISUAL_ALLIANCE   = 116086,
    BG_SM_CONTROL_VISUAL_HORDE      = 116085,
    BG_SM_CONTROL_VISUAL_NEUTRAL    = 118001,
    BG_SM_TRACK_SWITCH_OPENED       = 120228,
    BG_SM_TRACK_SWITCH_CLOSED       = 120229,
    BG_SM_FEIGN_DEATH_STUN          = 135781, // Prevent from turn moves
    BG_SM_PREVENTION_AURA           = 135846  // Prevent from using multiple times track switches
};

enum SMBattlegroundObjectEntry
{
    BG_SM_WATERFALL_DEPOT   = 212080,
    BG_SM_LAVA_DEPOT        = 212081,
    BG_SM_DIAMOND_DEPOT     = 212082,
    BG_SM_TROLL_DEPOT       = 212083,
    BG_SM_DOOR              = 157819
};

enum SMBattlegroundGaveyards
{
    SM_GRAVEYARD_MAIN_ALLIANCE     = 4062,
    SM_GRAVEYARD_MAIN_HORDE        = 4061
};

enum SMBattlegroundCreaturesTypes
{
    SM_SPIRIT_ALLIANCE        = 0,
    SM_SPIRIT_HORDE           = 1,
    NPC_SM_MINE_CART_1        = 2,
    NPC_SM_MINE_CART_2        = 3,
    NPC_SM_MINE_CART_3        = 4,
    SM_TRACK_SWITCH_EAST      = 5,
    SM_TRACK_SWITCH_NORTH     = 6,

    BG_SM_CREATURES_MAX       = 7
};

enum SMDepots
{
    SM_WATERFALL_DEPOT,
    SM_LAVA_DEPOT,
    SM_DIAMOND_DEPOT,
    SM_TROLL_DEPOT
};

enum SMTracks
{
    SM_EAST_TRACK_SWITCH,
    SM_NORTH_TRACK_SWITCH,
    SM_MAX_TRACK_SWITCH
};

enum SMBattlegroundObjectTypes
{
    BG_SM_OBJECT_DOOR_A_1                   = 0,
    BG_SM_OBJECT_DOOR_H_1                   = 1,
    BG_SM_OBJECT_DOOR_A_2                   = 2,
    BG_SM_OBJECT_DOOR_H_2                   = 3,
    BG_SM_OBJECT_WATERFALL_DEPOT            = 4,
    BG_SM_OBJECT_LAVA_DEPOT                 = 5,
    BG_SM_OBJECT_DIAMOND_DEPOT              = 6,
    BG_SM_OBJECT_TROLL_DEPOT                = 7,
    BG_SM_OBJECT_BERSERKING_BUFF_EAST       = 8,
    BG_SM_OBJECT_BERSERKING_BUFF_WEST       = 9,
    BG_SM_OBJECT_RESTORATION_BUFF_WATERFALL = 10,
    BG_SM_OBJECT_RESTORATION_BUFF_LAVA      = 11,

    BG_SM_OBJECT_MAX                        = 12
};

enum BG_SM_Score
{
    BG_SM_WARNING_NEAR_VICTORY_SCORE    = 1400,
    BG_SM_MAX_TEAM_SCORE                = 1600
};

enum SMBattlegroundMineCartState
{
    SM_MINE_CART_CONTROL_NEUTRAL    = 0,
    SM_MINE_CART_CONTROL_ALLIANCE   = 1,
    SM_MINE_CART_CONTROL_HORDE      = 2
};

enum BG_SM_CreatureIds
{
    NPC_TRACK_SWITCH         = 60283,
    NPC_MINE_CART_1          = 60378,
    NPC_MINE_CART_2          = 60379,
    NPC_MINE_CART_3          = 60380,
};

enum BG_SM_Paths
{
    SM_EAST_PATH,
    SM_NORTH_PATH
};

const float BG_SM_BuffPos[4][4] =
{
    { 749.444153f, 64.338188f,  369.535797f, 6.058259f },  // Berserking buff East
    { 789.979431f, 281.883575f, 355.389984f, 0.652173f },  // Berserking buff West
    { 539.873596f, 396.386749f, 345.722412f, 3.994188f },  // Restoration buff Waterfall
    { 614.202698f, 120.924660f, 294.430908f, 4.241807f }   // Restoration buff Lava
};

const float BG_SM_DepotPos[4][4] =
{
    { 566.950989f, 337.05801f,  347.295013f, 1.559089f }, // Waterfall
    { 619.469971f, 79.719597f,  299.067993f, 1.625564f }, // Lava
    { 895.974426f, 27.210802f,  364.390991f, 3.445790f }, // Diamond
    { 778.444946f, 500.949707f, 359.738983f, 0.737040f }  // Troll
};

const float BG_SM_DoorPos[4][4] =
{
    { 852.0289952f, 158.216003f,  326.761011f, 0.150758f }, // Alliance 1
    { 830.092102f,  143.925507f,  326.5f,      3.130245f }, // Alliance 2
    { 652.177612f,  228.493423f,  326.917480f, 0.163844f }, // Horde 1
    { 635.622925f,  208.220886f,  326.648315f, 3.717332f }  // Horde 2
};

const float BG_SM_TrackPos[2][4] =
{
    { 715.585388f, 101.272034f, 319.994690f, 4.647377f }, // East
    { 847.481689f, 308.032562f, 346.573242f, 0.587086f }  // North
};

const float BG_SM_StartPos[3][4] =
{
    { 744.542053f, 183.545883f, 319.658203f, 4.356342f },
    { 739.400330f, 203.598511f, 319.603333f, 2.308198f },
    { 760.184509f, 198.844742f, 319.446655f, 0.351249f }
};

const uint32 BG_SM_CartIDs[3]
{
    NPC_MINE_CART_1,
    NPC_MINE_CART_2,
    NPC_MINE_CART_3
};

const uint32 BG_SM_CartTypes[3]
{
    NPC_SM_MINE_CART_1,
    NPC_SM_MINE_CART_2,
    NPC_SM_MINE_CART_3
};

enum
{
    MINE_CART_AT_DEPOT_POINTS       = 200,
    POINTS_PER_MINE_CART            = 150,
    DISPLAY_WORLDSTATE              = 1,
    BG_SM_NOT_SMWEEKEND_HONOR_TICKS = 200,
    BG_SM_SMWEEKEND_HONOR_TICKS     = 130,
    SM_MINE_CART_MAX                = 3,
    SM_MAX_PATHS                    = 2,
};

struct BattlegroundSMScore : public BattlegroundScore
{
    BattlegroundSMScore() : BattlegroundScore(), MineCartCaptures(0) { }
    virtual ~BattlegroundSMScore() { }

    uint32 MineCartCaptures;
};

class BattlegroundSM : public Battleground
{
    public:
        BattlegroundSM();
        ~BattlegroundSM();

        /* inherited from BattlegroundClass */
        void AddPlayer(Player* player) override;
        void StartingEventCloseDoors() override;
        void StartingEventOpenDoors() override;

        void HandleKillPlayer(Player* player, Player* killer) override;
        bool SetupBattleground() override;
        void Reset() override;
        WorldSafeLocsEntry const* GetClosestGraveYard(Player* player) override;
        void PostUpdateImpl(uint32 diff) override;
        void UpdateTeamScore(uint32 team);
        void EndBattleground(uint32 winner);
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value, bool doAddHonor = true) override;
        void FillInitialWorldStates(WorldStateBuilder& builder) override;
        void EventPlayerClickedOnFlag(Player* player, Unit* target) override;

        /* Battleground Events */
        void EventReopenDepot(uint32 diff);

    private:
        void EventTeamCapturedMineCart(uint32 team, uint8 mineCart);
        void SummonMineCart(uint32 diff);
        void FirstMineCartSummon(uint32 diff);
        void MineCartsMoves();
        void CheckPlayerNearMineCart(uint32 diff);
        void CheckMineCartNearDepot();
        void MineCartAddPoints(uint32 diff);
        void ResetDepotsAndMineCarts(uint8 depot, uint8 mineCart);
        void CheckTrackSwitch();
        void ProcessCartTicks();
        void CartSpawned(BG_SM_MineCarts cart);
        void CartControllSeized(BG_SM_MineCarts cart, Team team);

        /* Scorekeeping */
        uint32 GetTeamScore(uint32 teamID) const { return m_TeamScores[GetTeamIndexByTeamId(teamID)]; }
        uint32 GetMaxScore() const { return BG_SM_MAX_TEAM_SCORE; }
        bool IsScoreIncremental() const { return true; }

        void AddPoints(uint32 team, uint32 points);

        void RemovePoint(uint32 teamID, uint32 points = 1) { m_TeamScores[GetTeamIndexByTeamId(teamID)] -= points; }
        void SetTeamPoint(uint32 teamID, uint32 points = 0) { m_TeamScores[GetTeamIndexByTeamId(teamID)] = points; }

        uint32 GetMineCartTeamKeeper(uint8 mineCart);
        uint32 m_HonorScoreTics[2];

        uint32 m_mineCartsTrigger[SM_MINE_CART_MAX]; // m_mineCartsTrigger is not used
        int32 m_mineCartsProgressBar[SM_MINE_CART_MAX];
        uint32 m_mineCartTeamKeeper[SM_MINE_CART_MAX]; // keepers team // m_mineCartTeamKeeper is not used
        int32 m_firstMineCartSummonTimer;
        int32 m_mineCartCheckTimer;
        int32 m_depotCloseTimer[4];
        uint32 m_mineCartAddPointsTimer;
        int32 m_trackSwitchClickTimer[SM_MAX_TRACK_SWITCH];
        bool m_depot[4]; // 0 = Waterfall, 1 = Lava, 2 = Diamond, 3 = Troll
        bool m_mineCartReachedDepot[SM_MINE_CART_MAX];
        bool m_mineCartNearDepot[SM_MINE_CART_MAX];
        bool m_mineCartSpawned[SM_MINE_CART_MAX];
        bool m_firstMineCartSpawned;
        bool m_pathDone[SM_MINE_CART_MAX - 1][SM_MAX_PATHS]; // Only for first and third mine cart
        bool m_waterfallPathDone; // Waterfall path
        bool m_trackSwitch[SM_MAX_TRACK_SWITCH]; // East : true = open, false = close | North : true = close, false = open
        bool m_trackSwitchCanInterract[SM_MAX_TRACK_SWITCH];
        bool capturesCartByTeam[TEAM_NEUTRAL];
        bool m_endOfLineCriteria[SM_MINE_CART_MAX];
        uint32 m_cartTicks[SM_MINE_CART_MAX];
        std::map<uint32, uint64> m_playerTicks[SM_MINE_CART_MAX];
        std::map<uint64, uint32> m_fiveForFivePorgress;

        uint32 m_honorTicks;
        bool m_isInformedNearVictory;

        uint32 m_HonorWinKills;
        uint32 m_HonorEndKills;
};

#endif
