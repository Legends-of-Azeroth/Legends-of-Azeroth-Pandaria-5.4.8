/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "PaladinTriggers.h"

#include "PaladinActions.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool SealTrigger::IsActive()
{
	return !botAI->HasAura("seal of righteousness", bot) || !botAI->HasAura("seal of command", bot) || !botAI->HasAura("seal of insight", bot);
}