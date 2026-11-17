#ifndef _PLAYERBOT_RANDOMBOTUPDATEACTION_H
#define _PLAYERBOT_RANDOMBOTUPDATEACTION_H

#include "Action.h"

class PlayerbotAI;

class RandomBotUpdateAction : public Action
{
public:
    RandomBotUpdateAction(PlayerbotAI* botAI) : Action(botAI, "random bot update") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
