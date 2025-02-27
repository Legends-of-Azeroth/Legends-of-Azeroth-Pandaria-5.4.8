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

#include "BattlegroundTOK.h"
#include "Object.h"
#include "Player.h"
#include "Battleground.h"
#include "Creature.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "WorldPacket.h"
#include "Language.h"
#include "MapManager.h"

enum
{
    POWERBALL_CRITERIA      = 128768,   // 6950 - Powerball

    OBJECTIVE_EARN_SCORE    = 419,      // 6972 - What is Best in Life?
};

BattlegroundTOK::BattlegroundTOK()
{
    StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_TOK_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_TOK_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_TOK_HAS_BEGUN;

    BgObjects.resize(BG_TOK_OBJECT_MAX);
    BgCreatures.resize(BG_TOK_CREATURE_MAX);

    bool isBGWeekend = BattlegroundMgr::IsBGWeekend(GetTypeID());
    m_reputationCapture = (isBGWeekend) ? 45 : 35;
    m_honorWinKills = (isBGWeekend) ? 3 : 1;
    m_honorEndKills = (isBGWeekend) ? 4 : 2;
}

void BattlegroundTOK::PostUpdateImpl(uint32 diff)
{
    m_scheduler->Update(diff);

    for (auto&& itr : m_orbKeepers)
    {
        if (!itr.Guid)
            continue;

        if (m_playersZone[itr.Guid] == BG_TOK_ZONE_MILLDE)
        {
            itr.AchievementTimer += Milliseconds(diff);
            if (itr.AchievementTimer >= Seconds(90))
                if (Player* player = ObjectAccessor::FindPlayer(itr.Guid))
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, POWERBALL_CRITERIA);
        }
    }
}

void BattlegroundTOK::StartingEventCloseDoors()
{
    SpawnBGObject(BG_TOK_OBJECT_A_DOOR, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_TOK_OBJECT_H_DOOR, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_TOK_OBJECT_A_COLLISION, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_TOK_OBJECT_H_COLLISION, RESPAWN_IMMEDIATELY);

    DoorClose(BG_TOK_OBJECT_A_DOOR);
    DoorClose(BG_TOK_OBJECT_H_DOOR);
    DoorClose(BG_TOK_OBJECT_A_COLLISION);
    DoorClose(BG_TOK_OBJECT_H_COLLISION);

    for (uint8 i = 0; i < BG_TOK_MAX_ORBS; ++i)
        SpawnBGObject(BG_TOK_OBJECT_ORB_1 + i, RESPAWN_ONE_DAY);

    // Spawn Buffs
    for (uint32 i = 0; i < BG_TOK_MAX_BUFFS; i++)
        SpawnBGObject(BG_TOK_OBJECT_BUFF_NORTH + i, RESPAWN_IMMEDIATELY);

    m_scheduler->Schedule(Seconds(BATTLEGROUND_COUNTDOWN_MAX - 6), [=](TaskContext ctx)
    {
        DoorOpen(BG_TOK_OBJECT_A_DOOR);
        DoorOpen(BG_TOK_OBJECT_H_DOOR);
    });
}

void BattlegroundTOK::StartingEventOpenDoors()
{
    DoorOpen(BG_TOK_OBJECT_A_COLLISION);
    DoorOpen(BG_TOK_OBJECT_H_COLLISION);

    for (uint8 i = 0; i < BG_TOK_MAX_ORBS; ++i)
        SpawnBGObject(BG_TOK_OBJECT_ORB_1 + i, RESPAWN_IMMEDIATELY);

    m_scheduler->Schedule(Minutes(1), [=](TaskContext ctx)
    {
        --m_minutesRemain;
        UpdateWorldState(BG_TOK_TIME_REMAINING, m_minutesRemain);
        if (!m_minutesRemain)
        {
            uint32 allianceScore = GetTeamScore(ALLIANCE);
            uint32 hordeScore = GetTeamScore(HORDE);

            if (allianceScore > hordeScore)
                EndBattleground(ALLIANCE);
            else if (allianceScore < hordeScore)
                EndBattleground(HORDE);
            else // if 0 => tie
                EndBattleground(m_lastCapturedOrbTeam);
        }
        else
            ctx.Repeat();
    });

    // This is only for .bg start command, they must start opening 5 seconds before start
    DoorOpen(BG_TOK_OBJECT_A_DOOR);
    DoorOpen(BG_TOK_OBJECT_H_DOOR);
}

