#ifndef _PLAYERBOT_LASTSPELLCASTTIMEVALUE_H
#define _PLAYERBOT_LASTSPELLCASTTIMEVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class LastSpellCastTimeValue : public ManualSetValue<time_t>, public Qualified
{
public:
    LastSpellCastTimeValue(PlayerbotAI* botAI) : ManualSetValue<time_t>(botAI, 0), Qualified() {}
};

#endif
