#include "AffliWarlockStrategy.h"
#include "Playerbots.h"

class AffliWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    AffliWarlockStrategyActionNodeFactory()
    {
        creators["dark soul: misery"] = &dark_soul;
        creators["malefic grasp"] = &malefic_grasp;
        creators["agony"] = &agony;
        creators["drain soul"] = &drain_soul;
        creators["haunt"] = &haunt;
        creators["unstable affliction"] = &unstable_affliction;
        creators["unstable affliction on attacker"] = &unstable_affliction_on_attacker;
        creators["seed of corruption"] = &seed_of_corruption;
    }

private:
    
    ACTION_NODE(dark_soul, "dark soul: misery");
    ACTION_NODE_A(malefic_grasp, "malefic grasp", "shadow bolt");
    ACTION_NODE(agony, "agony");
    ACTION_NODE_P(drain_soul, "drain soul", "corruption");
    ACTION_NODE(haunt, "haunt");
    ACTION_NODE(unstable_affliction, "unstable affliction");
    ACTION_NODE(unstable_affliction_on_attacker, "unstable affliction on attacker");
    ACTION_NODE(seed_of_corruption, "seed of corruption");
};

AffliWarlockStrategy::AffliWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new AffliWarlockStrategyActionNodeFactory());
}

NextAction** AffliWarlockStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("dark soul: misery", ACTION_DEFAULT + 0.5f),
        new NextAction("haunt", ACTION_DEFAULT + 0.4f),
        new NextAction("fel flame", 3.0f),
        new NextAction("malefic grasp", ACTION_HIGH),
        nullptr);
}

void AffliWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("corruption", NextAction::array(0, new NextAction("corruption", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("agony", NextAction::array(0, new NextAction("agony", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("unstable affliction", NextAction::array(0, new NextAction("unstable affliction", 20.0f), nullptr)));

    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("drain soul", ACTION_HIGH + 20), nullptr)));
    // cast during movement
    triggers.push_back(new TriggerNode("high mana", NextAction::array(0, new NextAction("life tap", ACTION_DEFAULT + 0.1f), nullptr)));
    triggers.push_back(new TriggerNode("life tap glyph buff", NextAction::array(0, new NextAction("life tap", 28.0f), nullptr)));
    triggers.push_back(new TriggerNode("metamorphosis", NextAction::array(0, new NextAction("metamorphosis", 20.0f), nullptr)));
}

