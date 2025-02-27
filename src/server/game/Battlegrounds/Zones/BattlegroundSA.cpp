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

#include "BattlegroundSA.h"
#include "Language.h"
#include "Player.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Chat.h"

// Achievements
enum
{
    WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_A = 6450,
    WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_H = 6451,
    WORLD_STATE_NOT_EVEN_A_SCRATCH_A      = 6449,
    WORLD_STATE_NOT_EVEN_A_SCRATCH_H      = 6448,
    WORLD_STATE_STORM_THE_BEACH_A         = 5668,
    WORLD_STATE_STORM_THE_BEACH_H         = 5669,
};

BattlegroundSA::BattlegroundSA()
{
    // temp hack for ships
    StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SA_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SA_HAS_BEGUN;
    BgObjects.resize(BG_SA_MAXOBJ);
    BgCreatures.resize(BG_SA_MAXNPC + BG_SA_MAX_GY);
    TimerEnabled = false;
    UpdateWaitTimer = 0;
    EndTimestamp = 0;
    SignaledRoundTwo = false;
    SignaledRoundTwoHalfMin = false;
    InitSecondRound = false;

    //! This is here to prevent an uninitialised variable warning
    //! The warning only occurs when SetUpBattleGround fails though.
    //! In the future this function should be called BEFORE sending initial worldstates.
    memset(&GraveyardStatus, 0, sizeof(GraveyardStatus));
}

BattlegroundSA::~BattlegroundSA() { }

void BattlegroundSA::Reset()
{
    TotalTime = 0;
    Attackers = ((urand(0, 1)) ? TEAM_ALLIANCE : TEAM_HORDE);
    for (uint8 i = 0; i <= 5; i++)
        GateStatus[i] = BG_SA_GATE_OK;
    ShipsStarted = false;
    gateDestroyed = false;
    _allVehiclesAlive[TEAM_ALLIANCE] = true;
    _allVehiclesAlive[TEAM_HORDE] = true;
    Status = BG_SA_WARMUP;
}

bool BattlegroundSA::SetupBattleground()
{
    return ResetObjs();
}

bool BattlegroundSA::ResetObjs()
{
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            SendTransportsRemove(player);

    uint32 atF = BG_SA_Factions[Attackers];
    uint32 defF = BG_SA_Factions[Attackers ? TEAM_ALLIANCE : TEAM_HORDE];

    for (uint8 i = 0; i < BG_SA_MAXOBJ; i++)
        DelObject(i);

    for (uint8 i = 0; i < BG_SA_MAXNPC; i++)
        DelCreature(i);

    for (uint8 i = BG_SA_MAXNPC; i < BG_SA_MAXNPC + BG_SA_MAX_GY; i++)
        DelCreature(i);

    for (uint8 i = 0; i < 6; i++)
        GateStatus[i] = BG_SA_GATE_OK;

    for (uint8 i = 0; i <= BG_SA_PORTAL_DEFFENDER_RED; i++)
    {
        if (!AddObject(i, BG_SA_ObjEntries[i], BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, RESPAWN_ONE_DAY))
            return false;
    }

    for (uint8 i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
    {
        uint32 boatid = 0;
        switch (i)
        {
            case BG_SA_BOAT_ONE:
                boatid = Attackers ? BG_SA_BOAT_ONE_H : BG_SA_BOAT_ONE_A;
                break;
            case BG_SA_BOAT_TWO:
                boatid = Attackers ? BG_SA_BOAT_TWO_H : BG_SA_BOAT_TWO_A;
                break;
            default:
                break;
        }
        if (!AddObject(i, boatid, BG_SA_ObjSpawnlocs[i][0],
          BG_SA_ObjSpawnlocs[i][1],
          BG_SA_ObjSpawnlocs[i][2]+(Attackers ? -3.750f: 0),
          BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, RESPAWN_ONE_DAY))
            return false;
    }

    for (uint8 i = BG_SA_SIGIL_1; i <= BG_SA_LEFT_FLAGPOLE; i++)
    {
        if (!AddObject(i, BG_SA_ObjEntries[i],
            BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
            BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
            0, 0, 0, 0, RESPAWN_ONE_DAY))
        return false;
    }

    // MAD props for Kiper for discovering those values - 4 hours of his work.
    GetBGObject(BG_SA_BOAT_ONE)->SetParentRotation({ 0.0f, 0.0f, 1.0f, 0.0002f });
    GetBGObject(BG_SA_BOAT_TWO)->SetParentRotation({ 0.0f, 0.0f, 1.0f, 0.00001f });

    //Cannons and demolishers - NPCs are spawned
    //By capturing GYs.
    for (uint8 i = 0; i < BG_SA_DEMOLISHER_5; i++)
    {
        if (!AddCreature(BG_SA_NpcEntries[i], i, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
              BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1],
              BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3], i >= BG_SA_GUN_1 && i <= BG_SA_GUN_10 ? 1 : 600))
        return false;
    }

    // Hack to make cannons not miss because for some reason SPELL_ATTR3_IGNORE_HIT_RESULT is handled after miss chance handling for SPELL_DAMAGE_CLASS_MAGIC spells
    for (uint32 i = BG_SA_GUN_1; i <= BG_SA_GUN_10; ++i)
        if (Creature* cannon = GetBGCreature(i))
            cannon->m_modSpellHitChance = 1000.0f;

    OverrideGunFaction();
    DemolisherStartState(true);

    for (uint8 i = 0; i <= BG_SA_PORTAL_DEFFENDER_RED; i++)
    {
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
        GetBGObject(i)->SetFaction(defF);
    }

    GetBGObject(BG_SA_TITAN_RELIC)->SetFaction(atF);
    GetBGObject(BG_SA_TITAN_RELIC)->Refresh();

    for (uint8 i = 0; i <= 5; i++)
        GateStatus[i] = BG_SA_GATE_OK;

    TotalTime = 0;
    ShipsStarted = false;

    //Graveyards
    for (uint8 i = 0; i < BG_SA_MAX_GY; i++)
    {
        WorldSafeLocsEntry const* sg = NULL;
        sg = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);

        if (!sg)
        {
            TC_LOG_ERROR("bg.battleground", "SOTA: Can't find GY entry %u", BG_SA_GYEntries[i]);
            return false;
        }

        if (i == BG_SA_BEACH_GY)
        {
            GraveyardStatus[i] = Attackers;
            AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], ((Attackers == TEAM_HORDE)? HORDE : ALLIANCE));
        }
        else
        {
            GraveyardStatus[i] = ((Attackers == TEAM_HORDE)? TEAM_ALLIANCE : TEAM_HORDE);
            if (!AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], ((Attackers == TEAM_HORDE)? ALLIANCE : HORDE)))
                TC_LOG_ERROR("bg.battleground", "SOTA: couldn't spawn GY: %u", i);
        }
    }

    //GY capture points
    for (uint8 i = BG_SA_CENTRAL_FLAG; i <= BG_SA_LEFT_FLAG; i++)
    {
        AddObject(i, (BG_SA_ObjEntries[i] - (Attackers == TEAM_ALLIANCE ? 1 : 0)),
            BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
            BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
            0, 0, 0, 0, RESPAWN_ONE_DAY);
        GetBGObject(i)->SetFaction(atF);
    }

    UpdateBombSpawns();

    //Player may enter BEFORE we set up BG - lets update his worldstates anyway...
    UpdateWorldState(BG_SA_RIGHT_GY_HORDE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    UpdateWorldState(BG_SA_LEFT_GY_HORDE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    UpdateWorldState(BG_SA_CENTER_GY_HORDE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);

    UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);

    if (Attackers == TEAM_ALLIANCE)
    {
        UpdateWorldState(BG_SA_ALLY_ATTACKS, 1);
        UpdateWorldState(BG_SA_HORDE_ATTACKS, 0);

        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_ALL, 1);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_ALL, 1);
        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_HRD, 0);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_HRD, 0);

        UpdateWorldState(BG_SA_HORDE_DEFENCE_TOKEN, 1);
        UpdateWorldState(BG_SA_ALLIANCE_DEFENCE_TOKEN, 0);
    }
    else
    {
        UpdateWorldState(BG_SA_HORDE_ATTACKS, 1);
        UpdateWorldState(BG_SA_ALLY_ATTACKS, 0);

        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_ALL, 0);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_ALL, 0);
        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_HRD, 1);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_HRD, 1);

        UpdateWorldState(BG_SA_HORDE_DEFENCE_TOKEN, 0);
        UpdateWorldState(BG_SA_ALLIANCE_DEFENCE_TOKEN, 1);
    }

    UpdateWorldState(BG_SA_PURPLE_GATEWS, 1);
    UpdateWorldState(BG_SA_RED_GATEWS, 1);
    UpdateWorldState(BG_SA_BLUE_GATEWS, 1);
    UpdateWorldState(BG_SA_GREEN_GATEWS, 1);
    UpdateWorldState(BG_SA_YELLOW_GATEWS, 1);
    UpdateWorldState(BG_SA_ANCIENT_GATEWS, 1);

    for (int i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                SendTransportInit(player);

    // set status manually so preparation is cast correctly in 2nd round too
    SetStatus(STATUS_WAIT_JOIN);

    TeleportPlayers();
    return true;
}

