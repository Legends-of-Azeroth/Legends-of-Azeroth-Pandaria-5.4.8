#ifndef _PLAYERBOT_FOLLOWMASTERSTRATEGY_H
#define _PLAYERBOT_FOLLOWMASTERSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class FollowMasterStrategy : public NonCombatStrategy
{
public:
    FollowMasterStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "follow"; }
    NextAction** getDefaultActions() override;
};

#endif
