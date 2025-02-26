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

#include "BattlegroundSM.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "ScriptedCreature.h"
#include "BattlegroundMgr.h"
#include "GossipDef.h"

enum
{
    OBJECTIVE_CAPTURE_MINE_CART           = 428,   // 7102 - Escort Service

    SPELL_FIVE_FOR_FIVE_CREDIT            = 128650,// 7099 - Five for Five. Spell exists and called 'Three for Three Complete', lol
    SPELL_THE_LONG_RIDE_CREDIT            = 128672,// 7039 - The Long Riders. Where are 6 of them but our implementation is a hack
    SPELL_END_OF_THE_LINE_CREDIT          = 128720,// 7057 - End of the Line

    WORLD_STATE_MY_DIAMONDS_AND_YOUR_RUST = 12385, // 7100 - My Diamonds and Your Rust
};

BattlegroundSM::BattlegroundSM()
{
    m_BuffChange = true;
    BgObjects.resize(BG_SM_OBJECT_MAX);
    BgCreatures.resize(BG_SM_CREATURES_MAX);

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SM_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SM_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SM_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SM_HAS_BEGUN;
}

BattlegroundSM::~BattlegroundSM() { }

void BattlegroundSM::Reset()
{
    // Call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE] = 0;
    m_TeamScores[TEAM_HORDE] = 0;
    m_HonorScoreTics[TEAM_ALLIANCE] = 0;
    m_HonorScoreTics[TEAM_HORDE] = 0;
    m_mineCartCheckTimer = 1000;
    m_firstMineCartSummonTimer = 110 * IN_MILLISECONDS; // 10 sec before opening doors
    bool isBGWeekEnd = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_honorTicks = (isBGWeekEnd) ? BG_SM_SMWEEKEND_HONOR_TICKS : BG_SM_NOT_SMWEEKEND_HONOR_TICKS;
    m_isInformedNearVictory = false;

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

    m_mineCartAddPointsTimer = 2000;
    m_firstMineCartSpawned = false;
    m_waterfallPathDone = false;
    m_trackSwitch[SM_EAST_TRACK_SWITCH] = true;
    m_trackSwitch[SM_NORTH_TRACK_SWITCH] = false;
    m_trackSwitchClickTimer[SM_EAST_TRACK_SWITCH] = 3000;
    m_trackSwitchClickTimer[SM_NORTH_TRACK_SWITCH] = 3000;
    m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH] = true;
    m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH] = true;
    capturesCartByTeam[TEAM_ALLIANCE] = false;
    capturesCartByTeam[TEAM_HORDE] = false;

    for (uint8 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        m_mineCartsProgressBar[i] = BG_SM_PROGRESS_BAR_NEUTRAL;
        m_mineCartReachedDepot[i] = false;
        m_mineCartNearDepot[i] = false;
        m_mineCartSpawned[i] = false;
        m_endOfLineCriteria[i] = false;
    }

    for (uint8 i = 0; i < SM_MAX_PATHS; ++i)
        for (uint8 j = 0; j < SM_MAX_PATHS; ++j)
            m_pathDone[i][j] = false;

    for (uint8 i = 0; i < 4; ++i)
    {
        m_depot[i] = false;
        m_depotCloseTimer[i] = 3000;
    }
}

void BattlegroundSM::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (m_firstMineCartSpawned)
            SummonMineCart(diff);

        ProcessCartTicks();
        CheckPlayerNearMineCart(diff);
        CheckMineCartNearDepot();
        EventReopenDepot(diff);
        MineCartAddPoints(diff);
        CheckTrackSwitch();
        MineCartsMoves();
    }

    if (!m_firstMineCartSpawned)
        FirstMineCartSummon(diff);

    if (GetBgMap()->GetCreature(BgCreatures[NPC_SM_MINE_CART_1]))
        m_mineCartSpawned[BG_SM_MINE_CART_1] = true;
    else
    {
        m_mineCartSpawned[BG_SM_MINE_CART_1] = false;
        m_pathDone[SM_EAST_PATH][0] = false;
        m_pathDone[SM_EAST_PATH][1] = false;
    }

    if (GetBgMap()->GetCreature(BgCreatures[NPC_SM_MINE_CART_2]))
        m_mineCartSpawned[BG_SM_MINE_CART_2] = true;
    else
    {
        m_mineCartSpawned[BG_SM_MINE_CART_2] = false;
        m_waterfallPathDone = false;
    }

    if (GetBgMap()->GetCreature(BgCreatures[NPC_SM_MINE_CART_3]))
        m_mineCartSpawned[BG_SM_MINE_CART_3] = true;
    else
    {
        m_mineCartSpawned[BG_SM_MINE_CART_3] = false;
        m_pathDone[SM_NORTH_PATH][0] = false;
        m_pathDone[SM_NORTH_PATH][1] = false;
    }

    if (!m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH])
    {
        if (m_trackSwitchClickTimer[SM_EAST_TRACK_SWITCH] <= 0)
        {
            if (Creature* trackSwitch = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_EAST]))
            {
                for (auto&& itr : GetPlayers())
                {
                    if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                    {
                        if (player->GetExactDist2d(trackSwitch->GetPositionX(), trackSwitch->GetPositionY()) <= 10.0f)
                            player->PlayerTalkClass->SendCloseGossip(); // Prevent from using multiple times track switches
                    }
                }

                trackSwitch->RemoveAurasDueToSpell(BG_SM_PREVENTION_AURA);
                m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH] = true;
            }
        }
        else
            m_trackSwitchClickTimer[SM_EAST_TRACK_SWITCH] -= diff;
    }

    if (!m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH])
    {
        if (m_trackSwitchClickTimer[SM_NORTH_TRACK_SWITCH] <= 0)
        {
            if (Creature* trackSwitch = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_NORTH]))
            {
                for (auto&& itr : GetPlayers())
                {
                    if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                    {
                        if (player->GetExactDist2d(trackSwitch->GetPositionX(), trackSwitch->GetPositionY()) <= 10.0f)
                            player->PlayerTalkClass->SendCloseGossip(); // Prevent from using multiple times track switches
                    }
                }

                trackSwitch->RemoveAurasDueToSpell(BG_SM_PREVENTION_AURA);
                m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH] = true;
            }
        }
        else
            m_trackSwitchClickTimer[SM_NORTH_TRACK_SWITCH] -= diff;
    }
}

