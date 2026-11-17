#include "TankAssistStrategy.h"

#include "Playerbots.h"

void TankAssistStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("tank assist", NextAction::array(0, new NextAction("tank assist", 50.0f), nullptr)));
}
