#include "MeleeCombatStrategy.h"

#include "Playerbots.h"

void MeleeCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("enemy out of melee", NextAction::array(0, new NextAction("reach melee", ACTION_HIGH + 1), nullptr)));
}

void SetBehindCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("not behind target", NextAction::array(0, new NextAction("set behind", ACTION_MOVE + 7), nullptr)));
}
