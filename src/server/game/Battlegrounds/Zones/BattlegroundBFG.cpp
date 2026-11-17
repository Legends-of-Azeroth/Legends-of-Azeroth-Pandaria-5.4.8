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

#include "Battleground.h"
#include "BattlegroundBFG.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"

// Achievements
enum
{
    EVENT_START_BATTLE_BFG              = 9158, // 5254 - Newbs to Plowshares

    WORLD_STATE_DONT_GET_COCKY_KID_H    = 5809, // 5252 - Don't Get Cocky Kid
    WORLD_STATE_DONT_GET_COCKY_KID_A    = 5810, // 5252 - Don't Get Cocky Kid
    WORLD_STATE_BFG_PERFECTION          = 5716, // 5247 - Battle for Gilneas Perfection
    WORLD_STATE_FULL_COVERAGE           = 8656, // 5253 - Full Coverage
    WORLD_STATE_JUGGER_NOT              = 9316, // 5255 - Jugger Not
};

BattlegroundBFG::BattlegroundBFG()
{
    m_BuffChange = true;
    BgObjects.resize(BG_BFG_OBJECT_MAX);
    BgCreatures.resize(BG_BFG_ALL_NODES_COUNT + 3); // +3 for aura triggers

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_BFG_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_BFG_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_BFG_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_BFG_HAS_BEGUN;
}

