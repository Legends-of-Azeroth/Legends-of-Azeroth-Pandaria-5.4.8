/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "WarlockTriggers.h"

#include "GenericTriggers.h"
#include "Playerbots.h"

bool MetamorphosisTrigger::IsActive()
{
    if (botAI->HasAura("metamorphosis", bot) && bot->GetPower(Powers::POWER_DEMONIC_FURY) <= 50)
    {
        botAI->RemoveAura("metamorphosis");
    }
    return BuffTrigger::IsActive() && bot->GetPower(Powers::POWER_DEMONIC_FURY) >= 800;
}

bool CorruptionTrigger::IsActive()
{
    if (botAI->HasAura("metamorphosis", bot))
    {
        spell = "doom";
        return DebuffTrigger::IsActive();
    }

    spell = "corruption";
    return DebuffTrigger::IsActive() && !botAI->HasAura("doom", GetTarget(), false, true);
}

bool FireAndBrimeStoneTrigger::IsActive()
{
    if (botAI->HasAura("fire and brimstone", bot) && bot->GetPower(Powers::POWER_BURNING_EMBERS) <= 0)
    {
        botAI->RemoveAura("fire and brimstone");
    }
    return BuffTrigger::IsActive() && bot->GetPower(Powers::POWER_BURNING_EMBERS) >= 40;
}

bool WarlockPetLowHealthTrigger::IsActive()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    return pet && AI_VALUE2(uint8, "health", "pet target") < 40 && !AI_VALUE2(bool, "dead", "pet target") /*&& !AI_VALUE2(bool, "mounted", "self target")*/;
}
bool WarlockPetMediumHealthTrigger::IsActive()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    return pet && AI_VALUE2(uint8, "health", "pet target") < sPlayerbotAIConfig->mediumHealth /* && !AI_VALUE2(bool, "dead", "pet target") && !AI_VALUE2(bool, "mounted", "self target")*/;
}