#include "DestruWarlockStrategy.h"
#include "Playerbots.h"

class DestruWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DestruWarlockStrategyActionNodeFactory()
    {
        creators["dark soul: instability"] = &dark_soul;
        creators["chaos bolt"] = &chaos_bolt;
        creators["conflagrate"] = &conflagrate;
        creators["fire and brimstone"] = &fire_and_brimstone;

        creators["flames of xoroth"] = &flames_of_xoroth;
        creators["immolate"] = &immolate;
        creators["incinerate"] = &incinerate;
        creators["shadowburn"] = &shadowburn;
    }

private:
    ACTION_NODE(dark_soul, "dark soul: instability");
    ACTION_NODE(chaos_bolt, "chaos bolt");
    ACTION_NODE(conflagrate, "conflagrate");
    ACTION_NODE(fire_and_brimstone, "fire and brimstone");
    ACTION_NODE(flames_of_xoroth, "flames of xoroth");
    ACTION_NODE_C(immolate, "immolate", "conflagrate");
    ACTION_NODE(incinerate, "incinerate");
    ACTION_NODE(shadowburn, "shadowburn");
};

DestruWarlockStrategy::DestruWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new DestruWarlockStrategyActionNodeFactory());
}

NextAction** DestruWarlockStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("dark soul: instability", ACTION_DEFAULT + 0.5f),
        new NextAction("conflagrate", ACTION_DEFAULT + 20.0f),
        new NextAction("incinerate", ACTION_HIGH),
        nullptr);
}

void DestruWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("immolate", NextAction::array(0, new NextAction("immolate", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("backdraft", NextAction::array(0, new NextAction("chaos bolt", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("fire and brimstone", NextAction::array(0, new NextAction("fire and brimstone", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("shadowburn", ACTION_HIGH + 20), nullptr)));
}