void BattlegroundSM::StartingEventCloseDoors()
{
    // Starting doors
    for (int i = BG_SM_OBJECT_DOOR_A_1; i <= BG_SM_OBJECT_DOOR_H_2; ++i)
    {
        DoorClose(i);
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
    }

    for (uint8 i = BG_SM_OBJECT_WATERFALL_DEPOT; i < BG_SM_OBJECT_MAX; ++i)
        SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattlegroundSM::CheckTrackSwitch()
{
    if (m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH])
    {
        if (Creature* trackSwitch = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_EAST]))
        {
            if (trackSwitch->HasAura(BG_SM_TRACK_SWITCH_OPENED) && !m_trackSwitch[SM_EAST_TRACK_SWITCH])
            {
                SendMessageToAll(LANG_BG_SM_EAST_DIRECTION_CHANGED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                m_trackSwitchClickTimer[SM_EAST_TRACK_SWITCH] = 3000;
                m_trackSwitch[SM_EAST_TRACK_SWITCH] = true;
                m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH] = false;
            }

            if (trackSwitch->HasAura(BG_SM_TRACK_SWITCH_CLOSED) && m_trackSwitch[SM_EAST_TRACK_SWITCH])
            {
                SendMessageToAll(LANG_BG_SM_EAST_DIRECTION_CHANGED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                m_trackSwitchClickTimer[SM_EAST_TRACK_SWITCH] = 3000;
                m_trackSwitch[SM_EAST_TRACK_SWITCH] = false;
                m_trackSwitchCanInterract[SM_EAST_TRACK_SWITCH] = false;
            }
        }
    }

    if (m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH])
    {
        if (Creature* trackSwitch = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_NORTH]))
        {
            if (trackSwitch->HasAura(BG_SM_TRACK_SWITCH_CLOSED) && m_trackSwitch[SM_NORTH_TRACK_SWITCH])
            {
                SendMessageToAll(LANG_BG_SM_NORTH_DIRECTION_CHANGED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                m_trackSwitchClickTimer[SM_NORTH_TRACK_SWITCH] = 3000;
                m_trackSwitch[SM_NORTH_TRACK_SWITCH] = false;
                m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH] = false;
            }

            if (trackSwitch->HasAura(BG_SM_TRACK_SWITCH_OPENED) && !m_trackSwitch[SM_NORTH_TRACK_SWITCH])
            {
                SendMessageToAll(LANG_BG_SM_NORTH_DIRECTION_CHANGED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                m_trackSwitchClickTimer[SM_NORTH_TRACK_SWITCH] = 3000;
                m_trackSwitch[SM_NORTH_TRACK_SWITCH] = true;
                m_trackSwitchCanInterract[SM_NORTH_TRACK_SWITCH] = false;
            }
        }
    }
}

void BattlegroundSM::ProcessCartTicks()
{
    for (uint32 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        if (Creature* cart = GetBGCreature(BG_SM_CartTypes[i]))
        {
            ++m_cartTicks[i];
            for (auto&& itr : GetPlayers())
            {
                if (Player* player = ObjectAccessor::GetPlayer(*cart, itr.first))
                {
                    if (player->GetBGTeam() == ALLIANCE && m_mineCartsProgressBar[i] < BG_SM_PROGRESS_BAR_NEUTRAL)
                        continue;
                    if (player->GetBGTeam() == HORDE && m_mineCartsProgressBar[i] > BG_SM_PROGRESS_BAR_NEUTRAL)
                        continue;

                    if (player->IsWithinDistInMap(cart, 22.0f) )
                    {
                        auto itr2 = m_playerTicks[i].find(itr.first.GetCounter());
                        if (itr2 == m_playerTicks[i].end())
                            itr2 = m_playerTicks[i].insert({ itr.first.GetCounter(), 0 }).first;

                        ++itr2->second;
                    }
                }
            }
        }
    }
}

void BattlegroundSM::CartSpawned(BG_SM_MineCarts cart)
{
    m_cartTicks[cart] = 0;
    m_playerTicks[cart].clear();
    m_endOfLineCriteria[cart] = false;
}

