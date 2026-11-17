#include "HasTotemValue.h"

#include "Playerbots.h"
#include "Helper.h"

#define MAX_SUMMON_SLOT 7
bool HasTotemValue::Calculate()
{
    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
    {
        if (!bot->m_SummonSlot[i])
        {
            continue;
        }

        if (Creature* OldTotem = bot->GetMap()->GetCreature(bot->m_SummonSlot[i]))
        {
            if (OldTotem->IsSummon() && OldTotem->GetDistance(bot) <= 30.0f)
            {
                if (caseInsensitiveEqual(OldTotem->GetName().c_str(), qualifier.c_str()))
                    return true;
            }
        }
    }
    return false;
}