void BattlegroundBFG::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        int team_points[BG_TEAMS_COUNT] = { 0, 0 };

        for (int node = 0; node < BG_BFG_DYNAMIC_NODES_COUNT; ++node)
        {
            // 3 sec delay to spawn new a banner.
            if (m_bannerTimers[node].Timer)
            {
                if (m_bannerTimers[node].Timer > diff)
                    m_bannerTimers[node].Timer -= diff;
                else
                {
                    m_bannerTimers[node].Timer = 0;
                    CreateBanner(node, m_bannerTimers[node].Type, m_bannerTimers[node].TeamIndex, false);
                }
            }

            // 1-minute cap timer on each node from a contested state.
            if (m_nodeTimers[node])
            {
                if (m_nodeTimers[node] > diff)
                    m_nodeTimers[node] -= diff;
                else
                {
                    m_nodeTimers[node] = 0;

                    // Change from contested to occupied !
                    uint8 teamIndex = m_nodes[node] - 1;
                    m_prevNodes[node] = m_nodes[node];
                    m_nodes[node] += 2;

                    // burn current contested banner
                    DelBanner(node, BG_BFG_NODE_TYPE_CONTESTED, teamIndex);

                    // create new occupied banner
                    CreateBanner(node, BG_BFG_NODE_TYPE_OCCUPIED, teamIndex, true);
                    SendNodeUpdate(node);
                    NodeOccupied(node, (teamIndex == 0) ? ALLIANCE : HORDE);

                    // Message to chatlog
                    if (teamIndex == 0)
                    {
                        // FIXME: need to fix Locales for team and nodes names.
                        SendMessage2ToAll(LANG_BG_BFG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL, LANG_BG_BFG_ALLY, GetNodeNameId(node));
                        PlaySoundToAll(BG_BFG_SOUND_NODE_CAPTURED_ALLIANCE);
                    }
                    else
                    {
                        // FIXME: team and node names not localized
                        SendMessage2ToAll(LANG_BG_BFG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_HORDE, NULL, LANG_BG_BFG_HORDE, GetNodeNameId(node));
                        PlaySoundToAll(BG_BFG_SOUND_NODE_CAPTURED_HORDE);
                    }
                }
            }

            for (int team = 0; team < BG_TEAMS_COUNT; ++team)
                if (m_nodes[node] == team + BG_BFG_NODE_TYPE_OCCUPIED)
                    ++team_points[team];
        }

        // Accumulate points
        for (int team = 0; team < BG_TEAMS_COUNT; ++team)
        {
            int points = team_points[team];
            if (!points)
                continue;

            m_lastTick[team] += diff;
            if (m_lastTick[team] > BG_BFG_TickIntervals[points])
            {
                m_lastTick[team] -= BG_BFG_TickIntervals[points];
                m_TeamScores[team] += BG_BFG_TickPoints[points];
                m_honorScoreTicks[team] += BG_BFG_TickPoints[points];
                m_reputationScoreTicks[team] += BG_BFG_TickPoints[points];

                if (m_reputationScoreTicks[team] >= m_reputationTicks)
                {
                    RewardReputationToTeam(509, 510, 10, (team == TEAM_ALLIANCE) ? ALLIANCE : HORDE);
                    m_reputationScoreTicks[team] -= m_reputationTicks;
                }

                if (m_honorScoreTicks[team] >= m_honorTicks)
                {
                    RewardHonorToTeam(GetBonusHonorFromKill(3), (team == TEAM_ALLIANCE) ? ALLIANCE : HORDE);
                    m_honorScoreTicks[team] -= m_honorTicks;
                }

                if (!m_isInformedNearVictory && m_TeamScores[team] > BG_BFG_WARNING_NEAR_VICTORY_SCORE)
                {
                    if (team == TEAM_ALLIANCE)
                        SendMessageToAll(LANG_BG_BFG_A_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    else
                        SendMessageToAll(LANG_BG_BFG_H_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);

                    PlaySoundToAll(BG_BFG_SOUND_NEAR_VICTORY);
                    m_isInformedNearVictory = true;
                }

                if (m_TeamScores[team] > BG_BFG_MAX_TEAM_SCORE)
                    m_TeamScores[team] = BG_BFG_MAX_TEAM_SCORE;

                if (team == TEAM_ALLIANCE)
                    UpdateWorldState(BG_BFG_OP_RESOURCES_ALLY, m_TeamScores[team]);

                if (team == TEAM_HORDE)
                    UpdateWorldState(BG_BFG_OP_RESOURCES_HORDE, m_TeamScores[team]);
                // update achievement flags
                // we increased m_TeamScores[team] so we just need to check if it is 500 more than other teams resources
                uint8 otherTeam = (team + 1) % BG_TEAMS_COUNT;
                if (m_TeamScores[team] > m_TeamScores[otherTeam] + 500)
                    GetBgMap()->SetWorldState(otherTeam == TEAM_ALLIANCE ? WORLD_STATE_DONT_GET_COCKY_KID_A : WORLD_STATE_DONT_GET_COCKY_KID_H, 1);
                int32 scoreDiff = std::abs(m_TeamScores[TEAM_ALLIANCE] - m_TeamScores[TEAM_HORDE]);
                GetBgMap()->SetWorldState(WORLD_STATE_JUGGER_NOT, scoreDiff <= 100 ? 1 : 0);
            }
        }

        // Test win condition
        if (m_TeamScores[TEAM_ALLIANCE] >= BG_BFG_MAX_TEAM_SCORE)
        {
            GetBgMap()->SetWorldState(WORLD_STATE_DONT_GET_COCKY_KID_H, 0);
            if (m_TeamScores[TEAM_HORDE] == 0)
                GetBgMap()->SetWorldState(WORLD_STATE_BFG_PERFECTION, 1);
            if (IsAllNodesControlledByTeam(TEAM_ALLIANCE))
                GetBgMap()->SetWorldState(WORLD_STATE_FULL_COVERAGE, 1);
            EndBattleground(ALLIANCE);
        }

        if (m_TeamScores[TEAM_HORDE] >= BG_BFG_MAX_TEAM_SCORE)
        {
            GetBgMap()->SetWorldState(WORLD_STATE_DONT_GET_COCKY_KID_A, 0);
            if (m_TeamScores[TEAM_ALLIANCE] == 0)
                GetBgMap()->SetWorldState(WORLD_STATE_BFG_PERFECTION, 1);
            if (IsAllNodesControlledByTeam(HORDE))
                GetBgMap()->SetWorldState(WORLD_STATE_FULL_COVERAGE, 1);
            EndBattleground(HORDE);
        }
    }
}

