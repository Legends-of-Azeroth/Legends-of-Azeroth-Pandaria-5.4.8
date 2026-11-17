#ifndef _PLAYERBOT_PARTYMEMBERWITHOUTAURAVALUE_H
#define _PLAYERBOT_PARTYMEMBERWITHOUTAURAVALUE_H

#include "NamedObjectContext.h"
#include "PartyMemberValue.h"
#include "PlayerbotAIConfig.h"

class PlayerbotAI;
class Unit;

class PartyMemberWithoutAuraValue : public PartyMemberValue, public Qualified
{
public:
    PartyMemberWithoutAuraValue(PlayerbotAI* botAI, std::string const name = "party member without aura",
                                float range = sPlayerbotAIConfig->sightDistance)
        : PartyMemberValue(botAI, name)
    {
    }

protected:
    Unit* Calculate() override;
};

#endif
