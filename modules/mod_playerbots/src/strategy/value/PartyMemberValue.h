#ifndef _PLAYERBOT_PARTYMEMBERVALUE_H
#define _PLAYERBOT_PARTYMEMBERVALUE_H

#include "Player.h"
#include "Value.h"

class PlayerbotAI;

class FindPlayerPredicate
{
public:
    virtual ~FindPlayerPredicate() = default;
    virtual bool Check(Unit* /*unit*/) = 0;
};

class SpellEntryPredicate
{
public:
    virtual bool Check(SpellInfo const* /*spellInfo*/) = 0;
};

class PartyMemberValue : public UnitCalculatedValue
{
public:
    PartyMemberValue(PlayerbotAI* botAI, std::string const name = "party member", int checkInterval = 1)
        : UnitCalculatedValue(botAI, name, checkInterval)
    {
    }

    bool IsTargetOfSpellCast(Player* target, SpellEntryPredicate& predicate);

protected:
    Unit* FindPartyMember(FindPlayerPredicate& predicate, bool ignoreOutOfGroup = false);
    Unit* FindPartyMember(std::vector<Player*>* party, FindPlayerPredicate& predicate);
    virtual bool Check(Unit* player);
};

class PartyMemberMainTankValue : public PartyMemberValue
{
public:
    PartyMemberMainTankValue(PlayerbotAI* botAI) : PartyMemberValue(botAI, "main tank member", 2 * 1000) {}
    virtual Unit* Calculate();
};

#endif
