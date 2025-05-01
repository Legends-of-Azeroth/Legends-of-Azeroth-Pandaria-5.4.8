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

#include "ReviveFromCorpseAction.h"

#include "Event.h"
#include "MapManager.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "ServerFacade.h"
#include "Corpse.h"
#include "WorldPosition.h"

bool ReviveFromCorpseAction::Execute(Event event)
{
    Player* master = botAI->GetGroupMaster();
    Corpse* corpse = bot->GetCorpse();

    // follow master when master revives
    WorldPacket& p = event.getPacket();
    if (!p.empty() && p.GetOpcode() == CMSG_RECLAIM_CORPSE && master && !corpse && bot->IsAlive())
    {
        if (sServerFacade->IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"),
            sPlayerbotAIConfig->farDistance))
        {
            if (!botAI->HasStrategy("follow", BOT_STATE_NON_COMBAT))
            {
                botAI->TellMasterNoFacing("Welcome back!");
                botAI->ChangeStrategy("+follow,-stay", BOT_STATE_NON_COMBAT);
                return true;
            }
        }
    }

    if (!corpse)
        return false;

    if (master)
    {
        if (!GET_PLAYERBOT_AI(master) && master->isDead() && master->GetCorpse() &&
            sServerFacade->IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"),
                sPlayerbotAIConfig->farDistance))
            return false;
    }

    if (!botAI->HasRealPlayerMaster())
    {
        uint32 dCount = AI_VALUE(uint32, "death count");

        if (dCount >= 5)
        {
            return botAI->DoSpecificAction("spirit healer");
        }
    }

    TC_LOG_DEBUG("playerbots", "Bot %s %s:%u <%s> revives at body", bot->GetGUID().ToString().c_str(),
        bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

    bot->GetMotionMaster()->Clear();
    bot->StopMoving();

    WorldPacket packet(CMSG_RECLAIM_CORPSE);

    ObjectGuid guid = bot->GetGUID();
    packet.WriteBit(guid[1]);
    packet.WriteBit(guid[5]);
    packet.WriteBit(guid[7]);
    packet.WriteBit(guid[2]);
    packet.WriteBit(guid[6]);
    packet.WriteBit(guid[3]);
    packet.WriteBit(guid[0]);
    packet.WriteBit(guid[4]);

    packet.WriteByteSeq(guid[2]);
    packet.WriteByteSeq(guid[5]);
    packet.WriteByteSeq(guid[4]);
    packet.WriteByteSeq(guid[6]);
    packet.WriteByteSeq(guid[1]);
    packet.WriteByteSeq(guid[0]);
    packet.WriteByteSeq(guid[7]);
    packet.WriteByteSeq(guid[3]);

    bot->GetSession()->HandleReclaimCorpseOpcode(packet);

    return true;
}

bool FindCorpseAction::Execute(Event event)
{
    if (bot->InBattleground())
        return false;

    Player* master = botAI->GetGroupMaster();
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;


    uint32 dCount = AI_VALUE(uint32, "death count");

    if (!botAI->HasRealPlayerMaster())
    {
        if (dCount >= 5)
        {
            context->GetValue<uint32>("death count")->Set(0);
            sRandomPlayerbotMgr->Revive(bot);
            return true;
        }
    }

    WorldPosition botPos(bot);
    WorldPosition corpsePos(corpse);
    WorldPosition moveToPos = corpsePos;
    WorldPosition masterPos(master);

    float reclaimDist = CORPSE_RECLAIM_RADIUS - 5.0f;
    float corpseDist = botPos.distance(corpsePos);
    int64 deadTime = time(nullptr) - corpse->GetGhostTime();

    bool moveToMaster = master && master != bot && masterPos.fDist(corpsePos) < reclaimDist;

    // Should we ressurect? If so, return false.
    if (corpseDist < reclaimDist)
    {
        if (moveToMaster)  // We are near master.
        {
            if (botPos.fDist(masterPos) < sPlayerbotAIConfig->spellDistance)
                return false;
        }
        else if (deadTime > 8 * MINUTE)  // We have walked too long already.
            return false;
        else
        {
            GuidVector units = AI_VALUE(GuidVector, "possible targets no los");
            if (botPos.getUnitsAggro(units, bot) == 0)  // There are no mobs near.
                return false;
        }
    }

    // If we are getting close move to a save ressurrection spot instead of just the corpse.
    if (corpseDist < sPlayerbotAIConfig->reactDistance)
    {
        if (moveToMaster)
            moveToPos = masterPos;
        else
        {
            /*FleeManager manager(bot, reclaimDist, 0.0, urand(0, 1), moveToPos);

            if (manager.isUseful())
            {
                float rx, ry, rz;
                if (manager.CalculateDestination(&rx, &ry, &rz))
                    moveToPos = WorldPosition(moveToPos.getMapId(), rx, ry, rz, 0.0);
                else if (!moveToPos.GetReachableRandomPointOnGround(bot, reclaimDist, urand(0, 1)))
                    moveToPos = corpsePos;
            }*/
        }
    }

    // Actual mobing part.
    bool moved = false;

    if (!botAI->AllowActivity(ALL_ACTIVITY))
    {
        uint32 delay = sServerFacade->GetDistance2d(bot, corpse) /
            bot->GetSpeed(MOVE_RUN);        // Time a bot would take to travel to it's corpse.
        delay = std::min(delay, uint32(10 * MINUTE));  // Cap time to get to corpse at 10 minutes.

        if (deadTime > delay)
        {
            bot->GetMotionMaster()->Clear();
            bot->TeleportTo(moveToPos.getMapId(), moveToPos.getX(), moveToPos.getY(), moveToPos.getZ(), 0);
        }

        moved = true;
    }
    else
    {
        if (bot->isMoving())
            moved = true;
        else
        {
            if (deadTime < 10 * MINUTE && dCount < 5)  // Look for corpse up to 30 minutes.
            {
                moved = MoveTo(moveToPos.getMapId(), moveToPos.getX(), moveToPos.getY(), moveToPos.getZ(), false, false);
            }

            if (!moved)
            {
                moved = botAI->DoSpecificAction("spirit healer", Event(), true);
            }
        }
    }

    return moved;
}

