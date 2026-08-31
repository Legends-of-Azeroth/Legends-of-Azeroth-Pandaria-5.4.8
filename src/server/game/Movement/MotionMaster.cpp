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

#include "MotionMaster.h"
#include "AbstractFollower.h"
#include "CreatureAISelector.h"
#include "Creature.h"

#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "GenericMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "ChaseMovementGenerator.h"
#include "FollowMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "PathGenerator.h"
#include <cassert>
#include <algorithm>
#include <iterator>

inline bool isStatic(MovementGenerator *mv)
{
    return (mv == &si_idleMovement);
}

inline void MovementGeneratorPointerDeleter(MovementGenerator* a)
{
    if (a != nullptr && !isStatic(a))
        delete a;
}

void MovementGeneratorDeleter::operator()(MovementGenerator* a)
{
    MovementGeneratorPointerDeleter(a);
}

bool MovementGeneratorComparator::operator()(MovementGenerator const* a, MovementGenerator const* b) const
{
    return a->Priority > b->Priority;
}

MovementGeneratorInformation::MovementGeneratorInformation(MovementGeneratorType type, ObjectGuid targetGUID, std::string const& targetName) : Type(type), TargetGUID(targetGUID), TargetName(targetName) { }

MotionMaster::MotionMaster(Unit* unit) : _owner(unit), _defaultGenerator(nullptr), _flags(MOTIONMASTER_FLAG_INITIALIZATION_PENDING) { }

MotionMaster::~MotionMaster()
{
    _delayedActions.clear();

    for (MovementGenerator* movement : _generators)
        MovementGeneratorPointerDeleter(movement);
}

void MotionMaster::Initialize()
{
    DirectClearDefault();
    DirectClear();
    InitDefault();
}

void MotionMaster::InitDefault()
{
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        MovementGenerator* movement = FactorySelector::selectMovementGenerator(_owner->ToCreature());
        SetDefault(movement == nullptr ? &si_idleMovement : movement);
    }
    else
    {
        SetDefault(&si_idleMovement);
    }
}

void MotionMaster::InitializeDefault()
{
    InitDefault();
}

bool MotionMaster::Empty() const
{
    return !_defaultGenerator && _generators.empty();
}

uint32 MotionMaster::Size() const
{
    return (_defaultGenerator ? 1 : 0) + uint32(_generators.size());
}

std::vector<MovementGeneratorInformation> MotionMaster::GetMovementGeneratorsInformation() const
{
    std::vector<MovementGeneratorInformation> list;

    if (_defaultGenerator)
        list.emplace_back(_defaultGenerator->GetMovementGeneratorType(), ObjectGuid::Empty, std::string());

    for (auto itr = _generators.begin(); itr != _generators.end(); ++itr)
    {
        MovementGenerator* movement = *itr;
        MovementGeneratorType const type = movement->GetMovementGeneratorType();
        switch (type)
        {
            case CHASE_MOTION_TYPE:
            case FOLLOW_MOTION_TYPE:
                if (AbstractFollower* followInformation = dynamic_cast<AbstractFollower*>(movement))
                {
                    if (Unit* target = followInformation->GetTarget())
                        list.emplace_back(type, target->GetGUID(), target->GetName());
                    else
                        list.emplace_back(type, ObjectGuid::Empty, std::string());
                }
                else
                    list.emplace_back(type, ObjectGuid::Empty, std::string());
                break;
            default:
                list.emplace_back(type, ObjectGuid::Empty, std::string());
                break;
        }
    }

    return list;
}

MovementSlot MotionMaster::GetCurrentSlot() const
{
    if (!_generators.empty())
        return MOTION_SLOT_ACTIVE;

    if (_defaultGenerator)
        return MOTION_SLOT_DEFAULT;

    return MAX_MOTION_SLOT;
}

MovementGenerator* MotionMaster::GetCurrentMovementGenerator() const
{
    if (!_generators.empty())
        return *_generators.begin();

    if (_defaultGenerator)
        return _defaultGenerator.get();

    return nullptr;
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
    if (Empty())
        return IDLE_MOTION_TYPE;

    MovementGenerator* movement = GetCurrentMovementGenerator();
    if (!movement)
        return IDLE_MOTION_TYPE;

    return movement->GetMovementGeneratorType();
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType(MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return MAX_MOTION_TYPE;

    switch (slot)
    {
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            if (!_generators.empty())
                return (*_generators.begin())->GetMovementGeneratorType();
            break;
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator)
                return _defaultGenerator->GetMovementGeneratorType();
            break;
        default:
            break;
    }

    return MAX_MOTION_TYPE;
}

