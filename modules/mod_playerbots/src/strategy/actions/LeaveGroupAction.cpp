/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "LeaveGroupAction.h"

#include "Event.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool LeaveGroupAction::Leave(Player* player)
{
    if (player && !GET_PLAYERBOT_AI(player))
        return false;

    bool aiMaster = GET_PLAYERBOT_AI(botAI->GetMaster()) != nullptr;

    if (bot->GetGroup())
    {
        //bot->GetGroup()->Disband();
        bot->RemoveFromGroup();
    }

    bool randomBot = sRandomPlayerbotMgr->IsRandomBot(bot);
    if (randomBot)
    {
        GET_PLAYERBOT_AI(bot)->SetMaster(nullptr);
    }

    if (!aiMaster)
        botAI->ResetStrategies();

    botAI->Reset();

    return true;
}

bool LeaveGroupAction::Execute(Event event)
{
    Player* master = event.getOwner();
    return Leave(master);
}

bool UninviteAction::Execute(Event event)
{
    WorldPacket& p = event.getPacket();
    if (p.GetOpcode() == CMSG_GROUP_DISBAND)
    {
        return Leave(bot);
    }

    if (p.GetOpcode() == CMSG_GROUP_UNINVITE_GUID)
    {
        p.rpos(1);
        uint32 guid_low;
        p >> guid_low;

        if (bot->GetGUID().GetCounter() == guid_low)
            return Leave(bot);
    }

    //botAI->Reset();

    return false;
}
