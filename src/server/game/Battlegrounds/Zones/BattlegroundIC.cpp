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

#include "Player.h"
#include "BattlegroundIC.h"
#include "WorldPacket.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Transport.h"
#include "ScriptedCreature.h"
#include "CreatureAI.h"
#include "BattlegroundMgr.h"

BattlegroundIC::BattlegroundIC()
{
    BgObjects.resize(MAX_NORMAL_GAMEOBJECTS_SPAWNS + MAX_AIRSHIPS_SPAWNS + MAX_HANGAR_TELEPORTERS_SPAWNS + MAX_FORTRESS_TELEPORTERS_SPAWNS + MAX_HANGAR_TELEPORTER_EFFECTS_SPAWNS + MAX_FORTRESS_TELEPORTER_EFFECTS_SPAWNS);
    BgCreatures.resize(MAX_NORMAL_NPCS_SPAWNS + MAX_WORKSHOP_SPAWNS + MAX_DOCKS_SPAWNS + MAX_SPIRIT_GUIDES_SPAWNS + MAX_HANGAR_NPCS_SPAWNS + MAX_HONORABLE_DEFENDER_TRIGGER_SPAWNS);

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_IC_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_IC_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_IC_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_IC_HAS_BEGUN;

    for (uint8 i = 0; i < 2; i++)
        factionReinforcements[i] = MAX_REINFORCEMENTS;

    for (uint8 i = 0; i < BG_IC_MAXDOOR; i++)
        GateStatus[i] = BG_IC_GATE_OK;

    closeFortressDoorsTimer = CLOSE_DOORS_TIME; // the doors are closed again... in a special way
    doorsClosed = false;
    docksTimer = DOCKS_UPDATE_TIME;
    resourceTimer = IC_RESOURCE_TIME;

    for (uint8 i = NODE_TYPE_REFINERY; i < MAX_NODE_TYPES; i++)
    {
        nodePoint[i] =  nodePointInitial[i];
        nodePoint[i].original_gameobject_entry = nodePoint[i].gameobject_entry;
    }

    siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;

    gunshipHorde = NULL;
    gunshipAlliance = NULL;

    if (sBattlegroundMgr->IsBGWeekend(GetTypeID()))
    {
        honorWinKills = 3;
        honorEndKills = 4;
    }
    else
    {
        honorWinKills = 1;
        honorEndKills = 2;
    }
}

BattlegroundIC::~BattlegroundIC() { }

