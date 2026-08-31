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
#include "MovementDefines.h"
#include "MovementGenerator.h"
#include "Object.h"
#include "MoveSplineInitArgs.h"
#include <deque>
#include <functional>
#include <set>
#include <unordered_map>
#include <vector>

class MovementGenerator;
class Unit;
class PathGenerator;
class WorldObject;

namespace Movement
{
    class MoveSplineInit;
}

// Creature Entry ID used for waypoints show, visible only for GMs
#define VISUAL_WAYPOINT 1

enum MotionMasterFlags : uint8
{
    MOTIONMASTER_FLAG_NONE                          = 0x0,
    MOTIONMASTER_FLAG_UPDATE                        = 0x1,
    MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING = 0x2,
    MOTIONMASTER_FLAG_INITIALIZATION_PENDING        = 0x4,
    MOTIONMASTER_FLAG_INITIALIZING                  = 0x8,

    MOTIONMASTER_FLAG_DELAYED = MOTIONMASTER_FLAG_UPDATE | MOTIONMASTER_FLAG_INITIALIZATION_PENDING
};

enum MotionMasterDelayedActionType : uint8
{
    MOTIONMASTER_DELAYED_CLEAR = 0,
    MOTIONMASTER_DELAYED_CLEAR_SLOT,
    MOTIONMASTER_DELAYED_CLEAR_PRIORITY,
    MOTIONMASTER_DELAYED_CLEAR_ACTIVE,
    MOTIONMASTER_DELAYED_ADD,
    MOTIONMASTER_DELAYED_REMOVE,
    MOTIONMASTER_DELAYED_REMOVE_TYPE,
    MOTIONMASTER_DELAYED_EXPIRE
};

struct MovementGeneratorDeleter
{
    void operator()(MovementGenerator* a);
};

struct MovementGeneratorComparator
{
    public:
        bool operator()(MovementGenerator const* a, MovementGenerator const* b) const;
};

struct MovementGeneratorInformation
{
    MovementGeneratorInformation(MovementGeneratorType type, ObjectGuid targetGUID, std::string const& targetName);

    MovementGeneratorType Type;
    ObjectGuid TargetGUID;
    std::string TargetName;
};

class TC_GAME_API MotionMaster
{
    public:
        typedef MovementGenerator* _Ty;
        typedef std::function<void()> DelayedActionDefine;
        typedef std::function<bool()> DelayedActionValidator;

        class DelayedAction
        {
            public:
                explicit DelayedAction(DelayedActionDefine&& action, DelayedActionValidator&& validator, MotionMasterDelayedActionType type) : Action(std::move(action)), Validator(std::move(validator)), Type(type) { }
                explicit DelayedAction(DelayedActionDefine&& action, MotionMasterDelayedActionType type) : Action(std::move(action)), Validator(sEmptyValidator), Type(type) { }

                void Resolve() { if (Validator()) Action(); }

                DelayedActionDefine Action;
                DelayedActionValidator Validator;
                uint8 Type;

            private:
                static bool sEmptyValidator() { return true; }
        };

        explicit MotionMaster(Unit* unit);
        ~MotionMaster();

        void Initialize();
        void InitDefault();
        void InitializeDefault();

        bool empty() const { return Empty(); }
        bool Empty() const;
        uint32 Size() const;
        int size() const { return int(Size()); }
        std::vector<MovementGeneratorInformation> GetMovementGeneratorsInformation() const;

        MovementGenerator* top() const { return GetCurrentMovementGenerator(); }
        MovementSlot GetCurrentSlot() const;
        MovementGenerator* GetCurrentMovementGenerator() const;
        MovementGeneratorType GetCurrentMovementGeneratorType() const;
        MovementGeneratorType GetCurrentMovementGeneratorType(MovementSlot slot) const;
        MovementGenerator* GetCurrentMovementGenerator(MovementSlot slot) const;
        MovementGenerator* GetMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot = MOTION_SLOT_ACTIVE) const;
        bool HasMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot = MOTION_SLOT_ACTIVE) const;

        void Update(uint32 diff);
        void UpdateMotion(uint32 diff) { Update(diff); }
        void Add(MovementGenerator* movement, MovementSlot slot = MOTION_SLOT_ACTIVE);
        void Remove(MovementGenerator* movement, MovementSlot slot = MOTION_SLOT_ACTIVE);
        void Remove(MovementGeneratorType type, MovementSlot slot = MOTION_SLOT_ACTIVE);
        void Clear(bool reset = true);
        void Clear(MovementSlot slot);
        void Clear(MovementGeneratorPriority priority);
        void MovementExpired(bool reset = true);

        void PropagateSpeedChange();
        void propagateSpeedChange();
        bool GetDestination(float &x, float &y, float &z);
        bool StopOnDeath();
        void InterruptOnTeleport();

        void DirectDelete(int slot);
        void DirectDelete(MovementGenerator* curr);

        _Ty GetMotionSlot(MovementSlot slot) const;
        _Ty GetMotionSlot(int slot) const;
        MovementGeneratorType GetMotionSlotType(int slot) const;

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

    private:
        typedef std::unique_ptr<MovementGenerator, MovementGeneratorDeleter> MovementGeneratorPointer;
        typedef std::set<MovementGenerator*, MovementGeneratorComparator> MotionMasterContainer;
        typedef std::unordered_multimap<uint32, MovementGenerator const*> MotionMasterUnitStatesContainer;

        void Mutate(MovementGenerator* m, MovementSlot slot);

        void AddFlag(uint8 const flag) { _flags |= flag; }
        bool HasFlag(uint8 const flag) const { return (_flags & flag) != 0; }
        void RemoveFlag(uint8 const flag) { _flags &= ~flag; }

        void ResolveDelayedActions();
        void Remove(MotionMasterContainer::iterator& iterator, bool active, bool movementInform);
        void Pop(bool active, bool movementInform);
        void DirectInitialize();
        void DirectClear();
        void DirectClearDefault();
        void DirectClear(std::function<bool(MovementGenerator*)> const& filter);
        void DirectAdd(MovementGenerator* movement, MovementSlot slot);

        void Delete(MovementGenerator* movement, bool active, bool movementInform);
        void DeleteDefault(bool active, bool movementInform);
        void AddBaseUnitState(MovementGenerator const* movement);
        void ClearBaseUnitState(MovementGenerator const* movement);
        void ClearBaseUnitStates();
        void InitializeTopIfNeeded();

        void DirectExpire(bool reset);
        void DirectClean(bool reset);

        _Ty GetMotionSlotImpl(MovementSlot slot) const;
        void SetDefault(MovementGenerator* movement);

        Unit* _owner;
        MovementGeneratorPointer _defaultGenerator;
        MotionMasterContainer _generators;
        MotionMasterUnitStatesContainer _baseUnitStatesMap;
        std::deque<DelayedAction> _delayedActions;
        uint8 _flags;
};
#endif
