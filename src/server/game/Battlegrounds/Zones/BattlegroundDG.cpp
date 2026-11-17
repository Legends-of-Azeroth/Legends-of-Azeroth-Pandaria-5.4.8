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

#include "BattlegroundDG.h"
#include "Player.h"
#include "Language.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "ScriptedCreature.h"
#include "BattlegroundMgr.h"

enum
{
    OBJECTIVE_CAPTURE_CART  = 457,
    OBJECTIVE_RETURN_CART   = 458,
    OBJECTIVE_ASSAULT_MINE  = 459,
    OBJECTIVE_DEFEND_MINE   = 460,

    // Achievements
    WORLD_STATE_DG_PERFECTION = 14105,
};

/************************************************************************/
/*                     STARTING BATTLEGROUND                            */
/************************************************************************/
BattlegroundDG::BattlegroundDG() : Battleground()
{
    m_isInformedNearVictory = false;
    m_BuffChange = true;

    BgObjects.resize(BG_DG_OBJECT_MAX);
    BgCreatures.resize(BG_DG_CREATURES_MAX);

    bool isBGWeekEnd = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_honorTicks = (isBGWeekEnd) ? BG_DG_BBGWeekendHonorTicks : BG_DG_NotBGWeekendHonorTicks;

    StartMessageIds[BG_STARTING_EVENT_FIRST]    = LANG_BG_DG_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND]   = LANG_BG_DG_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]    = LANG_BG_DG_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH]   = LANG_BG_DG_HAS_BEGUN;
}

BattlegroundDG::~BattlegroundDG() { }