void BattlegroundSM::CartControllSeized(BG_SM_MineCarts cart, Team team)
{
    Creature* mineCart = GetBGCreature(BG_SM_CartTypes[cart]);
    for (auto&& itr : BG_SM_DepotPos)
        if (mineCart->IsInDist(itr[0], itr[1], itr[2], 29.0f)) // This is because depot has size, and caret stops even further than depot edge. And this is fucking rough
            m_endOfLineCriteria[cart] = true;

    uint32 spellId   = team == ALLIANCE ? BG_SM_CONTROL_VISUAL_ALLIANCE : BG_SM_CONTROL_VISUAL_HORDE;
    uint32 msg       = team == ALLIANCE ? LANG_BG_SM_ALLIANCE_CONTROLLED_MINE_CART : LANG_BG_SM_HORDE_CONTROLLED_MINE_CART;
    ChatMsg chatType = team == ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE;

    mineCart->CastSpell(mineCart, spellId, true);
    SendMessageToAll(msg, chatType);
}

void BattlegroundSM::FirstMineCartSummon(uint32 /*diff*/)
{
    for (uint32 cart = 0; cart < SM_MINE_CART_MAX; cart++)
    {
        if (Creature* mineCart = AddCreature(BG_SM_CartIDs[cart], BG_SM_CartTypes[cart], 0, BG_SM_StartPos[cart][0], BG_SM_StartPos[cart][1], BG_SM_StartPos[cart][2], BG_SM_StartPos[cart][3]))
        {
            mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
            mineCart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
            mineCart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            mineCart->SetSpeed(MOVE_WALK, 0.8f);
            m_mineCartsProgressBar[cart] = BG_SM_PROGRESS_BAR_NEUTRAL;
            CartSpawned(BG_SM_MineCarts(cart));
        }
    }

    m_firstMineCartSpawned = true;
}

void BattlegroundSM::StartingEventOpenDoors()
{
    // Open doors
    for (int doorType = BG_SM_OBJECT_DOOR_A_1; doorType <= BG_SM_OBJECT_DOOR_H_2; ++doorType)
        DoorOpen(doorType);
    
    for (uint8 i = BG_SM_OBJECT_WATERFALL_DEPOT; i < BG_SM_OBJECT_MAX; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattlegroundSM::SummonMineCart(uint32 /*diff*/)
{
    for (uint32 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        if (!m_mineCartSpawned[i])
        {
            if (Creature* mineCart = AddCreature(BG_SM_CartIDs[i], BG_SM_CartTypes[i], 0, BG_SM_StartPos[i][0], BG_SM_StartPos[i][1], BG_SM_StartPos[i][2], BG_SM_StartPos[i][3]))
            {
                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                mineCart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
                mineCart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                mineCart->SetSpeed(MOVE_WALK, 0.8f);
                m_mineCartsProgressBar[i] = BG_SM_PROGRESS_BAR_NEUTRAL;
                SendMessageToAll(LANG_BG_SM_MINE_CART_SPAWNED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                PlaySoundToAll(BG_SM_SOUND_MINE_CART_SPAWNED);
                CartSpawned(BG_SM_MineCarts(i));
            }
        }
    }
}

void BattlegroundSM::CheckPlayerNearMineCart(uint32 diff)
{
    if (m_mineCartCheckTimer <= 0)
    {
        for (auto&& itr : GetPlayers())
        {
            if (Player* player = ObjectAccessor::FindPlayer(itr.first))
            {
                if (player->HasAuraType(AuraType::SPELL_AURA_MOD_STEALTH))
                    continue;

                if (player->isDead())
                {
                    UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
                    continue;
                }

                if (player->GetBGTeam() == ALLIANCE)
                {
                    if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_1, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] >= 100)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_1] = 100;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_1], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_1]++;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_1], player);
                        }
                        
                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] > BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                CartControllSeized(BG_SM_MINE_CART_1, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }
                    else if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_2, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] >= 100)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_2] = 100;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_2], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_2]++;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_2], player);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] > BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                CartControllSeized(BG_SM_MINE_CART_2, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }
                    else if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_3, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] >= 100)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_3] = 100;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_3], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_3]++;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_3], player);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] > BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                CartControllSeized(BG_SM_MINE_CART_3, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }
                    else
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
                }
                else // for GetBGTeam() == HORDE
                {
                    if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_1, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] <= 0)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_1] = 0;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_1], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_1]--;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_1], player);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] < BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                CartControllSeized(BG_SM_MINE_CART_1, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }

                    else if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_2, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] <= 0)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_2] = 0;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_2], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_2]--;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_2], player);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] < BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                CartControllSeized(BG_SM_MINE_CART_2, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }
                    else if (Creature* mineCart = player->FindNearestCreature(NPC_MINE_CART_3, 22.0f, true))
                    {
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] <= 0)
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_3] = 0;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_3], player);
                        }
                        else
                        {
                            m_mineCartsProgressBar[BG_SM_MINE_CART_3]--;
                            UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_mineCartsProgressBar[BG_SM_MINE_CART_3], player);
                        }
                        
                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] < BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                CartControllSeized(BG_SM_MINE_CART_3, player->GetBGTeam());
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] == BG_SM_PROGRESS_BAR_NEUTRAL)
                        {
                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, mineCart->GetGUID());

                            if (mineCart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
                                mineCart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, mineCart->GetGUID());

                            if (!mineCart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
                                mineCart->CastSpell(mineCart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
                        }
                    }
                    else
                        UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
                }

                m_mineCartCheckTimer = 1000;
            }
        }
    }
    else
        m_mineCartCheckTimer -= diff;
}

