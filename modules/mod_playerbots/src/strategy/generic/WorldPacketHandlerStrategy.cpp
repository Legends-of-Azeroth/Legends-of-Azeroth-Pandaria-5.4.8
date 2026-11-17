/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "WorldPacketHandlerStrategy.h"

#include "Playerbots.h"

void WorldPacketHandlerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("levelup", NextAction::array(0, new NextAction("auto maintenance on levelup", relevance + 3), nullptr)));

    // party handlers
    triggers.push_back(new TriggerNode("group invite", NextAction::array(0, new NextAction("accept invitation", relevance), nullptr)));
    triggers.push_back(new TriggerNode("uninvite guid", NextAction::array(0, new NextAction("uninvite", relevance), nullptr)));
    triggers.push_back(new TriggerNode("uninvite", NextAction::array(0, new NextAction("uninvite", relevance), nullptr)));

    triggers.push_back(new TriggerNode("release spirit", NextAction::array(0, new NextAction("release", relevance), nullptr)));
    triggers.push_back(new TriggerNode("revive from corpse", NextAction::array(0, new NextAction("revive from corpse", relevance), nullptr)));
}

WorldPacketHandlerStrategy::WorldPacketHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI)
{
    supported.push_back("uninvite");
}
