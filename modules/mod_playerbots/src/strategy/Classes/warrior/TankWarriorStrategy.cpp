/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "TankWarriorStrategy.h"

#include "Playerbots.h"

class TankWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    TankWarriorStrategyActionNodeFactory()
    {
        creators["commanding shout"] = &commanding_shout;
        creators["demoralizing shout"] = &demoralizing_shout;
        creators["devastate"] = &devastate;
        creators["last stand"] = &last_stand;
        creators["revenge"] = &revenge;
        creators["shield barrier"] = &shield_barrier;
        creators["shield block"] = &shield_block;
        creators["shield slam"] = &shield_slam;
        creators["taunt"] = &taunt;
        creators["enraged regeneration"] = &enraged_regeneration;
        creators["vigilance"] = &vigilance;
        creators["storm bolt"] = &storm_bolt;
        creators["mocking banner"] = &mocking_banner;
    }

private:
    ACTION_NODE(commanding_shout, "commanding shout");
    ACTION_NODE(demoralizing_shout, "demoralizing shout");
    ACTION_NODE_A(devastate, "devastate", "sunder armor");
    ACTION_NODE(last_stand, "last stand");
    ACTION_NODE(revenge, "revenge");
    ACTION_NODE(shield_barrier, "shield barrier");
    ACTION_NODE(shield_block, "shield block");
    ACTION_NODE(shield_slam, "shield slam");
    ACTION_NODE(taunt, "taunt");
    ACTION_NODE(enraged_regeneration, "enraged regeneration");
    ACTION_NODE(vigilance, "vigilance");
    ACTION_NODE(storm_bolt, "storm bolt");
    ACTION_NODE(mocking_banner, "mocking banner");
};

TankWarriorStrategy::TankWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new TankWarriorStrategyActionNodeFactory());
}

NextAction** TankWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0, 
        new NextAction("shield slam", ACTION_DEFAULT + 0.5f),
        new NextAction("storm bolt", ACTION_DEFAULT + 0.4f),
        new NextAction("devastate", ACTION_DEFAULT + 0.3f),
        new NextAction("revenge", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void TankWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("commanding shout", NextAction::array(0, new NextAction("commanding shout", ACTION_HIGH + 8), nullptr)));
    triggers.push_back(new TriggerNode("demoralizing shout", NextAction::array(0, new NextAction("demoralizing shout", ACTION_HIGH + 8), nullptr)));

    triggers.push_back(new TriggerNode("vigilance", NextAction::array(0, new NextAction("vigilance", ACTION_HIGH + 7), nullptr)));
    triggers.push_back(new TriggerNode("shield block", NextAction::array(0, new NextAction("shield block", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(new TriggerNode("revenge", NextAction::array(0, new NextAction("revenge", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("lose aggro", NextAction::array(0, new NextAction("taunt", ACTION_INTERRUPT + 1), nullptr)));
    triggers.push_back(new TriggerNode("shield barrier", NextAction::array(0, new NextAction("shield barrier", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("medium rage available",NextAction::array(0, new NextAction("thunder clap", ACTION_HIGH + 1), nullptr)));
}
