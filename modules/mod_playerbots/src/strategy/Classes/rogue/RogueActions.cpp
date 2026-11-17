/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "RogueActions.h"

#include "Event.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Playerbots.h"

bool CastStealthAction::isPossible()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool UnstealthAction::Execute(Event event)
{
    botAI->RemoveAura("stealth");
    // botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);

    return true;
}

bool CheckStealthAction::Execute(Event event)
{
    if (botAI->HasAura("stealth", bot))
    {
        botAI->ChangeStrategy("-dps,+stealthed", BOT_STATE_COMBAT);
    }
    else
    {
        botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);
    }

    return true;
}

bool CastVanishAction::isUseful()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool CastTricksOfTheTradeOnMainTankAction::isUseful()
{
    return CastSpellAction::isUseful() && AI_VALUE2(float, "distance", GetTargetName()) < 20.0f;
}