MovementGenerator* MotionMaster::GetCurrentMovementGenerator(MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return nullptr;

    switch (slot)
    {
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            if (!_generators.empty())
                return *_generators.begin();
            break;
        case MOTION_SLOT_DEFAULT:
            return _defaultGenerator.get();
        default:
            break;
    }

    return nullptr;
}

MovementGenerator* MotionMaster::GetMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return nullptr;

    MovementGenerator* movement = nullptr;
    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && filter(_defaultGenerator.get()))
                movement = _defaultGenerator.get();
            break;
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            for (MovementGenerator* m : _generators)
            {
                if (filter(m))
                {
                    movement = m;
                    break;
                }
            }
            break;
        default:
            break;
    }

    return movement;
}

bool MotionMaster::HasMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return false;

    bool value = false;
    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            value = _defaultGenerator && filter(_defaultGenerator.get());
            break;
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            for (MovementGenerator* m : _generators)
            {
                if (filter(m))
                {
                    value = true;
                    break;
                }
            }
            break;
        default:
            break;
    }

    return value;
}

void MotionMaster::Update(uint32 diff)
{
    if (!_owner)
        return;

    if (_owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
        return;

    ASSERT(!Empty());

    AddFlag(MOTIONMASTER_FLAG_UPDATE);

    MovementGenerator* top = GetCurrentMovementGenerator();
    InitializeTopIfNeeded();

    if (!top->Update(_owner, diff))
        DirectExpire(true);

    RemoveFlag(MOTIONMASTER_FLAG_UPDATE);

    ResolveDelayedActions();
}

void MotionMaster::Add(MovementGenerator* movement, MovementSlot slot)
{
    if (!movement)
        return;

    MovementGeneratorPriority priority = MOTION_PRIORITY_NORMAL;
    if (slot == MOTION_SLOT_DEFAULT || slot == MOTION_SLOT_IDLE)
        slot = MOTION_SLOT_DEFAULT;
    else if (slot == MOTION_SLOT_CONTROLLED || slot == MOTION_SLOT_CRITICAL)
    {
        slot = MOTION_SLOT_ACTIVE;
        priority = MOTION_PRIORITY_HIGHEST;
    }
    else if (slot != MOTION_SLOT_ACTIVE)
    {
        delete movement;
        return;
    }

    movement->Priority = priority;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, movement, slot]()
        {
            Add(movement, slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_ADD);
    }
    else
        DirectAdd(movement, slot);
}

void MotionMaster::Remove(MovementGenerator* movement, MovementSlot slot)
{
    if (!movement || IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, movement, slot]()
        {
            Remove(movement, slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_REMOVE);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && _defaultGenerator.get() == movement)
                DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
        {
            auto itr = _generators.find(movement);
            if (itr != _generators.end())
                Remove(itr, GetCurrentMovementGenerator() == *itr, false);
            break;
        }
        default:
            break;
    }
}

void MotionMaster::Remove(MovementGeneratorType type, MovementSlot slot)
{
    if (IsInvalidMovementGeneratorType(type) || IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, type, slot]()
        {
            Remove(type, slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_REMOVE_TYPE);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && _defaultGenerator->GetMovementGeneratorType() == type)
                DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            DirectClear([type](MovementGenerator* a) { return a->GetMovementGeneratorType() == type; });
            break;
        default:
            break;
    }
}

void MotionMaster::Clear(bool reset)
{
    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, reset]()
        {
            Clear(reset);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR);
    }
    else
        DirectClean(reset);
}

void MotionMaster::Clear(MovementSlot slot)
{
    if (IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, slot]()
        {
            Clear(slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR_SLOT);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            DirectClear();
            break;
        default:
            break;
    }
}

void MotionMaster::Clear(MovementGeneratorPriority priority)
{
    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, priority]()
        {
            Clear(priority);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR_PRIORITY);
        return;
    }

    if (Empty())
        return;

    DirectClear([priority](MovementGenerator* a) { return a->Priority == priority; });
}

void MotionMaster::MovementExpired(bool reset)
{
    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, reset]()
        {
            DirectExpire(reset);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_EXPIRE);
    }
    else
        DirectExpire(reset);
}

