#ifndef _PLAYERBOT_DISTANCEVALUE_H
#define _PLAYERBOT_DISTANCEVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class DistanceValue : public FloatCalculatedValue, public Qualified
{
public:
    DistanceValue(PlayerbotAI* botAI, std::string const name = "distance") : FloatCalculatedValue(botAI, name) {}

    float Calculate() override;
};

class InsideTargetValue : public BoolCalculatedValue, public Qualified
{
public:
    InsideTargetValue(PlayerbotAI* botAI, std::string const name = "inside target") : BoolCalculatedValue(botAI, name)
    {
    }

    bool Calculate() override;
};

#endif
