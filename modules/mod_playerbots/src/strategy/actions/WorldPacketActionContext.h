/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H
#define _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H


#include "NamedObjectContext.h"

#include "AcceptInvitationAction.h"
#include "ReleaseSpiritAction.h"
#include "ReviveFromCorpseAction.h"
#include "LeaveGroupAction.h"

class PlayerbotAI;

class WorldPacketActionContext : public NamedObjectContext<Action>
{
public:
    WorldPacketActionContext()
    {
        // party handlers
        creators["accept invitation"] = &WorldPacketActionContext::accept_invitation;
        creators["uninvite"] = &WorldPacketActionContext::uninvite;

        // death
        creators["revive from corpse"] = &WorldPacketActionContext::revive_from_corpse;
        creators["find corpse"] = &WorldPacketActionContext::find_corpse;
        creators["auto release"] = &WorldPacketActionContext::auto_release;
    }

private:
    static Action* accept_invitation(PlayerbotAI* botAI) { return new AcceptInvitationAction(botAI); }
    static Action* uninvite(PlayerbotAI* botAI) { return new UninviteAction(botAI); }

    static Action* find_corpse(PlayerbotAI* botAI) { return new FindCorpseAction(botAI); }
    static Action* auto_release(PlayerbotAI* botAI) { return new AutoReleaseSpiritAction(botAI); }
    static Action* revive_from_corpse(PlayerbotAI* botAI) { return new ReviveFromCorpseAction(botAI); }
};

#endif
