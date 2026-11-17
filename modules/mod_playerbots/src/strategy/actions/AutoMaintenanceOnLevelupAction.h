#ifndef _PLAYERBOT_AUTOTELEPORTFORLEVELACTION_H
#define _PLAYERBOT_AUTOTELEPORTFORLEVELACTION_H

#include "Action.h"

class PlayerbotAI;

class AutoMaintenanceOnLevelupAction : public Action
{
public:
    AutoMaintenanceOnLevelupAction(PlayerbotAI* botAI, std::string const name = "auto maintenance on levelup")
        : Action(botAI, name)
    {
    }

    bool Execute(Event event);

protected:
    void AutoTeleportForLevel();
    void AutoPickTalents();
    void AutoUpgradeEquip();
};

#endif
