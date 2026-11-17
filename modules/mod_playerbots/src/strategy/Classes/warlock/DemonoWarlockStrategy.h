#ifndef _PLAYERBOT_DEMONOWARLOCKSTRATEGY_H
#define _PLAYERBOT_DEMONOWARLOCKSTRATEGY_H

#include "GenericWarlockStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

class DemonoWarlockStrategy : public GenericWarlockStrategy
{
public:
    DemonoWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "demono"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return GenericWarlockStrategy::GetType() | STRATEGY_TYPE_DPS; }
};

#endif
