#ifndef _PLAYERBOT_RUNAWAYSTRATEGY_H
#define _PLAYERBOT_RUNAWAYSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class RunawayStrategy : public NonCombatStrategy
{
public:
    RunawayStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "runaway"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