void BattlegroundSM::CheckMineCartNearDepot()
{
    if (!m_mineCartNearDepot[BG_SM_MINE_CART_1])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
        {
            if (mineCart->GetExactDist2d(BG_SM_DepotPos[SM_LAVA_DEPOT][0], BG_SM_DepotPos[SM_LAVA_DEPOT][1]) <= 6.0f)
            {
                m_depot[SM_LAVA_DEPOT] = true;
                EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), BG_SM_MINE_CART_1);
                m_mineCartNearDepot[BG_SM_MINE_CART_1] = true;
            }

            if (mineCart->GetExactDist2d(BG_SM_DepotPos[SM_DIAMOND_DEPOT][0], BG_SM_DepotPos[SM_DIAMOND_DEPOT][1]) <= 6.0f)
            {
                m_depot[SM_DIAMOND_DEPOT] = true;
                EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), BG_SM_MINE_CART_1);
                m_mineCartNearDepot[BG_SM_MINE_CART_1] = true;
            }
        }
    }

    if (!m_mineCartNearDepot[BG_SM_MINE_CART_2])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_2))
        {
            if (mineCart->GetExactDist2d(BG_SM_DepotPos[SM_WATERFALL_DEPOT][0], BG_SM_DepotPos[SM_WATERFALL_DEPOT][1]) <= 6.0f)
            {
                m_depot[SM_WATERFALL_DEPOT] = true;
                EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_2), BG_SM_MINE_CART_2);
                m_mineCartNearDepot[BG_SM_MINE_CART_2] = true;
            }
        }
    }

    if (!m_mineCartNearDepot[BG_SM_MINE_CART_3])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
        {
            if (mineCart->GetExactDist2d(BG_SM_DepotPos[SM_DIAMOND_DEPOT][0], BG_SM_DepotPos[SM_DIAMOND_DEPOT][1]) <= 6.0f)
            {
                m_depot[SM_DIAMOND_DEPOT] = true;
                EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), BG_SM_MINE_CART_3);
                m_mineCartNearDepot[BG_SM_MINE_CART_3] = true;
            }

            if (mineCart->GetExactDist2d(BG_SM_DepotPos[SM_TROLL_DEPOT][0], BG_SM_DepotPos[SM_TROLL_DEPOT][1]) <= 6.0f)
            {
                m_depot[SM_TROLL_DEPOT] = true;
                EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), BG_SM_MINE_CART_3);
                m_mineCartNearDepot[BG_SM_MINE_CART_3] = true;
            }
        }
    }
}

void BattlegroundSM::EventTeamCapturedMineCart(uint32 team, uint8 mineCart)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    for (auto&& itr : GetPlayers())
    {
        if (Player* player = ObjectAccessor::FindPlayer(itr.first))
        {
            if (player->GetBGTeam() == team)
            {
                if (player->FindNearestCreature(BG_SM_CartIDs[mineCart], 22.0f))
                {
                    UpdatePlayerScore(player, SCORE_CART_CONTROLLED, 1);
                    player->RewardHonor(player, 1, irand(10, 12));
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, OBJECTIVE_CAPTURE_MINE_CART, 1);
                    auto itr = m_playerTicks[mineCart].find(player->GetGUID());
                    if (itr != m_playerTicks[mineCart].end() && m_cartTicks[mineCart] == itr->second)
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_THE_LONG_RIDE_CREDIT);
                    if (m_endOfLineCriteria[mineCart])
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_END_OF_THE_LINE_CREDIT);
                    if (++m_fiveForFivePorgress[player->GetGUID()] == 5)
                        player->CastSpell(player, SPELL_FIVE_FOR_FIVE_CREDIT, true);
                }
            }
        }
    }

    switch (mineCart)
    {
        case BG_SM_MINE_CART_1:
        {
            if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
            {
                if (m_depot[SM_LAVA_DEPOT])
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_LAVA_DEPOT]))
                    {
                        mineCart->StopMoving();
                        depot->UseDoorOrButton();
                        m_mineCartReachedDepot[BG_SM_MINE_CART_1] = true;
                    }
                }

                if (m_depot[SM_DIAMOND_DEPOT])
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
                    {
                        mineCart->StopMoving();
                        depot->UseDoorOrButton();
                        m_mineCartReachedDepot[BG_SM_MINE_CART_1] = true;
                    }
                }
            }
            break;
        }
        case BG_SM_MINE_CART_2:
        {
            if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_2))
            {
                if (m_depot[SM_WATERFALL_DEPOT])
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_WATERFALL_DEPOT]))
                    {
                        mineCart->StopMoving();
                        depot->UseDoorOrButton();
                        m_mineCartReachedDepot[BG_SM_MINE_CART_2] = true;
                    }
                }
            }
            break;
        }
        case BG_SM_MINE_CART_3:
        {
            if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
            {
                if (m_depot[SM_DIAMOND_DEPOT])
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
                    {
                        mineCart->StopMoving();
                        depot->UseDoorOrButton();
                        m_mineCartReachedDepot[BG_SM_MINE_CART_3] = true;
                    }
                }

                if (m_depot[SM_TROLL_DEPOT])
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_TROLL_DEPOT]))
                    {
                        mineCart->StopMoving();
                        depot->UseDoorOrButton();
                        m_mineCartReachedDepot[BG_SM_MINE_CART_3] = true;
                    }
                }
            }
            break;
        }
    }
}

