#ifndef _PLAYERBOT_SAVEMANAACTION_H
#define _PLAYERBOT_SAVEMANAACTION_H

#include "Action.h"

class PlayerbotAI;

class SaveManaAction : public Action
{
public:
    SaveManaAction(PlayerbotAI* botAI) : Action(botAI, "save mana") {}

    bool Execute(Event event) override;

private:
    std::string const Format(double value);
};

#endif