bool BattlegroundDG::SetupBattleground()
{
    // Nodes related
    for (int i = 0; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        if (!AddCreature(BG_DG_OBJECTID_CAPT_POINT, BG_DG_OBJECT_CAPT_POINT_START + i, TEAM_NEUTRAL, BG_DG_NodePositions[i][0], BG_DG_NodePositions[i][1], BG_DG_NodePositions[i][2], BG_DG_NodePositions[i][3], RESPAWN_IMMEDIATELY))
        {
            TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn flags PNJ. Battleground not created!");
            return false;
        }
        if (!AddObject(BG_DG_OBJECT_AURA_ALLY + BG_DG_OBJECT_DYNAMIC_TOTAL * i, BG_DG_OBJECTID_AURA_A, BG_DG_AuraPositions[i][0], BG_DG_AuraPositions[i][1], BG_DG_AuraPositions[i][2], BG_DG_AuraPositions[i][3], 0, 0, std::sin(BG_DG_AuraPositions[i][3] / 2), std::cos(BG_DG_AuraPositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_DG_OBJECT_AURA_HORDE + BG_DG_OBJECT_DYNAMIC_TOTAL * i, BG_DG_OBJECTID_AURA_H, BG_DG_AuraPositions[i][0], BG_DG_AuraPositions[i][1], BG_DG_AuraPositions[i][2], BG_DG_AuraPositions[i][3], 0, 0, std::sin(BG_DG_AuraPositions[i][3] / 2), std::cos(BG_DG_AuraPositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_DG_OBJECT_AURA_CONTESTED + BG_DG_OBJECT_DYNAMIC_TOTAL * i, BG_DG_OBJECTID_AURA_C, BG_DG_AuraPositions[i][0], BG_DG_AuraPositions[i][1], BG_DG_AuraPositions[i][2], BG_DG_AuraPositions[i][3], 0, 0, std::sin(BG_DG_AuraPositions[i][3] / 2), std::cos(BG_DG_AuraPositions[i][3] / 2), RESPAWN_ONE_DAY)
            )
        {
            TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn flags aura objects. Battleground not created!");
            return false;
        }
        if (!AddObject(BG_DG_OBJECT_PJ_COLLISION + BG_DG_OBJECT_DYNAMIC_TOTAL * i, BG_DG_OBJECTID_PJ_COLLISION, BG_DG_CollisionPJPositions[i][0], BG_DG_CollisionPJPositions[i][1], BG_DG_CollisionPJPositions[i][2], BG_DG_CollisionPJPositions[i][3], 0, 0, std::sin(BG_DG_CollisionPJPositions[i][3] / 2), std::cos(BG_DG_CollisionPJPositions[i][3] / 2), RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn flags collision objects. Battleground not created!");
            return false;
        }
    }
    // Carts
    if (!AddObject(BG_DG_OBJECT_CART_ALLIANCE, BG_DG_OBJECTID_CART_ALLY, BG_DG_CartPositions[0][0], BG_DG_CartPositions[0][1], BG_DG_CartPositions[0][2], BG_DG_CartPositions[0][3], 0, 0, std::sin(BG_DG_CartPositions[0][3] / 2), std::cos(BG_DG_CartPositions[0][3] / 2), BG_DG_CART_RESPAWN_TIME / 1000)
        || !AddObject(BG_DG_OBJECT_CART_HORDE, BG_DG_OBJECTID_CART_HORDE, BG_DG_CartPositions[1][0], BG_DG_CartPositions[1][1], BG_DG_CartPositions[1][2], BG_DG_CartPositions[1][3], 0, 0, std::sin(BG_DG_CartPositions[1][3] / 2), std::cos(BG_DG_CartPositions[1][3] / 2), BG_DG_CART_RESPAWN_TIME / 1000)
        )
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn carts. Battleground not created!");
        return false;
    }
    // Doors
    if (!AddObject(BG_DG_OBJECT_GATE_1, BG_DG_OBJECTID_GATE, BG_DG_DoorPositions[0][0], BG_DG_DoorPositions[0][1], BG_DG_DoorPositions[0][2], BG_DG_DoorPositions[0][3], BG_DG_DoorPositions[0][4], BG_DG_DoorPositions[0][5], BG_DG_DoorPositions[0][6], BG_DG_DoorPositions[0][7], RESPAWN_IMMEDIATELY)
        || !AddObject(BG_DG_OBJECT_GATE_2, BG_DG_OBJECTID_GATE, BG_DG_DoorPositions[1][0], BG_DG_DoorPositions[1][1], BG_DG_DoorPositions[1][2], BG_DG_DoorPositions[1][3], BG_DG_DoorPositions[1][4], BG_DG_DoorPositions[1][5], BG_DG_DoorPositions[1][6], BG_DG_DoorPositions[1][7], RESPAWN_IMMEDIATELY)
        || !AddObject(BG_DG_OBJECT_GATE_3, BG_DG_OBJECTID_GATE, BG_DG_DoorPositions[2][0], BG_DG_DoorPositions[2][1], BG_DG_DoorPositions[2][2], BG_DG_DoorPositions[2][3], BG_DG_DoorPositions[2][4], BG_DG_DoorPositions[2][5], BG_DG_DoorPositions[2][6], BG_DG_DoorPositions[2][7], RESPAWN_IMMEDIATELY)
        || !AddObject(BG_DG_OBJECT_GATE_4, BG_DG_OBJECTID_GATE, BG_DG_DoorPositions[3][0], BG_DG_DoorPositions[3][1], BG_DG_DoorPositions[3][2], BG_DG_DoorPositions[3][3], BG_DG_DoorPositions[3][4], BG_DG_DoorPositions[3][5], BG_DG_DoorPositions[3][6], BG_DG_DoorPositions[3][7], RESPAWN_IMMEDIATELY)
        )
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn door object. Battleground not created!");
        return false;
    }
    // Buffs
    for (int i = 0; i < MAX_BUFFS_DG; i++)
    {
        if (!AddObject(BG_DG_OBJECT_BUFF_NORTH + i, Buff_Entries[urand(0, 2)], BG_DG_BuffPositions[i][0], BG_DG_BuffPositions[i][1], BG_DG_BuffPositions[i][2], BG_DG_BuffPositions[i][3], 0, 0, std::sin(BG_DG_BuffPositions[i][3] / 2), std::cos(BG_DG_BuffPositions[i][3] / 2), BUFF_RESPAWN_TIME))
        {
            TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn buff object. Battleground not created!");
            return false;
        }
    }

    WorldSafeLocsEntry const* safeLoc = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[TEAM_ALLIANCE][BG_DG_GRAVEYARD_NORTHERN]);
    if (!safeLoc || !AddSpiritGuide(BG_DG_SPIRIT_NORTHERN_ALLIANCE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o + M_PI, ALLIANCE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn Alliance-Northern spirit guide! Battleground not created!");
        return false;
    }

    safeLoc = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[TEAM_ALLIANCE][BG_DG_GRAVEYARD_SOUTHERN]);
    if (!safeLoc || !AddSpiritGuide(BG_DG_SPIRIT_SOUTHERN_ALLIANCE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o + M_PI, ALLIANCE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn Alliance-Southern spirit guide! Battleground not created!");
        return false;
    }

    safeLoc = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[TEAM_HORDE][BG_DG_GRAVEYARD_NORTHERN]);
    if (!safeLoc || !AddSpiritGuide(BG_DG_SPIRIT_NORTHERN_HORDE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o + M_PI, HORDE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn Horde-Northern spirit guide! Battleground not created!");
        return false;
    }

    safeLoc = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[TEAM_HORDE][BG_DG_GRAVEYARD_SOUTHERN]);
    if (!safeLoc || !AddSpiritGuide(BG_DG_SPIRIT_SOUTHERN_HORDE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o + M_PI, HORDE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundDG: Failed to spawn Horde-Southern spirit guide! Battleground not created!");
        return false;
    }

    return true;
}

void BattlegroundDG::Reset()
{
    // Call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE]     = 0;
    m_TeamScores[TEAM_HORDE]        = 0;
    m_lastTick[TEAM_ALLIANCE]       = 0;
    m_lastTick[TEAM_HORDE]          = 0;
    m_honorScoreTics[TEAM_ALLIANCE] = 0;
    m_honorScoreTics[TEAM_HORDE]    = 0;
    m_isInformedNearVictory         = false;

    if (sBattlegroundMgr->IsBGWeekend(GetTypeID()))
    {
        m_HonorWinKills = 3;
        m_HonorEndKills = 4;
    }
    else
    {
        m_HonorWinKills = 1;
        m_HonorEndKills = 2;
    }

    for (uint8 i = 0; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        m_nodes[i]      = BG_DG_NODE_TYPE_NEUTRAL;
        m_prevNodes[i]  = BG_DG_NODE_TYPE_NEUTRAL;
        m_nodeTimers[i] = 0;
    }

    m_flagKeepers[TEAM_ALLIANCE].Clear();
    m_flagKeepers[TEAM_HORDE].Clear();
    m_flagState[TEAM_ALLIANCE]       = BG_DG_CART_STATE_ON_BASE;
    m_flagState[TEAM_HORDE]          = BG_DG_CART_STATE_ON_BASE;
    m_flagGold[TEAM_ALLIANCE]        = 0;
    m_flagGold[TEAM_HORDE]           = 0;
    m_lastFlagCaptureTeam            = 0;
    m_flagsDropTimer[TEAM_ALLIANCE]  = 0;
    m_flagsDropTimer[TEAM_HORDE]     = 0;
    m_flagsTimer[TEAM_ALLIANCE]      = 0;
    m_flagsTimer[TEAM_HORDE]         = 0;
}

void BattlegroundDG::EndBattleground(uint32 winner)
{
    // Win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), HORDE);
    // Complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), HORDE);

    Battleground::EndBattleground(winner);
}

void BattlegroundDG::StartingEventCloseDoors()
{
    // Despawn everything
    for (int i = BG_DG_OBJECT_AURA_ALLY; i < BG_DG_OBJECT_MAX; ++i)
        SpawnBGObject(i, RESPAWN_ONE_DAY);

    for (int i = BG_DG_NODE_GOBLIN_MINE; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        // Spawn collision PJ
        SpawnBGObject((i * BG_DG_OBJECT_DYNAMIC_TOTAL) + BG_DG_OBJECT_PJ_COLLISION, RESPAWN_IMMEDIATELY);
        // Spawn neutral aura
        SpawnBGObject((i * BG_DG_OBJECT_DYNAMIC_TOTAL) + BG_DG_OBJECT_AURA_CONTESTED, RESPAWN_IMMEDIATELY);
        // Add capt points auras
        if (Creature* capturePoint = GetBgMap()->GetCreature(BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + i]))
            capturePoint->CastSpell(capturePoint, BG_DG_CAPT_POINT_NEUTRAL, true);
    }

    // Spawn carts
    for (uint32 i = BG_DG_OBJECT_CART_ALLIANCE; i <= BG_DG_OBJECT_CART_HORDE; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);

    // Spawn Buffs
    for (uint32 i = 0; i < MAX_BUFFS_DG; i++)
        SpawnBGObject(BG_DG_OBJECT_BUFF_NORTH + i, RESPAWN_IMMEDIATELY);

    // Starting doors
    for (int i = BG_DG_OBJECT_GATE_1; i <= BG_DG_OBJECT_GATE_4; ++i)
    {
        DoorClose(i);
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
    }
}