void BattlegroundSA::StartShips()
{
    if (ShipsStarted)
        return;

    //DoorOpen(BG_SA_BOAT_ONE);
    //DoorOpen(BG_SA_BOAT_TWO);
    //
    //for (int i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
    //{
    //    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    //    {
    //        if (Player* p = ObjectAccessor::FindPlayer(itr->first))
    //        {
    //            UpdateData data(p->GetMapId());
    //            WorldPacket pkt;
    //            GetBGObject(i)->BuildValuesUpdateBlockForPlayer(&data, p);
    //            data.BuildPacket(&pkt);
    //            p->SendDirectMessage(&pkt);
    //        }
    //    }
    //}
    ShipsStarted = true;
}

void BattlegroundSA::PostUpdateImpl(uint32 diff)
{
    if (InitSecondRound)
    {
        if (UpdateWaitTimer < diff)
        {
            if (!SignaledRoundTwo)
            {
                SignaledRoundTwo = true;
                InitSecondRound = false;
                SendMessageToAll(LANG_BG_SA_ROUND_TWO_ONE_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            }
        }
        else
        {
            UpdateWaitTimer -= diff;
            return;
        }
    }
    TotalTime += diff;

    if (Status == BG_SA_WARMUP)
    {
        EndRoundTimer = BG_SA_ROUNDLENGTH;
        if (TotalTime >= BG_SA_WARMUPLENGTH)
        {
            TotalTime = 0;
            ToggleTimer();
            DemolisherStartState(false);
            Status = BG_SA_ROUND_ONE;
            StartCriteria(CRITERIA_START_TYPE_EVENT, (Attackers == TEAM_ALLIANCE) ? 23748 : 21702);

            EndTimestamp = time(nullptr) + BG_SA_ROUNDLENGTH / IN_MILLISECONDS;
            UpdateWorldState(BG_SA_ENABLE_TIMER, 1);
            UpdateWorldState(BG_SA_TIMER, EndTimestamp);
        }
        if (TotalTime >= BG_SA_BOAT_START)
            StartShips();
        if (TotalTime >= 59000) // temp hack for ships
        {
            for (auto&& itr : GetPlayers())
            {
                if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                    if (player->GetBGTeamId() == Attackers && (player->IsInWater() || player->IsUnderWater() || player->GetTransport()))
                    {
                        if (player->GetPositionY() < 0) // left
                            player->TeleportTo(607, 1598.571f, -106.671f, 8.873f, 4.1359f, 0);
                        else // right
                            player->TeleportTo(607, 1607.596f, 49.766f, 7.5795f, 2.31865f, 0);
                    }
            }
        }
        return;
    }
    else if (Status == BG_SA_SECOND_WARMUP)
    {
        if (RoundScores[0].time<BG_SA_ROUNDLENGTH)
            EndRoundTimer = RoundScores[0].time;
        else
            EndRoundTimer = BG_SA_ROUNDLENGTH;

        if (TotalTime >= 60000)
        {
            SendWarningToAll(LANG_BG_SA_HAS_BEGUN);
            TotalTime = 0;
            ToggleTimer();
            DemolisherStartState(false);
            gateDestroyed = false;
            Status = BG_SA_ROUND_TWO;

            EndTimestamp = time(nullptr) + EndRoundTimer / IN_MILLISECONDS;
            UpdateWorldState(BG_SA_ENABLE_TIMER, 1);
            UpdateWorldState(BG_SA_TIMER, EndTimestamp);

            StartCriteria(CRITERIA_START_TYPE_EVENT, (Attackers == TEAM_ALLIANCE) ? 23748 : 21702);
            // status was set to STATUS_WAIT_JOIN manually for Preparation, set it back now
            SetStatus(STATUS_IN_PROGRESS);
            for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (Player* p = ObjectAccessor::FindPlayer(itr->first))
                    p->RemoveAurasDueToSpell(SPELL_PREPARATION);
        }
        if (TotalTime >= 30000)
        {
            if (!SignaledRoundTwoHalfMin)
            {
                SignaledRoundTwoHalfMin = true;
                SendMessageToAll(LANG_BG_SA_ROUND_TWO_START_HALF_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            }
        }
        StartShips();
        if (TotalTime >= 53000) // temp hack for ships
        {
            for (auto&& itr : GetPlayers())
            {
                if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                    if (player->GetBGTeamId() == Attackers && (player->IsInWater() || player->IsUnderWater() || player->GetTransport()))
                    {
                        if (player->GetPositionY() < 0) // left
                            player->TeleportTo(607, 1598.571f, -106.671f, 8.873f, 4.1359f, 0);
                        else // right
                            player->TeleportTo(607, 1607.596f, 49.766f, 7.5795f, 2.31865f, 0);
                    }
            }
        }
        return;
    }
    else if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (Status == BG_SA_ROUND_ONE)
        {
            if (TotalTime >= BG_SA_ROUNDLENGTH)
            {
                // Defense of the Ancients (1757 / 2200)
                if (!gateDestroyed)
                    GetBgMap()->SetWorldState(GetDefenerTeam() == TEAM_ALLIANCE ? WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_A : WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_H, 1);

                CastSpellOnTeam(SPELL_END_OF_ROUND, ALLIANCE);
                CastSpellOnTeam(SPELL_END_OF_ROUND, HORDE);

                RoundScores[0].winner = Attackers;
                RoundScores[0].time = BG_SA_ROUNDLENGTH;
                TotalTime = 0;
                Status = BG_SA_SECOND_WARMUP;
                EndTimestamp = 0;
                Attackers = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;
                UpdateWaitTimer = 5000;
                SignaledRoundTwo = false;
                SignaledRoundTwoHalfMin = false;
                InitSecondRound = true;
                ToggleTimer();
                ResetObjs();
                return;
            }
        }
        else if (Status == BG_SA_ROUND_TWO)
        {
            if (TotalTime >= EndRoundTimer)
            {
                // Defense of the Ancients (1757 / 2200)
                if (!gateDestroyed)
                    GetBgMap()->SetWorldState(GetDefenerTeam() == TEAM_ALLIANCE ? WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_A : WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_H, 1);

                CastSpellOnTeam(SPELL_END_OF_ROUND, ALLIANCE);
                CastSpellOnTeam(SPELL_END_OF_ROUND, HORDE);

                EndTimestamp = 0;
                UpdateWorldState(BG_SA_WorldStates::BG_SA_ENABLE_TIMER, 0);
                UpdateWorldState(BG_SA_WorldStates::BG_SA_TIMER, EndTimestamp);

                RoundScores[1].time = BG_SA_ROUNDLENGTH;
                RoundScores[1].winner = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;

                if (RoundScores[0].time == RoundScores[1].time)
                    EndBattleground(0);
                else if (RoundScores[0].time < RoundScores[1].time)
                    EndBattleground(RoundScores[0].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                else
                    EndBattleground(RoundScores[1].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                return;
            }
        }
        if (Status == BG_SA_ROUND_ONE || Status == BG_SA_ROUND_TWO)
        {
            timeUpdateTimer.Update(diff);
            if (timeUpdateTimer.Passed())
            {
                timeUpdateTimer.Reset(IN_MILLISECONDS);
                SendTime();
            }
            UpdateDemolisherSpawns();
        }
    }
}

void BattlegroundSA::StartingEventCloseDoors() { }

void BattlegroundSA::StartingEventOpenDoors() { }

void BattlegroundSA::FillInitialWorldStates(WorldStateBuilder& builder)
{
    uint32 ally_attacks = uint32(Attackers == TEAM_ALLIANCE ? 1 : 0);
    uint32 horde_attacks = uint32(Attackers == TEAM_HORDE ? 1 : 0);

    builder.AppendState(BG_SA_ANCIENT_GATEWS, GateStatus[BG_SA_ANCIENT_GATE]);
    builder.AppendState(BG_SA_YELLOW_GATEWS, GateStatus[BG_SA_YELLOW_GATE]);
    builder.AppendState(BG_SA_GREEN_GATEWS, GateStatus[BG_SA_GREEN_GATE]);
    builder.AppendState(BG_SA_BLUE_GATEWS, GateStatus[BG_SA_BLUE_GATE]);
    builder.AppendState(BG_SA_RED_GATEWS, GateStatus[BG_SA_RED_GATE]);
    builder.AppendState(BG_SA_PURPLE_GATEWS, GateStatus[BG_SA_PURPLE_GATE]);

    builder.AppendState(BG_SA_BONUS_TIMER, 0);

    builder.AppendState(BG_SA_HORDE_ATTACKS, horde_attacks);
    builder.AppendState(BG_SA_ALLY_ATTACKS, ally_attacks);

    //Time will be sent on first update...
    /*
    builder.AppendState(BG_SA_ENABLE_TIMER, TimerEnabled ? 1 : 0);
    builder.AppendState(BG_SA_TIMER_MINS, 0);
    builder.AppendState(BG_SA_TIMER_SEC_TENS, 0);
    builder.AppendState(BG_SA_TIMER_SEC_DECS, 0);
    */

    builder.AppendState(BG_SA_RIGHT_GY_HORDE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    builder.AppendState(BG_SA_LEFT_GY_HORDE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    builder.AppendState(BG_SA_CENTER_GY_HORDE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);

    builder.AppendState(BG_SA_RIGHT_GY_ALLIANCE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    builder.AppendState(BG_SA_LEFT_GY_ALLIANCE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    builder.AppendState(BG_SA_CENTER_GY_ALLIANCE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);

    builder.AppendState(BG_SA_HORDE_DEFENCE_TOKEN, ally_attacks);
    builder.AppendState(BG_SA_ALLIANCE_DEFENCE_TOKEN, horde_attacks);

    builder.AppendState(BG_SA_LEFT_ATT_TOKEN_HRD, horde_attacks);
    builder.AppendState(BG_SA_RIGHT_ATT_TOKEN_HRD, horde_attacks);
    builder.AppendState(BG_SA_RIGHT_ATT_TOKEN_ALL, ally_attacks);
    builder.AppendState(BG_SA_LEFT_ATT_TOKEN_ALL, ally_attacks);

    if (EndTimestamp)
    {
        builder.AppendState(BG_SA_ENABLE_TIMER, 1);
        builder.AppendState(BG_SA_TIMER, EndTimestamp);
    }
}

void BattlegroundSA::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map, default values are set in constructor
    BattlegroundSAScore* sc = new BattlegroundSAScore{};
    sc->Team = player->GetBGTeam();

    if (player->GetBGTeamId() == Attackers)
    {
        if (TotalTime <= 5000 || GetPlayers().size() <= 2)
        {
            player->CastSpell(player, 12438, true);//Without this player falls before boat loads...

            if (urand(0, 1)) // left ship
                player->TeleportTo(607, 2682.936f, -830.368f, 15.0f, 2.895f, 0);
            else  // right ship
                player->TeleportTo(607, 2577.003f, 980.261f, 15.0f, 0.807f, 0);
        }
        else if (!ShipsStarted) // ship not arrived yet, but already deported
        {
            player->CastSpell(player, 12438, true);//Without this player falls before boat loads...

            // teleport to nearest mate
            for (auto&& itr : GetPlayers())
            {
                if (Player* mate = ObjectAccessor::FindPlayer(itr.first))
                    if (mate->GetBGTeamId() == Attackers)
                        if (player != mate)
                        {
                            player->TeleportTo(607, mate->GetPositionX(), mate->GetPositionY(), mate->GetPositionZ() + 5.0f, 0.0f, 0);
                            continue;
                        }
            }
        }
        else
        {
            if (urand(0, 1))
                player->TeleportTo(607, 1598.571f, -106.671f, 8.873f, 4.1359f, 0);
            else
                player->TeleportTo(607, 1607.596f, 49.766f, 7.5795f, 2.31865f, 0);
        }
    }
    else
        player->TeleportTo(607, 1209.7f, -65.16f, 70.1f, 0.0f, 0);

    SendTransportInit(player);
    PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundSA::RemovePlayer(Player* player, ObjectGuid guid, uint32 team)
{
    if (player)
    {
        player->RemoveAura(52418);
    }
    Battleground::RemovePlayer(player, guid, team);
}

void BattlegroundSA::HandleAreaTrigger(Player* /*Source*/, uint32 /*Trigger*/, bool /*Entered*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void BattlegroundSA::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found...
        return;

    if (type == SCORE_DESTROYED_DEMOLISHER)
        ((BattlegroundSAScore*)itr->second)->demolishers_destroyed += value;
    else if (type == SCORE_DESTROYED_WALL)
        ((BattlegroundSAScore*)itr->second)->gates_destroyed += value;
    else
        Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
}

void BattlegroundSA::TeleportPlayers()
{
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
        {
            player->StopMoving();
            player->GetMotionMaster()->Clear();
            player->GetMotionMaster()->MoveIdle();

            // should remove spirit of redemption
            if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
                player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

            // remove all negative auras before start new round
            for (auto itr = player->GetAppliedAuras().begin(); itr != player->GetAppliedAuras().end();)
            {
                AuraApplication const* aurApp = itr->second;
                ++itr;
                Aura* aura = aurApp->GetBase();
                if (!aurApp->IsPositive() && aura->GetDuration() <= 30 * IN_MILLISECONDS && !aura->IsPermanent())
                {
                    player->RemoveAura(aura);
                    itr = player->GetAppliedAuras().begin();
                }
            }

            if (!player->IsAlive())
            {
                player->ResurrectPlayer(1.0f);
                player->SpawnCorpseBones();
            }

            player->ResetAllPowers();
            player->CombatStopWithPets(true);

            player->RemoveAllControlled();
            player->CastSpell(player, SPELL_PREPARATION, true);

            if (player->GetBGTeamId() == Attackers)
            {
                if (TotalTime <= 5000 || GetPlayers().size() <= 2)
                {
                    player->CastSpell(player, 12438, true);//Without this player falls before boat loads...

                    if (urand(0, 1)) // left ship
                        player->TeleportTo(607, 2682.936f, -830.368f, 15.0f, 2.895f, 0);
                    else  // right ship
                        player->TeleportTo(607, 2577.003f, 980.261f, 15.0f, 0.807f, 0);
                }
                else if (!ShipsStarted) // ship not arrived yet, but already deported
                {
                    player->CastSpell(player, 12438, true);//Without this player falls before boat loads...

                    // teleport to nearest mate
                    for (auto&& itr : GetPlayers())
                    {
                        if (Player* mate = ObjectAccessor::FindPlayer(itr.first))
                            if (mate->GetBGTeamId() == Attackers)
                                if (player != mate)
                                {
                                    player->TeleportTo(607, mate->GetPositionX(), mate->GetPositionY(), mate->GetPositionZ() + 5.0f, 0.0f, 0);
                                    continue;
                                }
                    }
                }
                else
                {
                    if (urand(0, 1))
                        player->TeleportTo(607, 1598.571f, -106.671f, 8.873f, 4.1359f, 0);
                    else
                        player->TeleportTo(607, 1607.596f, 49.766f, 7.5795f, 2.31865f, 0);
                }
            }
            else
                player->TeleportTo(607, 1209.7f, -65.16f, 70.1f, 0.0f, 0);
        }
    }
}

void BattlegroundSA::EventPlayerDamagedGO(Player* /*player*/, GameObject* go, uint32 eventType)
{
    if (!go || !go->GetGOInfo())
        return;

    if (eventType == go->GetGOInfo()->building.damagedEvent)
    {
        uint32 i = getGateIdFromDamagedOrDestroyEventId(eventType);
        GateStatus[i] = BG_SA_GATE_DAMAGED;
        uint32 uws = getWorldStateFromGateId(i);
        if (uws)
            UpdateWorldState(uws, GateStatus[i]);
    }

    if (eventType == go->GetGOInfo()->building.destroyedEvent)
    {
        if (go->GetGOInfo()->building.destroyedEvent == BG_SA_EVENT_ANCIENT_GATE_DESTROYED)
            SendWarningToAll(LANG_BG_SA_CHAMBER_BREACHED);
        else
            SendGateDestroyed(LANG_BG_SA_WAS_DESTROYED, go->GetGOInfo()->entry);
    }

    if (eventType == go->GetGOInfo()->building.damagedEvent)
        SendGateDestroyed(LANG_BG_SA_IS_UNDER_ATTACK, go->GetGOInfo()->entry);
}

void BattlegroundSA::HandleKillUnit(Creature* creature, Player* killer)
{
    if (creature->GetEntry() == NPC_DEMOLISHER_SA)
    {
        UpdatePlayerScore(killer, SCORE_DESTROYED_DEMOLISHER, 1);
        _allVehiclesAlive[Attackers] = false;
    }
}

/*
  You may ask what the fuck does it do?
  Prevents owner overwriting guns faction with own.
 */
void BattlegroundSA::OverrideGunFaction()
{
    if (!BgCreatures[0])
        return;

    for (uint8 i = BG_SA_GUN_1; i <= BG_SA_GUN_10; i++)
    {
        if (Creature* gun = GetBGCreature(i))
            gun->SetFaction(BG_SA_Factions[Attackers ? TEAM_ALLIANCE : TEAM_HORDE]);
    }

    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_4; i++)
    {
        if (Creature* dem = GetBGCreature(i))
            dem->SetFaction(BG_SA_Factions[Attackers]);
    }
}

void BattlegroundSA::DemolisherStartState(bool start)
{
    if (!BgCreatures[0])
        return;

    // set flags only for the demolishers on the beach, factory ones dont need it
    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_4; i++)
    {
        if (Creature* dem = GetBGCreature(i))
        {
            if (start)
                dem->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            else
                dem->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }
    }
}

void BattlegroundSA::DestroyGate(Player* player, GameObject* go)
{
    uint32 i = getGateIdFromDamagedOrDestroyEventId(go->GetGOInfo()->building.destroyedEvent);
    if (!GateStatus[i])
        return;

    if (GameObject* g = GetBGObject(i))
    {
        if (g->GetGOValue()->Building.Health == 0)
        {
            GateStatus[i] = BG_SA_GATE_DESTROYED;
            uint32 uws = getWorldStateFromGateId(i);
            if (uws)
                UpdateWorldState(uws, GateStatus[i]);
            bool rewardHonor = true;
            gateDestroyed = true;
            switch (i)
            {
                case BG_SA_GREEN_GATE:
                    if (GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED)
                        rewardHonor = false;
                    break;
                case BG_SA_BLUE_GATE:
                    if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED)
                        rewardHonor = false;
                    break;
                case BG_SA_RED_GATE:
                    if (GateStatus[BG_SA_PURPLE_GATE] == BG_SA_GATE_DESTROYED)
                        rewardHonor = false;
                    break;
                case BG_SA_PURPLE_GATE:
                    if (GateStatus[BG_SA_RED_GATE] == BG_SA_GATE_DESTROYED)
                        rewardHonor = false;
                    break;
                default:
                    break;
            }
            // Delete teleport triggers
            switch (i)
            {
                case BG_SA_GREEN_GATE:
                case BG_SA_BLUE_GATE:
                    for (uint32 i = BG_SA_NPC_TRIGGER_1; i <= BG_SA_NPC_TRIGGER_2; ++i)
                        DelCreature(i);
                    break;
                case BG_SA_RED_GATE:
                case BG_SA_PURPLE_GATE:
                    for (uint32 i = BG_SA_NPC_TRIGGER_3; i <= BG_SA_NPC_TRIGGER_4; ++i)
                        DelCreature(i);
                    break;
                default:
                    break;
            }

            if (i < 5)
                DelObject(i + 14);

            if (player)
            {
                UpdatePlayerScore(player, SCORE_DESTROYED_WALL, 1);
                if (rewardHonor)
                    UpdatePlayerScore(player, SCORE_BONUS_HONOR, GetBonusHonorFromKill(1));
            }
        }
    }
}

WorldSafeLocsEntry const* BattlegroundSA::GetClosestGraveYard(Player* player)
{
    uint32 safeloc = 0;
    WorldSafeLocsEntry const* ret;
    WorldSafeLocsEntry const* closest;
    float dist, nearest;
    float x, y, z;

    player->GetPosition(x, y, z);

    if (player->GetBGTeamId() == Attackers)
        safeloc = BG_SA_GYEntries[BG_SA_BEACH_GY];
    else
        safeloc = BG_SA_GYEntries[BG_SA_DEFENDER_LAST_GY];

    closest = sWorldSafeLocsStore.LookupEntry(safeloc);
    nearest = sqrt((closest->x - x)*(closest->x - x) + (closest->y - y)*(closest->y - y) + (closest->z - z)*(closest->z - z));

    for (uint8 i = BG_SA_RIGHT_CAPTURABLE_GY; i < BG_SA_MAX_GY; i++)
    {
        if (GraveyardStatus[i] != player->GetBGTeamId())
            continue;

        ret = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
        dist = sqrt((ret->x - x)*(ret->x - x) + (ret->y - y)*(ret->y - y) + (ret->z - z)*(ret->z - z));
        if (dist < nearest)
        {
            closest = ret;
            nearest = dist;
        }
    }

    return closest;
}

void BattlegroundSA::SendTime()
{
    // moved to BG_SA_TIMER in MoP
    /*
    uint32 end_of_round = (EndRoundTimer - TotalTime);
    UpdateWorldState(BG_SA_TIMER_MINS, end_of_round/60000);
    UpdateWorldState(BG_SA_TIMER_SEC_TENS, (end_of_round%60000)/10000);
    UpdateWorldState(BG_SA_TIMER_SEC_DECS, ((end_of_round%60000)%10000)/1000);
    */
}

void BattlegroundSA::EventPlayerClickedOnFlag(Player* Source, GameObject* target_obj)
{
    switch (target_obj->GetEntry())
    {
        case 191307:
        case 191308:
            if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED)
                CaptureGraveyard(BG_SA_LEFT_CAPTURABLE_GY, Source);
            else
                target_obj->SendUpdateToPlayer(Source);
            break;
        case 191305:
        case 191306:
            if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED)
                CaptureGraveyard(BG_SA_RIGHT_CAPTURABLE_GY, Source);
            else
                target_obj->SendUpdateToPlayer(Source);
            break;
        case 191310:
        case 191309:
            if ((GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED) && (GateStatus[BG_SA_RED_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_PURPLE_GATE] == BG_SA_GATE_DESTROYED))
            {
                CaptureGraveyard(BG_SA_CENTRAL_CAPTURABLE_GY, Source);
                CaptureGraveyard(BG_SA_LEFT_CAPTURABLE_GY, Source);
                CaptureGraveyard(BG_SA_RIGHT_CAPTURABLE_GY, Source);
            }
            else
                target_obj->SendUpdateToPlayer(Source);
            break;
        default:
            return;
    };
}

void BattlegroundSA::CaptureGraveyard(BG_SA_Graveyards i, Player* Source)
{
    if (GraveyardStatus[i] == Attackers)
        return;

    ObjectGuid spiritGuideGUID = BgCreatures[BG_SA_MAXNPC + i];

    DelCreature(BG_SA_MAXNPC + i);
    GraveyardStatus[i] = Source->GetBGTeamId();
    WorldSafeLocsEntry const* sg = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
    if (!sg)
    {
        TC_LOG_ERROR("bg.battleground", "BattlegroundSA::CaptureGraveyard: non-existant GY entry: %u", BG_SA_GYEntries[i]);
        return;
    }

    AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], (GraveyardStatus[i] == TEAM_ALLIANCE?  ALLIANCE : HORDE));
    uint32 npc = 0;
    uint32 flag = 0;

    switch (i)
    {
        case BG_SA_LEFT_CAPTURABLE_GY:
            flag = BG_SA_LEFT_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetBGTeamId() == TEAM_ALLIANCE ? 0 : 1)),
            BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
            BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);

            npc = BG_SA_NPC_RIGSPARK;
            AddCreature(BG_SA_NpcEntries[npc], npc, Attackers,
              BG_SA_NpcSpawnlocs[npc][0], BG_SA_NpcSpawnlocs[npc][1],
              BG_SA_NpcSpawnlocs[npc][2], BG_SA_NpcSpawnlocs[npc][3]);

            for (uint8 j = BG_SA_DEMOLISHER_7; j <= BG_SA_DEMOLISHER_8; j++)
            {
                AddCreature(BG_SA_NpcEntries[j], j, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
                    BG_SA_NpcSpawnlocs[j][0], BG_SA_NpcSpawnlocs[j][1],
                    BG_SA_NpcSpawnlocs[j][2], BG_SA_NpcSpawnlocs[j][3], 600);

                if (Creature* dem = GetBGCreature(j))
                    dem->SetFaction(BG_SA_Factions[Attackers]);
            }

            UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_LEFT_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetBGTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_WEST);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_WEST);
            break;
        case BG_SA_RIGHT_CAPTURABLE_GY:
            flag = BG_SA_RIGHT_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetBGTeamId() == TEAM_ALLIANCE ? 0 : 1)),
              BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
              BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);

            npc = BG_SA_NPC_SPARKLIGHT;
            AddCreature(BG_SA_NpcEntries[npc], npc, Attackers,
              BG_SA_NpcSpawnlocs[npc][0], BG_SA_NpcSpawnlocs[npc][1],
              BG_SA_NpcSpawnlocs[npc][2], BG_SA_NpcSpawnlocs[npc][3]);

            for (uint8 j = BG_SA_DEMOLISHER_5; j <= BG_SA_DEMOLISHER_6; j++)
            {
                AddCreature(BG_SA_NpcEntries[j], j, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
                    BG_SA_NpcSpawnlocs[j][0], BG_SA_NpcSpawnlocs[j][1],
                    BG_SA_NpcSpawnlocs[j][2], BG_SA_NpcSpawnlocs[j][3], 600);

                if (Creature* dem = GetBGCreature(j))
                    dem->SetFaction(BG_SA_Factions[Attackers]);
            }

            UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_RIGHT_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetBGTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_EAST);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_EAST);
            break;
        case BG_SA_CENTRAL_CAPTURABLE_GY:
            flag = BG_SA_CENTRAL_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetBGTeamId() == TEAM_ALLIANCE ? 0 : 1)),
              BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
              BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);

            UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_CENTER_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetBGTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_SOUTH);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_SOUTH);
            break;
        default:
            ASSERT(false);
            break;
    };

    UpdateBombSpawns();

    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    WorldSafeLocsEntry const* grave = nullptr;
    std::vector<ObjectGuid> guids = m_ReviveQueue[spiritGuideGUID];
    for (auto&& guid : guids)
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
            continue;

        if (!grave)
            grave = GetClosestGraveYard(player);

        if (grave)
            player->TeleportTo(GetMapId(), grave->x, grave->y, grave->z, player->GetOrientation());
    }
}

