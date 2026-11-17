#ifndef _PLAYERBOT_AFFLIWARLOCKSTRATEGY_H
#define _PLAYERBOT_AFFLIWARLOCKSTRATEGY_H

#include "GenericWarlockStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

class AffliWarlockStrategy : public GenericWarlockStrategy
{
public:
    AffliWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "affli"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return GenericWarlockStrategy::GetType() | STRATEGY_TYPE_DPS; }
};

#endif