void MotionMaster::PropagateSpeedChange()
{
    propagateSpeedChange();
}

void MotionMaster::propagateSpeedChange()
{
    if (_defaultGenerator)
        _defaultGenerator->unitSpeedChanged();

    for (MovementGenerator* m : _generators)
        m->unitSpeedChanged();
}

bool MotionMaster::GetDestination(float &x, float &y, float &z)
{
    if (_owner->movespline->Finalized())
        return false;

    G3D::Vector3 const& dest = _owner->movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;
    return true;
}

bool MotionMaster::StopOnDeath()
{
    if (MovementGenerator* movementGenerator = GetCurrentMovementGenerator())
        if (movementGenerator->HasFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH))
            return false;

    if (_owner->IsInWorld())
    {
        Clear();
        MoveIdle();
    }

    _owner->StopMoving();

    return true;
}

void MotionMaster::InterruptOnTeleport()
{
    if (MovementGenerator* top = GetCurrentMovementGenerator())
        if (!top->HasFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED | MOVEMENTGENERATOR_FLAG_FINALIZED))
            top->Deactivate(_owner);
}

void MotionMaster::MoveIdle()
{
    if (!_defaultGenerator || !isStatic(_defaultGenerator.get()))
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
}

void MotionMaster::MoveRandom(float wanderDistance)
{
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        TC_LOG_DEBUG("misc", "Creature (GUID: %u) start moving random", _owner->GetGUID().GetCounter());
        Mutate(new RandomMovementGenerator<Creature>(wanderDistance), MOTION_SLOT_IDLE);
    }
}

void MotionMaster::MoveTargetedHome()
{
    Clear(false);

    if (_owner->GetTypeId() == TYPEID_UNIT && !_owner->ToCreature()->GetCharmerOrOwnerGUID())
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) targeted home", _owner->GetEntry(), _owner->GetGUID().GetCounter());
        Mutate(new HomeMovementGenerator<Creature>(), MOTION_SLOT_ACTIVE);
    }
    else if (_owner->GetTypeId() == TYPEID_UNIT && _owner->ToCreature()->GetCharmerOrOwnerGUID())
    {
        TC_LOG_DEBUG("misc", "Pet or controlled creature (Entry: %u GUID: %u) targeting home", _owner->GetEntry(), _owner->GetGUID().GetCounter());
        Unit* target = _owner->ToCreature()->GetCharmerOrOwner();
        if (target)
        {
            TC_LOG_DEBUG("misc", "Following %s (GUID: %u)", target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature", target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : ((Creature*)target)->GetDBTableGUIDLow());
            Mutate(new FollowMovementGenerator(target, PET_FOLLOW_DIST, ChaseAngle(PET_FOLLOW_ANGLE)), MOTION_SLOT_ACTIVE);
        }
    }
    else
    {
        TC_LOG_ERROR("misc", "Player (GUID: %u) attempt targeted home", _owner->GetGUID().GetCounter());
    }
}

void MotionMaster::MoveConfused()
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) move confused", _owner->GetGUID().GetCounter());
        Mutate(new ConfusedMovementGenerator<Player>(), MOTION_SLOT_CONTROLLED);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) move confused",
            _owner->GetEntry(), _owner->GetGUID().GetCounter());
        Mutate(new ConfusedMovementGenerator<Creature>(), MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveChase(Unit* target, float dist, float angle)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) chase to %s (GUID: %u)",
            _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator(target, ChaseRange(dist), ChaseAngle(angle)), MOTION_SLOT_ACTIVE);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) chase to %s (GUID: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator(target, ChaseRange(dist), ChaseAngle(angle)), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveFollow(Unit* target, float dist, float angle, MovementSlot slot)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) follow to %s (GUID: %u)", _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator(target, dist, ChaseAngle(angle)), slot);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) follow to %s (GUID: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator(target, dist, ChaseAngle(angle)), slot);
    }
}

void MotionMaster::MovePoint(uint32 id, float x, float y, float z, bool generatePath, MovementSlot slot)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) targeted point (Id: %u X: %f Y: %f Z: %f)", _owner->GetGUID().GetCounter(), id, x, y, z);
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath), slot);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) targeted point (ID: %u X: %f Y: %f Z: %f)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), id, x, y, z);
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath), slot);
    }
}

