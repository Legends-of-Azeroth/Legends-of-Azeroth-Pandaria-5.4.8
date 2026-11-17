#ifndef _PLAYERBOT_ACCEPTINVITATIONACTION_H
#define _PLAYERBOT_ACCEPTINVITATIONACTION_H

#include "Action.h"

class PlayerbotAI;

class AcceptInvitationAction : public Action// : public SummonAction
{
public:
    AcceptInvitationAction(PlayerbotAI* botAI, const std::string name = "accept invitation");

    bool Execute(Event event) override;
};

#endif
