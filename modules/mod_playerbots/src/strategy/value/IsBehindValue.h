#ifndef _PLAYERBOT_ISBEHINDVALUE_H
#define _PLAYERBOT_ISBEHINDVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class IsBehindValue : public BoolCalculatedValue, public Qualified
{
public:
    IsBehindValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI) {}

    bool Calculate() override;
};

class IsFacingValue : public BoolCalculatedValue, public Qualified
{
public:
    IsFacingValue(PlayerbotAI* botAI, std::string const name = "is facing") : BoolCalculatedValue(botAI, name) {}

    bool Calculate() override;
};

#endif