void BattlegroundDG::StartingEventOpenDoors()
{
    // Open doors
    for (int i = BG_DG_OBJECT_GATE_1; i <= BG_DG_OBJECT_GATE_4; ++i)
        DoorOpen(i);

    // Force update of nodes
    for (int i = 0; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        if (Creature* capturePoint = GetBgMap()->GetCreature(BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + i]))
        {
            for (auto&& itr : m_Players)
            {
                if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                    capturePoint->SendUpdateToPlayer(player); // does it needed?
            }
        }
    }
}

/************************************************************************/
/*                           CORE UPDATES                               */
/************************************************************************/
void BattlegroundDG::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    BattlegroundDGScore* sc = new BattlegroundDGScore{};
    sc->Team = player->GetBGTeam();
    PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundDG::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        PostUpdateImplFlags(diff);
        PostUpdateImplCart(diff);
    }
}

void BattlegroundDG::FillInitialWorldStates(WorldStateBuilder& builder)
{
    builder.AppendState(uint32(WORLDSTATE_DG_CART_STATE_ALLIANCE), uint32((m_flagState[TEAM_ALLIANCE] == BG_DG_CART_STATE_ON_PLAYER) ? 2 : 1));
    builder.AppendState(uint32(WORLDSTATE_DG_CART_STATE_HORDE), uint32((m_flagState[TEAM_HORDE] == BG_DG_CART_STATE_ON_PLAYER) ? 2 : 1));

    for (uint8 node = 0; node < BG_DG_ALL_NODES_COUNT; ++node)
    {
        for (uint8 status = BG_DG_NODE_TYPE_NEUTRAL; status <= BG_DG_NODE_STATUS_HORDE_OCCUPIED; status++)
        {
            if (m_nodes[node] == status)
                builder.AppendState(uint32(BG_DG_NodesWs[node][status].m_WorldState), uint32(BG_DG_NodesWs[node][status].m_SentValueToShow));
            else if (status >= BG_DG_NODE_TYPE_OCCUPIED)
            {
                // This is there because the occupied worldstate is the same for ally & horde, so we don't want to worldstate that erase the good value
                // If the node is correct status, send the value, else if status not occupied send 0 (to hide)
                // BUT IF ITS OCCUPED, SEND 0 ONLY IF THE NODE ITSELF ISNT OCCUPED
                if (m_nodes[node] < BG_DG_NODE_TYPE_OCCUPIED)
                    builder.AppendState(uint32(BG_DG_NodesWs[node][status].m_WorldState), uint32(0));
            }
            else
                builder.AppendState(uint32(BG_DG_NodesWs[node][status].m_WorldState), uint32(0));
        }
    }

    // How many bases each team owns
    uint8 ally = 0, horde = 0;
    for (uint8 node = 0; node < BG_DG_ALL_NODES_COUNT; ++node)
    {
        if (m_nodes[node] == BG_DG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_nodes[node] == BG_DG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;
    }

    builder.AppendState(uint32(WORLDSTATE_DG_OCCUPIED_BASES_ALLIANCE), uint32(ally));
    builder.AppendState(uint32(WORLDSTATE_DG_OCCUPIED_BASES_HORDE), uint32(horde));

    builder.AppendState(uint32(WORLDSTATE_DG_SCORE_ALLIANCE), uint32(m_TeamScores[TEAM_ALLIANCE]));
    builder.AppendState(uint32(WORLDSTATE_DG_SCORE_HORDE), uint32(m_TeamScores[TEAM_HORDE]));
}

void BattlegroundDG::UpdatePlayerScore(Player* source, uint32 type, uint32 value, bool addHonor)
{
    BattlegroundScoreMap::iterator i = PlayerScores.find(source->GetGUID());
    if (i == PlayerScores.end())
        return;

    uint32 teamIndex = GetTeamIndexByTeamId(source->GetBGTeam());
    switch (type)
    {
        case SCORE_CART_CAPTURES:
            ((BattlegroundDGScore*)i->second)->CapturedCart += value;
            source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_CAPTURE_CART, 1);
            break;
        case SCORE_CART_RETURNS:
            ((BattlegroundDGScore*)i->second)->ReturnedCart += value;
            source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_RETURN_CART, 1);
            break;
        case SCORE_BASES_ASSAULTED:
            ((BattlegroundDGScore*)i->second)->AssaultedMines += value;
            source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_ASSAULT_MINE, 1);
            break;
        case SCORE_BASES_DEFENDED:
            ((BattlegroundDGScore*)i->second)->DefendedMines += value;
            source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_DEFEND_MINE, 1);
            break;
        default:
            Battleground::UpdatePlayerScore(source, type, value, addHonor);
            break;
    }
}

void BattlegroundDG::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    EventPlayerDroppedFlag(player);

    Battleground::HandleKillPlayer(player, killer);
}

WorldSafeLocsEntry const* BattlegroundDG::GetClosestGraveYard(Player* player)
{
    uint32 teamIndex = GetTeamIndexByTeamId(player->GetBGTeam());

    WorldSafeLocsEntry const* north = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[teamIndex][0]);
    float northDist = player->GetExactDist2dSq(north->x, north->y);
    WorldSafeLocsEntry const* south = sWorldSafeLocsStore.LookupEntry(BG_DG_GraveyardIds[teamIndex][1]);
    float southDist = player->GetExactDist2dSq(south->x, south->y);

    // If you are on the southern side of the map, respawn at northern (and vice versa)
    // http://us.battle.net/wow/en/game/pvp/battlegrounds/deepwind-gorge
    return northDist < southDist ? south : north;
}

void BattlegroundDG::UpdateTeamScore(int team, int32 value)
{
    m_TeamScores[team] += value;
    m_TeamScores[team] = std::min(int32(BG_DG_MAX_VICTORY_POINTS), m_TeamScores[team]);
    m_TeamScores[team] = std::max(0, m_TeamScores[team]);

    UpdateWorldState(team == TEAM_ALLIANCE ? WORLDSTATE_DG_SCORE_ALLIANCE : WORLDSTATE_DG_SCORE_HORDE, m_TeamScores[team]);

    if (m_TeamScores[team] == BG_DG_MAX_VICTORY_POINTS)
    {
        if (!GetTeamScore(team == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE))
            GetBgMap()->SetWorldState(WORLD_STATE_DG_PERFECTION, 1);

        UpdateWorldState(WORLDSTATE_DG_CART_STATE_ALLIANCE, 1);
        UpdateWorldState(WORLDSTATE_DG_CART_STATE_HORDE, 1);
        EndBattleground(team == TEAM_ALLIANCE ? ALLIANCE : HORDE);
    }

    else if (!m_isInformedNearVictory && m_TeamScores[team] > BG_DG_NEAR_VICTORY_POINTS)
    {
        SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_DG_A_NEAR_VICTORY : LANG_BG_DG_H_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll((team == TEAM_ALLIANCE) ? BG_DG_SOUND_ALLIANCE_NEAR_VICTORY : BG_DG_SOUND_HORDE_NEAR_VICTORY);
        m_isInformedNearVictory = true;
    }
}