void BattlegroundBFG::StartingEventCloseDoors()
{
    // Remove banners, auras and buffs
    for (int object = BG_BFG_OBJECT_BANNER_NEUTRAL; object < BG_BFG_DYNAMIC_NODES_COUNT * 8; ++object)
        SpawnBGObject(object, RESPAWN_ONE_DAY);
    for (int i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT * 3; ++i)
        SpawnBGObject(BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE + i, RESPAWN_ONE_DAY);

    // Starting doors
    DoorClose(BG_BFG_OBJECT_GATE_A_1);
    DoorClose(BG_BFG_OBJECT_GATE_H_1);

    // Starting base spirit guides
    NodeOccupied(BG_BFG_SPIRIT_ALIANCE, ALLIANCE);
    NodeOccupied(BG_BFG_SPIRIT_HORDE, HORDE);
}

void BattlegroundBFG::StartingEventOpenDoors()
{
    for (int banner = BG_BFG_OBJECT_BANNER_NEUTRAL, i = 0; i < 3; banner += 8, ++i)
        SpawnBGObject(banner, RESPAWN_IMMEDIATELY);
    for (int i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
    {
        uint8 buff = urand(0, 2);
        SpawnBGObject(BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE + buff + i * 3, RESPAWN_IMMEDIATELY);
    }
    DoorOpen(BG_BFG_OBJECT_GATE_A_1);
    DoorOpen(BG_BFG_OBJECT_GATE_H_1);

    StartCriteria(CRITERIA_START_TYPE_EVENT, EVENT_START_BATTLE_BFG);
}

void BattlegroundBFG::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    // Create score and add it to map, default values are set in constructor
    BattlegroundBFGScore* score = new BattlegroundBFGScore{};
    score->Team = player->GetBGTeam();

    PlayerScores[player->GetGUID()] = score;

    if (GetStatus() == STATUS_IN_PROGRESS)
        player->StartCriteria(CRITERIA_START_TYPE_EVENT, EVENT_START_BATTLE_BFG, (TimeValue::Now() - m_doorOpeningTime).ToMilliseconds());
}

void BattlegroundBFG::RemovePlayer(Player* player, ObjectGuid guid, uint32 team)
{
    Battleground::RemovePlayer(player, guid, team);
}

void BattlegroundBFG::CreateBanner(uint8 node, uint8 type, uint8 teamIndex, bool delay)
{
    // Just put it into the queue
    if (delay)
    {
        m_bannerTimers[node].Timer = 2000;
        m_bannerTimers[node].Type = type;
        m_bannerTimers[node].TeamIndex = teamIndex;
        return;
    }

    uint8 object = node * 8 + type + teamIndex;

    SpawnBGObject(object, RESPAWN_IMMEDIATELY);

    // Handle banner and auras
    if (!type)
        return;

    object = node * 8 + ((type == BG_BFG_NODE_TYPE_OCCUPIED) ? (3 + teamIndex) : 7);
    SpawnBGObject(object, RESPAWN_IMMEDIATELY);
}

void BattlegroundBFG::DelBanner(uint8 node, uint8 type, uint8 teamIndex)
{
    uint8 object = node * 8 + type + teamIndex;
    SpawnBGObject(object, RESPAWN_ONE_DAY);

    // Handle banner and auras
    if (!type)
        return;

    object = node * 8 + ((type == BG_BFG_NODE_TYPE_OCCUPIED) ? (3 + teamIndex) : 7);
    SpawnBGObject(object, RESPAWN_ONE_DAY);
}

int32 BattlegroundBFG::GetNodeNameId(uint8 node)
{
    switch (node)
    {
        case BG_BFG_NODE_LIGHTHOUSE: return LANG_BG_BFG_NODE_LIGHTHOUSE;
        case BG_BFG_NODE_WATERWORKS: return LANG_BG_BFG_NODE_WATERWORKS;
        case BG_BFG_NODE_MINE: return LANG_BG_BFG_NODE_MINE;
        default:
            ASSERT(0);
    }
    return 0;
}

