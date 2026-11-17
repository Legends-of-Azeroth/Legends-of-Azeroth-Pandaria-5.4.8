#ifndef _PLAYERBOT_RANGEVALUES_H
#define _PLAYERBOT_RANGEVALUES_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class RangeValue : public ManualSetValue<float>, public Qualified
{
public:
    RangeValue(PlayerbotAI* botAI);

    std::string const Save() override;
    bool Load(std::string const value) override;
};

#endif
