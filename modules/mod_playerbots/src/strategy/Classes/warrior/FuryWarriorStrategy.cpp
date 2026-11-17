/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "FuryWarriorStrategy.h"

#include "Playerbots.h"

class FuryWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FuryWarriorStrategyActionNodeFactory()
    {
        creators["raging blow"] = &raging_blow;
        creators["bloodthirst"] = &bloodthirst;
        creators["wild strike"] = &wild_strike;
        creators["skull banner"] = &skull_banner;
    }

private:
    ACTION_NODE(raging_blow, "raging blow");
    ACTION_NODE(bloodthirst, "bloodthirst");
    ACTION_NODE(wild_strike, "wild strike");
    ACTION_NODE(skull_banner, "skull banner");
};

FuryWarriorStrategy::FuryWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new FuryWarriorStrategyActionNodeFactory());
}

NextAction** FuryWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("skull banner", ACTION_DEFAULT + 0.8f),
        new NextAction("recklessness", ACTION_DEFAULT + 0.7f),
        new NextAction("bloodbath", ACTION_DEFAULT + 0.6f),
        new NextAction("bloodthirst", ACTION_DEFAULT + 0.5f),
        new NextAction("whirlwind", ACTION_DEFAULT + 0.4f),
        new NextAction("sunder armor", ACTION_DEFAULT + 0.3f),
        new NextAction("execute", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT), NULL);
}

void FuryWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("battle shout", NextAction::array(0, new NextAction("battle shout", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("raging blow", NextAction::array(0, new NextAction("raging blow", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("meat cleaver", NextAction::array(0, new NextAction("raging blow", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("bloodsurge", NextAction::array(0, new NextAction("wild strike", ACTION_HIGH + 9), nullptr)));
}
