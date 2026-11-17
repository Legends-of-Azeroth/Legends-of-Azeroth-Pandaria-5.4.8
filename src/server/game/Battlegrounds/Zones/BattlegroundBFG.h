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

#ifndef BATTLEGROUND_BFG_H
#define BATTLEGROUND_BFG_H

#include "Battleground.h"

enum BG_BFG_Rewards
{
    BG_BFG_MAP_COMPLETE,
    BG_BFG_REWARD_NUM
};

enum BG_BFG_WorldStates
{
    BG_BFG_OP_OCCUPIED_BASES_HORDE       = 1778,
    BG_BFG_OP_OCCUPIED_BASES_ALLY        = 1779,
    BG_BFG_OP_RESOURCES_ALLY             = 1776,
    BG_BFG_OP_RESOURCES_HORDE            = 1777,
    BG_BFG_OP_RESOURCES_MAX              = 1780,
    BG_BFG_OP_RESOURCES_WARNING          = 1955
};

const uint32 BG_BFG_OP_NODESTATES[3] = { 1767, 1782, 1772 };
const uint32 BG_BFG_OP_NODEICONS[3]  = { 1842, 1846, 1845 };

enum BG_BFG_NodeObjectId
{
    BG_BFG_OBJECTID_NODE_BANNER_0    = 208779,       // Lighthouse banner
    BG_BFG_OBJECTID_NODE_BANNER_1    = 208780,       // Waterworks banner
    BG_BFG_OBJECTID_NODE_BANNER_2    = 208781,       // Mines banner
};

enum BG_BFG_ObjectType
{
    BG_BFG_OBJECT_BANNER_NEUTRAL          = 0,
    BG_BFG_OBJECT_BANNER_CONT_A           = 1,
    BG_BFG_OBJECT_BANNER_CONT_H           = 2,
    BG_BFG_OBJECT_BANNER_ALLY             = 3,
    BG_BFG_OBJECT_BANNER_HORDE            = 4,
    BG_BFG_OBJECT_AURA_ALLY               = 5,
    BG_BFG_OBJECT_AURA_HORDE              = 6,
    BG_BFG_OBJECT_AURA_CONTESTED          = 7,
    BG_BFG_OBJECT_GATE_A_1                = 24,
    BG_BFG_OBJECT_GATE_A_2                = 25,
    BG_BFG_OBJECT_GATE_H_1                = 26,
    BG_BFG_OBJECT_GATE_H_2                = 27,
    //buffs
    BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE    = 28,
    BG_BFG_OBJECT_REGENBUFF_LIGHTHOUSE    = 29,
    BG_BFG_OBJECT_BERSERKBUFF_LIGHTHOUSE  = 30,
    BG_BFG_OBJECT_SPEEDBUFF_WATERWORKS    = 31,
    BG_BFG_OBJECT_REGENBUFF_WATERWORKS    = 32,
    BG_BFG_OBJECT_BERSERKBUFF_WATERWORKS  = 33,
    BG_BFG_OBJECT_SPEEDBUFF_MINE          = 34,
    BG_BFG_OBJECT_REGENBUFF_MINE          = 35,
    BG_BFG_OBJECT_BERSERKBUFF_MINE        = 36,
    BG_BFG_OBJECT_MAX                     = 37,
};

enum BG_BFG_ObjectTypes
{
    BG_BFG_OBJECTID_BANNER_A             = 180058,
    BG_BFG_OBJECTID_BANNER_CONT_A        = 180059,
    BG_BFG_OBJECTID_BANNER_H             = 180060,
    BG_BFG_OBJECTID_BANNER_CONT_H        = 180061,

    BG_BFG_OBJECTID_AURA_A               = 180100,
    BG_BFG_OBJECTID_AURA_H               = 180101,
    BG_BFG_OBJECTID_AURA_C               = 180102,