void BattlegroundIC::HandlePlayerResurrect(Player* player)
{
    if (nodePoint[NODE_TYPE_QUARRY].nodeState == (player->GetBGTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_QUARRY, true);

    if (nodePoint[NODE_TYPE_REFINERY].nodeState == (player->GetBGTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_OIL_REFINERY, true);
}

void BattlegroundIC::DoAction(uint32 action, ObjectGuid var)
{
    if (action != ACTION_TELEPORT_PLAYER_TO_TRANSPORT)
        return;

    Player* player = ObjectAccessor::FindPlayer(var);

    if (!player || !gunshipAlliance || !gunshipHorde)
        return;

    player->CastSpell(player, SPELL_SLOW_FALL, true);

    (player->GetBGTeamId() == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde)->AddPassenger(player);
    auto&& info = TransportMovementInfo[player->GetBGTeamId()];

    player->m_movementInfo.transport.pos.m_positionX = info.GetPositionX();
    player->m_movementInfo.transport.pos.m_positionY = info.GetPositionY();
    player->m_movementInfo.transport.pos.m_positionZ = info.GetPositionZ();
    player->m_movementInfo.transport.pos.SetOrientation(info.GetOrientation());

    if (Transport* transport = player->GetTransport())
    {
        WorldLocation dest;
        dest.m_mapId = GetMapId();
        dest.Relocate(player->m_movementInfo.transport.pos.GetPosition());
        float m_orientation = dest.GetOrientation();
        transport->CalculatePassengerPosition(dest.m_positionX, dest.m_positionY, dest.m_positionZ, &m_orientation);
        player->TeleportTo(dest, TELE_TO_NOT_LEAVE_TRANSPORT);
    }
}

void BattlegroundIC::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!doorsClosed)
    {
        if (closeFortressDoorsTimer <= diff)
        {
            GetBGObject(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR01)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR01)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR02)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR02)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR03)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR03)->RemoveFromWorld();

            GetBGObject(BG_IC_GO_ALLIANCE_GATE_1)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
            GetBGObject(BG_IC_GO_HORDE_GATE_1)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
            GetBGObject(BG_IC_GO_ALLIANCE_GATE_2)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
            GetBGObject(BG_IC_GO_HORDE_GATE_2)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
            GetBGObject(BG_IC_GO_ALLIANCE_GATE_3)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
            GetBGObject(BG_IC_GO_HORDE_GATE_3)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);

            doorsClosed = true;
        } else closeFortressDoorsTimer -= diff;
    }

    for (uint8 i = NODE_TYPE_REFINERY; i < MAX_NODE_TYPES; i++)
    {
        uint32 faction = nodePoint[i].faction;

        if (nodePoint[i].nodeType == NODE_TYPE_DOCKS)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A ||
                nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                if (docksTimer <= diff)
                {
                    // we need to confirm this, i am not sure if this every 3 minutes
                    for (uint8 type = (faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_A : BG_IC_NPC_CATAPULT_1_H); type <= (faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_4_A : BG_IC_NPC_CATAPULT_4_H); type++)
                    {
                        if (Creature* catapult = GetBGCreature(type))
                        {
                            if (!catapult->IsAlive())
                            {
                                uint32 pos = type - (faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_A : BG_IC_NPC_CATAPULT_1_H);
                                DelCreature(type);
                                if (AddCreature(NPC_CATAPULT, type, faction,
                                    BG_IC_DocksVehiclesCatapults[pos].GetPositionX(), BG_IC_DocksVehiclesCatapults[pos].GetPositionY(),
                                    BG_IC_DocksVehiclesCatapults[pos].GetPositionZ(), BG_IC_DocksVehiclesCatapults[pos].GetOrientation(),
                                    DAY))
                                    GetBGCreature(type)->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                            }
                        }
                    }

                    // we need to confirm this is blizzlike,not sure if it is every 3 minutes
                    for (uint8 type = (faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_A : BG_IC_NPC_GLAIVE_THROWER_1_H); type <= (faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_2_A : BG_IC_NPC_GLAIVE_THROWER_2_H); type++)
                    {
                        if (Creature* glaiveThrower = GetBGCreature(type))
                        {
                            if (!glaiveThrower->IsAlive())
                            {
                                uint32 pos = type - (faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_A : BG_IC_NPC_GLAIVE_THROWER_1_H);
                                DelCreature(type);
                                if (AddCreature(nodePoint->faction == TEAM_ALLIANCE ? NPC_GLAIVE_THROWER_A : NPC_GLAIVE_THROWER_H, type, nodePoint->faction, BG_IC_DocksVehiclesGlaives[pos], RESPAWN_ONE_DAY))
                                    GetBGCreature(type)->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                            }
                        }
                    }

                    docksTimer = DOCKS_UPDATE_TIME;
                } else docksTimer -= diff;
            }
        }

        if (nodePoint[i].nodeType == NODE_TYPE_WORKSHOP)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A || nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                if (siegeEngineWorkshopTimer <= diff)
                {
                    uint8 type = (faction == TEAM_ALLIANCE ? BG_IC_NPC_SIEGE_ENGINE_A : BG_IC_NPC_SIEGE_ENGINE_H);

                    if (Creature* siege = GetBGCreature(type)) // this always should be true
                    {
                        if (siege->IsAlive())
                        {
                            if (siege->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_CANNOT_SWIM|UNIT_FLAG_IMMUNE_TO_PC))
                                // following sniffs the vehicle always has UNIT_FLAG_CANNOT_SWIM
                                siege->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_IMMUNE_TO_PC);
                        }
                        else
                        {
                            DelCreature(type);
                            if (Creature* siegeEngine = AddCreature((faction == TEAM_ALLIANCE ? NPC_SIEGE_ENGINE_A : NPC_SIEGE_ENGINE_H), type, faction,
                                BG_IC_WorkshopVehicles[4].GetPositionX(), BG_IC_WorkshopVehicles[4].GetPositionY(),
                                BG_IC_WorkshopVehicles[4].GetPositionZ(), BG_IC_WorkshopVehicles[4].GetOrientation(),
                                DAY))
                            {
                                siegeEngine->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                                if (siegeEngine->GetVehicleKit())
                                {
                                    if (Unit* flameTurret = siegeEngine->GetVehicleKit()->GetPassenger(1))
                                        flameTurret->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                                    if (Unit* flameTurret = siegeEngine->GetVehicleKit()->GetPassenger(2))
                                        flameTurret->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                                    if (Unit* siegeTurret = siegeEngine->GetVehicleKit()->GetPassenger(7))
                                        siegeTurret->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                                }
                            }
                        }
                    }

                    // we need to confirm this, i am not sure if this every 3 minutes
                    for (uint8 type = (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_A : BG_IC_NPC_DEMOLISHER_1_H); type <= (faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_4_A : BG_IC_NPC_DEMOLISHER_4_H); type++)
                    {
                        if (Creature* demolisher = GetBGCreature(type))
                        {
                            if (!demolisher->IsAlive())
                            {
                                uint32 pos = type - (faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_A : BG_IC_NPC_DEMOLISHER_1_H);
                                DelCreature(type);
                                if (AddCreature(NPC_DEMOLISHER, type, faction,
                                    BG_IC_WorkshopVehicles[pos].GetPositionX(), BG_IC_WorkshopVehicles[pos].GetPositionY(),
                                    BG_IC_WorkshopVehicles[pos].GetPositionZ(), BG_IC_WorkshopVehicles[pos].GetOrientation(),
                                    DAY))
                                    GetBGCreature(type)->SetFaction(BG_IC_Factions[(faction == TEAM_ALLIANCE ? 0 : 1)]);
                            }
                        }
                    }
                    siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;
                } else siegeEngineWorkshopTimer -= diff;
            }
        }

        // the point is waiting for a change on his banner
        if (nodePoint[i].needChange)
        {
            if (nodePoint[i].timer <= diff)
            {
                bool unused = false;
                uint32 nextBanner = GetNextBanner(&nodePoint[i], nodePoint[i].faction, true, unused);

                nodePoint[i].last_entry = nodePoint[i].gameobject_entry;
                nodePoint[i].gameobject_entry = nextBanner;
                // nodePoint[i].faction = the faction should be the same one...

                GameObject* banner = GetBGObject(nodePoint[i].gameobject_type);

                if (!banner) // this should never happen
                    return;

                float cords[4] = {banner->GetPositionX(), banner->GetPositionY(), banner->GetPositionZ(), banner->GetOrientation() };

                DelObject(nodePoint[i].gameobject_type);
                AddObject(nodePoint[i].gameobject_type, nodePoint[i].gameobject_entry, cords[0], cords[1], cords[2], cords[3], 0, 0, 0, 0, RESPAWN_IMMEDIATELY);

                GetBGObject(nodePoint[i].gameobject_type)->SetFaction(nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_Factions[1] : BG_IC_Factions[0]);

                UpdateNodeWorldState(&nodePoint[i]);
                HandleCapturedNodes(&nodePoint[i], false);

                SendMessage2ToAll(LANG_BG_IC_TEAM_HAS_TAKEN_NODE, CHAT_MSG_BG_SYSTEM_NEUTRAL, NULL, (nodePoint[i].faction == TEAM_ALLIANCE ? LANG_BG_IC_ALLIANCE : LANG_BG_IC_HORDE), nodePoint[i].string);

                nodePoint[i].needChange = false;
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME;
            } else nodePoint[i].timer -= diff;
        }
    }

    if (resourceTimer <= diff)
    {
        for (uint8 i = 0; i < NODE_TYPE_DOCKS; i++)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A || nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                factionReinforcements[nodePoint[i].faction] += 1;
                RewardHonorToTeam(RESOURCE_HONOR_AMOUNT, nodePoint[i].faction == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                UpdateWorldState((nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_ALLIANCE_RENFORT : BG_IC_HORDE_RENFORT), factionReinforcements[nodePoint[i].faction]);
            }
        }
        resourceTimer = IC_RESOURCE_TIME;
    } else resourceTimer -= diff;
}

void BattlegroundIC::StartingEventCloseDoors() { }

void BattlegroundIC::StartingEventOpenDoors()
{
    // after 20 seconds they should be despawned
    DoorOpen(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR01);
    DoorOpen(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR01);
    DoorOpen(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR02);
    DoorOpen(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR02);
    DoorOpen(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR03);
    DoorOpen(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR03);

    DoorOpen(BG_IC_GO_DOODAD_HU_PORTCULLIS01_1);
    DoorOpen(BG_IC_GO_DOODAD_HU_PORTCULLIS01_2);
    DoorOpen(BG_IC_GO_DOODAD_VR_PORTCULLIS01_1);
    DoorOpen(BG_IC_GO_DOODAD_VR_PORTCULLIS01_2);

    for (uint8 i = 0; i < MAX_FORTRESS_TELEPORTERS_SPAWNS; i++)
        GetBGObject(BG_IC_Teleporters[i].type)->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

    for (uint8 i = 0; i < MAX_FORTRESS_TELEPORTER_EFFECTS_SPAWNS; ++i)
        GetBGObject(BG_IC_TeleporterEffects[i].type)->SetGoState(GO_STATE_ACTIVE);
}

