#include "LastSpellCastValue.h"

#include "Playerbots.h"

LastSpellCast::LastSpellCast() : id(0), timer(0) {}

void LastSpellCast::Set(uint32 newId, ObjectGuid newTarget, time_t newTime)
{
    id = newId;
    target = newTarget;
    timer = newTime;
}

void LastSpellCast::Reset()
{
    id = 0;
    target.Clear();
    timer = 0;
}
