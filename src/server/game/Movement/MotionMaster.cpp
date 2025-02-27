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

#include "MotionMaster.h"
#include "CreatureAISelector.h"
#include "Creature.h"

#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "GenericMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"

inline bool isStatic(MovementGenerator *mv)
{
    return (mv == &si_idleMovement);
}

void MotionMaster::Initialize()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DirectDelete(curr);
    }

    InitDefault();
}

// set new default movement generator
void MotionMaster::InitDefault()
{
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        MovementGenerator* movement = FactorySelector::selectMovementGenerator(_owner->ToCreature());
        Mutate(movement == NULL ? &si_idleMovement : movement, MOTION_SLOT_IDLE);
    }
    else
    {
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
    }
}

MotionMaster::~MotionMaster()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator *curr = top();
        pop();
        if (curr && !isStatic(curr))
            delete curr;
    }
}

// MovementGenerator* MotionMaster::GetCurrentMovementGenerator() const
// {
//     if (!_generators.empty())
//         return *_generators.begin();

//     if (_defaultGenerator)
//         return _defaultGenerator.get();

//     return nullptr;
// }

// MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
// {
//     if (Empty())
//         return MAX_MOTION_TYPE;

//     MovementGenerator const* movement = GetCurrentMovementGenerator();
//     if (!movement)
//         return MAX_MOTION_TYPE;

//     return movement->GetMovementGeneratorType();
// }

void MotionMaster::UpdateMotion(uint32 diff)
{
    if (!_owner)
        return;

    if (_owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED)) // what about UNIT_STATE_DISTRACTED? Why is this not included?
        return;

    ASSERT(!empty());

    _cleanFlag |= MMCF_UPDATE;
    if (!top()->Update(_owner, diff))
    {
        _cleanFlag &= ~MMCF_UPDATE;
        MovementExpired();
    }
    else
        _cleanFlag &= ~MMCF_UPDATE;

    if (_expList)
    {
        for (size_t i = 0; i < _expList->size(); ++i)
        {
            MovementGenerator* mg = (*_expList)[i];
            DirectDelete(mg);
        }

        delete _expList;
        _expList = NULL;

        if (empty())
            Initialize();
        else if (needInitTop())
            InitTop();
        else if (_cleanFlag & MMCF_RESET)
            top()->Reset(_owner);

        _cleanFlag &= ~MMCF_RESET;
    }

}

void MotionMaster::DirectClean(bool reset)
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr) DirectDelete(curr);
    }

    if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(_owner);
}

void MotionMaster::DelayedClean()
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DelayedDelete(curr);
    }
}

void MotionMaster::DirectExpire(bool reset)
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        DirectDelete(curr);
    }

    while (!top())
        --_top;

    if (empty())
        Initialize();
    else if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(_owner);
}

void MotionMaster::DelayedExpire()
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        DelayedDelete(curr);
    }

    while (!top())
        --_top;
}

void MotionMaster::MoveIdle()
{
    //! Should be preceded by MovementExpired or Clear if there's an overlying movementgenerator active
    if (empty() || !isStatic(top()))
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
            Mutate(new FollowMovementGenerator<Creature>(target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE), MOTION_SLOT_ACTIVE);
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

    //_owner->ClearUnitState(UNIT_STATE_FOLLOW);
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) chase to %s (GUID: %u)",
            _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator<Player>(target, dist, angle), MOTION_SLOT_ACTIVE);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) chase to %s (GUID: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new ChaseMovementGenerator<Creature>(target, dist, angle), MOTION_SLOT_ACTIVE);
    }
}

