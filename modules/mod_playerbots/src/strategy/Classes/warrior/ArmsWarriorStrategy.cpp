/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "ArmsWarriorStrategy.h"

#include "Playerbots.h"

class ArmsWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArmsWarriorStrategyActionNodeFactory()
    {
        creators["mortal strike"] = &mortal_strike;
        creators["slam"] = &slam;

        creators["overpower"] = &overpower;
        creators["sweeping strikes"] = &sweeping_strikes;
        creators["demoralizing banner"] = &demoralizing_banner;
    }

private:
    ACTION_NODE(mortal_strike, "mortal strike");
    ACTION_NODE(slam, "slam");
    ACTION_NODE(overpower, "overpower");
    ACTION_NODE(sweeping_strikes, "sweeping strikes");
    ACTION_NODE(demoralizing_banner, "demoralizing banner");
};

ArmsWarriorStrategy::ArmsWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new ArmsWarriorStrategyActionNodeFactory());
}

NextAction** ArmsWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("demoralizing banner", ACTION_DEFAULT + 0.8f),
        new NextAction("recklessness", ACTION_DEFAULT + 0.7f),
        new NextAction("bloodbath", ACTION_DEFAULT + 0.6f),
        new NextAction("execute", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT + 0.1f),
        nullptr);
}

void ArmsWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("battle shout", NextAction::array(0, new NextAction("battle shout", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("high rage available",NextAction::array(0, new NextAction("mortal strike", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("overpower", NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("taste for blood", NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
}
