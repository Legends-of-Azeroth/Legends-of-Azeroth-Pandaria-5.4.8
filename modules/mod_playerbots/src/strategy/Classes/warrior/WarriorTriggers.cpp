#include "WarriorTriggers.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"

bool ShieldBarrierTrigger::IsActive()
{
    return BuffTrigger::IsActive() && AI_VALUE2(uint8, "rage", "self target") >= 40;
}

bool VigilanceTrigger::IsActive()
{
    if (!bot->HasSpell(114030))
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    Player* currentVigilanceTarget = nullptr;
    Player* mainTank = nullptr;
    Player* assistTank1 = nullptr;
    Player* assistTank2 = nullptr;
    Player* lowestLifePlayer = nullptr;
    uint32 lowestLife = 0;

    // Iterate once through the group to gather all necessary information
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsAlive())
            continue;

        // Check if member has Vigilance applied by the bot
        if (!currentVigilanceTarget && botAI->HasAura("vigilance", member, false, true))
        {
            currentVigilanceTarget = member;
        }

        // Identify Main Tank
        if (!mainTank && PlayerBotSpec::IsMainTank(member))
        {
            mainTank = member;
        }

        // Identify Assist Tanks
        if (assistTank1 == nullptr && PlayerBotSpec::IsAssistTankOfIndex(bot, member, 0))
        {
            assistTank1 = member;
        }
        else if (assistTank2 == nullptr && PlayerBotSpec::IsAssistTankOfIndex(bot, member, 1))
        {
            assistTank2 = member;
        }

        // Determine Highest Gear Score
        uint32 curLife = botAI->GetBot()->GetHealth();
        if (curLife > lowestLife)
        {
            lowestLife = curLife;
            lowestLifePlayer = member;
        }
    }

    // Determine the highest-priority target
    Player* highestPriorityTarget = mainTank ? mainTank :
        (assistTank1 ? assistTank1 :
            (assistTank2 ? assistTank2 : lowestLifePlayer));

    // Trigger if no Vigilance is active or the current target is not the highest-priority target
    if (!currentVigilanceTarget || currentVigilanceTarget != highestPriorityTarget)
    {
        return true;
    }

    return false; // No need to reassign Vigilance
}