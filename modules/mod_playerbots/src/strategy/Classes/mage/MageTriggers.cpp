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

bool RuneOfPowerTrigger::IsActive()
{
    return BuffTrigger::IsActive() && !botAI->HasAura("rune of power", GetTarget());
}

bool MageArmorTrigger::IsActive()
{
    return 
        (botAI->GetBot()->GetSpecialization() != Specializations::SPEC_MAGE_FIRE && bot->GetGroup() ||
        botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_ARCANE) &&
        !botAI->HasAura("mage armor", bot);
}

bool FrostArmorTrigger::IsActive()
{
    return (botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_FROST && !botAI->HasAura("frost armor", bot) && !bot->GetGroup());
}

bool MoltenArmorTrigger::IsActive()
{
    return (botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_FIRE && !botAI->HasAura("molten armor", bot));
}

bool FrostNovaOnTargetTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return botAI->HasAura(spell, target);
}

bool IcyVeinsTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return !botAI->HasAura("icy veins", target);
}

bool FingersOfFrostSingleTrigger::IsActive()
{
    // Fingers of Frost "stack" count is always 1.
    // The value is instead stored in the charges.
    Aura* aura = botAI->GetAura("fingers of frost", bot, false, true, -1);
    return (aura && aura->GetCharges() == 1);
}

bool SlowNotOnTargetTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return !botAI->HasAura(spell, target);
}

bool HeatingUpTrigger::IsActive()
{
    Aura* aura = botAI->GetAura("heating up", bot, false, false, -1);
    return aura != nullptr;
}