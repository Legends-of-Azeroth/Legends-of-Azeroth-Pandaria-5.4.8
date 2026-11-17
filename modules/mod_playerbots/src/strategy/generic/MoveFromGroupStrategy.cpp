#include "MoveFromGroupStrategy.h"
#include "PassiveMultiplier.h"
#include "Playerbots.h"

NextAction** MoveFromGroupStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("move from group", 1.0f), nullptr);
}

void MoveFromGroupStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new PassiveMultiplier(botAI));
}
