#ifndef _PLAYERBOT_STAYACTIONS_H
#define _PLAYERBOT_STAYACTIONS_H

#include "MovementActions.h"

class PlayerbotAI;

class StayActionBase : public MovementAction
{
public:
    StayActionBase(PlayerbotAI* botAI, std::string const name) : MovementAction(botAI, name) {}

protected:
    bool Stay();
};

class StayAction : public StayActionBase
{
public:
    StayAction(PlayerbotAI* botAI) : StayActionBase(botAI, "stay") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class SitAction : public StayActionBase
{
public:
    SitAction(PlayerbotAI* botAI) : StayActionBase(botAI, "sit") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
