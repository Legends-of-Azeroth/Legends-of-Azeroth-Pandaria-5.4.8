/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WARRIORACTIONS_H
#define _PLAYERBOT_WARRIORACTIONS_H

#include "AiObject.h"
#include "GenericSpellActions.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotSpec.h"
#include "ReachTargetActions.h"

// -- Generic
class CastBattleStanceAction : public CastBuffSpellAction
{
public:
    CastBattleStanceAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "battle stance", true) {}
    bool isUseful() override;
};
class CastBerserkerStanceAction : public CastBuffSpellAction
{
public:
    CastBerserkerStanceAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "berserker stance", true) {}
    bool isUseful() override;
};
class CastDefensiveStanceAction : public CastBuffSpellAction
{
public:
    CastDefensiveStanceAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "defensive stance", true) {}
    bool isUseful() override;
};

REACH_ACTION(CastChargeAction, "charge", 8.0f);
MELEE_ACTION(CastHeroicStrikeAction, "heroic strike");
BUFF_ACTION(CastCommandingShoutAction, "commanding shout");
BUFF_ACTION(CastBattleShoutAction, "battle shout");

class CastHeroicLeapAction : public CastSpellAction
{
public:
    CastHeroicLeapAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "heroic leap") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

BUFF_ACTION(CastBloodbathAction, "bloodbath");
BUFF_ACTION(CastBerserkerRageAction, "berserker rage");
BUFF_ACTION(CastRallyingCryAction, "rallying cry");
BUFF_ACTION(CastShieldWallAction, "shield wall");
BUFF_ACTION(CastRecklessnessAction, "recklessness");
BUFF_ACTION(CastDieByTheSwordAction, "die by the sword");

MELEE_ACTION(CastColossusSmashAction, "colossus smash");
MELEE_ACTION(CastVictoryRushAction, "victory rush");
MELEE_ACTION(CastExecuteAction, "execute");
class CastSunderArmorAction : public CastDebuffSpellAction
{
public:
    CastSunderArmorAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "sunder armor")
    {
        range = ATTACK_DISTANCE;
    }

    bool isUseful() override;
};

// -- aoe
MELEE_ACTION(CastThunderClapAction, "thunder clap");
MELEE_ACTION(CastWhirlWindAction, "whirlwind");
MELEE_ACTION(CastCleaveAction, "cleave");
MELEE_ACTION(CastBladestormAction, "bladestorm");
MELEE_ACTION(CastDragonroarAction, "dragonroar");
DEBUFF_ACTION_R(CastShockwaveAction, "shockwave", 8.0f);

// -- arms
MELEE_ACTION(CastMortalStrikeAction, "mortal strike");
MELEE_ACTION(CastSlamAction, "slam");
MELEE_ACTION(CastOverPowerAction, "overpower");
BUFF_ACTION(CastSweepingStrikesAction, "sweeping strikes");
class CastDemoralizingBannerAction : public CastSpellAction
{
public:
    CastDemoralizingBannerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "demoralizing banner") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// -- FURY
MELEE_ACTION(CastRagingBlowAction, "raging blow");
MELEE_ACTION(CastBloodThirstAction, "bloodthirst");
MELEE_ACTION(CastWildStrikeAction, "wild strike");
class CastSkullBannerAction : public CastSpellAction
{
public:
    CastSkullBannerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "skull banner") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// -- PROT
class CastDemoralizingShoutAction : public CastMeleeDebuffSpellAction
{
public:
    CastDemoralizingShoutAction(PlayerbotAI* botAI)
        : CastMeleeDebuffSpellAction(botAI, "demoralizing shout") {}
};
MELEE_ACTION(CastDevastateAction, "devastate");
MELEE_ACTION_U(CastTauntAction, "taunt", GetTarget() && GetTarget()->GetTarget() != bot->GetGUID());
MELEE_ACTION(CastShieldBashAction, "shield bash");
MELEE_ACTION(CastRevengeAction, "revenge");
BUFF_ACTION(CastShieldBlockAction, "shield block");
BUFF_ACTION(CastShieldBarrierAction, "shield barrier");
DEBUFF_ACTION_U(CastDisarmAction, "disarm",
    GetTarget() && GetTarget()->IsPlayer() ? !PlayerBotSpec::IsRanged((Player*)GetTarget())
    : CastDebuffSpellAction::isUseful());
BUFF_ACTION(CastLastStandAction, "last stand");
BUFF_ACTION(CastEnragedRegenerationAction, "enraged regeneration");
MELEE_ACTION(CastShieldSlamAction, "shield slam");
class CastVigilanceAction : public BuffOnPartyAction
{
public:
    CastVigilanceAction(PlayerbotAI* botAI) : BuffOnPartyAction(botAI, "vigilance") {}

    Unit* GetTarget() override;
    bool Execute(Event event) override;
};
DEBUFF_ACTION_R(CastStormBoltAction, "storm bolt", 8.0f);

class CastMockingBannerAction : public CastSpellAction
{
public:
    CastMockingBannerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "mocking banner") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

#endif
