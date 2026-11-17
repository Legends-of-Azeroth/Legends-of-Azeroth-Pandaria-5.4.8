/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "GenericMageStrategy.h"

#include "Playerbots.h"
#include "RangedCombatStrategy.h"

class GenericMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericMageStrategyActionNodeFactory()
    {
        creators["frostfire bolt"] = &frostfire_bolt;
        creators["frost nova"] = &frost_nova;
        creators["fire blast"] = &fire_blast;
        creators["counterspell"] = &counterspell;
        creators["arcane explosion"] = &arcane_explosion;
        creators["blizzard"] = &blizzard;
        creators["ice lance"] = &ice_lance;
        creators["arcane missiles"] = &arcane_missiles;
        creators["cone of cold"] = &cone_of_cold;
        creators["remove curse"] = &remove_curse;
        creators["remove curse on party"] = &remove_curse_on_party;
        creators["evocation"] = &evocation;
        creators["deep freeze"] = &deep_freeze;
        creators["blink"] = &blink;

        // talents generic define
        creators["rune of power"] = &rune_of_power;
        creators["ice barrier"] = &ice_barrier;
        creators["flameglow"] = &flameglow;
        creators["frostjaw"] = &frostjaw;
    }

private:
    static ActionNode* frostfire_bolt([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frostfire bolt",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("shoot"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* ice_lance([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("ice lance",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* deep_freeze([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("deep freeze",
            /*P*/ nullptr,
            /*A*/ NextAction::array(0, new NextAction("ice lance"), nullptr),
            /*C*/ nullptr);
    }
    static ActionNode* fire_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("fire blast",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* frost_nova([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frost nova",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* cone_of_cold([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cone of cold",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* evocation([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("evocation",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("mana potion"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* remove_curse([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("remove curse",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("remove lesser curse"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* remove_curse_on_party([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("remove curse on party",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("remove lesser curse on party"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* arcane_missiles([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane missiles",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* blizzard([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("blizzard",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* counterspell([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("counterspell",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* arcane_explosion([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane explosion",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* ice_barrier([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("ice barrier",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* flameglow([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("flameglow",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* rune_of_power([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("rune of power",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* blink([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("blink",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* frostjaw([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("frostjaw",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
};

GenericMageStrategy::GenericMageStrategy(PlayerbotAI* botAI) : RangedCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericMageStrategyActionNodeFactory());
}

void GenericMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    RangedCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("enemy is close", NextAction::array(0,
        new NextAction("frost nova", 25.0f),
        new NextAction("cone of cold", 5.0f),
        new NextAction("dragon's breath", 25.0f),
        new NextAction("blink", 50.0f),
        nullptr)));
    triggers.push_back(new TriggerNode("frost nova on target", NextAction::array(0, new NextAction("deep freeze", ACTION_NORMAL + 1), nullptr)));
    triggers.push_back(new TriggerNode("counterspell on enemy healer",NextAction::array(0, new NextAction("counterspell on enemy healer", 40.0f), nullptr)));
    triggers.push_back(new TriggerNode("critical health", NextAction::array(0, new NextAction("ice block", 80.0f), nullptr)));
    triggers.push_back(new TriggerNode("spellsteal", NextAction::array(0, new NextAction("spellsteal", 40.0f), nullptr)));
    triggers.push_back(new TriggerNode("medium threat", NextAction::array(0, new NextAction("invisibility", 60.0f), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("evocation", ACTION_EMERGENCY + 5), nullptr)));
    triggers.push_back(new TriggerNode("fire ward", NextAction::array(0, new NextAction("fire ward", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("frost ward", NextAction::array(0, new NextAction("frost ward", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("rune of power", NextAction::array(0, new NextAction("rune of power", 50.0f), nullptr)));
    triggers.push_back(new TriggerNode("mirror image", NextAction::array(0, new NextAction("mirror image", 41.0f), nullptr)));
}
