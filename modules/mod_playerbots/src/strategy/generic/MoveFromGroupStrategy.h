#ifndef _PLAYERBOT_MOVEFROMGROUPSTRATEGY_H
#define _PLAYERBOT_MOVEFROMGROUPSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class MoveFromGroupStrategy : public Strategy
{
public:
    MoveFromGroupStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "move from group"; }
    NextAction** getDefaultActions() override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

#endif