void MotionMaster::MoveLand(uint32 id, Position const& pos, float speed)
{
    float x, y, z;
    pos.GetPosition(x, y, z);

    TC_LOG_DEBUG("misc", "Creature (Entry: %u) landing point (ID: %u X: %f Y: %f Z: %f)", _owner->GetEntry(), id, x, y, z);

    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_ACTIVE);
    Movement::MoveSplineInit init(_owner);
    init.MoveTo(x, y, z, false);
    init.SetAnimation(_owner->IsHovering() ? AnimTier::Hover : AnimTier::Ground);
    if (speed > 0)
        init.SetVelocity(speed);
    init.Launch();
}

void MotionMaster::MoveTakeoff(uint32 id, Position const& pos, float speed)
{
    float x, y, z;
    pos.GetPosition(x, y, z);

    TC_LOG_DEBUG("misc", "Creature (Entry: %u) landing point (ID: %u X: %f Y: %f Z: %f)", _owner->GetEntry(), id, x, y, z);

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(x, y, z, false);
    init.SetAnimation(AnimTier::Fly);
    init.Launch();
    if (speed > 0)
        init.SetVelocity(speed);
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveKnockbackFrom(float srcX, float srcY, float speedXY, float speedZ)
{
    //this function may make players fall below map
    if (_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    float x, y, z;
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    _owner->GetNearPoint(_owner, x, y, z, _owner->GetObjectSize(), dist, _owner->GetAngle(srcX, srcY) + M_PI);

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(x, y, z);
    init.SetParabolic(max_height, 0);
    init.SetOrientationFixed(true);
    init.SetVelocity(speedXY);
    init.Launch();
    Mutate(new EffectMovementGenerator(0), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveJumpTo(float angle, float speedXY, float speedZ)
{
    //this function may make players fall below map
    if (_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    float x, y, z;

    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    _owner->GetClosePoint(x, y, z, _owner->GetObjectSize(), dist, angle);
    MoveJump(x, y, z, speedXY, speedZ);
}

void MotionMaster::MoveJump(float x, float y, float z, float speedXY, float speedZ, uint32 id)
{
    TC_LOG_DEBUG("misc", "Unit (GUID: %u) jump to point (X: %f Y: %f Z: %f)", _owner->GetGUID().GetCounter(), x, y, z);

    float moveTimeHalf = speedZ / Movement::gravity;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(x, y, z, false);
    init.SetParabolic(max_height, 0);
    init.SetVelocity(speedXY);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::CustomJump(float x, float y, float z, float speedXY, float speedZ, uint32 id)
{
    speedZ *= 2.3f;
    speedXY *= 2.3f;
    float moveTimeHalf = speedZ / Movement::gravity;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);
    max_height /= 15.0f;

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(x, y, z, false, true);
    init.SetParabolic(max_height, 0);
    init.SetVelocity(speedXY);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveSmoothPath(uint32 pointId, Position const* pathPoints, size_t pathSize, bool walk, bool fly)
{
    Movement::MoveSplineInit init(_owner);

    if (fly)
    {
        init.SetFly();
        init.SetUncompressed();
        init.SetSmooth();
    }

    Movement::PointsArray path;
    path.reserve(pathSize);
    std::transform(pathPoints, pathPoints + pathSize, std::back_inserter(path), [](Position const& point)
    {
        return G3D::Vector3(point.GetPositionX(), point.GetPositionY(), point.GetPositionZ());
    });

    init.MovebyPath(path);
    init.SetWalk(walk);
    init.Launch();

    Mutate(new EffectMovementGenerator(pointId), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveCirclePath(float x, float y, float z, float radius, bool clockwise, uint8 stepCount, float velocityOrDuration)
{
    float step = 2 * float(M_PI) / stepCount * (clockwise ? -1.0f : 1.0f);
    Position const& pos = { x, y, z, 0.0f };
    float angle = pos.GetAngle(_owner->GetPositionX(), _owner->GetPositionY());

    Movement::PointsArray points;
    points.reserve(stepCount + 1);
    points.emplace_back(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ());
    for (uint8 i = 0; i < stepCount; angle += step, ++i)
    {
        G3D::Vector3 point;
        point.x = x + radius * cosf(angle);
        point.y = y + radius * sinf(angle);

        if (_owner->CanFly() || _owner->IsFlying())
            point.z = z;
        else
            point.z = _owner->GetMap()->GetHeight(_owner->GetPhaseMask(), point.x, point.y, z);

        points.push_back(point);
    }

    Movement::MoveSplineInit init(_owner);
    init.SetCyclic();
    if (velocityOrDuration)
        init.SetVelocity(velocityOrDuration);
    init.MovebyPath(points);

    init.Launch();
}

void MotionMaster::MoveFall(uint32 id /*=0*/)
{
    // use larger distance for vmap height search than in most other cases
    float tz = _owner->GetMap()->GetHeight(_owner->GetPhaseMask(), _owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ(), true, MAX_FALL_DISTANCE);
    if (tz <= INVALID_HEIGHT)
    {
        TC_LOG_DEBUG("misc", "MotionMaster::MoveFall: unable retrive a proper height at map %u (x: %f, y: %f, z: %f).",
            _owner->GetMap()->GetId(), _owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ());
        return;
    }

    // Abort too if the ground is very near
    if (fabs(_owner->GetPositionZ() - tz) < 0.1f)
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
        _owner->SetFall(true);

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(_owner->GetPositionX(), _owner->GetPositionY(), tz, false);
    init.SetFall();
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveCharge(float x, float y, float z, float speed, uint32 id, bool generatePath)
{
    if (GetMotionSlot(MOTION_SLOT_CONTROLLED) && GetMotionSlot(MOTION_SLOT_CONTROLLED)->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE || GetMotionSlot(MOTION_SLOT_CRITICAL))
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) charge point (X: %f Y: %f Z: %f)", _owner->GetGUID().GetCounter(), x, y, z);
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath, speed), MOTION_SLOT_CONTROLLED);
        _owner->ToPlayer()->SetFallInformation(0, _owner->GetPositionZ());
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) charge point (X: %f Y: %f Z: %f)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), x, y, z);
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, speed), MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveCharge(PathGenerator const& path)
{
    G3D::Vector3 dest = path.GetActualEndPosition();

    MoveCharge(dest.x, dest.y, dest.z, SPEED_CHARGE, EVENT_CHARGE_PREPATH);

    // Charge movement is not started when using EVENT_CHARGE_PREPATH
    Movement::MoveSplineInit init(_owner);
    init.MovebyPath(path.GetPath());
    init.SetVelocity(SPEED_CHARGE);
    init.Launch();
}

void MotionMaster::MoveSeekAssistance(float x, float y, float z)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_ERROR("misc", "Player (GUID: %u) attempt to seek assistance", _owner->GetGUID().GetCounter());
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) seek assistance (X: %f Y: %f Z: %f)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), x, y, z);
        _owner->AttackStop();
        _owner->ToCreature()->SetReactState(REACT_PASSIVE);
        Mutate(new AssistanceMovementGenerator(x, y, z), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveSeekAssistanceDistract(uint32 time)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_ERROR("misc", "Player (GUID: %u) attempt to call distract after assistance", _owner->GetGUID().GetCounter());
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) is distracted after assistance call (Time: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), time);
        Mutate(new AssistanceDistractMovementGenerator(time), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveFleeing(Unit* enemy, uint32 time)
{
    if (!enemy)
        return;

    if (_owner->HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) flee from %s (GUID: %u)", _owner->GetGUID().GetCounter(),
            enemy->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            enemy->GetTypeId() == TYPEID_PLAYER ? enemy->GetGUID().GetCounter() : enemy->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FleeingMovementGenerator<Player>(enemy->GetGUID()), MOTION_SLOT_CONTROLLED);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) flee from %s (GUID: %u)%s",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(),
            enemy->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            enemy->GetTypeId() == TYPEID_PLAYER ? enemy->GetGUID().GetCounter() : enemy->ToCreature()->GetDBTableGUIDLow(),
            time ? " for a limited time" : "");
        if (time)
            Mutate(new TimedFleeingMovementGenerator(enemy->GetGUID(), time), MOTION_SLOT_CONTROLLED);
        else
            Mutate(new FleeingMovementGenerator<Creature>(enemy->GetGUID()), MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveTaxiFlight(uint32 path, uint32 pathnode)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        if (path < sTaxiPathNodesByPath.size())
        {
            TC_LOG_DEBUG("misc", "%s taxi to (Path %u node %u)", _owner->GetName().c_str(), path, pathnode);
            FlightPathMovementGenerator* mgen = new FlightPathMovementGenerator(sTaxiPathNodesByPath[path], pathnode);
            Mutate(mgen, MOTION_SLOT_CONTROLLED);
        }
        else
        {
            TC_LOG_ERROR("misc", "%s attempt taxi to (not existed Path %u node %u)",
            _owner->GetName().c_str(), path, pathnode);
        }
    }
    else
    {
        TC_LOG_ERROR("misc", "Creature (Entry: %u GUID: %u) attempt taxi to (Path %u node %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), path, pathnode);
    }
}

