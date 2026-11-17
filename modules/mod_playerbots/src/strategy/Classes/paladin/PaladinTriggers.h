/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_PALADINTRIGGERS_H
#define _PLAYERBOT_PALADINTRIGGERS_H

#include "GenericTriggers.h"
#include "SharedDefines.h"
#include "Unit.h"

class PlayerbotAI;

class SealTrigger : public HasAuraTrigger
{
public:
	SealTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "seal", 5.0f) {}
	bool IsActive() override;
};

#endif /*_PLAYERBOT_PALADINTRIGGERS_H*/