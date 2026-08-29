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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRINITY_MOTIONMASTER_H
#define TRINITY_MOTIONMASTER_H

#include "Common.h"
#include <vector>
#include <functional>
#include "SharedDefines.h"
#include "Object.h"
#include "MoveSplineInitArgs.h"

class MovementGenerator;
class Unit;
class PathGenerator;

namespace Movement
{
    class MoveSplineInit;
}

// Creature Entry ID used for waypoints show, visible only for GMs
#define VISUAL_WAYPOINT 1

// values 0 ... MAX_DB_MOTION_TYPE-1 used in DB
enum MovementGeneratorType : uint8
{
    IDLE_MOTION_TYPE      = 0,                              // IdleMovementGenerator.h
    RANDOM_MOTION_TYPE    = 1,                              // RandomMovementGenerator.h
    WAYPOINT_MOTION_TYPE  = 2,                              // WaypointMovementGenerator.h
    MAX_DB_MOTION_TYPE    = 3,                              // *** this and below motion types can't be set in DB.
    ANIMAL_RANDOM_MOTION_TYPE = MAX_DB_MOTION_TYPE,         // AnimalRandomMovementGenerator.h
    CONFUSED_MOTION_TYPE  = 4,                              // ConfusedMovementGenerator.h
    CHASE_MOTION_TYPE     = 5,                              // TargetedMovementGenerator.h
    HOME_MOTION_TYPE      = 6,                              // HomeMovementGenerator.h
    FLIGHT_MOTION_TYPE    = 7,                              // WaypointMovementGenerator.h
    POINT_MOTION_TYPE     = 8,                              // PointMovementGenerator.h
    FLEEING_MOTION_TYPE   = 9,                              // FleeingMovementGenerator.h
    DISTRACT_MOTION_TYPE  = 10,                             // IdleMovementGenerator.h
    ASSISTANCE_MOTION_TYPE= 11,                             // PointMovementGenerator.h (first part of flee for assistance)
    ASSISTANCE_DISTRACT_MOTION_TYPE = 12,                   // IdleMovementGenerator.h (second part of flee for assistance)
    TIMED_FLEEING_MOTION_TYPE = 13,                         // FleeingMovementGenerator.h (alt.second part of flee for assistance)
    FOLLOW_MOTION_TYPE    = 14,
    ROTATE_MOTION_TYPE    = 15,
    EFFECT_MOTION_TYPE    = 16,
    NULL_MOTION_TYPE      = 17,
    FORMATION_MOTION_TYPE = 18,                             // FormationMovementGenerator.h
    MAX_MOTION_TYPE                                         // SKIP
};

enum MovementSlot : uint8
{
    MOTION_SLOT_IDLE,
    MOTION_SLOT_ACTIVE,
    MOTION_SLOT_CONTROLLED,
    MOTION_SLOT_CRITICAL,
    MAX_MOTION_SLOT
};

enum MMCleanFlag
{
    MMCF_NONE   = 0,
    MMCF_UPDATE = 1, // Clear or Expire called from update
    MMCF_RESET  = 2  // Flag if need top()->Reset()
};

enum RotateDirection
{
    ROTATE_DIRECTION_LEFT,
    ROTATE_DIRECTION_RIGHT
};

// assume it is 25 yard per 0.6 second
#define SPEED_CHARGE    42.0f

inline bool IsInvalidMovementGeneratorType(uint8 const type) { return type == MAX_DB_MOTION_TYPE || type >= MAX_MOTION_TYPE; }
inline bool IsInvalidMovementSlot(uint8 const slot) { return slot >= MAX_MOTION_SLOT; }

class TC_GAME_API MotionMaster //: private std::stack<MovementGenerator *>
{
    private:
        //typedef std::stack<MovementGenerator *> Impl;
        typedef MovementGenerator* _Ty;

        void pop()
        {
            ASSERT(_top < MAX_MOTION_SLOT && _top >= 0);
            Impl[_top] = NULL;
            while (!empty() && !top())
                --_top;
        }
        void push(_Ty _Val)
        {
            ++_top;
            ASSERT(_top < MAX_MOTION_SLOT && _top >= 0);
            Impl[_top] = _Val;
        }

        bool needInitTop() const 
        {
            if (empty())
                return false;
            return _needInit[_top];
        }
        void InitTop();
    public:

        explicit MotionMaster(Unit* unit) : _expList(NULL), _top(-1), _owner(unit), _cleanFlag(MMCF_NONE)
        {
            for (uint8 i = 0; i < MAX_MOTION_SLOT; ++i)
            {
                Impl[i] = NULL;
                _needInit[i] = true;
            }
        }
        ~MotionMaster();

        void Initialize();
        void InitDefault();

        bool empty() const { return (_top < 0); }
        int size() const { return _top + 1; }

        //MovementGeneratorType GetCurrentMovementGeneratorType() const; // hack


        _Ty top() const { ASSERT(_top < MAX_MOTION_SLOT && _top >= 0);  return Impl[_top]; }
        _Ty GetMotionSlot(int slot) const { ASSERT(_top < MAX_MOTION_SLOT && _top >= 0); return Impl[slot]; }