bool FindCorpseAction::isUseful()
{
    if (bot->InBattleground())
        return false;

    return bot->GetCorpse();
}

WorldSafeLocsEntry const* SpiritHealerAction::GetGrave(bool startZone)
{
    WorldSafeLocsEntry const* ClosestGrave = nullptr;
    WorldSafeLocsEntry const* NewGrave = nullptr;

    ClosestGrave = sObjectMgr->GetClosestGraveYard(bot->GetWorldLocation(), bot->GetTeam(), bot);

    if (!startZone && ClosestGrave)
        return ClosestGrave;

    if (botAI->HasStrategy("follow", BOT_STATE_NON_COMBAT) && botAI->GetGroupMaster() && botAI->GetGroupMaster() != bot)
    {
        Player* master = botAI->GetGroupMaster();
        if (master && master != bot)
        {
            ClosestGrave = sObjectMgr->GetClosestGraveYard(master->GetWorldLocation(), bot->GetTeamId(), bot);

            if (ClosestGrave)
                return ClosestGrave;
        }
    }
    /*else if (startZone && AI_VALUE(uint8, "durability"))
    {
        TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");

        if (travelTarget->getPosition())
        {
            WorldPosition travelPos = *travelTarget->getPosition();
            if (travelPos.GetMapId() != uint32(-1))
            {
                uint32 areaId = 0;
                uint32 zoneId = 0;
                sMapMgr->GetZoneAndAreaId(bot->GetPhaseMask(), zoneId, areaId, travelPos.getMapId(), travelPos.getX(),
                                          travelPos.getY(), travelPos.getZ());
                ClosestGrave = sGraveyard->GetClosestGraveyard(travelPos.getMapId(), travelPos.getX(), travelPos.getY(),
                                                               travelPos.getZ(), bot->GetTeamId(), areaId, zoneId,
                                                               bot->getClass() == CLASS_DEATH_KNIGHT);

                if (ClosestGrave)
                    return ClosestGrave;
            }
        }
    }*/

    TC_LOG_ERROR("playerbots", "Graveyard nullptr");
    return ClosestGrave;
}

bool SpiritHealerAction::Execute(Event event)
{
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
    {
        //botAI->TellError("I am not a spirit");
        return false;
    }

    uint32 dCount = AI_VALUE(uint32, "death count");
    int64 deadTime = time(nullptr) - corpse->GetGhostTime();

    WorldSafeLocsEntry const* ClosestGrave = GetGrave(dCount > 10 || deadTime > 15 * MINUTE || AI_VALUE(uint8, "durability") < 10);

    if (bot->GetDistance2d(ClosestGrave->x, ClosestGrave->y) < sPlayerbotAIConfig->sightDistance)
    {
        GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
        for (GuidVector::iterator i = npcs.begin(); i != npcs.end(); i++)
        {
            Unit* unit = botAI->GetUnit(*i);
            if (unit && unit->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER))
            {
                TC_LOG_DEBUG("playerbots", "Bot %s %s:%u <%s> revives at spirit healer", bot->GetGUID().ToString().c_str(), bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());
                //PlayerbotChatHandler ch(bot);
                bot->ResurrectPlayer(0.5f);
                bot->SpawnCorpseBones();
                context->GetValue<Unit*>("current target")->Set(nullptr);
                bot->SetTarget(ObjectGuid::Empty);
                botAI->TellMaster("Hello");

                if (dCount > 20)
                    context->GetValue<uint32>("death count")->Set(0);

                return true;
            }
        }
    }

    if (!ClosestGrave)
    {
        return false;
    }

    bool moved = false;

    if (bot->IsWithinLOS(ClosestGrave->x, ClosestGrave->y, ClosestGrave->z))
        moved = MoveNear(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, 0.0);
    else
        moved = MoveTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, false, false);

    if (moved)
        return true;

    context->GetValue<uint32>("death count")->Set(dCount + 1);
    return bot->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, 0.f);
}

bool SpiritHealerAction::isUseful()
{
    return bot->HasPlayerFlag(PLAYER_FLAGS_GHOST);
}
