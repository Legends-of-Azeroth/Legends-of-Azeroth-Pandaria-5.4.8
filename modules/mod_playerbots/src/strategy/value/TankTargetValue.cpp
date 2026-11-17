#include "TankTargetValue.h"

#include "AttackersValue.h"
#include "ThreatManager.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"

class FindTargetForTankStrategy : public FindNonCcTargetStrategy
{
public:
    FindTargetForTankStrategy(PlayerbotAI* botAI) : FindNonCcTargetStrategy(botAI), minThreat(0) {}

    void CheckAttacker(Unit* creature, ThreatManager* threatMgr) override
    {
        if (!creature || !creature->IsAlive())
        {
            return;
        }
        Player* bot = botAI->GetBot();
        float threat = threatMgr->GetThreat(bot);
        if (!result)
        {
            minThreat = threat;
            result = creature;
        }
        // neglect if victim is main tank, or no victim (for untauntable target)
        if (threatMgr->GetCurrentVictim())
        {
            // float max_threat = threatMgr->GetThreat(threatMgr->GetCurrentVictim()->GetVictim());
            Unit* victim = threatMgr->GetCurrentVictim()->GetVictim();
            if (victim && victim->ToPlayer() && PlayerBotSpec::IsMainTank(victim->ToPlayer()))
            {
                return;
            }
        }
        if (minThreat >= threat)
        {
            minThreat = threat;
            result = creature;
        }
    }

protected:
    float minThreat;
};

class FindTankTargetSmartStrategy : public FindTargetStrategy
{
public:
    FindTankTargetSmartStrategy(PlayerbotAI* botAI) : FindTargetStrategy(botAI) {}

    void CheckAttacker(Unit* attacker, ThreatManager* threatMgr) override
    {
        if (Group* group = botAI->GetBot()->GetGroup())
        {
            ObjectGuid guid = group->GetTargetIcon(4);
            if (guid && attacker->GetGUID() == guid)
                return;
        }
        if (!attacker->IsAlive())
        {
            return;
        }
        if (!result || IsBetter(attacker, result))
        {
            result = attacker;
        }
    }
    bool IsBetter(Unit* new_unit, Unit* old_unit)
    {
        Player* bot = botAI->GetBot();
        // if group has multiple tanks, main tank just focus on the current target
        Unit* currentTarget = botAI->GetAiObjectContext()->GetValue<Unit*>("current target")->Get();
        if (currentTarget && PlayerBotSpec::IsMainTank(bot) && PlayerBotSpec::GetGroupTankNum(bot) > 1)
        {
            if (old_unit == currentTarget)
                return false;
            if (new_unit == currentTarget)
                return true;
        }
        float new_threat = new_unit->GetThreatManager().GetThreat(bot);
        float old_threat = old_unit->GetThreatManager().GetThreat(bot);
        float new_dis = bot->GetDistance(new_unit);
        float old_dis = bot->GetDistance(old_unit);
        // hasAggro? -> withinMelee? -> threat
        if (GetIntervalLevel(new_unit) != GetIntervalLevel(old_unit))
        {
            return GetIntervalLevel(new_unit) > GetIntervalLevel(old_unit);;
        }
        int32_t interval = GetIntervalLevel(new_unit);
        if (interval == 2)
        {
            return new_dis < old_dis;
        }
        return new_threat < old_threat;
    }
    int32_t GetIntervalLevel(Unit* unit)
    {
        if (!botAI->HasAggro(unit))
        {
            return 2;
        }
        if (botAI->GetBot()->IsWithinMeleeRange(unit))
        {
            return 1;
        }
        return 0;
    }
};

Unit* TankTargetValue::Calculate()
{
    // FindTargetForTankStrategy strategy(botAI);
    FindTankTargetSmartStrategy strategy(botAI);
    return FindTarget(&strategy);
}
