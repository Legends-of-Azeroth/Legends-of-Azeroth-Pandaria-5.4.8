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

#ifndef TRINITY_CONFUSEDGENERATOR_H
#define TRINITY_CONFUSEDGENERATOR_H

#include "MovementGenerator.h"
#include "Timer.h"

template<class T>
class ConfusedMovementGenerator : public MovementGeneratorMedium< T, ConfusedMovementGenerator<T> >
{
    public:
        explicit ConfusedMovementGenerator() : m_nextMoveTime(0) { }

        bool DoInitialize(T*);
        void DoFinalize(T*, bool, bool);
        bool DoReset(T*);
        bool DoUpdate(T*, uint32);
        void DoDeactivate(T*) { }

        MovementGeneratorType GetMovementGeneratorType() const { return CONFUSED_MOTION_TYPE; }
    private:
        void SetDestination(Unit* unit);
    private:
        TimeTracker m_nextMoveTime;
        float m_x, m_y, m_z;
};
#endif
