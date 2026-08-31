/*
 * This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "StringFormat.h"

MovementGenerator::~MovementGenerator() { }

std::string MovementGenerator::GetDebugInfo() const
{
    return Trinity::StringFormat("Priority: %u, Flags: %u, BaseUnitState: %u", Priority, Flags, BaseUnitState);
}

IdleMovementFactory::IdleMovementFactory() : MovementGeneratorCreator(IDLE_MOTION_TYPE)
{
    RegisterSelf();
}

MovementGenerator* IdleMovementFactory::Create(void* /*object*/) const
{
    return &si_idleMovement;
}

RandomMovementFactory::RandomMovementFactory() : MovementGeneratorCreator(RANDOM_MOTION_TYPE)
{
    RegisterSelf();
}

MovementGenerator* RandomMovementFactory::Create(void* /*object*/) const
{
    return new RandomMovementGenerator<Creature>();
}

WaypointMovementFactory::WaypointMovementFactory() : MovementGeneratorCreator(WAYPOINT_MOTION_TYPE)
{
    RegisterSelf();
}

MovementGenerator* WaypointMovementFactory::Create(void* /*object*/) const
{
    return new WaypointMovementGenerator<Creature>();
}

static bool RegisterMovementGeneratorFactories()
{
    static IdleMovementFactory idleMovementFactory;
    static RandomMovementFactory randomMovementFactory;
    static WaypointMovementFactory waypointMovementFactory;
    return true;
}

static bool _movementGeneratorFactoriesRegistered = RegisterMovementGeneratorFactories();