/************************************************************************/
/*                          FLAGS UPDATES                               */
/************************************************************************/
void BattlegroundDG::ChangeBanner(uint8 node, uint8 type)
{
    // 1. Manage auras objects
    if (type == BG_DG_NODE_TYPE_CONTESTED && (m_prevNodes[node] == BG_DG_NODE_STATUS_ALLY_OCCUPIED || m_prevNodes[node] == BG_DG_NODE_STATUS_HORDE_OCCUPIED))
    {
        // Capt point goes from occupied to contested
        // 1.1. Despawn controlled aura
        SpawnBGObject(node * BG_DG_OBJECT_DYNAMIC_TOTAL + BG_DG_OBJECT_AURA_ALLY + (m_nodes[node] - 1), RESPAWN_ONE_DAY);
        // 1.2. Spawn contested aura
        SpawnBGObject(node * BG_DG_OBJECT_DYNAMIC_TOTAL + BG_DG_OBJECT_AURA_CONTESTED, RESPAWN_IMMEDIATELY);
    }
    else if (type == BG_DG_NODE_TYPE_OCCUPIED && (m_prevNodes[node] == BG_DG_NODE_STATUS_ALLY_CONTESTED || m_prevNodes[node] == BG_DG_NODE_STATUS_HORDE_CONTESTED))
    {
        // Capt point goes from contested to occupied
        // 1.1. Despawn contested aura
        SpawnBGObject(node * BG_DG_OBJECT_DYNAMIC_TOTAL + BG_DG_OBJECT_AURA_CONTESTED, RESPAWN_ONE_DAY);
        // 1.2. Spawn controlled aura
        SpawnBGObject(node * BG_DG_OBJECT_DYNAMIC_TOTAL + BG_DG_OBJECT_AURA_ALLY + (m_nodes[node] - 1), RESPAWN_IMMEDIATELY);
    }

    // 2. Manage the capt point
    if (Creature* capturePoint = GetBgMap()->GetCreature(BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + node]))
    {
        // 2.1. Remove the previous aura
        uint32 removeAura = BG_DG_CAPT_POINT_NEUTRAL;
        switch (m_prevNodes[node])
        {
            case BG_DG_NODE_STATUS_ALLY_CONTESTED:
                removeAura = BG_DG_CAPT_POINT_ALLIANCE_CONTEST;
                break;
            case BG_DG_NODE_STATUS_HORDE_CONTESTED:
                removeAura = BG_DG_CAPT_POINT_HORDE_CONTEST;
                break;
            case BG_DG_NODE_STATUS_ALLY_OCCUPIED:
                removeAura = BG_DG_CAPT_POINT_ALLIANCE_CONTROL;
                break;
            case BG_DG_NODE_STATUS_HORDE_OCCUPIED:
                removeAura = BG_DG_CAPT_POINT_HORDE_CONTROL;
                break;
            default:
                break;
        }

        capturePoint->RemoveAura(removeAura);

        // 2.2. Add the new aura
        uint32 addAura = BG_DG_CAPT_POINT_NEUTRAL;
        switch (m_nodes[node])
        {
            case BG_DG_NODE_STATUS_ALLY_CONTESTED:
                addAura = BG_DG_CAPT_POINT_ALLIANCE_CONTEST;
                break;
            case BG_DG_NODE_STATUS_HORDE_CONTESTED:
                addAura = BG_DG_CAPT_POINT_HORDE_CONTEST;
                break;
            case BG_DG_NODE_STATUS_ALLY_OCCUPIED:
                addAura = BG_DG_CAPT_POINT_ALLIANCE_CONTROL;
                break;
            case BG_DG_NODE_STATUS_HORDE_OCCUPIED:
                addAura = BG_DG_CAPT_POINT_HORDE_CONTROL;
                break;
            default:
                break;
        }

        capturePoint->CastSpell(capturePoint, addAura, true);

        // 2.3 Force update to players
        for (auto&& itr : m_Players)
        {
            if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                capturePoint->SendUpdateToPlayer(player);

        }
    }
}

void BattlegroundDG::SendNodeUpdate(uint8 node)
{
    // Node map
    UpdateWorldState(BG_DG_NodesWs[node][m_prevNodes[node]].m_WorldState, 0);
    UpdateWorldState(BG_DG_NodesWs[node][m_nodes[node]].m_WorldState, BG_DG_NodesWs[node][m_nodes[node]].m_SentValueToShow);

    // How many bases each team owns
    uint8 ally = 0, horde = 0;
    for (uint8 i = 0; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        if (m_nodes[i] == BG_DG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_nodes[i] == BG_DG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;
    }

    UpdateWorldState(WORLDSTATE_DG_OCCUPIED_BASES_ALLIANCE, ally);
    UpdateWorldState(WORLDSTATE_DG_OCCUPIED_BASES_HORDE, horde);
}

bool BattlegroundDG::CanSeeSpellClick(Player const* player, Unit const* clicked)
{
    for (int i = 0; i < BG_DG_ALL_NODES_COUNT; ++i)
    {
        if (clicked->GetGUID() == BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + i])
        {
            TeamId teamIndex = GetTeamIndexByTeamId(player->GetBGTeam());
            // Check if player really could use this banner, not cheated
            // Horde team give 0 as rest of modulo (resp. 1 for ally), but horde is 1 as a team (resp. 0 for ally)
            // So if teamIndex != m_nodes[node] % 2 means that ur team HAS the flag
            if (m_nodes[i] != BG_DG_NODE_TYPE_NEUTRAL && teamIndex != m_nodes[i] % 2)
                return false;

            return true;
        }
    }

    return true;
}

