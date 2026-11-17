/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "GenericWarlockNonCombatStrategy.h"

#include "Playerbots.h"

class GenericWarlockNonCombatStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarlockNonCombatStrategyActionNodeFactory()
    {
        creators["dark intent"] = &dark_intent;
        creators["summon voidwalker"] = &summon_voidwalker;
        creators["summon felguard"] = &summon_felguard;
        creators["summon succubus"] = &summon_succubus;
        creators["summon felhunter"] = &summon_felhunter;
        creators["summon doomguard"] = &summon_doomguard;
        creators["summon infernal"] = &summon_infernal;

        creators["summon wrathguard"] = &summon_wrathguard;
        creators["summon fel imp"] = &summon_fel_imp;
        creators["summon voidlord"] = &summon_voidlord;
        creators["summon shivarra"] = &summon_shivarra;
        creators["summon observer"] = &summon_observer;
        creators["summon abyssal"] = &summon_abyssal;
        creators["summon terrorguard"] = &summon_terrorguard;
    }

private:
    ACTION_NODE(dark_intent, "dark intent");

    ACTION_NODE_A(summon_fel_imp, "summon fel imp", "summon imp");
    ACTION_NODE_A(summon_voidlord, "summon voidlord", "summon voidwalker");
    ACTION_NODE_A(summon_shivarra, "summon shivarra", "summon succubus");
    ACTION_NODE_A(summon_observer, "summon observer", "summon felhunter");
    ACTION_NODE_A(summon_abyssal, "summon abyssal", "summon infernal");
    ACTION_NODE_A(summon_terrorguard, "summon terrorguard", "summon doomguard");

    ACTION_NODE_A(summon_voidwalker, "summon voidwalker", "summon fel imp");
    ACTION_NODE_A(summon_felguard, "summon felguard", "summon voidlord");
    ACTION_NODE_A(summon_succubus, "summon succubus", "summon voidlord");
    ACTION_NODE_A(summon_felhunter, "summon felhunter", "summon shivarra");
    ACTION_NODE_A(summon_wrathguard, "summon wrathguard", "summon felguard");
    ACTION_NODE(summon_doomguard, "summon doomguard");
    ACTION_NODE(summon_infernal, "summon infernal");
};

GenericWarlockNonCombatStrategy::GenericWarlockNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericWarlockNonCombatStrategyActionNodeFactory());
}

void GenericWarlockNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("dark intent", NextAction::array(0, new NextAction("dark intent", 60.0f), nullptr)));
    triggers.push_back(new TriggerNode("has pet", NextAction::array(0, new NextAction("toggle pet spell", 60.0f), nullptr)));
    triggers.push_back(new TriggerNode("no healthstone", NextAction::array(0, new NextAction("create healthstone", 15.0f), nullptr)));
}

void WarlockPetStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // TODO Warlock pets
}

SummonImpStrategy::SummonImpStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}

void SummonImpStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no pet", NextAction::array(0, new NextAction("summon fel imp", 11.0f), NULL)));
}

SummonFelguardStrategy::SummonFelguardStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}

void SummonFelguardStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no pet", NextAction::array(0, new NextAction("summon wrathguard", 11.0f), NULL)));
}

SummonFelhunterStrategy::SummonFelhunterStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}

void SummonFelhunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no pet", NextAction::array(0, new NextAction("summon observer", 11.0f), NULL)));
}