void MotionMaster::MoveDistract(uint32 timer)
{
    if (GetMotionSlot(MOTION_SLOT_CONTROLLED) || GetMotionSlot(MOTION_SLOT_CRITICAL))
        return;

    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) distracted (timer: %u)", _owner->GetGUID().GetCounter(), timer);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) (timer: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(), timer);
    }

    DistractMovementGenerator* mgen = new DistractMovementGenerator(timer);
    Mutate(mgen, MOTION_SLOT_CONTROLLED);
}

void MotionMaster::Mutate(MovementGenerator *m, MovementSlot slot)
{
    Add(m, slot);
}

void MotionMaster::MovePath(uint32 path_id, bool repeatable)
{
    if (!path_id)
        return;

    Mutate(new WaypointMovementGenerator<Creature>(path_id, repeatable), MOTION_SLOT_IDLE);

    TC_LOG_DEBUG("misc", "%s (GUID: %u) start moving over path(Id:%u, repeatable: %s)",
        _owner->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature",
        _owner->GetGUID().GetCounter(), path_id, repeatable ? "YES" : "NO");
}

void MotionMaster::MoveSplinePath(const Position* path, uint32 count, bool fly, bool walk, float speed, bool cyclic, bool catmullrom, bool uncompressed)
{
    if (_owner->isMoving())
        _owner->StopMoving();

    Movement::MoveSplineInit init(_owner);
    float x, y, z;
    _owner->GetPosition(x, y, z);
    G3D::Vector3 vertice(x, y, z);
    init.Path().push_back(vertice);

    for (uint32 i = 0; i < count; i++)
        init.Path().push_back(G3D::Vector3(path[i].m_positionX, path[i].m_positionY, path[i].m_positionZ));

    init.SetWalk(walk);
    if (fly)
        init.SetFly();
    if (cyclic)
        init.SetCyclic();
    if (speed)
        init.SetVelocity(speed);
    if (uncompressed)
        init.SetUncompressed();
    init.Launch();
}