void BattlegroundTOK::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    BattlegroundTOKScore* sc = new BattlegroundTOKScore{};
    sc->Team = player->GetBGTeam();
    PlayerScores[player->GetGUID()] = sc;
    m_playersZone[player->GetGUID()] = BG_TOK_ZONE_OUT;
}

void BattlegroundTOK::EventPlayerClickedOnFlag(Player* source, GameObject* targetObj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!source->IsWithinDistInMap(targetObj, 10))
        return;

    uint32 index = targetObj->GetEntry() - BG_TOK_OBJECT_ORB_1_ENTRY;

    if (index >= BG_TOK_MAX_ORBS)
        return;

    if (m_orbKeepers[index].Guid)    // GO can be double clicked, it's okay
        return;

    ObjectGuid guid = source->GetGUID();

    if (HasOrb(guid))
        return;

    m_orbKeepers[index].Guid = source->GetGUID();
    m_orbKeepers[index].AchievementTimer = TimeValue::zero();

    m_scheduler->Schedule(Seconds(5), index, [=](TaskContext ctx)
    {
        if (m_orbKeepers[index].Guid != guid)
            return;
        if (Player* keeper = ObjectAccessor::FindPlayer(guid))
        {
            UpdateScore(keeper);
            ctx.Repeat();
        }
    });

    TakeOrb(source, index);
}

