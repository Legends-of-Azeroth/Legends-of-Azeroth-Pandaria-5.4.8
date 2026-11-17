#include "GenericWarriorStrategy.h"

#include "Playerbots.h"

class GenericWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarriorStrategyActionNodeFactory()
    {
        creators["heroic strike"] = &GenericWarriorStrategyActionNodeFactory::heroic_strike;
        creators["heroic leap"] = &GenericWarriorStrategyActionNodeFactory::heroic_leap;
        creators["charge"] = &GenericWarriorStrategyActionNodeFactory::charge;

        creators["whirlwind"] = &GenericWarriorStrategyActionNodeFactory::whirlwind;
        creators["victory rush"] = &GenericWarriorStrategyActionNodeFactory::victory_rush;
        creators["execute"] = &GenericWarriorStrategyActionNodeFactory::execute;
        creators["sunder armor"] = &GenericWarriorStrategyActionNodeFactory::sunder_armor;
        creators["thunder clap"] = &GenericWarriorStrategyActionNodeFactory::thunder_clap;
        creators["bloodbath"] = &GenericWarriorStrategyActionNodeFactory::bloodbath;

        creators["berserker rage"] = &GenericWarriorStrategyActionNodeFactory::berserker_rage;
        creators["rallying cry"] = &GenericWarriorStrategyActionNodeFactory::rallying_cry;
        creators["shield wall"] = &GenericWarriorStrategyActionNodeFactory::shield_wall;
        creators["recklessness"] = &GenericWarriorStrategyActionNodeFactory::recklessness;
        creators["colossus smash"] = &GenericWarriorStrategyActionNodeFactory::colossus_smash;
        creators["die by the sword"] = &GenericWarriorStrategyActionNodeFactory::die_by_the_sword;
        creators["pummel"] = &GenericWarriorStrategyActionNodeFactory::pummel;
    }

private:
    ACTION_NODE(heroic_strike, "heroic strike");
    ACTION_NODE_A(heroic_leap, "heroic leap", "melee");
    ACTION_NODE_A(charge, "charge", "heroic leap");

    ACTION_NODE(whirlwind, "whirlwind");
    ACTION_NODE(victory_rush, "victory rush");
    ACTION_NODE(execute, "execute");
    ACTION_NODE(sunder_armor, "sunder armor");
    ACTION_NODE(thunder_clap, "thunder clap");
    ACTION_NODE(bloodbath, "bloodbath");
    
    ACTION_NODE(berserker_rage, "berserker rage");
    ACTION_NODE_A(rallying_cry, "rallying cry", "die by the sword");
    ACTION_NODE_A(shield_wall, "shield wall", "last stand");
    ACTION_NODE(recklessness, "recklessness");
    ACTION_NODE(colossus_smash, "colossus smash");
    ACTION_NODE_A(die_by_the_sword, "die by the sword", "enraged regeneration");
    ACTION_NODE(pummel, "pummel");
};

GenericWarriorStrategy::GenericWarriorStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericWarriorStrategyActionNodeFactory());
}

void GenericWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("battle stance", NextAction::array(0, new NextAction("battle stance", ACTION_DEFAULT), nullptr)));
    triggers.push_back(new TriggerNode("defensive stance", NextAction::array(0, new NextAction("defensive stance", ACTION_DEFAULT), nullptr)));
    triggers.push_back(new TriggerNode("berserker stance", NextAction::array(0, new NextAction("berserker stance", ACTION_DEFAULT), nullptr)));

    triggers.push_back(new TriggerNode("light rage available", NextAction::array(0, new NextAction("berserker rage", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("enemy out of melee", NextAction::array(0, new NextAction("charge", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("execute", ACTION_HIGH + 20), nullptr)));
    triggers.push_back(new TriggerNode("victory rush", NextAction::array(0, new NextAction("victory rush", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("colossus smash", NextAction::array(0, new NextAction("colossus smash", ACTION_HIGH + 4), nullptr)));
    //triggers.push_back(new TriggerNode("sunder armor", NextAction::array(0, new NextAction("sunder armor", ACTION_HIGH + 4), nullptr)));
    //triggers.push_back(new TriggerNode("thunder clap", NextAction::array(0, new NextAction("thunder clap", ACTION_HIGH + 2), nullptr)));

    //triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("shield wall", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("critical health", NextAction::array(0, new NextAction("rallying cry", ACTION_INTERRUPT), nullptr)));

    //triggers.push_back(new TriggerNode("pummel on enemy healer", NextAction::array(0, new NextAction("pummel on enemy healer", ACTION_INTERRUPT), nullptr)));
    triggers.push_back(new TriggerNode("pummel", NextAction::array(0, new NextAction("pummel", ACTION_INTERRUPT), nullptr)));
}

class WarriorAoeStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    WarriorAoeStrategyActionNodeFactory()
    {
        // -- talents
        creators["cleave"] = &WarriorAoeStrategyActionNodeFactory::cleave;
        creators["bladestorm"] = &WarriorAoeStrategyActionNodeFactory::bladestorm;
        creators["dragon roar"] = &WarriorAoeStrategyActionNodeFactory::dragon_roar;
        creators["shockwave"] = &WarriorAoeStrategyActionNodeFactory::shockwave;
    }

private:
    ACTION_NODE_A(cleave, "cleave", "melee");
    ACTION_NODE_A(bladestorm, "bladestorm", "melee");
    ACTION_NODE_A(dragon_roar, "dragon_roar", "melee");
    ACTION_NODE_A(shockwave, "shockwave", "melee");
};

WarriorAoeStrategy::WarriorAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new WarriorAoeStrategyActionNodeFactory());
}

void WarriorAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0,
        new NextAction("mocking banner", ACTION_DEFAULT + 5.0f),
        new NextAction("bloodbath", 25.0f),
        new NextAction("bladestorm", 25.0f),
        new NextAction("dragon_roar", 25.0f),
        new NextAction("shockwave", 25.0f),
        new NextAction("demoralizing shout", 21.0f),
        new NextAction("sweeping strikes", ACTION_HIGH + 7),
        new NextAction("thunder clap", ACTION_HIGH + 5),
        new NextAction("cleave", ACTION_HIGH),
        nullptr)));

    triggers.push_back(new TriggerNode("light aoe", NextAction::array(0,
        new NextAction("mocking banner", ACTION_DEFAULT + 2.0f),
        new NextAction("bloodbath", 25.0f),
        new NextAction("bladestorm", 25.0f),
        new NextAction("dragon_roar", 25.0f),
        new NextAction("shockwave", 25.0f),
        new NextAction("sweeping strikes", ACTION_HIGH + 7),
        new NextAction("thunder clap", ACTION_HIGH + 5),
        new NextAction("cleave", ACTION_HIGH),
        nullptr)));
}