void MotionMaster::MoveRotate(uint32 time, RotateDirection direction)
{
    if (!time)
        return;

    Mutate(new RotateMovementGenerator(time, direction), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveKnockbackFromForPlayer(float srcX, float srcY, float speedXY, float speedZ)
{
    if (speedXY <= 0.1f)
        return;

    Position dest = _owner->GetPosition();
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    _owner->MovePositionToFirstCollision(dest, dist, _owner->GetRelativeAngle(srcX, srcY) + float(M_PI));

    Movement::MoveSplineInit init(_owner);
    init.MoveTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());
    init.SetParabolic(max_height, 0);
    init.SetOrientationFixed(true);
    init.SetVelocity(speedXY);
    init.Launch();
    Mutate(new EffectMovementGenerator(0), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MovePointBackwards(uint32 id, float x, float y, float z, bool generatePath, bool forceDestination, MovementSlot slot, float orientation /* = 0.0f*/)
{
    if (_owner->HasUnitFlag(UNIT_FLAG_DISABLE_MOVE))
        return;

    if (_owner->IsPlayer())
    {
        TC_LOG_DEBUG("movement.motionmaster", "Player (%u) targeted point (Id: %u X: %f Y: %f Z: %f)", _owner->GetGUID().GetCounter(), id, x, y, z);
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath), slot);
    }
    else
    {
        TC_LOG_DEBUG("movement.motionmaster", "Creature (%u) targeted point (ID: %u X: %f Y: %f Z: %f)", _owner->GetGUID().GetCounter(), id, x, y, z);
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath), slot);
    }
}

void MotionMaster::LaunchMoveSpline(Movement::MoveSplineInit&& init, uint32 id/*= 0*/, MovementSlot slot/*= MOTION_SLOT_ACTIVE*/, MovementGeneratorType type/*= EFFECT_MOTION_TYPE*/)
{
    if (IsInvalidMovementGeneratorType(type))
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '%u', tried to launch a spline with an invalid MovementGeneratorType: %u (Id: %u, Slot: %u)", _owner->GetGUID().GetCounter(), type, id, slot);
        return;
    }

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '%u', initiates spline Id: %u (Type: %u, Slot: %u)", _owner->GetGUID().GetCounter(), id, type, slot);
    Mutate(new GenericMovementGenerator(std::move(init), type, id), slot);
}

