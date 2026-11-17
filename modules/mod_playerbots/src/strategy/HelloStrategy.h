#ifndef _HELLO_STRATEGY_H_
#define _HELLO_STRATEGY_H_

#include "Strategy.h"
#include "Trigger.h"
#include "Action.h"

class PlayerbotAI;
class HelloStrategy : public Strategy
{
public:
    HelloStrategy(PlayerbotAI* ai) : Strategy(ai) {}
    std::string const getName() override { return "say hello"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override
    {
        //triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("say hello", 1.10f), nullptr)));
        triggers.push_back(new TriggerNode("random", NextAction::array(0, new NextAction("move random", 3.0f), nullptr)));
        triggers.push_back(new TriggerNode("no target", NextAction::array(0, new NextAction("attack anything", 10.0f), nullptr)));
    }
};

#endif