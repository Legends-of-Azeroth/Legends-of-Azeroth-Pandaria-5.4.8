/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "FrostMageStrategy.h"

#include "Playerbots.h"

class FrostMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FrostMageStrategyActionNodeFactory()
    {
        creators["frostbolt"] = &frostbolt;
        creators["summon water elemental"] = &summon_water_elemental;
        creators["fingers of frost"] = &fingers_of_frost;
        creators["icy veins"] = &icy_veins;
        creators["frozen orb"] = &frozen_orb;
        creators["cold snap"] = &cold_snap;
        creators["deep freeze"] = &deep_freeze;
        creators["frost bomb"] = &frost_bomb;
    }

private:
    static ActionNode* frostbolt([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frostbolt",
            /*P*/ nullptr,
            /*A*/ NextAction::array(0, new NextAction("frostfire bolt"), nullptr),
            /*C*/ nullptr);
    }
    static ActionNode* summon_water_elemental([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("summon water elemental",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* fingers_of_frost([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fingers of frost",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* icy_veins([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("icy veins",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* frozen_orb([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frozen orb",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* cold_snap([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cold snap",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* deep_freeze([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("deep freeze",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* frost_bomb([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frost bomb",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
};

FrostMageStrategy::FrostMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    actionNodeFactories.Add(new FrostMageStrategyActionNodeFactory());
}

NextAction** FrostMageStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("frostjaw", ACTION_DEFAULT + 0.10f),
        new NextAction("frostbolt", ACTION_DEFAULT + 0.9f),
        new NextAction("frost bomb", ACTION_DEFAULT + 0.8f),
        new NextAction("frostfire bolt", ACTION_DEFAULT + 0.7f),
        new NextAction("ice lance", ACTION_DEFAULT + 0.5f),
        new NextAction("shoot", ACTION_DEFAULT), nullptr);
}

void FrostMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);
    triggers.push_back(new TriggerNode("icy veins", NextAction::array(0, new NextAction("icy veins", 50.0f), nullptr)));

    triggers.push_back(new TriggerNode("no pet", NextAction::array(0, new NextAction("summon water elemental", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("has pet", NextAction::array(0, new NextAction("toggle pet spell", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(new TriggerNode("brain freeze", NextAction::array(0, new NextAction("frostfire bolt", ACTION_NORMAL + 3), nullptr)));
    triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("ice barrier", ACTION_NORMAL), nullptr)));
    // Combo cast the last charge of fingers of frost for double crits.
    // Should only do this on the final charge of FoF.
    triggers.push_back(new TriggerNode("fingers of frost single",
        NextAction::array(0, new NextAction("deep freeze", ACTION_NORMAL + 2),
        new NextAction("ice lance", ACTION_NORMAL + 1), nullptr)));

    triggers.push_back(new TriggerNode("forst bomb", NextAction::array(0, new NextAction("forst bomb", 19.0f), nullptr)));
}

void FrostMageAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0,
        new NextAction("frost bomb on attackers", 24.0f),
        new NextAction("frozen orb", 23.0f),
        new NextAction("blizzard", 22.0f), nullptr)));

    triggers.push_back(new TriggerNode("light aoe", NextAction::array(0,
        new NextAction("frost bomb on attackers", 24.0f),
        new NextAction("cone of cold", 23.0f),
        new NextAction("frozen orb", 22.0f), nullptr)));
}