void MotionMaster::MoveFollow(Unit* target, float dist, float angle, MovementSlot slot)
{
    // ignore movement request if target not exist
    if (!target || target == _owner || _owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        return;

    //_owner->AddUnitState(UNIT_STATE_FOLLOW);
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("misc", "Player (GUID: %u) follow to %s (GUID: %u)", _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator<Player>(target, dist, angle), slot);
    }
    else
    {
        TC_LOG_DEBUG("misc", "Creature (Entry: %u GUID: %u) follow to %s (GUID: %u)",
            _owner->GetEntry(), _owner->GetGUID().GetCounter(),
            target->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            target->GetTypeId() == TYPEID_PLAYER ? target->GetGUID().GetCounter() : target->ToCreature()->GetDBTableGUIDLow());
        Mutate(new FollowMovementGenerator<Creature>(target, dist, angle), slot);
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
    if ((Impl[MOTION_SLOT_CONTROLLED] && Impl[MOTION_SLOT_CONTROLLED]->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE) || Impl[MOTION_SLOT_CRITICAL])
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
    if (Impl[MOTION_SLOT_CONTROLLED] || Impl[MOTION_SLOT_CRITICAL])
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
    if (MovementGenerator *curr = Impl[slot])
    {
        Impl[slot] = NULL; // in case a new one is generated in this slot during directdelete
        if (_top == slot && (_cleanFlag & MMCF_UPDATE))
            DelayedDelete(curr);
        else
            DirectDelete(curr);
    }
    else if (_top < slot)
    {
        _top = slot;
    }

    Impl[slot] = m;
    if (_top > slot)
        _needInit[slot] = true;
    else
    {
        _needInit[slot] = false;
        m->Initialize(_owner);
    }
}

void MotionMaster::MovePath(uint32 path_id, bool repeatable)
{
    if (!path_id)
        return;
    //We set waypoint movement as new default movement generator
    // clear ALL movement generators (including default)
    /*while (!empty())
    {
        MovementGenerator *curr = top();
        curr->Finalize(*_owner);
        pop();
        if (!isStatic(curr))
            delete curr;
    }*/

    //_owner->GetTypeId() == TYPEID_PLAYER ?
        //Mutate(new WaypointMovementGenerator<Player>(path_id, repeatable)):
    Mutate(new WaypointMovementGenerator<Creature>(path_id, repeatable), MOTION_SLOT_IDLE);

    TC_LOG_DEBUG("misc", "%s (GUID: %u) start moving over path(Id:%u, repeatable: %s)",
        _owner->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature",
        _owner->GetGUID().GetCounter(), path_id, repeatable ? "YES" : "NO");
}

void MotionMaster::MoveSplinePath(const Position* path, uint32 count, bool fly, bool walk, float speed, bool cyclic, bool /*catmullrom*/, bool uncompressed)
{
    if (_owner->isMoving())
        _owner->StopMoving();

    Movement::MoveSplineInit init(_owner);
    float x, y, z;
    _owner->GetPosition(x, y, z);
    G3D::Vector3 vertice(x, y, z);
    init.Path().push_back(vertice);

    for (uint32 i = 0; i < count; i++)
        init.Path().emplace_back(path[i].m_positionX, path[i].m_positionY, path[i].m_positionZ);

    init.SetWalk(walk);
    if (fly)
        init.SetFly();
    if (cyclic)
        init.SetCyclic();
    if (speed != 0)
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

void MotionMaster::propagateSpeedChange()
{
    /*Impl::container_type::iterator it = Impl::c.begin();
    for (; it != end(); ++it)
    {
        (*it)->unitSpeedChanged();
    }*/
    for (int i = 0; i <= _top; ++i)
    {
        if (Impl[i])
            Impl[i]->unitSpeedChanged();
    }
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
   if (empty())
       return IDLE_MOTION_TYPE;

   return top()->GetMovementGeneratorType();
}

MovementGeneratorType MotionMaster::GetMotionSlotType(int slot) const
{
    if (!Impl[slot])
        return NULL_MOTION_TYPE;
    else
        return Impl[slot]->GetMovementGeneratorType();
}

void MotionMaster::InitTop()
{
    top()->Initialize(_owner);
    _needInit[_top] = false;
}

void MotionMaster::DirectDelete(_Ty curr)
{
    if (isStatic(curr))
        return;
    curr->Finalize(_owner);
    delete curr;
}

void MotionMaster::DirectDelete(int slot)
{
    if (MovementGenerator* curr = Impl[slot])
    {
        Impl[slot] = NULL;
        if (_top == slot && (_cleanFlag & MMCF_UPDATE))
            DelayedDelete(curr);
        else
            DirectDelete(curr);

        while (!top())
            --_top;
    }
}

void MotionMaster::DelayedDelete(_Ty curr)
{
    //TC_LOG_FATAL("misc", "Unit (Entry %u) is trying to delete its updating MG (Type %u)!", _owner->GetEntry(), curr->GetMovementGeneratorType());
    if (isStatic(curr))
        return;
    if (!_expList)
        _expList = new ExpireList();
    _expList->push_back(curr);
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
