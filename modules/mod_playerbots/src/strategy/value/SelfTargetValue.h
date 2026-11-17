#ifndef _PLAYERBOT_SELFTARGETVALUE_H
#define _PLAYERBOT_SELFTARGETVALUE_H

#include "Value.h"

class PlayerbotAI;
class Unit;

class SelfTargetValue : public UnitCalculatedValue
{
public:
    SelfTargetValue(PlayerbotAI* botAI, std::string const name = "self target") : UnitCalculatedValue(botAI, name) {}

    Unit* Calculate() override;
};

#endif