void BattlegroundDG::EventPlayerClickedOnFlag(Player* source, Unit* flag)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 node = BG_DG_NODE_GOBLIN_MINE;
    Creature* capturePoint = GetBgMap()->GetCreature(BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + node]);
    while ((node < BG_DG_ALL_NODES_COUNT) && ((!capturePoint) || (!source->IsWithinDistInMap(capturePoint, 10))))
    {
        ++node;
        capturePoint = GetBgMap()->GetCreature(BgCreatures[BG_DG_OBJECT_CAPT_POINT_START + node]);
    }

    // This means our player isn't close to any of banners - maybe cheater ??
    if (node == BG_DG_ALL_NODES_COUNT)
        return;

    TeamId teamIndex = GetTeamIndexByTeamId(source->GetBGTeam());

    // Check if player really could use this banner, not cheated
    // Horde team give 0 as rest of modulo (resp. 1 for ally), but horde is 1 as a team (resp. 0 for ally)
    // So if teamIndex != m_nodes[node] % 2 means that ur team HAS the flag
    if (m_nodes[node] != BG_DG_NODE_TYPE_NEUTRAL && teamIndex != m_nodes[node] % 2)
        return;

    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    uint32 soundID = 0;
    // If node is neutral, change to contested
    if (m_nodes[node] == BG_DG_NODE_TYPE_NEUTRAL)
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_nodes[node];
        m_nodes[node] = BG_DG_NODE_TYPE_CONTESTED + teamIndex;
        // Create new contested banner
        ChangeBanner(node, BG_DG_NODE_TYPE_CONTESTED);
        SendNodeUpdate(node);
        m_nodeTimers[node] = BG_DG_FLAG_CAPTURING_TIME;
        // Warn players
        SendMessage2ToAll(LANG_BG_DG_NODE_CLAIMED, ChatMsg(CHAT_MSG_BG_SYSTEM_ALLIANCE + teamIndex), source, GetNodeNameId(node), LANG_BG_DG_ALLY + teamIndex);
        soundID = BG_DG_SOUND_NODE_CLAIMED;
    }
    // If node is contested
    else if ((m_nodes[node] == BG_DG_NODE_STATUS_ALLY_CONTESTED) || (m_nodes[node] == BG_DG_NODE_STATUS_HORDE_CONTESTED))
    {
        // If last state is NOT occupied, change node to enemy-contested
        if (m_prevNodes[node] < BG_DG_NODE_TYPE_OCCUPIED)
        {
            UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
            m_prevNodes[node] = m_nodes[node];
            m_nodes[node] = BG_DG_NODE_TYPE_CONTESTED + teamIndex;
            // Create new contested banner
            ChangeBanner(node, BG_DG_NODE_TYPE_CONTESTED);
            SendNodeUpdate(node);
            m_nodeTimers[node] = BG_DG_FLAG_CAPTURING_TIME;

            SendMessage2ToAll(LANG_BG_DG_NODE_CLAIMED, ChatMsg(CHAT_MSG_BG_SYSTEM_ALLIANCE + teamIndex), source, GetNodeNameId(node), LANG_BG_DG_ALLY + teamIndex);
        }
        // If contested, change back to occupied
        else
        {
            UpdatePlayerScore(source, SCORE_BASES_DEFENDED, 1);
            m_prevNodes[node] = m_nodes[node];
            m_nodes[node] = BG_DG_NODE_TYPE_OCCUPIED + teamIndex;
            // Create new occupied banner
            ChangeBanner(node, BG_DG_NODE_TYPE_OCCUPIED);
            SendNodeUpdate(node);
            m_nodeTimers[node] = 0;

            SendMessage2ToAll(LANG_BG_DG_NODE_DEFENDED, ChatMsg(CHAT_MSG_BG_SYSTEM_ALLIANCE + teamIndex), source, GetNodeNameId(node));
        }
        soundID = (teamIndex == TEAM_ALLIANCE) ? BG_DG_SOUND_NODE_ASSAULTED_ALLIANCE : BG_DG_SOUND_NODE_ASSAULTED_HORDE;
    }
    // If node is occupied, change to enemy-contested
    else
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_nodes[node];
        m_nodes[node] = teamIndex + BG_DG_NODE_TYPE_CONTESTED;
        // Create new contested banner
        ChangeBanner(node, BG_DG_NODE_TYPE_CONTESTED);
        SendNodeUpdate(node);
        m_nodeTimers[node] = BG_DG_FLAG_CAPTURING_TIME;

        SendMessage2ToAll(LANG_BG_DG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_NEUTRAL, source, GetNodeNameId(node));

        soundID = (teamIndex == TEAM_ALLIANCE) ? BG_DG_SOUND_NODE_ASSAULTED_ALLIANCE : BG_DG_SOUND_NODE_ASSAULTED_HORDE;
    }

    // If node is occupied again, send "X has taken the Y" msg.
    if (m_nodes[node] >= BG_DG_NODE_TYPE_OCCUPIED)
        SendMessage2ToAll(LANG_BG_DG_NODE_TAKEN, ChatMsg(CHAT_MSG_BG_SYSTEM_ALLIANCE + teamIndex), source, LANG_BG_DG_ALLY + teamIndex, GetNodeNameId(node));

    PlaySoundToAll(soundID);
}

void BattlegroundDG::PostUpdateImplFlags(uint32 diff)
{
    int teamPoints[2] = { 0, 0 };

    for (int node = 0; node < BG_DG_ALL_NODES_COUNT; ++node)
    {
        // 1-minute to occupy a node from contested state
        ContestedTime(node, diff);

        for (int team = 0; team < 2; ++team)
        {
            if (m_nodes[node] == team + BG_DG_NODE_TYPE_OCCUPIED)
                ++teamPoints[team];
        }
    }

    // Accumulate points
    for (int team = 0; team < 2; ++team)
    {
        int points = teamPoints[team];
        if (!points)
            continue;

        m_lastTick[team] += diff;

        if (m_lastTick[team] > BG_DG_TickIntervals[points])
        {
            m_lastTick[team] -= BG_DG_TickIntervals[points];
            m_honorScoreTics[team] += BG_DG_TickPoints[points];
            if (m_honorScoreTics[team] >= m_honorTicks)
            {
                RewardHonorToTeam(GetBonusHonorFromKill(1), (team == TEAM_ALLIANCE) ? ALLIANCE : HORDE);
                m_honorScoreTics[team] -= m_honorTicks;
            }

            // Need to be last because there can be a last tick for honor before end (cart give 200, it's a lot)
            UpdateTeamScore(team, BG_DG_TickPoints[points]);
        }
    }
}

void BattlegroundDG::ContestedTime(int node, uint32 diff)
{
    if (m_nodeTimers[node] == 0)
        return;

    if (m_nodeTimers[node] > diff)
        m_nodeTimers[node] -= diff;
    else
    {
        m_nodeTimers[node] = 0;
        // Change from contested to occupied !
        uint8 teamIndex = m_nodes[node] - 1;
        m_prevNodes[node] = m_nodes[node];
        m_nodes[node] += 2;
        // Create new occupied banner
        ChangeBanner(node, BG_DG_NODE_TYPE_OCCUPIED);
        SendNodeUpdate(node);
        // Message to chat log
        SendMessage2ToAll(LANG_BG_DG_NODE_TAKEN, ChatMsg(CHAT_MSG_BG_SYSTEM_ALLIANCE + teamIndex), NULL, LANG_BG_DG_ALLY + teamIndex, GetNodeNameId(node));
        PlaySoundToAll((teamIndex == TEAM_ALLIANCE) ? BG_DG_SOUND_NODE_CAPTURED_ALLIANCE : BG_DG_SOUND_NODE_CAPTURED_HORDE);
    }
}

