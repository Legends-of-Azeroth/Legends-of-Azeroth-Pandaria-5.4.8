/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "BotAIObjectContext.h"

#include "HelloStrategy.h"
#include "MeleeCombatStrategy.h"
#include "NonCombatStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"

// non combat default class strategy
class GenericBotAiNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericBotAiNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override
    {
        NonCombatStrategy::InitTriggers(triggers);
    }
};

class BotAiStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    BotAiStrategyFactoryInternal()
    {
        creators["nc"] = &BotAiStrategyFactoryInternal::nc;
        creators["melee"] = &BotAiStrategyFactoryInternal::melee;
        creators["say hello"] = &BotAiStrategyFactoryInternal::hello;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericBotAiNonCombatStrategy(botAI); }
    static Strategy* melee(PlayerbotAI* botAI) { return new MeleeCombatStrategy(botAI); }
    static Strategy* hello(PlayerbotAI* ai) { return new HelloStrategy(ai); }
};

class BotAiCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    BotAiCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {

    }
private:
};

class BotAiTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    BotAiTriggerFactoryInternal()
    {
    }
private:
};

class BotAiAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    BotAiAiObjectContextInternal()
    {
    }
private:
};

BotAiObjectContext::BotAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new BotAiStrategyFactoryInternal());
    strategyContexts.Add(new BotAiCombatStrategyFactoryInternal());
    actionContexts.Add(new BotAiAiObjectContextInternal());
    triggerContexts.Add(new BotAiTriggerFactoryInternal());
}
