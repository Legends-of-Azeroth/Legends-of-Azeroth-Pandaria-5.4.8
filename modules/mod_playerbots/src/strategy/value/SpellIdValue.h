#ifndef _PLAYERBOT_SPELLIDVALUE_H
#define _PLAYERBOT_SPELLIDVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class SpellIdValue : public CalculatedValue<uint32>, public Qualified
{
public:
    SpellIdValue(PlayerbotAI* botAI);

    uint32 Calculate() override;
};

class VehicleSpellIdValue : public CalculatedValue<uint32>, public Qualified
{
public:
    VehicleSpellIdValue(PlayerbotAI* botAI);

    uint32 Calculate() override;
};

#endif