int32 BattlegroundDG::GetNodeNameId(uint8 node)
{
    switch (node)
    {
        case BG_DG_NODE_GOBLIN_MINE:
            return LANG_BG_DG_NODE_LIGHTHOUSE;
        case BG_DG_NODE_CENTER_MINE:
            return LANG_BG_DG_NODE_WATERWORKS;
        case BG_DG_NODE_PANDAREN_MINE:
            return LANG_BG_DG_NODE_MINE;
        default:
            break;
    }

    return 0;
}

/************************************************************************/
/*                          CARTS UPDATES                               */
/************************************************************************/
void BattlegroundDG::EventPlayerClickedOnFlag(Player* player, GameObject* gameObject)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    int32 messageID = 0;
    ChatMsg chatType = CHAT_MSG_BG_SYSTEM_NEUTRAL;

    // Alliance flag picked up from base
    if (player->GetBGTeam() == HORDE && GetFlagState(ALLIANCE) == BG_DG_CART_STATE_ON_BASE
        && BgObjects[BG_DG_OBJECT_CART_ALLIANCE] == gameObject->GetGUID())
    {
        messageID = LANG_BG_DG_CAPTURED_ALLIANCE_CART;
        chatType = CHAT_MSG_BG_SYSTEM_HORDE;
        PlaySoundToAll(BG_DG_SOUND_ALLIANCE_CART_PICKED_UP);
        SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE, RESPAWN_ONE_DAY);
        SetAllianceCartPicker(player->GetGUID());
        m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_PLAYER;
        // Update world state to show correct flag carrier
        UpdateCartState(HORDE, BG_DG_CART_STATE_ON_PLAYER);
        //UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, 1);
        player->CastSpell(player, BG_DG_ALLIANCE_MINE_CART, true);
        player->CastSpell(player, BG_DG_ALLIANCE_CART_HOLDER_AURA, true);
        m_flagGold[TEAM_ALLIANCE] = (m_TeamScores[TEAM_ALLIANCE] > 200) ? 200 : m_TeamScores[TEAM_ALLIANCE];
        UpdateTeamScore(TEAM_ALLIANCE, -m_flagGold[TEAM_ALLIANCE]);
    }

    // Horde flag picked up from base
    if (player->GetBGTeam() == ALLIANCE && GetFlagState(HORDE) == BG_DG_CART_STATE_ON_BASE
        && BgObjects[BG_DG_OBJECT_CART_HORDE] == gameObject->GetGUID())
    {
        messageID = LANG_BG_DG_CAPTURED_HORDE_CART;
        chatType = CHAT_MSG_BG_SYSTEM_ALLIANCE;
        PlaySoundToAll(BG_DG_SOUND_HORDE_CART_PICKED_UP);
        SpawnBGObject(BG_DG_OBJECT_CART_HORDE, RESPAWN_ONE_DAY);
        SetHordeFlagPicker(player->GetGUID());
        m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_PLAYER;
        // Update world state to show correct flag carrier
        UpdateCartState(ALLIANCE, BG_DG_CART_STATE_ON_PLAYER);
        //UpdateWorldState(BG_WS_FLAG_UNK_HORDE, 1);
        player->CastSpell(player, BG_DG_HORDE_MINE_CART, true);
        player->CastSpell(player, BG_DG_HORDE_CART_HOLDER_AURA, true);
        m_flagGold[TEAM_HORDE] = (m_TeamScores[TEAM_HORDE] > 200) ? 200 : m_TeamScores[TEAM_HORDE];
        UpdateTeamScore(TEAM_HORDE, -m_flagGold[TEAM_HORDE]);
    }

    // Alliance flag on ground(not in base) (returned or picked up again from ground!)
    if (GetFlagState(ALLIANCE) == BG_DG_CART_STATE_ON_GROUND && player->IsWithinDistInMap(gameObject, 10)
        && gameObject->GetGOInfo()->entry == BG_DG_OBJECTID_CART_ALLY_GROUND)
    {
        if (player->GetBGTeam() == ALLIANCE)
        {
            messageID = LANG_BG_DG_RETURNED_ALLIANCE_CART;
            chatType = CHAT_MSG_BG_SYSTEM_ALLIANCE;
            UpdateCartState(HORDE, BG_DG_CART_STATE_ON_BASE);
            m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_BASE;
            SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE, RESPAWN_IMMEDIATELY);
            PlaySoundToAll(BG_DG_SOUND_CART_RETURNED);
            UpdatePlayerScore(player, SCORE_CART_RETURNS, 1);
            //UpdateTeamScore(TEAM_ALLIANCE, m_flagGold[TEAM_ALLIANCE]);
            m_flagGold[TEAM_ALLIANCE] = 0;
        }
        else
        {
            messageID = LANG_BG_DG_CAPTURED_ALLIANCE_CART;
            chatType = CHAT_MSG_BG_SYSTEM_HORDE;
            PlaySoundToAll(BG_DG_SOUND_ALLIANCE_CART_PICKED_UP);
            SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE, RESPAWN_ONE_DAY);
            SetAllianceCartPicker(player->GetGUID());
            player->CastSpell(player, BG_DG_ALLIANCE_MINE_CART, true);
            player->CastSpell(player, BG_DG_ALLIANCE_CART_HOLDER_AURA, true);
            m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_PLAYER;
            UpdateCartState(HORDE, BG_DG_CART_STATE_ON_PLAYER);
            //UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, 1);
        }
        gameObject->Delete();
    }

    // Horde flag on ground(not in base) (returned or picked up again)
    if (GetFlagState(HORDE) == BG_DG_CART_STATE_ON_GROUND && player->IsWithinDistInMap(gameObject, 10)
        && gameObject->GetGOInfo()->entry == BG_DG_OBJECTID_CART_HORDE_GROUND)
    {
        if (player->GetBGTeam() == HORDE)
        {
            messageID = LANG_BG_DG_RETURNED_HORDE_CART;
            chatType = CHAT_MSG_BG_SYSTEM_HORDE;
            UpdateCartState(ALLIANCE, BG_DG_CART_STATE_ON_BASE);
            m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_BASE;
            SpawnBGObject(BG_DG_OBJECT_CART_HORDE, RESPAWN_IMMEDIATELY);
            PlaySoundToAll(BG_DG_SOUND_CART_RETURNED);
            UpdatePlayerScore(player, SCORE_CART_RETURNS, 1);
            //UpdateTeamScore(TEAM_HORDE, m_flagGold[TEAM_HORDE]);
            m_flagGold[TEAM_HORDE] = 0;
        }
        else
        {
            messageID = LANG_BG_DG_CAPTURED_HORDE_CART;
            chatType = CHAT_MSG_BG_SYSTEM_ALLIANCE;
            PlaySoundToAll(BG_DG_SOUND_HORDE_CART_PICKED_UP);
            SpawnBGObject(BG_DG_OBJECT_CART_HORDE, RESPAWN_ONE_DAY);
            SetHordeFlagPicker(player->GetGUID());
            player->CastSpell(player, BG_DG_HORDE_MINE_CART, true);
            player->CastSpell(player, BG_DG_HORDE_CART_HOLDER_AURA, true);
            m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_PLAYER;
            UpdateCartState(ALLIANCE, BG_DG_CART_STATE_ON_PLAYER);
            //UpdateWorldState(BG_WS_FLAG_UNK_HORDE, 1);
        }
        gameObject->Delete();
    }

    if (!messageID)
        return;

    SendMessageToAll(messageID, chatType, player);
    player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattlegroundDG::HandleAreaTrigger(Player* player, uint32 triggerId, bool entered)
{
    switch (triggerId)
    {
        case 9012: // Alliance cart spawn
            if (m_flagState[TEAM_HORDE])
            {
                if (GetFlagPickerGUID(TEAM_HORDE) == player->GetGUID())
                    EventPlayerCapturedFlag(player);
            }
            break;
        case 9013: // Horde cart spawn
            if (m_flagState[TEAM_ALLIANCE])
            {
                if (GetFlagPickerGUID(TEAM_ALLIANCE) == player->GetGUID())
                    EventPlayerCapturedFlag(player);
            }
            break;
        case 9139: // behind the wood on the spawn building alliance on the right
        case 9140: // inside building
        case 9159: // buff location
        case 9160: // buff location
        case 9161: // buff location
        case 9162: // buff location
        case 9299: // on the roof
        case 9301: // on the roof
        case 9302: // flying => should tp outside the mine when triggered
        case 9303: // flying => should tp outside the mine when triggered
            TC_LOG_DEBUG("bg.battleground", "BattlegroundDG : Handled AreaTrigger(ID : {}) have been activated by Player {} (ID : {})",
                triggerId, player->GetName().c_str(), player->GetGUID().GetCounter());
            break;
        default:
            Battleground::HandleAreaTrigger(player, triggerId, entered);
            break;
    }
}

