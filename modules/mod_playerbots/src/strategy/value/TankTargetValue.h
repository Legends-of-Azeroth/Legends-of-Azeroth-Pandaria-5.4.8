#ifndef _PLAYERBOT_TANKTARGETVALUE_H
#define _PLAYERBOT_TANKTARGETVALUE_H

#include "TargetValue.h"

class PlayerbotAI;

class TankTargetValue : public TargetValue
{
public:
    TankTargetValue(PlayerbotAI* botAI, std::string const name = "tank target") : TargetValue(botAI, name) {}

    Unit* Calculate() override;
};

#endif
