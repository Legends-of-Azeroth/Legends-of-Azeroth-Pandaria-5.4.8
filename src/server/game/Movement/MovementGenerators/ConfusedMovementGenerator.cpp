/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Creature.h"
#include "MapManager.h"
#include "ConfusedMovementGenerator.h"
#include "PathGenerator.h"
#include "VMapFactory.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"

#ifdef MAP_BASED_RAND_GEN
#define rand_norm() unit.rand_norm()
#define urand(a, b) unit.urand(a, b)
#endif

template<class T>
bool ConfusedMovementGenerator<T>::DoInitialize(T* unit)
{
    unit->AddUnitState(UNIT_STATE_CONFUSED);
    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->GetPosition(m_x, m_y, m_z);

    if (!unit->IsAlive())
        return true;

    unit->StopMoving();
    unit->AddUnitState(UNIT_STATE_CONFUSED_MOVE);

    if (unit->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return true;

    this->SetDestination(unit);
    return true;
}

template<class T>
bool ConfusedMovementGenerator<T>::DoReset(T* unit)
{
    m_nextMoveTime.Reset(0);

    if (!unit->IsAlive())
        return false;

    unit->StopMoving();
    unit->AddUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    return true;
}

template<class T>
bool ConfusedMovementGenerator<T>::DoUpdate(T* unit, uint32 diff)
{
    if (unit->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return true;

    // waiting for next move
    m_nextMoveTime.Update(diff);
    if (m_nextMoveTime.Passed())
    {
        m_nextMoveTime.Reset(700);
        this->SetDestination(unit);
    }

    return true;
}

template<class T>
void ConfusedMovementGenerator<T>::SetDestination(Unit* unit)
{
    // start moving
    unit->AddUnitState(UNIT_STATE_CONFUSED_MOVE);

    float dest = 4.0f * (float)rand_norm() - 2.0f;

    Position pos;
    pos.Relocate(m_x, m_y, m_z);
    unit->MovePositionToFirstCollision(pos, dest, 0.0f);

    PathGenerator path(unit);
    path.SetPathLengthLimit(30.0f);
    bool result = path.CalculatePath(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    if (!result || (path.GetPathType() & PATHFIND_NOPATH))
    {
        m_nextMoveTime.Reset(100);
        return;
    }

    Movement::MoveSplineInit init(unit);
    init.MovebyPath(path.GetPath());
    init.SetWalk(true);
    init.Launch();
}

template<>
void ConfusedMovementGenerator<Player>::DoFinalize(Player* unit, bool, bool)
{
    if (unit->HasAuraType(SPELL_AURA_MOD_CONFUSE))
        return;

    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    unit->StopMoving();
}

template<>
void ConfusedMovementGenerator<Creature>::DoFinalize(Creature* unit, bool, bool)
{
    if (unit->HasAuraType(SPELL_AURA_MOD_CONFUSE))
        return;

    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    if (unit->GetVictim())
        unit->SetTarget(unit->GetVictim()->GetGUID());
}

template bool ConfusedMovementGenerator<Player>::DoInitialize(Player*);
template bool ConfusedMovementGenerator<Creature>::DoInitialize(Creature*);
template bool ConfusedMovementGenerator<Player>::DoReset(Player*);
template bool ConfusedMovementGenerator<Creature>::DoReset(Creature*);
template bool ConfusedMovementGenerator<Player>::DoUpdate(Player*, uint32 diff);
template bool ConfusedMovementGenerator<Creature>::DoUpdate(Creature*, uint32 diff);