void BattlegroundSA::EventPlayerUsedGO(Player* Source, GameObject* object)
{
    if (object->GetEntry() == BG_SA_ObjEntries[BG_SA_TITAN_RELIC] && GateStatus[BG_SA_ANCIENT_GATE] == BG_SA_GATE_DESTROYED)
    {
        if (Source->GetBGTeamId() == Attackers)
        {
            if (Source->GetBGTeamId() == TEAM_ALLIANCE)
                SendMessageToAll(LANG_BG_SA_ALLIANCE_CAPTURED_RELIC, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            else SendMessageToAll(LANG_BG_SA_HORDE_CAPTURED_RELIC, CHAT_MSG_BG_SYSTEM_NEUTRAL);

            if (Status == BG_SA_ROUND_ONE)
            {
                // Defense of the Ancients (1757 / 2200)
                if (!gateDestroyed)
                    GetBgMap()->SetWorldState(GetDefenerTeam() == TEAM_ALLIANCE ? WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_A : WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_H, 1);

                CastSpellOnTeam(SPELL_END_OF_ROUND, ALLIANCE);
                CastSpellOnTeam(SPELL_END_OF_ROUND, HORDE);

                RoundScores[0].winner = Attackers;
                RoundScores[0].time = TotalTime;
                // Achievement Storm the Beach (1310)
                if (RoundScores[0].winner == Attackers && TotalTime <= 240000)
                {
                    GetBgMap()->SetWorldState(Attackers == TEAM_ALLIANCE ? WORLD_STATE_STORM_THE_BEACH_A : WORLD_STATE_STORM_THE_BEACH_H, 1);
                    for (auto&& itr : GetPlayers())
                        if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                            if (player->GetBGTeamId() == Attackers)
                                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 65246);
                }

                Attackers = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;
                Status = BG_SA_SECOND_WARMUP;
                EndTimestamp = 0;
                TotalTime = 0;
                ToggleTimer();
                SendWarningToAll(LANG_BG_SA_ROUND_ONE_END);
                UpdateWaitTimer = 5000;
                SignaledRoundTwo = false;
                SignaledRoundTwoHalfMin = false;
                InitSecondRound = true;
                ResetObjs();
            }
            else if (Status == BG_SA_ROUND_TWO)
            {
                // Defense of the Ancients (1757 / 2200)
                if (!gateDestroyed)
                    GetBgMap()->SetWorldState(GetDefenerTeam() == TEAM_ALLIANCE ? WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_A : WORLD_STATE_DEFENCE_OF_THE_ANCIENTS_H, 1);

                CastSpellOnTeam(SPELL_END_OF_ROUND, ALLIANCE);
                CastSpellOnTeam(SPELL_END_OF_ROUND, HORDE);

                RoundScores[1].winner = Attackers;
                RoundScores[1].time = TotalTime;
                ToggleTimer();
                // Achievement Storm the Beach (1310)
                if (RoundScores[1].winner == Attackers && TotalTime <= 240000)
                {
                    GetBgMap()->SetWorldState(Attackers == TEAM_ALLIANCE ? WORLD_STATE_STORM_THE_BEACH_A : WORLD_STATE_STORM_THE_BEACH_H, 1);
                    for (auto&& itr : GetPlayers())
                        if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                            if (player->GetBGTeamId() == Attackers)
                                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 65246);
                }

                EndTimestamp = 0;
                UpdateWorldState(BG_SA_ENABLE_TIMER, 0);
                UpdateWorldState(BG_SA_TIMER, 0);

                if (RoundScores[0].time == RoundScores[1].time)
                    EndBattleground(0);
                else if (RoundScores[0].time < RoundScores[1].time)
                    EndBattleground(RoundScores[0].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                else
                    EndBattleground(RoundScores[1].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
            }
        }
    }
}

void BattlegroundSA::ToggleTimer()
{
    TimerEnabled = !TimerEnabled;
    UpdateWorldState(BG_SA_ENABLE_TIMER, (TimerEnabled) ? 1 : 0);
}

void BattlegroundSA::EndBattleground(uint32 winner)
{
    // Not Even a Scratch (1762 / 2192)
    if (_allVehiclesAlive[winner == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE])
        GetBgMap()->SetWorldState(winner == ALLIANCE ? WORLD_STATE_NOT_EVEN_A_SCRATCH_A : WORLD_STATE_NOT_EVEN_A_SCRATCH_H, 1);

    // honor reward for winning
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    else if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    //complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(2), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(2), HORDE);

    Battleground::EndBattleground(winner);
}

void BattlegroundSA::UpdateDemolisherSpawns()
{
    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_8; i++)
    {
        if (i >= BG_SA_DEMOLISHER_1 && i <= BG_SA_DEMOLISHER_2 && GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == Attackers)
            continue;
        if (i >= BG_SA_DEMOLISHER_3 && i <= BG_SA_DEMOLISHER_4 && GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == Attackers)
            continue;

        if (BgCreatures[i])
        {
            if (Creature* Demolisher = GetBGCreature(i))
            {
                if (Demolisher->isDead())
                {
                    // Demolisher is not in list
                    if (DemoliserRespawnList.find(i) == DemoliserRespawnList.end())
                    {
                        DemoliserRespawnList[i] = getMSTime()+30000;
                    }
                    else
                    {
                        if (DemoliserRespawnList[i] < getMSTime())
                        {
                            Demolisher->Relocate(BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1],
                                BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3]);

                            Demolisher->Respawn();
                            DemoliserRespawnList.erase(i);
                        }
                    }
                }
            }
        }
    }
}

