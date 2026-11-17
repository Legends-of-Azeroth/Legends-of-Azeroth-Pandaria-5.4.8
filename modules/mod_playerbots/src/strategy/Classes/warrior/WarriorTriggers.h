/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WARRIORTRIGGERS_H
#define _PLAYERBOT_WARRIORTRIGGERS_H

#include "GenericTriggers.h"
#include "Playerbots.h"
#include "PlayerbotAI.h"

class BattleShoutTrigger : public BuffTrigger
{
public:
	BattleShoutTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "battle shout", 10) {}
};
class CommandingShoutTrigger : public BuffTrigger
{
public:
	CommandingShoutTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "commanding shout", 10) {}
};
BUFF_TRIGGER(BattleStanceTrigger, "battle stance");
BUFF_TRIGGER(DefensiveStanceTrigger, "defensive stance");
BUFF_TRIGGER(BerserkerStanceTrigger, "berserker stance");

CAN_CAST_TRIGGER(VictoryRushTrigger, "victory rush");
DEBUFF_TRIGGER(ThunderClapTrigger, "thunder clap");
DEBUFF_TRIGGER(SunderArmorTrigger, "sunder armor");
DEBUFF_TRIGGER(ColossusSmashTrigger, "colossus smash");
INTERRUPT_HEALER_TRIGGER(PummelInterruptEnemyHealerSpellTrigger, "pummel");
INTERRUPT_TRIGGER(PummelInterruptSpellTrigger, "pummel");

// -- arms
CAN_CAST_TRIGGER(OverpowerAvailableTrigger, "overpower");
DEBUFF_CHECKISOWNER_TRIGGER(MortalWoundsDebuffTrigger, "mortal wounds");
HAS_AURA_TRIGGER(TasteForBloodTrigger, "taste for blood");

// -- fury
CAN_CAST_TRIGGER(RagingBlowAvailableTrigger, "raging blow");
class MeatCleaverTrigger : public HasAuraTrigger
{
public:
	MeatCleaverTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "meat cleaver") {}
};
class BloodSurgeTrigger : public HasAuraTrigger
{
public:
	BloodSurgeTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "bloodsurge") {}
};

// -- prot
class ShieldBarrierTrigger : public BuffTrigger
{
public:
	ShieldBarrierTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "shield barrier", 10) {}
	bool IsActive() override;
};
BUFF_TRIGGER(ShieldBlockTrigger, "shield block");
DEBUFF_TRIGGER(DisarmDebuffTrigger, "disarm");
CAN_CAST_TRIGGER(RevengeAvailableTrigger, "revenge");
class VigilanceTrigger : public BuffOnPartyTrigger
{
public:
	VigilanceTrigger(PlayerbotAI* botAI) : BuffOnPartyTrigger(botAI, "vigilance") {}

	bool IsActive() override;
};
#endif
