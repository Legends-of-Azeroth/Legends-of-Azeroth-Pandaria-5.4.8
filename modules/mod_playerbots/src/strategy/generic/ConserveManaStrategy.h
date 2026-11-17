#ifndef _PLAYERBOT_CONSERVEMANASTRATEGY_H
#define _PLAYERBOT_CONSERVEMANASTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class HealerAutoSaveManaMultiplier : public Multiplier
{
public:
    HealerAutoSaveManaMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "save mana") {}

    float GetValue(Action* action) override;
};

class HealerAutoSaveManaStrategy : public Strategy
{
public:
    HealerAutoSaveManaStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    std::string const getName() override { return "save mana"; }
};

#endif