void BattlegroundSM::EventReopenDepot(uint32 diff)
{
    if (m_mineCartReachedDepot[BG_SM_MINE_CART_1])
    {
        if (m_depot[SM_LAVA_DEPOT])
        {
            if (m_depotCloseTimer[SM_LAVA_DEPOT] <= 0)
            {
                if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_LAVA_DEPOT]))
                    {
                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE)
                        {
                            SendMessageToAll(LANG_BG_SM_ALLIANCE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE);
                        }

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE)
                        {
                            SendMessageToAll(LANG_BG_SM_HORDE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_HORDE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_HORDE);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] != BG_SM_PROGRESS_BAR_NEUTRAL)
                            AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), POINTS_PER_MINE_CART);

                        ResetDepotsAndMineCarts(SM_LAVA_DEPOT, BG_SM_MINE_CART_1);
                        depot->ResetDoorOrButton();
                        mineCart->DespawnOrUnsummon();
                        DelCreature(NPC_SM_MINE_CART_1);
                    }
                }
            }
            else
                m_depotCloseTimer[SM_LAVA_DEPOT] -= diff;
        }

        if (m_depot[SM_DIAMOND_DEPOT])
        {
            if (m_depotCloseTimer[SM_DIAMOND_DEPOT] <= 0)
            {
                if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
                    {
                        capturesCartByTeam[GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE];

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE)
                        {
                            SendMessageToAll(LANG_BG_SM_ALLIANCE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE);
                        }

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE)
                        {
                            SendMessageToAll(LANG_BG_SM_HORDE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_HORDE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_HORDE);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_1] != BG_SM_PROGRESS_BAR_NEUTRAL)
                            AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), POINTS_PER_MINE_CART);

                        ResetDepotsAndMineCarts(SM_DIAMOND_DEPOT, BG_SM_MINE_CART_1);
                        depot->ResetDoorOrButton();
                        mineCart->DespawnOrUnsummon();
                        DelCreature(NPC_SM_MINE_CART_1);
                    }
                }
            }
            else
                m_depotCloseTimer[SM_DIAMOND_DEPOT] -= diff;
        }
    }

    if (m_mineCartReachedDepot[BG_SM_MINE_CART_2])
    {
        if (m_depot[SM_WATERFALL_DEPOT])
        {
            if (m_depotCloseTimer[SM_WATERFALL_DEPOT] <= 0)
            {
                if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_2))
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_WATERFALL_DEPOT]))
                    {
                        capturesCartByTeam[GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE];

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE)
                        {
                            SendMessageToAll(LANG_BG_SM_ALLIANCE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE);
                        }

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == HORDE)
                        {
                            SendMessageToAll(LANG_BG_SM_HORDE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_HORDE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_HORDE);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_2] != BG_SM_PROGRESS_BAR_NEUTRAL)
                            AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_2), POINTS_PER_MINE_CART);

                        ResetDepotsAndMineCarts(SM_WATERFALL_DEPOT, BG_SM_MINE_CART_2);
                        depot->ResetDoorOrButton();
                        mineCart->DespawnOrUnsummon();
                        DelCreature(NPC_SM_MINE_CART_2);
                    }
                }
            }
            else
                m_depotCloseTimer[SM_WATERFALL_DEPOT] -= diff;
        }
    }

    if (m_mineCartReachedDepot[BG_SM_MINE_CART_3])
    {
        if (m_depot[SM_DIAMOND_DEPOT])
        {
            if (m_depotCloseTimer[SM_DIAMOND_DEPOT] <= 0)
            {
                if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
                    {
                        capturesCartByTeam[GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE];

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
                        {
                            SendMessageToAll(LANG_BG_SM_ALLIANCE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE);
                        }

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
                        {
                            SendMessageToAll(LANG_BG_SM_HORDE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_HORDE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_HORDE);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] != BG_SM_PROGRESS_BAR_NEUTRAL)
                            AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), POINTS_PER_MINE_CART);
                            
                        ResetDepotsAndMineCarts(SM_DIAMOND_DEPOT, BG_SM_MINE_CART_3);
                        depot->ResetDoorOrButton();
                        mineCart->DespawnOrUnsummon();
                        DelCreature(NPC_SM_MINE_CART_3);
                    }
                }
            }
            else
                m_depotCloseTimer[SM_DIAMOND_DEPOT] -= diff;
        }

        if (m_depot[SM_TROLL_DEPOT])
        {
            if (m_depotCloseTimer[SM_TROLL_DEPOT] <= 0)
            {
                if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
                {
                    if (GameObject* depot = GetBgMap()->GetGameObject(BgObjects[BG_SM_OBJECT_TROLL_DEPOT]))
                    {
                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
                        {
                            SendMessageToAll(LANG_BG_SM_ALLIANCE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE);
                        }

                        if (GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
                        {
                            SendMessageToAll(LANG_BG_SM_HORDE_CAPTURED_MINE_CART, CHAT_MSG_BG_SYSTEM_HORDE);
                            PlaySoundToAll(BG_SM_SOUND_MINE_CART_CAPTURED_HORDE);
                        }

                        if (m_mineCartsProgressBar[BG_SM_MINE_CART_3] != BG_SM_PROGRESS_BAR_NEUTRAL)
                            AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), POINTS_PER_MINE_CART);
                            
                        ResetDepotsAndMineCarts(SM_TROLL_DEPOT, BG_SM_MINE_CART_3);
                        depot->ResetDoorOrButton();
                        mineCart->DespawnOrUnsummon();
                        DelCreature(NPC_SM_MINE_CART_3);
                    }
                }
            }
            else
                m_depotCloseTimer[SM_TROLL_DEPOT] -= diff;
        }
    }
}

