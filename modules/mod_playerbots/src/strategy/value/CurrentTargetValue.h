#ifndef _PLAYERBOT_CURRENTTARGETVALUE_H
#define _PLAYERBOT_CURRENTTARGETVALUE_H

#include "Value.h"

class PlayerbotAI;
class Unit;

class CurrentTargetValue : public UnitManualSetValue
{
public:
    CurrentTargetValue(PlayerbotAI* botAI, std::string const name = "current target")
        : UnitManualSetValue(botAI, nullptr, name)
    {
    }

    Unit* Get() override;
    void Set(Unit* unit) override;

private:
    ObjectGuid selection;
};

#endif
