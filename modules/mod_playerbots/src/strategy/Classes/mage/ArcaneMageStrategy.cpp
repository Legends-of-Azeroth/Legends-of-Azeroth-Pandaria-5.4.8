/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "ArcaneMageStrategy.h"

#include "Playerbots.h"

class ArcaneMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArcaneMageStrategyActionNodeFactory()
    {
        creators["arcane blast"] = &arcane_blast;
        creators["arcane barrage"] = &arcane_barrage;
        creators["slow"] = &slow;
        creators["nether tempest"] = &nether_tempest;
    }

private:
    static ActionNode* arcane_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane blast",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("arcane missiles"), nullptr),
                              /*C*/ nullptr);
    }

    static ActionNode* arcane_barrage([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane barrage",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* slow([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("slow",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }

    static ActionNode* nether_tempest([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("nether tempest",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
};

ArcaneMageStrategy::ArcaneMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    actionNodeFactories.Add(new ArcaneMageStrategyActionNodeFactory());
}

NextAction** ArcaneMageStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("frostjaw", ACTION_DEFAULT + 0.10f),
        new NextAction("nether tempest", ACTION_DEFAULT + 0.3f),
        new NextAction("arcane blast", ACTION_DEFAULT + 0.3f),
        new NextAction("fire blast", ACTION_DEFAULT + 0.1f), // cast during movement
        new NextAction("shoot", ACTION_DEFAULT), nullptr);
}

void ArcaneMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("arcane blast stack", NextAction::array(0, new NextAction("arcane missiles", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane missiles", NextAction::array(0, new NextAction("arcane missiles", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane charge", NextAction::array(0, new NextAction("arcane missiles", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane charge", NextAction::array(0, new NextAction("arcane barrage", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("slow not on target", NextAction::array(0, new NextAction("slow", ACTION_NORMAL + 1), nullptr)));
    triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("ice barrier", ACTION_NORMAL), nullptr)));
    triggers.push_back(new TriggerNode("nether tempest", NextAction::array(0, new NextAction("nether tempest", 19.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane power", NextAction::array(0, new NextAction("arcane power", 19.0f), nullptr)));
}

void ArcaneMageAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0,
        new NextAction("nether tempest on attackers", 24.0f),
        new NextAction("blizzard", 23.0f), nullptr)));
}