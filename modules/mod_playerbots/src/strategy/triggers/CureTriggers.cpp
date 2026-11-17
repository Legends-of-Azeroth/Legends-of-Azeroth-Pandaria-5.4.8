#include "CureTriggers.h"

#include "Playerbots.h"

bool NeedCureTrigger::IsActive()
{
    Unit* target = GetTarget();
    return target && target->IsInWorld() && botAI->HasAuraToDispel(target, dispelType);
}

Value<Unit*>* PartyMemberNeedCureTrigger::GetTargetValue()
{
    return context->GetValue<Unit*>("party member to dispel", dispelType);
}

bool PartyMemberNeedCureTrigger::IsActive()
{
    Unit* target = GetTarget();
    return target && target->IsInWorld();
}

//bool NeedWorldBuffTrigger::IsActive() { return !WorldBuffAction::NeedWorldBuffs(bot).empty(); }
