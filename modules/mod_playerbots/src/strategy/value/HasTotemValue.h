#ifndef _PLAYERBOT_HASTOTEMVALUE_H
#define _PLAYERBOT_HASTOTEMVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class HasTotemValue : public BoolCalculatedValue, public Qualified
{
public:
    HasTotemValue(PlayerbotAI* botAI, std::string const name = "has totem") : BoolCalculatedValue(botAI, name) {}

    bool Calculate() override;
};

#endif