void BattlegroundTOK::TakeOrb(Player* player, uint8 index)
{
    bool alliance = player->GetBGTeam() == ALLIANCE;

    PlaySoundToAll(alliance ? BG_TOK_SOUND_A_ORB_PICKED_UP : BG_TOK_SOUND_H_ORB_PICKED_UP);

    player->CastSpell(player, alliance ? BG_TOK_ALLIANCE_INSIGNIA : BG_TOK_HORDE_INSIGNIA, true);
    player->CastSpell(player, m_orbSpells[index], true);

    UpdatePlayerScore(player, SCORE_ORB_HANDLES, 1);

    UpdateWorldState(OrbsWorldStates[index], 0);

    UpdateWorldState(BG_TOK_ICON_A, 1);
    SpawnBGObject(BG_TOK_OBJECT_ORB_1 + index, RESPAWN_ONE_DAY);

    if (Creature* aura = GetBGCreature(BG_TOK_CREATURE_ORB_AURA_1 + index))
        aura->RemoveAllAuras();

    SendMessageToAll(LANG_BG_TOK_PICKEDUP_BLUE + index, alliance ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, player);
    player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattlegroundTOK::ReleaseOrb(Player* player)
{
    uint8 index = GetOrbIndex(player->GetGUID());
    if (index == BG_TOK_MAX_ORBS)
        return;

    bool alliance = player->GetBGTeam() == ALLIANCE;

    PlaySoundToAll(alliance ? BG_TOK_SOUND_A_ORB_PICKED_UP : BG_TOK_SOUND_H_ORB_PICKED_UP);
    player->RemoveAurasDueToSpell(BG_TOK_ALLIANCE_INSIGNIA);
    player->RemoveAurasDueToSpell(BG_TOK_HORDE_INSIGNIA);

    m_orbKeepers[index].Guid.Clear();
    m_scheduler->CancelGroup(index);

    UpdateWorldState(OrbsWorldStates[index], 1);

    SpawnBGObject(BG_TOK_OBJECT_ORB_1 + index, RESPAWN_IMMEDIATELY);

    if (Creature* aura = GetBGCreature(BG_TOK_CREATURE_ORB_AURA_1 + index))
        aura->AddAura(m_orbAuras[index], aura);

    UpdateWorldState(BG_TOK_ICON_A, 0);
    SendMessageToAll(LANG_BG_TOK_DROPPED_BLUE + index, alliance ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, player);
    player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattlegroundTOK::RemovePlayer(Player* player, ObjectGuid guid, uint32 team)
{
    Battleground::RemovePlayer(player, guid, team);
    if (!player)
        return;

    ReleaseOrb(player);
}

void BattlegroundTOK::UpdateOrbState(Team team, uint32 value)
{
    uint32 worldState = team == ALLIANCE ? BG_TOK_ICON_A : BG_TOK_ICON_H;
    UpdateWorldState(worldState, GetTeamScore(team));
}

void BattlegroundTOK::UpdateTeamScore(Team team)
{
    uint32 worldState = team == ALLIANCE ? BG_TOK_ORB_POINTS_A : BG_TOK_ORB_POINTS_H;
    UpdateWorldState(worldState, GetTeamScore(team));
}

void BattlegroundTOK::HandleAreaTrigger(Player* player, uint32 trigger, bool entered)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    ObjectGuid playerGuid = player->GetGUID();
    // GM teleported to bg
    if (m_Players.find(playerGuid) == m_Players.end())
        return;

    auto itr = m_playersZone.find(playerGuid);
    ASSERT(itr != m_playersZone.end());

    switch(trigger)
    {
        case 7734: // Out-In trigger
        {
            itr->second = itr->second == BG_TOK_ZONE_OUT ? BG_TOK_ZONE_IN : BG_TOK_ZONE_OUT;
            break;
        }
        case 7735: // Middle-In trigger
        {
            itr->second = itr->second == BG_TOK_ZONE_IN ? BG_TOK_ZONE_MILLDE : BG_TOK_ZONE_IN;
            break;
        }
        default:
            Battleground::HandleAreaTrigger(player, trigger, entered);
            break;
    }

    auto index = GetOrbIndex(playerGuid);
    if (index != BG_TOK_MAX_ORBS)
        m_orbKeepers[index].AchievementTimer = TimeValue::zero();
}

bool BattlegroundTOK::SetupBattleground()
{
    // Doors
    if (!AddObject(BG_TOK_OBJECT_A_DOOR, BG_TOK_OBJECT_DOOR_ENTRY, m_doorPositions[0][0], m_doorPositions[0][1], m_doorPositions[0][2], m_doorPositions[0][3], 0, 0, sin(m_doorPositions[0][3]/2), cos(m_doorPositions[0][3]/2), RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_TOK_OBJECT_H_DOOR, BG_TOK_OBJECT_DOOR_ENTRY, m_doorPositions[1][0], m_doorPositions[1][1], m_doorPositions[1][2], m_doorPositions[1][3], 0, 0, sin(m_doorPositions[1][3]/2), cos(m_doorPositions[1][3]/2), RESPAWN_IMMEDIATELY))
        return false;

    // Collisions
    if (!AddObject(BG_TOK_OBJECT_A_COLLISION, BG_TOK_OBJECT_COL_ENTRY, m_doorPositions[0][0], m_doorPositions[0][1], m_doorPositions[0][2], m_doorPositions[0][3], 0, 0, sin(m_doorPositions[0][3] / 2), cos(m_doorPositions[0][3] / 2), RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_TOK_OBJECT_H_COLLISION, BG_TOK_OBJECT_COL_ENTRY, m_doorPositions[1][0], m_doorPositions[1][1], m_doorPositions[1][2], m_doorPositions[1][3], 0, 0, sin(m_doorPositions[1][3] / 2), cos(m_doorPositions[1][3] / 2), RESPAWN_IMMEDIATELY))
        return false;

    if (!AddSpiritGuide(BG_TOK_CREATURE_SPIRIT_1, m_spiritPositions[0][0], m_spiritPositions[0][1], m_spiritPositions[0][2], m_spiritPositions[0][3], ALLIANCE) ||
        !AddSpiritGuide(BG_TOK_CREATURE_SPIRIT_2, m_spiritPositions[1][0], m_spiritPositions[1][1], m_spiritPositions[1][2], m_spiritPositions[1][3], HORDE))
        return false;

    // Buffs
    for (int i = 0; i < BG_TOK_MAX_BUFFS; i++)
    {
        if (!AddObject(BG_TOK_OBJECT_BUFF_NORTH + i, Buff_Entries[2], m_buffPositions[i][0], m_buffPositions[i][1], m_buffPositions[i][2], m_buffPositions[i][3], 0, 0, std::sin(m_buffPositions[i][3] / 2), std::cos(m_buffPositions[i][3] / 2), BUFF_RESPAWN_TIME))
        {
            TC_LOG_ERROR("bg.battleground", "BatteGroundKT: Failed to spawn buff object. Battleground not created!");
            return false;
        }
    }
    // Orbs
    for (uint8 i = 0; i < BG_TOK_MAX_ORBS; ++i)
    {
        if (!AddObject(BG_TOK_OBJECT_ORB_1 + i, BG_TOK_OBJECT_ORB_1_ENTRY + i, m_orbPositions[i][0], m_orbPositions[i][1], m_orbPositions[i][2], m_orbPositions[i][3], 0, 0, sin(m_orbPositions[i][3]/2), cos(m_orbPositions[i][3]/2), RESPAWN_ONE_DAY))
            return false;

        if (Creature* trigger = AddCreature(WORLD_TRIGGER, BG_TOK_CREATURE_ORB_AURA_1 + i, TEAM_NEUTRAL, m_orbPositions[i][0], m_orbPositions[i][1], m_orbPositions[i][2], m_orbPositions[i][3], RESPAWN_IMMEDIATELY))
            trigger->AddAura(m_orbAuras[i], trigger);
    }

    return true;
}

void BattlegroundTOK::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    ReleaseOrb(player);

    Battleground::HandleKillPlayer(player, killer);

    killer->CastSpell(killer, BG_TOK_10_VICTORY_POINTS);
    AddScore(killer, 10);
}