void BattlegroundIC::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map, default values are set in constructor
    BattlegroundICScore* sc = new BattlegroundICScore;
    sc->Team = player->GetBGTeam();
    PlayerScores[player->GetGUID()] = sc;

    if (nodePoint[NODE_TYPE_QUARRY].nodeState == (player->GetBGTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player,SPELL_QUARRY,true);

    if (nodePoint[NODE_TYPE_REFINERY].nodeState == (player->GetBGTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player,SPELL_OIL_REFINERY,true);
}

void BattlegroundIC::RemovePlayer(Player* player, ObjectGuid /*guid*/, uint32 /*team*/)
{
    if (player)
    {
        player->RemoveAura(SPELL_QUARRY);
        player->RemoveAura(SPELL_OIL_REFINERY);
    }
}

void BattlegroundIC::HandleAreaTrigger(Player* player, uint32 trigger, bool /*entered*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // not really possible to get this with the ships being broken and all, but it's here nonetheless.
    if (trigger == 5555 && player->GetBGTeamId() == TEAM_HORDE)
    {
        if (GateStatus[BG_IC_A_FRONT] != BG_IC_GATE_DESTROYED && GateStatus[BG_IC_A_WEST] != BG_IC_GATE_DESTROYED && GateStatus[BG_IC_A_EAST] != BG_IC_GATE_DESTROYED)
            player->CastSpell(player, SPELL_BACK_DOOR_JOB_ACHIEVEMENT, true);
    }
    else if (trigger == 5535 && player->GetBGTeamId() == TEAM_ALLIANCE)
    {
        if (GateStatus[BG_IC_H_FRONT] != BG_IC_GATE_DESTROYED && GateStatus[BG_IC_H_WEST] != BG_IC_GATE_DESTROYED && GateStatus[BG_IC_H_EAST] != BG_IC_GATE_DESTROYED)
            player->CastSpell(player, SPELL_BACK_DOOR_JOB_ACHIEVEMENT, true);
    }
}

void BattlegroundIC::UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor)
{
    std::map<ObjectGuid, BattlegroundScore*>::iterator itr = PlayerScores.find(player->GetGUID());

    if (itr == PlayerScores.end())                         // player not found...
        return;

    switch (type)
    {
        case SCORE_BASES_ASSAULTED:
            ((BattlegroundICScore*)itr->second)->BasesAssaulted += value;
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, IC_OBJECTIVE_ASSAULT_BASE, 1);
            break;
        case SCORE_BASES_DEFENDED:
            ((BattlegroundICScore*)itr->second)->BasesDefended += value;
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, IC_OBJECTIVE_DEFEND_BASE, 1);
            break;
        default:
            Battleground::UpdatePlayerScore(player, type, value, doAddHonor);
            break;
    }
}

void BattlegroundIC::FillInitialWorldStates(WorldStateBuilder& builder)
{
    builder.AppendState(uint32(BG_IC_ALLIANCE_RENFORT_SET), uint32(1));
    builder.AppendState(uint32(BG_IC_HORDE_RENFORT_SET), uint32(1));
    builder.AppendState(uint32(BG_IC_ALLIANCE_RENFORT), uint32(factionReinforcements[TEAM_ALLIANCE]));
    builder.AppendState(uint32(BG_IC_HORDE_RENFORT), uint32(factionReinforcements[TEAM_HORDE]));

    for (uint8 i = 0; i < MAX_FORTRESS_GATES_SPAWNS; i++)
    {
        uint32 uws = GetWorldStateFromGateEntry(BG_IC_ObjSpawnlocs[i].entry,(GateStatus[GetGateIDFromEntry(BG_IC_ObjSpawnlocs[i].entry)] == BG_IC_GATE_DESTROYED ? true : false));
        builder.AppendState(uint32(uws), uint32(1));
    }

    for (uint8 i = 0 ; i < MAX_NODE_TYPES ; i++)
        for (uint8 j = 0; j < 5; ++j)
            builder.AppendState(uint32(nodePoint[i].worldStates[j]), uint32(j == nodePoint[i].nodeState));
}

bool BattlegroundIC::SetupBattleground()
{
    for (uint8 i = 0; i < MAX_NORMAL_GAMEOBJECTS_SPAWNS; i++)
    {
        if (!AddObject(BG_IC_ObjSpawnlocs[i].type, BG_IC_ObjSpawnlocs[i].entry, BG_IC_ObjSpawnlocs[i].x, BG_IC_ObjSpawnlocs[i].y, BG_IC_ObjSpawnlocs[i].z, BG_IC_ObjSpawnlocs[i].o, 0, 0, 0, 0, RESPAWN_IMMEDIATELY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning gameobject {}",BG_IC_ObjSpawnlocs[i].entry);
            return false;
        }
    }

    for (uint8 i = 0; i < MAX_FORTRESS_TELEPORTERS_SPAWNS; ++i)
    {
        if (!AddObject(BG_IC_Teleporters[i].type, BG_IC_Teleporters[i].entry, BG_IC_Teleporters[i].x, BG_IC_Teleporters[i].y, BG_IC_Teleporters[i].z, BG_IC_Teleporters[i].o, 0, 0, 0, 0, RESPAWN_IMMEDIATELY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest | Starting Event Open Doors: There was an error spawning gameobject {}", BG_IC_Teleporters[i].entry);
            return false;
        }
    }

    for (uint8 i = 0; i < MAX_FORTRESS_TELEPORTER_EFFECTS_SPAWNS; ++i)
    {
        if (!AddObject(BG_IC_TeleporterEffects[i].type, BG_IC_TeleporterEffects[i].entry, BG_IC_TeleporterEffects[i].x, BG_IC_TeleporterEffects[i].y, BG_IC_TeleporterEffects[i].z, BG_IC_TeleporterEffects[i].o, 0, 0, 0, 0, RESPAWN_IMMEDIATELY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest | Starting Event Open Doors: There was an error spawning gameobject {}", BG_IC_Teleporters[i].entry);
            return false;
        }
    }

    for (uint8 i = 0; i < MAX_NORMAL_NPCS_SPAWNS; i++)
    {
        if (!AddCreature(BG_IC_NpcSpawnlocs[i].entry, BG_IC_NpcSpawnlocs[i].type, BG_IC_NpcSpawnlocs[i].team, BG_IC_NpcSpawnlocs[i].x, BG_IC_NpcSpawnlocs[i].y, BG_IC_NpcSpawnlocs[i].z, BG_IC_NpcSpawnlocs[i].o, RESPAWN_IMMEDIATELY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning creature {}",BG_IC_NpcSpawnlocs[i].entry);
            return false;
        }
    }

    if (!AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+3,BG_IC_SpiritGuidePos[5][0], BG_IC_SpiritGuidePos[5][1],BG_IC_SpiritGuidePos[5][2], BG_IC_SpiritGuidePos[5][3],ALLIANCE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+4,BG_IC_SpiritGuidePos[6][0], BG_IC_SpiritGuidePos[6][1],BG_IC_SpiritGuidePos[6][2], BG_IC_SpiritGuidePos[6][3],HORDE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+5,BG_IC_SpiritGuidePos[7][0], BG_IC_SpiritGuidePos[7][1],BG_IC_SpiritGuidePos[7][2], BG_IC_SpiritGuidePos[7][3],ALLIANCE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+6,BG_IC_SpiritGuidePos[8][0], BG_IC_SpiritGuidePos[8][1],BG_IC_SpiritGuidePos[8][2], BG_IC_SpiritGuidePos[8][3],HORDE))
    {
        TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn initial spirit guide!");
        return false;
    }

    for (uint8 i = 0; i < MAX_NODE_TYPES; i++)
    {
        ICNodePoint* nodePoint = &this->nodePoint[i];
        if (nodePoint->faction == TEAM_NEUTRAL)
            continue;

        bool is25ydTrigger = nodePoint->nodeType == NODE_TYPE_GRAVEYARD_A || nodePoint->nodeType == NODE_TYPE_GRAVEYARD_H;

        DelCreature(BG_IC_NPC_HONORABLE_DEFENDER_TRIGGER_1 + nodePoint->nodeType);
        if (Creature* trigger = AddCreature(nodePoint->faction == TEAM_ALLIANCE ? (is25ydTrigger ? NPC_HONORABLE_DEFENDER_TRIGGER_ALLIANCE_25_YD : NPC_HONORABLE_DEFENDER_TRIGGER_ALLIANCE) : (is25ydTrigger ? NPC_HONORABLE_DEFENDER_TRIGGER_HORDE_25_YD : NPC_HONORABLE_DEFENDER_TRIGGER_HORDE),
            BG_IC_NPC_HONORABLE_DEFENDER_TRIGGER_1 + nodePoint->nodeType,
            (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE),
            BG_IC_HonorableDefenderPos[nodePoint->nodeType]))
            trigger->CastSpell(trigger, is25ydTrigger ? SPELL_HONORABLE_DEFENDER_25Y : SPELL_HONORABLE_DEFENDER_60Y, false);
        else
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn initial honorable defender trigger! point: {}, team: {},", nodePoint->nodeType, nodePoint->faction);
            return false;
        }
    }

    gunshipHorde = sTransportMgr->CreateTransport(GO_HORDE_GUNSHIP, 0, GetBgMap());
    gunshipAlliance = sTransportMgr->CreateTransport(GO_ALLIANCE_GUNSHIP, 0, GetBgMap());

    if (!gunshipAlliance || !gunshipHorde)
    {
        TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error creating gunships!");
        return false;
    }

    gunshipHorde->EnableMovement(false);
    gunshipAlliance->EnableMovement(false);

    // setting correct factions for Keep Cannons
    for (uint8 i = BG_IC_NPC_KEEP_CANNON_1; i <= BG_IC_NPC_KEEP_CANNON_12; ++i)
        GetBGCreature(i)->SetFaction(BG_IC_Factions[0]);
    for (uint8 i = BG_IC_NPC_KEEP_CANNON_13; i <= BG_IC_NPC_KEEP_CANNON_24; ++i)
        GetBGCreature(i)->SetFaction(BG_IC_Factions[1]);

    // correcting spawn time for keeps bombs
    for (uint8 i = BG_IC_GO_HUGE_SEAFORIUM_BOMBS_A_1; i < BG_IC_GO_HUGE_SEAFORIUM_BOMBS_H_4; i++)
        GetBGObject(i)->SetRespawnTime(10);

    // Make bosses invisible
    ActivateBoss(TEAM_ALLIANCE, false);
    ActivateBoss(TEAM_HORDE, false);

    return true;
}

void BattlegroundIC::HandleKillUnit(Creature* unit, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
       return;

    uint32 entry = unit->GetEntry();
    if (entry == NPC_HIGH_COMMANDER_HALFORD_WYRMBANE)
    {
        RewardHonorToTeam(WINNER_HONOR_AMOUNT,HORDE);
        EndBattleground(HORDE);
    }
    else if (entry == NPC_OVERLORD_AGMAR)
    {
        RewardHonorToTeam(WINNER_HONOR_AMOUNT,ALLIANCE);
        EndBattleground(ALLIANCE);
    }

    // Achievement Mowed Down
    // TO-DO: This should be done on the script of each vehicle of the BG.
    if (unit->IsVehicle())
        killer->CastSpell(killer,SPELL_DESTROYED_VEHICLE_ACHIEVEMENT,true);
}

void BattlegroundIC::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    Battleground::HandleKillPlayer(player, killer);

    factionReinforcements[player->GetBGTeamId()] -= 1;

    UpdateWorldState((player->GetBGTeamId() == TEAM_ALLIANCE ? BG_IC_ALLIANCE_RENFORT : BG_IC_HORDE_RENFORT), factionReinforcements[player->GetBGTeamId()]);

    // we must end the battleground
    if (factionReinforcements[player->GetBGTeamId()] < 1)
        EndBattleground(killer->GetBGTeam());
}

void BattlegroundIC::EndBattleground(uint32 winner)
{
    // Win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(honorWinKills), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(honorWinKills), HORDE);
    // Complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(honorEndKills), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(honorEndKills), HORDE);

    SendMessage2ToAll(LANG_BG_IC_TEAM_WINS, CHAT_MSG_BG_SYSTEM_NEUTRAL, NULL, (winner == ALLIANCE ? LANG_BG_IC_ALLIANCE : LANG_BG_IC_HORDE));

    Battleground::EndBattleground(winner);
}

void BattlegroundIC::RealocatePlayers(ICNodePointType nodeType)
{
    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<ObjectGuid> ghost_list = m_ReviveQueue[BgCreatures[BG_IC_NPC_SPIRIT_GUIDE_1+nodeType-2]];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const* closestGrave = NULL;
        for (std::vector<ObjectGuid>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!closestGrave)                              // cache
                closestGrave = GetClosestGraveYard(player);

            if (closestGrave)
                player->TeleportTo(GetMapId(), closestGrave->x, closestGrave->y, closestGrave->z, player->GetOrientation());
        }
    }
}

void BattlegroundIC::EventPlayerClickedOnFlag(Player* player, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // All the node points are iterated to find the clicked one
    for (uint8 i = 0; i < MAX_NODE_TYPES; i++)
    {
        if (nodePoint[i].gameobject_entry == target_obj->GetEntry())
        {
            // THIS SHOULD NEEVEER HAPPEN
            if (nodePoint[i].faction == player->GetBGTeamId())
                return;

            bool neutralRecaptured = false;
            uint32 nextBanner = GetNextBanner(&nodePoint[i], player->GetBGTeamId(), false, neutralRecaptured);

            // we set the new settings of the nodePoint
            nodePoint[i].faction = player->GetBGTeamId();
            if (!neutralRecaptured)
                nodePoint[i].last_entry = nodePoint[i].gameobject_entry;
            nodePoint[i].gameobject_entry = nextBanner;

            UpdateNodeWorldState(&nodePoint[i]);

            // this is just needed if the next banner is grey
            if (nodePoint[i].banners[BANNER_A_CONTESTED] == nextBanner || nodePoint[i].banners[BANNER_H_CONTESTED] == nextBanner)
            {
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME; // 1 minute for last change (real faction banner)
                nodePoint[i].needChange = true;

                RealocatePlayers(nodePoint[i].nodeType);

                // if we are here means that the point has been lost, or it is the first capture

                if (nodePoint[i].nodeType != NODE_TYPE_REFINERY && nodePoint[i].nodeType != NODE_TYPE_QUARRY)
                    if (BgCreatures[BG_IC_NPC_SPIRIT_GUIDE_1+(nodePoint[i].nodeType)-2])
                        DelCreature(BG_IC_NPC_SPIRIT_GUIDE_1+(nodePoint[i].nodeType)-2);

                UpdatePlayerScore(player, SCORE_BASES_ASSAULTED, 1);

                SendMessage2ToAll(LANG_BG_IC_TEAM_ASSAULTED_NODE_1, CHAT_MSG_BG_SYSTEM_NEUTRAL, player, nodePoint[i].string);
                SendMessage2ToAll(LANG_BG_IC_TEAM_ASSAULTED_NODE_2, CHAT_MSG_BG_SYSTEM_NEUTRAL, player, nodePoint[i].string, (player->GetBGTeamId() == TEAM_ALLIANCE ? LANG_BG_IC_ALLIANCE : LANG_BG_IC_HORDE));
                HandleContestedNodes(&nodePoint[i]);
            }
            else if (nextBanner == nodePoint[i].banners[BANNER_A_CONTROLLED] || nextBanner == nodePoint[i].banners[BANNER_H_CONTROLLED]) // if we are going to spawn the definitve faction banner, we dont need the timer anymore
            {
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME;
                nodePoint[i].needChange = false;
                SendMessage2ToAll(LANG_BG_IC_TEAM_DEFENDED_NODE, CHAT_MSG_BG_SYSTEM_NEUTRAL, player, nodePoint[i].string);
                HandleCapturedNodes(&nodePoint[i],true);
                UpdatePlayerScore(player, SCORE_BASES_DEFENDED, 1);
            }

            GameObject* banner = GetBGObject(nodePoint[i].gameobject_type);

            if (!banner) // this should never happen
                return;

            float cords[4] = {banner->GetPositionX(), banner->GetPositionY(), banner->GetPositionZ(), banner->GetOrientation() };

            DelObject(nodePoint[i].gameobject_type);
            AddObject(nodePoint[i].gameobject_type,nodePoint[i].gameobject_entry,cords[0],cords[1],cords[2],cords[3],0,0,0,0,RESPAWN_IMMEDIATELY);

            GetBGObject(nodePoint[i].gameobject_type)->SetFaction(nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_Factions[1] : BG_IC_Factions[0]);

            if (nodePoint[i].nodeType == NODE_TYPE_WORKSHOP && nodePoint[i].needChange)
            {
                DelObject(BG_IC_GO_SEAFORIUM_BOMBS_1);
                DelObject(BG_IC_GO_SEAFORIUM_BOMBS_2);
            }

            // we dont need iterating if we are here
            // If the needChange bool was set true, we will handle the rest in the Update Map function.
            return;
        }
    }
}

void BattlegroundIC::UpdateNodeWorldState(ICNodePoint* nodePoint)
{
    //updating worldstate
    if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTROLLED])
        nodePoint->nodeState = NODE_STATE_CONTROLLED_A;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTESTED])
        nodePoint->nodeState = NODE_STATE_CONFLICT_A;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTROLLED])
        nodePoint->nodeState = NODE_STATE_CONTROLLED_H;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTESTED])
        nodePoint->nodeState = NODE_STATE_CONFLICT_H;
    else
        nodePoint->nodeState = NODE_STATE_UNCONTROLLED;

    uint32 worldstate = nodePoint->worldStates[nodePoint->nodeState];

    // with this we are sure we dont bug the client
    for (uint8 i = 0; i < 5; i++)
        UpdateWorldState(nodePoint->worldStates[i],0);

    UpdateWorldState(worldstate,1);
}

uint32 BattlegroundIC::GetNextBanner(ICNodePoint* nodePoint, uint32 team, bool returnDefinitve, bool& neutralRecaptured)
{
    // this is only used in the update map function
    if (returnDefinitve)
        // here is a special case, here we must return the definitve faction banner after the grey banner was spawned 1 minute
        return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTROLLED : BANNER_H_CONTROLLED)];

    // there were no changes, this point has never been captured by any faction or at least clicked
    if (nodePoint->last_entry == 0)
        // 1 returns the CONTESTED ALLIANCE BANNER, 3 returns the HORDE one
        return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];

    // If the actual banner is the definitive faction banner, we must return the grey banner of the player's faction
    if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTROLLED] || nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTROLLED])
        return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];

    // If the actual banner is the grey faction banner, we must return the previous banner
    if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTESTED] || nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTESTED])
    {
        if (nodePoint->last_entry == nodePoint->original_gameobject_entry
            // Check that this point was initially neutral (i.e. not keep's graveyard)
            && nodePoint->original_gameobject_entry != nodePoint->banners[BANNER_A_CONTROLLED]
            && nodePoint->original_gameobject_entry != nodePoint->banners[BANNER_A_CONTESTED]
            && nodePoint->original_gameobject_entry != nodePoint->banners[BANNER_H_CONTROLLED]
            && nodePoint->original_gameobject_entry != nodePoint->banners[BANNER_H_CONTESTED])
        {
            neutralRecaptured = true;
            return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
        }
        return nodePoint->last_entry;
    }

    // we should never be here...
    TC_LOG_ERROR("bg.battleground", "Isle Of Conquest: Unexpected return in GetNextBanner function");
    return 0;
}

void BattlegroundIC::HandleContestedNodes(ICNodePoint* nodePoint)
{
    if (nodePoint->nodeType == NODE_TYPE_HANGAR)
    {
        if (gunshipAlliance && gunshipHorde)
            (nodePoint->faction == TEAM_ALLIANCE ? gunshipHorde : gunshipAlliance)->EnableMovement(false);

        for (uint8 u = BG_IC_GO_HANGAR_TELEPORTER_1; u <= BG_IC_GO_HANGAR_TELEPORTER_3; ++u)
            DelObject(u);

        for (uint8 u = BG_IC_GO_HANGAR_TELEPORTER_EFFECT_1; u <= BG_IC_GO_HANGAR_TELEPORTER_EFFECT_3; ++u)
            DelObject(u);

        DelCreature(BG_IC_NPC_WORLD_TRIGGER_NOT_FLOATING);

        for (uint8 u = 0; u < MAX_CAPTAIN_SPAWNS_PER_FACTION; ++u)
        {
            uint8 type = BG_IC_NPC_GUNSHIP_CAPTAIN_1 + u;
            DelCreature(type);
        }

        std::list<Creature*> cannons;
        if (nodePoint->faction == TEAM_HORDE)
            gunshipAlliance->GetCreatureListWithEntryInGrid(cannons, NPC_ALLIANCE_GUNSHIP_CANNON, 150.0f);
        else
            gunshipHorde->GetCreatureListWithEntryInGrid(cannons, NPC_HORDE_GUNSHIP_CANNON, 150.0f);

        for (Creature* cannon : cannons)
        {
            cannon->GetVehicleKit()->RemoveAllPassengers();
            cannon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }
    //else if (nodePoint->nodeType == NODE_TYPE_WORKSHOP)
    //{
    //    DelObject(BG_IC_GO_SEAFORIUM_BOMBS_1);
    //    DelObject(BG_IC_GO_SEAFORIUM_BOMBS_2);
    //}

    switch (nodePoint->gameobject_type)
    {
        case BG_IC_GO_QUARRY_BANNER:
            RemoveAuraOnTeam(SPELL_QUARRY,(nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
            break;
        case BG_IC_GO_REFINERY_BANNER:
            RemoveAuraOnTeam(SPELL_OIL_REFINERY,(nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
            break;
        default:
            break;
    }
}

void BattlegroundIC::HandleCapturedNodes(ICNodePoint* nodePoint, bool recapture)
{
    if (nodePoint->nodeType != NODE_TYPE_REFINERY && nodePoint->nodeType != NODE_TYPE_QUARRY)
    {
        if (!AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+nodePoint->nodeType-2,
            BG_IC_SpiritGuidePos[nodePoint->nodeType][0], BG_IC_SpiritGuidePos[nodePoint->nodeType][1],
            BG_IC_SpiritGuidePos[nodePoint->nodeType][2], BG_IC_SpiritGuidePos[nodePoint->nodeType][3],
            (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE)))
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn spirit guide! point: {}, team: {},", nodePoint->nodeType, nodePoint->faction);
    }

    // Spawn Honorable Defender Trigger
    {
        bool is25ydTrigger = nodePoint->nodeType == NODE_TYPE_GRAVEYARD_A || nodePoint->nodeType == NODE_TYPE_GRAVEYARD_H;

        DelCreature(BG_IC_NPC_HONORABLE_DEFENDER_TRIGGER_1 + nodePoint->nodeType);
        if (Creature* trigger = AddCreature(nodePoint->faction == TEAM_ALLIANCE ? (is25ydTrigger ? NPC_HONORABLE_DEFENDER_TRIGGER_ALLIANCE_25_YD : NPC_HONORABLE_DEFENDER_TRIGGER_ALLIANCE) : (is25ydTrigger ? NPC_HONORABLE_DEFENDER_TRIGGER_HORDE_25_YD : NPC_HONORABLE_DEFENDER_TRIGGER_HORDE),
            BG_IC_NPC_HONORABLE_DEFENDER_TRIGGER_1 + nodePoint->nodeType,
            (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE),
            BG_IC_HonorableDefenderPos[nodePoint->nodeType]))
            trigger->CastSpell(trigger, is25ydTrigger ? SPELL_HONORABLE_DEFENDER_25Y : SPELL_HONORABLE_DEFENDER_60Y, false);
        else
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn honorable defender trigger! point: {}, team: {},", nodePoint->nodeType, nodePoint->faction);
    }

    switch (nodePoint->gameobject_type)
    {
        case BG_IC_GO_HANGAR_BANNER:
        {
            if (!gunshipAlliance || !gunshipHorde)
                break;

            std::list<Creature*> cannons;
            if (nodePoint->faction == TEAM_ALLIANCE)
                gunshipAlliance->GetCreatureListWithEntryInGrid(cannons, NPC_ALLIANCE_GUNSHIP_CANNON, 150.0f);
            else
                gunshipHorde->GetCreatureListWithEntryInGrid(cannons, NPC_HORDE_GUNSHIP_CANNON, 150.0f);

            for (Creature* cannon : cannons)
                cannon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            for (uint8 u = 0; u < MAX_HANGAR_TELEPORTERS_SPAWNS; ++u)
            {
                uint8 type = BG_IC_GO_HANGAR_TELEPORTER_1 + u;
                if (!AddObject(type, (nodePoint->faction == TEAM_ALLIANCE ? GO_ALLIANCE_GUNSHIP_PORTAL : GO_HORDE_GUNSHIP_PORTAL), BG_IC_HangarTeleporters[u], 0, 0, 0, 0, RESPAWN_IMMEDIATELY))
                    TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning a gunship portal. Type: {}", BG_IC_GO_HANGAR_TELEPORTER_1 + u);
            }

            for (uint8 u = 0; u < MAX_HANGAR_TELEPORTER_EFFECTS_SPAWNS; ++u)
            {
                uint8 type = BG_IC_GO_HANGAR_TELEPORTER_EFFECT_1 + u;
                if (!AddObject(type, (nodePoint->faction == TEAM_ALLIANCE ? GO_ALLIANCE_GUNSHIP_PORTAL_EFFECTS : GO_HORDE_GUNSHIP_PORTAL_EFFECTS), BG_IC_HangarTeleporterEffects[u], 0, 0, 0, 0, RESPAWN_IMMEDIATELY, GO_STATE_ACTIVE))
                    TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning a gunship portal effects. Type: {}", BG_IC_GO_HANGAR_TELEPORTER_1 + u);
            }

            for (uint8 u = 0; u < MAX_TRIGGER_SPAWNS_PER_FACTION; ++u)
            {
                if (!AddCreature(NPC_WORLD_TRIGGER_NOT_FLOATING, BG_IC_NPC_WORLD_TRIGGER_NOT_FLOATING, nodePoint->faction, BG_IC_HangarTrigger[nodePoint->faction], RESPAWN_IMMEDIATELY, nodePoint->faction == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde))
                    TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning a world trigger. Type: {}", BG_IC_NPC_WORLD_TRIGGER_NOT_FLOATING);
            }

            for (uint8 u = 0; u < MAX_CAPTAIN_SPAWNS_PER_FACTION; ++u)
            {
                uint8 type = BG_IC_NPC_GUNSHIP_CAPTAIN_1 + u;

                if (type == BG_IC_NPC_GUNSHIP_CAPTAIN_1)
                    if (AddCreature(nodePoint->faction == TEAM_ALLIANCE ? NPC_ALLIANCE_GUNSHIP_CAPTAIN : NPC_HORDE_GUNSHIP_CAPTAIN, type, nodePoint->faction, BG_IC_HangarCaptains[nodePoint->faction == TEAM_ALLIANCE ? 2 : 0], RESPAWN_IMMEDIATELY))
                        GetBGCreature(BG_IC_NPC_GUNSHIP_CAPTAIN_1)->GetAI()->DoAction(ACTION_GUNSHIP_READY);

                if (type == BG_IC_NPC_GUNSHIP_CAPTAIN_2)
                    if (!AddCreature(nodePoint->faction == TEAM_ALLIANCE ? NPC_ALLIANCE_GUNSHIP_CAPTAIN : NPC_HORDE_GUNSHIP_CAPTAIN, type, nodePoint->faction, BG_IC_HangarCaptains[nodePoint->faction == TEAM_ALLIANCE ? 3 : 1], RESPAWN_IMMEDIATELY, nodePoint->faction == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde))
                        TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning a world trigger. Type: {}", BG_IC_NPC_GUNSHIP_CAPTAIN_2);
            }

            (nodePoint->faction == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde)->EnableMovement(true);
            break;
        }
        case BG_IC_GO_QUARRY_BANNER:
            RemoveAuraOnTeam(SPELL_QUARRY,(nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
            CastSpellOnTeam(SPELL_QUARRY,(nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE));
            break;
        case BG_IC_GO_REFINERY_BANNER:
            RemoveAuraOnTeam(SPELL_OIL_REFINERY,(nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
            CastSpellOnTeam(SPELL_OIL_REFINERY,(nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE));
            break;
        case BG_IC_GO_DOCKS_BANNER:
        {
            if (recapture)
                break;

            if (docksTimer < DOCKS_UPDATE_TIME)
                docksTimer = DOCKS_UPDATE_TIME;

            // we must del opposing faction vehicles when the node is captured (unused ones)
            for (uint8 i = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_H : BG_IC_NPC_GLAIVE_THROWER_1_A); i <= (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_2_H : BG_IC_NPC_GLAIVE_THROWER_2_A); i++)
            {
                if (Creature* glaiveThrower = GetBGCreature(i))
                {
                    if (Vehicle* vehicleGlaive = glaiveThrower->GetVehicleKit())
                    {
                        if (!vehicleGlaive->GetPassenger(0))
                            DelCreature(i);
                    }
                }
            }

            for (uint8 i = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_H : BG_IC_NPC_CATAPULT_1_A); i <= (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_4_H : BG_IC_NPC_CATAPULT_4_A); i++)
            {
                if (Creature* catapult = GetBGCreature(i))
                {
                    if (Vehicle* vehicleGlaive = catapult->GetVehicleKit())
                    {
                        if (!vehicleGlaive->GetPassenger(0))
                            DelCreature(i);
                    }
                }
            }

            // spawning glaive throwers
            for (uint8 i = 0; i < MAX_GLAIVE_THROWERS_SPAWNS_PER_FACTION; i++)
            {
                uint8 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_A : BG_IC_NPC_GLAIVE_THROWER_1_H) + i;

                if (GetBGCreature(type) && GetBGCreature(type)->IsAlive())
                    continue;

                if (AddCreature(nodePoint->faction == TEAM_ALLIANCE ? NPC_GLAIVE_THROWER_A : NPC_GLAIVE_THROWER_H, type, nodePoint->faction,
                    BG_IC_DocksVehiclesGlaives[i].GetPositionX(), BG_IC_DocksVehiclesGlaives[i].GetPositionY(),
                    BG_IC_DocksVehiclesGlaives[i].GetPositionZ(), BG_IC_DocksVehiclesGlaives[i].GetOrientation(),
                    DAY))
                    GetBGCreature(type)->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
            }

            // spawning catapults
            for (uint8 i = 0; i < MAX_CATAPULTS_SPAWNS_PER_FACTION; i++)
            {
                uint8 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_A : BG_IC_NPC_CATAPULT_1_H) + i;

                if (GetBGCreature(type) && GetBGCreature(type)->IsAlive())
                    continue;

                if (AddCreature(NPC_CATAPULT, type, nodePoint->faction,
                    BG_IC_DocksVehiclesCatapults[i].GetPositionX(), BG_IC_DocksVehiclesCatapults[i].GetPositionY(),
                    BG_IC_DocksVehiclesCatapults[i].GetPositionZ(), BG_IC_DocksVehiclesCatapults[i].GetOrientation(),
                    DAY))
                    GetBGCreature(type)->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
            }
            break;
        }
        case BG_IC_GO_WORKSHOP_BANNER:
        {
            if (siegeEngineWorkshopTimer < WORKSHOP_UPDATE_TIME)
                siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;

            if (!recapture)
            {
                // we must del opposing faction vehicles when the node is captured (unused ones)
                for (uint8 i = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_H : BG_IC_NPC_DEMOLISHER_1_A); i <= (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_4_H : BG_IC_NPC_DEMOLISHER_4_A); i++)
                {
                    if (Creature* demolisher = GetBGCreature(i))
                    {
                        if (Vehicle* vehicleDemolisher = demolisher->GetVehicleKit())
                        {
                            if (!vehicleDemolisher->GetPassenger(0) && !vehicleDemolisher->GetPassenger(2) && !vehicleDemolisher->GetPassenger(3))
                                DelCreature(i);
                        }
                    }
                }

                for (uint8 i = 0; i < MAX_DEMOLISHERS_SPAWNS_PER_FACTION; i++)
                {
                    uint8 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_A : BG_IC_NPC_DEMOLISHER_1_H)+i;

                    if (GetBGCreature(type) && GetBGCreature(type)->IsAlive())
                        continue;

                    if (AddCreature(NPC_DEMOLISHER,type,nodePoint->faction,
                        BG_IC_WorkshopVehicles[i].GetPositionX(),BG_IC_WorkshopVehicles[i].GetPositionY(),
                        BG_IC_WorkshopVehicles[i].GetPositionZ(),BG_IC_WorkshopVehicles[i].GetOrientation(),
                        DAY))
                        GetBGCreature(type)->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                }

                // we check if the opossing siege engine is in use
                int8 enemySiege = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_SIEGE_ENGINE_H : BG_IC_NPC_SIEGE_ENGINE_A);

                if (Creature* siegeEngine = GetBGCreature(enemySiege))
                {
                    if (Vehicle* vehicleSiege = siegeEngine->GetVehicleKit())
                    {
                        Unit* flameTurret1 = vehicleSiege->GetPassenger(1);
                        Unit* flameTurret2 = vehicleSiege->GetPassenger(2);
                        Unit* siegeTurret = vehicleSiege->GetPassenger(7);
                        if (!vehicleSiege->GetPassenger(0) &&
                            !(flameTurret1 && flameTurret1->GetVehicleKit() && flameTurret1->GetVehicleKit()->GetPassenger(0)) &&
                            !(flameTurret2 && flameTurret2->GetVehicleKit() && flameTurret2->GetVehicleKit()->GetPassenger(0)) &&
                            !(siegeTurret && siegeTurret->GetVehicleKit() && siegeTurret->GetVehicleKit()->GetPassenger(0)))
                            DelCreature(enemySiege);
                    }
                }

                uint8 siegeType = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_SIEGE_ENGINE_A : BG_IC_NPC_SIEGE_ENGINE_H);
                if (!GetBGCreature(siegeType) || !GetBGCreature(siegeType)->IsAlive())
                {
                    AddCreature((nodePoint->faction == TEAM_ALLIANCE ? NPC_SIEGE_ENGINE_A : NPC_SIEGE_ENGINE_H),siegeType,nodePoint->faction,
                        BG_IC_WorkshopVehicles[4].GetPositionX(),BG_IC_WorkshopVehicles[4].GetPositionY(),
                        BG_IC_WorkshopVehicles[4].GetPositionZ(),BG_IC_WorkshopVehicles[4].GetOrientation(),
                        DAY);

                    if (Creature* siegeEngine = GetBGCreature(siegeType))
                    {
                        siegeEngine->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_CANNOT_SWIM|UNIT_FLAG_IMMUNE_TO_PC);
                        siegeEngine->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                        if (siegeEngine->GetVehicleKit())
                        {
                            if (Unit* flameTurret = siegeEngine->GetVehicleKit()->GetPassenger(1))
                                flameTurret->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                            if (Unit* flameTurret = siegeEngine->GetVehicleKit()->GetPassenger(2))
                                flameTurret->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                            if (Unit* siegeTurret = siegeEngine->GetVehicleKit()->GetPassenger(7))
                                siegeTurret->SetFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                        }
                    }
                }
            }

            for (uint8 i = 0; i < MAX_WORKSHOP_BOMBS_SPAWNS_PER_FACTION; i++)
            {
                AddObject(BG_IC_GO_SEAFORIUM_BOMBS_1+i,GO_SEAFORIUM_BOMBS,
                    workshopBombs[i].GetPositionX(),workshopBombs[i].GetPositionY(),
                    workshopBombs[i].GetPositionZ(),workshopBombs[i].GetOrientation(),
                    0,0,0,0,10);

                if (GameObject* seaforiumBombs = GetBGObject(BG_IC_GO_SEAFORIUM_BOMBS_1+i))
                {
                    seaforiumBombs->SetRespawnTime(10);
                    seaforiumBombs->SetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE, BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                }
            }
            break;
        }
        default:
            break;
    }
}

void BattlegroundIC::DestroyGate(Player* player, GameObject* go)
{
    GateStatus[GetGateIDFromEntry(go->GetEntry())] = BG_IC_GATE_DESTROYED;
    uint32 uws_open = GetWorldStateFromGateEntry(go->GetEntry(), true);
    uint32 uws_close = GetWorldStateFromGateEntry(go->GetEntry(), false);
    if (uws_open)
    {
        UpdateWorldState(uws_close,0);
        UpdateWorldState(uws_open, 1);
    }

    uint32 bossTeam = 0;
    uint32 lang_entry = 0;
    uint32 portcullis_entry = 0;
    uint32 portcullis_entry_2 = 0;

    switch (go->GetEntry())
    {
        case GO_HORDE_GATE_1:
            bossTeam = TEAM_ALLIANCE;
            lang_entry = LANG_BG_IC_NORTH_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_HORDE_KEEP_PORTCULLIS;
            break;
        case GO_HORDE_GATE_2:
            bossTeam = TEAM_ALLIANCE;
            lang_entry = LANG_BG_IC_WEST_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_HORDE_KEEP_PORTCULLIS;
            break;
        case GO_ALLIANCE_GATE_1:
            bossTeam = TEAM_HORDE;
            lang_entry = LANG_BG_IC_WEST_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_DOODAD_PORTCULLISACTIVE01;
            portcullis_entry_2 = BG_IC_GO_DOODAD_PORTCULLISACTIVE02;
            break;
        case GO_HORDE_GATE_3:
            bossTeam = TEAM_ALLIANCE;
            lang_entry = LANG_BG_IC_EAST_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_HORDE_KEEP_PORTCULLIS;
            break;
        case GO_ALLIANCE_GATE_2:
            bossTeam = TEAM_HORDE;
            lang_entry = LANG_BG_IC_EAST_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_DOODAD_PORTCULLISACTIVE01;
            portcullis_entry_2 = BG_IC_GO_DOODAD_PORTCULLISACTIVE02;
            break;
        case GO_ALLIANCE_GATE_3:
            bossTeam = TEAM_HORDE;
            lang_entry = LANG_BG_IC_SOUTH_GATE_DESTROYED;
            portcullis_entry = BG_IC_GO_DOODAD_PORTCULLISACTIVE01;
            portcullis_entry_2 = BG_IC_GO_DOODAD_PORTCULLISACTIVE02;
            break;
        default:
            break;
    }

    SendMessage2ToAll(lang_entry, CHAT_MSG_BG_SYSTEM_NEUTRAL, NULL, (player->GetBGTeamId() == TEAM_ALLIANCE ? LANG_BG_IC_HORDE_KEEP : LANG_BG_IC_ALLIANCE_KEEP));

    ActivateBoss(bossTeam);
    if (portcullis_entry)
        if (GameObject* go = GetBGObject(portcullis_entry))
            if (go->GetGoState() == GO_STATE_READY)
                DoorOpen(portcullis_entry);
    if (portcullis_entry_2)
        if (GameObject* go = GetBGObject(portcullis_entry_2))
            if (go->GetGoState() == GO_STATE_READY)
                DoorOpen(portcullis_entry_2);
}

void BattlegroundIC::ActivateBoss(uint8 faction, bool visible)
{
    if (Creature* boss = GetBGCreature(faction == TEAM_ALLIANCE ? BG_IC_NPC_OVERLORD_AGMAR : BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE))
    {
        if (boss->IsVisible() != visible)
        {
            boss->SetVisible(visible);
            if (visible)
                boss->AI()->Talk(0);
        }
    }

    if (GameObject* banner = GetBGObject(faction == TEAM_ALLIANCE ? BG_IC_GO_HORDE_BANNER : BG_IC_GO_ALLIANCE_BANNER))
        banner->ApplyModFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE, !visible);
}

WorldSafeLocsEntry const* BattlegroundIC::GetClosestGraveYard(Player* player)
{
    TeamId teamIndex = GetTeamIndexByTeamId(player->GetBGTeam());

    // Is there any occupied node for this team?
    std::vector<uint8> nodes;
    for (uint8 i = 0; i < MAX_NODE_TYPES; ++i)
        if (nodePoint[i].faction == player->GetBGTeamId() && !nodePoint[i].needChange)
            nodes.push_back(i);

    WorldSafeLocsEntry const* good_entry = NULL;
    // If so, select the closest node to place ghost on
    if (!nodes.empty())
    {
        float player_x = player->GetPositionX();
        float player_y = player->GetPositionY();

        float mindist = 999999.0f;
        for (uint8 i = 0; i < nodes.size(); ++i)
        {
            WorldSafeLocsEntry const*entry = sWorldSafeLocsStore.LookupEntry(BG_IC_GraveyardIds[nodes[i]]);
            if (!entry)
                continue;
            float dist = (entry->x - player_x)*(entry->x - player_x)+(entry->y - player_y)*(entry->y - player_y);
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
        good_entry = sWorldSafeLocsStore.LookupEntry(BG_IC_GraveyardIds[teamIndex+MAX_NODE_TYPES]);

    return good_entry;
}

bool BattlegroundIC::IsAllNodesControlledByTeam(uint32 team) const
{
    uint32 count = 0;
    ICNodeState controlledState = team == ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H;
    for (int i = 0; i < NODE_TYPE_WORKSHOP; ++i)
    {
        if (nodePoint[i].nodeState == controlledState)
            ++count;
    }

    return count == NODE_TYPE_WORKSHOP;
}

bool BattlegroundIC::IsSpellAllowed(uint32 spellId, Player const* player) const
{
    switch (spellId)
    {
        case SPELL_OIL_REFINERY:
        case SPELL_QUARRY:
        {
            uint32 team = player->GetBGTeamId();
            uint8 nodeType = spellId == SPELL_OIL_REFINERY ? NODE_TYPE_REFINERY : NODE_TYPE_QUARRY;
            uint8 nodeState = team == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H;
            return GetNodeState(nodeType) == nodeState;
        }
        default:
           break;
    }

    return true;
}

bool BattlegroundIC::CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscValue)
{
    switch (criteriaId)
    {
        case BG_CRITERIA_CHECK_RESOURCE_GLUT:
            return IsSpellAllowed(SPELL_OIL_REFINERY, source) && IsSpellAllowed(SPELL_QUARRY, source);
    }

    return Battleground::CheckAchievementCriteriaMeet(criteriaId, source, target, miscValue);
}
