#ifndef _PLAYERBOT_FLEESTRATEGY_H
#define _PLAYERBOT_FLEESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class FleeStrategy : public Strategy
{
public:
    FleeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "flee"; };
};

class FleeFromAddsStrategy : public Strategy
{
public:
    FleeFromAddsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "flee from adds"; };
};

#endif
