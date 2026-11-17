#ifndef _PLAYERBOT_CHANGESTRATEGYACTION_H
#define _PLAYERBOT_CHANGESTRATEGYACTION_H

#include "Action.h"

class PlayerbotAI;

class ChangeCombatStrategyAction : public Action
{
public:
    ChangeCombatStrategyAction(PlayerbotAI* botAI, std::string const name = "co") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class ChangeNonCombatStrategyAction : public Action
{
public:
    ChangeNonCombatStrategyAction(PlayerbotAI* botAI) : Action(botAI, "nc") {}

    bool Execute(Event event) override;
};

class ChangeDeadStrategyAction : public Action
{
public:
    ChangeDeadStrategyAction(PlayerbotAI* botAI) : Action(botAI, "de") {}

    bool Execute(Event event) override;
};

#endif