void BattlegroundBFG::FillInitialWorldStates(WorldStateBuilder& builder)
{
    const uint8 plusArray[] = { 0, 2, 3, 0, 1 };

    // Node icons
    for (uint8 node = 0; node < BG_BFG_DYNAMIC_NODES_COUNT; ++node)
        builder.AppendState(uint32(BG_BFG_OP_NODEICONS[node]), uint32((m_nodes[node] == 0) ? 1 : 0));

    // Node occupied states
    for (uint8 node = 0; node < BG_BFG_DYNAMIC_NODES_COUNT; ++node)
        for (uint8 i = 1; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
            builder.AppendState(uint32(BG_BFG_OP_NODESTATES[node] + plusArray[i]), uint32((m_nodes[node] == i) ? 1 : 0));

    // How many bases each team owns
    uint8 ally = 0, horde = 0;
    for (uint8 node = 0; node < BG_BFG_DYNAMIC_NODES_COUNT; ++node)
        if (m_nodes[node] == BG_BFG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_nodes[node] == BG_BFG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;

    builder.AppendState(uint32(BG_BFG_OP_OCCUPIED_BASES_ALLY), uint32(ally));
    builder.AppendState(uint32(BG_BFG_OP_OCCUPIED_BASES_HORDE), uint32(horde));

    // Team scores
    builder.AppendState(uint32(BG_BFG_OP_RESOURCES_MAX), uint32(BG_BFG_MAX_TEAM_SCORE));
    builder.AppendState(uint32(BG_BFG_OP_RESOURCES_WARNING), uint32(BG_BFG_WARNING_NEAR_VICTORY_SCORE));
    builder.AppendState(uint32(BG_BFG_OP_RESOURCES_ALLY), uint32(m_TeamScores[TEAM_ALLIANCE]));
    builder.AppendState(uint32(BG_BFG_OP_RESOURCES_HORDE), uint32(m_TeamScores[TEAM_HORDE]));

    // other unknown
    //builder.AppendState(uint32(0x745), uint32(0x2));           // 37 1861 unk
}

void BattlegroundBFG::SendNodeUpdate(uint8 node)
{
    // Send to client owner node state updates to refresh map icons.
    const uint8 plusArray[] = { 0, 2, 3, 0, 1 };

    if (m_prevNodes[node])
        UpdateWorldState(BG_BFG_OP_NODESTATES[node] + plusArray[m_prevNodes[node]], 0);
    else
        UpdateWorldState(BG_BFG_OP_NODEICONS[node], 0);

    UpdateWorldState(BG_BFG_OP_NODESTATES[node] + plusArray[m_nodes[node]], 1);

    // How many bases each team owns
    uint8 ally = 0, horde = 0;

    for (uint8 i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
        if (m_nodes[i] == BG_BFG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_nodes[i] == BG_BFG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;

    UpdateWorldState(BG_BFG_OP_OCCUPIED_BASES_ALLY, ally);
    UpdateWorldState(BG_BFG_OP_OCCUPIED_BASES_HORDE, horde);
}

void BattlegroundBFG::NodeOccupied(uint8 node, Team team)
{
    if (!AddSpiritGuide(node, BG_BFG_SpiritGuidePos[node][0], BG_BFG_SpiritGuidePos[node][1], BG_BFG_SpiritGuidePos[node][2], BG_BFG_SpiritGuidePos[node][3], team))
        TC_LOG_ERROR("bg.battleground", "Failed to spawn spirit guide! point: {}, team: {}, ", node, team);

    uint8 capturedNodes = 0;
    for (uint8 i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (m_nodes[node] == GetTeamIndexByTeamId(team) + BG_BFG_NODE_TYPE_OCCUPIED && !m_nodeTimers[i])
            ++capturedNodes;
    }

    if (node >= BG_BFG_DYNAMIC_NODES_COUNT) // only dynamic nodes, no start points
        return;

    Creature* trigger = GetBGCreature(node + 5); // 0-5 spirit guides

    if (!trigger)
        trigger = AddCreature(WORLD_TRIGGER, node + 5, team, BG_BFG_NodePositions[node][0], BG_BFG_NodePositions[node][1], BG_BFG_NodePositions[node][2], BG_BFG_NodePositions[node][3]);

    // Add bonus honor aura trigger creature when node is occupied
    // Cast bonus aura (+50% honor in 25yards)
    // aura should only apply to players who have occupied the node, set correct faction for trigger
    if (trigger)
    {
        trigger->SetFaction(team == ALLIANCE ? 84 : 83);
        trigger->CastSpell(trigger, SPELL_HONORABLE_DEFENDER_25Y, false);
    }
}

void BattlegroundBFG::NodeDeOccupied(uint8 node)
{
    if (node >= BG_BFG_DYNAMIC_NODES_COUNT)
        return;

    // Remove bonus honor aura trigger bunny when node is lost
    if (node < BG_BFG_DYNAMIC_NODES_COUNT)  // Only dynamic nodes, no start points
        DelCreature(node + 5);                    // NULL checks are in DelCreature! 0-5 spirit guides

                                                  // Players waiting to resurrect at this node are sent to closest owned graveyard
    std::vector<ObjectGuid> ghost_list = m_ReviveQueue[BgCreatures[node]];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const *ClosestGrave = NULL;
        for (std::vector<ObjectGuid>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!ClosestGrave)
                ClosestGrave = GetClosestGraveYard(player);

            if (ClosestGrave)
                player->TeleportTo(GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
        }
    }

    if (BgCreatures[node])
        DelCreature(node);

    // Buff object is not removed
}

/* Invoked if a player used a banner as a GameObject */
void BattlegroundBFG::EventPlayerClickedOnFlag(Player* source, GameObject* /*target_obj*/)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 node = BG_BFG_NODE_LIGHTHOUSE;
    GameObject* object = GetBgMap()->GetGameObject(BgObjects[node * 8 + 5]);
    while ((node < BG_BFG_DYNAMIC_NODES_COUNT) && ((!object) || (!source->IsWithinDistInMap(object, 10))))
    {
        ++node;
        object = GetBgMap()->GetGameObject(BgObjects[node * 8 + BG_BFG_OBJECT_AURA_CONTESTED]);
    }

    if (node == BG_BFG_DYNAMIC_NODES_COUNT)
    {
        // this means our player isn't close to any of banners - maybe cheater ??
        return;
    }

    TeamId teamIndex = GetTeamIndexByTeamId(source->GetBGTeam());

    // Check if player really could use this banner, and has not cheated
    if (!(m_nodes[node] == 0 || teamIndex == m_nodes[node] % 2))
        return;

    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    uint32 sound = 0;
    // If node is neutral, change to contested
    if (m_nodes[node] == BG_BFG_NODE_TYPE_NEUTRAL)
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_nodes[node];
        m_nodes[node] = teamIndex + 1;

        // burn current neutral banner
        DelBanner(node, BG_BFG_NODE_TYPE_NEUTRAL, 0);

        // create new contested banner
        CreateBanner(node, BG_BFG_NODE_TYPE_CONTESTED, teamIndex, true);
        SendNodeUpdate(node);
        m_nodeTimers[node] = BG_BFG_FLAG_CAPTURING_TIME;

        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == 0)
            SendMessage2ToAll(LANG_BG_BFG_NODE_CLAIMED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, GetNodeNameId(node), LANG_BG_BFG_ALLY);
        else
            SendMessage2ToAll(LANG_BG_BFG_NODE_CLAIMED, CHAT_MSG_BG_SYSTEM_HORDE, source, GetNodeNameId(node), LANG_BG_BFG_HORDE);

        sound = BG_BFG_SOUND_NODE_CLAIMED;
    }
    // If node is contested
    else if ((m_nodes[node] == BG_BFG_NODE_STATUS_ALLY_CONTESTED) || (m_nodes[node] == BG_BFG_NODE_STATUS_HORDE_CONTESTED))
    {
        // If last state is NOT occupied, change node to enemy-contested
        if (m_prevNodes[node] < BG_BFG_NODE_TYPE_OCCUPIED)
        {
            UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
            m_prevNodes[node] = m_nodes[node];
            m_nodes[node] = teamIndex + BG_BFG_NODE_TYPE_CONTESTED;

            // burn current contested banner
            DelBanner(node, BG_BFG_NODE_TYPE_CONTESTED, !teamIndex);

            // create new contested banner
            CreateBanner(node, BG_BFG_NODE_TYPE_CONTESTED, teamIndex, true);
            SendNodeUpdate(node);
            m_nodeTimers[node] = BG_BFG_FLAG_CAPTURING_TIME;

            // FIXME: need to fix Locales for team and node names.
            if (teamIndex == TEAM_ALLIANCE)
                SendMessage2ToAll(LANG_BG_BFG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, GetNodeNameId(node));
            else
                SendMessage2ToAll(LANG_BG_BFG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_HORDE, source, GetNodeNameId(node));
        }
        // If contested, change back to occupied
        else
        {
            UpdatePlayerScore(source, SCORE_BASES_DEFENDED, 1);
            m_prevNodes[node] = m_nodes[node];
            m_nodes[node] = teamIndex + BG_BFG_NODE_TYPE_OCCUPIED;

            // burn current contested banner
            DelBanner(node, BG_BFG_NODE_TYPE_CONTESTED, !teamIndex);

            // create new occupied banner
            CreateBanner(node, BG_BFG_NODE_TYPE_OCCUPIED, teamIndex, true);
            SendNodeUpdate(node);
            m_nodeTimers[node] = 0;
            NodeOccupied(node, (teamIndex == TEAM_ALLIANCE) ? ALLIANCE : HORDE);

            // FIXME: need to fix Locales for team and node names.
            if (teamIndex == TEAM_ALLIANCE)
                SendMessage2ToAll(LANG_BG_BFG_NODE_DEFENDED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, GetNodeNameId(node));
            else
                SendMessage2ToAll(LANG_BG_BFG_NODE_DEFENDED, CHAT_MSG_BG_SYSTEM_HORDE, source, GetNodeNameId(node));
        }
        sound = (teamIndex == TEAM_ALLIANCE) ? BG_BFG_SOUND_NODE_ASSAULTED_ALLIANCE : BG_BFG_SOUND_NODE_ASSAULTED_HORDE;
    }
    // If node is occupied, change to enemy-contested
    else
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_nodes[node];
        m_nodes[node] = teamIndex + BG_BFG_NODE_TYPE_CONTESTED;

        // burn current occupied banner
        DelBanner(node, BG_BFG_NODE_TYPE_OCCUPIED, !teamIndex);

        // create new contested banner
        CreateBanner(node, BG_BFG_NODE_TYPE_CONTESTED, teamIndex, true);
        SendNodeUpdate(node);
        NodeDeOccupied(node);
        m_nodeTimers[node] = BG_BFG_FLAG_CAPTURING_TIME;

        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == TEAM_ALLIANCE)
            SendMessage2ToAll(LANG_BG_BFG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, GetNodeNameId(node));
        else
            SendMessage2ToAll(LANG_BG_BFG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_HORDE, source, GetNodeNameId(node));

        sound = (teamIndex == TEAM_ALLIANCE) ? BG_BFG_SOUND_NODE_ASSAULTED_ALLIANCE : BG_BFG_SOUND_NODE_ASSAULTED_HORDE;
    }

    // If node is occupied again, send "X has taken the Y" msg.
    if (m_nodes[node] >= BG_BFG_NODE_TYPE_OCCUPIED)
    {
        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == TEAM_ALLIANCE)
            SendMessage2ToAll(LANG_BG_BFG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL, LANG_BG_BFG_ALLY, GetNodeNameId(node));
        else
            SendMessage2ToAll(LANG_BG_BFG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_HORDE, NULL, LANG_BG_BFG_HORDE, GetNodeNameId(node));
    }
    PlaySoundToAll(sound);
}

