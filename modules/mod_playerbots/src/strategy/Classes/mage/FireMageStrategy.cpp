/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "FireMageStrategy.h"

#include "Playerbots.h"

/*
* Rune of power is cast on target not self
*/

class FireMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FireMageStrategyActionNodeFactory()
    {
        creators["fireball"] = &fireball;
        creators["dragon's breath"] = &dragon_breath;
        creators["inferno blast"] = &inferno_blast;
        creators["pyroblast"] = &pyroblast;
        creators["scorch"] = &scorch;
        creators["flamestrike"] = &flamestrike;
    }

private:
    static ActionNode* fireball([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fireball",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* dragon_breath([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("dragon's breath",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* inferno_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("inferno blast",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* pyroblast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("pyroblast",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* scorch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("scorch",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* flamestrike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("flamestrike",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
};

FireMageStrategy::FireMageStrategy(PlayerbotAI* botAI)
    : GenericMageStrategy(botAI)
{
    actionNodeFactories.Add(new FireMageStrategyActionNodeFactory());
}

NextAction** FireMageStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("frostjaw", ACTION_DEFAULT + 0.10f),
        new NextAction("combustion", ACTION_DEFAULT + 0.9f),
        new NextAction("inferno blast", ACTION_DEFAULT + 0.8f), // cast during movement
        new NextAction("living bomb", ACTION_DEFAULT + 0.7f),
        new NextAction("fireball", ACTION_DEFAULT + 0.6f),
        new NextAction("scorch", ACTION_DEFAULT), nullptr);
}

void FireMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("ice barrier", ACTION_NORMAL), nullptr)));
    triggers.push_back(new TriggerNode("heating up", NextAction::array(0, new NextAction("inferno blast", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("pyroblast!", NextAction::array(0, new NextAction("pyroblast", ACTION_INTERRUPT + 10), nullptr)));
    triggers.push_back(new TriggerNode("combustion", NextAction::array(0, new NextAction("combustion", 50.0f), nullptr)));
    triggers.push_back(new TriggerNode("living bomb", NextAction::array(0, new NextAction("living bomb", 19.0f), nullptr)));
    // triggers.push_back(new TriggerNode("enemy too close for spell", NextAction::array(0, new NextAction("dragon's
    // breath", 70.0f), nullptr)));
}

void FireMageAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("medium aoe", NextAction::array(0,
            new NextAction("dragon's breath", 24.0f),
            new NextAction("flamestrike", 23.0f), 
            new NextAction("living bomb on attackers", 21.0f), 
            new NextAction("blizzard", 20.0f), nullptr)));
}
