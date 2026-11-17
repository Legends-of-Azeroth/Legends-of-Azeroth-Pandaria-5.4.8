/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "GenericWarlockStrategy.h"

#include "Playerbots.h"

class GenericWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarlockStrategyActionNodeFactory()
    {
        creators["corruption"] = &corruption;
        creators["curse of enfeeblement"] = curse_of_enfeeblement;
        creators["shadow bolt"] = &shadow_bolt;
        creators["banish"] = &banish;
        creators["fear"] = &fear;
        creators["howl of terror"] = &howl_of_terror;
        creators["drain life"] = &drain_life;
        creators["life tap"] = &life_tap;
        creators["summon infernal"] = &summon_infernal;

        // low threat
        creators["soulshatter"] = &soulshatter;
        // low life cd
        creators["unending resolve"] = &unending_resolve;
        creators["fel flame"] = &fel_flame;
        creators["dark intent"] = &dark_intent;

        // low life cd
        creators["twilight ward"] = &twilight_ward;
        creators["health funnel"] = &health_funnel;

        // -- talents
        creators["dark regeneration"] = &dark_regeneration;
        creators["demonic breath"] = &demonic_breath;
        creators["mortal coil"] = &mortal_coil;
        creators["shadowfury"] = &shadowfury;
        creators["soul link"] = &soul_link;
        creators["sacrificial pact"] = &sacrificial_pact;
        creators["dark bargain"] = &dark_bargain;
        creators["blood horror"] = &blood_horror;
        creators["grimoire of supremacy"] = &grimoire_of_supremacy;
        creators["grimoire of sacrifice"] = &grimoire_of_sacrifice;
        creators["mannoroth's fury"] = &mannoroths_fury;
    }

private:
    ACTION_NODE_A(corruption, "corruption", "curse of enfeeblement");
    ACTION_NODE(curse_of_enfeeblement, "curse of enfeeblement");
    ACTION_NODE(shadow_bolt, "shadow bolt");
    ACTION_NODE_A(banish, "banish", "fear");
    ACTION_NODE_A(fear, "fear", "howl of terror");
    ACTION_NODE(howl_of_terror, "howl of terror");
    ACTION_NODE(drain_life, "drain life");
    ACTION_NODE(life_tap, "life tap");
    ACTION_NODE(summon_infernal, "summon infernal");

    ACTION_NODE(unending_resolve, "unending resolve");
    ACTION_NODE(fel_flame, "fel flame");
    ACTION_NODE(dark_intent, "dark intent");
    ACTION_NODE(twilight_ward, "twilight ward");
    ACTION_NODE(soulshatter, "soulshatter");
    ACTION_NODE(health_funnel, "health funnel");
    // -- talents
    ACTION_NODE(dark_regeneration, "dark regeneration");
    ACTION_NODE(demonic_breath, "demonic breath");
    ACTION_NODE(mortal_coil, "mortal coil");
    ACTION_NODE(shadowfury, "shadowfury");
    ACTION_NODE(soul_link, "soul link");
    ACTION_NODE(sacrificial_pact, "sacrificial pact");
    ACTION_NODE(dark_bargain, "dark bargain");
    ACTION_NODE(blood_horror, "blood horror");
    ACTION_NODE(grimoire_of_supremacy, "grimoire of supremacy");
    ACTION_NODE(grimoire_of_sacrifice, "grimoire of sacrifice");
    ACTION_NODE(mannoroths_fury, "mannoroth's fury");
};

GenericWarlockStrategy::GenericWarlockStrategy(PlayerbotAI* botAI) : RangedCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericWarlockStrategyActionNodeFactory());
}

NextAction** GenericWarlockStrategy::getDefaultActions() { return NextAction::array(0, nullptr); }

void GenericWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    RangedCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("curse of enfeeblement", NextAction::array(0, new NextAction("curse of enfeeblement", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode("curse of the elements", NextAction::array(0, new NextAction("curse of the elements", ACTION_HIGH + 5), nullptr)));

    triggers.push_back(new TriggerNode("warlock pet medium health", NextAction::array(0, new NextAction("health funnel", 40.0f), nullptr)));
    triggers.push_back(new TriggerNode("warlock pet low health", NextAction::array(0, new NextAction("dark regeneration", 60.0f), new NextAction("health funnel", 60.0f), nullptr)));

    triggers.push_back(new TriggerNode("soul link", NextAction::array(0, new NextAction("soul link", ACTION_EMERGENCY + 5), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("life tap", ACTION_EMERGENCY + 5), nullptr)));
    triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("dark bargaing", ACTION_INTERRUPT), new NextAction("twilight ward", ACTION_INTERRUPT), new NextAction("soul burn", ACTION_INTERRUPT), new NextAction("drain life", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("sacrificial pact", ACTION_INTERRUPT), new NextAction("unending resolve", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("critical health", NextAction::array(0, new NextAction("dark regeneration", ACTION_INTERRUPT), new NextAction("drain life", ACTION_INTERRUPT), nullptr)));

    triggers.push_back(new TriggerNode("enemy is close", NextAction::array(0,
        new NextAction("fear", 25.0f),
        new NextAction("blood horror", 25.0f),
        new NextAction("demonic breath", 50.0f),
        new NextAction("mortal coil", 50.0f),
        new NextAction("shadowfury", 25.0f),
        nullptr)));
}

void WarlockBoostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("amplify curse", NextAction::array(0, new NextAction("amplify curse", 41.0f), nullptr)));
}

void WarlockCcStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("banish", NextAction::array(0, new NextAction("banish on cc", 32.0f), nullptr)));
    triggers.push_back(new TriggerNode("fear", NextAction::array(0, new NextAction("fear on cc", 33.0f), nullptr)));
}

void AoeWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("medium aoe", NextAction::array(0,
            new NextAction("mannoroth's fury", 35.0f),
            new NextAction("immolation aura", 34.0f),
            new NextAction("seed of corruption", 33.0f),
            new NextAction("shadowfury", 32.0f),
            new NextAction("summon abyssal", 31.0f), 
            new NextAction("rain of fire", 30.0f),
            nullptr)));
}
