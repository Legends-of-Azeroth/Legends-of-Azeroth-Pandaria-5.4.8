/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "PlayerbotAIBase.h"
#include "PlayerbotAIConfig.h"
#include "PerformanceMonitor.h"

PlayerbotAIBase::PlayerbotAIBase(bool isBotAI)
    : nextAICheckDelay(0)
    , _isBotAI(isBotAI)
{}

void PlayerbotAIBase::UpdateAI(uint32 elapsed, bool minimal)
{
    if (totalPmo)
        totalPmo->finish();

    totalPmo = sPerformanceMonitor->start(PERF_MON_TOTAL, "PlayerbotAIBase::FullTick");

    if (nextAICheckDelay > elapsed)
        nextAICheckDelay -= elapsed;
    else
        nextAICheckDelay = 0;

    if (!CanUpdateAI())
        return;

    UpdateAIInternal(elapsed, minimal);
    YieldThread();
}

void PlayerbotAIBase::SetNextCheckDelay(uint32 const delay)
{
    nextAICheckDelay = delay;
}

void PlayerbotAIBase::IncreaseNextCheckDelay(uint32 delay)
{
    nextAICheckDelay += delay;
}

bool PlayerbotAIBase::CanUpdateAI() { return nextAICheckDelay == 0; }

void PlayerbotAIBase::YieldThread(uint32 delay)
{
    if (nextAICheckDelay < delay)
        nextAICheckDelay = delay;
}

bool PlayerbotAIBase::IsActive() { return nextAICheckDelay < sPlayerbotAIConfig->maxWaitForMove; }

bool PlayerbotAIBase::IsBotAI() const { return _isBotAI; }
