#ifndef _PLAYERBOT_LASTSPELLCASTVALUE_H
#define _PLAYERBOT_LASTSPELLCASTVALUE_H

#include "Value.h"

class PlayerbotAI;

class LastSpellCast
{
public:
    LastSpellCast();

    void Set(uint32 id, ObjectGuid target, time_t time);
    void Reset();

    uint32 id;
    ObjectGuid target;
    time_t timer;
};

class LastSpellCastValue : public ManualSetValue<LastSpellCast&>
{
public:
    LastSpellCastValue(PlayerbotAI* botAI, std::string const name = "last spell cast")
        : ManualSetValue<LastSpellCast&>(botAI, data, name)
    {
    }

private:
    LastSpellCast data;
};

#endif
