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

#include "ReleaseSpiritAction.h"

#include "DatabaseEnvFwd.h"
#include "Battleground.h"
#include "Event.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "Corpse.h"
#include "WorldPosition.h"

bool ReleaseSpiritAction::Execute(Event event)
{
    if (bot->IsAlive())
    {
        botAI->TellMasterNoFacing("I am not dead, will wait here");
        botAI->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->GetCorpse() && bot->HasPlayerFlag(PLAYER_FLAGS_GHOST))
    {
        botAI->TellMasterNoFacing("I am already a spirit");
        return false;
    }

    WorldPacket& p = event.getPacket();
    if (!p.empty() && p.GetOpcode() == CMSG_REPOP_REQUEST)
        botAI->TellMasterNoFacing("Releasing...");
    else
        botAI->TellMasterNoFacing("Meet me at the graveyard");

    // Death Count to prevent skeleton piles
    Player* master = GetMaster();
    if (!master || (master && GET_PLAYERBOT_AI(master)))
    {
        uint32 dCount = AI_VALUE(uint32, "death count");
        context->GetValue<uint32>("death count")->Set(dCount + 1);
    }

    TC_LOG_INFO("playerbots", "Bot {} {}:{} <{}> released", bot->GetGUID().ToString().c_str(),
             bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

    WorldPacket packet(CMSG_REPOP_REQUEST);
    packet << uint8(0);
    WorldPackets::Combat::RepopRequest repopPkt(std::move(packet));
    repopPkt.Read();
    bot->GetSession()->HandleRepopRequestOpcode(repopPkt);

    // // add waiting for ress aura
    // if (bot->InBattleground() && !botAI->HasAura(SPELL_WAITING_FOR_RESURRECT, bot) && !bot->IsAlive())
    // {
    //     // cast Waiting for Resurrect
    //     GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    //     ObjectGuid guid;
    //     Unit* unit;
    //     for (GuidVector::iterator i = npcs.begin(); i != npcs.end(); i++)
    //     {
    //         unit = botAI->GetUnit(*i);
    //         if (unit && unit->IsSpiritService())
    //         {
    //             guid = unit->GetGUID();
    //             break;
    //         }
    //     }
    //     if (!guid) {
    //         return true;
    //     }
    //     if (bot->GetDistance(unit) >= INTERACTION_DISTANCE) {
    //         bot->GetMotionMaster()->MoveChase(unit);
    //     } else {
    //         WorldPacket packet(CMSG_GOSSIP_HELLO);
    //         packet << guid;
    //         bot->GetSession()->HandleGossipHelloOpcode(packet);
    //     }
    // }

    return true;
}

bool AutoReleaseSpiritAction::Execute(Event event)
{
    // Death Count to prevent skeleton piles
    Player* master = GetMaster();
    if (!master || (master && GET_PLAYERBOT_AI(master)))
    {
        uint32 dCount = AI_VALUE(uint32, "death count");
        context->GetValue<uint32>("death count")->Set(dCount + 1);
    }

    TC_LOG_DEBUG("playerbots", "Bot {} {}:{} <{}> auto released", bot->GetGUID().ToString().c_str(),
              bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

    WorldPacket packet(CMSG_REPOP_REQUEST);
    packet << uint8(0);
    WorldPackets::Combat::RepopRequest repopPkt(std::move(packet));
    repopPkt.Read();
    bot->GetSession()->HandleRepopRequestOpcode(repopPkt);

    TC_LOG_DEBUG("playerbots", "Bot {} {}:{} <{}> releases spirit", bot->GetGUID().ToString().c_str(),
              bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

    if (bot->InBattleground() && (time(NULL) - bg_gossip_time >= 15 || !bot->HasAura(SPELL_WAITING_FOR_RESURRECT)))
    {
        GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
        ObjectGuid guid;
        Unit* unit;
        for (GuidVector::iterator i = npcs.begin(); i != npcs.end(); i++)
        {
            unit = botAI->GetUnit(*i);
            if (unit && unit->IsSpiritService())
            {
                guid = unit->GetGUID();
                break;
            }
        }
        if (!guid)
        {
            return true;
        }
        if (bot->GetDistance(unit) >= INTERACTION_DISTANCE)
        {
            // bot needs to actually click spirit-healer in BG to get res timer going
            // and in IOC it's not within clicking range when they res in own base
            MotionMaster& mm = *bot->GetMotionMaster();
            mm.Clear();
            mm.MovePoint(bot->GetMapId(), unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), true);
        }
        else if (!botAI->IsRealPlayer()) // below doesnt work properly on realplayer, but its also not needed
        {
            bg_gossip_time = time(NULL);
            WorldPacket data(CMSG_GOSSIP_HELLO);
            data << guid;
            WorldPackets::NPC::GossipHello gossipHello(std::move(data));
            gossipHello.Read();
            bot->GetSession()->HandleGossipHelloOpcode(gossipHello);
        }
    }
    botAI->SetNextCheckDelay(1000);
    return true;
}

bool AutoReleaseSpiritAction::isUseful()
{
    if (!bot->isDead())
        return false;

    if (bot->InArena())
        return false;

    if (bot->InBattleground())
        return true;

    if (bot->HasPlayerFlag(PLAYER_FLAGS_GHOST))
        return false;

    if (!bot->GetGroup())
        return true;

    if (!botAI->GetGroupMaster())
        return true;

    if (botAI->GetGroupMaster() == bot)
        return true;

    if (!botAI->HasActivePlayerMaster())
        return true;

    if (botAI->HasActivePlayerMaster() && botAI->GetGroupMaster()->GetMapId() == bot->GetMapId() && bot->GetMap() &&
        (bot->GetMap()->IsRaid() || bot->GetMap()->IsDungeon()))
        return false;

    if (botAI->GetGroupMaster()->isDead())
        return true;

    if (sServerFacade->IsDistanceGreaterThan(AI_VALUE2(float, "distance", "master target"),
                                             sPlayerbotAIConfig->sightDistance))
        return true;

    return false;
}

bool RepopAction::Execute(Event event)
{
    TC_LOG_DEBUG("playerbots", "Bot {} {}:{} <{}> repops at graveyard", bot->GetGUID().ToString().c_str(),
              bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

    int64 deadTime;

    Corpse* corpse = bot->GetCorpse();
    if (corpse)
        deadTime = time(nullptr) - corpse->GetGhostTime();
    else if (bot->isDead())
        deadTime = 0;
    else
        deadTime = 60 * MINUTE;

    uint32 dCount = AI_VALUE(uint32, "death count");

    WorldSafeLocsEntry const* ClosestGrave = GetGrave(dCount > 10 || deadTime > 30 * MINUTE);
    if (!ClosestGrave)
        return false;

    auto ClosestGraveyard = sObjectMgr->GetClosestGraveYard(bot->GetWorldLocation(), bot->GetTeam(), bot);
    if (!ClosestGraveyard)
        return false;

    bot->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, 0.f);

    RESET_AI_VALUE(bool, "combat::self target");
    RESET_AI_VALUE(WorldPosition, "current position");

    return true;
}

bool RepopAction::isUseful()
{
    if (bot->InBattleground())
        return false;

    return true;
}
