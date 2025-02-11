/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "MageTriggers.h"

#include "MageActions.h"
#include "Playerbots.h"

bool ArcaneBrillianceTrigger::IsActive()
{
    return BuffTrigger::IsActive() && !botAI->HasAura("arcane brilliance", GetTarget());
}

bool MageArmorTrigger::IsActive()
{
    Unit* target = GetTarget();
    return !botAI->HasAura("frost armor", target) && !botAI->HasAura("molten armor", target) && !botAI->HasAura("mage armor", target);
}

bool FingersOfFrostSingleTrigger::IsActive()
{
    // Fingers of Frost "stack" count is always 1.
    // The value is instead stored in the charges.
    Aura* aura = botAI->GetAura("fingers of frost", bot, false, true, -1);
    return (aura && aura->GetCharges() == 1);
}