bool BattlegroundBFG::SetupBattleground()
{
    for (int i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (!AddObject(BG_BFG_OBJECT_BANNER_NEUTRAL + 8 * i, BG_BFG_OBJECTID_NODE_BANNER_0 + i, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_BANNER_CONT_A + 8 * i, BG_BFG_OBJECTID_BANNER_CONT_A, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_BANNER_CONT_H + 8 * i, BG_BFG_OBJECTID_BANNER_CONT_H, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_BANNER_ALLY + 8 * i, BG_BFG_OBJECTID_BANNER_A, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_BANNER_HORDE + 8 * i, BG_BFG_OBJECTID_BANNER_H, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_AURA_ALLY + 8 * i, BG_BFG_OBJECTID_AURA_A, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_AURA_HORDE + 8 * i, BG_BFG_OBJECTID_AURA_H, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_AURA_CONTESTED + 8 * i, BG_BFG_OBJECTID_AURA_C, BG_BFG_NodePositions[i][0], BG_BFG_NodePositions[i][1], BG_BFG_NodePositions[i][2], BG_BFG_NodePositions[i][3], 0, 0, sin(BG_BFG_NodePositions[i][3] / 2), cos(BG_BFG_NodePositions[i][3] / 2), RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR("bg.battleground", "BattleForGilneas: Can't Create Some Object");
            return false;
        }
    }

    if (!AddObject(BG_BFG_OBJECT_GATE_A_1, BG_BFG_OBJECTID_GATE_A_1, BG_BFG_DoorPositions[0][0], BG_BFG_DoorPositions[0][1], BG_BFG_DoorPositions[0][2], BG_BFG_DoorPositions[0][3], BG_BFG_DoorPositions[0][4], BG_BFG_DoorPositions[0][5], BG_BFG_DoorPositions[0][6], BG_BFG_DoorPositions[0][7], RESPAWN_IMMEDIATELY)
        || !AddObject(BG_BFG_OBJECT_GATE_H_1, BG_BFG_OBJECTID_GATE_H_1, BG_BFG_DoorPositions[2][0], BG_BFG_DoorPositions[2][1], BG_BFG_DoorPositions[2][2], BG_BFG_DoorPositions[2][3], BG_BFG_DoorPositions[2][4], BG_BFG_DoorPositions[2][5], BG_BFG_DoorPositions[2][6], BG_BFG_DoorPositions[2][7], RESPAWN_IMMEDIATELY))
    {
        TC_LOG_ERROR("bg.battleground", "BattleForGilneas: Can't Create Doors");
        return false;
    }

    // Buffs
    for (int i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (!AddObject(BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i, Buff_Entries[0], BG_BFG_BuffPositions[i][0], BG_BFG_BuffPositions[i][1], BG_BFG_BuffPositions[i][2], BG_BFG_BuffPositions[i][3], 0, 0, sin(BG_BFG_BuffPositions[i][3] / 2), cos(BG_BFG_BuffPositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i + 1, Buff_Entries[1], BG_BFG_BuffPositions[i][0], BG_BFG_BuffPositions[i][1], BG_BFG_BuffPositions[i][2], BG_BFG_BuffPositions[i][3], 0, 0, sin(BG_BFG_BuffPositions[i][3] / 2), cos(BG_BFG_BuffPositions[i][3] / 2), RESPAWN_ONE_DAY)
            || !AddObject(BG_BFG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i + 2, Buff_Entries[2], BG_BFG_BuffPositions[i][0], BG_BFG_BuffPositions[i][1], BG_BFG_BuffPositions[i][2], BG_BFG_BuffPositions[i][3], 0, 0, sin(BG_BFG_BuffPositions[i][3] / 2), cos(BG_BFG_BuffPositions[i][3] / 2), RESPAWN_ONE_DAY))
            TC_LOG_ERROR("bg.battleground", "BattleForGilneas: Can't Create Buffs");
    }

    return true;
}

void BattlegroundBFG::Reset()
{
    //call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE] = 0;
    m_TeamScores[TEAM_HORDE] = 0;
    m_lastTick[TEAM_ALLIANCE] = 0;
    m_lastTick[TEAM_HORDE] = 0;
    m_honorScoreTicks[TEAM_ALLIANCE] = 0;
    m_honorScoreTicks[TEAM_HORDE] = 0;
    m_reputationScoreTicks[TEAM_ALLIANCE] = 0;
    m_reputationScoreTicks[TEAM_HORDE] = 0;
    m_isInformedNearVictory = false;
    bool isBGWeekend = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_honorTicks = (isBGWeekend) ? BG_BFG_BGWeekendHonorTicks : BG_BFG_NotBGWeekendHonorTicks;
    m_reputationTicks = (isBGWeekend) ? BG_BFG_BGWeekendRepTicks : BG_BFG_NotBGWeekendRepTicks;

    for (uint8 i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
    {
        m_nodes[i] = 0;
        m_prevNodes[i] = 0;
        m_nodeTimers[i] = 0;
        m_bannerTimers[i].Timer = 0;
    }

    for (uint8 i = 0; i < BG_BFG_ALL_NODES_COUNT + 3; ++i)// +3 for aura triggers
        if (BgCreatures[i])
            DelCreature(i);
}

void BattlegroundBFG::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());

    if (itr == PlayerScores.end())      // player was not found...
        return;

    switch (type)
    {
        case SCORE_BASES_ASSAULTED:
            ((BattlegroundBFGScore*)itr->second)->BasesAssaulted += value;
            Source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, BG_OBJECTIVE_ASSAULT_BASE, 1);
            break;
        case SCORE_BASES_DEFENDED:
            ((BattlegroundBFGScore*)itr->second)->BasesDefended += value;
            Source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, BG_OBJECTIVE_DEFEND_BASE, 1);
            break;
        default:
            Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
            break;
    }
}

