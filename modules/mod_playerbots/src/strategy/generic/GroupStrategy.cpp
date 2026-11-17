#include "GroupStrategy.h"

#include "Playerbots.h"

void GroupStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("invite nearby", 4.0f), nullptr)));
    triggers.push_back(new TriggerNode("random", NextAction::array(0, new NextAction("invite guild", 4.0f), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("leave far away", 4.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("seldom", NextAction::array(0, new NextAction("reset instances", 1.0f), nullptr)));
}