void MotionMaster::LaunchMoveSpline(std::function<void(Movement::MoveSplineInit&)>&& initializer, uint32 id/*= 0*/, MovementSlot slot/*= MOTION_SLOT_ACTIVE*/, MovementGeneratorType type/*= EFFECT_MOTION_TYPE*/)
{
    if (IsInvalidMovementGeneratorType(type))
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '%u', tried to launch a spline with an invalid MovementGeneratorType: %u (Id: %u, Slot: %u)", _owner->GetGUID().GetCounter(), type, id, slot);
        return;
    }

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '%u', initiates spline Id: %u (Type: %u, Slot: %u)", _owner->GetGUID().GetCounter(), id, type, slot);
    Mutate(new GenericMovementGenerator(std::move(initializer), type, id), slot);
}

/******************** Private methods ********************/

void MotionMaster::ResolveDelayedActions()
{
    while (!_delayedActions.empty())
    {
        _delayedActions.front().Resolve();
        _delayedActions.pop_front();
    }
}

void MotionMaster::Remove(MotionMasterContainer::iterator& iterator, bool active, bool movementInform)
{
    MovementGenerator* pointer = *iterator;
    iterator = _generators.erase(iterator);
    Delete(pointer, active, movementInform);
}

void MotionMaster::Pop(bool active, bool movementInform)
{
    auto itr = _generators.begin();
    if (itr != _generators.end())
        Remove(itr, active, movementInform);
}

void MotionMaster::DirectClear()
{
    if (!_generators.empty())
        Pop(true, false);

    while (!_generators.empty())
        Pop(false, false);

    ClearBaseUnitStates();
}

void MotionMaster::DirectClearDefault()
{
    if (_defaultGenerator)
    {
        _defaultGenerator->Finalize(_owner, _generators.empty(), false);
        _defaultGenerator = nullptr;
    }
}

void MotionMaster::DirectClear(std::function<bool(MovementGenerator*)> const& filter)
{
    MovementGenerator const* top = GetCurrentMovementGenerator();
    for (auto itr = _generators.begin(); itr != _generators.end();)
    {
        if (filter(*itr))
            Remove(itr, *itr == top, false);
        else
            ++itr;
    }
}

void MotionMaster::DirectAdd(MovementGenerator* movement, MovementSlot slot)
{
    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
        {
            SetDefault(movement);
            break;
        }
        case MOTION_SLOT_ACTIVE:
        {
            if (!_generators.empty())
            {
                auto where = _generators.lower_bound(movement);
                bool replacesExisting = where != _generators.end() && !_generators.key_comp()(movement, *where);
                auto top = _generators.begin();
                if (replacesExisting)
                    Remove(where, where == top, false);
                else if (where == top)
                    (*top)->Deactivate(_owner);
            }
            else if (_defaultGenerator)
                _defaultGenerator->Deactivate(_owner);

            movement->AddFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING);
            _generators.insert(movement);
            AddBaseUnitState(movement);
            break;
        }
        default:
            delete movement;
            break;
    }

    InitializeTopIfNeeded();
}

void MotionMaster::Delete(MovementGenerator* movement, bool active, bool movementInform)
{
    movement->Finalize(_owner, active, movementInform);
    ClearBaseUnitState(movement);
    MovementGeneratorPointerDeleter(movement);
}

void MotionMaster::AddBaseUnitState(MovementGenerator const* movement)
{
    if (!movement || !movement->BaseUnitState)
        return;

    _baseUnitStatesMap.emplace(movement->BaseUnitState, movement);
    _owner->AddUnitState(movement->BaseUnitState);
}

void MotionMaster::ClearBaseUnitState(MovementGenerator const* movement)
{
    if (!movement || !movement->BaseUnitState)
        return;

    auto range = _baseUnitStatesMap.equal_range(movement->BaseUnitState);
    for (auto itr = range.first; itr != range.second;)
    {
        if (itr->second == movement)
            itr = _baseUnitStatesMap.erase(itr);
        else
            ++itr;
    }

    if (_baseUnitStatesMap.count(movement->BaseUnitState) == 0)
        _owner->ClearUnitState(movement->BaseUnitState);
}