void BattlegroundDG::PostUpdateImplCart(uint32 diff)
{
    if (m_flagState[TEAM_ALLIANCE] == BG_DG_CART_STATE_ON_GROUND)
    {
        m_flagsDropTimer[TEAM_ALLIANCE] -= diff;

        if (m_flagsDropTimer[TEAM_ALLIANCE] <= 0)
        {
            m_flagsDropTimer[TEAM_ALLIANCE] = 0;
            RespawnFlagAfterDrop(TEAM_ALLIANCE);
        }
    }

    if (m_flagState[TEAM_HORDE] == BG_DG_CART_STATE_ON_GROUND)
    {
        m_flagsDropTimer[TEAM_HORDE] -= diff;

        if (m_flagsDropTimer[TEAM_HORDE] <= 0)
        {
            m_flagsDropTimer[TEAM_HORDE] = 0;
            RespawnFlagAfterDrop(TEAM_HORDE);
        }
    }
}

void BattlegroundDG::RespawnFlagAfterDrop(uint32 team)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    m_flagState[team] = BG_DG_CART_STATE_ON_BASE;

    SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE + team, RESPAWN_IMMEDIATELY);

    //PlaySoundToAll(BG_DG_SOUND_CARTS_RESPAWNED);

    if (GameObject* go = GetBGObject(BG_DG_OBJECT_CART_ALLY_GROUND + team))
        go->Delete();
    else
        TC_LOG_ERROR("bg.battleground", "Battleground DG : unknown dropped flag bg");
}

void BattlegroundDG::EventPlayerCapturedFlag(Player* player)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint32 winnerID = 0;

    player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    if (player->GetBGTeam() == ALLIANCE)
    {
        if (!IsHordeFlagPickedup())
            return;

        SetHordeFlagPicker(ObjectGuid::Empty);                         // must be before aura remove to prevent 2 events (drop+capture) at the same time
        SendClearFlagsPositions();
        // horde flag in base (but not respawned yet)
        m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_BASE;
        UpdateTeamScore(TEAM_ALLIANCE, m_flagGold[TEAM_HORDE]);
        m_flagGold[TEAM_HORDE] = 0;
        // Drop Horde Flag from Player
        player->RemoveAurasDueToSpell(BG_DG_HORDE_MINE_CART);
        player->RemoveAurasDueToSpell(BG_DG_HORDE_CART_HOLDER_AURA);
        PlaySoundToAll(BG_DG_SOUND_CART_CAPTURED_ALLIANCE);
        SpawnBGObject(BG_DG_OBJECT_CART_HORDE, RESPAWN_IMMEDIATELY);
    }
    else
    {
        if (!IsAllianceFlagPickedup())
            return;

        SetAllianceCartPicker(ObjectGuid::Empty);                       // must be before aura remove to prevent 2 events (drop+capture) at the same time
        SendClearFlagsPositions();

        // alliance flag in base (but not respawned yet)
        m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_BASE;
        UpdateTeamScore(TEAM_HORDE, m_flagGold[TEAM_ALLIANCE]);
        m_flagGold[TEAM_ALLIANCE] = 0;

        // Drop Alliance Flag from Player
        player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_MINE_CART);
        player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_CART_HOLDER_AURA);

        PlaySoundToAll(BG_DG_SOUND_CART_CAPTURED_HORDE);
        SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE, RESPAWN_IMMEDIATELY);
    }
    // For flag capture is reward 2 honorable kills
    RewardHonorToTeam(GetBonusHonorFromKill(2), player->GetBGTeam());

    if (player->GetBGTeam() == ALLIANCE)
        SendMessageToAll(LANG_BG_DG_CAPTURED_HORDE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE, player);
    else
        SendMessageToAll(LANG_BG_DG_CAPTURED_ALLIANCE_CART, CHAT_MSG_BG_SYSTEM_HORDE, player);

    UpdateCartState(player->GetBGTeam(), BG_DG_CART_STATE_ON_BASE); // Flag state none
    // Only flag capture should be updated
    UpdatePlayerScore(player, SCORE_CART_CAPTURES, 1);      // +1 flag captures

    // Update last flag capture to be used if team score is equal
    SetLastFlagCapture(player->GetBGTeam());

    m_flagsTimer[GetTeamIndexByTeamId(player->GetBGTeam()) ? 0 : 1] = BG_DG_CART_RESPAWN_TIME;
}

