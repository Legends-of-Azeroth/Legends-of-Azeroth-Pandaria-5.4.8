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

#ifndef TRINITY_RANDOMMOTIONGENERATOR_H
#define TRINITY_RANDOMMOTIONGENERATOR_H

#include "MovementGenerator.h"

template<class T>
class RandomMovementGenerator : public MovementGeneratorMedium< T, RandomMovementGenerator<T> >
{
    public:
        RandomMovementGenerator(float wanderDistance = 0.0f) : i_nextMoveTime(0), wander_distance(wanderDistance) { }

        void _setRandomLocation(T*);
        bool DoInitialize(T*);
        void DoFinalize(T*, bool, bool);
        bool DoReset(T*);
        bool DoUpdate(T*, const uint32);
        void DoDeactivate(T*) { }
        bool GetResetPosition(T*, float& x, float& y, float& z);
        MovementGeneratorType GetMovementGeneratorType() const { return RANDOM_MOTION_TYPE; }
    private:
        TimeTrackerSmall i_nextMoveTime;

        uint32 i_nextMove;
        float wander_distance;
};
#endif