WorldSafeLocsEntry const* BattlegroundBFG::GetClosestGraveYard(Player* player)
{
    TeamId teamIndex = GetTeamIndexByTeamId(player->GetBGTeam());

    // Is there any occupied node for this team?
    std::vector<uint8> nodes;
    for (uint8 i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
        if (m_nodes[i] == teamIndex + 3)
            nodes.push_back(i);

    WorldSafeLocsEntry const* good_entry = NULL;

    // If so, select the closest node to place ghost on
    if (!nodes.empty())
    {
        float player_x = player->GetPositionX();
        float player_y = player->GetPositionY();

        float mindist = 999999.0f; // Temp Hack
        for (uint8 i = 0; i < nodes.size(); ++i)
        {
            WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(BG_BFG_GraveyardIds[nodes[i]]);

            if (!entry)
                continue;

            float dist = (entry->x - player_x)*(entry->x - player_x) + (entry->y - player_y)*(entry->y - player_y);

            if (mindist > dist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }
        nodes.clear();
    }

    // If not, place ghost on starting location
    if (!good_entry)
        good_entry = sWorldSafeLocsStore.LookupEntry(BG_BFG_GraveyardIds[teamIndex + 3]);

    return good_entry;
}

bool BattlegroundBFG::IsAllNodesControlledByTeam(uint32 team) const
{
    uint32 count = 0;
    for (int i = 0; i < BG_BFG_DYNAMIC_NODES_COUNT; ++i)
        if ((team == ALLIANCE && m_nodes[i] == BG_BFG_NODE_STATUS_ALLY_OCCUPIED) || (team == HORDE && m_nodes[i] == BG_BFG_NODE_STATUS_HORDE_OCCUPIED))
            ++count;

    return count == BG_BFG_DYNAMIC_NODES_COUNT;
}

uint32 BattlegroundBFG::GetRBGLoserReward(uint32 team) const
{
    return (m_TeamScores[GetTeamIndexByTeamId(team)] * 200 / 1900) * 100;
}