void BattlegroundDG::EventPlayerDroppedFlag(Player* player)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
    {
        // If not running, do not cast things at the dropper player (prevent spawning the "dropped" flag), neither send unnecessary messages
        // Just take off the aura
        if (player->GetBGTeam() == ALLIANCE)
        {
            if (!IsHordeFlagPickedup())
                return;

            if (GetFlagPickerGUID(TEAM_HORDE) == player->GetGUID())
            {
                SetHordeFlagPicker(ObjectGuid::Empty);
                SendClearFlagsPositions();
                player->RemoveAurasDueToSpell(BG_DG_HORDE_MINE_CART);
                player->RemoveAurasDueToSpell(BG_DG_HORDE_CART_HOLDER_AURA);
            }
        }
        else
        {
            if (!IsAllianceFlagPickedup())
                return;

            if (GetFlagPickerGUID(TEAM_ALLIANCE) == player->GetGUID())
            {
                SetAllianceCartPicker(ObjectGuid::Empty);
                player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_MINE_CART);
                player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_CART_HOLDER_AURA);

            }
        }
        return;
    }

    bool set = false;

    if (player->GetBGTeam() == ALLIANCE)
    {
        if (!IsHordeFlagPickedup())
            return;

        if (GetFlagPickerGUID(TEAM_HORDE) == player->GetGUID())
        {
            SetHordeFlagPicker(ObjectGuid::Empty);
            SendClearFlagsPositions();
            player->RemoveAurasDueToSpell(BG_DG_HORDE_MINE_CART);
            player->RemoveAurasDueToSpell(BG_DG_HORDE_CART_HOLDER_AURA);
            m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_GROUND;
            UpdateTeamScore(TEAM_HORDE, m_flagGold[TEAM_HORDE]);
            if (AddObject(BG_DG_OBJECT_CART_HORDE_GROUND, BG_DG_OBJECTID_CART_HORDE_GROUND, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f))
            {
                SpawnBGObject(BG_DG_OBJECT_CART_HORDE_GROUND, RESPAWN_IMMEDIATELY);
                // Dunno how to cast spell on gob :/
                //GetBGObject(BG_DG_OBJECT_CART_HORDE_GROUND)->CastSpell(GetBGObject(BG_DG_OBJECT_CART_HORDE_GROUND), true);
            }
            m_flagsDropTimer[TEAM_HORDE] = BG_DG_CART_DROP_TIME;
            set = true;
        }
    }
    else
    {
        if (!IsAllianceFlagPickedup())
            return;

        if (GetFlagPickerGUID(TEAM_ALLIANCE) == player->GetGUID())
        {
            SetAllianceCartPicker(ObjectGuid::Empty);
            SendClearFlagsPositions();
            player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_MINE_CART);
            player->RemoveAurasDueToSpell(BG_DG_ALLIANCE_CART_HOLDER_AURA);
            m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_GROUND;
            UpdateTeamScore(TEAM_ALLIANCE, m_flagGold[TEAM_ALLIANCE]);
            if (AddObject(BG_DG_OBJECT_CART_ALLY_GROUND, BG_DG_OBJECTID_CART_ALLY_GROUND, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f))
            {
                SpawnBGObject(BG_DG_OBJECT_CART_ALLY_GROUND, RESPAWN_IMMEDIATELY);
                // Dunno how to cast spell on gob :/
                //GetBGObject(BG_DG_OBJECT_CART_ALLY_GROUND)->CastSpell(GetBGObject(BG_DG_OBJECT_CART_ALLY_GROUND), true);
            }
            m_flagsDropTimer[TEAM_ALLIANCE] = BG_DG_CART_DROP_TIME;
            set = true;
        }
    }

    if (set)
    {
        player->CastSpell(player, SPELL_RECENTLY_DROPPED_FLAG, true);
        UpdateCartState(player->GetBGTeam(), BG_DG_CART_STATE_ON_BASE);

        if (player->GetBGTeam() == ALLIANCE)
        {
            SendMessageToAll(LANG_BG_DG_DROPPED_HORDE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE, player);
            //UpdateWorldState(BG_WS_FLAG_UNK_HORDE, uint32(-1));
        }
        else
        {
            SendMessageToAll(LANG_BG_DG_DROPPED_ALLIANCE_CART, CHAT_MSG_BG_SYSTEM_HORDE, player);
            //UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, uint32(-1));
        }
    }
}

void BattlegroundDG::UpdateCartState(uint32 team, uint32 value)
{
    if (team == ALLIANCE)
        UpdateWorldState(WORLDSTATE_DG_CART_STATE_ALLIANCE, value);
    else
        UpdateWorldState(WORLDSTATE_DG_CART_STATE_HORDE, value);
}

/************************************************************************/
/*                        ENDING BATTLEGROUND                           */
/************************************************************************/
void BattlegroundDG::RemovePlayer(Player* player, ObjectGuid guid, uint32 team)
{
    Battleground::RemovePlayer(player, guid, team);
    // Sometimes flag aura not removed :(
    if (IsAllianceFlagPickedup() && m_flagKeepers[TEAM_ALLIANCE] == guid)
    {
        if (!player)
        {
            TC_LOG_ERROR("bg.battleground", "BattlegroundWS: Removing offline player who has the FLAG!!");
            SetAllianceCartPicker(ObjectGuid::Empty);
            SendClearFlagsPositions();
            m_flagState[TEAM_ALLIANCE] = BG_DG_CART_STATE_ON_BASE;
            SpawnBGObject(BG_DG_OBJECT_CART_ALLIANCE, RESPAWN_IMMEDIATELY);
        }
        else
            EventPlayerDroppedFlag(player);
    }

    if (IsHordeFlagPickedup() && m_flagKeepers[TEAM_HORDE] == guid)
    {
        if (!player)
        {
            TC_LOG_ERROR("bg.battleground", "BattlegroundWS: Removing offline player who has the FLAG!!");
            SetHordeFlagPicker(ObjectGuid::Empty);
            SendClearFlagsPositions();
            m_flagState[TEAM_HORDE] = BG_DG_CART_STATE_ON_BASE;
            SpawnBGObject(BG_DG_OBJECT_CART_HORDE, RESPAWN_IMMEDIATELY);
        }
        else
            EventPlayerDroppedFlag(player);
    }
}