void BattlegroundTOK::UpdatePlayerScore(Player* source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found
        return;

    switch(type)
    {
        case SCORE_ORB_HANDLES:                           // orb handles
            ((BattlegroundTOKScore*)itr->second)->OrbHandles += value;
            break;
        case SCORE_ORB_SCORE:
            ((BattlegroundTOKScore*)itr->second)->Score += value;
            break;
        default:
            Battleground::UpdatePlayerScore(source, type, value, doAddHonor);
            break;
    }
}

WorldSafeLocsEntry const* BattlegroundTOK::GetClosestGraveYard(Player* player)
{
    //if status in progress, it returns main graveyards with spiritguides
    //else it will return the graveyard in the flagroom - this is especially good
    //if a player dies in preparation phase - then the player can't cheat
    //and teleport to the graveyard outside the flagroom
    //and start running around, while the doors are still closed
    if (player->GetBGTeam() == ALLIANCE)
    {
        if (GetStatus() == STATUS_IN_PROGRESS)
            return sWorldSafeLocsStore.LookupEntry(BG_TOK_GRAVEYARD_RECTANGLEA1);
        else
            return sWorldSafeLocsStore.LookupEntry(BG_TOK_GRAVEYARD_RECTANGLEA2);
    }
    else
    {
        if (GetStatus() == STATUS_IN_PROGRESS)
            return sWorldSafeLocsStore.LookupEntry(GB_TOK_GRAVEYARD_RECTANGLEH1);
        else
            return sWorldSafeLocsStore.LookupEntry(BG_TOK_GRAVEYARD_RECTANGLEH2);
    }
}

void BattlegroundTOK::EndBattleground(uint32 winner)
{
    for (auto&& itr : GetPlayers())
    {
        if (itr.second.Team == winner)
        {
            Player* player = ObjectAccessor::FindPlayer(itr.first);
            if (player)
                player->KilledMonsterCredit(68931);
        }
    }

    Battleground::EndBattleground(winner);
}