    BG_BFG_OBJECTID_GATE_A_1             = 207177,
    BG_BFG_OBJECTID_GATE_A_2             = 180322,
    BG_BFG_OBJECTID_GATE_H_1             = 207178,
    BG_BFG_OBJECTID_GATE_H_2             = 180322,
};

enum BG_BFG_Timers
{
    BG_BFG_FLAG_CAPTURING_TIME           = 60000,
};

enum BG_BFG_Score
{
    BG_BFG_WARNING_NEAR_VICTORY_SCORE    = 1800,
    BG_BFG_MAX_TEAM_SCORE                = 2000
};

/* Do not change below, or this thing will go boom! */
enum BG_BFG_BattlegroundNodes
{
    BG_BFG_NODE_LIGHTHOUSE       = 0,
    BG_BFG_NODE_WATERWORKS       = 1,
    BG_BFG_NODE_MINE             = 2,

    BG_BFG_DYNAMIC_NODES_COUNT   = 3,                        // Dynamic nodes that can be captured

    BG_BFG_SPIRIT_ALIANCE        = 3,
    BG_BFG_SPIRIT_HORDE          = 4,

    BG_BFG_ALL_NODES_COUNT       = 5,                        // All nodes (dynamic and static)
};

enum BG_BFG_NodeStatus
{
    BG_BFG_NODE_TYPE_NEUTRAL             = 0,
    BG_BFG_NODE_TYPE_CONTESTED           = 1,
    BG_BFG_NODE_STATUS_ALLY_CONTESTED    = 1,
    BG_BFG_NODE_STATUS_HORDE_CONTESTED   = 2,
    BG_BFG_NODE_TYPE_OCCUPIED            = 3,
    BG_BFG_NODE_STATUS_ALLY_OCCUPIED     = 3,
    BG_BFG_NODE_STATUS_HORDE_OCCUPIED    = 4
};

enum BG_BFG_Sounds
{
    BG_BFG_SOUND_NODE_CLAIMED            = 8192,
    BG_BFG_SOUND_NODE_CAPTURED_ALLIANCE  = 8173,
    BG_BFG_SOUND_NODE_CAPTURED_HORDE     = 8213,
    BG_BFG_SOUND_NODE_ASSAULTED_ALLIANCE = 8212,
    BG_BFG_SOUND_NODE_ASSAULTED_HORDE    = 8174,
    BG_BFG_SOUND_NEAR_VICTORY            = 8456
};

enum BG_BFG_Objectives
{
    BG_OBJECTIVE_ASSAULT_BASE           = 370,
    BG_OBJECTIVE_DEFEND_BASE            = 371
};

/* Holiday/Reg Honor/Rep gains */
#define BG_BFG_NotBGWeekendHonorTicks   330
#define BG_BFG_BGWeekendHonorTicks      200
#define BG_BFG_NotBGWeekendRepTicks     200
#define BG_BFG_BGWeekendRepTicks        150

const float BG_BFG_NodePositions[BG_BFG_DYNAMIC_NODES_COUNT][4] =
{
    { 1057.790f, 1278.285f, 3.1500f, 1.945662f },        // Lighthouse
    { 980.0446f, 948.7411f, 12.650f, 5.904071f },        // Waterworks
    { 1251.010f, 958.2685f, 5.6000f, 5.892280f },        // Mine  
};

// x, y, z, o, rot0, rot1, rot2, rot3
const float BG_BFG_DoorPositions[4][8] =
{
    { 918.160f, 1336.75f, 27.6299f, 2.87927f, 0.0f, 0.0f, 0.983231f, 0.182367f },
    { 918.160f, 1336.75f, 26.6299f, 2.87927f, 0.0f, 0.0f, 0.983231f, 0.182367f },
    { 1396.15f, 977.014f, 7.43169f, 6.27043f, 0.0f, 0.0f, 0.006378f, -0.99998f },
    { 1396.15f, 977.014f, 0.33169f, 6.27043f, 0.0f, 0.0f, 0.006378f, -0.99998f },
};

