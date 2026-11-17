/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "ChooseTargetActions.h"

#include "Event.h"
#include "NewRpgStrategy.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool AttackEnemyPlayerAction::isUseful()
{
    //if (PlayerHasFlag::IsCapturingFlag(bot))
        //return false;

    return !sPlayerbotAIConfig->IsPvpProhibited(bot->GetZoneId(), bot->GetAreaId());
}

bool AttackEnemyFlagCarrierAction::isUseful()
{
    //Unit* target = context->GetValue<Unit*>("enemy flag carrier")->Get();
    //return target && sServerFacade->IsDistanceLessOrEqualThan(sServerFacade->GetDistance2d(bot, target), 75.0f) && PlayerHasFlag::IsCapturingFlag(bot);

    return false;
}

bool AttackAnythingAction::isUseful()
{
    if (!botAI->AllowActivity(GRIND_ACTIVITY))  // Bot not allowed to be active
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    Unit* target = GetTarget();

    if (!target)
        return false;

    const NewRpgStatus &status = botAI->GetRpgInfo().status;
    bool inactiveGrindStatus = status == NewRpgStatus::GO_GRIND || status == NewRpgStatus::NEAR_NPC || status == NewRpgStatus::REST || status == NewRpgStatus::GO_INNKEEPER;

    if (inactiveGrindStatus && bot->GetDistance(target) > 25.0f)
        return false;

    std::string const name = std::string(target->GetName());
    // Check for invalid targets: Dummy, Charge Target, Melee Target, Ranged Target
    if (!name.empty() &&
        (name.find("Dummy") != std::string::npos ||
            name.find("Charge Target") != std::string::npos ||
            name.find("Melee Target") != std::string::npos ||
            name.find("Ranged Target") != std::string::npos))
    {
        return false;  // Target is one of the disallowed types
    }

    return true;
}

bool DropTargetAction::Execute(Event event)
{
    Unit* target = context->GetValue<Unit*>("current target")->Get();
    if (target && target->isDead())
    {
        ObjectGuid guid = target->GetGUID();
        //if (guid)
            //context->GetValue<LootObjectStack*>("available loot")->Get()->Add(guid);
    }

    context->GetValue<Unit*>("current target")->Set(nullptr);

    bot->SetTarget(ObjectGuid::Empty);
    bot->SetSelection(ObjectGuid());
    botAI->ChangeEngine(BOT_STATE_NON_COMBAT);
    bot->AttackStop();

    return true;
}

bool AttackAnythingAction::Execute(Event event)
{
    bool result = AttackAction::Execute(event);
    if (result)
    {
        if (Unit* grindTarget = GetTarget())
        {
            if (char const* grindName = grindTarget->GetName().c_str())
            {
                context->GetValue<ObjectGuid>("pull target")->Set(grindTarget->GetGUID());
                bot->GetMotionMaster()->Clear();
            }
        }
    }

    return result;
}

bool AttackAnythingAction::isPossible()
{
    return AttackAction::isPossible() && GetTarget();
}

bool DpsAssistAction::isUseful()
{
    //if (PlayerHasFlag::IsCapturingFlag(bot))
        //return false;

    return true;
}

bool AttackRtiTargetAction::Execute(Event event)
{
    Unit* rtiTarget = AI_VALUE(Unit*, "rti target");

    if (rtiTarget && rtiTarget->IsInWorld() && rtiTarget->GetMapId() == bot->GetMapId())
    {
        botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ rtiTarget->GetGUID() });
        bool result = Attack(botAI->GetUnit(rtiTarget->GetGUID()));
        if (result)
        {
            context->GetValue<ObjectGuid>("pull target")->Set(rtiTarget->GetGUID());
            return true;
        }
    }
    else
    {
        botAI->TellError("I dont see my rti attack target");
    }

    return false;
}

bool AttackRtiTargetAction::isUseful()
{
    if (botAI->ContainsStrategy(STRATEGY_TYPE_HEAL))
        return false;

    return true;
}