        void DirectDelete(int slot);
        void DirectDelete(_Ty curr);
        void DelayedDelete(_Ty curr);

        void UpdateMotion(uint32 diff);
        void Clear(bool reset = true)
        {
            if (_cleanFlag & MMCF_UPDATE)
            {
                if (reset)
                    _cleanFlag |= MMCF_RESET;
                else
                    _cleanFlag &= ~MMCF_RESET;
                DelayedClean();
            }
            else
                DirectClean(reset);
        }
        void MovementExpired(bool reset = true)
        {
            if (_cleanFlag & MMCF_UPDATE)
            {
                if (reset)
                    _cleanFlag |= MMCF_RESET;
                else
                    _cleanFlag &= ~MMCF_RESET;
                DelayedExpire();
            }
            else
                DirectExpire(reset);
        }

        void MoveIdle();
        void MoveTargetedHome();
        void MoveRandom(float wanderDistance = 0.0f);
        void MoveFollow(Unit* target, float dist, float angle, MovementSlot slot = MOTION_SLOT_ACTIVE);
        void MoveChase(Unit* target, float dist = 0.0f, float angle = 0.0f);
        void MoveConfused();
        void MoveFleeing(Unit* enemy, uint32 time = 0);
        void MovePoint(uint32 id, Position const& pos, bool generatePath = true)
            { MovePoint(id, pos.m_positionX, pos.m_positionY, pos.m_positionZ, generatePath); }
        void MovePoint(uint32 id, float x, float y, float z, bool generatePath = true, MovementSlot slot = MOTION_SLOT_ACTIVE);

        // These two movement types should only be used with creatures having landing/takeoff animations
        void MoveLand(uint32 id, Position const& pos, float speed = 0);
        void MoveTakeoff(uint32 id, Position const& pos, float speed = 0);

        void MoveCharge(float x, float y, float z, float speed = SPEED_CHARGE, uint32 id = EVENT_CHARGE, bool generatePath = false);
        void MoveCharge(PathGenerator const& path);
        void MoveKnockbackFrom(float srcX, float srcY, float speedXY, float speedZ);
        void MoveJumpTo(float angle, float speedXY, float speedZ);
        void MoveJump(Position const& pos, float speedXY, float speedZ, uint32 id = EVENT_JUMP)
            { MoveJump(pos.m_positionX, pos.m_positionY, pos.m_positionZ, speedXY, speedZ, id); };
        void MoveJump(float x, float y, float z, float speedXY, float speedZ, uint32 id = EVENT_JUMP);
        void CustomJump(float x, float y, float z, float speedXY, float speedZ, uint32 id = 0);
        void MoveSmoothPath(uint32 pointId, Position const* pathPoints, size_t pathSize, bool walk = false, bool fly = false);
        void MoveCirclePath(float x, float y, float z, float radius, bool clockwise, uint8 stepCount, float velocityOrDuration = 0.0f);
        void MoveFall(uint32 id = 0);

        void MoveSeekAssistance(float x, float y, float z);
        void MoveSeekAssistanceDistract(uint32 timer);
        void MoveTaxiFlight(uint32 path, uint32 pathnode);
        void MoveDistract(uint32 time);
        void MovePath(uint32 path_id, bool repeatable);
        void MoveSplinePath(const Position* path, uint32 count, bool fly = false, bool walk = false, float speed = 0.0f, bool cyclic = false, bool catmullrom = true, bool uncompressed = true);
        void MoveRotate(uint32 time, RotateDirection direction);

        void MoveKnockbackFromForPlayer(float srcX, float srcY, float speedXY, float speedZ);
        void MovePointBackwards(uint32 id, float x, float y, float z, bool generatePath = true, bool forceDestination = true, MovementSlot slot = MOTION_SLOT_ACTIVE, float orientation = 0.0f);

        void LaunchMoveSpline(Movement::MoveSplineInit&& init, uint32 id = 0, MovementSlot slot = MOTION_SLOT_ACTIVE, MovementGeneratorType type = EFFECT_MOTION_TYPE);
        void LaunchMoveSpline(std::function<void(Movement::MoveSplineInit&)>&& initializer, uint32 id = 0, MovementSlot slot = MOTION_SLOT_ACTIVE, MovementGeneratorType type = EFFECT_MOTION_TYPE);

        MovementGeneratorType GetCurrentMovementGeneratorType() const;
        MovementGeneratorType GetMotionSlotType(int slot) const;

        void propagateSpeedChange();

        bool GetDestination(float &x, float &y, float &z);
    private:
        void Mutate(MovementGenerator *m, MovementSlot slot);                  // use Move* functions instead

        void DirectClean(bool reset);
        void DelayedClean();

        void DirectExpire(bool reset);
        void DelayedExpire();

        typedef std::vector<_Ty> ExpireList;
        ExpireList* _expList;
        _Ty Impl[MAX_MOTION_SLOT];
        int _top;
        Unit* _owner;
        bool _needInit[MAX_MOTION_SLOT];
        uint8 _cleanFlag;
};
#endif
