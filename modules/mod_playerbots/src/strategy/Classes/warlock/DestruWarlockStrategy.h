#ifndef _PLAYERBOT_DESTRUWARLOCKSTRATEGY_H
#define _PLAYERBOT_DESTRUWARLOCKSTRATEGY_H

#include "GenericWarlockStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

class DestruWarlockStrategy : public GenericWarlockStrategy
{
public:
    DestruWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "destru"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return GenericWarlockStrategy::GetType() | STRATEGY_TYPE_DPS; }
};

#endif
