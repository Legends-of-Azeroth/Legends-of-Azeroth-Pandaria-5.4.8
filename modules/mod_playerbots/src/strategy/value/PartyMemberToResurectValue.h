#ifndef _PLAYERBOT_PARTYMEMBERTORESURRECT_H
#define _PLAYERBOT_PARTYMEMBERTORESURRECT_H

#include "PartyMemberValue.h"

class PlayerbotAI;
class Unit;

class PartyMemberToResurrect : public PartyMemberValue
{
public:
    PartyMemberToResurrect(PlayerbotAI* botAI, std::string const name = "party member to resurrect")
        : PartyMemberValue(botAI, name)
    {
    }

protected:
    Unit* Calculate() override;
};

#endif
