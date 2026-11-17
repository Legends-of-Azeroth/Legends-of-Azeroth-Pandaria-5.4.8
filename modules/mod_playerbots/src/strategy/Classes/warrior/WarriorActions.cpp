#include "WarriorActions.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"

bool CastSunderArmorAction::isUseful()
{
    Aura* aura = botAI->GetAura("sunder armor", GetTarget(), false, true);
    return !aura || aura->GetStackAmount() < 3 || aura->GetDuration() <= 6000;
}

bool CastHeroicLeapAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return sServerFacade->IsDistanceGreaterOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 15.f);
}

bool CastBattleStanceAction::isUseful()
{
    return (botAI->GetBot() && botAI->GetBot()->GetGroup() && !PlayerBotSpec::IsTank(botAI->GetBot()));
}
bool CastBerserkerStanceAction::isUseful()
{
    return (botAI->GetBot() && !botAI->GetBot()->GetGroup() && !PlayerBotSpec::IsTank(botAI->GetBot()));
}
bool CastDefensiveStanceAction::isUseful()
{
    return PlayerBotSpec::IsTank(botAI->GetBot());
}

Unit* CastVigilanceAction::GetTarget()
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return nullptr;
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
        uint32 curLife = bot->GetHealth();
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

    // If no valid target, return nullptr
    if (!highestPriorityTarget)
    {
        return nullptr;
    }

    // If the current target is already the highest-priority target, do nothing
    if (currentVigilanceTarget == highestPriorityTarget)
    {
        return nullptr;
    }

    // Assign the new target
    Unit* targetUnit = highestPriorityTarget->ToUnit();
    if (targetUnit)
    {
        return targetUnit;
    }

    return nullptr;
}

bool CastVigilanceAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target || target == bot)
        return false;

    return botAI->CastSpell("vigilance", target);
}

bool CastMockingBannerAction::isUseful()
{
    return !botAI->HasAura(spell, bot);
}