const uint32 BG_BFG_TickIntervals[4] = { 0, 8000, 3000, 1000 };
const uint32 BG_BFG_TickPoints[4]    = { 0, 10, 10, 30 };

//Light, Water, Mine, Ally, Horde
const uint32 BG_BFG_GraveyardIds[BG_BFG_ALL_NODES_COUNT] = { 1736, 1738, 1735, 1740, 1739 };

const float BG_BFG_SpiritGuidePos[BG_BFG_ALL_NODES_COUNT][4] =
{
    { 1034.82f, 1335.58f, 12.0095f, 5.15f },     // Lighthouse
    { 887.578f, 937.337f, 23.7737f, 0.45f },     // Waterworks
    { 1252.23f, 836.547f, 27.7895f, 1.60f },     // Mine
    { 908.274f, 1338.60f, 27.6449f, 5.95f },     // Alliance
    { 1401.38f, 977.125f, 7.44215f, 3.04f },     // Horde
};

const float BG_BFG_BuffPositions[BG_BFG_DYNAMIC_NODES_COUNT][4] =
{
    { 1063.57f, 1313.42f, 4.91f, 4.14f },        // Lighthouse
    { 961.830f, 977.03f, 14.15f, 4.55f },        // Waterworks
    { 1193.09f, 1017.46f, 7.98f, 0.24f },        // Mine
};

struct BattlegroundBFGScore : public BattlegroundScore
{
    uint32 BasesAssaulted = 0;
    uint32 BasesDefended  = 0;
};

class BattlegroundBFG : public Battleground
{
    struct BannerTimer
    {
        uint32 Timer;
        uint8  Type;
        uint8  TeamIndex;
    };

    friend class BattlegroundMgr;

public:
    BattlegroundBFG();

    /* inherited from BattlegroundClass */
    void AddPlayer(Player* player) override;
    void RemovePlayer(Player* player, ObjectGuid guid, uint32 team) override;
    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    void Reset() override;
    WorldSafeLocsEntry const* GetClosestGraveYard(Player* player) override;

    bool SetupBattleground() override;

    /* Score-keeping */
    void UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor = true) override;
    void FillInitialWorldStates(WorldStateBuilder& builder) override;

    void EventPlayerClickedOnFlag(Player* source, GameObject*) override;

    /* achievement requirements. */
    bool IsAllNodesControlledByTeam(uint32 team) const override;

    uint32 GetRBGLoserReward(uint32 team) const override;

private:
    void PostUpdateImpl(uint32 diff) override;
    /* GameObject spawning/removing */
    void SendNodeUpdate(uint8 node);
    void CreateBanner(uint8 node, uint8 type, uint8 teamIndex, bool delay);
    void DelBanner(uint8 node, uint8 type, uint8 teamIndex);

    /* Creature spawning/removing */
    // TODO: need to get the peons spawns scripted
    void NodeOccupied(uint8 node, Team team);
    void NodeDeOccupied(uint8 node);

    int32 GetNodeNameId(uint8 node);

    /* Nodes info:
     *  0: neutral
     *  1: ally contested
     *  2: horde contested
     *  3: ally occupied
     *  4: horde occupied
     */
    uint8   m_nodes[BG_BFG_DYNAMIC_NODES_COUNT];
    uint8   m_prevNodes[BG_BFG_DYNAMIC_NODES_COUNT];
    BannerTimer m_bannerTimers[BG_BFG_DYNAMIC_NODES_COUNT];
    uint32  m_nodeTimers[BG_BFG_DYNAMIC_NODES_COUNT];

    uint32  m_lastTick[BG_TEAMS_COUNT];
    uint32  m_honorScoreTicks[BG_TEAMS_COUNT];
    uint32  m_reputationScoreTicks[BG_TEAMS_COUNT];

    bool    m_isInformedNearVictory;
    uint32  m_honorTicks;
    uint32  m_reputationTicks;
};

#endif