void MotionMaster::ClearBaseUnitStates()
{
    uint32 unitState = 0;
    for (auto itr = _baseUnitStatesMap.begin(); itr != _baseUnitStatesMap.end(); ++itr)
        unitState |= itr->first;

    _owner->ClearUnitState(unitState);
    _baseUnitStatesMap.clear();
}

void MotionMaster::InitializeTopIfNeeded()
{
    MovementGenerator* top = GetCurrentMovementGenerator();
    if (!top)
        return;

    if (isStatic(top))
    {
        if (HasFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING))
        {
            top->Initialize(_owner);
            RemoveFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
        }
        return;
    }

    if (top->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING))
    {
        top->Initialize(_owner);
        top->RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING);
    }
    else if (top->HasFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED))
    {
        top->Reset(_owner);
        top->RemoveFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    }
}

void MotionMaster::DirectExpire(bool reset)
{
    if (!_generators.empty())
        Pop(true, true);

    MovementGenerator* newTop = GetCurrentMovementGenerator();
    if (!newTop)
    {
        Initialize();
        return;
    }

    if (isStatic(newTop))
    {
        if (HasFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING))
            newTop->Initialize(_owner);
        RemoveFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
    }
    else if (newTop->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING))
    {
        newTop->Initialize(_owner);
        newTop->RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING);
    }
    else if (newTop->HasFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED))
    {
        newTop->Reset(_owner);
        newTop->RemoveFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    }
    else if (reset)
        newTop->Reset(_owner);
}

void MotionMaster::DirectClean(bool reset)
{
    DirectClear();

    MovementGenerator* top = GetCurrentMovementGenerator();
    if (!top)
        return;

    if (isStatic(top))
    {
        if (HasFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING))
            top->Initialize(_owner);
        RemoveFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
    }
    else if (top->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING))
    {
        top->Initialize(_owner);
        top->RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING);
    }
    else if (reset)
        top->Reset(_owner);
}

void MotionMaster::SetDefault(MovementGenerator* movement)
{
    if (_defaultGenerator && _defaultGenerator.get() != movement)
        _defaultGenerator->Finalize(_owner, _generators.empty(), false);

    _defaultGenerator = MovementGeneratorPointer(movement);

    if (isStatic(movement))
        AddFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
    else
        movement->AddFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING);
}

void MotionMaster::DirectDelete(int slot)
{
    if (slot == MOTION_SLOT_ACTIVE)
        DirectClear([](MovementGenerator* m) { return m->Priority == MOTION_PRIORITY_NORMAL; });
    else if (slot == MOTION_SLOT_CONTROLLED || slot == MOTION_SLOT_CRITICAL)
        Pop(true, true);
    else if (slot == MOTION_SLOT_DEFAULT || slot == MOTION_SLOT_IDLE)
        DirectClearDefault();
}

void MotionMaster::DirectDelete(MovementGenerator* curr)
{
    if (!curr)
        return;

    auto itr = _generators.find(curr);
    if (itr != _generators.end())
        Remove(itr, GetCurrentMovementGenerator() == curr, false);
    else if (_defaultGenerator && _defaultGenerator.get() == curr)
        DirectClearDefault();
}

MovementGenerator* MotionMaster::GetMotionSlot(MovementSlot slot) const
{
    return GetMotionSlotImpl(slot);
}

MovementGenerator* MotionMaster::GetMotionSlot(int slot) const
{
    return GetMotionSlotImpl(MovementSlot(slot));
}

MovementGenerator* MotionMaster::GetMotionSlotImpl(MovementSlot slot) const
{
    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            return _defaultGenerator.get();
        case MOTION_SLOT_ACTIVE:
            for (MovementGenerator* m : _generators)
                if (m->Priority == MOTION_PRIORITY_NORMAL)
                    return m;
            return nullptr;
        case MOTION_SLOT_CONTROLLED:
        case MOTION_SLOT_CRITICAL:
            for (MovementGenerator* m : _generators)
                if (m->Priority >= MOTION_PRIORITY_HIGHEST)
                    return m;
            return nullptr;
        default:
            return nullptr;
    }
}

MovementGeneratorType MotionMaster::GetMotionSlotType(int slot) const
{
    if (MovementGenerator* m = GetMotionSlot(slot))
        return m->GetMovementGeneratorType();

    return NULL_MOTION_TYPE;
}