void BattlegroundTOK::UpdateScore(Player* player)
{
    auto it = m_playersZone.find(player->GetGUID());
    ASSERT(it != m_playersZone.end());

    uint32 zone = it->second;

    uint32 const points[] = { 3, 4, 5 };
    uint32 const spells[] = { 112912, 112913, 112914 };

    AddScore(player, points[zone]);
    player->CastSpell(player, spells[zone]);

    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_EARN_SCORE, points[zone]);
}

void BattlegroundTOK::AddScore(Player* player, uint32 value)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    uint32 team = player->GetBGTeam() == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE;

    m_TeamScores[team] += value;

    if (m_TeamScores[team] > BG_TOK_MAX_TEAM_SCORE)
        m_TeamScores[team] = BG_TOK_MAX_TEAM_SCORE;

    UpdatePlayerScore(player, SCORE_ORB_SCORE, value);
    UpdateTeamScore(Team(player->GetBGTeam()));

    // Test win condition
    if (m_TeamScores[TEAM_ALLIANCE] >= BG_TOK_MAX_TEAM_SCORE)
        EndBattleground(ALLIANCE);
    else if (m_TeamScores[TEAM_HORDE] >= BG_TOK_MAX_TEAM_SCORE)
        EndBattleground(HORDE);
}

uint32 BattlegroundTOK::GetRBGLoserReward(uint32 team) const
{
    return (m_TeamScores[GetTeamIndexByTeamId(team)] * 200 / 1500) * 100;
}

void BattlegroundTOK::FillInitialWorldStates(WorldStateBuilder& builder)
{
    builder.AppendState(uint32(BG_TOK_ORB_POINTS_A), GetTeamScore(ALLIANCE));
    builder.AppendState(uint32(BG_TOK_ORB_POINTS_H), GetTeamScore(HORDE));

    /*if (m_OrbState[BG_TEAM_ALLIANCE] == BG_TOK_ORB_STATE_ON_GROUND)
        builder.AppendState(BG_TOK_ICON_A, -1);
    else if (m_OrbState[BG_TEAM_ALLIANCE] == BG_TOK_ORB_STATE_ON_PLAYER)
        builder.AppendState(BG_TOK_ICON_A, 1);
    else
        builder.AppendState(BG_TOK_ICON_A, 0);

    if (m_OrbState[BG_TEAM_HORDE] == BG_TOK_ORB_STATE_ON_GROUND)
        builder.AppendState(BG_TOK_ICON_H, -1);
    else if (m_OrbState[BG_TEAM_HORDE] == BG_TOK_ORB_STATE_ON_PLAYER)
        builder.AppendState(BG_TOK_ICON_H, 1);
    else
        builder.AppendState(BG_TOK_ICON_H, 0);*/

    builder.AppendState(BG_TOK_ORB_POINTS_MAX, BG_TOK_MAX_TEAM_SCORE);

    /*if (m_OrbState[BG_TEAM_HORDE] == BG_TOK_ORB_STATE_ON_PLAYER)
        builder.AppendState(BG_TOK_ORB_STATE, 2);
    else
        builder.AppendState(BG_TOK_ORB_STATE, 1);

    if (m_OrbState[BG_TEAM_ALLIANCE] == BG_TOK_ORB_STATE_ON_PLAYER)
        builder.AppendState(BG_TOK_ORB_STATE, 2);
    else
        builder.AppendState(BG_TOK_ORB_STATE, 1);*/

    builder.AppendState(BG_TOK_TIME_ENABLED, 1);
    builder.AppendState(BG_TOK_TIME_REMAINING, m_minutesRemain);

    for (int i = 0; i < BG_TOK_MAX_ORBS; i++)
        builder.AppendState(OrbsWorldStates[i], m_orbKeepers[i].Guid != 0 ? 0 : 1);
}
