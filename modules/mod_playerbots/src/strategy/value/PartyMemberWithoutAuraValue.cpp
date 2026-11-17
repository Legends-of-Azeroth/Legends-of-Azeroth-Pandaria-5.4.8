#include "PartyMemberWithoutAuraValue.h"

#include "Playerbots.h"
#include "Helper.h"

class PlayerWithoutAuraPredicate : public FindPlayerPredicate, public PlayerbotAIAware
{
public:
    PlayerWithoutAuraPredicate(PlayerbotAI* botAI, std::string const aura)
        : PlayerbotAIAware(botAI), FindPlayerPredicate(), auras(split(aura, ','))
    {
    }

public:
    bool Check(Unit* unit) override
    {
        if (!unit->IsAlive())
            return false;

        for (std::vector<std::string>::iterator i = auras.begin(); i != auras.end(); ++i)
        {
            if (botAI->HasAura(*i, unit))
                return false;
        }

        return true;
    }

private:
    std::vector<std::string> auras;
};

Unit* PartyMemberWithoutAuraValue::Calculate()
{
    PlayerWithoutAuraPredicate predicate(botAI, qualifier);
    return FindPartyMember(predicate);
}
