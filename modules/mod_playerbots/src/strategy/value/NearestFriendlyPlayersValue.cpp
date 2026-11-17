#include "NearestFriendlyPlayersValue.h"

#include "GridObject.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Playerbots.h"

void NearestFriendlyPlayersValue::FindUnits(std::list<Unit*>& targets)
{
    Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(bot, bot, range);
    Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);
}

bool NearestFriendlyPlayersValue::AcceptUnit(Unit* unit)
{
    ObjectGuid guid = unit->GetGUID();
    return guid.IsPlayer() && guid != botAI->GetBot()->GetGUID();
}
