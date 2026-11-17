/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "CombatStrategy.h"

#include "Playerbots.h"
#include "Strategy.h"

void CombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("enemy out of spell", NextAction::array(0, new NextAction("reach spell", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("invalid target", NextAction::array(0, new NextAction("drop target", 100), nullptr)));
    triggers.push_back(new TriggerNode("mounted", NextAction::array(0, new NextAction("check mount state", 54), nullptr)));
    triggers.push_back(new TriggerNode("combat stuck", NextAction::array(0, new NextAction("reset", 1.0f), nullptr)));
    triggers.push_back(new TriggerNode("not facing target", NextAction::array(0, new NextAction("set facing", ACTION_MOVE + 7), nullptr)));
    triggers.push_back(new TriggerNode("pet attack", NextAction::array(0, new NextAction("pet attack", 40.0f), nullptr)));
}

AvoidAoeStrategy::AvoidAoeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

NextAction** AvoidAoeStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("avoid aoe", ACTION_EMERGENCY), nullptr);
}

void AvoidAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void AvoidAoeStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
}

TankFaceStrategy::TankFaceStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

NextAction** TankFaceStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("tank face", ACTION_MOVE), nullptr);
}

void TankFaceStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

NextAction** CombatFormationStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("combat formation move", ACTION_NORMAL), nullptr);
}
