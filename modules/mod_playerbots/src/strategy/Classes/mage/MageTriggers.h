/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_MAGETRIGGERS_H
#define _PLAYERBOT_MAGETRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "SharedDefines.h"

class ArcaneBrillianceTrigger : public BuffTrigger
{
public:
    ArcaneBrillianceTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "arcane brilliance", 2 * 2000) {}
    bool IsActive() override;
};

class MageArmorTrigger : public BuffTrigger
{
public:
    MageArmorTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "mage armor", 5 * 2000) {}
    bool IsActive() override;
};

class FrostArmorTrigger : public BuffTrigger
{
public:
    FrostArmorTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "frost armor", 5 * 2000) {}
    bool IsActive() override;
};

class MoltenArmorTrigger : public BuffTrigger
{
public:
    MoltenArmorTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "molten armor", 5 * 2000) {}
    bool IsActive() override;
};

class CounterspellInterruptSpellTrigger : public InterruptSpellTrigger
{
public:
    CounterspellInterruptSpellTrigger(PlayerbotAI* botAI) : InterruptSpellTrigger(botAI, "counterspell") {}
};

class SpellstealTrigger : public TargetAuraDispelTrigger
{
public:
    SpellstealTrigger(PlayerbotAI* botAI) : TargetAuraDispelTrigger(botAI, "spellsteal", DISPEL_MAGIC) {}
};
class LivingBombTrigger : public DebuffTrigger
{
public:
    LivingBombTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "living bomb", 1, true) {}
};

class MissileBarrageTrigger : public HasAuraTrigger
{
public:
    MissileBarrageTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "missile barrage") {}
};

class FingersOfFrostSingleTrigger : public HasAuraStackTrigger
{
public:
    FingersOfFrostSingleTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "fingers of frost", 1, 1) {}
    bool IsActive() override;
};

class FingersOfFrostDoubleTrigger : public HasAuraStackTrigger
{
public:
    FingersOfFrostDoubleTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "fingers of frost", 2, 1) {}
};

class ArcanePowerTrigger : public BuffTrigger
{
public:
    ArcanePowerTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "arcane power") {}
};

class PresenceOfMindTrigger : public BuffTrigger
{
public:
    PresenceOfMindTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "presence of mind") {}
};

// -- forst triggers
class FrostNovaOnTargetTrigger : public DebuffTrigger
{
public:
    FrostNovaOnTargetTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "frost nova", 1, false) {}
    bool IsActive() override;
};

class IcyVeinsTrigger : public BuffTrigger
{
public:
    IcyVeinsTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "icy veins") {}
    bool IsActive() override;
};

class FrostBombTrigger : public DebuffTrigger
{
public:
    FrostBombTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "frost bomb", 1, true) {}
};

// -- arcane triggers
class ArcaneBlastTrigger : public BuffTrigger
{
public:
    ArcaneBlastTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "arcane blast") {}
};

class ArcaneMissilesTrigger : public HasAuraStackTrigger
{
public:
    ArcaneMissilesTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "arcane missiles", 1, 1) {}
};

class BrainFreezeTrigger : public HasAuraStackTrigger
{
public:
    BrainFreezeTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "brain freeze", 1, 1) {}
};

class ArcaneChargeTrigger : public HasAuraStackTrigger
{
public:
    ArcaneChargeTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "arcane charge", 1, 1) {}
};

class SlowNotOnTargetTrigger : public DebuffTrigger
{
public:
    SlowNotOnTargetTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "slow", 1, false) {}
    bool IsActive() override;
};

class NetherTempestTrigger : public DebuffTrigger
{
public:
    NetherTempestTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "nether tempest", 1, true) {}
};

class RuneOfPowerTrigger : public BuffTrigger
{
public:
    RuneOfPowerTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "rune of power", 5 * 2000) {}
    bool IsActive() override;
};

class HeatingUpTrigger : public HasAuraTrigger
{
public:
    HeatingUpTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "heating up", 1) {}
    bool IsActive() override;
};

class PyroblastTrigger : public HasAuraTrigger
{
public:
    PyroblastTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "pyroblast!", 1) {}
};

#endif
