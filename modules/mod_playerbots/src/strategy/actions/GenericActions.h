#ifndef _PLAYERBOT_GENERICACTIONS_H
#define _PLAYERBOT_GENERICACTIONS_H

#include "AttackActions.h"

class PlayerbotAI;

class MeleeAction : public AttackAction
{
public:
    MeleeAction(PlayerbotAI* botAI) : AttackAction(botAI, "melee") {}

    std::string const GetTargetName() override { return "current target"; }
    bool isUseful() override;
};

class TogglePetSpellAutoCastAction : public Action
{
public:
    TogglePetSpellAutoCastAction(PlayerbotAI* ai) : Action(ai, "toggle pet spell") {}
    virtual bool Execute(Event event) override;
};

class PetAttackAction : public Action
{
public:
    PetAttackAction(PlayerbotAI* ai) : Action(ai, "pet attack") {}
    virtual bool Execute(Event event) override;
};

#endif
