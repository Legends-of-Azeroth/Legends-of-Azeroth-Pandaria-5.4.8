#ifndef _PLAYERBOT_SPELLCASTUSEFULVALUE_H
#define _PLAYERBOT_SPELLCASTUSEFULVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class SpellCastUsefulValue : public BoolCalculatedValue, public Qualified
{
public:
    SpellCastUsefulValue(PlayerbotAI* botAI, std::string const name = "spell cast useful")
        : BoolCalculatedValue(botAI, name)
    {
    }

    bool Calculate() override;
};

#endif
