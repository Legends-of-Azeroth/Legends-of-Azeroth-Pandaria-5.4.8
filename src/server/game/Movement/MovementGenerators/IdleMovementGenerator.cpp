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

#include "IdleMovementGenerator.h"
#include "CreatureAI.h"
#include "Creature.h"
#include <G3D/g3dmath.h>

IdleMovementGenerator si_idleMovement;

bool IdleMovementGenerator::Reset(Unit* owner)
{
    if (!owner->IsStopped())
        owner->StopMoving();

    return true;
}

bool RotateMovementGenerator::Initialize(Unit* owner)
{
    if (!owner->IsStopped())
        owner->StopMoving();

    if (owner->GetVictim())
        owner->SetInFront(owner->GetVictim());

    owner->AddUnitState(UNIT_STATE_ROTATING);
    owner->AttackStop();
    return true;
}

bool RotateMovementGenerator::Update(Unit* owner, uint32 diff)
{
    float angle = owner->GetOrientation();
    angle += (float(diff) * static_cast<float>(M_PI * 2) / m_maxDuration) * (m_direction == ROTATE_DIRECTION_LEFT ? 1.0f : -1.0f);
    angle = G3D::wrap(angle, 0.0f, float(G3D::twoPi()));

    owner->SetOrientation(angle);   // UpdateSplinePosition does not set orientation with UNIT_STATE_ROTATING
    owner->SetFacingTo(angle);      // Send spline movement to clients

    if (m_duration > diff)
        m_duration -= diff;
    else
        return false;

    return true;
}

void RotateMovementGenerator::Finalize(Unit* unit, bool, bool)
{
    unit->ClearUnitState(UNIT_STATE_ROTATING);
    if (unit->GetTypeId() == TYPEID_UNIT)
      unit->ToCreature()->AI()->MovementInform(ROTATE_MOTION_TYPE, 0);
}

bool DistractMovementGenerator::Initialize(Unit* owner)
{
    owner->AddUnitState(UNIT_STATE_DISTRACTED);
    return true;
}

void DistractMovementGenerator::Finalize(Unit* owner, bool, bool)
{
    owner->ClearUnitState(UNIT_STATE_DISTRACTED);
}

bool DistractMovementGenerator::Update(Unit* owner, uint32 time_diff)
{
    if (owner->IsInCombat())
        return false;

    if (time_diff > m_timer)
        return false;

    m_timer -= time_diff;
    return true;
}

void AssistanceDistractMovementGenerator::Finalize(Unit* unit, bool, bool)
{
    unit->ClearUnitState(UNIT_STATE_DISTRACTED);
    unit->ToCreature()->SetReactState(REACT_AGGRESSIVE);
}
