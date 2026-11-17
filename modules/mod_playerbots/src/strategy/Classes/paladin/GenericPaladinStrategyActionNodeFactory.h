#ifndef _PLAYERBOT_GENERICPALADINSTRATEGYACTIONNODEFACTORY_H
#define _PLAYERBOT_GENERICPALADINSTRATEGYACTIONNODEFACTORY_H

#include "Action.h"
#include "NamedObjectContext.h"

class PlayerbotAI;

/*
* creators["consecration"] = &consecration;
* ACTION_NODE(consecration, "consecration");
*/
class GenericPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericPaladinStrategyActionNodeFactory()
    {
        // -- buffs
        creators["seal of righteousness"] = &seal_of_righteousness;
        creators["seal of command"] = &seal_of_command;
        creators["seal of insight"] = &seal_of_insight;

        creators["blessing of kings"] = &blessing_of_kings;
        creators["blessing of might"] = &blessing_of_might;

        creators["righteous fury"] = &righteous_fury;

        // -- heals
        creators["word of glory"] = &word_of_glory;
        creators["flash of light"] = &flash_of_light;
        creators["lay on hands"] = &lay_on_hands;
        creators["redemption"] = &redemption;
        creators["cleanse"] = &cleanse;

        // -- cds
        creators["divine protection"] = &divine_protection;
        creators["hand of protection"] = &hand_of_protection;
        creators["hand of freedom"] = &hand_of_freedom;
        creators["devotion aura"] = &devotion_aura;
        creators["hand of salvation"] = &hand_of_salvation;
        creators["avenging wrath"] = &avenging_wrath;
        creators["hand of sacrifice"] = &hand_of_sacrifice;
        creators["blinding light"] = &blinding_light;

        // -- damages
        creators["hammer of wrath"] = &hammer_of_wrath;
        creators["rebuke"] = &rebuke;
        creators["reckoning"] = &reckoning;
        creators["crusader strike"] = &crusader_strike;
        creators["judgment"] = &judgment;
        creators["hammer of justice"] = &hammer_of_justice;
    }

private:
    // -- buffs
    ACTION_NODE(seal_of_righteousness, "seal of righteousness");
    ACTION_NODE(seal_of_command, "seal of command");
    ACTION_NODE(seal_of_insight, "seal of insight");

    ACTION_NODE(blessing_of_kings, "blessing of kings");
    ACTION_NODE(blessing_of_might, "blessing of might");

    ACTION_NODE(righteous_fury, "righteous fury");

    // -- heals
    ACTION_NODE_A(word_of_glory, "word of glory", "flash of light");
    ACTION_NODE(flash_of_light, "flash of light");
    ACTION_NODE(lay_on_hands, "lay on hands");
    ACTION_NODE(redemption, "redemption");
    ACTION_NODE(cleanse, "cleanse");

    // -- cds
    ACTION_NODE(divine_protection, "divine protection");
    ACTION_NODE(hand_of_protection, "hand of protection");
    ACTION_NODE(hand_of_freedom, "hand of freedom");
    ACTION_NODE(devotion_aura, "devotion aura");
    ACTION_NODE(hand_of_salvation, "hand of salvation");
    ACTION_NODE(avenging_wrath, "avenging wrath");
    ACTION_NODE(hand_of_sacrifice, "hand of sacrifice");
    ACTION_NODE(blinding_light, "blinding light");

    // -- damages
    ACTION_NODE(hammer_of_wrath, "hammer of wrath");
    ACTION_NODE(rebuke, "rebuke");
    ACTION_NODE(reckoning, "reckoning");
    ACTION_NODE(crusader_strike, "crusader strike");
    ACTION_NODE(judgment, "judgment");
    ACTION_NODE(hammer_of_justice, "hammer of justice");
};

#endif