void BattlegroundSA::UpdateBombSpawns()
{
    for (uint8 i = BG_SA_BOMB; i < BG_SA_MAXOBJ; i++)
    {
        if (GetBGObject(i))
            continue;

        if (bombsLocationWest.GetExactDist2d(BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1]) < 50.0f && GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] != Attackers)
            continue;
        if (bombsLocationEast.GetExactDist2d(BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1]) < 50.0f && GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] != Attackers)
            continue;
        if (bombsLocationSouth.GetExactDist2d(BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1]) < 50.0f && GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] != Attackers)
            continue;
        if (bombsLocationRelic.GetExactDist2d(BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1]) < 50.0f && GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] != Attackers) // Which graveyard?
            continue;

        AddObject(i, BG_SA_ObjEntries[BG_SA_BOMB],
            BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
            BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
            0,0,0,0,RESPAWN_ONE_DAY);
        GetBGObject(i)->SetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE, BG_SA_Factions[Attackers]);
    }
}

void BattlegroundSA::SendTransportInit(Player* player)
{
    if (BgObjects[BG_SA_BOAT_ONE] ||  BgObjects[BG_SA_BOAT_TWO])
    {
        UpdateData transData(player->GetMapId());
        if (BgObjects[BG_SA_BOAT_ONE])

            GetBGObject(BG_SA_BOAT_ONE)->BuildCreateUpdateBlockForPlayer(&transData, player);
        if (BgObjects[BG_SA_BOAT_TWO])
            GetBGObject(BG_SA_BOAT_TWO)->BuildCreateUpdateBlockForPlayer(&transData, player);
        WorldPacket packet;
        transData.BuildPacket(&packet);
        player->SendDirectMessage(&packet);
    }
}

