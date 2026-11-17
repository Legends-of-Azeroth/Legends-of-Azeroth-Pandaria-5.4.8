#ifndef _PLAYERBOT_PARTYMEMBERTODISPEL_H
#define _PLAYERBOT_PARTYMEMBERTODISPEL_H

#include "NamedObjectContext.h"
#include "PartyMemberValue.h"

class PlayerbotAI;
class Unit;

class PartyMemberToDispel : public PartyMemberValue, public Qualified
{
public:
    PartyMemberToDispel(PlayerbotAI* botAI, std::string const name = "party member to dispel")
        : PartyMemberValue(botAI, name, 1000), Qualified()
    {
    }

protected:
    Unit* Calculate() override;
};

#endif