uint32 BattlegroundSM::GetMineCartTeamKeeper(uint8 mineCart)
{
    if (m_mineCartsProgressBar[mineCart] > BG_SM_PROGRESS_BAR_NEUTRAL)
        return ALLIANCE;

    if (m_mineCartsProgressBar[mineCart] < BG_SM_PROGRESS_BAR_NEUTRAL)
        return HORDE;

    return 0;
}

void BattlegroundSM::MineCartAddPoints(uint32 diff)
{
    if (m_mineCartAddPointsTimer <= diff)
    {
        uint8 allianceCount = 0;
        uint8 hordeCount = 0;

        for (uint32 i = 0; i < SM_MINE_CART_MAX; ++i)
        {
            if (GetMineCartTeamKeeper(i) == ALLIANCE)
                ++allianceCount;
            else if (GetMineCartTeamKeeper(i) == HORDE)
                ++hordeCount;
        }

        if (allianceCount)
            AddPoints(ALLIANCE, allianceCount);

        if (hordeCount)
            AddPoints(HORDE, hordeCount);

        m_mineCartAddPointsTimer = 1000;
    }
    else
        m_mineCartAddPointsTimer -= diff;
}

void BattlegroundSM::ResetDepotsAndMineCarts(uint8 depot, uint8 mineCart)
{
    m_depotCloseTimer[depot] = 3000;
    m_depot[depot] = false;
    m_mineCartsProgressBar[mineCart] = BG_SM_PROGRESS_BAR_NEUTRAL;
    m_mineCartNearDepot[mineCart] = false;
    m_mineCartReachedDepot[mineCart] = false;
    m_mineCartSpawned[mineCart] = false;
}

void BattlegroundSM::MineCartsMoves()
{
    if (!m_pathDone[SM_EAST_PATH][0])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
        {
            mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_1, false);
            m_pathDone[SM_EAST_PATH][0] = true;
        }
    }

    if (m_pathDone[SM_EAST_PATH][0] && !m_pathDone[SM_EAST_PATH][1])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
        {
            if (mineCart->GetExactDist2d(717.169312f, 114.258339f) < 0.5f) // East pos
            {
                if (Creature* track = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_EAST]))
                {
                    if (track->HasAura(BG_SM_TRACK_SWITCH_OPENED))
                    {
                        mineCart->GetMotionMaster()->Clear(true);
                        mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_1 * 10, false);
                        m_pathDone[SM_EAST_PATH][1] = true;
                    }
                    else if ((!track->HasAura(BG_SM_TRACK_SWITCH_OPENED) && !track->HasAura(BG_SM_TRACK_SWITCH_CLOSED)) ||
                        track->HasAura(BG_SM_TRACK_SWITCH_CLOSED))
                    {
                        mineCart->GetMotionMaster()->Clear(true);
                        mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_1 * 100, false);
                        m_pathDone[SM_EAST_PATH][1] = true;
                    }
                }
            }
        }
    }

    if (!m_waterfallPathDone)
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_2))
        {
            mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_2, false);
            m_waterfallPathDone = true;
        }
    }

    if (!m_pathDone[SM_NORTH_PATH][0])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
        {
            mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_3, false);
            m_pathDone[SM_NORTH_PATH][0] = true;
        }
    }

    if (m_pathDone[SM_NORTH_PATH][0] && !m_pathDone[SM_NORTH_PATH][1])
    {
        if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
        {
            if (mineCart->GetExactDist2d(834.727234f, 299.809753f) < 0.5f) // North pos
            {
                if (Creature* track = GetBgMap()->GetCreature(BgCreatures[SM_TRACK_SWITCH_NORTH]))
                {
                    if ((!track->HasAura(BG_SM_TRACK_SWITCH_OPENED) && !track->HasAura(BG_SM_TRACK_SWITCH_CLOSED)) ||
                        track->HasAura(BG_SM_TRACK_SWITCH_CLOSED))
                    {
                        mineCart->GetMotionMaster()->Clear(true);
                        mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_3 * 10, false);
                        m_pathDone[SM_NORTH_PATH][1] = true;
                    }
                    else
                    {
                        mineCart->GetMotionMaster()->Clear(true);
                        mineCart->GetMotionMaster()->MovePath(NPC_MINE_CART_3 * 100, false);
                        m_pathDone[SM_NORTH_PATH][1] = true;
                    }
                }
            }
        }
    }
}

void BattlegroundSM::AddPoints(uint32 team, uint32 points)
{
    TeamId teamIndex = GetTeamIndexByTeamId(team);
    m_TeamScores[teamIndex] += points;
    m_HonorScoreTics[teamIndex] += points;

    if (m_HonorScoreTics[teamIndex] >= m_honorTicks)
    {
        RewardHonorToTeam(GetBonusHonorFromKill(6), team);
        m_HonorScoreTics[teamIndex] -= m_honorTicks;
    }

    UpdateTeamScore(team);
}

