#ifndef _PLAYERBOT_GUARDSTRATEGY_H
#define _PLAYERBOT_GUARDSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GuardStrategy : public NonCombatStrategy
{
public:
    GuardStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "guard"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
