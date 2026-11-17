#include "AutoMaintenanceOnLevelupAction.h"

#include "BotFactory.h"
#include "GuildMgr.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "SharedDefines.h"

bool AutoMaintenanceOnLevelupAction::Execute(Event event)
{
    AutoPickTalents();
    AutoUpgradeEquip();
    AutoTeleportForLevel();
    return true;
}

void AutoMaintenanceOnLevelupAction::AutoTeleportForLevel()
{
    /*if (!sPlayerbotAIConfig->autoTeleportForLevel || !sRandomPlayerbotMgr->IsRandomBot(bot))
    {
        return;
    }
    if (botAI->HasRealPlayerMaster())
    {
        return;
    }
    sRandomPlayerbotMgr->RandomTeleportForLevel(bot);*/
}

void AutoMaintenanceOnLevelupAction::AutoPickTalents()
{
    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return;

    //if (!sPlayerbotAIConfig->autoPickTalents || !sRandomPlayerbotMgr->IsRandomBot(bot))
        //return;

    if ((bot->CalculateTalentsPoints() - bot->GetUsedTalentCount()) <= 0)
        return;

    BotFactory factory(bot, bot->GetLevel());
    factory.InitTalentsTree(false);

    //PlayerbotFactory factory(bot, bot->GetLevel());
    //factory.InitTalentsTree(true, true, true);
    //factory.InitPetTalents();
}

void AutoMaintenanceOnLevelupAction::AutoUpgradeEquip()
{
    if (!sRandomPlayerbotMgr->IsRandomBot(bot)) return;

    BotFactory factory(bot, bot->GetLevel());
    factory.InitEquipment(true);
    factory.InitPet();
    /*if (!sPlayerbotAIConfig->autoUpgradeEquip || !sRandomPlayerbotMgr->IsRandomBot(bot))
    {
        return;
    }
    PlayerbotFactory factory(bot, bot->GetLevel());
    if (!sPlayerbotAIConfig->equipmentPersistence || bot->GetLevel() < sPlayerbotAIConfig->equipmentPersistenceLevel)
    {
        factory.InitEquipment(true);
    }
    factory.InitAmmo();*/
    return;
}