void BattlegroundSA::SendTransportsRemove(Player* player)
{
    if (BgObjects[BG_SA_BOAT_ONE] ||  BgObjects[BG_SA_BOAT_TWO])
    {
        UpdateData transData(player->GetMapId());
        if (BgObjects[BG_SA_BOAT_ONE])
            GetBGObject(BG_SA_BOAT_ONE)->BuildOutOfRangeUpdateBlock(&transData);
        if (BgObjects[BG_SA_BOAT_TWO])
            GetBGObject(BG_SA_BOAT_TWO)->BuildOutOfRangeUpdateBlock(&transData);
        WorldPacket packet;
        transData.BuildPacket(&packet);
        player->SendDirectMessage(&packet);
    }
}

void BattlegroundSA::SendGateDestroyed(uint32 text, uint32 entry)
{
    auto go = sObjectMgr->GetGameObjectTemplate(entry);
    if (!go)
        return;

    std::string name = go->name;

    std::map<uint32, WorldPacket> localizedPackets;

    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
        {
            if (localizedPackets.find(player->GetSession()->GetSessionDbLocaleIndex()) == localizedPackets.end())
            {
                char const* format = sObjectMgr->GetTrinityString(text, player->GetSession()->GetSessionDbLocaleIndex());

                if (auto locale = sObjectMgr->GetGameObjectLocale(entry))
                    sObjectMgr->GetLocaleString(locale->Name, player->GetSession()->GetSessionDbLocaleIndex(), name);

                char buf[256];
                sprintf(buf, format, name.c_str());

                ChatHandler::BuildChatPacket(localizedPackets[player->GetSession()->GetSessionDbLocaleIndex()], CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, NULL, NULL, buf);
            }

            player->SendDirectMessage(&localizedPackets[player->GetSession()->GetSessionDbLocaleIndex()]);
        }
}