void BattlegroundSM::UpdateTeamScore(uint32 team)
{
    uint32 score = GetTeamScore(team);
    if (!m_isInformedNearVictory && score >= BG_SM_WARNING_NEAR_VICTORY_SCORE)
    {
        if (team == ALLIANCE)
            SendMessageToAll(LANG_BG_SM_A_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        else
            SendMessageToAll(LANG_BG_SM_H_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll(BG_SM_SOUND_NEAR_VICTORY);
        m_isInformedNearVictory = true;
    }

    if (score >= BG_SM_MAX_TEAM_SCORE)
    {
        score = BG_SM_MAX_TEAM_SCORE;

        if (capturesCartByTeam[team == ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE] == 0) // check losing team
            GetBgMap()->SetWorldState(WORLD_STATE_MY_DIAMONDS_AND_YOUR_RUST, 1);

        EndBattleground(team);
    }

    if (team == ALLIANCE)
        UpdateWorldState(SM_ALLIANCE_RESOURCES, score);
    else
        UpdateWorldState(SM_HORDE_RESOURCES, score);
}

void BattlegroundSM::EndBattleground(uint32 winner)
{
    // Win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), HORDE);
    // Complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), HORDE);

    for (auto&& itr : GetPlayers())
    {
        if (itr.second.Team == winner)
        {
            Player* player = ObjectAccessor::FindPlayer(itr.first);
            if (player)
                player->KilledMonsterCredit(68932);
        }
    }

    if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_1))
        mineCart->DespawnOrUnsummon();

    if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_2))
        mineCart->DespawnOrUnsummon();

    if (Creature* mineCart = GetBGCreature(NPC_SM_MINE_CART_3))
        mineCart->DespawnOrUnsummon();

    Battleground::EndBattleground(winner);
}

void BattlegroundSM::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    // create score and add it to map
    BattlegroundSMScore* score = new BattlegroundSMScore();
    score->Team = player->GetBGTeam();
    PlayerScores[player->GetGUID()] = score;

    m_fiveForFivePorgress[player->GetGUID()] = 0;
}

bool BattlegroundSM::SetupBattleground()
{
    // doors
    if (!AddObject(BG_SM_OBJECT_WATERFALL_DEPOT, BG_SM_WATERFALL_DEPOT, BG_SM_DepotPos[SM_WATERFALL_DEPOT][0], BG_SM_DepotPos[SM_WATERFALL_DEPOT][1], BG_SM_DepotPos[SM_WATERFALL_DEPOT][2], BG_SM_DepotPos[SM_WATERFALL_DEPOT][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)    // Waterfall
        || !AddObject(BG_SM_OBJECT_LAVA_DEPOT, BG_SM_LAVA_DEPOT, BG_SM_DepotPos[SM_LAVA_DEPOT][0], BG_SM_DepotPos[SM_LAVA_DEPOT][1], BG_SM_DepotPos[SM_LAVA_DEPOT][2], BG_SM_DepotPos[SM_LAVA_DEPOT][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)                            // Lava
        || !AddObject(BG_SM_OBJECT_DIAMOND_DEPOT, BG_SM_DIAMOND_DEPOT, BG_SM_DepotPos[SM_DIAMOND_DEPOT][0], BG_SM_DepotPos[SM_DIAMOND_DEPOT][1], BG_SM_DepotPos[SM_DIAMOND_DEPOT][2], BG_SM_DepotPos[SM_DIAMOND_DEPOT][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)            // Diamond
        || !AddObject(BG_SM_OBJECT_TROLL_DEPOT, BG_SM_TROLL_DEPOT, BG_SM_DepotPos[SM_TROLL_DEPOT][0], BG_SM_DepotPos[SM_TROLL_DEPOT][1], BG_SM_DepotPos[SM_TROLL_DEPOT][2], BG_SM_DepotPos[SM_TROLL_DEPOT][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)                        // Troll
        || !AddObject(BG_SM_OBJECT_DOOR_A_1, BG_SM_DOOR, BG_SM_DoorPos[0][0], BG_SM_DoorPos[0][1], BG_SM_DoorPos[0][2], BG_SM_DoorPos[0][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_DOOR_A_2, BG_SM_DOOR, BG_SM_DoorPos[1][0], BG_SM_DoorPos[1][1], BG_SM_DoorPos[1][2], BG_SM_DoorPos[1][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_DOOR_H_1, BG_SM_DOOR, BG_SM_DoorPos[2][0], BG_SM_DoorPos[2][1], BG_SM_DoorPos[2][2], BG_SM_DoorPos[2][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_DOOR_H_2, BG_SM_DOOR, BG_SM_DoorPos[3][0], BG_SM_DoorPos[3][1], BG_SM_DoorPos[3][2], BG_SM_DoorPos[3][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_BERSERKING_BUFF_EAST, BG_OBJECTID_BERSERKERBUFF_ENTRY, BG_SM_BuffPos[0][0], BG_SM_BuffPos[0][1], BG_SM_BuffPos[0][2], BG_SM_BuffPos[0][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_BERSERKING_BUFF_WEST, BG_OBJECTID_BERSERKERBUFF_ENTRY, BG_SM_BuffPos[1][0], BG_SM_BuffPos[1][1], BG_SM_BuffPos[1][2], BG_SM_BuffPos[1][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_RESTORATION_BUFF_WATERFALL, BG_OBJECTID_REGENBUFF_ENTRY, BG_SM_BuffPos[2][0], BG_SM_BuffPos[2][1], BG_SM_BuffPos[2][2], BG_SM_BuffPos[2][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_RESTORATION_BUFF_LAVA, BG_OBJECTID_REGENBUFF_ENTRY, BG_SM_BuffPos[3][0], BG_SM_BuffPos[3][1], BG_SM_BuffPos[3][2], BG_SM_BuffPos[3][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundSM: Failed to spawn some object Battleground not created!");
        return false;
    }

    if (Creature* tracker = AddCreature(NPC_TRACK_SWITCH, SM_TRACK_SWITCH_EAST, 0, BG_SM_TrackPos[SM_EAST_PATH][0], BG_SM_TrackPos[SM_EAST_PATH][1], BG_SM_TrackPos[SM_EAST_PATH][2], BG_SM_TrackPos[SM_EAST_PATH][3]))
        tracker->CastSpell(tracker, BG_SM_FEIGN_DEATH_STUN, true);
    else
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundSM: Failed to spawn some creatures Battleground not created!");
        return false;
    }

    if (Creature* tracker = AddCreature(NPC_TRACK_SWITCH, SM_TRACK_SWITCH_NORTH, 0, BG_SM_TrackPos[SM_NORTH_PATH][0], BG_SM_TrackPos[SM_NORTH_PATH][1], BG_SM_TrackPos[SM_NORTH_PATH][2], BG_SM_TrackPos[SM_NORTH_PATH][3]))
        tracker->CastSpell(tracker, BG_SM_FEIGN_DEATH_STUN, true);
    else
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundSM: Failed to spawn some creatures Battleground not created!");
        return false;
    }

    WorldSafeLocsEntry const* safeLoc = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_ALLIANCE);
    if (!safeLoc || !AddSpiritGuide(SM_SPIRIT_ALLIANCE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o, ALLIANCE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundSM: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }

    safeLoc = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_HORDE);
    if (!safeLoc || !AddSpiritGuide(SM_SPIRIT_HORDE, safeLoc->x, safeLoc->y, safeLoc->z, safeLoc->o, HORDE))
    {
        TC_LOG_ERROR("bg.battleground", "BatteGroundSM: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }

    return true;
}

WorldSafeLocsEntry const* BattlegroundSM::GetClosestGraveYard(Player* player)
{
    SMBattlegroundGaveyards BGGraveyard = player->GetBGTeam() == ALLIANCE ? SM_GRAVEYARD_MAIN_ALLIANCE : SM_GRAVEYARD_MAIN_HORDE;
    WorldSafeLocsEntry const* safeLoc = sWorldSafeLocsStore.LookupEntry(BGGraveyard);
    return safeLoc;
}

void BattlegroundSM::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    Battleground::HandleKillPlayer(player, killer);
    EventPlayerDroppedFlag(player);

    m_fiveForFivePorgress[player->GetGUID()] = 0;
}

void BattlegroundSM::UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool addHonor)
{
    BattlegroundScoreMap::iterator i = PlayerScores.find(player->GetGUID());
    if (i == PlayerScores.end())                         // player not found
        return;

    switch (type)
    {
        case SCORE_CART_CONTROLLED:                           // Mine Carts captures
            ((BattlegroundSMScore*)i->second)->MineCartCaptures += value;
            break;
        default:
            Battleground::UpdatePlayerScore(player, type, value, addHonor);
            break;
    }
}

void BattlegroundSM::FillInitialWorldStates(WorldStateBuilder& builder)
{
    builder.AppendState(uint32(SM_MINE_CARTS_DISPLAY), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_ALLIANCE_RESOURCES), uint32(m_TeamScores[TEAM_ALLIANCE]));
    builder.AppendState(uint32(SM_HORDE_RESOURCES), uint32(m_TeamScores[TEAM_HORDE]));
    builder.AppendState(uint32(SM_MINE_CART_1), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_MINE_CART_2), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_DISPLAY_ALLIANCE_RESSOURCES), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_MINE_CART_3), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_DISPLAY_HORDE_RESSOURCES), uint32(DISPLAY_WORLDSTATE));
    builder.AppendState(uint32(SM_DISPLAY_PROGRESS_BAR), uint32(BG_SM_PROGRESS_BAR_DONT_SHOW)); // This shows the mine cart control bar
    builder.AppendState(uint32(SM_PROGRESS_BAR_STATUS), uint32(BG_SM_PROGRESS_BAR_NEUTRAL)); // Neutral
    builder.AppendState(uint32(SM_UNK), uint32(0));
}

void BattlegroundSM::EventPlayerClickedOnFlag(Player* player, Unit* target)
{
    if (GetStatus() != STATUS_IN_PROGRESS || !player || !target)
        return;

    if (target->HasAura(BG_SM_PREVENTION_AURA)) // Prevention aura
        return;

    target->CastSpell(target, BG_SM_PREVENTION_AURA, true);

    if (!target->HasAura(BG_SM_TRACK_SWITCH_OPENED) &&
        !target->HasAura(BG_SM_TRACK_SWITCH_CLOSED))
        target->CastSpell(target, BG_SM_TRACK_SWITCH_OPENED, false);
    else if (target->HasAura(BG_SM_TRACK_SWITCH_OPENED))
    {
        target->CastSpell(target, BG_SM_TRACK_SWITCH_CLOSED, false);
        target->RemoveAura(BG_SM_TRACK_SWITCH_OPENED);
    }
    else if (target->HasAura(BG_SM_TRACK_SWITCH_CLOSED))
    {
        target->CastSpell(target, BG_SM_TRACK_SWITCH_OPENED, false);
        target->RemoveAura(BG_SM_TRACK_SWITCH_CLOSED);
    }
}
